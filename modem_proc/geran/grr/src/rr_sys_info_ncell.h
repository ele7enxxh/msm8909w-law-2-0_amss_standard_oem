#ifndef RR_SYS_INFO_NCELL_H
#define RR_SYS_INFO_NCELL_H

/*! \file rr_sys_info_ncell.h 
 
  This is the header file for the rr_sys_info_ncell.c module.
 
                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info_ncell.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "sys_type.h"
#include "rr_gprs_defs.h"
#include "geran_dual_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern void set_waiting_for_surround_response_metrics(
  boolean success,
  const gas_id_t gas_id
);

extern boolean get_waiting_for_surround_response(
  const gas_id_t gas_id
);

extern void set_waiting_for_surround_response(
  ARFCN_T arfcn,
  boolean si_from_cache,
  const gas_id_t gas_id
);

extern void mark_surround_response_as_cached(
  ARFCN_T arfcn,
  boolean si_from_cache,
  const gas_id_t gas_id
);

extern void clear_waiting_for_surround_response(
  const gas_id_t gas_id
);

extern void print_waiting_for_surround_response(
  const gas_id_t gas_id
);

extern boolean match_waiting_for_surround_response(
  ARFCN_T arfcn,
  const gas_id_t gas_id
);

extern boolean match_surround_response_with_cache_check(
  ARFCN_T arfcn,
  boolean *served_by_cache_ptr,
  const gas_id_t gas_id);

extern void rr_gprs_reset_ncell_sysinfo_manager(
  const gas_id_t gas_id
);

extern void rr_gprs_ncell_bcch_read_manager(
  gsm_ncell_list_member_t * bcch_ncell_info,
  boolean                   cancel_non_drx,
  const gas_id_t gas_id
);

extern void rr_init_ncell_data(
  const gas_id_t gas_id
);

extern void rr_gprs_init_gsm_neighbour_cell_list(
  const gas_id_t gas_id
);

extern boolean rr_is_this_the_pncell(
  ARFCN_T bcch_arfcn,
  const gas_id_t gas_id
);

extern boolean rr_gprs_is_this_a_surround_ncell(
  ARFCN_T bcch_arfcn,
  const gas_id_t gas_id
);

extern boolean rr_is_this_a_ncell(
  ARFCN_T bcch_arfcn,
  bcch_or_pmo_format_t *data_format,
  const gas_id_t gas_id
);

extern boolean rr_gprs_gsm_neighbour_cell_list_is_empty(
  const gas_id_t gas_id
);

extern boolean rr_gprs_pmo_pcco_ncells_are_present(
  const gas_id_t gas_id
);

extern void rr_gprs_remove_pmo_ncell_from_ba_gprs(
  uint8 ncell_index,
  const gas_id_t gas_id
);

extern void rr_gprs_add_ncell_to_ba_gprs(
  ARFCN_T                       bcch_arfcn,
  BSIC_T                       *bsic,
  gsm_bsic_description_entry_T *bsic_description,
  const gas_id_t                gas_id
);

extern void rr_gprs_reassign_band(
  uint16  freq_list_mask,
  boolean is_camped,
  const gas_id_t gas_id
);

extern boolean rr_gprs_add_pmo_ncell_to_ba_gprs(
  ARFCN_T                       bcch_arfcn,
  BSIC_T                       *bsic,
  gprs_cell_selection_params_t *gprs_cell_selection_params,
  const gas_id_t                gas_id
);

extern inter_task_BCCH_allocation_T * rr_gprs_get_ba_gprs(
  const gas_id_t gas_id
);

#ifdef FEATURE_SGLTE

/**
  @brief Copies the current unfiltered BA list to the location provided.
*/
extern void rr_gprs_get_ba_gprs_unfiltered(
  inter_task_BCCH_allocation_T * ba_gprs_unfiltered,
  const gas_id_t gas_id
);

#endif /* FEATURE_SGLTE */

extern void rr_gprs_invalidate_surround_ncell_sys_info(
  ARFCN_T arfcn,
  BSIC_T *bsic,
  const gas_id_t gas_id
);

extern void rr_gprs_revert_ba_gprs(
  const gas_id_t gas_id
);

extern void rr_gprs_ncell_sys_info_timer_expiry(
  const gas_id_t gas_id
);

extern uint8 rr_gprs_find_ba_gprs_index(
  ARFCN_T arfcn,
  BSIC_T *bsic,
  const gas_id_t gas_id
);

extern gsm_ncell_list_member_t * rr_gprs_get_ncell_info(
  ARFCN_T arfcn,
  BSIC_T *bsic,
  const gas_id_t gas_id
);

extern bcch_ncell_info_t * rr_gprs_get_surround_ncell_sys_info(
  ARFCN_T arfcn,
  const gas_id_t gas_id
);

extern pmo_ncell_info_t * rr_gprs_get_pmo_pcco_ncell_sys_info(
  ARFCN_T arfcn,
  BSIC_T *bsic,
  const gas_id_t gas_id
);

extern information_T * rr_gprs_get_surround_ncell_gsm_sys_info(
  ARFCN_T arfcn,
  const gas_id_t gas_id
);

extern gprs_ncells_info_t *rr_gprs_get_ncells_info(
  const gas_id_t gas_id
);

extern void rr_gprs_nsi_notify_scell_si_acquisition_required(
  const gas_id_t gas_id
);

extern void rr_ncell_si3_si4_received(
  ARFCN_T bcch_arfcn,
  bcch_ncell_info_t *ncell_info_ptr,
  const gas_id_t gas_id
);

extern void rr_ncell_si7_si8_received(
  ARFCN_T bcch_arfcn,
  bcch_ncell_info_t *ncell_info_ptr,
  const gas_id_t gas_id
);

#endif /* #ifndef RR_SYS_INFO_NCELL_H */

/* EOF */

