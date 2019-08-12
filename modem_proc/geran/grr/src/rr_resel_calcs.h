#ifndef RR_RESEL_CALCS_H
#define RR_RESEL_CALCS_H

/*============================================================================
  @file rr_resel_calcs.h

  Header file for rr_gprs_reselection_calculations.c module.

                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_resel_calcs.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_task.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef struct
{
  gsm_ncell_list_member_t * ncell_data;
  rr_timer_id_T             penalty_timer_id;
  reselection_info_T        reselection_info;
} rr_resel_candidate_t;

/* Best six list */
typedef struct
{
  uint8                no_of_entries;
  rr_resel_candidate_t ncell_entry[MAX_NUMBER_OF_BEST_CELLS];
} best_six_ncells_t;

/*----------------------------------------------------------------------------
 * Global Data Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/



extern void rr_gprs_init_bad_snr_counter(
  const gas_id_t gas_id
);

extern boolean rr_gprs_is_snr_counter_zero(
  const gas_id_t gas_id
);

extern boolean rr_gprs_check_for_path_loss(
  const gas_id_t gas_id
);

extern boolean rr_gprs_check_for_bad_scell(
  const gas_id_t gas_id
);

extern void rr_gprs_process_serving_aux_meas_ind(
  mph_serving_aux_meas_ind_T * mph_serving_aux_meas_ind,
  const gas_id_t gas_id
);

extern void rr_gprs_store_serving_meas(
  mph_serving_meas_ind_t* mph_serving_meas_ind,
  const gas_id_t gas_id
);

extern void rr_gprs_store_serving_idle_meas(
  mph_serving_idle_meas_ind_T* mph_serving_idle_meas_ind,
  const gas_id_t gas_id
);

extern void rr_gprs_store_surround_meas(
  mph_surround_meas_ind_T* mph_surround_meas_ind,
  const gas_id_t gas_id
);

extern void rr_gprs_process_nc_meas_ind(
  mph_nc_measurement_ind_t* mph_nc_measurement_ind,
  const gas_id_t gas_id
);

extern void rr_gprs_store_surrounding_measurement_MFRM51(
  best_six_info_T * p_best_six_info,
  const gas_id_t gas_id
);

extern int rr_gprs_compute_surrounding_C2(
  int                    C1,
  rr_resel_candidate_t * resel_candidate,
  const gas_id_t         gas_id
);

extern int rr_gprs_build_candidate_cell_list_GSM(
  const gas_id_t gas_id
);

extern int rr_gprs_build_candidate_cell_list_GPRS(
  const gas_id_t gas_id
);

extern boolean rr_gprs_check_for_better_ncell(
  boolean recent_reselection_flag,
  const gas_id_t gas_id
);

extern boolean rr_gprs_compare_serving_and_surrounding_C2(
  int                 servingC2,
  int                 surroundingC2,
  bcch_ncell_info_t * ptr_ncell_info,
  boolean             recent_reselection_flag,
  LAI_T             * ncell_lai_ptr,
  const gas_id_t      gas_id
);

extern boolean rr_gprs_compare_serving_and_surrounding_C32(
  gprs_reselection_values_T * surround_data,
  gprs_reselection_values_T * serving_data,
  boolean                     recent_reselection_flag,
  const gas_id_t              gas_id
);

extern int rr_gprs_surround_calculate_C32(
  int              C1,
  int              GPRS_TEMPORARY_OFFSET,
  int              GPRS_RESELECT_OFFSET,
  int              PRIORITY_CLASSs,
  int              PRIORITY_CLASSn,
  rr_timer_id_T    penalty_timer_id,
  byte             PENALTY_TIME,
  boolean          strongest_RLA_P,
  const gas_id_t   gas_id
);

/****************************************************************************
 *   Function name:  rr_get_better_cell_list_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to better_cell_list.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  gprs_better_cell_list_T *
 *   --------
 *
 ****************************************************************************/
gprs_better_cell_list_T *rr_get_better_cell_list_ptr(
  const gas_id_t gas_id
);

/****************************************************************************
 *   Function name:  rr_get_best_six_ncells_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to best_six_ncells
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  best_six_ncells_t *
 *   --------
 *
 ****************************************************************************/
best_six_ncells_t *rr_get_best_six_ncells_ptr(
  const gas_id_t gas_id
);

#ifdef FEATURE_LTE

/**
  @brief Returns the greatest GSM S-value (C1) of all GSM neighbour cells, and
         optionally the serving cell.

  @param include_serving_cell TRUE if the serving cell value should be included
         when determining the greatest S-value, FALSE if it should not.
*/
extern int rr_resel_greatest_gsm_s_value(
  boolean include_serving_cell,
  const gas_id_t gas_id
);

#endif /* FEATURE_LTE */

#endif /* RR_RESEL_CALCS_H */

/* EOF */

