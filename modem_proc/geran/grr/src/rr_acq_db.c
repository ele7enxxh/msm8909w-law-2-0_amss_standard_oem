/*! \file rr_acq_db.c

  This module implements the RR acquisition database.  This provides an API
  for querying whether an ARFCN is known to be in a particular PLMN.

  - Implemented as a lookup table (array) and a list.  This allows both fast
    access (ARFCNs are hashed to reduce them to a unique array index) but also
    allows some temporal information to be stored, i.e. items at the back of
    the list are most recent.
  - Up to one PLMN Id is stored per ARFCN.
  - All ARFCNs are stored in RAM, but only the most recent n are saved to NV
    due to NV item size limits.

  EXTERNALIZED FUNCTIONS
    rr_acq_db_init
    rr_acq_db_save
    rr_acq_db_is_arfcn_in_plmn
    rr_acq_db_add_arfcn
    rr_acq_db_populate_pscan_db
    rr_acq_db_prioritise_pscan_results

  INITIALIZATION AND SEQUENCING REQUIREMENTS
    rr_acq_db_init should be called at task initialisation to load the saved
    acquisition database from NV.

    rr_acq_db_save should be called at power-off or at any other appropriate
    time to allow the acquisition database to be saved to NV.

                Copyright (c) 2007-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_acq_db.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#include "rr_seg_load.h"
#include "rr_acq_db.h"
#include "msg.h"
#include "list.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "nv_items.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "rr_pscan.h"
#include "rr_multi_sim.h"
#include "rr_mode.h"
#include "stringl/stringl.h"
#include "rr_cell_selection.h"
#include "rr_nv.h"
#include "rr_control.h"
#include "rr_general.h"
#include "rr_sys_info_si.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define MAX_RR_ACQ_DB_ENTRIES 971
#if defined(FEATURE_DUAL_SIM)
  #define MAX_RR_ACQ_DB_ENTRIES_PER_PLMN      32
#else
  #define MAX_RR_ACQ_DB_ENTRIES_PER_PLMN      64
#endif /* FEATURE_DUAL_SIM */

#define MAX_RR_ACQ_DB_PSCAN_FREQS           192
#define MAX_RR_ACQ_DB_PSCAN_FREQS_PER_PLMN  64

/* This should be equal to the number of nv_rr_acq_db_chunk_xx items that are
stored in NV. */
#define NUM_RR_ACQ_DB_NV_ITEMS 8
#define RR_ACQ_DB_CHUNK_COUNT NUM_RR_ACQ_DB_NV_ITEMS

#define NV_RR_ACQ_DB_INVALID_HASHED_ARFCN 0xffff
#define NV_RR_ACQ_DB_INVALID_PLMN_ID_BYTE 0xff



#define NV_RR_ACQ_DB_DEBUG_CHUNK_INDEX(nvid) ((int)(nvid)-(int)(NV_RR_ACQ_DB_CHUNK_00_I))

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


typedef struct
{
  list_link_type     list_link;
  boolean            valid;
  sys_plmn_id_s_type plmn_id;
  ARFCN_T            arfcn;
} rr_acq_db_entry_t;

typedef struct
{
  rr_acq_db_entry_t array[MAX_RR_ACQ_DB_ENTRIES];
  list_type list;
} rr_acq_db_t;

typedef struct
{
  rr_acq_db_t   rr_acq_db;
  nv_item_type  nv_rr_acq_db_chunks[RR_ACQ_DB_CHUNK_COUNT];
} rr_acq_db_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static const nv_items_enum_type rr_acq_db_nv_items[NUM_RR_ACQ_DB_NV_ITEMS] = {
  NV_RR_ACQ_DB_CHUNK_00_I,
  NV_RR_ACQ_DB_CHUNK_01_I,
  NV_RR_ACQ_DB_CHUNK_02_I,
  NV_RR_ACQ_DB_CHUNK_03_I,
  NV_RR_ACQ_DB_CHUNK_04_I,
  NV_RR_ACQ_DB_CHUNK_05_I,
  NV_RR_ACQ_DB_CHUNK_06_I,
  NV_RR_ACQ_DB_CHUNK_07_I
};

static rr_acq_db_data_t rr_acq_db_data;

#if defined(FEATURE_DUAL_SIM)
static boolean rr_acq_db_init_done = FALSE;

static boolean rr_irat_acq_db_init_done = FALSE;

#endif /* FEATURE_DUAL_SIM */


static rr_irat_acq_db_t rr_irat_acq_db;


/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \return const nv_items_enum_type *
 */
static const nv_items_enum_type *rr_acq_db_get_nv_ptr(void)
{
  return(rr_acq_db_nv_items);
}

/*!
 * \brief Return a pointer to the module data.
 *
 * \return rr_acq_db_data_t *
 */
static rr_acq_db_data_t *rr_acq_db_get_data_ptr(void)
{
  return(&rr_acq_db_data);
}

/*!
 * \brief Return a pointer to the module data.
 *
 * \return rr_irat_acq_db_t *
 */
rr_irat_acq_db_t *rr_irat_acq_db_get_data_ptr(void)
{
  return(&rr_irat_acq_db);
}

/*!
 * \brief Resets the local copy of the NV chunks for the specified ACQ DB
 *
 * \param database_ptr - pointer to the ACQ database
 */
static void rr_acq_db_clear_local_nv_store(rr_acq_db_data_t *database_ptr)
{
  uint16 chunk_index;
  uint16 entry_index;

  for (chunk_index = 0; chunk_index < RR_ACQ_DB_CHUNK_COUNT; chunk_index++)
  {
    for (entry_index = 0; entry_index < NV_RR_ACQ_DB_MAX_ENTRIES_PER_CHUNK; entry_index++)
    {
      database_ptr->nv_rr_acq_db_chunks[chunk_index].rr_acq_db_chunk_00[entry_index].hashed_arfcn = NV_RR_ACQ_DB_INVALID_HASHED_ARFCN;
      database_ptr->nv_rr_acq_db_chunks[chunk_index].rr_acq_db_chunk_00[entry_index].plmn_id[0] = NV_RR_ACQ_DB_INVALID_PLMN_ID_BYTE;
      database_ptr->nv_rr_acq_db_chunks[chunk_index].rr_acq_db_chunk_00[entry_index].plmn_id[1] = NV_RR_ACQ_DB_INVALID_PLMN_ID_BYTE;
      database_ptr->nv_rr_acq_db_chunks[chunk_index].rr_acq_db_chunk_00[entry_index].plmn_id[2] = NV_RR_ACQ_DB_INVALID_PLMN_ID_BYTE;
    }
  }

  return;
}

#ifdef RR_ACQ_DB_DEBUG
/**
 * Performs checks on the integrity of the acquisition database.
 *
 * This function checks that the various redundant data in the acquisition
 * database (data that is used to speed up various operations) is correct.
 */
static void rr_acq_db_validate(rr_acq_db_data_t *database_ptr)
{
  list_size_type entries_in_list = list_size(&database_ptr->rr_acq_db.list);
  uint16         valid_entries_in_array = 0;
  uint16         i;

  /* Check that the length of the acquisition database list is equal to the
  number of 'valid' entries in the acquisition database array. */
  for (i = 0; i < MAX_RR_ACQ_DB_ENTRIES; i++)
  {
    if (database_ptr->rr_acq_db.array[i].valid)
    {
      valid_entries_in_array++;
    }
  }

  if (!(valid_entries_in_array == entries_in_list))
  {
    MSG_GERAN_ERROR_2_G("valid_entries_in_array (%d) != entries_in_list (%d)",
               valid_entries_in_array,
               entries_in_list);
  }

  return;
}
#endif /* RR_ACQ_DB_DEBUG */


static int rr_acq_db_entry_compare_plmn_id_fn(void *list_item, void *value)
{
  rr_acq_db_entry_t * entry = (rr_acq_db_entry_t *) list_item;
  sys_plmn_id_s_type plmn_id = *((sys_plmn_id_s_type *) value);

  if (sys_plmn_match(plmn_id, entry->plmn_id))
  {
    return 1;
  }

  return 0;
}

/**
 * rr_acq_db_hash_arfcn
 *
 * Returns a unique value to be used as an index in to the lookup table
 * 'frequency_index' to optimise searching of the candidate cell database
 * for a specific ARFCN.
 *
 * This is basically a mapping of the 971 unique ARFCNs to unique indexes
 * within the range 0 - 1023.  To achieve this, E/PGSM ARFCNs remain
 * unchanged, GSM850 and DCS1800 ARFCNs are shifted down by a fixed amount
 * and PCS1900 are shifted up by a fixed amount.
 *
 * This results in the following mappings:
 *
 *                        Channel Number     Mapped Value   Offset
 *   GSM900 lower range   0 - 124            0 - 124        0
 *   GSM850               128 - 251          125 - 248      -3
 *   DCS1800              512 - 885          249 - 622      -263
 *   PCS1900              512 - 810          623 - 921      +111
 *   GSM900 upper range   975 - 1023         922 - 970      -53
 *
 * The valid return values for this function are 0 - 1023.  If the ARFCN
 * is invalid and cannot be hashed, a value of MAX_RR_ACQ_DB_ENTRIES will be returned.
 */
static uint16 rr_acq_db_hash_arfcn(ARFCN_T arfcn)
{
  #define RR_CANDIDATE_DB_ARFCN_HASH_GSM_900_LOWER_OFFSET  (0)
  #define RR_CANDIDATE_DB_ARFCN_HASH_DCS_1800_OFFSET       (-263)
  #define RR_CANDIDATE_DB_ARFCN_HASH_GSM_850_OFFSET        (-3)
  #define RR_CANDIDATE_DB_ARFCN_HASH_PCS_1900_OFFSET       (+111)
  #define RR_CANDIDATE_DB_ARFCN_HASH_GSM_900_UPPER_OFFSET  (-53)

  uint16 hashed_value = MAX_RR_ACQ_DB_ENTRIES;

  switch (arfcn.band)
  {
    case SYS_BAND_PGSM_900:
    case SYS_BAND_EGSM_900:
    {
      if (arfcn.num <= 124)
      {
        hashed_value = (uint16) (arfcn.num + RR_CANDIDATE_DB_ARFCN_HASH_GSM_900_LOWER_OFFSET);
      }
      else
      {
        #ifdef RR_ACQ_DB_DEBUG
        if (!(arfcn.num >= 975 && arfcn.num <= 1023))
        {
          MSG_GERAN_ERROR_2_G("arfcn.num >= 975 && arfcn.num <= 1023, arfcn.num:%d, band:%d", arfcn.num, arfcn.band);
        }
        #endif /* RR_ACQ_DB_DEBUG */
        hashed_value = (uint16) (arfcn.num + RR_CANDIDATE_DB_ARFCN_HASH_GSM_900_UPPER_OFFSET);
      }
    }
    break;

    case SYS_BAND_DCS_1800:
    {
      hashed_value = (uint16) (arfcn.num + RR_CANDIDATE_DB_ARFCN_HASH_DCS_1800_OFFSET);
    }
    break;

    case SYS_BAND_PCS_1900:
    {
      hashed_value = (uint16) (arfcn.num + RR_CANDIDATE_DB_ARFCN_HASH_PCS_1900_OFFSET);
    }
    break;

    case SYS_BAND_CELL_850:
    {
      hashed_value = (uint16) (arfcn.num + RR_CANDIDATE_DB_ARFCN_HASH_GSM_850_OFFSET);
    }
    break;

    default:
      MSG_GERAN_ERROR_2("ARFCN(%d,%d) band is unexpected value",
                arfcn.num,
                arfcn.band);
  }

  if (hashed_value >= MAX_RR_ACQ_DB_ENTRIES)
  {
    MSG_GERAN_ERROR_2("Hashed value %d is out of range, returning %d",
              hashed_value,
              MAX_RR_ACQ_DB_ENTRIES);
    hashed_value = MAX_RR_ACQ_DB_ENTRIES;
  }

  return hashed_value;
}


/**
 * rr_acq_db_unhash_arfcn
 *
 * Performs the inverse operation to rr_acq_db_hash_arfcn.
 */
static ARFCN_T rr_acq_db_unhash_arfcn(uint16 hashed_arfcn)
{
  ARFCN_T arfcn;

  if (hashed_arfcn <= 124)
  {
    arfcn.band = SYS_BAND_EGSM_900;
    arfcn.num = (uint16) (hashed_arfcn - RR_CANDIDATE_DB_ARFCN_HASH_GSM_900_LOWER_OFFSET);
  }
  else if (hashed_arfcn >= 125 && hashed_arfcn <= 248)
  {
    arfcn.band = SYS_BAND_CELL_850;
    arfcn.num = (uint16) (hashed_arfcn - RR_CANDIDATE_DB_ARFCN_HASH_GSM_850_OFFSET);
  }
  else if (hashed_arfcn >= 249 && hashed_arfcn <= 622)
  {
    arfcn.band = SYS_BAND_DCS_1800;
    arfcn.num = (uint16) (hashed_arfcn - RR_CANDIDATE_DB_ARFCN_HASH_DCS_1800_OFFSET);
  }
  else if (hashed_arfcn >= 623 && hashed_arfcn <= 921)
  {
    arfcn.band = SYS_BAND_PCS_1900;
    arfcn.num = (uint16) (hashed_arfcn - RR_CANDIDATE_DB_ARFCN_HASH_PCS_1900_OFFSET);
  }
  else if (hashed_arfcn >= 922 && hashed_arfcn <= 970)
  {
    arfcn.band = SYS_BAND_EGSM_900;
    arfcn.num = (uint16) (hashed_arfcn - RR_CANDIDATE_DB_ARFCN_HASH_GSM_900_UPPER_OFFSET);
  }
  else
  {
    MSG_GERAN_ERROR_1("Hashed arfcn is out of range: %d", hashed_arfcn);
    arfcn.num = INVALID_ARFCN;
    arfcn.band = SYS_BAND_NONE;
  }

  return arfcn;
}


/**
 * Removes all entries in the RR acquisition database for the specified PLMN.
 *
 * This function operates on the list part of the RR acquisition database.  It
 * traverses the list marking any entries that match the specified PLMN Id as
 * not valid.
 *
 * @param plmn_id The PLMN Id to search for and invalidate.
 */
static void rr_acq_db_delete_all_entries_for_plmn(
  sys_plmn_id_s_type plmn_id,
  const gas_id_t gas_id
)
{
  rr_acq_db_entry_t *entry;
  rr_acq_db_data_t  *rr_acq_db_data_ptr;
#ifdef RR_ACQ_DB_DEBUG
  boolean            plmn_is_undefined;
  boolean            mnc_includes_pcs_digit;
  sys_mcc_type       mcc;
  sys_mnc_type       mnc;

  sys_plmn_get_mcc_mnc(plmn_id,
                       &plmn_is_undefined,
                       &mnc_includes_pcs_digit,
                       &mcc,
                       &mnc);

  MSG_GERAN_MED_2_G("Deleting all entries for PLMN %d-%d", mcc, mnc);
#endif /* RR_ACQ_DB_DEBUG */

  // Obtain a pointer to the module data
  rr_acq_db_data_ptr = rr_acq_db_get_data_ptr();
  RR_NULL_CHECK_FATAL(rr_acq_db_data_ptr);

  while ((entry = list_linear_search(&rr_acq_db_data_ptr->rr_acq_db.list,
                                    rr_acq_db_entry_compare_plmn_id_fn,
                                    &plmn_id)) != NULL)
  {
    entry->valid = FALSE;
    list_pop_item(&rr_acq_db_data_ptr->rr_acq_db.list, &entry->list_link);
  }

#ifdef RR_ACQ_DB_DEBUG
  rr_acq_db_validate(rr_acq_db_data_ptr);
#endif /* RR_ACQ_DB_DEBUG */

  return;
}


sys_band_mask_type convert_sys_band_to_sys_band_mask(sys_band_T band)
{
  sys_band_mask_type band_mask = SYS_BAND_MASK_EMPTY;

  if (band == SYS_BAND_EGSM_900)
  {
    band_mask = SYS_BAND_MASK_GSM_EGSM_900;
  }
  else
  if (band == SYS_BAND_PGSM_900)
  {
    band_mask = SYS_BAND_MASK_GSM_PGSM_900;
  }
  else
  if (band == SYS_BAND_PCS_1900)
  {
    band_mask = SYS_BAND_MASK_GSM_PCS_1900;
  }
  else
  if (band == SYS_BAND_DCS_1800)
  {
    band_mask = SYS_BAND_MASK_GSM_DCS_1800;
  }
  else
  if (band == SYS_BAND_CELL_850)
  {
    band_mask = SYS_BAND_MASK_GSM_850;
  }

  return band_mask;

} /* end get_band_mask() */


/**
 * Counts the number of ARFCNs in the acquisition database that are associated
 * with the specified PLMN.
 *
 * @param plmn_id The PLMN to search for.
 * @return The number of ARFCNs associated with the specified PLMN.
 */
static uint16 rr_acq_db_num_of_arfcns_for_plmn(
  sys_plmn_id_s_type plmn_id,
  const gas_id_t gas_id
)
{
  rr_acq_db_entry_t *entry;
  uint16 count = 0;
  rr_acq_db_data_t  *rr_acq_db_data_ptr;

  // Obtain a pointer to the module data
  rr_acq_db_data_ptr = rr_acq_db_get_data_ptr();
  RR_NULL_CHECK_FATAL(rr_acq_db_data_ptr);

  entry = (rr_acq_db_entry_t *) list_peek_front(&rr_acq_db_data_ptr->rr_acq_db.list);

  while (entry != NULL)
  {
    if (sys_plmn_match(plmn_id, entry->plmn_id))
    {
      count++;
    }
    entry = (rr_acq_db_entry_t *) list_peek_next(&rr_acq_db_data_ptr->rr_acq_db.list, &entry->list_link);
  }

  return count;
}

static void rr_acq_db_read_from_nvmem(
  const gas_id_t gas_id
)
{
  ghdi_nvmem_parameter_id_T  nv_item;
  ghdi_nvmem_data_T         *nv_data;
  ghdi_status_T              status;
  uint16                     i;
  uint16                     j;
  rr_acq_db_data_t  *rr_acq_db_data_ptr;
  const nv_items_enum_type *rr_acq_db_nv_ptr;

  // Obtain a pointer to the module data
  rr_acq_db_data_ptr = rr_acq_db_get_data_ptr();
  RR_NULL_CHECK_FATAL(rr_acq_db_data_ptr);

  // Obtain a pointer to the NV data
  rr_acq_db_nv_ptr = rr_acq_db_get_nv_ptr();
  RR_NULL_CHECK_FATAL(rr_acq_db_nv_ptr);

  /* initialise the database structure */
  memset(&rr_acq_db_data_ptr->rr_acq_db, 0, sizeof(rr_acq_db_t));
  list_init(&rr_acq_db_data_ptr->rr_acq_db.list);

  for (i = 0; i < RR_ACQ_DB_CHUNK_COUNT; i++)
  {
    nv_item = (ghdi_nvmem_parameter_id_T)(rr_acq_db_nv_ptr[i]);
    nv_data = (ghdi_nvmem_data_T *)&rr_acq_db_data_ptr->nv_rr_acq_db_chunks[i];

    status = ghdi_nvmem_read(nv_item, nv_data);

    if (status == GHDI_SUCCESS)
    {
      MSG_GERAN_MED_1_G("Read RR Acquisition Database chunk 0x%02x from NV successfully", NV_RR_ACQ_DB_DEBUG_CHUNK_INDEX(nv_item));
    }
    else
    {
      /* Read has failed.  Not sure what state NV task will leave the RAM in,
      so initialise it to 'invalid' values. */
      MSG_GERAN_MED_1_G("Read RR Acquisition Database chunk 0x%02x from NV failed", NV_RR_ACQ_DB_DEBUG_CHUNK_INDEX(nv_item));
      for (j = 0; j < NV_RR_ACQ_DB_MAX_ENTRIES_PER_CHUNK; j++)
      {
        rr_acq_db_data_ptr->nv_rr_acq_db_chunks[i].rr_acq_db_chunk_00[j].hashed_arfcn = NV_RR_ACQ_DB_INVALID_HASHED_ARFCN;
        rr_acq_db_data_ptr->nv_rr_acq_db_chunks[i].rr_acq_db_chunk_00[j].plmn_id[0] = NV_RR_ACQ_DB_INVALID_PLMN_ID_BYTE;
        rr_acq_db_data_ptr->nv_rr_acq_db_chunks[i].rr_acq_db_chunk_00[j].plmn_id[1] = NV_RR_ACQ_DB_INVALID_PLMN_ID_BYTE;
        rr_acq_db_data_ptr->nv_rr_acq_db_chunks[i].rr_acq_db_chunk_00[j].plmn_id[2] = NV_RR_ACQ_DB_INVALID_PLMN_ID_BYTE;
      }
    }
  }

  return;
}

/*==============================================================================

                    PUBLIC FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/**
 * See header file for description.
 */
void rr_irat_acq_db_init(void)
{
  rr_irat_acq_db_t  *rr_irat_acq_db_data_ptr;

#if defined(FEATURE_DUAL_SIM)
  if (rr_irat_acq_db_init_done)
  {
    /* database only needs to be initialised once */
    return;
  }
  rr_irat_acq_db_init_done = TRUE;
#endif /* FEATURE_DUAL_SIM */

  // Obtain a pointer to the module data
  rr_irat_acq_db_data_ptr = rr_irat_acq_db_get_data_ptr();
  RR_NULL_CHECK_FATAL(rr_irat_acq_db_data_ptr);

  rr_irat_acq_db_data_ptr->rat = RR_RAT_UNDEFINED;
  memset(&rr_irat_acq_db_data_ptr->plmn_id, 0, sizeof(rr_irat_acq_db_data_ptr->plmn_id));
  memset(&rr_irat_acq_db_data_ptr->freq_list, 0, sizeof(rr_irat_acq_db_data_ptr->freq_list));

  rr_irat_acq_db_data_ptr->is_hst_cell = FALSE;

  
  return;
}

/**
 * See header file for description.
 */
void rr_acq_db_init(
  const gas_id_t gas_id
)
{
  nv_rr_acq_db_chunk_00_type *entry;
  nv_item_type               *nv_rr_acq_db_entry;
  uint16                      i;
  uint16                      j;
  rr_acq_db_data_t  *rr_acq_db_data_ptr;

#if defined(FEATURE_DUAL_SIM)
  if (rr_acq_db_init_done)
  {
    /* database only needs to be initialised once */
    return;
  }
  rr_acq_db_init_done = TRUE;
#endif /* FEATURE_DUAL_SIM */

  // Obtain a pointer to the module data
  rr_acq_db_data_ptr = rr_acq_db_get_data_ptr();
  RR_NULL_CHECK_FATAL(rr_acq_db_data_ptr);

  rr_acq_db_read_from_nvmem(gas_id);

  /* Now that the NV chunks are available, iterate over them and create
  the RR acq-db list ready to use.

  Note that we iterate over the elements in reverse order, so that elements at
  the lowest indices of the arrays are added to the list last, meaning that they
  are the most recent. */

  for (i = RR_ACQ_DB_CHUNK_COUNT; i > 0; i--)
  {
    nv_rr_acq_db_entry = &rr_acq_db_data_ptr->nv_rr_acq_db_chunks[i-1];

    for (j = NV_RR_ACQ_DB_MAX_ENTRIES_PER_CHUNK; j > 0; j--)
    {
      entry = &nv_rr_acq_db_entry->rr_acq_db_chunk_00[j-1];

      if (entry->hashed_arfcn != NV_RR_ACQ_DB_INVALID_HASHED_ARFCN &&
          entry->plmn_id[0] != NV_RR_ACQ_DB_INVALID_PLMN_ID_BYTE &&
          entry->plmn_id[1] != NV_RR_ACQ_DB_INVALID_PLMN_ID_BYTE &&
          entry->plmn_id[2] != NV_RR_ACQ_DB_INVALID_PLMN_ID_BYTE)
      {
        sys_plmn_id_s_type plmn_id;
        ARFCN_T            arfcn;

        arfcn = rr_acq_db_unhash_arfcn(entry->hashed_arfcn);
        plmn_id.identity[0] = entry->plmn_id[0];
        plmn_id.identity[1] = entry->plmn_id[1];
        plmn_id.identity[2] = entry->plmn_id[2];

        if ((arfcn.num != INVALID_ARFCN) && (arfcn.band != SYS_BAND_NONE))
        {
          (void)rr_acq_db_add_arfcn(arfcn, plmn_id, gas_id);
        }
      }
    }
  }

#ifdef RR_ACQ_DB_DEBUG
  rr_acq_db_validate(rr_acq_db_data_ptr);
#endif /* RR_ACQ_DB_DEBUG */

  return;
}

static void rr_acq_db_save_internal(rr_acq_db_data_t *database_ptr, const nv_items_enum_type *nv_items_ptr)
{
  ghdi_nvmem_parameter_id_T  nv_item;
  ghdi_nvmem_data_T         *nv_data;
  ghdi_status_T              status;
  rr_acq_db_entry_t         *entry;
  uint16                     i = 0;
  uint16                     j = 0;

  rr_acq_db_clear_local_nv_store(database_ptr);

  /* Write the acquisition database to NV */

  /* Only the most recent n entries are saved in the RR acquisition database. */
  entry = (rr_acq_db_entry_t *)list_peek_front(&database_ptr->rr_acq_db.list);

  while ((entry != NULL) && (i < RR_ACQ_DB_CHUNK_COUNT))
  {
    if (!entry->valid)
    {
      MSG_GERAN_ERROR_0("Invalid entry in RR acq-db list!");
      continue;
    }

    if (j < NV_RR_ACQ_DB_MAX_ENTRIES_PER_CHUNK)
    {
      database_ptr->nv_rr_acq_db_chunks[i].rr_acq_db_chunk_00[j].hashed_arfcn = rr_acq_db_hash_arfcn(entry->arfcn);
      database_ptr->nv_rr_acq_db_chunks[i].rr_acq_db_chunk_00[j].plmn_id[0] = entry->plmn_id.identity[0];
      database_ptr->nv_rr_acq_db_chunks[i].rr_acq_db_chunk_00[j].plmn_id[1] = entry->plmn_id.identity[1];
      database_ptr->nv_rr_acq_db_chunks[i].rr_acq_db_chunk_00[j].plmn_id[2] = entry->plmn_id.identity[2];
      ++j;
    }

    if (j >= NV_RR_ACQ_DB_MAX_ENTRIES_PER_CHUNK)
    {
      /* We've reached the end of the current chunk, so reset the entry index
      and increment the chunk index. */
      j = 0;
      i++;
    }

    entry = (rr_acq_db_entry_t *) list_peek_next(&database_ptr->rr_acq_db.list, &entry->list_link);
  }

  MSG_GERAN_MED_0("Writing AcqDB to NV...");

  /* Write the acquisition database to NV */
  for (i = 0; i < RR_ACQ_DB_CHUNK_COUNT; i++)
  {
    nv_item = (ghdi_nvmem_parameter_id_T)(nv_items_ptr[i]);
    nv_data = (ghdi_nvmem_data_T *)&database_ptr->nv_rr_acq_db_chunks[i];

    status = ghdi_nvmem_write(nv_item, nv_data);

    if (status != GHDI_SUCCESS)
    {
      /* Write has failed.  Not a lot we can do except output some debug messages. */
      MSG_GERAN_ERROR_2("Failed to write RR AcqDB chunk 0x%02x to NV, status: %d", NV_RR_ACQ_DB_DEBUG_CHUNK_INDEX(nv_item), status);
    }
  }

#ifdef RR_ACQ_DB_DEBUG
  rr_acq_db_validate(database_ptr);
#endif /* RR_ACQ_DB_DEBUG */

  return;
}

/**
 * See header file for description.
 */
void rr_acq_db_save(const gas_id_t gas_id)
{
  rr_acq_db_data_t *rr_acq_db_data_ptr = rr_acq_db_get_data_ptr();
  const nv_items_enum_type *rr_acq_db_nv_ptr = rr_acq_db_get_nv_ptr();

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* DB is shared across all subscriptions, only the last active stack writes it */
  if (rr_ms_any_other_active_gas_id(gas_id))
  {
    MSG_GERAN_HIGH_0_G("Another GAS ID is active, not writing ACQ DB to NV");
    return;
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  if ((rr_acq_db_data_ptr != NULL) &&
      (rr_acq_db_nv_ptr   != NULL))
  {
    rr_acq_db_save_internal(rr_acq_db_data_ptr, rr_acq_db_nv_ptr);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("RR ACQ database pointer is NULL, not writing to NV");
  }

  return;
}


/**
 * See header file for description.
 */
boolean rr_acq_db_is_arfcn_in_plmn(
  ARFCN_T arfcn,
  sys_plmn_id_s_type plmn_id,
  const gas_id_t gas_id
)
{
  uint16 hashed_arfcn = rr_acq_db_hash_arfcn(arfcn);
  rr_acq_db_data_t  *rr_acq_db_data_ptr;

  // Obtain a pointer to the module data
  rr_acq_db_data_ptr = rr_acq_db_get_data_ptr();
  RR_NULL_CHECK_FATAL(rr_acq_db_data_ptr);

  if (hashed_arfcn == MAX_RR_ACQ_DB_ENTRIES)
  {
    /* The ARFCN is not supported and was not hashable, so it can not be
    in the RR ACQ-DB. */
    return FALSE;
  }
  else
  {
    rr_acq_db_entry_t *entry = &rr_acq_db_data_ptr->rr_acq_db.array[hashed_arfcn];

    if (entry->valid)
    {
      if (sys_plmn_match(plmn_id, entry->plmn_id))
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
}


/**
 * See header file for description.
 */
rr_acq_db_result_t rr_acq_db_add_arfcn(
  ARFCN_T arfcn,
  sys_plmn_id_s_type plmn_id,
  const gas_id_t gas_id
)
{
  rr_acq_db_result_t db_result;
  sys_mcc_type       mcc;
  sys_mnc_type       mnc;
  boolean            plmn_is_undefined;
  boolean            mnc_includes_pcs_digit;
  uint16             hashed_arfcn;
  rr_acq_db_data_t  *rr_acq_db_data_ptr;

  // Obtain a pointer to the module data
  rr_acq_db_data_ptr = rr_acq_db_get_data_ptr();
  RR_NULL_CHECK_FATAL(rr_acq_db_data_ptr);

  hashed_arfcn = rr_acq_db_hash_arfcn(arfcn);

  if (hashed_arfcn == MAX_RR_ACQ_DB_ENTRIES)
  {
    db_result = RR_ACQ_DB_HASH_FAILED;

    MSG_GERAN_ERROR_2_G("rr_acq_db_hash_arfcn() failed to hash ARFCN (%d, %d)",
      arfcn.num,
      arfcn.band);
  }
  else
  {
    rr_acq_db_entry_t *entry = &rr_acq_db_data_ptr->rr_acq_db.array[hashed_arfcn];

    db_result = RR_ACQ_DB_ARFCN_ADDED;

    sys_plmn_get_mcc_mnc(plmn_id,
                         &plmn_is_undefined,
                         &mnc_includes_pcs_digit,
                         &mcc,
                         &mnc);

    if (entry->valid)
    {
      /* The ARFCN is already in the acquisition database.  If it is already in
      the correct PLMN then we just need to pull it out of the list and move it
      to the end to record that it has been seen most recently. */
      if (sys_plmn_match(plmn_id, entry->plmn_id))
      {
        list_pop_item(&rr_acq_db_data_ptr->rr_acq_db.list, &entry->list_link);
      }
      else
      {
        MSG_GERAN_HIGH_2_G("PLMN for ARFCN (%d, %d) changed", arfcn.num, arfcn.band);
        rr_acq_db_delete_all_entries_for_plmn(entry->plmn_id, gas_id);
        db_result = RR_ACQ_DB_PLMN_CHANGE;
      }
    }

    if (rr_acq_db_num_of_arfcns_for_plmn(plmn_id, gas_id) >= MAX_RR_ACQ_DB_ENTRIES_PER_PLMN)
    {
      /* We need to delete the oldest entry for this PLMN */
      rr_acq_db_entry_t *tmp;

      MSG_GERAN_HIGH_0_G("Max entries for PLMN reached, purging oldest entry");

      /* need to search the list from back to front to find the oldest entry -
      list utils provide no functionality for this so we use our own. */

      tmp = (rr_acq_db_entry_t *) list_peek_back(&rr_acq_db_data_ptr->rr_acq_db.list);
      while (tmp != NULL)
      {
        if (sys_plmn_match(tmp->plmn_id, plmn_id))
        {
          /* We've found a matching entry.  Invalidate it/delete it from the list */
          tmp->valid = FALSE;
          list_pop_item(&rr_acq_db_data_ptr->rr_acq_db.list, &tmp->list_link);
          break;
        }
        tmp = list_peek_prev(&rr_acq_db_data_ptr->rr_acq_db.list, &tmp->list_link);
      }
    }

    if (arfcn.band == SYS_BAND_PGSM_900 ||
        arfcn.band == SYS_BAND_EGSM_900)
    {
      MSG_GERAN_MED_3_G("Adding ARFCN %d (900 band) to acquisition database, PLMN %d-%d",
        arfcn.num,
        mcc,
        mnc);
    }
    else if (arfcn.band == SYS_BAND_DCS_1800)
    {
      MSG_GERAN_MED_3_G("Adding ARFCN %d (1800 band) to acquisition database, PLMN %d-%d",
        arfcn.num,
        mcc,
        mnc);
    }
    else if (arfcn.band == SYS_BAND_PCS_1900)
    {
      MSG_GERAN_MED_3_G("Adding ARFCN %d (1900 band) to acquisition database, PLMN %d-%d",
        arfcn.num,
        mcc,
        mnc);
    }
    else if (arfcn.band == SYS_BAND_CELL_850)
    {
      MSG_GERAN_MED_3_G("Adding ARFCN %d (850 band) to acquisition database, PLMN %d-%d",
        arfcn.num,
        mcc,
        mnc);
    }
    else
    {
      MSG_GERAN_MED_3_G("Adding ARFCN %d (unknown band) to acquisition database, PLMN %d-%d",
        arfcn.num,
        mcc,
        mnc);
    }

    entry->arfcn = arfcn;
    entry->plmn_id = plmn_id;
    entry->valid = TRUE;

    list_push_front(&rr_acq_db_data_ptr->rr_acq_db.list, &entry->list_link);
  }

#ifdef RR_ACQ_DB_DEBUG
  rr_acq_db_validate(rr_acq_db_data_ptr);
#endif /* RR_ACQ_DB_DEBUG */

  return(db_result);
}


/**
 * See header file for description.
 */
void rr_acq_db_populate_pscan_db(
  rr_l1_pscan_results_T * pscan_request,
  sys_band_mask_type      band_pref,
  sys_plmn_id_s_type      plmn_id,
  boolean                 requested_plmn_only,
  rr_pscan_request_type_e req_type,
  const gas_id_t          gas_id
)
{
  boolean plmn_is_undefined;
  boolean mnc_includes_pcs_digit;
  sys_mcc_type mcc;
  sys_mnc_type mnc;

  rr_acq_db_entry_t *entry;
  uint16 num_freqs = 0;
  rr_acq_db_data_t  *rr_acq_db_data_ptr;

  // Obtain a pointer to the module data
  rr_acq_db_data_ptr = rr_acq_db_get_data_ptr();
  RR_NULL_CHECK_FATAL(rr_acq_db_data_ptr);

  sys_plmn_get_mcc_mnc(plmn_id,
                       &plmn_is_undefined,
                       &mnc_includes_pcs_digit,
                       &mcc,
                       &mnc);

  MSG_5(MSG_SSID_DFLT,
        MSG_LEGACY_HIGH,
        "rr_acq_db_populate_pscan_db(band_pref = 0x%08x, plmn_id = %d-%d (%d), requested_plmn_only = %d)",
        band_pref,
        mcc,
        mnc,
        mnc_includes_pcs_digit,
        requested_plmn_only);

  MSG_GERAN_MED_1_G("Number of entries in rr_acq_db: %d", list_size(&rr_acq_db_data_ptr->rr_acq_db.list));

  memset(pscan_request, 0, sizeof(rr_l1_pscan_results_T));

  /* Iterate over the list adding entries from the requested PLMN */
  entry = (rr_acq_db_entry_t *) list_peek_front(&rr_acq_db_data_ptr->rr_acq_db.list);

  while ((entry != NULL) &&
         (pscan_request->num_frequencies < MAX_RR_ACQ_DB_PSCAN_FREQS_PER_PLMN) &&
         (num_freqs < RR_MAX_PSCAN_FREQ_SCAN))
  {
    boolean found_plmn_is_undefined;
    boolean found_mnc_includes_pcs_digit;
    sys_mcc_type found_mcc;
    sys_mnc_type found_mnc;

    sys_plmn_get_mcc_mnc(entry->plmn_id,
                         &found_plmn_is_undefined,
                         &found_mnc_includes_pcs_digit,
                         &found_mcc,
                         &found_mnc);
 
    if (sys_plmn_match(plmn_id, entry->plmn_id))
    {
      if (convert_sys_band_to_sys_band_mask(entry->arfcn.band) & band_pref)
      {
        MSG_GERAN_MED_3_G("Adding ARFCN %d (PLMN %d-%d) to power scan request (requested PLMN)",
          entry->arfcn.num,
          found_mcc,
          found_mnc);

        pscan_request->meas[num_freqs].arfcn = entry->arfcn;
        num_freqs++;
      }
    }

    entry = (rr_acq_db_entry_t *) list_peek_next(&rr_acq_db_data_ptr->rr_acq_db.list, &entry->list_link);
  }

#ifdef FEATURE_GERAN_X2G_ACQ_DB_SCAN
  if ( rr_is_sglte_mode_active(gas_id) &&
       req_type == RR_PSCAN_REQ_CELL_SELECTION)
  {
    if (rr_irat_acq_db_valid(gas_id))
    {
      uint16 start_num_freqs = num_freqs;
      uint16 num_duplicates = 0;
      rr_irat_acq_db_t *rr_irat_acq_db_data_ptr = rr_irat_acq_db_get_data_ptr();
      uint8 arfcn_index = 0;

      RR_NULL_CHECK_FATAL(rr_irat_acq_db_data_ptr);

      MSG_GERAN_MED_3_G("IRAT acq DB valid adding to power scan. current num freq:%d RAT type:%d irat num freq:%d", start_num_freqs,
                                                                                                                    rr_irat_acq_db_data_ptr->rat,
                                                                                                                    rr_irat_acq_db_data_ptr->freq_list.num_arfcns);

      for (arfcn_index = 0; 
           ( (arfcn_index < rr_irat_acq_db_data_ptr->freq_list.num_arfcns) &&
             (pscan_request->num_frequencies < MAX_RR_ACQ_DB_PSCAN_FREQS) &&
             (num_freqs < RR_MAX_PSCAN_FREQ_SCAN) );
           arfcn_index++ )
      {
        if (rr_pscan_arfcn_present(pscan_request, num_freqs, rr_irat_acq_db_data_ptr->freq_list.arfcn[arfcn_index], gas_id) == TRUE)
        {
          num_duplicates++;
        }
        else
        {
          pscan_request->meas[num_freqs].arfcn = rr_irat_acq_db_data_ptr->freq_list.arfcn[arfcn_index];
          num_freqs++;
        }
      }

      MSG_GERAN_HIGH_2_G("IRAT acq DB added %d freq, duplicates %d",num_freqs - start_num_freqs, num_duplicates);
    }
    else
    {
      MSG_GERAN_MED_0_G("IRAT acq DB invalid, do not add to power scan");
    }
  }
#endif /*FEATURE_GERAN_X2G_ACQ_DB_SCAN*/

  if (requested_plmn_only == FALSE)
  {
    /* Now iterate over the list adding entries from other PLMNs */
    entry = (rr_acq_db_entry_t *) list_peek_front(&rr_acq_db_data_ptr->rr_acq_db.list);

    while ((entry != NULL) &&
           (pscan_request->num_frequencies < MAX_RR_ACQ_DB_PSCAN_FREQS) &&
           (num_freqs < RR_MAX_PSCAN_FREQ_SCAN))
    {
      boolean found_plmn_is_undefined;
      boolean found_mnc_includes_pcs_digit;
      sys_mcc_type found_mcc;
      sys_mnc_type found_mnc;

      sys_plmn_get_mcc_mnc(entry->plmn_id,
                           &found_plmn_is_undefined,
                           &found_mnc_includes_pcs_digit,
                           &found_mcc,
                           &found_mnc);
 
      if (!sys_plmn_match(plmn_id, entry->plmn_id))
      {
        if (convert_sys_band_to_sys_band_mask(entry->arfcn.band) & band_pref)
        {
          MSG_GERAN_MED_3_G("Adding ARFCN %d (PLMN %d-%d) to power scan request (other PLMN)",
            entry->arfcn.num,
            found_mcc,
            found_mnc);

          pscan_request->meas[num_freqs].arfcn = entry->arfcn;
          num_freqs++;
        }
      }

      entry = (rr_acq_db_entry_t *) list_peek_next(&rr_acq_db_data_ptr->rr_acq_db.list, &entry->list_link);
    }
  }

  pscan_request->num_frequencies = num_freqs;

  MSG_GERAN_HIGH_1_G("rr_acq_db_populate_pscan_db: found %d freqs", num_freqs);
}


/**
 * Push any entries for the requested PLMN to the front of the pscan_results
 * structure.  This maintains the relative signal-strength order of entries
 * where it can, e.g. all entries for the requested PLMN should end up in the
 * same order as they started in, just with the intermediate entries bumped
 * out towards the end of the list, so results like this:
 *
 * PLMN    A  B  B  A  A  B  B  A
 * ARFCN   1  4  2  3  5  6  9  8
 *
 * Would result in this after reordering with a requested PLMN of 'A':
 *
 * PLMN    A  A  A  A  B  B  B  B
 * ARFCN   1  3  5  8  4  2  6  9
 */
void rr_acq_db_prioritise_pscan_results(
  rr_l1_pscan_results_T *pscan_results,
  sys_plmn_id_s_type plmn_id,
  const gas_id_t gas_id
)
{
  int32 i;
  uint16 guard = 0;
  rr_l1_freq_power_T tmp;

  /* Here, the guard variable basically tracks the number of elements that we
  have actually moved to the front of the array.  This is used as the for-
  statement terminate condition as we do not wish to reorder these elements
  more than once, so when i finally moves towards the reordered elements we
  can terminate the sorting. */

  if (pscan_results->num_frequencies > 1)
  {
    for (i = pscan_results->num_frequencies - 1;
         (i >= guard) && (i < RR_MAX_PSCAN_FREQ_SCAN);
         --i)
    {
      tmp = pscan_results->meas[i];

      if (rr_acq_db_is_arfcn_in_plmn(tmp.arfcn, plmn_id, gas_id))
      {
        MSG_GERAN_MED_2_G("Prioritising ARFCN (%d, %d) for requested PLMN",
                tmp.arfcn.num,
                tmp.arfcn.band);

        /* Move the entry to the front of the list.  To achieve this, we need to
        move all entries from 0 to i-1 along by one entry (to make some space
        at the front of the list for the entry being moved). */

		memsmove(&pscan_results->meas[1],
				 ((sizeof(pscan_results->meas))-(sizeof(pscan_results->meas[0]))),
				 &pscan_results->meas[0],
				 (unsigned int) (sizeof(rr_l1_freq_power_T) * (unsigned int) i));

        pscan_results->meas[0] = tmp;

        /* Increment i because an element has been inserted at the front of the
        list, so our 'current' element has been moved. */
        i++;
        guard++;
      }
    }
  }
}

/**
 * check how many arfcn in acq db
 * @param band_pref band_pref information
*/
uint16 rr_num_of_arfcn_in_acq_db(sys_band_mask_type band_pref, const gas_id_t gas_id)
{
  rr_pscan_results_db_T   temp_pscan_db;
  rr_PLMN_list_req_info_T * rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  sys_plmn_id_s_type      plmn_id = rr_plmn_list_req_info_ptr->plmn_list.info[0].plmn;
  uint16                  num_of_arfcn = 0;

  rr_acq_db_populate_pscan_db(
                              &temp_pscan_db.pscan_results,   // pscan_request
                              band_pref,                  // band_pref
                              plmn_id,                    // plmn_id
                              FALSE,        // requested_plmn_only
                              RR_PSCAN_REQ_NONE,
                              gas_id                      // gas_id
                              );
  num_of_arfcn = temp_pscan_db.pscan_results.num_frequencies;
  
  return num_of_arfcn;
}


#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/**
 * check if the IRAT acq DB is valid to use
 * @param gas_id
 * @return IRAT acq db valid
*/
boolean rr_irat_acq_db_valid(const gas_id_t gas_id)
{
  rr_irat_acq_db_t *rr_irat_acq_db_data_ptr = rr_irat_acq_db_get_data_ptr();
  rr_PLMN_request_details_T * rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  boolean irat_acq_db_valid = FALSE;

  RR_NULL_CHECK_FATAL(rr_irat_acq_db_data_ptr);
  RR_NULL_CHECK_FATAL(rr_PLMN_request_details_ptr);

  if ( (rr_irat_acq_db_data_ptr->rat != RR_RAT_UNDEFINED) &&
       (rr_irat_acq_db_data_ptr->freq_list.num_arfcns > 0) )
  {

    if (FALSE == rr_is_any_cell_selection_mode(gas_id))
    {
      /*Check if the IRAT acq DB was received on the same PLMN as the current GERAN scell*/
      if (sys_plmn_match(rr_PLMN_request_details_ptr->requested_PLMN_id,
                         rr_irat_acq_db_data_ptr->plmn_id))
      {
        MSG_GERAN_HIGH_0_G("IRAT acq DB PLMN's match");
        irat_acq_db_valid = TRUE;
      }
      else /*PLMN does not match check if there is an equivalent*/
      {
        boolean eplmn_ok, ehplmn_ok;      

        eplmn_ok = ( rr_eplmn_list_equivalent_plmn(rr_PLMN_request_details_ptr->requested_PLMN_id, gas_id) &&
                     rr_eplmn_list_equivalent_plmn(rr_irat_acq_db_data_ptr->plmn_id, gas_id ));

        ehplmn_ok = ( rr_ehplmn_list_equivalent_plmn(rr_PLMN_request_details_ptr->requested_PLMN_id, gas_id) &&
                      rr_ehplmn_list_equivalent_plmn(rr_irat_acq_db_data_ptr->plmn_id, gas_id ));

        MSG_GERAN_MED_2_G("IRAT acq db eplmn_ok:%d ehplmn_ok:%d", eplmn_ok,
                                                                   ehplmn_ok);

        irat_acq_db_valid = eplmn_ok || ehplmn_ok;
      }
    }
    else
    {
      MSG_GERAN_HIGH_0_G("IRAT acq db, in any selection mode, not matching PLMN's");
    }
  }

  return irat_acq_db_valid;
}

#ifdef FEATURE_GERAN_HST_OPT
/**
 * Returns the high speed indicator for the current irat acq db db
 * @param void
 * @return boolean - High speed flag
*/
boolean rr_high_speed_ind_set(void)
{
  rr_irat_acq_db_t *rr_irat_acq_db_data_ptr = rr_irat_acq_db_get_data_ptr();

  RR_NULL_CHECK_FATAL(rr_irat_acq_db_data_ptr);
  return(rr_irat_acq_db_data_ptr->is_hst_cell);
}

/**
 * Copy GERAN freq info update to internal storage
 * @param freq_info_ptr 
 *        gas_id 
*/
void rr_store_lte_geran_freq_info_ind(geran_grr_lte_geran_freq_info_ind_t *freq_info_ptr,
                                      const gas_id_t gas_id)
{
  rr_irat_acq_db_t *rr_irat_acq_db_data_ptr = rr_irat_acq_db_get_data_ptr();
  uint8 i = 0;

  RR_NULL_CHECK_FATAL(rr_irat_acq_db_data_ptr);
  RR_NULL_CHECK_FATAL(freq_info_ptr);

  MSG_GERAN_HIGH_4_G("LTE GERAN freq update rxd freq_info_present %d, num frequencies %d, LTE PLMN Id %6x AND HST Flag %d",
                      freq_info_ptr->freq_info_present,
                      freq_info_ptr->freq_info.num_of_freq,
                      ( (freq_info_ptr->plmn_id.identity[0] << 16) + \
                        (freq_info_ptr->plmn_id.identity[1] << 8)  + \
                         freq_info_ptr->plmn_id.identity[2] ),
                       freq_info_ptr->is_hst_cell );
  
   if (freq_info_ptr->freq_info_present)
   {
     /*reset the store*/
     memset(&rr_irat_acq_db_data_ptr->freq_list, 0, sizeof(rr_irat_acq_db_data_ptr->freq_list));

     /*sanity check input*/
     if (freq_info_ptr->freq_info.num_of_freq > ARR_SIZE(freq_info_ptr->freq_info.freq_list))
     {
       MSG_GERAN_ERROR_2_G("Out of range freq list length:%d, truncating to %d", freq_info_ptr->freq_info.num_of_freq,
                                                                                      ARR_SIZE(freq_info_ptr->freq_info.freq_list));

       freq_info_ptr->freq_info.num_of_freq = ARR_SIZE(freq_info_ptr->freq_info.freq_list);
     }

     rr_irat_acq_db_data_ptr->rat = RR_RAT_LTE;
     rr_irat_acq_db_data_ptr->plmn_id = freq_info_ptr->plmn_id;
     rr_irat_acq_db_data_ptr->is_hst_cell = freq_info_ptr->is_hst_cell;

     for ( i=0; 
          (i < freq_info_ptr->freq_info.num_of_freq) &&
           (rr_irat_acq_db_data_ptr->freq_list.num_arfcns < ARR_SIZE(rr_irat_acq_db_data_ptr->freq_list.arfcn)); 
           i++)
     {
       rr_irat_acq_db_data_ptr->freq_list.arfcn[i].num = freq_info_ptr->freq_info.freq_list[i].arfcn;

       rr_irat_acq_db_data_ptr->freq_list.arfcn[i].band = (sys_band_T) rr_get_band_using_band_indicator(
                                                                  freq_info_ptr->freq_info.freq_list[i].arfcn,
                                                                  (si_psi_band_ind_T)freq_info_ptr->freq_info.freq_list[i].band_ind,
                                                                  gas_id
                                                                );
       rr_irat_acq_db_data_ptr->freq_list.num_arfcns++;
     }
   }
   else
   {
     MSG_GERAN_HIGH_0_G("LTE GERAN freq info not present, keep existing store");
   }
}
#endif /*FEATURE_GERAN_HST_OPT*/

/* EOF */