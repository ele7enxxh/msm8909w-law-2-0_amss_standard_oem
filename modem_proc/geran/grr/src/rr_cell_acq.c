/*============================================================================
  FILE:         rr_cell_acq.c

  OVERVIEW:     This module provides functions to control GSM cell acquisition

  DEPENDENCIES: None

                Copyright (c) 2002-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_cell_acq.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_cell_acq_if.h"
#include "rr_rr.h"
#include "rr_general.h"
#include "rr_resel_calcs.h"
#include "rr_cell_acq.h"
#include "rr_l1_send.h"
#include "rr_resel.h"
#include "rr_resel_utils.h"
#include "rr_sys_info_ncell.h"
#include "rr_sys_info_pcell.h"
#include "rr_resel_nacc.h"
#include "rr_multi_sim.h"
#include "rr_mac_send.h"
#include "rr_ps_access.h"
#include "rr_l1_idle_mode.h"
#include "rr_sys_info_cache.h"
#include "rr_x2g_resel.h"
#include "rr_nv.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#define FEATURE_RESELECT_ON_SI3_RECONF_TIMEOUT

#define BCCH_SI3_SI4_TIMEOUT                    4000
#define BCCH_SI13_TIMEOUT                       8000
#define SI3_SI4_RESTART_TIMEOUT                 6000

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/


typedef enum
{
  RR_CELL_ACQ_STATE_NULL,
  RR_CELL_ACQ_STATE_SURROUND_UPDATE,
  RR_CELL_ACQ_STATE_SUSPENDING_MAC,
  RR_CELL_ACQ_STATE_SUSPENDING_MAC_ABORTED,
  RR_CELL_ACQ_STATE_RESELECTING,
  RR_CELL_ACQ_STATE_RESELECTING_ABORTED,
  RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM,
  RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM_EARLY_CAMPED,
  RR_CELL_ACQ_STATE_ACQUIRING_BSI_GPRS_EARLY_CAMPED,
  RR_CELL_ACQ_STATE_WAITING_FOR_IDLE_MODE_CNF,
  RR_CELL_ACQ_STATE_MAX
} rr_cell_acq_state_e;


typedef enum
{
  RR_CELL_ACQ_EARLY_CAMPING_NOT_STARTED,
  RR_CELL_ACQ_EARLY_CAMPING_REQ_SENT,
  RR_CELL_ACQ_EARLY_CAMPING_COMPLETED
} rr_early_camping_status_e;


typedef struct
{
  rr_cell_acq_state_e               state;
  rr_cell_acq_state_e               old_state;
  ARFCN_T                           target_cell;
  uint8                             RXLEV_average;
  boolean                           bsic_valid;
  BSIC_T                            bsic;
  boolean                           recent_reselection_flag;
  rr_cell_acq_action_e              action;
  boolean                           sync_flag;
  gprs_scell_info_t                *gprs_cell_info;
  rr_early_camping_status_e         early_camping_status;
  boolean                           clear_si_db;
  boolean                           requested_plmn_valid;
  PLMN_id_T                         requested_plmn;
  boolean                           restart_sys_info_timer;
} rr_cell_acq_data_t;


/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

extern const int GPRS_RESELECT_OFFSET_dB[32];

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

static rr_cell_acq_data_t rr_cell_acq_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_cell_acq_data_t*
 */
static rr_cell_acq_data_t *rr_cell_acq_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_cell_acq_data[as_index];
}

/*!
 * \brief Initialisation function
 *
 * \param gas_id
 */
static void rr_cell_acq_init(const gas_id_t gas_id)
{
  // Obtain the pointer to the module data
  rr_cell_acq_data_t *rr_cell_acq_data_ptr = rr_cell_acq_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_cell_acq_data_ptr);

  rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
  rr_cell_acq_data_ptr->old_state = RR_CELL_ACQ_STATE_NULL;
}

static boolean rr_cell_acq_control_state_is_transitory(rr_cell_acq_state_e state)
{
  switch (state)
  {
    case RR_CELL_ACQ_STATE_SUSPENDING_MAC:
    case RR_CELL_ACQ_STATE_SUSPENDING_MAC_ABORTED:
    case RR_CELL_ACQ_STATE_RESELECTING:
    case RR_CELL_ACQ_STATE_RESELECTING_ABORTED:
    case RR_CELL_ACQ_STATE_WAITING_FOR_IDLE_MODE_CNF:
    {
      return TRUE;
    }

    default:
    {
      return FALSE;
    }
  }
}

static void rr_cell_acq_reset_parameters(const gas_id_t gas_id)
{
  (void)rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id);

  rr_csi_deactivate_pend_store(gas_id);

  rr_cell_acq_init(gas_id);
}


/**
 * Called to suspend GPRS. This uses the RR-PS-ACCESS module to provide this functionality.
 */
static void rr_cell_acq_suspend_gprs(const gas_id_t gas_id)
{
  // Send request to block PS access
  rr_ps_send_imsg_no_ps_access_req(
    FALSE,     // suspend_gmm
    NO_DEL,    // delete_pdu
    gas_id     // gas_id
  );

  // Once PS access is blocked, RR_PS_OMSG_NO_PS_ACCESS_CNF will be sent.
  // If messages to this module are coming via RR-RESEL, then the confirrmation will be EV_MAC_IDLE_MODE.
  // If not, RR_PS_OMSG_NO_PS_ACCESS_CNF will be received.
  // So both these inputs should be treated as equivalent.
}


/****************************************************************************
 *   Function name:  rr_gprs_surround_update_SI3_SI4_for_ARFCN
 *   -------------
 *
 *   Description: Sends a request to L1 to receive the SI3 or SI4 from a surround
 *   ------------ cell. The system information handler is also set up ready
 *                to receive the data.
 *
 *   Parameters: Target cells ARFCN
 *   -----------
 *
 *   Returns:  void
 *   --------
 *
 ****************************************************************************/
static void rr_gprs_surround_update_SI3_SI4_for_ARFCN(ARFCN_T BCCH_ARFCN, const gas_id_t gas_id)
{
  gprs_surround_data_t          gprs_surround_data;
  bcch_ncell_info_t            *bcch_ncell_info;
  gprs_reselection_structure_T *gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
  boolean                       si_from_cache;
  #define WEAK_CELL_RXLEV 30 // -80dB

  /************************************************************/
  /* Take L1 out of paging re-org if the scell is not too hot */
  /* If the surround read fails then the next paging message  */
  /* will re-instigate the correct paging mode.               */
  /************************************************************/
  if (gprs_reselection_list_ptr->serving_values.RXLEV_average < WEAK_CELL_RXLEV)
  {
    if (rr_ce_compare_and_update_l1_page_mode(PAGING_REORGANIZATION, NORMAL_PAGING, gas_id))
    {
      MSG_GERAN_HIGH_0_G("Force Normal Paging for reselection");
    }
  }

  /* Check for a matching BCCH AFRCN in the surround database */
  if ((bcch_ncell_info = rr_gprs_get_surround_ncell_sys_info(BCCH_ARFCN, gas_id)) != NULL)
  {
    /* want new SI for this cell, consider any existing information invalid */
    bcch_ncell_info->gsm.update_message_received = 0;
    bcch_ncell_info->gsm.si_expected = TRUE;
    bcch_ncell_info->gprs_supported = FALSE;
    /* set this to false as Si13 may have been received before for this cell */
    bcch_ncell_info->gprs.si13_data_valid_flag = FALSE;
    /*************************************************************************/
    /* invalidate the CCCH description, so if Si4 comes in instead of SI3 we */
    /* don't consider it valid and reuse it for early camping                */
    /*************************************************************************/
    bcch_ncell_info->gsm.control_channel_description_valid = FALSE;
  }

  (void)rr_timer_start(RR_GPRS_SYS_INFO_TIMER, BCCH_SI3_SI4_TIMEOUT, gas_id);

  /*Request L1 to read the candidate surrounding ncells SI3 and 4*/
  gprs_surround_data.bcch_params.bcch_arfcn = BCCH_ARFCN;
  gprs_surround_data.bcch_params.si13_normal_or_ext = BCCH_NORMAL;
  gprs_surround_data.bcch_params.si_requested = GRR_L1_DECODE_SI3_ONLY;

  /*Send request to L1*/
  si_from_cache = rr_send_mph_gprs_surround_update_req(&gprs_surround_data, TRUE, bcch_ncell_info, gas_id);

  /* Set up the surround response handler */
  set_waiting_for_surround_response( BCCH_ARFCN, si_from_cache, gas_id );

  return;
}

/****************************************************************************
 *   Function name:  rr_cell_acq_early_camping_confirmed
 *   --------------------------
 *
 *   Description: Called when L1 confirms early camping
 *   ------------
 *
 *   Parameters: none
 *   -----------
 *
 *   Returns:  nothing
 *   --------
 *
 ****************************************************************************/
static void rr_cell_acq_early_camping_confirmed(const gas_id_t gas_id)
{
  rr_cell_acq_data_t *rr_cell_acq_data_ptr = rr_cell_acq_get_data_ptr(gas_id);

  rr_cell_acq_data_ptr->early_camping_status = RR_CELL_ACQ_EARLY_CAMPING_COMPLETED;

  /*
  Unless the ccd is invalidated there will be no indication of RR_EV_EARLY_CAMP_SI_RECEIVED
  which is needed for immediate reselections to check that the cell is suitable
  */
  rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description_valid = FALSE;
}


/**
  @brief Determine whether the target cell is suitable for selection.

  @return TRUE if the target cell is suitable for reselect, FALSE otherwise.
*/
static boolean rr_cell_acq_is_cell_suitable(
  ARFCN_T                arfcn,
  byte                   RXLEV_average,
  rr_cell_acq_status_e * status,
  const gas_id_t         gas_id
)
{
  rr_cell_acq_data_t *rr_cell_acq_data_ptr = rr_cell_acq_get_data_ptr(gas_id);
  rr_cell_type_T       cell_type;
  int                  C1 = 0;

  /***************************************************/
  /* get the cell priority for this surrounding cell */
  /* to make sure its not barred                     */
  /***************************************************/
  cell_type = rr_get_cell_priority(
    rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_selection_parameters.CELL_BAR_QUALIFY,
    rr_cell_acq_data_ptr->gprs_cell_info->gsm.RACH_control_parameters.CELL_BAR_ACCESS,
    &rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification,
    rr_cell_acq_data_ptr->gprs_cell_info->gsm.RACH_control_parameters.access_control,
    NULL,
    gas_id
  );

  if (cell_type == RR_CELL_BARRED)
  {
    MSG_GERAN_HIGH_0_G("Aborting acquisition, cell barred");
    if (status != NULL)
    {
      *status = RR_CELL_ACQ_STATUS_FAILURE_CELL_BARRED;
    }
    return FALSE;
  }

  /***********************************************************************/
  /* We have to calculate the C1 and C2 parameters again, with the new   */
  /* system information to see if the cell si still the best target      */
  /***********************************************************************/
  /******************************************************************/
  /* Calculate only the C1 parameter for the potential serving cell */
  /******************************************************************/
  C1 = rr_get_C1(
    rr_cell_acq_data_ptr->gprs_cell_info->gsm.BCCH_ARFCN,
    RXLEV_average,
    rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
    rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
    rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_selection_parameters.POWER_OFFSET,
    gas_id
  );

  if (rr_allow_negative_c1(
        rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification.plmn_id,
        gas_id))
  {
    if (C1 < 0)
    {
      MSG_GERAN_HIGH_1_G("C1 Relaxation, ignoring low C1 value %d", C1);
    }
    else
    {
      MSG_GERAN_HIGH_1_G("C1 Relaxation, C1 value %d ok", C1);
    }
  }
  else
  {
    /*********************************/
    /* If C1 >= 0 then proceed with  */
    /* the reselection to this cell  */
    /*********************************/
    if (C1 < 0)
    {
      MSG_GERAN_HIGH_1_G("Aborting acquisition, C1 (%d) < 0", C1);
      if (status != NULL)
      {
        *status = RR_CELL_ACQ_STATUS_FAILURE_CELL_LOW_C1;
      }
      return FALSE;
    }
  }

  if (rr_cell_acq_data_ptr->requested_plmn_valid &&
      !sys_plmn_id_is_undefined(rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification.plmn_id))
  {
    /* Check if the target cell's PLMN matches our requested PLMN. */
    boolean plmn_ok = sys_plmn_match(
      rr_cell_acq_data_ptr->requested_plmn,
      rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification.plmn_id
    );

    if (!plmn_ok)
    {
      /* Check if both the target cell's PLMN and our requested PLMN are in
      the equivalent PLMN list. */
      plmn_ok = rr_eplmn_list_equivalent_plmn(rr_cell_acq_data_ptr->requested_plmn, gas_id) &&
                rr_eplmn_list_equivalent_plmn(rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification.plmn_id, gas_id);
    }

    if (!plmn_ok)
    {
      MSG_GERAN_HIGH_0_G("Aborting acquisition, PLMN mismatch");
      if (status != NULL)
      {
        *status = RR_CELL_ACQ_STATUS_FAILURE_PLMN_MISMATCH;
        return FALSE;
      }
    }
  }

  /* Always check the Forbidden LAI list in reselection, regardless manual/automatic */
  if (rr_is_any_cell_reselection_mode(gas_id) == FALSE)
  {
    if (rr_is_forbidden_LAI(&rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification, gas_id))
    {
      MSG_GERAN_HIGH_0_G("Aborting acquisition, forbidden LAI");
      if (status != NULL)
      {
        *status = RR_CELL_ACQ_STATUS_FAILURE_FORBIDDEN_LAI;
        return FALSE;
      }
    }
  }

  return TRUE;
}

/**
  @brief Verify whether the target cell is suitable for cell reselection.

  @param recent_reselection_flag TRUE if reselection has been performed
         recently, FALSE otherwise.  Applies cell reselection hysteresis if
         necessary.

  @return TRUE if the cell is suitable for reselection, FALSE otherwise.
*/
static boolean rr_gprs_handle_surround_SI3_SI4(
  boolean recent_reselection_flag,
  const gas_id_t gas_id
)
{
  int                            C1 = 0;
  int                            C2 = 0;
  boolean                        surrounding_better_than_serving = FALSE;
  bcch_ncell_info_t *            surround_db_entry_ptr = NULL;
  gprs_scell_info_t *            scell_info = rr_gprs_get_scell_info(gas_id);
  uint8                          ncell_priority_class;
  uint8                          scell_priority_class;
  rr_resel_candidate_t *         rr_resel_candidate;
  gprs_reselection_structure_T * gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
  rr_cell_acq_data_t           * rr_cell_acq_data_ptr = rr_cell_acq_get_data_ptr(gas_id);

  surround_db_entry_ptr = rr_gprs_get_surround_ncell_sys_info(rr_cell_acq_data_ptr->target_cell, gas_id);

  rr_resel_candidate = rr_resel_best_six_find(
    rr_cell_acq_data_ptr->target_cell,
    NULL,
    gas_id
  );

  if (rr_resel_candidate == NULL)
  {
    MSG_GERAN_ERROR_2_G("Failed to find best six entry for ARFCN %d (band %d)",
              rr_cell_acq_data_ptr->target_cell.num,
              rr_cell_acq_data_ptr->target_cell.band);
    return FALSE;
  }

  MSG_GERAN_MED_1_G("Checking reselected cell: %d",CONVERT_FROM_ARFCN_T(rr_cell_acq_data_ptr->target_cell));

  if (rr_gprs_resel_suitable_cell(surround_db_entry_ptr, gas_id))
  {
    if (rr_gprs_parameters_available(gas_id) == FALSE)
    {
      /***********************************************************************/
      /* We have to calculate the C1 and C2 parameters again, with the new   */
      /* system information to see if the cell is still the best target      */
      /***********************************************************************/
      /*calculate C1 for the target cell*/
      C1 = rr_get_C1(
        surround_db_entry_ptr->gsm.BCCH_ARFCN,
        rr_cell_acq_data_ptr->RXLEV_average,
        surround_db_entry_ptr->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
        surround_db_entry_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
        surround_db_entry_ptr->gsm.cell_selection_parameters.POWER_OFFSET,
        gas_id
      );

      /****************************************************/
      /* Make sure that the cells C1 is greater than 0    */
      /* otherwise it has already failed the suitable cell*/
      /* criteria                                         */
      /****************************************************/
      if (C1 >= 0 || rr_allow_negative_c1(surround_db_entry_ptr->gsm.location_area_identification.plmn_id, gas_id))
      {
        /* calculate C2 for the target cell */
        C2 = rr_gprs_compute_surrounding_C2(C1, rr_resel_candidate, gas_id);

        MSG_GERAN_LOW_0_G("Comparing serving and updated SI surrounding C2s");

        /****************************************************/
        /* Make sure that the cell is still better than the */
        /* scell.                                           */
        /****************************************************/
        surrounding_better_than_serving = rr_gprs_compare_serving_and_surrounding_C2(
          gprs_reselection_list_ptr->serving_values.C2,
          C2,
          surround_db_entry_ptr,
          recent_reselection_flag,
          &(surround_db_entry_ptr->gsm.location_area_identification),
          gas_id
        );
      }
    }
    else
    {
      gprs_reselection_values_T local_fresh_reselection_values;
      /***********************************************************************/
      /* We have to calculate the C1 and C32 parameters again, with the new  */
      /* system information to see if the cell is still suitable             */
      /***********************************************************************/
      /********************************************/
      /* Convert the BCCH parameters as described */
      /* see table 3a GSM05.08                    */
      /* and calculate C1, C31 and C32            */
      /********************************************/
      int GPRS_TEMPORARY_OFFSET = 0;
      int GPRS_RESELECT_OFFSETn = 16; //this is zero
      int PRIORITY_CLASSs = 0;
      int PRIORITY_CLASSn = 0;

       /****************************************************************/
       /* If the cell selection parameters were present then load them */
       /* otherwise they will stay set as 0 and C32 will in effect be  */
       /* the same as C1.                                              */
       /****************************************************************/
       if (surround_db_entry_ptr->gsm.cell_selection_parameters.CELL_RESELECT_PARAM_IND)
       {
         uint8 ro_index;
         GPRS_TEMPORARY_OFFSET = surround_db_entry_ptr->gsm.cell_selection_parameters.TEMPORARY_OFFSET;
         GPRS_RESELECT_OFFSETn = surround_db_entry_ptr->gsm.cell_selection_parameters.CELL_RESELECT_OFFSET*2 -
                                   scell_info->gsm.cell_selection_parameters.CELL_RESELECT_OFFSET*2;

         /* convert the reselection offset to a index value to represent the gprs_reselect_offset */
         for (ro_index=0; ro_index<32; ro_index++)
         {
           if (GPRS_RESELECT_OFFSET_dB[ro_index] > GPRS_RESELECT_OFFSETn)
           {
             GPRS_RESELECT_OFFSETn = ro_index-1;
             break;
           }
         }
       }

      /********************************************/
      /* Calculate the C1 parameter for the ncell */
      /********************************************/
      local_fresh_reselection_values.C1 = rr_get_C1(
        surround_db_entry_ptr->gsm.BCCH_ARFCN,
        rr_cell_acq_data_ptr->RXLEV_average,
        surround_db_entry_ptr->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
        surround_db_entry_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
        0, /*Not used for GPRS cell reselection 5.08 10.1.2*/
        gas_id
      );

      /****************************************************/
      /* Make sure that the cells C1 is greater than 0    */
      /* otherwise it has already failed the suitable cell*/
      /* criteria                                         */
      /****************************************************/
      if ((local_fresh_reselection_values.C1 >= 0 || rr_allow_negative_c1(surround_db_entry_ptr->gsm.location_area_identification.plmn_id, gas_id)) &&
          (rr_cell_acq_data_ptr->action == RR_CELL_ACQ_ACTION_RESELECT_BCCH_AFTER_SU))
      {
        /********************************************/
        /* On BCCH the C31 parameter is set to zero */
        /* see table 3a GSM05.08                    */
        /********************************************/
        local_fresh_reselection_values.C31 = 0;

        /*********************************************/
        /* Calculate the C32 parameter for the ncell */
        /*********************************************/
        local_fresh_reselection_values.C32 = rr_gprs_surround_calculate_C32(
          local_fresh_reselection_values.C1,
          GPRS_TEMPORARY_OFFSET,
          GPRS_RESELECT_OFFSETn,
          PRIORITY_CLASSs,
          PRIORITY_CLASSn,
          rr_resel_candidate->penalty_timer_id, /*only needed for ncells*/
          surround_db_entry_ptr->gsm.cell_selection_parameters.PENALTY_TIME, /*only needed for ncells*/
          FALSE,/*Doesn't matter on 51 frame - C32_QUAL only*/
          gas_id
        );

        /*Now store the values in a reselection structure in preparation for the comparision*/
        local_fresh_reselection_values.RXLEV_average = rr_cell_acq_data_ptr->RXLEV_average;
        local_fresh_reselection_values.BCCH_ARFCN = rr_cell_acq_data_ptr->target_cell;

        /*******************************************************************/
        /* Decide weather to include HCS reselection in better cell checks */
        /*******************************************************************/
        if(scell_info->bcch_sys_info.pmo_scell_params_valid &&
            scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params_present )
        {
          ncell_priority_class = 0;
          scell_priority_class = scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params.priority_class;

          /***********************************************************/
          /* Check that the C31 of the scell is greater than or equal*/
          /* to zero. If it isn't then according to GSM05.08 it      */
          /* cannot be amongst the best cells as there is already a  */
          /* ncell that has C31 >= 0                                 */
          /***********************************************************/
          if((gprs_reselection_list_ptr->serving_values.C31 < 0 ) && ( local_fresh_reselection_values.C31 >= 0))
          {
            /*The scell has C31 < 0 and is therefore not better than the ncell*/
            MSG_GERAN_MED_1_G("Ncell C31 >= 0, Scell C31 < 0.Ncell %d better", CONVERT_FROM_ARFCN_T(local_fresh_reselection_values.BCCH_ARFCN));
            surrounding_better_than_serving = TRUE;
          }

          /**********************************************************/
          /* Compare the PRIORITY_CLASS of the ncell with that of   */
          /* the scells. If greater this alone make the ncell better*/
          /**********************************************************/
          if((scell_priority_class < ncell_priority_class) && surrounding_better_than_serving == FALSE)
          {
            /*The ncell has C31 > 0 and a higher PRIORITY_CLASS than the SCELL so it must
            qualify as a better cell*/
            MSG_GERAN_MED_1_G("Ncell Scell C31>=0, ARFCN%d is higher priority",CONVERT_FROM_ARFCN_T(local_fresh_reselection_values.BCCH_ARFCN));
            surrounding_better_than_serving = TRUE;
          }

          /************************************************************/
          /* Compare the C32 values of the scell and the ncell. If it */
          /* is greater than the C32 of the scell, then the ncell must*/
          /* be of equal PRIORITY CLASS to be a better cell           */
          /************************************************************/
          if((scell_priority_class == ncell_priority_class)
              && surrounding_better_than_serving == FALSE)
          {
            surrounding_better_than_serving = rr_gprs_compare_serving_and_surrounding_C32(
                                                              &local_fresh_reselection_values,
                                                              &gprs_reselection_list_ptr->serving_values,
                                                              recent_reselection_flag,
                                                              gas_id);
          }
        }
        else /* don't perform HCS checks */
        {
          /***************************************************************/
          /* The computed C32 values for both the surrounding and serving*/
          /* cells are available, perform the comparison using the values*/
          /* in the current reselection structure                        */
          /***************************************************************/
          surrounding_better_than_serving = rr_gprs_compare_serving_and_surrounding_C32(
                                                           &local_fresh_reselection_values,
                                                           &gprs_reselection_list_ptr->serving_values,
                                                           recent_reselection_flag,
                                                           gas_id);
        }
      }
      else if (local_fresh_reselection_values.C1 >= 0 || rr_allow_negative_c1(surround_db_entry_ptr->gsm.location_area_identification.plmn_id, gas_id))
      {
        /***************************************************/
        /* For reselection reasons other than better cell  */
        /* the C1 value must be >= 0 for it to be a viable */
        /* candidate. It doeasn't need to be compared to   */
        /* the old scell.                                  */
        /***************************************************/
        surrounding_better_than_serving = TRUE;
      }
    }

    if(surrounding_better_than_serving)
    {
      /*******************************************************/
      /* This is still the best cell carry on reselecting it */
      /*******************************************************/
      MSG_GERAN_MED_1_G("ncell (%d) better",CONVERT_FROM_ARFCN_T(rr_cell_acq_data_ptr->target_cell));
      return TRUE;
    }
    else
    {
      /*******************************************/
      /* The cell was not better than the scell  */
      /*******************************************/
      MSG_GERAN_MED_1_G("ncell (%d) no better",CONVERT_FROM_ARFCN_T(rr_cell_acq_data_ptr->target_cell));
      return FALSE;
    }
  }
  else /*The target cell was barred*/
  {
    /********************************************************************/
    /* Kill reselection to this target as it is not suitable move on to */
    /* the next best cell stop the sys info handler from waiting for the*/
    /* SI13 from the ncell                                              */
    /********************************************************************/
    return FALSE;
  }
}

/****************************************************************************
 *   Function name:  rr_gprs_action_cell_reselection
 *   -------------
 *
 *   Description: Performs the send of the MPH_GPRS_RESELECTION_REQ to L1 to
 *   ------------ action the reselection. Data is loaded into the primitive
 *                a number of ways depending upon the target multiframe and
 *                the data that exists from PSIs or SIs.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  TRUE if RESELECTION_REQ or SELECT_SPECIFIC was sent to L1
 *   --------
 *
 ****************************************************************************/
static void rr_gprs_action_cell_reselection(const gas_id_t gas_id)
{
  gprs_surround_data_t gprs_surround_data;
  BSIC_T BSIC = {INVALID_PLMN_CC, INVALID_BSIC_CC};
  rr_cell_acq_data_t *rr_cell_acq_data_ptr = rr_cell_acq_get_data_ptr(gas_id);

  /* Set the union to zero. Preferred by LINT */
  memset(&gprs_surround_data, 0, sizeof(gprs_surround_data_t));
  rr_cell_acq_data_ptr->restart_sys_info_timer = FALSE;

  switch (rr_cell_acq_data_ptr->action)
  {
    case RR_CELL_ACQ_ACTION_RESELECT_BCCH_NACC_PCCO_PCCC:
    case RR_CELL_ACQ_ACTION_RESELECT_BCCH_PCCO_FAILED:
    {
      /************************************************************/
      /* This path is taken for a cell reslection to a BCCH/CCCH  */
      /* The system information handler is prepared and then the  */
      /* reselection request is sent to L1.                       */
      /************************************************************/
      /* Set up the pcell to receive the data*/
      rr_cell_acq_data_ptr->gprs_cell_info->gsm.generate_si_received_event = TRUE;

      /*Request L1 to reselect to the ncells*/
      CONVERT_TO_ARFCN_T(gprs_surround_data.bcch_params.bcch_arfcn,
                         rr_cell_acq_data_ptr->target_cell);
      gprs_surround_data.bcch_params.si13_normal_or_ext = BCCH_NORMAL;

      /*
      Send reselection request to L1. Sync flag is not set if the reselection is for better
      cell.
      */
      if (rr_cell_acq_data_ptr->sync_flag )
      {
        /*L1 is sync'ed up to the target cell*/
        if (rr_cell_acq_data_ptr->action == RR_CELL_ACQ_ACTION_SELECT_BCCH)
        {
          rr_send_mph_select_specific_channel(rr_cell_acq_data_ptr->target_cell,BSIC, gas_id);
        }
        else
        {
          rr_send_mph_gprs_reselection_req(&gprs_surround_data, FALSE, gas_id);
        }

        /*Clear out the list of synced cells*/
        rr_gprs_clear_sync_cell_list(gas_id);
      }
      else
      {
        /*L1 has no sync with the target cell*/
        if (rr_cell_acq_data_ptr->action == RR_CELL_ACQ_ACTION_SELECT_BCCH)
        {
          if ( rr_log_reselection_log_complete(gas_id) )
          {
            /* Start reselection logging if not already done */
            rr_log_reselect_start(RR_RESELECTION_TRIGGER_NONE, gas_id);
          }
          rr_send_mph_select_specific_channel(rr_cell_acq_data_ptr->target_cell,BSIC, gas_id);

        }
        else
        {
          rr_send_mph_gprs_reselection_req(&gprs_surround_data, TRUE, gas_id);
        }
      }

      break;
    }

    case RR_CELL_ACQ_ACTION_SELECT_BCCH:
    case RR_CELL_ACQ_ACTION_RESELECT_BCCH:
    case RR_CELL_ACQ_ACTION_RESELECT_BCCH_AFTER_SU:
    case RR_CELL_ACQ_ACTION_RESELECT_BCCH_WITH_GPRS_CELL_SELECTION_PARAMS:
    {
      /************************************************************/
      /* This path is taken for a cell reslection to a BCCH/CCCH  */
      /* The system information handler is prepared and then the  */
      /* reselection request is sent to L1.                       */
      /************************************************************/

      /* Set up the pcell to receive the data*/
      rr_cell_acq_data_ptr->gprs_cell_info->gsm.generate_si_received_event = TRUE;
      rr_cell_acq_data_ptr->gprs_cell_info->gsm.BCCH_ARFCN = rr_cell_acq_data_ptr->target_cell;
      rr_cell_acq_data_ptr->gprs_cell_info->gsm.valid_data_flag = FALSE;
      rr_cell_acq_data_ptr->gprs_cell_info->gsm.update_message_received = 0;
      rr_cell_acq_data_ptr->gprs_cell_info->bcch_sys_info.si13_data_valid_flag = FALSE;

      MSG_GERAN_MED_0_G("Starting RR_GPRS_SYS_INFO_TIMER");
      rr_cell_acq_data_ptr->restart_sys_info_timer = rr_timer_start(
        RR_GPRS_SYS_INFO_TIMER,
        BCCH_SI3_SI4_TIMEOUT,
        gas_id
      );

      /*Request L1 to reselect to the ncells*/
      CONVERT_TO_ARFCN_T(gprs_surround_data.bcch_params.bcch_arfcn,
                         rr_cell_acq_data_ptr->target_cell);
      gprs_surround_data.bcch_params.si13_normal_or_ext = BCCH_NORMAL;

      /*
      Send reselection request to L1. Sync flag is not set if the reselection is for better
      cell.
      */
      if (rr_cell_acq_data_ptr->sync_flag )
      {
        /*L1 is sync'ed up to the target cell*/
        if (rr_cell_acq_data_ptr->action == RR_CELL_ACQ_ACTION_SELECT_BCCH)
        {
          BSIC_T bsic = {0};
#ifdef FEATURE_LTE
          if (rr_cell_acq_data_ptr->bsic_valid)
          {
            bsic = rr_cell_acq_data_ptr->bsic;
          }
#endif /* FEATURE_LTE */
          rr_log_camping_attempt_start(
            rr_cell_acq_data_ptr->target_cell,
            bsic,
            gas_id
          );

          rr_send_mph_select_specific_channel(rr_cell_acq_data_ptr->target_cell,BSIC, gas_id);
        }
        else
        {
          rr_send_mph_gprs_reselection_req(&gprs_surround_data, FALSE, gas_id);
        }

        /*Clear out the list of synced cells*/
        rr_gprs_clear_sync_cell_list(gas_id);
      }
      else
      {
        /*L1 has no sync with the target cell*/
        if (rr_cell_acq_data_ptr->action == RR_CELL_ACQ_ACTION_SELECT_BCCH)
        {
          BSIC_T bsic = {0};
#ifdef FEATURE_LTE
          if (rr_cell_acq_data_ptr->bsic_valid)
          {
            bsic = rr_cell_acq_data_ptr->bsic;
          }
#endif /* FEATURE_LTE */
          rr_log_camping_attempt_start(
            rr_cell_acq_data_ptr->target_cell,
            bsic,
            gas_id
          );

          if ( rr_log_reselection_log_complete(gas_id) )
          {
            /* Start reselection logging if not already done */
            rr_log_reselect_start(RR_RESELECTION_TRIGGER_NONE, gas_id);
          }
          rr_send_mph_select_specific_channel(rr_cell_acq_data_ptr->target_cell,BSIC, gas_id);

        }
        else
        {
          rr_send_mph_gprs_reselection_req(&gprs_surround_data, TRUE, gas_id);
        }
      }

      break;
    }

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected action: %d", rr_cell_acq_data_ptr->action);
    }
  }

  /* It is possible that this reselection request was for a NACC procedure e.g. */
  /* an unsolicited PCCO with PNCD, or a normal NACC procedure that has failed. */
  /* In any case, notify the NACC state machine, so that it tracks the process. */
  rr_gprs_nacc_notify_event(CCN_REQUESTED_TO_L1, gas_id);
}

/**
 * handle_surround_update_failure_ind
 *
 * Verifies if the mph_surround_update_failure_ind is for the current target
 * cell and takes appropriate action.  The failure could be either because of
 * a state transition in L1 (IDLE->TRANSFER, TRANSFER->IDLE) or because L1 has
 * genuinely failed to acquire the system information.
 *
 */
static void handle_surround_update_failure_ind(
  const mph_surround_update_failure_ind_T *msg,
  ARFCN_T target_arfcn,
  const gas_id_t gas_id
)
{
  if (ARFCNS_EQUAL(msg->BCCH_ARFCN, target_arfcn))
  {
    rr_resel_candidate_t * resel_candidate;
    bcch_ncell_info_t *    bcch_ncell_db_ptr;

    MSG_GERAN_MED_1_G("Surround Failure for %d",
      CONVERT_FROM_ARFCN_T(target_arfcn));

    bcch_ncell_db_ptr = rr_gprs_get_surround_ncell_sys_info(target_arfcn, gas_id);

    if (bcch_ncell_db_ptr == NULL)
    {
      MSG_GERAN_ERROR_2_G("Failed to find ncell sys info for ARFCN %d (band %d)",
                target_arfcn.num, target_arfcn.band);
      return;
    }

    resel_candidate = rr_resel_best_six_find(target_arfcn, NULL, gas_id);

    if (resel_candidate == NULL)
    {
      MSG_GERAN_ERROR_2_G("Failed to find best six entry for ARFCN %d (band %d)",
                target_arfcn.num, target_arfcn.band);
      return;
    }

    if (rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id))
    {
      MSG_GERAN_LOW_0_G("Stopped the RR_GPRS_SYS_INFO_TIMER");
    }

    /*********************************************************************/
    /* If the failure was due to a state transition then re-validate the */
    /* ncell system info. This will then make it an immediate candidate  */
    /* in the new state.                                                 */
    /* If this was not the case then remove the cell from the database   */
    /*********************************************************************/
    if ((rr_gprs_camped_on_ccch(gas_id) || rr_gsm_camped_on_ccch(gas_id)) && bcch_ncell_db_ptr)
    {
      if (msg->state_changed)
      {
        bcch_ncell_db_ptr->gsm.valid_data_flag = TRUE;

        (void)rr_timer_create_expired_timer(resel_candidate->reselection_info.five_second_timer_id, gas_id);

        MSG_GERAN_MED_1_G("Re-validate ncell DB entry %d",
          CONVERT_FROM_ARFCN_T(target_arfcn));
      }
      else
      {
        rr_gprs_remove_from_best_six(bcch_ncell_db_ptr->gsm.BCCH_ARFCN,
                                     bcch_ncell_db_ptr->gsm.BSIC,
                                     gas_id);

        rr_gprs_invalidate_surround_ncell_sys_info(bcch_ncell_db_ptr->gsm.BCCH_ARFCN,
                                                   NULL,
                                                   gas_id);

        MSG_GERAN_MED_1_G("Removed the ncell from DB %d",
          CONVERT_FROM_ARFCN_T(bcch_ncell_db_ptr->gsm.BCCH_ARFCN));
      }
    }
  }
}

static const char *rr_cell_acq_state_name(const rr_cell_acq_state_e state)
{
  const char * s;

  switch (state)
  {
    case RR_CELL_ACQ_STATE_NULL:
      s = "NULL";
      break;

    case RR_CELL_ACQ_STATE_SURROUND_UPDATE:
      s = "SURROUND_UPDATE";
      break;

    case RR_CELL_ACQ_STATE_SUSPENDING_MAC:
      s = "SUSPENDING_MAC";
      break;

    case RR_CELL_ACQ_STATE_SUSPENDING_MAC_ABORTED:
      s = "SUSPENDING_MAC_ABORTED";
      break;

    case RR_CELL_ACQ_STATE_RESELECTING:
      s = "RESELECTING";
      break;

    case RR_CELL_ACQ_STATE_RESELECTING_ABORTED:
      s = "RESELECTING_ABORTED";
      break;

    case RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM:
      s = "ACQUIRING_BSI_GSM";
      break;

    case RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM_EARLY_CAMPED:
      s = "ACQUIRING_BSI_GSM_EARLY_CAMPED";
      break;

    case RR_CELL_ACQ_STATE_ACQUIRING_BSI_GPRS_EARLY_CAMPED:
      s = "ACQUIRING_BSI_GPRS_EARLY_CAMPED";
      break;

    case RR_CELL_ACQ_STATE_WAITING_FOR_IDLE_MODE_CNF:
      s = "WAITING_FOR_IDLE_MODE_CNF";
      break;

    default:
      MSG_GERAN_ERROR_1("Unknown state: %d", state);
      s = "?";
  }

  return s;

} /* end rr_cell_acq_state_name() */

/*======================================================================

  FUNCTION        rr_cell_acq_check_sys_info_timer

  DESCRIPTION     Checks that the sys info timer is still running when
                  a cell acquisition primitive has completed in Layer 1

  DEPENDENCIES    None

  PARAMETERS      None

  RETURN VALUE    Void

  SIDE EFFECTS    May restart RR_GPRS_SYS_INFO_TIMER

======================================================================*/
static void rr_cell_acq_check_sys_info_timer(const rr_l1_message_id_enum_T gl1_msg_id, const gas_id_t gas_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_cell_acq_data_t *rr_cell_acq_data_ptr = rr_cell_acq_get_data_ptr(gas_id);
  boolean             restart_now;

  if ((MPH_SELECT_SPECIFIC_BCCH_CNF == gl1_msg_id) ||
      (MPH_RESELECTION_CNF          == gl1_msg_id))
  {
    restart_now = rr_cell_acq_data_ptr->restart_sys_info_timer;
    rr_cell_acq_data_ptr->restart_sys_info_timer = FALSE;
  }
  else if (MPH_START_IDLE_MODE_CNF == gl1_msg_id)
  {
    restart_now = TRUE;
  }
  else
  {
    restart_now = FALSE;
  }

  if (restart_now)
  {
    (void)rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id);
    if (rr_timer_start(RR_GPRS_SYS_INFO_TIMER, SI3_SI4_RESTART_TIMEOUT, gas_id))
    {        
      MSG_GERAN_HIGH_1_G("Restarted RR_GPRS_SYS_INFO_TIMER for %dms", SI3_SI4_RESTART_TIMEOUT);
    }
  }
#else
  /* just output some useful debug */
  if (FALSE == rr_timer_is_running(RR_GPRS_SYS_INFO_TIMER, gas_id))
  {
    MSG_GERAN_HIGH_1_G("RR_GPRS_SYS_INFO_TIMER is not running after L1 response 0x%02X", (int)gl1_msg_id);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return;
}

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/

boolean rr_cell_acq_pending_mac_suspension(
  const gas_id_t gas_id
)
{
  rr_cell_acq_data_t *rr_cell_acq_data_ptr = rr_cell_acq_get_data_ptr(gas_id);

  if (rr_cell_acq_data_ptr != NULL)
  {
    if (rr_cell_acq_data_ptr->state == RR_CELL_ACQ_STATE_SUSPENDING_MAC)
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*======================================================================

  FUNCTION        rr_cell_acq_control

  DESCRIPTION     Main RR-CELL ACQ state machine

  DEPENDENCIES    None

  PARAMETERS      event_in, event_data_ptr

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
void rr_cell_acq_control(
  rr_event_T event_in,
  rr_cmd_bdy_type *message,
  const gas_id_t gas_id
)
{
  byte message_id;
  byte message_set;

  rr_event_T  new_event = EV_NO_EVENT;
  rr_cell_acq_data_t *rr_cell_acq_data_ptr = rr_cell_acq_get_data_ptr(gas_id);

  if(event_in == EV_RESET_SOFTWARE)
  {
    rr_cell_acq_reset_parameters(gas_id);
    rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
    return;
  }
  else if(event_in == EV_RECENT_RESELECTION_TIMEOUT)
  {
    /*Reselection occurred over 15 seconds ago now - remove any restriction*/
    MSG_GERAN_MED_0_G("RECENT RESELECTION TIMEOUT");
    event_in = EV_NO_EVENT;
    rr_cell_acq_data_ptr->recent_reselection_flag = FALSE;
  }

  /* If EV_INPUT_MESSAGE is passed in, message_set & message_id will be setup from */
  /* message_ptr and EV_INPUT_MESSAGE returned. If message_ptr, message_set or */
  /* message_id are NULL, then EV_NO_EVENT will be returned */
  /* If any other event is passed in, this is simply returned */
  event_in = rr_extract_message_header_details(
    event_in,
    message,
    &message_set,
    &message_id
  );

  /* If we get a start request and this state machine is in the wrong state,
  reset everything in preparation for the new acquisition. */
  if (event_in == EV_INPUT_MESSAGE)
  {
    switch (message_set)
    {
      case MS_RR_RR:
      {
        switch (message->rr.header.rr_message_set)
        {
          case RR_CELL_ACQ_IMSG:
          {
            switch (message->rr.cell_acq.imsg.header.imh.message_id)
            {
              case RR_CELL_ACQ_IMSG_START_IND:
              {
                if (rr_cell_acq_data_ptr->state != RR_CELL_ACQ_STATE_NULL)
                {
                  MSG_GERAN_ERROR_1_G("rr_cell_acq_control() stuck in state %d, resetting to process start_ind",
                            rr_cell_acq_data_ptr->state);
                  rr_cell_acq_reset_parameters(gas_id);
                  rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                }
                break;
              } /* RR_CELL_ACQ_IMSG_START_IND */

              default:
              {
                /* No action necessary */;
              }
            }
            break;
          } /* RR_CELL_ACQ_IMSG */

          default:
          {
            /* No action necessary */;
          }
        }
      } /* MS_RR_RR */

      default:
      {
        /* No action necessary */;
      }
    }
  }


  while (event_in != EV_NO_EVENT)
  {
    switch (rr_cell_acq_data_ptr->state)
    {
     /**
      * RR_CELL_ACQ_STATE_NULL
      *
      *
      * In this state:This is the initial state of cell acq state machine.
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      * RR_CELL_ACQ_IMSG_START_IND
      *
      * Events:
      *
      **/
      case RR_CELL_ACQ_STATE_NULL:
      {
        switch ( event_in )
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    switch (message->rr.cell_acq.imsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_IMSG_START_IND:
                      {
                        MSG_GERAN_HIGH_1_G("RR_CELL_ACQ_IMSG_START_IND action:0x%x rxd",
                                 message->rr.cell_acq.imsg.start_ind.action);

                        rr_cell_acq_data_ptr->target_cell             = message->rr.cell_acq.imsg.start_ind.target_cell;
                        rr_cell_acq_data_ptr->recent_reselection_flag = message->rr.cell_acq.imsg.start_ind.recent_reselection_flag;
                        rr_cell_acq_data_ptr->RXLEV_average           = message->rr.cell_acq.imsg.start_ind.RXLEV_average;
                        rr_cell_acq_data_ptr->bsic_valid              = message->rr.cell_acq.imsg.start_ind.bsic_valid;
                        rr_cell_acq_data_ptr->bsic                    = message->rr.cell_acq.imsg.start_ind.bsic;
                        rr_cell_acq_data_ptr->action                  = message->rr.cell_acq.imsg.start_ind.action;
                        rr_cell_acq_data_ptr->sync_flag               = message->rr.cell_acq.imsg.start_ind.sync_flag;
                        rr_cell_acq_data_ptr->gprs_cell_info          = message->rr.cell_acq.imsg.start_ind.gprs_cell_info;
                        rr_cell_acq_data_ptr->clear_si_db             = message->rr.cell_acq.imsg.start_ind.clear_si_database;
                        rr_cell_acq_data_ptr->requested_plmn_valid    = message->rr.cell_acq.imsg.start_ind.requested_plmn_valid;
                        rr_cell_acq_data_ptr->requested_plmn          = message->rr.cell_acq.imsg.start_ind.requested_plmn;


                        /* And start the reselection procedure with either a surround update or
                        an immediate suspension of MAC, depending on the cause. */
                        if (rr_cell_acq_data_ptr->action == RR_CELL_ACQ_ACTION_RESELECT_BCCH_AFTER_SU)
                        {
                          rr_gprs_surround_update_SI3_SI4_for_ARFCN(rr_cell_acq_data_ptr->target_cell, gas_id);

                          rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_SURROUND_UPDATE;
                        }
                        else
                        {
                          rr_cell_acq_data_ptr->gprs_cell_info->gsm.camped_on_flag = FALSE;

                          if (rr_cell_acq_data_ptr->clear_si_db)
                          {
                            MSG_GERAN_MED_0_G("RR CELL ACQ Clear SI Database");
                            rr_clear_system_information(&rr_cell_acq_data_ptr->gprs_cell_info->gsm);
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
                            rr_set_rr_using_pseudo_si2q_data(FALSE, gas_id);
#endif                            
                          }
                          if( !resel_type_C2_based(gas_id))
                          {
                            rr_initialize_SI2q_instances(gas_id);
                            rr_initialize_SI2ter_instances(gas_id);                          

                          }	



                          /***************************************************************************/
                          /* Set up the pending channel system information database for reselection  */
                          /* to the new target cell.                                                 */
                          /***************************************************************************/
                          /* Load the ARFCN of the target cell into the pending channel database */
                          rr_cell_acq_data_ptr->gprs_cell_info->gsm.BCCH_ARFCN = rr_cell_acq_data_ptr->target_cell;

                          rr_cell_acq_suspend_gprs(gas_id);

                          rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_SUSPENDING_MAC;
                        }

                        break;
                      }

                      case RR_CELL_ACQ_IMSG_ABORT_REQ:
                      {
                        /* no state transition required, send confirm immediately */
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_ABORT_REQ in NULL state");
                        rr_cell_acq_send_omsg_abort_cnf(gas_id);
                        break;
                      }

                      case RR_CELL_ACQ_IMSG_SURROUNDING_SI_RECEIVED_IND:
                      case RR_CELL_ACQ_IMSG_EARLY_CAMPING_BSI_RECEIVED_IND:
                      case RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND:
                      case RR_CELL_ACQ_IMSG_GPRS_BSI_RECEIVED_IND:
                      {
                        /* ignore these signals */
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_PS_IMSG:
                  {
                    rr_ps_access_control(message, gas_id);
                    break;
                  }

                  case RR_PS_OMSG:
                  {
                    switch (message->rr.ps.omsg.header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        new_event = EV_MAC_IDLE_MODE;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }

                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_RR_L1:
              {
                switch (message->message_header.message_id)
                {
                  /* This may be posted in to the rr_cell_acq_control() state machine even
                  when no cell reselection is in progress, e.g. if surround update of a
                  neighbour cell fails while in idle mode.  No action necessary. */
                  case MPH_SURROUND_UPDATE_FAILURE_IND:
                  {
                    break;
                  } /* MPH_SURROUND_UPDATE_FAILURE_IND */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_L1 */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                );
              }
            }
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event_in),
                                 rr_cell_acq_state_name(rr_cell_acq_data_ptr->state));
          }
        }

        break;
      }

     /**
      * RR_CELL_ACQ_STATE_SURROUND_UPDATE
      *
      *
      * In this state:This state represents that surround update is in progress for ncell better
      * reselection
      *
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      * RR_CELL_ACQ_IMSG_SURROUNDING_SI_RECEIVED_IND
      * RR_CELL_ACQ_IMSG_ABORT_REQ
      * MPH_SURROUND_UPDATE_FAILURE_IND
      * RR_GPRS_SYS_INFO_TIMER
      *
      * Events:
      *
      **/
      case RR_CELL_ACQ_STATE_SURROUND_UPDATE:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("event_in == EV_INPUT_MESSAGE, message == NULL");
              continue;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    switch (message->rr.cell_acq.imsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_IMSG_SURROUNDING_SI_RECEIVED_IND:
                      {
                        bcch_ncell_info_t *ptr_surround_database;

                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_SURROUNDING_SI_RECEIVED_IND rxd");

                        ptr_surround_database = rr_gprs_get_surround_ncell_sys_info(
                          rr_cell_acq_data_ptr->target_cell,
                          gas_id
                        );
                        if (ptr_surround_database != NULL)
                        {
                          /* Data exists in the surround database for the cell, check to see if its valid*/
                          if (ptr_surround_database->gsm.valid_data_flag)
                          {
                            /* The correct SI's have been acquired cancel the sample timer */
                            if (rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id))
                            {
                              MSG_GERAN_LOW_0_G("Stopped the RR_GPRS_SYS_INFO_TIMER");
                            }

                            if (rr_gprs_handle_surround_SI3_SI4(
                                  rr_cell_acq_data_ptr->recent_reselection_flag,
                                  gas_id
                                ))
                            {
                              rr_cell_acq_data_ptr->gprs_cell_info->gsm.camped_on_flag = FALSE;

                              rr_clear_system_information(&rr_cell_acq_data_ptr->gprs_cell_info->gsm);

                              rr_initialize_SI2q_instances(gas_id);
                              rr_initialize_SI2ter_instances(gas_id);;

                              rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description_valid = TRUE;
                              rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description = ptr_surround_database->gsm.control_channel_description;
                              rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity_valid_flag = ptr_surround_database->gsm.cell_id_valid;
                              rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity = ptr_surround_database->gsm.cell_id;
                              rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification = ptr_surround_database->gsm.location_area_identification;

                              /***************************************************************************/
                              /* Set up the pending channel system information database for reselection  */
                              /* to the new target cell.                                                 */
                              /***************************************************************************/
                              /*Load the ARFCN of the target cell into the pending channel database*/
                              rr_cell_acq_data_ptr->gprs_cell_info->gsm.BCCH_ARFCN = rr_cell_acq_data_ptr->target_cell;

                              rr_cell_acq_suspend_gprs(gas_id);

                              rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_SUSPENDING_MAC;
                            }
                            else
                            {
                              /* Cell is not suitable then send FINISHED IND with acq failied */
                              rr_cell_acq_send_omsg_finished_ind(
                                RR_CELL_ACQ_STATUS_FAILURE_CELL_NOT_BETTER,
                                gas_id
                              );
                              rr_cell_acq_reset_parameters(gas_id);

                              rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                            }
                          }
                        }
                        else
                        {
                          /*There is no data in the database for the requested cell*/
                          MSG_GERAN_MED_1_G("No ncell data for ARFCN %d",CONVERT_FROM_ARFCN_T(rr_cell_acq_data_ptr->target_cell));
                        }
                        break;
                      }

                      case RR_CELL_ACQ_IMSG_ABORT_REQ:
                      {
                        bcch_ncell_info_t *bcch_ncell_info;

                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_ABORT_REQ rxd");

                        /* Instruct GL1 to stop surround update req */
                        rr_send_mph_stop_surround_update_req(gas_id);
                        clear_waiting_for_surround_response(gas_id);

                        bcch_ncell_info = rr_gprs_get_surround_ncell_sys_info(
                          rr_cell_acq_data_ptr->target_cell,
                          gas_id
                        );

                        if (bcch_ncell_info != NULL)
                        {
                          bcch_ncell_info->gsm.si_expected           = FALSE;
                          bcch_ncell_info->gprs.si13_expected        = FALSE;
                          bcch_ncell_info->gprs.si13_data_valid_flag = FALSE;
                        }
                        else
                        {
                          MSG_GERAN_MED_1_G("No ncell info for target ARFCN %d", (int)(rr_cell_acq_data_ptr->target_cell.num));
                        }

                        rr_cell_acq_send_omsg_abort_cnf(gas_id);
                        rr_cell_acq_reset_parameters(gas_id);
                        rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /*RR_CELL_ACQ_IMSG*/

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              } /*MS_RR_RR*/

              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_SURROUND_UPDATE_FAILURE_IND:
                  {
                    MSG_GERAN_HIGH_0_G("MPH_SURROUND_UPDATE_FAILURE_IND rxd");

                    handle_surround_update_failure_ind(
                      (mph_surround_update_failure_ind_T *) message,
                      rr_cell_acq_data_ptr->target_cell,
                      gas_id
                    );

                    rr_cell_acq_send_omsg_finished_ind(
                      RR_CELL_ACQ_STATUS_FAILURE_SURROUND_UPDATE_FAILED,
                      gas_id
                    );

                    rr_cell_acq_reset_parameters(gas_id);

                    rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;

                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              }

              case MS_TIMER:
              {
                switch (message_id)
                {
                  case MID_TIMER_EXPIRY:
                  {
                    timer_expiry_T *timer_expiry = (timer_expiry_T *) message;

                    if (timer_expiry->timer_id == RR_GPRS_SYS_INFO_TIMER)
                    {
                      MSG_GERAN_HIGH_0_G("RR_GPRS_SYS_INFO_TIMER rxd");
#ifdef FEATURE_RESELECT_ON_SI3_RECONF_TIMEOUT
                      MSG_GERAN_MED_0_G("SI3 reconfirmation has timed out, reselect anyway");
                      rr_cell_acq_data_ptr->sync_flag = FALSE;
                      rr_cell_acq_suspend_gprs(gas_id);
                      rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_SUSPENDING_MAC;
#else
                      bcch_ncell_info_t *bcch_ncell_info;

                      /* Instruct GL1 to stop surround update req */
                      rr_send_mph_stop_surround_update_req(gas_id);
                      clear_waiting_for_surround_response(gas_id);

                      bcch_ncell_info = rr_gprs_get_surround_ncell_sys_info(
                        rr_cell_acq_data_ptr->target_cell,
                        gas_id
                      );

                      if (bcch_ncell_info != NULL)
                      {
                        bcch_ncell_info->gsm.si_expected           = FALSE;
                        bcch_ncell_info->gprs.si13_expected        = FALSE;
                        bcch_ncell_info->gprs.si13_data_valid_flag = FALSE;
                      }
                      else
                      {
                        MSG_GERAN_MED_1_G("No ncell info for target ARFCN %d", (int)(rr_cell_acq_data_ptr->target_cell.num));
                      }

                      rr_cell_acq_send_omsg_finished_ind(
                        RR_CELL_ACQ_STATUS_FAILURE_SURROUND_UPDATE_FAILED,
                        gas_id
                      );

                      rr_cell_acq_reset_parameters(gas_id);

                      rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
#endif /* FEATURE_RESELECT_ON_SI3_RECONF_TIMEOUT */
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                );
              }
            }
            break;
          }

          case EV_MAC_IDLE_MODE:
          {
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event_in),
                                 rr_cell_acq_state_name(rr_cell_acq_data_ptr->state));
          }
        }
        break;
      }

      /**
      * RR_CELL_ACQ_STATE_SUSPENDING_MAC
      *
      *
      * In this state: CELL ACQ state machine is waiting for GMAC IDLE from GMAC
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      * RR_CELL_ACQ_IMSG_ABORT_REQ
      *
      *
      * Events:
      * EV_MAC_GUARD_TIMER_EXPIRED
      * EV_MAC_IDLE_MODE
      **/

      case RR_CELL_ACQ_STATE_SUSPENDING_MAC:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    switch (message->rr.cell_acq.imsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_ABORT_REQ rxd in state RR_CELL_ACQ_STATE_SUSPENDING_MAC");

                        /* Wait for MAC IDLE before sending abort cnf */
                        rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_SUSPENDING_MAC_ABORTED;

                        /* if there is a state mismatch and GMAC is still camped, suspend it now */
                        if (rr_mac_ps_access_granted(gas_id))
                        {
                          MSG_GERAN_ERROR_0_G("GMAC is still camped on, suspending now");
                          rr_cell_acq_suspend_gprs(gas_id);
                        }

                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_PS_IMSG:
                  {
                    rr_ps_access_control(message, gas_id);
                    break;
                  }

                  case RR_PS_OMSG:
                  {
                    switch (message->rr.ps.omsg.header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        new_event = EV_MAC_IDLE_MODE;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                        message,
                        rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }

                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              } /*MS_RR_RR*/

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                );
              }
            }
            break;
          }

          case EV_MAC_GUARD_TIMER_EXPIRED:
          {
            if(geran_get_nv_recovery_restart_enabled(gas_id))
            {
              MSG_GERAN_HIGH_0_G("EV_MAC_GUARD_TIMER_EXPIRED rxd");
              /* No response from MAC to reselection request - trigger panic reset */
              geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
            }
            else
            {
              /* No response from MAC to reselection request - trigger err fatal */
              ERR_GERAN_FATAL_0_G("EV_MAC_GUARD_TIMER_EXPIRED rxd");
            }
            break;
          }

          case EV_MAC_IDLE_MODE:
          {
            MSG_GERAN_HIGH_0_G("EV_MAC_IDLE_MODE rxd");

            /* It is possible that MAC autonomously returns to IDLE mode
            while RR is waiting for a surround update to be carried out.
            It is important for RR to ignore the EV_MAC_IDLE_MODE until it is
            certain that it is in response to a GRR_MAC_NO_PS_ACCESS_IND. */
            if (rr_mac_ps_access_blocked(gas_id))
            {
              rr_log_gprs_surround_search_success(gas_id);

              if (rr_cell_acq_data_ptr->action == RR_CELL_ACQ_ACTION_RESELECT_BCCH_AFTER_SU)
              {
                bcch_ncell_info_t *ptr_surround_database;
                ptr_surround_database = rr_gprs_get_surround_ncell_sys_info(
                  rr_cell_acq_data_ptr->target_cell,
                  gas_id
                );

                if (NULL == ptr_surround_database)
                {
                  rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description_valid = FALSE;
                  MSG_GERAN_ERROR_0_G("No surround sys info for ncell");
                }
                else if (ptr_surround_database->gsm.valid_data_flag)
                {
                  rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description_valid = TRUE;
                  rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description = ptr_surround_database->gsm.control_channel_description;
                }
              }

              rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_RESELECTING;

              rr_cell_acq_send_omsg_mac_suspended_ind(gas_id);

              rr_gprs_action_cell_reselection(gas_id);
            }

            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event_in),
                                 rr_cell_acq_state_name(rr_cell_acq_data_ptr->state));
          }
        }
        break;
      }

      /**
      * RR_CELL_ACQ_STATE_SUSPENDING_MAC_ABORTED
      *
      *
      * In this state: CELL ACQ state machine is aborted the procedure and waiting for GMAC IDLE from GMAC
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      *
      *
      * Events:
      * EV_MAC_IDLE_MODE
      **/

      case RR_CELL_ACQ_STATE_SUSPENDING_MAC_ABORTED:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_PS_IMSG:
                  {
                    rr_ps_access_control(message, gas_id);
                    break;
                  }

                  case RR_PS_OMSG:
                  {
                    switch (message->rr.ps.omsg.header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        new_event = EV_MAC_IDLE_MODE;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }

                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
		          break; 
              } /*MS_RR_RR*/

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                );
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_MAC_IDLE_MODE:
          {
            MSG_GERAN_HIGH_0_G("EV_MAC_IDLE_MODE rxd");

            /* It is possible that MAC autonomously returns to IDLE mode
            while RR is waiting for a surround update to be carried out.
            It is important for RR to ignore the EV_MAC_IDLE_MODE until it is
            certain that it is in response to a GRR_MAC_NO_PS_ACCESS_IND. */
            if (rr_mac_ps_access_blocked(gas_id))
            {
              rr_log_gprs_surround_search_success(gas_id);

              rr_cell_acq_send_omsg_abort_cnf(gas_id);

              rr_cell_acq_reset_parameters(gas_id);
              rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
            }

            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event_in),
                                 rr_cell_acq_state_name(rr_cell_acq_data_ptr->state));
          }
        }
        break;
      }

      /**
      * RR_CELL_ACQ_STATE_RESELECTING
      *
      *
      * In this state: CELL ACQ state machine is given a reselection request or select bcch req to GL1 and waiting for CNF from GL1
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      * MPH_RESELECTION_CNF
      * MPH_SELECT_SPECIFIC_BCCH_CNF
      * RR_CELL_ACQ_IMSG_ABORT_REQ
      *
      * Events:
      *
      **/
      case RR_CELL_ACQ_STATE_RESELECTING:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_RESELECTION_CNF:
                  {
                    mph_reselection_cnf_T *message_ptr = (mph_reselection_cnf_T *)message;

                    MSG_GERAN_HIGH_0_G("MPH_RESELECTION_CNF rxd");

                    if (message_ptr->BCCH_found)
                    {
                      rr_cell_acq_check_sys_info_timer(MPH_RESELECTION_CNF, gas_id);
                      rr_cell_acq_send_omsg_reselected_ind(gas_id);

                      if (rr_cell_acq_data_ptr->action == RR_CELL_ACQ_ACTION_RESELECT_BCCH ||
                          rr_cell_acq_data_ptr->action == RR_CELL_ACQ_ACTION_RESELECT_BCCH_AFTER_SU ||
                          rr_cell_acq_data_ptr->action == RR_CELL_ACQ_ACTION_RESELECT_BCCH_PCCO_FAILED ||
                          rr_cell_acq_data_ptr->action == RR_CELL_ACQ_ACTION_RESELECT_BCCH_WITH_GPRS_CELL_SELECTION_PARAMS)
                      {
                        if (rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description_valid)
                        {
                          /* cell is still suitable */
                          /* perform early camping on the pending channel */
                          rr_l1_idle_send_imsg_start_early_camp_req(
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.BCCH_ARFCN,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.BSIC,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description_valid,
                            gas_id
                          );

                          rr_cell_acq_data_ptr->early_camping_status = RR_CELL_ACQ_EARLY_CAMPING_REQ_SENT;

                          rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM_EARLY_CAMPED;
                        }
                        else
                        {
                          rr_cell_acq_data_ptr->early_camping_status = RR_CELL_ACQ_EARLY_CAMPING_NOT_STARTED;
                          rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM;
                        }
                      }
                      else if (rr_cell_acq_data_ptr->action == RR_CELL_ACQ_ACTION_RESELECT_BCCH_NACC_PCCO_PCCC)
                      {
                        rr_cell_acq_data_ptr->early_camping_status = RR_CELL_ACQ_EARLY_CAMPING_NOT_STARTED;
                        rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM;
                      }

                      if (rr_is_fast_si_acq_enabled(NULL, gas_id))
                      {
                        /* Set up BA_INDs with value of 2.  This is a value that can not
                        be received OTA, so allows us to detect after camping whether or
                        not SI2 & friends have been received or not, which lets us trigger
                        their acquisition if necessary. */
                        rr_cell_acq_data_ptr->gprs_cell_info->gsm.complete_BA_list.no_of_entries = 0;
                        rr_cell_acq_data_ptr->gprs_cell_info->gsm.complete_idle_BA_list_received = FALSE;
                        rr_cell_acq_data_ptr->gprs_cell_info->gsm.complete_BA_list.BA_IND        = 0x02;
                        rr_cell_acq_data_ptr->gprs_cell_info->gsm.SI2_BA_list.BA_IND             = 0x02;
                        rr_cell_acq_data_ptr->gprs_cell_info->gsm.SI2bis_BA_list.BA_IND          = 0x02;
                        rr_cell_acq_data_ptr->gprs_cell_info->gsm.SI2ter_BA_list.BA_IND          = 0x02;
                      }
                    }
                    else
                    {
                      /* Failed to move to BCCH on the target cell - abort reselection */
                      /* Change the reselection state to manage the sending of the no service message to MAC*/
                      /* Stop the sys info timer*/
                      if (rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id))
                      {
                        MSG_GERAN_LOW_0_G("Stopped the RR_GPRS_SYS_INFO_TIMER");
                      }

                      rr_cell_acq_send_omsg_finished_ind(
                        RR_CELL_ACQ_STATUS_FAILURE_BCCH_NOT_FOUND,
                        gas_id
                      );
                      rr_cell_acq_reset_parameters(gas_id);
                      rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                    }
                    break;
                  }

                  case MPH_SELECT_SPECIFIC_BCCH_CNF:
                  {
                    mph_select_specific_bcch_cnf_T *message_ptr = (mph_select_specific_bcch_cnf_T *)message;
                    bcch_ncell_info_t *ptr_surround_database;                   
                    ptr_surround_database = rr_gprs_get_surround_ncell_sys_info(rr_cell_acq_data_ptr->target_cell,gas_id);

                    MSG_GERAN_HIGH_0_G("MPH_SELECT_SPECIFIC_BCCH_CNF rxd");

                    if (message_ptr->BCCH_found)
                    {
                      uint8 PLMN_colour_code = DECODE_NCC(message_ptr);
                      uint8 BS_colour_code = DECODE_BCC(message_ptr);

                      rr_cell_acq_check_sys_info_timer(MPH_SELECT_SPECIFIC_BCCH_CNF, gas_id);

                      if (rr_cell_acq_data_ptr->bsic_valid)
                      {
                        if (rr_cell_acq_data_ptr->bsic.PLMN_colour_code != PLMN_colour_code ||
                            rr_cell_acq_data_ptr->bsic.BS_colour_code != BS_colour_code)
                        {
                          rr_cell_acq_send_omsg_finished_ind(
                            RR_CELL_ACQ_STATUS_FAILURE_BSIC_MISMATCH,
                            gas_id
                          );
                          rr_cell_acq_reset_parameters(gas_id);
                          rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                          break;
                        }
                      }

                      rr_cell_acq_data_ptr->gprs_cell_info->gsm.BSIC.PLMN_colour_code = PLMN_colour_code;
                      rr_cell_acq_data_ptr->gprs_cell_info->gsm.BSIC.BS_colour_code = BS_colour_code;

                      /*This is done to make sure that if SIs are not retrived from cache if BSIC of that cell changes */
                      if (ptr_surround_database != NULL)
                      {
                        if (( TRUE             == ptr_surround_database->gsm.valid_data_flag) &&
                            ((PLMN_colour_code != ptr_surround_database->gsm.BSIC.PLMN_colour_code) ||
                             (BS_colour_code   != ptr_surround_database->gsm.BSIC.BS_colour_code))                            
                           ) 
                        {
                           
                          rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description_valid = FALSE;
                        }

                        MSG_GERAN_HIGH_2_G("BSIC %d , %d of Target cell",
                                            ptr_surround_database->gsm.BSIC.PLMN_colour_code,
                                            ptr_surround_database->gsm.BSIC.BS_colour_code 
                                         );

                        MSG_GERAN_HIGH_2_G("BSIC %d , %d Received of Target cell",
                                            PLMN_colour_code,
                                            BS_colour_code 
                                          );

                        MSG_GERAN_HIGH_1_G("Valid data cell = %d ", ptr_surround_database->gsm.valid_data_flag);
                      }

                      if (rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description_valid)
                      {
                        rr_cell_acq_status_e status = RR_CELL_ACQ_STATUS_UNDEFINED;
                        boolean              cell_suitable;

                        cell_suitable = rr_cell_acq_is_cell_suitable(
                          rr_cell_acq_data_ptr->target_cell,
                          rr_cell_acq_data_ptr->RXLEV_average,
                          &status,
                          gas_id
                        );

                        if (cell_suitable)
                        {
                          /* cell is still suitable */
                          /* perform early camping on the pending channel */
                          rr_cell_acq_send_omsg_reselected_ind(gas_id);

                          rr_l1_idle_send_imsg_start_early_camp_req(
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.BCCH_ARFCN,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.BSIC,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description_valid,
                            gas_id
                          );

                          rr_cell_acq_data_ptr->early_camping_status = RR_CELL_ACQ_EARLY_CAMPING_REQ_SENT;

                          rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM_EARLY_CAMPED;
                        }
                        else
                        {
                          rr_cell_acq_send_omsg_finished_ind(status, gas_id);
                          rr_cell_acq_reset_parameters(gas_id);
                          rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                          break;
                        }
                      }
                      else
                      {
                        rr_cell_acq_send_omsg_reselected_ind(gas_id);
                        rr_csi_activate_pend_store(gas_id); /* CGI of target cell is unknown, store any SI's before SI3 is received
                                                               in the pend store */
                        rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM;
                        rr_cell_acq_data_ptr->early_camping_status = RR_CELL_ACQ_EARLY_CAMPING_NOT_STARTED;
                      }

                      if (rr_is_fast_si_acq_enabled(NULL, gas_id))
                      {
                        /* Set up BA_INDs with value of 2.  This is a value that can not
                        be received OTA, so allows us to detect after camping whether or
                        not SI2 & friends have been received or not, which lets us trigger
                        their acquisition if necessary. */
                        rr_cell_acq_data_ptr->gprs_cell_info->gsm.complete_BA_list.no_of_entries = 0;
                        rr_cell_acq_data_ptr->gprs_cell_info->gsm.complete_idle_BA_list_received = FALSE;
                        rr_cell_acq_data_ptr->gprs_cell_info->gsm.complete_BA_list.BA_IND        = 0x02;
                        rr_cell_acq_data_ptr->gprs_cell_info->gsm.SI2_BA_list.BA_IND             = 0x02;
                        rr_cell_acq_data_ptr->gprs_cell_info->gsm.SI2bis_BA_list.BA_IND          = 0x02;
                        rr_cell_acq_data_ptr->gprs_cell_info->gsm.SI2ter_BA_list.BA_IND          = 0x02;
                      }
                    }
                    else
                    {
                      rr_cell_acq_send_omsg_finished_ind(
                        RR_CELL_ACQ_STATUS_FAILURE_BCCH_NOT_FOUND,
                        gas_id
                      );
                      rr_cell_acq_reset_parameters(gas_id);
                      rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_L1 */

              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    switch (message->rr.cell_acq.imsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_ABORT_REQ rxd, wait for GL1 response");

                        /* Wait for the outstanding GL1 primitive before sending abort cnf */
                        rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_RESELECTING_ABORTED;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /*RR_CELL_ACQ_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                      );
                  }
                }
                break;
              } /* MS_RR_RR */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
              /* Under Dual SIM conditions, it is possible for timers to expire */
              /* whilst waiting for GL1 to respond to a (re)selection primitive */
              case MS_TIMER:
                if ((message_id == MID_TIMER_EXPIRY) && (rr_cell_acq_data_ptr != NULL))
                {
                  MSG_GERAN_ERROR_2_G("Timer %d expired whilst waiting for GL1 (restart=%d)",
                            (int)(message->mid_timer_expiry.timer_id),
                            (int)(rr_cell_acq_data_ptr->restart_sys_info_timer));
                }
                break;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                );
              }
            }
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event_in),
                                 rr_cell_acq_state_name(rr_cell_acq_data_ptr->state));
          }
        }
        break;
      }

      /**
      * RR_CELL_ACQ_STATE_RESELECTING_ABORTED
      *
      *
      * In this state: CELL ACQ state machine aborted the acquisition and waiting for reselction cnf or select specific cnf from GL1
      *
      *
      * This state handles the following messages and events:
      *
      * Messages:
      * MPH_RESELECTION_CNF
      * MPH_SELECT_SPECIFIC_BCCH_CNF
      *
      *
      * Events:
      *
      **/
      case RR_CELL_ACQ_STATE_RESELECTING_ABORTED:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_RESELECTION_CNF:
                  case MPH_SELECT_SPECIFIC_BCCH_CNF:
                  {
                    MSG_GERAN_HIGH_1_G("Received message 0x%02X from GL1",(int)message_id);

                    /* Send abort cnf and move back NULL state */
                    rr_cell_acq_send_omsg_abort_cnf(gas_id);
                    rr_cell_acq_reset_parameters(gas_id);
                    rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;

                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_L1 */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                );
              }
            }
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event_in),
                                 rr_cell_acq_state_name(rr_cell_acq_data_ptr->state));
          }
        }
        break;
      }

      /**
      * RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM
      *
      *
      * In this state: CELL ACQ state machine is waiting for early camping SI. Once these SI received
      * cell acq state machine moved to RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM_EARLY_CAMPED state.
      * In case of NACC, resel control sends NACC complete ind to cell acq and cell acq completed acquistion
      *
      * This state handles the following messages and events:
      *
      * Messages:
      * RR_CELL_ACQ_IMSG_EARLY_CAMPING_BSI_RECEIVED_IND
      * RR_CELL_ACQ_IMSG_ABORT_REQ
      * RR_CELL_ACQ_IMSG_NACC_ACQ_COMPLETE_IND
      *
      * Events:
      *
      **/

      case RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    switch (message->rr.cell_acq.imsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_IMSG_EARLY_CAMPING_BSI_RECEIVED_IND:
                      {
                        rr_cell_acq_status_e status = RR_CELL_ACQ_STATUS_UNDEFINED;
                        boolean              cell_suitable;
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_EARLY_CAMPING_BSI_RECEIVED_IND rxd");

                        cell_suitable = rr_cell_acq_is_cell_suitable(
                          rr_cell_acq_data_ptr->target_cell,
                          rr_cell_acq_data_ptr->RXLEV_average,
                          &status,
                          gas_id
                        );

                        if (cell_suitable)
                        {
                          boolean x2g_resel_with_nacc = FALSE; 
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
                          x2g_resel_with_nacc = rr_x2g_resel_with_nacc_in_progress(gas_id); 
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

                          /* cell is still suitable */
                          /* perform early camping on the pending channel */
                          rr_l1_idle_send_imsg_start_early_camp_req(
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.BCCH_ARFCN,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.BSIC,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description,
                            rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description_valid,
                            gas_id
                          );

                          if ( ( rr_cell_acq_data_ptr->action != RR_CELL_ACQ_ACTION_RESELECT_BCCH_NACC_PCCO_PCCC ) &&
                               ( x2g_resel_with_nacc == FALSE ) )
                          {
                            if (rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity_valid_flag)
                            {
                              rr_csi_copy_pend_store_to_cache(rr_cell_acq_data_ptr->gprs_cell_info->gsm.BCCH_ARFCN,
                                                              &(rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification),
                                                              &(rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity),
                                                              gas_id);
                            }
                          }

                          rr_csi_deactivate_pend_store(gas_id);

                          rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM_EARLY_CAMPED;
                          rr_cell_acq_data_ptr->early_camping_status = RR_CELL_ACQ_EARLY_CAMPING_REQ_SENT;
                        }
                        else
                        {
                          rr_cell_acq_send_omsg_finished_ind(status, gas_id);
                          rr_cell_acq_reset_parameters(gas_id);
                          rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                        }
                        break;
                      } /* RR_CELL_ACQ_IMSG_EARLY_CAMPING_BSI_RECEIVED_IND */

                      case RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND:
                      {
                        rr_cell_acq_status_e status = RR_CELL_ACQ_STATUS_UNDEFINED;
                        boolean              cell_suitable;

                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND rxd");

                        /* Check that cell is still suitable - this is required if
                        SI3 was received after SI1, SI2 and SI4 as the check will
                        not have been done yet. */
                        cell_suitable = rr_cell_acq_is_cell_suitable(
                          rr_cell_acq_data_ptr->target_cell,
                          rr_cell_acq_data_ptr->RXLEV_average,
                          &status,
                          gas_id
                        );

                        if (cell_suitable)
                        {
                          MSG_GERAN_MED_4_G("cell_suitable=%d, gsm.gprs_indicator=%d, si13_data_valid_flag=%d, early_camping_status=%d",
                                                  cell_suitable,
                                                  rr_cell_acq_data_ptr->gprs_cell_info->gsm.gprs_indicator,
                                                  rr_cell_acq_data_ptr->gprs_cell_info->bcch_sys_info.si13_data_valid_flag,
                                                  rr_cell_acq_data_ptr->early_camping_status);

                          if (rr_cell_acq_data_ptr->gprs_cell_info->gsm.gprs_indicator)
                          {
                            if (rr_cell_acq_data_ptr->gprs_cell_info->bcch_sys_info.si13_data_valid_flag)
                            {
                              {
                                if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_NOT_STARTED)
                                {
                                  rr_cell_acq_send_omsg_finished_ind(
                                    RR_CELL_ACQ_STATUS_SUCCESS_BCCH,
                                    gas_id
                                  );
                                  rr_cell_acq_reset_parameters(gas_id);
                                  rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                                }
                                else if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_COMPLETED)
                                {
                                  rr_cell_acq_send_omsg_finished_ind(
                                    RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED,
                                    gas_id
                                  );
                                  rr_cell_acq_reset_parameters(gas_id);
                                  rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                                }
                                else if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_REQ_SENT)
                                {
                                  rr_csi_deactivate_pend_store(gas_id);
                                  rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_WAITING_FOR_IDLE_MODE_CNF;
                                }
                              }
                            }
                            else
                            {
                              boolean wait_for_si13 = TRUE;
                              boolean is_fast_si_acq_enabled = rr_is_fast_si_acq_enabled(&wait_for_si13, gas_id);

                              MSG_GERAN_MED_3_G("wait_for_si13=%d, is_fast_si_acq_enabled=%d,cell_identity_valid_flag=%d"
                                                                    ,wait_for_si13
                                                                    ,is_fast_si_acq_enabled
                                                                    ,rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity_valid_flag);

                              (void)rr_timer_start(RR_GPRS_SYS_INFO_TIMER, BCCH_SI13_TIMEOUT, gas_id);

                              if (is_fast_si_acq_enabled && !wait_for_si13)
                              {
                                MSG_GERAN_HIGH_0_G("Fast SI Acq enabled, SI13 not yet received, start GSM idle mode");
                                if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_NOT_STARTED)
                                {
                                  rr_cell_acq_send_omsg_finished_ind(RR_CELL_ACQ_STATUS_SUCCESS_BCCH, gas_id);
                                  rr_cell_acq_reset_parameters(gas_id);
                                  rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                                }
                                else if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_COMPLETED)
                                {
                                  rr_cell_acq_send_omsg_finished_ind(RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED, gas_id);
                                  rr_cell_acq_reset_parameters(gas_id);
                                  rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                                }
                                else if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_REQ_SENT)
                                {
                                  rr_csi_deactivate_pend_store(gas_id);
                                  rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_WAITING_FOR_IDLE_MODE_CNF;
                                }
                              }
                              else
                              {
                                /* this is used to prepare to receive the bcch si13 in the pending store */
                                rr_cell_acq_data_ptr->gprs_cell_info->bcch_sys_info.si13_expected_flag = TRUE;
                                rr_cell_acq_data_ptr->gprs_cell_info->bcch_sys_info.si13_data_valid_flag = FALSE;
    
                                MSG_GERAN_MED_0_G("Starting RR_GPRS_SYS_INFO_TIMER");
                                (void)rr_timer_start(RR_GPRS_SYS_INFO_TIMER, BCCH_SI13_TIMEOUT, gas_id);
    
                                if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_NOT_STARTED)
                                {
                                  rr_l1_idle_send_imsg_start_early_camp_req(
                                    rr_cell_acq_data_ptr->gprs_cell_info->gsm.BCCH_ARFCN,
                                    rr_cell_acq_data_ptr->gprs_cell_info->gsm.BSIC,
                                    rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification,
                                    rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity,
                                    rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description,
                                    rr_cell_acq_data_ptr->gprs_cell_info->gsm.control_channel_description_valid,
                                    gas_id
                                  );
    
                                  rr_cell_acq_data_ptr->early_camping_status = RR_CELL_ACQ_EARLY_CAMPING_REQ_SENT;
                                }

                                rr_csi_deactivate_pend_store(gas_id);
                                rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_ACQUIRING_BSI_GPRS_EARLY_CAMPED;
                              }
                            }
                          }
                          else
                          {
                            if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_NOT_STARTED)
                            {
                              rr_cell_acq_send_omsg_finished_ind(
                                RR_CELL_ACQ_STATUS_SUCCESS_BCCH,
                                gas_id
                              );
                              rr_cell_acq_reset_parameters(gas_id);
                              rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                            }
                            else if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_COMPLETED)
                            {
                              rr_cell_acq_send_omsg_finished_ind(
                                RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED,
                                gas_id
                              );
                              rr_cell_acq_reset_parameters(gas_id);
                              rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                            }
                            else if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_REQ_SENT)
                            {
                              rr_csi_deactivate_pend_store(gas_id);
                              rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_WAITING_FOR_IDLE_MODE_CNF;
                            }
                          }

                          if (rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity_valid_flag)
                          {
                            if (rr_csi_retrieve_all_SIs(  rr_cell_acq_data_ptr->gprs_cell_info->gsm.BCCH_ARFCN, 
                                                        &(rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification),
                                                        &(rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity),
                                                          CSI_RETRIEVE_ALL_SIS,
                                                          gas_id))
                            {
                              MSG_GERAN_HIGH_3_G("SIs retrieved after RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND",0,0,0);
                            }
                          }
                        }
                        else
                        {
                          /* Cell is not suitable, indicate acquisition failure. */
                          rr_cell_acq_send_omsg_finished_ind(status, gas_id);
                          rr_cell_acq_reset_parameters(gas_id);
                          rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                        }
                        break;
                      } /* RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND */

                      case RR_CELL_ACQ_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_ABORT_REQ rxd");

                        rr_cell_acq_reset_parameters(gas_id);
                        rr_cell_acq_send_omsg_abort_cnf(gas_id);
                        rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                        break;
                      } /* RR_CELL_ACQ_IMSG_ABORT_REQ */

                      case RR_CELL_ACQ_IMSG_NACC_ACQ_COMPLETE_IND:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_NACC_ACQ_COMPLETE_IND rxd");

                        rr_cell_acq_reset_parameters(gas_id);
                        rr_cell_acq_send_omsg_finished_ind(
                          RR_CELL_ACQ_STATUS_SUCCESS_BCCH,
                          gas_id
                        );
                        rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                        break;
                      } /* RR_CELL_ACQ_IMSG_NACC_ACQ_COMPLETE_IND */

                      case RR_CELL_ACQ_IMSG_GPRS_BSI_RECEIVED_IND:
                      {
                        /* Ignore this signal. Wait for RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND */
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_SELECT_BCCH_IMSG:
                  {
                    /* No action necessary. */
                    break;
                  } /* RR_SELECT_BCCH_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_TIMER:
              {
                switch (message_id)
                {
                  case MID_TIMER_EXPIRY:
                  {
                    timer_expiry_T *timer_expiry = (timer_expiry_T *) message;
                    if (timer_expiry->timer_id == RR_GPRS_SYS_INFO_TIMER)
                    {
                      MSG_GERAN_HIGH_0_G("RR_GPRS_SYS_INFO_TIMER rxd");
                      rr_cell_acq_send_omsg_finished_ind(
                        RR_CELL_ACQ_STATUS_FAILURE_BSI_ACQUISITION_FAILURE,
                        gas_id
                      );
                      rr_cell_acq_reset_parameters(gas_id);
                      rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                    }
                    break;
                  } /* MID_TIMER_EXPIRY */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_TIMER */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                );
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          default:
          {
            LOG_UNEXPECTED_EVENT(
              rr_event_name(event_in),
              rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
            );
          }
        }
        break;
      } /* RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM */

      /**
      * RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM_EARLY_CAMPED
      *
      *
      * In this state: CELL ACQ state machine is waiting for remainig GSM SI in this state. once these SI receives,
      * cell acq state machine moves to RR_CELL_ACQ_STATE_ACQUIRING_BSI_GPRS_EARLY_CAMPED when it requires SI13. if
      * SI13 is already received and PBCCH is present then cell acq state machine moves to RR_CELL_ACQ_STATE_ACQUIRING_BSI_PSI1
      * state. If there is no PBCCH is present then cell acq sends finished ind and moves to NULL
      *
      * This state handles the following messages and events:
      *
      * Messages:
      * RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND
      * RR_CELL_ACQ_IMSG_ABORT_REQ
      * RR_CELL_ACQ_IMSG_NACC_ACQ_COMPLETE_IND
      * RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF
      * RR_GPRS_SYS_INFO_TIMER
      *
      * Events:
      *
      **/

      case RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM_EARLY_CAMPED:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    switch (message->rr.cell_acq.imsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND:
                      {
                        boolean              cell_suitable;
                        rr_cell_acq_status_e status = RR_CELL_ACQ_STATUS_UNDEFINED;
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND rxd");

                       /* Check that cell is still suitable - this is required if                     
                                          SI3 was received after SI1, SI2 and SI4 as the check will   
                                          not have been done yet. */
                            
                        cell_suitable = rr_cell_acq_is_cell_suitable(
                          rr_cell_acq_data_ptr->target_cell,
                          rr_cell_acq_data_ptr->RXLEV_average,
                          &status,
                          gas_id
                        );
                            
                        if (!cell_suitable)
                        {
                           MSG_GERAN_HIGH_0_G("Cell not suitable, aborting acquisition");
                           rr_cell_acq_send_omsg_finished_ind(status,gas_id);
                           rr_cell_acq_reset_parameters(gas_id);
                           rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                           break;
                        }


                        if (rr_cell_acq_data_ptr->gprs_cell_info->gsm.gprs_indicator)
                        {
                          if (rr_cell_acq_data_ptr->gprs_cell_info->bcch_sys_info.si13_data_valid_flag)
                          {
                            if (rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_NOT_STARTED)
                            {
                              rr_cell_acq_send_omsg_finished_ind(
                                RR_CELL_ACQ_STATUS_SUCCESS_BCCH,
                                gas_id
                              );
                              rr_cell_acq_reset_parameters(gas_id);
                              rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                            }
                            else if (rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_COMPLETED)
                            {
                              rr_cell_acq_send_omsg_finished_ind(
                                RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED,
                                gas_id
                              );
                              rr_cell_acq_reset_parameters(gas_id);
                              rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                            }
                            else if (rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_REQ_SENT)
                            {
                              rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_WAITING_FOR_IDLE_MODE_CNF;
                            }
                          }
                          else
                          {
                            boolean wait_for_si13 = TRUE;
                            boolean is_fast_si_acq_enabled = rr_is_fast_si_acq_enabled(&wait_for_si13, gas_id);

                            if (is_fast_si_acq_enabled && !wait_for_si13)
                            {
                              MSG_GERAN_HIGH_0_G("Fast SI Acq enabled, SI13 not yet received, start GSM idle mode");
                              if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_NOT_STARTED)
                              {
                                rr_cell_acq_send_omsg_finished_ind(RR_CELL_ACQ_STATUS_SUCCESS_BCCH, gas_id);
                                rr_cell_acq_reset_parameters(gas_id);
                                rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                              }
                              else if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_COMPLETED)
                              {
                                rr_cell_acq_send_omsg_finished_ind(RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED, gas_id);
                                rr_cell_acq_reset_parameters(gas_id);
                                rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                              }
                              else if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_REQ_SENT)
                              {
                                rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_WAITING_FOR_IDLE_MODE_CNF;
                              }
                            }
                            else
                            {
                              /* this is used to prepare to receive the bcch si13 in the pending store */
                              rr_cell_acq_data_ptr->gprs_cell_info->bcch_sys_info.si13_expected_flag = TRUE;
                              rr_cell_acq_data_ptr->gprs_cell_info->bcch_sys_info.si13_data_valid_flag = FALSE;

                              MSG_GERAN_MED_0_G("Starting RR_GPRS_SYS_INFO_TIMER");
                              (void)rr_timer_start(RR_GPRS_SYS_INFO_TIMER, BCCH_SI13_TIMEOUT, gas_id);

                              rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_ACQUIRING_BSI_GPRS_EARLY_CAMPED;
                            }
                          }
                        }
                        else
                        {
                          if (rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_NOT_STARTED)
                          {
                            rr_cell_acq_send_omsg_finished_ind(
                              RR_CELL_ACQ_STATUS_SUCCESS_BCCH,
                              gas_id
                            );
                            rr_cell_acq_reset_parameters(gas_id);
                            rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                          }
                          else if (rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_COMPLETED)
                          {
                            rr_cell_acq_send_omsg_finished_ind(
                              RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED,
                              gas_id
                            );
                            rr_cell_acq_reset_parameters(gas_id);
                            rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                          }
                          else if (rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_REQ_SENT)
                          {
                            rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_WAITING_FOR_IDLE_MODE_CNF;
                          }
                        }
                        break;
                      } /* RR_CELL_ACQ_IMSG_GSM_BSI_RECEIVED_IND */

                      case RR_CELL_ACQ_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_ABORT_REQ rxd");

                        rr_cell_acq_reset_parameters(gas_id);
                        rr_cell_acq_send_omsg_abort_cnf(gas_id);
                        rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                        break;
                      } /* RR_CELL_ACQ_IMSG_ABORT_REQ */

                      case RR_CELL_ACQ_IMSG_NACC_ACQ_COMPLETE_IND:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_NACC_ACQ_COMPLETE_IND rxd");

                        rr_cell_acq_reset_parameters(gas_id);
                        rr_cell_acq_send_omsg_finished_ind(
                          RR_CELL_ACQ_STATUS_SUCCESS_BCCH,
                          gas_id
                        );
                        rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                        break;
                      } /* RR_CELL_ACQ_IMSG_NACC_ACQ_COMPLETE_IND */

                      case RR_CELL_ACQ_IMSG_EARLY_CAMPING_BSI_RECEIVED_IND:
                      {
                        rr_cell_acq_status_e status = RR_CELL_ACQ_STATUS_UNDEFINED;
                        boolean              cell_suitable;

                        /* We are already early-camped, but check again whether
                        the cell is still suitable or not.  This check is needed
                        because it is possible that early camping is performed
                        based on old system information, whereas the cell has
                        now become barred and we need to prevent camping on it
                        at this time. */
                        cell_suitable = rr_cell_acq_is_cell_suitable(
                          rr_cell_acq_data_ptr->target_cell,
                          rr_cell_acq_data_ptr->RXLEV_average,
                          &status,
                          gas_id
                        );

                        if (!cell_suitable)
                        {
                          MSG_GERAN_HIGH_0_G("Cell not suitable, aborting acquisition");
                          rr_cell_acq_send_omsg_finished_ind(status, gas_id);
                          rr_cell_acq_reset_parameters(gas_id);
                          rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                        }
                        break;
                      } /* RR_CELL_ACQ_IMSG_EARLY_CAMPING_BSI_RECEIVED_IND */

                      case RR_CELL_ACQ_IMSG_GPRS_BSI_RECEIVED_IND:
                      {
                        /* no action wait for RR_CELL_ACQ_GSM_BSI_RECEIVED_IND */
                        break;
                      } /* RR_CELL_ACQ_IMSG_GPRS_BSI_RECEIVED_IND */


                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_SELECT_BCCH_IMSG:
                  {
                    /* No action necessary. */
                    break;
                  } /* RR_SELECT_BCCH_IMSG */

                  case RR_L1_IDLE_IMSG:
                  {
                    rr_l1_idle_control(message, gas_id);
                    break;
                  }

                  case RR_L1_IDLE_OMSG:
                  {
                    switch (message->rr.l1_idle.omsg.header.imh.message_id)
                    {
                      case RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF:
                      {
                        boolean x2g_resel_with_nacc = FALSE; 
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
                        x2g_resel_with_nacc = rr_x2g_resel_with_nacc_in_progress(gas_id); 
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

                        rr_cell_acq_early_camping_confirmed(gas_id);
                        rr_cell_acq_check_sys_info_timer(MPH_START_IDLE_MODE_CNF, gas_id);

                        if ( (rr_cell_acq_data_ptr->action != RR_CELL_ACQ_ACTION_RESELECT_BCCH_NACC_PCCO_PCCC) &&
                             (x2g_resel_with_nacc == FALSE) )
                        {
                          if (rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity_valid_flag)
                          {
                            if (rr_csi_retrieve_all_SIs(  rr_cell_acq_data_ptr->gprs_cell_info->gsm.BCCH_ARFCN, 
                                                        &(rr_cell_acq_data_ptr->gprs_cell_info->gsm.location_area_identification),
                                                        &(rr_cell_acq_data_ptr->gprs_cell_info->gsm.cell_identity),
                                                          CSI_RETRIEVE_ALL_SIS,
                                                          gas_id) )
                            {
                              MSG_GERAN_HIGH_0_G("RR retrieve SI's after RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF");
                            }
                          }
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_TIMER:
              {
                switch (message_id)
                {
                  case MID_TIMER_EXPIRY:
                  {
                    timer_expiry_T *timer_expiry = (timer_expiry_T *) message;
                    if (timer_expiry->timer_id == RR_GPRS_SYS_INFO_TIMER)
                    {
                      MSG_GERAN_HIGH_0_G("RR_GPRS_SYS_INFO_TIMER rxd");
                      rr_cell_acq_send_omsg_finished_ind(
                        RR_CELL_ACQ_STATUS_FAILURE_BSI_ACQUISITION_FAILURE,
                        gas_id
                      );
                      rr_cell_acq_reset_parameters(gas_id);
                      rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                    }
                    break;
                  } /* MID_TIMER_EXPIRY */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_TIMER */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                );
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event_in),
                                 rr_cell_acq_state_name(rr_cell_acq_data_ptr->state));
          }
        }
        break;
      } /* RR_CELL_ACQ_STATE_ACQUIRING_BSI_GSM_EARLY_CAMPED */

      /**
      * RR_CELL_ACQ_STATE_ACQUIRING_BSI_GPRS_EARLY_CAMPED
      *
      *
      * In this state: CELL ACQ state machine is waiting for SI13 in this state. once SI13 is received if the PBCCH is present
      * then it moves to RR_CELL_ACQ_STATE_ACQUIRING_BSI_PSI1 otherwise cell acq finished acquisition and sends finished ind to
      * parent state machine and moves to NULL state
      *
      * This state handles the following messages and events:
      *
      * Messages:
      * RR_CELL_ACQ_IMSG_GPRS_BSI_RECEIVED_IND
      * RR_CELL_ACQ_IMSG_ABORT_REQ
      * RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF
      * RR_GPRS_SYS_INFO_TIMER
      *
      * Events:
      *
      **/

      case RR_CELL_ACQ_STATE_ACQUIRING_BSI_GPRS_EARLY_CAMPED:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_CELL_ACQ_IMSG:
                  {
                    switch (message->rr.cell_acq.imsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_IMSG_GPRS_BSI_RECEIVED_IND:
                      {
                        MSG_GERAN_HIGH_0_G("RR_CELL_ACQ_IMSG_GPRS_BSI_RECEIVED_IND rxd");

                        {
                          if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_NOT_STARTED)
                          {
                            rr_cell_acq_send_omsg_finished_ind(
                              RR_CELL_ACQ_STATUS_SUCCESS_BCCH,
                              gas_id
                            );
                            rr_cell_acq_reset_parameters(gas_id);
                            rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                          }
                          else if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_COMPLETED)
                          {
                            rr_cell_acq_send_omsg_finished_ind(
                              RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED,
                              gas_id
                            );
                            rr_cell_acq_reset_parameters(gas_id);
                            rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                          }
                          else if(rr_cell_acq_data_ptr->early_camping_status == RR_CELL_ACQ_EARLY_CAMPING_REQ_SENT)
                          {
                            rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_WAITING_FOR_IDLE_MODE_CNF;
                          }
                        }

                        break;
                      }

                      case RR_CELL_ACQ_IMSG_EARLY_CAMPING_BSI_RECEIVED_IND:
                      {
                        break;
                      }

                      case RR_CELL_ACQ_IMSG_ABORT_REQ:
                      {
                        rr_cell_acq_reset_parameters(gas_id);

                        rr_cell_acq_send_omsg_abort_cnf(gas_id);

                        rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;

                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  case RR_SELECT_BCCH_IMSG:
                  {
                    /* No action necessary. */
                    break;
                  } /* RR_SELECT_BCCH_IMSG */

                  case RR_L1_IDLE_IMSG:
                  {
                    rr_l1_idle_control(message, gas_id);
                    break;
                  }

                  case RR_L1_IDLE_OMSG:
                  {
                    switch (message->rr.l1_idle.omsg.header.imh.message_id)
                    {
                      case RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF:
                      {
                        rr_cell_acq_early_camping_confirmed(gas_id);
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }

                break;
              }

              case MS_TIMER:
              {
                switch (message_id)
                {
                  case MID_TIMER_EXPIRY:
                  {
                    timer_expiry_T *timer_expiry = (timer_expiry_T *) message;
                    if (timer_expiry->timer_id == RR_GPRS_SYS_INFO_TIMER)
                    {
                      MSG_GERAN_HIGH_0_G("RR_GPRS_SYS_INFO_TIMER rxd");

                      rr_cell_acq_send_omsg_finished_ind(
                        RR_CELL_ACQ_STATUS_FAILURE_BSI_ACQUISITION_FAILURE,
                        gas_id
                      );
                      rr_cell_acq_reset_parameters(gas_id);
                      rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                    }
                    break;
                  } /* MID_TIMER_EXPIRY */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_TIMER */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                );
              }
            }
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event_in),
                                 rr_cell_acq_state_name(rr_cell_acq_data_ptr->state));
          }
        }
        break;
      } /* RR_CELL_ACQ_STATE_ACQUIRING_BSI_GPRS_EARLY_CAMPED */

      /**
      * RR_CELL_ACQ_STATE_WAITING_FOR_IDLE_MODE_CNF
      *
      *
      * In this state: CELL ACQ state machine is waiting for idle mode cnf for the early camping before it sends cell acq finished ind to resel control
      * This state handles the following messages and events:
      *
      * Messages:
      * RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF
      *
      * Events:
      *
      **/

      case RR_CELL_ACQ_STATE_WAITING_FOR_IDLE_MODE_CNF:
      {
        switch (event_in)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message->rr.header.rr_message_set)
                {
                  case RR_L1_IDLE_IMSG:
                  {
                    rr_l1_idle_control(message, gas_id);
                    break;
                  }

                  case RR_L1_IDLE_OMSG:
                  {
                    switch (message->rr.l1_idle.omsg.header.imh.message_id)
                    {
                      case RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF:
                      {
                        rr_cell_acq_early_camping_confirmed(gas_id);

                        rr_cell_acq_send_omsg_finished_ind(
                          RR_CELL_ACQ_STATUS_SUCCESS_BCCH_EARLY_CAMPED,
                          gas_id
                        );
                        rr_cell_acq_reset_parameters(gas_id);
                        rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  case RR_CELL_ACQ_IMSG:
                  {
                    switch (message->rr.cell_acq.imsg.header.imh.message_id)
                    {
                      case RR_CELL_ACQ_IMSG_ABORT_REQ:
                      {
                        rr_cell_acq_reset_parameters(gas_id);

                        rr_cell_acq_send_omsg_abort_cnf(gas_id);

                        rr_cell_acq_data_ptr->state = RR_CELL_ACQ_STATE_NULL;

                        break;
                      }
					  
                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message,
                          rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                         );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message,
                      rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                    );
                  }
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message,
                  rr_cell_acq_state_name(rr_cell_acq_data_ptr->state)
                );
              }
            }
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event_in),
                                 rr_cell_acq_state_name(rr_cell_acq_data_ptr->state));
          }
        }
        break;
      }


      default:
      {
        /* TODO:  Only likely reason for this to happen is if memory corruption
        has occurred - trigger a panic reset? */
        MSG_GERAN_ERROR_1_G("In unknown state (%d)", rr_cell_acq_data_ptr->state);
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    /* no old_state in this state machine: NOTE event conversion */
    if ((event_in != new_event) && (new_event != EV_NO_EVENT))
    {
      rr_store_trace_msg_buf(
        RR_CELL_ACQ_SM,
        new_event,
        (byte)rr_cell_acq_data_ptr->state,
        NULL,
        gas_id
      );
    }
#endif

    if (new_event != EV_NO_EVENT)
    {
      event_in = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event_in = EV_NO_EVENT;
    }

    if (rr_cell_acq_data_ptr->state != rr_cell_acq_data_ptr->old_state)
    {
      MSG_SPRINTF_2(MSG_SSID_DFLT,
                    MSG_LEGACY_HIGH,
                    "rr_cell_acq_control(%s->%s)",
                    rr_cell_acq_state_name(rr_cell_acq_data_ptr->old_state),
                    rr_cell_acq_state_name(rr_cell_acq_data_ptr->state));

      /* If the old state is a transitory state, cancel the state transition
      guard timer. */
      if (rr_cell_acq_control_state_is_transitory(rr_cell_acq_data_ptr->old_state))
      {
        MSG_GERAN_MED_1_G("Cancelling state transition guard timer (left state %d)",
                rr_cell_acq_data_ptr->old_state);
        rr_timer_cancel(RR_CELL_ACQ_CONTROL_STATE_TRANSITION_GUARD_TIMER, gas_id);
      }

      /* If the new state is a transitory state, start the state transition
      guard timer. */
      if (rr_cell_acq_control_state_is_transitory(rr_cell_acq_data_ptr->state))
      {
        MSG_GERAN_MED_2_G("Starting state transition guard timer (entered state %d, duration %d ms)",
                rr_cell_acq_data_ptr->state, RR_CELL_ACQ_CONTROL_STATE_TRANSITION_TIMEOUT);
        rr_timer_start(RR_CELL_ACQ_CONTROL_STATE_TRANSITION_GUARD_TIMER,
                       RR_CELL_ACQ_CONTROL_STATE_TRANSITION_TIMEOUT,
                       gas_id);
      }

      rr_cell_acq_data_ptr->old_state = rr_cell_acq_data_ptr->state;
    }
    else if (rr_cell_acq_data_ptr->state == RR_CELL_ACQ_STATE_NULL)
    {
      /* If this state machine is reset and the old_state variable is cleared
      while the state transition guard timer is running, the timer will not be
      cancelled by the above code, as no state transition will be detected.  To
      avoid this false detection of an error, we explicitly check for the guard
      timer running if the current state is RR_CELL_ACQ_STATE_NULL (which
      should never be the case) and cancel the timer if necessary. */
      if (rr_timer_is_running(RR_CELL_ACQ_CONTROL_STATE_TRANSITION_GUARD_TIMER, gas_id))
      {
        MSG_GERAN_MED_1_G("Cancelling state transition guard timer (state %d)",
                rr_cell_acq_data_ptr->state);
        rr_timer_cancel(RR_CELL_ACQ_CONTROL_STATE_TRANSITION_GUARD_TIMER, gas_id);
      }
    }
  }

} /* end rr_cell_acq_control() */

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/* EOF */

