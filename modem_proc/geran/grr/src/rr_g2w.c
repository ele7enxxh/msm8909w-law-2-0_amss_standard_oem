/*==============================================================================

                        RR G2W Module

GENERAL DESCRIPTION

  This module contains general G2W related functions for use by RR.

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS

  None.

Copyright (c) 2005-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_g2w.c#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
20130411  sjw     Block G2W reselection if opposite stack is in GMM READY state
20120816  sjw     Block G2W reselection if GPRS Tuneaway is enabled
20050712  gdj     Initial version

==============================================================================*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/
#include "comdef.h"
#include "amssassert.h"
#include "rr_seg_load.h"
#include "rr_g2w.h"
#include "rr_log.h"
#include "rr_general.h"
#include "rr_l1_send.h"
#include "rr_ps_domain.h"
#include "list.h"
#include "rr_g2x.h"
#ifdef FEATURE_WCDMA
#include "rrcrrif.h"
#endif /* FEATURE_WCDMA */
#include "rr_multi_sim.h"
#include "log_codes_gsm_v.h"
#include "rr_log_types.h"
#include "rr_defs.h"
#include "rr_nv.h"
#include "gprs_mem.h"
#include "rr_msgr.h"

#include "stringl/stringl.h"
/*==============================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

#define RR_IRAT_MEASUREMENTS_PER_RAT 6
#define RR_G2W_FORBIDDEN_LAI_DB_SIZE 6
#define NUM_OF_T_RESEL_ENTRIES       4 

#ifdef FEATURE_LTE
#define RR_IRAT_PRIORITY_LIST_LENGTH (RR_IRAT_MEASUREMENTS_PER_RAT * 2)
#else
#define RR_IRAT_PRIORITY_LIST_LENGTH (RR_IRAT_MEASUREMENTS_PER_RAT)
#endif /* FEATURE_LTE */

/*==============================================================================
  Typedefs
==============================================================================*/

/*******************************************/
/* Surrounding Strong wcdma Cell Details   */
/* Reported by L1                          */
/*******************************************/

#ifdef FEATURE_WCDMA
typedef struct
{
  word uarfcn;
  /*!< The UARFCN of the WCDMA cell. */

  word scrambling_code;
  /*!< The Scrambling Code of the WCDMA cell. */

  boolean diversity;
  /*!< The Diversity flag for the cell. */

  int16 rscp;
  /*! The RSCP value measured for this cell. */

  int16 ecno;
  /*! The Ec/No value measured for this cell. */

  int16 rssi;
  /*!< The RSSI across the whole WCDMA frequency. */

  uint16 pn_offset;
  /*!< PN Offset for the cell. */

  // Increment this by 1 each time an XFER measurement report is received that
  // does not contain this cell.  If this reaches MAX_WCDMA_XFER_VALIDITY_LIMIT
  // then delete the cell.
  int xfer_validity_counter;
} rr_irat_measurement_wcdma_t;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
typedef struct
{
  uint16 uarfcn;
  /*!< UMTS Frequency Channel of the cell. */

  uint16 cell_parameter_id;
  /*!< Cell Parameter ID of the cell. */

  boolean diversity;
  /*!< Diversity value for the cell. */

  int16  rscp;
  /*!< RSCP of the cell. */

  uint16 ts0_pos;
  /*!< TS0 pos of the cell. */

} rr_irat_measurement_utran_tdd_t;
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE

typedef struct
{
  lte_earfcn_t earfcn;
  uint16       pcid;
  int16        rsrp;
  int16        rsrq;
} rr_irat_measurement_lte_t;

#endif /* FEATURE_LTE */

typedef union
{
#ifdef FEATURE_WCDMA
  rr_irat_measurement_wcdma_t wcdma;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
  rr_irat_measurement_lte_t   lte;
#endif /* FEATUTE_GSM_TO_LTE */

#ifdef FEATURE_GSM_TDS
  rr_irat_measurement_utran_tdd_t utran_tdd;
#endif /*FEATURE_GSM_TDS*/
} rr_irat_measurement_data_u;

typedef struct
{
  rr_rat_e                   rat;
  rr_irat_measurement_data_u data;

  rr_timer_id_T              resel_timer;
  boolean                  * resel_timer_flag;

#ifdef FEATURE_LTE
  rr_timer_id_e              thresh_high_timer;
  rr_timer_id_e              thresh_low_timer;
  rr_timer_id_e              h_prio_timer;
  boolean                  * priority_resel_timer_flag;
#endif /* FEATURE_LTE */

} rr_irat_measurement_t;

typedef struct
{
  // No of entries in the surrounding wcdma cell data base
  byte                  no_of_entries;
  // wcdma cell entries
  rr_irat_measurement_t entries[RR_IRAT_MEASUREMENTS_PER_RAT];
} rr_irat_measurements_t;

#ifdef FEATURE_LTE
typedef struct
{
  rr_irat_measurement_t *   measurement;
  int16                     s_value;
  rr_resel_priority_e       priority;
  int16                     thresh_high;
  int16                     thresh_low;
} rr_irat_priority_list_entry_t;

typedef struct
{
  uint32                        length;
  rr_irat_priority_list_entry_t entries[RR_IRAT_PRIORITY_LIST_LENGTH];
} rr_irat_priority_list_t;
#endif /* FEATURE_LTE */

typedef struct
{
  boolean        valid;
  rr_irat_cell_t cell;
  int16          wcdma_rscp;
  int16          utran_tdd_rscp;
} rr_irat_resel_target_t;

#ifdef FEATURE_LTE_REL11
typedef struct
{
  boolean depri_all_freq;
  uint8   num_depri_freq;
  uint32  depri_earfcn[GERAN_GRR_MAX_LTE_DEPRI_FREQ]; 
} rr_deprioritized_lte_earfcn_list_t;
#endif /*FEATURE_LTE_REL11*/


typedef struct
{
#ifdef FEATURE_WCDMA
  rr_irat_measurements_t                   wcdma_measurements;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
  rr_irat_measurements_t                   lte_measurements;
#endif /* FEATURE_LTE */

  rr_irat_resel_target_t                   resel_target;

  rr_irat_resel_target_t                   prev_resel_target;
  
  boolean                                  resel_timers[RR_IRAT_MEASUREMENTS_PER_RAT];

#ifdef FEATURE_LTE
  boolean                                  priority_resel_timers[RR_IRAT_PRIORITY_LIST_LENGTH];
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
  rr_irat_measurements_t                   utran_tdd_measurements;
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE_REL11
  rr_deprioritized_lte_earfcn_list_t            depri_lte_earfcn_list;
#endif /*FEATURE_LTE_REL11*/

} rr_g2w_data_t;

typedef boolean (* rr_irat_measurement_match_fn)(rr_irat_measurement_t *, void *);

typedef enum
{
  RR_IRAT_PRIORITY_RESEL_TIMER_TYPE_THRESH_HIGH = 0,
  RR_IRAT_PRIORITY_RESEL_TIMER_TYPE_THRESH_LOW  = 1,
  RR_IRAT_PRIORITY_RESEL_TIMER_TYPE_H_PRIO      = 2,
  RR_IRAT_PRIORITY_RESEL_TIMER_TYPE_COUNT       = 3
} rr_irat_priority_resel_timer_type_e;

/*==============================================================================
  Variables
==============================================================================*/

static rr_g2w_data_t rr_g2w_data_store[NUM_GERAN_DATA_SPACES];

static const int8 FDD_Qmin_mapping[] = { -20, -6, -18, -8, -16, -10, -14, -12 };

static const rr_timer_id_e rr_irat_resel_timers[6] =
{
  RR_G2WR_FIVE_SEC_TIMER_1,
  RR_G2WR_FIVE_SEC_TIMER_2,
  RR_G2WR_FIVE_SEC_TIMER_3,
  RR_G2WR_FIVE_SEC_TIMER_4,
  RR_G2WR_FIVE_SEC_TIMER_5,
  RR_G2WR_FIVE_SEC_TIMER_6
};

#ifdef FEATURE_LTE

static const rr_timer_id_e rr_irat_priority_resel_timers[RR_IRAT_PRIORITY_LIST_LENGTH][RR_IRAT_PRIORITY_RESEL_TIMER_TYPE_COUNT] =
{
  { RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_1,    RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_1,  RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_1 },
  { RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_2,    RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_2,  RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_2 },
  { RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_3,    RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_3,  RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_3 },
  { RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_4,    RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_4,  RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_4 },
  { RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_5,    RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_5,  RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_5 },
  { RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_6,    RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_6,  RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_6 },
  { RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_7,    RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_7,  RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_7 },
  { RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_8,    RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_8,  RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_8 },
  { RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_9,    RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_9,  RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_9 },
  { RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_10,   RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_10, RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_10 },
  { RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_11,   RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_11, RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_11 },
  { RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_12,   RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_12, RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_12 }
};

/* tlx hack: qsort comaparator expects function with two parameters while gas_id is also
   required to find feran_prioroty in G <>L scenario.
   */
static rr_resel_priority_e rr_irat_priority_list_entries_sort_geran_priority;

static const uint32 T_RESELECTION_MS[NUM_OF_T_RESEL_ENTRIES] = { 5000, 10000, 15000, 20000 };

#endif /* FEATURE_LTE */

/*==============================================================================

                    FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Private functions
==============================================================================*/

#ifdef FEATURE_LTE_REL11
/*check lte de-priority list to see if LTE entry in priritised  list has not been de-priority */
static boolean rr_irat_lte_arfcn_deprioritized(rr_irat_measurement_lte_t lte_data_earfcn,const gas_id_t gas_id);
#endif /*FEATURE_LTE_REL11*/

static void rr_irat_check_and_delete_prev_resel_target( rr_irat_measurement_t * measurement, boolean cell_id_to_check,  const gas_id_t gas_id );

static uint32 rr_irat_get_arfcn_for_meas_entry(rr_irat_measurement_t* measurement, const gas_id_t gas_id)
{
  uint32 arfcn;

  switch(measurement->rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      arfcn = measurement->data.wcdma.uarfcn;
      break;
    }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      arfcn = measurement->data.lte.earfcn;
      break;
    }
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      arfcn = measurement->data.utran_tdd.uarfcn;
      break;
    }
#endif /*FEATURE_GSM_TDS*/
    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", measurement->rat);
      arfcn = 0;
      break;
    }
  }

  return arfcn;
}

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_g2w_data_t*
 */
static rr_g2w_data_t *rr_g2w_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_g2w_data_store[as_index];
}

/*!
 * @brief Fills in a lookup table to determine whether G2X is allowed
 *
 * @param rat_allowed array of RR_RAT_MAX entries
 * @param gas_id
 *
 * @return void
 */
static boolean rr_ms_g2x_build_allowed_list(boolean *rat_allowed, const int num_rats, const gas_id_t gas_id)
{
  boolean resel_possible = TRUE;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_SGLTE) && defined(FEATURE_SGLTE_G2X_CONN_MODE_RESEL)
  /* if SGLTE is active and all stacks are in dedicated mode, reselection is not possible */
  resel_possible = !(rr_is_sglte_mode_active(gas_id) && rr_ms_all_stacks_in_state(RR_DATA_TRANSFER));
#else
  /* always allow reselection to proceed */
  resel_possible = TRUE;
#endif /* FEATURE_DUAL_SIM && FEATURE_SGLTE && FEATURE_SGLTE_G2X_CONN_MODE_RESEL */

  if (rat_allowed != NULL)
  {
    boolean is_multimode = geran_is_multi_mode_gas_id(gas_id);
    int     rat_id;

    /* loop through the list of RATs */
    for (rat_id = 0; rat_id < num_rats; rat_id++)
    {
      if (RR_RAT_GSM == (rr_rat_e)rat_id)
      {
        /* not inter-RAT, always allowed */
        rat_allowed[rat_id] = TRUE;
      }
      else
      {
        /* default value for each entry is based on inter-RAT capability */
        rat_allowed[rat_id] = is_multimode;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
        if (is_multimode)
        {
          /* if the decision so far is "allowed", perform further checks for multi-standby */
          /* default value for each entry is based on inter-RAT capability */
          if (rr_g2x_rat_supported((rr_rat_e)rat_id, gas_id))
          {
            rat_allowed[rat_id] = rr_g2x_allowed_by_gprs_activity((rr_rat_e)rat_id, gas_id);
          }
          else
          {
            rat_allowed[rat_id] = FALSE;
          }
        }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
      }
    } /* end for loop */
  }/* rat_allowed != NULL */

  return(resel_possible);
}

#ifdef FEATURE_LTE
static int rr_irat_priority_list_entries_sort_by_priority_and_s_value(const void * p1, const void * p2)
{
  const rr_irat_priority_list_entry_t * e1;
  const rr_irat_priority_list_entry_t * e2;

  RR_NULL_CHECK_RETURN_PARAM(p1, 0);
  RR_NULL_CHECK_RETURN_PARAM(p1, 0);

  e1 = (const rr_irat_priority_list_entry_t *) p1;
  e2 = (const rr_irat_priority_list_entry_t *) p2;

  if (e1->priority > e2->priority)
  {
    return -1;
  }
  else if (e1->priority < e2->priority)
  {
    return 1;
  }
  else
  {
    /* Equal priority, sort in descending order of S-value. */
    if (e1->s_value > e2->s_value)
    {
      return -1;
    }
    else if (e1->s_value < e2->s_value)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

/**
  @brief Sorts low priority cells in to descending order of priority.

  This sort is done according to the rules in 45.008 section 6.6.6.
*/
static int rr_irat_priority_list_entries_sort_low_priority(const void * p1, const void * p2)
{
  const rr_irat_priority_list_entry_t * e1;
  const rr_irat_priority_list_entry_t * e2;

  RR_NULL_CHECK_RETURN_PARAM(p1, 0);
  RR_NULL_CHECK_RETURN_PARAM(p2, 0);

  e1 = (const rr_irat_priority_list_entry_t *) p1;
  e2 = (const rr_irat_priority_list_entry_t *) p2;

  if (e1->s_value > e1->thresh_low &&
      e2->s_value <= e2->thresh_low)
  {
    return -1;
  }
  else if (e1->s_value <= e1->thresh_low &&
           e2->s_value > e2->thresh_low)
  {
    return 1;
  }
  else
  {
    if (e1->s_value > e1->thresh_low &&
        e2->s_value > e2->thresh_low)
    {
      /* Both cells satisfy the primary criteria of exceeding low threshold,
      so rank on priority and s-value. */
      return rr_irat_priority_list_entries_sort_by_priority_and_s_value(e1, e2);
    }
    else
    {
      /* Both cells are below their respective low thresholds, rank based
      on S-value only. */
      if (e1->s_value > e2->s_value)
      {
        return -1;
      }
      else if (e1->s_value < e2->s_value)
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }
  }
}

static int rr_irat_priority_list_entries_sort(const void * p1, const void * p2)
{
  const rr_irat_priority_list_entry_t * e1;
  const rr_irat_priority_list_entry_t * e2;
  const rr_resel_priority_e             geran_priority = rr_irat_priority_list_entries_sort_geran_priority;

  RR_NULL_CHECK_RETURN_PARAM(p1, 0);
  RR_NULL_CHECK_RETURN_PARAM(p1, 0);

  e1 = (const rr_irat_priority_list_entry_t *) p1;
  e2 = (const rr_irat_priority_list_entry_t *) p2;

  /* If e1 is high priority and e2 is low priority then no further comparisons
  are necessary. */
  if (e1->priority > geran_priority &&
      e2->priority <= geran_priority)
  {
    return -1;
  }

  /* If e2 is high priority and e1 is low priority then no further comparisons
  are necessary. */
  else if (e2->priority > geran_priority &&
           e1->priority <= geran_priority)
  {
    return 1;
  }

  else
  {
    /* So now either both cells are high priority, or both are low priority. */
    if (e1->priority > geran_priority &&
        e2->priority > geran_priority)
    {
      /* Both cells are high priority - sort in descending order of priority and
      S-value. */
      return rr_irat_priority_list_entries_sort_by_priority_and_s_value(e1, e2);
    }
    else
    {
      /* Both cells are low priority. */
      return rr_irat_priority_list_entries_sort_low_priority(e1, e2);
    }
  }
}
#endif /* FEATURE_LTE */

#ifdef FEATURE_WCDMA
/**
  @brief Searches the specified list of measurements for a WCDMA cell matching
         the specified UARFCN and Scrambling Code.

  @param uarfcn The UARFCN to search for.
  @param scrambling_code The Scrambling Code to search for.
  @return The index of the cell in the ncell store if it is found, or
          RR_IRAT_MEASUREMENTS_PER_RAT if it is not found.
*/
static uint8 rr_irat_measurements_find_wcdma_cell_index(
  rr_irat_measurements_t * measurements,
  uint16                   uarfcn,
  uint16                   scrambling_code
)
{
  uint8 i;

  for (i = 0; i < measurements->no_of_entries && i < RR_IRAT_MEASUREMENTS_PER_RAT; ++i)
  {
    if (measurements->entries[i].rat == RR_RAT_WCDMA &&
        measurements->entries[i].data.wcdma.uarfcn == uarfcn &&
        measurements->entries[i].data.wcdma.scrambling_code == scrambling_code)
    {
      return i;
    }
  }

  return RR_IRAT_MEASUREMENTS_PER_RAT;
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
/**
  @brief Searches the specified list of measurements for a UTRAN
         TDD cell matching the specified UARFCN and Cell
         Parameter ID.

  @param uarfcn The UARFCN to search for.
  @param cpid The Cell Parameter to search for.
  @return The index of the cell in the ncell store if it is found, or
          RR_IRAT_MEASUREMENTS_PER_RAT if it is not found.
*/
static uint8 rr_irat_measurements_find_utran_tdd_cell_index(
  rr_irat_measurements_t * measurements,
  uint16                   uarfcn,
  uint16                   cpid
)
{
  uint8 i;

  for (i = 0; i < measurements->no_of_entries && i < RR_IRAT_MEASUREMENTS_PER_RAT; ++i)
  {
    if (measurements->entries[i].rat == RR_RAT_UTRAN_TDD &&
        measurements->entries[i].data.utran_tdd.uarfcn == uarfcn &&
        measurements->entries[i].data.utran_tdd.cell_parameter_id == cpid)
    {
      return i;
    }
  }

  return RR_IRAT_MEASUREMENTS_PER_RAT;
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
/**
  @brief Searches the specified list of measurements for a LTE cell matching
         the specified EARFCN and PCID.

  @param earfcn The EARFCN to search for.
  @param pcid The PCID to search for.
  @return The index of the cell in the ncell store if it is found, or
          RR_IRAT_MEASUREMENTS_PER_RAT if it is not found.
*/
static uint8 rr_irat_measurements_find_lte_cell_index(
  rr_irat_measurements_t * measurements,
  lte_earfcn_t             earfcn,
  uint16                   pcid
)
{
  uint8 i;

  for (i = 0; i < measurements->no_of_entries && i < RR_IRAT_MEASUREMENTS_PER_RAT; ++i)
  {
    if (measurements->entries[i].rat == RR_RAT_LTE &&
        measurements->entries[i].data.lte.earfcn == earfcn &&
        measurements->entries[i].data.lte.pcid == pcid)
    {
      return i;
    }
  }

  return RR_IRAT_MEASUREMENTS_PER_RAT;
}
#endif /* FEATURE_LTE */

/**
 * Allocates a new five_second_timer_id from the wcdma surrounding 5sec timer pool and returns
 * it in the argument
 *
 * @param five_sec_timer_id [out] set to the new five second timerId that was allocated
 */
static void rr_irat_measurement_allocate_timers(
  rr_irat_measurement_t * measurement,
  boolean                 allocate_rank_based_resel_timer,
  const                   gas_id_t gas_id
)
{
  uint32 i;
  rr_g2w_data_t *rr_g2w_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(measurement);

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  RR_ASSERT_RETURN_VOID(measurement->resel_timer_flag == NULL);
  RR_ASSERT_RETURN_VOID(measurement->resel_timer == RR_TIMER_UNDEFINED);

  if (allocate_rank_based_resel_timer)
  {
    for (i = 0; i < RR_IRAT_MEASUREMENTS_PER_RAT; ++i)
    {
      if (rr_g2w_data_ptr->resel_timers[i] == FALSE)
      {
        rr_g2w_data_ptr->resel_timers[i] = TRUE;
        measurement->resel_timer = rr_irat_resel_timers[i];
        measurement->resel_timer_flag = &rr_g2w_data_ptr->resel_timers[i];
        break;
      }
    }

    RR_ASSERT_RETURN_VOID(measurement->resel_timer_flag != NULL);
    RR_ASSERT_RETURN_VOID(measurement->resel_timer != RR_TIMER_UNDEFINED);
  }

#ifdef FEATURE_LTE
  RR_ASSERT_RETURN_VOID(measurement->priority_resel_timer_flag == NULL);
  RR_ASSERT_RETURN_VOID(measurement->thresh_high_timer == RR_TIMER_UNDEFINED);
  RR_ASSERT_RETURN_VOID(measurement->thresh_low_timer == RR_TIMER_UNDEFINED);
  RR_ASSERT_RETURN_VOID(measurement->h_prio_timer == RR_TIMER_UNDEFINED);

  for (i = 0; i < RR_IRAT_PRIORITY_LIST_LENGTH; ++i)
  {
    if (rr_g2w_data_ptr->priority_resel_timers[i] == FALSE)
    {
      rr_g2w_data_ptr->priority_resel_timers[i] = TRUE;
      measurement->thresh_high_timer = rr_irat_priority_resel_timers[i][RR_IRAT_PRIORITY_RESEL_TIMER_TYPE_THRESH_HIGH];
      measurement->thresh_low_timer = rr_irat_priority_resel_timers[i][RR_IRAT_PRIORITY_RESEL_TIMER_TYPE_THRESH_LOW];
      measurement->h_prio_timer = rr_irat_priority_resel_timers[i][RR_IRAT_PRIORITY_RESEL_TIMER_TYPE_H_PRIO];
      measurement->priority_resel_timer_flag = &rr_g2w_data_ptr->priority_resel_timers[i];
      break;
    }
  }

  RR_ASSERT_RETURN_VOID(measurement->priority_resel_timer_flag != NULL);
  RR_ASSERT_RETURN_VOID(measurement->thresh_high_timer != RR_TIMER_UNDEFINED);
  RR_ASSERT_RETURN_VOID(measurement->thresh_low_timer != RR_TIMER_UNDEFINED);
  RR_ASSERT_RETURN_VOID(measurement->h_prio_timer != RR_TIMER_UNDEFINED);
#endif /* FEATURE_LTE */
}

#ifdef FEATURE_WCDMA
/**
  @brief Adds a new WCDMA cell to the provided list of measurements.

  @param measurements The list of measurements to modify.
  @param uarfcn The UARFCN of the cell.
  @param scrambling_code The Scrambling Code of the cell.
  @param diversity The Diversity flag for the cell.
  @param rscp The measured RSCP value for the cell.
  @param ecno The measured Ec/No value for the cell.
  @param rssi The measured RSSI for the frequency of the cell.
*/
static void rr_g2wr_add_wcdma_surrounding_database_entry(
  rr_irat_measurements_t * measurements,
  uint16                   uarfcn,
  uint16                   scrambling_code,
  boolean                  diversity,
  int16                    rscp,
  int16                    ecno,
  int16                    rssi,
  uint16                   pn_offset,
  const gas_id_t           gas_id
)
{
  rr_irat_measurement_t * measurement = NULL;

  if (measurements->no_of_entries < RR_IRAT_MEASUREMENTS_PER_RAT)
  {
    measurement = &measurements->entries[measurements->no_of_entries];

    rr_irat_measurement_allocate_timers(measurement, TRUE, gas_id);

    measurement->rat = RR_RAT_WCDMA;
    measurement->data.wcdma.uarfcn = uarfcn;
    measurement->data.wcdma.scrambling_code = scrambling_code;
    measurement->data.wcdma.diversity = diversity;
    measurement->data.wcdma.rscp = rscp;
    measurement->data.wcdma.ecno = ecno;
    measurement->data.wcdma.rssi = rssi;
    measurement->data.wcdma.pn_offset = pn_offset;

    measurement->data.wcdma.xfer_validity_counter = 0;

    ++measurements->no_of_entries;
  }
  else /* otherwise, no more entries in the database */
  {
    MSG_GERAN_ERROR_0_G("Overflow when trying to add new WCDMA measurement");
  }
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
/**
  @brief Adds a new UTRAN TDD cell to the provided list of
         measurements.

  @param measurements The list of measurements to modify.
  @param uarfcn The UARFCN of the cell.
  @param cpid The Cell Parameter ID of the cell.
  @param diversity The Diversity flag for the cell.
  @param rscp The measured RSCP value for the cell.
  @param rssi The measured RSSI for the frequency of the cell.
  @param pnoffset
*/
static void rr_g2wr_add_utran_tdd_surrounding_database_entry(
  rr_irat_measurements_t * measurements,
  uint16                   uarfcn,
  uint16                   cpid,
  boolean                  diversity,
  int16                    rscp,
  uint16                   ts0_pos,
  const gas_id_t           gas_id
)
{
  rr_irat_measurement_t * measurement = NULL;

  if (measurements->no_of_entries < RR_IRAT_MEASUREMENTS_PER_RAT)
  {
    measurement = &measurements->entries[measurements->no_of_entries];

    rr_irat_measurement_allocate_timers(measurement, TRUE, gas_id);

    measurement->rat = RR_RAT_UTRAN_TDD;
    measurement->data.utran_tdd.uarfcn = uarfcn;
    measurement->data.utran_tdd.cell_parameter_id = cpid;
    measurement->data.utran_tdd.diversity = diversity;
    measurement->data.utran_tdd.rscp = rscp;
    measurement->data.utran_tdd.ts0_pos = ts0_pos;

    ++measurements->no_of_entries;
  }
  else /* otherwise, no more entries in the database */
  {
    MSG_GERAN_ERROR_0_G("Overflow when trying to add new UTRAN TDD measurement");
  }
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
/**
  @brief Adds a new LTE cell to the provided list of measurements.

  @param measurements The neighbour cell list to add the new cell to.
  @param earfcn The EARFCN of the cell.
  @param pcid The PCID of the cell.
  @param rsrp The measured RSRP value for the cell.
  @param rsrq The measured RSRQ value for the cell.
*/
static void rr_g2wr_add_lte_surrounding_database_entry(
  rr_irat_measurements_t * measurements,
  lte_earfcn_t             earfcn,
  uint16                   pcid,
  int16                    rsrp,
  int16                    rsrq,
  const gas_id_t           gas_id
)
{
  rr_irat_measurement_t * measurement = NULL;

  /****************************************************************/
  /* make sure we don't erroneously have too many entries. Should */
  /* never happen since cells were taken out of surround DB above */
  /****************************************************************/

  if (measurements->no_of_entries < RR_IRAT_MEASUREMENTS_PER_RAT)
  {
    measurement = &measurements->entries[measurements->no_of_entries];

    rr_irat_measurement_allocate_timers(measurement, FALSE, gas_id);

    measurement->rat = RR_RAT_LTE;
    measurement->data.lte.earfcn = earfcn;
    measurement->data.lte.pcid = pcid;
    measurement->data.lte.rsrp = rsrp;
    measurement->data.lte.rsrq = rsrq;

    ++measurements->no_of_entries;
  }
  else /* otherwise, no more entries in the database */
  {
    MSG_GERAN_ERROR_0_G("Overflow when trying to add new LTE measurement");
  }
}
#endif /* FEATURE_LTE */

#ifdef FEATURE_WCDMA
/*
Returns the index of the WCDMA cell with the lowest RSCP, or
RR_IRAT_MEASUREMENTS_PER_RAT if there are no cells in the db;
*/
static byte rr_g2wr_find_worst_wcdma_surround_database_entry(
  rr_irat_measurements_t * measurements
)
{
  if (measurements->no_of_entries == 0)
  {
    return RR_IRAT_MEASUREMENTS_PER_RAT;
  }
  else if (measurements->no_of_entries == 1)
  {
    return 0;
  }
  else
  {
    byte worst_cell_index = 0;
    byte cell_index = 1;
    while (cell_index < measurements->no_of_entries && cell_index < ARR_SIZE(measurements->entries))
    {
      if (measurements->entries[cell_index].data.wcdma.rscp <
          measurements->entries[worst_cell_index].data.wcdma.rscp)
      {
        worst_cell_index = cell_index;
      }
      cell_index++;
    }
    return worst_cell_index;
  }
}

/**
 * Returns TRUE if the UARFCN/SC are included in the xfer_meas_ind or FALSE otherwise.
 */
static boolean cell_is_in_xfer_meas_ind(
  uint16 uarfcn,
  uint16 sc,
  mph_surround_wcdma_xfer_meas_ind_T *xfer_meas_ind
)
{
  int i;

  for (i = 0; i < xfer_meas_ind->num_best_cells; i++)
  {
    if (xfer_meas_ind->wcdma_xfer_meas[i].UARFCN == uarfcn &&
        xfer_meas_ind->wcdma_xfer_meas[i].Scrambling_Code == sc)
    {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  @brief Determines whether a WCDMA cell's RSCP exceeds the value of RLA_C for
         the serving cell and all suitable non-serving GSM cells by the value
         of FDD_Qoffset.

  @param measurement The WCDMA cell measurements that are being checked.
  @param ignore_serving_cell_rxlev If TRUE then the WCDMA cell's RSCP will only
         be compared to neighbouring GSM cells' RLA_C, if FALSE the WCDMA
         cell's RSCP will also be compared to the serving cell's RLA_C.  This
         should normally be set to FALSE, but will be set to TRUE if the
         serving cell is no longer suitable for some reason.
  @param recent_reselection_flag Indicates whether a reselection has been
         performed recently, if TRUE an additional offset is applied to
         the calculation, such that the WCDMA cell's RSCP must exceed all
         RLA_C values by an additional 5 dBm.
  @return TRUE if the WCDMA cell's RSCP is better than the serving and all
          neighbouring GSM cells, FALSE if it is not.
*/
static boolean rr_wcdma_cell_rscp_is_better(
  rr_irat_measurement_t * measurement,
  boolean                 ignore_serving_cell_rxlev,
  boolean                 recent_reselection_flag,
  const gas_id_t          gas_id
)
{
  int16                          RxLev = 0;
  byte                           FDD_Qoffset = 0;
  int                            FDD_Qoffset_db = 0;
  better_cell_entry_T           *better_cell_ptr = NULL;
  int                            INTERRAT_G2W_CRH_DB = 0;
  boolean                        surrounding_wcdma_better = FALSE;
  int                            index = 0;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_reselection_structure_T  *local_gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);
  gprs_better_cell_list_T       *local_better_cell_list_ptr = rr_get_better_cell_list_ptr(gas_id);

  /* prepare comparison parameters */
  if (rr_gprs_camped_on_ccch(gas_id) || rr_gsm_camped_on_ccch(gas_id) )
  {
    if (scell_info_ptr->gsm.utran_neighbor_list.valid_data)
    {
      FDD_Qoffset = scell_info_ptr->gsm.utran_neighbor_list.si2q_params.FDD_Qoffset & 0x0f;
    }
    else if (scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.valid_data)
    {
      FDD_Qoffset = scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.FDD_Qoffset & 0x0f;
    }
    else
    {
      FDD_Qoffset = 8; /* default value */
    }
  }
  else
  {
    FDD_Qoffset = scell_info_ptr->gprs_3g_meas_params.FDD_GPRS_Qoffset;
  }

  if (FDD_Qoffset == 0)
  {
    MSG_GERAN_HIGH_0_G("FDD_Qoffset = 0, always select a cell if acceptable");
    return TRUE;
  }

  if (recent_reselection_flag == TRUE)
  {
    /**********************************************************************/
    /* If Recent Reselection flag is set, use the CRH of 5 DB, in         */
    /* validating the Neighbor WCDMA cell ie., basically discouraging the */
    /* reselection ( InterRAT ) if reselection happened recently.         */
    /**********************************************************************/
      INTERRAT_G2W_CRH_DB = 5;
      MSG_GERAN_HIGH_0_G("Using INTERRAT_G2W_CRH = 5dB");
  } /* recent_reselection_flag set */

  FDD_Qoffset_db = (FDD_Qoffset * 4) - 32;
  RxLev = CONVERT_TO_RXLEVdB(local_gprs_reselection_list_ptr->serving_values.RXLEV_average);
  RxLev += (int16) (FDD_Qoffset_db + INTERRAT_G2W_CRH_DB);

  MSG_GERAN_HIGH_3_G("WCDMA cell RSCP:%d, SCell RxLev: %d, ignore_scell: %d ", \
           measurement->data.wcdma.rscp, RxLev, ignore_serving_cell_rxlev);
  /* check the wcdma cell with serving cell rxlev */
  if ((measurement->data.wcdma.rscp > RxLev) || ignore_serving_cell_rxlev)
  {
    surrounding_wcdma_better = TRUE;
  }
  else
  {
    return FALSE;
  }

  /* check the wcdma cell with other gsm ncells */
  for (index = 0; index < local_better_cell_list_ptr->no_of_entries; ++index )
  {
    better_cell_ptr = &(local_better_cell_list_ptr->candidate_cell[index]);
    RxLev = (int16) (CONVERT_TO_RXLEVdB(better_cell_ptr->reselection_data.RXLEV_average) \
            + FDD_Qoffset_db + INTERRAT_G2W_CRH_DB);
    if (!(measurement->data.wcdma.rscp > RxLev))
    {
      surrounding_wcdma_better = FALSE;
      MSG_GERAN_HIGH_3_G("WCDMA cell RSCP:%d, NCell(%d) RxLev: %d", \
        measurement->data.wcdma.rscp, better_cell_ptr->reselection_data.BCCH_ARFCN.num, RxLev);
      break;
    }
  } /* for (index=0; index < no_of_neighbor_gsm_cells; ++index ) */

  return surrounding_wcdma_better;
}
#endif /* FEATURE_WCDMA */


#ifdef FEATURE_GSM_TDS
/**
  @brief Determines whether a UTRAN TDD cell's RSCP is equal to
         or greater than TDD_Qoffset.

  @param measurement The UTRAN TDD cell measurements that are
                     being checked.

  @param recent_reselection_flag Indicates whether a reselection has been
         performed recently, if TRUE an additional offset is applied to
         the calculation, such that the UTRAN TDD cell's RSCP
         must be >= to TDD_QOFFSET by an additional 5 dBm.
  @return TRUE if the UTRAN TDD cell's RSCP is >= to TDD_QOFFSET
*/
static boolean rr_utran_tdd_cell_rscp_is_better(
  rr_irat_measurement_t * measurement,
  boolean                 recent_reselection_flag,
  boolean                 immediate_reselection,
  const gas_id_t          gas_id
)
{
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  uint8                          TDD_Qoffset = 0;
  int16                          TDD_Qoffset_dbm = 0;
  int                            INTERRAT_G2T_CRH_DB = 0;
  boolean                        surrounding_utran_tdd_better = FALSE;

  /* prepare comparison parameters */
  if (immediate_reselection)
  {
    TDD_Qoffset = DEFAULT_TDD_QOFFSET;
    MSG_GERAN_MED_1_G("RR TDS forced cell change apply min TDD_Qoffset %d",TDD_Qoffset);
  }
  else
  {
    if (rr_gprs_camped_on_ccch(gas_id) || rr_gsm_camped_on_ccch(gas_id) )
    {
      if (scell_info_ptr->gsm.utran_neighbor_list.valid_data)
      {
        TDD_Qoffset = scell_info_ptr->gsm.utran_neighbor_list.si2q_params.TDD_Qoffset & 0x0f;
      }
      else if (scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.valid_data)
      {
        TDD_Qoffset = scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.TDD_Qoffset & 0x0f;
      }
      else
      {
        TDD_Qoffset = DEFAULT_TDD_QOFFSET;
      }
    }
    else
    {
      TDD_Qoffset = scell_info_ptr->gprs_3g_meas_params.TDD_GPRS_Qoffset;
    }
  }

  if (recent_reselection_flag == TRUE)
  {
    /*************************************************************************/
    /* If Recent Reselection flag is set, use the CRH of 5 dB, in            */
    /* validating the Neighbor URAN TDD cell ie., basically discouraging the */
    /* reselection ( InterRAT ) if reselection happened recently.            */
    /*************************************************************************/
      INTERRAT_G2T_CRH_DB = 5;
      MSG_GERAN_HIGH_0_G("Using INTERRAT_G2T_CRH = 5dB");
  } /* recent_reselection_flag set */

  TDD_Qoffset_dbm = (TDD_Qoffset * 3) - 105; /*0 is -105dBm, 15 is -60dBm, See 45.008 Sect 9 Table 1 */
  TDD_Qoffset_dbm += INTERRAT_G2T_CRH_DB;

  MSG_GERAN_HIGH_3_G("UTRAN TDD cell CPID:%d, RSCP:%d, TDD_Qoffset dBm: %d", \
           measurement->data.utran_tdd.cell_parameter_id, \
           measurement->data.utran_tdd.rscp, \
           TDD_Qoffset_dbm);

  /* check the utran TDD RSCP is greater than or equal to TDD_QOFFSET */
  if (measurement->data.utran_tdd.rscp >= TDD_Qoffset_dbm)
  {
    surrounding_utran_tdd_better = TRUE;
  }

  return surrounding_utran_tdd_better;
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
/****************************************************************************/
/* calculate FDD_RSCP_threshold                                             */
/* return TRUE if the wcdma cell's RSCP value is equal to or greater than   */
/* FDD_RSCP_threshold, otherwise, return FALSE                              */
/****************************************************************************/
static boolean rr_wcdma_cell_rscp_is_suitable(
  rr_irat_measurement_t * measurement,
  const gas_id_t          gas_id
)
{
  int                FDD_RSCP_threshold = 0;

  byte               FDD_RSCPmin = DEFAULT_FDD_RSCPMIN;
  int                FDD_RSCPmin_dbm = 0;
  boolean            FDD_RSCPmin_is_broadcast = FALSE;
  int                P_MAX = 24; /* default MAX WCDMA Tx power for power class 3 UE */
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  if (rr_gprs_camped_on_ccch(gas_id) || rr_gsm_camped_on_ccch(gas_id) )
  {
    if (scell_info_ptr->gsm.utran_neighbor_list.valid_data)
    {
      FDD_RSCPmin = scell_info_ptr->gsm.utran_neighbor_list.si2q_params.FDD_RSCPmin;
      FDD_RSCPmin_is_broadcast = scell_info_ptr->gsm.utran_neighbor_list.si2q_params.FDD_additional_MPs_PI;
    }
    else if (scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.valid_data)
    {
      FDD_RSCPmin = scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.FDD_RSCPmin;
      FDD_RSCPmin_is_broadcast = scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.FDD_additional_MPs_valid;
    }
    else
    {
      FDD_RSCPmin = DEFAULT_FDD_RSCPMIN;
      FDD_RSCPmin_is_broadcast = FALSE;
    }
  }
  else
  {
    FDD_RSCPmin = scell_info_ptr->gprs_3g_meas_params.FDD_RSCPmin;
    FDD_RSCPmin_is_broadcast = scell_info_ptr->gprs_3g_meas_params.FDD_additional_MPs_valid;
  }

  /* convert FDD_RSCPmin from broadcasted value to dbm */
  FDD_RSCPmin_dbm = (FDD_RSCPmin * 2) + MIN_FDD_RSCPMIN_DBM;

  /* get P_MAX */
  P_MAX = 24;  //todo: RRC returns MS power class


  /* calculate FDD_RSCP_threshold:
   *
   * Rel-5:
   *  FDD_RSCP_threshold equals FDD_RSCPmin - min((P_MAX - 21 dBm), 3 dB)
   *  if FDD_RSCPmin is broadcast on the serving cell,
   *  else Qrxlevmin + Pcompensation + 10 dB, if these parameters are available,
   *  otherwise the default value of FDD_RSCPmin.
   *
   * Rel99:
   *  FDD_RSCP_threshold equals Qrxlevmin + Pcompensation + 10 dB, if these parameters are available,
   *  otherwise -  (criterion not effective).
   */
  if (FDD_RSCPmin_is_broadcast)
  {
    FDD_RSCP_threshold = FDD_RSCPmin_dbm - MIN((P_MAX - 21), 3);
  }
  else
  {
    boolean srxlevmin_valid;
    int16 srxlevmin = 0;

    srxlevmin_valid = rr_g2x_wcdma_srxlevmin(
      &srxlevmin,
      gas_id
    );

    if (srxlevmin_valid)
    {
      /* Srxlevmin = q_rxlevmin + pcompensation, see RRC's implementation
       * 05.08: Pcompensation = max(UE_TXPWR_MAX_RACH - P_MAX, 0)
       */
      FDD_RSCP_threshold = srxlevmin + 10;
    }
    else
    {
      FDD_RSCP_threshold = FDD_RSCPmin_dbm;
    }
  }

  MSG_GERAN_HIGH_2_G("WCDMA cell RSCP:%d, FDD_RSCP_threshold: %d", \
    measurement->data.wcdma.rscp, FDD_RSCP_threshold);
  if (measurement->data.wcdma.rscp >= FDD_RSCP_threshold)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

static boolean rr_gprs_wcdma_cell_ecno_is_suitable(
  rr_irat_measurement_t * measurement,
  const gas_id_t          gas_id
)
{
  gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);
  byte FDD_Qmin = 7; /* see 05.08 */
  byte FDD_Qmin_offset = 0;
  int8 FDD_Qmin_db;
  int16 ecno = measurement->data.wcdma.ecno / 2;

  RR_NULL_CHECK_RETURN_PARAM(measurement, FALSE);

  if (scell->gsm.utran_neighbor_list.valid_data)
    {
      FDD_Qmin = scell->gsm.utran_neighbor_list.si2q_params.FDD_Qmin;
      FDD_Qmin_offset = scell->gsm.utran_neighbor_list.si2q_params.FDD_Qmin_offset;
  }
  else if (scell->gsm.wcdma_neighbor_list_si2ter.valid_data)
  {
    FDD_Qmin = scell->gsm.wcdma_neighbor_list_si2ter.FDD_Qmin;
    FDD_Qmin_offset = scell->gsm.wcdma_neighbor_list_si2ter.FDD_Qmin_offset;
  }

#ifdef FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN
  #error code not present
#endif /* FEATURE_G2W_RESEL_OPT_ADJUST_FDD_QMIN */

  FDD_Qmin_db = FDD_Qmin_mapping[FDD_Qmin];

  /* for usage of FDD_Qmin_offset, see 45.008 */
  FDD_Qmin_db -= (int8) (FDD_Qmin_offset * 2);
  MSG_GERAN_MED_1_G("FDD_Qmin_offset: %d", FDD_Qmin_offset);

  if (ecno >= FDD_Qmin_db)
  {
    boolean squalmin_valid;
    int16 squalmin = 0;

    squalmin_valid = rr_g2x_wcdma_squalmin(
      &squalmin,
      gas_id
    );

    if (squalmin_valid)
    {
      if (ecno >= squalmin)
      {
        MSG_GERAN_HIGH_3_G("Ec/No: %d, FDD Qmin (dB): %d, Squalmin: %d, cell meets criterion", ecno, FDD_Qmin_db, squalmin);
        return TRUE;
      }
      else
      {
        MSG_GERAN_HIGH_3_G("Ec/No: %d, FDD Qmin (dB): %d, Squalmin: %d, cell does not meet criterion", ecno, FDD_Qmin_db, squalmin);
        return FALSE;
      }
    }
    else
    {
      MSG_GERAN_HIGH_2_G("Ec/No: %d, FDD Qmin (dB): %d, cell meets criterion (Squalmin not known)", ecno, FDD_Qmin_db);
      return TRUE;
    }
  }
  else
  {
    MSG_GERAN_HIGH_2_G("Ec/No: %d, FDD Qmin (dB): %d, cell does not meet criterion", ecno, FDD_Qmin_db);
    return FALSE;
  }
   /*lint -restore*/
}
#endif /* FEATURE_WCDMA */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
/**
  @brief Stops the reselection timer associated with the specified cell.

  @param ncell Pointer to the cell structure whose timer should be stopped.
*/
static void rr_g2wr_stop_wcdma_five_second_timer(
  rr_irat_measurement_t *ncell,
  const gas_id_t         gas_id
)
{
  rr_timer_id_T five_second_timer_id;

  RR_ASSERT_RETURN_VOID(ncell != NULL);

  five_second_timer_id = ncell->resel_timer;

  if (five_second_timer_id==RR_G2WR_FIVE_SEC_TIMER_1 || five_second_timer_id==RR_G2WR_FIVE_SEC_TIMER_2 ||
      five_second_timer_id==RR_G2WR_FIVE_SEC_TIMER_3 || five_second_timer_id==RR_G2WR_FIVE_SEC_TIMER_4 ||
      five_second_timer_id==RR_G2WR_FIVE_SEC_TIMER_5 || five_second_timer_id==RR_G2WR_FIVE_SEC_TIMER_6
     )
  {
    /* only generate stop message if timer is not already stopped */
    if (rr_timer_cancel(five_second_timer_id, gas_id))
    {
#ifdef FEATURE_WCDMA
      if (ncell->rat == RR_RAT_WCDMA)
      {
        MSG_GERAN_HIGH_3_G("Stopped WCDMA 5s Timer=%d Cell=%d",
                 five_second_timer_id,
                 ncell->data.wcdma.uarfcn,
                 ncell->data.wcdma.scrambling_code);
      }
      else
#endif /* FEATURE_WCDMA */
      {
        MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", ncell->rat);
      }
    }
    /* do not reset the timer_id for the cell, it should always be associated with that surrounding cell */
  }
  else
  {
#ifdef FEATURE_WCDMA
    if (ncell->rat == RR_RAT_WCDMA)
    {
      MSG_GERAN_ERROR_3_G("Invalid wcdma 5s timerId=%d for Cell=%d",
                five_second_timer_id,
                ncell->data.wcdma.uarfcn,
                ncell->data.wcdma.scrambling_code);
    }
    else
#endif /* FEATURE_WCDMA */
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", ncell->rat);
    }
  }
} /* rr_g2wr_stop_wcdma_five_second_timer */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
/*
@detail
This function checks to see if the given entry belongs to a cell with Frequency belonging to the highest priority 
Frequency Entry in the measured DB ?
@param  (a) Prioroty List (b) Priority Entry 
@ returns TRUE if the entry is the highest priority entry, FALSE otherwsie

*/

static boolean rr_irat_check_if_highest_priority_cell(rr_irat_priority_list_t*  priority_list_ptr, rr_irat_priority_list_entry_t *input_entry, const gas_id_t gas_id )
{

  uint32 i;

  for (i = 0; 
       i < priority_list_ptr->length &&  
       i < RR_IRAT_PRIORITY_LIST_LENGTH;
       ++i)
  {
    rr_irat_priority_list_entry_t *entry = &priority_list_ptr->entries[i];

    if ( entry->priority > input_entry->priority )
    {
       return FALSE;
    }
  }

  return TRUE;
}
#endif /* FEATURE_LTE */


/*
@detail
This function checks to see if the given measurement Entry belongs to the same cell Frequency and PSC Id as that of the Backed Up
Reselection Target. If found true, then  Delete the Back Up Target from the Data Base.

@param  (a) rr_irat_measurement_t IRAT Meas Entry (b) cell_id_to_chec (c) gas_id
@ returns NONE

*/


static void rr_irat_check_and_delete_prev_resel_target( rr_irat_measurement_t * measurement, boolean cell_id_to_check,  const gas_id_t gas_id )
{
  rr_g2w_data_t                      *rr_g2w_data_ptr;
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);

  if ( !rr_g2w_data_ptr->prev_resel_target.valid ) return ;

  if ( measurement->rat == rr_g2w_data_ptr->prev_resel_target.cell.rat )
  {
    switch (measurement->rat)
    {
#ifdef FEATURE_WCDMA
      case RR_RAT_WCDMA:
      {
        if ( measurement->data.wcdma.uarfcn == rr_g2w_data_ptr->prev_resel_target.cell.data.wcdma.uarfcn )
        {
          if ( !cell_id_to_check )
          {
            /* This implies all cells on the UARFCN are gone */
            rr_g2w_data_ptr->prev_resel_target.valid = FALSE;
          }
          else if ( measurement->data.wcdma.scrambling_code == rr_g2w_data_ptr->prev_resel_target.cell.data.wcdma.scrambling_code )
          {
            rr_g2w_data_ptr->prev_resel_target.valid = FALSE;
          }
        }
        break;
      } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */
                        
#ifdef FEATURE_LTE
      case RR_RAT_LTE:
      {
        if ( measurement->data.lte.earfcn == rr_g2w_data_ptr->prev_resel_target.cell.data.lte.earfcn )
        {
          if ( !cell_id_to_check )
          {
           /* This implies all cells on the LTE EARFCN are gone */
            rr_g2w_data_ptr->prev_resel_target.valid = FALSE;
          }
          else if ( measurement->data.lte.pcid == rr_g2w_data_ptr->prev_resel_target.cell.data.lte.pcid )
          {
            rr_g2w_data_ptr->prev_resel_target.valid = FALSE;
          }
        }
          
        break;
      } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */
                        
#ifdef FEATURE_GSM_TDS
      case RR_RAT_UTRAN_TDD:
      {
        if ( measurement->data.utran_tdd.uarfcn == rr_g2w_data_ptr->prev_resel_target.cell.data.utran_tdd.uarfcn)
        {
          if ( !cell_id_to_check )
          {
            /* This implies all cells on the UARFCN are gone */
            rr_g2w_data_ptr->prev_resel_target.valid = FALSE;
          }
          else if ( measurement->data.utran_tdd.cell_parameter_id == rr_g2w_data_ptr->prev_resel_target.cell.data.utran_tdd.cpid )
          {
            rr_g2w_data_ptr->prev_resel_target.valid = FALSE;
          }
        }

        break;
      } /* FEATURE_GSM_TDS */
#endif /*FEATURE_GSM_TDS*/
                          
      default:
      {
        MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", measurement->rat);
      }
    }        
  }  
}

/*
@detail
This function loops for all the candidate cell entries to see if there is any  cell with Frequency Priority value lower than the given entry running, 
if Found:
(a) Identify first candidate in this ordered list as a Back Up for Reselection, i.e. next candidate for Reselection.
(b) Restart the candidate entry reselection timer with a fixed offset of T-reselection
@param  (a) Prioroty List (b) Priority Entry 
*/

#ifdef FEATURE_LTE
static void rr_irat_adjust_all_running_low_priority_timer_values(rr_irat_priority_list_t*  priority_list_ptr, rr_irat_priority_list_entry_t *input_entry, const gas_id_t gas_id )
{
  uint32 i;
  rr_g2w_data_t                      *rr_g2w_data_ptr;
  uint8                              t_reselection;
  uint32                             t_reselection_ms;
  gprs_scell_info_t                  *scell;
  boolean                            timer_adjusted;
 
  scell = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_RETURN_VOID(scell);

  t_reselection = scell->serving_cell_priority_params.t_reselection;

  t_reselection_ms = T_RESELECTION_MS[t_reselection];
  timer_adjusted= FALSE;
  
  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  
  
  for (i = 0; 
       i < priority_list_ptr->length &&
       i < RR_IRAT_PRIORITY_LIST_LENGTH; 
       ++i)
  {
    rr_irat_priority_list_entry_t *entry = &priority_list_ptr->entries[i];

    if ( entry->priority < input_entry->priority )
    {
      if ( rr_timer_is_running(entry->measurement->thresh_high_timer, gas_id)) 
      {
        rr_timer_extend(entry->measurement->thresh_high_timer, t_reselection_ms, gas_id );
        timer_adjusted= TRUE;
      }
      else if ( rr_timer_is_running(entry->measurement->thresh_low_timer, gas_id))
      {
        rr_timer_extend(entry->measurement->thresh_low_timer, t_reselection_ms, gas_id );        
        timer_adjusted= TRUE;
      }
      else if ( rr_timer_is_running(entry->measurement->h_prio_timer, gas_id))
      {
        rr_timer_extend(entry->measurement->h_prio_timer, t_reselection_ms, gas_id );        
        timer_adjusted= TRUE;
      }


      /* Check if this is the first entry meeting criterion in the ordered list */
      /* If so select this entry as next candidate for reselection, should the reconfirmed measurements for 
         highest priority candidate fails */
      if( !rr_g2w_data_ptr->prev_resel_target.valid && timer_adjusted )
      {
        switch (entry->measurement->rat)
        {
 #ifdef FEATURE_WCDMA
          case RR_RAT_WCDMA:
          {
            rr_g2w_data_ptr->prev_resel_target.valid = TRUE;
            rr_g2w_data_ptr->prev_resel_target.cell.rat = RR_RAT_WCDMA;
            rr_g2w_data_ptr->prev_resel_target.cell.data.wcdma.uarfcn = entry->measurement->data.wcdma.uarfcn;
            rr_g2w_data_ptr->prev_resel_target.cell.data.wcdma.scrambling_code = entry->measurement->data.wcdma.scrambling_code;
            rr_g2w_data_ptr->prev_resel_target.cell.data.wcdma.pn_offset = entry->measurement->data.wcdma.pn_offset;
            rr_g2w_data_ptr->prev_resel_target.cell.data.wcdma.diversity = entry->measurement->data.wcdma.diversity;
            MSG_GERAN_HIGH_3_G("rr_irat_adjust_all_running_low_priority_timer_values G2W Back Up Resel Target Set RAT %d Freq %d PSC %d",
                                rr_g2w_data_ptr->prev_resel_target.cell.rat, rr_g2w_data_ptr->prev_resel_target.cell.data.wcdma.uarfcn, rr_g2w_data_ptr->prev_resel_target.cell.data.wcdma.scrambling_code );
            break;
          } /* RR_RAT_WCDMA */
 #endif /* FEATURE_WCDMA */
                        
 #ifdef FEATURE_LTE
          case RR_RAT_LTE:
          {
            rr_g2w_data_ptr->prev_resel_target.valid = TRUE;
            rr_g2w_data_ptr->prev_resel_target.cell.rat = RR_RAT_LTE;
            rr_g2w_data_ptr->prev_resel_target.cell.data.lte.earfcn = entry->measurement->data.lte.earfcn;
            rr_g2w_data_ptr->prev_resel_target.cell.data.lte.pcid = entry->measurement->data.lte.pcid;
            MSG_GERAN_HIGH_3_G("rr_irat_adjust_all_running_low_priority_timer_values G2W Back Up Resel Target Set RAT %d Freq %d PD IC %d",
                               rr_g2w_data_ptr->prev_resel_target.cell.rat, rr_g2w_data_ptr->prev_resel_target.cell.data.lte.earfcn, rr_g2w_data_ptr->prev_resel_target.cell.data.lte.pcid );
            
            break;
          } /* RR_RAT_LTE */
 #endif /* FEATURE_LTE */
                        
 #ifdef FEATURE_GSM_TDS
          case RR_RAT_UTRAN_TDD:
          {
            rr_g2w_data_ptr->prev_resel_target.valid = TRUE;
            rr_g2w_data_ptr->prev_resel_target.cell.rat = RR_RAT_UTRAN_TDD;
            rr_g2w_data_ptr->prev_resel_target.cell.data.utran_tdd.uarfcn = entry->measurement->data.utran_tdd.uarfcn;
            rr_g2w_data_ptr->prev_resel_target.cell.data.utran_tdd.cpid = entry->measurement->data.utran_tdd.cell_parameter_id;
            rr_g2w_data_ptr->prev_resel_target.cell.data.utran_tdd.diversity = entry->measurement->data.utran_tdd.diversity;
            MSG_GERAN_HIGH_3_G("rr_irat_adjust_all_running_low_priority_timer_values G2W Back Up Resel Target Set RAT %d Freq %d PSC %d", 
                               rr_g2w_data_ptr->prev_resel_target.cell.rat, rr_g2w_data_ptr->prev_resel_target.cell.data.utran_tdd.uarfcn, rr_g2w_data_ptr->prev_resel_target.cell.data.utran_tdd.cpid );
            
            break;
          } /* FEATURE_GSM_TDS */
#endif /*FEATURE_GSM_TDS*/
                          
          default:
          {
            MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", entry->measurement->rat);
          }
        }        
      }
    }
  }
}
#endif /* FEATURE_LTE */


/**
  @brief Deletes the entry at the specified index from the specified
         list of measurements.

  @param ncells The list to modify.
  @param index The index of the element that should be removed.
*/
static void rr_irat_measurements_delete_index(
  rr_irat_measurements_t * measurements,
  uint32                   i,
  const gas_id_t           gas_id
)
{
  uint32 remaining_index;

  RR_NULL_CHECK_RETURN_VOID(measurements);

  if (i < measurements->no_of_entries && i < RR_IRAT_MEASUREMENTS_PER_RAT)
  {
    if (measurements->entries[i].resel_timer != RR_TIMER_UNDEFINED)
    {
      (void)rr_timer_cancel(measurements->entries[i].resel_timer, gas_id);

      if (measurements->entries[i].resel_timer_flag != NULL)
      {
        *measurements->entries[i].resel_timer_flag = FALSE;
        measurements->entries[i].resel_timer_flag = NULL;
      }

      measurements->entries[i].resel_timer = RR_TIMER_UNDEFINED;
    }

#ifdef FEATURE_LTE
    (void)rr_timer_cancel(measurements->entries[i].thresh_high_timer, gas_id);
    (void)rr_timer_cancel(measurements->entries[i].thresh_low_timer, gas_id);
    (void)rr_timer_cancel(measurements->entries[i].h_prio_timer, gas_id);
    if (measurements->entries[i].priority_resel_timer_flag != NULL)
    {
      *measurements->entries[i].priority_resel_timer_flag = FALSE;
      measurements->entries[i].priority_resel_timer_flag = NULL;
    }
    measurements->entries[i].thresh_high_timer = RR_TIMER_UNDEFINED;
    measurements->entries[i].thresh_low_timer = RR_TIMER_UNDEFINED;
    measurements->entries[i].h_prio_timer = RR_TIMER_UNDEFINED;
#endif /* FEATURE_LTE */

   rr_irat_check_and_delete_prev_resel_target(&measurements->entries[i], TRUE, gas_id );

    /*******************************************************************/
    /* Delete the current entry in the surrounding wcdma    database.  */
    /* Shift the remainder of the list down 1 place to fill in the gap */
    /*******************************************************************/
    for (remaining_index = i + 1;
         remaining_index < measurements->no_of_entries &&
         remaining_index < RR_IRAT_MEASUREMENTS_PER_RAT;
         ++remaining_index)
    {
      measurements->entries[remaining_index-1] = measurements->entries[remaining_index];
    }

    if (measurements->no_of_entries > 0)
    {
      --measurements->no_of_entries;
    }
    else
    {
      ERR("Invalid entry=%d in wcdma Surround DB=%d", i, measurements->no_of_entries, 0);
    }

    /* Clear out the last entry as it is just a copy of the last-but-one entry
    now. */
    measurements->entries[measurements->no_of_entries].resel_timer_flag = NULL;
    measurements->entries[measurements->no_of_entries].resel_timer = RR_TIMER_UNDEFINED;
#ifdef FEATURE_LTE
    measurements->entries[measurements->no_of_entries].priority_resel_timer_flag = NULL;
    measurements->entries[measurements->no_of_entries].thresh_high_timer = RR_TIMER_UNDEFINED;
    measurements->entries[measurements->no_of_entries].thresh_low_timer = RR_TIMER_UNDEFINED;
    measurements->entries[measurements->no_of_entries].h_prio_timer = RR_TIMER_UNDEFINED;
#endif /* FEATURE_LTE */
  }
  else
  {
    ERR("invalid entry: %d , size = %d", i, measurements->no_of_entries, 0);
  }
}

#ifdef FEATURE_WCDMA
/**
 * Finds an entry in the surrounding database matching the given UARFCN & Scrambling_code
 * and returns a TRUE if entry is found, FALSE otherwise.
 *
 * @param surrounding_db_ptr surrounding database pointer
 *
 * @param ARFCN              ARFCN to search for
 *
 * @return TRUE if entry is found, otherwise FALSE
 */
static boolean rr_g2wr_find_wcdma_surround_database_entry(
  rr_irat_measurements_t * measurements,
  word                     UARFCN,
  word                     Scrambling_code
)
{
  uint32 i;

  for (i = 0; i < measurements->no_of_entries && i < ARR_SIZE(measurements->entries); ++i)
  {
    if (measurements->entries[i].rat == RR_RAT_WCDMA &&
        measurements->entries[i].data.wcdma.uarfcn == UARFCN &&
        measurements->entries[i].data.wcdma.scrambling_code == Scrambling_code)
    {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * Determines if the surrounding wcdma cell (ncell) is in the best
 * six wcdma measurement report (returns TRUE if it is, FALSE otherwise).
 *
 * @ncell
 *                         pointer to surrounding wcdma database entry
 *
 * @param p_best_six_info  pointer to best wcdma six info from measurement report
 *
 * @return TRUE if wcdma cell is still in best six set
 */
static boolean rr_g2wr_surrounding_wcdma_cell_in_best_six(
  rr_irat_measurement_t * measurement,
  best_six_wcdma_info_T * new_measurements
)
{
  RR_NULL_CHECK_RETURN_PARAM(measurement, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(new_measurements, FALSE);

  if (measurement->rat == RR_RAT_WCDMA)
  {
    surrounding_wcdma_channel_info_T * new_measurement;
    uint32 i;

    for (i = 0; i < new_measurements->no_of_entries; ++i)
    {
      new_measurement = &new_measurements->surrounding_wcdma_channel_info[i];

      if (measurement->data.wcdma.uarfcn == new_measurement->UARFCN &&
          measurement->data.wcdma.scrambling_code == new_measurement->Scrambling_code)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}
#endif /* FEATURE_WCDMA */

/**
  @brief Delete any entries in the measurements data structure that match
         the specified value, using the specified match function.

  @param measurements The measurements data structure to modify.
  @param match_fn Function to use to determine whether a measurement matches
         the value specified.
  @param match_value The value that is being searched for - if a measurement
         entry matches this value, it will be removed from the measurement
         structure.
*/
static void rr_irat_measurements_delete_matching_entries(
  rr_irat_measurements_t       * measurements,
  rr_irat_measurement_match_fn   match_fn,
  void                         * match_value,
  const gas_id_t                 gas_id
)
{
  uint32 i;

  RR_ASSERT_RETURN_VOID(measurements != NULL);
  RR_ASSERT_RETURN_VOID(match_fn != NULL);
  RR_ASSERT_RETURN_VOID(match_value != NULL);

  RR_ASSERT_RETURN_VOID(measurements->no_of_entries <= RR_IRAT_MEASUREMENTS_PER_RAT);

  for (i = measurements->no_of_entries; i > 0; --i)
  {
    if (match_fn(&measurements->entries[i-1], match_value))
    {
      rr_irat_measurements_delete_index(measurements, i-1, gas_id);
    }
  }
}


#ifdef FEATURE_WCDMA
static boolean rr_irat_measurement_match_wcdma_uarfcn(
  rr_irat_measurement_t * measurement,
  void                  * value
)
{
  uint16 * uarfcn;

  RR_NULL_CHECK_RETURN_PARAM(measurement, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(value, FALSE);

  uarfcn = (uint16 *) value;

  if (measurement->rat == RR_RAT_WCDMA &&
      measurement->data.wcdma.uarfcn == *uarfcn)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
static boolean rr_irat_measurement_match_tdd_uarfcn(
  rr_irat_measurement_t * measurement,
  void                  * value
)
{
  uint16 * uarfcn;

  RR_NULL_CHECK_RETURN_PARAM(measurement, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(value, FALSE);

  uarfcn = (uint16 *) value;

  if (measurement->rat == RR_RAT_UTRAN_TDD &&
      measurement->data.utran_tdd.uarfcn == *uarfcn)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

static int rr_irat_measurement_utran_tdd_cmp_rscp_descending(
  const void * v1,
  const void * v2
)
{
  const rr_irat_measurement_t * m1;
  const rr_irat_measurement_t * m2;

  RR_NULL_CHECK_RETURN_PARAM(v1, 0);
  RR_NULL_CHECK_RETURN_PARAM(v2, 0);

  m1 = (const rr_irat_measurement_t *) v1;
  m2 = (const rr_irat_measurement_t *) v2;

  if (m2->data.utran_tdd.rscp > m1->data.utran_tdd.rscp)
  {
    return 1;
  }
  else if (m2->data.utran_tdd.rscp < m1->data.utran_tdd.rscp)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}
#endif /*FEATURE_GSM_TDS*/


#ifdef FEATURE_WCDMA
static int rr_irat_measurement_wcdma_cmp_rscp_descending(
  const void * v1,
  const void * v2
)
{
  const rr_irat_measurement_t * m1;
  const rr_irat_measurement_t * m2;

  RR_NULL_CHECK_RETURN_PARAM(v1, 0);
  RR_NULL_CHECK_RETURN_PARAM(v2, 0);

  m1 = (const rr_irat_measurement_t *) v1;
  m2 = (const rr_irat_measurement_t *) v2;

  if (m2->data.wcdma.rscp > m1->data.wcdma.rscp)
  {
    return 1;
  }
  else if (m2->data.wcdma.rscp < m1->data.wcdma.rscp)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}


static int rr_l1_wcdma_measurement_cmp_rscp_descending(
  const void * v1,
  const void * v2
)
{
  const rr_l1_wcdma_measurement_t * m1;
  const rr_l1_wcdma_measurement_t * m2;

  RR_NULL_CHECK_RETURN_PARAM(v1, 0);
  RR_NULL_CHECK_RETURN_PARAM(v2, 0);

  m1 = (const rr_l1_wcdma_measurement_t *) v1;
  m2 = (const rr_l1_wcdma_measurement_t *) v2;

  if (m2->rscp > m1->rscp)
  {
    return 1;
  }
  else if (m2->rscp < m1->rscp)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
static int rr_l1_utran_tdd_measurement_cmp_rscp_descending(
  const void * v1,
  const void * v2
)
{
  const rr_l1_utran_tdd_measurement_t * m1;
  const rr_l1_utran_tdd_measurement_t * m2;

  RR_NULL_CHECK_RETURN_PARAM(v1, 0);
  RR_NULL_CHECK_RETURN_PARAM(v2, 0);

  m1 = (const rr_l1_utran_tdd_measurement_t *) v1;
  m2 = (const rr_l1_utran_tdd_measurement_t *) v2;

  if (m2->rscp > m1->rscp)
  {
    return 1;
  }
  else if (m2->rscp < m1->rscp)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}
#endif /*FEATURE_GSM_TDS*/


#ifdef FEATURE_LTE
static int rr_irat_measurement_lte_cmp_rsrp_descending(
  const void * v1,
  const void * v2
)
{
  const rr_irat_measurement_t * m1;
  const rr_irat_measurement_t * m2;

  RR_NULL_CHECK_RETURN_PARAM(v1, 0);
  RR_NULL_CHECK_RETURN_PARAM(v2, 0);

  m1 = (const rr_irat_measurement_t *) v1;
  m2 = (const rr_irat_measurement_t *) v2;

  if (m2->data.lte.rsrp > m1->data.lte.rsrp)
  {
    return 1;
  }
  else if (m2->data.lte.rsrp < m1->data.lte.rsrp)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

static int rr_l1_lte_measurement_cmp_rsrp_descending(
  const void * v1,
  const void * v2
)
{
  const rr_l1_lte_measurement_t * m1;
  const rr_l1_lte_measurement_t * m2;

  RR_NULL_CHECK_RETURN_PARAM(v1, 0);
  RR_NULL_CHECK_RETURN_PARAM(v2, 0);

  m1 = (const rr_l1_lte_measurement_t *) v1;
  m2 = (const rr_l1_lte_measurement_t *) v2;

  if (m2->rsrp > m1->rsrp)
  {
    return 1;
  }
  else if (m2->rsrp < m1->rsrp)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}
#endif /*FEATURE_LTE*/

#ifdef FEATURE_WCDMA
static void rr_irat_store_wcdma_measurements(
  rr_l1_wcdma_measurements_t * new_wcdma_measurements,
  const gas_id_t               gas_id
)
{
  rr_irat_measurements_t *wcdma_measurements;
  rr_g2w_data_t *rr_g2w_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  RR_NULL_CHECK_RETURN_VOID(new_wcdma_measurements);

  MSG_GERAN_HIGH_2_G("Received %d WCDMA measurements for UARFCN %d",
           new_wcdma_measurements->no_of_entries,
           new_wcdma_measurements->uarfcn);

  wcdma_measurements = &rr_g2w_data_ptr->wcdma_measurements;

  if (new_wcdma_measurements->no_of_entries == 0)
  {
    /* We just need to delete any entries in the WCDMA ncells list that match
    the UARFCN indicated in this measurement report. */
    rr_irat_measurements_delete_matching_entries(
      wcdma_measurements,
      rr_irat_measurement_match_wcdma_uarfcn,
      &new_wcdma_measurements->uarfcn,
      gas_id
    );
  }
  else
  {
    uint32 i;

    if (new_wcdma_measurements->no_of_entries > MAX_WCDMA_MEASUREMENTS_PER_FREQUENCY)
    {
      MSG_GERAN_ERROR_2_G(
        "UTRAN FDD no_of_entries:%d, out of range, max:%d",
        new_wcdma_measurements->no_of_entries,
        MAX_WCDMA_MEASUREMENTS_PER_FREQUENCY
      );
      new_wcdma_measurements->no_of_entries = MAX_WCDMA_MEASUREMENTS_PER_FREQUENCY;
    }

    for (i = 0; i < new_wcdma_measurements->no_of_entries; ++i)
    {
      MSG_GERAN_HIGH_3_G("  PSC: %d, RSCP: %d, Ec/No: %d",
               new_wcdma_measurements->entries[i].scrambling_code,
               new_wcdma_measurements->entries[i].rscp,
               new_wcdma_measurements->entries[i].ecno);
    }

    /* Now, delete any entries in the ncells store for the UARFCN in the new
    measurements if the entry is not included in the new measurements. */
    for (i = wcdma_measurements->no_of_entries; i > 0; --i)
    {
      if (wcdma_measurements->entries[i-1].data.wcdma.uarfcn == new_wcdma_measurements->uarfcn)
      {
        uint32 j;

        for (j = 0; j < new_wcdma_measurements->no_of_entries && j < MAX_WCDMA_MEASUREMENTS_PER_FREQUENCY; ++j)
        {
          if (wcdma_measurements->entries[i-1].data.wcdma.scrambling_code ==
              new_wcdma_measurements->entries[j].scrambling_code)
          {
            break;
          }
        }

        if (j == new_wcdma_measurements->no_of_entries)
        {
          /* The WCDMA ncell is not present in the new measurements from L1, so
          should be deleted from the store. */
          rr_irat_measurements_delete_index(wcdma_measurements, i-1, gas_id);
        }
      }
    }

    /* Now iterate over the new measurements, adding or updating values in the
    store.  If we try to add a value and the store is full, we need to make sure
    that we make room for the new value by removing the weakest entry, but only
    if the entry that we're adding has greater RSCP than the one we are removing. */

    /* Sort the new measurements by RSCP, in descending order. */
    qsort(
      new_wcdma_measurements->entries,
      new_wcdma_measurements->no_of_entries,
      sizeof(rr_l1_wcdma_measurement_t),
      rr_l1_wcdma_measurement_cmp_rscp_descending
    );

    for (i = 0;
         i < MIN(new_wcdma_measurements->no_of_entries, MAX_WCDMA_MEASUREMENTS_PER_FREQUENCY);
         ++i)
    {
      byte position = rr_irat_measurements_find_wcdma_cell_index(
        wcdma_measurements,
        new_wcdma_measurements->uarfcn,
        new_wcdma_measurements->entries[i].scrambling_code
      );

      if (position == RR_IRAT_MEASUREMENTS_PER_RAT)
      {
        /* cell is not present in the database, need to add it if there is space */
        if (wcdma_measurements->no_of_entries < RR_IRAT_MEASUREMENTS_PER_RAT)
        {
          rr_g2wr_add_wcdma_surrounding_database_entry(
            wcdma_measurements,
            new_wcdma_measurements->uarfcn,
            new_wcdma_measurements->entries[i].scrambling_code,
            new_wcdma_measurements->entries[i].diversity,
            new_wcdma_measurements->entries[i].rscp,
            new_wcdma_measurements->entries[i].ecno,
            new_wcdma_measurements->entries[i].rssi,
            new_wcdma_measurements->entries[i].pn_offset,
            gas_id
          );
        }
        else
        {
          RR_ASSERT_RETURN_VOID(wcdma_measurements->no_of_entries == RR_IRAT_MEASUREMENTS_PER_RAT);

          /* the WCDMA database is full.  Replace the lowest RSCP cell with the current
          one, if it is better. */

          qsort(
            wcdma_measurements->entries,
            MIN(wcdma_measurements->no_of_entries, RR_IRAT_MEASUREMENTS_PER_RAT),
            sizeof(rr_irat_measurement_t),
            rr_irat_measurement_wcdma_cmp_rscp_descending
          );

          if (new_wcdma_measurements->entries[i].rscp > wcdma_measurements->entries[RR_IRAT_MEASUREMENTS_PER_RAT - 1].data.wcdma.rscp)
          {
            rr_irat_measurements_delete_index(
              wcdma_measurements,
              RR_IRAT_MEASUREMENTS_PER_RAT - 1,
              gas_id
            );

            rr_g2wr_add_wcdma_surrounding_database_entry(
              wcdma_measurements,
              new_wcdma_measurements->uarfcn,
              new_wcdma_measurements->entries[i].scrambling_code,
              new_wcdma_measurements->entries[i].diversity,
              new_wcdma_measurements->entries[i].rscp,
              new_wcdma_measurements->entries[i].ecno,
              new_wcdma_measurements->entries[i].rssi,
              new_wcdma_measurements->entries[i].pn_offset,
              gas_id
            );
          }
        }
      }
      else
      {
        /* cell was found, just update it's RSCP to the new value */
        rr_irat_measurement_t * cell = &wcdma_measurements->entries[position];

        cell->data.wcdma.rscp = new_wcdma_measurements->entries[i].rscp;
        cell->data.wcdma.ecno = new_wcdma_measurements->entries[i].ecno;
        cell->data.wcdma.rssi = new_wcdma_measurements->entries[i].rssi;
        cell->data.wcdma.pn_offset = new_wcdma_measurements->entries[i].pn_offset;
        cell->data.wcdma.xfer_validity_counter = 0;
      }
    }
  }
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
static void rr_irat_store_utran_tdd_measurements(
  rr_l1_utran_tdd_measurements_t * new_utran_tdd_measurements,
  const gas_id_t gas_id
)
{
  rr_irat_measurements_t * utran_tdd_measurements;
  rr_g2w_data_t *rr_g2w_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  RR_NULL_CHECK_RETURN_VOID(new_utran_tdd_measurements);

  MSG_GERAN_HIGH_2_G("Received %d UTRAN TDD measurements for UARFCN %d",
           new_utran_tdd_measurements->no_of_entries,
           new_utran_tdd_measurements->uarfcn);

  utran_tdd_measurements = &rr_g2w_data_ptr->utran_tdd_measurements;

  if (new_utran_tdd_measurements->no_of_entries == 0)
  {
    /* We just need to delete any entries in the UTRAN TDD ncells list that match
    the UARFCN indicated in this measurement report. */
    rr_irat_measurements_delete_matching_entries(
      utran_tdd_measurements,
      rr_irat_measurement_match_tdd_uarfcn,
      &new_utran_tdd_measurements->uarfcn,
      gas_id
    );
  }
  else
  {
    uint32 i;

    if (new_utran_tdd_measurements->no_of_entries > MAX_UTRAN_TDD_MEASUREMENTS_PER_FREQUENCY)
    {
      MSG_GERAN_ERROR_2_G("UTRAN TDD no_of_entries:%d, out of range, max:%d",new_utran_tdd_measurements->no_of_entries,
                                                                   MAX_UTRAN_TDD_MEASUREMENTS_PER_FREQUENCY);
      new_utran_tdd_measurements->no_of_entries = MAX_UTRAN_TDD_MEASUREMENTS_PER_FREQUENCY;
    }

    for (i = 0; i < new_utran_tdd_measurements->no_of_entries; ++i)
    {
      MSG_GERAN_HIGH_2_G("TDS CPID: %d, RSCP: %d",
               new_utran_tdd_measurements->entries[i].cell_parameter_id,
               new_utran_tdd_measurements->entries[i].rscp);
    }

    /* Now, delete any entries in the ncells store for the UARFCN in the new
    measurements if the entry is not included in the new measurements. */
    for (i = utran_tdd_measurements->no_of_entries; i > 0; --i)
    {
      if (utran_tdd_measurements->entries[i-1].data.utran_tdd.uarfcn == new_utran_tdd_measurements->uarfcn)
      {
        uint32 j;

        for (j = 0; j < new_utran_tdd_measurements->no_of_entries && j < MAX_UTRAN_TDD_MEASUREMENTS_PER_FREQUENCY; ++j)
        {
          if (utran_tdd_measurements->entries[i-1].data.utran_tdd.cell_parameter_id ==
              new_utran_tdd_measurements->entries[j].cell_parameter_id)
          {
            break;
          }
        }

        if (j == new_utran_tdd_measurements->no_of_entries)
        {
          /* The WCDMA ncell is not present in the new measurements from L1, so
          should be deleted from the store. */
          rr_irat_measurements_delete_index(utran_tdd_measurements, i-1, gas_id);
        }
      }
    }

    /* Now iterate over the new measurements, adding or updating values in the
    store.  If we try to add a value and the store is full, we need to make sure
    that we make room for the new value by removing the weakest entry, but only
    if the entry that we're adding has greater RSCP than the one we are removing. */

    /* Sort the new measurements by RSCP, in descending order. */
    qsort(
      new_utran_tdd_measurements->entries,
      new_utran_tdd_measurements->no_of_entries,
      sizeof(rr_l1_utran_tdd_measurement_t),
      rr_l1_utran_tdd_measurement_cmp_rscp_descending
    );

    for (i = 0;
         i < MIN(new_utran_tdd_measurements->no_of_entries, MAX_UTRAN_TDD_MEASUREMENTS_PER_FREQUENCY);
         ++i)
    {
      byte position = rr_irat_measurements_find_utran_tdd_cell_index(
        utran_tdd_measurements,
        new_utran_tdd_measurements->uarfcn,
        new_utran_tdd_measurements->entries[i].cell_parameter_id
      );

      if (position == RR_IRAT_MEASUREMENTS_PER_RAT)
      {
        /* cell is not present in the database, need to add it if there is space */
        if (utran_tdd_measurements->no_of_entries < RR_IRAT_MEASUREMENTS_PER_RAT)
        {
          rr_g2wr_add_utran_tdd_surrounding_database_entry(
            utran_tdd_measurements,
            new_utran_tdd_measurements->uarfcn,
            new_utran_tdd_measurements->entries[i].cell_parameter_id,
            new_utran_tdd_measurements->entries[i].diversity,
            new_utran_tdd_measurements->entries[i].rscp,
            new_utran_tdd_measurements->ts0_pos,
            gas_id
          );
        }
        else
        {
          RR_ASSERT_RETURN_VOID(utran_tdd_measurements->no_of_entries == RR_IRAT_MEASUREMENTS_PER_RAT);

          /* the UTRAN TDD database is full.  Replace the lowest RSCP cell with the current
          one, if it is better. */

          qsort(
            utran_tdd_measurements->entries,
            MIN(utran_tdd_measurements->no_of_entries, RR_IRAT_MEASUREMENTS_PER_RAT),
            sizeof(rr_irat_measurement_t),
            rr_irat_measurement_utran_tdd_cmp_rscp_descending
          );

          if (new_utran_tdd_measurements->entries[i].rscp > utran_tdd_measurements->entries[RR_IRAT_MEASUREMENTS_PER_RAT - 1].data.utran_tdd.rscp)
          {
            rr_irat_measurements_delete_index(
              utran_tdd_measurements,
              RR_IRAT_MEASUREMENTS_PER_RAT - 1,
              gas_id
            );

            rr_g2wr_add_utran_tdd_surrounding_database_entry(
              utran_tdd_measurements,
              new_utran_tdd_measurements->uarfcn,
              new_utran_tdd_measurements->entries[i].cell_parameter_id,
              new_utran_tdd_measurements->entries[i].diversity,
              new_utran_tdd_measurements->entries[i].rscp,
              new_utran_tdd_measurements->ts0_pos,
              gas_id
            );
          }
        }
      }
      else
      {
        /* cell was found, just update it's RSCP to the new value */
        rr_irat_measurement_t * cell = &utran_tdd_measurements->entries[position];

        cell->data.utran_tdd.rscp = new_utran_tdd_measurements->entries[i].rscp;
        cell->data.utran_tdd.ts0_pos = new_utran_tdd_measurements->ts0_pos;
      }
    }
  }
}
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
static boolean rr_irat_measurement_match_lte_earfcn(
  rr_irat_measurement_t * measurement,
  void                  * value
)
{
  lte_earfcn_t * earfcn;

  RR_NULL_CHECK_RETURN_PARAM(measurement, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(value, FALSE);

  earfcn = (lte_earfcn_t *) value;

  if (measurement->rat == RR_RAT_LTE &&
      measurement->data.lte.earfcn == *earfcn)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

static void rr_irat_store_lte_measurements(
  rr_l1_lte_measurements_t * new_lte_measurements,
  const gas_id_t gas_id
)
{
  rr_irat_measurements_t * lte_measurements;
  rr_g2w_data_t *rr_g2w_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  RR_NULL_CHECK_RETURN_VOID(new_lte_measurements);

  MSG_GERAN_HIGH_2_G("Received %d LTE measurements for EARFCN %d",
           new_lte_measurements->no_of_entries,
           new_lte_measurements->earfcn);

  lte_measurements = &rr_g2w_data_ptr->lte_measurements;

  if (new_lte_measurements->no_of_entries == 0)
  {
    /* We just need to delete any entries in the LTE ncells list that match
    the UARFCN indicated in this measurement report. */
    rr_irat_measurements_delete_matching_entries(
      lte_measurements,
      rr_irat_measurement_match_lte_earfcn,
      &new_lte_measurements->earfcn,
      gas_id
    );
  }
  else
  {
    uint32 i;

    if (new_lte_measurements->no_of_entries > MAX_LTE_MEASUREMENTS_PER_FREQUENCY)
    {
      MSG_GERAN_ERROR_2_G(
        "LTE no_of_entries:%d, out of range, max:%d",
        new_lte_measurements->no_of_entries,
        MAX_LTE_MEASUREMENTS_PER_FREQUENCY
      );
      new_lte_measurements->no_of_entries = MAX_LTE_MEASUREMENTS_PER_FREQUENCY;
    }

    for (i = 0; i < new_lte_measurements->no_of_entries; ++i)
    {
      MSG_GERAN_HIGH_3_G("  PCID: %d, RSRP: %d, RSRQ: %d",
               new_lte_measurements->entries[i].pcid,
               new_lte_measurements->entries[i].rsrp,
               new_lte_measurements->entries[i].rsrq);
    }

    /* Now, delete any entries in the ncells store for the UARFCN in the new
    measurements if the entry is not included in the new measurements. */
    for (i = lte_measurements->no_of_entries; i > 0; --i)
    {
      if (lte_measurements->entries[i-1].data.lte.earfcn == new_lte_measurements->earfcn)
      {
        uint32 j;

        for (j = 0; j < new_lte_measurements->no_of_entries && j < MAX_LTE_MEASUREMENTS_PER_FREQUENCY; ++j)
        {
          if (lte_measurements->entries[i-1].data.lte.pcid ==
              new_lte_measurements->entries[j].pcid)
          {
            break;
          }
        }

        if (j == new_lte_measurements->no_of_entries)
        {
          /* The LTE ncell is not present in the new measurements from L1, so
          should be deleted from the store. */
          rr_irat_measurements_delete_index(lte_measurements, i-1, gas_id);
        }
      }
    }

    /* Now iterate over the new measurements, adding or updating values in the
    store.  If we try to add a value and the store is full, we need to make sure
    that we make room for the new value by removing the weakest entry, but only
    if the entry that we're adding has greater RSCP than the one we are removing. */

    /* Sort the new measurements by RSCP, in descending order. */
    qsort(
      new_lte_measurements->entries,
      new_lte_measurements->no_of_entries,
      sizeof(rr_l1_lte_measurement_t),
      rr_l1_lte_measurement_cmp_rsrp_descending
    );

    for (i = 0;
         i < MIN(new_lte_measurements->no_of_entries, MAX_LTE_MEASUREMENTS_PER_FREQUENCY);
         ++i)
    {
      byte position = rr_irat_measurements_find_lte_cell_index(
        lte_measurements,
        new_lte_measurements->earfcn,
        new_lte_measurements->entries[i].pcid
      );

      if (position == RR_IRAT_MEASUREMENTS_PER_RAT)
      {
        /* cell is not present in the database, need to add it if there is space */
        if (lte_measurements->no_of_entries < RR_IRAT_MEASUREMENTS_PER_RAT)
        {
          rr_g2wr_add_lte_surrounding_database_entry(
            lte_measurements,
            new_lte_measurements->earfcn,
            new_lte_measurements->entries[i].pcid,
            new_lte_measurements->entries[i].rsrp,
            new_lte_measurements->entries[i].rsrq,
            gas_id
          );
        }
        else
        {
          RR_ASSERT_RETURN_VOID(lte_measurements->no_of_entries == RR_IRAT_MEASUREMENTS_PER_RAT);

          /* the LTE database is full.  Replace the lowest RSCP cell with the current
          one, if it is better. */

          qsort(
            lte_measurements->entries,
            MIN(lte_measurements->no_of_entries, RR_IRAT_MEASUREMENTS_PER_RAT),
            sizeof(rr_irat_measurement_t),
            rr_irat_measurement_lte_cmp_rsrp_descending
          );

          if (new_lte_measurements->entries[i].rsrp > lte_measurements->entries[RR_IRAT_MEASUREMENTS_PER_RAT - 1].data.lte.rsrp)
          {
            rr_irat_measurements_delete_index(
              lte_measurements,
              RR_IRAT_MEASUREMENTS_PER_RAT - 1,
              gas_id
            );

            rr_g2wr_add_lte_surrounding_database_entry(
              lte_measurements,
              new_lte_measurements->earfcn,
              new_lte_measurements->entries[i].pcid,
              new_lte_measurements->entries[i].rsrp,
              new_lte_measurements->entries[i].rsrq,
              gas_id
            );
          }
        }
      }
      else
      {
        /* cell was found, just update it's RSCP to the new value */
        rr_irat_measurement_t * cell = &lte_measurements->entries[position];

        MSG_GERAN_MED_3_G("New RSRP: %d for LTE cell %d,%d",
          new_lte_measurements->entries[i].rsrp,
          new_lte_measurements->earfcn,
          new_lte_measurements->entries[i].pcid);

        cell->data.lte.rsrp = new_lte_measurements->entries[i].rsrp;
        cell->data.lte.rsrq = new_lte_measurements->entries[i].rsrq;
      }
    }
  }
}
#endif /*FEATURE_LTE*/

#ifdef FEATURE_WCDMA
static void rr_irat_dump_wcdma_measurements(const gas_id_t gas_id)
{
  rr_irat_measurements_t * measurements;
  uint32 i;
  rr_g2w_data_t *rr_g2w_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  measurements = &rr_g2w_data_ptr->wcdma_measurements;

  MSG_GERAN_MED_0_G("WCDMA Measurements:");

  for (i = 0; i < measurements->no_of_entries && i < RR_IRAT_MEASUREMENTS_PER_RAT; ++i)
  {
    MSG_5(MSG_SSID_DFLT,
          MSG_LEGACY_HIGH,
          " UARFCN: %d, PSC: %d, RSCP: %d, Ec/No: %d, RSSI: %d",
          measurements->entries[i].data.wcdma.uarfcn,
          measurements->entries[i].data.wcdma.scrambling_code,
          measurements->entries[i].data.wcdma.rscp,
          measurements->entries[i].data.wcdma.ecno,
          measurements->entries[i].data.wcdma.rssi);
  }
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
static void rr_irat_dump_utran_tdd_measurements(const gas_id_t gas_id)
{
  rr_irat_measurements_t * measurements;
  uint32 i;
  rr_g2w_data_t *rr_g2w_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  measurements = &rr_g2w_data_ptr->utran_tdd_measurements;

  MSG_GERAN_MED_0_G("UTRAN TDD Measurements:");

  for (i = 0; i < measurements->no_of_entries && i < RR_IRAT_MEASUREMENTS_PER_RAT; ++i)
  {
    MSG_GERAN_MED_3_G(" UARFCN: %d, CPID: %d, RSCP: %d",
            measurements->entries[i].data.utran_tdd.uarfcn,
            measurements->entries[i].data.utran_tdd.cell_parameter_id,
            measurements->entries[i].data.utran_tdd.rscp);
  }
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
static void rr_irat_dump_lte_measurements(const gas_id_t gas_id)
{
  rr_irat_measurements_t * measurements;
  uint32 i;
  rr_g2w_data_t *rr_g2w_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  measurements = &rr_g2w_data_ptr->lte_measurements;

  MSG_GERAN_MED_0_G("LTE Measurements:");

  for (i = 0; i < measurements->no_of_entries && i < RR_IRAT_MEASUREMENTS_PER_RAT; ++i)
  {
    MSG_4(MSG_SSID_DFLT,
          MSG_LEGACY_HIGH,
          " EARFCN: %d, PCID: %d, RSRP: %d, RSRQ: %d",
          measurements->entries[i].data.lte.earfcn,
          measurements->entries[i].data.lte.pcid,
          measurements->entries[i].data.lte.rsrp,
          measurements->entries[i].data.lte.rsrq);
  }
}


static void rr_irat_priority_list_populate(
  rr_irat_priority_list_t * priority_list,
#ifdef FEATURE_WCDMA
  rr_irat_measurements_t  * wcdma_measurements,
#endif /* FEATURE_WCDMA */
  rr_irat_measurements_t  * lte_measurements,
#ifdef FEATURE_GSM_TDS
  rr_irat_measurements_t  * tds_measurements,
#endif /*FEATURE_GSM_TDS*/
  const gas_id_t            gas_id
)
{
  uint32 i;

#ifdef FEATURE_WCDMA
  for (i = 0; i < wcdma_measurements->no_of_entries; ++i)
  {
    rr_irat_measurement_t * measurement = &wcdma_measurements->entries[i];
    rr_irat_cell_t cell;

    cell.rat = RR_RAT_WCDMA;
    cell.data.wcdma.uarfcn = measurement->data.wcdma.uarfcn;
    cell.data.wcdma.scrambling_code = measurement->data.wcdma.scrambling_code;
    cell.data.wcdma.diversity = measurement->data.wcdma.diversity;

    if (rr_g2x_resel_to_irat_cell_is_allowed(&cell, gas_id))
    {
      rr_resel_priority_e priority = rr_get_resel_priority_for_uarfcn(
        measurement->data.wcdma.uarfcn,
        RR_UTRAN_FDD,
        gas_id
      );

      /* Do not add a cell if its priority is not available */
      if (priority != RR_RESEL_PRIORITY_UNDEFINED)
      {
        rr_irat_priority_list_entry_t * priority_list_entry = &priority_list->entries[priority_list->length];
  
        priority_list_entry->measurement = measurement;
        priority_list_entry->priority = priority;
        priority_list_entry->s_value = measurement->data.wcdma.rscp - rr_get_qrxlevmin_for_uarfcn(measurement->data.wcdma.uarfcn, RR_UTRAN_FDD, gas_id);
        priority_list_entry->thresh_high = rr_get_thresh_utran_high_for_uarfcn(measurement->data.wcdma.uarfcn, RR_UTRAN_FDD, gas_id);
        priority_list_entry->thresh_low = rr_get_thresh_utran_low_for_uarfcn(measurement->data.wcdma.uarfcn, RR_UTRAN_FDD, gas_id);
  
        ++priority_list->length;
      }
      else
      {
        MSG_GERAN_HIGH_1_G("Priority not available for UTRAN FDD freq %d", measurement->data.wcdma.uarfcn);
      }
    }
  }
#endif /* FEATURE_WCDMA */

  for (i = 0; i < lte_measurements->no_of_entries; ++i)
  {
    rr_irat_measurement_t * measurement = &lte_measurements->entries[i];
    rr_irat_cell_t cell;

    cell.rat = RR_RAT_LTE;
    cell.data.lte.earfcn = measurement->data.lte.earfcn;
    cell.data.lte.pcid = measurement->data.lte.pcid;

   if (rr_g2x_resel_to_irat_cell_is_allowed(&cell, gas_id))
   {
     
      rr_resel_priority_e priority = rr_get_resel_priority_for_earfcn(
        measurement->data.lte.earfcn,
        gas_id
      );

      /* Do not add a cell if its priority is not available */
      if (priority != RR_RESEL_PRIORITY_UNDEFINED)
      {
        rr_irat_priority_list_entry_t * priority_list_entry = &priority_list->entries[priority_list->length];
  
        priority_list_entry->measurement = measurement;
  
  #ifdef FEATURE_LTE_REL11 
        /*check whether earfcn has be deprioritized by LTE*/
        if (rr_irat_lte_arfcn_deprioritized(measurement->data.lte,gas_id) == TRUE)
        {
          /* deprioritized requires that earfcn be set below lowest allowable priority*/
          priority_list_entry->priority = RR_RESEL_PRIORITY_UNDEFINED;
        }
        else
        {
          priority_list_entry->priority = priority;
        }
  #else /* FEATURE_LTE_REL11 */
        priority_list_entry->priority = priority;
  #endif /* FEATURE_LTE_REL11 */
       
        priority_list_entry->s_value = measurement->data.lte.rsrp - rr_get_qrxlevmin_for_earfcn(measurement->data.lte.earfcn, gas_id);
        priority_list_entry->thresh_high = rr_get_thresh_high_for_earfcn(measurement->data.lte.earfcn, gas_id);
        priority_list_entry->thresh_low = rr_get_thresh_low_for_earfcn(measurement->data.lte.earfcn, gas_id);
  
        ++priority_list->length;
      }
      else
      {
        MSG_GERAN_HIGH_1_G("Priority not available for LTE freq %d", measurement->data.lte.earfcn);
      }
    }
  }

#ifdef FEATURE_GSM_TDS
  for (i = 0; i < tds_measurements->no_of_entries; ++i)
  {
    rr_irat_measurement_t * measurement = &tds_measurements->entries[i];
    rr_irat_cell_t cell;

    cell.rat = RR_RAT_UTRAN_TDD;
    cell.data.utran_tdd.uarfcn = measurement->data.utran_tdd.uarfcn;
    cell.data.utran_tdd.cpid = measurement->data.utran_tdd.cell_parameter_id;
    cell.data.utran_tdd.diversity = measurement->data.utran_tdd.diversity;

    if (rr_g2x_resel_to_irat_cell_is_allowed(&cell, gas_id))
    {
      rr_resel_priority_e priority = rr_get_resel_priority_for_uarfcn(
        measurement->data.utran_tdd.uarfcn,
        RR_UTRAN_TDD,
        gas_id
      );

      /* Do not add a cell if its priority is not available */
      if (priority != RR_RESEL_PRIORITY_UNDEFINED)
      {
        rr_irat_priority_list_entry_t * priority_list_entry = &priority_list->entries[priority_list->length];
  
        priority_list_entry->measurement = measurement;
        priority_list_entry->priority = priority;
        priority_list_entry->s_value = measurement->data.utran_tdd.rscp - rr_get_qrxlevmin_for_uarfcn(measurement->data.utran_tdd.uarfcn, RR_UTRAN_TDD, gas_id);
        priority_list_entry->thresh_high = rr_get_thresh_utran_high_for_uarfcn(measurement->data.utran_tdd.uarfcn, RR_UTRAN_TDD, gas_id);
        priority_list_entry->thresh_low = rr_get_thresh_utran_low_for_uarfcn(measurement->data.utran_tdd.uarfcn, RR_UTRAN_TDD, gas_id);
  
        ++priority_list->length;
      }
      else
      {
        MSG_GERAN_HIGH_1_G("Priority not available for UTRAN TDD freq %d", measurement->data.utran_tdd.uarfcn);
      }
    }
  }
#endif /*FEATURE_GSM_TDS*/

  rr_irat_priority_list_entries_sort_geran_priority = rr_geran_priority(gas_id);

  qsort(
    priority_list->entries,
    priority_list->length,
    sizeof(rr_irat_priority_list_entry_t),
    rr_irat_priority_list_entries_sort
  );
}


static void rr_irat_priority_list_dump(rr_irat_priority_list_t * priority_list, const gas_id_t gas_id)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(priority_list);

  MSG_GERAN_HIGH_1_G("Priority List Length: %d", priority_list->length);
  for (i = 0; i < priority_list->length; ++i)
  {
    switch (priority_list->entries[i].measurement->rat)
    {
#ifdef FEATURE_WCDMA
      case RR_RAT_WCDMA:
      {
        MSG_8(MSG_SSID_DFLT,
              MSG_LEGACY_HIGH,
              "[%d] RAT: WCDMA, UARFCN: %d, PSC: %d, S-value: %d, Priority: %d, High Threshold: %d, Low Threshold: %d, Timers: 0x%06x",
              i,
              priority_list->entries[i].measurement->data.wcdma.uarfcn,
              priority_list->entries[i].measurement->data.wcdma.scrambling_code,
              priority_list->entries[i].s_value,
              priority_list->entries[i].priority,
              priority_list->entries[i].thresh_high,
              priority_list->entries[i].thresh_low,
              rr_timer_is_running(priority_list->entries[i].measurement->thresh_high_timer, gas_id) << 16 |
              rr_timer_is_running(priority_list->entries[i].measurement->thresh_low_timer, gas_id)  << 8  |
              rr_timer_is_running(priority_list->entries[i].measurement->h_prio_timer, gas_id));
        break;
      }
#endif /* FEATURE_WCDMA */

      case RR_RAT_LTE:
      {
        MSG_8(MSG_SSID_DFLT,
              MSG_LEGACY_HIGH,
              "[%d] RAT: LTE, E-ARFCN: %d, PCID: %d, S-value: %d, Priority: %d, High Threshold: %d, Low Threshold: %d, Timers: 0x%06x",
              i,
              priority_list->entries[i].measurement->data.lte.earfcn,
              priority_list->entries[i].measurement->data.lte.pcid,
              priority_list->entries[i].s_value,
              priority_list->entries[i].priority,
              priority_list->entries[i].thresh_high,
              priority_list->entries[i].thresh_low,
              rr_timer_is_running(priority_list->entries[i].measurement->thresh_high_timer, gas_id) << 16 |
              rr_timer_is_running(priority_list->entries[i].measurement->thresh_low_timer, gas_id)  << 8  |
              rr_timer_is_running(priority_list->entries[i].measurement->h_prio_timer, gas_id));
        break;
      }

#ifdef FEATURE_GSM_TDS
      case RR_RAT_UTRAN_TDD:
      {
        MSG_8(MSG_SSID_DFLT,
              MSG_LEGACY_HIGH,
              "[%d] RAT: TDSCDMA, UARFCN: %d, CPID: %d, S-value: %d, Priority: %d, High Threshold: %d, Low Threshold: %d, Timers: 0x%06x",
              i,
              priority_list->entries[i].measurement->data.utran_tdd.uarfcn,
              priority_list->entries[i].measurement->data.utran_tdd.cell_parameter_id,
              priority_list->entries[i].s_value,
              priority_list->entries[i].priority,
              priority_list->entries[i].thresh_high,
              priority_list->entries[i].thresh_low,
              rr_timer_is_running(priority_list->entries[i].measurement->thresh_high_timer, gas_id) << 16 |
              rr_timer_is_running(priority_list->entries[i].measurement->thresh_low_timer, gas_id)  << 8  |
              rr_timer_is_running(priority_list->entries[i].measurement->h_prio_timer, gas_id));
        break;
      }
#endif /*FEATURE_GSM_TDS*/

      default:
      {
        MSG_GERAN_ERROR_1_G("Unknown measurement type: %d", priority_list->entries[i].measurement->rat);
      }
    }
  }
}

static void rr_irat_priority_list_log(
  ARFCN_T                   scell_arfcn,
  uint8                     scell_priority,
  uint8                     scell_rxlev,
  int16                     scell_s_value,
  rr_irat_priority_list_t * priority_list,
  const gas_id_t            gas_id
)
{
  gsmdiag_pkt_info_t            pkt_info;
  rr_log_irat_priority_list_t * log_ptr;

  RR_NULL_CHECK_RETURN_VOID(priority_list);

  // TODO DSDA

  rr_log_packet_init(&pkt_info, LOG_GSM_RR_IRAT_PRIORITY_LIST_C, sizeof(rr_log_irat_priority_list_t), gas_id);
  log_ptr = (rr_log_irat_priority_list_t *) gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    uint8 i;

    log_ptr->version = 1;

    log_ptr->scell.arfcn = scell_arfcn.num;
    log_ptr->scell.band  = scell_arfcn.band;
    log_ptr->scell.priority = scell_priority;
    log_ptr->scell.rxlev = scell_rxlev;
    log_ptr->scell.s_value = scell_s_value;

    for (i = 0;
         i < priority_list->length &&
         i < RR_IRAT_PRIORITY_LIST_LENGTH &&
         i < RR_LOG_MAX_IRAT_PRIORITY_LIST_LENGTH;
         ++i)
    {
      rr_log_irat_priority_list_entry_t * entry = &log_ptr->entries[i];

      switch (priority_list->entries[i].measurement->rat)
      {
#ifdef FEATURE_WCDMA
        case RR_RAT_WCDMA:
        {
          entry->rat                        = RR_LOG_IRAT_RAT_WCDMA;
          entry->cell.wcdma.uarfcn          = priority_list->entries[i].measurement->data.wcdma.uarfcn;
          entry->cell.wcdma.scrambling_code = priority_list->entries[i].measurement->data.wcdma.scrambling_code;
          entry->cell.wcdma.diversity       = priority_list->entries[i].measurement->data.wcdma.diversity;
          entry->cell.wcdma.rscp            = priority_list->entries[i].measurement->data.wcdma.rscp;
          entry->cell.wcdma.ecno            = priority_list->entries[i].measurement->data.wcdma.ecno;
          entry->cell.wcdma.rssi            = priority_list->entries[i].measurement->data.wcdma.rssi;
          entry->cell.wcdma.pn_offset       = priority_list->entries[i].measurement->data.wcdma.pn_offset;
          entry->rank_resel_timer_running   = rr_timer_is_running(priority_list->entries[i].measurement->resel_timer, gas_id);
          break;
        } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

        case RR_RAT_LTE:
        {
          entry->rat             = RR_LOG_IRAT_RAT_LTE;
          entry->cell.lte.earfcn = priority_list->entries[i].measurement->data.lte.earfcn;
          entry->cell.lte.pcid   = priority_list->entries[i].measurement->data.lte.pcid;
          entry->cell.lte.rsrp   = priority_list->entries[i].measurement->data.lte.rsrp;
          entry->cell.lte.rsrq   = priority_list->entries[i].measurement->data.lte.rsrq;
          entry->rank_resel_timer_running = FALSE; /* Rank-based reselection is never performed toward LTE */
          break;
        } /* RR_RAT_LTE */

        default:
        {
          MSG_GERAN_ERROR_1_G("Unknown RAT: %d", priority_list->entries[i].measurement->rat);
        }
      }

      entry->thresh_high_timer_running = rr_timer_is_running(priority_list->entries[i].measurement->thresh_high_timer, gas_id);
      entry->thresh_low_timer_running  = rr_timer_is_running(priority_list->entries[i].measurement->thresh_low_timer, gas_id);
      entry->h_prio_timer_running      = rr_timer_is_running(priority_list->entries[i].measurement->h_prio_timer, gas_id);

      entry->s_value = priority_list->entries[i].s_value;
      entry->priority = priority_list->entries[i].priority;
      entry->thresh_high = priority_list->entries[i].thresh_high;
      entry->thresh_low = priority_list->entries[i].thresh_low;
    }
    log_ptr->length = i;

    gsmdiag_log_commit(&pkt_info);
  }
}
#endif /*FEATURE_LTE*/

/*==============================================================================
  Public functions
==============================================================================*/

void rr_g2w_init(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_g2w_data_t *rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  memset(rr_g2w_data_ptr, 0, sizeof(rr_g2w_data_t));
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
/**
 * Cancels all the five second timers for the surrounding wcdma cells.
 */
void rr_g2wr_cancel_all_wcdma_five_second_timers(
  const gas_id_t gas_id
)
{
  /*******************************************************************************************/
  /* do NOT reset the wcdma five second timerId values, they are issued when cells are added */
  /* to DB and should last as long as the cell remains in the database                       */
  /*******************************************************************************************/

  /* cancel all the surrounding timers */
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_1, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_2, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_3, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_4, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_5, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_6, gas_id);

  if (!rr_gsm_only(gas_id))
  {
    MSG_GERAN_HIGH_0_G("All G2W Resel wcdma 5 sec timers stopped");
  }
} /* rr_g2wr_cancel_all_wcdma_five_second_timers */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
/**
 * Called when a five second timer has expired. Verifies that
 * the timer matches a valid entry in the surrounding database.
 *
 * @param five_second_timer_id Id of the wcdma five second timer that has expired
 */
void rr_g2wr_update_wcdma_five_second_timer_state(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  rr_irat_measurements_t *measurements;
  int                     i = 0;
  boolean                 found = FALSE;
  rr_g2w_data_t          *rr_g2w_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  measurements = &rr_g2w_data_ptr->wcdma_measurements;

  for (i = 0; i < measurements->no_of_entries && i < RR_IRAT_MEASUREMENTS_PER_RAT; ++i)
  {
    /* entry should have a valid wcdma five second timer id associated with it */
    rr_irat_measurement_t * measurement = &measurements->entries[i];

    if (measurement->resel_timer == RR_TIMER_UNDEFINED)
    {
      MSG_GERAN_ERROR_0_G("Undefined timer for IRAT neighbour cell at index %d");
    }
    else if (timer_id == measurement->resel_timer)
    {
      if (!found)
      {
        if (measurement->rat == RR_RAT_WCDMA)
        {
          MSG_GERAN_HIGH_2_G("WCDMA cell (UARFCN: %d, PSC: %d): five second timer expired",
            measurement->data.wcdma.uarfcn,
            measurement->data.wcdma.scrambling_code);
        }
        else
        {
          MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", measurement->rat);
        }

        /* timer should be set to expired */
        RR_ASSERT_RETURN_VOID(rr_timer_has_expired(timer_id, gas_id));

        found = TRUE;
      }
      else /* same id was already found */
      {
        MSG_GERAN_ERROR_3_G("Same timerId=%d for cell=%d",
                  timer_id,
                  measurement->data.wcdma.uarfcn,
                  measurement->data.wcdma.scrambling_code
        );
      }
    }
  } /* foreach cell in the surrounding wcdma database */

  /* timer should have been found */
  if (!found)
  {  
#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
    /* timer should have been found */
    MSG_GERAN_ERROR_1_G("TimerId=%d not found in WCDMA surround info db",(int)timer_id);
#else
    #error code not present
#endif /* !PERLUTF && !TEST_FRAMEWORK */

  }

  return;
} /* rr_g2wr_update_wcdma_five_second_timer_state */


/**
  @brief Delete any stored WCDMA measurements.
*/
void rr_g2w_wcdma_measurements_delete(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_g2w_data_t *rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  MSG_GERAN_HIGH_0_G("Deleting stored WCDMA measurements");
  while (rr_g2w_data_ptr->wcdma_measurements.no_of_entries > 0)
  {
    rr_irat_measurements_delete_index(
      &rr_g2w_data_ptr->wcdma_measurements,   // measurements
      0,                                      // i
      gas_id
    );
  }
}

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE

/**
  @brief Delete any stored LTE measurements.
*/
void rr_g2w_lte_measurements_delete(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_g2w_data_t *rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  MSG_GERAN_HIGH_0_G("Deleting stored LTE measurements");
  while (rr_g2w_data_ptr->lte_measurements.no_of_entries > 0)
  {
    rr_irat_measurements_delete_index(
      &rr_g2w_data_ptr->lte_measurements,   // measurements
      0,                                    // i
      gas_id
    );
  }
}

#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS

/**
  @brief Delete any stored UTRAN TDD measurements.
*/
void rr_g2w_utran_tdd_measurements_delete(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_g2w_data_t *rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  MSG_GERAN_HIGH_0_G("Deleting stored UTRAN TDD measurements");
  while (rr_g2w_data_ptr->utran_tdd_measurements.no_of_entries > 0)
  {
    rr_irat_measurements_delete_index(
      &rr_g2w_data_ptr->utran_tdd_measurements,   // measurements
      0,                                          // i
      gas_id
    );
  }
}

#endif /* FEATURE_GSM_TDS */

/**
 * Resets the surrounding wcdma database
 */
void rr_g2wr_reset_wcdma_surrounding_database(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_g2w_data_t *rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

#ifdef FEATURE_WCDMA
  rr_g2w_wcdma_measurements_delete(gas_id);
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
  rr_g2w_lte_measurements_delete(gas_id);
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
  rr_g2w_utran_tdd_measurements_delete(gas_id);
#endif /* FEATURE_GSM_TDS */

  rr_g2w_data_ptr->resel_target.valid = FALSE;
  rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_UNDEFINED;

  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_1, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_2, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_3, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_4, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_5, gas_id);
  (void)rr_timer_cancel(RR_G2WR_FIVE_SEC_TIMER_6, gas_id);
}

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

#ifdef FEATURE_WCDMA
/**
 * Processes the measurements of the 6 best surrounding wcdma cells. Updates the
 * surrounding wcdma  database as appropriate to match the 6
 * best surrounding wcdma cells (adding/deleting entries as appropriate).
 *
 * @param p_best_six_info pointer to measurement report for the six best surrounding wcdma cells
 */
void rr_g2wr_store_wcdma_surrounding_measurement(
  best_six_wcdma_info_T * new_measurements,
  const gas_id_t gas_id
)
{
  rr_irat_measurements_t  *measurements;
  boolean                  channel_present = FALSE;
  boolean                  still_best_six = FALSE;
  uint32                   i;
  rr_g2w_data_t           *rr_g2w_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  measurements = &rr_g2w_data_ptr->wcdma_measurements;

  MSG_GERAN_MED_1_G("Received %d surrounding wcdma measurements", new_measurements->no_of_entries);

  /* 3g Idle mode log Packet */
  (void) rr_log_gprs_3g_reselection_measurements(gas_id);

  /*******************************************************************************/
  /* RR has just received a new measurement report of the best six surrounding   */
  /* cells (based on the BA list sent to L1). Check the surrounding information  */
  /* database to determine if it has entries for cells that are no longer in     */
  /* the set of the best six surrounding cells. Any such cells should be deleted */
  /* from the surrounding wcdma database since they are no longer needed         */
  /*******************************************************************************/

  for (i = measurements->no_of_entries; i > 0; --i)
  {
    rr_irat_measurement_t * measurement = &measurements->entries[i-1];

    /******************************************************/
    /* Determine if the surrounding cell is still in the  */
    /* best six measurement report (also determine if the */
    /* cell is present.                                   */
    /******************************************************/
    still_best_six = rr_g2wr_surrounding_wcdma_cell_in_best_six(
      measurement,
      new_measurements
    );

    if (still_best_six == FALSE)
    {
      // Just check if this is what stored as the best WCDMA cell earlier
      // Then clear the same.
      if ((measurement->rat == RR_RAT_WCDMA && rr_g2w_data_ptr->resel_target.cell.rat == RR_RAT_WCDMA) &&
           measurement->data.wcdma.uarfcn == rr_g2w_data_ptr->resel_target.cell.data.wcdma.uarfcn &&
           measurement->data.wcdma.scrambling_code == rr_g2w_data_ptr->resel_target.cell.data.wcdma.scrambling_code)
      {
        rr_g2w_data_ptr->resel_target.valid = FALSE;
        rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_UNDEFINED;
      }

      MSG_GERAN_MED_2_G("Wcdma Cell(%d): Deleting cell from surrounding Wcdma DB",
              measurement->data.wcdma.uarfcn,
              measurement->data.wcdma.scrambling_code);

      rr_irat_measurements_delete_index(measurements, i-1, gas_id);
    }
  }


  /************************************************************************/
  /* At this point, all the cells in the surrounding information DB that  */
  /* were not in the measurement report have been deleted. Now it is time */
  /* to add new cells in the measurement report that are not already in   */
  /* the database                                                         */
  /************************************************************************/

  /* foreach cell in the measurement report */
  for (i = 0; i < new_measurements->no_of_entries; ++i)
  {
    surrounding_wcdma_channel_info_T * wcdma_meas_chan_info_ptr = &new_measurements->surrounding_wcdma_channel_info[i];

    /* set to true if the channel is present in the surrounding DB */
    /* and it is not the serving cell */
    channel_present = rr_g2wr_find_wcdma_surround_database_entry(
      measurements,
      wcdma_meas_chan_info_ptr->UARFCN,
      wcdma_meas_chan_info_ptr->Scrambling_code
    );

    /* if the measurement report channel is not in the surround DB */
    if (channel_present == FALSE)
    {
      MSG_GERAN_MED_2_G("Wcdma Cell(%d) (%d): Adding cell to wcdma DB",
              wcdma_meas_chan_info_ptr->UARFCN,
              wcdma_meas_chan_info_ptr->Scrambling_code);

      /* add an entry in the surrounding DB for the new cell in the measurement report */
      rr_g2wr_add_wcdma_surrounding_database_entry(
        measurements,
        wcdma_meas_chan_info_ptr->UARFCN,
        wcdma_meas_chan_info_ptr->Scrambling_code,
        FALSE, /* Diversity, not available in legacy interface. */
        wcdma_meas_chan_info_ptr->RSCP,
        wcdma_meas_chan_info_ptr->EcNo,
        wcdma_meas_chan_info_ptr->RSSI,
        wcdma_meas_chan_info_ptr->PN_Offset,
        gas_id
      );
    } /* measurement report channel was not present in surrounding DB */
    else
    {
      // Channel is present, so update the RSCP value, after finding out the cell index.
      byte found_index = rr_irat_measurements_find_wcdma_cell_index(
        measurements,
        wcdma_meas_chan_info_ptr->UARFCN,
        wcdma_meas_chan_info_ptr->Scrambling_code
      );

      if (found_index < RR_IRAT_MEASUREMENTS_PER_RAT)
      {
        measurements->entries[found_index].data.wcdma.pn_offset = wcdma_meas_chan_info_ptr->PN_Offset;
        measurements->entries[found_index].data.wcdma.rscp = wcdma_meas_chan_info_ptr->RSCP;
        measurements->entries[found_index].data.wcdma.ecno = wcdma_meas_chan_info_ptr->EcNo;
        measurements->entries[found_index].data.wcdma.xfer_validity_counter = 0;
      }
      else
      {
        MSG_GERAN_ERROR_1_G("Index out of range: %d", found_index);
      }
    }

  } /* foreach cell in the measurement report */

  /***************************************************************************/
  /* At this point, all the cells in the measurement report should be in the */
  /* surrounding database. Some may have been there previously, while others */
  /* may have been added above                                               */
  /***************************************************************************/

  if (measurements->no_of_entries != new_measurements->no_of_entries)
  {
    /* It is possible for these values to not be equal if for some reason
    L1 has put duplicate entries in the measurement report.  If this does
    happen there is no need to ASSERT as it is non-fatal error. */

    MSG_GERAN_ERROR_2_G("rr_irat_ncells_store.no_of_entries(%d) != new_measurements->no_of_entries(%d)",
      measurements->no_of_entries,
      new_measurements->no_of_entries);
  }

  return;
} /* rr_g2wr_store_wcdma_surrounding_measurement */

/**
 * We require some special processing here.  If more than one UTRAN frequency has
 * been sent to L1, it will send a single mph_surround_wcdma_xfer_meas_ind PER FREQUENCY,
 * consecutively.  There may be up to three frequencies, so we can only delete cells
 * if they are not present in at least three consecutive measurement reports.
 */
void rr_gprs_store_wcdma_xfer_measurements(
  mph_surround_wcdma_xfer_meas_ind_T *xfer_meas_ind,
  const gas_id_t gas_id
)
{
  rr_irat_measurements_t *rr_irat_ncells;
  int                     i;
  int                     no_of_entries;
  rr_g2w_data_t          *rr_g2w_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  rr_irat_ncells = &rr_g2w_data_ptr->wcdma_measurements;

  no_of_entries = rr_irat_ncells->no_of_entries;

  if (no_of_entries > RR_IRAT_MEASUREMENTS_PER_RAT)
  {
    MSG_GERAN_ERROR_2_G("no_of_entries (%d) out of range, truncating to %d",
              no_of_entries,
              RR_IRAT_MEASUREMENTS_PER_RAT);
  }

  /* need to delete any cells in the rr_irat_ncells_store database
  that are not in the current measurement report. */

  for (i = no_of_entries; i > 0; i--)
  {
    if (rr_irat_ncells->entries[i-1].rat == RR_RAT_WCDMA)
    {
      uint16 uarfcn = rr_irat_ncells->entries[i-1].data.wcdma.uarfcn;
      uint16 sc = rr_irat_ncells->entries[i-1].data.wcdma.scrambling_code;

      boolean present = cell_is_in_xfer_meas_ind(uarfcn, sc, xfer_meas_ind);

      if (!present)
      {
        byte position = rr_irat_measurements_find_wcdma_cell_index(
          rr_irat_ncells,
          uarfcn,
          sc
        );

        if (position < RR_IRAT_MEASUREMENTS_PER_RAT)
        {
          rr_irat_ncells->entries[position].data.wcdma.xfer_validity_counter++;

          if (rr_irat_ncells->entries[position].data.wcdma.xfer_validity_counter >=
              rr_get_num_uarfcns_sent_to_l1(gas_id))
          {
            if (rr_g2w_data_ptr->resel_target.valid &&
                rr_g2w_data_ptr->resel_target.cell.rat == RR_RAT_WCDMA &&
                uarfcn == rr_g2w_data_ptr->resel_target.cell.data.wcdma.uarfcn &&
                sc == rr_g2w_data_ptr->resel_target.cell.data.wcdma.scrambling_code)
            {
              rr_g2w_data_ptr->resel_target.valid = FALSE;
            }

            MSG_GERAN_HIGH_2_G("Delete WCDMA cell %d, %d from surround cell database", uarfcn, sc);

            rr_irat_measurements_delete_index(
              rr_irat_ncells,
              position,
              gas_id
            );
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("Index out of range: %d", position);
        }
      }
    }
  }

  for (i = 0; i < xfer_meas_ind->num_best_cells; i++)
  {
    byte position = rr_irat_measurements_find_wcdma_cell_index(
      rr_irat_ncells,
      xfer_meas_ind->wcdma_xfer_meas[i].UARFCN,
      xfer_meas_ind->wcdma_xfer_meas[i].Scrambling_Code
    );

    if (position == RR_IRAT_MEASUREMENTS_PER_RAT)
    {
      /* cell is not present in the database, need to add it if there is space */
      if (rr_irat_ncells->no_of_entries < RR_IRAT_MEASUREMENTS_PER_RAT)
      {
        rr_g2wr_add_wcdma_surrounding_database_entry(
          rr_irat_ncells,
          xfer_meas_ind->wcdma_xfer_meas[i].UARFCN,
          xfer_meas_ind->wcdma_xfer_meas[i].Scrambling_Code,
          FALSE,
          xfer_meas_ind->wcdma_xfer_meas[i].RSCP,
          xfer_meas_ind->wcdma_xfer_meas[i].Ec_Nox2,
          xfer_meas_ind->wcdma_xfer_meas[i].RSSI,
          xfer_meas_ind->wcdma_xfer_meas[i].PN_Offset,
          gas_id
        );
      }
      else
      {
        /* the WCDMA database is full.  Replace the lowest RSCP cell with the current
        one, if it is better. */
        byte worst_cell_index = rr_g2wr_find_worst_wcdma_surround_database_entry(
          rr_irat_ncells
        );

        if (worst_cell_index == RR_IRAT_MEASUREMENTS_PER_RAT)
        {
          MSG_GERAN_ERROR_0_G("Unable to find weakest cell in database, abort");
          break;
        }
        else if (worst_cell_index < RR_IRAT_MEASUREMENTS_PER_RAT)
        {
          if (xfer_meas_ind->wcdma_xfer_meas[i].RSCP > rr_irat_ncells->entries[worst_cell_index].data.wcdma.rscp)
          {
            rr_irat_measurements_delete_index(
              rr_irat_ncells,
              worst_cell_index,
              gas_id
            );

            rr_g2wr_add_wcdma_surrounding_database_entry(
              rr_irat_ncells,
              xfer_meas_ind->wcdma_xfer_meas[i].UARFCN,
              xfer_meas_ind->wcdma_xfer_meas[i].Scrambling_Code,
              FALSE, /* Diversity, not known in legacy interface. */
              xfer_meas_ind->wcdma_xfer_meas[i].RSCP,
              xfer_meas_ind->wcdma_xfer_meas[i].Ec_Nox2,
              xfer_meas_ind->wcdma_xfer_meas[i].RSSI,
              xfer_meas_ind->wcdma_xfer_meas[i].PN_Offset,
              gas_id
            );
          }
        }
        else
        {
          MSG_GERAN_ERROR_1_G("worst_cell_index %d out of range", worst_cell_index);
        }
      }
    }
    else
    {
      /* cell was found, just update it's RSCP to the new value */
      rr_irat_measurement_t *cell = &(rr_irat_ncells->entries[position]);

      MSG_GERAN_MED_3_G("New RSCP: %d for cell %d,%d",
        xfer_meas_ind->wcdma_xfer_meas[i].RSCP,
        xfer_meas_ind->wcdma_xfer_meas[i].UARFCN,
        xfer_meas_ind->wcdma_xfer_meas[i].Scrambling_Code);

      cell->data.wcdma.rscp = xfer_meas_ind->wcdma_xfer_meas[i].RSCP;
      cell->data.wcdma.ecno = xfer_meas_ind->wcdma_xfer_meas[i].Ec_Nox2;
      cell->data.wcdma.xfer_validity_counter = 0;
    }
  }

  (void) rr_log_gprs_3g_reselection_measurements(gas_id);
}
#endif /* FEATURE_WCDMA */

void rr_irat_process_surround_irat_meas_ind(
  mph_surround_irat_meas_ind_t * surround_irat_meas_ind,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(surround_irat_meas_ind);

  rr_log_irat_measurements(surround_irat_meas_ind, gas_id);

  switch (surround_irat_meas_ind->rat)
  {
    case RR_L1_IRAT_RAT_UNDEFINED:
    {
      break;
    } /* RR_L1_IRAT_RAT_UNDEFINED */

#ifdef FEATURE_WCDMA
    case RR_L1_IRAT_RAT_WCDMA:
    {
      if (rr_is_wcdma_enabled(gas_id))
      {
        rr_irat_store_wcdma_measurements(&surround_irat_meas_ind->measurements.wcdma, gas_id);
        rr_irat_dump_wcdma_measurements(gas_id);
      }
      else
      {
        MSG_GERAN_HIGH_0_G("Ignoring WCDMA measurements received when WCDMA not enabled");
      }
      break;
    } /* RR_L1_IRAT_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
    case RR_L1_IRAT_RAT_LTE:
    {
      if (rr_is_lte_enabled(gas_id))
      {
        rr_irat_store_lte_measurements(&surround_irat_meas_ind->measurements.lte, gas_id);
        rr_irat_dump_lte_measurements(gas_id);
      }
      else
      {
        MSG_GERAN_HIGH_0_G("Ignoring LTE measurements received when LTE not enabled");
      }
      break;
    } /* RR_L1_IRAT_RAT_LTE */
#endif /*FEATURE_LTE*/

#ifdef FEATURE_GSM_TDS
    case RR_L1_IRAT_RAT_UTRAN_TDD:
    {
      if (rr_is_utran_tdd_enabled(gas_id))
      {
        rr_irat_store_utran_tdd_measurements(&surround_irat_meas_ind->measurements.utran_tdd, gas_id);
        rr_irat_dump_utran_tdd_measurements(gas_id);
      }
      else
      {
        MSG_GERAN_HIGH_0_G("Ignoring UTRAN TDD measurements received when UTRAN TDD not enabled");
      }
      break;
    }
#endif /*FEATURE_GSM_TDS*/

    default:
    {
      MSG_GERAN_ERROR_1_G("Unknown RAT: %d", surround_irat_meas_ind->rat);
    }
  }
}

boolean rr_irat_check_for_rank_based_reselection(
  boolean recent_reselection_flag,
  boolean immediate_reselection,
  boolean ignore_serving_cell_rxlev,
  boolean inhibit_utran_reselection,
  boolean is_c2_based_resel,
  const gas_id_t gas_id
)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  byte                       count;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
#ifdef FEATURE_WCDMA
  byte                       no_of_neighbor_wcdma_cells = 0;
  boolean                    Candidate_wcdma_cell_present = FALSE;
  rr_irat_measurements_t    *rr_wcdma_ncells;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  rr_irat_measurements_t    *rr_utran_tdd_ncells;
  byte                       no_of_neighbor_utran_tdd_cells = 0;
  boolean                    Candidate_utran_tdd_cell_present = FALSE;
#endif /*FEATURE_GSM_TDS*/
  rr_g2w_data_t             *rr_g2w_data_ptr;
  boolean                    allowed_rat_list[RR_RAT_MAX] = {0};
  /* assume that no reselection is required until proven otherwise */
  boolean                    resel_triggered = FALSE;
  boolean                    resel_possible;
  specific_update_freq_T     specific_irat_freq_update_list[RR_IRAT_MEASUREMENTS_PER_RAT];

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  memset(specific_irat_freq_update_list,0xFF,sizeof(specific_irat_freq_update_list));

#ifdef FEATURE_WCDMA
  rr_wcdma_ncells = &rr_g2w_data_ptr->wcdma_measurements;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  rr_utran_tdd_ncells = &rr_g2w_data_ptr->utran_tdd_measurements;
#endif /*FEATURE_GSM_TDS*/

  if (inhibit_utran_reselection)
  {
    MSG_GERAN_MED_0_G("UTRAN reselection has been inhibited");
    return FALSE;
  }

  if (rr_gsm_only(gas_id))
  {
    return FALSE;
  }
  
#ifdef FEATURE_SGLTE

  if((rr_is_sglte_mode_active(gas_id)) && (TRUE == immediate_reselection ))
  {
    MSG_GERAN_MED_0_G("SGLTE active,Forced resel TRUE: GSM should first find service");
    return FALSE;	
  }
#endif /*FEATURE_SGLTE*/

  /* build the list of "allowed" flags, one per RAT */
  resel_possible = rr_ms_g2x_build_allowed_list(allowed_rat_list, (int)RR_RAT_MAX, gas_id);

#ifdef FEATURE_WCDMA
  no_of_neighbor_wcdma_cells = rr_wcdma_ncells->no_of_entries;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
  no_of_neighbor_utran_tdd_cells = rr_utran_tdd_ncells->no_of_entries;

  /* UTRAN TDD and FDD are not supported at the same time.  MS has not been tested to perform
     TDD and FDD measurements, also no T->W interrat testing has been performed.  It is expected
     that TDD and FDD will not co-exist in the same NW so this capability is currently not required.*/
  if ( (no_of_neighbor_utran_tdd_cells > 0) &&
       (no_of_neighbor_wcdma_cells > 0) )
  {
    MSG_GERAN_ERROR_2_G("RR Unexpected UTRAN TDD and FDD measurements present, FDD cells:%d, TDD cells:%d",
               no_of_neighbor_wcdma_cells,
               no_of_neighbor_utran_tdd_cells);
    return FALSE;
  }
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
  if (allowed_rat_list[RR_RAT_WCDMA])
  {
    MSG_GERAN_LOW_1_G("Checking %d WCDMA cells", no_of_neighbor_wcdma_cells);
  }
  else
  {
    no_of_neighbor_wcdma_cells = 0;
    MSG_GERAN_HIGH_0_G("G2W reselection blocked due to opposite stack activity");
  }

  for (count = 0; count < no_of_neighbor_wcdma_cells; ++count)
  {
    rr_irat_measurement_t *wcdma_info_ptr = &rr_wcdma_ncells->entries[count];

    if (wcdma_info_ptr->rat == RR_RAT_WCDMA)
    {
      MSG_GERAN_MED_3_G("Cell: %d, RSCP: %d, Ec/No: %d",
              count,
              wcdma_info_ptr->data.wcdma.rscp,
              wcdma_info_ptr->data.wcdma.ecno / 2);

#ifdef FEATURE_DUAL_SIM
      if (rrc_rr_check_cell_barred_sub_id(wcdma_info_ptr->data.wcdma.uarfcn,
                                          wcdma_info_ptr->data.wcdma.scrambling_code,
                                          geran_map_gas_id_to_nas_id(gas_id)) != WCDMA_NOT_BARRED)
#else
      if (rrc_rr_check_cell_barred(wcdma_info_ptr->data.wcdma.uarfcn,
                                   wcdma_info_ptr->data.wcdma.scrambling_code) != WCDMA_NOT_BARRED)
#endif /* FEATURE_DUAL_SIM */
      {
        /* wcdma cell or frequency is barred */
        MSG_GERAN_HIGH_2_G("WCDMA cell barred,  UARFCN = %d, PSC = %d", wcdma_info_ptr->data.wcdma.uarfcn, wcdma_info_ptr->data.wcdma.scrambling_code);
        continue;
      }

      if (!rr_g2x_resel_to_wcdma_frequency_is_allowed(
             wcdma_info_ptr->data.wcdma.uarfcn,
             rr_band_pref_wcdma(gas_id),
             TRUE,
             rr_is_any_cell_selection_mode(gas_id),
             gas_id
           ))
      {
        MSG_GERAN_MED_1_G("WCDMA UARFCN %d in Forbidden LAI / non-equivalent PLMN, not reselecting",
                wcdma_info_ptr->data.wcdma.uarfcn);
        continue;
      }

      /*************************************************************************/
      /* 45.008 rules for g2w reselection                                      */
      /* 1) its measured RSCP value exceeds the value of RLA_C for the serving */
      /*    cell and all of the suitable (see 3GPP TS 43.022) non-serving GSM  */
      /*    cells by the value XXX_Qoffset                                     */
      /* 2) its measured Ec/No value is equal or greater than the value        */
      /*    FDD_Qmin - FDD_Qmin_Offset                                         */
      /* 3) its measured RSCP value is equal to or greater than                */
      /*    FDD_RSCP_threshold                                                 */
      /*************************************************************************/
      if (rr_wcdma_cell_rscp_is_better(
            wcdma_info_ptr,
            ignore_serving_cell_rxlev,
            recent_reselection_flag,
            gas_id
          ) &&
          rr_gprs_wcdma_cell_ecno_is_suitable(wcdma_info_ptr, gas_id) &&
          rr_wcdma_cell_rscp_is_suitable(wcdma_info_ptr, gas_id))
      {
        boolean surrounding_wcdma_better = TRUE;

        /**********************************************************************************/
        /* check if this particular WCDMA cell is present in the Rejected wcdma cell db   */
        /* no need to check in the rejected threshold db, since once it is in the best    */
        /* Six reported by L1, it will be automatically removed from that db.             */
        /**********************************************************************************/
        rr_irat_cell_t cell;
        cell.rat = RR_RAT_WCDMA;
        cell.data.wcdma.uarfcn = wcdma_info_ptr->data.wcdma.uarfcn;
        cell.data.wcdma.scrambling_code = wcdma_info_ptr->data.wcdma.scrambling_code;
        cell.data.wcdma.diversity = wcdma_info_ptr->data.wcdma.diversity;

        if (rr_g2x_resel_to_irat_cell_is_allowed(&cell, gas_id) == FALSE)
        {
          MSG_GERAN_MED_2_G("Reselection to WCDMA Cell (%d,%d) not currently allowed",
                  wcdma_info_ptr->data.wcdma.uarfcn,
                  wcdma_info_ptr->data.wcdma.scrambling_code);
          surrounding_wcdma_better = FALSE;
        }

        if (surrounding_wcdma_better)
        {
          rr_timer_state_T timer_state;

          MSG_GERAN_HIGH_2_G("WCDMA Cell (%d,%d) is better",
                   wcdma_info_ptr->data.wcdma.uarfcn,
                   wcdma_info_ptr->data.wcdma.scrambling_code);

          /* Check if it's 5 Sec timer expired, else start it. */
          timer_state = rr_timer_get_state(wcdma_info_ptr->resel_timer, gas_id);

          /* if the surrounding cell has been better than the serving cell for >5 seconds */
          if (timer_state == RR_TIMER_EXPIRED)
          {
            /*****************************************************************************/
            /* indicates that the surrounding cell was also better than the serving cell */
            /* the last time C2 values were checked. Since it is still better, it has    */
            /* been better than the serving cell for at least 5 seconds meeting the      */
            /* criteria for reselection                                                  */
            /*****************************************************************************/
            MSG_GERAN_HIGH_0_G("And has been for 5 seconds");

            /*
            make this cell the candidate cell, but only if it is better than an existing candidate.
            */
            if (!Candidate_wcdma_cell_present)
            {
              rr_g2w_data_ptr->resel_target.valid = TRUE;
              rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_WCDMA;
              rr_g2w_data_ptr->resel_target.cell.data.wcdma.uarfcn = wcdma_info_ptr->data.wcdma.uarfcn;
              rr_g2w_data_ptr->resel_target.cell.data.wcdma.diversity = wcdma_info_ptr->data.wcdma.diversity;
              rr_g2w_data_ptr->resel_target.cell.data.wcdma.scrambling_code = wcdma_info_ptr->data.wcdma.scrambling_code;
              rr_g2w_data_ptr->resel_target.cell.data.wcdma.pn_offset = wcdma_info_ptr->data.wcdma.pn_offset;
              rr_g2w_data_ptr->resel_target.wcdma_rscp = wcdma_info_ptr->data.wcdma.rscp;
              Candidate_wcdma_cell_present = TRUE;
            }
            else if (wcdma_info_ptr->data.wcdma.rscp > rr_g2w_data_ptr->resel_target.wcdma_rscp)
            {
              rr_g2w_data_ptr->resel_target.cell.data.wcdma.uarfcn = wcdma_info_ptr->data.wcdma.uarfcn;
              rr_g2w_data_ptr->resel_target.cell.data.wcdma.scrambling_code = wcdma_info_ptr->data.wcdma.scrambling_code;
              rr_g2w_data_ptr->resel_target.cell.data.wcdma.diversity = wcdma_info_ptr->data.wcdma.diversity;
              rr_g2w_data_ptr->resel_target.cell.data.wcdma.pn_offset = wcdma_info_ptr->data.wcdma.pn_offset;
              rr_g2w_data_ptr->resel_target.wcdma_rscp = wcdma_info_ptr->data.wcdma.rscp;
            }

            rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_GSM_TO_WCDMA, gas_id);

            resel_triggered = TRUE;
          }
          else if (timer_state == RR_TIMER_STOPPED)
          {
            /*************************************************************/
            /* Surrounding cell has just now become better than serving  */
            /* start its five second timer to reselect to it if it stays */
            /* better for more than five seconds                         */
            /*************************************************************/
            (void)rr_timer_start(wcdma_info_ptr->resel_timer, RR_FIVE_SEC_TIMEOUT, gas_id);
            rr_send_mph_specific_irat_freq_update_req(
              rr_irat_get_arfcn_for_meas_entry(wcdma_info_ptr,gas_id),
              wcdma_info_ptr->rat,
              RR_FIVE_SEC_TIMEOUT,
              specific_irat_freq_update_list,
              RR_IRAT_MEASUREMENTS_PER_RAT,
              gas_id);
          }

          /* Update target cell for immediate reselection case.
           * Do not update if a candidate cell exists (i.e. if a cell already satisfied for 5 seconds) */
          if((immediate_reselection || is_c2_based_resel) && 
             (!Candidate_wcdma_cell_present) &&
             (rr_g2w_data_ptr->resel_target.valid == FALSE ||
              wcdma_info_ptr->data.wcdma.rscp > rr_g2w_data_ptr->resel_target.wcdma_rscp))
          {
            rr_g2w_data_ptr->resel_target.valid = TRUE;
            rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_WCDMA;
            rr_g2w_data_ptr->resel_target.cell.data.wcdma.uarfcn = wcdma_info_ptr->data.wcdma.uarfcn;
            rr_g2w_data_ptr->resel_target.cell.data.wcdma.diversity = wcdma_info_ptr->data.wcdma.diversity;
            rr_g2w_data_ptr->resel_target.cell.data.wcdma.scrambling_code = wcdma_info_ptr->data.wcdma.scrambling_code;
            rr_g2w_data_ptr->resel_target.wcdma_rscp = wcdma_info_ptr->data.wcdma.rscp;
          }
        }
        else
        {
          MSG_GERAN_MED_2_G("UARFCN:(%d), SC(%d)", (int) wcdma_info_ptr->data.wcdma.uarfcn,(int) wcdma_info_ptr->data.wcdma.scrambling_code);
          // Stop the Five Sec timer, it's ok to stop it even it's not running.
          rr_g2wr_stop_wcdma_five_second_timer(wcdma_info_ptr, gas_id);
        }
      }
      else
      {
        MSG_GERAN_MED_2_G("WCDMA Cell (%d,%d) !better than SCell",
                wcdma_info_ptr->data.wcdma.uarfcn,
                wcdma_info_ptr->data.wcdma.scrambling_code);

        // Stop the Five Sec timer, it's ok to stop it even if it's not running.
        rr_g2wr_stop_wcdma_five_second_timer(wcdma_info_ptr, gas_id);
      }
    }
  }
#endif /* FEATURE_WCDMA */

  memset(specific_irat_freq_update_list,0xFF,sizeof(specific_irat_freq_update_list));

#ifdef FEATURE_GSM_TDS
  if (allowed_rat_list[RR_RAT_UTRAN_TDD])
  {
    MSG_GERAN_LOW_1_G("Checking %d UTRAN TDD cells", no_of_neighbor_utran_tdd_cells);
  }
  else
  {
    no_of_neighbor_utran_tdd_cells = 0;
    MSG_GERAN_HIGH_0_G("G2T reselection blocked due to opposite stack activity");
  }

  /*Checking UTRAN TDD Cells*/
  for (count = 0; count < no_of_neighbor_utran_tdd_cells; ++count)
  {
    rr_irat_measurement_t *utran_tdd_info_ptr = &rr_utran_tdd_ncells->entries[count];

    if (utran_tdd_info_ptr->rat == RR_RAT_UTRAN_TDD)
    {
      MSG_GERAN_MED_3_G("Cell: %d, CPID: %d, RSCP: %d",
              count,
              utran_tdd_info_ptr->data.utran_tdd.cell_parameter_id,
              utran_tdd_info_ptr->data.utran_tdd.rscp);

#ifdef FEATURE_TDS_SUB_ID_INTERFACE
      if (tdsrrc_check_cell_barred_sub_id(
            utran_tdd_info_ptr->data.utran_tdd.uarfcn,
            (uint8)utran_tdd_info_ptr->data.utran_tdd.cell_parameter_id) != TDSRRC_NOT_BARRED,
            RR_GAS_ID_TO_AS_ID)
#else
      if (tdsrrc_check_cell_barred(
            utran_tdd_info_ptr->data.utran_tdd.uarfcn,
            (uint8)utran_tdd_info_ptr->data.utran_tdd.cell_parameter_id) != TDSRRC_NOT_BARRED)
#endif /* FEATURE_TDS_SUB_ID_INTERFACE */
      {
        /* UTRAN TDD cell or frequency is barred */
        MSG_GERAN_HIGH_2_G("UTRAN TDD cell barred,  UARFCN = %d, CPID = %d",
                 utran_tdd_info_ptr->data.utran_tdd.uarfcn,
                 utran_tdd_info_ptr->data.utran_tdd.cell_parameter_id);
        continue;
      }

      if (!rr_g2x_resel_to_utran_tdd_frequency_is_allowed(
             utran_tdd_info_ptr->data.utran_tdd.uarfcn,
             rr_band_pref_utran_tdd(gas_id),
             TRUE,
             rr_is_any_cell_selection_mode(gas_id),
             gas_id
           ))
      {
        MSG_GERAN_MED_1_G("TDD UARFCN %d in Forbidden LAI / non-equivalent PLMN, not reselecting",
                utran_tdd_info_ptr->data.utran_tdd.uarfcn);
        continue;
      }

      /*************************************************************************/
      /* 45.008 rules for G to T reselection                                   */
      /* 1) TDD measured RSCP value is >= TDD_Qoffset for period of 5 seconds  */
      /*************************************************************************/
      if (rr_utran_tdd_cell_rscp_is_better(
            utran_tdd_info_ptr,
            recent_reselection_flag,
            immediate_reselection,
            gas_id
          )
         )
      {
        boolean surrounding_utran_tdd_better = TRUE;

        /****************************************************************************************/
        /* check if this particular UTRAN TDD cell is present in the Rejected utran tdd cell db */
        /* no need to check in the rejected threshold db, since once it is in the best          */
        /* Six reported by L1, it will be automatically removed from that db.                   */
        /****************************************************************************************/
        rr_irat_cell_t cell;
        cell.rat = RR_RAT_UTRAN_TDD;
        cell.data.utran_tdd.uarfcn = utran_tdd_info_ptr->data.utran_tdd.uarfcn;
        cell.data.utran_tdd.cpid = utran_tdd_info_ptr->data.utran_tdd.cell_parameter_id;
        cell.data.utran_tdd.diversity = utran_tdd_info_ptr->data.utran_tdd.diversity;

        if (rr_g2x_resel_to_irat_cell_is_allowed(&cell, gas_id) == FALSE)
        {
          MSG_GERAN_MED_2_G("Reselection to UTRAN TDD Cell (%d,%d) not currently allowed",
                  utran_tdd_info_ptr->data.utran_tdd.uarfcn,
                  utran_tdd_info_ptr->data.utran_tdd.cell_parameter_id);
          surrounding_utran_tdd_better = FALSE;
        }

        if (surrounding_utran_tdd_better)
        {
          rr_timer_state_T timer_state;

          MSG_GERAN_HIGH_2_G("UTRAN TDD Cell (%d,%d) is better",
                   utran_tdd_info_ptr->data.utran_tdd.uarfcn,
                   utran_tdd_info_ptr->data.utran_tdd.cell_parameter_id);

          /* Check if it's 5 Sec timer expired, else start it. */
          timer_state = rr_timer_get_state(utran_tdd_info_ptr->resel_timer, gas_id);

          /* if the surrounding cell has been better than the serving cell for >5 seconds */
          if (timer_state == RR_TIMER_EXPIRED)
          {
            /*****************************************************************************/
            /* indicates that the surrounding cell was also better than the serving cell */
            /* the last time C2 values were checked. Since it is still better, it has    */
            /* been better than the serving cell for at least 5 seconds meeting the      */
            /* criteria for reselection                                                  */
            /*****************************************************************************/
            MSG_GERAN_HIGH_0_G("And has been for 5 seconds");

            /*
            make this cell the candidate cell, but only if it is better than an existing candidate.
            */
            if (!Candidate_utran_tdd_cell_present)
            {
              rr_g2w_data_ptr->resel_target.valid = TRUE;
              rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_UTRAN_TDD;
              rr_g2w_data_ptr->resel_target.cell.data.utran_tdd.uarfcn = utran_tdd_info_ptr->data.utran_tdd.uarfcn;
              rr_g2w_data_ptr->resel_target.cell.data.utran_tdd.diversity = utran_tdd_info_ptr->data.utran_tdd.diversity;
              rr_g2w_data_ptr->resel_target.cell.data.utran_tdd.cpid = utran_tdd_info_ptr->data.utran_tdd.cell_parameter_id;
              rr_g2w_data_ptr->resel_target.utran_tdd_rscp = utran_tdd_info_ptr->data.utran_tdd.rscp;
              Candidate_utran_tdd_cell_present = TRUE;
            }
            else if (utran_tdd_info_ptr->data.utran_tdd.rscp > rr_g2w_data_ptr->resel_target.utran_tdd_rscp)
            {
              rr_g2w_data_ptr->resel_target.cell.data.utran_tdd.uarfcn = utran_tdd_info_ptr->data.utran_tdd.uarfcn;
              rr_g2w_data_ptr->resel_target.cell.data.utran_tdd.diversity = utran_tdd_info_ptr->data.utran_tdd.diversity;
              rr_g2w_data_ptr->resel_target.cell.data.utran_tdd.cpid = utran_tdd_info_ptr->data.utran_tdd.cell_parameter_id;
              rr_g2w_data_ptr->resel_target.utran_tdd_rscp = utran_tdd_info_ptr->data.utran_tdd.rscp;
            }

            if (resel_possible)
            {
              resel_triggered = TRUE;

            }
            else
            {
              MSG_GERAN_HIGH_1_G("Reselection to RAT Blocked due to both Subs in Voice Call", rr_g2w_data_ptr->resel_target.cell.rat );
              rr_g2w_data_ptr->resel_target.valid = FALSE;   
              rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_UNDEFINED;
              (void)rr_timer_start(utran_tdd_info_ptr->resel_timer, RR_FIVE_SEC_TIMEOUT, gas_id);
              resel_triggered = FALSE;
            }
          }
          else if (timer_state == RR_TIMER_STOPPED)
          {
            /******************************************************************/
            /* Surrounding cell has just now become better than the threshold */
            /* start its five second timer to reselect to it if it stays      */
            /* better for more than five seconds                              */
            /******************************************************************/
            rr_timer_start(utran_tdd_info_ptr->resel_timer, RR_FIVE_SEC_TIMEOUT, gas_id);
            rr_send_mph_specific_irat_freq_update_req(
              rr_irat_get_arfcn_for_meas_entry(utran_tdd_info_ptr,gas_id),
              utran_tdd_info_ptr->rat,
              RR_FIVE_SEC_TIMEOUT,
              specific_irat_freq_update_list,
              RR_IRAT_MEASUREMENTS_PER_RAT,
              gas_id);
          }

          /* Update target cell for immediate reselection case.
           * Do not update if a candidate cell exists (i.e. if a cell already satisfied for 5 seconds) */
          if((immediate_reselection || is_c2_based_resel) &&
             (!Candidate_utran_tdd_cell_present) &&
             (rr_g2w_data_ptr->resel_target.valid == FALSE ||
              utran_tdd_info_ptr->data.utran_tdd.rscp > rr_g2w_data_ptr->resel_target.utran_tdd_rscp))
          {
            rr_g2w_data_ptr->resel_target.valid = TRUE;
            rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_UTRAN_TDD;
            rr_g2w_data_ptr->resel_target.cell.data.utran_tdd.uarfcn = utran_tdd_info_ptr->data.utran_tdd.uarfcn;
            rr_g2w_data_ptr->resel_target.cell.data.utran_tdd.diversity = utran_tdd_info_ptr->data.utran_tdd.diversity;
            rr_g2w_data_ptr->resel_target.cell.data.utran_tdd.cpid = utran_tdd_info_ptr->data.utran_tdd.cell_parameter_id;
            rr_g2w_data_ptr->resel_target.utran_tdd_rscp = utran_tdd_info_ptr->data.utran_tdd.rscp;
          }
       }
        else
        {
          MSG_GERAN_MED_2_G("TDD UARFCN:(%d), CPID(%d)", (int) utran_tdd_info_ptr->data.utran_tdd.uarfcn,(int) utran_tdd_info_ptr->data.utran_tdd.cell_parameter_id);
          // Stop the Five Sec timer, it's ok to stop it even it's not running.
          rr_g2wr_stop_wcdma_five_second_timer(utran_tdd_info_ptr, gas_id);
        }
      }
      else
      {
        MSG_GERAN_MED_2_G("UTRAN TDD Cell (%d,%d) NOT better than TDD_QOffset",
                utran_tdd_info_ptr->data.utran_tdd.uarfcn,
                utran_tdd_info_ptr->data.utran_tdd.cell_parameter_id);

        // Stop the Five Sec timer, it's ok to stop it even if it's not running.
        rr_g2wr_stop_wcdma_five_second_timer(utran_tdd_info_ptr, gas_id);
      }
    }
  }
#endif /*FEATURE_GSM_TDS*/

  if (immediate_reselection)
  {
    if (rr_g2w_data_ptr->resel_target.valid)
    {
      MSG_GERAN_HIGH_0_G("FORCE UTRAN RESEL immediate resel");
      resel_triggered = TRUE;
    }
  }

  /* when C2/C32 based reselection starts, and there is TDS cell's RSCP > TDD_Qoffset
     perform G2T reselection instead of C2/C32 reselection */
  if (is_c2_based_resel)
  {
    if (rr_g2w_data_ptr->resel_target.valid)
    {
      MSG_GERAN_HIGH_1_G("Found GSM cell for C2 resel and RAT:%d meets criteria, perform IRAT resel", rr_g2w_data_ptr->resel_target.cell.rat);

      resel_triggered = TRUE;
    }
  }

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
  /* RR is not allowed to trigger a reselection back to WCDMA while the
  redirection timer is running. */
  if (rr_timer_is_running(RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER, gas_id))
  {
    if (resel_triggered)
    {
      MSG_GERAN_HIGH_0_G("Not allowing G2W reselection, redirection wait timer is running");
      resel_triggered = FALSE;
    }
  }
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

  /* Check if a RR_DEACT_REQ is queued for processing - if so, cancel the G2W reselection */
  if (rr_g2w_data_ptr->resel_target.valid)
  {
    if (grr_verify_saved_mm_message(RR_DEACT_REQ, gas_id) ||
        grr_verify_saved_mm_message(RR_STOP_GSM_MODE_REQ, gas_id))
    {
      MSG_GERAN_HIGH_0_G("Not allowing G2W reselection, RR deact/stop pending");
      resel_triggered = FALSE;
    }
  }

  if ( resel_triggered && rr_g2w_data_ptr->prev_resel_target.valid )
  {
    rr_g2w_data_ptr->prev_resel_target.valid = FALSE;
  }
  
  return resel_triggered;
}

#ifdef FEATURE_LTE

static boolean rr_irat_priority_list_entry_suitable(
  rr_irat_priority_list_entry_t * entry,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_PARAM(entry, FALSE);

  switch (entry->measurement->rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      if (rr_gprs_wcdma_cell_ecno_is_suitable(entry->measurement, gas_id))
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
#endif /* FEATURE_WCDMA */

    case RR_RAT_LTE:
    {
      if (rr_is_any_cell_selection_mode(gas_id))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    }

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      return TRUE;
    }
#endif /*FEATURE_GSM_TDS*/

    default:
    {
      MSG_GERAN_ERROR_1_G("Unknown measurement type: %d", entry->measurement->rat);
      return FALSE;
    }
  }
}

/**
  @detail
  This function receives the Reselection Candidate Entry as input and sets the Reselection Target in the Resel G2W DB.

  @param  entry: which is the candidate cell which has met the reselection criterion
         

  @return TRUE if Reselection Target is set FALSE otherwise.
**/

static boolean rr_set_reselection_target(rr_irat_priority_list_entry_t *entry, const gas_id_t gas_id )
{

  rr_g2w_data_t                      *rr_g2w_data_ptr;
  
  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);

  RR_NULL_CHECK_RETURN_PARAM(rr_g2w_data_ptr, 0);

  rr_g2w_data_ptr->resel_target.valid = FALSE;

  switch (entry->measurement->rat)
  {
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      rr_g2w_data_ptr->resel_target.valid = TRUE;
      rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_WCDMA;
      rr_g2w_data_ptr->resel_target.cell.data.wcdma.uarfcn = entry->measurement->data.wcdma.uarfcn;
      rr_g2w_data_ptr->resel_target.cell.data.wcdma.scrambling_code = entry->measurement->data.wcdma.scrambling_code;
      rr_g2w_data_ptr->resel_target.cell.data.wcdma.pn_offset = entry->measurement->data.wcdma.pn_offset;
      rr_g2w_data_ptr->resel_target.cell.data.wcdma.diversity = entry->measurement->data.wcdma.diversity;
      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

    case RR_RAT_LTE:
    {
      rr_g2w_data_ptr->resel_target.valid = TRUE;
      rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_LTE;
      rr_g2w_data_ptr->resel_target.cell.data.lte.earfcn = entry->measurement->data.lte.earfcn;
      rr_g2w_data_ptr->resel_target.cell.data.lte.pcid = entry->measurement->data.lte.pcid;
      break;
    } /* RR_RAT_LTE */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      rr_g2w_data_ptr->resel_target.valid = TRUE;
      rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_UTRAN_TDD;
      rr_g2w_data_ptr->resel_target.cell.data.utran_tdd.uarfcn = entry->measurement->data.utran_tdd.uarfcn;
      rr_g2w_data_ptr->resel_target.cell.data.utran_tdd.cpid = entry->measurement->data.utran_tdd.cell_parameter_id;
      rr_g2w_data_ptr->resel_target.cell.data.utran_tdd.diversity = entry->measurement->data.utran_tdd.diversity;
      break;
    } /* FEATURE_GSM_TDS */
#endif /*FEATURE_GSM_TDS*/
                  
   default:
   {
     MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", entry->measurement->rat);
   }
 }

  return ( rr_g2w_data_ptr->resel_target.valid);
}

/**
  @detail

  Cell reselection to a cell of another inter-RAT frequency shall be performed
  if any of the conditions below (to be evaluated in the order shown) is
  satisfied:

  - The S_non-serving_XXX of one or more cells of a higher priority inter-RAT
    frequency is greater than THRESH_XXX_high during a time interval
    T_reselection; in that case, the mobile station shall consider the cells
    for reselection in decreasing order of priority and, for cells of the same
    inter-RAT frequency, in decreasing order of S_non-serving_XXX, and reselect
    the first cell that satisfies the conditions above;

  - The value of S_GSM is lower than THRESH_GSM_low for the serving cell and
    all measured GSM cells during a time interval T_reselection; in this case,
    the mobile station shall consider for reselection the inter-RAT cells in
    the following order, and reselect the first one that satisfies the
    following criteria:

    - cells of a lower priority inter-RAT frequency whose S_non-serving_XXX is
      greater than THRESH_XXX_low during a time interval T_reselection; these
      cells shall be considered in decreasing order of priority and, for cells
      of the same RAT, in decreasing order of S_non-serving_XXX;

    - if no cells satisfy the criterion above, inter-RAT cells for which,
      during a time interval T_reselection, S_non-serving_XXX is higher than
      S_GSM for the serving cell by at least a specific hysteresis H_PRIO;
      these cells shall be considered in decreasing order of S_non-serving_XXX.

  A UTRAN FDD cell shall only be reselected if, in addition to the criteria
  above, its measured Ec/No value is equal to or greater than FDD_Qmin -
  FDD_Qmin_Offset.

  @param priority_resel_to_lower_layers_permitted TRUE if priority reselection
         to lower priority layers is permitted, FALSE otherwise.

  @return TRUE if reselection should be triggered, FALSE otherwise.
*/
boolean rr_irat_check_for_priority_based_reselection(
  boolean priority_resel_to_lower_layers_permitted,
  boolean immediate_reselection,
  const gas_id_t gas_id
)
{
  rr_irat_priority_list_t             priority_list = {0};
#ifdef FEATURE_WCDMA
  rr_irat_measurements_t             *wcdma_measurements;
#endif /* FEATURE_WCDMA */
  rr_irat_measurements_t             *lte_measurements;
  gprs_scell_info_t                  *scell;
  uint32                              i;
  rr_resel_priority_e                geran_priority;
  uint8                              h_prio;
  const gprs_reselection_structure_T *gprs_reselection_list;
  int                                s_gsm;
  uint8                              t_reselection;
#ifdef FEATURE_GSM_TDS
  rr_irat_measurements_t             *tds_measurements;
#endif /*FEATURE_GSM_TDS*/
  rr_g2w_data_t                      *rr_g2w_data_ptr;
  uint32                              t_reselection_ms;
  boolean                             allowed_rat_list[RR_RAT_MAX] = {0};
  boolean                             resel_possible;
  int                                 meas_skipped1 = 0;
  int                                 meas_skipped2 = 0;
  specific_update_freq_T              specific_irat_freq_update_list[RR_IRAT_PRIORITY_LIST_LENGTH];

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

#ifdef FEATURE_WCDMA
  wcdma_measurements = &rr_g2w_data_ptr->wcdma_measurements;
#endif /* FEATURE_WCDMA */
  lte_measurements = &rr_g2w_data_ptr->lte_measurements;

  scell = rr_gprs_get_scell_info(gas_id);
  geran_priority = rr_geran_priority(gas_id);

  t_reselection = scell->serving_cell_priority_params.t_reselection;
  RR_ASSERT_RETURN_VALUE(t_reselection < ARR_SIZE(T_RESELECTION_MS), FALSE);

  t_reselection_ms = T_RESELECTION_MS[t_reselection];

  h_prio = scell->serving_cell_priority_params.h_prio;

  gprs_reselection_list = rr_get_gprs_reselection_list_ptr(gas_id);
  s_gsm = gprs_reselection_list->serving_values.C1;

#ifdef FEATURE_GSM_TDS
  tds_measurements = &rr_g2w_data_ptr->utran_tdd_measurements;
#endif /*FEATURE_GSM_TDS*/

  rr_g2w_data_ptr->resel_target.valid = FALSE;

#ifdef FEATURE_SGLTE
  
  if((rr_is_sglte_mode_active(gas_id)) && (TRUE == immediate_reselection ))
  {
	MSG_GERAN_MED_0_G("SGLTE active,Forced resel TRUE: GSM should first find service");
	return FALSE;   
  }
#endif /*FEATURE_SGLTE*/


  MSG_GERAN_MED_1_G("GERAN Priority: %d", geran_priority);

  rr_irat_priority_list_populate(
    &priority_list,
#ifdef FEATURE_WCDMA
    wcdma_measurements,
#endif /* FEATURE_WCDMA */
    lte_measurements,
#ifdef FEATURE_GSM_TDS
    tds_measurements,
#endif /*FEATURE_GSM_TDS*/
    gas_id
  );

  rr_irat_priority_list_dump(&priority_list, gas_id);

  rr_irat_priority_list_log(
    scell->gsm.BCCH_ARFCN,
    geran_priority,
    gprs_reselection_list->serving_values.RXLEV_average,
    (int16) s_gsm,
    &priority_list,
    gas_id
  );

  if (priority_resel_to_lower_layers_permitted)
  {
    MSG_GERAN_MED_0_G("Reselection to lower priority layers permitted");
  }
  else
  {
    MSG_GERAN_MED_0_G("Reselection to lower priority layers not permitted");
  }

  /* build the list of "allowed" flags, one per RAT */
  resel_possible = rr_ms_g2x_build_allowed_list(allowed_rat_list, (int)RR_RAT_MAX, gas_id);

  memset(specific_irat_freq_update_list,0xFF,sizeof(specific_irat_freq_update_list));

  /* Iterate over all entries in the list.  For each entry, check whether it
  meets any of the criteria defined in 3GPP 45.008 section 6.6.6 for priority-
  based reselection. */
  for (i = 0; i < priority_list.length; ++i)
  {
    rr_irat_priority_list_entry_t *entry = &priority_list.entries[i];

    /* determine whether reselection to the current entry's RAT is permitted */
    if (allowed_rat_list[entry->measurement->rat] == FALSE)
    {
      meas_skipped1++;
    }
    else if (entry->priority > geran_priority)
    {
      /* Entry is a high priority cell. */
      if (entry->s_value > entry->thresh_high && rr_irat_priority_list_entry_suitable(entry, gas_id))
      {
        if (rr_timer_is_running(entry->measurement->thresh_high_timer, gas_id))
        { 
           /* No action necessary. */
        }
        else if (rr_timer_has_expired(entry->measurement->thresh_high_timer, gas_id))
        {        
          rr_set_reselection_target(entry, gas_id ); 
          rr_g2w_data_ptr->prev_resel_target.valid = FALSE;

          if (!resel_possible)
          {
            MSG_GERAN_HIGH_1_G("Reselection to RAT Blocked due to both Subs in Voice Call", rr_g2w_data_ptr->resel_target.cell.rat );
            rr_g2w_data_ptr->resel_target.valid = FALSE;   
            rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_UNDEFINED;

            MSG_GERAN_MED_3_G("Cell %d: Starting thresh_high_timer (0x%02x) duration %d ms",
                    i,
                    entry->measurement->thresh_high_timer,
                    t_reselection_ms);
            
            (void)rr_timer_start(entry->measurement->thresh_high_timer, t_reselection_ms, gas_id);          
          }

          /* Break out of the loop, either we have a target cell, or reselection is not possible */
          break;
        }
        else
        {
          MSG_GERAN_MED_3_G("Cell %d: Starting thresh_high_timer (0x%02x) duration %d ms",
                  i,
                  entry->measurement->thresh_high_timer,
                  t_reselection_ms);
          (void)rr_timer_start(entry->measurement->thresh_high_timer, t_reselection_ms, gas_id);
          rr_send_mph_specific_irat_freq_update_req(
            rr_irat_get_arfcn_for_meas_entry(entry->measurement,gas_id),
            entry->measurement->rat,
            t_reselection_ms,
            specific_irat_freq_update_list,
            RR_IRAT_PRIORITY_LIST_LENGTH,
            gas_id);

          if ( (rr_get_nv_anite_gcf_flag(RR_GAS_ID_TO_AS_ID) == FALSE) )
          {
            if ( rr_irat_check_if_highest_priority_cell(&priority_list, entry, gas_id) && !(rr_g2w_data_ptr->prev_resel_target.valid) )
            { 
              /* For Highest Priority Cell, Check and Restart any low priority timer running so that they do not expire prior to High Priority Timer Expiry */            
              rr_irat_adjust_all_running_low_priority_timer_values(&priority_list, entry, gas_id);
            }
          }
        }
      }
      else
      {
        if (rr_timer_cancel(entry->measurement->thresh_high_timer, gas_id))
        {
          MSG_GERAN_MED_2_G("Cell %d: Cancelled thresh_high_timer (0x%02x)", i, entry->measurement->thresh_high_timer);
          
        /* This indicates that the High Priority Cell no longer exists */
          /* Check if this was the cell, which was backed up and Timers extended? */
          /*  If so remove theBack up Entry */        
          if (rr_g2w_data_ptr->prev_resel_target.valid)
          {
            rr_irat_check_and_delete_prev_resel_target(entry->measurement, TRUE, gas_id );
          }          
        }
      }

      if (rr_timer_cancel(entry->measurement->thresh_low_timer, gas_id))
      {
        MSG_GERAN_MED_2_G("Cell %d: Cancelled thresh_low_timer (0x%02x)", i, entry->measurement->thresh_low_timer);
      }

      if (rr_timer_cancel(entry->measurement->h_prio_timer, gas_id))
      {
        MSG_GERAN_MED_2_G("Cell %d: Cancelled h_prio_timer (0x%02x)", i, entry->measurement->h_prio_timer);
      }
    }
    else
    {
      /* Entry belongs to a lower priority layer - cancel thresh_high_timer if
      it is running, then check for low priority reselection. */
      if (rr_timer_cancel(entry->measurement->thresh_high_timer, gas_id))
      {           
        MSG_GERAN_MED_2_G("Cell %d: Cancelled thresh_high_timer (0x%02x)", i, entry->measurement->thresh_high_timer);
      }

      if (priority_resel_to_lower_layers_permitted)
      {
        if (entry->s_value > entry->thresh_low &&  rr_irat_priority_list_entry_suitable(entry, gas_id))
        {
          if (rr_timer_is_running(entry->measurement->thresh_low_timer, gas_id))
          {
            /* No action necessary. */
          }
          else if (rr_timer_has_expired(entry->measurement->thresh_low_timer, gas_id))
          {

            rr_set_reselection_target(entry, gas_id );            
            rr_g2w_data_ptr->prev_resel_target.valid = FALSE;

            if (!resel_possible)
            {
              MSG_GERAN_HIGH_1_G("Reselection to RAT Blocked due to both Subs in Voice Call", rr_g2w_data_ptr->resel_target.cell.rat );
              rr_g2w_data_ptr->resel_target.valid = FALSE;   
              rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_UNDEFINED;
            
              MSG_GERAN_MED_3_G("Cell %d: Starting thresh_low_timer (0x%02x) duration %d ms", i, entry->measurement->thresh_low_timer, t_reselection_ms);              
              (void)rr_timer_start(entry->measurement->thresh_low_timer, t_reselection_ms, gas_id);            
            }

            /* Break out of the loop, either we have a target cell, or reselection is not possible */
            break;
          }
          else
          {
            MSG_GERAN_MED_3_G("Cell %d: Starting thresh_low_timer (0x%02x) duration %d ms", i, entry->measurement->thresh_low_timer, t_reselection_ms);
            (void)rr_timer_start(entry->measurement->thresh_low_timer, t_reselection_ms, gas_id);
            rr_send_mph_specific_irat_freq_update_req(
              rr_irat_get_arfcn_for_meas_entry(entry->measurement,gas_id),
              entry->measurement->rat,
              t_reselection_ms,
              specific_irat_freq_update_list,
              RR_IRAT_PRIORITY_LIST_LENGTH,
              gas_id);
          }
        }
        else
        {
          if (rr_timer_cancel(entry->measurement->thresh_low_timer, gas_id))
          {
            MSG_GERAN_MED_2_G("Cell %d: Cancelled thresh_low_timer (0x%02x)", i, entry->measurement->thresh_low_timer);
            if (rr_g2w_data_ptr->prev_resel_target.valid)
            {
              rr_irat_check_and_delete_prev_resel_target(entry->measurement, TRUE, gas_id );
            }  
          }

          if (h_prio != 0 && entry->s_value > (s_gsm + (RR_CONVERT_H_PRIO_TO_DB(h_prio)))
              &&  rr_irat_priority_list_entry_suitable(entry, gas_id))
          {
            if (rr_timer_is_running(entry->measurement->h_prio_timer, gas_id))
            {
              /* No action necessary. */
            }
            else if (rr_timer_has_expired(entry->measurement->h_prio_timer, gas_id))
            {
              rr_set_reselection_target(entry, gas_id );            
              rr_g2w_data_ptr->prev_resel_target.valid = FALSE;
              
              if (!resel_possible)
              {
                MSG_GERAN_HIGH_1_G("Reselection to RAT Blocked due to both Subs in Voice Call", rr_g2w_data_ptr->resel_target.cell.rat );
                rr_g2w_data_ptr->resel_target.valid = FALSE;   
                rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_UNDEFINED;
              
                MSG_GERAN_MED_3_G("Cell %d: Starting h_prio_timer (0x%02x) duration %d ms", i, entry->measurement->h_prio_timer, t_reselection_ms); 
                (void)rr_timer_start(entry->measurement->h_prio_timer, t_reselection_ms, gas_id);
              }

              /* Break out of the loop, either we have a target cell, or reselection is not possible */
              break;
            }
            else
            {
              MSG_GERAN_MED_3_G("Cell %d: Starting h_prio_timer (0x%02x) duration %d ms", i, entry->measurement->h_prio_timer, t_reselection_ms);
              (void)rr_timer_start(entry->measurement->h_prio_timer, t_reselection_ms, gas_id);
              rr_send_mph_specific_irat_freq_update_req(
                rr_irat_get_arfcn_for_meas_entry(entry->measurement,gas_id),
                entry->measurement->rat,
                t_reselection_ms,
                specific_irat_freq_update_list,
                RR_IRAT_PRIORITY_LIST_LENGTH,
                gas_id);
            }
          }
          else
          {
            if (rr_timer_cancel(entry->measurement->h_prio_timer, gas_id))
            {
              MSG_GERAN_MED_2_G("Cell %d: Cancelled h_prio_timer (0x%02x)", i, entry->measurement->h_prio_timer);
              if (rr_g2w_data_ptr->prev_resel_target.valid)
              {
                rr_irat_check_and_delete_prev_resel_target(entry->measurement, TRUE, gas_id );
              }                
            }
          }
        }
      }
      else
      {
        if (rr_timer_cancel(entry->measurement->thresh_low_timer, gas_id))
        {
          MSG_GERAN_MED_2_G("Cell %d: Cancelled thresh_low_timer (0x%02x)", i, entry->measurement->thresh_low_timer);
        }

        if (rr_timer_cancel(entry->measurement->h_prio_timer, gas_id))
        {
          MSG_GERAN_MED_2_G("Cell %d: Cancelled h_prio_timer (0x%02x)", i, entry->measurement->h_prio_timer);
        }
      }
    }
  }

  MSG_GERAN_MED_2_G("Immediate reselection field set %d and target valid set to %d)", immediate_reselection,rr_g2w_data_ptr->resel_target.valid);

  if ((FALSE == rr_g2w_data_ptr->resel_target.valid) && (immediate_reselection == TRUE))
  {
    for (i = 0; i < priority_list.length; ++i)
    {
      rr_irat_priority_list_entry_t * entry = &priority_list.entries[i];

      /* determine whether reselection to the current entry's RAT is permitted */
      if (allowed_rat_list[entry->measurement->rat] == FALSE)
      {
        meas_skipped2++;
      }
      else if (entry->priority > geran_priority)
      {
        /* Entry is a high priority cell */
        if (entry->s_value > entry->thresh_high && rr_irat_priority_list_entry_suitable(entry,gas_id))
        {
          if (rr_timer_is_running(entry->measurement->thresh_high_timer,gas_id))
          {                    
            if(rr_set_reselection_target(entry, gas_id ))
            {              
              rr_g2w_data_ptr->prev_resel_target.valid = FALSE;
              break;
            }
          }
        }
      }
      else
      {
        if (priority_resel_to_lower_layers_permitted)
        {
          if (entry->s_value > entry->thresh_low &&  rr_irat_priority_list_entry_suitable(entry,gas_id))
          {
            if (rr_timer_is_running(entry->measurement->thresh_low_timer,gas_id))
            {                          
              if (rr_set_reselection_target(entry, gas_id ))
              {
                rr_g2w_data_ptr->prev_resel_target.valid = FALSE;            
                break;
              }
            }
          }
        }
        if(rr_g2w_data_ptr->resel_target.valid)
        {
          break;
        }
      }
    }
  }

  if ((rr_g2w_data_ptr->resel_target.valid == TRUE) && (immediate_reselection == TRUE))
  {
    MSG_GERAN_HIGH_0_G("Cell found in priority do force reselection");
  }

  /* debug for any entries that were ignored */
  if (meas_skipped1 || meas_skipped2)
  {
    MSG_GERAN_HIGH_2_G("Ignored %d,%d inter-RAT priority entries", meas_skipped1, meas_skipped2);
  }

  return(rr_g2w_data_ptr->resel_target.valid);
}

void rr_irat_lte_measurements_delete(const gas_id_t gas_id)
{
  rr_irat_measurements_t * lte_measurements;
  uint32 i;
  uint32 no_of_entries;
  rr_g2w_data_t *rr_g2w_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  lte_measurements = &rr_g2w_data_ptr->lte_measurements;

  no_of_entries = MIN(lte_measurements->no_of_entries, RR_IRAT_MEASUREMENTS_PER_RAT);

  for (i = 0; i < no_of_entries; ++i)
  {
    rr_irat_measurements_delete_index(lte_measurements, 0, gas_id);
  }
}
#endif /* FEATURE_LTE */

#ifdef FEATURE_WCDMA
void rr_irat_populate_gprs_3g_reselection_measurements_log_packet(
  LOG_GPRS_RR_3G_RESELECTION_MEASUREMENTS_C_type * log_ptr,
  const gas_id_t gas_id
)
{
  rr_irat_measurements_t * ncells;
  uint32 i;
  rr_g2w_data_t *rr_g2w_data_ptr;

  // Obtain a pointer to the module data
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  ncells = &rr_g2w_data_ptr->wcdma_measurements;

  if (log_ptr != NULL)
  {
    log_ptr->no_of_entries = 0;
    for (i = 0; i < RR_IRAT_MEASUREMENTS_PER_RAT; ++i)
    {
      if (i < ncells->no_of_entries)
      {
        if (ncells->entries[i].rat == RR_RAT_WCDMA)
        {
          log_ptr->no_of_entries++;
          log_ptr->_3g_best_six_list[i].uarfcn = ncells->entries[i].data.wcdma.uarfcn;
          log_ptr->_3g_best_six_list[i].scrambling_code = ncells->entries[i].data.wcdma.scrambling_code;
          log_ptr->_3g_best_six_list[i].diversity = ncells->entries[i].data.wcdma.diversity;
          log_ptr->_3g_best_six_list[i].rscp = ncells->entries[i].data.wcdma.rscp;
          log_ptr->_3g_best_six_list[i]._5_second_timer_running = rr_timer_is_running(
            ncells->entries[i].resel_timer, gas_id
          );
        }
      }
      else
      {
        log_ptr->_3g_best_six_list[i].uarfcn = 0xffff;
      }
    }
  }
}

void rr_irat_resel_target_set(
  uint16 uarfcn,
  uint16 scrambling_code,
  uint16 pn_offset,
  boolean diversity,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_g2w_data_t *rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  rr_g2w_data_ptr->resel_target.valid = TRUE;
  rr_g2w_data_ptr->resel_target.cell.rat = RR_RAT_WCDMA;
  rr_g2w_data_ptr->resel_target.cell.data.wcdma.uarfcn = uarfcn;
  rr_g2w_data_ptr->resel_target.cell.data.wcdma.scrambling_code = scrambling_code;
  rr_g2w_data_ptr->resel_target.cell.data.wcdma.pn_offset = pn_offset;
  rr_g2w_data_ptr->resel_target.cell.data.wcdma.diversity = diversity;
}
#endif /* FEATURE_WCDMA */

rr_irat_cell_t rr_irat_resel_target_get(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_g2w_data_t *rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  return rr_g2w_data_ptr->resel_target.cell;
}

#ifdef FEATURE_GSM_TDS
boolean rr_tds_is_measured (const gas_id_t gas_id)
{
  rr_irat_measurements_t    *rr_utran_tdd_ncells;
  boolean                    tds_is_measured = FALSE;

  rr_g2w_data_t *rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  rr_utran_tdd_ncells = &rr_g2w_data_ptr->utran_tdd_measurements;

  if (rr_utran_tdd_ncells->no_of_entries >0)
  {
    tds_is_measured = TRUE;
  }
  return tds_is_measured;
}
#endif

#else

/* no WCDMA support in this build */
int rr_g2w_module_supported(void)
{
  return(0);
}

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */


#ifdef FEATURE_LTE_REL11
/*check lte de-priority list to see if LTE entry in the Prioritised */
/*list has not been de-Prioritised                                  */
boolean rr_irat_lte_arfcn_deprioritized( rr_irat_measurement_lte_t lte_data_earfcn,
                                         const gas_id_t gas_id)
{
  boolean has_been_deprioritized = FALSE;
  uint8 i;

  // Obtain a pointer to the module data
  rr_g2w_data_t *rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  if( (rr_g2w_data_ptr->depri_lte_earfcn_list.num_depri_freq == 0) 
       && (rr_g2w_data_ptr->depri_lte_earfcn_list.depri_all_freq == FALSE) )
  {
    /*NO action*/
  }
  else
  {
     if(rr_g2w_data_ptr->depri_lte_earfcn_list.depri_all_freq == TRUE)
     {
        MSG_GERAN_MED_0_G("At lte request: All earfcn's are deprioritized");
        has_been_deprioritized = TRUE;
     }
     else
     {
        if(rr_g2w_data_ptr->depri_lte_earfcn_list.num_depri_freq > GERAN_GRR_MAX_LTE_DEPRI_FREQ)   
        {
         rr_g2w_data_ptr->depri_lte_earfcn_list.num_depri_freq = GERAN_GRR_MAX_LTE_DEPRI_FREQ;
        }

        /* All PCID's associated with the LTE earfcn are deprioritized .*/
        /* so no need to check PCID as well as the erafcn               */ 
        for (i = 0; i < rr_g2w_data_ptr->depri_lte_earfcn_list.num_depri_freq; ++i)  
        {
          if( lte_data_earfcn.earfcn == rr_g2w_data_ptr->depri_lte_earfcn_list.depri_earfcn[i] )
          {
            MSG_GERAN_MED_2_G("At lte request: deprioritized earfcn's = %d pcid =%d", lte_data_earfcn.earfcn,lte_data_earfcn.pcid);
            has_been_deprioritized = TRUE;
            break;
          }
        }
     }
  }

 return (has_been_deprioritized);  
}

/*Store the list of de-prioritised frequencies LTE has sent RR */
/*if the lisdt has an entry OR depri_all_freq is set true then */
/*LTE de-prioritization is active                              */
extern void rr_g2w_update_lte_arfcn_deprioritized_list(geran_grr_lte_depri_freq_req_type *lte_depri_freq_ptr,
                                                       const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_g2w_data_t *rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  memset(rr_g2w_data_ptr->depri_lte_earfcn_list.depri_earfcn, 
         0, sizeof(rr_g2w_data_ptr->depri_lte_earfcn_list.depri_earfcn[0]) * GERAN_GRR_MAX_LTE_DEPRI_FREQ);

  if(lte_depri_freq_ptr->num_depri_freq > GERAN_GRR_MAX_LTE_DEPRI_FREQ)
  {
   lte_depri_freq_ptr->num_depri_freq = GERAN_GRR_MAX_LTE_DEPRI_FREQ;
  }

  rr_g2w_data_ptr->depri_lte_earfcn_list.depri_all_freq  = lte_depri_freq_ptr->depri_all_freq;
  rr_g2w_data_ptr->depri_lte_earfcn_list.num_depri_freq = lte_depri_freq_ptr->num_depri_freq;

  memscpy(rr_g2w_data_ptr->depri_lte_earfcn_list.depri_earfcn,
          sizeof(rr_g2w_data_ptr->depri_lte_earfcn_list.depri_earfcn[0]) * GERAN_GRR_MAX_LTE_DEPRI_FREQ, 
          lte_depri_freq_ptr->depri_freqs, 
          sizeof(lte_depri_freq_ptr->depri_freqs[0]) * lte_depri_freq_ptr->num_depri_freq);
}

/*indicate in lte irat Priority list which lte fequencies has been de-prioritised in */
/* ie set to to undefined Priority value                                             */
extern void rr_g2w_update_lte_dedicated_pri_info_with_depri_list(
 lte_irat_dedicated_priority_list_s *lte_irat_ded_pri_list_ptr,
 const gas_id_t gas_id)
{

  uint8 i,j,k = 0;

  // Obtain a pointer to the module data
  rr_g2w_data_t *rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);

  if(rr_g2w_data_ptr->depri_lte_earfcn_list.num_depri_freq > LTE_IRAT_MAX_EUTRAN_FREQ)
  {
    rr_g2w_data_ptr->depri_lte_earfcn_list.num_depri_freq = LTE_IRAT_MAX_EUTRAN_FREQ; 
  }

  if( (rr_g2w_data_ptr->depri_lte_earfcn_list.num_depri_freq == 0) 
       && (rr_g2w_data_ptr->depri_lte_earfcn_list.depri_all_freq == FALSE) )
  {
    MSG_GERAN_MED_2_G("At lte request: No deprioritized earfcn's. num_depri_freq = %d depri_all_freq %d", 
                      rr_g2w_data_ptr->depri_lte_earfcn_list.num_depri_freq, 
                      rr_g2w_data_ptr->depri_lte_earfcn_list.depri_all_freq);
  }
  else
  {
    if(rr_g2w_data_ptr->depri_lte_earfcn_list.depri_all_freq == TRUE)
    {
      MSG_GERAN_MED_0_G("At lte request: all earfcn's deprioritized"); 

      for(j = 0 ; j < lte_irat_ded_pri_list_ptr->num_eutra_freq; j++)
      {
        lte_irat_ded_pri_list_ptr->eutra_freq_list[j].priority = RR_RESEL_PRIORITY_UNDEFINED;
      }

    }
    else
    {              
      for(j = 0 ; j < lte_irat_ded_pri_list_ptr->num_eutra_freq; j++)
      {
        for(i = 0 ; i < rr_g2w_data_ptr->depri_lte_earfcn_list.num_depri_freq; i++)
        {                  
          if(lte_irat_ded_pri_list_ptr->eutra_freq_list[j].arfcn == rr_g2w_data_ptr->depri_lte_earfcn_list.depri_earfcn[i])
          {
            MSG_GERAN_MED_1_G("At lte request: deprioritized lte earfcn's  = %d", 
                              lte_irat_ded_pri_list_ptr->eutra_freq_list[j].arfcn);

              lte_irat_ded_pri_list_ptr->eutra_freq_list[j].priority = RR_RESEL_PRIORITY_UNDEFINED;
              break;
          }
        }
      }
    }
  }
}


/*on NAS request to select a PLMN Check if LTE ERFCAN's have been deprioritized */
/*its frequencies if so LTE needs to be informed, a change of PLMN may results  */
/*in de-prioritization being lifted.                                            */
/*                                                                              */
/*if this is the case then LTE will send RR GERAN_GRR_LTE_DEPRI_FREQ_REQ to     */
/*indicate that  de-prioritization should be removed.                           */

extern void rr_g2w_depri_update_lte_about_requested_plmn(PLMN_id_T *requested_plmn_id,
  const gas_id_t gas_id)
{
  rr_g2w_data_t                      *rr_g2w_data_ptr;

  lte_rrc_depri_requested_plmn_ind_s *lte_rrc_depri_requested_plmn_ind_ptr;
  errno_enum_type                    result;

  /* Obtain a pointer to the module data*/
  rr_g2w_data_ptr = rr_g2w_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_g2w_data_ptr);
  
  /*check de-prioritization is active*/                                                      
  if( (rr_g2w_data_ptr->depri_lte_earfcn_list.num_depri_freq != 0) 
       || (rr_g2w_data_ptr->depri_lte_earfcn_list.depri_all_freq ==TRUE) )
  {
    
    lte_rrc_depri_requested_plmn_ind_ptr = (lte_rrc_depri_requested_plmn_ind_s *)GPRS_MEM_CALLOC(sizeof(lte_rrc_depri_requested_plmn_ind_s));

    RR_NULL_CHECK_RETURN_VOID(lte_rrc_depri_requested_plmn_ind_ptr );

    msgr_init_hdr(
      &lte_rrc_depri_requested_plmn_ind_ptr->msg_hdr,
      MSGR_GERAN_GRR,
      LTE_RRC_DEPRI_REQUESTED_PLMN_IND
    );



    lte_rrc_depri_requested_plmn_ind_ptr->requested_plmn = (* requested_plmn_id);
    result = rr_msgr_send(&lte_rrc_depri_requested_plmn_ind_ptr->msg_hdr, sizeof(lte_rrc_depri_requested_plmn_ind_s), gas_id);
    if (result != E_SUCCESS)
    {
      MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
    }
    GPRS_MEM_FREE(lte_rrc_depri_requested_plmn_ind_ptr);
  }

}

#endif /*FEATURE_LTE_REL11*/
