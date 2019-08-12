/*============================================================================
  FILE:         rr_resel_calcs.c

  OVERVIEW:     RR reselection related calculations.

  DEPENDENCIES: None

                Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_resel_calcs.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_resel_calcs.h"
#include "rr_resel.h"
#include "rr_resel_utils.h"
#include "rr_general.h"
#include "rr_gprs_defs.h"
#include "rr_gprs_dsc.h"
#include "rr_sys_info_ncell.h"
#include "geran_eng_mode_info.h"
#include "rr_log.h"
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
#include "rr_callback.h"
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
#include "rr_ded_meas_reporting.h"
#ifdef FEATURE_WCDMA
#include "rr_g2w.h"
#endif /* FEATURE_WCDMA */
#include "rr_multi_sim.h"
#include "rr_ps_domain.h"
#include "rr_nv.h"
#include "rr_sim.h"
#ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST
#include "rr_l1_send.h"
#endif /* FEATURE_GSM_SCE_REDUCED_NCELL_LIST */

/*----------------------------------------------------------------------------
 * Local Defines
 *--------------------------------------------------------------------------*/

/* Idle SNR reselection parameters */
#define SCELL_IDLE_SNR_COUNTER_URGENCY_THRESHOLD 3 //no. of times idle snr_counter is decremented before things are considered urgent
#define SCELL_XFER_SNR_COUNTER_URGENCY_THRESHOLD 4 //no. of times xfer snr_counter is decremented before things are considered urgent
#define VALID_NCELL_SNR_RESELECTION_CANDIDATE 1
#define FAST_HYSTERESIST_ADJUST 5 //dB
#define FAST_INSTANT_HYSTERESIST_ADJUST 10 //dB
#define MAX_SLOW_HYSTERESIS_ADJUST_dB 3 //dB
#define MIN_SLOW_HYSTERESIS_ADJUST_dB 0 //dB

#ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST
#define RECVD_REDUCED_NCELL_LIST_SIZE   4
#define MIN_SERV_C2_DIFF_TOP6_MODE      5  //dB
#define MIN_SERV_C2_DIFF_TOP2_MODE      3  //dB
#endif /* FEATURE_GSM_SCE_REDUCED_NCELL_LIST */

#ifdef FEATURE_DUAL_SIM
#define SERVING_RX_VAL_THRESHOLD  30   /*This is equal to -80dbm*/
 
#endif /*FEATURE_DUAL_SIM*/


/*----------------------------------------------------------------------------
 * Local Types
 *--------------------------------------------------------------------------*/

typedef struct
{
  /* Counter used to judge how long the scell snr has been judged as bad by L1 */
  uint8 bad_snr_counter;
} rr_gprs_resel_calc_data_t;

/*----------------------------------------------------------------------------
 * Public Variables
 *--------------------------------------------------------------------------*/

/* Array of possible values of cell reselection offset, the GPRS_RESELECTION_OFFSET value
should be used as the index into the array */
const int GPRS_RESELECT_OFFSET_dB[32] = {
  -52, -48, -44, -40, -36, -32, -28, -24, -20, -16, -12, -10,  -8,  -6,  -4,  -2,
    0,   2,   4,   6,   8,  10,  12,  16,  20,  24,  28,  32,  36,  40,  44,  48
};

/**
  @brief Data store containing the details of the current best six neighbour
  cells.

  GRR maintains a list of the current best six neighbour cells, as reported
  by GL1.  This corresponds to the strongest six entries from the last
  received MPH_SURROUND_MEAS_IND.

  Whenever an MPH_SURROUND_MEAS_IND is received, the strongest (in terms of
  RXLEV_average) six entries contained within it are added to the
  best_six_ncells store.  A penalty timer is allocated for each entry as it
  is added to the store.  A five second timer is allocated for each entry if
  the serving cell does not have PBCCH (or if it does but the UE is camped
  on the BCCH for some reason).

  @see
  */
static best_six_ncells_t  best_six_ncells[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Local Variables
 *--------------------------------------------------------------------------*/

static rr_gprs_resel_calc_data_t  rr_resel_calcs_data[NUM_GERAN_DATA_SPACES];

/***********************************************************************/
/* A list of measurement values for the serving and surrounding cells. */
/* Each time a measurement report is received from L1 a new entry is   */
/* added to the list                                                   */
/***********************************************************************/
static gprs_reselection_structure_T gprs_reselection_list[NUM_GERAN_DATA_SPACES];

/* This type will hold a list of potential target cells for reselection */
static gprs_better_cell_list_T better_cell_list[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_gprs_resel_calc_data_t*
 */
static rr_gprs_resel_calc_data_t *rr_resel_calcs_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_resel_calcs_data[as_index];
}

#ifndef DISABLE_XFER_SNR_RESELECTION
/****************************************************************************
 *   Function name:  rr_gprs_check_for_bad_scell_in_xfer
 *   -------------
 *
 *   Description: This function is used to run the Qualcomm devised scell SNR
 *   ------------ algorithm, to determine if the scell should be dumped and
 *                another ncell targeted for reselection. This algorithm is only
 *                for use in transfer mode. It will look to find if any cells have
 *                thier 5 sec timers running and if so will chose one of these.
 *                Otherwise it will go through the list of available ncells and
 *                see if any are better if the Ready timer hysteresis is ignored.
 *                After that if any are better without the RA/LA hysteresis.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  TRUE = bad scell / FALSE = scell is OK
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_check_for_bad_scell_in_xfer(
  const gas_id_t gas_id
)
{
  boolean                       force_bad_scell_reselection = FALSE;
  uint8                         better_ncell_index, tested_cell_index;
  bcch_ncell_info_t            *ptr_bcch_surround_db = NULL;
  rr_resel_candidate_t         *resel_candidate = NULL;
  gprs_reselection_values_T    *ptr_best_ncell_c2 = NULL;
  gprs_reselection_structure_T  temp_ncell_scell_reselection_values;
  gprs_reselection_structure_T  out_of_ra_la_reselection_candidates;
  rr_gprs_resel_calc_data_t    *rr_resel_calcs_data_ptr;
  gprs_reselection_structure_T *local_gprs_reselection_list_ptr;

  // Obtain a pointer to the module data
  rr_resel_calcs_data_ptr = rr_resel_calcs_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_calcs_data_ptr);

  local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
  RR_NULL_CHECK_FATAL(local_gprs_reselection_list_ptr);

  RR_ZERO_STRUCTURE(out_of_ra_la_reselection_candidates);

  if (rr_gprs_camped_on_ccch(gas_id) && local_gprs_reselection_list_ptr->transfer_serving_snr_is_bad)
  {

    MSG_GERAN_HIGH_1_G("++SNR reselection Xfer test bsc=%d++",rr_resel_calcs_data_ptr->bad_snr_counter);
    /*
    Store the data in the reselection structure as it may be destroyed during
    the call to test for nC2 near sC2
    */
    temp_ncell_scell_reselection_values = *local_gprs_reselection_list_ptr;

    while (local_gprs_reselection_list_ptr->no_of_entries && rr_gprs_is_snr_counter_zero(gas_id))
    {
      /* Assume that the first entry is the best C2 and match up instant values*/
      ptr_best_ncell_c2 = &local_gprs_reselection_list_ptr->surrounding_values[0];
      tested_cell_index = 0;

      /* Go through the other entries to make sure that this is still the best cell */
      for (better_ncell_index = 1;
           (better_ncell_index < local_gprs_reselection_list_ptr->no_of_entries) &&
           (better_ncell_index < RR_GPRS_RESELECTION_STRUCTURE_SIZE);
           ++better_ncell_index)
      {
        if (ptr_best_ncell_c2->C2 < local_gprs_reselection_list_ptr->surrounding_values[better_ncell_index].C2)
        {
          ptr_best_ncell_c2 = &local_gprs_reselection_list_ptr->surrounding_values[better_ncell_index];
          tested_cell_index = better_ncell_index;
        }
      }

      resel_candidate = rr_resel_best_six_find(ptr_best_ncell_c2->BCCH_ARFCN, NULL, gas_id);

      if (resel_candidate == NULL)
      {
        MSG_GERAN_ERROR_2_G("No resel candidate found in best six for ARFCN %d (band %d)",
          ptr_best_ncell_c2->BCCH_ARFCN.num,
          ptr_best_ncell_c2->BCCH_ARFCN.band);
      }

      /* Make sure that the cell is not on the forbidden list */
      else if (rr_cell_forbidden(ptr_best_ncell_c2->BCCH_ARFCN, gas_id))
      {
        MSG_GERAN_MED_1_G("Ncell is forbidden %d",CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN));
      }

      /* If a 5 second timer is already running for this cell the just move to it ! */
      else if (rr_timer_is_running(resel_candidate->reselection_info.five_second_timer_id, gas_id))
      {
        force_bad_scell_reselection = TRUE;
        MSG_GERAN_HIGH_1_G("++SNR xfer-resel to better cell %d++",CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN));
        /*
        Make this candidate the only entry in the reselection list. This needs to be done so that
        this cell is definatly the one targeted. Sometimes when starting the reselection process off
        the cell with the 5 sec timer running is not chosen. This is due to the Ready state
        reselection hysteresis preventing another cell from having started its better cell timer off
        but this cell is then deemed OK when choosing cells via the DSF route.
        */
        local_gprs_reselection_list_ptr->surrounding_values[0] = *ptr_best_ncell_c2;
        local_gprs_reselection_list_ptr->no_of_entries = 1;
        break;
      }

      /*
      Remove the tested entry from the reselection list
      */
      rr_gprs_reselection_list_delete_index(
        local_gprs_reselection_list_ptr,
        tested_cell_index
      );
    }

    if (!force_bad_scell_reselection)
    {

      /* restore the reselection measurment list */
      *local_gprs_reselection_list_ptr = temp_ncell_scell_reselection_values;

      /* Next test is .....*/
      /* See if an ncell exists that is better when no READY timer hysteresis is applied */
      /* See if an ncell exists that is better without the RA/LA hysteresis */

      while ( local_gprs_reselection_list_ptr->no_of_entries && rr_gprs_is_snr_counter_zero(gas_id) )
      {
        /* Assume that the first entry is the best C2 and match up instant values*/
        ptr_best_ncell_c2 = &local_gprs_reselection_list_ptr->surrounding_values[0];
        tested_cell_index = 0;

        /* Go through the other entries to make sure that this is still the best cell */
        for (better_ncell_index = 1;
             (better_ncell_index < local_gprs_reselection_list_ptr->no_of_entries) &&
             (better_ncell_index < RR_GPRS_RESELECTION_STRUCTURE_SIZE);
             ++better_ncell_index)
        {
          if (ptr_best_ncell_c2->C2 < local_gprs_reselection_list_ptr->surrounding_values[better_ncell_index].C2)
          {
            ptr_best_ncell_c2 = &local_gprs_reselection_list_ptr->surrounding_values[better_ncell_index];
            tested_cell_index = better_ncell_index;
          }
        }

        ptr_bcch_surround_db = rr_gprs_get_surround_ncell_sys_info(ptr_best_ncell_c2->BCCH_ARFCN, gas_id);

        if ( ptr_bcch_surround_db == NULL )
        {
          MSG_GERAN_MED_1_G("No bcch entry found %d",CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN));
        }
        /* Make sure that the cell is not on the forbidden list */
        else if ( rr_cell_forbidden( ptr_best_ncell_c2->BCCH_ARFCN, gas_id ) )
        {
          MSG_GERAN_MED_1_G("Ncell is forbidden %d",CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN));
        }
        /* Make sure that the cell is suitable before bothering to test it further */
        else if ( rr_gprs_resel_suitable_cell( ptr_bcch_surround_db, gas_id ) && ( ptr_best_ncell_c2->C1 > 0 || rr_allow_negative_c1(ptr_bcch_surround_db->gsm.location_area_identification.plmn_id, gas_id) )
) {
          /*
          Test the ncell to see if it meets the criteria of being in the same RA/LA and being better
          than the scell without the usual ready timer hysteresis.
          */
          gprs_scell_info_t * scell_info = rr_gprs_get_scell_info(gas_id);

          boolean same_ra = TRUE;

          /*Check to see if the cells are in the same RA, this can only be done if both the cells support gprs */
          if ( (ptr_bcch_surround_db->gprs.ra_colour != scell_info->bcch_sys_info.ra_colour) &&
                scell_info->gsm.gprs_indicator &&
                ptr_bcch_surround_db->gprs_supported
             )
          {
            same_ra = FALSE;
          }

          if( (ptr_best_ncell_c2->C2 >= local_gprs_reselection_list_ptr->serving_values.C2) &&
               same_ra &&
              (rr_same_lai(&ptr_bcch_surround_db->gsm.location_area_identification,&scell_info->gsm.location_area_identification)))
          {
            force_bad_scell_reselection = TRUE;
            MSG_GERAN_HIGH_1_G("++SNR xfer-resel cell %d++",CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN));
            rr_log_set_reselection_trigger(RR_XFER_SNR_RESELECTION_TRIGGER, gas_id);
            break;
          }
          else

          if( ptr_best_ncell_c2->C2 >= local_gprs_reselection_list_ptr->serving_values.C2 )
          {
            out_of_ra_la_reselection_candidates.surrounding_values[out_of_ra_la_reselection_candidates.no_of_entries++]
              = *ptr_best_ncell_c2;
          }
        }

        /*
        Remove the tested entry from the reselection list
        */
        rr_gprs_reselection_list_delete_index(
          local_gprs_reselection_list_ptr,
          tested_cell_index
        );
      }

#if 0 // don't allow further options for reselection this is the RALA reselection decision
      if ( out_of_ra_la_reselection_candidates.no_of_entries && !force_bad_scell_reselection)
      {
        gprs_reselection_list = out_of_ra_la_reselection_candidates;
        force_bad_scell_reselection = TRUE;
        MSG_GERAN_HIGH_1_G("++SNR xfer-LARA resel cell %d++",ptr_best_ncell_c2->BCCH_ARFCN);
        rr_log_set_reselection_trigger(RR_XFER_LARA_SNR_RESELECTION_TRIGGER);
      }
#endif //don't allow further options for reselection
    }
  }

  return force_bad_scell_reselection;

}
#endif /* DISABLE_XFER_SNR_RESELECTION */

#ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST
/****************************************************************************
 *   Function name:  rr_gprs_compare_serv_and_surr_C2
 *   --------------
 *
 *   Description: This function examines that   
 *   -----------  serving cell C2 is within 3dB to nC2 (sC2-nC2 <= 3) when GL1 in TOP2 mode  
 *                serving cell C2 is within 5dB to nC2 (sC2-nC2 >= 5) when GL1 in TOP6 mode
 *
 *   Parameters: pointer to reselection data
 *   ----------- serv_c2_offset - 3dB = TOP2 mode, 5dB= TOP6 mode
 *               
 *
 *   Returns:  TRUE/FALSE
 *   --------
 *
 ****************************************************************************/

static boolean rr_gprs_compare_serv_and_surr_C2(
  gprs_reselection_structure_T * reselection_structure_ptr,
  int                            serv_c2_offset, 
  const gas_id_t                 gas_id
)
{
  uint8 better_ncell_index;
  boolean reduced_ncell_list = FALSE;
  uint8  num_entries = reselection_structure_ptr->no_of_entries;

  MSG_GERAN_HIGH_1_G("Reduced ncell list test: %ddB check",serv_c2_offset);
  
  /*
  Go through the ncell list starting from the best C2, and see if any cells have
  nc2 within 5dB(TOP6 mode) or 3dB(TOP2 mode)of sc2(not considering the all aspects of hysteresis, 
  as this is not for reselection). The complete list is compared.
  */
  
  /* don't go over the size */
  if(num_entries > RR_GPRS_RESELECTION_STRUCTURE_SIZE)
  {
    num_entries = RR_GPRS_RESELECTION_STRUCTURE_SIZE;
  }

  /* for each cell in the reselection database */
  for (better_ncell_index=0;
       (better_ncell_index < num_entries);
       better_ncell_index++)
  {
    /**********************************************************************/
    /* compare the C2 values for both the surrounding and serving         */
    /* cells are available, perform the comparison using the values       */
    /* cells, to see if the surrounding cell is within 5dB or 3dB         */
    /* check is sC2 - nC2 >= 5dB ~ sC2 >= nC2+5dB  // SCE of GL1 in TOP6  */
    /* check is sC2 - nC2 >= 3dB ~ sC2 >= nC2+3dB  // SCE of GL1 in TOP2  */
    /**********************************************************************/

    MSG_GERAN_MED_4_G("ncell ARFCN:%d, nC2:%d, scell ARFCN:%d, sc2:%d",
        CONVERT_FROM_ARFCN_T(reselection_structure_ptr->surrounding_values[better_ncell_index].BCCH_ARFCN),
        reselection_structure_ptr->surrounding_values[better_ncell_index].C2,
        CONVERT_FROM_ARFCN_T(reselection_structure_ptr->serving_values.BCCH_ARFCN),
        reselection_structure_ptr->serving_values.C2);

    if (reselection_structure_ptr->serving_values.C2 >= reselection_structure_ptr->surrounding_values[better_ncell_index].C2 + serv_c2_offset ) 
    {
      reduced_ncell_list = TRUE;
    }
    else
    {
      reduced_ncell_list = FALSE;
      break;
    }
  }
  MSG_GERAN_HIGH_1_G("reduced_ncell_list: %d",reduced_ncell_list);
  return reduced_ncell_list;
}
#endif /* FEATURE_GSM_SCE_REDUCED_NCELL_LIST*/

/****************************************************************************
 *   Function name:  rr_gprs_is_nC2_near_sC2_and_nC2inst_greater_than_sC2inst
 *   -------------
 *
 *   Description: This function tests all the current ncells to see if a ncell C2
 *   ------------ is close to the serving cell C2. The closeness is defined by the
 *                hysteresis_adjust parameter.
 *                If a ncell passes this test then the instantaneous values are
 *                examined. The ncell's instantaneous C2 values must be greater
 *                than the scells instantaneous C2 values by instant_hysteresis_adjust.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns: TRUE = ncell passes both tests / FALSE = no ncell
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_is_nC2_near_sC2_and_nC2inst_greater_than_sC2inst(
  const gas_id_t gas_id
)
{
  uint8                              better_ncell_index, tested_cell_index;
  uint8                              hysteresis_adjust = FAST_HYSTERESIST_ADJUST;
#ifdef PERLUTF
  #error code not present
#else
  uint8                              instant_hysteresis_adjust = FAST_INSTANT_HYSTERESIST_ADJUST;
#endif
  bcch_ncell_info_t                 *ptr_bcch_surround_db = NULL;
  boolean                            ncell_near_scell = FALSE;
  boolean                            snr_target_cell_found = FALSE;
  gprs_reselection_values_T         *ptr_best_ncell_c2 = NULL;
  gprs_instant_reselection_values_T *ptr_best_ncell_instant_c2 = NULL;
  gprs_reselection_structure_T       snr_reselection_target_list;
  rr_resel_candidate_t              *resel_candidate;
  gprs_reselection_structure_T      *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  snr_reselection_target_list = *local_gprs_reselection_list_ptr;

  /******************************************************************************/
  /* The snr_reselection_target_list will hold the ncells that are marked the   */
  /* VALID_NCELL_SNR_RESELECTION_CANDIDATE number of times, and can now be used */
  /* in the snr reselection. There can be no entries at this point              */
  /* There may be several targets and cell reselection will decide the best cell*/
  /* from these to target                                                       */
  /******************************************************************************/
  snr_reselection_target_list.no_of_entries = 0;

  /*
  Go through the ncell list starting from the best C2 down, and see if any cells have
  nc2 within 5dB of sc2 including all aspects of hysteresis.
  The complete list is compared as there may be cells in the bottom part of the list that
  have lower C2 but fulfill the criteria as they are in the same RA whilst others at the
  top are not.
  */
  MSG_GERAN_MED_0_G("++SNR Reselection Test++");

  while (local_gprs_reselection_list_ptr->no_of_entries)
  {
    /* Assume that the first entry is the best C2 and match up instant values*/
    ptr_best_ncell_c2 = &local_gprs_reselection_list_ptr->surrounding_values[0];
    ptr_best_ncell_instant_c2 = &local_gprs_reselection_list_ptr->instant_surrounding_values[0];
    tested_cell_index = 0;

    /* Go through the other entries to make sure that this is still the best cell */
    for (better_ncell_index = 1;
         (better_ncell_index < local_gprs_reselection_list_ptr->no_of_entries) &&
         (better_ncell_index < RR_GPRS_RESELECTION_STRUCTURE_SIZE);
         ++better_ncell_index)
    {
      if (ptr_best_ncell_c2->C2 < local_gprs_reselection_list_ptr->surrounding_values[better_ncell_index].C2)
      {
        ptr_best_ncell_c2 = &local_gprs_reselection_list_ptr->surrounding_values[better_ncell_index];
        ptr_best_ncell_instant_c2 = &local_gprs_reselection_list_ptr->instant_surrounding_values[better_ncell_index];
        tested_cell_index = better_ncell_index;
      }
    }

    ptr_bcch_surround_db = rr_gprs_get_surround_ncell_sys_info(ptr_best_ncell_c2->BCCH_ARFCN, gas_id);
    resel_candidate = rr_resel_best_six_find(ptr_best_ncell_c2->BCCH_ARFCN, NULL, gas_id);

    if (ptr_bcch_surround_db == NULL)
    {
      MSG_GERAN_MED_1_G("No bcch entry found %d", CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN));
    }

    else if (resel_candidate == NULL)
    {
      MSG_GERAN_MED_1_G("Resel candidate not found %d", CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN));
    }

    /* Make sure that the cell is not on the forbidden list */
    else if (rr_cell_forbidden( ptr_best_ncell_c2->BCCH_ARFCN, gas_id))
    {
      MSG_GERAN_MED_1_G("Ncell is forbidden %d", CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN));
    }

    /* If a 5 second timer is already running for this cell then just move to it ! */
    else if (rr_timer_is_running(resel_candidate->reselection_info.five_second_timer_id, gas_id))
    {
      /* Mark the cell as a candidate */
      ptr_bcch_surround_db->fast_snr_candidate_marker++;

      MSG_GERAN_HIGH_2_G("++SNR fast mark target ncell:%d %d ++",
        CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN),
        ptr_bcch_surround_db->fast_snr_candidate_marker);

      if (ptr_bcch_surround_db->fast_snr_candidate_marker >= VALID_NCELL_SNR_RESELECTION_CANDIDATE)
      {
        if (snr_reselection_target_list.no_of_entries < RR_GPRS_RESELECTION_STRUCTURE_SIZE)
        {
          snr_target_cell_found = TRUE;
          MSG_GERAN_HIGH_1_G("++SNR resel to better cell %d++",
                             CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN));
          snr_reselection_target_list.surrounding_values[snr_reselection_target_list.no_of_entries] = *ptr_best_ncell_c2;
          snr_reselection_target_list.no_of_entries++;
        }
        else
        {
          MSG_GERAN_ERROR_0_G("snr_reselection_target_list full");
        }
      }
    }
    /* Make sure that the cell is suitable before bothering to test it further */
    else if (rr_gprs_resel_suitable_cell(ptr_bcch_surround_db, gas_id) && (ptr_best_ncell_c2->C1 > 0 || rr_allow_negative_c1(ptr_bcch_surround_db->gsm.location_area_identification.plmn_id, gas_id)))
    {
      /*
      Test the ncell to see if it meets the criteria of being within 5dB of the scell
      after the hysteresis has been added.
      */
      ncell_near_scell = rr_gprs_compare_serving_and_surrounding_C2(
        local_gprs_reselection_list_ptr->serving_values.C2 - hysteresis_adjust,
        ptr_best_ncell_c2->C2,
        ptr_bcch_surround_db,
        FALSE,
        &(ptr_bcch_surround_db->gsm.location_area_identification),
        gas_id
      );

      /*
      If the scell passes the test and is close enough to the scell then try the second
      test to see if the instantaneous values of the ncell are greater than those of the
      scell.
      */
      if (ncell_near_scell)
      {
        MSG_GERAN_MED_3_G("ARFCN:%d nC2:%d sC2-hyst.adj:%d",
          CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN),
          ptr_best_ncell_c2->C2,
          local_gprs_reselection_list_ptr->serving_values.C2 - hysteresis_adjust);

        ncell_near_scell = FALSE;

        ncell_near_scell = rr_gprs_compare_serving_and_surrounding_C2(
          local_gprs_reselection_list_ptr->instant_serving_values.C2_instant + instant_hysteresis_adjust,
          ptr_best_ncell_instant_c2->C2_instant,
          ptr_bcch_surround_db,
          FALSE,
          &(ptr_bcch_surround_db->gsm.location_area_identification),
          gas_id
        );

        MSG_GERAN_MED_3_G("ARFCN:%d nC2inst:%d sC2inst+hyst.adj:%d",
          CONVERT_FROM_ARFCN_T(ptr_best_ncell_instant_c2->BCCH_ARFCN),
          ptr_best_ncell_instant_c2->C2_instant,
          local_gprs_reselection_list_ptr->instant_serving_values.C2_instant + instant_hysteresis_adjust
        );

        /*
        If the ncell passes the test mark it as passing. After VALID_NCELL_SNR_RESELECTION_CANDIDATE number
        of passes the cell reselection is started to this cell.
        */
        if (ncell_near_scell)
        {
          ptr_bcch_surround_db->fast_snr_candidate_marker++;

          MSG_GERAN_HIGH_2_G("++SNR fast mark target ncell:%d %d ++",
              CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN),
              ptr_bcch_surround_db->fast_snr_candidate_marker);

          if (ptr_bcch_surround_db->fast_snr_candidate_marker >= VALID_NCELL_SNR_RESELECTION_CANDIDATE)
          {
            if (snr_reselection_target_list.no_of_entries < RR_GPRS_RESELECTION_STRUCTURE_SIZE)
            {
              MSG_GERAN_HIGH_0_G("++Start SNR reselection++");
              snr_target_cell_found = TRUE;
              snr_reselection_target_list.surrounding_values[snr_reselection_target_list.no_of_entries] = *ptr_best_ncell_c2;
              snr_reselection_target_list.no_of_entries++;
            }
            else
            {
              MSG_GERAN_ERROR_0_G("snr_reselection_target_list full");
            }
          }
        }
        else
        {
          ptr_bcch_surround_db->fast_snr_candidate_marker = 0;
        }
      }
    }

    /*
    Remove the tested entry from the reselection list
    */
    rr_gprs_reselection_list_delete_index(
      local_gprs_reselection_list_ptr,
      tested_cell_index
    );
  }

  if (snr_target_cell_found)
  {
    /* Copy over the list of candidates */
    *local_gprs_reselection_list_ptr = snr_reselection_target_list;
  }

  return snr_target_cell_found;
}


/****************************************************************************
 *   Function name:  rr_gprs_test_for_slow_snr_reselection
 *   -------------
 *
 *   Description: Simple test to see if the ncell is greater than the scell
 *   ------------ when no CRH is applied whether the cell is in the same RA/LA
 *                or the MS is in the READY state.
 *                The purpose is to see if there is a better cell to move to as
 *                this function is called when the SNR is bad on the scell.
 *
 *   Parameters: hysteresis_adjust
 *   -----------
 *
 *   Returns: TRUE = ncell passes both tests / FALSE = no ncell
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_test_for_slow_snr_reselection(
  uint8 hysteresis_adjust,
  const gas_id_t gas_id
)
{
  uint8                         better_ncell_index, tested_cell_index;
  bcch_ncell_info_t            *ptr_bcch_surround_db = NULL;
  boolean                       ncell_better = FALSE;
  gprs_reselection_values_T    *ptr_best_ncell_c2 = NULL;
  gprs_reselection_structure_T  snr_reselection_target_list;
  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  /******************************************************************************/
  /* The snr_reselection_target_list will hold the ncells that are marked the   */
  /* VALID_NCELL_SNR_RESELECTION_CANDIDATE number of times, and can now be used */
  /* in the snr reselection. There can be no entries at this point              */
  /* There may be several targets and cell reselection will decide the best cell*/
  /* from these to target                                                       */
  /******************************************************************************/
  snr_reselection_target_list = *local_gprs_reselection_list_ptr;
  snr_reselection_target_list.no_of_entries = 0;

  while (local_gprs_reselection_list_ptr->no_of_entries)
  {
    /* Assume that the first entry is the best C2 and match up instant values*/
    ptr_best_ncell_c2 = &local_gprs_reselection_list_ptr->surrounding_values[0];
    tested_cell_index = 0;

    /* Go through the other entries to make sure that this is still the best cell */
    for (better_ncell_index = 1;
         (better_ncell_index < local_gprs_reselection_list_ptr->no_of_entries) &&
         (better_ncell_index < RR_GPRS_RESELECTION_STRUCTURE_SIZE);
         ++better_ncell_index)
    {
      if (ptr_best_ncell_c2->C2 < local_gprs_reselection_list_ptr->surrounding_values[better_ncell_index].C2)
      {
        ptr_best_ncell_c2 = &local_gprs_reselection_list_ptr->surrounding_values[better_ncell_index];
        tested_cell_index = better_ncell_index;
      }
    }

    ptr_bcch_surround_db = rr_gprs_get_surround_ncell_sys_info(ptr_best_ncell_c2->BCCH_ARFCN, gas_id);

    if ( ptr_bcch_surround_db == NULL )
    {
      MSG_GERAN_MED_1_G("No bcch entry found %d",CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN));
    }
    /* Make sure that the cell is not on the forbidden list */
    else if ( rr_cell_forbidden( ptr_best_ncell_c2->BCCH_ARFCN, gas_id ) )
    {
      MSG_GERAN_MED_1_G("Ncell is forbidden %d",CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN));
    }
    /* Make sure that the cell is suitable before bothering to test it further */
    else if ( rr_gprs_resel_suitable_cell(ptr_bcch_surround_db, gas_id) && ( ptr_best_ncell_c2->C1 > 0 || rr_allow_negative_c1(ptr_bcch_surround_db->gsm.location_area_identification.plmn_id, gas_id) ) )
    {
      /*
      Test the ncell to see if it meets the criteria of being within better than the scell
      by hysteresis_adjust.
      */
      MSG_GERAN_MED_3_G("slow-ARFCN:%d nC2:%d sC2+hyst.adj:%d",
            CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN),
            ptr_best_ncell_c2->C2,
            local_gprs_reselection_list_ptr->serving_values.C2 + hysteresis_adjust);

      if ((local_gprs_reselection_list_ptr->serving_values.C2 + hysteresis_adjust) <= ptr_best_ncell_c2->C2)
      {

        ptr_bcch_surround_db->slow_snr_candidate_marker++;

        MSG_GERAN_HIGH_2_G("++SNR slow mark target ncell:%d %d ++",
              CONVERT_FROM_ARFCN_T(ptr_best_ncell_c2->BCCH_ARFCN),
              ptr_bcch_surround_db->slow_snr_candidate_marker);

        if (ptr_bcch_surround_db->slow_snr_candidate_marker >= VALID_NCELL_SNR_RESELECTION_CANDIDATE)
        {
          if (snr_reselection_target_list.no_of_entries < RR_GPRS_RESELECTION_STRUCTURE_SIZE)
          {
            ncell_better = TRUE;
            snr_reselection_target_list.surrounding_values[snr_reselection_target_list.no_of_entries] = *ptr_best_ncell_c2;
            snr_reselection_target_list.no_of_entries++;
            MSG_GERAN_HIGH_0_G("++Start Slow SNR reselection++");
          }
          else
          {
            MSG_GERAN_ERROR_0_G("snr_reselection_target_list full");
          }
        }
      }
      else
      {
        ptr_bcch_surround_db->slow_snr_candidate_marker = 0;
      }
    }

    /*
    Remove the tested entry from the reselection list
    */
    rr_gprs_reselection_list_delete_index(
      local_gprs_reselection_list_ptr,
      tested_cell_index
    );
  }

  if ( ncell_better )
  {
    /* Copy over the list of candidates */
    *local_gprs_reselection_list_ptr = snr_reselection_target_list;
  }

  return ncell_better;
}

/****************************************************************************
 *   Function name:  rr_gprs_is_nC2_greater_than_sC2
 *   -------------
 *
 *   Description: Simple test to see if the ncell is greater than the scell
 *   ------------ when no CRH is applied whether the cell is in the same RA/LA
 *                or the MS is in the READY state.
 *                The purpose is to see if there is a better cell to move to as
 *                this function is called when the SNR is bad on the scell.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns: TRUE = ncell passes both tests / FALSE = no ncell
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_is_nC2_greater_than_sC2(
  const gas_id_t gas_id
)
{
  uint8                          index;
  uint8                          hysteresis_adjust = MAX_SLOW_HYSTERESIS_ADJUST_dB;
  boolean                        ncell_better = FALSE;
  gprs_reselection_structure_T   different_la_ra_cell_list;
  bcch_ncell_info_t             *ptr_bcch_surround_db = NULL;
  gprs_scell_info_t             *scell_info = rr_gprs_get_scell_info(gas_id);
  gprs_reselection_structure_T  *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_resel_calc_data_t *rr_resel_calcs_data_ptr = rr_resel_calcs_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_calcs_data_ptr);

  RR_ZERO_STRUCTURE(different_la_ra_cell_list);

  /*
  Go through the ncell list starting from the best C2 down, and see if any cells have
  nc2 within 5dB of sc2 including all aspects of hysteresis.
  The complete list is compared as there may be cells in the bottom part of the list that
  have lower C2 but fulfill the criteria as they are in the same RA whilst others at the
  top are not.
  */
  /* Define the value of hysteresis_adjust to use */
  if( rr_gprs_is_snr_counter_zero(gas_id) )
  {
    hysteresis_adjust = MIN_SLOW_HYSTERESIS_ADJUST_dB;
  }
  else
  {
    hysteresis_adjust = MAX_SLOW_HYSTERESIS_ADJUST_dB;
  }

  MSG_GERAN_MED_2_G("++Slow SNR Reselection Test++ bsc=%d ha=%d",rr_resel_calcs_data_ptr->bad_snr_counter,hysteresis_adjust);

  /*
  Extract from the reselection list any cells that are not in the same RA/LA, and load them into the
  different_la_ra_cell_list.
  */
  for ( index=0; index<local_gprs_reselection_list_ptr->no_of_entries; index++)
  {
    boolean same_ra = TRUE;
    ptr_bcch_surround_db = rr_gprs_get_surround_ncell_sys_info(
      local_gprs_reselection_list_ptr->surrounding_values[index].BCCH_ARFCN,
      gas_id
    );

    if (ptr_bcch_surround_db != NULL)
    {
      /*Check to see if the cells are in the same RA, this can only be done if both the cells support gprs */
      if ( (ptr_bcch_surround_db->gprs.ra_colour != scell_info->bcch_sys_info.ra_colour) &&
            scell_info->gsm.gprs_indicator &&
            ptr_bcch_surround_db->gprs_supported
         )
      {
        same_ra = FALSE;
      }

      if ( (!same_ra ) ||
           (!rr_same_lai(&ptr_bcch_surround_db->gsm.location_area_identification,&scell_info->gsm.location_area_identification))
         )
      {
        different_la_ra_cell_list.surrounding_values[different_la_ra_cell_list.no_of_entries] = local_gprs_reselection_list_ptr->surrounding_values[index];
        different_la_ra_cell_list.no_of_entries++;
      }
    }
  }

  /********************************************************************************************/
  /* Extract from the reselection list any cells that are not in the same RA/LA that have now */
  /* been placed in the alternative different_la_ra_cell_list                                 */
  /********************************************************************************************/
  for (index = 0; index < different_la_ra_cell_list.no_of_entries; index++)
  {
    remove_entry_from_reselection_list(
      different_la_ra_cell_list.surrounding_values[index].BCCH_ARFCN,
      gas_id
    );
  }

  /* Check for cells only within the same RA/LA first to see if any meet the SNR reselection Criterion */
  ncell_better = rr_gprs_test_for_slow_snr_reselection(hysteresis_adjust, gas_id);


  /******************************************************************/
  /* If no cells were found then check the cells outside the LA/RA. */
  /* If there are any they will have been store away in the         */
  /* different_la_ra_cell_list                                      */
  /******************************************************************/
  if ( !ncell_better && different_la_ra_cell_list.no_of_entries)
  {

    local_gprs_reselection_list_ptr->no_of_entries = different_la_ra_cell_list.no_of_entries;

    for ( index=0; index<local_gprs_reselection_list_ptr->no_of_entries; index++)
    {
      local_gprs_reselection_list_ptr->surrounding_values[index] = different_la_ra_cell_list.surrounding_values[index];
    }

    MSG_GERAN_MED_0_G("++Slow SNR outside LA/RA Test++");

    ncell_better = rr_gprs_test_for_slow_snr_reselection(hysteresis_adjust, gas_id);

  }

  return ncell_better;
}

/****************************************************************************
 *   Function name:  rr_gprs_check_for_bad_scell_in_idle
 *   -------------
 *
 *   Description: This function is used to run the Qualcomm devised scell SNR
 *   ------------ algorithm, to determine if the scell should be dumped and
 *                another ncell targeted for reselection. This is only used in
 *                the
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  TRUE = bad scell / FALSE = scell is OK
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_check_for_bad_scell_in_idle(
  const gas_id_t gas_id
)
{
  boolean                       bad_scell_reselection = FALSE;
  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_resel_calc_data_t *rr_resel_calcs_data_ptr = rr_resel_calcs_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_calcs_data_ptr);

  if (rr_gprs_camped_on_ccch(gas_id) &&
      local_gprs_reselection_list_ptr->idle_serving_snr_is_bad &&
      rr_gprs_is_dsc_below_max_value(gas_id))
  {
    gprs_reselection_structure_T temp_ncell_scell_reselection_values;
    /*
    Store the data in the reselection structure as it may be destroyed during
    the call to test for nC2 near sC2
    */
    temp_ncell_scell_reselection_values = *local_gprs_reselection_list_ptr;

    /***************************************************************************/
    /* Decrement the snr counter to track how desperate the scell situation is.*/
    /* Once at zero the SNR reselection algorithms are run to find a cell to   */
    /* move to.                                                                */
    /***************************************************************************/
    if ( !rr_gprs_is_snr_counter_zero(gas_id) )
    {
      rr_resel_calcs_data_ptr->bad_snr_counter--;
      MSG_GERAN_MED_1_G("++SNR BSC = %d++",rr_resel_calcs_data_ptr->bad_snr_counter);
    }


    /***************************************************************************/
    /* FAST RESELECTION TEST                                                   */
    /* Run the test to see if there is a cell that meets the requirements for  */
    /* the fast reselection                                                    */
    /***************************************************************************/

    if (rr_gprs_is_nC2_near_sC2_and_nC2inst_greater_than_sC2inst(gas_id))
    {
      rr_gprs_init_bad_snr_counter(gas_id);
      bad_scell_reselection = TRUE;

      rr_clear_all_best_six_snr_markers(gas_id);
      rr_log_set_reselection_trigger(RR_FAST_SNR_RESELECTION_TRIGGER, gas_id);
    }
    else
    {
      /* restore the C2 values potentially stripped out of the reselection list by the
         previous fast reselection check function */
      *local_gprs_reselection_list_ptr = temp_ncell_scell_reselection_values;

    /***************************************************************************/
    /* SLOW RESELECTION TEST                                                   */
    /* Run the test to see if there is a cell that meets the requirements for  */
    /* the slow reselection                                                    */
    /***************************************************************************/

      if ( rr_gprs_is_nC2_greater_than_sC2(gas_id) )
      {
        rr_gprs_init_bad_snr_counter(gas_id);
        bad_scell_reselection = TRUE;

        rr_clear_all_best_six_snr_markers(gas_id);
        rr_log_set_reselection_trigger(RR_SLOW_SNR_RESELECTION_TRIGGER, gas_id);
      }
      else
      {
        /* restore the C2 values potentially stripped out of the reselection list by the
           previous slow reselection check function */
        *local_gprs_reselection_list_ptr = temp_ncell_scell_reselection_values;
      }
    }
  }
  else
  {
    rr_clear_all_best_six_snr_markers(gas_id);
  }

  return bad_scell_reselection;
}




/****************************************************************************
 *   Function name:  rr_gprs_check_scell_path_loss_GPRS
 *   --------------
 *
 *   Description: This function is used to see of the path loss of the scell
 *   ------------ has become too high using the algorithm in GSM05.08 10.1.3
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  TRUE = Path Loss too High / FALSE = Path loss is OK
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_check_scell_path_loss_GPRS(
  const gas_id_t gas_id
)
{
  boolean                       Path_Loss_Too_High = FALSE;
  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  /********************************************************/
  /* Check the C1 of the serving cell. If it is < 0 in    */
  /* the last serving cell entry in the reselection       */
  /* structure list then the path loss has become too high*/
  /********************************************************/

  /* Make sure that there is a valid entry in the structure*/
  if (local_gprs_reselection_list_ptr->serving_values_valid == TRUE &&
    (local_gprs_reselection_list_ptr->serving_values.C1 < 0 && !rr_allow_negative_c1(rr_gprs_get_scell_plmn_id(gas_id), gas_id)))
  {
    /*The path loss is too high*/
    Path_Loss_Too_High = TRUE;
  }

  if (Path_Loss_Too_High == TRUE)
  {
    MSG_GERAN_MED_0_G("Path loss on scell too high, force reselection");
  }
  else
  {
    MSG_GERAN_MED_0_G("Path loss on scell is OK");
  }

return Path_Loss_Too_High;
}


/****************************************************************************
 *   Function name:  rr_gprs_check_for_path_loss
 *   --------------------------
 *
 *   Description: This function is used to see of the path loss of the scell
 *   ------------ has become too high. If so the best target ncell is chosen
 *                to use in the reselection.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  TRUE = Path Loss too High / FALSE = Path loss is OK
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_check_scell_path_loss_GSM(
  const gas_id_t gas_id
)
{
  boolean                        path_loss_too_high = FALSE;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_reselection_structure_T  *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  if ( local_gprs_reselection_list_ptr->serving_values.C1 < 0 && !rr_allow_negative_c1(rr_gprs_get_scell_plmn_id(gas_id), gas_id))
  {
    if (rr_timer_get_state(RR_SERV_FIVE_SEC_C1_TIMER, gas_id) == RR_TIMER_STOPPED)
    {
      /*Serving C1 below 0, must start 5s timer*/
      scell_info_ptr->gsm.reselection_info.five_second_timer_id = RR_SERV_FIVE_SEC_C1_TIMER;
      (void)rr_timer_start(RR_SERV_FIVE_SEC_C1_TIMER, RR_FIVE_SEC_TIMEOUT, gas_id);
      MSG_GERAN_MED_0_G("Serv 5 sec timer started");
    }
    else if (rr_timer_get_state(RR_SERV_FIVE_SEC_C1_TIMER, gas_id) == RR_TIMER_EXPIRED)
    {
      MSG_GERAN_HIGH_0_G("Serving cell C1 too low, force reselection");
      path_loss_too_high = TRUE;
    }
  }
  else
  {
    if (rr_timer_get_state(RR_SERV_FIVE_SEC_C1_TIMER, gas_id) != RR_TIMER_STOPPED)
    {
      /*
      If timer is running or has expired but the C1 is OK now then
      Cleanup five second timer since serving is ok now
      */
      scell_info_ptr->gsm.reselection_info.five_second_timer_id = RR_SERV_FIVE_SEC_C1_TIMER;
      (void)rr_timer_cancel(RR_SERV_FIVE_SEC_C1_TIMER, gas_id);
      MSG_GERAN_MED_0_G("Serv 5 sec timer stopped");
    }
  }

  return path_loss_too_high;
}

/****************************************************************************
 *   Function name:  rr_gprs_store_C1_C2_instant_serving_measurement
 *   --------------------------
 *
 *   Description: This function is used to store and calculate C1, C2
 *   ------------ values for the serving cell. These are based upon measurement
 *                reports from L1.
 *
 *   Parameters: RXLEV
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
static void rr_gprs_store_C1_C2_instant_serving_measurement(
  byte RXLEV_instant,
  const gas_id_t gas_id
)
{
  int C1;
  int C2;
  int PENALTY_TIME;
  int CELL_RESELECT_OFFSET_DB;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_reselection_structure_T  *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

/***************************************************/
/* Calculate the C1 parameter for the serving cell */
/***************************************************/
  C1 = rr_get_C1(
    scell_info_ptr->gsm.BCCH_ARFCN,RXLEV_instant,
    scell_info_ptr->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
    scell_info_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
    scell_info_ptr->gsm.cell_selection_parameters.POWER_OFFSET,
    gas_id
  );

/***************************************************/
/* Calculate the C2 parameter for the serving cell */
/***************************************************/
  if (scell_info_ptr->gsm.cell_selection_parameters.CELL_RESELECT_PARAM_IND == 1)
  {
    /*Use this rule if cell selection parameters are broadcast for the scell*/
    PENALTY_TIME = scell_info_ptr->gsm.cell_selection_parameters.PENALTY_TIME;
    CELL_RESELECT_OFFSET_DB = scell_info_ptr->gsm.cell_selection_parameters.CELL_RESELECT_OFFSET * 2;

    /* No penalty timer associated with serving cell GSM05.08 - 6.4*/
    if (PENALTY_TIME != 31)
    {
      C2 = C1 + CELL_RESELECT_OFFSET_DB;
    }
    else
    {
      C2 = C1 - CELL_RESELECT_OFFSET_DB;
    }
  }
  else
  {
    /*Use this rule if no cell selection parameters are broadcast for the scell*/
    C2 = C1;
  }

/***************************************************************/
/* Fill the reselection structure with the serving cell values */
/***************************************************************/
  local_gprs_reselection_list_ptr->instant_serving_values.BCCH_ARFCN = scell_info_ptr->gsm.BCCH_ARFCN;
  local_gprs_reselection_list_ptr->instant_serving_values.C1_instant = C1;
  local_gprs_reselection_list_ptr->instant_serving_values.C2_instant = C2;
  local_gprs_reselection_list_ptr->instant_serving_values.RXLEV_instant = RXLEV_instant;

#ifdef FEATURE_ENABLE_GPRS_RESELECTION_MESSAGING
  MSG_GERAN_MED_3_G("ScellInst: ARFCN=%d,lev=%d,C2=%d",
          CONVERT_FROM_ARFCN_T(scell_info_ptr->gsm.BCCH_ARFCN),RXLEV_instant,C2);
#endif
}


/****************************************************************************
 *   Function name:  rr_gprs_calculate_C31
 *   -------------
 *
 *   Description: This function calculates the C31 parameter for ncells and scells
 *   ------------ It should not be called if the scell does not have any HCS struct.
 *                In such a case C31=0.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  int C31
 *   --------
 *
 ****************************************************************************/
static int rr_gprs_calculate_C31(
  boolean          serving_cell_type,
  int              GPRS_TEMPORARY_OFFSET,
  int              HCS_THR,
  int              PRIORITY_CLASSs,
  int              PRIORITY_CLASSn,
  int              RLA_P,
  rr_timer_state_T penalty_timer_state /*only needed for ncells*/
)
{
  int C31;
  int HCS_THR_dB;
  int Ln;
  int GPRS_TEMPORARY_OFFSET_dB;

  Ln = (PRIORITY_CLASSs == PRIORITY_CLASSn)?0:1;

  /*********************************************************/
  /*Convert the HCS_THR value into a DBM value that can be */
  /*applied to the equation to calculate C31               */
  /*********************************************************/
  HCS_THR_dB = (-110 + (2*HCS_THR));

  /*********************************************************/
  /*Calculate the value of TEMPORARY_OFFSET that should be */
  /*applied to the equation to calculate C31               */
  /*********************************************************/
  if (serving_cell_type == FALSE)
  {
    /*The calculation is for an ncell*/
    /* if the penalty timer for the current ncell is running */
    if (penalty_timer_state == RR_TIMER_RUNNING)
    {
      if (GPRS_TEMPORARY_OFFSET == 7)
      {
        /*As this ,means that there is an infinite temporary offset just set
        C31 to a suitably large negative number*/
        C31 = -100;
      }
      else
      {
        /*Convert the GPRS_TEMPORARY_OFFSET to a corresponding dBm value*/
        GPRS_TEMPORARY_OFFSET_dB = (GPRS_TEMPORARY_OFFSET*10);
        C31 = CONVERT_TO_RXLEVdB(RLA_P ) - HCS_THR_dB - GPRS_TEMPORARY_OFFSET_dB * Ln;
      }
    }
    else /* otherwise, the penalty timer is not running */
    {
      /*the temporary offset no longer applies*/
      C31 = CONVERT_TO_RXLEVdB(RLA_P ) - HCS_THR_dB;
    }
  }
  else
  {
    /*Calculate C31 for the serving cell*/
    C31 = CONVERT_TO_RXLEVdB(RLA_P) - HCS_THR_dB;
  }

  return C31;
}


/****************************************************************************
 *   Function name:  rr_gprs_store_C31_C32_serving_measurement
 *   --------------------------
 *
 *   Description: This function is used to store and calculate C31, C32
 *   ------------ values for the serving cell. These are based upon measurement
 *                reports from L1.
 *
 *   Parameters: RXLEV, RXQUAL
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
static void rr_gprs_store_C31_C32_serving_measurement(
  byte RXLEV_average,
  byte RXQUAL_value,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t             *scell_info = rr_gprs_get_scell_info(gas_id);
  rr_measured_signal_data_t     *rr_measured_signal_data_ptr = rr_get_measured_signal_data_ptr(gas_id);
  gprs_reselection_structure_T  *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
  int                C1;
  int                C31;
  int                C32;

  /* Load the global variable with the RX_LEV */
  rr_measured_signal_data_ptr->current_signal_strength = RXLEV_average;
  rr_measured_signal_data_ptr->current_signal_quality  = RXQUAL_value;

  /***************************************************/
  /* Calculate the C1 parameter for the serving cell */
  /*                                                 */
  /***************************************************/
  /*Calculate C1 for a scell that has recieved PCCO or PMO*/
  C1 = rr_get_C1(
    scell_info->gsm.BCCH_ARFCN,
    RXLEV_average,
    scell_info->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
    scell_info->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
    0, /*POWER_OFFSET is not used under GPRS*/
    gas_id
  );

  /********************************************************************/
  /* If HCS parameters have been received in a PMO then calculate C31 */
  /********************************************************************/
  if (scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params_present)
  {
    if (scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params.hcs_thr == INVALID_HCS_THRESHOLD)
    {
      /* we are using a dummy set of HCS parameters for the serving cell, so C31 is 0 */
      C31 = 0;
    }
    else
    {
      C31 = rr_gprs_calculate_C31(
        TRUE,
        0,
        scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params.hcs_thr,
        0,
        0,
        RXLEV_average,
        RR_TIMER_STOPPED
      );
    }
  }
  else
  {
    /*we are on BCCH using GPRS Parameter conversion, C31 should be set = 0*/
    C31 = 0;
  }


/****************************************************/
/* Calculate the C32 parameter for the serving cell */
/* This is equal to C1 for the scell                */
/****************************************************/

  C32 = C1;

/***************************************************************/
/* Fill the reselection structure with the serving cell values */
/***************************************************************/
  local_gprs_reselection_list_ptr->serving_values.BCCH_ARFCN = scell_info->gsm.BCCH_ARFCN;
  local_gprs_reselection_list_ptr->serving_values_valid = TRUE;
  local_gprs_reselection_list_ptr->serving_values.C1 = C1;
  local_gprs_reselection_list_ptr->serving_values.C31 = C31;
  local_gprs_reselection_list_ptr->serving_values.C32 = C32;
  local_gprs_reselection_list_ptr->serving_values.RXLEV_average = RXLEV_average;

  {
    rr_mac_status_t mac_status = rr_macinfo_get_mac_status(gas_id);
    eng_mode_scell_measurements_t eng_mode_scell_meas = {0};

    eng_mode_scell_meas.C1 = C1;
    eng_mode_scell_meas.C2 = 0;
    eng_mode_scell_meas.rx_lev = RXLEV_average;
    eng_mode_scell_meas.rx_qual_sub = (uint16)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
    eng_mode_scell_meas.rx_qual_full = (uint16)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
    eng_mode_scell_meas.rx_lev_sub = (uint16)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
    eng_mode_scell_meas.rx_lev_full = (uint16)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
    eng_mode_scell_meas.C31 = C31;
    eng_mode_scell_meas.C32 = C32;

    if (mac_status == MAC_IDLE)
    {
      eng_mode_scell_meas.channel_type = ENG_MODE_RX_LEVEL_BCCH;
    }
    else
    {
      eng_mode_scell_meas.channel_type = ENG_MODE_RX_LEVEL_TCH_PDTCH;
    }

    geran_eng_mode_data_write(
      ENG_MODE_SCELL_MEASUREMENTS,    // geran_eng_mode_info_type
      &eng_mode_scell_meas,           // param_copy
      TRUE,                           // valid_flag
      gas_id                          // gas_id
    );
  }

#ifdef FEATURE_ENABLE_GPRS_RESELECTION_MESSAGING
  MSG_GERAN_MED_3_G("Scell:C1=%d,C31=%d,C32=%d",C1,C31,C32);
#endif
}

/****************************************************************************
 *   Function name:  rr_gprs_store_C1_C2_serving_measurement
 *   --------------------------
 *
 *   Description: This function is used to store and calculate C1, C2
 *   ------------ values for the serving cell. These are based upon measurement
 *                reports from L1.
 *
 *   Parameters: RXLEV, RXQUAL
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
static void rr_gprs_store_C1_C2_serving_measurement(
  byte RXLEV_average,
  byte RXQUAL_value,
  const gas_id_t gas_id
)
{
  int C1;
  int C2;
  int PENALTY_TIME;
  int CELL_RESELECT_OFFSET_DB;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_measured_signal_data_t     *rr_measured_signal_data_ptr = rr_get_measured_signal_data_ptr(gas_id);
  gprs_reselection_structure_T  *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);


  /* Store the RXLEV and RXQUAL */
  rr_measured_signal_data_ptr->current_signal_strength = RXLEV_average;
  rr_measured_signal_data_ptr->current_signal_quality  = RXQUAL_value;

/***************************************************/
/* Calculate the C1 parameter for the serving cell */
/***************************************************/
  C1 = rr_get_C1(
    scell_info_ptr->gsm.BCCH_ARFCN,RXLEV_average,
    scell_info_ptr->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
    scell_info_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
    scell_info_ptr->gsm.cell_selection_parameters.POWER_OFFSET,
    gas_id
  );

/***************************************************/
/* Calculate the C2 parameter for the serving cell */
/***************************************************/
  if (scell_info_ptr->gsm.cell_selection_parameters.CELL_RESELECT_PARAM_IND == 1)
  {
    /*Use this rule if cell selection parameters are broadcast for the scell*/
    PENALTY_TIME = scell_info_ptr->gsm.cell_selection_parameters.PENALTY_TIME;
    CELL_RESELECT_OFFSET_DB = scell_info_ptr->gsm.cell_selection_parameters.CELL_RESELECT_OFFSET * 2;

    /* No penalty timer associated with serving cell GSM05.08 - 6.4*/
    if (PENALTY_TIME != 31)
    {
      C2 = C1 + CELL_RESELECT_OFFSET_DB;
    }
    else
    {
      C2 = C1 - CELL_RESELECT_OFFSET_DB;
    }
  }
  else
  {
    /*Use this rule if no cell selection parameters are broadcast for the scell*/
    C2 = C1;
  }

/***************************************************************/
/* Fill the reselection structure with the serving cell values */
/***************************************************************/
  local_gprs_reselection_list_ptr->serving_values.BCCH_ARFCN = scell_info_ptr->gsm.BCCH_ARFCN;
  local_gprs_reselection_list_ptr->serving_values_valid = TRUE;

  local_gprs_reselection_list_ptr->serving_values.C1 = C1;
  local_gprs_reselection_list_ptr->serving_values.C2 = C2;
  local_gprs_reselection_list_ptr->serving_values.RXLEV_average = RXLEV_average;

  {
    rr_mac_status_t mac_status = rr_macinfo_get_mac_status(gas_id);
    eng_mode_scell_measurements_t eng_mode_scell_meas = {0};

    eng_mode_scell_meas.C1 = C1;
    eng_mode_scell_meas.C2 = C2;
    eng_mode_scell_meas.rx_lev = RXLEV_average;
    eng_mode_scell_meas.rx_qual_sub = (uint16)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
    eng_mode_scell_meas.rx_qual_full = (uint16)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
    eng_mode_scell_meas.rx_lev_sub = (uint16)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
    eng_mode_scell_meas.rx_lev_full = (uint16)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
    eng_mode_scell_meas.C31 = 0;
    eng_mode_scell_meas.C32 = 0;

    if (mac_status == MAC_IDLE)
    {
      eng_mode_scell_meas.channel_type = ENG_MODE_RX_LEVEL_BCCH;
    }
    else
    {
      eng_mode_scell_meas.channel_type = ENG_MODE_RX_LEVEL_TCH_PDTCH;
    }

    geran_eng_mode_data_write(
      ENG_MODE_SCELL_MEASUREMENTS,    // geran_eng_mode_info_type
      &eng_mode_scell_meas,           // param_copy
      TRUE,                           // valid_flag
      gas_id                          // gas_id
    );
  }

#ifdef FEATURE_ENABLE_GPRS_RESELECTION_MESSAGING
  MSG_GERAN_MED_3_G("Scell: ARFCN=%d,lev=%d,C2=%d",
          CONVERT_FROM_ARFCN_T(scell_info_ptr->gsm.BCCH_ARFCN),RXLEV_average,C2);
#endif
} /* rr_gprs_store_serving_measurement */

/****************************************************************************
 *   Function name:  rr_gprs_calculate_Cvalues_for_ncell
 *   --------------------------
 *
 *   Description: This function is used to store and calculate C1, C2,
 *                C31, C32 values for the ncell. These are based upon measurement
 *                reports from L1. The C31 and C32 values are calculated if there
 *                has been a PMO or PCCO recieved that contained cells with GPRS
 *                paramters.
 *
 *   Parameters: RXLEV
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
static void rr_gprs_calculate_Cvalues_for_ncell(
  gprs_reselection_structure_T * reselection_structure_ptr,
  surrounding_channel_info_T *   ptr_meas_report,
  boolean                        strongest_RLA_P,
  const gas_id_t                 gas_id
)
{
  boolean                         valid_data_flag = FALSE;
  bcch_ncell_info_t *             ptr_bcch_surround_db = NULL;
  pmo_ncell_info_t *              ptr_pbcch_ncell_info = NULL;
  int                             C1;
  int                             C2;
  int                             C31;
  int                             C32;
  BSIC_T                          BSIC = {0};
  ARFCN_T                         BCCH_ARFCN = ptr_meas_report->BCCH_ARFCN;
  byte                            RXLEV_average = ptr_meas_report->RXLEV_average;
  uint8                           entry_index = reselection_structure_ptr->no_of_entries;
  gprs_scell_info_t *             scell_info = rr_gprs_get_scell_info(gas_id);
  rr_resel_candidate_t *          resel_candidate;

  resel_candidate = rr_resel_best_six_find(BCCH_ARFCN, NULL, gas_id);

  if (resel_candidate == NULL)
  {
    MSG_GERAN_MED_2_G("Unable to locate ARFCN %d (band %d) in best six", BCCH_ARFCN.num, BCCH_ARFCN.band);
    return;
  }

  /*******************************************************************/
  /* Compute the BSIC for the cell data (if information is available)*/
  /*******************************************************************/
  if (ptr_meas_report->valid_info == TRUE)
  {
    BSIC.PLMN_colour_code = DECODE_NCC(ptr_meas_report);
    BSIC.BS_colour_code = DECODE_BCC(ptr_meas_report);
  }

  /****************************************************************/
  /* calculate the C1 and C2 parameters for the surrounding cells */
  /****************************************************************/
  /* compute C1 for the current surrounding cell */
  /* Make sure that there is valid data for the surround cell */
  ptr_bcch_surround_db = rr_gprs_get_surround_ncell_sys_info(BCCH_ARFCN, gas_id);

  if (ptr_bcch_surround_db != NULL)
  {
    valid_data_flag = ptr_bcch_surround_db->gsm.valid_data_flag;
  }

  /* can only compute C1/C2 values for cells that contain valid SI */
  if (ptr_bcch_surround_db != NULL && valid_data_flag == TRUE)
  {
    if (rr_gprs_parameters_available(gas_id) == FALSE)
    {
      /*****************************************/
      /* We are on BCCH and normal reselection */
      /* applies without GPRS C31 C32          */
      /* Calculate C1, C2 using GSM 05.08 6.4  */
      /*****************************************/

      C1 = rr_get_C1(
        BCCH_ARFCN,
        RXLEV_average,
        ptr_bcch_surround_db->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
        ptr_bcch_surround_db->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
        ptr_bcch_surround_db->gsm.cell_selection_parameters.POWER_OFFSET,
        gas_id
      );

      C2 = rr_gprs_compute_surrounding_C2(
        C1,
        resel_candidate,
        gas_id
      );

      reselection_structure_ptr->surrounding_values[entry_index].BCCH_ARFCN = BCCH_ARFCN;
      reselection_structure_ptr->surrounding_values[entry_index].C1 = C1;
      reselection_structure_ptr->surrounding_values[entry_index].C2 = C2;
      reselection_structure_ptr->surrounding_values[entry_index].C31 = 0;
      reselection_structure_ptr->surrounding_values[entry_index].C32 = 0;
      reselection_structure_ptr->surrounding_values[entry_index].RXLEV_average = RXLEV_average;
      reselection_structure_ptr->no_of_entries++;

      /**************************************************/
      /* Instantaneous values are calculated and stored */
      /**************************************************/

      C1 = rr_get_C1(
        BCCH_ARFCN,
        ptr_meas_report->RXLEV_instant,
        ptr_bcch_surround_db->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
        ptr_bcch_surround_db->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
        ptr_bcch_surround_db->gsm.cell_selection_parameters.POWER_OFFSET,
        gas_id
      );

      C2 = rr_gprs_compute_surrounding_C2(
        C1,
        resel_candidate,
        gas_id
      );

      reselection_structure_ptr->instant_surrounding_values[entry_index].BCCH_ARFCN = BCCH_ARFCN;
      reselection_structure_ptr->instant_surrounding_values[entry_index].C1_instant = C1;
      reselection_structure_ptr->instant_surrounding_values[entry_index].C2_instant = C2;
      reselection_structure_ptr->instant_surrounding_values[entry_index].C31_instant = 0;
      reselection_structure_ptr->instant_surrounding_values[entry_index].C32_instant = 0;
      reselection_structure_ptr->instant_surrounding_values[entry_index].RXLEV_instant = ptr_meas_report->RXLEV_instant;

#ifdef FEATURE_ENABLE_GPRS_RESELECTION_MESSAGING
      MSG_GERAN_MED_3_G("ncell ARFCN=%d lev=%d, C2=%d",
        (int)CONVERT_FROM_ARFCN_T(BCCH_ARFCN),
        RXLEV_average,
        reselection_structure_ptr->surrounding_values[entry_index].C2);
#endif /* FEATURE_ENABLE_GPRS_RESELECTION_MESSAGING */
    }
    else
    {
      /*GPRS-reselection parameters have been recieved via a PCCO or PMO*/
      /********************************************/
      /* Convert the BCCH parameters as described */
      /* see table 3a GSM05.08                    */
      /* and calculate C1, C31 and C32            */
      /********************************************/
      int GPRS_TEMPORARY_OFFSET = 0;
      int GPRS_RESELECT_OFFSETn = 16; /* for array index zero */
      int PRIORITY_CLASSs = 0;
      int PRIORITY_CLASSn = 0;

      /****************************************************************/
      /* If the cell selection parameters were present then load them */
      /* otherwise they will stay set as 0 and C32 will in effect be  */
      /* the same as C1.                                              */
      /****************************************************************/
      if (ptr_bcch_surround_db->gsm.cell_selection_parameters.CELL_RESELECT_PARAM_IND)
      {
        uint8 ro_index;
        GPRS_TEMPORARY_OFFSET = ptr_bcch_surround_db->gsm.cell_selection_parameters.TEMPORARY_OFFSET;
        GPRS_RESELECT_OFFSETn = ptr_bcch_surround_db->gsm.cell_selection_parameters.CELL_RESELECT_OFFSET * 2 -
                                scell_info->gsm.cell_selection_parameters.CELL_RESELECT_OFFSET * 2;

        /* convert the reselection offset to a index value to represent the gprs_reselect_offset */
        for (ro_index = 0; ro_index < 32; ++ro_index)
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
      C1 = rr_get_C1(
        BCCH_ARFCN,
        RXLEV_average,
        ptr_bcch_surround_db->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN,
        ptr_bcch_surround_db->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH,
        0, /*Not used for GPRS cell reselection 5.08 10.1.2*/
        gas_id
      );

      /********************************************/
      /* On BCCH the C31 parameter is set to zero */
      /* see table 3a GSM05.08                  */
      /********************************************/
      C31 = 0;

      /*********************************************/
      /* Calculate the C32 parameter for the ncell */
      /*********************************************/
      C32 = rr_gprs_surround_calculate_C32(
        C1,
        GPRS_TEMPORARY_OFFSET,
        GPRS_RESELECT_OFFSETn,
        PRIORITY_CLASSs,
        PRIORITY_CLASSn,
        resel_candidate->penalty_timer_id,
        ptr_bcch_surround_db->gsm.cell_selection_parameters.PENALTY_TIME,
        strongest_RLA_P,
        gas_id
      );

      /* Now strore the values in the reselection structure */
      reselection_structure_ptr->surrounding_values[entry_index].BCCH_ARFCN = BCCH_ARFCN;
      reselection_structure_ptr->surrounding_values[entry_index].C1 = C1;
      reselection_structure_ptr->surrounding_values[entry_index].C2 = 0;
      reselection_structure_ptr->surrounding_values[entry_index].C31 = C31;
      reselection_structure_ptr->surrounding_values[entry_index].C32 = C32;
      reselection_structure_ptr->surrounding_values[entry_index].RXLEV_average = RXLEV_average;
      reselection_structure_ptr->no_of_entries++;
#ifdef FEATURE_ENABLE_GPRS_RESELECTION_MESSAGING
      MSG_GERAN_MED_3_G("ncell=%d, C31=%d, C32=%d",(int)CONVERT_FROM_ARFCN_T(BCCH_ARFCN),C31,C32);
#endif /* FEATURE_ENABLE_GPRS_RESELECTION_MESSAGING */
    }
  }
  else if (rr_gprs_parameters_available(gas_id) == TRUE)
  {
    /*************************************************/
    /* We are on BCCH and no entries have been found */
    /* in the GSM system information database for the*/
    /* ARFCN, but a PMO must have been recieved as   */
    /* GPRS parameters are available. Check if the   */
    /* pbcch ncell database has a matching entry for */
    /* this ARFCN                                    */
    /*************************************************/
    if ((ptr_pbcch_ncell_info = rr_gprs_get_pmo_pcco_ncell_sys_info(BCCH_ARFCN, &BSIC, gas_id)) != NULL)
    {
      /************************************************************************/
      /* There is an entry for this ARFCN and the BSIC has been checked for a */
      /* match so calculate the C values                                      */
      /************************************************************************/
      /********************************************/
      /* Calculate the C1 parameter for the ncell */
      /********************************************/
      C1 = rr_get_C1(
        BCCH_ARFCN,
        RXLEV_average,
        ptr_pbcch_ncell_info->gprs_cell_selection_params.gprs_rxlev_access_min,
        ptr_pbcch_ncell_info->gprs_cell_selection_params.gprs_ms_txpwr_max_cch,
        0, /*Not used for GPRS cell reselection 5.08 10.1.2*/
        gas_id
      );

      /****************************************************************/
      /* On BCCH the value of the C31 parameter depends upon whether */
      /* the HCS_struct is present on the scell. If not present then  */
      /* the HCS threshold is set to infinity, ensuring that all      */
      /* ncells have a C31 < 0                                        */
      /****************************************************************/
      if (scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params_present)
      {
        C31 = rr_gprs_calculate_C31(
          FALSE,
          ptr_pbcch_ncell_info->gprs_cell_selection_params.gprs_temporary_offset,
          ptr_pbcch_ncell_info->gprs_cell_selection_params.hcs_params.hcs_thr,
          scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params.priority_class,
          ptr_pbcch_ncell_info->gprs_cell_selection_params.hcs_params.priority_class,
          RXLEV_average,
          rr_timer_get_state(resel_candidate->penalty_timer_id, gas_id)
        );
      }
      else
      {
        C31 = -100;
      }

      /*********************************************/
      /* Calculate the C32 parameter for the ncell */
      /*********************************************/
      C32 = rr_gprs_surround_calculate_C32(
        C1,
        ptr_pbcch_ncell_info->gprs_cell_selection_params.gprs_temporary_offset,
        ptr_pbcch_ncell_info->gprs_cell_selection_params.gprs_reselect_offset,
        scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params.priority_class,
        ptr_pbcch_ncell_info->gprs_cell_selection_params.hcs_params.priority_class,
        resel_candidate->penalty_timer_id,
        ptr_pbcch_ncell_info->gprs_cell_selection_params.gprs_penalty_time,
        strongest_RLA_P,
        gas_id
      );

      /* Now store the calculated values in the reselection structure */
      reselection_structure_ptr->surrounding_values[entry_index].BCCH_ARFCN = BCCH_ARFCN;
      reselection_structure_ptr->surrounding_values[entry_index].C1 = C1;
      reselection_structure_ptr->surrounding_values[entry_index].C2 = 0;
      reselection_structure_ptr->surrounding_values[entry_index].C31 = C31;
      reselection_structure_ptr->surrounding_values[entry_index].C32 = C32;
      reselection_structure_ptr->surrounding_values[entry_index].RXLEV_average = RXLEV_average;
      reselection_structure_ptr->no_of_entries++;
#ifdef FEATURE_ENABLE_GPRS_RESELECTION_MESSAGING
      MSG_GERAN_MED_3_G("ncell=%d, C31=%d, C32=%d",(int) CONVERT_FROM_ARFCN_T(BCCH_ARFCN),C31,C32);
#endif /* FEATURE_ENABLE_GPRS_RESELECTION_MESSAGING */
    }
    else
    {
      MSG_GERAN_MED_1_G("BSIC heteroclitical for %d",(int) CONVERT_FROM_ARFCN_T(BCCH_ARFCN));
    }
  }
  else
  {
    MSG_GERAN_MED_1_G("No SI for ncell ARFCN=%d",(int) CONVERT_FROM_ARFCN_T(BCCH_ARFCN));
  }

  return;
} /* rr_gprs_calculate_Cvalues_for_ncell */

/****************************************************************************
 *   Function name:  rr_gprs_store_surrounding_measurement_MFRM52
 *   -------------
 *
 *   Description: This function is used to store the recieved measurements from
 *   -----------  L1 for the surrounding ncells. The C1 and C31, C32 values for the
 *                measurements are calculated. This is always used whilst camped
 *                on PBCCH/PCCCH.
 *
 *   Parameters: best_six_info_T - a list of the 6 stongest ncells
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
static void rr_gprs_store_surrounding_measurement_MFRM52(
  best_six_info_T * p_best_six_info,
  const gas_id_t gas_id
)
{
  surrounding_channel_info_T   *meas_chan_info_ptr;
  BSIC_T                        recieved_bsic;
  int                           cell_index;
  ARFCN_T                       BCCH_ARFCN;
  boolean                       strongest_RLA_P = TRUE;
  boolean                       bsic_changed = FALSE;
  boolean                       still_best_six = FALSE;
  boolean                       channel_present = FALSE;
  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
  best_six_ncells_t            *local_best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  struct {
    uint8 no_of_cells_to_remove;
    struct {
      ARFCN_T arfcn;
      BSIC_T bsic;
    } details[MAX_NUMBER_OF_BEST_CELLS];
  } cells_to_remove={0};

  /* clean out the current entries in the list*/
  local_gprs_reselection_list_ptr->no_of_entries = 0;

  /*Clean out the list of sync cells ready for the latest entries*/
  rr_gprs_clear_sync_cell_list(gas_id);

  /*#############################################################################*/
  /* RR has just received a new measurement report of the best six surrounding   */
  /* cells (based on the BA list sent to L1). Check the best six database to     */
  /* make sure that the entries are still coming in in the latest measurment     */
  /* report from L1                                                              */
  /*#############################################################################*/

  /* foreach cell in the ncell bcch surrounding info database */
  for (cell_index = 0; cell_index < local_best_six_ncells_ptr->no_of_entries; cell_index++)
  {

    /******************************************************/
    /* Determine if the surrounding cell is still in the  */
    /* best six measurement report (also determine if the */
    /* cell is present, but the BSIC has changed)         */
    /******************************************************/
    still_best_six = rr_gprs_surrounding_cell_in_best_six(
      local_best_six_ncells_ptr->ncell_entry[cell_index].ncell_data,
      p_best_six_info,
      &bsic_changed,
      NULL,
      gas_id
    );

    /* check to see if the BSIC has changed */
    if (bsic_changed == TRUE)
    {
      /********************************************************************/
      /* BSIC has changed !!                                              */
      /* We need to update system information for this surrounding cell   */
      /* since it really represents a completely new cell. Remove its     */
      /* entry and the next measurment report will treat it as a brand new*/
      /* cell                                                             */
      /********************************************************************/
      MSG_GERAN_MED_1_G("BSIC changed for ncell %d",CONVERT_FROM_ARFCN_T(local_best_six_ncells_ptr->ncell_entry[cell_index].ncell_data->arfcn));
      cells_to_remove.details[cells_to_remove.no_of_cells_to_remove].arfcn = local_best_six_ncells_ptr->ncell_entry[cell_index].ncell_data->arfcn;
      cells_to_remove.details[cells_to_remove.no_of_cells_to_remove].bsic = local_best_six_ncells_ptr->ncell_entry[cell_index].ncell_data->bsic;
      cells_to_remove.no_of_cells_to_remove++;
    }
    else if (still_best_six == FALSE)
    {
      /*************************************************************************/
      /* The current channel in best 6 database is not part of the best six    */
      /* cells in the measurement report. Take it out of the list and stop any */
      /* penalty timers for this entry                                         */
      /*************************************************************************/
      cells_to_remove.details[cells_to_remove.no_of_cells_to_remove].arfcn = local_best_six_ncells_ptr->ncell_entry[cell_index].ncell_data->arfcn;
      cells_to_remove.details[cells_to_remove.no_of_cells_to_remove].bsic = local_best_six_ncells_ptr->ncell_entry[cell_index].ncell_data->bsic;
      cells_to_remove.no_of_cells_to_remove++;
    }
    else /* still_best_six == TRUE */
    {
      /*************************************************************************/
      /* The cell is still ok to stay in the best six database. Leave things   */
      /* as they are.                                                          */
      /*************************************************************************/
      ;
    }
    /* otherwise, the cell is still part of the best six... keep it arround */
  } /* for each cell in the surrounding SI DB */


  /* Only now can the identified cells be deleted */
  for (cell_index = 0; cell_index < cells_to_remove.no_of_cells_to_remove; cell_index++)
  {
    rr_gprs_remove_from_best_six(
      cells_to_remove.details[cell_index].arfcn,
      cells_to_remove.details[cell_index].bsic,
      gas_id
    );
  }

  /************************************************************************/
  /* At this point, all the cells in the best six database that           */
  /* were not in the measurement report have been deleted. Now it is time */
  /* to add new cells in the measurement report that are not already in   */
  /* the database (this includes any cells for which BSIC has changed)    */
  /************************************************************************/

  /* foreach cell in the measurement report */
  for (cell_index = 0; cell_index < p_best_six_info->no_of_entries; cell_index++)
  {
    surrounding_channel_info_T *cell_meas_chan_info_ptr = &(p_best_six_info->surrounding_channel_info[cell_index]);

    /********************************************************************************/
    /* Try to match the ARFCN in the measurement report with a ncell database entry */
    /* this function will look in the GSM surround database and also the pbcch      */
    /* surround cell database. If the channel is present in the pbcch necll database*/
    /* but has no penalty timer allocated, then this function recognises this as a  */
    /* new cell on the best 6 list so allocates a penalty timer to it and starts it */
    /********************************************************************************/
    channel_present = rr_gprs_is_ncell_in_best_six(
      cell_meas_chan_info_ptr,
      gas_id
    );

    /* if the measurement report channel is not in the surround DB then an entry must be added */
    if (channel_present == FALSE)
    {
      BSIC_T bsic = {0};
      /*******************************************************************/
      /* Compute the BSIC for the new cell (if information is available) */
      /*******************************************************************/
      if (cell_meas_chan_info_ptr->valid_info == TRUE)
      {
        /*********************************************/
        /* L1 was able to decode information for the */
        /* surrounding channel                       */
        /*********************************************/
        bsic.PLMN_colour_code = DECODE_NCC(cell_meas_chan_info_ptr);
        bsic.BS_colour_code = DECODE_BCC(cell_meas_chan_info_ptr);
      }
      /* otherwise, use a zeroed out BSIC */

      MSG_GERAN_MED_1_G("Adding arfcn %d to ncell database",CONVERT_FROM_ARFCN_T(cell_meas_chan_info_ptr->BCCH_ARFCN));
      rr_gprs_add_to_best_six(
        cell_meas_chan_info_ptr->BCCH_ARFCN,
        bsic,
        TRUE,
        gas_id
      );

    } /* measurement report channel was not present in surrounding DB */
  }


  /****************************************************/
  /* For each entry in the measurement report find    */
  /* the matching system info, then validate the BSIC */
  /* and make it part of the best 6 database.         */
  /* and finally calculate and store the C1, C31 and  */
  /* C32 values                                       */
  /****************************************************/
  for (cell_index = 0; cell_index < p_best_six_info->no_of_entries; cell_index++)
  {
    /* Initialise the pointer and work with this */
    meas_chan_info_ptr = &(p_best_six_info->surrounding_channel_info[cell_index]);
    BCCH_ARFCN = meas_chan_info_ptr->BCCH_ARFCN;

    if (meas_chan_info_ptr->valid_info == TRUE)
    {
      /*********************************************/
      /* L1 was able to decode information for the */
      /* surrounding channel                       */
      /*********************************************/
      recieved_bsic.PLMN_colour_code = DECODE_NCC(meas_chan_info_ptr);
      recieved_bsic.BS_colour_code = DECODE_BCC(meas_chan_info_ptr);
    }

    /* Check that there is a matching entry in the database */
    if (rr_gprs_get_best_six_by_arfcn(meas_chan_info_ptr->BCCH_ARFCN, &recieved_bsic, gas_id) != NULL)
    {
       /*Add the channel to the list of cells that L1 is sync'ed to*/
       rr_gprs_add_sync_cell(BCCH_ARFCN, gas_id);
    }
    else
    {
      MSG_GERAN_HIGH_1_G("No PBCCH ncell data ARFCN=%d",CONVERT_FROM_ARFCN_T(BCCH_ARFCN));
      continue;
    }

    /**********************************************************************/
    /* Calculate the C1, C31 and C32 values for this cell and store in the*/
    /* reselection structure.                                             */
    /**********************************************************************/
    rr_gprs_calculate_Cvalues_for_ncell(
      local_gprs_reselection_list_ptr,
      meas_chan_info_ptr,
      strongest_RLA_P,
      gas_id
    );

    /****************************************************************/
    /*The first call to the rr_gprs_calculate_Cvalues_for_ncell will*/
    /*be for the strongest RLA_P as the measurement report list is  */
    /*ordered in RLA_P order the strongest first. So all other calls*/
    /*should be set to false, as these will have lower RLA_P values */
    /****************************************************************/
    strongest_RLA_P = FALSE;

  }/* for each cell in measurement report */

} /* rr_gprs_store_surrounding_measurement_MFRM52*/


/****************************************************************************
 *   Function name:  rr_gprs_compute_C2_with_CRH
 *   --------------
 *
 *   Description: This function computes the C2 value of the ncell with the
 *   -----------  Cell Reselection Hysteresis broadcast on the BCCH of the scell
 *                added to the ncell C2 if it is in the same RA as the scell.
 *                The reason for this is in the spec 3GPP TS 03.22 3.4, and in TC 20.22.19.
 *                Basically if a couple af cells are placed into the better cell
 *                list, eg
 *
 *                          cell1 - C2=10, RAcolour=3 (scell RAcolour=3)
 *                          cell2 - C2=18, RAcolour=1
 *
 *                then cell1 will have CRH added to it to make it more attractive
 *                as a target than cell2, so after this function....
 *                if CRH=10dB
 *                          cell1 - C2=20, RAcolour=3 (scell RAcolour=3)
 *                          cell2 - C2=18, RAcolour=1
 *
 *   Parameters:  CRH, scell RAC, NcellRAC and surroundC2
 *   -----------
 *
 *   Returns:  changed surroundC2 value
 *   --------
 *
 ****************************************************************************/
static int rr_gprs_compute_C2_with_CRH(
  byte              CELL_RESELECT_HYSTERESIS,
  gprs_scell_info_t *scell_info,
  bcch_ncell_info_t *ncell_info,
  int               surroundingC2,
  const gas_id_t    gas_id
)
{

  boolean same_ra = TRUE;

  /*Check to see if the cells are in the same RA, this can only be done if both the cells support gprs */
  if ( (ncell_info->gprs.ra_colour != scell_info->bcch_sys_info.ra_colour) &&
        scell_info->gsm.gprs_indicator &&
        ncell_info->gprs_supported
     )
  {
    same_ra = FALSE;
  }

  if ( rr_gsm_camped_on_ccch(gas_id) &&
       rr_same_lai(&scell_info->gsm.location_area_identification, &ncell_info->gsm.location_area_identification))
  {
    /* If on a GSM cell then use the LAI to decide about addition of CRH */
    surroundingC2 += (CELL_RESELECT_HYSTERESIS);
  }
  else
  if ( same_ra &&
       rr_same_lai(&scell_info->gsm.location_area_identification, &ncell_info->gsm.location_area_identification)
     )
  {
    /* If on a GPRS cell then use the RAC to decide about addition of CRH */
    /***********************************************************/
    /* If the cell is in the same RA as the scell then add the */
    /* CELL_RESELECT_HYSTERESIS to the cell to make it more    */
    /* attractive as a target cell than another that may be in */
    /* the better cell list that is not in the same RA this is */
    /* as described in 3GPP TS 03.22 3.4, and in TC 20.22.19   */
    /***********************************************************/
    surroundingC2 += (CELL_RESELECT_HYSTERESIS);
  }

return surroundingC2;
} /* rr_gprs_compute_C2_with_CRH */

/****************************************************************************
 *   Function name:  rr_gprs_compute_C32_with_CRH
 *   --------------
 *
 *   Description: This function computes the C32 value of the ncell with the
 *   -----------  GPRS Cell Reselection Hysteresis applied. It may also modify the
 *                C31 value if the C31_HYST flag is set. The C31 and C32 values are
 *                provided in the src_gprs_reselection_values structure. The results
 *                are placed into the dest_gprs_reselection_values.
 *
 *   Parameters:  src_gprs reselection structure
 *   -----------  dest_gprs reselection structure
 *                ncell_RAC
 *                recent_reselection_flag
 *
 *   Returns:  No. of entries in the newly generated candidate cell list
 *   --------
 *
 ****************************************************************************/
static void rr_gprs_compute_C32_with_CRH(
  gprs_reselection_values_T *src_gprs_reselection_values,
  gprs_reselection_values_T *dest_gprs_reselection_values,
  boolean                    recent_reselection_flag,
  const gas_id_t             gas_id
)
{
  int        CELL_RESELECT_HYSTERESIS_dB            = 0;
  gprs_scell_info_t             *scell_info = rr_gprs_get_scell_info(gas_id);
  pmo_ncell_info_t              *ptr_surround_pbcch;
  bcch_ncell_info_t             *ptr_surround_bcch;
  byte                           CRH_value = 0;
  byte                           RAH_value = 0;
  boolean                        ncell_params_found = FALSE;
  boolean                        same_routing_area  = FALSE;

  /**********************************************************************************/
  /* Copy over the current values from the Source to the Destination data structure */
  /* as only C32 and maybe C31 will be copied otherwise                             */
  /**********************************************************************************/
  *dest_gprs_reselection_values = *src_gprs_reselection_values;

  /******************************************************/
  /* Set up the parameters depending upon the cell type */
  /******************************************************/

  /****************************************************************/
  /* Look for the BCCH ARFCN in the GSM bcch surrounding database */
  /****************************************************************/
  if ((ptr_surround_bcch = rr_gprs_get_surround_ncell_sys_info(src_gprs_reselection_values->BCCH_ARFCN, gas_id)) != NULL)
  {
    RAH_value = CRH_value = scell_info->gsm.cell_selection_parameters.CELL_RESELECT_HYSTERESIS;
    same_routing_area = TRUE;

    if ((scell_info->bcch_sys_info.ra_colour != ptr_surround_bcch->gprs.ra_colour) &&
         scell_info->gsm.gprs_indicator &&
         ptr_surround_bcch->gprs_supported
       )
    {
      same_routing_area = FALSE;
    }
    /*Proceed with the calculation*/
    ncell_params_found = TRUE;
  }
  /******************************************************************/
  /* No entry has been found in the bcch database one must exist in */
  /* the pbcch ncell database.                                      */
  /******************************************************************/
  else if ((ptr_surround_pbcch = rr_gprs_get_pmo_pcco_ncell_sys_info(src_gprs_reselection_values->BCCH_ARFCN,NULL, gas_id)) != NULL)
  {
    /*System info decodes this parameter for use on BCCH*/
    RAH_value = CRH_value = scell_info->gsm.cell_selection_parameters.CELL_RESELECT_HYSTERESIS;
    same_routing_area = ptr_surround_pbcch->gprs_cell_selection_params.same_ra_as_serving_cell;
    /*Proceed with the calculation*/
    ncell_params_found = TRUE;
  }
  else
  {
    MSG_GERAN_ERROR_1_G("No entry in bcch database for ARFCN %d",CONVERT_FROM_ARFCN_T(src_gprs_reselection_values->BCCH_ARFCN));
  }

  /*Only proceed if the ncell sys info parameters have been located and loaded*/
  if (ncell_params_found)
  {
    if (same_routing_area && (rr_gmminfo_get_gmm_status(gas_id) == GMM_STANDBY_STATE))
    {
      /**********************************************************************/
      /* 1. in standby state if the new cell is in the same routing area      */
      /**********************************************************************/
      CELL_RESELECT_HYSTERESIS_dB = 0;
    }
    else if (same_routing_area && (rr_gmminfo_get_gmm_status(gas_id) == GMM_READY_STATE))
    {
      /*********************************************************************/
      /* 2. In ready state if the new cell is in the same routing area   */
      /*********************************************************************/
      CELL_RESELECT_HYSTERESIS_dB = CRH_value;
    }
    else if (!same_routing_area && (rr_gmminfo_get_gmm_status(gas_id) != GMM_IDLE_STATE))
    {
      /********************************************************************************/
      /* 3. in standby or ready state, if the new cell is in a different routing area */
      /********************************************************************************/
      CELL_RESELECT_HYSTERESIS_dB = RAH_value;
    }

    /*****************************************************************/
    /* If the flag indicates that a cell reselection has taken place */
    /* recently (in the last 15s as specified in GSM05.08 10.1.3     */
    /*****************************************************************/
    if (recent_reselection_flag == TRUE)
    {
      /**********************************************************************/
      /* Make sure the CRH value used is at least 5dB. This will discourage */
      /* a reselection while the recent flag is set even if the CRH value   */
      /* in the SI parameters for the serving cell is < 5dB                 */
      /**********************************************************************/
      if (CELL_RESELECT_HYSTERESIS_dB < 5)
      {
        CELL_RESELECT_HYSTERESIS_dB = 5;
        MSG_GERAN_LOW_0_G("-5dB recent cell reselection flag is set");
      }
    } /* recent_reselection_flag set */

    dest_gprs_reselection_values->C32 = src_gprs_reselection_values->C32 - CELL_RESELECT_HYSTERESIS_dB;
  }
} /* rr_gprs_compute_C32_with_CRH */

/****************************************************************************
 *   Function name:  rr_gprs_check_for_better_GSM_ncell
 *   --------------
 *
 *   Description: This function checks the measurements of the ncells over the
 *   -----------  last path loss period (5secs) and checks to see if a better
 *                cell or cells are available. If they are a list of better cells
 *                (candidate list) is compiled.
 *
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  No. of entries in the newly generated candidate cell list
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_check_for_better_GSM_ncell(
  boolean recent_reselection_flag,
  const gas_id_t gas_id
)
{
  /* Assume that no reselection is required until proven otherwise */
  boolean                        better_cell = FALSE;
  int                            candidate_index = 0;
  boolean                        surrounding_better;
  ARFCN_T                        BCCH_ARFCN;
  uint8                          sys_index;
  bcch_ncell_info_t             *surround_db_entry_ptr = NULL;
  uint8                          cell_index = 0;
  gprs_scell_info_t             *scell_info = rr_gprs_get_scell_info(gas_id);
  rr_resel_candidate_t          *resel_candidate;
  gprs_reselection_structure_T  *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
  gprs_better_cell_list_T       *local_better_cell_list_ptr = rr_get_better_cell_list_ptr(gas_id);
  best_six_ncells_t             *local_best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  /* Reset the better cell list, which will store the best candidate surrounding cells */
  local_better_cell_list_ptr->no_of_entries = 0;

  /********************************************************************/
  /* Check the C2 parameters, for each entry in the surround sys info */
  /* check all the measurement results for that cell to see if its      */
  /* better than the serving cell                                     */
  /********************************************************************/

  /* foreach cell in the surrounding database */
  for (sys_index = 0;
       (sys_index < local_best_six_ncells_ptr->no_of_entries) && (sys_index < MAX_NUMBER_OF_BEST_CELLS);
       sys_index++)
  {
    /* point to the surround SI entry */
    surround_db_entry_ptr = &local_best_six_ncells_ptr->ncell_entry[sys_index].ncell_data->ncell_data.bcch;
    /* assume the surrounding cell will not be better */
    resel_candidate = rr_resel_best_six_find(surround_db_entry_ptr->gsm.BCCH_ARFCN, NULL, gas_id);

    if (resel_candidate == NULL)
    {
      MSG_GERAN_ERROR_0_G("resel_candidate == NULL");
      continue;
    }

    surrounding_better = FALSE;

    if (rr_gprs_resel_suitable_cell(surround_db_entry_ptr, gas_id))
    {
      BCCH_ARFCN = surround_db_entry_ptr->gsm.BCCH_ARFCN;
      /************************************************************************/
      /* Find then Point to the measurement values for the current surrounding*/
      /* cell in the current reselection structure                            */
      /************************************************************************/
      for (cell_index = 0;
           cell_index < local_gprs_reselection_list_ptr->no_of_entries;
           ++cell_index)
      {
        if (ARFCNS_EQUAL(BCCH_ARFCN, local_gprs_reselection_list_ptr->surrounding_values[cell_index].BCCH_ARFCN))
        {
          /******************************************************************/
          /* now compare the C2 values for both the surrounding and serving */
          /* cells are available, perform the comparison using the values   */
          /* cells, to see if the surrounding cell is better                */
          /* Don't include the recent reselection hysteresis. Only check    */
          /* this when the 5 sec imer has expired.                          */
          /******************************************************************/

          surrounding_better = rr_gprs_compare_serving_and_surrounding_C2(
            local_gprs_reselection_list_ptr->serving_values.C2,
            local_gprs_reselection_list_ptr->surrounding_values[cell_index].C2,
            surround_db_entry_ptr,
            FALSE,
            &(surround_db_entry_ptr->gsm.location_area_identification),
            gas_id
          );

          /* Also the ncell must have C1 > 0 to be better */
          if (local_gprs_reselection_list_ptr->surrounding_values[cell_index].C1 < 0 && !rr_allow_negative_c1(surround_db_entry_ptr->gsm.location_area_identification.plmn_id, gas_id))
          {
            surrounding_better = FALSE;
          }

          break;
        }
      }
    } /* surround data valid and cell is not barred */

    /* The surrounding cell has had a better C2 for 5 secs. It is a Better Cell. */
    if (surrounding_better == TRUE)
    {
      /* if the surrounding cell was previously identified as a reselection candidate */
      if (rr_timer_get_state(resel_candidate->reselection_info.five_second_timer_id, gas_id) == RR_TIMER_EXPIRED)
      {
        boolean surrounding_better_with_rrh = TRUE;

        if (recent_reselection_flag)
        {
          /* check that the cell really is better with recent reselection hysteresis
          applied as the better cell 5 sec timers are started regardless of this recent
          reselection hysteresis */
          surrounding_better_with_rrh = rr_gprs_compare_serving_and_surrounding_C2(
            local_gprs_reselection_list_ptr->serving_values.C2,
            local_gprs_reselection_list_ptr->surrounding_values[cell_index].C2,
            surround_db_entry_ptr,
            recent_reselection_flag,
            &(surround_db_entry_ptr->gsm.location_area_identification),
            gas_id
          );
        }


        if ( surrounding_better_with_rrh == FALSE )
        {
          MSG_GERAN_MED_1_G("Ncell %d not better with RRH added",CONVERT_FROM_ARFCN_T(surround_db_entry_ptr->gsm.BCCH_ARFCN));
        }
        else if ( rr_cell_forbidden( surround_db_entry_ptr->gsm.BCCH_ARFCN, gas_id ) )
        {
          MSG_GERAN_MED_1_G("Ncell %d is currently forbidden",CONVERT_FROM_ARFCN_T(surround_db_entry_ptr->gsm.BCCH_ARFCN));
        }
        else
        {
          /*  are any other ncell 5sec timers running for ncells with a greater C2 than this one ???
              if so wait for these to expire then choose the best cell to move to   */
          if (grr_any_other_reselection_candidates(surround_db_entry_ptr->gsm.BCCH_ARFCN, gas_id))
          {
            MSG_GERAN_HIGH_0_G("RESELECTION HOLD OFF - Probable better ncell candidate");
          }
          else
          {
            /****************************************************************************/
            /* Stop/deallocate the timer state to stopped so that if reselection is     */
            /* abandoned for any reason, 5secs must again elapse before starting a cell */
            /* reselection attempt.                                                     */
            /****************************************************************************/
            (void)rr_timer_cancel(resel_candidate->reselection_info.five_second_timer_id, gas_id);

            /****************************************************************************/
            /* Build the list of reselection candidates. 5 seconds must have elapsed    */
            /* since the cell was identified as better. Add the better surrounding      */
            /* cell to the list of better cells that are candidates for cell reselection*/
            /****************************************************************************/
            MSG_GERAN_MED_2_G("ncell better scell, %d in list, force reselection (GAS-ID=%d)",
                    (int)(local_better_cell_list_ptr->no_of_entries),(int)gas_id);

            local_better_cell_list_ptr->candidate_cell[candidate_index].cell_reselection_attempted = FALSE;

            /*Load the data into the better cell list*/
            local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data.BCCH_ARFCN = local_gprs_reselection_list_ptr->surrounding_values[cell_index].BCCH_ARFCN;
            local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data.C1 = local_gprs_reselection_list_ptr->surrounding_values[cell_index].C1;
            local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data.C2 = local_gprs_reselection_list_ptr->surrounding_values[cell_index].C2;
            local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data.RXLEV_average = local_gprs_reselection_list_ptr->surrounding_values[cell_index].RXLEV_average;

            /*Load the data into the better cell list then add the C2 value with Cell Reselection Hysteresis*/
            local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data_with_CRH = local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data;
            local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data_with_CRH.C2 =
              rr_gprs_compute_C2_with_CRH(
                scell_info->gsm.cell_selection_parameters.CELL_RESELECT_HYSTERESIS,
                scell_info,
                surround_db_entry_ptr,
                local_gprs_reselection_list_ptr->surrounding_values[cell_index].C2,
                gas_id
              );

            candidate_index++;
            /*Increment the number of entries in the better cell list*/
            local_better_cell_list_ptr->no_of_entries++;
            /* found a better surrounding cell, reselection should take place */
            better_cell =  TRUE;
          }
        }
      }
      else if (rr_timer_get_state(
                 resel_candidate->reselection_info.five_second_timer_id,
                 gas_id
               ) == RR_TIMER_STOPPED)
      {
        MSG_GERAN_MED_2_G("Reselection candidate ARFCN %d (band %d), wait 5 seconds",
                          surround_db_entry_ptr->gsm.BCCH_ARFCN.num,
                          surround_db_entry_ptr->gsm.BCCH_ARFCN.band);
        /************************************************************************/
        /* the surrounding cell is better, store it as a potential reselection  */
        /* candidate in case it is still better next time C2 values are checked */
        /************************************************************************/
        rr_gprs_start_five_second_timer(&resel_candidate->reselection_info, gas_id);
      }
    }
    else
    {
      /**************************************************************************/
      /* The serving cell is still better than this surrounding cell. If a timer*/
      /* was allocated to this ncell then stop it as the ncell no longer meets  */
      /* the C2 criteria, not better for more than 5 secs                       */
      /**************************************************************************/
      if (resel_candidate->reselection_info.five_second_timer_id != 0)
      {
        rr_gprs_stop_five_second_timer(&resel_candidate->reselection_info, gas_id);
      }
    }/* surrounding is better */
  }/*foreach cell in the surrounding database */

return better_cell;
} /* rr_gprs_check_for_better_GSM_ncell */

/****************************************************************************
 *   Function name:  rr_gprs_check_for_better_GPRS51_ncell
 *   --------------
 *
 *   Description: This function checks the measurements of the ncells to see if a better
 *  ------------  cell or cells are available. The algorithm for testing for a better cell
 *                different to the GSM in as much as just the last recieved ncell values
 *                need to be examined in the reselection structure.
 *                The values that are sent to GRR are averaged over a period and so include
 *                previous measurements, so examining the last recieved measurement does
 *                in fact take in to account all the other previous measurements in the
 *                reporting period.
 *
 *                If a better cell is found it is added to the better cell list.
 *
 *
 *   Parameters:  No input parameters
 *   -----------
 *
 *   Returns:     No. of entries in the newly generated candidate cell list
 *   --------
 *
 ****************************************************************************/
static boolean rr_gprs_check_for_better_GPRS51_ncell(
  boolean recent_reselection_flag,
  const gas_id_t gas_id
)
{
  boolean                      better_cell = FALSE;
  boolean                      HCS_C31_cells_exist = FALSE;
  boolean                      surrounding_better;
  uint8                        candidate_index = 0;
  int                          index;
  int                          CRH_index = 0;
  int                          list_index;
  gprs_reselection_values_T   *ncell = NULL;
  gprs_reselection_values_T    gprs_CRH_reselection_values[MAX_NUMBER_OF_BEST_CELLS];
  pmo_ncell_info_t            *ptr_ncell_pbcch_data = NULL;
  gprs_scell_info_t           *scell_info = rr_gprs_get_scell_info(gas_id);
  bcch_ncell_info_t           *ptr_ncell_bcch_data = NULL;
  uint8                         ncell_priority_class = 0;
  uint8                         scell_priority_class = 0;
  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
  gprs_better_cell_list_T      *local_better_cell_list_ptr = rr_get_better_cell_list_ptr(gas_id);

  /* Reset the CRH values */
  memset(gprs_CRH_reselection_values, 0, sizeof(gprs_CRH_reselection_values));
  /* Reset the better cell list, which will store the best candidate surrounding cells */
  local_better_cell_list_ptr->no_of_entries = 0;

  /********************************************************************/
  /* Check the C32 parameters, for each entry in the surround sys info*/
  /* check all the measurement results for that cell to see if it is  */
  /* better than the serving cell. This comparison demands that there */
  /* are valid serving cell values within the structure               */
  /********************************************************************/
  /**
   * If we have no serving cell measurements we have nothing to compare
   * neighbour cell measurements to.
   */
  if (!local_gprs_reselection_list_ptr->serving_values_valid)
  {
    MSG_GERAN_ERROR_0_G("No valid serving values to compare against");
    return FALSE;
  }

  /**
   * For each neighbour cell in the reselection list, determine whether the
   * cell is suitable.  If it is, add it to a local list of neighbour cells
   * and calculate it's C32-with-CRH value.
   */
  for (index = 0;
       (index < local_gprs_reselection_list_ptr->no_of_entries) && (index < RR_GPRS_RESELECTION_STRUCTURE_SIZE);
       index++)
  {
    ncell = &local_gprs_reselection_list_ptr->surrounding_values[index];

    /* Assume the surrounding cell will not be better */
    surrounding_better = FALSE;

    /* Neighbour cell PLMN is not available in this function, so just use the
    serving cell's PLMN in the call to rr_allow_negative_c1.  They are likely
    to be the same anyway. */
    if (ncell->C1 < 0 && !rr_allow_negative_c1(rr_gprs_get_scell_plmn_id(gas_id), gas_id))
    {
      MSG_GERAN_MED_2_G("Ncell (%d,%d) C1 < 0, skipping",
        ncell->BCCH_ARFCN.num,
        ncell->BCCH_ARFCN.band);
      continue;
    }

    if (rr_cell_forbidden(ncell->BCCH_ARFCN, gas_id))
    {
      MSG_GERAN_MED_2_G("Ncell (%d,%d) forbidden, skipping",
        ncell->BCCH_ARFCN.num,
        ncell->BCCH_ARFCN.band);
      continue;
    }

    /* RR must not reselect a barred cell.  To determine this, we need to find
    the neighbour cell's system information.  This will either be in the
    Surround BCCH database or PSI ncell sys info database. */
    if ((ptr_ncell_bcch_data = rr_gprs_get_surround_ncell_sys_info(ncell->BCCH_ARFCN, gas_id)) != NULL)
    {
      if (!rr_gprs_resel_suitable_cell(ptr_ncell_bcch_data, gas_id))
      {
        MSG_GERAN_MED_2_G("Ncell (%d,%d) not suitable",
          ncell->BCCH_ARFCN.num,
          ncell->BCCH_ARFCN.band);
        continue;
      }
    }
    else if ((ptr_ncell_pbcch_data = rr_gprs_get_pmo_pcco_ncell_sys_info(
                ncell->BCCH_ARFCN,
                NULL,
                gas_id
              )) != NULL)
    {
      if (ptr_ncell_pbcch_data->gprs_cell_selection_params.cell_bar_access_2 == TRUE /*TODO check also the EXC_ACC setting ??*/)
      {
        MSG_GERAN_MED_2_G("Ncell (%d,%d) barred",
          ncell->BCCH_ARFCN.num,
          ncell->BCCH_ARFCN.band);
        continue;
      }
    }
    else
    {
      MSG_GERAN_ERROR_2_G("Ncell (%d,%d) unknown",
        ncell->BCCH_ARFCN.num,
        ncell->BCCH_ARFCN.band);
      continue;
    }

    rr_gprs_compute_C32_with_CRH(
      ncell,
      &gprs_CRH_reselection_values[CRH_index],
      recent_reselection_flag,
      gas_id
    );

    CRH_index++;
  }

  /*********************************************************************/
  /*                        Reselection using HCS                      */
  /*                                                                   */
  /* If HCS parameters are valid on the scell then the HCS Threshold   */
  /* is applied to the C31 calculation, otherwise the value of C31 is  */
  /* always less than zero as HCS_THR is infinity, this applies to all */
  /* cells (scell and ncells).                                         */
  /* According to 05.08 10.1.3 the best cell is either the one with the*/
  /* highest priority among those with C31 >= 0. So for the first e.g  */
  /*                                                                   */
  /* Example:1                                                         */
  /*        C31     C32     PRIORITY_CLASS                             */
  /*        ---     ---     --------------                             */
  /* SCELL   5       32         2                                      */
  /* NCELL1  10      15         1                                      */
  /* NCELL2  20      22         1(Best Cell)                           */
  /* NCELL3 -3       15         3                                      */
  /*                                                                   */
  /* NCELL2 is the best cell. Even though lower C32 the PRIORITY CLASS */
  /* is greater than the scell.                                        */
  /*                                                                   */
  /* Example:2                                                         */
  /*        C31     C32     PRIORITY_CLASS                             */
  /*        ---     ---     --------------                             */
  /* SCELL   5       32         2(Best Cell)                           */
  /* NCELL1  10      15         2                                      */
  /* NCELL2  20      22         2                                      */
  /* NCELL3 -3       15         3                                      */
  /*                                                                   */
  /* SCELL is still the best cell. Best C32 among the highest PRIORITY */
  /*                                                                   */
  /* Example:3                                                         */
  /*        C31     C32     PRIORITY_CLASS                             */
  /*        ---     ---     --------------                             */
  /* SCELL  -5       32         NA                                     */
  /* NCELL1 -10      35         NA(Best Cell)                          */
  /* NCELL2 -20      22         NA                                     */
  /* NCELL3 -3       15         NA                                     */
  /*                                                                   */
  /* NCELL2 is the best cell. No cells have C31 >= 0 so PRIORITY CLASS */
  /* doesn't apply                                                     */
  /*                                                                   */
  /* This means for deciding upon a reselection due to a 'better cell' */
  /* the C31, C32 and PRIORITY_CLASS parameters for both scell and     */
  /* ncell must be considered                                          */
  /*********************************************************************/

  /******************************************************/
  /* See if HCS parameters are available and if so then */
  /* check values to see if any cells have C31 > 0.   */
  /******************************************************/
  if (scell_info->bcch_sys_info.pmo_scell_params_valid &&
      scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params_present &&
      rr_gmminfo_get_gmm_status(gas_id) != GMM_IDLE_STATE)
  {
    for (index = 0, candidate_index = 0;
         (index < CRH_index) && (candidate_index < MAX_NUMBER_OF_BEST_CELLS);
         index++)
    {
      /* Assume that the cell is not better */
      surrounding_better = FALSE;

      if (gprs_CRH_reselection_values[index].C31 >= 0)
      {
        /*********************************************************/
        /* A surrounding ncell has been found that has C31 > 0   */
        /* compare it with the scells C31, C32 and Priority Class*/
        /* at this point a flag is set to ensure that the second */
        /* algorithm isn't tried after the HCS algorithm. Cells  */
        /* have been found with C31 >= 0 so don't check amongst  */
        /* all cells for the best value of C32                   */
        /*********************************************************/
        HCS_C31_cells_exist = TRUE;

        if ((ptr_ncell_bcch_data = rr_gprs_get_surround_ncell_sys_info(
               gprs_CRH_reselection_values[index].BCCH_ARFCN,
               gas_id
             )) != NULL)
        {
          ncell_priority_class = 0;
        }
        else if ((ptr_ncell_pbcch_data = rr_gprs_get_pmo_pcco_ncell_sys_info(
                    gprs_CRH_reselection_values[index].BCCH_ARFCN,
                    NULL,
                    gas_id
                  )) != NULL)
        {
          ncell_priority_class = ptr_ncell_pbcch_data->gprs_cell_selection_params.hcs_params.priority_class;
        }

        scell_priority_class = scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params.priority_class;

        /***********************************************************/
        /* Check that the C31 of the scell is greater than or equal*/
        /* to zero. If it isn't then according to GSM05.08 it      */
        /* cannot be amongst the best cells as there is already a  */
        /* ncell that has C31 >= 0                                 */
        /***********************************************************/
        if (local_gprs_reselection_list_ptr->serving_values.C31 < 0)
        {
          /* The scell has C31 < 0 and is therefore not better than the ncell */
          MSG_GERAN_MED_1_G("Ncell C31 >= 0, Scell C31 < 0.Ncell %d better", CONVERT_FROM_ARFCN_T(gprs_CRH_reselection_values[index].BCCH_ARFCN));
          surrounding_better = TRUE;
        }

        /**********************************************************/
        /* Compare the PRIORITY_CLASS of the ncell with that of   */
        /* the scells. If greater this alone make the ncell better*/
        /**********************************************************/
        if (scell_priority_class < ncell_priority_class && surrounding_better == FALSE)
        {
          /* The ncell has C31 > 0 and a higher PRIORITY_CLASS than the SCELL so it must
          qualify as a better cell */
          MSG_GERAN_MED_1_G("Ncell Scell C31>=0, ARFCN%d is higher priority",CONVERT_FROM_ARFCN_T(gprs_CRH_reselection_values[index].BCCH_ARFCN));
          surrounding_better = TRUE;
        }

        /************************************************************/
        /* Compare the C32 values of the scell and the ncell. If it */
        /* is greater than the C32 of the scell, then the ncell must*/
        /* be of equal PRIORITY CLASS to be a better cell           */
        /************************************************************/
        if (scell_priority_class == ncell_priority_class &&
            local_gprs_reselection_list_ptr->serving_values.C32 < gprs_CRH_reselection_values[index].C32 &&
            surrounding_better == FALSE)
        {
          MSG_GERAN_MED_1_G("Ncell Scell have C31>=0, ncell ARFCN%d has greatest C32",CONVERT_FROM_ARFCN_T(gprs_CRH_reselection_values[index].BCCH_ARFCN));
          surrounding_better = TRUE;
        }

        /*************************************************************/
        /* At this stage all checks have been performed for a better */
        /* ncell. If a better ncell has been detected it is added to */
        /* the better cell list below                                */
        /*************************************************************/
        if (surrounding_better == TRUE)
        {
          /***********************************************/
          /* Add the better surrounding cell to the list */
          /* of better cells that are candidates for     */
          /* cell reselection                            */
          /***********************************************/
          local_better_cell_list_ptr->candidate_cell[candidate_index].cell_reselection_attempted = FALSE;

          if (candidate_index < MAX_NUMBER_OF_BEST_CELLS)
          {
            /* Retrieve the original reselection list data in the last structure */
            for (list_index = 0; list_index < local_gprs_reselection_list_ptr->no_of_entries; list_index++)
            {
              if (ARFCNS_EQUAL(local_gprs_reselection_list_ptr->surrounding_values[list_index].BCCH_ARFCN, gprs_CRH_reselection_values[index].BCCH_ARFCN))
              {
                /* Load the data into the better cell list */
                local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data =
                local_gprs_reselection_list_ptr->surrounding_values[list_index];
              }
            }

            /* Load the CRH data into the better cell list */
            local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data_with_CRH = gprs_CRH_reselection_values[index];
          }

          candidate_index++;
          /*Increment the number of entries in the better cell list*/
          local_better_cell_list_ptr->no_of_entries++;
          /*Set the returned flag to show that a better cell exists*/
          better_cell = TRUE;
        } /* surrounding is better */
      }
    }
  }

  /*************************************************************/
  /* No better surrounding cells have been determined as yet   */
  /* using the above HCS reselection better cell algorithm, or */
  /* HCS isn't in operation on the scell. This is the time to  */
  /* look for a better cell in terms of C32 alone. I.e. the    */
  /* best cell is the cell with the greatest value of C32 among*/
  /* ALL cells.                                                */
  /*************************************************************/
  if ((better_cell == FALSE) && (HCS_C31_cells_exist == FALSE))
  {
    for (index = 0; (index < CRH_index) && (candidate_index < MAX_NUMBER_OF_BEST_CELLS); index++ )
    {
      surrounding_better = FALSE;

      if (gprs_CRH_reselection_values[index].C32 > local_gprs_reselection_list_ptr->serving_values.C32)
      {
        /**********************************************************/
        /* The C32 of the scell is greater than that of the scell */
        /* It therefore qualifies as a better cell              */
        /***********************************************************/
        surrounding_better = TRUE;
        MSG_GERAN_MED_2_G("ncell is better C32n = %d C32s = %d",
                gprs_CRH_reselection_values[index].C32,
                local_gprs_reselection_list_ptr->serving_values.C32);

        /***********************************************/
        /* Add the better surrounding cell to the list */
        /* of better cells that are candidates for   */
        /* cell reselection                            */
        /***********************************************/
        local_better_cell_list_ptr->candidate_cell[candidate_index].cell_reselection_attempted = FALSE;

        /* Set up the candidates multiframe */
        ptr_ncell_bcch_data = rr_gprs_get_surround_ncell_sys_info(
          gprs_CRH_reselection_values[index].BCCH_ARFCN,
          gas_id
        );

        /* Retrieve the original reselection list data in the last structure */
        for (list_index = 0; list_index < local_gprs_reselection_list_ptr->no_of_entries; list_index++)
        {
          if (ARFCNS_EQUAL(local_gprs_reselection_list_ptr->surrounding_values[list_index].BCCH_ARFCN, gprs_CRH_reselection_values[index].BCCH_ARFCN))
          {
            local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data = local_gprs_reselection_list_ptr->surrounding_values[list_index];
          }
        }

        /* Load the CRH data into the better cell list */
        local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data_with_CRH = gprs_CRH_reselection_values[index];
        candidate_index++;

        /* Increment the number of entries in the better cell list */
        local_better_cell_list_ptr->no_of_entries++;

        /* Set the returned flag to show that a better cell exists */
        better_cell = TRUE;
      } /* surrounding is better */
    }
  }

  return better_cell;
} /* rr_gprs_check_for_better_GPRS51_ncell */


/*----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/


/****************************************************************************
 *   Function name:  rr_gprs_init_bad_snr_counter
 *   --------------------------
 *
 *   Description: Initialises the bad_snr_sounter to its correct starting value
 *   ------------
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_init_bad_snr_counter(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_gprs_resel_calc_data_t *rr_resel_calcs_data_ptr = rr_resel_calcs_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_calcs_data_ptr);

  if ( rr_macinfo_get_mac_status(gas_id) == MAC_TRANSFER )
  {
    rr_resel_calcs_data_ptr->bad_snr_counter = SCELL_XFER_SNR_COUNTER_URGENCY_THRESHOLD;
  }
  else
  {
    rr_resel_calcs_data_ptr->bad_snr_counter = SCELL_IDLE_SNR_COUNTER_URGENCY_THRESHOLD;
  }
}

/****************************************************************************
 *   Function name:  rr_gprs_is_snr_counter_zero
 *   --------------------------
 *
 *   Description: returns true if the bad_snr_counter is equal to zero
 *   ------------
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  TRUE if bad_snr_counter is equal to zero
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_is_snr_counter_zero(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_gprs_resel_calc_data_t *rr_resel_calcs_data_ptr = rr_resel_calcs_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_calcs_data_ptr);

  return (rr_resel_calcs_data_ptr->bad_snr_counter == 0);
}

/****************************************************************************
 *   Function name:  rr_gprs_check_for_path_loss
 *   --------------------------
 *
 *   Description: This function is used to see of the path loss of the scell
 *   ------------ has become too high. If so the best target ncell is chosen
 *                to use in the reselection.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  TRUE = Path Loss too High / FALSE = Path loss is OK
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_check_for_path_loss(
  const gas_id_t gas_id
)
{
  boolean Path_Loss_Reselection = FALSE;

  if (rr_gprs_parameters_available(gas_id))
  {
    /*Use the GPRS path loss algorithm in GSM05.08 10.1.3*/
    Path_Loss_Reselection = rr_gprs_check_scell_path_loss_GPRS(gas_id);
  }
  else
  {
    /*Use the GSM path loss algorithm in GSM05.08 6.6.2*/
    Path_Loss_Reselection = rr_gprs_check_scell_path_loss_GSM(gas_id);
  }

  return Path_Loss_Reselection;
}

/****************************************************************************
 *   Function name:  rr_gprs_check_for_bad_scell
 *   -------------
 *
 *   Description: This function is used to run the Qualcomm devised scell SNR
 *   ------------ algorithm, to determine if the scell should be dumped and
 *                another ncell targeted for reselection
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  TRUE = bad scell / FALSE = scell is OK
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_check_for_bad_scell(
  const gas_id_t gas_id
)
{
  boolean bad_scell_reselection = FALSE;

  if (rr_macinfo_get_mac_status(gas_id) == MAC_TRANSFER)
  {
#ifdef DISABLE_XFER_SNR_RESELECTION
    #error code not present
#else
    bad_scell_reselection = rr_gprs_check_for_bad_scell_in_xfer(gas_id);
#endif
  }
  else
  {
#ifdef DISABLE_IDLE_SNR_RESELECTION
    #error code not present
#else
    bad_scell_reselection = rr_gprs_check_for_bad_scell_in_idle(gas_id);
#endif
  }

  return bad_scell_reselection;

}

/****************************************************************************
 *   Function name:  rr_gprs_process_serving_aux_meas_ind
 *   --------------
 *
 *   Description: This function is used to process the mph_serving_aux_meas_ind
 *   ------------ primitive from L1. This indicates is cell current scell is bad
 *                and an immediate reselection is required.
 *
 *   Parameters: mph_serving_aux_meas_ind_T
 *   -----------
 *
 *   Returns: none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_process_serving_aux_meas_ind(
  mph_serving_aux_meas_ind_T* mph_serving_aux_meas_ind,
  const gas_id_t gas_id
)
{
  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  rr_gprs_store_C1_C2_instant_serving_measurement(
    mph_serving_aux_meas_ind->RXLEV_instant,
    gas_id
  );

  local_gprs_reselection_list_ptr->idle_serving_snr_is_bad = mph_serving_aux_meas_ind->SNR_is_bad;

  local_gprs_reselection_list_ptr->transfer_serving_snr_is_bad = FALSE;

  if ( local_gprs_reselection_list_ptr->idle_serving_snr_is_bad == FALSE)
  {
    /* Reset the counter */
    rr_gprs_init_bad_snr_counter(gas_id);
  }
}


/****************************************************************************
 *   Function name:  rr_gprs_store_serving_meas
 *   --------------------------
 *
 *   Description: This function is used to store and calculate C1, C2 or C31, C32
 *   ------------ values for the serving cell. These are based upon measurement
 *                reports from L1.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_store_serving_meas(
  mph_serving_meas_ind_t* mph_serving_meas_ind,
  const gas_id_t gas_id
)
{
  byte RXLEV;
  byte RXQUAL;

  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_resel_calc_data_t *rr_resel_calcs_data_ptr = rr_resel_calcs_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_calcs_data_ptr);

  /* Extract the RXLEV and RXQUAL from the message */
  RXLEV  = mph_serving_meas_ind->rxlev_average;
  RXQUAL = mph_serving_meas_ind->rxqual;

  if (rr_gprs_parameters_available(gas_id))
  {
    rr_gprs_store_C31_C32_serving_measurement(RXLEV, RXQUAL, gas_id);
  }
  else
  {
    rr_gprs_store_C1_C2_serving_measurement(RXLEV, RXQUAL, gas_id);
    //transfer SNR
    local_gprs_reselection_list_ptr->idle_serving_snr_is_bad = FALSE;

    if ( mph_serving_meas_ind->SNR_is_bad_valid )
    {
      local_gprs_reselection_list_ptr->transfer_serving_snr_is_bad = mph_serving_meas_ind->SNR_is_bad;
    }

    if ( ( local_gprs_reselection_list_ptr->transfer_serving_snr_is_bad == FALSE ) &&
         ( mph_serving_meas_ind->SNR_is_bad_valid ) )
    {

      /* If the SNR is OK after it has been bad print out F3 to show this */
      if ( rr_resel_calcs_data_ptr->bad_snr_counter != SCELL_XFER_SNR_COUNTER_URGENCY_THRESHOLD )
      {
        MSG_GERAN_HIGH_0_G("scell SNR is now OK");
      }

      /* Reset the counter */
      rr_gprs_init_bad_snr_counter(gas_id);
    }
    else
    /*
    Decrement the snr counter to track how desperate the scell situation is. Once at zero
    The reselection tests are allowed to run. This stop any reselection for a one-off
    bad snr report
    */
    if ( !rr_gprs_is_snr_counter_zero(gas_id) &&
         ( mph_serving_meas_ind->SNR_is_bad_valid ) &&
         ( local_gprs_reselection_list_ptr->transfer_serving_snr_is_bad == TRUE ) )
    {
      rr_resel_calcs_data_ptr->bad_snr_counter--;
    }

  }
}

/****************************************************************************
 *   Function name:  rr_gprs_store_serving_idle_meas
 *   --------------------------
 *
 *   Description: This function is used to store and calculate C1, C2 or C31, C32
 *   ------------ values for the serving cell. These are based upon measurement
 *                reports from L1.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_store_serving_idle_meas(
  mph_serving_idle_meas_ind_T* mph_serving_idle_meas_ind,
  const gas_id_t gas_id
)
{
  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  if (rr_gprs_parameters_available(gas_id))
  {
    rr_gprs_store_C31_C32_serving_measurement(
      mph_serving_idle_meas_ind->RXLEV_average,
      RR_INVALID_RXQUAL_VALUE,
      gas_id
    );
  }
  else
  {
    rr_gprs_store_C1_C2_serving_measurement(
      mph_serving_idle_meas_ind->RXLEV_average,
      RR_INVALID_RXQUAL_VALUE,
      gas_id
    );

    rr_gprs_store_C1_C2_instant_serving_measurement(
      mph_serving_idle_meas_ind->RXLEV_average,
      gas_id
    );

    local_gprs_reselection_list_ptr->transfer_serving_snr_is_bad = FALSE;
    local_gprs_reselection_list_ptr->idle_serving_snr_is_bad = mph_serving_idle_meas_ind->SNR_is_bad;
    if ( local_gprs_reselection_list_ptr->idle_serving_snr_is_bad == FALSE)
    {
      /* Reset the counter */
      rr_gprs_init_bad_snr_counter(gas_id);
    }
  }
}

/****************************************************************************
 *   Function name:  rr_gprs_store_surround_meas
 *   -------------
 *
 *   Description: This function is used top decide wether to store the measurent
 *   ------------ report as C1,C2 values or new gprs c31 c32 values.
 *
 *   Parameters: New inpout message
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_store_surround_meas(
  mph_surround_meas_ind_T* mph_surround_meas_ind,
  const gas_id_t gas_id
)
{
  best_six_ncells_t   *local_best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);
  
  if (mph_surround_meas_ind->best_six_info.no_of_entries > MAX_NUMBER_OF_BEST_CELLS)
  {
    MSG_GERAN_HIGH_1_G("MPH_SURROUND_MEAS_IND: %d entries rcvd, capping at 6",
      mph_surround_meas_ind->best_six_info.no_of_entries);
    mph_surround_meas_ind->best_six_info.no_of_entries = MAX_NUMBER_OF_BEST_CELLS;
  }

  if ( rr_gprs_camped_on_ccch(gas_id) || rr_gsm_camped_on_ccch(gas_id) )
  {
    /*Store the new surround measurements and use them to calculate C1, C2, or C31, C32*/
    rr_gprs_store_surrounding_measurement_MFRM51(&mph_surround_meas_ind->best_six_info, gas_id);
  }
  else
  {
    /*Store the surrounding measurements and use them to calculate C1, C31 and C32*/
    rr_gprs_store_surrounding_measurement_MFRM52(&mph_surround_meas_ind->best_six_info, gas_id);
  }

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  rr_handle_ncell_callback(*local_best_six_ncells_ptr, gas_id);
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */

  {
    gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
    gprs_reselection_values_T *curr_resel_entry_ptr = NULL;
    eng_mode_nmr_info_t  nmr_info;
    byte                 i,j;

    memset(&nmr_info, 0, sizeof(nmr_info));

    /* save NMR info for AGPS */
    for (i=0; i < local_best_six_ncells_ptr->no_of_entries; i++)
    {
      byte bsic = CONVERT_FROM_BSIC_T(local_best_six_ncells_ptr->ncell_entry[i].ncell_data->bsic);
      curr_resel_entry_ptr = NULL;
      for (j=0; j < local_gprs_reselection_list_ptr->no_of_entries; j++)
      {
        if (ARFCNS_EQUAL(local_gprs_reselection_list_ptr->surrounding_values[j].BCCH_ARFCN,
                         local_best_six_ncells_ptr->ncell_entry[i].ncell_data->arfcn))
        {
          curr_resel_entry_ptr = &local_gprs_reselection_list_ptr->surrounding_values[j];
          break;
        }
      }
  
      rr_ded_meas_add_eng_mode_nmr_info(
        local_best_six_ncells_ptr->ncell_entry[i].ncell_data->arfcn,
        bsic,
        local_best_six_ncells_ptr->ncell_entry[i].ncell_data->rxlev,
        local_best_six_ncells_ptr->ncell_entry[i].ncell_data,
        &nmr_info,
        curr_resel_entry_ptr);
    }
  
    if (nmr_info.num_of_items != 0)
    {
      (void)time_get(nmr_info.time_stamp);
  
      geran_eng_mode_data_write(
        ENG_MODE_NMR_INFO,    // geran_eng_mode_info_type
        (void *)&nmr_info,    // param_copy
        TRUE,                 // valid_flag
        gas_id                // gas_id
      );
    }
  }
}

/****************************************************************************
 *   Function name:  rr_gprs_store_nc_measurements
 *   -------------
 *
 *   Description: This function is used to format and store the nc measurements
 *   ------------ from L1. These may come in instead of the mph_surround_meas_ind.
 *                The big difference is that nc measurement message has a list of
 *                up to 32 frequencies, these are in signal strength order.
 *                Cell reselection requires the best 6 from this list, so these are
 *                extracted then sent to the normal processing route as a
 *                mph_surround_meas_ind message.
 *
 *   Parameters: New inpout message
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_process_nc_meas_ind(
  mph_nc_measurement_ind_t* mph_nc_measurement_ind,
  const gas_id_t gas_id
)
{
mph_surround_meas_ind_T mph_surround_meas_ind;
uint8 index;

  /* Set the count to zero */
  mph_surround_meas_ind.best_six_info.no_of_entries = 0;

  for (index = 0; index < mph_nc_measurement_ind->no_of_entries; index ++)
  {
    /* Add the rxlev into the structure */
    if ( mph_nc_measurement_ind->surrounding_channel_info[index].valid_info )
    {
      mph_surround_meas_ind.best_six_info.surrounding_channel_info[mph_surround_meas_ind.best_six_info.no_of_entries].RXLEV_average =
        mph_nc_measurement_ind->surrounding_channel_info[index].RXLEV_average;

      mph_surround_meas_ind.best_six_info.surrounding_channel_info[mph_surround_meas_ind.best_six_info.no_of_entries].BCCH_ARFCN =
        mph_nc_measurement_ind->surrounding_channel_info[index].BCCH_ARFCN;

      mph_surround_meas_ind.best_six_info.surrounding_channel_info[mph_surround_meas_ind.best_six_info.no_of_entries].SCH_block[0] =
        mph_nc_measurement_ind->surrounding_channel_info[index].SCH_block[0];

      mph_surround_meas_ind.best_six_info.surrounding_channel_info[mph_surround_meas_ind.best_six_info.no_of_entries].SCH_block[1] =
        mph_nc_measurement_ind->surrounding_channel_info[index].SCH_block[1];

     mph_surround_meas_ind.best_six_info.surrounding_channel_info[mph_surround_meas_ind.best_six_info.no_of_entries].SCH_block[2] =
        mph_nc_measurement_ind->surrounding_channel_info[index].SCH_block[2];

      mph_surround_meas_ind.best_six_info.surrounding_channel_info[mph_surround_meas_ind.best_six_info.no_of_entries].SCH_block[3] =
        mph_nc_measurement_ind->surrounding_channel_info[index].SCH_block[3];

      mph_surround_meas_ind.best_six_info.surrounding_channel_info[mph_surround_meas_ind.best_six_info.no_of_entries].valid_info = TRUE;

      mph_surround_meas_ind.best_six_info.no_of_entries++;

      /* See if we have filled the best 6 locations */
      if ( mph_surround_meas_ind.best_six_info.no_of_entries == MAX_NUMBER_OF_BEST_CELLS )
      {
        /* The best 6 have bee found - terminate the loop */
        break;
      }
    }

  }

  if(mph_nc_measurement_ind->no_of_entries > 0)
  {
#ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST
    /* initialize the flags which are only sent by GL1 in MPH_SURROUD_MEAS_IND*/
    mph_surround_meas_ind.best_six_info.reduced_ncell_list_stored_in_l1 = FALSE;
    mph_surround_meas_ind.best_six_info.reduced_ncell_list_mode_active = FALSE;
    mph_surround_meas_ind.best_six_info.reduced_ncell_list_allowed = FALSE;
#endif /* FEATURE_GSM_SCE_REDUCED_NCELL_LIST */

    /*process the message as normal*/
    rr_gprs_store_surround_meas(&mph_surround_meas_ind, gas_id);
  }
  else
  {
    MSG_GERAN_LOW_0_G("==No NC entries in RPT==");
  }
}






/****************************************************************************
 *   Function name:  rr_gprs_store_surrounding_measurement_MFRM51
 *   -------------
 *
 *   Description: This function is used to handle the recieved measurements from
 *   -----------  L1 for the surrounding ncells. The C1 and C2 values for the
 *                measurements are calculated. This is always used whilst camped
 *                on BCCH/CCCH.
 *
 *   Parameters: best_six_info_T - a list of the 6 stongest ncells
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 ****************************************************************************/
void rr_gprs_store_surrounding_measurement_MFRM51(
  best_six_info_T * p_best_six_info,
  const gas_id_t gas_id
)
{
  int                           measurement_report_entries;
  boolean                       bsic_changed = FALSE;
  boolean                       channel_present = FALSE;
  boolean                       still_best_six = FALSE;
  int                           cell_index;
  BSIC_T                        new_bsic;
  ARFCN_T                       scell_arfcn = rr_gprs_get_scell_arfcn(gas_id);
  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
  best_six_ncells_t            *local_best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  struct {
    uint8 no_of_cells_to_remove;
    struct {
      ARFCN_T arfcn;
      BSIC_T bsic;
    } details[MAX_NUMBER_OF_BEST_CELLS];
  } cells_to_remove = {0};


  /* Clean out the current entries in the reselection structure list */
  local_gprs_reselection_list_ptr->no_of_entries = 0;

  /*********************************************************************/
  /* Remove the serving cell from the measurement report (if present). */
  /* The serving cell should not be included in the BA list, but just  */
  /* in case, it should not be in the measurement report               */
  /*********************************************************************/
  rr_gprs_remove_serving_cell_from_surrounding_measurement(
    rr_gprs_get_scell_arfcn(gas_id),
    p_best_six_info
  );

  /* Get stats on how many cells in best 6 database and the measurement report */
  measurement_report_entries = p_best_six_info->no_of_entries;

#ifdef FEATURE_ENABLE_GPRS_RESELECTION_MESSAGING
  MSG_GERAN_MED_1_G("NCELL_MEAS_RPT:%d entries",measurement_report_entries);
#endif

  /*#############################################################################*/
  /* RR has just received a new measurement report of the best six surrounding   */
  /* cells (based on the BA list sent to L1). Check the best six database to     */
  /* make sure that the entries are still coming in in the latest measurment     */
  /* report from L1. Identify best six entries to be removed at this point.      */
  /*#############################################################################*/

  /* foreach cell in the ncell bcch surrounding info database */
  for (cell_index = 0; cell_index < local_best_six_ncells_ptr->no_of_entries; cell_index++)
  {

    /******************************************************/
    /* Determine if the surrounding cell is still in the  */
    /* best six measurement report (also determine if the */
    /* cell is present, but the BSIC has changed)         */
    /******************************************************/
    still_best_six = rr_gprs_surrounding_cell_in_best_six(
      local_best_six_ncells_ptr->ncell_entry[cell_index].ncell_data,
      p_best_six_info,
      &bsic_changed,
      &new_bsic,
      gas_id
    );

    /* check to see if the BSIC has changed */
    if (bsic_changed == TRUE)
    {
      /********************************************************************/
      /* BSIC has changed !!                                              */
      /* We need to update system information for this surrounding cell   */
      /* since it really represents a completely new cell. Don't remove   */
      /* the entry, just invalidate the data and change the BSIC. This now*/
      /* represents a brand new cell                                      */
      /********************************************************************/
      MSG_GERAN_MED_1_G("BSIC changed for ncell %d",CONVERT_FROM_ARFCN_T(local_best_six_ncells_ptr->ncell_entry[cell_index].ncell_data->arfcn));
      rr_gprs_change_bcch_best_six_bsic(
        local_best_six_ncells_ptr->ncell_entry[cell_index].ncell_data->arfcn,
        new_bsic,
        gas_id
      );
    }
    else if (still_best_six == FALSE)
    {
      /*************************************************************************/
      /* The current channel in best 6 database is not part of the best six    */
      /* cells in the measurement report. Mark that it needs to be removed     */
      /*************************************************************************/
      cells_to_remove.details[cells_to_remove.no_of_cells_to_remove].arfcn = local_best_six_ncells_ptr->ncell_entry[cell_index].ncell_data->arfcn;
      cells_to_remove.details[cells_to_remove.no_of_cells_to_remove].bsic = local_best_six_ncells_ptr->ncell_entry[cell_index].ncell_data->bsic;
      cells_to_remove.no_of_cells_to_remove++;
    }
    else /* still_best_six == TRUE */
    {
      /*************************************************************************/
      /* The cell is still ok to stay in the best six database. Leave things   */
      /* as they are.                                                          */
      /*************************************************************************/
      ;
    }
    /* otherwise, the cell is still part of the best six... keep it arround */
  } /* for each cell in the surrounding SI DB */


  /* Only now can the identified cells be deleted */
  for (cell_index = 0; cell_index < cells_to_remove.no_of_cells_to_remove; cell_index++)
  {
    rr_gprs_remove_from_best_six(
      cells_to_remove.details[cell_index].arfcn,
      cells_to_remove.details[cell_index].bsic,
      gas_id
    );
  }

/*##################################################################################*/


  /************************************************************************/
  /* At this point, all the cells in the best six database that           */
  /* were not in the measurement report have been deleted. Now it is time */
  /* to add new cells in the measurement report that are not already in   */
  /* the database (this includes any cells for which BSIC has changed)    */
  /************************************************************************/

  /* foreach cell in the measurement report */
  for (cell_index = 0; cell_index < measurement_report_entries; cell_index++)
  {
    surrounding_channel_info_T *meas_chan_info_ptr = &(p_best_six_info->surrounding_channel_info[cell_index]);
    BSIC_T bsic = {0};

    /* Always decode the BSIC for later use */
    if (meas_chan_info_ptr->valid_info == TRUE)
    {
      /*********************************************/
      /* L1 was able to decode information for the */
      /* surrounding channel                       */
      /*********************************************/
      bsic.PLMN_colour_code = DECODE_NCC(meas_chan_info_ptr);
      bsic.BS_colour_code = DECODE_BCC(meas_chan_info_ptr);
    }

    /********************************************************************************/
    /* Try to match the ARFCN in the measurement report with a ncell database entry */
    /* this function will look in the GSM surround database and also the pbcch      */
    /* surround cell database. If the channel is present in the pbcch necll database*/
    /* but has no penalty timer allocated, then this function recognises this as a  */
    /* new cell on the best 6 list so allocates a penalty timer to it and starts it */
    /********************************************************************************/
    channel_present = rr_gprs_is_ncell_in_best_six(meas_chan_info_ptr, gas_id);

    /* if the measurement report channel is not in the surround DB then an entry must be added */
    if (channel_present == FALSE)
    {
      MSG_GERAN_MED_1_G("Adding arfcn %d to ncell database",CONVERT_FROM_ARFCN_T(meas_chan_info_ptr->BCCH_ARFCN));
      rr_gprs_add_to_best_six(
        meas_chan_info_ptr->BCCH_ARFCN,
        bsic,
        TRUE,
        gas_id
      );
    } /* measurement report channel was not present in surrounding DB */

    /* If the ncell is synced to see if we need to schedule a surround read */
    if ( meas_chan_info_ptr->valid_info &&
         ( rr_gprs_camped_on_ccch(gas_id) || rr_gsm_camped_on_ccch(gas_id) ) )
    {
      gsm_ncell_list_member_t * best_six_member = rr_gprs_get_best_six_by_arfcn(
        meas_chan_info_ptr->BCCH_ARFCN,
        &bsic,
        gas_id
      );

      if( best_six_member && best_six_member->bcch_or_pmo == BCCH_FORMAT )
      {
        /* allow non drx preemption for the best cell if sys info is needed */
        rr_gprs_ncell_bcch_read_manager(best_six_member, (boolean)((cell_index == 0) ? TRUE : FALSE), gas_id);
      }
    }

    /*ENG_MODE_NMR_INFO*/
    {
      if (meas_chan_info_ptr->valid_info == TRUE)
      {
        gsm_ncell_list_member_t * best_six_member = rr_gprs_get_best_six_by_arfcn(
          meas_chan_info_ptr->BCCH_ARFCN,
          &bsic,
          gas_id
        );

        if (best_six_member != NULL)
        {
          best_six_member->rxlev = (uint8)meas_chan_info_ptr->RXLEV_average;
        }
      }
    }

  } /* foreach cell in the measurement report */

  /***************************************************************************/
  /* At this point, all the cells in the measurement report should be in the */
  /* surrounding database. Some may have been there previously, while others */
  /* may have been added above                                               */
  /***************************************************************************/

   /*Clean out the list of sync cells ready for the latest entries*/
  rr_gprs_clear_sync_cell_list(gas_id);

  /* foreach cell in the measurement report */
  for (cell_index = 0; cell_index < measurement_report_entries; cell_index++)
  {
    ARFCN_T BCCH_ARFCN;
    /***********************************************************************/
    /* Channel information associated with the current surrounding channel */
    /* in the measurement report                                           */
    /***********************************************************************/
    surrounding_channel_info_T *current_meas_channel_ptr;
    current_meas_channel_ptr = &(p_best_six_info->surrounding_channel_info[cell_index]);

    BCCH_ARFCN = current_meas_channel_ptr->BCCH_ARFCN;

    /***************************************************************/
    /* continue to ignore reports for the serving cell, as its not */
    /* surrounding                                                 */
    /***************************************************************/
    if ( ARFCNS_EQUAL( scell_arfcn, BCCH_ARFCN ) )
    {
      MSG_GERAN_MED_0_G("Ignoring report from serving cell");
    }
    else
    {
      /************************************************************************/
      /* Compute the C1/C2 or C31/C32 values for the current surrounding cell */
      /* and store in the reselection structure                               */
      /************************************************************************/
      rr_gprs_calculate_Cvalues_for_ncell(
        local_gprs_reselection_list_ptr,
        current_meas_channel_ptr,
        FALSE,                 /*C32_QUAL not used on GPRS 51 so value doesn't matter*/
        gas_id
      );
      /*Add the channel to the list of cells that L1 is sync'ed to*/
      rr_gprs_add_sync_cell(BCCH_ARFCN, gas_id);
    }
  }/* for each cell in measurement report */

#ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST
  if(p_best_six_info->reduced_ncell_list_allowed)
  {
    /* Received from GL1 in MPH_SURROUND_MEAS_IND: TOP2(TRUE), TOP6(FALSE)*/
    boolean  reduced_ncell_list_l1_view = p_best_six_info->reduced_ncell_list_stored_in_l1;
    /* L1's current mode of operation, TOP2(TRUE), TOP6(FALSE) */
    boolean  reduced_ncell_list_current_mode = p_best_six_info->reduced_ncell_list_mode_active;

    /* GRR has computed the C2 values for the available ncells now check if we can enter into TOP 2 or 6 mode */
    /* GL1 in TOP6 mode */
    if ( !reduced_ncell_list_current_mode)
    {
      uint8  entry_index = local_gprs_reselection_list_ptr->no_of_entries;
      if ( entry_index >= RECVD_REDUCED_NCELL_LIST_SIZE)
      {
        /* GL1 in TOP6 mode so do 5dB check */
        reduced_ncell_list_current_mode = rr_gprs_compare_serv_and_surr_C2(local_gprs_reselection_list_ptr, MIN_SERV_C2_DIFF_TOP6_MODE, gas_id);
      }
    }
    else
    {
      /* GL1 in TOP2 mode so do 3dB check*/
      reduced_ncell_list_current_mode = rr_gprs_compare_serv_and_surr_C2(local_gprs_reselection_list_ptr, MIN_SERV_C2_DIFF_TOP2_MODE, gas_id);
    }

    /* send indication to GL1 if mode needs to be changed */
    if ( reduced_ncell_list_current_mode != reduced_ncell_list_l1_view)
    {
      rr_l1_send_reduced_ncell_meas_ind(reduced_ncell_list_current_mode, gas_id);
    }
  } /* if(p_best_six_info->reduced_ncell_list_allowed) */
#endif /* FEATURE_GSM_SCE_REDUCED_NCELL_LIST*/     
} /* rr_gprs_store_surrounding_measurement_MFRM51 */










/*===========================================================================

FUNCTION  rr_gprs_compute_surrounding_C2

DESCRIPTION

Computes the C2 value for a surrounding cell using the given C1 value and
 system information

===========================================================================*/

int rr_gprs_compute_surrounding_C2(
  int C1,
  rr_resel_candidate_t *resel_candidate,
  const gas_id_t gas_id
)
{
  int C2 = 0;
  information_T * surround_db_entry_ptr;

  if (resel_candidate == NULL)
  {
    MSG_GERAN_ERROR_0_G("resel_candidate == NULL");
    return 0;
  }

  surround_db_entry_ptr = &resel_candidate->ncell_data->ncell_data.bcch.gsm;

  /*******************************************************************/
  /* if the current cell has the cell reselection parameters present */
  /* (from SI 3/4/7/8)                                               */
  /*******************************************************************/
  if (surround_db_entry_ptr->cell_selection_parameters.CELL_RESELECT_PARAM_IND)
  {
    int PENALTY_TIME;
    int CELL_RESELECT_OFFSET_DB;
    int TEMPORARY_OFFSET_DB;

    /*************************************************************************/
    /* The additional cell reselection parameters are present for this cell. */
    /* These parameters include the peanlty time associated with the cell    */
    /*************************************************************************/
    PENALTY_TIME = surround_db_entry_ptr->cell_selection_parameters.PENALTY_TIME;
    CELL_RESELECT_OFFSET_DB = surround_db_entry_ptr->cell_selection_parameters.CELL_RESELECT_OFFSET * 2;
    TEMPORARY_OFFSET_DB = surround_db_entry_ptr->cell_selection_parameters.TEMPORARY_OFFSET * 10;

    /**********************************************************/
    /* Compute the C2 value for the current surrounding cell. */
    /* See GSM05.08 6.4 for formulas used                     */
    /**********************************************************/
    if (PENALTY_TIME != 31)
    {
      /* if the penalty timer for the current cell is running */
      if (rr_timer_is_running(resel_candidate->penalty_timer_id, gas_id))
      {
        if (surround_db_entry_ptr->cell_selection_parameters.TEMPORARY_OFFSET == 7)
        {
          /************************************************/
          /* should be negative infinity but close enough */
          /* See GSM05.08 section 9                       */
          /************************************************/
          C2=-100;
        }
        else
        {
          /* subtract temporary offset DB while penalty timer is running */
          C2 = C1 + CELL_RESELECT_OFFSET_DB - TEMPORARY_OFFSET_DB;
        }
      }
      else /* otherwise, the penalty timer is not running */
      {
        /* the temporary offset no longer applies */
        C2 = C1 + CELL_RESELECT_OFFSET_DB;
      }
    }
    else /* PENALTY_TIME != 31 */
    {
      /**********************************************************************/
      /* A special value of 31 as the penalty time indicates that           */
      /* no penalty time is used and the reselect offset must be subtracted */
      /**********************************************************************/
      C2 = C1 - CELL_RESELECT_OFFSET_DB;
    }
  }
  else /* additional cell reselection parameters not present for current cell */
  {
    /**************************/
    /* C2 value is same as C1 */
    /**************************/
    C2 = C1;
  }

  return C2;
} /* rr_gprs_compute_surrounding_C2 */








/****************************************************************************
 *   Function name:  rr_gprs_build_candidate_cell_list_GSM
 *   --------------
 *
 *   Description: This function is used compile a candidate cell list. The cell
 *   -----------  list is not in order, and contains only entries for ncells with
 *                C1 > 0
 *
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  No. of entries in the newly generated candidate cell list
 *   --------
 *
 ****************************************************************************/
int rr_gprs_build_candidate_cell_list_GSM(
  const gas_id_t gas_id
)
{
  int                            index;
  int                            candidate_index = 0;
  bcch_ncell_info_t             *ptr_surround_database = NULL;
  gprs_scell_info_t             *scell_info = rr_gprs_get_scell_info(gas_id);
  gprs_reselection_structure_T  *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
  gprs_better_cell_list_T       *local_better_cell_list_ptr = rr_get_better_cell_list_ptr(gas_id);

  local_better_cell_list_ptr->no_of_entries = 0;

  /* Build the better cell list in preparation for reselection. */
  for(index=0;
      (index < local_gprs_reselection_list_ptr->no_of_entries) && (index < RR_GPRS_RESELECTION_STRUCTURE_SIZE);
      index++)
  {
    ptr_surround_database = rr_gprs_get_surround_ncell_sys_info(
      local_gprs_reselection_list_ptr->surrounding_values[index].BCCH_ARFCN,
      gas_id
    );

    if (ptr_surround_database != NULL)
    {
      /*First step is to only add to candidate cell list those entries with C1 > 0*/
      if (local_gprs_reselection_list_ptr->surrounding_values[index].C1 > 0 || rr_allow_negative_c1(ptr_surround_database->gsm.location_area_identification.plmn_id, gas_id))
      {
        local_better_cell_list_ptr->candidate_cell[candidate_index].cell_reselection_attempted = FALSE;

        if( rr_gprs_resel_suitable_cell(ptr_surround_database, gas_id) )
        {
          /*Load the last measurements*/
          local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data = local_gprs_reselection_list_ptr->surrounding_values[index];

          /*Load the data into the better cell list then add the C2 value with Cell Reselection Hysteresis*/
          local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data_with_CRH = local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data;
          local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data_with_CRH.C2 =
            rr_gprs_compute_C2_with_CRH(
              scell_info->gsm.cell_selection_parameters.CELL_RESELECT_HYSTERESIS,
              scell_info,
              ptr_surround_database,
              local_gprs_reselection_list_ptr->surrounding_values[index].C2,
              gas_id
            );

          candidate_index++;
          local_better_cell_list_ptr->no_of_entries++;
        }
      }
      else
      {
        MSG_GERAN_MED_1_G(
          "NCELL ARFCN %d has C1 < 0",
          CONVERT_FROM_ARFCN_T(local_gprs_reselection_list_ptr->surrounding_values[index].BCCH_ARFCN)
        );
      }
    }
    else
    {
      MSG_GERAN_HIGH_1_G(
        "ncell %d database entry not found",
        CONVERT_FROM_ARFCN_T(local_gprs_reselection_list_ptr->surrounding_values[index].BCCH_ARFCN)
      );
    }
  }

  /*Check to make sure that there are entries in the candidate cell list after
  examining all the data. */
  if (local_better_cell_list_ptr->no_of_entries == 0)
  {
    MSG_GERAN_MED_0_G("No ncells have C1 > 0; No candidates");
  }

return (int)local_better_cell_list_ptr->no_of_entries;
}

/****************************************************************************
 *   Function name:  rr_gprs_build_candidate_cell_list_GPRS
 *   --------------
 *
 *   Description: This function is used compile a candidate cell list. It uses
 *   -----------  the rules defined in GSM05.08 10.1.3 to add cells that have a
 *                C1 value that is >= 0.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  No. of entries in the newly generated candidate cell list
 *   --------
 *
 ****************************************************************************/
int rr_gprs_build_candidate_cell_list_GPRS(
  const gas_id_t gas_id
)
{
  int                           index;
  rr_internal_band_T            band;
  boolean                       cell_on_supported_band = TRUE;  
  int                           candidate_index = 0;
  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
  gprs_better_cell_list_T      *local_better_cell_list_ptr = rr_get_better_cell_list_ptr(gas_id);

  /*Zero the target cell list*/
  local_better_cell_list_ptr->no_of_entries = 0;

  /*Build the better cell list in preparation for reselection*/
  for(index=0;
      (index < local_gprs_reselection_list_ptr->no_of_entries) && (index < RR_GPRS_RESELECTION_STRUCTURE_SIZE);
      index++)
  {
    /*First step is to only add to candidate cell list those entries with C1 > 0*/
    /* and check, if the cell is on a supported Band 

     There is a possibility that due to Band Coexistence, the ARFCN on which the Cell is
	 Temporarily Unsupported */
     band = GET_ARFCN_BAND(local_gprs_reselection_list_ptr->surrounding_values[index].BCCH_ARFCN);
     if (rr_gapi_is_supported_band((sys_band_T)band, gas_id) == FALSE)
     {
        MSG_GERAN_MED_1_G("Cell is on an unsupported band(%d)", (int)band);
        cell_on_supported_band = FALSE;
     }

    /* TODO: Don't have access to neighbour cell's PLMN, as assume same as serving. */
    if ( (local_gprs_reselection_list_ptr->surrounding_values[index].C1 > 0 || rr_allow_negative_c1(rr_gprs_get_scell_plmn_id(gas_id), gas_id)) &&
          cell_on_supported_band )
    {
      local_better_cell_list_ptr->candidate_cell[candidate_index].cell_reselection_attempted = FALSE;

      /*Load the last measurements*/
      local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data =
                        local_gprs_reselection_list_ptr->surrounding_values[index];
      /*Load the data into the better cell list then add the C2 value with Cell Reselection Hysteresis*/
      local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data_with_CRH =
                        local_gprs_reselection_list_ptr->surrounding_values[index];
      /*Change the C32 and possibly C31 values (if C31_HYST is set on PBCCH)*/
      rr_gprs_compute_C32_with_CRH(
        &local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data,
        &local_better_cell_list_ptr->candidate_cell[candidate_index].reselection_data_with_CRH,
        FALSE,  /*Reselection is not due to better cell, apply no recent resel hyst*/
        gas_id
      );

      candidate_index++;
      local_better_cell_list_ptr->no_of_entries++;
    }
    else
    {
      MSG_GERAN_MED_1_G("NCELL ARFCN %d has C1 < 0",CONVERT_FROM_ARFCN_T(local_gprs_reselection_list_ptr->surrounding_values[index].BCCH_ARFCN));
    }
  }

  /*Check to make sure that there are entries in the candidate cell list after
    examining all the data*/
  if (local_better_cell_list_ptr->no_of_entries == 0)
  {
    MSG_GERAN_MED_0_G("No ncells have C1 > 0; No candidates in cell list");
  }

  return (int)local_better_cell_list_ptr->no_of_entries;
}

/****************************************************************************
 *   Function name:  rr_gprs_check_for_better_ncell
 *   --------------
 *
 *   Description: This function checks the measurements of the ncells over the
 *   -----------  last path loss period (5secs) and checks to see if a better
 *                cell or cells are available. If they are a list of better cells
 *                (candidate list) is compiled.
 *                Different algorithms must be used depending upon the multiframe
 *                type.
 *
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  No. of entries in the newly generated candidate cell list
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_check_for_better_ncell(
  boolean recent_reselection_flag,
  const gas_id_t gas_id
)
{
boolean output = FALSE;

    if (rr_gprs_parameters_available(gas_id) == FALSE)
  {
    output = rr_gprs_check_for_better_GSM_ncell(recent_reselection_flag, gas_id);
    if(output)
    {
      rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_C2, gas_id);
    }
  }
  else
    if(rr_gprs_parameters_available(gas_id) == TRUE)
  {
    output = rr_gprs_check_for_better_GPRS51_ncell(recent_reselection_flag, gas_id);
    if(output)
    {
      rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_C32, gas_id);
    }
  }

return output;
}

/*===========================================================================

FUNCTION      rr_is_serving_rx_lev_greater_threshold

DESCRIPTION   Gets the  serving threshold value.

DEPENDENCIES  None

RETURN VALUE  VOID

SIDE EFFECTS  None

===========================================================================*/
static boolean rr_is_serving_rx_lev_greater_threshold(int8 scell_threshold_db,
       const gas_id_t gas_id)
{
  uint8 scell_threshold_rxlev = scell_threshold_db + 111;

  gprs_reselection_structure_T *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  MSG_GERAN_HIGH_2_G("scell_threshold_db=%d scell_threshold_rxlev=%d",
                     scell_threshold_db, scell_threshold_rxlev);

  if ((local_gprs_reselection_list_ptr->serving_values_valid) &&
      (local_gprs_reselection_list_ptr->serving_values.RXLEV_average >= scell_threshold_rxlev))
  {   
    MSG_GERAN_HIGH_1_G("scell rxlev=%d (greater than threshold)",
                       (int)local_gprs_reselection_list_ptr->serving_values.RXLEV_average);

    return TRUE;
  }   

  MSG_GERAN_HIGH_2_G("scell rxlev=%d (valid=%d)",
                     (int)local_gprs_reselection_list_ptr->serving_values.RXLEV_average,
                     (int)local_gprs_reselection_list_ptr->serving_values_valid);

  return FALSE;
}



/****************************************************************************
 *   Function name:  rr_gprs_compare_serving_and_surrounding_C2
 *   --------------
 *
 *   Description: This function compares the C2 of a ncell and a scell.
 *   -----------  Hysteresis is applied as necessary, and a decision is made
 *                as to whether the scell is still the best or not.
 *
 *   Parameters: No input parameters
 *   -----------
 *
 *   Returns:  TRUE if ncell is better than the scell otherwise FALSE
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_compare_serving_and_surrounding_C2(
  int                           servingC2,
  int                           surroundingC2,
  bcch_ncell_info_t             *ptr_ncell_info,
  boolean                       recent_reselection_flag,
  LAI_T                         *ncell_lai_ptr,
  const gas_id_t                gas_id
)
{
  int CELL_RESELECT_HYSTERESIS_DB = 0;
  gprs_scell_info_t * scell_info = rr_gprs_get_scell_info(gas_id);
  boolean             same_ra = TRUE;
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;

  /*Check to see if the cells are in the same RA, this can only be done if both the cells support gprs */
  if ( (ptr_ncell_info->gprs.ra_colour != scell_info->bcch_sys_info.ra_colour) &&
        scell_info->gsm.gprs_indicator &&
        ptr_ncell_info->gprs_supported
     )
  {
    same_ra = FALSE;
  }

  if (rr_is_any_cell_selection_mode(gas_id))
  {
    /*******************************************************************/
    /* If we are in limited service then a zero value of CRH should be */
    /* applied (GSM 03.22 4.5)                                         */
    /*******************************************************************/
     CELL_RESELECT_HYSTERESIS_DB = 0;
  }
  else
  if ( same_ra &&
      rr_same_lai(&(scell_info->gsm.location_area_identification), ncell_lai_ptr) &&
      (rr_gmminfo_get_gmm_status(gas_id) != GMM_READY_STATE)
     )
  {
    /******************************************************************/
    /* no hysteresis should be used If......                          */
    /* The serving and surrounding cell are in the same Routing Area  */
    /* and location area when GMM is not in READY state               */
    /******************************************************************/
    CELL_RESELECT_HYSTERESIS_DB = 0;

  }
  else
  {
    /*********************************************************************/
    /* if the new surrounding cell is from a different registration area */
    /* or always for a GPRS MS in READY state, the C2 value for the new  */
    /* cell shall exceed the C2 value of the scell by at least the value */
    /* of CELL RESELECTION HYSTERESIS - 3GPP TS05.08 - 6.6.2             */
    /*********************************************************************/
    CELL_RESELECT_HYSTERESIS_DB = scell_info->gsm.cell_selection_parameters.CELL_RESELECT_HYSTERESIS;

  } /* different Routing Area and no_hysteresis == FALSE */

  MSG_GERAN_MED_2_G("sur C2:%d srv C2:%d", surroundingC2, servingC2);

  /*****************************************************************/
  /* If the flag indicates that a cell reselection has taken place */
  /* recently (in the last 15s as specified in GSM05.08 6.6.2)     */
  /*****************************************************************/
  if (recent_reselection_flag == TRUE)
  {
    /**********************************************************************/
    /* Make sure the CRH value used is at least 5dB. This will discourage */
    /* a reselection while the recent flag is set even if the CRH value   */
    /* in the SI parameters for the serving cell is < 5dB                 */
    /*                                                                    */
    /* If the CRH value is more than 5dB, use the value provided in       */
    /* the SI parameter.                                                  */
    /**********************************************************************/
    if (CELL_RESELECT_HYSTERESIS_DB < 5)
    {
      CELL_RESELECT_HYSTERESIS_DB = 5;
      MSG_GERAN_MED_0_G("Using a CRH value of 5dB- recent cell reselection flag is set");
    }
  } /* recent_reselection_flag set */

  if (rr_get_nv_anite_gcf_flag(as_id) == FALSE)
  {
    boolean add_crh_enabled;
    int8 add_crh_db;
    int8 add_crh_scell_threshold_db;

    add_crh_enabled = rr_nv_get_add_crh_enabled(
      &add_crh_db,                 // add_crh_db_ptr
      &add_crh_scell_threshold_db,  // add_crh_scell_thresh_db_ptr
      as_id
    );

    if (add_crh_enabled)
    {
      MSG_GERAN_HIGH_0_G("Additional CRH adjustment enabled");

      if (rr_is_serving_rx_lev_greater_threshold(add_crh_scell_threshold_db,gas_id))
      {
        uint8 old_crh = CELL_RESELECT_HYSTERESIS_DB;

        // Add the additional offset
        CELL_RESELECT_HYSTERESIS_DB += add_crh_db;

        MSG_GERAN_HIGH_2_G("C2 hysteresis adjusted from %d to %d", old_crh, CELL_RESELECT_HYSTERESIS_DB);
      }
    }
  }

  if (surroundingC2 > servingC2 + CELL_RESELECT_HYSTERESIS_DB)
  {
    /*****************************************/
    /* The surrounding cell is a better cell */
    /*****************************************/
    return TRUE;
  }

return FALSE;
} /* rr_gprs_compare_serving_and_surrounding_C2 */

/****************************************************************************
 *   Function name:  rr_gprs_compare_serving_and_surrounding_C32
 *   --------------
 *
 *   Description: This function examines the reselection data to see if the
 *   -----------  ncell is better than the scell. It uses the procedure described
 *                in GSM05.08 10.1.3
 *
 *   Parameters: pointer to scell reselection data
 *   ----------- pointer to ncell reselection data
 *               recent_reselection_flag
 *
 *   Returns:  TRUE/FALSE
 *   --------
 *
 ****************************************************************************/
boolean rr_gprs_compare_serving_and_surrounding_C32(
  gprs_reselection_values_T     *surround_data,
  gprs_reselection_values_T     *serving_data,
  boolean                       recent_reselection_flag,
  const gas_id_t                gas_id
)
{
boolean better_cell = FALSE;
gprs_reselection_values_T local_reselection_structure;

  /***********************************************/
  /* Pass the surround values in to calculate the*/
  /* C2 values with cell reslection hysteresis   */
  /* The results will be placed into the local   */
  /* structure.                                  */
  /***********************************************/
  rr_gprs_compute_C32_with_CRH(
    surround_data,
    &local_reselection_structure,
    recent_reselection_flag,
    gas_id
  );

  /*************************************************/
  /* Simply test to see if the ncells C32 value is */
  /* greater than the scells C32 value             */
  /*************************************************/

  MSG_GERAN_LOW_2_G("sur C32:%d srv C32:%d",local_reselection_structure.C32,serving_data->C32);

  if (local_reselection_structure.C32 > serving_data->C32)
  {
    better_cell = TRUE;
    MSG_GERAN_LOW_2_G("ncell is better C32n=%d C32s=%d",local_reselection_structure.C32,serving_data->C32);
  }

  return better_cell;
} /* rr_gprs_compare_serving_and_surrounding_C32 */




/****************************************************************************
 *   Function name:  rr_gprs_surround_calculate_C32
 *   -------------
 *
 *   Description: This function calculates the C32 parameter for ncells only.
 *   ------------ For scells the C32 parameter is equal to the C1 parameter.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  int C32
 *   --------
 *
 ****************************************************************************/
int rr_gprs_surround_calculate_C32(
  int C1,
  int GPRS_TEMPORARY_OFFSET,
  int GPRS_RESELECT_OFFSET,
  int PRIORITY_CLASSs,
  int PRIORITY_CLASSn,
  rr_timer_id_T penalty_timer_id,
  byte PENALTY_TIME,
  boolean strongest_RLA_P,
  const gas_id_t gas_id
)
{
int C32;
int GPRS_TEMPORARY_OFFSET_dB;

  /*********************************************************/
  /*Calculate the value of TEMPORARY_OFFSET that should be */
  /*applied to the equation to calculate C32               */
  /*********************************************************/
  /* if the penalty timer for the current cell is running */
  if (rr_timer_is_running(penalty_timer_id, gas_id))
  {
    if (GPRS_TEMPORARY_OFFSET == 7)
    {
      /*Use an infinite temporary offset; 1000 is set to indicate infinity*/
      GPRS_TEMPORARY_OFFSET_dB = 1000;
    }
    else
    {
      /*Convert the GPRS_TEMPORARY_OFFSET to a corresponding dBm value*/
      GPRS_TEMPORARY_OFFSET_dB = (GPRS_TEMPORARY_OFFSET*10);
    }
  }
  else /* otherwise, the penalty timer is not running */
  {
    /*the temporary offset no longer applies*/
    GPRS_TEMPORARY_OFFSET_dB = 0;
  }

  if (PENALTY_TIME == 31)
  {
  /**************************************************/
  /* We are on BCCH and using C32 reselection as a  */
  /* PMO or PCCO has been recieved giving GPRS      */
  /* parameters for 1 or more cells                 */
  /**************************************************/
    /*The HCS_STRUCT for scell is not valid - we are on BCCH. Also PENALTY TIME == 31*/
    C32 = C1 - GPRS_RESELECT_OFFSET_dB[GPRS_RESELECT_OFFSET];
  }
  else
  {
    /*The HCS_STRUCT for scell is not valid - we are on BCCH. The PENALTY TIMER is not 31*/
    C32 = C1 + GPRS_RESELECT_OFFSET_dB[GPRS_RESELECT_OFFSET] - GPRS_TEMPORARY_OFFSET_dB;
  }

return C32;
}

/****************************************************************************
 *   Function name:  rr_get_gprs_reselection_list_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to gprs_reselection_list.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  gprs_reselection_structure_T *
 *   --------
 *
 ****************************************************************************/
gprs_reselection_structure_T *rr_get_gprs_reselection_list_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &gprs_reselection_list[as_index];
}

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
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &better_cell_list[as_index];
}

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
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &best_six_ncells[as_index];
}

#ifdef FEATURE_LTE

int rr_resel_greatest_gsm_s_value(
  boolean include_serving_cell,
  const gas_id_t gas_id
)
{
  gprs_better_cell_list_T      * better_cell_list = rr_get_better_cell_list_ptr(gas_id);
  int                            greatest_s_value = 0;
  uint32                         i;

  if (include_serving_cell)
  {
    gprs_reselection_structure_T * gprs_reselection_list = rr_get_gprs_reselection_list_ptr(gas_id);
    if (gprs_reselection_list->serving_values_valid)
    {
      greatest_s_value = gprs_reselection_list->serving_values.C1;
      MSG_GERAN_HIGH_1_G("Serving cell C1 (S_GSM): %d", gprs_reselection_list->serving_values.C1);
    }
  }

  for (i = 0;
       i < better_cell_list->no_of_entries && i < MAX_NUMBER_OF_BEST_CELLS;
       ++i)
  {
    if (better_cell_list->candidate_cell[i].reselection_data.C1 > greatest_s_value)
    {
      greatest_s_value = better_cell_list->candidate_cell[i].reselection_data.C1;
    }
  }

  MSG_GERAN_HIGH_1_G("Highest C1 (S_GSM) value (scell/ncells): %d", greatest_s_value);

  return greatest_s_value;
}

#endif /* FEATURE_LTE */

/* EOF */

