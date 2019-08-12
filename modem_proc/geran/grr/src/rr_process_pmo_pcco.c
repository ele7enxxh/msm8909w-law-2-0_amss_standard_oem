/*============================================================================
  rr_process_pmo_pcco.c

  The functionality in this module is related to decoding & processing of
  PMO & PCCO messages

                Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_process_pmo_pcco.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_process_pmo_pcco.h"
#include "rr_sys_info_pcell.h"
#include "rr_sys_info_ncell.h"
#include "gprs_mem.h"
#include "rr_general.h"
#include "rr_multi_sim.h"

#include "rr_decode_ie.h"
#include "rr_gprs_meas_rpt_control.h"
#include "rr_defs.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


#define DEFAULT_GPRS_RESELECT_OFFSET_INDEX      0x10

/* DRX TIMER lookup: see 3GPP 44.060 12.24 GPRS Cell Options */
static const uint8 DRX_TIMER_MAX[8] = {0, 1, 2, 4, 8, 16, 32, 64};

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* Definitions / Types required for PCCO / PMO processing */

#define PARAM_MASK_GPRS_RXLEV_ACCESS_MIN (1 << 0)
#define PARAM_MASK_GPRS_MS_TXPWR_MAX_CCH (1 << 1)
#define PARAM_MASK_GPRS_TEMPORARY_OFFSET (1 << 2)
#define PARAM_MASK_GPRS_PENALTY_TIME     (1 << 3)
#define PARAM_MASK_GPRS_RESELECT_OFFSET  (1 << 4)
#define PARAM_MASK_HCS_PARAMS            (1 << 5)
#define PARAM_MASK_SI13_PBCCH_LOCATION   (1 << 6)
#define PARAM_MASK_ALL_PARAMS_OMITTED    (1 << 7)  /* No cell selection structure was included */

typedef struct
{
  uint8                         instance_num;   // for PMO only
  ARFCN_T                       bcch_arfcn;
  BSIC_T                        bsic;
  gprs_cell_selection_params_t  cell_sel_params;
  uint16                        params_not_present_mask;

} pmo_pcco_ncell_info_t;

#define PMO_PCCO_MAX_NUM_OF_ADDED_NCELLS    10
#define PMO_PCCO_MAX_NUM_OF_REMOVED_NCELLS  32

typedef struct
{
  uint8                  num_of_added_ncells;
  pmo_pcco_ncell_info_t  added_ncells_info[PMO_PCCO_MAX_NUM_OF_ADDED_NCELLS];
  uint8                  num_of_removed_ncells;
  uint8                  removed_ncells[PMO_PCCO_MAX_NUM_OF_REMOVED_NCELLS];

} pmo_pcco_nc_freq_list_t;



typedef struct
{
  // Allocate the storage for PCCO/PMO when the message arrives
  pmo_pcco_data_t         * pcco_store_ptr;
  boolean                   pcco_nc_freq_list_valid;
  pmo_pcco_nc_freq_list_t * pcco_nc_freq_list;

  pmo_pcco_data_t         * pmo_store_ptr;
  boolean                   pmo_nc_freq_list_valid;
  pmo_pcco_nc_freq_list_t * pmo_nc_freq_list;

} pmo_pcco_store_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static pmo_pcco_store_t  pmo_pcco_store[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/**
  @brief Return a pointer to the module data

  @return
*/
static pmo_pcco_store_t *rr_process_pmo_pcco_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &pmo_pcco_store[as_index];

} /* end rr_gprs_get_pmo_pcco_store_ptr() */

/**
  @brief Returns a pointer to the PCCO data store

  @return Pointer to PCCO data store
*/
static pmo_pcco_data_t *rr_process_pmo_pcco_get_pcco_data_ptr(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  pmo_pcco_store_t *rr_process_pmo_pcco_data_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_process_pmo_pcco_data_ptr);

  return (rr_process_pmo_pcco_data_ptr->pcco_store_ptr);

} /* end rr_gprs_get_pmo_pcco_store_ptr() */


/**
  @brief Returns a pointer to the PMO data store

  @return Pointer to PMO data store
*/
static pmo_pcco_data_t *rr_process_pmo_pcco_get_pmo_data_ptr(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  pmo_pcco_store_t *rr_process_pmo_pcco_data_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_process_pmo_pcco_data_ptr);

  return (rr_process_pmo_pcco_data_ptr->pmo_store_ptr);

} /* end rr_gprs_get_pmo_pcco_store_ptr() */

pmo_pcco_data_t *rr_psi_get_pmo_store_ptr(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  pmo_pcco_store_t *rr_process_pmo_pcco_data_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_process_pmo_pcco_data_ptr);

  return (rr_process_pmo_pcco_data_ptr->pmo_store_ptr);


} /* end rr_gprs_get_pmo_pcco_store_ptr() */


/**
  @brief Allocates memory to store PCCO data and clears it

  @return Pointer to PCCO data store
*/
static pmo_pcco_data_t *rr_psi_prepare_for_pcco_data(
  pmo_pcco_store_t *pmo_pcco_store_ptr
)
{
  RR_NULL_CHECK_RETURN_PARAM(pmo_pcco_store_ptr, NULL);

  /* attempt to allocate and zero-init the required memory */
  pmo_pcco_store_ptr->pcco_store_ptr = GPRS_MEM_CALLOC(sizeof(pmo_pcco_data_t));

  if (pmo_pcco_store_ptr->pcco_store_ptr == NULL)
  {
    ERR_GERAN_FATAL_0("pcco_store_ptr alloc failed");
  }

  return pmo_pcco_store_ptr->pcco_store_ptr;

} /* end rr_psi_prepare_for_pcco_data() */


/**
  @brief Allocates memory to store PMO data and clears it

  @return Pointer to PMO data store
*/
static pmo_pcco_data_t *rr_psi_prepare_for_pmo_data(
  pmo_pcco_store_t *pmo_pcco_store_ptr
)
{
  RR_NULL_CHECK_RETURN_PARAM(pmo_pcco_store_ptr, NULL);


  /* attempt to allocate and zero-init the required memory */
  if (pmo_pcco_store_ptr->pmo_store_ptr == NULL)
  {
    pmo_pcco_store_ptr->pmo_store_ptr = GPRS_MEM_CALLOC(sizeof(pmo_pcco_data_t));

    if (pmo_pcco_store_ptr->pmo_store_ptr == NULL)
    {
      ERR_GERAN_FATAL_0("pmo_store_ptr alloc failed");
    }
  }

  return pmo_pcco_store_ptr->pmo_store_ptr;

} /* end rr_psi_prepare_for_pmo_data() */


/**
  @brief Clears the PMO data
*/
static void rr_psi_clear_pmo_data(const gas_id_t gas_id)
{
  uint32 i;
  pmo_pcco_store_t *pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  pmo_pcco_data_t *pmo_store_ptr = rr_process_pmo_pcco_get_pmo_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(pmo_pcco_store_ptr);
  RR_NULL_CHECK_FATAL(pmo_store_ptr);

  for (i = 0; i < MAX_PMO_INSTANCES; ++i)
  {
    if (pmo_store_ptr->priority_and_eutran_params.utran_priority_params[i] != NULL)
    {
      GPRS_MEM_FREE(pmo_store_ptr->priority_and_eutran_params.utran_priority_params[i]);
    }
    if (pmo_store_ptr->priority_and_eutran_params.eutran_params[i] != NULL)
    {
      GPRS_MEM_FREE(pmo_store_ptr->priority_and_eutran_params.eutran_params[i]);
    }
  }

  memset(pmo_store_ptr, 0, sizeof(pmo_pcco_data_t));

  pmo_pcco_store_ptr->pmo_nc_freq_list_valid = FALSE;
  if (pmo_pcco_store_ptr->pmo_nc_freq_list != NULL)
  {
    GPRS_MEM_FREE(pmo_pcco_store_ptr->pmo_nc_freq_list);
    pmo_pcco_store_ptr->pmo_nc_freq_list = NULL;
  }

} /* end rr_psi_clear_pmo_data() */

/**
  @brief Prepare for PCCO instances

  @param (in) msg_count number of PCCO instances expected
  @param (out) instances_expected_mask
*/
static uint8 prepare_for_new_msg_instances(
  uint8 msg_count,
  uint16 *instances_expected_mask
)
{
  uint8   num_of_indexes;
  uint16  expected_mask;

  /* PSIn_COUNT indicates range of PSIn_INDEX e.g. PSIn_COUNT = 2 => PSIn_INDEX = 0 -> 2 */
  num_of_indexes = msg_count + 1;

  /* expected_mask: bit mask indicating which PSIn_INDEX values are required */
  /* e.g. PSIn_COUNT = 2 => expected mask = 0000 0111 b */
  expected_mask = ~ (uint16) ( 0xFFFFU << num_of_indexes );

  /* bit 1=instance expected, 0=instance not required / has been received */
  (* instances_expected_mask) = expected_mask;

  return num_of_indexes;

} /* prepare_for_new_msg_instances() */

#ifdef FEATURE_LTE
/**
  @brief Converts a CSN.1 Individual Priorities IE in to an RR Individual
         Priorities IE.

  @param dst the variable in which to store the RR Individual Priorities IE.
  @param src the variable that contains the source CSN.1 Individal Priorities IE.
*/
static void rr_convert_csn_individual_priorities_ie(
  rr_individual_priorities_ie_t * dst,
  csn_individual_priorities_ie_t * src
)
{
  RR_NULL_CHECK_RETURN_VOID(dst);
  RR_NULL_CHECK_RETURN_VOID(src);

  dst->geran_priority = src->geran_priority;
  if (src->utran_valid)
  {
    dst->utran_individual_priority_parameters_description = GPRS_MEM_CALLOC(sizeof(rr_utran_individual_priority_parameters_description_t));
    if (dst->utran_individual_priority_parameters_description != NULL)
    {
      uint32 i;

      if (src->utran.default_priority_valid)
      {
        dst->utran_individual_priority_parameters_description->default_utran_priority_valid = TRUE;
        dst->utran_individual_priority_parameters_description->default_utran_priority = src->utran.default_priority;
      }

      for (i = 0;
           i < src->utran.uarfcn_count &&
           i < CSN_INDIVIDUAL_PRIORITIES_MAX_UARFCNS;
           ++i)
      {
        if (src->utran.uarfcns[i].type == CSN_UTRAN_TYPE_FDD)
        {
          if (dst->utran_individual_priority_parameters_description->num_utra_fdd_freq < LTE_IRAT_MAX_UTRAN_FREQ)
          {
            lte_irat_reselection_priority_s entry;

            entry.arfcn = src->utran.uarfcns[i].uarfcn;
            entry.priority = src->utran.uarfcns[i].priority;
            entry.band = 0;  /* zero band to ensure the whole struct is initialised */

            dst->utran_individual_priority_parameters_description->utra_fdd_freq_list[dst->utran_individual_priority_parameters_description->num_utra_fdd_freq] = entry;
            ++dst->utran_individual_priority_parameters_description->num_utra_fdd_freq;
          }
          else
          {
            MSG_GERAN_ERROR_1("Overflow adding UTRAN TDD entry %d", i);
          }
        }
        else if (src->utran.uarfcns[i].type == CSN_UTRAN_TYPE_TDD)
        {
          if (dst->utran_individual_priority_parameters_description->num_utra_tdd_freq < LTE_IRAT_MAX_UTRAN_FREQ)
          {
            lte_irat_reselection_priority_s entry;

            entry.arfcn = src->utran.uarfcns[i].uarfcn;
            entry.priority = src->utran.uarfcns[i].priority;
            entry.band = 0;  /* zero band to ensure the whole struct is initialised */

            dst->utran_individual_priority_parameters_description->utra_tdd_freq_list[dst->utran_individual_priority_parameters_description->num_utra_tdd_freq] = entry;
            ++dst->utran_individual_priority_parameters_description->num_utra_tdd_freq;
          }
          else
          {
            MSG_GERAN_ERROR_1("Overflow adding UTRAN TDD entry %d", i);
          }
        }
        else
        {
          MSG_GERAN_ERROR_2("Unknown UTRAN type %d at index %d", src->utran.uarfcns[i].type, i);
        }
      }
    }
  }

  if (src->eutran_valid)
  {
    dst->eutran_individual_priority_parameters_description = GPRS_MEM_MALLOC(sizeof(rr_eutran_individual_priority_parameters_description_t));
    if (dst->eutran_individual_priority_parameters_description != NULL)
    {
      uint32 i;

      if (src->eutran.default_priority_valid)
      {
        dst->eutran_individual_priority_parameters_description->default_eutran_priority_valid = TRUE;
        dst->eutran_individual_priority_parameters_description->default_eutran_priority = src->eutran.default_priority;
      }

      for (i = 0;
           i < src->eutran.earfcn_count &&
           i < CSN_INDIVIDUAL_PRIORITIES_MAX_EARFCNS;
           ++i)
      {
        if (dst->eutran_individual_priority_parameters_description->num_eutra_freq < LTE_IRAT_MAX_EUTRAN_FREQ)
        {
          lte_irat_reselection_priority_s entry;

          entry.arfcn = src->eutran.earfcns[i].earfcn;
          entry.priority = src->eutran.earfcns[i].priority;
          entry.band = 0;  /* zero band to ensure the whole struct is initialised */

          dst->eutran_individual_priority_parameters_description->eutra_freq_list[dst->eutran_individual_priority_parameters_description->num_eutra_freq] = entry;
          ++dst->eutran_individual_priority_parameters_description->num_eutra_freq;
        }
        else
        {
          MSG_GERAN_ERROR_1("Overflow adding E-UTRAN entry %d", i);
        }
      }
    }
  }

  if (src->t3230_timeout_value_valid)
  {
    dst->t3230_timeout_value_valid = TRUE;
    dst->t3230_timeout_value = src->t3230_timeout_value;
  }
}
#endif /* FEATURE_LTE */

/**
  @brief Decodes the cell selection parameters from a PMO/PCCO message

  @param (in) pcco_pmo_cell_sel_params
  @param (out) decoded_cell_sel_params

  @return params_not_present_mask
*/
static uint16 decode_pmo_pcco_cell_sel_params(
  cell_sel_params_t * pcco_pmo_cell_sel_params,
  gprs_cell_selection_params_t * decoded_cell_sel_params
)
{
  uint16  params_not_present_mask = 0;

  decoded_cell_sel_params->cell_bar_access_2 = pcco_pmo_cell_sel_params->cell_bar_access2;
  decoded_cell_sel_params->exc_acc = pcco_pmo_cell_sel_params->exc_acc;
  decoded_cell_sel_params->same_ra_as_serving_cell = pcco_pmo_cell_sel_params->same_ra_as_serv_cell;

  if ( pcco_pmo_cell_sel_params->option_bitmap & GPRS_RXLEV_VALID )
  {
    decoded_cell_sel_params->gprs_rxlev_access_min = pcco_pmo_cell_sel_params->gprs_rxlev_access_min;
    decoded_cell_sel_params->gprs_ms_txpwr_max_cch = pcco_pmo_cell_sel_params->gprs_ms_txpwr_max_cch;
  }
  else
  {
    params_not_present_mask |= PARAM_MASK_GPRS_RXLEV_ACCESS_MIN | PARAM_MASK_GPRS_MS_TXPWR_MAX_CCH;
  }

  if ( pcco_pmo_cell_sel_params->option_bitmap & GPRS_TEMP_OFFSET_VALID )
  {
    decoded_cell_sel_params->gprs_temporary_offset = pcco_pmo_cell_sel_params->gprs_temp_offset;
    decoded_cell_sel_params->gprs_penalty_time = pcco_pmo_cell_sel_params->gprs_penalty_time;
  }
  else
  {
    params_not_present_mask |= PARAM_MASK_GPRS_TEMPORARY_OFFSET | PARAM_MASK_GPRS_PENALTY_TIME;
  }

  if ( pcco_pmo_cell_sel_params->option_bitmap & GPRS_RESELECT_OFFEST_VALID )
  {
    decoded_cell_sel_params->gprs_reselect_offset = pcco_pmo_cell_sel_params->gprs_reselect_offset;
  }
  else
  {
    params_not_present_mask |= PARAM_MASK_GPRS_RESELECT_OFFSET;
  }

  if ( pcco_pmo_cell_sel_params->option_bitmap & HCS_VALID )
  {
    decoded_cell_sel_params->hcs_params.hcs_thr = pcco_pmo_cell_sel_params->hcs_params.hcs_thr;
    decoded_cell_sel_params->hcs_params.priority_class = pcco_pmo_cell_sel_params->hcs_params.priority_class;
    decoded_cell_sel_params->hcs_params_present = TRUE;
  }
  else
  {
    params_not_present_mask |= PARAM_MASK_HCS_PARAMS;
  }

  if ( pcco_pmo_cell_sel_params->option_bitmap & SI13_PBCCH_VALID )
  {
    if ( pcco_pmo_cell_sel_params->si13_pbcch_loc.si13_loc_flag == si13_loc_valid )
    {
      if (pcco_pmo_cell_sel_params->si13_pbcch_loc.si13_location == 0)
      {
        decoded_cell_sel_params->si13_location = BCCH_NORMAL;
      }
      else
      {
        decoded_cell_sel_params->si13_location = BCCH_EXTENDED;
      }
    }
    else
    {
      params_not_present_mask |= PARAM_MASK_SI13_PBCCH_LOCATION;
    }
  }
  else
  {
    params_not_present_mask |= PARAM_MASK_SI13_PBCCH_LOCATION;
  }

  return params_not_present_mask;

} /* end decode_pmo_pcco_cell_sel_params() */


/**
  @brief Decodes the cell selection parameters from a PMO/PCCO message

  @param (in) parameters_not_present_mask
  @param (in) ref_cell_sel_params
  @param (out) cell_sel_params
*/
static void process_pmo_pcco_cell_sel_params(
  uint16 parameters_not_present_mask,
  gprs_cell_selection_params_t * ref_cell_sel_params,
  gprs_cell_selection_params_t * cell_sel_params,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  /*
  If some GPRS Cell Selection paramters are omitted, then the values used in the supplied
  reference parameters are used. If these are not supplied, then defaults are used
  (as specified in 3GPP 44.060 Table 11.2.9b.2 : Packet Measurement Order information element details)
  */
  if ( parameters_not_present_mask & PARAM_MASK_GPRS_RXLEV_ACCESS_MIN )
  {
    if ( ref_cell_sel_params )
    {
      cell_sel_params->gprs_rxlev_access_min = ref_cell_sel_params->gprs_rxlev_access_min;
    }
    else
    {
      cell_sel_params->gprs_rxlev_access_min = scell_info_ptr->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN;
    }
  }

  if ( parameters_not_present_mask & PARAM_MASK_GPRS_MS_TXPWR_MAX_CCH )
  {
    if ( ref_cell_sel_params )
    {
      cell_sel_params->gprs_ms_txpwr_max_cch = ref_cell_sel_params->gprs_ms_txpwr_max_cch;
    }
    else
    {
      cell_sel_params->gprs_ms_txpwr_max_cch = scell_info_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH;
    }
  }

  if ( parameters_not_present_mask & PARAM_MASK_GPRS_TEMPORARY_OFFSET )
  {
    if ( ref_cell_sel_params )
    {
      cell_sel_params->gprs_temporary_offset = ref_cell_sel_params->gprs_temporary_offset;
    }
    else
    {
      /* GPRS_TEMP_OFFSET has no equivalent in scell params - set to zero */
      cell_sel_params->gprs_temporary_offset = 0;
    }
  }

  if ( parameters_not_present_mask & PARAM_MASK_GPRS_PENALTY_TIME )
  {
    if ( ref_cell_sel_params )
    {
      cell_sel_params->gprs_penalty_time = ref_cell_sel_params->gprs_penalty_time;
    }
    else
    {
      /* GPRS_PENALTY_TIME has no equivalent in scell params - set to zero */
      cell_sel_params->gprs_penalty_time = 0;
    }
  }

  if ( parameters_not_present_mask & PARAM_MASK_GPRS_RESELECT_OFFSET )
  {
    if ( ref_cell_sel_params )
    {
      cell_sel_params->gprs_reselect_offset = ref_cell_sel_params->gprs_reselect_offset;
    }
    else
    {
      /* the default value for the GPRS RESELECT OFFSET is 0dB, but the  */
      /* parameter actually represents an index into an array of offsets */
      /* offset 0 is actually index 16 - see 3GPP 44.060 Table 11.2.20.2 */
      cell_sel_params->gprs_reselect_offset = DEFAULT_GPRS_RESELECT_OFFSET_INDEX;
    }
  }

  if ( parameters_not_present_mask & PARAM_MASK_HCS_PARAMS )
  {
    if ( ref_cell_sel_params )
    {
      cell_sel_params->hcs_params_present = ref_cell_sel_params->hcs_params_present;
      cell_sel_params->hcs_params = ref_cell_sel_params->hcs_params;
    }
    else
    {
      // HCS parameters structure not present equate to "infinity"
      cell_sel_params->hcs_params_present = FALSE;
    }
  }

  if ( parameters_not_present_mask & PARAM_MASK_SI13_PBCCH_LOCATION )
  {
    if ( ref_cell_sel_params )
    {
      cell_sel_params->si13_location_present = ref_cell_sel_params->si13_location_present;
      cell_sel_params->si13_location = ref_cell_sel_params->si13_location;
    }
    else
    {
      cell_sel_params->si13_location_present = FALSE;
    }
  }

} /* end process_pmo_pcco_cell_sel_params() */


/**
  @brief This function amends the ncell frequency list from data supplied in a PCCO or PMO

  @param (in) num_of_added_ncells
  @param (in) added_ncells_info
  @param (in) num_of_removed_ncells
  @param (in) removed_ncells

  @return TRUE if BA(GPRS) list changed
*/
static boolean apply_nc_freq_list_deltas(
  uint8 num_of_added_ncells,
  pmo_pcco_ncell_info_t *added_ncells_info,
  uint8 num_of_removed_ncells,
  uint8 *removed_ncells,
  const gas_id_t gas_id
)
{
  boolean ba_changed = FALSE;
  uint8   i;

  MSG_GERAN_MED_2_G("apply_nc_freq_list_deltas(num_of_added_ncells: %d, num_of_removed_ncells: %d",
          num_of_added_ncells, num_of_removed_ncells);

  if ( num_of_removed_ncells > 0 )
  {
    for ( i = 0; (i < num_of_removed_ncells) && (i < PMO_PCCO_MAX_NUM_OF_REMOVED_NCELLS); i++ )
    {
      rr_gprs_remove_pmo_ncell_from_ba_gprs(
        removed_ncells[i],
        gas_id
      );
    }

    ba_changed = TRUE;
  }

  if ( num_of_added_ncells > 0 )
  {
    pmo_pcco_ncell_info_t         *added_ncell_info;
    gprs_cell_selection_params_t  *ref_cell_sel_params;
    gprs_scell_info_t             *scell_info = rr_gprs_get_scell_info(gas_id);
    boolean                        bHCS_scell = FALSE;
    boolean                        bHCS_ncell = FALSE;
    boolean                        bServing;

    /* Point to first ncell in the added ncell list */
    added_ncell_info = &added_ncells_info[0];

    /* For the first ncell in the list, there are no reference GPRS cell selection */
    /* parameters - subsequent ncells use the parameters from the previous ncell */
    ref_cell_sel_params = NULL;

    /* Now loop thru and add the ncells, using the GPRS cell selection */
    /* parameters of the previous ncell as a reference */
    for ( i = 0; i < num_of_added_ncells; i++ )
    {

      /* Only if the cell is not in the BA list already and not just extra scell params is the BA changed */
      if ((rr_gprs_get_ncell_info(
             added_ncell_info->bcch_arfcn,
             &added_ncell_info->bsic,
             gas_id
           ) == NULL) &&
          !ARFCNS_EQUAL( scell_info->gsm.BCCH_ARFCN, added_ncell_info->bcch_arfcn) &&
          !( scell_info->gsm.BSIC.BS_colour_code == added_ncell_info->bsic.BS_colour_code) &&
          !( scell_info->gsm.BSIC.PLMN_colour_code == added_ncell_info->bsic.PLMN_colour_code))
      {
        ba_changed = TRUE;
      }

      process_pmo_pcco_cell_sel_params(
        added_ncell_info->params_not_present_mask,
        ref_cell_sel_params,
        &added_ncell_info->cell_sel_params,
        gas_id
      );

      bServing = rr_gprs_add_pmo_ncell_to_ba_gprs(
        added_ncell_info->bcch_arfcn,
        &added_ncell_info->bsic,
        &added_ncell_info->cell_sel_params,
        gas_id
      );

      if (((added_ncell_info->params_not_present_mask & PARAM_MASK_HCS_PARAMS) == 0) &&
          (added_ncell_info->params_not_present_mask != PARAM_MASK_ALL_PARAMS_OMITTED))
      {
        /* HCS parameters WERE present in this added cell */
        if (bServing)
        {
          bHCS_scell = TRUE;
        }
        else
        {
          bHCS_ncell = TRUE;
        }
      }

      ref_cell_sel_params = &added_ncell_info->cell_sel_params;
      added_ncell_info++;
    } /* end for */

    if ((bHCS_scell == FALSE) && (bHCS_ncell == TRUE) && rr_gprs_camped_on_ccch(gas_id))
    {
      /* At least one of the added cells is using HCS parameters, but the BCCH  */
      /* serving cell is NOT one of them, so update the serving cell with dummy */
      /* values for HCS_THR and PRIORITY_CLASS. See 3GPP 05.08 Section Table 3a */
      scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params_present = TRUE;
      scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params.hcs_thr = INVALID_HCS_THRESHOLD;
      scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params.priority_class = 0;
      scell_info->bcch_sys_info.pmo_scell_params_valid = TRUE;
      MSG_GERAN_HIGH_0_G("PMO requires HCS params for serving cell");
    }
  }

  return ba_changed;

} /* end apply_nc_freq_list_deltas() */


/**
  @brief Copies the source GPRS Measurement Parameters in to the
         given modifications store

  @param (out) dst
  @param (in) src
*/
static void rr_gprs_meas_params_merge_csn_mods(
  gprs_meas_params_mods_t *dst,
  csn_gprs_meas_params_t *src
)
{
  if ((dst == NULL) || (src == NULL))
  {
    return;
  }

  memset(dst, 0, sizeof(gprs_meas_params_mods_t));

  dst->valid = TRUE;

  if (src->pmo_ind_valid)
  {
    dst->pmo_ind_valid = TRUE;
    dst->pmo_ind = src->pmo_ind;
  }

  if (src->report_type_valid)
  {
    dst->report_type_valid = TRUE;
    dst->report_type = (report_type_T)src->report_type;
  }

  if (src->reporting_rate_valid)
  {
    dst->reporting_rate_valid = TRUE;
    dst->reporting_rate = src->reporting_rate;
  }

  if (src->invalid_bsic_reporting_valid)
  {
    dst->invalid_bsic_reporting_valid = TRUE;
    dst->invalid_bsic_reporting = src->invalid_bsic_reporting;
  }

  if (src->ncc_permitted_valid)
  {
    MSG_GERAN_ERROR_0("NCC_PERMITTED is unhandled!");
  }

  if (src->multiband_reporting_valid)
  {
    dst->multiband_reporting_valid = TRUE;
    dst->multiband_reporting = src->multiband_reporting;
  }

  if (src->serving_band_reporting_valid)
  {
    dst->serving_band_reporting_valid = TRUE;
    dst->serving_band_reporting = src->serving_band_reporting;
  }

  if (src->scale_ord_valid)
  {
    dst->scale_ord_valid = TRUE;
    dst->scale_ord = src->scale_ord;
  }

  if (src->reporting_900_valid)
  {
    dst->reporting_900_valid = TRUE;
    dst->reporting_900_offset = src->reporting_900_offset;
    dst->reporting_900_threshold = src->reporting_900_threshold;
  }

  if (src->reporting_1800_valid)
  {
    dst->reporting_1800_valid = TRUE;
    dst->reporting_1800_offset = src->reporting_900_offset;
    dst->reporting_1800_threshold = src->reporting_900_threshold;
  }

  if (src->reporting_450_valid)
  {
    dst->reporting_450_valid = TRUE;
    dst->reporting_450_offset = src->reporting_900_offset;
    dst->reporting_450_threshold = src->reporting_900_threshold;
  }

  if (src->reporting_1900_valid)
  {
    dst->reporting_1900_valid = TRUE;
    dst->reporting_1900_offset = src->reporting_900_offset;
    dst->reporting_1900_threshold = src->reporting_900_threshold;
  }

  if (src->reporting_850_valid)
  {
    dst->reporting_850_valid = TRUE;
    dst->reporting_850_offset = src->reporting_900_offset;
    dst->reporting_850_threshold = src->reporting_900_threshold;
  }
}


#ifdef FEATURE_WCDMA

/**
  @brief Copies 3G measurement parameters

  @param (out) dst
  @param (in) src
*/
static void rr_gprs_3g_meas_params_merge_csn_mods(
  gprs_3g_meas_params_mods_t *dst,
  csn_gprs_3g_meas_params_t *src
)
{
  if ((dst == NULL) || (src == NULL))
  {
    return;
  }

  dst->valid = TRUE;

  if (src->qsearch_p_valid)
  {
    dst->Qsearch_P_valid = TRUE;
    dst->Qsearch_P = src->qsearch_p;
  }

  if (src->fdd_gprs_qoffset_valid && src->fdd_qmin_valid)
  {
    dst->FDD_valid = TRUE;
    dst->FDD_GPRS_Qoffset = src->fdd_gprs_qoffset;
    dst->FDD_Qmin = src->fdd_qmin;
  }

  if (src->_3g_search_prio_valid)
  {
    dst->_3G_SEARCH_PRIO_valid = TRUE;
    dst->_3G_SEARCH_PRIO = src->_3g_search_prio;
  }

  if (src->fdd_rep_quant_valid && src->fdd_multirat_reporting_valid)
  {
    dst->FDD_reporting_valid = TRUE;
    dst->FDD_REP_QUANT = src->fdd_rep_quant;
    dst->FDD_MULTIRAT_REPORTING = src->fdd_multirat_reporting;
  }

  if (src->fdd_reporting_offset_valid && src->fdd_reporting_threshold_valid)
  {
    dst->FDD_off_thresh_valid = TRUE;
    dst->FDD_REPORTING_OFFSET = src->fdd_reporting_offset;
    dst->FDD_REPORTING_THRESHOLD = src->fdd_reporting_threshold;
  }

  if (src->tdd_multirat_reporting_valid)
  {
    dst->TDD_reporting_valid = TRUE;
    dst->TDD_MULTIRAT_REPORTING = src->tdd_multirat_reporting;
  }

  if (src->tdd_reporting_offset_valid && src->tdd_reporting_threshold_valid)
  {
    dst->TDD_off_thresh_valid = TRUE;
    dst->TDD_REPORTING_OFFSET = src->tdd_reporting_offset;
    dst->TDD_REPORTING_THRESHOLD = src->tdd_reporting_threshold;
  }

  if (src->cdma2000_multirat_reporting_valid)
  {
    dst->CDMA2000_reporting_valid = TRUE;
    dst->CDMA2000_MULTIRAT_REPORTING = src->cdma2000_multirat_reporting;
  }

  if (src->cdma2000_reporting_offset_valid && src->cdma2000_reporting_threshold_valid)
  {
    dst->CDMA2000_off_thresh_valid = TRUE;
    dst->CDMA2000_REPORTING_OFFSET = src->cdma2000_reporting_offset;
    dst->CDMA2000_REPORTING_THRESHOLD = src->cdma2000_reporting_threshold;
  }
}

#endif /* FEATURE_WCDMA */


/*-----------------------------------------------------------------------------
 * Externalized Function Definitions
 * --------------------------------------------------------------------------*/

#ifdef FEATURE_WCDMA

/**
  @brief See rr_process_pmo_pcco.h
*/

gprs_3g_meas_params_mods_t *rr_gprs_get_pmo_gprs_3g_meas_params(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the PMO data
  pmo_pcco_data_t *pmo_store_ptr = rr_process_pmo_pcco_get_pmo_data_ptr(gas_id);

  if (pmo_store_ptr != NULL)
  {
    return(&(pmo_store_ptr->gprs_3g_meas_params));
  }

  return(NULL);
}

/**
  @brief See rr_process_pmo_pcco.h
*/
gprs_3g_meas_params_mods_t *rr_gprs_get_pcco_gprs_3g_meas_params(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the PCCO data
  pmo_pcco_data_t *pcco_store_ptr = rr_process_pmo_pcco_get_pcco_data_ptr(gas_id);

  if (pcco_store_ptr != NULL)
  {
    return(&(pcco_store_ptr->gprs_3g_meas_params));
  }

  return(NULL);
}

#endif /* FEATURE_WCDMA */

/**
  @brief See rr_process_pmo_pcco.h
*/
nc_meas_params_mods_t *rr_gprs_get_pmo_nc_meas_params(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the PMO data
  pmo_pcco_data_t *pmo_store_ptr = rr_process_pmo_pcco_get_pmo_data_ptr(gas_id);

  if (pmo_store_ptr != NULL)
  {
    return(&(pmo_store_ptr->nc_meas_params));
  }

  return(NULL);
}

/**
  @brief See rr_process_pmo_pcco.h
*/
gprs_meas_params_mods_t *rr_gprs_get_pmo_gprs_meas_params(
  const gas_id_t gas_id
)
{
  pmo_pcco_data_t *pmo_store_ptr = rr_process_pmo_pcco_get_pmo_data_ptr(gas_id);

  if (pmo_store_ptr != NULL)
  {
    return(&(pmo_store_ptr->gprs_meas_params));
  }

  return(NULL);
}

/**
  @brief See rr_process_pmo_pcco.h
*/
nc_meas_params_mods_t *rr_gprs_get_pcco_nc_meas_params(
  const gas_id_t gas_id
)
{
  pmo_pcco_data_t *pcco_store_ptr = rr_process_pmo_pcco_get_pcco_data_ptr(gas_id);

  if (pcco_store_ptr != NULL)
  {
    return(&(pcco_store_ptr->nc_meas_params));
  }

  return(NULL);
}

/**
  @brief See rr_process_pmo_pcco.h
*/
gprs_meas_params_mods_t *rr_gprs_get_pcco_gprs_meas_params(
  const gas_id_t gas_id
)
{
  pmo_pcco_data_t *pcco_store_ptr = rr_process_pmo_pcco_get_pcco_data_ptr(gas_id);

  if (pcco_store_ptr != NULL)
  {
    return(&(pcco_store_ptr->gprs_meas_params));
  }

  return(NULL);
}

/**
  @brief See rr_process_pmo_pcco.h
*/
pmo_pcco_nacc_params_t *rr_gprs_get_pcco_nacc_params(
  const gas_id_t gas_id
)
{
  pmo_pcco_data_t *pcco_store_ptr = rr_process_pmo_pcco_get_pcco_data_ptr(gas_id);

  if (pcco_store_ptr != NULL)
  {
    return(&(pcco_store_ptr->nacc_params));
  }

  return(NULL);
}

/**
  @brief See rr_process_pmo_pcco.h
*/
pmo_pcco_nacc_params_t *rr_gprs_get_pmo_nacc_params(
  const gas_id_t gas_id
)
{
  pmo_pcco_data_t *pmo_store_ptr = rr_process_pmo_pcco_get_pmo_data_ptr(gas_id);

  if (pmo_store_ptr != NULL)
  {
    return(&(pmo_store_ptr->nacc_params));
  }

  return(NULL);
}


boolean rr_get_pmo_priority_and_eutran_params_valid(const gas_id_t gas_id)
{

  pmo_pcco_store_t *pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(pmo_pcco_store_ptr);

  if (pmo_pcco_store_ptr != NULL &&
      pmo_pcco_store_ptr->pmo_store_ptr)
  {
    if (pmo_pcco_store_ptr->pmo_store_ptr->priority_and_eutran_params_valid)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}

boolean rr_get_pmo_serving_cell_priority_params(
  csn_gsm_priority_params_ie_t * dst,
  const gas_id_t gas_id
)
{
  pmo_pcco_store_t *pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(pmo_pcco_store_ptr);

  RR_NULL_CHECK_RETURN_PARAM(dst, FALSE);

  if (pmo_pcco_store_ptr->pmo_store_ptr != NULL)
  {
    if (pmo_pcco_store_ptr->pmo_store_ptr->priority_and_eutran_params_valid &&
        pmo_pcco_store_ptr->pmo_store_ptr->priority_and_eutran_params.serving_cell_priority_params_valid)
    {
      *dst = pmo_pcco_store_ptr->pmo_store_ptr->priority_and_eutran_params.serving_cell_priority_params;
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}

boolean rr_get_pmo_utran_priority_params(
  csn_utran_priority_params_ie_t * dst,
  const gas_id_t gas_id
)
{
  pmo_pcco_store_t *pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(pmo_pcco_store_ptr);

  RR_NULL_CHECK_RETURN_PARAM(dst, FALSE);

  if (pmo_pcco_store_ptr->pmo_store_ptr != NULL)
  {
    uint32 i;
    boolean valid = FALSE;

    for (i = 0; i < MAX_PMO_INSTANCES; ++i)
    {
      if (pmo_pcco_store_ptr->pmo_store_ptr->priority_and_eutran_params.utran_priority_params_valid_bitmap & (1 << i))
      {
        csn_utran_priority_params_ie_t * src = pmo_pcco_store_ptr->pmo_store_ptr->priority_and_eutran_params.utran_priority_params[i];

        if (src == NULL)
        {
          MSG_GERAN_ERROR_1_G("src == NULL for index %d", i);
          continue;
        }

        valid = TRUE;

        rr_utran_priority_params_merge_mods(dst, src);
      }
    }

    return valid;
  }
  else
  {
    return FALSE;
  }
}

boolean rr_get_pmo_eutran_params(
  csn_eutran_params_ie_t * dst,
  const gas_id_t gas_id
)
{
  pmo_pcco_store_t *pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(pmo_pcco_store_ptr);

  RR_NULL_CHECK_RETURN_PARAM(dst, FALSE);

  if (pmo_pcco_store_ptr->pmo_store_ptr != NULL)
  {
    uint32 i;
    boolean valid = FALSE;

    for (i = 0; i < MAX_PMO_INSTANCES; ++i)
    {
      if (pmo_pcco_store_ptr->pmo_store_ptr->priority_and_eutran_params.eutran_params_valid_bitmap & (1 << i))
      {
        csn_eutran_params_ie_t * src = pmo_pcco_store_ptr->pmo_store_ptr->priority_and_eutran_params.eutran_params[i];

        if (src == NULL)
        {
          MSG_GERAN_ERROR_1_G("src == NULL for index %d", i);
          continue;
        }

        valid = TRUE;

        csn_eutran_params_merge_csn_mods(dst, src);
      }
    }

    return valid;
  }
  else
  {
    return FALSE;
  }
}

/**
  @brief See rr_process_pmo_pcco.h
*/
void rr_psi_discard_pcco_data(
  const gas_id_t gas_id
)
{
  pmo_pcco_store_t *pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);

  if (pmo_pcco_store_ptr->pcco_store_ptr != NULL)
  {
    GPRS_MEM_FREE(pmo_pcco_store_ptr->pcco_store_ptr);
    pmo_pcco_store_ptr->pcco_store_ptr = NULL;
  }

} /* end rr_psi_discard_pcco_data() */


/**
  @brief See rr_process_pmo_pcco.h
*/
void rr_psi_discard_pmo_data(
  const gas_id_t gas_id
)
{
  pmo_pcco_store_t *pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);

  if (pmo_pcco_store_ptr->pmo_store_ptr != NULL)
  {
    rr_psi_clear_pmo_data(gas_id);
    GPRS_MEM_FREE(pmo_pcco_store_ptr->pmo_store_ptr);
    pmo_pcco_store_ptr->pmo_store_ptr = NULL;
  }

} /* end rr_psi_discard_pmo_data() */


/**
  @brief See rr_process_pmo_pcco.h
*/
void rr_gprs_reset_pcco_nc_frequency_list(
  const gas_id_t gas_id
)
{
  pmo_pcco_store_t *pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);

  if (pmo_pcco_store_ptr != NULL)
  {
    pmo_pcco_store_ptr->pcco_nc_freq_list_valid = FALSE;
    if (pmo_pcco_store_ptr->pcco_nc_freq_list != NULL)
    {
      GPRS_MEM_FREE(pmo_pcco_store_ptr->pcco_nc_freq_list);
      pmo_pcco_store_ptr->pcco_nc_freq_list = NULL;
    }
  }
}

/**
  @brief See rr_process_pmo_pcco.h
*/
void rr_gprs_pcco_decode_params(
  packet_cell_change_order_t *pcco,
  const gas_id_t gas_id
)
{
  pmo_pcco_store_t *pmo_pcco_store_ptr;
  pmo_pcco_data_t *pcco_store_ptr;

  pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(pmo_pcco_store_ptr);

  pcco_store_ptr = rr_psi_prepare_for_pcco_data(pmo_pcco_store_ptr);
  RR_NULL_CHECK_FATAL(pcco_store_ptr);

  MSG_GERAN_MED_0_G("Decoding Packet Cell Change Order");

  /* before decoding the PCCO, clear the current PCCO store contents */
  memset(pcco_store_ptr, 0, sizeof(pmo_pcco_data_t));

  if (pcco->gsm_3g_cell_type == CSN_PCCO_GSM_TARGET_CELL)
  {
    pcco_store_ptr->nc_meas_params.valid = TRUE;
    pcco_store_ptr->nc_meas_params.network_control_order = pcco->cell.gsm.nc_meas_params.network_control_order;

    pcco_store_ptr->nacc_params.valid_flag = (boolean)pcco->cell.gsm.r4_additions_flag;
    pcco_store_ptr->nacc_params.ccn_active = pcco->cell.gsm.ccn_active;
    pcco_store_ptr->nacc_params.ccn_support_flag = (boolean)pcco->cell.gsm.ccn_support_flag;
    pcco_store_ptr->nacc_params.ccn_supported_desc = pcco->cell.gsm.ccn_support_desc;

    if (pcco->cell.gsm.nc_meas_params.nc_period_valid)
    {
      pcco_store_ptr->nc_meas_params.nc_period_valid = TRUE;
      pcco_store_ptr->nc_meas_params.nc_non_drx_period = pcco->cell.gsm.nc_meas_params.nc_non_drx_period;
      pcco_store_ptr->nc_meas_params.nc_reporting_period_i = pcco->cell.gsm.nc_meas_params.nc_reporting_period_i;
      pcco_store_ptr->nc_meas_params.nc_reporting_period_t = pcco->cell.gsm.nc_meas_params.nc_reporting_period_t;
    }
    else
    {
      pcco_store_ptr->nc_meas_params.nc_period_valid = FALSE;
    }

    if (pcco->cell.gsm.nc_meas_params.nc_frequency_list_valid)
    {
      pmo_pcco_store_ptr->pcco_nc_freq_list = GPRS_MEM_CALLOC(sizeof(pmo_pcco_nc_freq_list_t));
      if (pmo_pcco_store_ptr->pcco_nc_freq_list != NULL)
      {
        pmo_pcco_store_ptr->pcco_nc_freq_list_valid = TRUE;

        if (pcco->cell.gsm.nc_meas_params.nc_frequency_list.num_of_removed_freqs > 0)
        {
          uint8 i;

          for (i = 0; i < pcco->cell.gsm.nc_meas_params.nc_frequency_list.num_of_removed_freqs; i++)
          {
            /* need to concatenate each new removed frequency index to the list of removed
            frequencies accumulated so far. */
            if (i < PMO_PCCO_MAX_NUM_OF_REMOVED_NCELLS)
            {
              pmo_pcco_store_ptr->pcco_nc_freq_list->removed_ncells[i] = pcco->cell.gsm.nc_meas_params.nc_frequency_list.removed_freqs[i];
            }
            else
            {
              break;
            }
          }
          pmo_pcco_store_ptr->pcco_nc_freq_list->num_of_removed_ncells = i;
        }

        if (pcco->cell.gsm.nc_meas_params.nc_frequency_list.num_of_added_freqs > 0)
        {
          uint8 i;

          for (i = 0; i < pcco->cell.gsm.nc_meas_params.nc_frequency_list.num_of_added_freqs; i++)
          {
            if (i < PMO_PCCO_MAX_NUM_OF_ADDED_NCELLS)
            {
              /* now we have a list of added frequencies, with possible cell selection params attached,
              so need to concatenate each one to the accumulated list. */
              csn_nc_meas_params_nc_freq_list_added_freq_t *src = pcco->cell.gsm.nc_meas_params.nc_frequency_list.added_freqs[i];
              pmo_pcco_ncell_info_t *dst = &pmo_pcco_store_ptr->pcco_nc_freq_list->added_ncells_info[i];

              dst->bcch_arfcn.num = src->frequency;

              dst->bcch_arfcn.band = (sys_band_T) rr_get_band(
                src->frequency,   // arfcn
                gas_id            // gas_id
              );
              CONVERT_TO_BSIC_T(dst->bsic, src->bsic);
              if (src->cell_selection_params_valid)
              {
                dst->params_not_present_mask = decode_pmo_pcco_cell_sel_params(
                  &src->cell_selection_params,    // pcco_pmo_cell_sel_params
                  &dst->cell_sel_params);         // decoded_cell_sel_params
              }
              else
              {
                dst->params_not_present_mask = PARAM_MASK_ALL_PARAMS_OMITTED;
              }
            }
            else
            {
              break;
            }
          }
          pmo_pcco_store_ptr->pcco_nc_freq_list->num_of_added_ncells = i;
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("Failed to allocate memory for PCCO NC Frequency List");
      }
    }
  }
  /* else it's a PCCO to a 3G target, so nothing to do here */
}


/**
  @brief See rr_process_pmo_pcco.h
*/
rr_event_T rr_gprs_pmo_decode_params(
  packet_measurement_order_t *pmo,
  const gas_id_t gas_id
)
{
  pmo_pcco_store_t *pmo_pcco_store_ptr;
  pmo_pcco_data_t *pmo_store_ptr;

  pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(pmo_pcco_store_ptr);

  pmo_store_ptr = rr_psi_prepare_for_pmo_data(pmo_pcco_store_ptr);
  RR_NULL_CHECK_FATAL(pmo_store_ptr);

  MSG_GERAN_MED_2_G("Received PMO(%d/%d)", pmo->pmo_index, pmo->pmo_count);
  if (pmo_store_ptr->indexes_expected_mask == 0)
  {
    (void) prepare_for_new_msg_instances(
      pmo->pmo_count,                         // msg_count
      &pmo_store_ptr->indexes_expected_mask   // indexes_expected_mask
    );

    pmo_store_ptr->pmo_count = pmo->pmo_count;

    pmo_pcco_store_ptr->pmo_nc_freq_list_valid = FALSE;

    if (pmo_pcco_store_ptr->pmo_nc_freq_list != NULL)
    {
      GPRS_MEM_FREE(pmo_pcco_store_ptr->pmo_nc_freq_list);
      pmo_pcco_store_ptr->pmo_nc_freq_list = NULL;
    }
  }
  else
  {
    /*indexes_expected_mask is not zero means grr has not received all the instances of
    PMO messages from the network*/
    if (pmo_store_ptr->pmo_count != pmo->pmo_count)
    {
      /*pmo count should be same in all the instances of PMO messages
      if the pmo count in this PMO message is not matching with stored pmo_count
      means network is sending the new set of PMO instances to mobile. so
      resetting the accumulated PMO data */
      MSG_GERAN_MED_3_G(
        "Received PMO_COUNT and Stored PMO_COUNT are(%d/%d)",
        pmo->pmo_count,
        pmo_store_ptr->pmo_count,
        0
      );

      // Clear the PMO data
      rr_psi_clear_pmo_data(gas_id);

      /* set the indexes_expected_mask for the new set of PMO instance messages*/
      (void) prepare_for_new_msg_instances(
        pmo->pmo_count,                         // msg_count
        &pmo_store_ptr->indexes_expected_mask   // indexes_expected_mask
      );

      pmo_store_ptr->pmo_count = pmo->pmo_count;
    }
  }

  if (pmo->nc_meas_params_valid)
  {
    /* The NC Measurement Parameters (NC mode, Reporting Period, etc) should
    only be received in a single instance, but if it occurs in more than one,
    use the highest index only. */
    if (!pmo_store_ptr->nc_meas_params.valid ||
        pmo->pmo_index > pmo_store_ptr->nc_meas_params_index)
    {
      pmo_store_ptr->nc_meas_params.valid = TRUE;
      pmo_store_ptr->nc_meas_params_index = pmo->pmo_index;
      pmo_store_ptr->nc_meas_params.network_control_order = pmo->nc_meas_params->network_control_order;
      if (pmo->nc_meas_params->nc_period_valid)
      {
        pmo_store_ptr->nc_meas_params.nc_period_valid = TRUE;
        pmo_store_ptr->nc_meas_params.nc_non_drx_period = pmo->nc_meas_params->nc_non_drx_period;
        pmo_store_ptr->nc_meas_params.nc_reporting_period_i = pmo->nc_meas_params->nc_reporting_period_i;
        pmo_store_ptr->nc_meas_params.nc_reporting_period_t = pmo->nc_meas_params->nc_reporting_period_t;
      }
      else
      {
        pmo_store_ptr->nc_meas_params.nc_period_valid = FALSE;
      }
    }

    /* NC Frequency List can be received in multiple instances, should all be
    processed. */
    if (pmo->nc_meas_params->nc_frequency_list_valid)
    {
      /* If this is the first PMO instance to be received that includes the
      NC Frequency List then allocate some storage for it. */
      if (pmo_pcco_store_ptr->pmo_nc_freq_list == NULL)
      {
        pmo_pcco_store_ptr->pmo_nc_freq_list = GPRS_MEM_CALLOC(sizeof(pmo_pcco_nc_freq_list_t));
      }

      if (pmo_pcco_store_ptr->pmo_nc_freq_list != NULL)
      {
        pmo_pcco_store_ptr->pmo_nc_freq_list_valid = TRUE;

        if (pmo->nc_meas_params->nc_frequency_list.num_of_removed_freqs > 0)
        {
          uint8 i;
          uint8 j = pmo_pcco_store_ptr->pmo_nc_freq_list->num_of_removed_ncells;

          MSG_GERAN_HIGH_1_G("PMO Removed Frequencies: %d", pmo->nc_meas_params->nc_frequency_list.num_of_removed_freqs);

          for (i = 0;
               (i < pmo->nc_meas_params->nc_frequency_list.num_of_removed_freqs) &&
               (i < MAX_NC_MEAS_PARAMS_REMOVED_FREQS);
               ++i)
          {
            /* need to concatenate each new removed frequency index to the list of removed
            frequencies accumulated so far. */
            if (j < PMO_PCCO_MAX_NUM_OF_REMOVED_NCELLS)
            {
              MSG_GERAN_HIGH_2_G("  [%d]: Index %d", j, pmo->nc_meas_params->nc_frequency_list.removed_freqs[i]);
              pmo_pcco_store_ptr->pmo_nc_freq_list->removed_ncells[j++] = pmo->nc_meas_params->nc_frequency_list.removed_freqs[i];
            }
            else
            {
              break;
            }
          }
          pmo_pcco_store_ptr->pmo_nc_freq_list->num_of_removed_ncells = j;
        }

        if (pmo->nc_meas_params->nc_frequency_list.num_of_added_freqs > 0)
        {
          uint8 i;
          uint8 j = pmo_pcco_store_ptr->pmo_nc_freq_list->num_of_added_ncells;

          for (i = 0; i < pmo->nc_meas_params->nc_frequency_list.num_of_added_freqs; i++)
          {
            if (j < PMO_PCCO_MAX_NUM_OF_ADDED_NCELLS)
            {
              /* now we have a list of added frequencies, with possible cell selection params attached,
              so need to concatenate each one to the accumulated list. */
              csn_nc_meas_params_nc_freq_list_added_freq_t *src = pmo->nc_meas_params->nc_frequency_list.added_freqs[i];
              pmo_pcco_ncell_info_t *dst = &pmo_pcco_store_ptr->pmo_nc_freq_list->added_ncells_info[j];

              dst->bcch_arfcn.num = src->frequency;

              dst->bcch_arfcn.band = (sys_band_T) rr_get_band(src->frequency, gas_id);
              CONVERT_TO_BSIC_T(dst->bsic, src->bsic);
              if (src->cell_selection_params_valid)
              {
                dst->params_not_present_mask = decode_pmo_pcco_cell_sel_params(
                  &src->cell_selection_params,    // pcco_pmo_cell_sel_params
                  &dst->cell_sel_params);         // decoded_cell_sel_params
              }
              else
              {
                dst->params_not_present_mask = PARAM_MASK_ALL_PARAMS_OMITTED;
              }
              j++;
            }
            else
            {
              break;
            }
          }
          pmo_pcco_store_ptr->pmo_nc_freq_list->num_of_added_ncells = j;
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("Failed to allocate memory for PMO NC Frequency List");
      }
    }
  }

#ifdef FEATURE_WCDMA
  if (pmo->_3g_neighbour_cells_valid)
  {
    csn_3g_neighbour_cells_t *ncell_desc = pmo->_3g_neighbour_cells;
    int index_start_3g = 0;

    pmo_store_ptr->nlist_3g_valid = TRUE;

    if (ncell_desc->index_start_3g_valid)
    {
      index_start_3g = ncell_desc->index_start_3g;
    }

    if (ncell_desc->absolute_index_start_emr_valid)
    {
      if (!pmo_store_ptr->nlist_3g.absolute_index_start_EMR_valid ||
           pmo->pmo_index > pmo_store_ptr->nlist_3g.absolute_index_start_EMR_instance)
      {
        pmo_store_ptr->nlist_3g.absolute_index_start_EMR_valid = TRUE;
        pmo_store_ptr->nlist_3g.absolute_index_start_EMR_instance = pmo->pmo_index;
        pmo_store_ptr->nlist_3g.absolute_index_start_EMR = ncell_desc->absolute_index_start_emr;
      }
    }

    if (ncell_desc->utran_fdd_description_valid)
    {
      rr_gprs_3g_nlist_decode_fdd_cells(
        &pmo_store_ptr->nlist_3g,             // list
        NULL,                                 // unindexed_list
        pmo->pmo_index,                       // instance
        ncell_desc->utran_fdd_description,    // fdd_cells
        &index_start_3g,                       // index_start_3g
        gas_id
      );
    }

    if (ncell_desc->utran_tdd_description_valid)
    {
#ifdef FEATURE_GSM_TDS
      if (rr_is_utran_tdd_enabled(gas_id))
      {
        rr_gprs_3g_nlist_decode_tdd_cells_for_tds(
          &pmo_store_ptr->nlist_3g,             // list
          pmo->pmo_index,                       // instance
          ncell_desc->utran_tdd_description,    // tdd_cells
          &index_start_3g,                    // index_start_3g
          gas_id
        );
      }
      else
#endif /*FEATURE_GSM_TDS*/
      {
        /*update index but don't add cells to nlist_3g*/
        rr_gprs_3g_nlist_decode_tdd_cells(
          ncell_desc->utran_tdd_description,    // tdd_cells
          &index_start_3g                       // index_start_3g
        );
      }
    }

    if (ncell_desc->cdma2000_description_valid)
    {
      rr_gprs_3g_nlist_decode_cdma2000_cells(
        ncell_desc->cdma2000_description,   // cdma2000_cells
        &index_start_3g                     // index_start_3g
      );
    }

    if (ncell_desc->removed_3g_cell_description_valid)
    {
      rr_gprs_3g_nlist_decode_removed_3g_cell_description(
        &pmo_store_ptr->nlist_3g,                 // list
        ncell_desc->removed_3g_cell_description   // removed_3g_cell_description
      );
    }
  }

  if (pmo->gprs_3g_meas_params_valid)
  {
    if (!pmo_store_ptr->gprs_3g_meas_params.valid ||
         pmo->pmo_index > pmo_store_ptr->gprs_3g_meas_params_index)
    {
      memset(&pmo_store_ptr->gprs_3g_meas_params, 0, sizeof(gprs_3g_meas_params_mods_t));
      pmo_store_ptr->gprs_3g_meas_params_index = pmo->pmo_index;

      rr_gprs_3g_meas_params_merge_csn_mods(
        &pmo_store_ptr->gprs_3g_meas_params,    // dst
        pmo->gprs_3g_meas_params                // src
      );
    }
  }
#endif /* FEATURE_WCDMA */

  if (pmo->gprs_meas_params_valid)
  {
    if (!pmo_store_ptr->gprs_meas_params.valid ||
         pmo->pmo_index > pmo_store_ptr->gprs_meas_params_index)
    {
      memset(&pmo_store_ptr->gprs_meas_params, 0, sizeof(gprs_meas_params_mods_t));
      pmo_store_ptr->gprs_meas_params_index = pmo->pmo_index;

      rr_gprs_meas_params_merge_csn_mods(
        &pmo_store_ptr->gprs_meas_params,   // dst
        pmo->gprs_meas_params               // src
      );
    }
  }

  /* copy over the NACC related parameters */
  pmo_store_ptr->nacc_params.valid_flag = (boolean)pmo->r4_additions_flag;
  pmo_store_ptr->nacc_params.ccn_active = (boolean)pmo->ccn_active;
  pmo_store_ptr->nacc_params.ccn_support_flag = (boolean)pmo->ccn_support_flag;
  pmo_store_ptr->nacc_params.ccn_supported_desc = pmo->ccn_support_desc;

  /* @@TODO add three_g_ccn_active and valid flag to CSN decode struct for PMO */
  /* if (pmo->three_g_ccn_active_valid) */
  {
    pmo_store_ptr->three_g_ccn_active = 0; /* pmo->three_g_ccn_active */
  }

  if (pmo->priority_and_eutran_params_valid)
  {
    pmo_store_ptr->priority_and_eutran_params_valid = TRUE;

    if (pmo->priority_and_eutran_params.serving_cell_priority_params != NULL)
    {
      if (pmo_store_ptr->priority_and_eutran_params.serving_cell_priority_params_valid == FALSE ||
          pmo->pmo_index > pmo_store_ptr->priority_and_eutran_params.serving_cell_priority_params_index)
      {
        pmo_store_ptr->priority_and_eutran_params.serving_cell_priority_params_valid = TRUE;
        pmo_store_ptr->priority_and_eutran_params.serving_cell_priority_params_index = pmo->pmo_index;
        pmo_store_ptr->priority_and_eutran_params.serving_cell_priority_params = *pmo->priority_and_eutran_params.serving_cell_priority_params;
      }
    }

    if (pmo->priority_and_eutran_params.utran_priority_params != NULL)
    {
      if (pmo_store_ptr->priority_and_eutran_params.utran_priority_params[pmo->pmo_index] == NULL)
      {
         pmo_store_ptr->priority_and_eutran_params.utran_priority_params[pmo->pmo_index] = GPRS_MEM_MALLOC(sizeof(csn_utran_priority_params_ie_t));
      }

      if (pmo_store_ptr->priority_and_eutran_params.utran_priority_params[pmo->pmo_index] != NULL)
      {
        pmo_store_ptr->priority_and_eutran_params.utran_priority_params_valid_bitmap |= 1 << pmo->pmo_index;
        *pmo_store_ptr->priority_and_eutran_params.utran_priority_params[pmo->pmo_index] = *pmo->priority_and_eutran_params.utran_priority_params;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("Memory allocation failure, unable to store UTRAN Priority Parameters");
      }
    }

    if (pmo->priority_and_eutran_params.eutran_params != NULL)
    {
       if (pmo_store_ptr->priority_and_eutran_params.eutran_params[pmo->pmo_index] == NULL)
       {
          pmo_store_ptr->priority_and_eutran_params.eutran_params[pmo->pmo_index] = GPRS_MEM_MALLOC(sizeof(csn_eutran_params_ie_t));
       }

      if (pmo_store_ptr->priority_and_eutran_params.eutran_params[pmo->pmo_index] != NULL)
      {
        pmo_store_ptr->priority_and_eutran_params.eutran_params_valid_bitmap |= 1 << pmo->pmo_index;
        *pmo_store_ptr->priority_and_eutran_params.eutran_params[pmo->pmo_index] = *pmo->priority_and_eutran_params.eutran_params;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("Memory allocation failure, unable to store E-UTRAN Priority Parameters");
      }
    }
  }

#ifdef FEATURE_LTE
  if (pmo->individual_priorities != NULL)
  {
    if (pmo_store_ptr->individual_priorities_valid == FALSE ||
        pmo->pmo_index > pmo_store_ptr->individual_priorities_index)
    {
      pmo_store_ptr->individual_priorities_valid = TRUE;
      pmo_store_ptr->individual_priorities_index = pmo->pmo_index;
      pmo_store_ptr->individual_priorities = *pmo->individual_priorities;
    }
  }
#endif /* FEATURE_LTE */



  /***************************************************************************/
  /* Mark this instance as received */
  CLR_BIT_NUM( pmo_store_ptr->indexes_expected_mask, pmo->pmo_index );

  /* If this was the final instance, we can move on to process the whole lot */
  if (pmo_store_ptr->indexes_expected_mask == 0)
  {
    if (pmo_pcco_store_ptr->pmo_nc_freq_list_valid &&
        pmo_pcco_store_ptr->pmo_nc_freq_list != NULL)
    {
      boolean notify_l1_ba_changed;

      MSG_GERAN_MED_0_G("PMO NC Frequency List is valid");

      rr_gprs_revert_ba_gprs(gas_id);

      notify_l1_ba_changed = apply_nc_freq_list_deltas(
        pmo_pcco_store_ptr->pmo_nc_freq_list->num_of_added_ncells,
        pmo_pcco_store_ptr->pmo_nc_freq_list->added_ncells_info,
        pmo_pcco_store_ptr->pmo_nc_freq_list->num_of_removed_ncells,
        pmo_pcco_store_ptr->pmo_nc_freq_list->removed_ncells,
        gas_id
      );

      /* for NC mode 1 or 2, the NC_MEASUREMENTS_REQ will send the new BA list to L1 */
      if ( (pmo_store_ptr->nc_meas_params.network_control_order != NC_MEAS_NC1) &&
           (pmo_store_ptr->nc_meas_params.network_control_order != NC_MEAS_NC2) &&
           (notify_l1_ba_changed != FALSE) )
      {
        rr_gprs_update_l1_params(gas_id);
      }

      /* TODO: now that the BA has been modified, can probably clear it out as it can
      only be used on this cell.  There may be a requirement to keep it though - if we
      reselect to another cell (either autonomously or via PCCO) but the reselection
      fails, on returning to the old cell we may need to restore the original PMO/PCCO
      parameters. */
    }
    else if (pmo_store_ptr->nc_meas_params.valid)
    {
      /* See 3GPP 44.060 section 5.6.3.2
       * "If the last PACKET MEASUREMENT ORDER message (full set of instances)
       *  does not contain a 'NC frequency list' (no added or deleted frequencies)
       *  the mobile station shall return to BA(GPRS)."
       */
      MSG_GERAN_HIGH_0_G("Empty NC Frequency List: revert BA(GPRS)");
      rr_gprs_revert_ba_gprs(gas_id);
      rr_gprs_update_l1_params(gas_id);
    }

#ifdef FEATURE_WCDMA
    if (pmo_store_ptr->nlist_3g_valid)
    {
      rr_sys_info_3g_data_t *rr_sys_info_3g_data_ptr = rr_get_sys_info_3g_data_ptr(gas_id);

      /* merge the PMO 3G BA list with the broadcast BA list to get the modified BA list,
      then send it to L1. */
      boolean nlist_3g_modified = rr_gprs_3g_nlist_apply_mods(
        &rr_sys_info_3g_data_ptr->grr_3g_nlist_modified,
        &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast,
        &pmo_store_ptr->nlist_3g
      );

      if (nlist_3g_modified)
      {
        rr_sys_info_3g_data_ptr->grr_3g_nlist = &rr_sys_info_3g_data_ptr->grr_3g_nlist_modified;
      }
      else
      {
        rr_sys_info_3g_data_ptr->grr_3g_nlist = &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast;
      }
    }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
    /* Check whether Individual Priorities IE has been received in PMO. */
    if (pmo_store_ptr->individual_priorities_valid)
    {
      /* Check whether Individual Priorities IE indicates for priorities to
      be stored (.valid == TRUE) or deleted (.valid == FALSE). */
      if (pmo_store_ptr->individual_priorities.valid)
      {
        rr_PLMN_request_details_T * rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
        rr_individual_priorities_ie_t individual_priorities_ie;

        /* Need to convert the CSN.1 Individual Priorities in to the RR local
        type, so they can be stored. */
        memset(&individual_priorities_ie, 0, sizeof(rr_individual_priorities_ie_t));

        rr_convert_csn_individual_priorities_ie(
          &individual_priorities_ie,
          &pmo_store_ptr->individual_priorities
        );

        rr_individual_priorities_ie_dump(
          &individual_priorities_ie
        );

        rr_individual_priorities_store(
          &individual_priorities_ie,
          rr_gprs_get_scell_arfcn(gas_id),
          rr_PLMN_request_details_ptr->requested_PLMN_id,
          gas_id
        );

        rr_individual_priorities_ie_free_children(
          &individual_priorities_ie
        );
      }
      else
      {
        rr_individual_priorities_clear(gas_id);
      }
    }
#endif /* FEATURE_LTE */

    return EV_PMO_MESSAGE;
  }

  return EV_NO_EVENT;
}


/**
  @brief See rr_process_pmo_pcco.h
*/
void rr_gprs_pcco_apply_ba_gprs_deltas(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  pmo_pcco_store_t *pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(pmo_pcco_store_ptr);

  if (pmo_pcco_store_ptr->pcco_nc_freq_list_valid &&
      pmo_pcco_store_ptr->pcco_nc_freq_list != NULL)
  {
    if ((pmo_pcco_store_ptr->pcco_nc_freq_list->num_of_added_ncells > 0) ||
        (pmo_pcco_store_ptr->pcco_nc_freq_list->num_of_removed_ncells > 0))
    {
      (void) apply_nc_freq_list_deltas(
        pmo_pcco_store_ptr->pcco_nc_freq_list->num_of_added_ncells,
        pmo_pcco_store_ptr->pcco_nc_freq_list->added_ncells_info,
        pmo_pcco_store_ptr->pcco_nc_freq_list->num_of_removed_ncells,
        pmo_pcco_store_ptr->pcco_nc_freq_list->removed_ncells,
        gas_id
      );
    }
  }
} /* end rr_gprs_pcco_apply_ba_gprs_deltas() */


/**
  @brief See rr_sys_info_pacch.h
*/
void rr_process_pmo_pcco_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(pmo_pcco_store, 0, sizeof(pmo_pcco_store));
}

/*CR550108*/
boolean  rr_is_pmo_nc_frequency_list_valid(const gas_id_t gas_id)
{
  pmo_pcco_store_t *pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(pmo_pcco_store_ptr);

  return pmo_pcco_store_ptr->pmo_nc_freq_list_valid;
}

pmo_pcco_data_t *rr_get_pmo_data(const gas_id_t gas_id)
{
  pmo_pcco_store_t *pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(pmo_pcco_store_ptr);

  if (pmo_pcco_store_ptr->pmo_store_ptr == NULL)
  {
    /* attempt to allocate and zero-init the required memory */
    pmo_pcco_store_ptr->pmo_store_ptr = GPRS_MEM_CALLOC(sizeof(pmo_pcco_data_t));

    if (pmo_pcco_store_ptr->pmo_store_ptr == NULL)
    {
      ERR_GERAN_FATAL_0_G("pmo_store_ptr alloc failed");
    }
  }

  return pmo_pcco_store_ptr->pmo_store_ptr;

} /* end rr_psi_prepare_for_pmo_data() */

/**
  @brief Clears the PMO/PCCO data
*/

void rr_psi_clear_pmo_data_going_inactive(const gas_id_t gas_id)
{
  pmo_pcco_store_t *pmo_pcco_store_ptr = rr_process_pmo_pcco_get_data_ptr(gas_id);
  pmo_pcco_store_ptr->pmo_nc_freq_list_valid = FALSE;
  if (pmo_pcco_store_ptr->pmo_nc_freq_list != NULL)
  {
    GPRS_MEM_FREE(pmo_pcco_store_ptr->pmo_nc_freq_list);
    pmo_pcco_store_ptr->pmo_nc_freq_list = NULL;
  } /* end rr_psi_clear_pmo_data_going_inactive() */

} /* rr_psi_clear_pmo_data_going_inactive */



/*CR550108*/

/* EOF */

