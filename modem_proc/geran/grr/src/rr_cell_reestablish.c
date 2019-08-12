/*! \file rr_cell_reestablish.c

  This module implements an optimised cell selection process, for the purpose of call
  re-establishment following Radio Link Failure.

                Copyright (c) 2007-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_cell_reestablish.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"  /* Contains FEATURE_... defines */
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_gprs_defs.h"
#include "rr_cell_reestablish.h"
#include "rr_resel_calcs.h"
#include "rr_general.h"
#include "rr_conn_establish.h"
#include "rr_l1_send.h"
#include "ms.h"
#include "rr_log.h"
#include "rr_mm_send.h"
#include "rr_candidate_cell_db.h"
#include "rr_pscan.h"
#include "rr_resel_utils.h"
#include "rr_sys_info_pcell.h"
#include "rr_sys_info_ncell.h"
#include "rr_select_bcch_if.h"
#include "rr_select_bcch.h"
#include "gprs_mem.h"
#include "rr_ded_config.h"
#include "rr_multi_sim.h"
#include "rr_l1_idle_mode.h"
#include "rr_acq_db.h"
#ifdef FEATURE_IDLE_DRX_SCALING
#include "rr_idle_drx.h"
#endif /* FEATURE_IDLE_DRX_SCALING */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  RR_REEST_NULL,
  RR_REEST_SELECT_CELL_REEST,
  RR_REEST_SELECT_CELL_CAMP,
  RR_REEST_ABORT_THEN_CAMP,
  RR_REEST_ABORT_THEN_EXIT
} rr_reest_state_e;

typedef struct
{
  uint8             num_of_ded_channels;
  channel_type_T    channel_type_1;
  channel_type_T    channel_type_2;
} rr_reest_api_data_t;

typedef struct
{
  rr_reest_state_e                  state;
  rr_reest_state_e                  old_state;
  rr_reselection_pscan_cell_list_T  pscan_cell_list;
  boolean                           rr_est_req_valid;
  rr_est_req_T                      rr_est_req;
  rr_candidate_cell_T              *candidate_cell_ptr;
} rr_reest_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_reest_api_data_t rr_reest_api_data[NUM_GERAN_DATA_SPACES];
static rr_reest_data_t *rr_reest_data_ptrs[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module API data.
 *
 * \param gas_id
 *
 * \return rr_reest_api_data_t*
 */
static rr_reest_api_data_t *rr_reest_api_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_reest_api_data[as_index];
}

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_reest_data_t*
 */
static rr_reest_data_t *rr_reest_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return rr_reest_data_ptrs[as_index];
}

/*!
 * \brief Initialisation function.
 *
 * \param gas_id
 */
static void rr_reest_init(rr_reest_data_t *rr_reest_data_ptr)
{
  RR_NULL_CHECK_FATAL(rr_reest_data_ptr);

  rr_reest_data_ptr->state = RR_REEST_NULL;
  rr_reest_data_ptr->old_state = RR_REEST_NULL;
}

/**
  @brief Allocate memory for the run-time data
 */
static rr_reest_data_t *rr_reest_alloc_and_init_data(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  rr_reest_data_t *rr_reest_data_ptr = rr_reest_data_ptrs[as_index];

  if (rr_reest_data_ptr == NULL)
  {
    rr_reest_data_ptr = GPRS_MEM_CALLOC(sizeof(rr_reest_data_t));

    rr_reest_data_ptrs[as_index] = rr_reest_data_ptr;
  }

  rr_reest_init(rr_reest_data_ptr);

  return rr_reest_data_ptr;
} /* rr_reest_alloc_and_init_data */


/**
  @brief Free the memory allocated for the run-time data
 */
static void rr_reest_free_data(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
  rr_reest_data_t *rr_reest_data_ptr = rr_reest_data_ptrs[as_index];

  if (rr_reest_data_ptr != NULL)
  {
    GPRS_MEM_FREE(rr_reest_data_ptr);

    rr_reest_data_ptrs[as_index] = NULL;
  }

} /* rr_reest_free_data */

static rr_reest_state_e rr_reest_get_state(
  const gas_id_t gas_id
)
{
  rr_reest_data_t *rr_reest_reest_data_ptr = rr_reest_get_data_ptr(gas_id);

  if (rr_reest_reest_data_ptr != NULL)
  {
    return rr_reest_reest_data_ptr->state;
  }

  return RR_REEST_NULL;
}

/*!
 * \brief Returns a string representing a given reest_state_T value
 *
 * \param state
 *
 * \return const char*
 */
static const char *rr_reest_state_name(rr_reest_state_e state)
{
  switch (state)
  {
    case RR_REEST_NULL:                return "NULL";
    case RR_REEST_SELECT_CELL_REEST:   return "SELECT_CELL_REEST";
    case RR_REEST_SELECT_CELL_CAMP:    return "SELECT_CELL_CAMP";
    case RR_REEST_ABORT_THEN_CAMP:     return "ABORT_THEN_CAMP";
    case RR_REEST_ABORT_THEN_EXIT:     return "ABORT_THEN_EXIT";

    default:
      MSG_GERAN_ERROR_1("Unknown state: %d", state);
      return "?";
  }

} /* rr_reest_state_name */

/*!
 * \brief Call-back function supplied to RR-SELECT-BCCH to check a cell's suitability for re-establishment.
 *
 * \param candidate_cell_ptr - Pointer to candidate cell information
 * \param gas_id
 *
 * \return boolean - TRUE is cell suitable; FALSE otherwise
 */
static boolean rr_reest_cell_suitable_reest_cb_fn(
  rr_candidate_cell_T *candidate_cell_ptr,
  const gas_id_t gas_id
)
{
  boolean cell_suitable;
  boolean any_cell_selection_mode;
  boolean automatic_plmn_selection_mode;

  /* Overrule cell selection mode if re-establishing an Emergency Call */
  if (rr_ce_for_emergency_call(gas_id))
  {
    any_cell_selection_mode = TRUE;
  }
  else
  {
    any_cell_selection_mode = rr_is_any_cell_selection_mode(gas_id);
  }

  automatic_plmn_selection_mode = rr_is_automatic_plmn_selection_mode(gas_id);

  cell_suitable = rr_check_candidate_suitable(
    candidate_cell_ptr,
    any_cell_selection_mode,
    automatic_plmn_selection_mode,
    gas_id
  );

  if (cell_suitable)
  {
    if (candidate_cell_ptr->si_info.REESTABLISHMENT_ALLOWED)
    {
      MSG_GERAN_HIGH_1_G("Attempt reestablishment on %d",
               (int)candidate_cell_ptr->meas.arfcn.num);
    }
    else
    {
      MSG_GERAN_HIGH_1_G("Cell %d not suitable (reestablishment not allowed)",
               (int)candidate_cell_ptr->meas.arfcn.num);
      cell_suitable = FALSE;
    }
  }
  else
  {
    MSG_GERAN_HIGH_3_G("Cell %d not suitable (candidate cell check failed any_cell:%d auto_plmn:%d)",
             (int)candidate_cell_ptr->meas.arfcn.num, any_cell_selection_mode, automatic_plmn_selection_mode);
  }

  return cell_suitable;

} /* rr_reest_cell_suitable_cb_fn */

/*!
 * \brief Call-back function supplied to RR-SELECT-BCCH to check a cell's suitability for normal camping.
 *
 * \param candidate_cell_ptr - Pointer to candidate cell information
 * \param gas_id
 *
 * \return boolean - TRUE is cell suitable; FALSE otherwise
 */
static boolean rr_reest_cell_suitable_camp_cb_fn(
  rr_candidate_cell_T *candidate_cell_ptr,
  const gas_id_t gas_id
)
{
  boolean cell_suitable;
  boolean any_cell_selection_mode;
  boolean automatic_plmn_selection_mode;

  any_cell_selection_mode = rr_is_any_cell_selection_mode(gas_id);
  automatic_plmn_selection_mode = rr_is_automatic_plmn_selection_mode(gas_id);

  cell_suitable = rr_check_candidate_suitable(
    candidate_cell_ptr,
    any_cell_selection_mode,
    automatic_plmn_selection_mode,
    gas_id
  );

  if (!cell_suitable)
  {
    MSG_GERAN_HIGH_3_G("Cell %d not suitable (candidate cell check failed any_cell:%d auto_plmn:%d)",
             (int)candidate_cell_ptr->meas.arfcn.num, any_cell_selection_mode, automatic_plmn_selection_mode);
  }

  return cell_suitable;

} /* rr_reest_cell_suitable_camp_cb_fn */

/*!
 * \brief This function scans the candidate cell database to find the strongest cell which is suitable for
 *        normal camping.
 *
 * \param gas_id
 *
 * \return rr_candidate_cell_T* - Pointer to candidate cell info; NULL if no suitable cells found
 */
static rr_candidate_cell_T *rr_reest_find_cell_suitable_for_idle_camp(
  const gas_id_t gas_id
)
{
  uint16 num_of_unprocessed_cells;
  rr_candidate_cell_T *candidate_cell_ptr = NULL;
  rr_candidate_cell_db_T *candidate_db_ptr = rr_get_candidate_cell_db_ptr(gas_id);

  RR_NULL_CHECK_FATAL(candidate_db_ptr);

  MSG_GERAN_HIGH_0_G("Try to find a normal cell to camp");

  // Reset the status of valid candidates to 'unprocessed'
  rr_candidate_db_mark_valid_entries_as_unprocessed(candidate_db_ptr);

  num_of_unprocessed_cells = rr_candidate_db_get_num_unprocessed_cell(candidate_db_ptr);

  if (num_of_unprocessed_cells > 0)
  {
    boolean cell_found;
    boolean cell_suitable = FALSE;

    while ( (num_of_unprocessed_cells > 0) && (cell_suitable == FALSE) )
    {
      boolean cell_processed = TRUE;

      /* Find the first unprocessed cell (which are in order of strength) */
      /* for which L1 has successfully decoded the BCCH */
      cell_found = rr_candidate_db_find_next(
        candidate_db_ptr,
        &candidate_cell_ptr,
        RR_CANDIDATE_STRONGEST_FIRST
      );

      if (cell_found && (candidate_cell_ptr != NULL))
      {
        if (candidate_cell_ptr->type == RR_CANDIDATE_VALID_BCCH)
        {
          // Use the call-back function to check suitability
          cell_suitable = rr_reest_cell_suitable_camp_cb_fn(candidate_cell_ptr, gas_id);
        }
        else
        {
          cell_processed = FALSE;
        }

        // Need to mark the cell as 'processed' otherwise rr_candidate_db_find_next() will return it again
        candidate_cell_ptr->processed = TRUE;
      }

      if (cell_processed)
      {
        num_of_unprocessed_cells--;
      }
    }

    if (cell_suitable)
    {
      MSG_GERAN_HIGH_1_G("Cell %d is suitable for camping",
               (int)candidate_cell_ptr->meas.arfcn.num);
    }
    else
    {
      candidate_cell_ptr = NULL;
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("No candidate cells");
  }

  return candidate_cell_ptr;

} /* end rr_reest_find_cell_suitable_for_idle_camp() */

/*!
 * \brief Finds a suitable cell (from the candidate cell database) for normal camping, and requests
 *        RR-SELECT-BCCH to select it (sends RR_SELECT_BCCH_IMSG_SELECT_REQ).
 *
 * \param candidate_cell_ptr (in) - pointer to the candidate cell data
 * \param gas_id (in)
 */
static void rr_reest_select_bcch_for_idle_camp(
  rr_candidate_cell_T *candidate_cell_ptr,
  const gas_id_t gas_id
)
{
  BSIC_T BSIC ={INVALID_PLMN_CC,INVALID_BSIC_CC};
  if (candidate_cell_ptr != NULL)
  {
    // Obtain a pointer to the module data
    rr_reest_data_t *rr_reest_data_ptr = rr_reest_get_data_ptr(gas_id);
    RR_NULL_CHECK_FATAL(rr_reest_data_ptr);

    // Store the pointer to the candidate cell for use later
    rr_reest_data_ptr->candidate_cell_ptr = candidate_cell_ptr;

    rr_select_bcch_send_imsg_select_req(
      candidate_cell_ptr->meas.arfcn,
      candidate_cell_ptr->meas.RXLEV_average,
      NULL,
      FALSE,
      MASK_SI_NONE,
      MASK_SI_NONE,
      BSIC,
      gas_id
    );
  }

} /* end rr_reest_select_bcch_for_idle_camp() */

/*!
 * \brief Checks that the dedicated channel config being used before the RLF is suitable for re-establishment
 *        or not. (The re-establishment procedure is not possible for a signalling connection).
 *
 * \param gas_id
 *
 * \return boolean - TRUE if re-establishment is allowed; FALSE otherwise
 */
static boolean rr_reest_is_channel_config_ok_for_reest(
  const gas_id_t gas_id
)
{
  boolean channel_config_ok_for_reest = FALSE;

  // Obtain a pointer to the module API data
  rr_reest_api_data_t *rr_reest_api_data_ptr = rr_reest_api_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_reest_api_data_ptr);

  if (rr_reest_api_data_ptr->num_of_ded_channels == 1)
  {
    if ((rr_reest_api_data_ptr->channel_type_1 == FULL_RATE_TRAFFIC) ||
        (rr_reest_api_data_ptr->channel_type_1 == HALF_RATE_TRAFFIC))
    {
      channel_config_ok_for_reest = TRUE;
    }
  }
  else if (rr_reest_api_data_ptr->num_of_ded_channels == 2)
  {
    byte NECI;
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

    RR_NULL_CHECK_FATAL(scell_info_ptr);

    NECI = scell_info_ptr->gsm.cell_selection_parameters.NECI;

    if ((rr_reest_api_data_ptr->channel_type_1 == HALF_RATE_TRAFFIC) &&
        (rr_reest_api_data_ptr->channel_type_2 == HALF_RATE_TRAFFIC) &&
        (NECI == 1))
    {
      channel_config_ok_for_reest = TRUE;
    }
  }

  if (!channel_config_ok_for_reest)
  {
    MSG_GERAN_HIGH_3_G("Channel config not suitable for reest (num=%d ch1=%d ch2=%d)",
             rr_reest_api_data_ptr->num_of_ded_channels,
             rr_reest_api_data_ptr->channel_type_1,
             rr_reest_api_data_ptr->channel_type_2);
  }

  return channel_config_ok_for_reest;

} /* rr_reest_is_channel_config_ok_for_reest */

/*!
 * \brief Saves a RR_EST_REQ (locally).
 *
 * \param rr_est_req
 * \param gas_id
 */
static void rr_reest_save_rr_est_req(
  rr_est_req_T *rr_est_req,
  const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_reest_data_t *rr_reest_data_ptr = rr_reest_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_reest_data_ptr);

  rr_reest_data_ptr->rr_est_req = (*rr_est_req);
  rr_reest_data_ptr->rr_est_req_valid = TRUE;

  return;
} /* rr_reest_save_rr_est_req */

/*!
 * \brief Discards any saved RR_EST_REQ.
 *
 * \param gas_id
 *
 * \return boolean - TRUE if there was a saved RR_EST_REQ; FALSE otherwise
 */
static boolean rr_reest_discard_rr_est_req(const gas_id_t gas_id)
{
  boolean rr_est_req_valid = FALSE;

  // Obtain a pointer to the module data
  rr_reest_data_t *rr_reest_data_ptr = rr_reest_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_reest_data_ptr);

  rr_est_req_valid = rr_reest_data_ptr->rr_est_req_valid;
  rr_reest_data_ptr->rr_est_req_valid = FALSE;

  return rr_est_req_valid;
} /* rr_reest_discard_rr_est_req */

/*!
 * \brief Sends the RR_EST_REQ saved locally to RR-CONTROL by re-saving.
 *
 * \param gas_id
 */
static void rr_reest_send_rr_est_req_to_rr_control(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_reest_data_t *rr_reest_data_ptr = rr_reest_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_reest_data_ptr);

  if (rr_reest_data_ptr->rr_est_req_valid)
  {
    rr_save_rr_est_req(&rr_reest_data_ptr->rr_est_req, gas_id);

    rr_reest_data_ptr->rr_est_req_valid = FALSE;
  }

} /* rr_reest_send_rr_est_req_to_rr_control */

/*!
 * \brief Checks if a RR_EST_REQ for re-establishment (establishment_cause = MM_RR_CALL_REEST) is pending.
 *
 * \param gas_id
 *
 * \return boolean - TRUE if there is; FALSE otherwise
 */
static boolean rr_reest_check_if_nas_reest_req_pending(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_reest_data_t *rr_reest_data_ptr = rr_reest_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_reest_data_ptr);

  /* Look for the expected saved RR_EST_REQ for re-establishment */
  if (rr_reest_data_ptr->rr_est_req_valid &&
      (rr_reest_data_ptr->rr_est_req.message_header.message_set == MS_MM_RR) &&
      (rr_reest_data_ptr->rr_est_req.message_header.message_id == (byte) RR_EST_REQ) &&
      (rr_reest_data_ptr->rr_est_req.establishment_cause == (uint8) MM_RR_CALL_REEST))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* rr_reest_check_if_nas_reest_req_pending */

/*!
 * \brief Checks if a RR_EST_REQ for re-establishment (establishment_cause = MM_RR_CALL_REEST) is pending and
 *        if so, the saved RR_EST_REQ is deleted and RR_ABORT_IND{OTHER_REASON} sent to NAS-MM.
 *
 * \param gas_id
 */
static void rr_reest_abort_nas_reest_req_if_pending(const gas_id_t gas_id)
{
  if (rr_reest_check_if_nas_reest_req_pending(gas_id))
  {
    rr_send_abort_ind(OTHER_REASON, MM_RR_OTHER_FAILURE, gas_id);

    // The RR_EST_REQ is not needed now so discard it
    (void) rr_reest_discard_rr_est_req(gas_id);
  }

} /* rr_reest_abort_nas_reest_req_if_pending */

/*!
 * \brief Prepares the reselection power-scan cell list, which will include upto six neighbour cells plus
 *        the serving cell
 *
 * \param dedicated_meas_ptr - Pointer to dedicated measurement information
 * \param pscan_cell_list_ptr - Pointer to rps cell list to be populated
 * \param gas_id
 *
 * \return uint8 - number of cells available to power-scan
 */
static uint8 prepare_power_scan_cell_list(
  const surrounding_measurement_T *dedicated_meas_ptr,
  rr_reselection_pscan_cell_list_T  *pscan_cell_list_ptr,
  const gas_id_t gas_id
)
{
  uint8 i;
  boolean include_scell_entry = TRUE;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  pscan_cell_list_ptr->no_of_entries = 0;
  pscan_cell_list_ptr->valid_data = FALSE;

  for (i = 0;
       (i < dedicated_meas_ptr->no_of_entries) && (i < MAX_NUMBER_OF_BEST_CELLS);
       i++)
  {
    grr_reselection_pscan_cell_list_add_entry(
      pscan_cell_list_ptr,
      dedicated_meas_ptr->measurement[i].BCCH_ARFCN
    );

    if (ARFCNS_EQUAL(dedicated_meas_ptr->measurement[i].BCCH_ARFCN, scell_info_ptr->gsm.BCCH_ARFCN))
    {
      include_scell_entry = FALSE;
    }
  }

  if (include_scell_entry == TRUE)
  {
    /* Add the current serving cell */
    grr_reselection_pscan_cell_list_add_entry(
      pscan_cell_list_ptr,
      scell_info_ptr->gsm.BCCH_ARFCN
    );
  }

#ifdef FEATURE_GERAN_HST_OPT
  /* Add IRAT acq DB frequency List to the Reselection Power Scan if in a high speed cell*/

  /* Check if it is NOT already present in the existing BA List */
  /* If NOT add it to the Reestablishment Power Scan List */
  if (rr_high_speed_train_opt_allowed(gas_id))
  {  
    ARFCN_T           BCCH_ARFCN;
    rr_irat_acq_db_t *acq_db_ptr;
    uint8             index;

    MSG_GERAN_HIGH_0_G("HST Cells Present During Call Reestablishment Power Scan List Preparation");

    acq_db_ptr = rr_irat_acq_db_get_data_ptr();
    for ( index=0; index < acq_db_ptr->freq_list.num_arfcns; index++ )
    {
      BCCH_ARFCN = acq_db_ptr->freq_list.arfcn[index];
 
      if ( (rr_ds_is_supported_band((rr_internal_band_T)BCCH_ARFCN.band, RR_GAS_ID_TO_AS_ID)) &&
           (FALSE == grr_reselection_pscan_cell_list_check_entry(pscan_cell_list_ptr, BCCH_ARFCN)) )
      {
        grr_reselection_pscan_cell_list_add_entry(pscan_cell_list_ptr, BCCH_ARFCN);
      }
      else
      {
        MSG_GERAN_HIGH_2_G("HST unsupported band %d or ARFCN %d already in list",BCCH_ARFCN.band, BCCH_ARFCN.num);
      }
    }    
  }
  else
  {
    MSG_GERAN_HIGH_0_G("High Speed Train opt disabled");
  }
#endif /*FEATURE_GERAN_HST_OPT*/

  return (uint8) pscan_cell_list_ptr->no_of_entries;

} /* end prepare_power_scan_cell_list() */


/*!
 * \brief Put L1 in GSM Idle following GSM sys info acquisition.
 *
 *  Minimal GSM sys info is received on the chosen cell (e.g. SI2 is not acquired). Put L1 in GSM Idle, and
 *  perform minimal setup required before attempting connection establishment for re-establishment
 */
static void start_gsm_idle(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  (void)rr_pcell_becomes_scell(gas_id);

  rr_gprs_reassign_band(
    CBCH_FREQ | CELL_ALLOC_FREQ_LIST | MOBILE_ALLOC_FREQ_LIST,
    TRUE,
    gas_id
  );

  rr_set_current_band(GET_ARFCN_BAND(scell_info_ptr->gsm.BCCH_ARFCN), gas_id);

  /* There is no neighbour cell information available at the moment */
  rr_gprs_init_gsm_neighbour_cell_list(gas_id);

  /* Start GSM Idle (early camping performed) */
  rr_gprs_start_gsm_idle_mode(scell_info_ptr, TRUE, gas_id);
  scell_info_ptr->l1_idle_mode = GRR_L1_IDLE_MODE_GSM;

  /* RR/L1 are now camped */
  scell_info_ptr->gsm.camped_on_flag = TRUE;

  rr_hz_update_zone_info(RR_HZ_UPDATE_INFO_ONLY, gas_id);
  rr_hz_notify_state_change(RR_HZ_STATE_IDLE, gas_id);

  /* As only partial sys info has been acquired, ensure that a full */
  /* sys info refresh occurs after connection release */
  rr_gprs_invalidate_sys_info_during_cs_conn(RR_SI_INCONSISTENT, gas_id);

  /* Send a RR_SERVICE_IND to MM */
  /* Note: No GPRS sys info will be available */
  grr_update_mm_with_rr_service_ind(gas_id);

  /* Now block further updates to MM and L1 whilst RACHing - these will be unblocked */
  /* once the CS connection is established */
  rr_gprs_sys_info_block_all_updates(gas_id);

  // Initialise L1-IDLE-MODE module, as GSM Idle has been started here without its use
  rr_l1_idle_init(gas_id);

} /* end start_gsm_idle() */

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

// Return the channel information used for the previous connection establishment
uint8 rr_cell_reest_get_channel_info(
  channel_type_T *channel_type_1,
  channel_type_T *channel_type_2,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module API data
  rr_reest_api_data_t *rr_reest_api_data_ptr = rr_reest_api_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_reest_api_data_ptr);

  (* channel_type_1) = rr_reest_api_data_ptr->channel_type_1;
  (* channel_type_2) = rr_reest_api_data_ptr->channel_type_2;

  return rr_reest_api_data_ptr->num_of_ded_channels;

} /* end rr_cell_reest_get_channel_info() */

// Store the current channel information to be used later for call re-establishment
void rr_cell_reest_rlf_occurred(
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T *dedicated_data_ptr;

  // Obtain a pointer to the module data
  rr_reest_api_data_t *rr_reest_api_data_ptr = rr_reest_api_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_reest_api_data_ptr);

  dedicated_data_ptr = rr_get_dedicated_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(dedicated_data_ptr);

  rr_reest_api_data_ptr->num_of_ded_channels =
    dedicated_data_ptr->curr_channel_spec.number_of_ded_channels;
  rr_reest_api_data_ptr->channel_type_1 =
    dedicated_data_ptr->curr_channel_spec.channel_info_1_after.channel_type;
  rr_reest_api_data_ptr->channel_type_2 =
    dedicated_data_ptr->curr_channel_spec.channel_info_2_after.channel_type;

} /* end rr_cell_reest_rlf_occurred() */

// RR-CELL-REESTABLISH state machine
rr_event_T rr_cell_reest_control(
  rr_event_T event_in,
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  byte message_id;
  byte message_set;
  rr_reest_state_e rr_reest_state;
  rr_reest_data_t *rr_reest_data_ptr = NULL;
  rr_event_T event_out = EV_NO_EVENT;
  rr_event_T new_event = EV_NO_EVENT;

  // Populate message_set & message_id (if applicable)
  event_in = rr_extract_message_header_details(
    event_in,
    message_ptr,
    &message_set,
    &message_id
  );

  // Perform processing of any events/messages that can be received while the module is INACTIVE
  // (i.e. module data has not been allocated)

  // This will return RR_REEST_NULL if the module is inactive
  rr_reest_state = rr_reest_get_state(gas_id);

  switch (rr_reest_state)
  {
    // -----------------------------------  RR_REEST_NULL  ------------------------------------
    //
    // Initial state

    case RR_REEST_NULL:
    {
      switch (event_in)
      {
        case EV_CHOOSE_CELL_FOR_REEST:
        case EV_CHOOSE_CELL_FOR_CAMP:
        {
          // Allocate and initialise the run-time data for this module
          rr_reest_data_ptr = rr_reest_alloc_and_init_data(gas_id);
          break;
        }

        case EV_CONNECTION_EXISTS:
        {
          /*
          Allow sys info updates to L1/MM, which would have been blocked during RACHing
          for Call Reestablishment.
          Note: This will have no effect if the Connection Establishment was for another
          reason
          */
          rr_gprs_sys_info_allow_all_updates(gas_id);
          break;
        }

        default: ;
      }

      break;
    }

    default:
    {
      rr_reest_data_ptr = rr_reest_get_data_ptr(gas_id);
    }
  }

  // Is module now active ?
  if (rr_reest_data_ptr == NULL)
  {
    return EV_NO_EVENT;
  }

  // Perform processing of any events/messages that can be received while the module is ACTIVE
  // (i.e. module data has been allocated)

  if (event_in == EV_RESET_SOFTWARE)
  {
    rr_reest_free_data(gas_id);

    // Exit early as there is no module data
    return EV_NO_EVENT;
  }

  while (event_in != EV_NO_EVENT)
  {
    switch (rr_reest_state)
    {
      // -----------------------------------  RR_REEST_NULL  ------------------------------------
      //
      // Initial state

      case RR_REEST_NULL:
      {
        switch (event_in)
        {
          case EV_CHOOSE_CELL_FOR_REEST:
          case EV_CHOOSE_CELL_FOR_CAMP:
          {
            uint8                      num_of_candidates;
            surrounding_measurement_T *surrounding_measurement_ptr;

            MSG_GERAN_HIGH_0_G("Attempt to regain service on known cells");

            /*
            populate the power-scan cell list with the strongest neighbours and
            the serving cell
            */
            surrounding_measurement_ptr = rr_get_surrounding_measurement_ptr(gas_id);

            num_of_candidates = prepare_power_scan_cell_list(
              surrounding_measurement_ptr,
              &rr_reest_data_ptr->pscan_cell_list,
              gas_id
            );

            if (num_of_candidates == 0)
            {
              MSG_GERAN_HIGH_0_G("Procedure failed - no candidates");

              event_out = EV_CHOOSE_CELL;
            }
            else
            {
              MSG_GERAN_MED_1_G("Stored=%d freqs in pscan cell list",(int)num_of_candidates);

              if ((event_in == EV_CHOOSE_CELL_FOR_REEST) &&
                  rr_reest_is_channel_config_ok_for_reest(gas_id))
              {
                rr_select_bcch_send_imsg_pscan_list_select_req(
                  rr_reest_data_ptr->pscan_cell_list.cell_list,
                  rr_reest_data_ptr->pscan_cell_list.no_of_entries,
                  TRUE,
                  rr_reest_cell_suitable_reest_cb_fn,
                  MASK_SI_NONE,
                  MASK_SI_1 | MASK_SI_3 | MASK_SI_4,
                  GRR_TIMEOUT_1_SECOND * 20,           // 20 sec time-out
                  gas_id
                );

                rr_reest_data_ptr->state = RR_REEST_SELECT_CELL_REEST;
              }
              else
              {
                uint32 acq_timeout;

#ifdef FEATURE_LTE
                if(event_in == EV_CHOOSE_CELL_FOR_REEST)
                {
                  if ( rr_connection_release_get_g2l_blind_redir_flag(gas_id) ||
                       rr_connection_release_get_g2l_blind_redir_after_csfb_flag(gas_id))
                  {
                     MSG_GERAN_HIGH_0_G("EV_G2L_BLIND_REDIR_REQUIRED is generated");
                     event_out = EV_G2L_BLIND_REDIR_REQUIRED;
                     rr_reest_data_ptr->state = RR_REEST_NULL;
                     break;
                  }
                }
#endif /* FEATURE_LTE */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                if (rr_multi_sim_mode_enabled())
                {
                  /* in multi-SIM mode, guard the process with the cell selection abort timer */
                  acq_timeout = GRR_TIMEOUT_1_SECOND * 20;
                  MSG_GERAN_HIGH_0_G("Setting cell selection abort timer for reestablishment");
                }
                else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
                {
                  /* default is to use no cell selection timeout */
                  acq_timeout = 0;
                }

                rr_select_bcch_send_imsg_pscan_list_select_req(
                  rr_reest_data_ptr->pscan_cell_list.cell_list,
                  rr_reest_data_ptr->pscan_cell_list.no_of_entries,
                  TRUE,
                  rr_reest_cell_suitable_camp_cb_fn,
                  MASK_SI_NONE,
                  MASK_SI_NONE,                        // Select the cell only
                  acq_timeout,
                  gas_id
                );

                rr_reest_data_ptr->state = RR_REEST_SELECT_CELL_CAMP;
              }

              event_out = RR_EV_CELL_REEST_TRIGGERED;
            }
            break;
          }

          default: MSG_GERAN_ERROR_2_G("Unex.EV=%d in State=%d",(int)event_in,(int)rr_reest_data_ptr->state);
        }

        break;
      }

      // -----------------------------  RR_REEST_SELECT_CELL_REEST  -------------------------------
      //
      // A cell is being chosen to allow a re-establishment attempt to proceed.
      // The RR-BCCH-SELECT module is currently selecting it.

      case RR_REEST_SELECT_CELL_REEST:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message_ptr == NULL)
            {
              MSG_GERAN_ERROR_0_G("message_ptr == NULL");
              break;
            }

            switch (message_ptr->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_SELECT_BCCH_OMSG:
                  {
                    switch (message_ptr->rr.select_bcch.omsg.header.imh.message_id)
                    {
                      case RR_SELECT_BCCH_OMSG_SELECT_CNF:
                      {
                        rr_select_bcch_omsg_select_cnf_t *omsg_select_cnf_ptr =
                          &message_ptr->rr.select_bcch.omsg.rr_select_bcch_omsg_select_cnf;

                        MSG_GERAN_HIGH_0_G("Minimal GSM sys info received");

                        if (rr_reest_check_if_nas_reest_req_pending(gas_id))
                        {
                          /*
                          Start GSM Idle Mode in L1
                          Note: As early camping has been performed, no L1 state transition is required
                          and a MPH_START_IDLE_MODE_CNF is not expected
                          */
                          start_gsm_idle(gas_id);
#ifdef FEATURE_IDLE_DRX_SCALING
                          rr_idle_drx_control(RR_EV_CAMPED_ON_CELL, NULL, gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */

                          // Now send the saved RR_EST_REQ into RR-CONTROL
                          rr_reest_send_rr_est_req_to_rr_control(gas_id);

                          event_out = RR_EV_CAMPED_ON_CELL;
                          rr_reest_data_ptr->state = RR_REEST_NULL;
                        }
                        else
                        {
                          rr_candidate_cell_T *candidate_cell_ptr;

                          MSG_GERAN_HIGH_0_G("Procedure failed - no RR_EST_REQ from NAS");
#ifdef FEATURE_LTE						  
                          if ( rr_connection_release_get_g2l_blind_redir_flag(gas_id) || 
                               rr_connection_release_get_g2l_blind_redir_after_csfb_flag(gas_id))                               
                          {
                            MSG_GERAN_HIGH_0_G("EV_G2L_BLIND_REDIR_REQUIRED is generated");
                            event_out = EV_G2L_BLIND_REDIR_REQUIRED;
                            rr_reest_data_ptr->state = RR_REEST_NULL;
                            break;
                          }
#endif

                          // Find the candidate cell which was tried
                          rr_reest_data_ptr->candidate_cell_ptr = rr_candidate_db_get_entry(
                            rr_get_candidate_cell_db_ptr(gas_id),
                            omsg_select_cnf_ptr->arfcn
                          );

                          // This will return a pointer to the candidate cell or NULL if none were suitable
                          candidate_cell_ptr = rr_reest_find_cell_suitable_for_idle_camp(gas_id);

                          if (candidate_cell_ptr != NULL)
                          {
                            // Check if the candidate selected is the same as before
                            if (candidate_cell_ptr == rr_reest_data_ptr->candidate_cell_ptr)
                            {
                              MSG_GERAN_HIGH_0_G("Same cell chosen for normal camping");

                              /**
                               * This chosen cell is already selected and some SIs have been acquired. 
                               * Return to RR-CONTROL to acquire the remaining SIs. 
                               * Note: L1 is already Early Camped (done in RR-SELECT-BCCH module) 
                               */
                              event_out = EV_CELL_CHOSEN_EARLY_CAMPED;
                              rr_reest_data_ptr->state = RR_REEST_NULL;
                            }
                            else
                            {
                              MSG_GERAN_HIGH_0_G("Different cell chosen for normal camping");

                              // A different cell has been chosen - select it
                              rr_reest_select_bcch_for_idle_camp(candidate_cell_ptr, gas_id);

                              // Wait for confirmation that the cell has been selected
                              rr_reest_data_ptr->state = RR_REEST_SELECT_CELL_CAMP;
                            }
                          }
                          else
                          {
                            // Trigger cell selection from RR-CONTROL
                            event_out = EV_NO_BCCH_FOUND;
                            rr_reest_data_ptr->state = RR_REEST_NULL;
                          }
                        }
                        break;
                      }

                      case RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND:
                      {
                        rr_candidate_cell_T *candidate_cell_ptr;

                        rr_select_bcch_omsg_select_fail_ind_t *omsg_select_fail_ind_ptr =
                          &message_ptr->rr.select_bcch.omsg.rr_select_bcch_omsg_select_fail_ind;

                        MSG_GERAN_HIGH_3_G(
                          "Select procedure failed (arfcn=%d reason=%d early_camped=%d)",
                          omsg_select_fail_ind_ptr->arfcn.num,
                          omsg_select_fail_ind_ptr->reason,
                          omsg_select_fail_ind_ptr->early_camped
                        );

                        // Abort any pending RR_EST_REQ (for re-establishment) from NAS
                        rr_reest_abort_nas_reest_req_if_pending(gas_id);
#ifdef FEATURE_LTE
                        if ( rr_connection_release_get_g2l_blind_redir_flag(gas_id) || 
                             rr_connection_release_get_g2l_blind_redir_after_csfb_flag(gas_id)) 
                        {
                          MSG_GERAN_HIGH_0_G("EV_G2L_BLIND_REDIR_REQUIRED is generated");
                          rr_reest_data_ptr->candidate_cell_ptr = NULL;
                          rr_reest_data_ptr->state = RR_REEST_NULL;
                          event_out = EV_G2L_BLIND_REDIR_REQUIRED;
                          break;
                        }
#endif

                        if (omsg_select_fail_ind_ptr->arfcn.num != INVALID_ARFCN)
                        {
                          // Find the candidate cell which was selected
                          rr_reest_data_ptr->candidate_cell_ptr = rr_candidate_db_get_entry(
                            rr_get_candidate_cell_db_ptr(gas_id),
                            omsg_select_fail_ind_ptr->arfcn
                          );
                        }
                        else
                        {
                          // No cell was selected
                          rr_reest_data_ptr->candidate_cell_ptr = NULL;
                        }

                        // This will return a pointer to the candidate cell or NULL if none were suitable
                        candidate_cell_ptr = rr_reest_find_cell_suitable_for_idle_camp(gas_id);

                        /**
                         * Note: If the cell could not be selected due to SI acquisition failure, then the 
                         * candidate cell database entry will have been updated as to mark this cell as 
                         * RR_CANDIDATE_BCCH_WITH_DECODE_FAILURES. This means that 
                         * rr_reest_find_cell_suitable_for_idle_camp() will not find choose this cell. 
                         */

                        if (candidate_cell_ptr != NULL)
                        {
                          // Check if the candidate selected is the same as before
                          if (candidate_cell_ptr == rr_reest_data_ptr->candidate_cell_ptr)
                          {
                            MSG_GERAN_HIGH_0_G("Same cell chosen for normal camping");

                            /**
                             * This chosen cell is already selected and some SIs have been acquired. 
                             * Return to RR-CONTROL to acquire the remaining SIs. 
                             * Note: L1 may or may not be Early Camped (done in RR-SELECT-BCCH module) as it 
                             * depends why the procedure failed.
                             */
                            if (omsg_select_fail_ind_ptr->early_camped)
                            {
                              event_out = EV_CELL_CHOSEN_EARLY_CAMPED;
                            }
                            else
                            {
                              event_out = EV_CELL_CHOSEN;
                            }

                            rr_reest_data_ptr->state = RR_REEST_NULL;
                          }
                          else
                          {
                            MSG_GERAN_HIGH_0_G("Different cell chosen for normal camping");

                            // A different cell has been chosen - select it
                            rr_reest_select_bcch_for_idle_camp(candidate_cell_ptr, gas_id);

                            // Wait for confirmation that the cell is selected
                            rr_reest_data_ptr->state = RR_REEST_SELECT_CELL_CAMP;
                          }
                        }
                        else
                        {
                          // Trigger cell selection from RR-CONTROL
                          event_out = EV_NO_BCCH_FOUND;
                          rr_reest_data_ptr->state = RR_REEST_NULL;
                        }

                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message_ptr,
                          rr_reest_state_name(rr_reest_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    rr_select_bcch_control(message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_MM_RR:
              {
                switch (message_id)
                {
                  case RR_EST_REQ:
                  {
                    rr_est_req_T *rr_est_req = &message_ptr->mm.rr_est_req;

                    // Save the message for processing once RR-CELL-REESTABLISH exits
                    rr_reest_save_rr_est_req(&message_ptr->mm.rr_est_req, gas_id);

                    /* A RR_EST_REQ for reestablishment is expected */
                    if (rr_est_req->establishment_cause == (uint8) MM_RR_CALL_REEST)
                    {
                      MSG_GERAN_HIGH_0_G("RR_EST_REQ received for call re-establishment");
                    }
                    /* If another cause is expected, then abort the re-establishment, but */
                    /* also abort the RR_EST_REQ, as RR may need to go out-of-service */
                    else
                    {
                      MSG_GERAN_HIGH_0_G("RR_EST_REQ received but not for call re-establishment");
                      rr_select_bcch_send_imsg_abort_req(gas_id);
#ifdef FEATURE_LTE
                      if ( rr_connection_release_get_g2l_blind_redir_flag(gas_id) || 
                           rr_connection_release_get_g2l_blind_redir_after_csfb_flag(gas_id))                           
                      {
                        /* Abort pending RR_EST_REQ as we are going to perform blind redirection */
                        rr_reest_discard_rr_est_req(gas_id);
                        rr_send_abort_ind(OTHER_REASON, MM_RR_OTHER_FAILURE, gas_id);

                        MSG_GERAN_HIGH_0_G("State changed to RR_REEST_ABORT_THEN_EXIT for blind redirection");
                        rr_reest_data_ptr->state = RR_REEST_ABORT_THEN_EXIT;
                      }
                      else
#endif
                      {
                        rr_reest_data_ptr->state = RR_REEST_ABORT_THEN_CAMP;
                      }
                    } 
                    break;
                  }

                  case RR_ABORT_REQ:
                  {
                    // If there is a RR_EST_REQ saved, discard it
                    (void) rr_reest_discard_rr_est_req(gas_id);

                    rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);
                    rr_select_bcch_send_imsg_abort_req(gas_id);

#ifdef FEATURE_LTE
                    if ( rr_connection_release_get_g2l_blind_redir_flag(gas_id) || 
                         rr_connection_release_get_g2l_blind_redir_after_csfb_flag(gas_id)) 
                    {
                      MSG_GERAN_HIGH_0_G("State changed to RR_REEST_ABORT_THEN_EXIT for blind redirection");
                      rr_reest_data_ptr->state = RR_REEST_ABORT_THEN_EXIT;
                    }
                    else
#endif
                    {
                      rr_reest_data_ptr->state = RR_REEST_ABORT_THEN_CAMP;
                    }
                    break;
                  }


                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_reest_state_name(rr_reest_data_ptr->state)
                    );
                  }
                }
                break;
              }

              default:
              {
                rr_select_bcch_control(message_ptr, gas_id);
              }
            }
            break;
          }

          case RR_EV_MM_REQ_RECEIVED_ABORT:
          {
            // Abort the ongoing search for a suitable cell and then exit
            rr_select_bcch_send_imsg_abort_req(gas_id);
            rr_reest_data_ptr->state = RR_REEST_ABORT_THEN_EXIT;
            break;
          }

          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          {
            break;
          }

          default: MSG_GERAN_ERROR_2_G("Unex.EV=%d in State=%d",(int)event_in,(int)rr_reest_data_ptr->state);
        }

        break;
      }

      // ------------------------------  RR_REEST_SELECT_CELL_CAMP  -------------------------------
      //
      // The re-establishment attempt was either not possible or was stopped, and a cell is being chosen
      // for normal camping. The RR-BCCH-SELECT module is currently selecting it.

      case RR_REEST_SELECT_CELL_CAMP:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message_ptr == NULL)
            {
              MSG_GERAN_ERROR_0_G("message_ptr == NULL");
              break;
            }

            switch (message_ptr->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_SELECT_BCCH_OMSG:
                  {
                    switch (message_ptr->rr.select_bcch.omsg.header.imh.message_id)
                    {
                      case RR_SELECT_BCCH_OMSG_SELECT_CNF:
                      {
                        rr_candidate_cell_T *candidate_cell_ptr;

                        rr_select_bcch_omsg_select_cnf_t *omsg_select_cnf_ptr =
                          &message_ptr->rr.select_bcch.omsg.rr_select_bcch_omsg_select_cnf;

                        // Abort any pending RR_EST_REQ (for re-establishment) from NAS
                        rr_reest_abort_nas_reest_req_if_pending(gas_id);

                        // If there is a saved RR_EST_REQ (for another reason) send this into RR-CONTROL
                        rr_reest_send_rr_est_req_to_rr_control(gas_id);

                        /**
                         * This chosen cell has been selected but no Early Camping has been done. 
                         * The Early Camping data is available for this candidate cell as SI3 was previously 
                         * received when determining suitability. 
                         */

                        // Find the candidate cell which was selected
                        candidate_cell_ptr = rr_candidate_db_get_entry(
                          rr_get_candidate_cell_db_ptr(gas_id),
                          omsg_select_cnf_ptr->arfcn
                        );

                        if (candidate_cell_ptr != NULL)
                        {
                          gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
                          RR_NULL_CHECK_FATAL(pcell_info_ptr);

                          /**
                           * Copy the Control Channel Description data, received earlier when SI3 was 
                           * acquired, over to the pcell data store to enable Early Camping to be done in 
                           * RR-CONTROL.
                           */
                          pcell_info_ptr->gsm.control_channel_description_valid =
                            candidate_cell_ptr->si_info.control_channel_description_valid;
                          pcell_info_ptr->gsm.control_channel_description =
                            candidate_cell_ptr->si_info.control_channel_description;
                        }
                        else
                        {
                          MSG_GERAN_ERROR_0_G("candidate_cell_ptr==NULL");
                        }

                        event_out = EV_CELL_CHOSEN;
                        rr_reest_data_ptr->state = RR_REEST_NULL;
                        break;
                      }

                      case RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND:
                      {
                        // Abort any pending RR_EST_REQ (for re-establishment) from NAS
                        rr_reest_abort_nas_reest_req_if_pending(gas_id);

                        // If there is a saved RR_EST_REQ (for another reason) send this into RR-CONTROL
                        rr_reest_send_rr_est_req_to_rr_control(gas_id);

                        MSG_GERAN_HIGH_0_G("Failed to select cell - trigger cell selection");

                        rr_reest_data_ptr->state = RR_REEST_NULL;
                        event_out = EV_NO_BCCH_FOUND;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message_ptr,
                          rr_reest_state_name(rr_reest_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    rr_select_bcch_control(message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_MM_RR:
              {
                switch (message_id)
                {
                  case RR_EST_REQ:
                  {
                    rr_est_req_T *rr_est_req = &message_ptr->mm.rr_est_req;

                    /* A RR_EST_REQ for reestablishment is expected */
                    if (rr_est_req->establishment_cause == (uint8) MM_RR_CALL_REEST)
                    {
                      MSG_GERAN_HIGH_0_G("RR_EST_REQ received for call re-establishment");

                      // Abort this establishment request, as re-establishment not possible
                      rr_send_abort_ind(OTHER_REASON, MM_RR_OTHER_FAILURE, gas_id);
                    }
                    /* If another cause is expected, then abort the re-establishment, but */
                    /* also abort the RR_EST_REQ, as RR may need to go out-of-service */
                    else
                    {
                      MSG_GERAN_HIGH_0_G("RR_EST_REQ received but not for call re-establishment");

                      /* save the message for later processing */
                      rr_reest_save_rr_est_req(&message_ptr->mm.rr_est_req, gas_id);
                    }
                    break;
                  }

                  case RR_ABORT_REQ:
                  {
                    // If there is a RR_EST_REQ saved, discard it
                    (void) rr_reest_discard_rr_est_req(gas_id);

                    rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_reest_state_name(rr_reest_data_ptr->state)
                    );
                  }
                }
                break;
              }

              default:
              {
                rr_select_bcch_control(message_ptr, gas_id);
              }
            }
            break;
          }

          case RR_EV_MM_REQ_RECEIVED_ABORT:
          {
            // If there is a RR_EST_REQ saved, discard it */
            if (rr_reest_discard_rr_est_req(gas_id))
            {
              rr_send_abort_ind(OTHER_REASON, MM_RR_OTHER_FAILURE, gas_id);
            }

            // Abort the ongoing search for a suitable cell and then exit
            rr_select_bcch_send_imsg_abort_req(gas_id);
            rr_reest_data_ptr->state = RR_REEST_ABORT_THEN_EXIT;
            break;
          }

          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          {
            break;
          }

          default: MSG_GERAN_ERROR_2_G("Unex.EV=%d in State=%d",(int)event_in,(int)rr_reest_data_ptr->state);
        }
        break;
      }

      // -------------------------------  RR_REEST_ABORT_THEN_CAMP  -------------------------------
      //
      // Re-establishment is no longer required due to
      // a) NAS sent RR_EST_REQ for another establishment cause (not MM_RR_REEST)
      // b) NAS had sent RR_EST_REQ{MM_RR_REEST} but then sent RR_ABORT_REQ
      //
      // Wait for the RR-SELECT-BCCH module to abort, and then find another cell from the original
      // cell list to camp onto. It is not known at what stage the previous selection of the cell
      // reached, so none of the previous data (e.g. power-scan data, candidate cell information, etc)
      // can be relied upon. Start again to find a cell to camp onto

      case RR_REEST_ABORT_THEN_CAMP:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message_ptr == NULL)
            {
              MSG_GERAN_ERROR_0_G("message_ptr == NULL");
              break;
            }

            switch (message_ptr->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_SELECT_BCCH_OMSG:
                  {
                    switch (message_ptr->rr.select_bcch.omsg.header.imh.message_id)
                    {
                      case RR_SELECT_BCCH_OMSG_SELECT_CNF:
                      case RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND:
                      {
                        rr_select_bcch_send_imsg_pscan_list_select_req(
                          rr_reest_data_ptr->pscan_cell_list.cell_list,
                          rr_reest_data_ptr->pscan_cell_list.no_of_entries,
                          TRUE,
                          rr_reest_cell_suitable_camp_cb_fn,
                          MASK_SI_NONE,
                          MASK_SI_NONE,                        // Select the cell only
                          0,                                   // No time-out
                          gas_id
                        );

                        rr_reest_data_ptr->state = RR_REEST_SELECT_CELL_CAMP;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message_ptr,
                          rr_reest_state_name(rr_reest_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    rr_select_bcch_control(message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_MM_RR:
              {
                switch (message_id)
                {
                  case RR_EST_REQ:
                  {
                    // Save the message for processing when camped
                    rr_reest_save_rr_est_req(&message_ptr->mm.rr_est_req, gas_id);
                    break;
                  }

                  case RR_ABORT_REQ:
                  {
                    // If there is a RR_EST_REQ saved, discard it
                    (void) rr_reest_discard_rr_est_req(gas_id);

                    rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_reest_state_name(rr_reest_data_ptr->state)
                    );
                  }
                }
                break;
              }

              default:
              {
                rr_select_bcch_control(message_ptr, gas_id);
              }
            }
            break;
          }

          case RR_EV_MM_REQ_RECEIVED_ABORT:
          {
            // Already aborting - need to exit when complete
            rr_reest_data_ptr->state = RR_REEST_ABORT_THEN_EXIT;
            break;
          }

          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          {
            /* No action necessary. */
            break;
          } /* RR_EV_CANDIDATE_CELL_SI_RECEIVED */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event_in), "ABORT_THEN_CAMP");
          }
        }
        break;
      }

      // -------------------------------  RR_REEST_ABORT_THEN_EXIT  -------------------------------
      //
      // Re-establishment has been aborted due to
      // a) NAS sent RR_PLMN_SELECT_REQ / RR_PLMN_LIST_REQ / RR_STOP_GSM_MODE_REQ / RR_DEACT_REQ
      //
      // Wait for the RR-SELECT-BCCH module to abort, and then exit

      case RR_REEST_ABORT_THEN_EXIT:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message_ptr == NULL)
            {
              MSG_GERAN_ERROR_0_G("message_ptr == NULL");
              break;
            }

            switch (message_ptr->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_SELECT_BCCH_OMSG:
                  {
                    switch (message_ptr->rr.select_bcch.omsg.header.imh.message_id)
                    {
                      case RR_SELECT_BCCH_OMSG_SELECT_CNF:
                      case RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND:
                      {
#ifdef FEATURE_LTE
                        if ( rr_connection_release_get_g2l_blind_redir_flag(gas_id) || 
                             rr_connection_release_get_g2l_blind_redir_after_csfb_flag(gas_id)) 
                        {                      
                          MSG_GERAN_HIGH_0_G("EV_G2L_BLIND_REDIR_REQUIRED is generated");
                          event_out = EV_G2L_BLIND_REDIR_REQUIRED;
                        }
                        else
#endif /* FEATURE_LTE */
                        {
                          event_out = EV_NO_BCCH_FOUND;
                        }
                        rr_reest_data_ptr->state = RR_REEST_NULL;
                        break;
                      }
                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message_ptr,
                          rr_reest_state_name(rr_reest_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    rr_select_bcch_control(message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_MM_RR:
              {
                switch (message_id)
                {
                  case RR_EST_REQ:
                  {
                    // Save the message for processing when camped
                    rr_reest_save_rr_est_req(&message_ptr->mm.rr_est_req, gas_id);
                    break;
                  }

                  case RR_ABORT_REQ:
                  {
                    // If there is a RR_EST_REQ saved, discard it
                    (void) rr_reest_discard_rr_est_req(gas_id);

                    rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_reest_state_name(rr_reest_data_ptr->state)
                    );
                  }
                }
                break;
              }

              default:
              {
                rr_select_bcch_control(message_ptr, gas_id);
              }
            }
            break;
          }

          case RR_EV_MM_REQ_RECEIVED_ABORT:
          {
            // Already aborting - about to exit anyway
            break;
          }

          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          {
            /* No action necessary. */
            break;
          } /* RR_EV_CANDIDATE_CELL_SI_RECEIVED */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event_in), "ABORT_THEN_EXIT");
          }
        }
        break;
      }

      default: ERR_GERAN_FATAL_1_G("Unknown state (%d)",(int)rr_reest_data_ptr->state);
    }

    #ifdef DEBUG_RR_TRACE_MSG
    if ( ((event_in != new_event) && (new_event != EV_NO_EVENT)) ||
         (rr_reest_data_ptr->old_state != rr_reest_data_ptr->state))
    {
      rr_store_trace_msg_buf(
        RR_REEST_CELL_SM,
        new_event,
        (byte) rr_reest_data_ptr->state, message_ptr,
        gas_id
      );
    }
    #endif /* DEBUG_RR_TRACE_MSG */

    if (new_event != EV_NO_EVENT)
    {
      event_in = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event_in = EV_NO_EVENT;
    }

    if (rr_reest_data_ptr->old_state != rr_reest_data_ptr->state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_cell_reest_control",
        rr_reest_data_ptr->old_state,
        rr_reest_data_ptr->state,
        rr_reest_state_name,
        gas_id
      );

      rr_reest_data_ptr->old_state = rr_reest_data_ptr->state;
    }

  } /* while (event_in != EV_NO_EVENT) */

  // If exiting RR-REEST, free any allocated data
  if ( (event_out == RR_EV_CAMPED_ON_CELL) ||
       (event_out == EV_NO_BCCH_FOUND) ||
       (event_out == EV_CELL_CHOSEN) )
  {
    rr_reest_free_data(gas_id);
  }

  return event_out;

} /* end rr_cell_reest_control() */

/*!
 * \brief Initialisation function. Called once at task start.
 */
void rr_cell_reest_task_start_init(
  void
)
{
  // Zero the control/data structure(s)
  memset(rr_reest_data_ptrs, 0, sizeof(rr_reest_data_ptrs));
  memset(rr_reest_api_data, 0, sizeof(rr_reest_api_data));

  // Setup any specific values not setup by the general module initialisation function

} /* rr_cell_reest_init */

/* EOF */

