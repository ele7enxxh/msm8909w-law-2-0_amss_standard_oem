#ifndef RR_SHARED_CELL_SELECTION_H
#define RR_SHARED_CELL_SELECTION_H

/*! \file rr_shared_cell_selection.h
 
  This is the header file for rr_shared_cell_selection.c.
  This module contains utility functions which are used by cell selection & reselection, such as
  for checking cell suitability for camping.
 
                Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_shared_cell_selection.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"
#include "rr_rrc_if.h"
#include "rr_gprs_defs.h"
#include "rr_resel_calcs.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern void rr_store_lai_reject_list(
  rr_lai_reject_list_ind_T *lai_reject_list_ind_ptr,
  const gas_id_t gas_id
);

extern boolean rr_check_lai_in_reject_list(
  LAI_T *location_area_identification,
  const gas_id_t gas_id
);

#ifdef FEATURE_WCDMA
extern boolean rr_check_cell_suitable_w2g_redirection(
  cell_selection_parameters_T *cell_selection_parameters,
  RACH_control_parameters_T   *RACH_control_parameters,
  LAI_T                       *location_area_identification,
  byte                        rxlev,
  ARFCN_T                     BCCH_ARFCN,
  BSIC_T                      *bsic_ptr,
  boolean                     check_acceptable,
  boolean                     automatic_plmn_selection_criteria,
  boolean                     gprs_indicator,
  const gas_id_t              gas_id
);
#endif /* FEATURE_WCDMA */

extern boolean rr_check_cell_suitable(
  cell_selection_parameters_T *cell_selection_parameters,
  RACH_control_parameters_T   *RACH_control_parameters,
  LAI_T                       *location_area_identification,
  byte                        rxlev,
  ARFCN_T                     BCCH_ARFCN,
  BSIC_T                      *bsic_ptr,
  boolean                     check_acceptable,
  boolean                     automatic_plmn_selection_criteria,
  boolean                     check_plmn_match,
  const gas_id_t              gas_id
);

extern boolean rr_cell_barred_or_c1_unsuitable(
  cell_selection_parameters_T *cell_selection_parameters,
  RACH_control_parameters_T   *RACH_control_parameters,
  LAI_T                       *location_area_identification,
  byte                        rxlev,
  ARFCN_T                     BCCH_ARFCN,
  BSIC_T                      *bsic_ptr,
  const gas_id_t              gas_id
);

extern void rr_stop_serving_five_second_timer(
  const gas_id_t gas_id
);

extern void rr_update_serving_five_second_timer(
  const gas_id_t gas_id
);

extern void rr_cancel_all_five_second_timers(
  const gas_id_t gas_id
);

extern void rr_reset_surrounding_database(
  const gas_id_t gas_id
);

extern boolean rr_cell_selection_PLMN_match(
  rr_PLMN_request_details_T *req_details_ptr,
  LAI_T *received_LAI_ptr,
  const gas_id_t gas_id
);

extern void rr_clear_reselection_info(
  reselection_info_T *reselection_info_ptr
);

extern void rr_start_penalty_timer(
  rr_resel_candidate_t *resel_candidate,
  byte PENALTY_TIME,
  const gas_id_t gas_id
);

extern best_six_info_T *rr_get_last_best_six(
  const gas_id_t gas_id
);

inter_task_lai_T rr_convert_rr_lai_to_inter_task_lai(
  LAI_T *src
);

void rr_shared_sel_task_start_init(
  void
);

#endif /* #ifndef RR_SHARED_CELL_SELECTION_H */

/* EOF */

