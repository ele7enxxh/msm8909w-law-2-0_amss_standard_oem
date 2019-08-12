/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Cell Selection Module

GENERAL DESCRIPTION
   This module contains functions for RR cell selection.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_cell_selection.c#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
May/20/11   SJW     Do not use the multi-pass retry flags during BPLMN activity
Jul/06/04    VT     For FEATURE_GERAN_STORED_LIST_CELL_SELECTION, For the
                      matching cell, moving the whole rr_l1_freq_power_T
                      type struct to the top of the list.
Jul/01/04    VT     For FEATURE_GERAN_STORED_LIST_CELL_SELECTION, increased
                      the number of freqs in pscan_list that are re-prioritized
                      from 75 to 150
Jun/24/04    VT     Added support for FEATURE_GERAN_STORED_LIST_CELL_SELECTION
May/26/04   DMB     Added MPH_START_IDLE_MODE_CNF handling
May/11/04    VT     In rr_selection_completed(), added call to
                      rr_send_abort_ind_if_lai_change_during_selection().
05/07/04    gc      Enhanced Trace Buffer Code
Jan/26/04    VT     QuadBand: In rr_store_plmn_select_req_info(), if band_pref
                      bit-mask has both PCS and DCS enabled, Set EGSM and DCS
                      as supported and allowed (Temporary).
Jan/24/04   VT      QuadBand: In rr_store_plmn_select_req_info(), added call to
                      rr_set_allowed_bands()
Jan/23/03   VT      QuadBand: In rr_store_plmn_select_req_info(), now
                      just ERRing instead of ASSERTing if both PCS & DCS are
                      enabled in band_pref.
Jan/09/03   VT      QuadBand: In rr_store_plmn_select_req_info(), now
                      ASSERTing if both PCS & DCS are enabled in band_pref.
05/29/03    he      Handled service domain change in PLMN select request.
05/08/03    he      Used separate debug_str buffer for logging.
05/02/03    He      Removed clear pscan results from reset function.
02/19/03    Rao     Added GSM Deep Sleep support.
02/07/03    he      Added support for cell re-establishment and access functions
                    to rr_cell_selection_details.
01/23/03    he      Fixed lint errors.
01/03/02    rmc     Log available PLMN database when cell selection ends
                    Return available PLMN information to MM when cell selection
                    abort timer expires
12/04/02    he      Cell selection not triggered only if service domain is unchanged.
11/15/02    rmc     Set band to PGSM when GPRS is supported
09/25/02    rmc     Perform early camping for PLMN selection
09/10/02    rmc     Invalidating the available PLMN database only when a parallel
                    decode operation is performed, but not after normal camping
08/26/02    rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
08/20/02    rmc     Updated to latest design to fix bugs in subsequent selections
                    after MM is given an available PLMN list
08/09/02    JAC     Added comments, cleanup
07/30/02    JAC     Cell selection optimizations version

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/

#include <string.h>
#include "rr_seg_load.h"
#include "rr_defs.h"
#include "ms.h"
#include "rr_cell_selection.h"
#include "rr_general.h"
#include "rr_l1_send.h"
#include "rr_mm_send.h"
#include "rr_shared_cell_selection.h"
#include "rr_decode_ie.h"
#include "rr_l2.h"
#include "rr_conn_establish.h"
#include "rr_log.h"
#include "rr_gprs_defs.h"
#include "rr_gprs_debug.h"
#include "rr_candidate_cell_db.h"
#include "rr_pscan.h"
#include "rr_plmn_list.h"
#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
#include "rr_w2g_service_redirection.h"
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */
#include "rr_arfcn.h"
#include "rr_uarfcn.h"
#include "rr_acq_db.h"
#include "rr_defs.h"
#include "rr_control.h"
#include "rr_sys_info_pcell.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
#include "rr_g2x.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

#include "rr_multi_sim.h"
#include "rr_l1_idle_mode.h"
#include "gprs_mem.h"

#include "rr_sys_info_cache.h"
#include "rr_sim.h"
#include "rr_select_bcch.h"
#include "rr_nv.h"
#include "rr_sys_info_si.h"

/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

typedef enum
{
  RR_SELECTION_IDLE,
  RR_SELECTION_WAIT_FOR_ACQ_DB_PSCAN_RESULTS,
  RR_SELECTION_WAIT_FOR_PSCAN_RESULTS,
  RR_SELECTION_DECODING_ACQ_DB_BCCH,
  RR_SELECTION_DECODING_BCCH,
  RR_SELECTION_WAIT_FOR_CHANNEL,
  RR_SELECTION_WAIT_FOR_SYS_INFO,
  RR_SELECTION_WAIT_FOR_SYS_INFO_RR_SELECT_BCCH,
  RR_SELECTION_ABORT
} cell_select_state_T;

typedef struct
{
  boolean bcch_list_cnf_received;

  /* signals whether early camping was performed */
  boolean early_camping_done;

  /* abort reasons */
  boolean mm_abort;
  boolean cell_selection_abort_timeout;

  /* True if RR_DEACT_REQ(RR_MM_MODE_CHANGE) is received during power scan, wait
  for the power scan to finish before aborting. */
  boolean abort_after_power_scan_cnf;

  #ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
  /* If aborted during IRAT redirection, abort_reason will be set to the
  appropriate value. */
  rr_event_T abort_reason;
  #ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
  boolean suitable_cell_found;
  #endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */
  #endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

  /* set FALSE if any service has been found, regardless PLMN, forbidden LAI, etc */
  boolean ue_oos;

  /* decode_bcch_list_req retry in progress */
  boolean retry_in_progress;

  /* indicated by L1 in MPH_DECODE_BCCH_LIST_CNF */
  boolean retry_required;


  boolean cell_selection_for_plmn_select_req;
} rr_cell_selection_details_T;

typedef struct
{
  rr_l1_idle_freq_list_T  high_priority_array;
  word                    tried_high_priority_cells;
  rr_l1_idle_freq_list_T  low_priority_array;
  word                    tried_low_priority_cells;
  boolean                 valid;
} rr_plmn_camping_list_T;

typedef struct
{
  cell_select_state_T cell_select_state;
  cell_select_state_T old_cell_select_state;
  rr_plmn_camping_list_T rr_plmn_camping_list;
  rr_cell_selection_details_T rr_cell_selection_details;
} rr_cell_selection_data_t;


/*==============================================================================
  Variables
==============================================================================*/

static rr_cell_selection_data_t rr_cell_selection_data[NUM_GERAN_DATA_SPACES];


/*==============================================================================

                    FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_ps_access_data_t*
 */
static rr_cell_selection_data_t *rr_cell_selection_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_cell_selection_data[as_index];
}

/**
 * Returns the name of the cell selection state in string form
 *
 * @param rr_cell_selection_data_ptr->cell_select_state State
 *
 * @return Name of the state in string form
 */
static char * rr_cell_selection_state_name(cell_select_state_T state)
{
  char *s = "";
  switch (state)
  {
    case RR_SELECTION_IDLE:
      s = "SelIdle";
      break;

    case RR_SELECTION_WAIT_FOR_ACQ_DB_PSCAN_RESULTS:
      s = "SelAcqDbPscan";
      break;

    case RR_SELECTION_WAIT_FOR_PSCAN_RESULTS:
      s = "SelPscan";
      break;

    case RR_SELECTION_DECODING_ACQ_DB_BCCH:
      s = "SelAcqDBBCCH";
      break;

    case RR_SELECTION_DECODING_BCCH:
      s = "SelBCCH";
      break;

    case RR_SELECTION_WAIT_FOR_CHANNEL:
      s = "SelChan";
      break;

    case RR_SELECTION_WAIT_FOR_SYS_INFO:
      s = "SelSI";
      break;

    case RR_SELECTION_WAIT_FOR_SYS_INFO_RR_SELECT_BCCH:
      s = "SelSI_RR_SELECT_BCCH";
      break;

    case RR_SELECTION_ABORT:
      s = "SelAbort";
      break;

    default:
      s = "Unknown";
      break;
  }

  return s;
} /* rr_cell_selection_state_name */

/*!
 * \brief Returns TRUE if the service request indicated ACQ-DB scan.
 * 
 * \param rr_PLMN_request_details_ptr 
 * 
 * \return boolean - TRUE if the RR_PLMN_SELECT_REQ indicated ACQ-DB scan, FALSE otherwise
 */
static boolean rr_sel_is_search_cause_service_req_acq_db_scan(
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr
)
{
  if ((rr_PLMN_request_details_ptr != NULL) &&
      (rr_PLMN_request_details_ptr->search_reason == RR_PLMN_SELECTION) &&
      (rr_PLMN_request_details_ptr->service_req_cause == RR_MM_ACQ_DB_SCAN))
  {
    return TRUE;
  }

  return FALSE;
}

/**
 * Updates the band preference data stored by WCDMA RRC
 *
 * @param band_pref_mask Latest band preference
 *
 */
void rr_update_band_preference_for_rrc(
  sys_band_mask_type band_pref_mask,
  const gas_id_t gas_id
)
{
  if ((rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE) ||
      (geran_is_multi_mode_gas_id(gas_id)          == FALSE))
  {
    /* do nothing, RRC does not need to be informed */
    return;
  }

#ifdef FEATURE_WCDMA
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rrc_update_band_preference_sub_id(band_pref_mask, geran_map_gas_id_to_nas_id(gas_id));
#else
  rrc_update_band_preference(band_pref_mask);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
#endif /* FEATURE_WCDMA */

  return;
}

void rr_update_band_preference_for_tdsrrc(
  sys_band_mask_type band_pref_mask,
  const gas_id_t gas_id
)
{
  if ((rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE) ||
      (geran_is_multi_mode_gas_id(gas_id)            == FALSE))
  {
    /* do nothing, TDS-RRC does not need to be informed */
    return;
  }

#ifdef FEATURE_GSM_TDS
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
  tdsrrc_update_band_preference_sub_id(band_pref_mask, RR_GAS_ID_TO_AS_ID);
#else
  tdsrrc_update_band_preference(band_pref_mask);
#endif /* FEATURE_TDS_SUB_ID_INTERFACE */
#endif /* FEATURE_GSM_TDS */

  return;
}

/**
 * Gets the ue_oos flag for use in RR_PLMN_SELECT_CNF.
 */
boolean rr_cell_selection_get_ue_oos_status(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  return rr_cell_selection_data_ptr->rr_cell_selection_details.ue_oos;
}

/**
 * Sets the ue_oos flag for use in RR_PLMN_SELECT_CNF.
 */
void rr_cell_selection_set_ue_oos_status(
  boolean status,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  rr_cell_selection_data_ptr->rr_cell_selection_details.ue_oos = status;
}

/**
 * Clears PLMN camping list
 */
static void rr_clear_plmn_list(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  rr_cell_selection_data_ptr->rr_plmn_camping_list.high_priority_array.num_frequencies = 0;
  rr_cell_selection_data_ptr->rr_plmn_camping_list.low_priority_array.num_frequencies = 0;
  rr_cell_selection_data_ptr->rr_plmn_camping_list.tried_high_priority_cells = 0;
  rr_cell_selection_data_ptr->rr_plmn_camping_list.tried_low_priority_cells = 0;
  rr_cell_selection_data_ptr->rr_plmn_camping_list.valid = FALSE;
} /* rr_clear_plmn_list */

/**
 * Clears cell selection details data structure
 */
void rr_clear_cell_selection_details(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received = FALSE;
  rr_cell_selection_data_ptr->rr_cell_selection_details.early_camping_done = FALSE;
  rr_cell_selection_data_ptr->rr_cell_selection_details.mm_abort = FALSE;
  rr_cell_selection_data_ptr->rr_cell_selection_details.cell_selection_abort_timeout = FALSE;
  rr_cell_selection_data_ptr->rr_cell_selection_details.abort_after_power_scan_cnf = FALSE;

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
  rr_cell_selection_data_ptr->rr_cell_selection_details.abort_reason = EV_NO_EVENT;
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
  rr_cell_selection_data_ptr->rr_cell_selection_details.suitable_cell_found = FALSE;
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

  rr_cell_selection_data_ptr->rr_cell_selection_details.ue_oos = TRUE;

  rr_cell_selection_data_ptr->rr_cell_selection_details.retry_required = FALSE;

  rr_cell_selection_data_ptr->rr_cell_selection_details.cell_selection_for_plmn_select_req = FALSE;
} /* rr_clear_cell_selection_details */

/**
 * Clears cell selection details, plmn list, pending
 * channel data and the cell selection abort timer
 */
static void rr_cleanup_cell_selection(
  const gas_id_t gas_id
)
{
  rr_clear_plmn_list(gas_id);
  rr_clear_cell_selection_details(gas_id);
  rr_clear_pending_channel_data(gas_id);
  rr_pscan_discard_common_results(gas_id);
  (void)rr_timer_cancel(RR_CELL_SELECTION_ABORT_TIMER, gas_id);
} /* rr_cleanup_cell_selection */


/**
 * Clears cell selection details, and starts abort timer
 */
static void rr_restart_cell_selection(
  rr_candidate_cell_db_T *candidate_db_ptr,
  const gas_id_t gas_id
)
{
  boolean retry_in_progress;

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  retry_in_progress = rr_cell_selection_data_ptr->rr_cell_selection_details.retry_in_progress;

  rr_clear_cell_selection_details(gas_id);
  rr_clear_plmn_list(gas_id);

  /* clear the pending channel data store */
  rr_clear_pending_channel_data(gas_id);

  /* reset the power scan offset in the candidate cell database in preparation for new selection */
  rr_candidate_db_clear_offset(candidate_db_ptr);

  MSG_GERAN_HIGH_0_G("Starting Cell Selection Abort Timer");
  (void)rr_timer_start(RR_CELL_SELECTION_ABORT_TIMER,RR_CELL_SELECTION_ABORT_TIMEOUT, gas_id);

  rr_cell_selection_data_ptr->rr_cell_selection_details.retry_in_progress = retry_in_progress;

} /* rr_restart_cell_selection */

/*!
 * \brief Sets the cell selection search reason.
 * 
 * \param search_reason (in)
 * \param gas_id (in)
 */
void rr_sel_set_search_reason(rr_search_reason_T search_reason, const gas_id_t gas_id)
{
  rr_get_PLMN_request_details_ptr(gas_id)->search_reason = search_reason;

} /* end rr_sel_set_search_reason() */

/*!
 * \brief Returns the cell selection search reason.
 * 
 * \param gas_id (in)
 * 
 * \return rr_search_reason_T 
 */
rr_search_reason_T rr_sel_get_search_reason(const gas_id_t gas_id)
{
  return rr_get_PLMN_request_details_ptr(gas_id)->search_reason;

} /* end rr_sel_get_search_reason() */

/**
 * Resets cell selection data structures
 */
static void rr_reset_cell_selection(
  rr_candidate_cell_db_T *candidate_db_ptr,
  const gas_id_t gas_id
)
{
  rr_clear_cell_selection_details(gas_id);
  rr_clear_plmn_list(gas_id);

  // Reset the search reason to its default value (may be relevant for later iRAT)
  rr_sel_set_search_reason(RR_AUTONOMOUS_CELL_SELECTION, gas_id);

  (void)rr_timer_cancel(RR_BCCH_DECODE_COMPLETE_TIMER, gas_id);
  (void)rr_timer_cancel(RR_CELL_SELECTION_ABORT_TIMER, gas_id);

  /*TODO this may be dangerous since other modules use these*/
  (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);
  (void)rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id);

} /* rr_reset_cell_selection */

/**
 * Stores the requested PLMN id. This also clears the equivalent PLMN id
 */
void rr_store_plmn_id(
  PLMN_id_T *PLMN_id,
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  rr_PLMN_request_details_ptr->requested_PLMN_id = (* PLMN_id);

  sys_plmn_undefine_plmn_id( &rr_PLMN_request_details_ptr->equivalent_PLMN_id );

} /* end rr_store_plmn_id() */


/**
 * Stores the equivalent PLMN id
 */
void rr_set_equivalent_plmn(
  PLMN_id_T *PLMN_id,
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  rr_PLMN_request_details_ptr->equivalent_PLMN_id = (* PLMN_id);
} /* end rr_set_equivalent_plmn() */

/**
 * Returns TRUE if the RR EPLMN is undefined
 */
boolean rr_equivalent_plmn_is_undefined(
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  if (rr_PLMN_request_details_ptr != NULL)
  {
    return (sys_plmn_id_is_undefined(rr_PLMN_request_details_ptr->equivalent_PLMN_id));
  }

  return TRUE;
} /* end rr_equivalent_plmn_is_undefined() */


// Returns the current service domain
sys_srv_domain_e_type rr_sel_get_service_domain(
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T * rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_PLMN_request_details_ptr);

  return rr_PLMN_request_details_ptr->service_domain;
}



byte rr_cell_selection_get_decode_bcch_retry_status(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  if (rr_cell_selection_data_ptr->rr_cell_selection_details.cell_selection_for_plmn_select_req)
  {
    return rr_cell_selection_data_ptr->rr_cell_selection_details.retry_in_progress;
  }

  return FALSE;
}

void rr_cell_selection_clear_decode_bcch_retry_status(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  rr_cell_selection_data_ptr->rr_cell_selection_details.retry_in_progress = FALSE;
}


/**
 * Compares the contents of a plmn_select_req with current settings
 *
 * @param rr_plmn_select_req
 *     message from MM for which PLMN to select
 */
boolean rr_compare_plmn_select_req_info(
  rr_plmn_select_req_T *rr_plmn_select_req,
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  if (rr_PLMN_request_details_ptr->network_select_mode == rr_plmn_select_req->network_select_mode &&
      rr_PLMN_request_details_ptr->service_domain == rr_plmn_select_req->service_domain &&
      rr_gsm_only(gas_id) == rr_rat_pri_list_info_gsm_only(&rr_plmn_select_req->rat_pri_list_info) &&
      rr_band_pref(gas_id) == RR_GET_BAND_CAP_CHGWT(rr_rat_pri_list_info_band_pref(&rr_plmn_select_req->rat_pri_list_info, SYS_SYS_MODE_GSM)) &&
      rr_search_band_pref(gas_id) == RR_GET_BAND_CAP_CHGWT(rr_rat_pri_list_info_search_band_pref(&rr_plmn_select_req->rat_pri_list_info, SYS_SYS_MODE_GSM)) &&
#ifdef FEATURE_WCDMA
      rr_is_wcdma_enabled(gas_id)==  rr_rat_pri_list_info_is_sys_mode_present(&rr_plmn_select_req->rat_pri_list_info,SYS_SYS_MODE_WCDMA) &&
      rr_band_pref_wcdma(gas_id) ==  RR_GET_BAND_CAP_CHGWT(rr_rat_pri_list_info_band_pref(&rr_plmn_select_req->rat_pri_list_info, SYS_SYS_MODE_WCDMA)) &&
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
      rr_is_lte_enabled(gas_id)== rr_rat_pri_list_info_is_sys_mode_present(&rr_plmn_select_req->rat_pri_list_info,SYS_SYS_MODE_LTE) &&
      rr_chk_if_equal_lte_band_mask(
        rr_band_pref_lte(gas_id), 
        RR_GET_BAND_CAP_LTE(rr_rat_pri_list_info_band_pref(&rr_plmn_select_req->rat_pri_list_info, SYS_SYS_MODE_LTE))
      ) &&
#endif /*FEATURE_LTE*/
#ifdef FEATURE_GSM_TDS
      rr_is_utran_tdd_enabled(gas_id) == rr_rat_pri_list_info_is_sys_mode_present(&rr_plmn_select_req->rat_pri_list_info,SYS_SYS_MODE_TDS) &&
      rr_band_pref_utran_tdd(gas_id) == RR_GET_BAND_CAP_CHGWT(rr_rat_pri_list_info_band_pref(&rr_plmn_select_req->rat_pri_list_info, SYS_SYS_MODE_TDS)) &&
#endif /*FEATURE_GSM_TDS*/
      sys_plmn_match(rr_PLMN_request_details_ptr->requested_PLMN_id, rr_plmn_select_req->PLMN_id))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* rr_compare_plmn_select_req_info */

/**
 * Process power scan results, stores them in the candidate cell db and requests a bcch list decode
 *
 * @return
 *     EV_NO_BCCH_FOUND - Power scan results indicate no cell are available
 *     RR_EV_DECODING_BCCH - A parallel BCCH decode operation was started
 */
static rr_event_T rr_selection_process_power_scan_results(
  rr_candidate_cell_db_T *candidate_db_ptr,
  rr_search_order_e       search_order,
  const gas_id_t          gas_id
)
{
  int num_frequencies_found;

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
  if (rr_x2g_redir_with_si_in_progress(gas_id))
  {
    rr_pscan_results_db_T * rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);

    rr_x2g_redir_prioritise_pscan_results(
      &rr_pscan_results_db_ptr->pscan_results, gas_id
    );
  }
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */

  num_frequencies_found = rr_pscan_process_power_scan_results(search_order, gas_id);

  /* if no frequencies were found in the power scan */
  if (num_frequencies_found == 0)
  {
    MSG_GERAN_HIGH_0_G("No frequencies found in pscan results");
    return EV_NO_BCCH_FOUND;
  }
  else /* some frequencies were found */
  {
    rr_event_T return_event;

    MSG_GERAN_HIGH_1_G("Stored=%d freqs in candidateDB",(int) num_frequencies_found);
    /* reset the flag to indicate that a MPH_DECODE_BCCH_LIST_CNF has not been received */
    rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received = FALSE;

    /* Tell L1 to do a bcch list decode */
    return_event = rr_perform_bcch_list_decode_from_candidate_db(candidate_db_ptr, gas_id);

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
    if (rr_x2g_redir_with_si_in_progress(gas_id))
    {
      rr_x2g_redir_with_si_send_si3_si4_if_available(
        candidate_db_ptr,
        gas_id
      );
    }
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */

    return return_event;
  }
} /* rr_selection_process_power_scan_results */

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG

/**
 * rr_perform_w2g_redirection_power_scan
 *
 * Performs a power scan.  If the global variable rr_w2g_redirection_cell_list
 * indicates that a 2G neighbour list is available, the power scan is
 * restricted to the entries in that list only.
 *
 * Side effect - the rr_w2g_redirection_cell_list is invalidated after use.
 */
static rr_event_T rr_perform_w2g_redirection_power_scan(
  rr_candidate_cell_db_T *candidate_db_ptr,
  boolean acq_db_search_allowed,
  const gas_id_t gas_id
)
{
  rr_pscan_results_db_T            *rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);
  rr_reselection_pscan_cell_list_T *rr_w2g_redir_cell_list_ptr = rr_get_w2g_redirection_cell_list_ptr(gas_id);

  if (rr_w2g_redir_cell_list_ptr->valid_data)
  {
    /* clear the candidate database and available plmn database since a new */
    /* power scan is being started from scratch and all the old results would be invalid */
    rr_pscan_clear_results(rr_pscan_results_db_ptr);
    rr_candidate_db_clear(candidate_db_ptr);

    /* log the start of a power scan */
    rr_log_reselection_power_scan_started(gas_id);

    /* Truncating no_of_entries to RR_MAX_RES_PSCAN_CELL_LIST_SIZE for KW*/   
    if(rr_w2g_redir_cell_list_ptr->no_of_entries > RR_MAX_RES_PSCAN_CELL_LIST_SIZE)
    {
      rr_w2g_redir_cell_list_ptr->no_of_entries = RR_MAX_RES_PSCAN_CELL_LIST_SIZE;
      MSG_GERAN_HIGH_2_G("W2G REDIR PSCAN cell list size capped to %d from %d ",
                          RR_MAX_RES_PSCAN_CELL_LIST_SIZE,
                          rr_w2g_redir_cell_list_ptr->no_of_entries
                        );
      
    }
    rr_pscan_add_frequencies(
      rr_w2g_redir_cell_list_ptr->cell_list,
      (uint16)rr_w2g_redir_cell_list_ptr->no_of_entries,
      gas_id
    );

    rr_send_mph_power_scan_req(
      &rr_pscan_results_db_ptr->pscan_results,
      rr_pscan_results_db_ptr->bands_searched,
      RR_SEARCH_RESELECTION,
      gas_id
    );

    rr_pscan_set_band_pref(rr_band_pref(gas_id), gas_id);

#ifndef FEATURE_LTE
#ifndef FEATURE_WTOG_REDIRECTION_AFTER_CONN_RELEASE
    rr_w2g_redir_cell_list_ptr->valid_data = FALSE;
#endif /* FEATURE_WTOG_REDIRECTION_AFTER_CONN_RELEASE */
#endif /* FEATURE_LTE */

    return RR_EV_PERFORM_POWER_SCAN;
  }
  else
  {
    /* RR may or may not have been provided with a PLMN id, depending on the
    type of X2G redirection that is being performed.  Regardless of this, we
    want to use the RR ACQ-DB if possible, so we set the PLMN here an undefined
    value.  This will cause the RR ACQ-DB power scan to use ARFCNs from the
    ACQ-DB for any PLMNs that are available. */
    if (acq_db_search_allowed)
    {
      rr_event_T output_event = EV_NO_EVENT;
      PLMN_id_T  plmn_id;
      plmn_id.identity[0] = 0xFF;
      plmn_id.identity[1] = 0xFF;
      plmn_id.identity[2] = 0xFF;

      output_event = rr_pscan_perform_pscan_acq_db(
        rr_pscan_results_db_ptr,
        rr_band_pref(gas_id),
        plmn_id,
        FALSE, /* Allow ACQ-DB search on entries that match other PLMNs */
        RR_SEARCH_NORMAL,
        RR_PSCAN_REQ_NONE,
        gas_id
      );

      if (output_event == EV_NO_BCCH_FOUND)
      {
        output_event = rr_perform_power_scan(
          RR_SEARCH_NORMAL,
          rr_band_pref(gas_id),
          FALSE,
          gas_id
        );
      }

      return output_event;
    }
    else
    {
      return rr_perform_power_scan(
        RR_SEARCH_NORMAL,
        rr_band_pref(gas_id),
        FALSE,
        gas_id
      );
    }
  }
}
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */


/**
 * Starts a cell selection, starts abort timer, and requests L1 to do a power scan
 *
 * @return
 *     RR_EV_PERFORM_POWER_SCAN - power scan requested
 *     Event from rr_selection_process_power_scan
 */
static rr_event_T rr_selection_start_cell_selection(
  boolean acq_db_search_allowed,
  const gas_id_t gas_id
)
{
  rr_candidate_cell_db_T    *candidate_db_ptr = rr_get_candidate_cell_db_ptr(gas_id);
  rr_available_plmn_db_T    *available_plmn_db_ptr = rr_get_available_plmn_db_ptr(gas_id);
  rr_event_T                 output_event = EV_NO_EVENT;
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  rr_pscan_results_db_T     *rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);

  if (rr_pscan_data_capture_enabled(gas_id))
  {
    MSG_GERAN_HIGH_1_G(
      "PSCAN data capture: ACQ-DB search not allowed (was %d)",
      (int) acq_db_search_allowed
    );
    acq_db_search_allowed = FALSE;
  }

  MSG_GERAN_MED_1_G(
    "rr_selection_start_cell_selection(acq_db_search_allowed => %d)",
    (int) acq_db_search_allowed
  );

  /**************************************************************************/
  /* Starting a cell selection, called by either rr_process_plmn_select_req */
  /* or rr_perform_autonomous_cell_selection                                */
  /**************************************************************************/

  /* clear out the plmn camping list used by cell selection */
  rr_clear_plmn_list(gas_id);

  /* Always clear the available PLMN database as it is no longer valid. */
  rr_plmn_db_clear(available_plmn_db_ptr, gas_id);

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
  if ((rr_PLMN_request_details_ptr->search_reason == RR_W2G_SERVICE_REDIRECTION) ||
      (rr_PLMN_request_details_ptr->search_reason == RR_W2G_SERVICE_REDIRECTION_EMERGENCY_CALL))
  {
    return rr_perform_w2g_redirection_power_scan(candidate_db_ptr, acq_db_search_allowed, gas_id);
  }
  else
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */
  {
    boolean perform_full_power_scan = FALSE;
    sys_band_mask_type band_pref = rr_search_band_pref(gas_id);

    // Check if previous PSCAN results can be used
    if (rr_pscan_results_are_reusable(band_pref, gas_id))
    {
      MSG_GERAN_HIGH_0_G("Use existing pscan results");

      // Exit early here
      return EV_USE_EXISTING_POWER_SCAN_RESULTS;
    }

    if (!acq_db_search_allowed)
    {
      perform_full_power_scan = TRUE;
    }
    else
    {
      boolean requested_plmn_only = TRUE;

      if (rr_is_any_cell_selection_mode(gas_id) ||
          rr_sel_is_search_cause_service_req_acq_db_scan(rr_PLMN_request_details_ptr))
      {
        /* Requested PLMN will be set to undefined value which will not
        match any entries in the ACQ-DB, but it is still advantageous to
        prioritise our search based on entries in the ACQ-DB (recently
        seen GSM cells), as we will camp on any GSM cell in limited service
        mode. */
        requested_plmn_only = FALSE;
      }

      /* Attempt to perform an acq-db power scan.  If this fails (no freqs
      for the requested PLMN) then perform a full power scan. */
      output_event = rr_pscan_perform_pscan_acq_db(
        rr_pscan_results_db_ptr,
        rr_search_band_pref(gas_id),
        rr_PLMN_request_details_ptr->requested_PLMN_id,
        requested_plmn_only,
        RR_SEARCH_NORMAL,
        RR_PSCAN_REQ_CELL_SELECTION,
        gas_id
      );

      if (output_event == EV_NO_BCCH_FOUND)
      {
        if (rr_sel_is_search_cause_service_req_acq_db_scan(rr_PLMN_request_details_ptr))
        {
          MSG_GERAN_HIGH_0_G("Acq DB empty and service_req_cause is RR_MM_ACQ_DB_SCAN, abort PLMN_SEL_REQ");
        }
        else
        {
          perform_full_power_scan = TRUE;
        }
      }
    }

    if (perform_full_power_scan)
    {
      sys_band_mask_type band_pref = rr_search_band_pref(gas_id);

      {
        /* No point doing acq-db power scan if in any cell selection mode, as we
        don't have a preferred PLMN. */
        output_event = rr_perform_power_scan(
          RR_SEARCH_NORMAL,
          band_pref,
          FALSE,
          gas_id
        );
      }
    }
  }

  return output_event;

} /* rr_selection_start_cell_selection */

/**
 * Ask L1 to do acquire a specific bcch. The pending channel data store is set up based on the candidate db.
 * The pscan results and available plmn database are cleared.
 *
 * @param candidate
 *     cell to select to
 *
 * @return
 *     RR_EV_SPECIFIC_CHANNEL_SEARCH
 */
static rr_event_T rr_selection_perform_specific_channel_search(
  rr_candidate_cell_db_T *candidate_db_ptr,
  rr_candidate_cell_T *candidate,
  boolean send_BSIC_in_slect_req,
  const gas_id_t gas_id
)
{
  BSIC_T BSIC={0};
  gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  /****************************************************************************************/
  /* NOTE: the available PLMN database is not cleared since it may still be valid at this */
  /* point                                                                                */
  /****************************************************************************************/

  /*********************************************************************/
  /* The candidate db should not be cleared because we may be in a     */
  /* in a plmn selection using the plmn camping list. But the offset   */
  /* in the candidate database should be cleared in case the candidate */
  /* DB is used for later cell selection operations                    */
  /*********************************************************************/
  rr_candidate_db_clear_offset(candidate_db_ptr);

  /* prepare the pending channel data for camping */
  rr_prepare_sidb_for_camping(
    &pcell_info_ptr->gsm,
    candidate->meas.arfcn,
    /* BSIC will not be known until L1 acquires the cell, for now it is set to 0 */
    BSIC,
    candidate->meas.RXLEV_average,
    gas_id
  );

  /*copy everything in candidate cell, need control channel description for early camping*/
  pcell_info_ptr->gsm.gprs_indicator = candidate->si_info.GPRS_indicator;
  if (candidate->si_info.control_channel_description_valid)
  {
    pcell_info_ptr->gsm.control_channel_description = candidate->si_info.control_channel_description;
    pcell_info_ptr->gsm.control_channel_description_valid = TRUE;
  }
  else
  {
    pcell_info_ptr->gsm.control_channel_description_valid = FALSE;
  }
  pcell_info_ptr->gsm.two_quater_position = candidate->si_info.two_quater_position;
  pcell_info_ptr->gsm.two_ti = candidate->si_info.two_ter_indicator;

  MSG_GERAN_HIGH_2_G("Attempting to camp on Cell=(%d,%d)",
    pcell_info_ptr->gsm.BCCH_ARFCN.num,
    pcell_info_ptr->gsm.BCCH_ARFCN.band);

  /* valid data flag should be set to false since not all SI needed to fully camp has been read */
  pcell_info_ptr->gsm.valid_data_flag = FALSE;

  rr_cell_selection_data_ptr->rr_cell_selection_details.early_camping_done = FALSE;

  if (candidate->si_info.cell_id_valid)
  {
    pcell_info_ptr->gsm.cell_identity_valid_flag = TRUE;
    pcell_info_ptr->gsm.cell_identity = candidate->si_info.cell_id;
  }

  pcell_info_ptr->gsm.location_area_identification = candidate->si_info.lai;

  // Setup BCCH SI scheduling information
  rr_candidate_get_bsi_scheduling_info(
    candidate,
    &pcell_info_ptr->bcch_sys_info.si_messages_on_bcch_ext,
    &pcell_info_ptr->bcch_sys_info.si_messages_not_broadcast
  );

  rr_si_print_bitmask("si_messages_on_bcch_ext", pcell_info_ptr->bcch_sys_info.si_messages_on_bcch_ext);
  rr_si_print_bitmask("si_messages_not_broadcast", pcell_info_ptr->bcch_sys_info.si_messages_not_broadcast);

  /* log the start of a camping attempt */
  rr_log_camping_attempt_start(pcell_info_ptr->gsm.BCCH_ARFCN,BSIC, gas_id);

  /* Tell L1 to reselect */
  if(send_BSIC_in_slect_req)
  {
    BSIC = candidate->bsic;
  }
  else
  {
    BSIC.PLMN_colour_code = INVALID_PLMN_CC;
    BSIC.BS_colour_code = INVALID_BSIC_CC;
  }

  rr_send_mph_select_specific_channel(pcell_info_ptr->gsm.BCCH_ARFCN, BSIC, gas_id);   	

  return RR_EV_SPECIFIC_CHANNEL_SEARCH;
} /* rr_selection_perform_specific_channel_search */

/**
 * Searches PLMN camping list for a untried cell to camp on
 *
 * @return
 *     Event from rr_selection_start_cell_selection if no cell found
 *     Event from rr_selection_perform_specific_channel_search if cell found
 */
static rr_event_T rr_selection_perform_plmn_camping_list_channel_search(
  rr_candidate_cell_db_T *candidate_db_ptr,
  const gas_id_t gas_id
)
{
  rr_candidate_cell_T *temp_candidate = NULL;
  ARFCN_T              temp_bcch;
  boolean              cell_found     = FALSE;
  word                 temp_channel_number;

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  /***************************************************************************/
  /* A PLMN_select_req was requested, and we are using our plmn_camping_list */
  /* to find a suitable cell                                                 */
  /***************************************************************************/

  /*****************************************************************************/
  /* Loop through the PLMN camping list looking for a suitable/acceptable cell */
  /* starting from the last high priority tried cell                           */
  /*****************************************************************************/
  while (cell_found == FALSE)
  {
    if (rr_cell_selection_data_ptr->rr_plmn_camping_list.high_priority_array.num_frequencies > rr_cell_selection_data_ptr->rr_plmn_camping_list.tried_high_priority_cells )
    {
      temp_bcch = rr_cell_selection_data_ptr->rr_plmn_camping_list.high_priority_array.arfcn[rr_cell_selection_data_ptr->rr_plmn_camping_list.tried_high_priority_cells];
      rr_cell_selection_data_ptr->rr_plmn_camping_list.tried_high_priority_cells++;
    }
    else if (rr_cell_selection_data_ptr->rr_plmn_camping_list.low_priority_array.num_frequencies > rr_cell_selection_data_ptr->rr_plmn_camping_list.tried_low_priority_cells )
    {
      temp_bcch = rr_cell_selection_data_ptr->rr_plmn_camping_list.low_priority_array.arfcn[rr_cell_selection_data_ptr->rr_plmn_camping_list.tried_low_priority_cells];
      rr_cell_selection_data_ptr->rr_plmn_camping_list.tried_low_priority_cells++;
    }
    else
    {
      MSG_GERAN_HIGH_0_G("No suitable cells in plmn camp list");
      break;
    }

    /* Got a cell but need to check cell using info in candidate db */
    temp_candidate = rr_candidate_db_get_entry(candidate_db_ptr, temp_bcch);

    if ( temp_candidate != NULL )
    {
      if ( rr_check_candidate_suitable(
             temp_candidate,
             rr_is_any_cell_selection_mode(gas_id),
             rr_is_automatic_plmn_selection_mode(gas_id),
             gas_id
           )
           ==TRUE
         )
      {
        cell_found=TRUE;
      }
      else
      {
        temp_channel_number = temp_bcch.num;
        MSG_GERAN_HIGH_1_G("%d not suitable for plmn camp list",(int) temp_channel_number);
      }
    }
    else
    {
      temp_channel_number = temp_bcch.num;
      MSG_GERAN_HIGH_1_G("Missing %d in candidate db",(int) temp_channel_number);
    }
  }

  if (cell_found==FALSE)
  {
    /*************************************************************/
    /* No suitable cells in plmn_camping_list                    */
    /* Instead of using plmn_camping list start a cell selection */
    /*************************************************************/
    return rr_selection_start_cell_selection(TRUE, gas_id);
  }
  else
  {
    /* We have a cell attempt to camp on it*/
    return rr_selection_perform_specific_channel_search(candidate_db_ptr, temp_candidate,FALSE, gas_id);
  }
} /* rr_selection_perform_plmn_camping_list_channel_search */


/* Returns a bitmask-representation of the given band */
static sys_band_mask_type get_band_mask(
  rr_internal_band_T band
)
{
  sys_band_mask_type band_mask = SYS_BAND_MASK_EMPTY;

  if ( band == BAND_EGSM )
  {
    band_mask = SYS_BAND_MASK_GSM_EGSM_900;
  }
  else
  if ( band == BAND_PGSM )
  {
    band_mask = SYS_BAND_MASK_GSM_PGSM_900;
  }
  else
  if ( band == BAND_PCS )
  {
    band_mask = SYS_BAND_MASK_GSM_PCS_1900;
  }
  else
  if ( band == BAND_DCS )
  {
    band_mask = SYS_BAND_MASK_GSM_DCS_1800;
  }
  else
  if ( band == BAND_GSM_850 )
  {
    band_mask = SYS_BAND_MASK_GSM_850;
  }

  return band_mask;

} /* end get_band_mask() */


/**
 * Responds to a plmn_select_req, stores info in rr_plmn_request_details. Does a
 * cell selection if current serving cell is not suitable.
 *
 * @param rr_plmn_select_req
 *               (in) message requesting plmn selection
 * @param service_domain_changed_ptr
 *               (out) indicates if the service domain has been changed (optional)
 * @return
 *     RR_EV_NO_CELL_RESELECTION - current serving cell is suitable
 *     Event from rr_selection_perform_plmn_camping_list_channel if available plmn db has that plmn
 *     Event from rr_selection_start_cell_selection
 */
rr_event_T rr_process_plmn_select_req(
  rr_plmn_select_req_T *rr_plmn_select_req,
  boolean *service_domain_changed_ptr,
  const gas_id_t gas_id
)
{
  rr_candidate_cell_db_T    *candidate_db_ptr = rr_get_candidate_cell_db_ptr(gas_id);
  sys_mcc_type               mcc;
  sys_mnc_type               mnc;
  boolean                    is_undefined;
  boolean                    includes_pcsd;
  boolean                    old_gsm_only;
  sys_band_mask_type         old_band_pref;
  boolean                    cell_suitable = FALSE;
#if defined(FEATURE_SGLTE_DUAL_SIM) && defined (FEATURE_ASYMMETRIC_GERAN_BANDS) 
  boolean                    band_changed = FALSE;
#endif /*FEATURE_SGLTE_DUAL_SIM && FEATURE_ASYMMETRIC_GERAN_BANDS*/
  
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
#ifdef FEATURE_WCDMA
  boolean            old_wcdma_enabled       = rr_is_wcdma_enabled(gas_id);
  sys_band_mask_type old_wcdma_band_pref     = rr_band_pref_wcdma(gas_id);
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
  boolean                  old_lte_enabled         = rr_is_lte_enabled(gas_id);
  sys_lte_band_mask_e_type old_lte_band_pref       = rr_band_pref_lte(gas_id);
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
  boolean            old_utran_tdd_enabled   = rr_is_utran_tdd_enabled(gas_id);
  sys_band_mask_type old_utran_tdd_band_pref = rr_band_pref_utran_tdd(gas_id);
#endif /* FEATURE_GSM_TDS */

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  /* RR must always clear any pending mode change info if a new
  RR_PLMN_SELECT_REQ is received. */
  rr_pending_mode_change_data_clear(gas_id);

  if ((rr_plmn_select_req == NULL) || (candidate_db_ptr == NULL))
  {
    return(EV_NO_EVENT);
  }

  old_gsm_only = rr_gsm_only(gas_id);
  old_band_pref = rr_band_pref(gas_id);

  // Check if the service domain has been changed and update the output parameter (if supplied)
  if (service_domain_changed_ptr != NULL)
  {
    if (rr_plmn_select_req->service_domain != rr_PLMN_request_details_ptr->service_domain)
    {
      (*service_domain_changed_ptr) = TRUE;
    }
    else
    {
      (*service_domain_changed_ptr) = FALSE;
    }
  }

  // Store the new values

  /* save the network select mode */
  rr_PLMN_request_details_ptr->network_select_mode = rr_plmn_select_req->network_select_mode;

  /*save the PLMN*/
  rr_store_plmn_id(&rr_plmn_select_req->PLMN_id, gas_id);

#ifdef FEATURE_LTE_REL11
  rr_g2w_depri_update_lte_about_requested_plmn(&rr_plmn_select_req->PLMN_id, gas_id);
#endif /*FEATURE_LTE_REL11*/

  rr_PLMN_request_details_ptr->requested_PLMN_id = rr_plmn_select_req->PLMN_id;

  /*setup the search reason*/
  rr_PLMN_request_details_ptr->search_reason = RR_PLMN_SELECTION;

  /*store the EHPLMN flag */
  rr_PLMN_request_details_ptr->is_ehplmn_to_be_camped = rr_plmn_select_req->is_ehplmn_to_be_camped;

  /* store the service_domain */
  rr_PLMN_request_details_ptr->service_domain = rr_plmn_select_req->service_domain; 

  rr_PLMN_request_details_ptr->rat_pri_list_info = rr_plmn_select_req->rat_pri_list_info;

  /* store the service request cause*/
  rr_PLMN_request_details_ptr->service_req_cause = rr_plmn_select_req->service_req_cause;

  if (rr_band_pref(gas_id) != old_band_pref)
  {
    MSG_GERAN_HIGH_2_G(
      "Band pref has changed (0x%08x to 0x%08x)",
      old_band_pref,
      rr_band_pref(gas_id)
    );
#if defined(FEATURE_SGLTE_DUAL_SIM) && defined (FEATURE_ASYMMETRIC_GERAN_BANDS)
   band_changed = TRUE;
#endif /*FEATURE_SGLTE_DUAL_SIM && FEATURE_ASYMMETRIC_GERAN_BANDS*/
  }

  rr_set_preferred_bands(rr_band_pref(gas_id), gas_id);

  rr_update_band_preference_for_rrc(rr_band_pref_wcdma(gas_id), gas_id);

#ifdef FEATURE_GSM_TDS

#ifdef FEATURE_SGLTE
  if (rr_is_sglte_mode_active(gas_id) && rr_gsm_only(gas_id))
  {
    MSG_GERAN_HIGH_0_G("SGLTE: SGLTE active, GSM-only, inhibit TDS band pref update");
  }
  else
#endif /* FEATURE_SGLTE */
  {
    if (rr_is_utran_tdd_enabled(gas_id))
    {
      rr_update_band_preference_for_tdsrrc(rr_band_pref_utran_tdd(gas_id), gas_id);
    }
  }

#endif /*FEATURE_GSM_TDS*/

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined (FEATURE_LTE)
  if (rr_band_pref(gas_id) != old_band_pref)
  {
    if (!rr_gsm_only(gas_id))
    {
      if (rr_is_camped(gas_id))
      {
        rr_l1_update_wcdma_neighbor_list(gas_id);
      }
    }
  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */


  MSG_GERAN_HIGH_0_G("RGS_MULTIPASS: reset retry_in_progress to 0");
  rr_cell_selection_data_ptr->rr_cell_selection_details.retry_in_progress = FALSE;

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  if (rr_gsm_only(gas_id) != old_gsm_only)
  {
    /* Indicate the changed mode to L1 */
    rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id), gas_id);
  }

#ifdef FEATURE_WCDMA
  if ((rr_is_wcdma_enabled(gas_id) != old_wcdma_enabled) ||
      (rr_band_pref_wcdma(gas_id)  != old_wcdma_band_pref))
  {
    rr_g2w_wcdma_measurements_delete(gas_id);
  }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
  if ((rr_is_lte_enabled(gas_id) != old_lte_enabled) ||
      (!rr_chk_if_equal_lte_band_mask(rr_band_pref_lte(gas_id), old_lte_band_pref)))
  {
    rr_g2w_lte_measurements_delete(gas_id);
  }
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
  if ((rr_is_utran_tdd_enabled(gas_id) != old_utran_tdd_enabled) ||
      (rr_band_pref_utran_tdd(gas_id)  != old_utran_tdd_band_pref))
  {
    rr_g2w_utran_tdd_measurements_delete(gas_id);
  }
#endif /* FEATURE_GSM_TDS */

  rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

#else /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
  rr_l1_gsm_stop_reason_set(RR_L1_DEEP_SLEEP, gas_id);
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

  rr_l1_gsm_deact_reason_set(RR_L1_MODE_CHANGE, gas_id);

  MSG_GERAN_HIGH_3_G("PLMNSelReq,any=%d,auto=%d,csOnly=%d",
           (int) rr_is_any_cell_selection_mode(gas_id),
           (int) rr_is_automatic_plmn_selection_mode(gas_id),
           (int) (rr_PLMN_request_details_ptr->service_domain == SYS_SRV_DOMAIN_CS_ONLY)
          );
  MSG_GERAN_HIGH_1_G("is_ehplmn_to_be_camped=%d", 
           (int) (rr_PLMN_request_details_ptr->is_ehplmn_to_be_camped)
          );

  sys_plmn_get_mcc_mnc(rr_PLMN_request_details_ptr->requested_PLMN_id,&is_undefined,&includes_pcsd,&mcc,&mnc);

  if (!rr_is_any_cell_selection_mode(gas_id))
  {
    MSG_GERAN_HIGH_3_G("RequestedPLMN=%d-%d (%d)",(int) mcc, (int) mnc, (int) includes_pcsd);
  }

  /* log cell selection starting */
  rr_log_cell_selection_start(
    RR_LOG_CELL_SELECTION_START_PLMN_SELECT_REQ,
    rr_is_any_cell_selection_mode(gas_id),
    rr_is_automatic_plmn_selection_mode(gas_id),
    gas_id
  );

  if (rr_is_camped(gas_id))
  {
    /* Check if we are already camped on a cell that fits the criteria*/
    cell_suitable = rr_cell_selection_is_camped_cell_suitable(gas_id);

    if (cell_suitable)
    {
      /* log the cell selection end */
      rr_log_cell_selection_end(RR_LOG_CELL_SELECTION_END_ALREADY_CAMPED, gas_id);

      MSG_GERAN_HIGH_0_G("Already camped on suitable cell");

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined (FEATURE_LTE)
      /* Update L1 with the current IRAT neighbour list - this will take in
      to account any changes in supported RATs or bands. */
      rr_l1_update_wcdma_neighbor_list(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
    }
  }

#if defined(FEATURE_SGLTE_DUAL_SIM) && defined (FEATURE_ASYMMETRIC_GERAN_BANDS)
 if(band_changed)
 {
   MSG_GERAN_HIGH_0_G("SGLTE:MultiSim: band changed ");  
   cell_suitable = FALSE;
   rr_invalidate_power_scan_results(gas_id);   
 } 
#endif /*FEATURE_SGLTE_DUAL_SIM && FEATURE_ASYMMETRIC_GERAN_BANDS*/
 


  if ( cell_suitable )
  {
    return RR_EV_NO_CELL_SELECTION_REQUIRED;
  }
  else
  {
    return RR_EV_PROCESS_PLMN_SELECT_REQ;
  }
} /* end rr_process_plmn_select_req() */



static rr_event_T rr_perform_plmn_select_req_selection(
  rr_candidate_cell_db_T * candidate_db_ptr,
  rr_available_plmn_db_T * available_plmn_db_ptr,
  boolean                  acq_db_search_allowed,
  const gas_id_t           gas_id
)
{
  /**********************************************************/
  /* We need to do a cell selection since we are not camped */
  /* on a cell that meets the criterias                     */
  /**********************************************************/
  rr_PLMN_request_details_T     *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  scell_info_ptr->gsm.camped_on_flag = FALSE;

  rr_initialize_SI2q_instances(gas_id);
  rr_initialize_SI2ter_instances(gas_id);

  /* Reset cell selection details, plmn camping list, and start abort timer */
  rr_restart_cell_selection(candidate_db_ptr, gas_id);
  rr_cell_selection_data_ptr->rr_cell_selection_details.cell_selection_for_plmn_select_req = TRUE;


  if (rr_plmn_db_is_valid(available_plmn_db_ptr, rr_band_pref(gas_id), gas_id))
  {
    /**************************************************************************/
    /* Check if our available plmn database is valid, if it is we don't need  */
    /* to do a complete search                                                */
    /**************************************************************************/

    /*************************************************************/
    /* Since the plmn db is still valid it must be fairly new    */
    /* We do not have to redo the power scan, e the */
    /* candidate cell list if we can find a suitable/acceptable  */
    /* cell                                                      */
    /*************************************************************/
    PLMN_id_T* temp_plmn_ptr=NULL;

    MSG_GERAN_HIGH_0_G("Using valid available PLMN DB");

    if (!rr_is_any_cell_selection_mode(gas_id))
    {
      temp_plmn_ptr=&rr_PLMN_request_details_ptr->requested_PLMN_id;
    }

    /* get new cells */
    rr_plmn_db_get_plmn_frequency_info(
          available_plmn_db_ptr,
          temp_plmn_ptr,
          &rr_cell_selection_data_ptr->rr_plmn_camping_list.high_priority_array,
          &rr_cell_selection_data_ptr->rr_plmn_camping_list.low_priority_array,
          gas_id
    );

    /* if there are either normal or low priority cells in the plmn camping list */
    if ((rr_cell_selection_data_ptr->rr_plmn_camping_list.high_priority_array.num_frequencies != 0) ||
        (rr_cell_selection_data_ptr->rr_plmn_camping_list.low_priority_array.num_frequencies  != 0))
    {
      /* try to use the plmn camping list first */
      rr_cell_selection_data_ptr->rr_plmn_camping_list.valid = TRUE;

      /*Frequencies were found for that plmn, do a specific search*/
      return rr_selection_perform_plmn_camping_list_channel_search(candidate_db_ptr, gas_id);
    }
    else /* the PLMN camping list is empty */
    {
      MSG_GERAN_HIGH_0_G("No cells found in available PLMN DB, invalidate");
      /******************************************************************/
      /* It is likely that there have been significant changes in the   */
      /* signaling environment, otherwise the requested PLMN would have */
      /* been found... invalidate the available PLMN database and       */
      /* power scan results                                             */
      /******************************************************************/
      rr_candidate_db_set_valid(candidate_db_ptr,FALSE);
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Available PLMN DB not valid");
  }

  return rr_selection_start_cell_selection(acq_db_search_allowed, gas_id);

} /* rr_perform_plmn_select_req_selection */

static rr_event_T rr_selection_process_low_priority_cells(
  rr_candidate_cell_db_T *candidate_db_ptr,
  rr_available_plmn_db_T *available_plmn_db_ptr,
  const gas_id_t gas_id
)
{
  rr_candidate_cell_T *candidate_cell_entry_ptr;
  boolean              done = FALSE;
  boolean              cell_found;

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  MSG_GERAN_HIGH_0_G("Attempting to find low priority cells");

  while (!done)
  {
    /* Find the strongest unprocessed normal priority cell in the database */
    cell_found = rr_candidate_db_find_next(
      candidate_db_ptr,
      &candidate_cell_entry_ptr,
      RR_CANDIDATE_LOW_PRIORITY_CELLS
    );

    if (!cell_found)
    {
      /* did not find any low priority unprocessed cells in the database */
      return EV_NO_BCCH_FOUND;
    }
    else if (candidate_cell_entry_ptr != NULL) /* low priority cell was found */
    {
      boolean cell_suitable;
      ARFCN_T arfcn = candidate_cell_entry_ptr->meas.arfcn;

      /* if the cell is still unknown */
      if (candidate_cell_entry_ptr->type == RR_CANDIDATE_UNKNOWN)
      {
        /* should not be unknown at this stage */
        MSG_GERAN_ERROR_2_G("Type for (%d,%d) was unknown", arfcn.num, arfcn.band);

        /* mark the type as NOT_BCCH */
        candidate_cell_entry_ptr->type = RR_CANDIDATE_NOT_BCCH;
      }

      /* mark the entry as processed */
      candidate_cell_entry_ptr->processed = TRUE;

      /* only process valid BCCH entries further */
      if (candidate_cell_entry_ptr->type == RR_CANDIDATE_VALID_BCCH)
      {
        cell_suitable = rr_check_candidate_suitable(
          candidate_cell_entry_ptr,
          rr_is_any_cell_selection_mode(gas_id),
          rr_is_automatic_plmn_selection_mode(gas_id),
          gas_id
        );

        if (cell_suitable)
        {
          /* Check if the cell is forbidden */
          if (rr_cell_forbidden(candidate_cell_entry_ptr->meas.arfcn, gas_id))
          {
            cell_suitable = FALSE;
            MSG_GERAN_HIGH_2_G("Cell=(%d,%d) forbidden, continue searching for suitable cell",
              arfcn.num,
              arfcn.band);
          }
        }

        /* There is a candidate cell, need to check for suitability */
        if (cell_suitable)
        {
          MSG_GERAN_HIGH_2_G("Cell=(%d,%d) suitable low priority, camp", arfcn.num, arfcn.band);

          /* before clearing available PLMN DB, log it */
          (void)rr_log_available_plmn_list(available_plmn_db_ptr, gas_id);

          /****************************************************************/
          /* A parallel decode operation is being interrupted. Invalidate */
          /* the PLMN database since it is not complete                   */
          /****************************************************************/
          rr_plmn_db_clear(available_plmn_db_ptr, gas_id);

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
          if (rr_w2g_service_search_in_progress(gas_id))
          {
            rr_cell_selection_data_ptr->rr_cell_selection_details.suitable_cell_found = TRUE;

            /* cell selection should be aborted and control returned to RRC */
            return RR_EV_ABORT_PROCEDURE;
          }
          else
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */
          {
            /* Cell is suitable, attempt to camp on it */
            return rr_selection_perform_specific_channel_search(
                     candidate_db_ptr,
                     candidate_cell_entry_ptr,
                     TRUE,
                     gas_id
                   );
          }
        }

        /****************************************************************/
        /* Candidate cell was not suitable we will loop around again    */
        /* to see if there are any more strong suitable candidate cells */
        /****************************************************************/
      }
      /* otherwise, the cell was not a BCCH or had decode failures, try next one */
      else
      {
        MSG_GERAN_HIGH_3_G("Cell=(%d,%d) not BCCH=%d, try next",
                 arfcn.num,
                 arfcn.band,
                 candidate_cell_entry_ptr->type);
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("candidate_cell_entry_ptr was NULL");
    }
  } /* while */

  return EV_NO_EVENT;
} /* rr_selection_process_low_priority_cells */

static rr_event_T rr_selection_check_for_forbidden_cells(
  rr_candidate_cell_db_T *candidate_db_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T output_event = EV_NO_BCCH_FOUND;
  rr_candidate_cell_T *candidate_cell_entry_ptr = NULL;

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  MSG_GERAN_HIGH_0_G("Checking for forbidden cells");

  /* All of the candidate cell database entries should now be marked as
  processed, but we now need to reprocess them again.  To achieve this,
  reset their processed flags to unprocessed. */
  rr_candidate_db_mark_valid_entries_as_unprocessed(candidate_db_ptr);

  /* Iterate over the cells in the candidate cell database */
  while (rr_candidate_db_find_next(candidate_db_ptr,
                                   &candidate_cell_entry_ptr,
                                   RR_CANDIDATE_STRONGEST_FIRST))
  {
    ARFCN_T arfcn;

    if (candidate_cell_entry_ptr == NULL)
    {
      /* This should not happen and would indicate some problem with the
      rr_candidate_db_find_next() function. */
      MSG_GERAN_ERROR_0_G("candidate_cell_entry_ptr == NULL!");
      continue;
    }

    arfcn = candidate_cell_entry_ptr->meas.arfcn;

    if (candidate_cell_entry_ptr->type == RR_CANDIDATE_UNKNOWN)
    {
      /* should not be unknown at this stage */
      MSG_GERAN_ERROR_2_G("Type for (%d,%d) was unknown", arfcn.num, arfcn.band);
      candidate_cell_entry_ptr->type = RR_CANDIDATE_NOT_BCCH;
    }

    candidate_cell_entry_ptr->processed = TRUE;

    if (candidate_cell_entry_ptr->type == RR_CANDIDATE_VALID_BCCH)
    {
      boolean cell_suitable = rr_check_candidate_suitable(
        candidate_cell_entry_ptr,
        rr_is_any_cell_selection_mode(gas_id),
        rr_is_automatic_plmn_selection_mode(gas_id),
        gas_id
      );

      /* There is a candidate cell, need to check for suitability */
      if (cell_suitable)
      {
        MSG_GERAN_HIGH_2_G("Cell=(%d,%d) suitable, previously forbidden but no others suitable, camp",
          arfcn.num,
          arfcn.band);

        /* Parallel decode operation is not being interrupted, as this function
        should only be called when the parallel decode has completed and RR has
        finished searching for suitable non-forbidden cells, so no need to
        clear/invalidate the PLMN database here. */

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
        if (rr_w2g_service_search_in_progress(gas_id))
        {
          rr_cell_selection_data_ptr->rr_cell_selection_details.suitable_cell_found = TRUE;

          /* cell selection should be aborted and control returned to RRC */
          output_event = RR_EV_ABORT_PROCEDURE;
          break;
        }
        else
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */
        {
          /* Cell is suitable, attempt to camp on it */
          output_event = rr_selection_perform_specific_channel_search(
                           candidate_db_ptr,
                           candidate_cell_entry_ptr,
                           TRUE,
                           gas_id
                         );
          break;
        }
      }
    }
    else
    {
      MSG_GERAN_HIGH_3_G("Cell=(%d,%d) not BCCH=%d, try next",
        arfcn.num,
        arfcn.band,
        candidate_cell_entry_ptr->type
      );
    }
  } /* while */

  return output_event;
}

/**
 * L1 has completed decoding the BCCH list. Checks to see if we should ask it to give it another bcch list to decode.
 *
 * @return EV_NO_BCCH_FOUND - RR was unable to find a suitable/acceptable BCCH and has given up
 *         Events returned from rr_selection_process_power_scan_results
 */
static rr_event_T rr_selection_bcch_decode_completed(
  rr_candidate_cell_db_T *candidate_db_ptr,
  rr_available_plmn_db_T *available_plmn_db_ptr,
  rr_search_order_e       search_order,
  const gas_id_t gas_id
)
{
  rr_event_T rr_event = EV_NO_EVENT;
  boolean    populated_from_acq_db = FALSE;

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  /* the message has been received */
  rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received = TRUE;
  rr_set_decode_cnf_received(TRUE, gas_id);

  if (rr_candidate_db_decode_bcch_complete(gas_id))
  {
    if (!rr_candidate_db_search_for_BCCH(candidate_db_ptr))
    {
      /* No BCCHs were found in the last parallel decode operation, try the next set of cells */
      MSG_GERAN_HIGH_0_G("No BCCHs found in last decode, search next set");
      populated_from_acq_db = candidate_db_ptr->populated_from_acq_db;

      /* Check the power scan to see if we have any cells left */
      rr_event = rr_selection_process_power_scan_results(candidate_db_ptr, search_order, gas_id);

      if(rr_event == RR_EV_DECODING_BCCH)
      {
        MSG_GERAN_HIGH_1_G(
          "rr_selection_bcch_decode_completed populated_from_acq_db = %d",
          populated_from_acq_db
        );
        candidate_db_ptr->populated_from_acq_db = populated_from_acq_db;
      }

      if (rr_event == EV_NO_BCCH_FOUND)
      {
        if (rr_cell_selection_data_ptr->rr_cell_selection_details.retry_in_progress)
        {
          rr_cell_selection_data_ptr->rr_cell_selection_details.retry_in_progress = FALSE;
          MSG_GERAN_HIGH_0_G("RGS multipass retry failed");
          rr_invalidate_power_scan_results(gas_id);
          rr_event = EV_MULTIPASS_SEARCH_FAILED;
        }
        else
        if (rr_cell_selection_data_ptr->rr_cell_selection_details.retry_required)
        {
          /* retry another round of cell selection */
          rr_cell_selection_data_ptr->rr_cell_selection_details.retry_in_progress = TRUE;
          MSG_GERAN_HIGH_0_G("RGS multipass retry");
          rr_invalidate_power_scan_results(gas_id);
          rr_event = EV_PERFORM_MULTIPASS_SEARCH;
        }
      }

    }
    else
    {
      /************************************************************************/
      /* At least 1 BCCH was found but it turned out to be unsuitable or      */
      /* could not be decoded. According to specs, RR can give up now without */
      /* having to do more parallel decoding (but still have to search for    */
      /* any low priority cells that may have been skipped)                   */
      /************************************************************************/
      rr_event = rr_selection_process_low_priority_cells(candidate_db_ptr, available_plmn_db_ptr, gas_id);

      if (rr_event == EV_NO_BCCH_FOUND)
      {
        rr_event = rr_selection_check_for_forbidden_cells(candidate_db_ptr, gas_id);
      }
    }
  } /* number of unknown entries != 1 */

  return rr_event;
} /* rr_selection_bcch_decode_completed */

/**
 * Checks for early camping called when an early SI received event is received while waiting for the full SI.
 * If the cell is a GPRS cell or undergoing a PLMN selection no early camping is performed.
 */
static void rr_selection_process_early_camp_data(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  /********************************************************/
  /* Attempt to perform early camping if not already done */
  /********************************************************/
  if (rr_cell_selection_data_ptr->rr_cell_selection_details.early_camping_done == FALSE)
  {
    gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

    if (pcell_info_ptr->gsm.control_channel_description_valid == TRUE)
    {
      MSG_GERAN_HIGH_1_G("Early camp on %d",pcell_info_ptr->gsm.BCCH_ARFCN.num);
      rr_cell_selection_data_ptr->rr_cell_selection_details.early_camping_done = TRUE;

      rr_l1_idle_send_imsg_start_early_camp_req(
        pcell_info_ptr->gsm.BCCH_ARFCN,
        pcell_info_ptr->gsm.BSIC,
        pcell_info_ptr->gsm.location_area_identification,
        pcell_info_ptr->gsm.cell_identity,
        pcell_info_ptr->gsm.control_channel_description,
        pcell_info_ptr->gsm.control_channel_description_valid,
        gas_id
      );
    }
  }
} /* rr_selection_process_early_camp_data */

/**
 * Called during a parallel decode operation whenever a parallel decode event is found, if an SI was received
 * the candidate cell database would already be modified.
 *
 * @return
 *     EV_NO_EVENT - No action should be taken, RR needs to wait for an event associated with the strongest channel
 *     RR_EV_SPECIFIC_CHANNEL_SEARCH - RR has decided to attempt to camp on a specific channel and has told L1
 *     Events returned from 'rr_selection_bcch_decode_completed':
 */
static rr_event_T rr_selection_process_bcch_decode_event(
  rr_candidate_cell_db_T *candidate_db_ptr,
  rr_available_plmn_db_T *available_plmn_db_ptr,
  rr_search_order_e       search_order,
  const gas_id_t gas_id
)
{
  rr_candidate_process_strategy_T  strategy;
  rr_candidate_cell_T             *candidate_cell_entry_ptr = NULL;
  boolean                          cell_found = TRUE;
  boolean                          done = FALSE;
#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
  boolean                          snr_based_selection = FALSE;
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */
  word                             channel_number;
  sys_band_T                       band;
  rr_PLMN_request_details_T       *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
  snr_based_selection = (rr_is_csfb_call_in_progress(gas_id) && 
                         rr_PLMN_request_details_ptr->search_reason == RR_W2G_SERVICE_REDIRECTION);
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */

  if (rr_PLMN_request_details_ptr->search_reason != RR_PERFORM_REEST)
  {
    strategy = RR_CANDIDATE_HIGH_PRIORITY_CELLS;
  }
  else
  {
    strategy = RR_CANDIDATE_STRONGEST_FIRST;
  }

  MSG_GERAN_HIGH_1_G("Processing decode BCCH event, candidate cell strategy %d",strategy);

  while (!done)
  {
#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
    if (snr_based_selection)
    {
        cell_found = rr_candidate_db_fcch_snr_find_next(
          candidate_db_ptr, &candidate_cell_entry_ptr, strategy);
    }
    else
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */
    {
        cell_found = rr_candidate_db_find_next(
          candidate_db_ptr, &candidate_cell_entry_ptr, strategy);
    }

    if (!cell_found)
    {
      /* did not find any normal unprocessed cells in the database */
      if (rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received)
      {
        /* Can occur since SIs go through L2 so the cnf can get here first */
        /******************************************************************************/
        /* BCCH list cnf was received and candidate db could not find any unprocessed */
        /* normal priority cells. No more normal cells left to select to              */
        /******************************************************************************/
        MSG_GERAN_HIGH_0_G("No more normal priority cells left for selection");

        /* call the completed function to take appropriate actions */
        return
          rr_selection_bcch_decode_completed(
            candidate_db_ptr,
            available_plmn_db_ptr,
            search_order,
            gas_id
          );
      }
      else /* bcch_list_cnf_received is false */
      {
        MSG_GERAN_HIGH_0_G("No strongest normal cell found yet");
        /* Still decoding BCCH, when we get another decode event, we can look again*/
        return EV_NO_EVENT;
      }
    }
    else
    {
      if (rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received)
      {
        /* If we have received MPH_DECODE_BCCH_LIST_CNF but have reached this
        block it indicates that we are still waiting for one or more BCCH decodes
        to be completed.  There may be one on it's way via L2, or one or more
        BCCHs may have been decoded but discarded at the top of RR due to
        having the incorrect protocol discriminator.  We start a short timer
        to allow any pending receives to be processed.  When it expires, we
        will mark any unknown BCCHs as 'not BCCH'. */
        (void) rr_candidate_db_decode_bcch_complete(gas_id);
      }

      if (candidate_cell_entry_ptr != NULL) /* normal priority cell was found */
      {
        boolean cell_suitable = FALSE;
#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
        boolean process_only_snr = FALSE;
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */
        
        channel_number = candidate_cell_entry_ptr->meas.arfcn.num;
        band = candidate_cell_entry_ptr->meas.arfcn.band;

        /* if the cell is still unknown */
        if (candidate_cell_entry_ptr->type == RR_CANDIDATE_UNKNOWN)
        {
#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
          /* If it is SNR based selection then we have received SNR and we need to process it. */
          if(snr_based_selection)
          {
            process_only_snr = TRUE;
          }
          else
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */
          {
            MSG_GERAN_HIGH_2_G("Type str. NP cell=(%d,%d) unknown, wait",(int) channel_number, (int) band);
            /* status of this cell not yet known, hold off on processing */
            return EV_NO_EVENT;
          }
        }

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
        if(process_only_snr)
        {
          /* mark the entry as fcch_snr_processed */
          candidate_cell_entry_ptr->fcch_snr_processed = TRUE;
        }
        else
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */
        {
          /* mark the entry as processed */
          candidate_cell_entry_ptr->processed = TRUE;
        }

        /* only process it further if it is a valid BCCH */
#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
        if (candidate_cell_entry_ptr->type == RR_CANDIDATE_VALID_BCCH || process_only_snr)
        {
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */
          if (candidate_cell_entry_ptr->type == RR_CANDIDATE_VALID_BCCH)
          {
            cell_suitable = rr_check_candidate_suitable(
              candidate_cell_entry_ptr,
              rr_is_any_cell_selection_mode(gas_id),
              rr_is_automatic_plmn_selection_mode(gas_id),
              gas_id
            );

            if (cell_suitable)
            {
              /* Check if the cell is forbidden */
              if (rr_cell_forbidden(candidate_cell_entry_ptr->meas.arfcn, gas_id))
              {
                cell_suitable = FALSE;
                MSG_GERAN_HIGH_2_G("Cell=(%d,%d) forbidden, continue searching for suitable cell",
                  channel_number,
                  band);
              }
            }

            /* If performing cell re-establishment, check for re-establishment flag */
            if(rr_PLMN_request_details_ptr->search_reason == RR_PERFORM_REEST)
            {
              if(candidate_cell_entry_ptr->si_info.REESTABLISHMENT_ALLOWED == FALSE)
              {
                cell_suitable = FALSE;
                MSG_GERAN_HIGH_2_G("Cell=(%d,%d) Re-establishment not allowed",
                  (int)channel_number,
                  (int)band);
              }
            }
  
#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
          }
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
          if(snr_based_selection)
          {
            /* Cell is not suitable, mark it. */
            if(!process_only_snr && !cell_suitable)
            {
              MSG_GERAN_HIGH_1_G("ARFCN=%d Not Suitable.",channel_number);
              candidate_cell_entry_ptr->is_suitable = RR_CELL_UNSUITABLE;
            }
            /* Cell is suitable. Perform FCCH SNR based check if it is a redirection for CSFB. */
            else
            {
              boolean best_cell_found = FALSE;
              uint16  best_cell_index;
  
              /* This candidate cell is suitable for cell selection without SNR criteria. Mark it as suitable. 
               * This flag will help us in getting strongest suitable cell. */
              if(!process_only_snr)
              {
                candidate_cell_entry_ptr->is_suitable = RR_CELL_SUITABLE;
              }
              
              rr_candidate_db_find_best_of_fcch_snr(candidate_db_ptr, &best_cell_found, &best_cell_index, gas_id);
  
              if(best_cell_found)
              {
                candidate_cell_entry_ptr = &candidate_db_ptr->candidate_cells[best_cell_index];
                channel_number = candidate_cell_entry_ptr->meas.arfcn.num;
                band = candidate_cell_entry_ptr->meas.arfcn.band;
                MSG_GERAN_HIGH_1_G("Best SNR based cell=%d",channel_number);
              }
  
              cell_suitable = best_cell_found;
            }
          }
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */

          /* There is a candidate cell, need to check for suitability */
          if (cell_suitable)
          {
            MSG_GERAN_HIGH_2_G("Cell=(%d,%d) is suitable", (int) channel_number, (int) band);

            /* before clearing available PLMN DB, log it */
            (void)rr_log_available_plmn_list(available_plmn_db_ptr, gas_id);

            /****************************************************************/
            /* A parallel decode operation is being interrupted. Invalidate */
            /* the PLMN database since it is not complete                   */
            /****************************************************************/
            rr_plmn_db_clear(available_plmn_db_ptr, gas_id);

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
            if (rr_w2g_service_search_in_progress(gas_id))
            {
              rr_cell_selection_data_ptr->rr_cell_selection_details.suitable_cell_found = TRUE;

              /* cell selection should be aborted and control returned to RRC */
              return RR_EV_ABORT_PROCEDURE;
            }
            else
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */
            {
              /* Cell is suitable, reselect it*/
              return
                rr_selection_perform_specific_channel_search(
                  candidate_db_ptr,
                  candidate_cell_entry_ptr,
                  TRUE,
                  gas_id
                );
            }
          }

          /****************************************************************/
          /* Candidate cell was not suitable we will loop around again    */
          /* to see if there are any more strong suitable candidate cells */
          /****************************************************************/
        }
        /* otherwise, candidate cell was not BCCH or had decode failures, try next one */
        else
        {
          MSG_GERAN_HIGH_3_G("Cell=(%d,%d) not valid BCCH=%d",
                   (int)channel_number,
                   (int)band,
                   (int)candidate_cell_entry_ptr->type);
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("candidate_cell_entry_ptr was NULL");
        /* try the next one */
      }
    } /* while */
  }

  return EV_NO_EVENT;
} /* rr_selection_process_bcch_decode_event */

/**
 * Finishes camping on a cell by updating the system information database
 *
 * @return
 *     RR_EV_CAMPED_ON_CELL
 */
static void rr_selection_complete_cell_camping(
  const gas_id_t gas_id
)
{
  int C1;
  gprs_scell_info_t         *cell_info_ptr = rr_gprs_get_pcell_info(gas_id);
  rr_measured_signal_data_t *rr_measured_signal_data_ptr = rr_get_measured_signal_data_ptr(gas_id);


  rr_measured_signal_data_ptr->current_signal_strength = cell_info_ptr->gsm.reselection_info.RXLEV_average;

  C1 = rr_get_C1(
    cell_info_ptr->gsm.BCCH_ARFCN,
    cell_info_ptr->gsm.reselection_info.RXLEV_average,    /* RXLEV_Average */
    cell_info_ptr->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
    cell_info_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
    cell_info_ptr->gsm.cell_selection_parameters.POWER_OFFSET,
    gas_id
  );

  /*Fill in reselection info*/
  cell_info_ptr->gsm.reselection_info.C1 = C1;
  cell_info_ptr->gsm.reselection_info.C2 = C1;  /*Can't calculate C2 just use C1*/
  cell_info_ptr->gsm.reselection_info.measurements_valid = TRUE;
  cell_info_ptr->gsm.reselection_info.five_second_timer_id = RR_SERV_FIVE_SEC_C1_TIMER;

  /* log the camping success */
  rr_log_camping_attempt_success(gas_id);

  /***************************************************************/
  /* NOTE: do not clear the available PLMN database or candidate */
  /* database at this point. The information will continue to be */
  /* available for future cell selection operations              */
  /***************************************************************/

  /**
   * Send an ABORT_IND to MM if there is an LAI change during cell selection and if
   * there is a saved RR_EST_REQ and if the search_reason is not RR_PERFORM_REEST.
   * Note: As per 24.008, during Reestablishment, call reestablishment take precedence
   *       over Location Updating, for MM.
   */
  rr_abort_pending_rr_est_req_if_lai_changed(
    &(cell_info_ptr->gsm.location_area_identification),
    gas_id
  );

} /* rr_selection_complete_cell_camping */


/**
 * Called when all the GSM SI has been received successful acquisition, or when an acquisition failure is detected
 * (BCCH cannot be decoded). Checks the pending channels if acquisition was successful and calls
 * rr_selection_complete_cell_camping (GSM) or rr_gprs_selection_control (GPRS).
 * Otherwise rr_selection_recover_from_acquisition_failure is called.
 *
 * @param acquisition_success
 *               True if SI was received
 * @return
 *    Events from rr_selection_complete_cell_camping
 *    Events from rr_gprs_selection_control
 *    Events from rr_selection_recover_from_acquisition_failure
 */
static rr_event_T rr_selection_process_channel_data(
  boolean acquisition_success,
  const gas_id_t gas_id
)
{
  rr_event_T         event_out = RR_EV_ACQUISITION_FAILURE;
  gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  /* cancel the sys info timer if it's still running */
  (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);

  if (acquisition_success==TRUE)
  {
    /* Check if cell is suitable*/
    boolean suitable = rr_check_cell_suitable(
      &pcell_info_ptr->gsm.cell_selection_parameters,
      &pcell_info_ptr->gsm.RACH_control_parameters,
      &pcell_info_ptr->gsm.location_area_identification,
      pcell_info_ptr->gsm.reselection_info.RXLEV_average,
      pcell_info_ptr->gsm.BCCH_ARFCN,
      &pcell_info_ptr->gsm.BSIC,
      rr_is_any_cell_selection_mode(gas_id),
      rr_is_automatic_plmn_selection_mode(gas_id),
      TRUE,
      gas_id
    );

    if (suitable)
    {
      event_out = EV_CHECK_GPRS;
    }
    else /* cell is not suitable */
    {
      MSG_GERAN_HIGH_1_G("Cell %d unsuitable, full SI",pcell_info_ptr->gsm.BCCH_ARFCN.num);

      /* log the camping attempt failure */
      rr_log_camping_attempt_failure(RR_LOG_CAMPING_FAILURE_CELL_NOT_SUITABLE, gas_id);
    }
  }
  else
  {
    MSG_GERAN_HIGH_1_G("Cell %d full SI failure",pcell_info_ptr->gsm.BCCH_ARFCN.num);
  }

  /* Cell is not suitable/acceptable or unable to acquire, need to recover*/
  return event_out;

} /* rr_selection_process_channel_data */

/**
 * Called when there is an acquisition failure on a specific channel (BCCH not found, SI timeout, BCCH decode failure,
 * etc.). If the rr_cell_selection_data_ptr->rr_plmn_camping_list is valid, undergoing a PLMN selection, there are some untried entries
 * and fewer than RR_MAX_CANDIDATE_FREQ_LIST entries have been tried another channel will be searched.
 * Otherwise, the rr_cell_selection_data_ptr->rr_plmn_camping_list, available PLMN db, and power scan is invalidated and a power scan is started
 *
 * @return
 *    RR_EV_SPECIFIC_CHANNEL_SEARCH - A specific channel search has been started
 *    RR_EV_PERFORM_POWER_SCAN - A power scan has been started
 */
static rr_event_T rr_selection_recover_from_acquisition_failure(
  rr_candidate_cell_db_T *candidate_db_ptr,
  rr_available_plmn_db_T *available_plmn_db_ptr,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t          *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
  rr_PLMN_request_details_T  *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  ARFCN_T                     arfcn_failed_to_camp_on;
  rr_event_T                  out_event = EV_NO_EVENT;

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  arfcn_failed_to_camp_on = pcell_info_ptr->gsm.BCCH_ARFCN;

  rr_ce_discard_saved_pages(gas_id);

  /* If there is a pending RR_EST_REQ (which there may be if this cell selection(RR_EV_TRIGGER_CELL_SELECTION) was */
  /* performed to recover from cell reselection failure), then send a RR_ABORT_IND to MM */
  (void) rr_abort_pending_rr_est_req(OTHER_REASON, gas_id); 
	
  /* clear the pending channel data store */
  rr_clear_pending_channel_data(gas_id);

  rr_initialize_SI2q_instances(gas_id);
  rr_initialize_SI2ter_instances(gas_id);

  if ((rr_PLMN_request_details_ptr->search_reason == RR_PLMN_SELECTION) &&
      (rr_cell_selection_data_ptr->rr_plmn_camping_list.valid            == TRUE))
  {
    /*************************************************************************/
    /* Check if we can give up on the plmn camping list because all the cell */
    /* have been tried or we reached the max number                          */
    /*************************************************************************/
    int num_cells_monitored = rr_cell_selection_data_ptr->rr_plmn_camping_list.tried_high_priority_cells + rr_cell_selection_data_ptr->rr_plmn_camping_list.tried_low_priority_cells;
    int total_cells = rr_cell_selection_data_ptr->rr_plmn_camping_list.high_priority_array.num_frequencies + rr_cell_selection_data_ptr->rr_plmn_camping_list.low_priority_array.num_frequencies;

    /**************************************************************************************/
    /* Check if we can  give up the plmn camping list since we monitored the max number   */
    /* of freq or no more cells left, do a complete power scan instead                    */
    /**************************************************************************************/
    if (num_cells_monitored < total_cells)
    {
      /*Keep on going*/
      out_event = rr_selection_perform_plmn_camping_list_channel_search(candidate_db_ptr, gas_id);
    }
    else
    {
      MSG_GERAN_HIGH_0_G("No more cells in camping list, power scan");
    }
  }
  /* Following condition is added to send a new mph_decode_bcch_list req. This
     is required if rr_cell_selection_data_ptr->cell_select_state == RR_SELECTION_WAIT_FOR_CHANNEL,
     mph_decode_bcch_list_cnf is not yet received, but mph_select_specific_bcch_cnf
     is received and cell is barred. */
  else if (!rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received &&
           (rr_cell_selection_data_ptr->cell_select_state == RR_SELECTION_WAIT_FOR_CHANNEL))
  {

    /* To avoid RR starting a new power scan before receiving a
       MPH_DECODE_BCCH_LIST_CNF.

       Inform layer1 to decode bcch for arfcns for which decodes have not yet
       been received. There may not be any. In that case, need to start a new
       power scan.

       Following function call goes through candidate cell db and find those
       cells which do not have 'processed' = TRUE set. Send just these arfcns
       only this time to avoid extra work for layer1. */

    MSG_GERAN_HIGH_0_G("Continue to rx bcch decodes");

    out_event = rr_redo_bcch_list_decode_from_candidate_db(candidate_db_ptr,RR_NORMAL_SEARCH_ORDER,gas_id);
  }
  else
  {
    if (rr_sel_is_search_cause_service_req_acq_db_scan(rr_PLMN_request_details_ptr))
    {
      MSG_GERAN_HIGH_1_G("Attempt to acquire %d failed, doing RR_MM_ACQ_DB_SCAN, abort search", arfcn_failed_to_camp_on.num);
      out_event = EV_NO_BCCH_FOUND;
    }
    else
    {
      /************************************************************************/
      /* We could not acquire a cell but we just decoded the bcch, signal env */
      /* is changing rapidly redo the power scan, instead of looking at       */
      /* candidate db again                                                   */
      /************************************************************************/
      MSG_GERAN_HIGH_1_G("Attempt to acquire %d failed, recover", arfcn_failed_to_camp_on.num);
    }
  }

  if (out_event == EV_NO_EVENT)
  {
    /***********************************************************************/
    /* PLMN database was invalid or not suitable freq for that plmn        */
    /***********************************************************************/
  
    /* clear out the plmn camping list used by cell selection */
    rr_clear_plmn_list(gas_id);
  
    switch (rr_PLMN_request_details_ptr->search_reason)
    {
      case RR_PERFORM_REEST:
      {
        /* In reestablishment we cannot do a power scan */
        out_event = EV_NO_BCCH_FOUND;
        break;
      }
  
      case RR_W2G_SERVICE_REDIRECTION:
      case RR_W2G_SERVICE_REDIRECTION_EMERGENCY_CALL:
      {
        out_event = EV_NO_BCCH_FOUND;
        break;
      }
  
      default:
      {
        rr_plmn_db_clear(available_plmn_db_ptr, gas_id);
  
        out_event = rr_perform_power_scan(RR_SEARCH_NORMAL, rr_band_pref(gas_id), FALSE, gas_id);
        break;
      }
    }
  }

  /**
   * It is possible that we would have stopped cell selection timer due to acqusition failure.
   * Need to check and start it again if we are going to perform power scan / bcch decoding / specific channel search.
   */
  if (out_event == RR_EV_PERFORM_POWER_SCAN ||
      out_event == RR_EV_PERFORM_ACQ_DB_POWER_SCAN ||
      out_event == EV_USE_EXISTING_POWER_SCAN_RESULTS ||
      out_event == RR_EV_SPECIFIC_CHANNEL_SEARCH ||
      out_event == RR_EV_DECODING_BCCH ||
      out_event == RR_EV_DECODING_ACQ_DB_BCCH)
  {
    if (!rr_timer_is_running(RR_CELL_SELECTION_ABORT_TIMER, gas_id))
    {
      MSG_GERAN_MED_1_G("Restarted the RR_CELL_SELECTION_ABORT_TIMER for event=%#02X", out_event);
      rr_timer_start(RR_CELL_SELECTION_ABORT_TIMER, RR_CELL_SELECTION_ABORT_TIMEOUT, gas_id);
    }
  }

  return out_event;
} /* rr_selection_recover_from_acquisition_failure */

static boolean rr_sel_use_rr_select_bcch(const gas_id_t gas_id)
{
  rr_search_reason_T search_reason = rr_sel_get_search_reason(gas_id);

  if (rr_nv_iterative_si_acq_is_enabled(RR_GAS_ID_TO_AS_ID) &&
      ((search_reason == RR_AUTONOMOUS_CELL_SELECTION) || (search_reason == RR_PLMN_SELECTION)))
  {
#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
    if (!rr_x2g_redir_with_si_in_progress(gas_id))
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */
    {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * Called when MPH_SELECT_SPECIFIC_BCCH_CNF message is received.
 * If the message indicates that the BCCH was found, RR saves the BSIC into the pending channel data stores
 * and waits for full SI.
 * Also for reselection power scan and autonomous cell selection searchs with a valid control channel description
 * information, RR will perform early camping. For all other search reasons, no early camping is performed
 *
 * @param mph_select_specific_bcch_cnf
 *               message received from L1, has a field for whether the channel was found
 * @return
 *     RR_EV_CHANNEL_FOUND - A BCCH was found and L1 is decoding the BCCH for the channel, waiting for SIs
 *     Events returned from rr_selection_recover_from_acquisition_failure when BCCH is not found
 */
rr_event_T rr_selection_process_select_specific_bcch_cnf(
  mph_select_specific_bcch_cnf_T* mph_select_specific_bcch_cnf,
  const gas_id_t gas_id
)
{
  rr_csi_cgi_t cgi;
  rr_csi_cgi_t *cgi_ptr = NULL;
  rr_candidate_cell_db_T *candidate_db_ptr = rr_get_candidate_cell_db_ptr(gas_id);
  rr_available_plmn_db_T *available_plmn_db_ptr = rr_get_available_plmn_db_ptr(gas_id);
  gprs_scell_info_t      *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  /* store the BSIC in the pending channel data structure */
  if(mph_select_specific_bcch_cnf->BCCH_found == TRUE)
  {
    rr_compute_BSIC_from_SCH_block(
      &pcell_info_ptr->gsm.BSIC,
      mph_select_specific_bcch_cnf->SCH_block
    );
  }
  /* BCCH was found */
  if (pcell_info_ptr->gsm.cell_identity_valid_flag == TRUE)
  {
    cgi.ci = pcell_info_ptr->gsm.cell_identity;
    cgi.lai = pcell_info_ptr->gsm.location_area_identification;
    cgi_ptr = &cgi;
  }

  /* Proceed with early camp only if bcch is found and pending cell is not in
     barred cell list */
  if (( mph_select_specific_bcch_cnf->BCCH_found == TRUE ) &&
      (!rr_is_cell_in_barred_cell_list(
          pcell_info_ptr->gsm.BCCH_ARFCN,
          pcell_info_ptr->gsm.BSIC,
          cgi_ptr,
          NULL,
          gas_id
         )))
  {
    /* BCCH was found */

    if (rr_sel_use_rr_select_bcch(gas_id))
    {
      MSG_GERAN_HIGH_0_G("Use RR-SELECT-BCCH for SI acquisition");
    }
    else
    {
      /***************************************************************/
      /* Check if we can do early camping, early camping will not be */
      /* performed if SI3 information has not yet been received      */
      /***************************************************************/
#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
      /* If RR is performing an L2G redirection with SI tunnelling then we avoid
      performing early camping as we are likely to receive enough system
      information messages in the SI contained from LTE RRC to be able to go
      straight in to idle mode. */
      if (!rr_x2g_redir_with_si_in_progress(gas_id))
      {
        rr_selection_process_early_camp_data(gas_id);
      }
#else
      rr_selection_process_early_camp_data(gas_id);
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */

      /* Will be set to true when SIs received*/
      pcell_info_ptr->gsm.valid_data_flag = FALSE;
      pcell_info_ptr->gsm.generate_si_received_event = TRUE;
      pcell_info_ptr->gsm.update_message_received = 0;

      MSG_GERAN_HIGH_0_G("Starting GSMSysInfo timer");
      (void)rr_timer_start(RR_GSM_SYS_INFO_TIMER, RR_GSM_SYS_INFO_TIMEOUT, gas_id);

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
      if (rr_is_fast_si_acq_enabled(NULL, gas_id))
      {
        gprs_scell_info_t * pcell = rr_gprs_get_pcell_info(gas_id);

        /* Set up BA_INDs with value of 2.  This is a value that can not
        be received OTA, so allows us to detect after camping whether or
        not SI2 & friends have been received or not, which lets us trigger
        their acquisition if necessary. */
        pcell->gsm.complete_BA_list.no_of_entries = 0;
        pcell->gsm.complete_idle_BA_list_received = FALSE;
        pcell->gsm.complete_BA_list.BA_IND        = 0x02;
        pcell->gsm.SI2_BA_list.BA_IND             = 0x02;
        pcell->gsm.SI2bis_BA_list.BA_IND          = 0x02;
        pcell->gsm.SI2ter_BA_list.BA_IND          = 0x02;

        rr_x2g_redir_send_all_si(
          pcell_info_ptr->gsm.BCCH_ARFCN,
          pcell_info_ptr->gsm.BSIC,
          gas_id
        );
      }
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */
    }

    return RR_EV_CHANNEL_FOUND;
  }
  else
  {
    /* BCCH was not found */
    MSG_GERAN_HIGH_1_G("Freq %d not found or cell in barred list",
                                       pcell_info_ptr->gsm.BCCH_ARFCN.num);

    /* log the failed camping attempt */
    rr_log_camping_attempt_failure(RR_LOG_CAMPING_FAILURE_BCCH_NOT_FOUND, gas_id);

    return rr_selection_recover_from_acquisition_failure(candidate_db_ptr, available_plmn_db_ptr, gas_id);
  }
} /* rr_selection_process_select_specific_bcch_cnf */

/**
 * Called when a cell selection operation is completed successfully.
 *
 * @param provided_event
 *               Event to be returned to caller
 * @return
 *     provided_event
 */
static rr_event_T rr_selection_completed(
  rr_event_T provided_event,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t          *cell_info_ptr = rr_gprs_get_pcell_info(gas_id);
  rr_PLMN_request_details_T  *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  /* we have successfully found a cell to camp on, we have started idle now, tell MM */
  MSG_GERAN_HIGH_1_G("Camped on %d",cell_info_ptr->gsm.BCCH_ARFCN.num);

  (void)rr_timer_cancel(RR_CELL_SELECTION_ABORT_TIMER, gas_id);

  /* Reset search_reason back to default value. This will stop any spurious       */
  /* RR_PLMN_SELECT_CNF being sent to MM if the cell selection mechanism is used  */
  /* to acquire sys info after end of CS call after handover, for instance, and   */
  /* search_reason is set to RR_PLMN_SELECTION from a previous RR_PLMN_SELECT_REQ */
  rr_PLMN_request_details_ptr->search_reason = RR_AUTONOMOUS_CELL_SELECTION;

  /* log the end of the cell selection */
  rr_log_cell_selection_end(RR_LOG_CELL_SELECTION_END_NEW_CELL, gas_id);

  return provided_event;
} /* rr_selection_completed */


/**
 * No more channels left to search. Sends a RR_PLMN_SELECT_CNF if search reason was
 * RR_PLMN_SELECTION. Otherwise sends a RR_SERVICE_IND reporting no service.
 * Also sets a timer to invalidate the available plmn db.
 *
 * @return EV_NO_BCCH_FOUND
 */
static rr_event_T rr_selection_process_no_more_channels(
  rr_candidate_cell_db_T *candidate_db_ptr,
  rr_available_plmn_db_T *available_plmn_db_ptr,
  const gas_id_t gas_id
)
{
  LAI_T dummy_lai;
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  memset((void *) &dummy_lai,0,sizeof(dummy_lai));

  /* No more channels left, tell MM */
  MSG_GERAN_HIGH_0_G("No suitable freq found");

  /* Should we store any info in the available PLMN db */

  /* the available PLMN database should now be valid */
  rr_plmn_db_set_valid(available_plmn_db_ptr, rr_band_pref(gas_id), gas_id);
  rr_candidate_db_set_valid(candidate_db_ptr, TRUE);

  /***************************************************************************************/
  /* reset the offset in the candidate database back to zero in preparation for the next */
  /* cell selection or PLMN list construction operation                                  */
  /***************************************************************************************/
  {
    rr_candidate_db_clear_offset(candidate_db_ptr);
  }

  /* log the available PLMN list information */
  (void) rr_log_available_plmn_list(available_plmn_db_ptr, gas_id);


  /*No more channels left tell MM*/
  if (rr_PLMN_request_details_ptr->search_reason == RR_PLMN_SELECTION)
  {
    sys_detailed_plmn_list_s_type * available_PLMN_list = GPRS_MEM_CALLOC(sizeof(sys_detailed_plmn_list_s_type));

    /*************************************************************************/
    /* construct the available PLMN list. The automatic vs. manual mode will */
    /* be used to determine whether to check the forbidden LAI list or not   */
    /* while building the list (yes for automatic mode, no for manual mode)  */
    /*************************************************************************/
    rr_plc_construct_plmn_list(
      available_plmn_db_ptr,
      available_PLMN_list,
      rr_is_automatic_plmn_selection_mode(gas_id)?CHECK_FORBIDDEN_LAI_LIST:CHECK_REJECT_LAI_LIST,
      gas_id
    );

    rr_send_plmn_select_cnf(
      MM_AS_SUCCESS,
      MM_AS_NO_SERVICE,
      NULL,                 /* LAI not valid    */
      available_PLMN_list,
      NULL,                 /*BA list not valid */
      NULL,                 /*SI DB             */
      gas_id
    );

    if (available_PLMN_list != NULL)
    {
      GPRS_MEM_FREE(available_PLMN_list);
    }
  }
  else
#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
  if ((rr_PLMN_request_details_ptr->search_reason != RR_W2G_SERVICE_REDIRECTION) &&
      (rr_PLMN_request_details_ptr->search_reason != RR_W2G_SERVICE_REDIRECTION_EMERGENCY_CALL))
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */
  {
    /* If there is a pending RR_EST_REQ (which there may be if this cell selection was */
    /* performed to recover from a Radio Link Failure), then send a RR_ABORT_IND to MM */
    /* to abort the connection establishment */
    if (grr_verify_saved_mm_message(RR_EST_REQ, gas_id))
    {
      rr_send_abort_ind(OTHER_REASON, MM_RR_OTHER_FAILURE, gas_id);
      grr_throwaway_saved_message(gas_id);
    }

    rr_send_no_service_ind(gas_id);
  }

  /************************************************************************************/
  /* log the end of the cell selection (if the event is still outstanding and has not */
  /* been sent already)                                                               */
  /************************************************************************************/
  if (!rr_log_cell_selection_log_complete(gas_id))
  {
    rr_log_cell_selection_end(RR_LOG_CELL_SELECTION_END_CELL_NOT_FOUND, gas_id);
  }

  return EV_NO_BCCH_FOUND;
} /* rr_selection_process_no_more_channels */

/**
 * Starts an automonous cell selection, L1 will be requested to do a power scan.
 *
 * @return
 *    Events returned from rr_selection_start_cell_selection
 */
static rr_event_T rr_perform_autonomous_cell_selection(
  rr_candidate_cell_db_T *candidate_db_ptr,
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  /****************************************************************/
  /* Loss of service need to do an autonomous cell selection      */
  /* Autonomous cell selection is triggered by us so no           */
  /* plmn select cnf is sent, it uses the rr_PLMN_request_details */
  /* to determine if we need to select to a specific plmn         */
  /****************************************************************/
  MSG_GERAN_HIGH_0_G("Auto cell selection triggered");
  rr_PLMN_request_details_ptr->search_reason = RR_AUTONOMOUS_CELL_SELECTION;

  /* log the start of the cell selection */
  rr_log_cell_selection_start(
    RR_LOG_CELL_SELECTION_START_AUTONOMOUS_SELECTION,
    rr_is_any_cell_selection_mode(gas_id),
    rr_is_automatic_plmn_selection_mode(gas_id),
    gas_id
  );

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* start acquisition manager for priority inversion */
  rr_ms_service_recovery_started(gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */  

  rr_restart_cell_selection(candidate_db_ptr, gas_id);

  return rr_selection_start_cell_selection(TRUE, gas_id);
} /* rr_perform_autonomous_cell_selection */

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG

/**
 * Starts an automonous cell selection for W2G service redirection, L1
 * will be requested to do a power scan.
 *
 * @return
 *    Events returned from rr_selection_start_cell_selection
 */
static rr_event_T rr_perform_w2g_service_redirection_cell_selection(
  rr_candidate_cell_db_T *candidate_db_ptr,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  MSG_GERAN_HIGH_0_G("Cell selection triggered for W2G Service Redirection");

  rr_log_cell_selection_start(
    RR_LOG_CELL_SELECTION_START_W2G_SERVICE_REDIRECTION,
    rr_is_any_cell_selection_mode(gas_id),
    rr_is_automatic_plmn_selection_mode(gas_id),
    gas_id
  );

  rr_restart_cell_selection(candidate_db_ptr, gas_id);
  rr_cell_selection_data_ptr->rr_cell_selection_details.cell_selection_for_plmn_select_req = FALSE;

  /* ACQ-DB search is always allowed. */
  return rr_selection_start_cell_selection(TRUE, gas_id);
} /* rr_perform_autonomous_cell_selection */

#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

/**
 * Called to complete an abort of the cell selection function.
 * The reason for the abort is stored in the 'rr_cell_selection_data_ptr->rr_cell_selection_details' store.
 * Note that multiple abort reasons could be received at the same time.
 * The MMRequestReceived reason takes precedence over other reasons.
 *
 *
 * @return
 *    RR_EV_MM_REQ_RECEIVED_ABORT - abort was due to an MM abort
 *    EV_NO_BCCH_FOUND - abort was due to a cell selection abort timeout
 */
static rr_event_T rr_selection_complete_abort(
  const gas_id_t gas_id
)
{
  rr_candidate_cell_db_T *candidate_db_ptr = rr_get_candidate_cell_db_ptr(gas_id);
  rr_available_plmn_db_T *available_plmn_db_ptr = rr_get_available_plmn_db_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  MSG_GERAN_HIGH_0_G("Abort completed");

  /*Clear out available PLMN database, plmn list*/
  rr_clear_plmn_list(gas_id);

  /***********************************************************************/
  /* NOTE: Don't invalidate the power scan results. They should still be */
  /* valid                                                               */
  /***********************************************************************/

  if (rr_cell_selection_data_ptr->rr_cell_selection_details.mm_abort == TRUE)
  {
    /* only clear the available/candidate info if aborting due to an MM request */
    /* this will allow all the collected information to be sent to MM in        */
    /* response message                                                         */
    rr_plmn_db_clear(available_plmn_db_ptr, gas_id);
    rr_candidate_db_clear(candidate_db_ptr);

    (void)rr_timer_cancel(RR_CELL_SELECTION_ABORT_TIMER, gas_id);

    /* log the end of the cell selection */
    rr_log_cell_selection_end(RR_LOG_CELL_SELECTION_END_ABORT_MM_REQUEST, gas_id);

    return RR_EV_MM_REQ_RECEIVED_ABORT;
  }
#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
  else if (rr_w2g_service_search_in_progress(gas_id))
  {
    /* A cell selection for W2G service search is aborted immediately if a
    suitable cell is found.  Just need to return an indication to RRC. */
    rr_plmn_db_clear(available_plmn_db_ptr, gas_id);
    rr_candidate_db_clear(candidate_db_ptr);
    (void)rr_timer_cancel(RR_CELL_SELECTION_ABORT_TIMER, gas_id);

    /* log the end of the cell selection */
    rr_log_cell_selection_end(RR_LOG_CELL_SELECTION_END_ABORT_MM_REQUEST, gas_id);

    if (rr_cell_selection_data_ptr->rr_cell_selection_details.suitable_cell_found)
    {
      return RR_EV_CHANNEL_FOUND;
    }
    else
    {
      return EV_NO_BCCH_FOUND;
    }
  }
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */
  else if (rr_cell_selection_data_ptr->rr_cell_selection_details.abort_reason == RR_EV_ABORT_PROCEDURE)
  {
    /* log the end of the cell selection */
    rr_log_cell_selection_end(RR_LOG_CELL_SELECTION_END_ABORT_MM_REQUEST, gas_id);

    return EV_CELL_SELECTION_ABORTED;
  }
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */
  else
  {
    /* must be aborted because of a timeout */
    rr_log_cell_selection_end(RR_LOG_CELL_SELECTION_END_ABORT_TIMEOUT, gas_id);

    return EV_NO_BCCH_FOUND;
  }

} /* rr_selection_complete_abort */

/**
 * Records the abort reason in rr_cell_selection_data_ptr->rr_cell_selection_details
 *
 * @param abort_reason
 *               Type of abort (MM or timeout)
 */
static void rr_selection_aborted(
  rr_event_T abort_reason,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  if ( abort_reason == RR_EV_CELL_SELECTION_ABORT_TIMEOUT )
  {
    rr_cell_selection_data_ptr->rr_cell_selection_details.cell_selection_abort_timeout = TRUE;
  }
  else if ( abort_reason == RR_EV_MM_REQ_RECEIVED_ABORT )
  {
    rr_cell_selection_data_ptr->rr_cell_selection_details.mm_abort = TRUE;
  }
#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
  else if ( abort_reason == RR_EV_ABORT_PROCEDURE )
  {
    rr_cell_selection_data_ptr->rr_cell_selection_details.abort_reason = RR_EV_ABORT_PROCEDURE;
  }
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */
  else if ( abort_reason == EV_NO_BCCH_FOUND )
  {
    /* don't need to do anything */
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Unexpected abort %d",abort_reason);
  }
} /* rr_selection_aborted */

/**
 * Tells L1 to abort bcch decode and records abort reason
 *
 * @param abort_reason
 *               Type of abort that occured (MM requested or timeout)
 */
static void rr_selection_abort_bcch_decode(
  rr_event_T abort_reason,
  const gas_id_t gas_id
)
{
  rr_selection_aborted(abort_reason, gas_id);
  rr_send_mph_decode_bcch_list_abort_req(gas_id);
} /* rr_selection_abort_bcch_decode */


/*!
 * \brief Timer expiry handler for the cell selection state machine.
 * 
 * \param timer_expiry (in)
 * 
 * \return rr_event_T 
 */
static rr_event_T rr_cell_selection_handle_timer_expiry(timer_expiry_T *timer_expiry, const gas_id_t gas_id)
{
  rr_event_T sel_event = EV_NO_EVENT;

  switch (timer_expiry->timer_id)
  {
    case RR_GSM_SYS_INFO_TIMER:
    {
      sel_event = RR_EV_SYS_INFO_TIMEOUT;
      break;
    }

    case RR_CELL_SELECTION_ABORT_TIMER:
    {
      sel_event = RR_EV_CELL_SELECTION_ABORT_TIMEOUT;
      break;
    }

    case RR_GPRS_SYS_INFO_TIMER:
    {
      sel_event = RR_EV_GPRS_SYS_INFO_TIMEOUT;
      break;
    }

    case RR_BCCH_DECODE_COMPLETE_TIMER:
    {
      sel_event = RR_EV_DECODE_BCCH_COMPLETE_TIMEOUT;
      break;
    }

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected timer expiry: %d", timer_expiry->timer_id);
    }
  }

  return sel_event;
}

/**
  @brief Checks whether the cell that RR is currently camped on is suitable
         or not.

  @return TRUE if the currently camped on cell is suitable
  @return FALSE if the currently camped on cell is not suitable
*/
boolean rr_cell_selection_is_camped_cell_suitable(const gas_id_t gas_id)
{
  gprs_scell_info_t * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  boolean             cell_suitable = FALSE;

  cell_suitable = rr_check_cell_suitable(
    &scell_info_ptr->gsm.cell_selection_parameters,
    &scell_info_ptr->gsm.RACH_control_parameters,
    &scell_info_ptr->gsm.location_area_identification,
    63, /*Automatically assume a high rxlev*/
    scell_info_ptr->gsm.BCCH_ARFCN,
    &scell_info_ptr->gsm.BSIC,
    rr_is_any_cell_selection_mode(gas_id),
    rr_is_automatic_plmn_selection_mode(gas_id),
    TRUE,
    gas_id
  );

#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
  if (rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
  {
    if (!rr_cell_is_in_plmn_lac_range(scell_info_ptr->gsm.location_area_identification))
    {
      MSG_GERAN_HIGH_1_G("cell's LAC %d is not in the range of the selected PLMN",
      scell_info_ptr->gsm.location_area_identification.location_area_code);
      cell_suitable = FALSE;
    }
  }
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */

  if (cell_suitable)
  {
    access_result_T access_result;

    access_result = rr_check_access_class_for_camping(&scell_info_ptr->gsm, gas_id);

    if ( access_result == NO_CALLS_ALLOWED )
    {
      cell_suitable = FALSE;
    }
  }

  /* Now check to see if the band of the serving cell is part of the */
  /* preferred bands (band_pref) */
  if (cell_suitable)
  {
    sys_band_mask_type current_band_mask;

    current_band_mask = get_band_mask(rr_gapi_get_current_band(gas_id));

    /* If the current band is not one if the preferred bands, trigger */
    /* a cell selection */
    if ((current_band_mask & rr_band_pref(gas_id)) == 0)
    {
      cell_suitable = FALSE;
    }
  }

  return cell_suitable;
}

/**
 * Returns TRUE if the Cell Selection state machine is waiting for power
 * scan results.
 */
boolean rr_cell_selection_waiting_for_pscan_results(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  return (rr_cell_selection_data_ptr->cell_select_state == RR_SELECTION_WAIT_FOR_PSCAN_RESULTS);
}

/**
 * Main cell selection state machine
 *
 * @param cell_select_event
 *               Event to be processed by state machine
 * @param new_message
 *               Pointer to message if RR received a msg that triggered
 *               the event
 * @return Event to be processed by rr_control
 */
rr_event_T rr_selection_control(
  rr_event_T cell_select_event,
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  rr_candidate_cell_db_T *candidate_db_ptr;
  rr_available_plmn_db_T *available_plmn_db_ptr;
  rr_event_T              rr_event = EV_NO_EVENT;
  rr_event_T              new_cell_select_event = EV_NO_EVENT;
  byte                    message_id  = 0;
  byte                    message_set = 0;

  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  rr_pscan_results_db_T     *rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_cell_selection_data_t *rr_cell_selection_data_ptr = rr_cell_selection_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_cell_selection_data_ptr);

  /* establish pointers to the candidate cell and available PLMN databases */
  candidate_db_ptr = rr_get_candidate_cell_db_ptr(gas_id);
  available_plmn_db_ptr = rr_get_available_plmn_db_ptr(gas_id);

  while (cell_select_event != EV_NO_EVENT)
  {
    if (cell_select_event == EV_RESET_SOFTWARE)
    {
      rr_reset_cell_selection(candidate_db_ptr, gas_id);
      rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;

      /* don't process in state handlers */
      cell_select_event = EV_NO_EVENT;
    }
    else if (cell_select_event == EV_INPUT_MESSAGE)
    {
      /************************************************************/
      /* Handle some input messages in a state independent manner */
      /************************************************************/
      if (message_ptr != NULL)
      {
        /* set the message_set & message_id for all input messages */
        message_set = message_ptr->message_header.message_set;
        message_id =  message_ptr->message_header.message_id;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("message_ptr was NULL!");

        /* exit from the loop now */
        cell_select_event = EV_NO_EVENT;
      }
    }

    /* an EV_NO_EVENT should not be processed by state handlers */
    if (cell_select_event == EV_NO_EVENT)
    {
      continue;
    }

    switch (rr_cell_selection_data_ptr->cell_select_state)
    {
      case RR_SELECTION_IDLE:
      {
        /***********************************************/
        /* The cell selection state machine is idle    */
        /* (not performing a cell selection) and ready */
        /* to process any new cell selection requests  */
        /***********************************************/
        if ( (cell_select_event == EV_INPUT_MESSAGE) && (message_ptr != NULL) )
        {
          if ((message_set == MS_MM_RR) && (message_id == (int) RR_PLMN_SELECT_REQ))
          {
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
            rr_g2x_plmn_selection_triggered(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

            /*************************************************/
            /* MM request to perform a new cell selection to */
            /* find service on a given PLMN (or any PLMN)    */
            /*************************************************/
            new_cell_select_event = rr_process_plmn_select_req(
              &message_ptr->rr_plmn_select_req,
              NULL,
              gas_id
            );
          }
          else if (message_set == MS_TIMER)
          {
            new_cell_select_event = rr_cell_selection_handle_timer_expiry(
              &message_ptr->mid_timer_expiry,
              gas_id
            );
          }
          else
          {
            ERR("Unex.Msg=%d,%d in State=%d",(int) message_set,(int) message_id, (int) rr_cell_selection_data_ptr->cell_select_state);
          }
        }

        else if ( cell_select_event == RR_EV_PROCESS_PLMN_SELECT_REQ )
        {
          rr_plmn_selection_triggered(gas_id);

          new_cell_select_event = rr_perform_plmn_select_req_selection(
            candidate_db_ptr,
            available_plmn_db_ptr,
            TRUE,
            gas_id
          );
        }
        else if ( cell_select_event == EV_TRIGGER_CELL_SELECTION )
        {
          rr_cell_selection_triggered(gas_id);

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
          rr_g2x_cell_selection_triggered(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

          new_cell_select_event = rr_perform_autonomous_cell_selection(
            candidate_db_ptr,
            gas_id
          );
        }
#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
        else if ( cell_select_event == RR_EV_TRIGGER_CELL_SELECTION_W2G )
        {
          new_cell_select_event = rr_perform_w2g_service_redirection_cell_selection(
            candidate_db_ptr,
            gas_id
          );
        }
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

        else if (cell_select_event == RR_EV_PERFORM_ACQ_DB_POWER_SCAN)
        {
          rr_event = EV_CELL_SELECTION_TRIGGERED;
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_ACQ_DB_PSCAN_RESULTS;
        }

        else if ( cell_select_event == RR_EV_PERFORM_POWER_SCAN )
        {
          /*******************************************************************/
          /* A cell selection is required and a power scan must be performed */
          /* (power scan has already been started). Inform rr_control and    */
          /* wait for the results                                            */
          /*******************************************************************/
          rr_event = EV_CELL_SELECTION_TRIGGERED;
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_PSCAN_RESULTS;
        }

        else if (cell_select_event == EV_USE_EXISTING_POWER_SCAN_RESULTS)
        {
          /*******************************************************************/
          /* Existing power scan results are used so make sure that GRR prioritized the freq */
          /* based on the requested PLMN    */
          /*******************************************************************/
          rr_acq_db_prioritise_pscan_results(
             &rr_pscan_results_db_ptr->pscan_results,
             rr_PLMN_request_details_ptr->requested_PLMN_id,
             gas_id
          );
        
          new_cell_select_event = rr_selection_process_power_scan_results(
            candidate_db_ptr,
            RR_NORMAL_SEARCH_ORDER,
            gas_id
          );

          rr_event = EV_CELL_SELECTION_TRIGGERED;
          /*As we are assumming that the previous results ar still valid so consider it as ACQ_DB scan, so that */
          /*incase of NO_BCCH found , full band scan can be done */		  
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_ACQ_DB_PSCAN_RESULTS;
        }

        else if ( cell_select_event == RR_EV_SPECIFIC_CHANNEL_SEARCH )
        {
          /********************************************************************/
          /* A specific PLMN was requested, we were able to use the available */
          /* DB to do a specif channel search. Inform rr_control and wait for */
          /* the results                                                      */
          /********************************************************************/

          rr_event = EV_CELL_SELECTION_TRIGGERED;
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_CHANNEL;
        }
        else if ( cell_select_event == RR_EV_DECODING_BCCH )
        {
          /******************************************************************/
          /* Note: this event could be received after a race condition if   */
          /* the PSCAN results are valid but the available PLMN list is not */
          /******************************************************************/
          rr_event = EV_CELL_SELECTION_TRIGGERED;
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_DECODING_BCCH;
        }

        else if ( cell_select_event == RR_EV_DECODING_ACQ_DB_BCCH )
        {
          /******************************************************************/
          /* Note: this event could be received after a race condition if   */
          /* the PSCAN results are valid but the available PLMN list is not */
          /******************************************************************/
          rr_event = EV_CELL_SELECTION_TRIGGERED;
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_DECODING_ACQ_DB_BCCH;
        }

        else if ( cell_select_event == RR_EV_NO_CELL_SELECTION_REQUIRED )
        {
          // Already camped on a suitable cell so no cell selection required
          rr_event = EV_GO_TO_IDLE;
        }
        else if ( cell_select_event == RR_EV_MM_REQ_RECEIVED_ABORT )
        {
          /* If the cell selection was aborted by RR_PLMN_LIST_REQ, RR_PLMN_LIST_REQ can be */
          /* deleted by RR_BPLMN_SEARCH_ABORT_REQ or still saved, both need an indication to NAS */
          /* on RR's service status   */
          /* a) if the cell selection was triggered due to a RR_PLMN_SELECT_REQ, */
          /* then respond to MM with a RR_PLMN_SELECT_CNF (indicating no service) */
          /* before processing the RR_PLMN_LIST_REQ */
          /* b) If the cell selection was triggered internally by RR (e.g. due to a */
          /* radio link failure cauing re-establishment, or a reselection power scan */
          /* which found no suitale cells), then MM is unaware that RR has lost service, */
          /* so send a RR_SERVICE_IND (this will ensure MM gets RR back into service */
          /* after the search results are sent) */

          if ( !grr_examine_saved_mm_message(NULL, gas_id) ||
               grr_verify_saved_mm_message(RR_PLMN_LIST_REQ, gas_id)
             )
          {
            if ( rr_PLMN_request_details_ptr->search_reason == RR_PLMN_SELECTION )
            {
              rr_send_plmn_select_cnf_no_service(MM_AS_SUCCESS, gas_id);
            }
            else
            if ( ( rr_PLMN_request_details_ptr->search_reason == RR_AUTONOMOUS_CELL_SELECTION ) ||
                 ( rr_PLMN_request_details_ptr->search_reason == RR_PERFORM_REEST ) )
            {
              rr_send_no_service_ind(gas_id);
            }
          }

          rr_cleanup_cell_selection(gas_id);
          rr_event = EV_NO_BCCH_FOUND;
        }
        else if ( cell_select_event == EV_NO_BCCH_FOUND )
        {
          rr_event = rr_selection_process_no_more_channels(
            candidate_db_ptr,
            available_plmn_db_ptr,
            gas_id
          );
          rr_cleanup_cell_selection(gas_id);
        }
        else if ( cell_select_event == EV_GO_TO_IDLE )
        {
          rr_cleanup_cell_selection(gas_id);
          rr_event = EV_GO_TO_IDLE;
        }
        else if ( cell_select_event == EV_GO_TO_IDLE_PLMN_SELECTION )
        {
          rr_cleanup_cell_selection(gas_id);
          rr_event = EV_GO_TO_IDLE_PLMN_SELECTION;
        }
#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
        else if ( cell_select_event == RR_EV_ABORT_PROCEDURE )
        {
          rr_cleanup_cell_selection(gas_id);
          rr_event = EV_CELL_SELECTION_ABORTED;
        }
        else if ( cell_select_event == EV_CELL_SELECTION_ABORTED )
        {
          rr_cleanup_cell_selection(gas_id);
          rr_event = EV_CELL_SELECTION_ABORTED;
        }
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
        else if ( cell_select_event == RR_EV_CHANNEL_FOUND )
        {
          rr_cleanup_cell_selection(gas_id);
          rr_event = RR_EV_CHANNEL_FOUND;
        }
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

        else if (cell_select_event == RR_EV_REPROCESS_CANDIDATE_CELL_DB)
        {
          ; /* This signal can be ignored in this state as the BCCH decode has
            not yet started. */
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(cell_select_event),
                               rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
        }
      } /* RR_SELECTION_IDLE */
      break;

      case RR_SELECTION_WAIT_FOR_ACQ_DB_PSCAN_RESULTS:
      {
        if ((cell_select_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          if (message_set == MS_RR_L1)
          {
            switch ((rr_l1_message_id_enum_T) message_id)
            {
              case MPH_POWER_SCAN_CNF:
              {
                rr_process_mph_power_scan_cnf(&message_ptr->mph_power_scan_cnf);

                rr_acq_db_prioritise_pscan_results(
                  &rr_pscan_results_db_ptr->pscan_results,
                  rr_PLMN_request_details_ptr->requested_PLMN_id,
                  gas_id
                );

                if (rr_cell_selection_data_ptr->rr_cell_selection_details.abort_after_power_scan_cnf)
                {
                  MSG_GERAN_HIGH_0_G("Aborting cell selection due to RR_DEACT_REQ");
                  rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
                  rr_cleanup_cell_selection(gas_id);
                  rr_event = EV_CELL_SELECTION_ABORTED;
                }
                else
                {
                  /* process the power scan results */
                  new_cell_select_event = rr_selection_process_power_scan_results(
                    candidate_db_ptr,
                    RR_NORMAL_SEARCH_ORDER,
                    gas_id
                  );
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr,
                                       rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
              }
            }
          }
          else if (message_set == MS_MM_RR)
          {
            switch ((mm_rr_message_id_enum_T) message_id)
            {
              case RR_DEACT_REQ:
              {
                if (message_ptr->rr_deact_req.deact_reason == RR_MM_MODE_CHANGE)
                {
                  MSG_GERAN_HIGH_0_G("Waiting for mph_power_scan_cnf before aborting cell selection");
                  rr_cell_selection_data_ptr->rr_cell_selection_details.abort_after_power_scan_cnf = TRUE;
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                  /* advise the multi-SIM controller of this abort */
                  rr_ms_cell_selection_deactivating(gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
                }
                else
                {
                  MSG_GERAN_HIGH_0_G("Aborting cell selection due to RR_DEACT_REQ");
                  rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
                  rr_cleanup_cell_selection(gas_id);
                  rr_event = EV_CELL_SELECTION_ABORTED;
                }
                break;
              }

              case RR_STOP_GSM_MODE_REQ:
              {
                MSG_GERAN_HIGH_0_G("Aborting cell selection due to RR_STOP_GSM_MODE_REQ");
                rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
                rr_cleanup_cell_selection(gas_id);
                rr_event = EV_CELL_SELECTION_ABORTED;
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr,
                                       rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
              }
            }
          }
          else if (message_set == MS_TIMER)
          {
            new_cell_select_event = rr_cell_selection_handle_timer_expiry(
              &message_ptr->mid_timer_expiry,
              gas_id
            );
          }
          else
          {
            LOG_UNEXPECTED_MESSAGE(message_ptr,
                                   rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
          }
        }

        else if ( cell_select_event == EV_NO_BCCH_FOUND )
        {

          if (rr_sel_is_search_cause_service_req_acq_db_scan(rr_PLMN_request_details_ptr))
          {
            MSG_GERAN_HIGH_0_G("Acq DB power scan found no channels, RR_MM_ACQ_DB_SCAN abort search ");
            (void)rr_selection_process_no_more_channels(candidate_db_ptr, available_plmn_db_ptr, gas_id);
            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
            rr_cleanup_cell_selection(gas_id);
            rr_event = EV_CELL_SELECTION_ABORTED;
          }
          else
          {
            /* Acquisition database power scan did not find any frequencies.
            Now need to go on to perform a complete power scan. */
            {
              sys_band_mask_type band_pref = rr_band_pref(gas_id);
              {
                new_cell_select_event = rr_perform_power_scan(
                  RR_SEARCH_NORMAL,
                  band_pref,
                  TRUE,
                  gas_id
                );
  
                rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_PSCAN_RESULTS;
              }
            }
          } /*RR_MM_ACQ_DB_SCAN*/
        }

        else if ( cell_select_event == RR_EV_DECODING_BCCH )
        {
          /*****************************************************************************************/
          /* Power scan results were used to request L1 to parallel decode the BCCH                */
          /* on a given set of frequencies (which are now stored in the rr_candidate_cell_database */
          /* and the rr_PLMN_request_details). SM transitions to RR_SELECTION_DECODING_BCCH state  */
          /* to await responses from L1.                                                           */
          /*****************************************************************************************/

          /* Indicate that that candidate cell database has been populated as a
          result of an ACQ-DB scan. */
          rr_candidate_db_set_populated_from_acq_db(
            candidate_db_ptr,
            TRUE
          );


          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_DECODING_ACQ_DB_BCCH;
        }
        else if ( cell_select_event == RR_EV_CELL_SELECTION_ABORT_TIMEOUT ||
                  cell_select_event == RR_EV_MM_REQ_RECEIVED_ABORT )
        {
          rr_selection_aborted(cell_select_event, gas_id);
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
          /* advise the multi-SIM controller of this abort */
          rr_ms_cell_selection_deactivating(gas_id);
          MSG_GERAN_HIGH_1_G("Abort MPH_POWER_SCAN_REQ due to event:%#02X", cell_select_event);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_ABORT;
        }
#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
        else if ( cell_select_event == RR_EV_ABORT_PROCEDURE )
        {
          /**
           * If service redirection is being aborted, either because of wait
           * timer expiry or because RRC wants to abort the procedure, RR must
           * return control to RRC immediately.  The cell selection state
           * machine is reset here, and the service redirection state machine
           * will deal with stopping L1.
           */
          rr_cleanup_cell_selection(gas_id);
          rr_log_cell_selection_end(
            RR_LOG_CELL_SELECTION_END_ABORT_TIMEOUT,
            gas_id
          );
          rr_event = EV_CELL_SELECTION_ABORTED;
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
        }
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

        else if (cell_select_event == RR_EV_REPROCESS_CANDIDATE_CELL_DB)
        {
          ; /* This signal can be ignored in this state as the BCCH decode has
            not yet started. */
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(cell_select_event),
                               rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
        }
      } /* RR_SELECTION_WAIT_FOR_ACQ_DB_PSCAN_RESULTS */
      break;

      case RR_SELECTION_WAIT_FOR_PSCAN_RESULTS:
      {
        /*****************************************************************/
        /* RR has initiated a power scan (sent MPH_POWER_SCAN_REQ to L1) */
        /* and is waiting for the results to be generated by L1.         */
        /*****************************************************************/

        if ((cell_select_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          if ((message_set == MS_RR_L1) && (message_id == (int) MPH_POWER_SCAN_CNF))
          {
            rr_process_mph_power_scan_cnf(&message_ptr->mph_power_scan_cnf);

            if (rr_cell_selection_data_ptr->rr_cell_selection_details.abort_after_power_scan_cnf)
            {
              MSG_GERAN_HIGH_0_G("Aborting cell selection due to RR_DEACT_REQ");
              rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
              rr_cleanup_cell_selection(gas_id);
              rr_event = EV_CELL_SELECTION_ABORTED;
            }
            else
            {
              /* process the power scan results */
              new_cell_select_event = rr_selection_process_power_scan_results(
                candidate_db_ptr,
                RR_NORMAL_SEARCH_ORDER,
                gas_id
              );
            }
          }
          else if (message_set == MS_MM_RR)
          {
            if ((mm_rr_message_id_enum_T) message_id == RR_DEACT_REQ)
            {
              rr_deact_req_T *rr_deact_req = (rr_deact_req_T *) message_ptr;

              if (rr_deact_req->deact_reason == RR_MM_MODE_CHANGE)
              {
                MSG_GERAN_HIGH_0_G("Waiting for mph_power_scan_cnf before aborting cell selection");
                rr_cell_selection_data_ptr->rr_cell_selection_details.abort_after_power_scan_cnf = TRUE;
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                  /* advise the multi-SIM controller of this abort */
                  rr_ms_cell_selection_deactivating(gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
              }
              else
              {
                MSG_GERAN_HIGH_0_G("Aborting cell selection due to RR_DEACT_REQ");
                rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
                rr_cleanup_cell_selection(gas_id);
                rr_event = EV_CELL_SELECTION_ABORTED;
              }
            }
            else
            if ((mm_rr_message_id_enum_T) message_id == RR_STOP_GSM_MODE_REQ)
            {
              MSG_GERAN_HIGH_0_G("Aborting cell selection due to RR_STOP_GSM_MODE_REQ");
              rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
              rr_cleanup_cell_selection(gas_id);
              rr_event = EV_CELL_SELECTION_ABORTED;
            }
            else
            {
              MSG_GERAN_ERROR_1_G("Unexpected message 0x%02x from MM", message_id);
            }
          }
          else if (message_set == MS_TIMER)
          {
            new_cell_select_event = rr_cell_selection_handle_timer_expiry(
              &message_ptr->mid_timer_expiry,
              gas_id
            );
          }
          else
          {
            MSG_GERAN_ERROR_3_G("Unex.Msg=%d,%d in State=%d",(int) message_set,(int) message_id, (int) rr_cell_selection_data_ptr->cell_select_state);
          }
        }
        else if ( cell_select_event == EV_NO_BCCH_FOUND )
        {
          /*************************************************************************/
          /* Power scan results indicate that there are no channels available.     */
          /*************************************************************************/

          /* call the process no more channels function to handle the condition */
          rr_event = rr_selection_process_no_more_channels(
            candidate_db_ptr,
            available_plmn_db_ptr,
            gas_id
          );
          rr_cleanup_cell_selection(gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
        }
        else if ( cell_select_event == RR_EV_DECODING_BCCH )
        {
          /*****************************************************************************************/
          /* Power scan results were used to request L1 to parallel decode the BCCH                */
          /* on a given set of frequencies (which are now stored in the rr_candidate_cell_database */
          /* and the rr_PLMN_request_details). SM transitions to RR_SELECTION_DECODING_BCCH state  */
          /* to await responses from L1.                                                           */
          /*****************************************************************************************/

          /* Indicate that that candidate cell database has been populated as a
          result of an full scan. */
          rr_candidate_db_set_populated_from_acq_db(
            candidate_db_ptr,
            FALSE
          );

          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_DECODING_BCCH;
        }
        else if ( cell_select_event == RR_EV_CELL_SELECTION_ABORT_TIMEOUT ||
                  cell_select_event == RR_EV_MM_REQ_RECEIVED_ABORT )
        {
          rr_selection_aborted(cell_select_event, gas_id);
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
          /* advise the multi-SIM controller of this abort */
          rr_ms_cell_selection_deactivating(gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
          MSG_GERAN_HIGH_1_G("requested L1 to respond to MPH_POWER_SCAN_REQ after receiving event:0x%02X ",(int)cell_select_event);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_ABORT;
        }
        #ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
        else if ( cell_select_event == RR_EV_ABORT_PROCEDURE )
        {
          /**
           * If service redirection is being aborted, either because of wait
           * timer expiry or because RRC wants to abort the procedure, RR must
           * return control to RRC immediately.  The cell selection state
           * machine is reset here, and the service redirection state machine
           * will deal with stopping L1.
           */
          rr_cleanup_cell_selection(gas_id);
          rr_log_cell_selection_end(
            RR_LOG_CELL_SELECTION_END_ABORT_TIMEOUT,
            gas_id
          );
          rr_event = EV_CELL_SELECTION_ABORTED;
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
        }
        #endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */


        else if (cell_select_event == RR_EV_PERFORM_POWER_SCAN)
        {
          MSG_GERAN_HIGH_0_G("Power scan restarted (ACQ-DB => FULL PSCAN)");
        }

        else if (cell_select_event == RR_EV_REPROCESS_CANDIDATE_CELL_DB)
        {
          ; /* This signal can be ignored in this state as the BCCH decode has
            not yet started. */
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(cell_select_event),
                               rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
        }
      } /* RR_SELECTION_WAIT_FOR_PSCAN_RESULTS */
      break;

      case RR_SELECTION_DECODING_ACQ_DB_BCCH:
      {
        /*********************************************************************************/
        /* RR has initiated a parallel decode operation in L1 and is waiting for         */
        /* indications from L1 about the state of the cells that have been found so far  */
        /* The RR_BCCH_DECODE_COMPLETE_TIMER was started to make sure we don't get stuck */
        /* , it must be stopped if we leave this state                                   */
        /********************************************************************************/

        if ((cell_select_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          if (message_set == MS_RR_L1)
          {
            if ((rr_l1_message_id_enum_T) message_id == MPH_DECODE_BCCH_LIST_FAILURE_IND)
            {
              word channel_number = message_ptr->mph_decode_bcch_list_failure_ind.frequency.num;

              sys_band_T band = message_ptr->mph_decode_bcch_list_failure_ind.frequency.band;
              /****************************************************************************************************/
              /* Indicates that a failure has occurred with a potential BCCH channel that L1 was trying to decode.*/
              /* (Sets the type of the indicated frequency in the candidate database appropriately based on the   */
              /* (failure reason from L1 and sets its status to processed (since it cannot be used for camping).  */
              /****************************************************************************************************/

              rr_candidate_db_set_type_from_failure_ind(
                candidate_db_ptr,
                message_ptr->mph_decode_bcch_list_failure_ind.frequency,
                message_ptr->mph_decode_bcch_list_failure_ind.failure_cause,
                gas_id
              );

              MSG_GERAN_HIGH_3_G("Cell=(%d,%d) BCCH decode failure=%d",
                       (int) channel_number,
                       (int) band,
                       (int) message_ptr->mph_decode_bcch_list_failure_ind.failure_cause
                      );

              new_cell_select_event = rr_selection_process_bcch_decode_event(
                candidate_db_ptr,
                available_plmn_db_ptr,
                RR_NORMAL_SEARCH_ORDER,
                gas_id
              );
            }
            else if ((rr_l1_message_id_enum_T) message_id == MPH_DECODE_BCCH_LIST_CNF)
            {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
              if (rr_background_plmn_search_in_progress(gas_id))
              {
                rr_cell_selection_data_ptr->rr_cell_selection_details.retry_required = FALSE;
              }
              else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
              {
                rr_cell_selection_data_ptr->rr_cell_selection_details.retry_required = message_ptr->mph_decode_bcch_list_cnf.retry_required;
              }

              rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received = TRUE;
              rr_set_decode_cnf_received(TRUE, gas_id);

              /* If MPH_DECODE_BCCH_LIST_CNF has been recieved, this function
              will first check for high priority cells, then low priority if
              no high priority cells are found. */
              new_cell_select_event = rr_selection_process_bcch_decode_event(
                candidate_db_ptr,
                available_plmn_db_ptr,
                RR_NORMAL_SEARCH_ORDER,
                gas_id
              );
            }

            else if ((rr_l1_message_id_enum_T) message_id == MPH_FCCH_SCH_DECODE_IND)
            {
              rr_candidate_update_FCCH_SCH(&(message_ptr->mph_fcch_sch_decode_ind), gas_id);

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
              new_cell_select_event = rr_selection_process_bcch_decode_event(
                candidate_db_ptr,
                available_plmn_db_ptr,
                RR_NORMAL_SEARCH_ORDER,
                gas_id
              );
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */
            }

            else
            {
              MSG_GERAN_ERROR_3_G("Unex.Msg=%d,%d in State=%d",
                (int) message_set,
                (int) message_id,
                (int) rr_cell_selection_data_ptr->cell_select_state);
            }
          }
          else if (message_set == MS_MM_RR)
          {
            switch ((mm_rr_message_id_enum_T) message_id)
            {
              case RR_DEACT_REQ:
              case RR_STOP_GSM_MODE_REQ:
              {
                (void)rr_timer_cancel(RR_BCCH_DECODE_COMPLETE_TIMER, gas_id);

                if (rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received)
                {
                  rr_selection_aborted(RR_EV_MM_REQ_RECEIVED_ABORT, gas_id);
                  new_cell_select_event = rr_selection_complete_abort(gas_id);
                  rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
                }
                else
                {
                  rr_selection_abort_bcch_decode(RR_EV_MM_REQ_RECEIVED_ABORT, gas_id);
                  rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_ABORT;
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr,
                                       rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
              }
            }
          }
          else if (message_set == MS_TIMER)
          {
            new_cell_select_event = rr_cell_selection_handle_timer_expiry(
              &message_ptr->mid_timer_expiry,
              gas_id
            );
          }
          else
          {
            MSG_GERAN_ERROR_3_G("Unex.Msg=%d,%d in State=%d",
              (int) message_set,
              (int) message_id,
              (int) rr_cell_selection_data_ptr->cell_select_state);
          }
        }
        else if ( cell_select_event == RR_EV_CANDIDATE_CELL_SI_RECEIVED )
        {
          /*****************************************************************************/
          /* Indicates that system information for a candidate cell has been received. */
          /* Calls the 'rr_selection_process_bcch_decode_event' function               */
          /*****************************************************************************/

          /* If this is the last cell that RR is expecting a BCCH decode for, RR
          should wait until receiving the MPH_DECODE_BCCH_LIST_CNF unless it has
          already been received.  This is to allow RR to correctly determine that
          the BCCH list decode is completed in the event of selection of this last
          cell failing.  This should result in a full power scan being triggered,
          rather than RR sending RR_PLMN_SELECT_CNF(NO_SERVICE) to NAS. */
          if (rr_candidate_db_get_num_unknown_cell(candidate_db_ptr) == 0 &&
              rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received == FALSE)
          {
            MSG_GERAN_HIGH_0_G("Wait for MPH_DECODE_BCCH_LIST_CNF before processing last SI");
          }
          else
          {
            new_cell_select_event = rr_selection_process_bcch_decode_event(
              candidate_db_ptr,
              available_plmn_db_ptr,
              RR_NORMAL_SEARCH_ORDER,
              gas_id
            );
          }
        }

        /**
         * No suitable cells were found during ACQ-DB search, so attempt a full search.
         */
        else if ( cell_select_event == EV_NO_BCCH_FOUND )
        {

          /* Acq DB has not found any suitable channels abort the search*/
          if (rr_sel_is_search_cause_service_req_acq_db_scan(rr_PLMN_request_details_ptr))
          {
            MSG_GERAN_HIGH_0_G("Acq DB found no channels, RR_MM_ACQ_DB_SCAN abort search ");
            (void)rr_selection_process_no_more_channels(candidate_db_ptr, available_plmn_db_ptr, gas_id);
            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
            rr_cleanup_cell_selection(gas_id);
            rr_event = EV_CELL_SELECTION_ABORTED;
          }
          else
          {
            /* Acquisition database power scan did not find any frequencies.
            Now need to go on to perform a complete power scan. */
            {
              sys_band_mask_type band_pref = rr_band_pref(gas_id);
              {
                new_cell_select_event = rr_perform_power_scan(
                  RR_SEARCH_NORMAL,
                  band_pref,
                  TRUE,
                  gas_id
                );
  
                rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_PSCAN_RESULTS;
  
              }
            }
          } /*RR_MM_ACQ_DB_SCAN*/
        }
        else if ( cell_select_event == RR_EV_DECODING_BCCH )
        {
          /************************************************************************/
          /* RR has decided to decode even more BCCHs in parallel and has started */
          /* another parallel BCCH decode operation. Since RR is already in the   */
          /* RR_SELECTION_DECODING_BCCH state, it remains in this state.          */
          /************************************************************************/
        }
        else if ( cell_select_event == RR_EV_SPECIFIC_CHANNEL_SEARCH )
        {
          /********************************************************************************************/
          /* RR has decided to try to camp on a cell (information is in pending channel)              */
          /* It has sent a MPH_SELECT_SPECIFIC_BCCH_REQ to L1 (which implicitly aborts the parallel   */
          /* decode operation). RR transitions to RR_SELECTION_WAIT_FOR_CHANNEL to await confirmation */
          /********************************************************************************************/

          (void)rr_timer_cancel(RR_BCCH_DECODE_COMPLETE_TIMER, gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_CHANNEL;
        }
        else if ( cell_select_event == RR_EV_DECODE_BCCH_COMPLETE_TIMEOUT )
        {
          /****************************************************************************/
          /* Indicates that the RR has timed out waiting for information for the last */
          /* remaining entry in the candidate cell database. All the unknown entries  */
          /* in the database are marked as NOT BCCH so they are not considered        */
          /****************************************************************************/

          /* mark all the unknown entries as NOT BCCH so they are not considered */
          rr_candidate_db_set_unknown_entry_types(candidate_db_ptr, RR_CANDIDATE_NOT_BCCH);

          new_cell_select_event = rr_selection_bcch_decode_completed(
            candidate_db_ptr,
            available_plmn_db_ptr,
            RR_NORMAL_SEARCH_ORDER,
            gas_id
          );
        }
        else if ( cell_select_event == RR_EV_CELL_SELECTION_ABORT_TIMEOUT ||
                  cell_select_event == RR_EV_MM_REQ_RECEIVED_ABORT )
        {
          (void)rr_timer_cancel(RR_BCCH_DECODE_COMPLETE_TIMER, gas_id);

          rr_selection_abort_bcch_decode(cell_select_event, gas_id);

          if ( rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received )
          {
            new_cell_select_event = rr_selection_complete_abort(gas_id);
            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
          }
          else
          {
            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_ABORT;
          }
        }

        else if( cell_select_event == EV_CELL_SELECTION_TRIGGERED)
        {
          /* This occurs in case of a service domain change */
          rr_event = EV_CELL_SELECTION_TRIGGERED;
        }

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
        else if ( cell_select_event == RR_EV_ABORT_PROCEDURE )
        {
          (void)rr_timer_cancel(RR_BCCH_DECODE_COMPLETE_TIMER, gas_id);

          if ( rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received )
          {
            rr_selection_aborted(cell_select_event, gas_id);
            new_cell_select_event = rr_selection_complete_abort(gas_id);
            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
          }
          else
          {
            /* only need to abort the decode if a bcch_list_cnf has
            not yet been received. */
            rr_selection_abort_bcch_decode(cell_select_event, gas_id);
            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_ABORT;
          }
        }
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */


        else if (cell_select_event == RR_EV_REPROCESS_CANDIDATE_CELL_DB)
        {
          /**
           * The conditions that determine cell suitability have changed so
           * all candidate cell database entries processed so far have to be
           * reprocessed.  Reset the 'processed' flag for all valid BCCHs
           * and check for a suitable cell.
           */
          rr_candidate_db_mark_valid_entries_as_unprocessed(candidate_db_ptr);
          new_cell_select_event = rr_selection_process_bcch_decode_event(
            candidate_db_ptr,
            available_plmn_db_ptr,
            RR_NORMAL_SEARCH_ORDER,
            gas_id
          );
        }

        else if (cell_select_event == EV_PERFORM_MULTIPASS_SEARCH)
        {
          /**
           * another round of cell selection needs to be conducted
           */
          rr_restart_cell_selection(candidate_db_ptr, gas_id);
          rr_cell_selection_data_ptr->rr_cell_selection_details.cell_selection_for_plmn_select_req = TRUE;

          new_cell_select_event = rr_selection_start_cell_selection(TRUE, gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
        }

        else if (cell_select_event == EV_MULTIPASS_SEARCH_FAILED)
        {
          rr_event = rr_selection_process_no_more_channels(
            candidate_db_ptr,
            available_plmn_db_ptr,
            gas_id
          );

          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
          rr_cleanup_cell_selection(gas_id);
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(cell_select_event),
                               rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
        }
      } /* RR_SELECTION_DECODING_BCCH */
      break;

      case RR_SELECTION_DECODING_BCCH:
      {
        /*********************************************************************************/
        /* RR has initiated a parallel decode operation in L1 and is waiting for         */
        /* indications from L1 about the state of the cells that have been found so far  */
        /* The RR_BCCH_DECODE_COMPLETE_TIMER was started to make sure we don't get stuck */
        /* , it must be stopped if we leave this state                                   */
        /********************************************************************************/

        if ((cell_select_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          if (message_set == MS_RR_L1)
          {
            if ((rr_l1_message_id_enum_T) message_id == MPH_DECODE_BCCH_LIST_FAILURE_IND)
            {
              word channel_number = message_ptr->mph_decode_bcch_list_failure_ind.frequency.num;

              sys_band_T band = message_ptr->mph_decode_bcch_list_failure_ind.frequency.band;
              /****************************************************************************************************/
              /* Indicates that a failure has occurred with a potential BCCH channel that L1 was trying to decode.*/
              /* (Sets the type of the indicated frequency in the candidate database appropriately based on the   */
              /* (failure reason from L1 and sets its status to processed (since it cannot be used for camping).  */
              /****************************************************************************************************/

              rr_candidate_db_set_type_from_failure_ind(
                candidate_db_ptr,
                message_ptr->mph_decode_bcch_list_failure_ind.frequency,
                message_ptr->mph_decode_bcch_list_failure_ind.failure_cause,
                gas_id
              );

              MSG_GERAN_HIGH_3_G("Cell=(%d,%d) BCCH decode failure=%d",
                       (int) channel_number,
                       (int) band,
                       (int) message_ptr->mph_decode_bcch_list_failure_ind.failure_cause
                      );

              new_cell_select_event = rr_selection_process_bcch_decode_event(
                candidate_db_ptr,
                available_plmn_db_ptr,
                RR_NORMAL_SEARCH_ORDER,
                gas_id
              );
            }
            else if ((rr_l1_message_id_enum_T) message_id == MPH_DECODE_BCCH_LIST_CNF)
            {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
              if (rr_background_plmn_search_in_progress(gas_id))
              {
                rr_cell_selection_data_ptr->rr_cell_selection_details.retry_required = FALSE;
              }
              else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
              {
                rr_cell_selection_data_ptr->rr_cell_selection_details.retry_required = message_ptr->mph_decode_bcch_list_cnf.retry_required;
              }

              /******************************************************/
              /* L1 done with the parallel decode operation for the */
              /* list of frequencies that were given.               */
              /******************************************************/
              new_cell_select_event = rr_selection_bcch_decode_completed(
                candidate_db_ptr,
                available_plmn_db_ptr,
                RR_NORMAL_SEARCH_ORDER,
                gas_id
              );
            }

            else if ((rr_l1_message_id_enum_T) message_id == MPH_FCCH_SCH_DECODE_IND)
            {
              rr_candidate_update_FCCH_SCH(&(message_ptr->mph_fcch_sch_decode_ind), gas_id);

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
              new_cell_select_event = rr_selection_process_bcch_decode_event(
                candidate_db_ptr,
                available_plmn_db_ptr,
                RR_NORMAL_SEARCH_ORDER,
                gas_id
              );
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */
            }

            else
            {
              MSG_GERAN_ERROR_3_G("Unex.Msg=%d,%d in State=%d",
                (int) message_set,
                (int) message_id,
                (int) rr_cell_selection_data_ptr->cell_select_state);
            }
          }
          else if (message_set == MS_MM_RR)
          {
            switch ((mm_rr_message_id_enum_T) message_id)
            {
              case RR_DEACT_REQ:
              case RR_STOP_GSM_MODE_REQ:
              {
                (void)rr_timer_cancel(RR_BCCH_DECODE_COMPLETE_TIMER, gas_id);

                if (rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received)
                {
                  rr_selection_aborted(RR_EV_MM_REQ_RECEIVED_ABORT, gas_id);
                  new_cell_select_event = rr_selection_complete_abort(gas_id);
                  rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
                }
                else
                {
                  rr_selection_abort_bcch_decode(RR_EV_MM_REQ_RECEIVED_ABORT, gas_id);
                  rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_ABORT;
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr,
                                       rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
              }
            }
          }
          else if (message_set == MS_TIMER)
          {
            new_cell_select_event = rr_cell_selection_handle_timer_expiry(
              &message_ptr->mid_timer_expiry,
              gas_id
            );
          }
          else
          {
            MSG_GERAN_ERROR_3_G("Unex.Msg=%d,%d in State=%d",
              (int) message_set,
              (int) message_id,
              (int) rr_cell_selection_data_ptr->cell_select_state);
          }
        }
        else if ( cell_select_event == RR_EV_CANDIDATE_CELL_SI_RECEIVED )
        {
          /*****************************************************************************/
          /* Indicates that system information for a candidate cell has been received. */
          /* Calls the 'rr_selection_process_bcch_decode_event' function               */
          /*****************************************************************************/
          new_cell_select_event = rr_selection_process_bcch_decode_event(
            candidate_db_ptr,
            available_plmn_db_ptr,
            RR_NORMAL_SEARCH_ORDER,
            gas_id
          );
        }
        else if ( cell_select_event == EV_NO_BCCH_FOUND )
        {
          /* No suitable/acceptable cells were found */
          /* call the process no more channels function to handle the condition */
          rr_event = rr_selection_process_no_more_channels(
            candidate_db_ptr,
            available_plmn_db_ptr,
            gas_id
          );
          rr_cleanup_cell_selection(gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
        }
        else if ( cell_select_event == RR_EV_DECODING_BCCH )
        {
          /************************************************************************/
          /* RR has decided to decode even more BCCHs in parallel and has started */
          /* another parallel BCCH decode operation. Since RR is already in the   */
          /* RR_SELECTION_DECODING_BCCH state, it remains in this state.          */
          /************************************************************************/
        }
        else if ( cell_select_event == RR_EV_SPECIFIC_CHANNEL_SEARCH )
        {
          /********************************************************************************************/
          /* RR has decided to try to camp on a cell (information is in pending channel)              */
          /* It has sent a MPH_SELECT_SPECIFIC_BCCH_REQ to L1 (which implicitly aborts the parallel   */
          /* decode operation). RR transitions to RR_SELECTION_WAIT_FOR_CHANNEL to await confirmation */
          /********************************************************************************************/

          (void)rr_timer_cancel(RR_BCCH_DECODE_COMPLETE_TIMER, gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_CHANNEL;
        }
        else if ( cell_select_event == RR_EV_DECODE_BCCH_COMPLETE_TIMEOUT )
        {
          /****************************************************************************/
          /* Indicates that the RR has timed out waiting for information for the last */
          /* remaining entry in the candidate cell database. All the unknown entries  */
          /* in the database are marked as NOT BCCH so they are not considered        */
          /****************************************************************************/

          /* mark all the unknown entries as NOT BCCH so they are not considered */
          rr_candidate_db_set_unknown_entry_types(candidate_db_ptr,RR_CANDIDATE_NOT_BCCH);

          new_cell_select_event = rr_selection_bcch_decode_completed(
            candidate_db_ptr,
            available_plmn_db_ptr,
            RR_NORMAL_SEARCH_ORDER,
            gas_id
          );
        }
        else if ( cell_select_event == RR_EV_CELL_SELECTION_ABORT_TIMEOUT ||
                  cell_select_event == RR_EV_MM_REQ_RECEIVED_ABORT )
        {
          (void)rr_timer_cancel(RR_BCCH_DECODE_COMPLETE_TIMER, gas_id);

          rr_selection_abort_bcch_decode(cell_select_event, gas_id);

          if ( rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received )
          {
            new_cell_select_event = rr_selection_complete_abort(gas_id);
            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
          }
          else
          {
            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_ABORT;
          }
        }

        else if( cell_select_event == EV_CELL_SELECTION_TRIGGERED)
        {
          /* This occurs in case of a service domain change */
          rr_event = EV_CELL_SELECTION_TRIGGERED;
        }

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
        else if ( cell_select_event == RR_EV_ABORT_PROCEDURE )
        {
          (void)rr_timer_cancel(RR_BCCH_DECODE_COMPLETE_TIMER, gas_id);

          if ( rr_cell_selection_data_ptr->rr_cell_selection_details.bcch_list_cnf_received )
          {
            rr_selection_aborted(cell_select_event, gas_id);
            new_cell_select_event = rr_selection_complete_abort(gas_id);
            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
          }
          else
          {
            /* only need to abort the decode if a bcch_list_cnf has
            not yet been received. */
            rr_selection_abort_bcch_decode(cell_select_event, gas_id);
            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_ABORT;
          }
        }
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */


        else if (cell_select_event == RR_EV_REPROCESS_CANDIDATE_CELL_DB)
        {
          /**
           * The conditions that determine cell suitability have changed so
           * all candidate cell database entries processed so far have to be
           * reprocessed.  Reset the 'processed' flag for all valid BCCHs
           * and check for a suitable cell.
           */
          rr_candidate_db_mark_valid_entries_as_unprocessed(candidate_db_ptr);
          new_cell_select_event = rr_selection_process_bcch_decode_event(
            candidate_db_ptr,
            available_plmn_db_ptr,
            RR_NORMAL_SEARCH_ORDER,
            gas_id
          );
        }

        else if (cell_select_event == EV_PERFORM_MULTIPASS_SEARCH)
        {
          /**
           * another round of cell selection needs to be conducted
           */
          rr_restart_cell_selection(candidate_db_ptr, gas_id);
          rr_cell_selection_data_ptr->rr_cell_selection_details.cell_selection_for_plmn_select_req = TRUE;

          new_cell_select_event = rr_selection_start_cell_selection(TRUE, gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
        }

        else if (cell_select_event == EV_MULTIPASS_SEARCH_FAILED)
        {

          rr_event = rr_selection_process_no_more_channels(
            candidate_db_ptr,
            available_plmn_db_ptr,
            gas_id
          );

          rr_cleanup_cell_selection(gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(cell_select_event),
                               rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
        }
      } /* RR_SELECTION_DECODING_BCCH */
      break;

      case RR_SELECTION_WAIT_FOR_CHANNEL:
      {
        /***********************************************************************************/
        /* RR has decided to camp on a specific channel (for various reasons) and has sent */
        /* a MPH_SELECT_SPECIFIC_BCCH_REQ to L1. It is waiting for the                     */
        /* MPH_SELECT_SPECIFIC_BCCH_CNF message. Note that the power scan db and available */
        /* plmn db have been cleared when the channel was selected, if the bcch is not     */
        /* found we will recover using the plmn camping list or doing a new power scan     */
        /***********************************************************************************/
        if (cell_select_event == EV_INPUT_MESSAGE && message_ptr != NULL)
        {
          if (message_set == MS_RR_L1)
          {
            switch ((rr_l1_message_id_enum_T) message_id)
            {
              case MPH_SELECT_SPECIFIC_BCCH_CNF:
              {
                new_cell_select_event = rr_selection_process_select_specific_bcch_cnf(
                  &message_ptr->mph_select_specific_bcch_cnf,
                  gas_id
                );
                break;
              }

              case MPH_DECODE_BCCH_LIST_CNF:
              {
                /* Ignore this message here - there is a race condition where
                if L1 sends the last SI and MPH_DECODE_BCCH_LIST_CNF at the
                same time, RR may receive the SI (in DECODING_BCCH state),
                send MPH_SELECT_SPECIFIC_BCCH_REQ to L1 and transition to this
                state before processing this signal. */
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr,
                                       rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
              }
            }
          }
          else if (message_set == MS_MM_RR)
          {
            switch ((mm_rr_message_id_enum_T) message_id)
            {
              case RR_DEACT_REQ:
              case RR_STOP_GSM_MODE_REQ:
              {
                rr_selection_aborted(RR_EV_MM_REQ_RECEIVED_ABORT, gas_id);
                rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_ABORT;
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr,
                                       rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
              }
            }
          }
          else if (message_set == MS_TIMER)
          {
            new_cell_select_event = rr_cell_selection_handle_timer_expiry(
              &message_ptr->mid_timer_expiry,
              gas_id
            );
          }
          else
          {
            LOG_UNEXPECTED_MESSAGE(message_ptr,
                                   rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
          }
        }

        else if ( cell_select_event == EV_USE_EXISTING_POWER_SCAN_RESULTS )
        {
          new_cell_select_event = rr_selection_process_power_scan_results(
            candidate_db_ptr,
            RR_NORMAL_SEARCH_ORDER,
            gas_id
          );
        }

        else if ( cell_select_event == RR_EV_CHANNEL_FOUND )
        {
          if (rr_sel_use_rr_select_bcch(gas_id))
          {
            rr_select_bcch_init_params_t init_params;
            rr_select_bcch_init_params_t *init_params_ptr;
            uint16 wait_for_si_mask;
            rr_select_bcch_early_camp_e early_camp_required;
            uint32 max_allowed_time_ms;

            gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
            boolean wait_for_si13 = FALSE;

            /**
             * Cell selection will wait for the SIs given in MASK_SI_REQUIRED_FOR_CAMP. However, if 'fast SI 
             * acq' is required, only a subset is required. These are specified in 'notify_si_mask', so 
             * RR-SELECT-BCCH module will indicate when this subset of SIs has been received. 
             * Then the RR-SELECT-BCCH acquisition can be aborted and cell selection can complete. 
             */
            if (rr_is_fast_si_acq_enabled(&wait_for_si13, gas_id))
            {
              wait_for_si_mask = MASK_SI_1 | MASK_SI_3;

              if (wait_for_si13)
              {
                wait_for_si_mask |= MASK_SI_13;
              }
            }
            else
            {
              // Include all SIs which must be received before entering GRR_CAMPED.
              // Note: Other SIs such as SI2quater will be requested to GL1, but not waited-for.
              wait_for_si_mask = (MASK_SI_1      |
                                  MASK_SI_2      |
                                  MASK_SI_2_BIS  |
                                  MASK_SI_2_TER  |
                                  MASK_SI_3      |
                                  MASK_SI_4      |
                                  MASK_SI_13);
            }

            if (pcell_info_ptr->gsm.control_channel_description_valid &&
                pcell_info_ptr->gsm.cell_identity_valid_flag)
            {
              /**
               * Populate initial parameters, which are optional, but required to 
               *  - enable Early Camping without waiting for SI3
               *  - enable use of the Sys Info Cache (as CGI is needed)
               */
              init_params.bs_ag_blks_res = pcell_info_ptr->gsm.control_channel_description.BS_AG_BLKS_RES;
              init_params.bs_cc_chans = pcell_info_ptr->gsm.control_channel_description.BS_CC_CHANS;
              init_params.bs_ccch_sdcch_comb = pcell_info_ptr->gsm.control_channel_description.BS_CCCH_SDCCH_COMB;
              init_params.bs_pa_mfrms = pcell_info_ptr->gsm.control_channel_description.BS_PA_MFRMS;
              init_params.imsi_valid = rr_get_imsi_mod_1000(&init_params.imsi_mod_1000, gas_id);
              init_params.lai = pcell_info_ptr->gsm.location_area_identification;
              init_params.ci = pcell_info_ptr->gsm.cell_identity;
              init_params.si_messages_on_bcch_ext = pcell_info_ptr->bcch_sys_info.si_messages_on_bcch_ext;
              init_params.si_messages_not_broadcast = pcell_info_ptr->bcch_sys_info.si_messages_not_broadcast;

              init_params_ptr = &init_params;
              early_camp_required = RR_SELECT_BCCH_EARLY_CAMP_USE_PARAMS;
            }
            else
            {
              init_params_ptr = NULL;
              early_camp_required = RR_SELECT_BCCH_EARLY_CAMP_SI3;
            }

            // Find out the time required for the acquisition
            max_allowed_time_ms = rr_si_calc_sys_info_acquisition_time(
              wait_for_si_mask & (~pcell_info_ptr->bcch_sys_info.si_messages_not_broadcast), // si_required
              pcell_info_ptr->bcch_sys_info.si_messages_on_bcch_ext                          // si_on_bcch_ext
            );

            /**
             * Send the request to RR-SELECT-BCCH module to acquire the sys info. 
             * This will first try the sys info cache, and then L1 for any remaining SIs. 
             * Note that ALL sys info is requested, even if only a subset is required for camping. This is 
             * because it is not desirable to exclude acquisition of sys info messages not required, but the 
             * acquistion will complete when the minimum set is received (as specified in 'notify_si_mask'). 
             */
            rr_select_bcch_send_imsg_acquire_si_req(
              pcell_info_ptr->gsm.BCCH_ARFCN,                       // arfcn
              pcell_info_ptr->gsm.reselection_info.RXLEV_average,   // rxlev_average
              pcell_info_ptr->gsm.BSIC,                             // bsic
              NULL,                                                 // cell_suitable_cb_fn
              early_camp_required,                                  // early_camp_required
              MASK_SI_NONE,                                         // notify_si_mask
              wait_for_si_mask,                                     // wait_for_si_mask
              init_params_ptr,                                      // init_params_ptr
              max_allowed_time_ms,                                  // max_allowed_time_ms
              gas_id                                                // gas_id
            );

            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_SYS_INFO_RR_SELECT_BCCH;
          }
          else
          {
            /*************************************************************************************/
            /* Channel was found and RR is ready to continue the attempt to camp on the channel. */
            /* The 'pending_channel_data' store updated and waiting for full SI. Depending       */
            /* on the type of selection, early camping may have been performed on this cell.     */
            /*************************************************************************************/
            rr_csi_activate_pend_store(gas_id);
            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_SYS_INFO;
          }
        }
        else if ( cell_select_event == RR_EV_PERFORM_POWER_SCAN )
        {
          /**************************************************************************/
          /* The channel was not found and RR is attempting to recover, but another */
          /* power scan was required (has already been requested).                  */
          /**************************************************************************/

          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_PSCAN_RESULTS;
        }
        else if ( cell_select_event == RR_EV_PERFORM_ACQ_DB_POWER_SCAN )
        {
          /**************************************************************************/
          /* The channel was not found and RR is attempting to recover, but another */
          /* acq db power scan was required (has already been requested).                  */
          /**************************************************************************/

          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_ACQ_DB_PSCAN_RESULTS;
        }
        else if ( cell_select_event == RR_EV_SPECIFIC_CHANNEL_SEARCH )
        {
          /*************************************************************************/
          /* Rather than perform a power scan or parallel decode at this time, the */
          /* RR has attempted to acquire another specific channel (stored in the   */
          /* 'rr_cell_selection_data_ptr->rr_cell_selection_details' structure). SM remains in the same state  */
          /* to await confirmation from L1.                                        */
          /*************************************************************************/
        }
        else if ( cell_select_event == RR_EV_DECODING_BCCH )
        {
          /* Either L1 has failed to select the target cell, or on receiving
          the MPH_SELECT_SPECIFIC_BCCH_CNF RR has decided that the cell is not
          suitable.  In either of these cases, if there are any entries in the
          candidate cell database that RR has not yet received system
          information for RR will trigger a further BCCH list decode. */
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_DECODING_BCCH;
        }
        else if (cell_select_event == RR_EV_DECODING_ACQ_DB_BCCH)
        {
          /* Either L1 has failed to select the target cell, or on receiving
          the MPH_SELECT_SPECIFIC_BCCH_CNF RR has decided that the cell is not
          suitable.  In either of these cases, if there are any entries in the
          candidate cell database that RR has not yet received system
          information for RR will trigger a further BCCH list decode. */
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_DECODING_ACQ_DB_BCCH;
        }
        else if ( cell_select_event == RR_EV_CELL_SELECTION_ABORT_TIMEOUT ||
                  cell_select_event == RR_EV_MM_REQ_RECEIVED_ABORT )

        {
          rr_selection_aborted(cell_select_event, gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_ABORT;
        }

        else if ( cell_select_event == EV_NO_BCCH_FOUND )
        {
          /* The cell is not suitable. */
          /* Call the process no more channels function to handle the condition */
          rr_event = rr_selection_process_no_more_channels(
            candidate_db_ptr,
            available_plmn_db_ptr,
            gas_id
          );
          rr_cleanup_cell_selection(gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
        }

        #ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
        else if ( cell_select_event == RR_EV_ABORT_PROCEDURE )
        {
          rr_selection_aborted(cell_select_event, gas_id);
          /* special case, process the generated event in the idle state */
          new_cell_select_event = rr_selection_complete_abort(gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
        }
        #endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

        else if (cell_select_event == RR_EV_REPROCESS_CANDIDATE_CELL_DB)
        {
          ; /* This signal can be ignored in this state as RR has already
            chosen a cell. */
        }
        else if (cell_select_event == RR_EV_CANDIDATE_CELL_SI_RECEIVED)
        {
          ; /* This signal can be ignored in this state as RR has already
            chosen a cell. */
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(cell_select_event),
                               rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
        }
      } /* RR_SELECTION_WAIT_FOR_CHANNEL state */
      break;

      case RR_SELECTION_WAIT_FOR_SYS_INFO:
      {
        /**************************************************************/
        /* RR has successfully acquired a cell and is waiting for     */
        /* all the system information for that cell to be decoded and */
        /* stored in the pending channel data store. RR may have      */
        /* already performed early camping on the cell before all the */
        /* system information is read. The SI timer was started       */
        /* it needs to be cancelled if we leave this state            */
        /**************************************************************/
        if ((cell_select_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (message_ptr->rr.header.rr_message_set)
            {
              case RR_L1_IDLE_OMSG:
              {
                switch (message_ptr->rr.header.imh.message_id)
                {
                  case RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF:
                  {
                    gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

                    MSG_GERAN_MED_0_G("L1 in Early Camping");

                    if (pcell_info_ptr->gsm.cell_identity_valid_flag)
                    {
                      rr_csi_copy_pend_store_to_cache(  pcell_info_ptr->gsm.BCCH_ARFCN, 
                                                      &(pcell_info_ptr->gsm.location_area_identification),
                                                      &(pcell_info_ptr->gsm.cell_identity),
                                                        gas_id);

                      if (rr_csi_retrieve_all_SIs(  pcell_info_ptr->gsm.BCCH_ARFCN, 
                                                  &(pcell_info_ptr->gsm.location_area_identification),
                                                  &(pcell_info_ptr->gsm.cell_identity),
                                                    rr_csi_is_retrieval_for_fast_acquisition(gas_id),
                                                    gas_id) )
                      {
                        MSG_GERAN_HIGH_0_G("RR retrieve SI's after Early Camp CNF");
                      }
                    }

                    rr_csi_deactivate_pend_store(gas_id);
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
                    );
                  }
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message_ptr,
                  rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
                );
              }
            }
          }
          else if (message_set == MS_MM_RR)
          {
            switch ((mm_rr_message_id_enum_T) message_id)
            {
              case RR_DEACT_REQ:
              case RR_STOP_GSM_MODE_REQ:
              {
                rr_selection_aborted(RR_EV_MM_REQ_RECEIVED_ABORT, gas_id);

                (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);
                (void)rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id);

                new_cell_select_event = rr_selection_complete_abort(gas_id);

                rr_csi_deactivate_pend_store(gas_id);

                rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message_ptr,
                  rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
                );
              }
            }
          }
          else if (message_set == MS_TIMER)
          {
            new_cell_select_event = rr_cell_selection_handle_timer_expiry(
              &message_ptr->mid_timer_expiry,
              gas_id
            );
          }
          else
          {
            LOG_UNEXPECTED_MESSAGE(
              message_ptr,
              rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
            );
          }
        }

        /*************************************************************************/
        /* RR_EV_EARLY_CAMP_SI_RECEIVED                                          */
        /* Received SI required for early camping                                */
        /*************************************************************************/
        else if ( cell_select_event == RR_EV_EARLY_CAMP_SI_RECEIVED )
        {
          /***************************************************************************/
          /* NOTE: even if early camping is performed, no events should be generated */
          /* since RR still needs to wait for full SI before camping                 */
          /* if early camping has already been performed, no action will be taken    */
          /***************************************************************************/
          rr_selection_process_early_camp_data(gas_id);
        }

        /*************************************************************************/
        /* RR_EV_GSM_SI_RECEIVED                                                 */
        /* All GSM SIs have been received                                        */
        /*************************************************************************/
        else if ( cell_select_event == RR_EV_GSM_SI_RECEIVED )
        {
          /* Check the cell's suitability for camping-on */
          /* This will either return RR_EV_ACQUISITION_FAILURE or EV_CHECK_GPRS */
          new_cell_select_event = rr_selection_process_channel_data(TRUE, gas_id);
        }

        /***************************************************************/
        /* EV_CHECK_GPRS                                               */
        /***************************************************************/
        else if ( cell_select_event == EV_CHECK_GPRS )
        {
          gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

          if ( pcell_info_ptr->gsm.gprs_indicator )
          {
            MSG_GERAN_HIGH_0_G("Cell supports GPRS");

            if (pcell_info_ptr->bcch_sys_info.si13_data_valid_flag)
            {
              new_cell_select_event = RR_EV_GPRS_SI13_RECEIVED;
            }
            else
            {
              boolean wait_for_si13 = TRUE;
              boolean is_fast_si_acq_enabled = rr_is_fast_si_acq_enabled(&wait_for_si13, gas_id);

              if (is_fast_si_acq_enabled && !wait_for_si13)
              {
                MSG_GERAN_HIGH_0_G("Fast SI Acq enabled, SI13 not yet received, start GSM idle mode");
                new_cell_select_event = EV_GO_TO_IDLE;
              }
              else
              {
                MSG_GERAN_HIGH_0_G("Wait for SI13 ...");

                /* Start guard timer for SI13 */
                (void)rr_timer_start( RR_GPRS_SYS_INFO_TIMER, RR_GPRS_SYS_INFO_TIMEOUT, gas_id);
              }
            }
          }
          else
          {
            MSG_GERAN_HIGH_0_G("Cell does not support GPRS");

            new_cell_select_event = EV_GO_TO_IDLE;
          }
        }

        /***************************************************************/
        /* RR_EV_GPRS_SI13_RECEIVED                                    */
        /***************************************************************/
        else if ( cell_select_event == RR_EV_GPRS_SI13_RECEIVED )
        {
          gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

          if ( ARE_SET( MASK_SI_2_TER, pcell_info_ptr->gsm.update_message_received ) )
          {
            new_cell_select_event = EV_GO_TO_IDLE;
          }
          else
          {
#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
            if (rr_x2g_redir_with_si_in_progress(gas_id))
            {
              new_cell_select_event = EV_GO_TO_IDLE;
            }
            else 
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */
            if (rr_is_fast_si_acq_enabled(NULL,gas_id))
            {
              MSG_GERAN_HIGH_0_G("Fast SI Acq enabled, need not wait for Si2ter");
              new_cell_select_event = EV_GO_TO_IDLE;
            }
            else
            {
              MSG_GERAN_MED_0_G("Wait for SI2ter...");
            }
          }
        }

        /***************************************************************/
        /* RR_EV_GPRS_SYS_INFO_TIMEOUT                                 */
        /***************************************************************/
        else if ( cell_select_event == RR_EV_GPRS_SYS_INFO_TIMEOUT )
        {
          new_cell_select_event = RR_EV_ACQUISITION_FAILURE;
        }

        /***************************************************************/
        /* RR_EV_CAMPED_ON_CELL                                        */
        /* All necessary SIs (and PSIs) have been received, and L1 has */
        /* started appropriate idle mode procedures                    */
        /***************************************************************/
        else if ( cell_select_event == EV_GO_TO_IDLE )
        {
          rr_selection_complete_cell_camping(gas_id);

          /* RR has gathered all the system information required for the GPRS cell */

          if (rr_PLMN_request_details_ptr->search_reason == RR_PLMN_SELECTION)
          {
            rr_event = rr_selection_completed(EV_GO_TO_IDLE_PLMN_SELECTION, gas_id);
          }
          else
          {
            rr_event = rr_selection_completed(EV_GO_TO_IDLE, gas_id);
          }

          rr_csi_deactivate_pend_store(gas_id);

          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
        }

        /***************************************************************/
        /* RR_EV_SYS_INFO_TIMEOUT                                      */
        /* Timeout waiting for GSM SI                                  */
        /***************************************************************/
        else if ( cell_select_event == RR_EV_SYS_INFO_TIMEOUT )
        {
          /*******************************************************************/
          /* Timeout waiting for SI, process the data indicating acquisition */
          /* failure                                                         */
          /*******************************************************************/

          /* log the camping attempt failure */
          rr_log_camping_attempt_failure(RR_LOG_CAMPING_FAILURE_BCCH_DECODE_TIMEOUT, gas_id);

          new_cell_select_event = rr_selection_process_channel_data(FALSE, gas_id);
        }

        /***************************************************************/
        /* RR_EV_ACQUISITION_FAILURE                                   */
        /* Timeout waiting for GPRS SI                                 */
        /***************************************************************/
        else if ( cell_select_event == RR_EV_ACQUISITION_FAILURE )
        {
          new_cell_select_event = rr_selection_recover_from_acquisition_failure(
            candidate_db_ptr,
            available_plmn_db_ptr,
            gas_id
          );
        }

        /***************************************************************/
        /* RR_EV_DECODING_BCCH                                         */
        /* Acquisition failure may result of sending DECODE BCCH LIST REQ to GL1 */
        /* So Moving back to RR_SELECTION_DECODING_BCCH                */
        /***************************************************************/
        else if(cell_select_event == RR_EV_DECODING_BCCH)
        {
          rr_csi_deactivate_pend_store(gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_DECODING_BCCH;
        }

        /***************************************************************/
        /* RR_EV_PERFORM_POWER_SCAN                                    */
        /* Power-scan has been started to try to find another cell, due*/
        /* to either sys info time-out or cell unsuitability           */
        /***************************************************************/
        else if ( cell_select_event == RR_EV_PERFORM_POWER_SCAN )
        {
          rr_csi_deactivate_pend_store(gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_PSCAN_RESULTS;
        }

        else if ( cell_select_event == RR_EV_PERFORM_ACQ_DB_POWER_SCAN )
        {
          /**************************************************************************/
          /* RR_EV_PERFORM_ACQ_DB_POWER_SCAN                                        */
          /* ACQ DB Power-scan has been started to try to find another cell, due               */
          /* to either sys info time-out or cell unsuitability                  */
          /**************************************************************************/

          rr_csi_deactivate_pend_store(gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_ACQ_DB_PSCAN_RESULTS;
        }

        /***************************************************************/
        /* RR_EV_SPECIFIC_CHANNEL_SEARCH                               */
        /* Channel was no good, we can ask for another channel if we   */
        /* have a valid plmn camping list and doing plmn selection     */
        /* instead of doing a power scan or parallel decode, new       */
        /* channel specified                                           */
        /***************************************************************/
        else if ( cell_select_event == RR_EV_SPECIFIC_CHANNEL_SEARCH )
        {
          rr_csi_deactivate_pend_store(gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_CHANNEL;
        }

        else if ( cell_select_event == RR_EV_CELL_SELECTION_ABORT_TIMEOUT ||
                  cell_select_event == RR_EV_MM_REQ_RECEIVED_ABORT )
        {
          rr_selection_aborted(cell_select_event, gas_id);
          (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);
          (void)rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id);
          new_cell_select_event = rr_selection_complete_abort(gas_id);

          rr_csi_deactivate_pend_store(gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
        }

        else if ( cell_select_event == RR_EV_ABORT_PROCEDURE ||
                  cell_select_event == EV_NO_BCCH_FOUND )
        {
          if (rr_sel_is_search_cause_service_req_acq_db_scan(rr_PLMN_request_details_ptr))
          {
            MSG_GERAN_HIGH_0_G("SI acquisition failed for RR_MM_ACQ_DB_SCAN, abort search");
            (void)rr_selection_process_no_more_channels(candidate_db_ptr, available_plmn_db_ptr, gas_id);
          }

          rr_selection_aborted(cell_select_event, gas_id);
          (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);
          (void)rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id);
          /*In case after sending early camping and before confirm is recvd if timer expires*/
          /*the procedure gets aborted and hence in such case we need to reset rr_l1_idle state */		       
          rr_l1_idle_init(gas_id);                   
          rr_event = rr_selection_complete_abort(gas_id);

          rr_csi_deactivate_pend_store(gas_id);
          rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
        }

        else if (cell_select_event == RR_EV_REPROCESS_CANDIDATE_CELL_DB)
        {
          ; /* This signal can be ignored in this state as RR has already
            chosen a cell. */
        }

        else if (cell_select_event == RR_EV_CANDIDATE_CELL_SI_RECEIVED)
        {
          ; /* This signal can be ignored */
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(cell_select_event),
                               rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
        }
      } /* RR_SELECTION_WAIT_FOR_SYS_INFO */
      break;

      case RR_SELECTION_WAIT_FOR_SYS_INFO_RR_SELECT_BCCH:
      {
        switch (cell_select_event)
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_L1_IDLE_OMSG:
                  {
                    rr_select_bcch_control(message_ptr, gas_id);
                    break;
                  }

                  case RR_SELECT_BCCH_OMSG:
                  {
                    switch (message_ptr->rr.select_bcch.omsg.header.imh.message_id)
                    {
                      case RR_SELECT_BCCH_OMSG_SELECT_CNF:
                      {
                        rr_select_bcch_omsg_select_cnf_t *omsg_ptr;

                        omsg_ptr = &message_ptr->rr.select_bcch.omsg.rr_select_bcch_omsg_select_cnf;

                        rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_SYS_INFO;
                        new_cell_select_event = EV_GO_TO_IDLE;
                        break;
                      }

                      case RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND:
                      {
                        rr_select_bcch_omsg_select_fail_ind_t *omsg_ptr;

                        omsg_ptr = &message_ptr->rr.select_bcch.omsg.rr_select_bcch_omsg_select_fail_ind;

                        switch (omsg_ptr->reason)
                        {
                          case RR_SELECT_BCCH_FAIL_REASON_ABORTED:
                          {
                            new_cell_select_event = rr_selection_complete_abort(gas_id);
                            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
                            break;
                          }

                          case RR_SELECT_BCCH_FAIL_REASON_TIMEOUT:
                          {
                            rr_log_camping_attempt_failure(RR_LOG_CAMPING_FAILURE_BCCH_DECODE_TIMEOUT, gas_id);

                            new_cell_select_event = rr_selection_recover_from_acquisition_failure(
                              candidate_db_ptr,
                              available_plmn_db_ptr,
                              gas_id
                            );

                            rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_WAIT_FOR_SYS_INFO;
                            break;
                          }

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("Unhandled falure reason (%d)",omsg_ptr->reason);
                          }
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message_ptr,
                          rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
                    );
                  }
                }
                break;
              } // case MS_RR_RR

              case MS_MM_RR:
              {
                switch ((mm_rr_message_id_enum_T) message_id)
                {
                  case RR_DEACT_REQ:
                  case RR_STOP_GSM_MODE_REQ:
                  {
                    rr_selection_aborted(RR_EV_MM_REQ_RECEIVED_ABORT, gas_id);

                    // Abort the RR-SELECT-BCCH procedure in progress
                    rr_select_bcch_send_imsg_abort_req(gas_id);

                    // Wait for RR-SELECT-BCCH to confirm the abort
                    rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_ABORT;
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
                    );
                  }
                }
                break;
              } // case MS_MM_RR

              case MS_TIMER:
              {
                switch (message_ptr->mid_timer_expiry.timer_id)
                {
                  case RR_CELL_SELECTION_ABORT_TIMER:
                  {
                    rr_selection_aborted(RR_EV_CELL_SELECTION_ABORT_TIMEOUT, gas_id);

                    // Abort RR-SELECT-BCCH - this will reply with RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND
                    rr_select_bcch_send_imsg_abort_req(gas_id);
                    break;
                  }

                  default:
                  {
                    rr_select_bcch_control(message_ptr, gas_id);
                  }
                }
                break;
              } // case MS_TIMER

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message_ptr,
                  rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
                );
              }
            } // switch (message_set)
            break;
          } // case EV_INPUT_MESSAGE

          case RR_EV_MM_REQ_RECEIVED_ABORT:
          {
            rr_selection_aborted(RR_EV_MM_REQ_RECEIVED_ABORT, gas_id);

            // Abort RR-SELECT-BCCH - this will reply with RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND
            rr_select_bcch_send_imsg_abort_req(gas_id);
            break;
          }

          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          {
            break;  // ignored
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(cell_select_event),
                                 rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
          }
        } // switch (cell_select_event)
        break;
      } /* RR_SELECTION_WAIT_FOR_SYS_INFO_RR_SELECT_BCCH state */

      case RR_SELECTION_ABORT:
      {
        switch (cell_select_event)
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_RETURN_PARAM(message_ptr, EV_NO_EVENT);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_L1_IDLE_OMSG:
                  {
                    rr_select_bcch_control(message_ptr, gas_id);
                    break;
                  }

                  case RR_SELECT_BCCH_OMSG:
                  {
                    switch (message_ptr->rr.select_bcch.omsg.header.imh.message_id)
                    {
                      case RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND:
                      {
                        new_cell_select_event = rr_selection_complete_abort(gas_id);
                        rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message_ptr,
                          rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
                    );
                  }
                }
                break;
              }

              case MS_RR_L1:
              {
                switch (message_ptr->message_header.message_id)
                {
                  case MPH_POWER_SCAN_CNF:
                  {
                    rr_process_mph_power_scan_cnf(&message_ptr->mph_power_scan_cnf);

                    new_cell_select_event = rr_selection_complete_abort(gas_id);
                    rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
                    break;
                  }

                  case MPH_DECODE_BCCH_LIST_CNF:
                  case MPH_SELECT_SPECIFIC_BCCH_CNF:
                  {
                    new_cell_select_event = rr_selection_complete_abort(gas_id);
                    rr_cell_selection_data_ptr->cell_select_state = RR_SELECTION_IDLE;
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
                    );
                  }
                }
                break;
              }

              case MS_MM_RR:
              {
                switch ((mm_rr_message_id_enum_T) message_id)
                {
                  case RR_DEACT_REQ:
                  case RR_STOP_GSM_MODE_REQ:
                  {
                    /* it can be possible to receive RR_DEACT_REQ in this state. CR 605501 */
                    /* Set the mm_abort flag to register abort request from NAS */
                    rr_selection_aborted(RR_EV_MM_REQ_RECEIVED_ABORT, gas_id);

                    MSG_GERAN_HIGH_0_G("received RR_DEACT_REQ in cell selection abort,still waiting for L1 response");
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
                    );
                  }
                }
                break;
              }

              case MS_TIMER:
              {
                new_cell_select_event = rr_cell_selection_handle_timer_expiry(
                  &message_ptr->mid_timer_expiry,
                  gas_id
                );
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message_ptr,
                  rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state)
                );
              }
            }
            break;
          } // case EV_INPUT_MESSAGE

          case RR_EV_MM_REQ_RECEIVED_ABORT:
          {
            rr_selection_aborted(RR_EV_MM_REQ_RECEIVED_ABORT, gas_id);
            break;
          }

          // Events which can be ignored while waiting to abort
          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          case RR_EV_REPROCESS_CANDIDATE_CELL_DB:
          {
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(cell_select_event),
                                 rr_cell_selection_state_name(rr_cell_selection_data_ptr->cell_select_state));
          }
        } // switch (cell_select_event)
        break;
      } /* RR_SELECTION_ABORT state */

      default:
      {
        MSG_GERAN_ERROR_1_G("Unknown state %d",rr_cell_selection_data_ptr->cell_select_state);
        break;
      }
    }

 #ifdef DEBUG_RR_TRACE_MSG
    if (((cell_select_event != new_cell_select_event) && (new_cell_select_event != EV_NO_EVENT)) ||
        (rr_cell_selection_data_ptr->old_cell_select_state !=
         rr_cell_selection_data_ptr->cell_select_state))
    {
      rr_store_trace_msg_buf(
        RR_CELL_SELECTION_SM,
        new_cell_select_event,
        (byte) rr_cell_selection_data_ptr->cell_select_state,
        message_ptr,
        gas_id
      );
    }
#endif

    if (new_cell_select_event != EV_NO_EVENT)
    {
      cell_select_event = new_cell_select_event;
      new_cell_select_event = EV_NO_EVENT;
    }
    else
    {
      cell_select_event = EV_NO_EVENT;
    }

    if (rr_cell_selection_data_ptr->old_cell_select_state != rr_cell_selection_data_ptr->cell_select_state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_selection_control",
        rr_cell_selection_data_ptr->old_cell_select_state,
        rr_cell_selection_data_ptr->cell_select_state,
        rr_cell_selection_state_name,
        gas_id
      );

      rr_cell_selection_data_ptr->old_cell_select_state = rr_cell_selection_data_ptr->cell_select_state;
    }
  } /* while (cell_select_event != EV_NO_EVENT) */

  return rr_event;
} /* rr_selection_control */

/* EOF */
