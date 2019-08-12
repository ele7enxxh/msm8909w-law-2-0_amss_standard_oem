/*============================================================================
  @file rr_g2x.c

  @brief This module provides various databases and APIs used in IRAT G2X
         reselection.

                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_g2x.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
#include "rr_seg_load.h"
#ifdef FEATURE_GSM_TDS
#include "tdsrrc_rr_if.h"
#endif /*FEATURE_GSM_TDS*/
#include "comdef.h"
#include "rr_g2x.h"
#include "list.h"
#include "gprs_mem.h"
#include "rr_grr.h"
#include "rr_general.h"
#ifdef FEATURE_WCDMA
#include "rrcrrif.h"
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
#include "lte_rrc_ext_api.h"
#endif /* FEATURE_LTE */
#include "rr_multi_sim.h"
#include "rr_ps_domain.h"
#include "geran_nv.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/* @brief Maximum number of entries that may be contained in the non-EPLMN
database.  This is fairly arbitrary and is mainly limited by the number of
timers available to RR. */
#define RR_G2X_BARRED_FREQUENCY_DB_SIZE 6U

/* @brief Entries will remain in the non-EPLN database for 20 minutes, or until
the UE performs cell selection or is redirected to the affected frequency. */
#define RR_G2X_BARRED_FREQUENCY_DB_TIMER_DURATION (20U*60U*1000U) /* 20 minutes */
#define RR_G2X_BARRED_FREQUENCY_DB_OPTIMIZED_TIMER_DURATION (20U*15U*1000U) /* 5 minutes */

/* @brief Maximum number of barred IRAT cells to track. */
#define RR_G2X_BARRED_CELL_DB_SIZE 6U

/* @brief In case of a reselection attempt towards a barred E-UTRAN cell, the
MS shall abandon further reselection attempts towards this E-UTRAN cell for a
period of up to 20 min. */
#define RR_G2X_BARRED_LTE_CELL_DB_TIMER_DURATION (20U*60U*1000U) /* 20 minutes */
#define RR_G2X_BARRED_LTE_CELL_DB_OPTIMIZED_TIMER_DURATION (20U*15U*1000U) /* 5 minutes */
#define RR_G2X_BARRED_WCDMA_CELL_DB_TIMER_DURATION  (5U*60U*1000U) /* 5 minutes */

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef struct
{
  list_link_type               list_link;

  /* The IRAT frequency that is locally barred. */
  rr_irat_frequency_t          frequency;

  /* When the timer expires, the frequency will be removed from the database.
  */
  rr_timer_id_e                timer;

  /* The reason the frequency has been locally barred is stored in the
  database so when a change in conditions is detected the presence of the
  frequency in the database can be re-evaluated. */
  rr_g2x_resel_failure_cause_e reason;

  /* Data related to the frequency in question - one of:
     - PLMN
     - LAI

     This field will only contain valid information if 'reason' is set
     to an appropriate value.
  */
  rr_g2x_resel_failure_data_t  data;

} rr_g2x_barred_frequency_db_entry_t;

typedef struct
{
  list_link_type               list_link;
  rr_irat_cell_t               cell;
  rr_timer_id_e                timer;
  rr_g2x_resel_failure_cause_e reason;
} rr_g2x_barred_cell_db_entry_t;

typedef struct
{
  boolean                          wcdma_valid;
  rr_g2x_cell_quality_data_wcdma_t wcdma;
} rr_g2x_cell_quality_data_t;

typedef struct
{
  /* Database of IRAT frequencies belonging to PLMNs that are not indicated
  as being equivalent to the serving cell or belong to a forbidden LAI (WCDMA)
  or Tracking Area (LTE) (see 3GPP 45.008). */
  list_type barred_frequency_db;

  /* Database of IRAT cells to which reselection is not allowed for some
  specific duration. */
  list_type barred_cell_db;

  /* 3GPP 45.008 section 6.6.5

  "The MS shall store the UTRAN cell RSCP suitability criterion parameters
  above, whenever decoded from a UTRAN FDD cell of an equivalent PLMN while
  attempting to camp on the UTRAN FDD cell.  The most recently decoded
  parameters from one UTRAN FDD cell of an equivalent PLMN are valid
  reselection criteria towards all UTRAN FDD cells.  This list of parameters
  shall be cleared after PLMN selection (see 3GPP TS 23.122)."

  This data should remain valid until PLMN selection occurs, however this is
  not always obvious to RR, e.g. if PLMN selection occurs on another RAT.  To
  this end, for the time being this data will simply be cleared on GSM
  deactivation, or on PLMN selection.
  */
  rr_g2x_cell_quality_data_t cell_quality_data;

} rr_g2x_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

static rr_g2x_data_t   rr_g2x_data[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_g2x_data_t*
 */
static rr_g2x_data_t *rr_g2x_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_g2x_data[as_index];
}

#ifdef FEATURE_WCDMA
static int rr_wcdma_cell_cmp(
  rr_wcdma_cell_t * a,
  rr_wcdma_cell_t * b
)
{
  if (a->uarfcn == b->uarfcn &&
      a->scrambling_code == b->scrambling_code &&
      a->diversity == b->diversity)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
static int rr_lte_cell_cmp(
  rr_lte_cell_t * a,
  rr_lte_cell_t * b
)
{
  if (a->earfcn == b->earfcn &&
      a->pcid == b->pcid)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
#endif /* FEATURE_LTE */

static int rr_irat_cell_cmp(
  rr_irat_cell_t * a,
  rr_irat_cell_t * b
)
{
  if (a->rat == b->rat)
  {
    switch (a->rat)
    {
#ifdef FEATURE_WCDMA
      case RR_RAT_WCDMA:
      {
        return rr_wcdma_cell_cmp(&a->data.wcdma, &b->data.wcdma);
      } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
      case RR_RAT_LTE:
      {
        return rr_lte_cell_cmp(&a->data.lte, &b->data.lte);
      } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

      default:
      {
        MSG_GERAN_ERROR_1("Unknown RAT: %d", a->rat);
        return FALSE;
      }
    }
  }
  else
  {
    return 0;
  }
}

/**
  @defgroup barred_frequency_db
*/

/**
  @ingroup barred_frequency_db
  @brief Check whether an entry in the barred frequency database has a specific
         timer value associated with it.
*/
static int rr_g2x_barred_frequency_db_entry_cmp_timer_fn(
  void * item_ptr,
  void * compare_val
)
{
  rr_g2x_barred_frequency_db_entry_t * entry;
  rr_timer_id_e                      * timer_id;

  RR_NULL_CHECK_RETURN_PARAM(item_ptr, 0);
  RR_NULL_CHECK_RETURN_PARAM(compare_val, 0);

  entry = (rr_g2x_barred_frequency_db_entry_t *) item_ptr;
  timer_id = (rr_timer_id_e *) compare_val;

  if (entry->timer == *timer_id)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  @ingroup barred_frequency_db
  @brief Check whether an entry in the barred frequency database has a specific
         frequency associated with it.
*/
static int rr_g2x_barred_frequency_db_entry_cmp_frequency_fn(
  void * item_ptr,
  void * compare_val
)
{
  rr_g2x_barred_frequency_db_entry_t * entry;
  rr_irat_frequency_t                * frequency;

  RR_NULL_CHECK_RETURN_PARAM(item_ptr, 0);
  RR_NULL_CHECK_RETURN_PARAM(compare_val, 0);

  entry = (rr_g2x_barred_frequency_db_entry_t *) item_ptr;
  frequency = (rr_irat_frequency_t *) compare_val;

  if (entry->frequency.rat == frequency->rat)
  {
    switch (entry->frequency.rat)
    {
#ifdef FEATURE_WCDMA
      case RR_RAT_WCDMA:
      {
        if (entry->frequency.data.wcdma == frequency->data.wcdma)
        {
          return 1;
        }
        break;
      } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS      
       case RR_RAT_UTRAN_TDD:
       {
         if (entry->frequency.data.tdscdma == frequency->data.tdscdma)
         {
           return 1;        
         }        
         break;
       } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */


#ifdef FEATURE_LTE
      case RR_RAT_LTE:
      {
        if (entry->frequency.data.lte == frequency->data.lte)
        {
          return 1;
        }
        break;
      } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

      default:
      {
        MSG_GERAN_ERROR_1("Unknown RAT: %d", entry->frequency.rat);
        return 0;
      }
    }
  }

  return 0;
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
/**
  @ingroup barred_frequency_db
  @brief Allocates a timer for a new entry in the barred frequency database.
  @return Id of newly allocated timer, or RR_TIMER_UNDEFINED if no timer is
          available.
*/
static rr_timer_id_e rr_g2x_barred_frequency_db_allocate_timer(
  const gas_id_t gas_id
)
{
  rr_timer_id_e timers[RR_G2X_BARRED_FREQUENCY_DB_SIZE] =
  {
    RR_G2X_BARRED_FREQUENCY_TIMER_1,
    RR_G2X_BARRED_FREQUENCY_TIMER_2,
    RR_G2X_BARRED_FREQUENCY_TIMER_3,
    RR_G2X_BARRED_FREQUENCY_TIMER_4,
    RR_G2X_BARRED_FREQUENCY_TIMER_5,
    RR_G2X_BARRED_FREQUENCY_TIMER_6,
  };

  uint32 i;
  rr_g2x_data_t *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  /* Search the barred frequency database for an entry matching each timer in
  turn.  When one of the timers is not found in the database, return it. */
  for (i = 0; i < RR_G2X_BARRED_FREQUENCY_DB_SIZE; ++i)
  {
    void * ptr = list_linear_search(
      &rr_g2x_data_ptr->barred_frequency_db,
      rr_g2x_barred_frequency_db_entry_cmp_timer_fn,
      &timers[i]
    );
    if (ptr == NULL)
    {
      return timers[i];
    }
  }

  return RR_TIMER_UNDEFINED;
}

/**
  @ingroup barred_frequency_db
  @brief Adds a frequency to the barred frequency database.
  @param frequency The frequency to add to the database.
  @param reason The reason that reselection to cells on that frequency is
         temporarily barred.
  @param data Optional data associated with the frequency.
  @param time_to_reselect The time in milliseconds for which reselection to the
         frequency shall be barred.
*/
static void rr_g2x_barred_frequency_db_add_frequency(
  rr_irat_frequency_t            frequency,
  rr_g2x_resel_failure_cause_e   reason,
  rr_g2x_resel_failure_data_t  * data,
  uint32                         time_to_reselect,
  const gas_id_t                 gas_id
)
{
  rr_g2x_barred_frequency_db_entry_t * entry = NULL;
  rr_g2x_data_t *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  RR_ASSERT_RETURN_VOID(list_size(&rr_g2x_data_ptr->barred_frequency_db) <= RR_G2X_BARRED_FREQUENCY_DB_SIZE);

  if (list_size(&rr_g2x_data_ptr->barred_frequency_db) == RR_G2X_BARRED_FREQUENCY_DB_SIZE)
  {
    /* Database is full, so pop of the oldest entry in the database and reuse
    it. */
    entry = (rr_g2x_barred_frequency_db_entry_t *) list_pop_front(
      &rr_g2x_data_ptr->barred_frequency_db
    );
    if (entry != NULL)
    {
      (void) rr_timer_cancel(entry->timer, gas_id);
    }
    else
    {
      RR_ASSERT_RETURN_VOID(entry != NULL); /* If memory allocation fails then return immediately. */
    }
  }
  else
  {
    entry = (rr_g2x_barred_frequency_db_entry_t *) GPRS_MEM_CALLOC(sizeof(rr_g2x_barred_frequency_db_entry_t));

    /* If memory allocation fails then return immediately. */
    RR_ASSERT_RETURN_VOID(entry != NULL);

    entry->timer = rr_g2x_barred_frequency_db_allocate_timer(gas_id);

    /* If we failed to get a timer then free up the memory and return. */
    if (entry->timer == RR_TIMER_UNDEFINED)
    {
      MSG_GERAN_ERROR_0_G("rr_g2x_barred_frequency_db_allocate_timer() failed");
      GPRS_MEM_FREE(entry);
      return;
    }
  }

  entry->frequency = frequency;
  entry->reason = reason;
  if (data != NULL)
  {
    entry->data = *data;
  }
  rr_timer_start(entry->timer, time_to_reselect, gas_id);

  list_push_back(&rr_g2x_data_ptr->barred_frequency_db, &entry->list_link);
}
#endif /* FEATURE_WCDMA || FEATURE_LTE */

/**
  @ingroup barred_frequency_db
  @brief Display the contents of the barred frequency database.
*/
static void rr_g2x_barred_frequency_db_dump(const gas_id_t gas_id)
{
  uint32                        i = 0;
  rr_g2x_barred_frequency_db_entry_t * entry;
  rr_g2x_data_t *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  MSG_GERAN_MED_1_G(
    "barred_frequency_db.count: %d",
    list_size(&rr_g2x_data_ptr->barred_frequency_db)
  );

  entry = (rr_g2x_barred_frequency_db_entry_t *) list_peek_front(&rr_g2x_data_ptr->barred_frequency_db);

  while (entry != NULL)
  {
    switch (entry->frequency.rat)
    {
#ifdef FEATURE_WCDMA
      case RR_RAT_WCDMA:
      {
        MSG_GERAN_MED_3_G(
          "barred_frequency_db.entries[%d]: WCDMA UARFCN %d, reason %d",
          i,
          entry->frequency.data.wcdma, entry->reason
        );
        break;
      } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
      case RR_RAT_UTRAN_TDD:
      {
        MSG_GERAN_MED_3_G(
          "barred_frequency_db.entries[%d]: TDSCDMA UARFCN %d, reason %d",
          i,
          entry->frequency.data.tdscdma, entry->reason
        );
        break;
      }
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
      case RR_RAT_LTE:
      {
        MSG_GERAN_MED_3_G(
          "barred_frequency_db.entries[%d]: LTE EARFCN %d, reason %d",
          i,
          entry->frequency.data.lte, entry->reason
        );
        break;
      } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

      default:
      {
        MSG_GERAN_ERROR_0_G("barred_frequency_db.entries[%d]: Unknown RAT");
      }
    }

    i++;

    entry = (rr_g2x_barred_frequency_db_entry_t *) list_peek_next(
      &rr_g2x_data_ptr->barred_frequency_db,
      &entry->list_link
    );
  }
}

/**
  @defgroup barred_cell_db
*/

/**
  @ingroup barred_cell_db
  @brief Check whether an entry in the barred cell database has a specific
         timer value associated with it.
*/
static int rr_g2x_barred_cell_db_entry_cmp_timer_fn(
  void * item_ptr,
  void * compare_val
)
{
  rr_g2x_barred_cell_db_entry_t * entry;
  rr_timer_id_e                 * timer_id;

  RR_NULL_CHECK_RETURN_PARAM(item_ptr, 0);
  RR_NULL_CHECK_RETURN_PARAM(compare_val, 0);

  entry = (rr_g2x_barred_cell_db_entry_t *) item_ptr;
  timer_id = (rr_timer_id_e *) compare_val;

  if (entry->timer == *timer_id)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  @ingroup barred_cell_db
  @brief Compare an entry in the barred cell database with an IRAT cell type.
  @return non-zero value if the entry in the barred cell database matches the
          cell specified, zero if they do not match.
*/
static int rr_g2x_barred_cell_db_entry_cmp_cell_fn(
  void * item_ptr,
  void * compare_val
)
{
  rr_g2x_barred_cell_db_entry_t * entry;
  rr_irat_cell_t                * cell;

  RR_NULL_CHECK_RETURN_PARAM(item_ptr, 0);
  RR_NULL_CHECK_RETURN_PARAM(compare_val, 0);

  entry = (rr_g2x_barred_cell_db_entry_t *) item_ptr;
  cell = (rr_irat_cell_t *) compare_val;

  return rr_irat_cell_cmp(&entry->cell, cell);
}

static rr_timer_id_e rr_g2x_barred_cell_db_allocate_timer(
  const gas_id_t gas_id
)
{
  rr_timer_id_e timers[RR_G2X_BARRED_CELL_DB_SIZE] =
  {
    RR_G2X_BARRED_CELL_TIMER_1,
    RR_G2X_BARRED_CELL_TIMER_2,
    RR_G2X_BARRED_CELL_TIMER_3,
    RR_G2X_BARRED_CELL_TIMER_4,
    RR_G2X_BARRED_CELL_TIMER_5,
    RR_G2X_BARRED_CELL_TIMER_6
  };

  uint32 i;
  rr_g2x_data_t *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  /* Search the barred cell database for an entry matching each timer in
  turn.  When one of the timers is not found in the database, return it. */
  for (i = 0; i < RR_G2X_BARRED_CELL_DB_SIZE; ++i)
  {
    void * ptr = list_linear_search(
      &rr_g2x_data_ptr->barred_cell_db,
      rr_g2x_barred_cell_db_entry_cmp_timer_fn,
      &timers[i]
    );
    if (ptr == NULL)
    {
      return timers[i];
    }
  }

  return RR_TIMER_UNDEFINED;
}

static void rr_g2x_barred_cell_db_add_cell(
  rr_irat_cell_t               * cell,
  uint32                         barred_time,
  rr_g2x_resel_failure_cause_e   reason,
  const gas_id_t                 gas_id
)
{
  rr_g2x_barred_cell_db_entry_t * entry = NULL;
  rr_g2x_data_t *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  RR_NULL_CHECK_RETURN_VOID(cell);

  if (list_size(&rr_g2x_data_ptr->barred_cell_db) == RR_G2X_BARRED_CELL_DB_SIZE)
  {
    /* Database is full, so pop of the oldest entry in the database and reuse
    it. */
    entry = (rr_g2x_barred_cell_db_entry_t *) list_pop_front(
      &rr_g2x_data_ptr->barred_cell_db
    );

    if (entry != NULL)
    {
      (void) rr_timer_cancel(entry->timer, gas_id);
    }
	else
    { 
      RR_ASSERT_RETURN_VOID(entry != NULL); /* If memory allocation fails then return immediately. */
    }
  }
  else
  {
    entry = GPRS_MEM_CALLOC(sizeof(rr_g2x_barred_cell_db_entry_t));

    RR_ASSERT_RETURN_VOID(entry != NULL);

    entry->timer = rr_g2x_barred_cell_db_allocate_timer(gas_id);

    if (entry->timer == RR_TIMER_UNDEFINED)
    {
      MSG_GERAN_ERROR_0_G("rr_g2x_barred_frequency_db_allocate_timer() failed");
      GPRS_MEM_FREE(entry);
      return;
    }
  }

  entry->cell = *cell;
  entry->reason = reason;

  if (barred_time > 0)
  {
    rr_timer_start(entry->timer, barred_time, gas_id);
  }
  else
  {
    switch (cell->rat)
    {
#ifdef FEATURE_WCDMA
      case RR_RAT_WCDMA:
      {
        MSG_HIGH("No time to reselect specified, barring UARFCN %d PSC %d indefinitely",
                 cell->data.wcdma.uarfcn, cell->data.wcdma.scrambling_code, 0);
        break;
      } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
      case RR_RAT_LTE:
      {
        MSG_HIGH("No time to reselect specified, barring EARFCN %d PCID %d indefinitely",
                 cell->data.lte.earfcn, cell->data.lte.pcid, 0);
        break;
      } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
      case RR_RAT_UTRAN_TDD:
      {
        MSG_HIGH("No time to reselect specified, barring UARFCN %d CPID %d indefinitely",
                 cell->data.utran_tdd.uarfcn, cell->data.utran_tdd.cpid, 0);
        break;
      } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

      default:
      {
        MSG_ERROR("Unknown RAT: %d", cell->rat, 0, 0);
      }
    }
  }

  list_push_back(&rr_g2x_data_ptr->barred_cell_db, &entry->list_link);
}

static void rr_g2x_barred_cell_db_dump(const gas_id_t gas_id)
{
  uint32                          i = 0;
  rr_g2x_barred_cell_db_entry_t * entry;
  rr_g2x_data_t *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  MSG_GERAN_MED_1_G("barred_cell_db.count: %d", list_size(&rr_g2x_data_ptr->barred_cell_db));

  entry = (rr_g2x_barred_cell_db_entry_t *) list_peek_front(&rr_g2x_data_ptr->barred_cell_db);

  while (entry != NULL)
  {
    switch (entry->cell.rat)
    {
#ifdef FEATURE_WCDMA
      case RR_RAT_WCDMA:
      {
        MSG_5(MSG_SSID_DFLT,
              MSG_LEGACY_HIGH,
              "barred_cell_db.entries[%d]: WCDMA UARFCN %d, PSC %d, reason %d, time (ms): %d",
              i,
              entry->cell.data.wcdma.uarfcn,
              entry->cell.data.wcdma.scrambling_code,
              entry->reason,
              rr_timer_get_time_remaining(entry->timer, gas_id));
        break;
      } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
      case RR_RAT_LTE:
      {
        MSG_5(MSG_SSID_DFLT,
              MSG_LEGACY_HIGH,
              "barred_cell_db.entries[%d]: LTE EARFCN %d, PCID %d, reason %d, time (ms): %d",
              i,
              entry->cell.data.lte.earfcn,
              entry->cell.data.lte.pcid,
              entry->reason,
              rr_timer_get_time_remaining(entry->timer, gas_id));
        break;
      } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

      default:
      {
        MSG_GERAN_ERROR_0_G("barred_cell_db.entries[%d]: Unknown RAT");
      }
    }

    i++;

    entry = (rr_g2x_barred_cell_db_entry_t *) list_peek_next(
      &rr_g2x_data_ptr->barred_cell_db,
      &entry->list_link
    );
  }
}

#ifdef FEATURE_LTE
static boolean rr_g2x_search_cell_in_pcid_to_ta_list(
  rr_irat_cell_t     * cell,
  uint8              * freq_count,
  uint16             * freq_list,
  rr_pcid_group_ie_t * pcid_to_ta_mapping,
  const gas_id_t       gas_id
)
{
  gprs_scell_info_t                    * scell = rr_gprs_get_scell_info(gas_id);
  rr_lte_neighbour_list_t              * lte_neighbour_list;
  rr_si2quater_ro_pcid_to_ta_mapping_t * pcid_to_ta_mapping_ptr;
  uint8                                  i, j, k;
  uint8                                  freq_indices_count;
  uint8                                  freq_index;
  uint16                                 earfcn = 0;
  boolean                                found = FALSE;
  uint8                                  found_index = 0;
  
  RR_NULL_CHECK_FATAL(scell);
  lte_neighbour_list = &scell->lte_neighbour_list;

  for (i = 0;
       !found &&
       i < lte_neighbour_list->eutran_pcid_to_ta_mapping_count;
       i++)
  {
    freq_indices_count = lte_neighbour_list->eutran_pcid_to_ta_mapping[i].eutran_frequency_indices_count;
    
    for (j = 0;
         !found && j < freq_indices_count;
         j++)
    {
      freq_index = lte_neighbour_list->eutran_pcid_to_ta_mapping[i].eutran_frequency_indices[j];
      
      if (freq_index < lte_neighbour_list->count)
      {
        earfcn = lte_neighbour_list->entries[freq_index].frequency.earfcn;
      }
      else
      {
        MSG_GERAN_ERROR_1_G("PCID to TA mapping contains invalid frequency index: %d", freq_index);
      }
      
      if(cell->data.lte.earfcn == earfcn)
      {
        /* Frequency matched. Check for PCID */
        pcid_to_ta_mapping_ptr = &lte_neighbour_list->eutran_pcid_to_ta_mapping[i].pcid_to_ta_mapping;

        for (k = 0;
             k < pcid_to_ta_mapping_ptr->pcid_to_ta_mapping_group_count;
             k++)
        {
          if (cell->data.lte.pcid <= RR_LTE_PCID_MAX &&
              RR_PCID_GROUP_IE_GET(pcid_to_ta_mapping_ptr->pcid_to_ta_mapping_group[k].bitmap, cell->data.lte.pcid))
          {
            /* PCID matched. Copy PCID to TA mapping */
            *pcid_to_ta_mapping = pcid_to_ta_mapping_ptr->pcid_to_ta_mapping_group[k];
            found_index = i;
            found = TRUE;
            break;
          }
        }
      }
    }
  }

  if (found)
  {
    /* Copy frequencies */
    freq_indices_count = lte_neighbour_list->eutran_pcid_to_ta_mapping[found_index].eutran_frequency_indices_count;

    for (i = 0, j = 0; i < freq_indices_count; i++)
    {
      freq_index = lte_neighbour_list->eutran_pcid_to_ta_mapping[found_index].eutran_frequency_indices[i];

      if (freq_index < lte_neighbour_list->count)
      {
        freq_list[j] = lte_neighbour_list->entries[freq_index].frequency.earfcn;
        ++j;
      }
    }
    
    *freq_count = j;
  }
  
  return found;
}

static void rr_g2x_update_barred_db_after_forbidden_ta(
  rr_irat_cell_t               * cell,
  rr_g2x_resel_failure_data_t  * data,
  const gas_id_t                 gas_id
)
{
  uint16                i, j;
  uint32                time_to_reselect;
  boolean               found;
  uint8                 freq_count = 0;
  uint16                freq_list[MAX_EUTRAN_PCID_TO_TA_MAPPING_EUTRAN_FREQUENCY_INDICES];
  rr_pcid_group_ie_t    pcid_to_ta_mapping;
  rr_irat_cell_t        barred_cell;
  rr_irat_frequency_t   barred_freq;

  RR_NULL_CHECK_RETURN_VOID(cell);

  if (data != NULL)
  {
    time_to_reselect = data->time_to_reselect * 1000;
  }
  else
  {
    time_to_reselect = RR_G2X_BARRED_LTE_CELL_DB_OPTIMIZED_TIMER_DURATION;
  }

  found = rr_g2x_search_cell_in_pcid_to_ta_list(
            cell,
            &freq_count,
            freq_list, 
            &pcid_to_ta_mapping,
            gas_id
          );

  if (found)
  {
    /* PCID to TA mapping is available: bar cells of same TA. */
    for (i = 0; i < freq_count; i++)
    {
      barred_cell.rat = RR_RAT_LTE;
      barred_cell.data.lte.earfcn = freq_list[i];

      for (j = 0; j <= RR_LTE_PCID_MAX; j++)
      {
        if (RR_PCID_GROUP_IE_GET(pcid_to_ta_mapping.bitmap, j))
        {
          barred_cell.data.lte.pcid = j;
          MSG_GERAN_HIGH_2_G("[PCID_to_TA] Bar cell: earfcn=%d, pcid=%d", barred_cell.data.lte.earfcn, barred_cell.data.lte.pcid);

          rr_g2x_barred_cell_db_add_cell(
            &barred_cell,
            time_to_reselect,
            RR_G2X_RESEL_FAILURE_CAUSE_FORBIDDEN_TA,
            gas_id
          );
        }
      }
    }
    
    rr_g2x_barred_cell_db_dump(gas_id);
  }
  else
  {
    /* PCID to TA mapping is not available: bar the frequency. */
    barred_freq.rat = RR_RAT_LTE;
    barred_freq.data.lte = cell->data.lte.earfcn;
    MSG_GERAN_HIGH_1_G("[PCID_to_TA] Bar freq: earfcn=%d", barred_freq.data.lte);

    rr_g2x_barred_frequency_db_add_frequency(
      barred_freq,
      RR_G2X_RESEL_FAILURE_CAUSE_FORBIDDEN_TA,
      data,
      RR_G2X_BARRED_FREQUENCY_DB_OPTIMIZED_TIMER_DURATION,
      gas_id
    );

    rr_g2x_barred_frequency_db_dump(gas_id);
  }
}
#endif /* FEATURE_LTE */

#ifdef FEATURE_WCDMA
static void rr_g2x_cell_quality_data_store(
  rr_rat_e rat,
  rr_g2x_cell_quality_data_u cell_quality_data,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_g2x_data_t *rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  switch (rat)
  {
    case RR_RAT_WCDMA:
    {
      rr_g2x_data_ptr->cell_quality_data.wcdma_valid = TRUE;
      rr_g2x_data_ptr->cell_quality_data.wcdma = cell_quality_data.wcdma;
      break;
    } /* RR_RAT_WCDMA */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unknown RAT: %d", rat);
    }
  }
}

/**
  @ingroup cell_quality_data
  @brief Display the contents of the Cell Quality data store.
*/
static void rr_g2x_cell_quality_data_dump(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_g2x_data_t *rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  if (rr_g2x_data_ptr->cell_quality_data.wcdma_valid)
  {
    MSG_GERAN_MED_0_G("cell_quality_data.wcdma.valid = TRUE");

    MSG_GERAN_MED_2_G(
      "  Squalmin: %d, Srxlevmin: %d",
      rr_g2x_data_ptr->cell_quality_data.wcdma.squalmin,
      rr_g2x_data_ptr->cell_quality_data.wcdma.srxlevmin
    );
  }
}
#endif /* FEATURE_WCDMA */

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

void rr_g2x_init(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_g2x_data_t *rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  memset(rr_g2x_data_ptr, 0, sizeof(rr_g2x_data_t));

  list_init(&rr_g2x_data_ptr->barred_frequency_db);
  list_init(&rr_g2x_data_ptr->barred_cell_db);
}

/**
  @ingroup barred_frequency_db
  @brief Remove all entries from the barred frequency database, cancel timers
         and free any allocated memory.
*/
void rr_g2x_barred_frequency_db_clear(const gas_id_t gas_id)
{
  rr_g2x_barred_frequency_db_entry_t * entry;
  rr_g2x_data_t *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  entry = (rr_g2x_barred_frequency_db_entry_t *) list_pop_front(
    &rr_g2x_data_ptr->barred_frequency_db
  );

  while (entry != NULL)
  {
    (void) rr_timer_cancel(entry->timer, gas_id);

    GPRS_MEM_FREE(entry);
    entry = (rr_g2x_barred_frequency_db_entry_t *) list_pop_front(
      &rr_g2x_data_ptr->barred_frequency_db
    );
  }
}

/**
  @ingroup barred_cell_db
  @brief Remove all entries from the barred cell database, cancel timers and
         free any allocated memory.
*/
void rr_g2x_barred_cell_db_clear(const gas_id_t gas_id)
{
  rr_g2x_barred_cell_db_entry_t * entry;
  rr_g2x_data_t *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  entry = (rr_g2x_barred_cell_db_entry_t *) list_pop_front(
    &rr_g2x_data_ptr->barred_cell_db
  );

  while (entry != NULL)
  {
    (void) rr_timer_cancel(entry->timer, gas_id);

    GPRS_MEM_FREE(entry);
    entry = (rr_g2x_barred_cell_db_entry_t *) list_pop_front(
      &rr_g2x_data_ptr->barred_cell_db
    );
  }
}

/**
  @ingroup cell_quality_data
  @brief Remove all entries from the cell quality failure database, cancel
         timers and free any allocated memory.
*/
void rr_g2x_cell_quality_data_clear(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_g2x_data_t *rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  rr_g2x_data_ptr->cell_quality_data.wcdma_valid = FALSE;
}

void rr_g2x_resel_failed(
  rr_irat_cell_t               * cell,
  rr_g2x_resel_failure_cause_e   cause,
  rr_g2x_resel_failure_data_t  * data,
  const gas_id_t                 gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(cell);

  MSG_GERAN_HIGH_1_G("G2X reselection failed, cause: %d", cause);

  switch (cause)
  {
    case RR_G2X_RESEL_FAILURE_CAUSE_FORBIDDEN_LA:
    case RR_G2X_RESEL_FAILURE_CAUSE_PLMN_MISMATCH:
    {
      switch (cell->rat)
      {
#ifdef FEATURE_WCDMA
        case RR_RAT_WCDMA:
        {
          rr_irat_frequency_t       frequency;
          uint32                    time_to_reselect_wcdma = RR_G2X_BARRED_FREQUENCY_DB_OPTIMIZED_TIMER_DURATION;

          frequency.rat = RR_RAT_WCDMA;
          frequency.data.wcdma = cell->data.wcdma.uarfcn;
          if (data != NULL)
          {
            time_to_reselect_wcdma = data->time_to_reselect * 1000;
          }

          rr_g2x_barred_frequency_db_add_frequency(
            frequency,
            cause,
            data,
            time_to_reselect_wcdma,
            gas_id
          );
          rr_g2x_barred_frequency_db_dump(gas_id);
          break;
        } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
        case RR_RAT_UTRAN_TDD:
        {
          rr_irat_frequency_t            frequency;
          uint32   time_to_reselect_tds = RR_G2X_BARRED_FREQUENCY_DB_OPTIMIZED_TIMER_DURATION;
          
          frequency.rat = RR_RAT_UTRAN_TDD;
          frequency.data.tdscdma = cell->data.utran_tdd.uarfcn;
          
          if (data != NULL)
          {
            time_to_reselect_tds = data->time_to_reselect * 1000;
          }
          
          rr_g2x_barred_frequency_db_add_frequency(
            frequency,
            cause,
            data,
            time_to_reselect_tds,
            gas_id
          );
          rr_g2x_barred_frequency_db_dump(gas_id);
          break;
        }
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
        case RR_RAT_LTE:
        {
          rr_irat_frequency_t            frequency;
          uint32   time_to_reselect_lte = RR_G2X_BARRED_FREQUENCY_DB_OPTIMIZED_TIMER_DURATION;

          frequency.rat = RR_RAT_LTE;
          frequency.data.lte = cell->data.lte.earfcn;
          if (data != NULL)
          {
            time_to_reselect_lte = data->time_to_reselect * 1000;
          }

          rr_g2x_barred_frequency_db_add_frequency(
            frequency,
            cause,
            data,
            time_to_reselect_lte,
            gas_id
          );
          rr_g2x_barred_frequency_db_dump(gas_id);
          break;
        } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

        default:
        {
          MSG_GERAN_ERROR_1_G("Unknown RAT: %d", cell->rat);
        }
      }
      break;
    } /* FORBIDDEN_LA / FORBIDDEN_TA / PLMN_MISMATCH */

    case RR_G2X_RESEL_FAILURE_CAUSE_FORBIDDEN_TA:
    {
      switch (cell->rat)
      {
#ifdef FEATURE_LTE
        case RR_RAT_LTE:
        {
          rr_g2x_update_barred_db_after_forbidden_ta(cell, data, gas_id);
          break;
         } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

        default:
        {
          MSG_GERAN_ERROR_1_G("Unknown RAT: %d", cell->rat);
        }
      }
      break;
    }
        

    case RR_G2X_RESEL_FAILURE_CAUSE_CELL_QUALITY_FAILURE:
    {
      switch (cell->rat)
      {
#ifdef FEATURE_WCDMA
        case RR_RAT_WCDMA:
        {
          if (data != NULL)
          {
            rr_g2x_cell_quality_data_store(
              RR_RAT_WCDMA,
              data->cell_quality_data,
              gas_id
            );
            rr_g2x_cell_quality_data_dump(gas_id);
          }
          else
          {
            MSG_GERAN_ERROR_0_G("data == NULL");
          }
          break;
        } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
        case RR_RAT_LTE:
        {
          /* No action necessary - cell quality not relevant to LTE IRAT */
          break;
        } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

        default:
        {
          MSG_GERAN_ERROR_1_G("Unknown RAT: %d", cell->rat);
        }
      }
      break;
    } /* RR_G2X_RESEL_FAILURE_CAUSE_WCDMA_CELL_QUALITY_FAILURE */

    case RR_G2X_RESEL_FAILURE_CAUSE_CELL_BARRED:
    {
      /* 45.008 section 6.6.5, 6.6.6 state:

      In case of a reselection attempt towards a barred UTRAN cell, the MS
      shall abandon further reselection attempts towards this UTRAN cell as
      defined by the Tbarred value on the barred UTRAN cell (see 3GPP TS
      25.331).

      In case of a reselection attempt towards a barred E-UTRAN cell, the MS
      shall abandon further reselection attempts towards this E-UTRAN cell
      for a period of up to 20 min.

      The LTE/UMTS/TDS network actually provides the time to bar the LTE cell, so
      this is used by RR.  It is passed in the failure data structure. */

      switch (cell->rat)
      {
#ifdef FEATURE_WCDMA
        case RR_RAT_WCDMA:
        {
            uint32 time_to_reselect_ms = RR_G2X_BARRED_WCDMA_CELL_DB_TIMER_DURATION;
            
            if (data != NULL)
            {
              time_to_reselect_ms = data->time_to_reselect * 1000;
            }
            
            rr_g2x_barred_cell_db_add_cell(
              cell,
              time_to_reselect_ms,
              cause,
              gas_id
            );
            rr_g2x_barred_cell_db_dump(gas_id);
          break;
        } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
        case RR_RAT_UTRAN_TDD:
        {
          uint32 time_to_reselect_utran_tdd = RR_G2X_BARRED_WCDMA_CELL_DB_TIMER_DURATION;
            
          if (data != NULL)
          {
            time_to_reselect_utran_tdd = data->time_to_reselect * 1000;
          }
            
          rr_g2x_barred_cell_db_add_cell(
            cell,
            time_to_reselect_utran_tdd,
            cause,
            gas_id
          );
          rr_g2x_barred_cell_db_dump(gas_id);
          break;
        } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */


#ifdef FEATURE_LTE
        case RR_RAT_LTE:
        {
          uint32 time_to_reselect_lte = RR_G2X_BARRED_LTE_CELL_DB_TIMER_DURATION;

          if (data != NULL)
          {
            time_to_reselect_lte = data->time_to_reselect * 1000;
          }

          rr_g2x_barred_cell_db_add_cell(
            cell,
            time_to_reselect_lte,
            cause,
            gas_id
          );
          rr_g2x_barred_cell_db_dump(gas_id);
          break;
        } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

        default:
        {
          MSG_GERAN_ERROR_1_G("Unknown RAT: %d", cell->rat);
        }
      }
      break;
    } /* RR_G2X_RESEL_FAILURE_CAUSE_CELL_BARRED */

    case RR_G2X_RESEL_FAILURE_CAUSE_FREQUENCY_BARRED:
    {
      switch (cell->rat)
      {
#ifdef FEATURE_WCDMA
        case RR_RAT_WCDMA:
        {
          /* Legacy rr_g2w.c does no-op here - possible that WCDMA RRC
          maintains cell / frequency barred information. */
          break;
        } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
        case RR_RAT_LTE:
        {
          /* Bar reselection to the frequency for the time specified by LTE,
          or 20 minutes if not specified. */
          uint32              time_to_reselect_lte = RR_G2X_BARRED_LTE_CELL_DB_TIMER_DURATION;
          rr_irat_frequency_t frequency;

          if (data != NULL)
          {
            time_to_reselect_lte = data->time_to_reselect * 1000;
          }

          frequency.rat = RR_RAT_LTE;
          frequency.data.lte = cell->data.lte.earfcn;

          rr_g2x_barred_frequency_db_add_frequency(
            frequency,
            cause,
            data,
            time_to_reselect_lte,
            gas_id
          );
          rr_g2x_barred_frequency_db_dump(gas_id);
          break;
        } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

        default:
        {
          MSG_GERAN_ERROR_1_G("Unknown RAT: %d", cell->rat);
        }
      }
      break;
    } /* RR_G2X_RESEL_FAILURE_CAUSE_FREQUENCY_BARRED */

    case RR_G2X_RESEL_FAILURE_CAUSE_CSG_FORBIDDEN:
    {
      switch (cell->rat)
      {
#ifdef FEATURE_WCDMA
        case RR_RAT_WCDMA:
        {
          rr_g2x_barred_cell_db_add_cell(
            cell,
            0,
            cause,
            gas_id
          );
          rr_g2x_barred_cell_db_dump(gas_id);
          break;
        } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

        default:
        {
          MSG_ERROR("Unknown RAT: %d", cell->rat, 0, 0);
        }
      }
      break;
    } /* RR_G2X_RESEL_FAILURE_CAUSE_CSG_FORBIDDEN */

    default:
    {
      switch (cell->rat)
      {
#ifdef FEATURE_WCDMA
        case RR_RAT_WCDMA:
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
        case RR_RAT_LTE:
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
        case RR_RAT_UTRAN_TDD:
#endif /*FEATURE_GSM_TDS*/
        {
          if (data != NULL)
          {
            if (data->time_to_reselect > 0)
            {
              rr_g2x_barred_cell_db_add_cell(
                cell,
                data->time_to_reselect * 1000,
                cause,
                gas_id
              );
              rr_g2x_barred_cell_db_dump(gas_id);
            }
          }
          break;
        } /* RR_RAT_WCDMA / RR_RAT_LTE */

        default:
        {
          MSG_GERAN_ERROR_1_G("Unknown RAT: %d", cell->rat);
        }
      }
    }
  }
}

#ifdef FEATURE_GSM_TDS
boolean rr_g2x_resel_to_utran_tdd_frequency_is_allowed(
  uint16             uarfcn,
  sys_band_mask_type band_pref_utran_tdd,
  boolean            any_cell_selection_mode_valid,
  boolean            any_cell_selection_mode,
  const gas_id_t     gas_id
)
{
  // Obtain a pointer to the module data
  rr_g2x_data_t *rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_GERAN_HIGH_0_G("TDS segment not loaded, TDS UARFCN not allowed");
    return FALSE;
  }

#ifdef FEATURE_TDS_SUB_ID_INTERFACE
  if (tdsrrc_is_arfcn_valid_sub_id(uarfcn, RR_GAS_ID_TO_AS_ID))
#else
  if (tdsrrc_is_arfcn_valid(uarfcn))
#endif /* FEATURE_TDS_SUB_ID_INTERFACE */
  {
    rr_irat_frequency_t frequency;

    frequency.rat = RR_RAT_UTRAN_TDD;
    frequency.data.lte = uarfcn;

    if (list_linear_search(&rr_g2x_data_ptr->barred_frequency_db, rr_g2x_barred_frequency_db_entry_cmp_frequency_fn, &frequency) != NULL)
    {
      return FALSE;
    }

    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA

boolean rr_g2x_is_uarfcn_valid(
  uint32 uarfcn,
  PLMN_id_T plmn_id,
  const gas_id_t gas_id
)
{
  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_GERAN_HIGH_1_G("WCDMA segment not loaded, UARFCN:%d is not supported",uarfcn);
    return(FALSE);
  }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* New RRC API only available under DSDS feature */
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
  return(rrc_is_arfcn_valid_sub_id(uarfcn, plmn_id, geran_map_gas_id_to_nas_id(gas_id)));
#else
  (void)plmn_id; /* not used */
  return(rrc_is_arfcn_valid_sub_id(uarfcn, geran_map_gas_id_to_nas_id(gas_id)));
#endif /* FEATURE_MCC_AND_850_800_BAND_COMBINATION */

#else

  /* Legacy RRC API without DSDS support */
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
  return(rrc_is_arfcn_valid(uarfcn, plmn_id));
#else
  (void)plmn_id; /* not used */
  return(rrc_is_arfcn_valid(uarfcn));
#endif /* FEATURE_MCC_AND_850_800_BAND_COMBINATION */

#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
}

boolean rr_g2x_resel_to_wcdma_frequency_is_allowed(
  uint16             uarfcn,
  sys_band_mask_type band_pref_wcdma,
  boolean            any_cell_selection_mode_valid,
  boolean            any_cell_selection_mode,
  const gas_id_t     gas_id
)
{
  gprs_scell_info_t *scell;
  rr_g2x_data_t *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  scell = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell);

  if (rr_g2x_is_uarfcn_valid(uarfcn, scell->gsm.location_area_identification.plmn_id, gas_id))
  {
    rr_irat_frequency_t frequency;

    frequency.rat = RR_RAT_WCDMA;
    frequency.data.lte = uarfcn;

    if (list_linear_search(&rr_g2x_data_ptr->barred_frequency_db, rr_g2x_barred_frequency_db_entry_cmp_frequency_fn, &frequency) != NULL)
    {
      return FALSE;
    }

    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
boolean rr_g2x_resel_to_lte_frequency_is_allowed(
  lte_earfcn_t             earfcn,
  sys_lte_band_mask_e_type band_pref_lte,
  boolean                  any_cell_selection_mode_valid,
  boolean                  any_cell_selection_mode,
  const gas_id_t           gas_id
)
{
  // Obtain a pointer to the module data
  rr_g2x_data_t *rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  if (lte_rrc_earfcn_is_supported(earfcn, band_pref_lte))
  {
    rr_irat_frequency_t frequency;

    frequency.rat = RR_RAT_LTE;
    frequency.data.lte = earfcn;

    if (list_linear_search(&rr_g2x_data_ptr->barred_frequency_db, rr_g2x_barred_frequency_db_entry_cmp_frequency_fn, &frequency) != NULL)
    {
      return FALSE;
    }

    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
  @brief Determines if reselection to the LTE cell specified is allowed or
         not.

  Checks in this function are based on whether the LTE cell is part of the
  Not Allowed Cells information provided by the serving GSM cell.
*/
static boolean rr_g2x_resel_to_lte_cell_is_allowed(
  rr_lte_cell_t * cell,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t       * scell;
  rr_lte_neighbour_list_t * lte_neighbour_list;
  uint32                    i;

  RR_NULL_CHECK_RETURN_PARAM(cell, FALSE);

  scell = rr_gprs_get_scell_info(gas_id); 
  lte_neighbour_list = &scell->lte_neighbour_list;

  for (i = 0;
       i < lte_neighbour_list->count &&
       i < MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE;
       ++i)
  {
    if (cell->earfcn == lte_neighbour_list->entries[i].frequency.earfcn)
    {
      if (cell->pcid <= RR_LTE_PCID_MAX)
      {
        if (RR_PCID_GROUP_IE_GET(lte_neighbour_list->entries[i].not_allowed_cells.bitmap, cell->pcid))
        {
          MSG_GERAN_HIGH_2_G(
            "Reselection to LTE cell EARFCN %d, PCID %d not allowed",
            cell->earfcn,
            cell->pcid
          );
          return FALSE;
        }
        else
        {
          return TRUE;
        }
      }
      else
      {
        MSG_GERAN_HIGH_2_G(
          "Reselection to LTE cell EARFCN %d, PCID %d not allowed (PCID out of range)",
          cell->earfcn,
          cell->pcid
        );
        return FALSE;
      }
    }
  }

  /* Cell is not in the LTE neighbour list? */
  return FALSE;
}
#endif /* FEATURE_LTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
/**
  @brief Determines if reselection to the UTRAN cell specified is allowed or not.

  Checks in this function are based on whether the URTAN cell is part of the
  3G Cell Reselection List provided by the serving GSM cell.
*/
static boolean rr_g2x_resel_to_utran_cell_is_allowed(
  rr_irat_cell_t *cell,
  const gas_id_t  gas_id
)
{
  boolean                           found = FALSE;
  byte                              i;
  rr_utran_rat_type_t               rat_type;
  word                              uarfcn;
  gprs_scell_info_t                *scell_ptr = rr_gprs_get_scell_info(gas_id);
  rr_utran_reselection_freq_list_T *si2q_freq_list = NULL;
  rr_utran_cell_list_T             *si2q_cell_list = NULL;
  wcdma_reselection_freq_list_T    *si2ter_freq_list = NULL;

  RR_NULL_CHECK_RETURN_PARAM(cell, FALSE);
  RR_NULL_CHECK_FATAL(scell_ptr);

  switch (cell->rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      rat_type  = RR_UTRAN_FDD;
      uarfcn    = (word) cell->data.wcdma.uarfcn;
      
      if (scell_ptr->gsm.wcdma_neighbor_list_si2ter.valid_data == TRUE)
      {
        si2ter_freq_list = &scell_ptr->gsm.wcdma_neighbor_list_si2ter.wcdma_freq_only_list;
      }
      break;
    }
#endif /* FEATURE_WCDMA */
      
#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      rat_type  = RR_UTRAN_TDD;
      uarfcn    = (word) cell->data.utran_tdd.uarfcn;
      
      if (scell_ptr->gsm.wcdma_neighbor_list_si2ter.valid_data == TRUE)
      {
        si2ter_freq_list = &scell_ptr->gsm.wcdma_neighbor_list_si2ter.tds_freq_only_list;
      }
      break;
    }
#endif /* FEATURE_GSM_TDS */
    
    default:
    {
      MSG_GERAN_ERROR_1_G("Invalid RAT %d", cell->rat);
      return FALSE;
    }
  }
  
  if (scell_ptr->gsm.utran_neighbor_list.valid_data == TRUE)
  {
    si2q_freq_list = &scell_ptr->gsm.utran_neighbor_list.wcdma_freq_only_list;
    si2q_cell_list = &scell_ptr->gsm.utran_neighbor_list.wcdma_cell_list;
  }


  /* Check if cell belongs to a frequency on its own received in SI2q */
  if (si2q_freq_list != NULL)
  {
    for (i = 0; i < si2q_freq_list->num_of_frequencies; i++)
    {
      if (si2q_freq_list->uarfcn[i].rat_type == rat_type &&
          si2q_freq_list->uarfcn[i].uarfcn == uarfcn)
      {
        found = TRUE;
        break;
      }
    }
  }
  
  /* Check if cell belongs to a frequency on its own received in SI2ter */
  if (!found && si2ter_freq_list != NULL)
  {
    for (i = 0; i < si2ter_freq_list->num_of_frequencies; i++)
    {
      if (si2ter_freq_list->uarfcn[i] == uarfcn)
      {
        found = TRUE;
        break;
      }
    }
  }
  
  /* Check if cell belongs to cell list received in SI2q */
  if (!found && si2q_cell_list != NULL)
  {
    for (i = 0; i < si2q_cell_list->num_of_cells; i++)
    {
      /* Deviation from 3GPP: allow a cell if any cell from same freq is present in SI2q */
      if (si2q_cell_list->cell_list[i].uarfcn.rat_type == rat_type &&
          si2q_cell_list->cell_list[i].uarfcn.uarfcn == uarfcn)
      {
        found = TRUE;
        break;
      }
    }
  }

  if (!found)
  {
    MSG_GERAN_HIGH_0_G("Cell not found in 3G Cell Reselection List");
  }

  return found;
}
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

boolean rr_g2x_resel_to_irat_cell_is_allowed(
  rr_irat_cell_t *cell,
  const gas_id_t  gas_id
)
{
  rr_g2x_data_t *rr_g2x_data_ptr;
  boolean        resel_allowed = FALSE;

  RR_NULL_CHECK_RETURN_PARAM(cell, FALSE);

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  if (list_linear_search(
        &rr_g2x_data_ptr->barred_cell_db,
        rr_g2x_barred_cell_db_entry_cmp_cell_fn,
        cell) == NULL)
  {
    /* Cell is not barred.
     * Now perform RAT-specific checks that are necessary. */
    switch (cell->rat)
    {
#ifdef FEATURE_LTE
      case RR_RAT_LTE:
      {
        resel_allowed = rr_g2x_resel_to_lte_cell_is_allowed(&cell->data.lte, gas_id);
        break;
      }
#endif /* FEATURE_LTE */

#ifdef FEATURE_WCDMA
      case RR_RAT_WCDMA:
      {
        resel_allowed = rr_g2x_resel_to_utran_cell_is_allowed(cell, gas_id);
        break;
      }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
      case RR_RAT_UTRAN_TDD:
      {
        resel_allowed = rr_g2x_resel_to_utran_cell_is_allowed(cell, gas_id);
        break;
      }
#endif /* FEATURE_GSM_TDS */

      default:
      {
        MSG_GERAN_ERROR_1_G("Unknown RAT %d", cell->rat);
        break;
      }
    }
  }
  
  return resel_allowed;
}

/**
  @brief Clear out various databases of barred cells / frequencies.
*/
void rr_g2x_cell_selection_triggered(const gas_id_t gas_id)
{
  MSG_GERAN_MED_0_G("Cell selection triggered, clearing IRAT databases");
  rr_g2x_barred_frequency_db_clear(gas_id);
  rr_g2x_barred_cell_db_clear(gas_id);
  rr_g2wr_reset_wcdma_surrounding_database(gas_id);
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
  rr_g2wr_cancel_all_wcdma_five_second_timers(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
}

/**
  @brief Clear cell quality data and do anything that is done on normal
         cell selection.
*/
void rr_g2x_plmn_selection_triggered(const gas_id_t gas_id)
{
  MSG_GERAN_MED_0_G("PLMN selection triggered, clearing cell quality data (Srxlevmin / Squalmin)");
  rr_g2x_cell_quality_data_clear(gas_id);
  rr_g2x_cell_selection_triggered(gas_id);
}

#ifdef FEATURE_WCDMA

void rr_irat_populate_gsm_idle_mode_3g_reselect_rej_log_packet(
  LOG_GSM_IDLE_MODE_3G_RESELECT_REJ_C_type * log_ptr
)
{
  /* This log packet is less useful with the new combined database
  approach.  TBD whether a new log packet should be defined that
  obsoletes this one. */
}

/**
  @brief Accesses the current Srxlevmin value, if known.

  @param srxlevmin Pointer to the variable in which Srxlevmin should be stored.
  @return TRUE if Srxlevmin is available, FALSE if it is not.
*/
boolean rr_g2x_wcdma_srxlevmin(
  int16 * srxlevmin,
  const gas_id_t gas_id
)
{
  rr_g2x_data_t *rr_g2x_data_ptr;

  RR_NULL_CHECK_RETURN_PARAM(srxlevmin, FALSE);

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  if (rr_g2x_data_ptr->cell_quality_data.wcdma_valid)
  {
    *srxlevmin = rr_g2x_data_ptr->cell_quality_data.wcdma.srxlevmin;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
  @brief Accesses the current Squalmin value, if known.

  @param squalmin Pointer to the variable in which Squalmin should be stored.
  @return TRUE if Squalmin is available, FALSE if it is not.
*/
boolean rr_g2x_wcdma_squalmin(
  int16 * squalmin,
  const gas_id_t gas_id
)
{
  rr_g2x_data_t *rr_g2x_data_ptr;

  RR_NULL_CHECK_RETURN_PARAM(squalmin, FALSE);

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  if (rr_g2x_data_ptr->cell_quality_data.wcdma_valid)
  {
    *squalmin = rr_g2x_data_ptr->cell_quality_data.wcdma.squalmin;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


#endif /* FEATURE_WCDMA */

/**
  @ingroup barred_frequency_db
  @brief Removes the entry associated with timer_id from the barred frequency
         database and updates L1 if necessary.

  @param timer_id The id of the timer that has expired.
*/
void rr_g2x_barred_frequency_timer_expired(
  rr_timer_id_e timer_id,
  const gas_id_t gas_id
)
{
  rr_g2x_barred_frequency_db_entry_t * entry;
  rr_g2x_data_t *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  entry = (rr_g2x_barred_frequency_db_entry_t *) list_linear_search(
    &rr_g2x_data_ptr->barred_frequency_db,
    rr_g2x_barred_frequency_db_entry_cmp_timer_fn,
    &timer_id
  );

  if (entry != NULL)
  {
    /* We have matched an entry with the associated timer ID.  We should delete
    the entry from the list and update L1 with a new neighbour list. */
    list_pop_item(&rr_g2x_data_ptr->barred_frequency_db, &entry->list_link);
    GPRS_MEM_FREE(entry);

    /* TODO: This would be much better indicated to RR using a signal that can
    be handled in all appropriate states. */

    /* Only update L1 with the new WCDMA neighbour list if GRR is in idle
    mode or establishing a PS connection (if GRR is in any other state then
    L1 will be automatically updated when GRR returns to idle mode). */
    if (grr_is_idle_camped(gas_id) || grr_tbf_est_in_progress(gas_id))
    {
      rr_l1_update_wcdma_neighbor_list(gas_id);
    }
  }
}

/**
  @ingroup barred_cell_db
  @brief Removes the entry associated with timer_id from the barred cell
         database and updates L1 if necessary.

  @param timer_id The id of the timer that has expired.
*/
void rr_g2x_barred_cell_timer_expired(
  rr_timer_id_e timer_id,
  const gas_id_t gas_id
)
{
  rr_g2x_barred_cell_db_entry_t * entry;
  rr_g2x_data_t *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  entry = (rr_g2x_barred_cell_db_entry_t *) list_linear_search(
    &rr_g2x_data_ptr->barred_cell_db,
    rr_g2x_barred_cell_db_entry_cmp_timer_fn,
    &timer_id
  );

  if (entry != NULL)
  {
    /* We have matched an entry with the associated timer ID.  We should delete
    the entry from the list and update L1 with a new neighbour list. */
    list_pop_item(&rr_g2x_data_ptr->barred_cell_db, &entry->list_link);
    GPRS_MEM_FREE(entry);

    /* TODO: This would be much better indicated to RR using a signal that can
    be handled in all appropriate states. */

    /* Only update L1 with the new WCDMA neighbour list if GRR is in idle
    mode or establishing a PS connection (if GRR is in any other state then
    L1 will be automatically updated when GRR returns to idle mode). */
    if (grr_is_idle_camped(gas_id) || grr_tbf_est_in_progress(gas_id))
    {
      rr_l1_update_wcdma_neighbor_list(gas_id);
    }
  }
}

/**
  @brief Removes any limitations on reselection to the specified frequency.
*/
void rr_g2x_redir_triggered(
  rr_irat_frequency_t frequency,
  const gas_id_t gas_id
)
{
  rr_g2x_barred_frequency_db_entry_t * entry;
  rr_g2x_data_t *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  entry = list_peek_front(&rr_g2x_data_ptr->barred_frequency_db);

  while (entry != NULL)
  {
    rr_g2x_barred_frequency_db_entry_t * next_entry = list_peek_next(
      &rr_g2x_data_ptr->barred_frequency_db,
      &entry->list_link
    );

    if (rr_g2x_barred_frequency_db_entry_cmp_frequency_fn(entry, &frequency))
    {
      switch (frequency.rat)
      {
#ifdef FEATURE_WCDMA
        case RR_RAT_WCDMA:
        {
          MSG_GERAN_MED_1_G("Unbarring WCDMA frequency %d", frequency.data.wcdma);
          break;
        } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
        case RR_RAT_LTE:
        {
          MSG_GERAN_MED_1_G("Unbarring LTE frequency %d", frequency.data.lte);
          break;
        } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

        default:
        {
          MSG_GERAN_ERROR_1_G("Unknown RAT: %d", frequency.rat);
        }
      }
      list_pop_item(&rr_g2x_data_ptr->barred_frequency_db, &entry->list_link);
      (void) rr_timer_cancel(entry->timer, gas_id);
      GPRS_MEM_FREE(entry);
    }

    entry = next_entry;
  }
}


/**
  @brief Determines whether inter-RAT lock exchange is allowed, based on the ongoing G2X Operation
  and the target RAT involved in inter-RAT.  
*/

boolean rr_g2x_lock_exchange_allowed( const rr_g2x_operation_type_e rr_g2x_operation_type, const rr_rat_e target_rat, const gas_id_t gas_id)
{

  boolean exchange_allowed = FALSE;

  switch(target_rat)
  {
#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      if ( (RR_G2X_OPERATION_TYPE_RESELECTION == rr_g2x_operation_type) ||
           (RR_G2X_OPERATION_TYPE_REDIRECTION == rr_g2x_operation_type)
         )
      {
        exchange_allowed = TRUE;
      }
      break;
    }
#endif /* FEATURE_LTE */
        
#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      if (RR_G2X_OPERATION_TYPE_REDIRECTION == rr_g2x_operation_type)
      { 
        exchange_allowed = TRUE;
      }    
      break;
    }
#endif /* FEATURE_GSM_TDS_G2T_REDIRECTION */
        
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    { 
      if (RR_G2X_OPERATION_TYPE_REDIRECTION == rr_g2x_operation_type)
      {
        exchange_allowed = TRUE;
      }     
      break;      
    }
#endif /* FEATURE_WCDMA */  
              
    default:
    break;
  }

  MSG_GERAN_HIGH_3_G("G2X exchange for RAT %d, Operation %d, allowed=%d",
                    (int)(target_rat), (int)rr_g2x_operation_type, (int)exchange_allowed );

  return(exchange_allowed);
}

/**
  @ingroup barred_frequency_db
  @brief Called when the Forbidden LAI List is updated, this function will
         remove from the barred frequency database any entries whose LAI is
         no longer forbidden.
*/
void rr_g2x_forbidden_lai_list_updated(const gas_id_t gas_id)
{
  rr_g2x_barred_frequency_db_entry_t *entry;
  boolean                             barred_frequency_db_updated = FALSE;
  rr_g2x_data_t                      *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  entry = list_peek_front(&rr_g2x_data_ptr->barred_frequency_db);

  while (entry != NULL)
  {
    rr_g2x_barred_frequency_db_entry_t * next_entry = list_peek_next(
      &rr_g2x_data_ptr->barred_frequency_db,
      &entry->list_link
    );

    if (entry->reason == RR_G2X_RESEL_FAILURE_CAUSE_FORBIDDEN_LA &&
        !rr_is_forbidden_LAI(&entry->data.lai, gas_id))
    {
      list_pop_item(&rr_g2x_data_ptr->barred_frequency_db, &entry->list_link);
      (void) rr_timer_cancel(entry->timer, gas_id);
      GPRS_MEM_FREE(entry);
      barred_frequency_db_updated = TRUE;
    }

    entry = next_entry;
  }

  if (barred_frequency_db_updated)
  {
    /* Only update L1 with the new WCDMA neighbour list if GRR is in idle
    mode or establishing a PS connection (if GRR is in any other state then
    L1 will be automatically updated when GRR returns to idle mode). */
    if (grr_is_idle_camped(gas_id) || grr_tbf_est_in_progress(gas_id))
    {
      rr_l1_update_wcdma_neighbor_list(gas_id);
    }
  }

  rr_g2x_barred_frequency_db_dump(gas_id);
}

/**
  @ingroup barred_frequency_db
  @brief Called when the Equivelent PLMN List is updated, this function will
         remove from the barred frequency database any entries whose PLMN is
         now equivelent to the serving cell's PLMN.
*/
void rr_g2x_equivalent_plmn_list_updated(const gas_id_t gas_id)
{
  rr_g2x_barred_frequency_db_entry_t *entry;
  boolean                             barred_frequency_db_updated = FALSE;
  rr_g2x_data_t                      *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  MSG_GERAN_HIGH_0_G("Equivalent PLMN List updated, re-verifying previously barred IRAT frequencies");

  rr_g2x_barred_frequency_db_dump(gas_id);

  entry = list_peek_front(&rr_g2x_data_ptr->barred_frequency_db);

  while (entry != NULL)
  {
    rr_g2x_barred_frequency_db_entry_t * next_entry = list_peek_next(
      &rr_g2x_data_ptr->barred_frequency_db,
      &entry->list_link
    );

    if (entry->reason == RR_G2X_RESEL_FAILURE_CAUSE_PLMN_MISMATCH)
    {
      gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

      boolean      serving_plmn_is_undefined;
      boolean      serving_mnc_includes_pcs_digit;
      sys_mcc_type serving_mcc;
      sys_mnc_type serving_mnc;

      boolean      non_serving_plmn_is_undefined;
      boolean      non_serving_mnc_includes_pcs_digit;
      sys_mcc_type non_serving_mcc;
      sys_mnc_type non_serving_mnc;

      sys_plmn_get_mcc_mnc(scell->gsm.location_area_identification.plmn_id,
                           &serving_plmn_is_undefined,
                           &serving_mnc_includes_pcs_digit,
                           &serving_mcc,
                           &serving_mnc);

      sys_plmn_get_mcc_mnc(entry->data.plmn_id,
                           &non_serving_plmn_is_undefined,
                           &non_serving_mnc_includes_pcs_digit,
                           &non_serving_mcc,
                           &non_serving_mnc);

      if (serving_plmn_is_undefined)
      {
        MSG_GERAN_ERROR_0_G("Serving PLMN: UNDEFINED");
      }
      else
      {
        MSG_GERAN_HIGH_3_G(
          "Serving PLMN: %d-%d (%d)",
          serving_mcc,
          serving_mnc,
          serving_mnc_includes_pcs_digit
        );
      }

      if (non_serving_plmn_is_undefined)
      {
        MSG_GERAN_ERROR_0_G("Non-serving PLMN: UNDEFINED");
      }
      else
      {
        MSG_GERAN_HIGH_3_G(
          "Non-serving PLMN: %d-%d (%d)",
          non_serving_mcc,
          non_serving_mnc,
          non_serving_mnc_includes_pcs_digit
        );
      }

      /* If the non-equivalent PLMN database entry has the same PLMN ID as the
      current serving cell then it should no longer be considered as non-
      equivalent and should be removed from the database. */
      if (sys_plmn_match(entry->data.plmn_id, scell->gsm.location_area_identification.plmn_id))
      {
        list_pop_item(&rr_g2x_data_ptr->barred_frequency_db, &entry->list_link);
        (void) rr_timer_cancel(entry->timer, gas_id);
        GPRS_MEM_FREE(entry);
        barred_frequency_db_updated = TRUE;
      }

      /* If the non-equivalent PLMN database entry has a PLMN ID that is
      equivalent to the current serving cell's PLMN ID then it should no longer
      be considered as non-equivalent and should be removed from the
      database. */
      else if (rr_eplmn_list_equivalent_plmn(scell->gsm.location_area_identification.plmn_id, gas_id) &&
               rr_eplmn_list_equivalent_plmn(entry->data.plmn_id, gas_id))
      {
        list_pop_item(&rr_g2x_data_ptr->barred_frequency_db, &entry->list_link);
        (void) rr_timer_cancel(entry->timer, gas_id);
        GPRS_MEM_FREE(entry);
        barred_frequency_db_updated = TRUE;
      }
    }

    entry = next_entry;
  }

  if (barred_frequency_db_updated)
  {
    /* Only update L1 with the new WCDMA neighbour list if GRR is in idle
    mode or establishing a PS connection (if GRR is in any other state then
    L1 will be automatically updated when GRR returns to idle mode). */
    if (grr_is_idle_camped(gas_id) || grr_tbf_est_in_progress(gas_id))
    {
      rr_l1_update_wcdma_neighbor_list(gas_id);
    }
  }

  rr_g2x_barred_frequency_db_dump(gas_id);
}

/**
  @ingroup barred_cell_db
  @brief Compare an entry in the barred cell database with an IRAT failure
         reason.
  @return non-zero value if the reason in the barred cell database matches the
          reason specified, zero if they do not match.
*/
static int rr_g2x_barred_cell_db_entry_cmp_reason_fn(
  void * item_ptr,
  void * compare_val
)
{
  rr_g2x_barred_cell_db_entry_t * entry;
  rr_g2x_resel_failure_cause_e  * reason;

  RR_NULL_CHECK_RETURN_PARAM(item_ptr, 0);
  RR_NULL_CHECK_RETURN_PARAM(compare_val, 0);

  entry = (rr_g2x_barred_cell_db_entry_t *) item_ptr;
  reason = (rr_g2x_resel_failure_cause_e *) compare_val;

  if (entry->reason == *reason)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  @brief Called when RR receives RR_CSG_WHITE_LIST_UPDATE_IND.

  On receipt of this signal, RR should un-bar any WCDMA cells that have
  previously been barred due to being forbidden CSG cells.
*/
void rr_g2x_csg_white_list_updated(const gas_id_t gas_id)
{
  rr_g2x_barred_cell_db_entry_t *entry = NULL;
  rr_g2x_resel_failure_cause_e   reason = RR_G2X_RESEL_FAILURE_CAUSE_CSG_FORBIDDEN;
  rr_g2x_data_t                 *rr_g2x_data_ptr;

  // Obtain a pointer to the module data
  rr_g2x_data_ptr = rr_g2x_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2x_data_ptr);

  MSG_GERAN_HIGH_0_G("CSG white list updated, clearing previously barred cells");

  entry = (rr_g2x_barred_cell_db_entry_t *) list_linear_search(
    &rr_g2x_data_ptr->barred_cell_db,
    rr_g2x_barred_cell_db_entry_cmp_reason_fn,
    &reason
  );

  while (entry != NULL)
  {
    /* We are deleting the entry, so cancel its timer, remove it from the list
    and free its associated memory. */

    switch (entry->cell.rat)
    {
#ifdef FEATURE_WCDMA
      case RR_RAT_WCDMA:
      {
        MSG_GERAN_HIGH_2_G("Unbarring UARFCN %d PSC %d",
                 entry->cell.data.wcdma.uarfcn, entry->cell.data.wcdma.scrambling_code);
        break;
      } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
      case RR_RAT_LTE:
      {
        MSG_GERAN_HIGH_2_G("Unbarring EARFCN %d PCID %d",
                 entry->cell.data.lte.earfcn, entry->cell.data.lte.pcid);
        break;
      } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
      case RR_RAT_UTRAN_TDD:
      {
        MSG_GERAN_HIGH_2_G("Unbarring UARFCN %d CPID %d",
                 entry->cell.data.utran_tdd.uarfcn, entry->cell.data.utran_tdd.cpid);
        break;
      } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

      default:
      {
        MSG_GERAN_ERROR_1_G("Unknown RAT: %d", entry->cell.rat);
      }
    }

    (void)rr_timer_cancel(entry->timer, gas_id);

    list_pop_item(
      &rr_g2x_data_ptr->barred_cell_db,
      &entry->list_link
    );
    GPRS_MEM_FREE(entry);

    /* And search for the next matching entry. */
    entry = (rr_g2x_barred_cell_db_entry_t *) list_linear_search(
      &rr_g2x_data_ptr->barred_cell_db,
      rr_g2x_barred_cell_db_entry_cmp_reason_fn,
      &reason
    );
  }

  rr_g2x_barred_cell_db_dump(gas_id);
}

/**
  @brief Determines whether the specified RAT is supported  by taking into account the segment loading 
*/
boolean rr_g2x_rat_supported(const rr_rat_e target_rat, const gas_id_t gas_id)
{
  switch(target_rat)
  {
    case RR_RAT_GSM:
      MSG_GERAN_ERROR_0_G("Unexpected use of GSM RAT, supported by default");
      return(TRUE);

#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
      /* verify that  segment has support for WCDMA */
      return(rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA));
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
      /* verify that  segment  has support for LTE */
      return(TRUE);
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
      /* verify that  segment has support for TDSCDMA */
      return(rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA));
#endif /* FEATURE_GSM_TDS */

    default:
      MSG_GERAN_ERROR_1_G("Unexpected RAT ID %d", (int)target_rat);
      break;
  }

  return(FALSE);
}

/*!
 * @brief Determines whether GPRS activity on another stack will prevent G2X activity
 *
 * @param target_rat (LTE, WCDMA, TDS etc.)
 * @param gas_id
 *
 * @return TRUE if there is no conflicting GPRS activity
 */
boolean rr_g2x_allowed_by_gprs_activity(const rr_rat_e target_rat, const gas_id_t gas_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (geran_in_multi_standby_mode())
  {
    boolean gprs_activity_detected;

    if (rr_ms_other_sim_in_background_traffic(gas_id))
    {
      /* explicitly check for an active TBF on any other GERAN stack */
      gprs_activity_detected = TRUE;
    }
    else
    {
      gas_id_t gas_ready = gas_id;

      /* determine whether GMM is ready on some other GERAN stack */
      if (grr_any_subscription_in_ready_state(&gas_ready))
      {
        gprs_activity_detected = (boolean)(gas_ready != gas_id);
      }
      else
      {
        gprs_activity_detected = FALSE;
      }
    }

    if (gprs_activity_detected)
    {
#ifdef FEATURE_WCDMA
      if (RR_RAT_WCDMA == target_rat)
      {
        return(GERAN_TA_SUPPORTED == geran_nv_g2w_ta_supported());
      }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
      if (RR_RAT_UTRAN_TDD == target_rat)
      {
        return(GERAN_TA_SUPPORTED == geran_nv_g2t_ta_supported());
      }
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
      if (RR_RAT_LTE == target_rat)
      {
        return(GERAN_TA_SUPPORTED == geran_nv_g2l_ta_supported());
      }
#endif /* FEATURE_LTE */

      /* any other non-GSM tech cannot currently coexist with GPRS */
      return((boolean)(RR_RAT_GSM == target_rat));
    }
  }
#else
  NOTUSED(target_rat);
  NOTUSED(gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(TRUE);
}

#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

