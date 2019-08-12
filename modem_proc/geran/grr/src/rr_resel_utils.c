/*============================================================================
  FILE:         rr_resel_utils.c

  OVERVIEW:     RR reselection related utility functions.

  DEPENDENCIES: None

                Copyright (c) 2009-2014 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_resel_utils.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_resel_utils.h"
#include "rr_resel.h"
#include "rr_general.h"
#include "rr_shared_cell_selection.h"
#include "rr_l1_send.h"
#include "rr_sys_info_ncell.h"
#include "stringl/stringl.h"
#include "rr_log.h"
#include "rr_multi_sim.h"
#include "rr_sys_info_cache.h"
#include "rr_defs.h"

/*----------------------------------------------------------------------------
 * Local Defines
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Local Types
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Public Variables
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Local Variables
 *--------------------------------------------------------------------------*/

typedef struct
{
  ARFCN_T sync_cell_arfcn[MAX_NUMBER_OF_BEST_CELLS];
  uint8   no_of_sync_cells;
} sync_cell_data_t;

static sync_cell_data_t sync_cell_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return sync_cell_data_t*
 */
static sync_cell_data_t *sync_cell_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &sync_cell_data[as_index];
  }

/*----------------------------------------------------------------------------
 * Externalised Function Definitions
 *--------------------------------------------------------------------------*/

/****************************************************************************
 *   Function name:  rr_gprs_init_best_six
 *   --------------
 *
 *   Description: Resets the best six back to zero, and cleans it out.
 *   -----------
 *
 *   Parameters: void
 *   -----------
 *
 *   Returns:  void
 *   --------
 *
 ****************************************************************************/
void rr_gprs_init_best_six(
  const gas_id_t gas_id
)
{
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  best_six_ncells_ptr->no_of_entries = 0;
}

/****************************************************************************
 *   Function name:  rr_clear_all_best_six_snr_markers
 *   --------------
 *
 *   Description: sets the snr markers to zero.Cleans out the best six database. Stops any timers and
 *   -----------
 *
 *   Parameters: none
 *   -----------
 *
 *   Returns:  None
 *   --------
 *
 ****************************************************************************/
void rr_clear_all_best_six_snr_markers(
  const gas_id_t gas_id
)
{
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
  uint8              i;

  /* Find the entry in the best six list */
  for (i = 0; i < best_six_ncells_ptr->no_of_entries; i++)
  {
    /* kill any timers and deallocate them */
    if (best_six_ncells_ptr->ncell_entry[i].ncell_data->bcch_or_pmo == BCCH_FORMAT)
    {
      bcch_ncell_info_t *bcch_data_ptr = &best_six_ncells_ptr->ncell_entry[i].ncell_data->ncell_data.bcch;

      bcch_data_ptr->slow_snr_candidate_marker = 0;
      bcch_data_ptr->fast_snr_candidate_marker = 0;
    }
  }

}

/****************************************************************************
 *   Function name:  rr_gprs_change_bcch_best_six_bsic
 *   --------------
 *
 *   Description: For bcch ncells it is possible to get a change of bsic as
 *   -----------  the bsic to monitor isn't specified. If this happens then
 *                the cell is treated as a new entry to the best 6 list.
 *                Timers are stopped and system info is invalidated. This will
 *                be re-acquired as a result.
 *
 *   Parameters: arfcn new_bsic
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_change_bcch_best_six_bsic(
  ARFCN_T arfcn,
  BSIC_T new_bsic,
  const gas_id_t gas_id
)
{
  rr_resel_candidate_t * resel_candidate = rr_resel_best_six_find(arfcn, NULL, gas_id);

  if (resel_candidate && resel_candidate->ncell_data->bcch_or_pmo == BCCH_FORMAT)
  {
    /* change the entry to the new BSIC */
    resel_candidate->ncell_data->bsic = new_bsic;
    resel_candidate->ncell_data->ncell_data.bcch.gsm.BSIC = new_bsic;

    /* Invalidate the data */
    resel_candidate->ncell_data->ncell_data.bcch.gsm.valid_data_flag = FALSE;
    resel_candidate->ncell_data->ncell_data.bcch.gprs.si13_data_valid_flag = FALSE;
    resel_candidate->ncell_data->ncell_data.bcch.system_info_timer_count = UNINITIALISED_SYS_INFO_COUNT;

    (void)rr_timer_cancel(resel_candidate->penalty_timer_id, gas_id);
    (void)rr_timer_cancel(resel_candidate->reselection_info.five_second_timer_id, gas_id);

    if (match_waiting_for_surround_response(arfcn, gas_id))
    {
      clear_waiting_for_surround_response(gas_id);
    }
  }

  /* diff BSIC detected on same channel, flush the cache for the old cell if identity is known otherwise
     flush for entire channel*/
  if (resel_candidate && resel_candidate->ncell_data->bcch_or_pmo == BCCH_FORMAT &&
      resel_candidate->ncell_data->ncell_data.bcch.gsm.cell_id_valid)
  {
    rr_csi_flush_data(&(resel_candidate->ncell_data->ncell_data.bcch.gsm.location_area_identification),
             (uint16*)&(resel_candidate->ncell_data->ncell_data.bcch.gsm.cell_id),
                       CSI_FLUSH_DIFF_CELL_ON_ARFCN,
                       gas_id);
  }
  else
  {
    rr_csi_flush_arfcn_data(arfcn,
                            CSI_FLUSH_DIFF_CELL_ON_ARFCN,
                            gas_id);
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_get_best_six_by_arfcn
 *   --------------
 *
 *   Description: Gets an entry in the best six list and returns a pointer to it
 *   -----------
 *
 *   Parameters: arfcn bsic(may or may not be specified)
 *   -----------
 *
 *   Returns:  gsm_ncell_list_member_t *
 *   --------
 *
 ****************************************************************************/
gsm_ncell_list_member_t * rr_gprs_get_best_six_by_arfcn(
  ARFCN_T arfcn,
  BSIC_T * bsic,
  const gas_id_t gas_id
)
{
  uint8 i;
  gsm_ncell_list_member_t *best_six_entry = NULL;
  best_six_ncells_t       *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  for (i = 0; i < best_six_ncells_ptr->no_of_entries; i++)
  {
    if (ARFCNS_EQUAL(best_six_ncells_ptr->ncell_entry[i].ncell_data->arfcn, arfcn))
    {
      if (bsic)
      {
        // test for matching bsic's
        if ((best_six_ncells_ptr->ncell_entry[i].ncell_data->bsic.PLMN_colour_code == bsic->PLMN_colour_code) &&
            (best_six_ncells_ptr->ncell_entry[i].ncell_data->bsic.BS_colour_code == bsic->BS_colour_code))
        {
          best_six_entry = best_six_ncells_ptr->ncell_entry[i].ncell_data;
          break;
        }
      }
      else
      {
        best_six_entry = best_six_ncells_ptr->ncell_entry[i].ncell_data;
        break;
      }
    }
  }

  return best_six_entry;
}


rr_resel_candidate_t * rr_resel_best_six_find(
  ARFCN_T arfcn,
  BSIC_T * bsic,
  const gas_id_t gas_id
)
{
  uint32 i;
  rr_resel_candidate_t * rr_resel_candidate = NULL;
  best_six_ncells_t    *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  for (i = 0;
       i < MAX_NUMBER_OF_BEST_CELLS && i < best_six_ncells_ptr->no_of_entries;
       ++i)
  {
    if (ARFCNS_EQUAL(best_six_ncells_ptr->ncell_entry[i].ncell_data->arfcn, arfcn))
    {
      if (bsic)
      {
        if ((best_six_ncells_ptr->ncell_entry[i].ncell_data->bsic.PLMN_colour_code == bsic->PLMN_colour_code) &&
            (best_six_ncells_ptr->ncell_entry[i].ncell_data->bsic.BS_colour_code == bsic->BS_colour_code))
        {
          rr_resel_candidate = &best_six_ncells_ptr->ncell_entry[i];
          break;
        }
      }
      else
      {
        rr_resel_candidate = &best_six_ncells_ptr->ncell_entry[i];
        break;
      }
    }
  }

  return rr_resel_candidate;
}

/****************************************************************************
 *   Function name:  rr_gprs_add_to_best_six
 *   --------------
 *
 *   Description: Adds a cell to the best six list. This will set the entry to
 *   -----------  point at the entry in the BA list. If a bcch ncell it will
 *                set up the arfcn and bsic values.
 *
 *   Parameters: index into the best six
 *   -----------
 *
 *   Returns:  rr_timer_id_T *
 *   --------
 *
 ****************************************************************************/
void rr_gprs_add_to_best_six(
  ARFCN_T arfcn,
  BSIC_T bsic,
  boolean start_ptimer,
  const gas_id_t gas_id
)
{
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  if (best_six_ncells_ptr->no_of_entries < MAX_NUMBER_OF_BEST_CELLS)
  {
    /* assign a pointer to the entry */
    best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data =
      rr_gprs_get_ncell_info(
        arfcn,
        &bsic,
        gas_id
      );

    if ( best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data == NULL )
    {
      if ( rr_gprs_camped_on_ccch(gas_id) || rr_gsm_camped_on_ccch(gas_id) )
      {
        /*
        Try to search again without specifying the BSIC. There may be a previous entry that has got
        system information and a valid BSIC. But if it was whilst camped on BCCH then the BSIC will be
        that of the nearest cell at the time. This entry should be used and treated as a new cell.
        */

        best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data =
          rr_gprs_get_ncell_info(
            arfcn,
            NULL,
            gas_id
          );

        if ( best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data != NULL )
        {
          if ( best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data->bcch_or_pmo == BCCH_FORMAT )
          {
            MSG_GERAN_MED_3_G("Update ARFCN %d with diff BSIC <%d,%d>",CONVERT_FROM_ARFCN_T(arfcn),bsic.PLMN_colour_code,bsic.BS_colour_code);

            /* change the entry to the new BSIC */
            best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data->bsic = bsic;
            best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data->ncell_data.bcch.gsm.BSIC = bsic;

            /* Invalidate the data */
            best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data->ncell_data.bcch.gsm.valid_data_flag = FALSE;
            best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data->ncell_data.bcch.gprs.si13_data_valid_flag = FALSE;
            best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data->ncell_data.bcch.system_info_timer_count = UNINITIALISED_SYS_INFO_COUNT;

            /* diff BSIC detected on same channel, flush the cache for the old cell if identity is known otherwise
               flush for entire channel*/
            if (best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data->ncell_data.bcch.gsm.cell_id_valid)
            {
              rr_csi_flush_data(&(best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data->ncell_data.bcch.gsm.location_area_identification),
                       (uint16*)&(best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries].ncell_data->ncell_data.bcch.gsm.cell_id),
                                 CSI_FLUSH_DIFF_CELL_ON_ARFCN,
                                 gas_id);
            }
            else
            {
              rr_csi_flush_arfcn_data(arfcn,
                                      CSI_FLUSH_DIFF_CELL_ON_ARFCN,
                                      gas_id);
            }
          }
        }
        else
        {
          MSG_GERAN_HIGH_1_G("No matching ncell entry found %d",CONVERT_FROM_ARFCN_T(arfcn));
        }
      }
      else
      {
        MSG_GERAN_HIGH_1_G("No matching ncell entry found %d",CONVERT_FROM_ARFCN_T(arfcn));
      }

    }
    else
    {
      rr_resel_candidate_t * resel_candidate;
      resel_candidate = &best_six_ncells_ptr->ncell_entry[best_six_ncells_ptr->no_of_entries];

      /* allocate the penalty timer for this entry and start it and 5 sec timer if necessary */
      if (resel_candidate->ncell_data->bcch_or_pmo == BCCH_FORMAT)
      {
        bcch_ncell_info_t * bcch_data_ptr = &resel_candidate->ncell_data->ncell_data.bcch;

        /* set up the bsic and arfcn info */
        resel_candidate->ncell_data->bsic = bsic;
        resel_candidate->ncell_data->bsic_valid = TRUE;
        bcch_data_ptr->gsm.BSIC = bsic;
        bcch_data_ptr->gsm.BCCH_ARFCN = arfcn;

        resel_candidate->penalty_timer_id = rr_gprs_get_gprs_penalty_timer_id(gas_id);
        resel_candidate->reselection_info.five_second_timer_id = rr_gprs_get_five_second_timer_id(gas_id);

        if (bcch_data_ptr->gsm.valid_data_flag)
        {
          MSG_GERAN_MED_2_G("Re-enter best six %d (%1d)",CONVERT_FROM_ARFCN_T(arfcn),(int)GET_ARFCN_BAND(arfcn));

          if ( (bcch_data_ptr->gsm.cell_selection_parameters.CELL_RESELECT_PARAM_IND == 1) &&
               (start_ptimer == TRUE) )
          {
            rr_start_penalty_timer(
              resel_candidate,
              bcch_data_ptr->gsm.cell_selection_parameters.PENALTY_TIME,
              gas_id
            );
          }
        }
      }
      else
      {
        /* For PMO/PCCO ncells start the penalty timers as they are added to the best six */
        pmo_ncell_info_t * psi_data_ptr = &resel_candidate->ncell_data->ncell_data.pmo_pcco;

        resel_candidate->penalty_timer_id = rr_gprs_get_gprs_penalty_timer_id(gas_id);

        if (start_ptimer == TRUE)
        {
          (void) rr_timer_start(
            resel_candidate->penalty_timer_id,
            (dword)10000*(psi_data_ptr->gprs_cell_selection_params.gprs_penalty_time + 1),
            gas_id
          );

          MSG_GERAN_MED_2_G("Started Ptimer of %dsecs for PSI ncell %d",
            (10*(psi_data_ptr->gprs_cell_selection_params.gprs_penalty_time + 1)),
            CONVERT_FROM_ARFCN_T(arfcn));
        }
      }

      best_six_ncells_ptr->no_of_entries++;
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Best six full! cannot add entry %d", CONVERT_FROM_ARFCN_T(arfcn));
  }

}

/****************************************************************************
 *   Function name:  rr_gprs_return_penalty_timer_id
 *   --------------
 *
 *   Description: Gets the penalty timer that is allocated to a best six entry
 *   -----------
 *
 *   Parameters: index into the best six
 *   -----------
 *
 *   Returns:  rr_timer_id_T *
 *   --------
 *
 ****************************************************************************/
rr_timer_id_T * rr_gprs_return_penalty_timer_id(
  uint16 index,
  const gas_id_t gas_id
)
{
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  return &best_six_ncells_ptr->ncell_entry[index].penalty_timer_id;
}

/****************************************************************************
 *   Function name:  rr_gprs_remove_from_best_six
 *   --------------
 *
 *   Description: Removes and entry from the best six database
 *   -----------
 *
 *   Parameters: arfcn bsic
 *   -----------
 *
 *   Returns:  None
 *   --------
 *
 ****************************************************************************/
void rr_gprs_remove_from_best_six(
  ARFCN_T arfcn,
  BSIC_T bsic,
  const gas_id_t gas_id
)
{
  rr_resel_candidate_t *resel_candidate;
  best_six_ncells_t    *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
  boolean               si_from_cache;
  uint8 i;

  /* Find the entry in the best six list */
  for (i = 0;
       i < best_six_ncells_ptr->no_of_entries && i < MAX_NUMBER_OF_BEST_CELLS;
       ++i)
  {
    resel_candidate = &best_six_ncells_ptr->ncell_entry[i];

    if (ARFCNS_EQUAL(resel_candidate->ncell_data->arfcn, arfcn) &&
        SAME_BSIC(resel_candidate->ncell_data->bsic, bsic))
    {
      MSG_GERAN_HIGH_3_G("Remove ARFCN %d BSIC <%d,%d> from best six",
                         CONVERT_FROM_ARFCN_T(arfcn),
                         bsic.PLMN_colour_code, bsic.BS_colour_code);

      (void) rr_timer_cancel(
        resel_candidate->penalty_timer_id,
        gas_id
      );

      resel_candidate->penalty_timer_id = RR_TIMER_UNDEFINED;

      if (best_six_ncells_ptr->ncell_entry[i].ncell_data->bcch_or_pmo == BCCH_FORMAT)
      {
        (void) rr_timer_cancel(
          best_six_ncells_ptr->ncell_entry[i].reselection_info.five_second_timer_id,
          gas_id
        );

        best_six_ncells_ptr->ncell_entry[i].reselection_info.five_second_timer_id = RR_TIMER_UNDEFINED;
      }

      /* If GRR is awaiting a surround response for this arfcn clear it */
      si_from_cache = FALSE;
      if (match_surround_response_with_cache_check(arfcn, &si_from_cache, gas_id))
      {
        if (!si_from_cache)
      {
        /* Stop L1, this is probably not needed but keeps the cycle consistant */
          rr_send_mph_stop_surround_update_req(gas_id);
        }
        clear_waiting_for_surround_response(gas_id);
        rr_log_gprs_surround_search_failure(RR_LOG_GPRS_NCELL_NOT_FOUND_BY_L1, gas_id);
      }

      break;
    }
    else if (ARFCNS_EQUAL(resel_candidate->ncell_data->arfcn, arfcn))
    {
      /* diff BSIC detected on same channel, flush the cache for the old cell if identity is known otherwise
         flush for entire channel*/
      if (resel_candidate->ncell_data->bcch_or_pmo == BCCH_FORMAT &&
          resel_candidate->ncell_data->ncell_data.bcch.gsm.cell_id_valid)
      {
        rr_csi_flush_data(&(resel_candidate->ncell_data->ncell_data.bcch.gsm.location_area_identification),
                 (uint16*)&(resel_candidate->ncell_data->ncell_data.bcch.gsm.cell_id),
                           CSI_FLUSH_DIFF_CELL_ON_ARFCN,
                           gas_id);
      }
      else
      {
        rr_csi_flush_arfcn_data(arfcn,
                                CSI_FLUSH_DIFF_CELL_ON_ARFCN,
                                gas_id);
      }

      MSG_GERAN_ERROR_3_G("ARFCN %d BSIC <%d,%d> mismatched", CONVERT_FROM_ARFCN_T(arfcn), bsic.PLMN_colour_code, bsic.BS_colour_code);
    }
  }

  /* Shuffle the entries down */
  while (i < (best_six_ncells_ptr->no_of_entries - 1) && (i + 1) < MAX_NUMBER_OF_BEST_CELLS)
  {
    best_six_ncells_ptr->ncell_entry[i] = best_six_ncells_ptr->ncell_entry[i+1];
    i++;
  }

  /* update the ncell count - if no match was found above, this won't change */
  best_six_ncells_ptr->no_of_entries = i;

  if (i < MAX_NUMBER_OF_BEST_CELLS)
  {
  /* Remove the last entry */
    best_six_ncells_ptr->ncell_entry[i].ncell_data = NULL;
    MSG_GERAN_HIGH_1_G("Last entry removed - best six now contains %d entries", i);
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Best six still contains %d entries", i);
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_remove_all_best_six_ncells
 *   --------------
 *
 *   Description: Cleans out the best six database. Stops any timers and
 *   -----------  deallocates them
 *
 *   Parameters: none
 *   -----------
 *
 *   Returns:  None
 *   --------
 *
 ****************************************************************************/
void rr_remove_all_best_six_ncells(
  const gas_id_t gas_id
)
{
  rr_resel_candidate_t *resel_candidate;
  best_six_ncells_t    *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
  boolean               si_from_cache;
  uint8                 i;

  /* Find the entry in the best six list */
  for (i = 0; i < best_six_ncells_ptr->no_of_entries; ++i)
  {
    resel_candidate = &best_six_ncells_ptr->ncell_entry[i];

    /* kill any timers and deallocate them */
    (void) rr_timer_cancel(
      resel_candidate->penalty_timer_id,
      gas_id
    );

    resel_candidate->penalty_timer_id = RR_TIMER_UNDEFINED;

    if (resel_candidate->ncell_data->bcch_or_pmo == BCCH_FORMAT)
    {
      (void)rr_timer_cancel(resel_candidate->reselection_info.five_second_timer_id, gas_id);
      resel_candidate->reselection_info.five_second_timer_id = RR_TIMER_UNDEFINED;
    }

    /* If GRR is awaiting a surround response check this the arfcn and clear it */
    si_from_cache = FALSE;
    if (match_surround_response_with_cache_check(
          best_six_ncells_ptr->ncell_entry[i].ncell_data->arfcn,
          &si_from_cache,
          gas_id
        ))
    {
      if (!si_from_cache)
    {
      /* Stop L1, this is probably not needed but keeps the cycle consistant */
        rr_send_mph_stop_surround_update_req(gas_id);
      }
      clear_waiting_for_surround_response(gas_id);
      rr_log_gprs_surround_search_failure(RR_LOG_GPRS_NCELL_NOT_FOUND_BY_L1, gas_id);
    }

    /* Remove the entry */
    best_six_ncells_ptr->ncell_entry[i].ncell_data = NULL;
  }

  best_six_ncells_ptr->no_of_entries = 0;
}

/****************************************************************************
 *   Function name:  rr_gprs_is_ncell_in_best_six
 *   --------------
 *
 *   Description: This function searches through the best six database and
 *   -----------  sees if a match can be found for the ARFCN passed in.
 *                If one is found then the function returns TRUE.
 *
 *   Parameters: surrounding_channel_info_T.... From the meas report.
 *   -----------
 *
 *   Returns:  boolean TRUE/FALSE if found/not found
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_is_ncell_in_best_six(
  surrounding_channel_info_T *meas_chan_info_ptr,
  const gas_id_t gas_id
)
{
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
  boolean            channel_present = FALSE;
  ARFCN_T            arfcn;
  uint8              index;

  /* Find the entry in the best six list */
  for( index = 0; index < best_six_ncells_ptr->no_of_entries ; index++ )
  {
    arfcn = best_six_ncells_ptr->ncell_entry[index].ncell_data->arfcn;

    if (ARFCNS_EQUAL(arfcn, meas_chan_info_ptr->BCCH_ARFCN))
    {
      if (meas_chan_info_ptr->valid_info == TRUE)
      {
        /*******************************************************************/
        /* Compute the BSIC for the new cell (if information is available) */
        /* just to check that this matches the cell in the pbcch database. */
        /*******************************************************************/
        BSIC_T measurementBSIC = {0};
        measurementBSIC.PLMN_colour_code = DECODE_NCC(meas_chan_info_ptr);
        measurementBSIC.BS_colour_code = DECODE_BCC(meas_chan_info_ptr);
        if (SAME_BSIC(best_six_ncells_ptr->ncell_entry[index].ncell_data->bsic, measurementBSIC))
        {
          channel_present = TRUE;
          break;
        }
        else
        {
          /* diff BSIC detected on same channel, flush the cache for the old cell if identity is known otherwise
             flush for entire channel*/
          if (best_six_ncells_ptr->ncell_entry[index].ncell_data->bcch_or_pmo == BCCH_FORMAT &&
              best_six_ncells_ptr->ncell_entry[index].ncell_data->ncell_data.bcch.gsm.cell_id_valid)
          {
            rr_csi_flush_data(&(best_six_ncells_ptr->ncell_entry[index].ncell_data->ncell_data.bcch.gsm.location_area_identification),
                     (uint16*)&(best_six_ncells_ptr->ncell_entry[index].ncell_data->ncell_data.bcch.gsm.cell_id),
                               CSI_FLUSH_DIFF_CELL_ON_ARFCN,
                               gas_id);
          }
          else
          {
            rr_csi_flush_arfcn_data(arfcn,
                                    CSI_FLUSH_DIFF_CELL_ON_ARFCN,
                                    gas_id);
          }
        }
      }
      else
      {
        channel_present = TRUE;
        break;
      }
    }
  }

  return(channel_present);
}

/****************************************************************************
 *   Function name:  rr_gprs_get_gprs_penalty_timer_id
 *   --------------
 *
 *   Description: This function looks for an unallocated penatly timer, by
 *   -----------  looking through the best six list
 *
 *   Parameters: void
 *   -----------
 *
 *   Returns:  a timer ID
 *   --------
 *
 ****************************************************************************/
rr_timer_id_T rr_gprs_get_gprs_penalty_timer_id(const gas_id_t gas_id)
{
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
  boolean            penalty_timer_used[6] = {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
  rr_timer_id_T      penalty_timer_id = GRR_PENALTY_TIMER_6;
  byte               index;

  /* Find all the used penalty timers in the best 6 */
  for (index = 0; index < best_six_ncells_ptr->no_of_entries; ++index)
  {
    switch (best_six_ncells_ptr->ncell_entry[index].penalty_timer_id)
    {
      case GRR_PENALTY_TIMER_1:
        penalty_timer_used[0] = TRUE;
        break;
      case GRR_PENALTY_TIMER_2:
        penalty_timer_used[1] = TRUE;
        break;
      case GRR_PENALTY_TIMER_3:
        penalty_timer_used[2] = TRUE;
        break;
      case GRR_PENALTY_TIMER_4:
        penalty_timer_used[3] = TRUE;
        break;
      case GRR_PENALTY_TIMER_5:
        penalty_timer_used[4] = TRUE;
        break;
      case GRR_PENALTY_TIMER_6:
        penalty_timer_used[5] = TRUE;
        break;
      default:
        break;
    }
  }

  /******************************************************************/
  /* Now that all allocated penalty timers have been set to TRUE in */
  /* the penalty_timer_used array, find the first entry that is set */
  /* to FALSE and return this ID                                    */
  /******************************************************************/
  for (index = 0; index < 6; ++index)
  {
    if (penalty_timer_used[index] == FALSE)
    {
      switch (index)
      {
        case 0:
          penalty_timer_id = GRR_PENALTY_TIMER_1;
          break;
        case 1:
          penalty_timer_id = GRR_PENALTY_TIMER_2;
          break;
        case 2:
          penalty_timer_id = GRR_PENALTY_TIMER_3;
          break;
        case 3:
          penalty_timer_id = GRR_PENALTY_TIMER_4;
          break;
        case 4:
          penalty_timer_id = GRR_PENALTY_TIMER_5;
          break;
        case 5:
          penalty_timer_id = GRR_PENALTY_TIMER_6;
          break;
        default:
          MSG_GERAN_ERROR_0_G("No free penalty timer, reusing number 6");
          penalty_timer_id = GRR_PENALTY_TIMER_6;
      }
      /*Stop any more iterations, a timer has been found*/
      break;
    }
  }
  return penalty_timer_id;
}


/****************************************************************************
 *   Function name:  rr_gprs_get_five_second_timer_id
 *   --------------
 *
 *   Description: This function looks for an unallocated 5 sec timer, by
 *   -----------  looking through the best six list
 *
 *   Parameters: void
 *   -----------
 *
 *   Returns:  a timer ID
 *   --------
 *
 ****************************************************************************/
rr_timer_id_T rr_gprs_get_five_second_timer_id(
  const gas_id_t gas_id
)
{
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
  boolean            five_sec_timer_used[6] = {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
  rr_timer_id_T      five_sec_timer_id = RR_FIVE_SEC_C2_TIMER_6;
  byte               index;

  /* Find the entry in the best six list */
  for (index = 0; index < best_six_ncells_ptr->no_of_entries; index++)
  {
    switch (best_six_ncells_ptr->ncell_entry[index].reselection_info.five_second_timer_id)
    {
      case RR_FIVE_SEC_C2_TIMER_1:
        five_sec_timer_used[0] = TRUE;
        break;
      case RR_FIVE_SEC_C2_TIMER_2:
        five_sec_timer_used[1] = TRUE;
        break;
      case RR_FIVE_SEC_C2_TIMER_3:
        five_sec_timer_used[2] = TRUE;
        break;
      case RR_FIVE_SEC_C2_TIMER_4:
        five_sec_timer_used[3] = TRUE;
        break;
      case RR_FIVE_SEC_C2_TIMER_5:
        five_sec_timer_used[4] = TRUE;
        break;
      case RR_FIVE_SEC_C2_TIMER_6:
        five_sec_timer_used[5] = TRUE;
        break;
      default:
        break;
    }

  }

  /******************************************************************/
  /* Now that all allocated penalty timers have been set to TRUE in */
  /* the penalty_timer_used array, find the first entry that is set */
  /* to FALSE and return this ID                                    */
  /******************************************************************/
  for (index=0;index<6;index++)
  {
    if (five_sec_timer_used[index] == FALSE)
    {
      switch (index)
      {
        case 0:
          five_sec_timer_id = RR_FIVE_SEC_C2_TIMER_1;
          break;
        case 1:
          five_sec_timer_id = RR_FIVE_SEC_C2_TIMER_2;
          break;
        case 2:
          five_sec_timer_id = RR_FIVE_SEC_C2_TIMER_3;
          break;
        case 3:
          five_sec_timer_id = RR_FIVE_SEC_C2_TIMER_4;
          break;
        case 4:
          five_sec_timer_id = RR_FIVE_SEC_C2_TIMER_5;
          break;
        case 5:
          five_sec_timer_id = RR_FIVE_SEC_C2_TIMER_6;
          break;
        default:
          MSG_GERAN_ERROR_0_G("No free 5 sec timer, reusing number 6");
          five_sec_timer_id = RR_FIVE_SEC_C2_TIMER_6;
      }
      /* Stop any more iterations, a timer has been found */
      break;
    }
  }
  return five_sec_timer_id;
}

/****************************************************************************
 *   Function name:  rr_gprs_blank_lai
 *   --------------------------
 *
 *   Description: This function checks whether a LAI structure is uninitialised
 *  ------------
 *
 *   Parameters: LAI structure pointer
 *   -----------
 *
 *   Returns:  TRUE if the contents of the structure are zeros
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_blank_lai(
  LAI_T *LAI_ptr
)
{
  boolean is_blank;

  if (LAI_ptr != NULL)
  {
    is_blank = ((LAI_ptr->plmn_id.identity[0] == 0) &&
                (LAI_ptr->plmn_id.identity[1] == 0) &&
                (LAI_ptr->plmn_id.identity[2] == 0) &&
                (LAI_ptr->location_area_code  == 0));
  }
  else
  {
    /* NULL input pointer is treated as a blank */
    is_blank = TRUE;
  }

  return(is_blank);
}

/****************************************************************************
 *   Function name:  rr_gprs_resel_suitable_cell
 *   --------------------------
 *
 *   Description: This function is used to decide if a cell is suitable for
 *                for Reselection i.e. not barred, home plmn and not in a
 *                 forbidden LA
 *  ------------
 *
 *   Parameters: cell ARFCN
 *   -----------
 *
 *   Returns:  arfcn returned
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_resel_suitable_cell(
  bcch_ncell_info_t *ptr_surround_database,
  const gas_id_t gas_id
)
{
  rr_internal_band_T band;
  rr_csi_cgi_t cgi;
  rr_csi_cgi_t *cgi_ptr = NULL;
  boolean suitable_cell = FALSE;
  rr_cell_type_T cell_type;
  boolean PLMN_ok = TRUE;
  boolean forbidden_lai = FALSE;
  boolean auth_failed_cell_barred = FALSE;
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  boolean supported_band = TRUE;
  boolean emergency_call_allowed = TRUE;

  if ( ptr_surround_database == NULL )
  {
    MSG_GERAN_ERROR_0_G("Null PTR Assigned");
    return suitable_cell;
  }

  /* Check if the Frequency of the cell is on a Band which is currently Not Supported 
     There is a possibility that due to Band Coexistence, the ARFCN on which the Cell is
	 Temporarily Unsupported */
     band = GET_ARFCN_BAND(ptr_surround_database->gsm.BCCH_ARFCN);
     if (rr_gapi_is_supported_band((sys_band_T)band, gas_id) == FALSE)
     {
        MSG_GERAN_MED_1_G("Cell is on an unsupported band(%d)", (int)band);
        supported_band = FALSE;
     }
     
  /***************************************************************/
  /* Get the cell type for the surrounding cell. The cell cannot */
  /* be barred to be considered for reselection                  */
  /***************************************************************/
  cell_type = rr_get_cell_priority(
    ptr_surround_database->gsm.cell_selection_parameters.CELL_BAR_QUALIFY,
                                   ptr_surround_database->gsm.RACH_control_parameters.CELL_BAR_ACCESS,
                                   &(ptr_surround_database->gsm.location_area_identification),
                                   ptr_surround_database->gsm.RACH_control_parameters.access_control,
    NULL,
    gas_id
  );

  /**********************************************************************/
  /* If we are not in any cell selection mode, the PLMN must be checked */
  /* against the PLMN requested by GMM in select_specific_plmn_req      */
  /**********************************************************************/
  if (!rr_is_any_cell_reselection_mode(gas_id))
  {

    if ( rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION )
    {
      /* in this mode make sure that the ncell's plmn is equal to the scells plmn */
      gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
      rr_PLMN_request_details_T scell_plmn_id = *rr_PLMN_request_details_ptr;

      scell_plmn_id.requested_PLMN_id = scell_info_ptr->gsm.location_area_identification.plmn_id;

      /* make sure that the PLMN is correct */
      PLMN_ok = rr_cell_selection_PLMN_match(
        &scell_plmn_id,
        &ptr_surround_database->gsm.location_area_identification,
        gas_id
      );
    }
    else
    {
      /* make sure that the PLMN is correct */
      PLMN_ok = rr_cell_selection_PLMN_match(
        rr_PLMN_request_details_ptr,
        &ptr_surround_database->gsm.location_area_identification,
        gas_id
      );
    }
  }

  /* 43.022 3.2.2 & 3.7 
     3.7 No suitable cell (limited service state)
     If the MS is unable to obtain normal service from a PLMN (see 3GPP TS 23.122), the MS attempts to camp on an
     acceptable cell, irrespective of its PLMN identity, so that emergency calls can be made or warning notifications can be
     received if necessary */

  /* Check for access_class when net selection mode is LIMITED */
  if (rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
  {
    access_result_T  access_result;

    access_result = rr_check_access_class(
                      MM_RR_EMERGENCY_CALL,
                      ptr_surround_database->gsm.location_area_identification.plmn_id,         // location_area_identification
                      ptr_surround_database->gsm.RACH_control_parameters.access_control,        // access_control
                      ptr_surround_database->gsm.RACH_control_parameters.CELL_BAR_ACCESS,       // CELL_BAR_ACCESS
                      ptr_surround_database->gsm.cell_selection_parameters.CELL_BAR_QUALIFY,    // CELL_BAR_QUALIFY
                      gas_id
                    );

    if (access_result == EMERGENCY_CALLS_ALLOWED || access_result == ALL_CALLS_ALLOWED)
    {
      emergency_call_allowed = TRUE;
      MSG_GERAN_HIGH_1_G("Emergency or All calls allowed: %d",emergency_call_allowed);
    }
    else
    {
      emergency_call_allowed = FALSE;
    }
  }

  /* always check forbidden LAI list in reselection, regardless manual or automatic */
  forbidden_lai = rr_is_forbidden_LAI(
    &ptr_surround_database->gsm.location_area_identification,
    gas_id
  );

  if (ptr_surround_database->gsm.cell_id_valid == TRUE)
  {
    cgi.ci = ptr_surround_database->gsm.cell_id;
    cgi.lai = ptr_surround_database->gsm.location_area_identification;
    cgi_ptr = &cgi;
  }

  if (rr_is_cell_in_barred_cell_list(
        ptr_surround_database->gsm.BCCH_ARFCN,
        ptr_surround_database->gsm.BSIC,
        cgi_ptr,
        NULL,
        gas_id
      ))
  {
    auth_failed_cell_barred = TRUE;
  }

  if (cell_type == RR_CELL_BARRED)
  {
    MSG_GERAN_MED_1_G("ncell(%d) barred", CONVERT_FROM_ARFCN_T(ptr_surround_database->gsm.BCCH_ARFCN));
  }
  else if (!PLMN_ok)
  {
    if (rr_gprs_blank_lai(&ptr_surround_database->gsm.location_area_identification))
    {
      MSG_GERAN_MED_1_G("LAI not recvd yet for ARFCN %d", CONVERT_FROM_ARFCN_T(ptr_surround_database->gsm.BCCH_ARFCN));
    }
    else
    {
      MSG_GERAN_MED_1_G("ncell(%d) wrong PLMN", CONVERT_FROM_ARFCN_T(ptr_surround_database->gsm.BCCH_ARFCN));
    }
  }
  else if (forbidden_lai)
  {
    MSG_GERAN_MED_1_G("ncell(%d) Forbidden lai", CONVERT_FROM_ARFCN_T(ptr_surround_database->gsm.BCCH_ARFCN));
  }
  else if (auth_failed_cell_barred)
  {
    MSG_GERAN_MED_3_G("ncell(%d), ncc(%d), bcc(%d) barred (auth failed)",
              CONVERT_FROM_ARFCN_T(ptr_surround_database->gsm.BCCH_ARFCN),
              ptr_surround_database->gsm.BSIC.BS_colour_code,
              ptr_surround_database->gsm.BSIC.PLMN_colour_code);
  }

  /***************************************************************/
  /* can only check C1/C2 values for surrounding cells that have */
  /* valid system information, because the Routing area and cell */
  /* priority must be checked i.e. cell not barred             */
  /* Also don't bother with cells that are barred, as they cannot*/
  /* be considered for reselection                             */
  /***************************************************************/
  if (ptr_surround_database->gsm.valid_data_flag == TRUE &&
      cell_type != RR_CELL_BARRED &&
      PLMN_ok &&
      !forbidden_lai &&
      !auth_failed_cell_barred &&
      supported_band &&
      emergency_call_allowed
      )
  {
    suitable_cell = TRUE;
  }

  return suitable_cell;
}


/****************************************************************************
 *   Function name:  rr_gprs_update_penalty_timer_state
 *   --------------
 *
 *   Description: This function moves through either the bcch or pbcch or both
 *   -----------  databases to find the penalty timer passed in so it can be
 *                set to expired.
 *
 *   Parameters: penalty_timer_id
 *   -----------
 *
 *   Returns:  void
 *   --------
 *
 ****************************************************************************/
void rr_gprs_update_penalty_timer_state(
  rr_timer_id_T penalty_timer_id,
  const gas_id_t gas_id
)
{
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
  rr_timer_id_T     *p_timer_id;
  uint16             index;

  for ( index = 0; index < best_six_ncells_ptr->no_of_entries &&
                   index < MAX_NUMBER_OF_BEST_CELLS; index++ )
  {
    p_timer_id = rr_gprs_return_penalty_timer_id(index, gas_id);

    if ( *p_timer_id == penalty_timer_id )
    {
      (void)rr_timer_create_expired_timer(*p_timer_id, gas_id);
      MSG_GERAN_MED_1_G("Penalty timer for ARFCN %d set to EXPIRED",
              CONVERT_FROM_ARFCN_T(best_six_ncells_ptr->ncell_entry[index].ncell_data->arfcn));
      break;
    }
  }
}

/****************************************************************************
 *   Function name:  rr_gprs_update_five_second_timer_state
 *   --------------
 *
 *   Description: This function moves through either the bcch or pbcch or both
 *   -----------  databases to find the penalty timer passed in so it can be
 *                set to expired.
 *
 *   Parameters: penalty_timer_id
 *   -----------
 *
 *   Returns:  void
 *   --------
 *
 ****************************************************************************/
void rr_gprs_update_five_second_timer_state(
  rr_timer_id_T five_sec_timer_id,
  const gas_id_t gas_id
)
{
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
  uint16             index;

  for (index = 0; index < best_six_ncells_ptr->no_of_entries; ++index)
  {
    if (best_six_ncells_ptr->ncell_entry[index].reselection_info.five_second_timer_id == five_sec_timer_id)
    {
      (void)rr_timer_create_expired_timer(five_sec_timer_id, gas_id);
      MSG_GERAN_MED_1_G("5sec timer for ARFCN %d set to EXPIRED",
              CONVERT_FROM_ARFCN_T(best_six_ncells_ptr->ncell_entry[index].ncell_data->arfcn));
      break;
    }
  }
}

/****************************************************************************
 *   Function name:  rr_resel_clear_any_active_five_sec_timer
 *   --------------
 *
 *   Description: This function moves through best six cell database to find any 
 *   -----------  already expired entry or running entry and clear it if found to be running or expired.
 *               
 *
 *   Parameters: gas_id
 *   -----------
 *
 *   Returns:  void
 *   --------
 *
 ****************************************************************************/
 void rr_resel_clear_any_active_five_sec_timer(
    const gas_id_t gas_id )
{
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
  uint16             index;
  rr_timer_state_T   rr_timer_state;
  
  for (index = 0; index < best_six_ncells_ptr->no_of_entries; ++index)
  {     
    rr_timer_state = rr_timer_get_state(best_six_ncells_ptr->ncell_entry[index].reselection_info.five_second_timer_id, gas_id);
    
    if ((rr_timer_state == RR_TIMER_EXPIRED) || (rr_timer_state == RR_TIMER_RUNNING))    
    {
      MSG_GERAN_MED_2_G(" Stop 5sec timer for ARFCN %d detected in state %d",
              CONVERT_FROM_ARFCN_T(best_six_ncells_ptr->ncell_entry[index].ncell_data->arfcn), rr_timer_state);
      
      /****************************************************************************/
      /* Stop/deallocate the timer state to stopped so that if reselection is     */
      /* abandoned for any reason, 5secs must again elapse before starting a cell */
      /* reselection attempt.                                                     */
      /****************************************************************************/
      (void)rr_timer_cancel(best_six_ncells_ptr->ncell_entry[index].reselection_info.five_second_timer_id, gas_id);      
    }
  }
}

/****************************************************************************
 *   Function name:  rr_gprs_start_five_second_timer
 *   --------------
 *
 *   Description: This function allocates a free measurement timer to the
 *   -----------  ncell. It then starts the timer
 *
 *   Parameters: rr_gprs_reselection_measurements - from within the ncell database
 *   -----------
 *
 *   Returns:  void
 *   --------
 *
 ****************************************************************************/
void rr_gprs_start_five_second_timer(
  reselection_info_T *rr_gprs_reselection_meas_ptr,
  const gas_id_t gas_id
)
{
  if (rr_timer_start(rr_gprs_reselection_meas_ptr->five_second_timer_id, RR_FIVE_SEC_TIMEOUT, gas_id))
  {
    MSG_GERAN_HIGH_2_G("Started 5 second timer (%d) (GAS-ID=%d)",
             rr_gprs_reselection_meas_ptr->five_second_timer_id, (int)gas_id);
  }
  else
{
    MSG_GERAN_ERROR_1_G("Failed to start 5 second timer (%d)", rr_gprs_reselection_meas_ptr->five_second_timer_id);
  }

  return;
} /* rr_gprs_start_five_second_timer */

/****************************************************************************
 *   Function name:  rr_gprs_stop_five_second_timer
 *   --------------
 *
 *   Description: This function stops the 5 sec timer in the ncell database
 *   -----------  entry that is passed in. The timer may be still running or
 *                expired. This function changes the state back to STOPPED
 *
 *   Parameters: rr_gprs_reselection_measurements_t (for the ncell database)
 *   -----------
 *
 *   Returns:  void
 *   --------
 *
 ****************************************************************************/
void rr_gprs_stop_five_second_timer(
  reselection_info_T *rr_gprs_reselection_meas_ptr,
  const gas_id_t gas_id
)
{
  if (rr_gprs_reselection_meas_ptr != NULL)
  {
    rr_timer_id_T five_second_timer_id = rr_gprs_reselection_meas_ptr->five_second_timer_id;

    if (five_second_timer_id==RR_SERV_FIVE_SEC_C1_TIMER || five_second_timer_id==RR_FIVE_SEC_C2_TIMER_1 ||
        five_second_timer_id==RR_FIVE_SEC_C2_TIMER_2 || five_second_timer_id==RR_FIVE_SEC_C2_TIMER_3 ||
        five_second_timer_id==RR_FIVE_SEC_C2_TIMER_4 || five_second_timer_id==RR_FIVE_SEC_C2_TIMER_5 ||
        five_second_timer_id==RR_FIVE_SEC_C2_TIMER_6)
    {
      if (rr_timer_cancel(five_second_timer_id, gas_id))
    {
        MSG_GERAN_HIGH_2_G("Stopped 5 second timer:%d (GAS-ID=%d)",
                 (int)five_second_timer_id, (int)gas_id);
      }
    }
    }
    else
    {
    MSG_GERAN_ERROR_0_G("NULL pointer for stopping 5 second timer");
    }

  return;
}

/*===========================================================================

FUNCTION  rr_cancel_all_five_second_timers

DESCRIPTION

Cancels all the five second timers

RETURN VALUE None

===========================================================================*/

void rr_gprs_cancel_all_five_second_timers(
  const gas_id_t gas_id
)
{
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_1, gas_id);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_2, gas_id);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_3, gas_id);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_4, gas_id);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_5, gas_id);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_6, gas_id);

  (void)rr_timer_cancel(RR_SERV_FIVE_SEC_C1_TIMER, gas_id);

  MSG_GERAN_HIGH_0_G("All 5 sec timers stopped");
} /* rr_gprs_cancel_all_five_second_timers */

/*===========================================================================

FUNCTION  rr_gprs_create_expired_penalty_timer

DESCRIPTION

Obtain a penalty timer ID for the supplied NCELL, and initialise it as expired

RETURN VALUE None

===========================================================================*/

void rr_gprs_create_expired_penalty_timer(
  rr_resel_candidate_t * resel_candidate,
  const gas_id_t gas_id
)
{
  if (resel_candidate == NULL)
  {
    MSG_GERAN_ERROR_0_G("resel_candidate == NULL");
  }
  else
  {
    (void)rr_timer_create_expired_timer(resel_candidate->penalty_timer_id, gas_id);
  }
}

/****************************************************************************
 *   Function name:  rr_gprs_surrounding_cell_in_best_six
 *   --------------
 *
 *   Description: This function makes sure that the surround cell that has its
 *   ------------ database passed in is still in the measurement report that is
 *                passed in. In other words check that the surround database is
 *                still one of the best 6.
 *
 *   Parameters: RXLEV
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_surrounding_cell_in_best_six(
  gsm_ncell_list_member_t *ncell_ptr,
  best_six_info_T         *p_best_six_info,
  boolean                 *bsic_changed_ptr,
  BSIC_T                  *newBSIC,
  const gas_id_t           gas_id
)
{
  boolean in_best_six = FALSE;
  byte index;
  ARFCN_T BCCH_ARFCN;
  /***********************************************************************/
  /* Channel information associated with the current surrounding channel */
  /* in the measurement report                                           */
  /***********************************************************************/
  surrounding_channel_info_T *current_meas_channel_ptr;
  byte measurement_entries = p_best_six_info->no_of_entries;

  /* assume bsic will not change */
  *bsic_changed_ptr = FALSE;

  /* loop through all the cells in the measurement report */
  for (index = 0; (index < measurement_entries) && (index < MAX_NUMBER_OF_SURROUND_CELLS) ; index++)
  {
    current_meas_channel_ptr = &(p_best_six_info->surrounding_channel_info[index]);

    /* compute the ARFCN of the current cell in the measurement report */
    BCCH_ARFCN = current_meas_channel_ptr->BCCH_ARFCN;

    /*******************************************************/
    /* If the current cell in the measurement report is in */
    /* the surrounding system information database         */
    /*******************************************************/
    if (ARFCNS_EQUAL(ncell_ptr->arfcn, BCCH_ARFCN))
    {
      /*************************************************************/
      /* Since the surrounding cell was found in the measurement   */
      /* report, it is one of the best six surrounding cells       */
      /*************************************************************/
      in_best_six = TRUE;

      /***************************************************************/
      /* The channel is still part of the best six cells. We have to */
      /* check if the BSIC is the same or not                        */
      /***************************************************************/
      if (current_meas_channel_ptr->valid_info == TRUE)
      {
        BSIC_T BSIC;

        /*********************************************/
        /* L1 was able to decode information for the */
        /* surrounding channel. Check to see if the  */
        /* BSIC for the cell has changed             */
        /*********************************************/
        BSIC.PLMN_colour_code = DECODE_NCC(current_meas_channel_ptr);
        BSIC.BS_colour_code = DECODE_BCC(current_meas_channel_ptr);

        if (!SAME_BSIC(BSIC, ncell_ptr->bsic))
        {
          *bsic_changed_ptr = TRUE;

          if ( newBSIC )
            *newBSIC = BSIC;

          in_best_six = FALSE;

          /* diff BSIC detected on same channel, flush the cache for the old cell if identity is known otherwise
             flush for entire channel*/
          if (ncell_ptr->bcch_or_pmo == BCCH_FORMAT &&
              ncell_ptr->ncell_data.bcch.gsm.cell_id_valid
             )
          {
            rr_csi_flush_data(&(ncell_ptr->ncell_data.bcch.gsm.location_area_identification),
                     (uint16*)&(ncell_ptr->ncell_data.bcch.gsm.cell_id),
                               CSI_FLUSH_DIFF_CELL_ON_ARFCN,
                               gas_id);
          }
          else
          {
            rr_csi_flush_arfcn_data(ncell_ptr->arfcn,
                                    CSI_FLUSH_DIFF_CELL_ON_ARFCN,
                                    gas_id);
          }
        }
      } /* information is valid for current measurement report cell info */
      else
      {
        /**************************************************************/
        /* The information reported for the current cell is not valid */
        /**************************************************************/
        MSG_GERAN_MED_1("Meas info not valid for cell: %d",(int) CONVERT_FROM_ARFCN_T(BCCH_ARFCN));
        /***********************************************************/
        /* Cannot compute the BSIC for the cell at this time, just */
        /* assume that they are the same                           */
        /***********************************************************/
        *bsic_changed_ptr = FALSE;
      } /* channel information not valid */

      /* found the cell in report, no need to keep looking */
      break;
    }/* surround DB cell matches cell in measurement report */
  } /* for all the cells in the measurement report */

  return in_best_six;
} /* rr_gprs_surrounding_cell_in_best_six */

/*===========================================================================

FUNCTION  rr_gprs_remove_serving_cell_from_surrounding_measurement
  Removes the serving cell from the measurement report (if present)

PARAMETERS

BCCH_ARFCN - ARFCN of serving cell
p_best_six_info - Measurement report for the best six surrounding cells

===========================================================================*/

void rr_gprs_remove_serving_cell_from_surrounding_measurement(
  ARFCN_T serving_BCCH_ARFCN,
  best_six_info_T *p_best_six_info
)
{
  int measurement_report_entries;
  int index;
  int remaining_index;

  measurement_report_entries = p_best_six_info->no_of_entries;

  if (measurement_report_entries > MAX_NUMBER_OF_SURROUND_CELLS)
  {
    MSG_GERAN_ERROR_2("measurement_report_entries (%d) out of range, truncating to %d",
              measurement_report_entries,
              MAX_NUMBER_OF_SURROUND_CELLS);
    measurement_report_entries = MAX_NUMBER_OF_SURROUND_CELLS;
  }

  /* foreach entry in the measurement report */
  for (index = 0; index < measurement_report_entries; index++)
  {
    ARFCN_T BCCH_ARFCN = p_best_six_info->surrounding_channel_info[index].BCCH_ARFCN;

    if (ARFCNS_EQUAL(BCCH_ARFCN, serving_BCCH_ARFCN))
    {
      /* delete the entry from the measurement report */
      for (remaining_index = index + 1; remaining_index < measurement_report_entries;++remaining_index)
      {
        p_best_six_info->surrounding_channel_info[remaining_index - 1] =
          p_best_six_info->surrounding_channel_info[remaining_index];
      }

      --(p_best_six_info->no_of_entries);

      /* since entry was deleted, must decrement both index and count for next iteration */
      index--;
      measurement_report_entries--;
    }
  }
} /* rr_gprs_remove_serving_cell_from_surrounding_measurement */


/****************************************************************************
 *   Function name:  rr_gprs_add_sync_cell
 *   -------------
 *
 *   Description:   adds a cell to the list of cells that L1 is sync'ed up to
 *
 *   Parameters:  arfcn
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_add_sync_cell(
  ARFCN_T arfcn,
  const gas_id_t gas_id
)
{
  sync_cell_data_t *sync_cell_data_ptr = sync_cell_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(sync_cell_data_ptr);

  if(!rr_gprs_is_cell_in_sync_list(arfcn, gas_id))
  {
    /*add the entry to the list*/
    if(sync_cell_data_ptr->no_of_sync_cells < MAX_NUMBER_OF_BEST_CELLS)
    {
      sync_cell_data_ptr->sync_cell_arfcn[sync_cell_data_ptr->no_of_sync_cells] = arfcn;
      sync_cell_data_ptr->no_of_sync_cells++;
    }
    else
    {
      MSG_GERAN_HIGH_1_G("Sync Cell overflow %d",CONVERT_FROM_ARFCN_T(arfcn));
    }
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_clear_sync_cell_list
 *   -------------
 *
 *   Description:   Clears the list of cells that L1 is sync'ed up to
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_clear_sync_cell_list(
  const gas_id_t gas_id
)
{
  uint8 count;

  sync_cell_data_t *sync_cell_data_ptr = sync_cell_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(sync_cell_data_ptr);

  sync_cell_data_ptr->no_of_sync_cells = 0;

  for (count = 0; count < MAX_NUMBER_OF_BEST_CELLS; count++)
  {
    sync_cell_data_ptr->sync_cell_arfcn[count].num  = INVALID_ARFCN;
    sync_cell_data_ptr->sync_cell_arfcn[count].band = (sys_band_T)BAND_NONE;
  }
}

/****************************************************************************
 *   Function name:  rr_gprs_is_cell_in_sync_list
 *   -------------
 *
 *   Description:   Checks arfcn against the sync cell list to see if L1 is
 *                  synced up to the cell.
 *
 *   Parameters:  arfcn
 *   -----------
 *
 *   Returns:  TRUE/FALSE
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_is_cell_in_sync_list(
  ARFCN_T arfcn,
  const gas_id_t gas_id
)
{
  boolean cell_already_in_list = FALSE;
  uint8   count;

  sync_cell_data_t *sync_cell_data_ptr = sync_cell_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(sync_cell_data_ptr);

  for (count = 0; count < sync_cell_data_ptr->no_of_sync_cells; count++)
  {
    if (ARFCNS_EQUAL(sync_cell_data_ptr->sync_cell_arfcn[count], arfcn))
    {
      cell_already_in_list = TRUE;
      break;
    }
  }

  return cell_already_in_list;
}
/*===========================================================================

FUNCTION  rr_gprs_is_scell_bsic_unchanged

DESCRIPTION
  Checks if the bsic in the scell measurement report matches that in the scell
  info database. returns true if they are equal.

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS

===========================================================================*/
boolean rr_gprs_is_scell_bsic_unchanged(
  mph_serving_idle_meas_ind_T * new_message,
  const gas_id_t gas_id
)
{
  BSIC_T bsic = {0};
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  bsic.PLMN_colour_code = DECODE_NCC( new_message );
  bsic.BS_colour_code = DECODE_BCC( new_message );

  return SAME_BSIC(bsic,scell_info_ptr->gsm.BSIC);
}

/****************************************************************************
 *   Function name:  rr_gprs_generate_rps_measurement_report
 *   -------------
 *
 *   Description:
 *   ------------
 *    Using the info in the best six data base combined with the results of the
 *    Pscan, a spoof measurement report can be constucted and processed. This is
 *    then used to decide which cells should be target in the reselection.
 *
 *
 *   Parameters: none
 *   ----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_generate_rps_measurement_report(
  rr_candidate_cell_db_T *candidate_db_ptr,
  const gas_id_t gas_id
)
{
  best_six_info_T    best_six_info = {0};
  uint8              index;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  PLMN_id_T          scell_plmn_id;
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  MSG_GERAN_HIGH_1_G("ALL CAND.ENTRIES PROCESSED. NUM_OF_NCELLS: %d", best_six_ncells_ptr->no_of_entries);


  /* Construct a fake measurement report */
  for (index = 0; index < best_six_ncells_ptr->no_of_entries; index++)
  {
    ARFCN_T arfcn_as_type;
    rr_candidate_cell_T *candidate_cell_ptr;
    uint8 spoof_sch_block = 0;
    surrounding_channel_info_T *surrounding_channel_info = &best_six_info.surrounding_channel_info[best_six_info.no_of_entries];
    bcch_ncell_info_t *ncell_ptr = &best_six_ncells_ptr->ncell_entry[index].ncell_data->ncell_data.bcch;

    CONVERT_TO_ARFCN_T(arfcn_as_type, ncell_ptr->gsm.BCCH_ARFCN);
    candidate_cell_ptr = rr_candidate_db_get_entry(candidate_db_ptr, arfcn_as_type);

    if ( candidate_cell_ptr != NULL && ncell_ptr->gsm.valid_data_flag )
    {
      MSG_GERAN_MED_2_G("Candidate cell %d valid BCCH with RXLEV %d", CONVERT_FROM_ARFCN_T(ncell_ptr->gsm.BCCH_ARFCN), candidate_cell_ptr->meas.RXLEV_average);

      surrounding_channel_info->BCCH_ARFCN = ncell_ptr->gsm.BCCH_ARFCN;
      surrounding_channel_info->RXLEV_average = candidate_cell_ptr->meas.RXLEV_average;
      /* Make up block 0 of the sch block using bsic already recived */
      spoof_sch_block = (((uint8)((ncell_ptr->gsm.BSIC.PLMN_colour_code << 3) | ncell_ptr->gsm.BSIC.BS_colour_code) << 2) & 0xfc);
      surrounding_channel_info->SCH_block[0] = spoof_sch_block;
      surrounding_channel_info->valid_info = TRUE;
      best_six_info.no_of_entries++;

      ncell_ptr->gprs.two_quater_position = candidate_cell_ptr->si_info.two_quater_position;
    }
    else
    {
      MSG_GERAN_MED_1_G("Candidate Cell %d not found", CONVERT_FROM_ARFCN_T(ncell_ptr->gsm.BCCH_ARFCN));
    }
  }

  /***************************************************************************************/
  /* At this point the scell info should be cleaned out. This is to stop any measurments */
  /* in the spoof measurement report for an arfcn that matches the one in the scell_info */
  /* they will be removed                                                                */
  /***************************************************************************************/

  /* Copy scell plmn id details */
  scell_plmn_id = scell_info_ptr->gsm.location_area_identification.plmn_id;

  rr_clear_system_information( &scell_info_ptr->gsm );
  
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
      rr_set_rr_using_pseudo_si2q_data(FALSE, gas_id);
#endif

  /* Restore scell plmn id now. scell plmn id is required if:

      - camped on scell for limited service,
      - network_selection_mode is 'limited routine acquisition',
      - reselection of a cell from same plmn as scell is required
        Ref: rr_gprs_resel_suitable_cell()
  */

  scell_info_ptr->gsm.location_area_identification.plmn_id = scell_plmn_id;

  rr_gprs_store_surrounding_measurement_MFRM51(
    &best_six_info,
    gas_id
  );

  (void)rr_gprs_build_candidate_cell_list_GSM(gas_id);
}

/****************************************************************************
 *   Function name:  grr_activate_reselection_if_allowed
 *   -------------
 *
 *   Description: Cell when camping on a new cell. Based on the NC mode, this allows
 *   ------------ or disables reselection
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  None
 *   --------
 *
 ****************************************************************************/
void grr_activate_reselection_if_allowed(
  const gas_id_t gas_id
)
{
  rr_event_T  grr_event;

  /* Determine if reselection handling should be activated based on NC mode */
  grr_event = rr_gprs_nc_measurement_control(RR_EV_MEASUREMENT_PARAMS_UPDATED, NULL, gas_id);

  /* Pass active/inactive indication into reselection control */
  (void) rr_resel_control(grr_event, NULL, gas_id);

  clear_waiting_for_surround_response(gas_id);

} /* end grr_activate_reselection_if_allowed() */

/****************************************************************************
 *   Function name:  rr_gprs_any_more_rps_cells_to_try
 *   -------------
 *
 *   Description:
 *   ------------
 *    Sees if any more cells exist in the pscan results that can be attempted
 *    for reselection. This check is only done after the cells that have been
 *    already tried are removed.
 *
 *   Parameters: none
 *   ----------
 *
 *   Returns:  boolean true/false
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_any_more_rps_cells_to_try(
  rr_reselection_pscan_cell_list_T *cell_list_ptr,
  const gas_id_t gas_id
)
{
  if (cell_list_ptr != NULL)
  {
    best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
    uint8              best_six_index;

    /*
    At this point the best 6 has the entries that have been already used for the RPS.
    Removing these cells from the grr_reselection_pscan_cell_list means they cannot be
    used again and the next strongest set will be used
    */
    for (best_six_index=0;best_six_index<best_six_ncells_ptr->no_of_entries;best_six_index++)
    {
      grr_reselection_pscan_cell_list_remove_entry(
        cell_list_ptr,
        best_six_ncells_ptr->ncell_entry[best_six_index].ncell_data->arfcn
      );
    }

    return (cell_list_ptr->no_of_entries != 0);
  }

  // No cell list supplied - indicate no more cells
  return FALSE;
}


/**
 * Adds an entry to the reselection power scan list
 *
 * @param pscan_cell_list_ptr pointer to power scan list
 *
 * @param ARFCN               ARFCN to add to the list
 */
void grr_reselection_pscan_cell_list_add_entry(
  rr_reselection_pscan_cell_list_T *pscan_cell_list_ptr,
  ARFCN_T ARFCN
)
{
  if (pscan_cell_list_ptr)
  {
    if (pscan_cell_list_ptr->no_of_entries < RR_MAX_RES_PSCAN_CELL_LIST_SIZE)
    {
      pscan_cell_list_ptr->cell_list[pscan_cell_list_ptr->no_of_entries] = ARFCN;
      (pscan_cell_list_ptr->no_of_entries) += 1;
    }
    else
    {
      MSG_GERAN_ERROR_0("pscan_cell_list_ptr->no_of_entries >= RR_MAX_RES_PSCAN_CELL_LIST_SIZE");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("pscan_cell_list_ptr == NULL");
  }
} /* grr_reselection_pscan_cell_list_add_entry */



/**
 * Checks an entry to see if it is already present in the reselection power scan list
 *
 * @param pscan_cell_list_ptr pointer to power scan list
 *
 * @param ARFCN               ARFCN to add to the list
 *
 * RETURNS                    TRUE - entry already exists, FALSE - otherwise
 * 
 */
boolean grr_reselection_pscan_cell_list_check_entry(
  rr_reselection_pscan_cell_list_T *pscan_cell_list_ptr,
  ARFCN_T                           ARFCN
)
{
  uint8   index = 0;
  boolean found = FALSE;
  
  if (pscan_cell_list_ptr)
  {
    if (pscan_cell_list_ptr->no_of_entries < RR_MAX_RES_PSCAN_CELL_LIST_SIZE)
    {
      for (index=0; index < pscan_cell_list_ptr->no_of_entries; index++)
      {
        if (ARFCNS_EQUAL(ARFCN, pscan_cell_list_ptr->cell_list[index]))
        {
          found = TRUE;
          break;
        }
      }
    }
    else
    {
      MSG_GERAN_ERROR_0("pscan_cell_list_ptr->no_of_entries >= RR_MAX_RES_PSCAN_CELL_LIST_SIZE");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("pscan_cell_list_ptr == NULL");
  }

  return(found);
} /* grr_reselection_pscan_cell_list_add_entry */


/**
 * Removes an entry from the reselection power scan list
 *
 * @param pscan_cell_list_ptr pointer to power scan list
 *
 * @param ARFCN               ARFCN to remove from the list
 */
void grr_reselection_pscan_cell_list_remove_entry(
  rr_reselection_pscan_cell_list_T * pscan_cell_list_ptr,
  ARFCN_T ARFCN
)
{
  if (pscan_cell_list_ptr)
  {
    int i;

    /* find the appropriate entry and remove from list */
    for (i = 0; i < pscan_cell_list_ptr->no_of_entries; ++i)
    {
      if (ARFCNS_EQUAL(ARFCN, pscan_cell_list_ptr->cell_list[i]))
      {
        /* this is the entry to be removed */
        /* decrement the size of the list */
        pscan_cell_list_ptr->no_of_entries -= 1;
        break;
      }
    }

    for (;i < pscan_cell_list_ptr->no_of_entries; ++i)
    {
      /* shift all subsequent entries down by one */
      pscan_cell_list_ptr->cell_list[i] = pscan_cell_list_ptr->cell_list[i + 1];
    }

    /* now clear out all remaining entries */
    for (;i < RR_MAX_RES_PSCAN_CELL_LIST_SIZE; ++i)
    {
      pscan_cell_list_ptr->cell_list[i].num = 0xFFFF;
      pscan_cell_list_ptr->cell_list[i].band = (sys_band_T) BAND_NONE;
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("pscan_cell_list_ptr == NULL");
  }
} /* grr_reselection_pscan_cell_list_remove_entry */

/****************************************************************************
 *   Function name:  grr_get_ncell_priority_class
 *   -------------
 *
 *   Description:
 *   ------------
 *    Gets the priority class for a given ncell. If the cell is in the form
 *    of a pbcch ncell then the priority class will be in the database. If
 *    the cell is a bcch cell then it is always defaulted to 0.
 *
 *   Parameters: bcch arfcn
 *   ----------
 *
 *   Returns:  uint8 priority_class
 *   --------
 *
 ****************************************************************************/
uint8 grr_get_ncell_priority_class(
  ARFCN_T bcch_arfcn,
  const gas_id_t gas_id
)
{
  uint8 priority = 0;
  gsm_ncell_list_member_t *ptr_ncell_data =
    rr_gprs_get_best_six_by_arfcn(
      bcch_arfcn,
      NULL,
      gas_id
    );

  if (ptr_ncell_data &&
      (ptr_ncell_data->bcch_or_pmo == PMO_FORMAT) &&
      ptr_ncell_data->ncell_data.pmo_pcco.gprs_cell_selection_params.hcs_params_present)
  {
    priority = ptr_ncell_data->ncell_data.pmo_pcco.gprs_cell_selection_params.hcs_params.priority_class;
  }

  return priority;
}


/**
  @brief Deletes the element at the specified index from the specified
         GPRS Reselection List.
  @param reselection_list The reselection list to modify.
  @param element_to_delete The index of the element to delete.
*/
void rr_gprs_reselection_list_delete_index(
  gprs_reselection_structure_T * reselection_list,
  uint8 element_to_delete
)
{
  if (reselection_list == NULL)
  {
    MSG_GERAN_ERROR_0("reselection_list == NULL");
    return;
  }

  if (reselection_list->no_of_entries == 0)
  {
    MSG_GERAN_ERROR_1("Requested to delete index %d from zero length list", element_to_delete);
    return;
  }
  else if (reselection_list->no_of_entries > RR_GPRS_RESELECTION_STRUCTURE_SIZE)
  {
    MSG_GERAN_ERROR_2("reselection_list->no_of_entries (%d) out of range, truncating to %d",
              reselection_list->no_of_entries,
              RR_GPRS_RESELECTION_STRUCTURE_SIZE);
    reselection_list->no_of_entries = RR_GPRS_RESELECTION_STRUCTURE_SIZE;
  }
  else if (element_to_delete >= reselection_list->no_of_entries)
  {
    MSG_GERAN_ERROR_2("Requested to delete index %d from list with only %d entries",
              element_to_delete, reselection_list->no_of_entries);
    return;
  }

  reselection_list->no_of_entries--;

  /* If we have been requested to delete the last entry in the list we do
  not need to take any further action, otherwise we need to move all higher
  indexed entries down one to fill the gap left by the deleted entry. */
  if ((element_to_delete == reselection_list->no_of_entries) ||
   (element_to_delete == (RR_GPRS_RESELECTION_STRUCTURE_SIZE-1)))
  {
    return;
  }
  else
  {
    /* Need to move all higher entries down the reselection list. */
	
    if (element_to_delete < RR_GPRS_RESELECTION_STRUCTURE_SIZE-1)
    {
      if (reselection_list->no_of_entries > 0)
      {
        int element_to_copy_from = element_to_delete+1;
        /* Replace memmove with memsmove */
        memsmove(&reselection_list->surrounding_values[element_to_delete],
                 (sizeof(reselection_list->surrounding_values)-(element_to_delete*sizeof(reselection_list->surrounding_values[0]))),
                 &reselection_list->surrounding_values[element_to_copy_from],
                 sizeof(gprs_reselection_values_T) * (reselection_list->no_of_entries - element_to_delete));
      }
    }
  }
}


/**
  @brief Iterates over the GPRS reselection list searching for an entry with
         the specified ARFCN.  The first such occurence is deleted from the list.

  @arfcn The ARFCN to delete.
*/
void remove_entry_from_reselection_list(
  ARFCN_T arfcn,
  const gas_id_t gas_id
)
{
  uint8                          i;
  gprs_reselection_structure_T * reselection_list = rr_get_gprs_reselection_list_ptr(gas_id);

  for (i = 0;
       i < reselection_list->no_of_entries && i < RR_GPRS_RESELECTION_STRUCTURE_SIZE;
       ++i)
  {
    if (ARFCNS_EQUAL(reselection_list->surrounding_values[i].BCCH_ARFCN, arfcn))
    {
      rr_gprs_reselection_list_delete_index(reselection_list, i);
      break;
    }
  }
}



/****************************************************************************
 *   Function name:  rr_clear_better_cell_list
 *   -------------
 *
 *   Description:
 *   ------------
 *   Cleans out the list of better cells that was built during reselection
 *
 *   Parameters:
 *   ----------
 *
 *   Returns:
 *   --------
 *
 ****************************************************************************/
void rr_clear_better_cell_list(
  const gas_id_t gas_id
)
{
  uint8                    loop_index;
  gprs_better_cell_list_T *better_cell_list_ptr = rr_get_better_cell_list_ptr(gas_id);

  for (loop_index = 0; loop_index < MAX_NUMBER_OF_BEST_CELLS; loop_index++)
  {
    better_cell_list_ptr->candidate_cell[loop_index].cell_reselection_attempted = FALSE;
    better_cell_list_ptr->candidate_cell[loop_index].reselection_data.BCCH_ARFCN.num = INVALID_ARFCN;
    better_cell_list_ptr->candidate_cell[loop_index].reselection_data_with_CRH.BCCH_ARFCN.num = INVALID_ARFCN;
    better_cell_list_ptr->candidate_cell[loop_index].reselection_data.BCCH_ARFCN.band = (sys_band_T)BAND_NONE;
    better_cell_list_ptr->candidate_cell[loop_index].reselection_data_with_CRH.BCCH_ARFCN.band = (sys_band_T)BAND_NONE;
  }

  better_cell_list_ptr->no_of_entries = 0;

}

/****************************************************************************
 *   Function name:  grr_any_other_reselection_candidates
 *   -------------
 *
 *   Description:
 *   ------------
 *    Looks to see if any of the ncells have 5sec timers running, and if so
 *    does that ncell have a better C2 than the ncell passed into the function
 *
 *   Parameters: bcch arfcn
 *   ----------
 *
 *   Returns:  boolean yes/no better ncells exist
 *   --------
 *
 ****************************************************************************/
boolean grr_any_other_reselection_candidates(
  ARFCN_T ref_arfcn,
  const gas_id_t gas_id
)
{
  gprs_reselection_values_T *ref_values;
  gprs_reselection_values_T *candidate_values;
  bcch_ncell_info_t         *surround_db_entry_ptr = NULL;
  best_six_ncells_t         *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
  boolean                    better_candidate_found = FALSE;
  uint8                      index;

  /* foreach cell in the surrounding database */
  for (index = 0; index < best_six_ncells_ptr->no_of_entries; index++)
  {
    /*point to the surround SI entry*/
    surround_db_entry_ptr = &best_six_ncells_ptr->ncell_entry[index].ncell_data->ncell_data.bcch;

    if ( ARFCNS_EQUAL( ref_arfcn, surround_db_entry_ptr->gsm.BCCH_ARFCN ))
    {
      // skip this entry it's the reference
      continue;
    }
    else
    {
      if (rr_timer_is_running(
            best_six_ncells_ptr->ncell_entry[index].reselection_info.five_second_timer_id,
            gas_id
          ))
      {
        ref_values = ptr_ncell_reselection_list(ref_arfcn, gas_id);
        candidate_values = ptr_ncell_reselection_list(surround_db_entry_ptr->gsm.BCCH_ARFCN, gas_id);

        if (ref_values != NULL && candidate_values != NULL)
        {
          if ( candidate_values->C2 > ref_values->C2 )
          {
            better_candidate_found = TRUE;
          }
        }
        else
        {
          MSG_GERAN_ERROR_2("ref_values (%p) or candidate_values (%p) == NULL", ref_values, candidate_values);
        }
      }
    }
  }

  return better_candidate_found;
}

/****************************************************************************
 *   Function name:  ptr_ncell_reselection_list
 *   -------------
 *
 *   Description:
 *   ------------
 *    Finds an entry corresponding to the arfcn in the reselection list
 *
 *   Parameters: bcch arfcn
 *   ----------
 *
 *   Returns:  gprs_reselection_values_T * to the reselection data for the ncell
 *   --------
 *
 ****************************************************************************/
gprs_reselection_values_T * ptr_ncell_reselection_list(
  ARFCN_T arfcn,
  const gas_id_t gas_id
)
{
  uint8                         i;
  gprs_reselection_structure_T *gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  for (i = 0; i < gprs_reselection_list_ptr->no_of_entries; i++)
  {
    if (ARFCNS_EQUAL(arfcn, gprs_reselection_list_ptr->surrounding_values[i].BCCH_ARFCN))
    {
      return &gprs_reselection_list_ptr->surrounding_values[i];
    }
  }

  return NULL;
}

/****************************************************************************
 *   Function name:  rr_gprs_check_if_any_g2x_resel_timer_is_running
 *   --------------
 *
 *   Description: This function checks if any on going IRAT G2W Reselection Timer is 
 *   -----------  still running or not ? Returns TRUE, if any one of the timer is running, 
 *                FALSE otherwise. 
 *
 *   Parameters: None
 *   -----------
 *
 *   Returns:  boolean: Returns if any IRAT G2W Timer is running 
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_check_if_any_g2x_resel_timer_is_running(const gas_id_t gas_id )
{
  boolean g2x_timer_running = FALSE;

  
  if (
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_1, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_2, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_3, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_4, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_5, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_6, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_7, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_8, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_9, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_10, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_11, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_12, gas_id) ||
        
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_1, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_2, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_3, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_4, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_5, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_6, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_7, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_8, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_9, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_10, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_11, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_12, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_1, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_2, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_3, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_4, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_5, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_6, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_7, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_8, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_9, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_10, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_11, gas_id) ||
        rr_timer_is_running(RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_12, gas_id) ||
 
        rr_timer_is_running(RR_G2WR_FIVE_SEC_TIMER_1, gas_id) ||
        rr_timer_is_running(RR_G2WR_FIVE_SEC_TIMER_2, gas_id) ||
        rr_timer_is_running(RR_G2WR_FIVE_SEC_TIMER_3, gas_id) ||
        rr_timer_is_running(RR_G2WR_FIVE_SEC_TIMER_4, gas_id) ||
        rr_timer_is_running(RR_G2WR_FIVE_SEC_TIMER_5, gas_id) ||
        rr_timer_is_running(RR_G2WR_FIVE_SEC_TIMER_6, gas_id) 
  )
 {
   g2x_timer_running = TRUE; 
 }


  return ( g2x_timer_running );

}

/* EOF */

