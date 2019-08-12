/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR PLMN List Module

GENERAL DESCRIPTION
   This module contains functions for RR PLMN list search.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_plmn_list.c#1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
01/18/12    sjw      Check for pre-emption of X2G search by other GSM stack
03/28/11    sjw      New DSC handler for BPLMN power scan in Dual SIM mode
01/06/08    wxu      Refactoring

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"
#include "ms.h"
#include <stdio.h>
#include "sys.h"
#include "sys_v.h"
#include "rr_seg_load.h"
#include "rr_plmn_list.h"
#include "rr_defs.h"
#include "rr_general.h"
#include "rr_shared_cell_selection.h"
#include "rr_log.h"
#include "rr_mm_send.h"
#include "rr_l1_send.h"
#include "rr_ps_access.h"
#include "rr_pscan.h"
#include "rr_candidate_cell_db.h"
#include "rr_decode_bcch.h"
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
#include "rr_mode.h"
#include "rr_g2x.h"
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
#include "rr_rrc_send.h"
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/
#include "rr_cell_selection.h"
#include "rr_arfcn.h"
#ifdef FEATURE_LTE
#include "fs_lib.h"
#endif /* FEATURE_LTE */
#include "rr_ps_access.h"
#include "rr_ps_domain.h"
#include "rr_mac_send.h"
#ifdef FEATURE_GSM_TDS
#include "rr_tds_send.h"
#endif /*FEATURE_GSM_TDS*/
#include "rr_msgr.h"
#ifdef FEATURE_LTE
#include "lte_rrc_ext_api.h"
#endif /*FEATURE_LTE*/
#include "rr_multi_sim.h"
#include "gprs_mem.h"
#include "stringl/stringl.h"
#include "rr_acq_db.h"
#include "geran_nv.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define MIN_REQUIRED_TIME_FOR_IRAT_PLMN_SEARCH 300
#define RR_G2X_SRCH_TIMEOUT_TOLERANCE          120

#ifdef FEATURE_SGLTE
#define RR_SGLTE_MAX_PLMN_LIST_SEARCH_TIME     (60000)  /* Default duration for Search on a Single RAT*/
#endif

#define RR_MAX_PLMN_LIST_SEARCH_TIME     (60000)  /* Default duration  */
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* RR PLMN SEARCH MAIN states */
typedef enum
{
  RR_PLC_STATE_NULL = 0,
  RR_PLC_STATE_GSM,
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  RR_PLC_STATE_G2X,
  RR_PLC_STATE_X2G,
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/
  RR_PLC_STATE_MAX
} rr_plc_state_e;

typedef enum
{
  RR_PLC_GSM_STATE_NULL,
  RR_PLC_GSM_STATE_BLOCKING_PS,
  RR_PLC_GSM_STATE_PSCAN,
  RR_PLC_GSM_STATE_BCCH_LIST_DECODE,
  RR_PLC_GSM_STATE_MAX
} rr_plc_gsm_state_e;

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)

typedef enum
{
  RR_PLC_G2X_STATE_NULL,
  /*!< The G2X state machine is inactive. */

  RR_PLC_G2X_STATE_BLOCKING_PS,
  /*!< PS access being blocked before starting a foreground search */

  RR_PLC_G2X_STATE_FOREGROUND_DEACTIVATING_L1,
  /*!< A foreground search is in progress, L1 is being deactivated. */

  RR_PLC_G2X_STATE_FOREGROUND_IN_PROGRESS,
  /*!< A foreground search is in progress, RR is waiting for a response from the target RAT. */

  RR_PLC_G2X_STATE_FOREGROUND_ACTIVATING_L1,
  /*!< A foreground search is in progress, RR has received a response from the
  target RAT and has sent an MPH_START_GSM_MODE_REQ to L1 and is waiting for
  MPH_START_GSM_MODE_CNF to be sent back. */

  RR_PLC_G2X_STATE_FOREGROUND_ABORTING,
  /*!< A abort event has been received during a foreground search.  RR has
  aborted the appropriate entity and is waiting for a response. */

  RR_PLC_G2X_STATE_BACKGROUND_WAIT_FOR_L1_TIME_IND,
  /*!< A background search is to be performed for a different RAT.  RR is
  waiting for L1 to send an indication of a sufficient gap for the other RAT
  to search in. */

  RR_PLC_G2X_STATE_BACKGROUND_SUSPENDING_L1,
  /*!< L1 is being suspended by RR. */

  RR_PLC_G2X_STATE_BACKGROUND_IN_PROGRESS,
  /*!< Search is in progress on the other RAT. */

  RR_PLC_G2X_STATE_BACKGROUND_RESUMING_L1,
  /*!< L1 is being resumed by RR. */

  RR_PLC_G2X_STATE_BACKGROUND_ABORTING,
  /*!< Background search has been aborted and RR is waiting for a response from
  L1 or the other RAT. */

  RR_PLC_G2X_STATE_BACKGROUND_SUSPENDING_GPRS,
  /*!< GPRS is being suspended prior to searching in the background on the
  other RAT. */

  RR_PLC_G2X_STATE_MAX
  /*!< For internal use only. */
} rr_plc_g2x_state_e;

typedef enum
{
  RR_PLC_G2X_L1_STATUS_SUSPENDING,
  RR_PLC_G2X_L1_STATUS_SUSPENDED,
  RR_PLC_G2X_L1_STATUS_RESUMING,
  RR_PLC_G2X_L1_STATUS_RESUMED
} rr_plc_g2x_l1_status_e;

typedef enum
{
  RR_PLC_X2G_STATE_NULL,
  RR_PLC_X2G_STATE_NULL_SUSPENDED,
  RR_PLC_X2G_STATE_ACTIVATING_L1,
  RR_PLC_X2G_STATE_ACTIVATING_L1_SUSPENDED_BY_RRC,
  RR_PLC_X2G_STATE_PSCAN,
  RR_PLC_X2G_STATE_PSCAN_SUSPENDED,
  RR_PLC_X2G_STATE_PSCAN_RESUMING,
  RR_PLC_X2G_STATE_BCCH_LIST_DECODE,
  RR_PLC_X2G_STATE_BCCH_LIST_DECODE_SUSPENDED,
  RR_PLC_X2G_STATE_BCCH_LIST_DECODE_RESUMING,
  RR_PLC_X2G_STATE_DEACTIVATING_L1,
  RR_PLC_X2G_STATE_DEACTIVATING_L1_SUSPENDED_BY_RRC,
  RR_PLC_X2G_STATE_MAX
  /*!< For internaluse only. */
} rr_plc_x2g_state_e;

#endif /* FEATURE_WCDMA || FEATURE_LTE || defined (FEATURE_GSM_TDS)*/

#ifdef FEATURE_SGLTE
typedef enum
{
  RR_PLC_SGLTE_G2X_STATE_NULL,
  RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_DEACT,
  RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_DEACT_ABORTING,
  RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS,
  RR_PLC_SGLTE_G2X_STATE_RAT_ABORTING,
  RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS_ABORTING,
  RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_ACTIVATION,
  RR_PLC_SGLTE_G2X_STATE_MAX
  /*!< For internaluse only. */
} rr_plc_sglte_g2x_state_e;
#endif /*FEATURE_SGLTE*/

/*!
  @brief This structure stores the search results for an individual RAT.

  When a PLMN list search procedure is initiated RR will first build an ordered
  list of all RATs that must be searched.  The search will then be carried out
  on each RAT in turn, until a termination condition is encountered.  The
  search results for each RAT will be stored in an instance of this data
  structure and when the search is terminated all results will be combined
  before they are provided to the initiating module.
*/
typedef struct
{
  rr_rat_e                      rat;
  /*!< The RAT that this structure is associated with. */

  boolean                       search_done;
  /*!< Indicates whether or not the search on this RAT is complete. */

  sys_detailed_plmn_list_s_type available_plmn_list;
  /*!< List of PLMNs that are available on this RAT. */

  boolean                       hplmn_found;
  /*!< Indication of whether the HPLMN was found on this RAT. */

#ifdef FEATURE_LTE
  detected_frequency_list_type  detected_frequency_list;
  /*!< Technology-agnostic list of frequencies that were identified as in use
  by this RAT.  This information may be used by other RATs to reduce the number
  of frequencies that need be searched. */
#endif /* FEATURE_LTE */

} rr_plc_rat_search_info_t;

typedef struct
{
  rr_plc_state_e               state;
  rr_plc_state_e               old_state;
  rr_plc_gsm_state_e           gsm_state;
  rr_plc_gsm_state_e           old_gsm_state;

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  rr_plc_g2x_state_e           g2x_state;
  rr_plc_g2x_state_e           old_g2x_state;

  /* the status of L1, used when aborting GTOW background search
   * depends on its value, RR may need resume L1 after receiving
   * abort confirm from RRC.
   */
  rr_plc_g2x_l1_status_e      l1_status;

  /* indicates if abort cnf has received from RRC. This is used
   * when RRC abort cnf arrives earlier than message from L1.
   */
  boolean                      rrc_abort_cnf_received;

  rr_plc_x2g_state_e           x2g_state;
  rr_plc_x2g_state_e           old_x2g_state;

  /* if L1 has indicated suspending in MPH_POWER_SCAN_CNF or
   * MPH_DECODE_BCCH_LIST_CNF, and if RR decides to do BCCH
   * list decoding, it sets the flag to TRUE, wait for
   * next cycle
   */
  boolean                      pending_decode_bcch_list_req;

  /* the timestamp at which an inter-RAT search was started */
  timetick_type                x2g_srch_start_sclks;

  /* the timer value from RRC to indicate how long the search can
   * be done on GSM before going back to WCDMA
   */
  uint32                       x2g_search_timer_val;
  uint32                       x2g_update_timer_val;
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

  /* Indicates if PLMN state machine has received an ABORT request */
  boolean                      abort;

  /* FOREGROUND, BACKGROUND or WTOG */
  rr_plc_search_type_e         search_type;

  /* if ps_service shall be enabled in the interval of
   * background search. setup by GRR, according to GRR state and
   * the PLMN search is manual or automatic.
   */
  boolean                      ps_service_enable;

  uint32                       rat_search_count;
  uint32                       rat_search_index;
  rr_plc_rat_search_info_t     rat_search_info[RR_RAT_MAX];
  rr_plc_rat_search_info_t   * rat_search_info_current;

  /* Bands to filter pscan results prior to BCCH decodes */
  sys_band_mask_type           band_filter_mask;

#ifdef FEATURE_LTE
  /* Filtered LTE band_pref, contains only LTE-TDD bands. 
   * This will be used for IRAT search type. */
  sys_lte_band_mask_e_type     band_pref_LTDD;
#endif /* FEATURE_LTE */

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined (FEATURE_LTE)
  // Pointer to allocated X-RAT partial results
  sys_detailed_plmn_list_s_type *x_rat_partial_results_ptr;
#endif // FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE
#endif // FEATURE_MPLMN_PARTIAL_UPDATING

#ifdef FEATURE_SGLTE
  rr_plc_sglte_g2x_state_e     old_sglte_g2x_state;
  rr_plc_sglte_g2x_state_e     sglte_g2x_state;
  boolean                      aborting_no_multirat_activation;
#endif /* FEATURE_SGLTE */
} rr_plc_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_plc_data_t  rr_plc_data[NUM_GERAN_DATA_SPACES];

#ifdef FEATURE_LTE
  /* The contents of the EFS file is defined locally in LTE.  Define
  a structure here to match that definition (found in lte_rrc_plmn_searchi.h) */
static struct
{
  boolean l_to_l_disabled;  /*!< Enable intraLTE searches */
  boolean l_to_w_disabled;  /*!< Enable LTE to W searches */
  boolean l_to_g_disabled;  /*!< Enable LTE to G searches */
  boolean gw_to_l_disabled; /*!< Enable IRAT to LTE searches */
  uint64  guard_tmri_val;   /*!< Guard Timer Value */
  uint64  acq_db_tmri_val;  /*!< ACQ DB Timer Scan Value */

} rr_plc_nv_settings;
#endif /* FEATURE_LTE */

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

#ifdef PERLUTF
#error code not present
#endif // PERLUTF

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id (in)
 *
 * \return rr_plc_data_t*
 */
static rr_plc_data_t *rr_plc_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_plc_data[as_index];
}

/*!
 * \brief Return a text string representing the given state.
 *
 * \param state (in)
 *
 * \return const char*
 */
static const char * rr_plc_state_name(rr_plc_state_e state)
{
  switch (state)
  {
    case RR_PLC_STATE_NULL:
      return "NULL";

    case RR_PLC_STATE_GSM:
      return "GSM";

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
    case RR_PLC_STATE_G2X:
      return "G2X";

    case RR_PLC_STATE_X2G:
      return "X2G";
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

    default:
      MSG_GERAN_ERROR_1("Unknown state: %d", state);
      return "?";
  }
}

/*!
 * \brief Return a text string representing the given state.
 *
 * \param state (in)
 *
 * \return const char*
 */
static const char * rr_plc_gsm_state_name(rr_plc_gsm_state_e state)
{
  switch (state)
  {
    case RR_PLC_GSM_STATE_NULL:
      return "NULL";

    case RR_PLC_GSM_STATE_BLOCKING_PS:
      return "BLOCKING_PS";

    case RR_PLC_GSM_STATE_PSCAN:
      return "PSCAN";

    case RR_PLC_GSM_STATE_BCCH_LIST_DECODE:
      return "BCCH_LIST_DECODE";

    default:
    {
      MSG_GERAN_ERROR_1("Unexpected state: %d", state);
      return "?";
    }
  }
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)

/*!
 * \brief Return a text string representing the given state.
 *
 * \param state (in)
 *
 * \return const char*
 */
static const char * rr_plc_g2x_state_name(rr_plc_g2x_state_e state)
{
  switch (state)
  {
    case RR_PLC_G2X_STATE_NULL:
      return "NULL";

    case RR_PLC_G2X_STATE_BLOCKING_PS:
      return "RR_PLC_G2X_STATE_BLOCKING_PS";

    case RR_PLC_G2X_STATE_FOREGROUND_DEACTIVATING_L1:
      return "FOREGROUND_DEACTIVATING_L1";

    case RR_PLC_G2X_STATE_FOREGROUND_IN_PROGRESS:
      return "FOREGROUND_IN_PROGRESS";

    case RR_PLC_G2X_STATE_FOREGROUND_ACTIVATING_L1:
      return "FOREGROUND_ACTIVATING_L1";

    case RR_PLC_G2X_STATE_FOREGROUND_ABORTING:
      return "FOREGROUND_ABORTING";

    case RR_PLC_G2X_STATE_BACKGROUND_WAIT_FOR_L1_TIME_IND:
      return "BACKGROUND_WAIT_FOR_L1_TIME_IND";

    case RR_PLC_G2X_STATE_BACKGROUND_SUSPENDING_L1:
      return "BACKGROUND_SUSPENDING_L1";

    case RR_PLC_G2X_STATE_BACKGROUND_RESUMING_L1:
      return "BACKGROUND_RESUMING_L1";

    case RR_PLC_G2X_STATE_BACKGROUND_IN_PROGRESS:
      return "BACKGROUND_IN_PROGRESS";

    case RR_PLC_G2X_STATE_BACKGROUND_ABORTING:
      return "BACKGROUND_ABORTING";

    case RR_PLC_G2X_STATE_BACKGROUND_SUSPENDING_GPRS:
      return "BACKGROUND_SUSPENDING_GPRS";

    default:
    {
      MSG_GERAN_ERROR_1("Unknown state: %d", state);
      return "?";
    }
  }
}

/*!
 * \brief Return a text string representing the given state.
 *
 * \param state (in)
 *
 * \return const char*
 */
static const char * rr_plc_x2g_state_name(rr_plc_x2g_state_e state)
{
  switch (state)
  {
    case RR_PLC_X2G_STATE_NULL:
      return "NULL";

    case RR_PLC_X2G_STATE_NULL_SUSPENDED:
      return "NULL_SUSPENDED";

    case RR_PLC_X2G_STATE_ACTIVATING_L1:
      return "ACTIVATING_L1";

    case RR_PLC_X2G_STATE_ACTIVATING_L1_SUSPENDED_BY_RRC:
      return "ACTIVATING_L1_SUSPENDED_BY_RRC";

    case RR_PLC_X2G_STATE_PSCAN:
      return "PSCAN";

    case RR_PLC_X2G_STATE_PSCAN_SUSPENDED:
      return "PSCAN_SUSPENDED";

    case RR_PLC_X2G_STATE_PSCAN_RESUMING:
      return "PSCAN_RESUMING";

    case RR_PLC_X2G_STATE_BCCH_LIST_DECODE:
      return "BCCH_LIST_DECODE";

    case RR_PLC_X2G_STATE_BCCH_LIST_DECODE_SUSPENDED:
      return "BCCH_LIST_DECODE_SUSPENDED";

    case RR_PLC_X2G_STATE_BCCH_LIST_DECODE_RESUMING:
      return "BCCH_LIST_DECODE_RESUMING";

    case RR_PLC_X2G_STATE_DEACTIVATING_L1:
      return "DEACTIVATING_L1";

    case RR_PLC_X2G_STATE_DEACTIVATING_L1_SUSPENDED_BY_RRC:
      return "DEACTIVATING_L1_SUSPENDED_BY_RRC";

    default:
      MSG_GERAN_ERROR_1("Unexpected state: %d", state);
      return "?";
  }
}

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

#ifdef FEATURE_SGLTE
/*!
 * \brief Return a text string representing the given state.
 *
 * \param state (in)
 *
 * \return const char*
 */
static const char * rr_sglte_g2x_plc_state_name(rr_plc_sglte_g2x_state_e state)
{
  switch (state)
  {
    case RR_PLC_SGLTE_G2X_STATE_NULL:
      return "NULL";

    case RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_DEACT:
      return "WAIT_L1_MULTIRAT_DEACT";

    case RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS:
      return "SEARCH_IN_PROGRESS";

    case RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS_ABORTING:
      return "SEARCH_IN_PROGRESS_ABORTING";

    case RR_PLC_SGLTE_G2X_STATE_RAT_ABORTING:
      return "RAT_ABORTING";

    case RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_DEACT_ABORTING:
      return "WAIT_L1_MULTIRAT_DEACT_ABORTING";

    case RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_ACTIVATION:
      return "WAIT_L1_MULTIRAT_ACTIVATION";

    default:
      MSG_GERAN_ERROR_1("Unknown state: %d", state);
      return "?";
  }
}
#endif /*FEATURE_SGLTE*/

/*!
 * \brief One-time initialisation function called at task start.
 */
void rr_plc_task_start_init(void)
{
  int as_index;

  // Zero the control/data structure(s)
  memset(rr_plc_data, 0, sizeof(rr_plc_data));

  // Setup default band filter mask (none)
  for (as_index = 0; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    rr_plc_data[as_index].band_filter_mask = SYS_BAND_MASK_EMPTY;
  }

#ifdef FEATURE_LTE
  if (geran_efs_read_primary("/nv/item_files/modem/lte/rrc/bplmn/bplmn_control",
              &rr_plc_nv_settings,
              sizeof(rr_plc_nv_settings)) < 0)
  {
    MSG_GERAN_HIGH_1("RR_PLMN_LIST_SEARCH: Guard Timer = %u ms (default, EFS file empty)",
                     RR_PLMN_LIST_SEARCH_GUARD_TIMER_DEFAULT_DURATION);
    rr_plc_nv_settings.guard_tmri_val = RR_PLMN_LIST_SEARCH_GUARD_TIMER_DEFAULT_DURATION;
  }
  else
  {
    if (rr_plc_nv_settings.guard_tmri_val > 0)
    {
      MSG_GERAN_HIGH_1("RR_PLMN_LIST_SEARCH: Guard Timer = %u ms",
                       rr_plc_nv_settings.guard_tmri_val);
    }
    else
    {
      MSG_GERAN_HIGH_1("RR_PLMN_LIST_SEARCH: Guart Timer = %u ms (default, EFS has 0 ms)",
                       RR_PLMN_LIST_SEARCH_GUARD_TIMER_DEFAULT_DURATION);
      rr_plc_nv_settings.guard_tmri_val = RR_PLMN_LIST_SEARCH_GUARD_TIMER_DEFAULT_DURATION;
    }
  }
#endif /* FEATURE_LTE */
}

/*!
 * \brief Internal reset function for FSMs and associated data
 *
 * \param Data space ptr
 */
static void rr_plc_reset(rr_plc_data_t *rr_plc_data_ptr)
{
  if (rr_plc_data_ptr != NULL)
  {
    memset(rr_plc_data_ptr, 0, sizeof(rr_plc_data_t));
    rr_plc_data_ptr->state         = RR_PLC_STATE_NULL;
    rr_plc_data_ptr->old_state     = RR_PLC_STATE_NULL;

    rr_plc_data_ptr->gsm_state     = RR_PLC_GSM_STATE_NULL;
    rr_plc_data_ptr->old_gsm_state = RR_PLC_GSM_STATE_NULL;

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
    rr_plc_data_ptr->g2x_state     = RR_PLC_G2X_STATE_NULL;
    rr_plc_data_ptr->old_g2x_state = RR_PLC_G2X_STATE_NULL;

    rr_plc_data_ptr->x2g_state     = RR_PLC_X2G_STATE_NULL;
    rr_plc_data_ptr->old_x2g_state = RR_PLC_X2G_STATE_NULL;
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
  }

  return;
}

/*!
 * \brief Reset the PLC state machine and sub-state machines to NULL
 *
 * \param gas_id (in)
 */
void rr_plc_init(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_plc_data_t *rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);
  rr_plc_reset(rr_plc_data_ptr);
  
  return;
}

/*!
 * \brief Returns the PLMN list search RAT priority list.
 *
 * \param gas_id (in)
 *
 * \return sys_rat_pri_list_info_s_type*
 */
sys_rat_pri_list_info_s_type *rr_plc_get_rat_pri_list_info(const gas_id_t gas_id)
{
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);

  // See if BST band filter is being used
  if (rr_plmn_list_req_info_ptr->mcc_mode_band_filter.num_items > 0)
  {
    // yes - return the BST RAT priority list
    MSG_GERAN_HIGH_0_G("BST filter is in effect for this PLMN search");
    return &rr_plmn_list_req_info_ptr->mcc_mode_band_filter;
  }
  else
  {
    // no - return the normal RAT priority list
    return &rr_plmn_list_req_info_ptr->rat_pri_list_info;
  }
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)

/*!
 * \brief Returns TRUE if the mode is GSM-only, FALSE otherwise.
 *
 * \param gas_id (in)
 *
 * \return boolean
 */
static boolean rr_plc_gsm_only(const gas_id_t gas_id)
{
  return rr_rat_pri_list_info_gsm_only(
    rr_plc_get_rat_pri_list_info(gas_id)
  );
}

/*!
 * \brief Returns TRUE if GSM is present in the RAT Priority List.
 *
 * \param gas_id (in)
 *
 * \return boolean
 */
static boolean rr_plc_is_gsm_supported(const gas_id_t gas_id)
{
  return rr_rat_pri_list_info_is_sys_mode_present(
    rr_plc_get_rat_pri_list_info(gas_id),
    SYS_SYS_MODE_GSM
  );
}

/*!
 * \brief Find a given PLMN in the requested PLMN list.
 *
 * \param plmn_ptr (in) - the PLMN the search for
 * \param gas_id (in)
 *
 * \return boolean - TRUE if the PLMN is present in the list, FALSE otherwise
 */
static boolean rr_plc_find_plmn_in_request_list(
  const sys_detailed_plmn_list_info_s_type *plmn_ptr,
  const gas_id_t gas_id
)
{
  uint32 i;
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);

  RR_NULL_CHECK_RETURN_PARAM(plmn_ptr, FALSE);

  for (i=0; i < rr_plmn_list_req_info_ptr->plmn_list.length; i++)
  {
    if ( (rr_plmn_list_req_info_ptr->plmn_list.info[i].rat == plmn_ptr->rat) &&
         sys_plmn_match(rr_plmn_list_req_info_ptr->plmn_list.info[i].plmn, plmn_ptr->plmn))
    {
      return TRUE;
    }
  }

  return FALSE;
}

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

#ifdef FEATURE_GSM_TDS

/*!
 * \brief Returns TRUE if TDS is present in the RAT Priority List, FALSE otherwise.
 *
 * \param gas_id (in)
 *
 * \return boolean
 */
static boolean rr_plc_is_tds_supported(const gas_id_t gas_id)
{
  if (!rr_g2x_rat_supported(RR_RAT_UTRAN_TDD, gas_id))
  {
    MSG_GERAN_MED_0_G("TDSCDMA not supported on this subscription");
    return(FALSE);
  }

  if (!rr_g2x_allowed_by_gprs_activity(RR_RAT_UTRAN_TDD, gas_id))
  {
    MSG_GERAN_MED_0_G("G2T search would conflict with GPRS activity");
    return(FALSE);
  }

  return rr_rat_pri_list_info_is_sys_mode_present(
    rr_plc_get_rat_pri_list_info(gas_id),
    SYS_SYS_MODE_TDS
  );
}

#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA

/*!
 * \brief Returns TRUE if WCDMA is present in the RAT Priority List, FALSE otherwise.
 *
 * \param gas_id (in)
 *
 * \return boolean
 */
static boolean rr_plc_is_wcdma_supported(const gas_id_t gas_id)
{

  if (!rr_g2x_rat_supported(RR_RAT_WCDMA, gas_id))
  {
    MSG_GERAN_MED_0_G("WCDMA not supported on this subscription");
    return(FALSE);
  }

  if (!rr_g2x_allowed_by_gprs_activity(RR_RAT_WCDMA, gas_id))
  {
    MSG_GERAN_MED_0_G("G2W search would conflict with GPRS activity");
    return(FALSE);
  }

  return rr_rat_pri_list_info_is_sys_mode_present(
    rr_plc_get_rat_pri_list_info(gas_id),
    SYS_SYS_MODE_WCDMA
  );
}

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE

/*!
 * \brief Returns TRUE if LTE is present in the RAT Priority List, FALSE otherwise.
 *
 * \param gas_id (in)
 *
 * \return boolean
 */
static boolean rr_plc_is_lte_supported(const gas_id_t gas_id)
{
  if (!rr_g2x_rat_supported(RR_RAT_LTE, gas_id))
  {
    MSG_GERAN_MED_0_G("LTE not supported on this subscription");
    return(FALSE);
  }

  if (!rr_g2x_allowed_by_gprs_activity(RR_RAT_LTE, gas_id))
  {
    MSG_GERAN_MED_0_G("G2L search would conflict with GPRS activity");
    return(FALSE);
  }

  return rr_rat_pri_list_info_is_sys_mode_present(
    rr_plc_get_rat_pri_list_info(gas_id),
    SYS_SYS_MODE_LTE
  );
}

#endif /* FEATURE_LTE */

/*!
 * \brief Returns the band mask to use for the PLMN list search power-scan.
 *
 * \param gas_id (in)
 *
 * \return boolean
 */
static sys_band_mask_type rr_plc_pscan_band_pref(const gas_id_t gas_id)
{
  sys_band_cap_u_type band_cap_u = {0};
  
  band_cap_u = rr_rat_pri_list_info_band_pref(
    rr_plc_get_rat_pri_list_info(gas_id),
    SYS_SYS_MODE_GSM
  );

  return band_cap_u.chgwt_band_cap;
}

/*!
 * \brief Returns the band mask for the given System Mode.
 *
 * \param sys_mode (in)
 * \param gas_id (in)
 *
 * \return sys_band_mask_type
 */
static sys_band_cap_u_type rr_plc_band_pref_sys_mode(
  sys_sys_mode_e_type sys_mode,
  const gas_id_t      gas_id
)
{
  return rr_rat_pri_list_info_band_pref(
    rr_plc_get_rat_pri_list_info(gas_id),
    sys_mode
  );
}

/*!
 * \brief Returns the band mask for GSM.
 *
 * \param gas_id
 *
 * \return sys_band_mask_type
 */
static sys_band_mask_type rr_plc_band_pref(const gas_id_t gas_id)
{
  sys_band_cap_u_type band_cap_u = {0};
  
  band_cap_u = rr_plc_band_pref_sys_mode(
    SYS_SYS_MODE_GSM,
    gas_id
  );
  
  return band_cap_u.chgwt_band_cap;
}

#ifdef FEATURE_FEMTO_CSG

/*!
 * \brief Returns TRUE if a CSG service search is allowed, FALSE otherwise.
 *
 * \param gas_id (in)
 *
 * \return boolean
 */
static boolean rr_passed_service_search_csg_check(const gas_id_t gas_id)
{
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  boolean                  wcdma_supported = FALSE;
  boolean                  lte_supported = FALSE;

#ifdef FEATURE_WCDMA
  wcdma_supported = rr_plc_is_wcdma_supported(gas_id);
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
  lte_supported = rr_plc_is_lte_supported(gas_id);
#endif /* FEATURE_LTE */

  /* Check that WCDMA support is enabled. */
  if (wcdma_supported || lte_supported)
  {
    /* If any other RATs are indicated in the RAT priority list they should be
    deleted, as a CSG search is only applicable for WCDMA. */
    if (rr_plmn_list_req_info_ptr->rat_pri_list_info.num_items > 1)
    {
      sys_rat_pri_list_info_s_type new_rat_pri_list;
      uint32 i;

      memset(&new_rat_pri_list, 0, sizeof(new_rat_pri_list));

      for (i = 0;
           i < rr_plmn_list_req_info_ptr->rat_pri_list_info.num_items &&
           i < SYS_MAX_ACQ_PREF_RAT_LIST_NUM;
           ++i)
      {
        /* Copy WCDMA and / or LTE entries in to the new list. */
        if (rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_WCDMA)
        {
          new_rat_pri_list.priority_list_info[new_rat_pri_list.num_items] = rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[i];
          ++new_rat_pri_list.num_items;
        }
        else if (rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
        {
          new_rat_pri_list.priority_list_info[new_rat_pri_list.num_items] = rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[i];
          ++new_rat_pri_list.num_items;
        }
      }

      rr_plmn_list_req_info_ptr->rat_pri_list_info = new_rat_pri_list;
    }
    return TRUE;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("SYS_SERVICE_SEARCH_CSG indicated, but WCDMA not in RAT priority list");
    return FALSE;
  }
}

#endif /* FEATURE_FEMTO_CSG */

/**
 * Function: Add an entry to the given PLMN list.
 *           If the PLMN already exists in the list, its entry is replaced
 *           if the signal strength of the new entry is greater than the existing entry's
 *
 * Input:    plmn_id - PLMN Id of the PLMN being added
 *           RXLEV   - Signal strength of the new entry (in RXLEV 0-63 units)
 *           service_capability - Service capability of the cell associated with the entry
 * Output:   list_ptr - pointer to the modified PLMN list
 */
static void rr_plc_add_entry_to_plmn_list(
  sys_detailed_plmn_list_s_type *list_ptr,
  PLMN_id_T                      plmn_id,
  byte                           RXLEV,
  sys_srv_capability_e_type      service_capability
)
{
  /* convert the signal strength from RXLEV to absolute dBm */
  byte signal_strength_abs_dbm = rr_convert_rxlex_to_absolute_dBm(RXLEV);
  sys_signal_quality_e_type signal_quality = SYS_SIGNAL_QUALITY_LOW;
  uint32 index;
  uint32 maxPLMNs;
  boolean entryFound = FALSE;

  ASSERT(list_ptr != NULL);

  /* determine the quality of the signal strength
   * the quality is based on the signal strength being above -85 dBm (03.22 4.4.3)
   * since absolute dBm is the unit, comparison should be < for better signal strength
   */
  if (signal_strength_abs_dbm < 85)
  {
    signal_quality = SYS_SIGNAL_QUALITY_HIGH;
  }

  maxPLMNs = list_ptr->length;

  /* go through the list to determine where to add the new entry
   * foreach PLMN already in the list
   */
  for (index = 0; index < maxPLMNs;++index)
  {
    /* if a plmn match is found (note that both should be PLMN ids received over the
     * air so the order should not matter)
     */
    if (sys_plmn_match(list_ptr->info[index].plmn,plmn_id))
    {
      /* the PLMN is already present in the list, compare signal strength */
      entryFound = TRUE;

      /* if the new signal strength is better than the old strength
       * (remember absolute dBm so better means <)
       */
      if (signal_strength_abs_dbm < list_ptr->info[index].signal_strength)
      {
        /* overwrite the entry with the new values */
        list_ptr->info[index].signal_strength = signal_strength_abs_dbm;
        list_ptr->info[index].signal_quality  = signal_quality;
        list_ptr->info[index].plmn_service_capability = service_capability;
        list_ptr->info[index].rat = SYS_RAT_GSM_RADIO_ACCESS;
      }
    }
  }

  /* if the PLMN is not already in the list */
  if (entryFound == FALSE)
  {
    /* make a new entry in the list for the PLMN */
    if (maxPLMNs < SYS_PLMN_LIST_MAX_LENGTH)
    {
      if (sys_plmn_id_is_valid(plmn_id))
      {
        list_ptr->info[maxPLMNs].plmn = plmn_id;
        list_ptr->info[maxPLMNs].signal_quality = signal_quality;
        list_ptr->info[maxPLMNs].signal_strength = signal_strength_abs_dbm;
        list_ptr->info[maxPLMNs].plmn_service_capability = service_capability;
        list_ptr->info[maxPLMNs].rat = SYS_RAT_GSM_RADIO_ACCESS;

        /* increment the length */
        ++(list_ptr->length);
      }
      else
      {
        MSG_GERAN_ERROR_0("Ignoring Invalid PLMN Id in function: rr_plmn_list_add");
      }
    }
    else
    {
      MSG_GERAN_ERROR_1("Out of entries=%d in PLMN list",(int) maxPLMNs);
    }
  }
}


/**
 * Function: appends PLMN list from source to the destination
 *
 * @Params:  src_list - the source plmn list to be copied from
 *           dst_list - the plmn list to be appended to
 *
 * @Outputs: updated dst_list
 */
static void rr_plc_append_plmn_list(
  sys_detailed_plmn_list_s_type *dst_list,
  sys_detailed_plmn_list_s_type *src_list
)
{
  uint32 num_plmns_to_add = 0;

  RR_NULL_CHECK_RETURN_VOID(dst_list);
  RR_NULL_CHECK_RETURN_VOID(src_list);

  if ((src_list->length == 0) || (dst_list->length >= SYS_PLMN_LIST_MAX_LENGTH))
  {
    MSG_GERAN_HIGH_3(
      "src_list->length=%d dst_list->length=%d (max=%d) - exit",
      src_list->length,
      dst_list->length,
      SYS_PLMN_LIST_MAX_LENGTH
    );
    return;
  }

  if (dst_list->length + src_list->length > SYS_PLMN_LIST_MAX_LENGTH)
  {
    num_plmns_to_add = SYS_PLMN_LIST_MAX_LENGTH - dst_list->length;
  }
  else
  {
    num_plmns_to_add = src_list->length;
  }

  memscpy(&dst_list->info[dst_list->length],
          (sizeof(dst_list->info)-(dst_list->length*sizeof(sys_detailed_plmn_list_info_s_type))),
          src_list->info,
          num_plmns_to_add * sizeof(sys_detailed_plmn_list_info_s_type));

  dst_list->length += num_plmns_to_add;
  MSG_GERAN_HIGH_2("%d PLMN(s) added, total=%d", num_plmns_to_add, dst_list->length);
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined (FEATURE_LTE)
static boolean rr_plc_should_available_plmn_list_be_filtered(const gas_id_t gas_id)
{
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info = rr_get_plmn_list_req_info_ptr(gas_id);
  boolean filter = TRUE;

  if (rr_plmn_list_req_info->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
  {
#ifdef FEATURE_HPPLMN_SRCH_WITH_MANY_PREF_PLMN
    if (rr_plmn_list_req_info->service_search == SYS_SERVICE_SEARCH_NONE)
    {
      MSG_GERAN_MED_0("Not filtering available PLMN list, NAS will filter");
      filter = FALSE;
    }
    else
#endif /* FEATURE_HPPLMN_SRCH_WITH_MANY_PREF_PLMN */
    if (rr_plmn_list_req_info->plmn_list.length == 0)
    {
      MSG_GERAN_MED_0("Not filtering available PLMN list, request list length = 0");
      filter = FALSE;
    }
    if (rr_plmn_list_req_info->service_search == SYS_SERVICE_SEARCH_PRIORITY_PLMN )
    {
        MSG_GERAN_MED_0("Not filtering available PLMN list, When service_search == SYS_SERVICE_SEARCH_PRIORITY_PLMN");
        filter = FALSE;
  }
  }
  else
  {
    MSG_GERAN_MED_1("Not filtering available PLMN list, network selection mode = %d",
            rr_plmn_list_req_info->network_selection_mode);

    filter = FALSE;
  }

#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  if (rr_plmn_list_req_info->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC)
  {
    MSG_GERAN_MED_0("Not filtering PLMN list (SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC)");
    filter = FALSE;
  }
#endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

  if (rr_plmn_list_req_info->service_search == SYS_SERVICE_SEARCH_IRAT_PLMN)
  {
    MSG_GERAN_MED_0("Not filtering PLMN list (SYS_SERVICE_SEARCH_IRAT_PLMN)");
    filter = FALSE;
  }

#ifdef FEATURE_FEMTO_CSG
  if (rr_plmn_list_req_info->service_search == SYS_SERVICE_SEARCH_CSG)
  {
    MSG_GERAN_MED_0("Not filtering PLMN list (SYS_SERVICE_SEARCH_CSG)");
    filter = FALSE;
  }
#endif /* FEATURE_FEMTO_CSG */

  return filter;
}


/**
  @brief Copies available_plmn_list to filtered_available_plmn_list, filtering
         if necessary.

  @param filtered_available_plmn_list Pointer to the destination list that
         will be populated by this function.

  @param available_plmn_list Pointer to the list of PLMNs that are available.
*/
static void rr_plc_filter_plmn_list_if_necessary(
  sys_detailed_plmn_list_s_type * filtered_available_plmn_list,
  sys_detailed_plmn_list_s_type * available_plmn_list,
  const gas_id_t gas_id
)
{
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);

  RR_NULL_CHECK_RETURN_VOID(filtered_available_plmn_list)
  RR_NULL_CHECK_RETURN_VOID(available_plmn_list)

  memset(filtered_available_plmn_list, 0, sizeof(sys_detailed_plmn_list_s_type));

  if (rr_plc_should_available_plmn_list_be_filtered(gas_id))
  {
    uint32 i = 0;
    MSG_GERAN_MED_1_G("Filter PLMN list based on NAS/RRC request, length=%d",
                      rr_plmn_list_req_info_ptr->plmn_list.length);

    /* populate PLMN list (with filter) */
    for (i = 0;
         i < available_plmn_list->length &&
         filtered_available_plmn_list->length < SYS_PLMN_LIST_MAX_LENGTH;
         ++i)
    {
      if (rr_plc_find_plmn_in_request_list(&available_plmn_list->info[i], gas_id))
      {
        filtered_available_plmn_list->info[filtered_available_plmn_list->length] = available_plmn_list->info[i];
        filtered_available_plmn_list->length++;
      }
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Populate PLMN list without filtering ");

    /* populate PLMN list */
    if (available_plmn_list->length != 0)
    {
      if (available_plmn_list->length > SYS_PLMN_LIST_MAX_LENGTH)
      {
        MSG_GERAN_ERROR_2_G("Truncating available_plmn_list->length %d to SYS_PLMN_LIST_MAX_LENGTH (%d)",
                  available_plmn_list->length, SYS_PLMN_LIST_MAX_LENGTH);

        available_plmn_list->length = SYS_PLMN_LIST_MAX_LENGTH;
      }

      *filtered_available_plmn_list = *available_plmn_list;
    }
  }
}
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || defined (FEATURE_LTE)*/

/**
 * Function: Get HPLMN's PLMN-id (from the requested PLMN list)
 *
 * Input:
 * Output:   @plmn_ptr: HPLMN's PLMN-id
 */
static boolean rr_plmn_list_get_hplmn(
  sys_plmn_rat_list_info_s_type *plmn_ptr,
  const gas_id_t gas_id
)
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

  RR_NULL_CHECK_RETURN_PARAM(plmn_ptr, FALSE);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
  if (rr_plmn_list_req_info_ptr->plmn_list.length > 0)
  {
    *plmn_ptr = rr_plmn_list_req_info_ptr->plmn_list.info[0];
    return TRUE;
  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
  return FALSE;
}

#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
/**
 * Function: Get the first GSM PLMN in the requested PLMN list
 *
 * Input:    None
 *
 * Output:   @plmn: the first GSM PLMN in the request PLMN list
 *           return TRUE if there is a GSM PLMN in the list
 */
static boolean rr_plmn_list_get_first_gsm_plmn(
  sys_plmn_rat_list_info_s_type *plmn,
  const gas_id_t gas_id
)
{
  uint32 i = 0;
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);

  while (i < rr_plmn_list_req_info_ptr->plmn_list.length)
  {
    if (rr_plmn_list_req_info_ptr->plmn_list.info[i].rat == SYS_RAT_GSM_RADIO_ACCESS)
    {
      *plmn = rr_plmn_list_req_info_ptr->plmn_list.info[i];
      return TRUE;
    }

    i++;
  }

  return FALSE;
}

#endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

/**
 * Function: Check if PLMN search should stop, when a new
 *           PLMN is found
 *
 * Input:    N/A
 * Output:   TRUE if the search should stop, FALSE else
 */
static boolean rr_plc_plmn_search_should_stop(
  rr_available_plmn_db_T *available_plmn_db_ptr,
  const gas_id_t gas_id
)
{
  sys_plmn_rat_list_info_s_type plmn_to_compare;
  boolean                       compare = FALSE;
  uint8                         index;
  rr_PLMN_list_req_info_T      *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);

  RR_NULL_CHECK_FATAL(available_plmn_db_ptr);
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);
  RR_ZERO_STRUCTURE(plmn_to_compare);

  if (rr_plmn_list_req_info_ptr->network_selection_mode != SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
  {
    return FALSE;
  }

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
  if (rr_plmn_list_req_info_ptr->plmn_list.length == 0)
  {
    return FALSE;
  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

  if (available_plmn_db_ptr->last_updated_plmn_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("last_updated_plmn_ptr is NULL");
    return FALSE;
  }

#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  if ((rr_plmn_list_req_info_ptr->service_search == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL)||
      (rr_plmn_list_req_info_ptr->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC))
  {
    /* compare with the 1st GSM PLMN in the list */
    if (rr_plmn_list_get_first_gsm_plmn(&plmn_to_compare, gas_id))
    {
     compare = TRUE;
    }
  }
  else if (rr_plmn_list_req_info_ptr->service_search == SYS_SERVICE_SEARCH_NONE)
  {
    /* only compare if HPLMN is GSM PLMN */
    if (rr_plmn_list_get_hplmn(&plmn_to_compare, gas_id) &&
        (plmn_to_compare.rat == SYS_RAT_GSM_RADIO_ACCESS))
    {
      compare = TRUE;
    }
  }
#else
  /* only compare if HPLMN is GSM PLMN */
  if (rr_plmn_list_get_hplmn(&plmn_to_compare) &&
      (plmn_to_compare.rat == SYS_RAT_GSM_RADIO_ACCESS))
  {
    compare = TRUE;
  }
#endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

  if (rr_plmn_list_req_info_ptr->service_search == SYS_SERVICE_SEARCH_PRIORITY_PLMN )
  {
    /* compare with the 1st GSM PLMN in the list */
    if (rr_plmn_list_get_first_gsm_plmn(&plmn_to_compare, gas_id))
    {
      compare = TRUE;
    }
  }

  if (compare)
  {
    if (sys_plmn_match(plmn_to_compare.plmn, available_plmn_db_ptr->last_updated_plmn_ptr->PLMN_id))
    {
      /*if we are able to find atleast one cell with allowed LAI then we found requested PLMN otherwise continue the
        PLMN search */
      for(index = 0; index < available_plmn_db_ptr->last_updated_plmn_ptr->num_frequencies;index++)
      {
        if(!rr_is_forbidden_LAI(
              &(available_plmn_db_ptr->last_updated_plmn_ptr->cell_info[index].lai),
              gas_id
            ))
        {
          MSG_GERAN_HIGH_3_G("Requested PLMN found: 0x%02x, 0x%02x, 0x%02x",
          plmn_to_compare.plmn.identity[0],
          plmn_to_compare.plmn.identity[1],
          plmn_to_compare.plmn.identity[2]);

          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
/**
 * Function: Check if HPLMN has been found
 *
 * Input:    N/A
 * Output:   TRUE if HPLMN is found, FALSE else
 */
static boolean rr_plc_w2g_bplmn_check_if_hplmn_found(const gas_id_t gas_id)
{
  uint32 i = 0;
  boolean hplmn_found = FALSE;
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr;
  rr_plc_data_t *rr_plc_data_ptr;

  /* Search through the rat_search_info for GSM RAT. */
  rr_plc_rat_search_info_t * gsm_search_info = NULL;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);

  for (i = 0; i < rr_plc_data_ptr->rat_search_count && i < RR_RAT_MAX; ++i)
  {
    if (rr_plc_data_ptr->rat_search_info[i].rat == RR_RAT_GSM)
    {
      gsm_search_info = &rr_plc_data_ptr->rat_search_info[i];
    }
  }

  if (gsm_search_info)
  {
    if (gsm_search_info->search_done == FALSE)
    {
      MSG_GERAN_ERROR_0_G("gsm_search_info->search_done == FALSE, unexpected");
    }

    for (i = 0; i < gsm_search_info->available_plmn_list.length; ++i)
    {
      if (sys_plmn_match(rr_plmn_list_req_info_ptr->plmn_list.info[0].plmn,
                         gsm_search_info->available_plmn_list.info[i].plmn))
      {
        hplmn_found = TRUE;
        break;
      }
    }
  }

  return hplmn_found;
}
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

static rr_plc_rat_search_info_t * rr_plc_rat_search_info_next(const gas_id_t gas_id)
{
  uint32 i;
  rr_plc_data_t *rr_plc_data_ptr;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  for (i = 0; i < rr_plc_data_ptr->rat_search_count && i < RR_RAT_MAX; ++i)
  {
    if (rr_plc_data_ptr->rat_search_info[i].search_done == FALSE)
    {
      return &rr_plc_data_ptr->rat_search_info[i];
    }
  }
  return NULL;
}

static rr_plc_rat_search_info_t * rr_plc_rat_search_info_find(rr_rat_e rat, const gas_id_t gas_id)
{
  uint32 i;
  rr_plc_data_t *rr_plc_data_ptr;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  for (i = 0; i < rr_plc_data_ptr->rat_search_count && i < RR_RAT_MAX; ++i)
  {
    if (rr_plc_data_ptr->rat_search_info[i].rat == rat)
    {
      return &rr_plc_data_ptr->rat_search_info[i];
    }
  }
  return NULL;
}

#ifdef FEATURE_SGLTE
static void rr_plc_rat_search_set_to_search_done(const gas_id_t gas_id)
{
  uint32 i;
  rr_plc_data_t *rr_plc_data_ptr;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  for (i = 0; i < rr_plc_data_ptr->rat_search_count && i < RR_RAT_MAX; ++i)
  {
    rr_plc_data_ptr->rat_search_info[i].search_done = TRUE;
  }
}
#endif /*FEATURE_SGLTE*/

#ifdef FEATURE_LTE

static void rr_plc_merge_detected_frequency_lists(
  detected_frequency_list_type * dst,
  detected_frequency_list_type * src
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(dst);
  RR_NULL_CHECK_RETURN_VOID(src);

  for (i = 0; i < src->num_freqs && i < LTE_IRAT_MAX_DETECTED_FREQ; ++i)
  {
    if (dst->num_freqs < LTE_IRAT_MAX_DETECTED_FREQ)
    {
      dst->freq[dst->num_freqs++] = src->freq[i];
    }
    else
    {
      MSG_GERAN_ERROR_0("detected frequency list structure is full");
      break;
    }
  }
}

static void rr_plc_detected_frequency_list_populate(
  detected_frequency_list_type * detected_frequency_list,
  const gas_id_t gas_id
)
{
  uint32 i;
  rr_plc_data_t *rr_plc_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(detected_frequency_list);

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  memset(detected_frequency_list, 0, sizeof(detected_frequency_list_type));

  for (i = 0; i < rr_plc_data_ptr->rat_search_count && i < RR_RAT_MAX; ++i)
  {
    if (rr_plc_data_ptr->rat_search_info[i].search_done)
    {
      MSG_GERAN_MED_1_G("Adding detected frequencies from RAT %d", rr_plc_data_ptr->rat_search_info[i].rat);
      rr_plc_merge_detected_frequency_lists(
        detected_frequency_list,
        &rr_plc_data_ptr->rat_search_info[i].detected_frequency_list
      );
    }
  }
}

/*!
  @brief This function checks if band_pref passed to GRR in RR_PLMN_LIST_REQ is valid or not.
 */
static boolean rr_plc_validate_plmn_search_band(rr_rat_e rat, const gas_id_t gas_id)
{
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  sys_band_cap_u_type band_cap_u = {0};
  sys_sys_mode_e_type sys_mode = SYS_SYS_MODE_GSM;
  
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);

  switch(rat)
  {
    case RR_RAT_GSM:
  {
      sys_mode = SYS_SYS_MODE_GSM;
      band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_GSM, gas_id);
      break;
    }
    
#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      sys_mode = SYS_SYS_MODE_WCDMA;
      band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_WCDMA, gas_id);
      break;
    }
#endif /* FEATURE_WCDMA */
    
#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      sys_mode = SYS_SYS_MODE_LTE;
      band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_LTE, gas_id);
      break;
    }
#endif /* FEATURE_LTE */
      
#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
      {
      sys_mode = SYS_SYS_MODE_TDS;
      band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_TDS, gas_id);
      break;
      }
#endif /*FEATURE_GSM_TDS*/

    default:
    {
      MSG_GERAN_ERROR_0_G("Invalid RAT in PLMN search");
      break;
    }
  }
  
  return (!rr_is_band_mask_empty(band_cap_u, sys_mode));
}
#endif /* FEATURE_LTE */

/*!
  @brief This function parses the information contained in the PLMN list search
         request and populates the rat_search_info list with details pertaining
         to the searches that need to be performed.
 */
static void rr_plc_start_plmn_search(const gas_id_t gas_id)
{
  rr_candidate_cell_db_T  *candidate_db_ptr;
  rr_available_plmn_db_T  *available_plmn_db_ptr;
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr;
  rr_plc_data_t           *rr_plc_data_ptr;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  candidate_db_ptr = rr_get_candidate_cell_db_ptr(gas_id);
  RR_NULL_CHECK_FATAL(candidate_db_ptr);

  available_plmn_db_ptr = rr_get_available_plmn_db_ptr(gas_id);
  RR_NULL_CHECK_FATAL(available_plmn_db_ptr);

  rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);

  if (rr_plmn_list_req_info_ptr->network_selection_mode != SYS_NETWORK_SELECTION_MODE_AUTOMATIC &&
      rr_plmn_list_req_info_ptr->network_selection_mode != SYS_NETWORK_SELECTION_MODE_MANUAL)
  {
    MSG_GERAN_ERROR_1_G("Unexpected network selection mode: %d",
              rr_plmn_list_req_info_ptr->network_selection_mode);
  }

  memset(rr_plc_data_ptr->rat_search_info, 0, sizeof(rr_plc_rat_search_info_t) * RR_RAT_MAX);

  if (rr_plmn_list_req_info_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
  {
    rr_plc_data_ptr->rat_search_count = 0;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined (FEATURE_LTE)
    if (rr_plc_gsm_only(gas_id))
    {
      MSG_GERAN_HIGH_0_G("PLMN list request indicates GSM-only, search on GSM only");
      rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].rat = RR_RAT_GSM;
      ++rr_plc_data_ptr->rat_search_count;
    }
    else
    {
      /* We need to search the RATs in the order in which they occur in the
      PLMN list contained in the request. */
      boolean sys_rat_gsm_radio_access_included = FALSE;
#ifdef FEATURE_WCDMA
      boolean sys_rat_umts_radio_access_included = FALSE;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
      boolean sys_rat_lte_radio_access_included = FALSE;
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
      boolean sys_rat_tds_radio_access_included = FALSE;
#endif /*FEATURE_GSM_TDS*/

      uint32 i;

      MSG_GERAN_HIGH_0_G("Building PLMN RAT search list:");

      for (i = 0;
           i < rr_plmn_list_req_info_ptr->plmn_list.length &&
           i < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH;
           ++i)
      {
        sys_radio_access_tech_e_type rat = rr_plmn_list_req_info_ptr->plmn_list.info[i].rat;

        switch (rat)
        {
          case SYS_RAT_GSM_RADIO_ACCESS:
          {
            if (sys_rat_gsm_radio_access_included == FALSE)
            {
              MSG_GERAN_HIGH_0_G("  GSM will be searched");
              rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].rat = RR_RAT_GSM;
              ++rr_plc_data_ptr->rat_search_count;
              sys_rat_gsm_radio_access_included = TRUE;
            }
            break;
          } /* SYS_RAT_GSM_RADIO_ACCESS */

#ifdef FEATURE_GSM_TDS
          case SYS_RAT_TDS_RADIO_ACCESS:
          {
            if (sys_rat_tds_radio_access_included == FALSE)
            {
              if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA))
              {
#ifdef FEATURE_SGLTE
                if (rr_sglte_g2x_bplmn_search_active(gas_id) &&
                    rr_sglte_inhibit_tds_bplmn_search(gas_id))
                {
                  MSG_GERAN_HIGH_0_G("  TDSCDMA search inhibited");
                  rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].search_done = TRUE;
                }
                else
#endif /*FEATURE_SGLTE*/
                {
                  MSG_GERAN_HIGH_0_G("  TDSCDMA will be searched");
                }

                rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].rat = RR_RAT_UTRAN_TDD;
                ++rr_plc_data_ptr->rat_search_count;
                sys_rat_tds_radio_access_included = TRUE;
              }
              else
              {
                MSG_GERAN_HIGH_0_G("RR TDSCDMA memory segmented not loaded, skip TDSCDMA search");
              }
            }
            break;
          } /* SYS_RAT_UMTS_RADIO_ACCESS */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
          case SYS_RAT_UMTS_RADIO_ACCESS:
          {
            if (sys_rat_umts_radio_access_included == FALSE)
            {
              if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA))
              {
                MSG_GERAN_HIGH_0_G("  WCDMA will be searched");
                rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].rat = RR_RAT_WCDMA;
                ++rr_plc_data_ptr->rat_search_count;
                sys_rat_umts_radio_access_included = TRUE;
              }
              else
              {
                MSG_GERAN_HIGH_0_G("RR WCDMA memory segmented not loaded, skip WCDMA search");
              }
            }
            break;
          } /* SYS_RAT_UMTS_RADIO_ACCESS */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
          case SYS_RAT_LTE_RADIO_ACCESS:
          {
            if (sys_rat_lte_radio_access_included == FALSE)
            {
#ifdef FEATURE_SGLTE
              if (rr_sglte_g2x_bplmn_search_active(gas_id) &&
                  rr_sglte_inhibit_lte_bplmn_search(gas_id))
              {
                MSG_GERAN_HIGH_0_G("  LTE search inhibited");
                rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].search_done = TRUE;
              }
              else
#endif /*FEATURE_SGLTE*/
              if (!rr_plc_validate_plmn_search_band(RR_RAT_LTE, gas_id))
              {
                MSG_GERAN_ERROR_0_G("  LTE search inhibited due to wrong band_pref");
                rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].search_done = TRUE;
              }
              else
              {
                MSG_GERAN_HIGH_0_G("  LTE will be searched");
              }

              rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].rat = RR_RAT_LTE;
              ++rr_plc_data_ptr->rat_search_count;
              sys_rat_lte_radio_access_included = TRUE;
            }
            break;
          } /* SYS_RAT_LTE_RADIO_ACCESS */
#endif /* FEATURE_LTE */

          default:
          {
            MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rat);
          }
        }


        if (sys_rat_gsm_radio_access_included
#ifdef FEATURE_WCDMA
           && sys_rat_umts_radio_access_included
#endif /*FEATURE_WCDMA*/
#ifdef FEATURE_LTE
           && sys_rat_lte_radio_access_included
#endif /*FEATURE_LTE*/
#ifdef FEATURE_GSM_TDS
           && sys_rat_tds_radio_access_included
#endif /*FEATURE_GSM_TDS*/
           )
        {
          break;
        }
      }

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
      // Clear pointer to allocated X-RAT partial results
      rr_plc_data_ptr->x_rat_partial_results_ptr = NULL;
#endif // FEATURE_MPLMN_PARTIAL_UPDATING
    }

    /* Legacy behaviour dictates that if the PLMN list is empty in the request
    then RR should search on only GSM. */
    if (rr_plc_data_ptr->rat_search_count == 0)
    {
      rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].rat = RR_RAT_GSM;
      ++rr_plc_data_ptr->rat_search_count;
    }
#else
    rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].rat = RR_RAT_GSM;
    ++rr_plc_data_ptr->rat_search_count;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE*/
  }
  else
  {
    rr_plc_data_ptr->rat_search_count = 0;

    MSG_GERAN_HIGH_0_G("Building PLMN RAT search list:");

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
    if (rr_plc_is_gsm_supported(gas_id))
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/
    {
      MSG_GERAN_HIGH_0_G("  GSM will be searched");
      /* In manual mode, RR will search all supported RATs in a fixed order. */
      rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].rat = RR_RAT_GSM;
      ++rr_plc_data_ptr->rat_search_count;
    }
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
    else
    {
      MSG_GERAN_HIGH_0_G("  GSM will not be searched");
    }
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
    if (rr_plc_is_lte_supported(gas_id))
    {
      if (!rr_plc_validate_plmn_search_band(RR_RAT_LTE, gas_id))
      {
        MSG_GERAN_ERROR_0_G("  LTE search inhibited due to wrong band_pref");
        rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].search_done = TRUE;
      }
      else
      {
        MSG_GERAN_HIGH_0_G("  LTE will be searched");
      }
      
      rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].rat = RR_RAT_LTE;
      ++rr_plc_data_ptr->rat_search_count;
    }
    else
    {
      MSG_GERAN_HIGH_0_G("LTE will not be searched");
    }
#endif /* FEATURE_LTE */

#ifdef FEATURE_WCDMA
    if (rr_plc_is_wcdma_supported(gas_id))
    {
      MSG_GERAN_HIGH_0_G("  WCDMA will be searched");
      rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].rat = RR_RAT_WCDMA;
      ++rr_plc_data_ptr->rat_search_count;
    }
    else
    {
      MSG_GERAN_HIGH_0_G("  WCDMA will not be searched");
    }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    if (rr_plc_is_tds_supported(gas_id))
    {
      MSG_GERAN_HIGH_0_G("  TDSCDMA will be searched");
      rr_plc_data_ptr->rat_search_info[rr_plc_data_ptr->rat_search_count].rat = RR_RAT_UTRAN_TDD;
      ++rr_plc_data_ptr->rat_search_count;
    }
    else
    {
      MSG_GERAN_HIGH_0_G("  TDSCDMA will not be searched");
    }
#endif /* FEATURE_GSM_TDS */
  }

  /* check if RR has a valid PLMN database, if plmn db is valid,
   * construct PLMN list from it (use manual mode selection criteria)
   * and no need search on GSM
   */
  if (rr_plmn_db_is_valid(available_plmn_db_ptr, rr_plc_pscan_band_pref(gas_id), gas_id))
  {
    rr_plc_rat_search_info_t * rat_search_info = rr_plc_rat_search_info_find(RR_RAT_GSM, gas_id);

    if (rat_search_info != NULL)
    {
      rr_plc_construct_plmn_list(
        available_plmn_db_ptr,
        &rat_search_info->available_plmn_list,
        (rr_plmn_list_req_info_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
         ? NO_CHECK
         : CHECK_FORBIDDEN_LAI_LIST,
        gas_id
      );
      /* TODO: Check for HPLMN-found here if in AUTOMATIC mode */
      rat_search_info->search_done = TRUE;
    }
  }

  /* PLMN list search procedure is being started: */
  MSG_GERAN_HIGH_0_G("Starting PLMN list search");
#ifdef FEATURE_FEMTO_CSG
  if (rr_plmn_list_req_info_ptr->service_search == SYS_SERVICE_SEARCH_CSG)
  {
    if (rr_plmn_list_req_info_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
    {
      rr_log_plmn_list_started(RR_LOG_PLMN_LIST_MANUAL_CSG_LIST_SEARCH, gas_id);
    }
    else
    {
      rr_log_plmn_list_started(RR_LOG_PLMN_LIST_AUTOMATIC_CSG_LIST_SEARCH, gas_id);
    }
  }
  else
#endif /* FEATURE_FEMTO_CSG */
  {
    rr_log_plmn_list_started(
      (rr_plc_data_ptr->search_type == RR_PLC_FOREGROUND_SEARCH)?
      RR_LOG_PLMN_LIST_IMMEDIATE:RR_LOG_PLMN_LIST_BACKGROUND,
      gas_id
    );
  }

  if (candidate_db_ptr != NULL)
  {
    rr_candidate_db_clear(candidate_db_ptr);
  }
  if (available_plmn_db_ptr != NULL)
  {
    rr_plmn_db_clear(available_plmn_db_ptr, gas_id);
  }
}


/**
 * Function: returns TRUE if PLMN search is in foreground
 *           WTOG search is considered in foreground
 *
 * @Params:  N/A
 */
boolean rr_plc_search_in_foreground(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_plc_data_t *rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  switch (rr_plc_data_ptr->search_type)
  {
    case RR_PLC_BACKGROUND_SEARCH:
#ifdef FEATURE_SGLTE
    case RR_PLC_SGLTE_X2G_BACKGROUND_SEARCH:
#endif /* FEATURE_SGLTE */
    {
      return FALSE;
    }

    default:
    {
      return TRUE;
    }
  }
}

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
#ifdef FEATURE_LTE

static void rr_plc_send_lte_rrc_g_get_plmn_prtl_results_req(
  const gas_id_t gas_id
)
{
  lte_irat_get_plmn_prtl_results_req_s  msg;
  errno_enum_type                       result;

  memset(&msg, 0, sizeof(lte_irat_get_plmn_prtl_results_req_s));

  msgr_init_hdr(
    &msg.msg_hdr,
    MSGR_GERAN_GRR,
    LTE_RRC_G_GET_PLMN_PRTL_RESULTS_REQ
  );

  result = rr_msgr_send(&msg.msg_hdr, sizeof(msg), gas_id);

  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}

static void rr_plc_send_geran_grr_lte_irat_plmn_srch_rsp_partial(
  const gas_id_t gas_id
)
{
  lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
  errno_enum_type           result;

  plmn_srch_rsp_ptr = (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));
  RR_NULL_CHECK_FATAL(plmn_srch_rsp_ptr);

  msgr_init_hdr(
    &plmn_srch_rsp_ptr->msg_hdr,
    MSGR_GERAN_GRR,
    GERAN_GRR_LTE_PLMN_SRCH_RSP
  );

  rr_get_inter_rat_bplmn_prtl_result(&plmn_srch_rsp_ptr->available_plmn_list);

  plmn_srch_rsp_ptr->srch_done = FALSE;
  plmn_srch_rsp_ptr->hplmn_found = FALSE;
  plmn_srch_rsp_ptr->partial_srch = TRUE;

  rr_plc_detected_frequency_list_populate(&plmn_srch_rsp_ptr->detected_frequency_list, gas_id);

  result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);

  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }

  GPRS_MEM_FREE(plmn_srch_rsp_ptr);
}

#endif // FEATURE_LTE
#endif // FEATURE_MPLMN_PARTIAL_UPDATING

/**
 * Function: Performs additional notifications to L1 after PLMN search
 *
 * Input:    GAS ID
 * Output:   MPH_DEVICE_MODE_IND to Layer 1 if device mode is DSDA
 */
static void rr_plc_update_l1_on_completion(const gas_id_t gas_id)
{
#if defined(FEATURE_SGLTE_DUAL_SIM) || defined(FEATURE_SVLTE_DUAL_SIM)
  if (SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE == geran_get_multi_sim_mode())
  {
    /* prompt GL1 with device mode in case chain reassignment is required */
    rr_send_mph_device_mode_ind(GERAN_DEVICE_MODE_CAUSE_PLMN, gas_id);
  }
  else
#endif /* FEATURE_SGLTE_DUAL_SIM || FEATURE_SVLTE_DUAL_SIM */
  {
    MSG_GERAN_HIGH_0_G("No need to update GL1 in current mode");
  }

  return;
}

/**
 * Function: Sends PLMN_LIST_CNF or RR_INTERRAT_PLMN_SRCH_CNF
 *           back to MM/RRC
 *
 * Input:    @N/A
 * Output:   RR_PLMN_LIST_CNF to MM or
 *           RR_INTERRAT_PLMN_SRCH_CNF to RRC
 */
static void rr_plc_search_complete(const gas_id_t gas_id)
{
  uint32 i;
  sys_detailed_plmn_list_s_type *available_plmn_list_ptr;
  sys_detailed_plmn_list_s_type *filtered_available_plmn_list_ptr;
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr;
  rr_plc_data_t *rr_plc_data_ptr;

  // Cancel partial results & guard timers
#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
  rr_timer_cancel(RR_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER, gas_id);
#endif // FEATURE_MPLMN_PARTIAL_UPDATING
  rr_timer_cancel(RR_PLMN_LIST_SEARCH_GUARD_TIMER, gas_id);

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);

  available_plmn_list_ptr = GPRS_MEM_CALLOC(sizeof(sys_detailed_plmn_list_s_type));
  RR_NULL_CHECK_FATAL(available_plmn_list_ptr);

  filtered_available_plmn_list_ptr = GPRS_MEM_CALLOC(sizeof(sys_detailed_plmn_list_s_type));
  RR_NULL_CHECK_FATAL(filtered_available_plmn_list_ptr);

  for (i = 0; i < rr_plc_data_ptr->rat_search_count && i < RR_RAT_MAX; ++i)
  {
    if (rr_plc_data_ptr->rat_search_info[i].search_done)
    {
      rr_plc_append_plmn_list(
        available_plmn_list_ptr,                                   // dst_list
        &rr_plc_data_ptr->rat_search_info[i].available_plmn_list   // src_list
      );
    }
  }

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined (FEATURE_LTE)
#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
  if (rr_plc_data_ptr->x_rat_partial_results_ptr != NULL)
  {
    // Add any partial results obtained when aborting
    rr_plc_append_plmn_list(
      available_plmn_list_ptr,                    // dst_list
      rr_plc_data_ptr->x_rat_partial_results_ptr  // src_list
    );

    // Don't need these partial results anymore
    GPRS_MEM_FREE(rr_plc_data_ptr->x_rat_partial_results_ptr);
	rr_plc_data_ptr->x_rat_partial_results_ptr = NULL;	
  }
#endif // FEATURE_MPLMN_PARTIAL_UPDATING

  rr_plc_filter_plmn_list_if_necessary(
    filtered_available_plmn_list_ptr,   // filtered_available_plmn_list
    available_plmn_list_ptr,            // available_plmn_list
    gas_id                              // gas_id
  );
#else
  *filtered_available_plmn_list_ptr = *available_plmn_list_ptr;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || defined (FEATURE_LTE)*/

  switch (rr_plc_data_ptr->search_type)
  {
    case RR_PLC_FOREGROUND_SEARCH:
    case RR_PLC_BACKGROUND_SEARCH:
    {
      rr_plc_update_l1_on_completion(gas_id);

      rr_send_plmn_list_cnf(
        filtered_available_plmn_list_ptr,                     // available_plmn_list_ptr
        rr_plc_data_ptr->abort?MM_AS_ABORTED:MM_AS_SUCCESS,   // status
        rr_plmn_list_req_info_ptr,                            // plmn_list_req
        gas_id                                                // gas_id
      );
      break;
    }

#ifdef FEATURE_WCDMA
    case RR_PLC_WTOG_SEARCH:
    {
      if (rr_plc_data_ptr->abort)
      {
        rr_send_interrat_plmn_search_abort_cnf(gas_id);
      }
      else
      {
        rr_send_interrat_plmn_search_cnf(
          TRUE,                                            // srch_done
          rr_plc_w2g_bplmn_check_if_hplmn_found(gas_id),   // hplmn_found
          filtered_available_plmn_list_ptr,                // plmn_list
          gas_id                                           // gas_id
        );
      }
      break;
    }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case RR_PLC_TTOG_SEARCH:
    {
      if (rr_plc_data_ptr->abort)
      {
        rr_send_tds_interrat_plmn_search_abort_cnf(gas_id);
      }
      else
      {
        rr_send_tds_interrat_plmn_search_cnf(
          TRUE,                                            // srch_done
          rr_plc_w2g_bplmn_check_if_hplmn_found(gas_id),   // hplmn_found
          filtered_available_plmn_list_ptr,                // plmn_list
          gas_id                                           // gas_id
        );
      }
      break;
    }
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
    case RR_PLC_LTOG_SEARCH:
    {
      if (rr_plc_data_ptr->abort)
      {
        lte_irat_abort_plmn_srch_rsp_s abort_plmn_srch_rsp;
        errno_enum_type                result;

        memset(&abort_plmn_srch_rsp, 0, sizeof(lte_irat_abort_plmn_srch_rsp_s));

        msgr_init_hdr(
          &abort_plmn_srch_rsp.msg_hdr,
          MSGR_GERAN_GRR,
          GERAN_GRR_LTE_ABORT_PLMN_SRCH_RSP
        );

        result = rr_msgr_send(&abort_plmn_srch_rsp.msg_hdr, sizeof(abort_plmn_srch_rsp), gas_id);
        if (result != E_SUCCESS)
        {
          MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
        }
      }
      else
      {
        lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
        errno_enum_type           result;

        plmn_srch_rsp_ptr = (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));
        RR_NULL_CHECK_FATAL(plmn_srch_rsp_ptr);

        msgr_init_hdr(
          &plmn_srch_rsp_ptr->msg_hdr,
          MSGR_GERAN_GRR,
          GERAN_GRR_LTE_PLMN_SRCH_RSP
        );

        if (filtered_available_plmn_list_ptr != NULL)
        {
          plmn_srch_rsp_ptr->available_plmn_list = *filtered_available_plmn_list_ptr;
        }

        rr_invalidate_csg_info(&plmn_srch_rsp_ptr->available_plmn_list);

        plmn_srch_rsp_ptr->srch_done = TRUE;
        plmn_srch_rsp_ptr->partial_srch = FALSE;

        if (rr_plmn_list_req_info_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
        {
          plmn_srch_rsp_ptr->hplmn_found = rr_plc_w2g_bplmn_check_if_hplmn_found(gas_id);
        }

        rr_plc_detected_frequency_list_populate(&plmn_srch_rsp_ptr->detected_frequency_list, gas_id);

        result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);

        if (result != E_SUCCESS)
        {
          MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
        }

        GPRS_MEM_FREE(plmn_srch_rsp_ptr);
      }
      break;
    }
#endif /* FEATURE_LTE */

#ifdef FEATURE_SGLTE
    case RR_PLC_SGLTE_X2G_FOREGROUND_SEARCH:
    case RR_PLC_SGLTE_X2G_BACKGROUND_SEARCH:
    {
      detected_frequency_list_type detected_frequency_list;
      boolean                      hplmn_found = FALSE;

      if (rr_plmn_list_req_info_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        hplmn_found = rr_plc_w2g_bplmn_check_if_hplmn_found(gas_id);
      }

      rr_plc_detected_frequency_list_populate(&detected_frequency_list, gas_id);

      rr_plmn_list_send_omsg_sglte_x2g_plmn_list_cnf(
        rr_plmn_list_req_info_ptr->network_selection_mode,
        filtered_available_plmn_list_ptr,
        hplmn_found,
        rr_plc_data_ptr->abort ? MM_AS_ABORTED : MM_AS_SUCCESS,
        rr_plmn_list_req_info_ptr->trans_id,
        rr_plmn_list_req_info_ptr->service_search,
        &detected_frequency_list,
        gas_id
      );
      break;
    } /* RR_PLC_SGLTE_X2G_FOREGROUND_SEARCH ... */
#endif /* FEATURE_SGLTE */

    default:
      MSG_GERAN_ERROR_1_G("Unexpected search_type %d", rr_plc_data_ptr->search_type);
      break;
  }

  /*
   * !!! don't reset rr_plc_data, it is used in rr_control()
   */
  rr_log_plmn_list_completed(gas_id);

  if (available_plmn_list_ptr != NULL)
  {
    GPRS_MEM_FREE(available_plmn_list_ptr);
  }
  if (filtered_available_plmn_list_ptr != NULL)
  {
    GPRS_MEM_FREE(filtered_available_plmn_list_ptr);
  }

  rr_plc_data_ptr->band_filter_mask = SYS_BAND_MASK_EMPTY;
}

#ifdef FEATURE_LTE

static void rr_plc_populate_detected_frequency_list(
  detected_frequency_list_type * detected_frequency_list,
  const rr_arfcn_list_t        * arfcn_list
)
{
  uint32 i;
  detected_frequency_type * detected_frequency;

  RR_NULL_CHECK_RETURN_VOID(detected_frequency_list);
  RR_NULL_CHECK_RETURN_VOID(arfcn_list);

  for (i = 0;
       i < arfcn_list->count &&
       i < RR_ARFCN_LIST_SIZE &&
       i < LTE_IRAT_MAX_DETECTED_FREQ;
       ++i)
  {
    uint32 frequency = rr_arfcn_frequency(arfcn_list->entries[i]);
    uint16 frequency_100khz = (uint16) (frequency / 100000);

    detected_frequency = &detected_frequency_list->freq[i];
    detected_frequency->frequency = frequency_100khz;
    detected_frequency->rat = SYS_RAT_GSM_RADIO_ACCESS;
    detected_frequency->bandwidth = 2;
  }
  detected_frequency_list->num_freqs = (uint16) i;
}

#endif /* FEATURE_LTE */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
/*===========================================================================

FUNCTION     rr_ms_x2g_search_preempted

DESCRIPTION  Checks whether a MPH_SUSPEND_GSM_MODE_IND implies pre-emption

DEPENDENCIES None

RETURN VALUE TRUE if MPH_SUSPEND_GSM_MODE_IND arrived earlier than expected

SIDE EFFECTS None

===========================================================================*/
static boolean rr_ms_x2g_search_preempted(const gas_id_t gas_id)
{
  rr_plc_data_t *rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  boolean        was_preempted;
  uint32         elapsed_ms;

  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  /* determine the number of milliseconds since the search was started */
  elapsed_ms = (uint32)timetick_get_elapsed(rr_plc_data_ptr->x2g_srch_start_sclks, T_MSEC);

  /* if there's a discrepancy (with some tolerance) assume that pre-emption has occurred */
  if ((elapsed_ms + RR_G2X_SRCH_TIMEOUT_TOLERANCE) < rr_plc_data_ptr->x2g_search_timer_val)
  {
    /* the search has completed earlier than expected, determine how much time is remaining */
    rr_plc_data_ptr->x2g_update_timer_val = rr_plc_data_ptr->x2g_search_timer_val - elapsed_ms;
    was_preempted = rr_multi_sim_mode_enabled();
    MSG_GERAN_HIGH_3_G("Search pre-empted in substate %d after %dms, resume GL1=%d", rr_plc_data_ptr->x2g_state, elapsed_ms, was_preempted);
  }
  else
  {
    was_preempted = FALSE;
  }

  return(was_preempted);
}
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
/*!
 * \brief Called when the timer RR_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER expires.
 *
 * \param partial_plmn_list_ptr (in)
 * \param gas_id (in)
 */
static void rr_plc_handle_partial_results_timer_expiry(
  sys_detailed_plmn_list_s_type *partial_plmn_list_ptr,
  const gas_id_t gas_id
)
{
  uint32 i;
  sys_detailed_plmn_list_s_type *available_plmn_list_ptr;
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr;
  rr_plc_data_t           *rr_plc_data_ptr;

#ifdef PERLUTF
  #error code not present
#endif // PERLUTF

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);

  if (rr_plmn_list_req_info_ptr->search_timer_value.MorePLMNTimer > 0)
  {
    rr_timer_start(
      RR_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER,
      CONVERT_SECONDS_TO_MILLISECONDS(
        rr_plmn_list_req_info_ptr->search_timer_value.MorePLMNTimer
      ),
      gas_id
    );

    MSG_GERAN_HIGH_1_G(
      "Partial results timer %d secs (MorePLMNTimer)",
      rr_plmn_list_req_info_ptr->search_timer_value.MorePLMNTimer
    );
  }

  available_plmn_list_ptr = GPRS_MEM_CALLOC(sizeof(sys_detailed_plmn_list_s_type));

  for (i = 0; (i < rr_plc_data_ptr->rat_search_count) && (i < RR_RAT_MAX); ++i)
  {
    if (rr_plc_data_ptr->rat_search_info[i].search_done)
    {
      rr_plc_append_plmn_list(
        available_plmn_list_ptr,
        &rr_plc_data_ptr->rat_search_info[i].available_plmn_list
      );
    }
  }

  // partial_plmn_list_ptr may be NULL if the PLMN list on the current RAT is empty
  if (partial_plmn_list_ptr != NULL)
  {
    rr_plc_append_plmn_list(
      available_plmn_list_ptr,
      partial_plmn_list_ptr
    );
  }

#ifdef PERLUTF
  #error code not present
#endif // PERLUTF

  rr_send_plmn_list_cnf(
    available_plmn_list_ptr,     // available_plmn_list_ptr
    MM_AS_SEARCH_PARTIAL,        // status
    rr_plmn_list_req_info_ptr,   // plmn_list_req
    gas_id                       // gas_id
  );

  GPRS_MEM_FREE(available_plmn_list_ptr);
}
#endif // FEATURE_MPLMN_PARTIAL_UPDATING

/**
  @brief This function implements a state machine that controls the PLMN list
  search procedure performed on GSM.
 */
static rr_event_T rr_plc_gsm_control(
  rr_event_T event,
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  rr_candidate_cell_db_T  *candidate_db_ptr;
  rr_available_plmn_db_T  *available_plmn_db_ptr;
  rr_event_T               new_event    = EV_NO_EVENT;
  rr_event_T               output_event = EV_NO_EVENT;
  byte                     message_id   = 0;
  byte                     message_set  = 0;
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr;
  rr_plc_data_t           *rr_plc_data_ptr;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);

  /* extract message_set and message_id (and check null message ptr) */
  event = rr_extract_message_header_details(
    event,
    message_ptr,
    &message_set,
    &message_id
  );

  /* establish pointers to the candidate cell and available PLMN databases */
  candidate_db_ptr      = rr_get_candidate_cell_db_ptr(gas_id);
  available_plmn_db_ptr = rr_get_available_plmn_db_ptr(gas_id);

  while (event != EV_NO_EVENT)
  {
    if (event == EV_INPUT_MESSAGE && message_ptr == NULL)
    {
      event = EV_NO_EVENT;
      continue;
    }

    switch (rr_plc_data_ptr->gsm_state)
    {
      /***************************************
       * PLMN search INACTIVE
       ***************************************/
      case RR_PLC_GSM_STATE_NULL:
      {
        if (event == EV_PLMN_SEARCH_START)
        {
          rr_pscan_setup(gas_id);

          /* if acq db scan is required, but there is no arfcn in db,
          finish searching GSM just like the way that there is no power scan result */
          if ((rr_plmn_list_req_info_ptr->scan_scope == SYS_SCAN_SCOPE_ACQ_DB) &&
              (rr_num_of_arfcn_in_acq_db(rr_plc_band_pref(gas_id),gas_id) == 0))
          {
            MSG_GERAN_HIGH_0_G("No freqs found in acq db");

            output_event = EV_PLMN_SEARCH_COMPLETE;
            rr_plc_data_ptr->gsm_state = RR_PLC_GSM_STATE_NULL;
          }
          else
          {
            /* setup PSCAN band_pref and foreground/background flags */
            if (rr_plc_search_in_foreground(gas_id))
            {
              // Ensure PS is blocked before starting a foreground pscan
              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                NO_DEL,     // delete_pdu
                gas_id      // gas_id
              );

              rr_plc_data_ptr->gsm_state = RR_PLC_GSM_STATE_BLOCKING_PS;
            }
            else
            {
              rr_pscan_send_start_ind(RR_SEARCH_BACKGROUND, rr_plc_band_pref(gas_id), gas_id);

              rr_plc_data_ptr->gsm_state = RR_PLC_GSM_STATE_PSCAN;
            }
          }
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event),
            rr_plc_gsm_state_name(rr_plc_data_ptr->gsm_state));
        }
        break;
      } /* RR_PLC_GSM_STATE_NULL */

      /***************************************
       * PS is being blocked before a foreground pscan
       ***************************************/
      case RR_PLC_GSM_STATE_BLOCKING_PS:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message_ptr == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_PS_OMSG:
                  {
                    switch (message_ptr->rr.ps.omsg.header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        if (rr_plc_data_ptr->abort)
                        {
                          output_event = EV_PLMN_SEARCH_COMPLETE;
                          rr_plc_data_ptr->gsm_state = RR_PLC_GSM_STATE_NULL;
                        }
                        else
                        {
                          rr_pscan_send_start_ind(RR_SEARCH_NORMAL, rr_plc_band_pref(gas_id), gas_id);
                          rr_plc_data_ptr->gsm_state = RR_PLC_GSM_STATE_PSCAN;
                        }
                        break;
                      } /* RR_PS_OMSG_NO_PS_ACCESS_CNF */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_gsm_state_name(rr_plc_data_ptr->gsm_state));
                      }
                    }
                    break;
                  } /* RR_PS_OMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_gsm_state_name(rr_plc_data_ptr->gsm_state));
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_gsm_state_name(rr_plc_data_ptr->gsm_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          /********************************************
           * commands from outside of rr_plmn_list_control()
           ********************************************/
          case EV_PLMN_SEARCH_ABORT:
          {
            rr_plc_data_ptr->abort = TRUE;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          /********************************************
           * by default:
           ********************************************/
          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_gsm_state_name(rr_plc_data_ptr->gsm_state));
          }
        }
        break;
      } /* RR_PLC_GSM_STATE_BLOCKING_PS */

      /***************************************
       * POWER SCAN in progress
       ***************************************/
      case RR_PLC_GSM_STATE_PSCAN:
      {
        switch(event)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message_ptr == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_PSCAN_OMSG:
                  {
                    switch (message_ptr->rr.pscan.omsg.header.imh.message_id)
                    {
                      case RR_PSCAN_OMSG_FINISHED_IND:
                      {
                        word num_freqs;

                        (void) rr_pscan_process_power_scan_results(RR_NORMAL_SEARCH_ORDER, gas_id);

                        num_freqs = rr_candidate_db_get_num_frequencies(candidate_db_ptr);

                        if (num_freqs != 0)
                        {
                          sys_band_mask_type filter_band_mask = rr_plc_band_pref(gas_id);

                          MSG_GERAN_HIGH_1_G("Num %d freqs found during PSCAN", num_freqs);
                          rr_decode_bcch_list_setup(gas_id);

                          if (rr_plc_search_in_foreground(gas_id))
                          {
                            (void) rr_candidate_db_send_decode_bcch_start_ind(
                              RR_SEARCH_NORMAL,    // bcch_decode_type
                              FALSE,               // si3_only
                              filter_band_mask,    // filter_band_mask
                              gas_id               // gas_id
                            );
                          }
                          else
                          {
                            (void) rr_candidate_db_send_decode_bcch_start_ind(
                              RR_SEARCH_BACKGROUND,   // bcch_decode_type
                              FALSE,                  // si3_only
                              filter_band_mask,       // filter_band_mask
                              gas_id                  // gas_id
                            );
                          }
                          rr_plc_data_ptr->gsm_state = RR_PLC_GSM_STATE_BCCH_LIST_DECODE;
                        }
                        else
                        {
                          MSG_GERAN_HIGH_0_G("No freqs found during PSCAN");
                          output_event = EV_PLMN_SEARCH_COMPLETE;
                          rr_plc_data_ptr->gsm_state = RR_PLC_GSM_STATE_NULL;
                        }
                        break;
                      } /* RR_PSCAN_OMSG_FINISHED_IND */

                      case RR_PSCAN_OMSG_ABORT_CNF:
                      {
                        output_event = EV_PLMN_SEARCH_COMPLETE;
                        rr_plc_data_ptr->gsm_state = RR_PLC_GSM_STATE_NULL;
                        break;
                      } /* RR_PSCAN_OMSG_ABORT_CNF */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_gsm_state_name(rr_plc_data_ptr->gsm_state));
                      }
                    }
                    break;
                  } /* RR_PSCAN_OMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_gsm_state_name(rr_plc_data_ptr->gsm_state));
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_RR_L1:
              {
                rr_pscan_control(event, message_ptr, gas_id);
                break;
              } /* MS_RR_L1 */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_gsm_state_name(rr_plc_data_ptr->gsm_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          /********************************************
           * commands from outside of rr_plmn_list_control()
           ********************************************/
          case EV_PLMN_SEARCH_ABORT:
          {
            rr_pscan_send_abort_req(gas_id);
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          /********************************************
           * by default:
           ********************************************/
          default:
          {
            rr_pscan_control(event, message_ptr, gas_id);
          }
        }
        break;
      } /* RR_PLC_GSM_STATE_PSCAN */

      /***************************************
       * Decode BCCH LIST in progress
       ***************************************/
      case RR_PLC_GSM_STATE_BCCH_LIST_DECODE:
      {
        switch(event)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message_ptr == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_DECODE_BCCH_OMSG:
                  {
                    switch (message_ptr->rr.decode_bcch.omsg.header.imh.message_id)
                    {
                      /********************************************
                       * outputs from rr_decode_bcch_list_control()
                       ********************************************/
                      case RR_DECODE_BCCH_OMSG_FINISHED_IND:
                      {
                        rr_plc_construct_plmn_list(
                          available_plmn_db_ptr,
                          &rr_plc_data_ptr->rat_search_info_current->available_plmn_list,
                          (rr_plmn_list_req_info_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)?
                           NO_CHECK:CHECK_FORBIDDEN_LAI_LIST,
                          gas_id
                        );

#ifdef FEATURE_LTE
                        rr_plc_populate_detected_frequency_list(
                          &rr_plc_data_ptr->rat_search_info_current->detected_frequency_list,
                          &message_ptr->rr.decode_bcch.omsg.decode_bcch_finished_ind.detected_frequency_list
                        );
#endif /* FEATURE_LTE */

                        output_event = EV_PLMN_SEARCH_COMPLETE;
                        rr_plc_data_ptr->gsm_state = RR_PLC_GSM_STATE_NULL;
                        break;
                      } /* RR_DECODE_BCCH_OMSG_FINISHED_IND */

                      case RR_DECODE_BCCH_OMSG_ABORT_CNF:
                      {
                        rr_plc_construct_plmn_list(
                          available_plmn_db_ptr,
                          &rr_plc_data_ptr->rat_search_info_current->available_plmn_list,
                          (rr_plmn_list_req_info_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)?
                           NO_CHECK:CHECK_FORBIDDEN_LAI_LIST,
                          gas_id
                        );

                        output_event = EV_PLMN_SEARCH_COMPLETE;
                        rr_plc_data_ptr->gsm_state = RR_PLC_GSM_STATE_NULL;
                        break;
                      } /* RR_DECODE_BCCH_OMSG_ABORT_CNF */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_gsm_state_name(rr_plc_data_ptr->gsm_state));
                      }
                    }
                    break;
                  } /* RR_DECODE_BCCH_OMSG */

                  case RR_DECODE_BCCH_IMSG:
                  {
                    rr_decode_bcch_list_control(event, message_ptr, gas_id);
                    break;
                  } /* RR_DECODE_BCCH_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_gsm_state_name(rr_plc_data_ptr->gsm_state));
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_RR_L1:
              {
                rr_decode_bcch_list_control(event, message_ptr, gas_id);
                break;
              } /* MS_RR_L1 */

              case MS_TIMER:
              {
                rr_decode_bcch_list_control(event, message_ptr, gas_id);
                break;
              } /* MS_TIMER */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_gsm_state_name(rr_plc_data_ptr->gsm_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          {
            // The candidate cell SI has already been passed to rr_decode_bcch_list_control
            // see rr_store_system_info() [rr_sys_info.c]

            /* check if the search should stop since a new SI arrives */
            if (rr_plc_plmn_search_should_stop(available_plmn_db_ptr, gas_id))
            {
              MSG_GERAN_HIGH_0_G("Requested PLMN found, stop search!");
              rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
              rr_plc_data_ptr->rat_search_info_current->hplmn_found = TRUE;
              rr_decode_bcch_send_abort_req(gas_id);
            }
            break;
          } /* RR_EV_CANDIDATE_CELL_SI_RECEIVED */

          /********************************************
           * commands from outside of rr_plmn_list_control()
           ********************************************/
          case EV_PLMN_SEARCH_ABORT:
          {
            rr_decode_bcch_send_abort_req(gas_id);
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          /********************************************
           * by default:
           ********************************************/
          default:
          {
            rr_decode_bcch_list_control(event, message_ptr, gas_id);
          }
        }
        break;
      } /* RR_PLC_GSM_STATE_BCCH_LIST_DECODE */

      default:
      {
        if (geran_get_nv_recovery_restart_enabled(gas_id))
        {
          MSG_GERAN_ERROR_1_G("Unexpected state: %d, triggering recovery", rr_plc_data_ptr->state);
          geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
        }
        else
        {
          ERR_GERAN_FATAL_1_G("Unexpected state: %d,", rr_plc_data_ptr->state);
        }
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((event != new_event) && (new_event != EV_NO_EVENT)) ||
        (rr_plc_data_ptr->old_gsm_state != rr_plc_data_ptr->gsm_state))
    {
      rr_store_trace_msg_buf(
        RR_PLC_GSM_SM,
        new_event,
        (byte) rr_plc_data_ptr->gsm_state,
        message_ptr,
        gas_id
      );
    }
#endif

    if (new_event != EV_NO_EVENT)
    {
      event = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event = EV_NO_EVENT;
    }

    if (rr_plc_data_ptr->old_gsm_state != rr_plc_data_ptr->gsm_state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_plc_gsm_control",
        rr_plc_data_ptr->old_gsm_state,
        rr_plc_data_ptr->gsm_state,
        rr_plc_gsm_state_name,
        gas_id
      );

      rr_plc_data_ptr->old_gsm_state = rr_plc_data_ptr->gsm_state;
    }
  }

  return output_event;
}


#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

/**
  @brief This function implements a state machine to control PLMN list search
  procedure on GSM RAT when the active RAT is not GSM.
*/
static rr_event_T rr_plc_x2g_control(
  rr_event_T event,
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  rr_available_plmn_db_T    *available_plmn_db_ptr;
  rr_event_T                 new_event    = EV_NO_EVENT;
  rr_event_T                 output_event = EV_NO_EVENT;
  byte                       message_id   = 0;
  byte                       message_set  = 0;
  rr_PLMN_list_req_info_T   *rr_plmn_list_req_info_ptr;
  rr_plc_data_t             *rr_plc_data_ptr;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);

  /* extract message_set and message_id (and check null message ptr) */
  event = rr_extract_message_header_details(
    event,
    message_ptr,
    &message_set,
    &message_id
  );

  /* establish pointer to the available PLMN database */
  available_plmn_db_ptr = rr_get_available_plmn_db_ptr(gas_id);

  /**
   * State-independent processing
   */
  switch (event)
  {
    case EV_INPUT_MESSAGE:
    {
      switch (message_ptr->message_header.message_set)
      {
        case MS_MSGR_RR:
        {
          switch (message_ptr->rr_msgr_msg.message.hdr.id)
          {
#ifdef FEATURE_LTE
#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
            case GERAN_GRR_LTE_GET_PLMN_PRTL_RESULTS_REQ:
            {
              // Send results so far to LTE-RRC
              rr_plc_send_geran_grr_lte_irat_plmn_srch_rsp_partial(gas_id);

              // No further processing required
              event = EV_NO_EVENT;
              break;
            }
#endif /* FEATURE_MPLMN_PARTIAL_UPDATING */
#endif /* FEATURE_LTE */

            default: ;
          }
          break;
        }

        default: ;
      }
      break;
    }

    default: ;
  }

  /**
   * State-dependent processing
   */
  while (event != EV_NO_EVENT)
  {
    switch (rr_plc_data_ptr->x2g_state)
    {
      case RR_PLC_X2G_STATE_NULL:
      {
        if (event == EV_PLMN_SEARCH_START)
        {
          /* band_pref is NOT passed by WCDMA or TDSCDMA RRC, that data is
           * set by rr_control in functions like rr_start_wtog_plmn_search
           */
          if (rr_plc_band_pref(gas_id) == SYS_BAND_MASK_EMPTY)
          {
            MSG_GERAN_ERROR_0_G("X2G PLMN List Search: No GSM bands supported!");

            /* As GL1 is not being started at all in this case (as no search is
            required) we set the required GL1 stop reason to whatever rr_mode.c
            has recorded as the last GL1 stop reason used.  This ensures that
            rr_mode.c will not detect a change in required GL1 stop reason, so
            will not attempt to restart GL1 in order to stop it with the
            new stop reason.  This avoids a situation where GL1 attempts to
            access the firmware after RR has sent the PLMN list search response
            to the originating RAT, which results in a crash in firmware. */
            rr_l1_gsm_stop_reason_set(rr_mode_get_l1_stop_reason(gas_id), gas_id);

            output_event = EV_PLMN_SEARCH_COMPLETE;
          }
          /* if acq db scan is required, but there is no frequency in db
          just finish the plmn search directly without searching  */
          else if (rr_plmn_list_req_info_ptr->scan_scope == SYS_SCAN_SCOPE_ACQ_DB)
          {
            if (rr_num_of_arfcn_in_acq_db(rr_plc_band_pref(gas_id),gas_id) == 0)
            {
              MSG_GERAN_ERROR_0_G("X2G PLMN List acq db Search: No frequency in acq db!");

              rr_l1_gsm_stop_reason_set(rr_mode_get_l1_stop_reason(gas_id), gas_id);

              output_event = EV_PLMN_SEARCH_COMPLETE;
            }
            else
            {
              new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE_FOR_BPLMN, message_ptr, gas_id);
              rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_ACTIVATING_L1;
            }
          }
          else
          {
            new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE_FOR_BPLMN, message_ptr, gas_id);
            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_ACTIVATING_L1;
          }
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
        }
        break;
      } /* RR_PLC_X2G_STATE_NULL */

      case RR_PLC_X2G_STATE_NULL_SUSPENDED:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_ptr->message_header.message_set)
            {
#ifdef FEATURE_WCDMA
              case MS_RRC_RR:
              {
                switch (message_ptr->message_header.message_id)
                {
                  case RR_INTERRAT_PLMN_SRCH_REQ:
                  {
                    MSG_GERAN_MED_0("X2G PLMN List Search: Resuming from NULL");

                    new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE_FOR_BPLMN, message_ptr,gas_id);
                    rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_ACTIVATING_L1;
                    break;
                  } /* RR_INTERRAT_PLMN_SRCH_REQ */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_RRC_RR */
#endif /* FEATURE_WCDMA */

              case MS_MSGR_RR:
              {
                switch (message_ptr->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case GERAN_GRR_LTE_PLMN_SRCH_REQ:
                  {
                    MSG_GERAN_MED_0("X2G PLMN List Search: Resuming from NULL");

                    new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE_FOR_BPLMN, message_ptr, gas_id);
                    rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_ACTIVATING_L1;
                    break;
                  } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */
#endif /* FEATURE_LTE */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_MSGR_RR */

#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch (message_ptr->message_header.message_id)
                {
                  case TDSRR_INTERRAT_PLMN_SRCH_REQ:
                  {
                    MSG_GERAN_MED_0_G("X2G PLMN List Search: Resuming from NULL");

                    new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE_FOR_BPLMN, message_ptr, gas_id);
                    rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_ACTIVATING_L1;
                    break;
                  } /* TDSRR_INTERRAT_PLMN_SRCH_REQ */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              }/* MS_TDSRRC_RR */
#endif

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          {
            MSG_GERAN_MED_0_G("X2G PLMN List Search: Aborting");

            /* Ensure that L1 stop reason is set appropriately.  This will trigger
            an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
            rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

            new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
          }
          break;
        }
        break;
      } /* RR_PLC_X2G_STATE_NULL_SUSPENDED */

      case RR_PLC_X2G_STATE_ACTIVATING_L1:
      {
        switch (event)
        {
          case EV_GSM_MODE_ACTIVATED:
          {
            MSG_GERAN_MED_0_G("X2G PLMN List Search: GL1 activated, starting power scan");
            rr_pscan_setup(gas_id);

            rr_pscan_send_start_ind(
              RR_SEARCH_NORMAL,
              rr_plc_pscan_band_pref(gas_id),
              gas_id
            );

            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_PSCAN;
            break;
          } /* EV_GSM_MODE_ACTIVATED */

          case EV_PLMN_SEARCH_ABORT:
          {
            MSG_GERAN_MED_0_G("X2G PLMN List Search: Aborting");

            /* Ensure that L1 stop reason is set appropriately.  This will trigger
            an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
            rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

            new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          case EV_PLMN_SEARCH_SUSPEND:
          {
            MSG_GERAN_HIGH_0_G("Suspending X2G BPLMN search for RRC");
            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_ACTIVATING_L1_SUSPENDED_BY_RRC;
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
          }
        }
        break;
      } /* RR_PLC_X2G_STATE_ACTIVATING_L1 */

      case RR_PLC_X2G_STATE_ACTIVATING_L1_SUSPENDED_BY_RRC:
      {
        switch (event)
        {
          case EV_GSM_MODE_ACTIVATED:
          {
            new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1_SUSPENDED_BY_RRC;
            break;
          } /* EV_GSM_MODE_ACTIVATED */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
          }
        }
        break;
      } /* RR_PLC_X2G_STATE_ACTIVATING_L1_SUSPENDED_BY_RRC */

      case RR_PLC_X2G_STATE_DEACTIVATING_L1_SUSPENDED_BY_RRC:
      {
        switch (event)
        {
          case EV_GSM_MODE_DEACTIVATED:
          {
            switch (rr_plc_data_ptr->search_type)
            {
#ifdef FEATURE_GSM_TDS
              case RR_PLC_TTOG_SEARCH:
              {
                rr_send_tds_interrat_plmn_search_cnf(FALSE, FALSE, NULL, gas_id);
                break;
              } /* RR_PLC_TTOG_SEARCH */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
              case RR_PLC_WTOG_SEARCH:
              {
                 rr_send_interrat_plmn_search_cnf(FALSE, FALSE, NULL, gas_id);
                break;
              } /* RR_PLC_WTOG_SEARCH */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
              case RR_PLC_LTOG_SEARCH:
              {
                lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
                errno_enum_type          result;

                plmn_srch_rsp_ptr = (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));
                RR_NULL_CHECK_FATAL(plmn_srch_rsp_ptr);

                msgr_init_hdr(
                  &plmn_srch_rsp_ptr->msg_hdr,
                  MSGR_GERAN_GRR,
                  GERAN_GRR_LTE_PLMN_SRCH_RSP
                );

                plmn_srch_rsp_ptr->srch_done = FALSE;

                rr_invalidate_csg_info(&plmn_srch_rsp_ptr->available_plmn_list);

                result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);
                if (result != E_SUCCESS)
                {
                  MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                }

                GPRS_MEM_FREE(plmn_srch_rsp_ptr);
                break;
              } /* RR_PLC_LTOG_SEARCH */
#endif /* FEATURE_LTE */

              default:
              {
                MSG_GERAN_ERROR_1_G("Unknown search_type: %d", rr_plc_data_ptr->search_type);
              }
            }

            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_NULL_SUSPENDED;
            break;
          } /* EV_GSM_MODE_DEACTIVATED */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
          }
        }
        break;
      } /* RR_PLC_X2G_STATE_DEACTIVATING_L1_SUSPENDED_BY_RRC */

      case RR_PLC_X2G_STATE_PSCAN:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_SUSPEND_GSM_MODE_IND:
                  {
                    rr_plc_x2g_state_e next_state;

                    MSG_GERAN_MED_0_G("X2G PLMN List Search: GL1 suspended");

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                    if (rr_ms_x2g_search_preempted(gas_id))
                    {
                      /* GL1 can be resumed immediately */
                      next_state = RR_PLC_X2G_STATE_PSCAN_RESUMING;
                      rr_send_mph_resume_gsm_mode_for_bplmn_req(rr_plc_data_ptr->x2g_update_timer_val, gas_id);
                    }
                    else
#endif /*  FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
                    {
                      next_state = RR_PLC_X2G_STATE_PSCAN_SUSPENDED;
                      rr_l1_send_mph_rat_change_ind(gas_id);
                    }

                    switch (rr_plc_data_ptr->search_type)
                    {
#ifdef FEATURE_GSM_TDS
                      case RR_PLC_TTOG_SEARCH:
                      {
                        if (RR_PLC_X2G_STATE_PSCAN_SUSPENDED == next_state)
                        {
                          rr_send_tds_interrat_plmn_search_cnf(FALSE, FALSE, NULL, gas_id);
                        }
                        break;
                      } /* RR_PLC_TTOG_SEARCH */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
                      case RR_PLC_WTOG_SEARCH:
                      {
                        if (RR_PLC_X2G_STATE_PSCAN_SUSPENDED == next_state)
                        {
                          rr_send_interrat_plmn_search_cnf(FALSE, FALSE, NULL, gas_id);
                        }
                        break;
                      } /* RR_PLC_WTOG_SEARCH */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
                      case RR_PLC_LTOG_SEARCH:
                      {
                        if (RR_PLC_X2G_STATE_PSCAN_SUSPENDED == next_state)
                        {
                          lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
                          errno_enum_type          result;

                          plmn_srch_rsp_ptr = (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));
                          RR_NULL_CHECK_FATAL(plmn_srch_rsp_ptr);

                          msgr_init_hdr(
                            &plmn_srch_rsp_ptr->msg_hdr,
                            MSGR_GERAN_GRR,
                            GERAN_GRR_LTE_PLMN_SRCH_RSP
                          );

                          plmn_srch_rsp_ptr->srch_done = FALSE;

                          rr_invalidate_csg_info(&plmn_srch_rsp_ptr->available_plmn_list);

                          result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);
                          if (result != E_SUCCESS)
                          {
                            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                          }

                          GPRS_MEM_FREE(plmn_srch_rsp_ptr);
                        }
                        break;
                      } /* RR_PLC_LTOG_SEARCH */
#endif /* FEATURE_LTE */

                      default:
                      {
                        MSG_GERAN_ERROR_1_G("Unknown search_type: %d", rr_plc_data_ptr->search_type);
                      }
                    }
                    /* transition to the appropriate next state */
                    rr_plc_data_ptr->x2g_state = next_state;
                    break;
                  } /* MPH_SUSPEND_GSM_MODE_IND */

                  case MPH_POWER_SCAN_CNF:
                  {
                    rr_pscan_control(event, message_ptr, gas_id);
                    break;
                  } /* MPH_POWER_SCAN_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_RR_L1 */

              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_PSCAN_OMSG:
                  {
                    switch (message_ptr->rr.pscan.omsg.header.imh.message_id)
                    {
                       case RR_PSCAN_OMSG_FINISHED_IND:
                       {
                         if (rr_pscan_process_power_scan_results(RR_NORMAL_SEARCH_ORDER, gas_id) != 0)
                         {
                           if (message_ptr->rr.pscan.omsg.pscan_finished_ind.l1_suspended)
                           {
                             MSG_GERAN_MED_0_G(
                               "pscan finished and L1 suspended, BCCH list decode on next gap"
                             );

                             rr_plc_data_ptr->pending_decode_bcch_list_req = TRUE;
                           }
                           else
                           {
                             sys_band_mask_type filter_band_mask = rr_plc_band_pref(gas_id);

                             MSG_GERAN_MED_0_G(
                               "pscan finished, start BCCH list decodes"
                             );

                             rr_decode_bcch_list_setup(gas_id);

                             if (rr_plc_search_in_foreground(gas_id) == TRUE)
                             {
                               (void) rr_candidate_db_send_decode_bcch_start_ind(
                                 RR_SEARCH_NORMAL,   // bcch_decode_type
                                 FALSE,              // si3_only
                                 filter_band_mask,   // filter_band_mask
                                 gas_id              // gas_id
                               );
                             }
                             else
                             {
                               (void) rr_candidate_db_send_decode_bcch_start_ind(
                                 RR_SEARCH_BACKGROUND,   // bcch_decode_type
                                 FALSE,                  // si3_only
                                 filter_band_mask,       // filter_band_mask
                                 gas_id                  // gas_id
                               );
                             }

                             rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_BCCH_LIST_DECODE;
                           }
                         }
                         else
                         {
                           MSG_GERAN_MED_0_G("X2G PLMN List Search: GL1 completed power scan, no channels found, search done");

                           /* Ensure that L1 stop reason is set appropriately.  This will trigger
                           an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
                           rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

                           new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
                           rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
                         }
                         break;
                       } /* RR_PSCAN_OMSG_FINISHED_IND */
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          {
            MSG_GERAN_MED_0_G("X2G PLMN List Search: Aborting");

            /* we don't need a proper abort PSCAN here, reset */
            rr_pscan_control(EV_RESET_SOFTWARE, NULL, gas_id);

            /* Ensure that L1 stop reason is set appropriately.  This will trigger
            an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
            rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

            new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          case EV_PLMN_SEARCH_SUSPEND:
          {
            MSG_GERAN_HIGH_0_G("X2G PLMN List Search: Suspending");
            rr_send_mph_suspend_gsm_mode_req(gas_id);
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
          }
        }
        break;
      } /* RR_PLC_X2G_STATE_PSCAN */

      case RR_PLC_X2G_STATE_PSCAN_SUSPENDED:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch(message_id)
                {
                  case TDSRR_INTERRAT_PLMN_SRCH_REQ:
                  {
                    MSG_GERAN_MED_0_G("T2G PLMN List Search: Resuming power scan");

                    rr_plc_setup(
                      RR_PLC_TTOG_SEARCH,                           // search_type
                      message_ptr->tdsrr_plmn_srch_req.timer_val,   // timer_val
                      gas_id                                        // gas_id
                    );

                    rr_send_mph_resume_gsm_mode_for_bplmn_req(
                      message_ptr->tdsrr_plmn_srch_req.timer_val,   // timer_val
                      gas_id                                        // gas_id
                    );

                    rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_PSCAN_RESUMING;
                    break;
                  } /* RR_INTERRAT_PLMN_SRCH_REQ */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_WCDMA
              case MS_RRC_RR:
              {
                switch(message_id)
                {
                  case RR_INTERRAT_PLMN_SRCH_REQ:
                  {
                    MSG_GERAN_MED_0_G("W2G PLMN List Search: Resuming power scan");

                    rr_plc_setup(
                      RR_PLC_WTOG_SEARCH,                                 // search_type
                      message_ptr->rr_interrat_plmn_srch_req.timer_val,   // timer_val
                      gas_id                                              // gas_id
                    );

                    rr_send_mph_resume_gsm_mode_for_bplmn_req(
                      message_ptr->rr_interrat_plmn_srch_req.timer_val,   // timer_val
                      gas_id                                              // gas_id
                    );

                    rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_PSCAN_RESUMING;
                    break;
                  } /* RR_INTERRAT_PLMN_SRCH_REQ */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_RRC_RR */
#endif /* FEATURE_WCDMA */

              case MS_MSGR_RR:
              {
                switch (message_ptr->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case GERAN_GRR_LTE_PLMN_SRCH_REQ:
                  {
                    MSG_GERAN_MED_0_G("L2G PLMN List Search: Resuming power scan");

                    rr_plc_setup(
                      RR_PLC_LTOG_SEARCH,                                                // search_type
                      message_ptr->
                        rr_msgr_msg.message.geran_grr_msg_req.plmn_srch_req.timer_val,   // timer_val
                      gas_id                                                             // gas_id
                    );

                    rr_send_mph_resume_gsm_mode_for_bplmn_req(
                      message_ptr->
                        rr_msgr_msg.message.geran_grr_msg_req.plmn_srch_req.timer_val,   // timer_val
                      gas_id                                                             // gas_id
                    );

                    rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_PSCAN_RESUMING;
                    break;
                  } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */
#endif /* FEATURE_LTE */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_MSGR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          {
            MSG_GERAN_MED_0_G("X2G PLMN List Search: Aborting");

            /* we don't need a proper PSCAN abort here, reset it */
            rr_pscan_control(EV_RESET_SOFTWARE, NULL, gas_id);

            /* Ensure that L1 stop reason is set appropriately.  This will trigger
            an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
            rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

            new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
          }
        }
        break;
      } /* RR_PLC_X2G_STATE_PSCAN_SUSPENDED */

      case RR_PLC_X2G_STATE_PSCAN_RESUMING:
      {
        switch(event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch(message_id)
                {
                  case MPH_RESUME_GSM_MODE_FOR_BPLMN_CNF:
                  {
                    if (rr_plc_data_ptr->pending_decode_bcch_list_req)
                    {
                      sys_band_mask_type filter_band_mask = rr_plc_band_pref(gas_id);

                      MSG_GERAN_MED_0_G("X2G PLMN List Search: GL1 resumed, starting BCCH list decode");

                      /* PSCAN finished in the last drx period, but there had no enough time
                       * to continue decoding BCCH, do it now
                       */
                      rr_decode_bcch_list_setup(gas_id);

                      if (rr_plc_search_in_foreground(gas_id))
                      {
                        (void) rr_candidate_db_send_decode_bcch_start_ind(
                          RR_SEARCH_NORMAL,    // bcch_decode_type
                          FALSE,               // si3_only
                          filter_band_mask,    // filter_band_mask
                          gas_id               // gas_id
                        );
                      }
                      else
                      {
                        (void) rr_candidate_db_send_decode_bcch_start_ind(
                          RR_SEARCH_BACKGROUND,   // bcch_decode_type
                          FALSE,                  // si3_only
                          filter_band_mask,       // filter_band_mask
                          gas_id                  // gas_id
                        );
                      }
                      rr_plc_data_ptr->pending_decode_bcch_list_req = FALSE;
                      rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_BCCH_LIST_DECODE;
                    }
                    else
                    {
                      MSG_GERAN_MED_0_G("X2G PLMN List Search: GL1 resumed, power scan ongoing");
                      /* RR needs do nothing, L1 will continue its unfinished task */
                      rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_PSCAN;
                    }
                    break;
                  } /* MPH_RESUME_GSM_MODE_FOR_BPLMN_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_RR_L1 */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
              }
            }
            break;
          }  /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          {
            MSG_GERAN_MED_0_G("X2G PLMN List Search: Aborting");

            /* we don't need a proper abort PSCAN here, reset */
            rr_pscan_control(EV_RESET_SOFTWARE, NULL, gas_id);

            /* Ensure that L1 stop reason is set appropriately.  This will trigger
            an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
            rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

            new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
          }
        }
        break;
      } /* RR_PLC_X2G_STATE_PSCAN_RESUMING */

      case RR_PLC_X2G_STATE_BCCH_LIST_DECODE:
      {
        switch(event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_SUSPEND_GSM_MODE_IND:
                  {
                    rr_plc_x2g_state_e next_state;

                    MSG_GERAN_MED_0_G("X2G PLMN List Search: GL1 suspended");

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                    if (rr_ms_x2g_search_preempted(gas_id))
                    {
                      /* GL1 can be resumed immediately */
                      next_state = RR_PLC_X2G_STATE_BCCH_LIST_DECODE_RESUMING;
                      rr_send_mph_resume_gsm_mode_for_bplmn_req(rr_plc_data_ptr->x2g_update_timer_val, gas_id);
                    }
                    else
#endif /*  FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
                    {
                      next_state = RR_PLC_X2G_STATE_BCCH_LIST_DECODE_SUSPENDED;
                      rr_l1_send_mph_rat_change_ind(gas_id);
                    }

                    switch (rr_plc_data_ptr->search_type)
                    {
#ifdef FEATURE_GSM_TDS
                      case RR_PLC_TTOG_SEARCH:
                      {
                        if (RR_PLC_X2G_STATE_BCCH_LIST_DECODE_SUSPENDED == next_state)
                        {
                          rr_send_tds_interrat_plmn_search_cnf(FALSE, FALSE, NULL, gas_id);
                        }
                        break;
                      } /* RR_PLC_WTOG_SEARCH */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
                      case RR_PLC_WTOG_SEARCH:
                      {
                        if (RR_PLC_X2G_STATE_BCCH_LIST_DECODE_SUSPENDED == next_state)
                        {
                          rr_send_interrat_plmn_search_cnf(FALSE, FALSE, NULL, gas_id);
                        }
                        break;
                      } /* RR_PLC_WTOG_SEARCH */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
                      case RR_PLC_LTOG_SEARCH:
                      {
                        if (RR_PLC_X2G_STATE_BCCH_LIST_DECODE_SUSPENDED == next_state)
                        {
                          lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
                          errno_enum_type          result;

                          plmn_srch_rsp_ptr = (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));
                          RR_NULL_CHECK_FATAL(plmn_srch_rsp_ptr);

                          msgr_init_hdr(
                            &plmn_srch_rsp_ptr->msg_hdr,
                            MSGR_GERAN_GRR,
                            GERAN_GRR_LTE_PLMN_SRCH_RSP
                          );

                          plmn_srch_rsp_ptr->srch_done = FALSE;

                          rr_invalidate_csg_info(&plmn_srch_rsp_ptr->available_plmn_list);

                          result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);
                          if (result != E_SUCCESS)
                          {
                            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                          }

                          GPRS_MEM_FREE(plmn_srch_rsp_ptr);
                        }
                      } /* RR_PLC_LTOG_SEARCH */
                      break;
#endif /* FEATURE_LTE */

                      default:
                      {
                        MSG_GERAN_ERROR_1_G("Unknown search_type: %d", rr_plc_data_ptr->search_type);
                      }
                    }
                    rr_plc_data_ptr->x2g_state = next_state;
                    break;
                  } /* MPH_SUSPEND_GSM_MODE_IND */

                  /* It is tricky here that MPH_DECODE_BCCH_LIST_CNF can't be
                   * directly input to decode_bcch_list_control -
                   * because if suspending in decode_bcch_list_cnf == TRUE,
                   * RR can't send another MPH_DECODE_BCCH_LIST_REQ immediately
                   * after receiving MPH_DECODE_BCCH_LIST_CNF to L1
                   */
                  case MPH_DECODE_BCCH_LIST_CNF:
                  {
                    if (message_ptr->mph_decode_bcch_list_cnf.suspending)
                    {
                      MSG_GERAN_MED_0_G("X2G PLMN List Search: GL1 indicates suspend pending, await MPH_SUSPEND_GSM_MODE_IND");
                      rr_plc_data_ptr->pending_decode_bcch_list_req = TRUE;
                    }
                    else
                    {
                      rr_decode_bcch_list_control(event, message_ptr, gas_id);
                    }
                    break;
                  } /* MPH_DECODE_BCCH_LIST_CNF */

                  /* No action necessary on receipt of MPH_FCCH_SCH_DECODE_IND. */
                  case MPH_FCCH_SCH_DECODE_IND:
                  {
                    break;
                  } /* MPH_FCCH_SCH_DECODE_IND */

                  case MPH_DECODE_BCCH_LIST_FAILURE_IND:
                  {
                    rr_decode_bcch_list_control(event, message_ptr, gas_id);
                    break;
                  } /* MPH_DECODE_BCCH_LIST_FAILURE_IND */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_RR_L1 */

              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_DECODE_BCCH_OMSG:
                  {
                    switch (message_ptr->rr.decode_bcch.omsg.header.imh.message_id)
                    {
                      case RR_DECODE_BCCH_OMSG_FINISHED_IND:
                      {
                        MSG_GERAN_MED_0_G("X2G PLMN List Search: BCCH list decode finished, deactivating GL1");

#ifdef FEATURE_LTE
                        rr_plc_populate_detected_frequency_list(
                          &rr_plc_data_ptr->rat_search_info_current->detected_frequency_list,
                          &message_ptr->rr.decode_bcch.omsg.decode_bcch_finished_ind.detected_frequency_list
                        );
#endif /* FEATURE_LTE */

                        /* Ensure that L1 stop reason is set appropriately.  This will trigger
                        an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
                        rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

                        new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
                        rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
                        break;
                      } /* RR_DECODE_BCCH_OMSG_FINISHED_IND */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message_ptr,
                          rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                      }
                    }
                    break;
                  } /* RR_DECODE_BCCH_OMSG */

                  case RR_DECODE_BCCH_IMSG:
                  {
                    rr_decode_bcch_list_control(event, message_ptr, gas_id);
                    break;
                  } /* RR_DECODE_BCCH_IMSG */

                  default:
                  {
                    rr_decode_bcch_list_control(event, message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                rr_decode_bcch_list_control(event, message_ptr, gas_id);
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          {
            // The candidate cell SI has already been passed to rr_decode_bcch_list_control
            // see rr_store_system_info() [rr_sys_info.c]

            if (rr_plc_plmn_search_should_stop(available_plmn_db_ptr, gas_id))
            {
              MSG_GERAN_MED_0_G("X2G PLMN List Search: Requested PLMN found, terminating search");
              /* requested PLMN found, search stop */
              rr_decode_bcch_list_control(EV_RESET_SOFTWARE, NULL, gas_id);

              /* Ensure that L1 stop reason is set appropriately.  This will trigger
              an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
              rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

              new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL,gas_id);
              rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
            }
            break;
          } /* RR_EV_CANDIDATE_CELL_SI_RECEIVED */

          case EV_PLMN_SEARCH_ABORT:
          {
            MSG_GERAN_MED_0_G("X2G PLMN List Search: Aborting");

            /* we don't need a proper abort DECODE_BCCH here, reset */
            rr_decode_bcch_list_control(EV_RESET_SOFTWARE, NULL, gas_id);

            /* Ensure that L1 stop reason is set appropriately.  This will trigger
            an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
            rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

            new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          case EV_PLMN_SEARCH_SUSPEND:
          {
            MSG_GERAN_HIGH_0_G("X2G PLMN List Search: Suspending");
            rr_send_mph_suspend_gsm_mode_req(gas_id);
            break;
          }

          default:
          {
            rr_decode_bcch_list_control(event, message_ptr, gas_id);
          }
        }
        break;
      } /* RR_PLC_X2G_STATE_BCCH_LIST_DECODE */

      case RR_PLC_X2G_STATE_BCCH_LIST_DECODE_SUSPENDED:
      {
        switch(event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch (message_id)
                {
                  case TDSRR_INTERRAT_PLMN_SRCH_REQ:
                  {
                    MSG_GERAN_MED_0_G("T2G PLMN List Search: Resuming BCCH list decode");

                    rr_plc_setup(
                      RR_PLC_TTOG_SEARCH,                           // search_type
                      message_ptr->tdsrr_plmn_srch_req.timer_val,   // timer_val
                      gas_id                                        // gas_id
                    );

                    rr_send_mph_resume_gsm_mode_for_bplmn_req(
                      message_ptr->tdsrr_plmn_srch_req.timer_val,   // timer_val
                      gas_id                                        // gas_id
                    );

                    rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_BCCH_LIST_DECODE_RESUMING;
                    break;
                  } /* TDSRR_INTERRAT_PLMN_SRCH_REQ */

                  case TDSRR_INTERRAT_PLMN_SRCH_ABORT_REQ:
                  {
                    MSG_GERAN_HIGH_0_G("Abort TTOG BPLMN search...");
                    new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
                    rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
                    break;
                  } /* RR_INTERRAT_PLMN_SRCH_ABORT_REQ */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
              case MS_RRC_RR:
              {
                switch (message_id)
                {
                  case RR_INTERRAT_PLMN_SRCH_REQ:
                  {
                    MSG_GERAN_MED_0_G("W2G PLMN List Search: Resuming BCCH list decode");

                    rr_plc_setup(
                      RR_PLC_WTOG_SEARCH,                                 // search_type
                      message_ptr->rr_interrat_plmn_srch_req.timer_val,   // timer_val
                      gas_id                                              // gas_id
                    );

                    rr_send_mph_resume_gsm_mode_for_bplmn_req(
                      message_ptr->rr_interrat_plmn_srch_req.timer_val,   // timer_val
                      gas_id                                              // gas_id
                    );

                    rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_BCCH_LIST_DECODE_RESUMING;
                    break;
                  } /* RR_INTERRAT_PLMN_SRCH_REQ */

                  case RR_INTERRAT_PLMN_SRCH_ABORT_REQ:
                  {
                    MSG_GERAN_HIGH_0_G("Abort WTOG BPLMN search...");

                    /* Ensure that L1 stop reason is set appropriately.  This will trigger
                    an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
                    rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

                    new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
                    rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
                    break;
                  } /* RR_INTERRAT_PLMN_SRCH_ABORT_REQ */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_RRC_RR */
#endif /* FEATURE_WCDMA */

              case MS_MSGR_RR:
              {
                switch (message_ptr->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case GERAN_GRR_LTE_PLMN_SRCH_REQ:
                  {
                    MSG_GERAN_MED_0_G("L2G PLMN List Search: Resuming BCCH list decode");

                    rr_plc_setup(
                      RR_PLC_LTOG_SEARCH,                                                // search_type
                      message_ptr->
                        rr_msgr_msg.message.geran_grr_msg_req.plmn_srch_req.timer_val,   // timer_val
                      gas_id                                                             // gas_id
                    );

                    rr_send_mph_resume_gsm_mode_for_bplmn_req(
                      message_ptr->
                        rr_msgr_msg.message.geran_grr_msg_req.plmn_srch_req.timer_val,   // timer_val
                      gas_id                                                             // gas_id
                    );

                    rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_BCCH_LIST_DECODE_RESUMING;
                    break;
                  } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */
#endif /* FEATURE_LTE */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_MSGR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          {
            MSG_GERAN_MED_0_G("X2G PLMN List Search: Aborting");

            /* we don't need a proper abort DECODE_BCCH here, reset */
            rr_decode_bcch_list_control(EV_RESET_SOFTWARE, NULL, gas_id);

            /* Ensure that L1 stop reason is set appropriately.  This will trigger
            an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
            rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

            new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
          }
        }
        break;
      } /* RR_PLC_X2G_STATE_BCCH_LIST_DECODE_SUSPENDED */

      case RR_PLC_X2G_STATE_BCCH_LIST_DECODE_RESUMING:
      {
        switch(event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_RESUME_GSM_MODE_FOR_BPLMN_CNF:
                  {
                    MSG_GERAN_MED_1_G("X2G PLMN List Search: GL1 resumed, pending decode BCCH list req: %d",
                            rr_plc_data_ptr->pending_decode_bcch_list_req);

                    if (rr_plc_data_ptr->pending_decode_bcch_list_req)
                    {
                      /* simulate a mph_decode_bcch_list_cnf, to drive decode_bcch control
                       * move forward
                       */
                      mph_decode_bcch_list_cnf_T decode_cnf;
                      decode_cnf.message_header.message_set = (byte) MS_RR_L1;
                      decode_cnf.message_header.message_id  = (byte) MPH_DECODE_BCCH_LIST_CNF;
                      decode_cnf.suspending = FALSE;
                      decode_cnf.retry_required = FALSE;
                      rr_decode_bcch_list_control(
                        EV_INPUT_MESSAGE,
                        (rr_cmd_bdy_type *) &decode_cnf,
                        gas_id
                      );
                      rr_plc_data_ptr->pending_decode_bcch_list_req = FALSE;
                    }
                    rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_BCCH_LIST_DECODE;
                    break;
                  } /* MPH_RESUME_GSM_MODE_FOR_BPLMN_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_RR_L1 */

              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_DECODE_BCCH_OMSG:
                  {
                    switch (message_ptr->rr.decode_bcch.omsg.header.imh.message_id)
                    {
                      /********************************************
                       * outputs from rr_decode_bcch_list_control()
                       ********************************************/
                      case RR_DECODE_BCCH_OMSG_FINISHED_IND:
                      {
                        MSG_GERAN_MED_0_G("X2G PLMN List Search: BCCH list decode finished, deactivating GL1");

                        /* Ensure that L1 stop reason is set appropriately.  This will trigger
                        an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
                        rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

                        new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
                        rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
                        break;
                      } /* RR_DECODE_BCCH_OMSG_FINISHED_IND */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message_ptr,
                          rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                      }
                    }
                    break;
                  } /* RR_DECODE_BCCH_OMSG */

                  case RR_DECODE_BCCH_IMSG:
                  {
                    rr_decode_bcch_list_control(event, message_ptr, gas_id);
                    break;
                  } /*RR_DECODE_BCCH_IMSG*/

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                rr_decode_bcch_list_control(event, message_ptr, gas_id);
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          {
            MSG_GERAN_MED_0_G("X2G PLMN List Search: Aborting");

            /* we don't need a proper abort DECODE_BCCH here, reset */
            rr_decode_bcch_list_control(EV_RESET_SOFTWARE, NULL, gas_id);

            /* Ensure that L1 stop reason is set appropriately.  This will trigger
            an MPH_RAT_CHANGE_IND from rr_control() when it goes to INACTIVE state. */
            rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

            new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_DEACTIVATING_L1;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
          }
        }
        break;
      } /* RR_PLC_X2G_STATE_BCCH_LIST_DECODE_RESUMING */

      case RR_PLC_X2G_STATE_DEACTIVATING_L1:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_DECODE_BCCH_LIST_CNF:
                  case MPH_DECODE_BCCH_LIST_FAILURE_IND:
                  case MPH_SUSPEND_GSM_MODE_IND:
                  {
                    /* DECODE_BCCH_LIST_CNF or MPH_DECODE_BCCH_LIST_FAILURE_IND can be received
                     * before STOP_GSM_MODE_REQ arriving L1, if HPLMN found
                     */

                    /* MPH_SUSPEND_GSM_MODE_IND can be received if RR sends a MPH_STOP_GSM_MODE_REQ
                     * when L1 is suspending
                     */
                    break;
                  } /* MPH_DECODE_BCCH_LIST_CNF ... */

                  default:
                  {
                    break;
                  }
                }
                break;
              } /* MS_RR_L1 */

              default:
              {
                break;
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          {
            /* ignore futher events caused by decoding other cells */
            break;
          } /* RR_EV_CANDIDATE_CELL_SI_RECEIVED */

          case EV_GSM_MODE_DEACTIVATED:
          {
            ASSERT(rr_plc_data_ptr->rat_search_info_current->rat == RR_RAT_GSM);

            rr_plc_construct_plmn_list(
              available_plmn_db_ptr,
              &rr_plc_data_ptr->rat_search_info_current->available_plmn_list,
              (rr_plmn_list_req_info_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)?
                NO_CHECK:CHECK_FORBIDDEN_LAI_LIST,
              gas_id
            );

            rr_plc_data_ptr->x2g_state = RR_PLC_X2G_STATE_NULL;
            output_event = EV_PLMN_SEARCH_COMPLETE;
            break;
          } /* EV_GSM_MODE_DEACTIVATED */

          case EV_PLMN_SEARCH_ABORT:
          {
            /* Abort has already been recorded in main state machine, no action necessary
            here as RR has already sent MPH_STOP_GSM_MODE_REQ to L1.  Just wait for the
            response, abort confirmation will be sent by parent state machine. */
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_x2g_state_name(rr_plc_data_ptr->x2g_state));
          }
        }
        break;
      } /* RR_PLC_X2G_STATE_DEACTIVATING_L1 */

      default:
      {
        if (geran_get_nv_recovery_restart_enabled(gas_id))
        {
          MSG_GERAN_ERROR_1_G("Unexpected state: %d, triggering panic reset!", rr_plc_data_ptr->x2g_state);
          geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
        }
        else
        {
          ERR_GERAN_FATAL_1_G("Unexpected state: %d,", rr_plc_data_ptr->x2g_state);
        }
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((event != new_event) && (new_event != EV_NO_EVENT)) ||
        (rr_plc_data_ptr->old_x2g_state != rr_plc_data_ptr->x2g_state))
    {
      rr_store_trace_msg_buf(
        RR_PLC_X2G_SM,
        new_event,
        (byte) rr_plc_data_ptr->x2g_state,
        message_ptr,
        gas_id
      );
    }
#endif

    if (new_event != EV_NO_EVENT)
    {
      event = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event = EV_NO_EVENT;
    }

    if (rr_plc_data_ptr->old_x2g_state != rr_plc_data_ptr->x2g_state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_plc_x2g_control",
        rr_plc_data_ptr->old_x2g_state,
        rr_plc_data_ptr->x2g_state,
        rr_plc_x2g_state_name,
        gas_id
      );

      rr_plc_data_ptr->old_x2g_state = rr_plc_data_ptr->x2g_state;
    }
  }

  return output_event;
}

#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

#ifdef FEATURE_LTE

void rr_plc_send_lte_rrc_g_plmn_srch_req(
  uint32 timer_value,
  detected_frequency_list_type * detected_frequency_list,
  const gas_id_t gas_id
)
{
  rr_PLMN_list_req_info_T  * rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  lte_irat_plmn_srch_req_s   plmn_srch_req;
  errno_enum_type            result;
  uint32                     i;

  memset(&plmn_srch_req, 0, sizeof(lte_irat_plmn_srch_req_s));

  msgr_init_hdr(
    &plmn_srch_req.msg_hdr,
    MSGR_GERAN_GRR,
    LTE_RRC_G_PLMN_SRCH_REQ
  );

  plmn_srch_req.network_select_mode = rr_plmn_list_req_info_ptr->network_selection_mode;
  plmn_srch_req.search_type = rr_plmn_list_req_info_ptr->service_search;

  for (i = 0;
       i < rr_plmn_list_req_info_ptr->plmn_list.length &&
       i < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH;
       ++i)
  {
    if (rr_plmn_list_req_info_ptr->plmn_list.info[i].rat == SYS_RAT_LTE_RADIO_ACCESS)
    {
      plmn_srch_req.plmn[plmn_srch_req.num_plmns++] = rr_plmn_list_req_info_ptr->plmn_list.info[i].plmn;
    }
  }

  plmn_srch_req.band_pref = rr_rat_pri_list_info_band_pref(
    rr_plc_get_rat_pri_list_info(gas_id),
    SYS_SYS_MODE_LTE
  );

  plmn_srch_req.timer_val = timer_value;
  plmn_srch_req.scan_scope = rr_plmn_list_req_info_ptr->scan_scope;

  if (detected_frequency_list != NULL)
  {
    plmn_srch_req.detected_frequency_list = *detected_frequency_list;
  }

  MSG_GERAN_MED_1_G("G2L PLMN List Search: GL1 deactivated, sending LTE_RRC_G_PLMN_SRCH_REQ (%d ms)", plmn_srch_req.timer_val);

  result = rr_msgr_send(&plmn_srch_req.msg_hdr, sizeof(plmn_srch_req), gas_id);
  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}

#endif /* FEATURE_LTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)

static void rr_plc_g2x_send_abort_req(
  rr_rat_e rat,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_plc_data_t *rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  switch (rat)
  {
#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      MSG_GERAN_HIGH_0_G("Aborting G2T PLMN List Search");

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
      // Obtain the partial results from T
      rr_plc_data_ptr->x_rat_partial_results_ptr = GPRS_MEM_CALLOC(sizeof(sys_detailed_plmn_list_s_type));

      if (rr_plc_data_ptr->x_rat_partial_results_ptr != NULL)
      {
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
        tdsrrc_get_inter_rat_bplmn_prtl_result_sub_id(
          rr_plc_data_ptr->x_rat_partial_results_ptr,  // available_plmn_list
          geran_map_gas_id_to_nas_id(gas_id)           // as_id
        );
#else
        tdsrrc_get_inter_rat_bplmn_prtl_result(
          rr_plc_data_ptr->x_rat_partial_results_ptr   // available_plmn_list
        );
#endif // FEATURE_TDS_SUB_ID_INTERFACE
      }
#endif // FEATURE_MPLMN_PARTIAL_UPDATING

      rr_send_tds_interrat_plmn_search_abort_req(gas_id);
      (void)rr_timer_start(RR_IRAT_PROC_TIMER,RR_IRAT_PROC_TIMEOUT, gas_id);

      break;
    } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      MSG_GERAN_HIGH_0_G("Aborting G2W PLMN List Search");

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
      // Obtain the partial results from W
      rr_plc_data_ptr->x_rat_partial_results_ptr = GPRS_MEM_CALLOC(sizeof(sys_detailed_plmn_list_s_type));

      if (rr_plc_data_ptr->x_rat_partial_results_ptr != NULL)
      {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
        wrrc_get_inter_rat_bplmn_prtl_result_sub_id(
          rr_plc_data_ptr->x_rat_partial_results_ptr,  // available_plmn_list
          geran_map_gas_id_to_nas_id(gas_id)           // as_id
        );
#else
        wrrc_get_inter_rat_bplmn_prtl_result(
          rr_plc_data_ptr->x_rat_partial_results_ptr   // available_plmn_list
        );
#endif // FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM
      }
#endif // FEATURE_MPLMN_PARTIAL_UPDATING

      rr_send_interrat_plmn_search_abort_req(gas_id);
      (void)rr_timer_start(RR_IRAT_PROC_TIMER,RR_IRAT_PROC_TIMEOUT, gas_id);

      break;
    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
    case RR_RAT_LTE:
    {
      lte_irat_abort_plmn_srch_req_s abort_plmn_srch_req;
      errno_enum_type                result;

      MSG_GERAN_HIGH_0_G("Aborting G2L PLMN List Search");
      memset(&abort_plmn_srch_req, 0, sizeof(lte_irat_abort_plmn_srch_req_s));

      msgr_init_hdr(
        &abort_plmn_srch_req.msg_hdr,
        MSGR_GERAN_GRR,
        LTE_RRC_G_ABORT_PLMN_SRCH_REQ
      );

      result = rr_msgr_send(&abort_plmn_srch_req.msg_hdr, sizeof(abort_plmn_srch_req), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      else
      {
        (void)rr_timer_start(RR_IRAT_PROC_TIMER,RR_IRAT_PROC_TIMEOUT, gas_id);
      }
      break;
    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rr_plc_data_ptr->rat_search_info_current->rat);
    }
  }
}

/**
  @brief This function implements a sub-state machine to perform IRAT PLMN
  searches when GSM is the active RAT.

  @param event An input even or command for this state machine.
  @param message_ptr An optional input message for this state machine.
  @return An event to the calling state machine.
*/
static rr_event_T rr_plc_g2x_control(
  rr_event_T event,
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  byte message_id  = 0;
  byte message_set = 0;
  rr_event_T  new_event = EV_NO_EVENT;
  rr_event_T  output_event = EV_NO_EVENT;
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr;
  rr_plc_data_t           *rr_plc_data_ptr;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);

  /* extract message_set and message_id (and check null message ptr) */
  event = rr_extract_message_header_details(
    event,
    message_ptr,
    &message_set,
    &message_id
  );

  while (event != EV_NO_EVENT)
  {
    switch (rr_plc_data_ptr->g2x_state)
    {
      case RR_PLC_G2X_STATE_NULL:
      {
        switch (event)
        {
          case EV_PLMN_SEARCH_START:
          {
            switch (rr_plc_data_ptr->rat_search_info_current->rat)
            {
#ifdef FEATURE_GSM_TDS
              case RR_RAT_UTRAN_TDD:
              {
                sys_band_cap_u_type band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_TDS, gas_id);
                MSG_GERAN_HIGH_0_G("Starting IRAT PLMN search on TDSCDMA");
                rr_update_band_preference_for_tdsrrc(band_cap_u.chgwt_band_cap, gas_id);
                break;
              } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
              case RR_RAT_WCDMA:
              {
                sys_band_cap_u_type band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_WCDMA, gas_id);
                MSG_GERAN_HIGH_0_G("Starting IRAT PLMN search on WCDMA");
                rr_update_band_preference_for_rrc(band_cap_u.chgwt_band_cap, gas_id);
                break;
              } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
              case RR_RAT_LTE:
              {
                MSG_GERAN_HIGH_0_G("Starting IRAT PLMN search on LTE");
                break;
              }
#endif /* FEATURE_LTE */

              default:
              {
                MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rr_plc_data_ptr->rat_search_info_current->rat);
              }
            }

            if (rr_plc_data_ptr->search_type == RR_PLC_FOREGROUND_SEARCH)
            {
              // Ensure PS is blocked before starting a foreground pscan
              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                NO_DEL,     // delete_pdu
                gas_id      // gas_id
              );

              rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BLOCKING_PS;
            }
            else
            {
              /* for background search, wait time_ind from L1 at the 1st step */
              rr_send_mph_interrat_list_search_time_req(
                RR_L1_LIST_SRCH_TIME_REQ_START,
                rr_plc_data_ptr->rat_search_info_current->rat,
                gas_id);
              rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_WAIT_FOR_L1_TIME_IND;
            }
            break;
          } /* EV_PLMN_SEARCH_START */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
          }
        }
        break;
      } /* RR_PLC_G2X_STATE_NULL */


      /*****************************************************************
       * FOREGROUND search sub-states
       *****************************************************************/
      case RR_PLC_G2X_STATE_BLOCKING_PS:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            if (message_ptr == NULL)
            {
              MSG_GERAN_ERROR_0_G("message == NULL");
              break;
            }

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_PS_OMSG:
                  {
                    switch (message_ptr->rr.ps.omsg.header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        if (rr_plc_data_ptr->abort)
                        {
                          output_event = EV_PLMN_SEARCH_COMPLETE;
                          rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_NULL;
                        }
                        else
                        {
                          /* for foreground search, deactivates L1 then do search on WCDMA */
                          new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
                          rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_DEACTIVATING_L1;
                        }
                        break;
                      } /* RR_PS_OMSG_NO_PS_ACCESS_CNF */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                      }
                    }
                    break;
                  } /* RR_PS_OMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          /********************************************
           * commands from outside of rr_plmn_list_control()
           ********************************************/
          case EV_PLMN_SEARCH_ABORT:
          {
            rr_plc_data_ptr->abort = TRUE;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          /********************************************
           * by default:
           ********************************************/
          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
          }
        }
        break;
      } /* RR_PLC_GSM_STATE_BLOCKING_PS */

      case RR_PLC_G2X_STATE_FOREGROUND_DEACTIVATING_L1:
      {
        switch (event)
        {
          case EV_GSM_MODE_DEACTIVATED:
          {
#ifdef FEATURE_LTE
            detected_frequency_list_type detected_frequency_list;

            rr_plc_detected_frequency_list_populate(&detected_frequency_list, gas_id);
#endif /* FEATURE_LTE */

            switch (rr_plc_data_ptr->rat_search_info_current->rat)
            {
#ifdef FEATURE_GSM_TDS
              case RR_RAT_UTRAN_TDD:
              {
                MSG_GERAN_MED_0_G("G2T PLMN List Search: GL1 deactivated, sending TDSRRC_INTERRAT_PLMN_SRCH_REQ");
                rr_send_tds_interrat_plmn_search_req(
                  &rr_plmn_list_req_info_ptr->plmn_list,
                  rr_plmn_list_req_info_ptr->network_selection_mode,
                  RR_MAX_PLMN_LIST_SEARCH_TIME,
                  FALSE,
                  gas_id
                );

                MSG_GERAN_HIGH_1_G(
                  "Max search timer %d secs (given)",
                  CONVERT_MILLISECONDS_TO_SECONDS(RR_MAX_PLMN_LIST_SEARCH_TIME)
                );

                break;
              }
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
              case RR_RAT_WCDMA:
              {
                MSG_GERAN_MED_0_G("G2W PLMN List Search: GL1 deactivated, sending RRC_INTERRAT_PLMN_SRCH_REQ");
#ifdef FEATURE_LTE
                rr_send_interrat_plmn_search_req(
                  &rr_plmn_list_req_info_ptr->plmn_list,
                  rr_plmn_list_req_info_ptr->network_selection_mode,
                  RR_MAX_PLMN_LIST_SEARCH_TIME,
                  FALSE,
                  &detected_frequency_list,
                  gas_id
                );

                MSG_GERAN_HIGH_1_G(
                  "Max search timer %d secs (given)",
                  CONVERT_MILLISECONDS_TO_SECONDS(RR_MAX_PLMN_LIST_SEARCH_TIME)
                );

#else
                rr_send_interrat_plmn_search_req(
                  &rr_plmn_list_req_info_ptr->plmn_list,
                  rr_plmn_list_req_info_ptr->network_selection_mode,
                  RR_MAX_PLMN_LIST_SEARCH_TIME,
                  FALSE,
                  gas_id
                );

                MSG_GERAN_HIGH_1_G(
                  "Max search timer %d secs (given)",
                  CONVERT_MILLISECONDS_TO_SECONDS(RR_MAX_PLMN_LIST_SEARCH_TIME)
                );

#endif /* FEATURE_LTE */
                break;
              }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
              case RR_RAT_LTE:
              {
                MSG_GERAN_MED_0_G("G2L PLMN List Search: GL1 deactivated, sending LTE_RRC_G_PLMN_SRCH_REQ");
                rr_plc_send_lte_rrc_g_plmn_srch_req(RR_MAX_PLMN_LIST_SEARCH_TIME, &detected_frequency_list, gas_id);

                MSG_GERAN_HIGH_1_G(
                  "Max search timer %d secs (given)",
                  CONVERT_MILLISECONDS_TO_SECONDS(RR_MAX_PLMN_LIST_SEARCH_TIME)
                );
                break;
              }
#endif /* FEATURE_LTE */

              default:
              {
                MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rr_plc_data_ptr->rat_search_info_current->rat);
              }
            }
            rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_IN_PROGRESS;
            break;
          } /* EV_GSM_MODE_DEACTIVATED */

          case EV_PLMN_SEARCH_ABORT:
          {
            MSG_GERAN_HIGH_0_G("G2X PLMN List Search: abort received, activating GL1");
            rr_timer_cancel(RR_PLMN_LIST_SEARCH_GUARD_TIMER, gas_id);
            new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
            rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_ACTIVATING_L1;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
          }
        }
        break;
      } /* RR_PLC_G2X_STATE_FOREGROUND_DEACTIVATING_L1 */

      case RR_PLC_G2X_STATE_FOREGROUND_IN_PROGRESS:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch (message_id)
                {
                  case TDSRRC_INTERRAT_PLMN_SRCH_CNF:
                  {
                    tdsrrc_interrat_plmn_srch_cnf_type *plmn_srch_cnf =
                      (tdsrrc_interrat_plmn_srch_cnf_type *)message_ptr;
                    MSG_GERAN_MED_2_G("G2T PLMN List Search: received TDSRRC_INTERRAT_PLMN_SRCH_CNF, srch_done=%d, hplmn_found=%d",
                      plmn_srch_cnf->srch_done, plmn_srch_cnf->hplmn_found);

                    if(plmn_srch_cnf->srch_done)
                    {
                      rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
                      rr_plc_data_ptr->rat_search_info_current->available_plmn_list = plmn_srch_cnf->available_plmn_list;

                      if (plmn_srch_cnf->hplmn_found)
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN found");
                        rr_plc_data_ptr->rat_search_info_current->hplmn_found = TRUE;
                      }
                      else
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN not found");
                      }

                      new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                      rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_ACTIVATING_L1;
                    }
                    else 
                    {
                      MSG_GERAN_HIGH_1_G("G2T PLMN List Search: aborting due to srch_done= %d received",plmn_srch_cnf->srch_done);
                      rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);
                      rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_ABORTING;
                    }
                    break;
                  } /* TDSRRC_INTERRAT_PLMN_SRCH_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
              case MS_RRC_RR:
              {
                switch (message_id)
                {
                  case RRC_INTERRAT_PLMN_SRCH_CNF:
                  {
                    rrc_interrat_plmn_srch_cnf_type *plmn_srch_cnf =
                      (rrc_interrat_plmn_srch_cnf_type *)message_ptr;
                    MSG_GERAN_MED_2_G("G2W PLMN List Search: received RRC_INTERRAT_PLMN_SRCH_CNF, srch_done=%d, hplmn_found=%d",
                      plmn_srch_cnf->srch_done, plmn_srch_cnf->hplmn_found);
                    
                    if(plmn_srch_cnf->srch_done)
                    {
                      rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
                      rr_plc_data_ptr->rat_search_info_current->available_plmn_list = plmn_srch_cnf->available_plmn_list;

                      if (plmn_srch_cnf->hplmn_found)
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN found");
                        rr_plc_data_ptr->rat_search_info_current->hplmn_found = TRUE;
                      }
                      else
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN not found");
                      }

                      new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                      rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_ACTIVATING_L1;
                    }
                    else 
                    {
                      MSG_GERAN_HIGH_1_G("G2W PLMN List Search: aborting due to srch_done= %d received",plmn_srch_cnf->srch_done);
                      rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);
                      rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_ABORTING;
                    }
                    break;
                  } /* RRC_INTERRAT_PLMN_SRCH_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_RRC_RR */
#endif /* FEATURE_WCDMA */

              case MS_MSGR_RR:
              {
                switch (message_ptr->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case LTE_RRC_G_PLMN_SRCH_RSP:
                  {
                    lte_irat_plmn_srch_rsp_s * plmn_srch_rsp = &message_ptr->rr_msgr_msg.message.lte_irat_plmn_srch_rsp;

                    MSG_GERAN_MED_2_G("G2L PLMN List Search: received LTE_RRC_G_PLMN_SRCH_CNF, srch_done=%d, hplmn_found=%d",
                             plmn_srch_rsp->srch_done,
                             plmn_srch_rsp->hplmn_found);
                    if(plmn_srch_rsp->srch_done)
                    {
                      rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
                      rr_plc_data_ptr->rat_search_info_current->available_plmn_list = plmn_srch_rsp->available_plmn_list;
                      rr_plc_data_ptr->rat_search_info_current->detected_frequency_list = plmn_srch_rsp->detected_frequency_list;

                      if (plmn_srch_rsp->hplmn_found)
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN found");
                        rr_plc_data_ptr->rat_search_info_current->hplmn_found = TRUE;
                      }
                      else
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN not found");
                      }

                      new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                      rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_ACTIVATING_L1;
                    }
                    else 
                    {
                      MSG_GERAN_HIGH_1_G("G2L PLMN List Search: aborting due to srch_done= %d received",plmn_srch_rsp->srch_done);
                      rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);
                      rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_ABORTING;
                    }
                    break;
                  } /* LTE_RRC_G_PLMN_SRCH_CNF */
#endif /* FEATURE_LTE */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_MSGR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          {
            rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);
            rr_timer_cancel(RR_PLMN_LIST_SEARCH_GUARD_TIMER, gas_id);
            rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_ABORTING;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
          }
        }
        break;
      } /* RR_PLC_G2X_STATE_FOREGROUND_IN_PROGRESS */

      case RR_PLC_G2X_STATE_FOREGROUND_ACTIVATING_L1:
      {
        switch (event)
        {
          case EV_GSM_MODE_ACTIVATED:
          {
            output_event = EV_PLMN_SEARCH_COMPLETE;
            rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_NULL;
            break;
          } /* EV_GSM_MODE_ACTIVATED */

          case EV_PLMN_SEARCH_ABORT:
          {
            /* Already deactivating L1, no action necessary. */
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
          }
        }
        break;
      } /* RR_PLC_G2X_STATE_FOREGROUND_ACTIVATING_L1 */

      case RR_PLC_G2X_STATE_FOREGROUND_ABORTING:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch (message_id)
                {
                  case TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF:
                  case TDSRRC_INTERRAT_PLMN_SRCH_CNF:
                  {
                    (void)rr_timer_cancel(RR_IRAT_PROC_TIMER,gas_id);
                    new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                    rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_ACTIVATING_L1;
                    break;
                  } /* TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF or TDSRRC_INTERRAT_PLMN_SRCH_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_TDSRRC_RR */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
              case MS_RRC_RR:
              {
                switch (message_id)
                {
                  case RRC_INTERRAT_PLMN_SRCH_ABORT_CNF:
                  case RRC_INTERRAT_PLMN_SRCH_CNF:
                  {
                    (void)rr_timer_cancel(RR_IRAT_PROC_TIMER,gas_id);
                    new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                    rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_ACTIVATING_L1;
                    break;
                  } /* RRC_INTERRAT_PLMN_SRCH_ABORT_CNF or RRC_INTERRAT_PLMN_SRCH_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_RRC_RR */
#endif /* FEATURE_WCDMA */

              case MS_MSGR_RR:
              {
                switch (message_ptr->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case LTE_RRC_G_ABORT_PLMN_SRCH_RSP:
                  {
                    (void)rr_timer_cancel(RR_IRAT_PROC_TIMER,gas_id);
                    new_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                    rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_FOREGROUND_ACTIVATING_L1;
                    break;
                  } /* LTE_RRC_G_ABORT_PLMN_SRCH_RSP */
#endif /* FEATURE_LTE */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_MSGR_RR */


              case MS_TIMER:
              {
                switch (message_ptr->mid_timer_expiry.timer_id)
                {
                  case RR_IRAT_PROC_TIMER:
                  {
                    ERR_FATAL("No response received from RAT: %d", rr_plc_data_ptr->rat_search_info_current->rat, 0, 0);
                  }
                }
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
          }
        }
        break;
      } /* RR_PLC_G2X_STATE_FOREGROUND_ABORTING */

      /*****************************************************************
       * BACKGROUND search sub-states
       *****************************************************************/

      case RR_PLC_G2X_STATE_BACKGROUND_WAIT_FOR_L1_TIME_IND:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_INTERRAT_LIST_SRCH_TIME_IND:
                  {
                    mph_interrat_list_srch_time_ind_T *time_ind =
                      (mph_interrat_list_srch_time_ind_T *)message_ptr;

                    if (time_ind->timer_value >= MIN_REQUIRED_TIME_FOR_IRAT_PLMN_SEARCH)
                    {
                      rr_ps_send_imsg_no_ps_access_req(
                        FALSE,      // suspend_gmm
                        NO_DEL,     // delete_pdu
                        gas_id      // gas_id
                      );

                      rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_SUSPENDING_GPRS;
                    }
                    else
                    {
                      MSG_GERAN_MED_1_G("G2X PLMN List Search: insufficient time search (%d ms), wait for next gap", time_ind->timer_value);
                    }
                    break;
                  } /* MPH_INTERRAT_LIST_SRCH_TIME_IND */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_RR_L1 */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          {
            rr_plc_data_ptr->l1_status = RR_PLC_G2X_L1_STATUS_RESUMED;
            rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);
            rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_ABORTING;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
          }
        }
        break;
      } /* RR_PLC_G2X_STATE_BACKGROUND_WAIT_FOR_L1_TIME_IND */

      case RR_PLC_G2X_STATE_BACKGROUND_SUSPENDING_GPRS:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_PS_OMSG:
                  {
                    switch (message_ptr->rr.header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        if (rr_plc_data_ptr->abort)
                        {
                          rr_plc_data_ptr->l1_status = RR_PLC_G2X_L1_STATUS_RESUMED;

                          rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);
                          rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_ABORTING;
                        }
                        else
                        {
                          MSG_GERAN_HIGH_0_G("GPRS suspended for GTOW plmn search");
                          rr_send_mph_suspend_gsm_mode_req(gas_id);
                          rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_SUSPENDING_L1;
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                      }

                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          {
            rr_plc_data_ptr->l1_status = RR_PLC_G2X_L1_STATUS_RESUMED;
            rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);
            rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_ABORTING;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
          }
        }
        break;
      } /* RR_PLC_G2X_STATE_BACKGROUND_SUSPENDING_GPRS */


      case RR_PLC_G2X_STATE_BACKGROUND_SUSPENDING_L1:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_INTERRAT_LIST_SRCH_TIME_IND:
                  {
                    MSG_GERAN_ERROR_0_G("GL1 may have ignored MPH_SUSPEND_GSM_MODE_REQ for G2X PLMN search");
                    break;
                  }

                  case MPH_SUSPEND_GSM_MODE_CNF:
                  {
                    if (message_ptr->mph.suspend_gsm_mode_cnf.suspended)
                    {
                      if (message_ptr->mph.suspend_gsm_mode_cnf.timer_value >= MIN_REQUIRED_TIME_FOR_IRAT_PLMN_SEARCH)
                      {
#ifdef FEATURE_LTE
                        detected_frequency_list_type detected_frequency_list;

                        rr_plc_detected_frequency_list_populate(&detected_frequency_list, gas_id);
#endif /* FEATURE_LTE */

                        switch (rr_plc_data_ptr->rat_search_info_current->rat)
                        {
#ifdef FEATURE_GSM_TDS
                          case RR_RAT_UTRAN_TDD:
                          {
                            boolean camped_suitably = FALSE;

                            if (rr_is_limited_plmn_selection_mode(gas_id) == FALSE &&
                                rr_cell_selection_is_camped_cell_suitable(gas_id))
                            {
                              camped_suitably = TRUE;
                            }

                            MSG_GERAN_MED_0_G("G2T PLMN List Search: GL1 suspended, sending TDSRRC_INTERRAT_PLMN_SRCH_REQ");

                            rr_send_tds_interrat_plmn_search_req(
                              &rr_plmn_list_req_info_ptr->plmn_list,
                              rr_plmn_list_req_info_ptr->network_selection_mode,
                              message_ptr->mph.suspend_gsm_mode_cnf.timer_value,
                              camped_suitably,
                              gas_id
                            );
                            break;
                          }
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
                          case RR_RAT_WCDMA:
                          {
                            boolean camped_suitably = FALSE;

                            if (rr_is_limited_plmn_selection_mode(gas_id) == FALSE &&
                                rr_cell_selection_is_camped_cell_suitable(gas_id))
                            {
                              camped_suitably = TRUE;
                            }

                            MSG_GERAN_MED_0_G("G2W PLMN List Search: GL1 suspended, sending RRC_INTERRAT_PLMN_SRCH_REQ");
#ifdef FEATURE_LTE
                            rr_send_interrat_plmn_search_req(
                              &rr_plmn_list_req_info_ptr->plmn_list,
                              rr_plmn_list_req_info_ptr->network_selection_mode,
                              message_ptr->mph.suspend_gsm_mode_cnf.timer_value,
                              camped_suitably,
                              &detected_frequency_list,
                              gas_id
                            );
#else
                            rr_send_interrat_plmn_search_req(
                              &rr_plmn_list_req_info_ptr->plmn_list,
                              rr_plmn_list_req_info_ptr->network_selection_mode,
                              message_ptr->mph.suspend_gsm_mode_cnf.timer_value,
                              camped_suitably,
                              gas_id
                            );
#endif /* FEATURE_LTE */
                            break;
                          }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
                          case RR_RAT_LTE:
                          {
                            MSG_GERAN_MED_0_G("G2L PLMN List Search: GL1 suspended, sending LTE_RRC_G_PLMN_SRCH_REQ");
                            rr_plc_send_lte_rrc_g_plmn_srch_req(
                              message_ptr->mph.suspend_gsm_mode_cnf.timer_value,
                              &detected_frequency_list,
                              gas_id
                            );
                            break;
                          }
#endif /* FEATURE_LTE */

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("Unexpected RAT: %d", rr_plc_data_ptr->rat_search_info_current->rat);
                          }
                        }

                        rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_IN_PROGRESS;
                      }
                      else
                      {
                        MSG_GERAN_MED_1_G("Insufficient time %d indicated by L1, resume and try again",
                                message_ptr->mph.suspend_gsm_mode_cnf.timer_value);
                        rr_send_mph_resume_gsm_mode_req(gas_id);
                        rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_RESUMING_L1;
                      }
                    }
                    else
                    {
                      MSG_GERAN_MED_0_G("L1 failed to suspend, try again on next iteration");
                      rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_WAIT_FOR_L1_TIME_IND;
                    }
                    break;
                  } /* MPH_SUSPEND_GSM_MODE_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          {
            rr_plc_data_ptr->l1_status = RR_PLC_G2X_L1_STATUS_SUSPENDING;
            rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);
            rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_ABORTING;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
          }
        }
        break;
      } /* RR_PLC_G2X_STATE_BACKGROUND_SUSPENDING_L1 */

      case RR_PLC_G2X_STATE_BACKGROUND_IN_PROGRESS:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch (message_id)
                {
                  case TDSRRC_INTERRAT_PLMN_SRCH_CNF:
                  {
                    tdsrrc_interrat_plmn_srch_cnf_type *plmn_srch_cnf;
                    plmn_srch_cnf = (tdsrrc_interrat_plmn_srch_cnf_type *)message_ptr;

                    if (plmn_srch_cnf->srch_done)
                    {
                      rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
                      rr_plc_data_ptr->rat_search_info_current->available_plmn_list = plmn_srch_cnf->available_plmn_list;
                      rr_plc_data_ptr->rat_search_info_current->hplmn_found = plmn_srch_cnf->hplmn_found;
                    }

                    MSG_GERAN_MED_2_G("G2T PLMN List Search: received TDSRRC_INTERRAT_PLMN_SRCH_CNF, srch_done=%d, hplmn_found=%d",
                             plmn_srch_cnf->srch_done,
                             plmn_srch_cnf->hplmn_found);

                    rr_send_mph_resume_gsm_mode_req(gas_id);
                    rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_RESUMING_L1;
                    break;
                  } /* RRC_INTERRAT_PLMN_SRCH_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
              case MS_RRC_RR:
              {
                switch (message_id)
                {
                  case RRC_INTERRAT_PLMN_SRCH_CNF:
                  {
                    rrc_interrat_plmn_srch_cnf_type *plmn_srch_cnf;
                    plmn_srch_cnf = (rrc_interrat_plmn_srch_cnf_type *)message_ptr;

                    if (plmn_srch_cnf->srch_done)
                    {
                      rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
                      rr_plc_data_ptr->rat_search_info_current->available_plmn_list = plmn_srch_cnf->available_plmn_list;
                      rr_plc_data_ptr->rat_search_info_current->hplmn_found = plmn_srch_cnf->hplmn_found;
                    }

                    MSG_GERAN_MED_2_G("G2W PLMN List Search: received RRC_INTERRAT_PLMN_SRCH_CNF, srch_done=%d, hplmn_found=%d",
                             plmn_srch_cnf->srch_done,
                             plmn_srch_cnf->hplmn_found);

                    rr_send_mph_resume_gsm_mode_req(gas_id);
                    rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_RESUMING_L1;
                    break;
                  } /* RRC_INTERRAT_PLMN_SRCH_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_RRC_RR */
#endif /* FEATURE_WCDMA */

              case MS_MSGR_RR:
              {
                switch (message_ptr->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case LTE_RRC_G_PLMN_SRCH_RSP:
                  {
                    lte_irat_plmn_srch_rsp_s * plmn_srch_rsp = &message_ptr->rr_msgr_msg.message.lte_irat_plmn_srch_rsp;

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
                    if (plmn_srch_rsp->partial_srch)
                    {
                      if (plmn_srch_rsp->available_plmn_list.length > 0)
                      {
                        rr_plc_handle_partial_results_timer_expiry(
                          &plmn_srch_rsp->available_plmn_list,
                          gas_id
                        );
                      }
                      else
                      {
                        rr_plc_handle_partial_results_timer_expiry(
                          NULL,
                          gas_id
                        );
                      }
                    }
                    else
#endif // FEATURE_MPLMN_PARTIAL_UPDATING
                    {
                      if (plmn_srch_rsp->srch_done)
                      {
                        rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
                        rr_plc_data_ptr->rat_search_info_current->available_plmn_list = plmn_srch_rsp->available_plmn_list;
                        rr_plc_data_ptr->rat_search_info_current->hplmn_found = plmn_srch_rsp->hplmn_found;
                        rr_plc_data_ptr->rat_search_info_current->detected_frequency_list = plmn_srch_rsp->detected_frequency_list;
                      }

                      MSG_GERAN_MED_2_G("G2L PLMN List Search: received LTE_RRC_G_PLMN_SRCH_RSP, srch_done=%d, hplmn_found=%d",
                               plmn_srch_rsp->srch_done,
                               plmn_srch_rsp->hplmn_found);

                      rr_send_mph_resume_gsm_mode_req(gas_id);
                      rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_RESUMING_L1;
                    }
                    break;
                  } /* LTE_RRC_G_PLMN_SRCH_RSP */
#endif /* FEATURE_LTE */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_MSGR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          {
            rr_plc_data_ptr->l1_status = RR_PLC_G2X_L1_STATUS_SUSPENDED;
            rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);
            rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_ABORTING;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
          }
        }
        break;
      } /* RR_PLC_G2X_STATE_BACKGROUND_IN_PROGRESS */

      case RR_PLC_G2X_STATE_BACKGROUND_RESUMING_L1:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_RESUME_GSM_MODE_CNF:
                  {
                    if (rr_plc_data_ptr->ps_service_enable)
                    {
                      MSG_GERAN_ERROR_0_G("TODO Enabling PS access is not correct here");
                      rr_send_grr_mac_ps_access_req(
                        FALSE,     // purge_gmm_signalling
                        gas_id     // gas_id
                      );
                    }

                    if (rr_plc_data_ptr->rat_search_info_current->search_done)
                    {
                      /* terminates the search */
                      rr_send_mph_interrat_list_search_time_req(
                        RR_L1_LIST_SRCH_TIME_REQ_STOP,
                        rr_plc_data_ptr->rat_search_info_current->rat,
                        gas_id
                      );
                      output_event = EV_PLMN_SEARCH_COMPLETE;
                      rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_NULL;
                    }
                    else
                    {
                      /* wait for next L1 time_ind */
                      rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_WAIT_FOR_L1_TIME_IND;
                    }
                    break;
                  } /* MPH_RESUME_GSM_MODE_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_RR_L1 */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          {
            rr_plc_data_ptr->l1_status = RR_PLC_G2X_L1_STATUS_RESUMING;
            rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);
            rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_BACKGROUND_ABORTING;
            break;
          } /* EV_PLMN_SEARCH_ABORT */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
          }
        }
        break;
      } /* RR_PLC_G2X_STATE_BACKGROUND_RESUMING_L1 */

      case RR_PLC_G2X_STATE_BACKGROUND_ABORTING:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch (message_id)
                {
                  case TDSRRC_INTERRAT_PLMN_SRCH_CNF:
                  case TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF:
                  {
                    rr_plc_data_ptr->rrc_abort_cnf_received = TRUE;
                    (void)rr_timer_cancel(RR_IRAT_PROC_TIMER,gas_id);
                    switch (rr_plc_data_ptr->l1_status)
                    {
                      case RR_PLC_G2X_L1_STATUS_SUSPENDING:
                      case RR_PLC_G2X_L1_STATUS_RESUMING:
                        /* wait till L1 comes back */
                        MSG_GERAN_MED_0_G("G2T PLMN List Search: TDSCDMA aborted, wait for GL1 to suspend/resume");
                        break;

                      case RR_PLC_G2X_L1_STATUS_SUSPENDED:
                        MSG_GERAN_MED_0_G("G2T PLMN List Search: TDSCDMA aborted, resume GL1");
                        rr_send_mph_resume_gsm_mode_req(gas_id);
                        rr_plc_data_ptr->l1_status = RR_PLC_G2X_L1_STATUS_RESUMING;
                        break;

                      case RR_PLC_G2X_L1_STATUS_RESUMED:
                        MSG_GERAN_MED_0_G("G2T PLMN List Search: TDSCDMA aborted, GL1 resumed");
                        /* abort complete */
                        rr_send_mph_interrat_list_search_time_req(
                          RR_L1_LIST_SRCH_TIME_REQ_STOP,
                          rr_plc_data_ptr->rat_search_info_current->rat,
                          gas_id
                        );
                        output_event = EV_PLMN_SEARCH_COMPLETE;
                        rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_NULL;
                        break;
                    }
                    break;
                  } /* TDSRRC_INTERRAT_PLMN_SRCH_CNF  TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_TDSRRC_RR */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
              case MS_RRC_RR:
              {
                switch (message_id)
                {
                  case RRC_INTERRAT_PLMN_SRCH_CNF:
                  case RRC_INTERRAT_PLMN_SRCH_ABORT_CNF:
                  {
                    rr_plc_data_ptr->rrc_abort_cnf_received = TRUE;
                    (void)rr_timer_cancel(RR_IRAT_PROC_TIMER,gas_id);
                    switch (rr_plc_data_ptr->l1_status)
                    {
                      case RR_PLC_G2X_L1_STATUS_SUSPENDING:
                      case RR_PLC_G2X_L1_STATUS_RESUMING:
                        /* wait till L1 comes back */
                        MSG_GERAN_MED_0_G("G2W PLMN List Search: WCMDA aborted, wait for GL1 to suspend/resume");
                        break;

                      case RR_PLC_G2X_L1_STATUS_SUSPENDED:
                        MSG_GERAN_MED_0_G("G2W PLMN List Search: WCMDA aborted, resume GL1");
                        rr_send_mph_resume_gsm_mode_req(gas_id);
                        rr_plc_data_ptr->l1_status = RR_PLC_G2X_L1_STATUS_RESUMING;
                        break;

                      case RR_PLC_G2X_L1_STATUS_RESUMED:
                        MSG_GERAN_MED_0_G("G2W PLMN List Search: WCDMA aborted, GL1 resumed");
                        /* abort complete */
                        rr_send_mph_interrat_list_search_time_req(
                          RR_L1_LIST_SRCH_TIME_REQ_STOP,
                          rr_plc_data_ptr->rat_search_info_current->rat,
                          gas_id
                        );
                        output_event = EV_PLMN_SEARCH_COMPLETE;
                        rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_NULL;
                        break;
                    }
                    break;
                  } /* RRC_INTERRAT_PLMN_SRCH_CNF RRC_INTERRAT_PLMN_SRCH_ABORT_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_RRC_RR */
#endif /* FEATURE_WCDMA */

              case MS_MSGR_RR:
              {
                switch (message_ptr->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case LTE_RRC_G_ABORT_PLMN_SRCH_RSP:
                  {
                    rr_plc_data_ptr->rrc_abort_cnf_received = TRUE;                   
                    (void)rr_timer_cancel(RR_IRAT_PROC_TIMER,gas_id);
                    switch (rr_plc_data_ptr->l1_status)
                    {
                      case RR_PLC_G2X_L1_STATUS_SUSPENDING:
                      case RR_PLC_G2X_L1_STATUS_RESUMING:
                        /* wait till L1 comes back */
                        MSG_GERAN_MED_0_G("G2L PLMN List Search: WCMDA aborted, wait for GL1 to suspend/resume");
                        break;

                      case RR_PLC_G2X_L1_STATUS_SUSPENDED:
                        MSG_GERAN_MED_0_G("G2L PLMN List Search: WCMDA aborted, resume GL1");
                        rr_send_mph_resume_gsm_mode_req(gas_id);
                        rr_plc_data_ptr->l1_status = RR_PLC_G2X_L1_STATUS_RESUMING;
                        break;

                      case RR_PLC_G2X_L1_STATUS_RESUMED:
                        MSG_GERAN_MED_0_G("G2L PLMN List Search: WCDMA aborted, GL1 resumed");
                        /* abort complete */
                        rr_send_mph_interrat_list_search_time_req(
                          RR_L1_LIST_SRCH_TIME_REQ_STOP,
                          rr_plc_data_ptr->rat_search_info_current->rat,
                          gas_id
                        );
                        output_event = EV_PLMN_SEARCH_COMPLETE;
                        rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_NULL;
                        break;
                    }
                    break;
                  } /* LTE_RRC_G_ABORT_PLMN_SRCH_RSP */
#endif /* FEATURE_LTE */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_MSGR_RR */

              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_SUSPEND_GSM_MODE_CNF:
                  {
                    if (rr_plc_data_ptr->l1_status == RR_PLC_G2X_L1_STATUS_SUSPENDING)
                    {
                      rr_plc_data_ptr->l1_status = RR_PLC_G2X_L1_STATUS_SUSPENDED;
                      rr_send_mph_resume_gsm_mode_req(gas_id);
                      rr_plc_data_ptr->l1_status = RR_PLC_G2X_L1_STATUS_RESUMING;
                    }
                    else
                    {
                      MSG_GERAN_ERROR_1_G("Received suspend_cnf but L1 wasn't suspending %d",
                        rr_plc_data_ptr->l1_status);
                    }
                    break;
                  } /* MPH_SUSPEND_GSM_MODE_CNF */

                  case MPH_RESUME_GSM_MODE_CNF:
                  {
                    if (rr_plc_data_ptr->l1_status == RR_PLC_G2X_L1_STATUS_RESUMING)
                    {
                      rr_plc_data_ptr->l1_status = RR_PLC_G2X_L1_STATUS_RESUMED;
                      if (rr_plc_data_ptr->rrc_abort_cnf_received)
                      {
                        /* abort complete */
                        rr_send_mph_interrat_list_search_time_req(
                          RR_L1_LIST_SRCH_TIME_REQ_STOP,
                          rr_plc_data_ptr->rat_search_info_current->rat,
                          gas_id
                        );
                        output_event = EV_PLMN_SEARCH_COMPLETE;
                        rr_plc_data_ptr->g2x_state = RR_PLC_G2X_STATE_NULL;
                      }
                    }
                    else
                    {
                      MSG_GERAN_ERROR_1_G("Received resume_cnf but L1 wasn't resuming %d",
                        rr_plc_data_ptr->l1_status);
                    }
                    break;
                  } /* MPH_RESUME_GSM_MODE_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
                  }
                }
                break;
              } /* MS_RR_L1 */

              case MS_TIMER:
              {
                switch (message_ptr->mid_timer_expiry.timer_id)
                {
                  case RR_IRAT_PROC_TIMER:
                  {
                    ERR_FATAL("No response received from RAT: %d", rr_plc_data_ptr->rat_search_info_current->rat, 0, 0);
                  }
                }
              }
              
              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr, rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_g2x_state_name(rr_plc_data_ptr->g2x_state));
          }
        }
        break;
      } /* RR_PLC_G2X_STATE_BACKGROUND_ABORTING */

      default:
      {
        if (geran_get_nv_recovery_restart_enabled(gas_id))
        {
          MSG_GERAN_ERROR_1_G("Unexpected state: %d, triggering panic reset!", rr_plc_data_ptr->g2x_state);
          geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
        }
        else
        {
          ERR_GERAN_FATAL_1_G("Unexpected state: %d, ", rr_plc_data_ptr->g2x_state);
        }
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((event != new_event) && (new_event != EV_NO_EVENT)) ||
        (rr_plc_data_ptr->old_g2x_state != rr_plc_data_ptr->g2x_state))
    {
      rr_store_trace_msg_buf(
        RR_PLC_G2X_SM,
        new_event,
        (byte) rr_plc_data_ptr->g2x_state,
        message_ptr,
        gas_id
      );
    }
#endif

    if (new_event != EV_NO_EVENT)
    {
      event = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event = EV_NO_EVENT;
    }

    if (rr_plc_data_ptr->old_gsm_state != rr_plc_data_ptr->gsm_state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_plc_g2x_control",
        rr_plc_data_ptr->old_g2x_state,
        rr_plc_data_ptr->g2x_state,
        rr_plc_g2x_state_name,
        gas_id
      );

      rr_plc_data_ptr->old_g2x_state = rr_plc_data_ptr->g2x_state;
    }
  }

  return output_event;
}
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION     rr_passed_service_search_irat_plmn_check

DESCRIPTION  IRAT PLMN SERVICE search must pass checks:
              LTE is only RAT in PLMN SEARCH REQ
              LTE is not in the SI2Q neighbour list

PARAMS       VOID

DEPENDENCIES

RETURN VALUE TRUE - passes IRAT PLMN service search check

===========================================================================*/
static boolean rr_passed_service_search_irat_plmn_check(const gas_id_t gas_id)
{
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr;
  boolean plmn_check_pass = TRUE;
  uint32 i = 0;

#ifdef FEATURE_SGLTE
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
#endif /* FEATURE_SGLTE */

  rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);


#ifdef FEATURE_SGLTE
  if (rr_is_sglte_mode_active(gas_id))
  {
    if (rr_PLMN_request_details_ptr->service_domain != SYS_SRV_DOMAIN_CS_PS)
    {
      MSG_GERAN_HIGH_1_G("RR IRAT PLMN service search rejected, SGLTE active, GSM does NOT have CS+PS : %d",
               rr_PLMN_request_details_ptr->service_domain);
      plmn_check_pass = FALSE;
    }
  }
#endif /* FEATURE_SGLTE */

  if (plmn_check_pass)
  {
    /* Check that LTE is the only RAT in the PLMN LIST SEARCH*/
    for (i = 0;
         i < rr_plmn_list_req_info_ptr->plmn_list.length &&
         i < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH;
         ++i)
    {
      if (rr_plmn_list_req_info_ptr->plmn_list.info[i].rat != SYS_RAT_LTE_RADIO_ACCESS)
      {
        MSG_GERAN_ERROR_1_G("RR IRAT PLMN service search contains Non LTE RAT: %d",rr_plmn_list_req_info_ptr->plmn_list.info[i].rat);
        plmn_check_pass = FALSE;
        break;
      }
    }

    /* Check that LTE is not in the neighbour list and also not measured */
    if (plmn_check_pass)
    {
      gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);
      if (rr_priority_and_threshold_information_available(
            rr_get_instances_ptr(SI2Q_INSTANCE, gas_id),gas_id)
          && (scell->gsm.lte_neighbour_list.count > 0)
          && (rr_is_lte_l1_measurements_configured(gas_id))
         )
      {
        MSG_GERAN_HIGH_0_G("RR IRAT PLMN service search abandoned LTE in neighbour cell list and measured");
        plmn_check_pass = FALSE;
      }
    }
  }

  return plmn_check_pass;
}
#endif /* FEATURE_LTE */

#ifdef FEATURE_SGLTE
static boolean rr_plc_sglte_g2x_start_search(const gas_id_t gas_id)
{
  rr_PLMN_list_req_info_T      *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  rr_plc_data_t                *rr_plc_data_ptr;
  boolean                      search_started = FALSE;
  detected_frequency_list_type detected_frequency_list;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  rr_plc_detected_frequency_list_populate(&detected_frequency_list, gas_id);

  if (rr_plc_data_ptr->rat_search_info_current != NULL)
  {
    switch (rr_plc_data_ptr->rat_search_info_current->rat)
    {
#ifdef FEATURE_GSM_TDS
      case RR_RAT_UTRAN_TDD:
      {
        sys_band_cap_u_type band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_TDS, gas_id);
        MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN Starting IRAT PLMN search on TDSCDMA");

        rr_update_band_preference_for_tdsrrc(band_cap_u.chgwt_band_cap, gas_id);

        rr_send_tds_interrat_plmn_search_req(
          &rr_plmn_list_req_info_ptr->plmn_list,               // plmn_list
          rr_plmn_list_req_info_ptr->network_selection_mode,   // network_select_mode
          RR_SGLTE_MAX_PLMN_LIST_SEARCH_TIME,                                               // timer_val
          FALSE,                                               // camped_suitably
          gas_id                                               // gas_id
        );

        search_started = TRUE;
        break;
      }
#endif // FEATURE_GSM_TDS

#ifdef FEATURE_LTE
      case RR_RAT_LTE:
      {
        MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN IRAT PLMN search on LTE");

        rr_plc_send_lte_rrc_g_plmn_srch_req(
          RR_SGLTE_MAX_PLMN_LIST_SEARCH_TIME,                      // timer_value
          &detected_frequency_list,   // detected_frequency_list
          gas_id                      // gas_id
        );
        search_started = TRUE;
        break;
      }
#endif // FEATURE_LTE

      default:
        MSG_GERAN_ERROR_1_G("Unexpected RAT type: %d",rr_plc_data_ptr->rat_search_info_current->rat);
      break;
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("No more RAT's to search");
  }

  return search_started;
}
#endif // FEATURE_SGLTE

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/**
 * Function: Constructs a plmn list based on plmn db
 *           used in RR_PLMN_SELECT_CNF, RR_PLMN_LIST_CNF,
 *           RR_INTERRAT_PLMN_SRCH_CNF
 *
 * Input:    @plmn_db_ptr:
 *                 Database to construct list from
 *           @automatic_mode_selection_criteria:
 *                 indicates if automatic or manual selection criteria
 *                 should be applied
 *
 * Output:   @plmn_list_ptr:
 *                 pointer to list to fill using the PLMN database
 *
 * Caution:  This function is public because it is used not only in
 *           PLMN search, but alos in other modules (e.g. Cell selection)
 *           Filtering is not applied in this function, in order to filter
 *           the list, function rr_plc_filter_plmn_list() needs to be called
 */
void rr_plc_construct_plmn_list(
  rr_available_plmn_db_T *plmn_db_ptr,
  sys_detailed_plmn_list_s_type *plmn_list_ptr,
  rr_forbidden_lai_check_option_e check_forbidden_lai,
  const gas_id_t gas_id
)
{
  int index=0;
  int cell_index;
  gas_id_t other_gas_id = GERAN_ACCESS_STRATUM_ID_1;
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr;

  rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);

  if ((plmn_list_ptr == NULL) || (plmn_db_ptr == NULL))
  {
    MSG_GERAN_ERROR_2_G("null pointer(s), plmn_list_ptr=%x, plmn_db_ptr=%d",
      plmn_list_ptr, plmn_db_ptr);
    return;
  }

  /* reset the plmn_list */
  memset(plmn_list_ptr, 0, sizeof(sys_detailed_plmn_list_s_type));

  for (index=0; index<plmn_db_ptr->num_plmn; index++)
  {
    /* cells for each PLMN are sorted by decreasing signal strength */
    for (cell_index = 0; cell_index < plmn_db_ptr->plmn_info[index].num_frequencies;++cell_index)
    {
      if (check_forbidden_lai == CHECK_FORBIDDEN_LAI_LIST)
      {
        /* stop at the first cell not in forbidden LAI */
        if (!rr_is_forbidden_LAI(
              &(plmn_db_ptr->plmn_info[index].cell_info[cell_index].lai),
              gas_id
            ))
        {
          break;
        }
      }
      else if (check_forbidden_lai == CHECK_REJECT_LAI_LIST)
      {
        if (!rr_check_lai_in_reject_list(&plmn_db_ptr->plmn_info[index].cell_info[cell_index].lai, gas_id))
        {
          break;
        }
      }
      else  /* neither check Forbidden LAI or Reject LAI list, add the strongest cell  */
      {
        if (plmn_db_ptr->plmn_info[index].num_frequencies > 0)
        {
          break;
        }
      }
    }

    if (cell_index < plmn_db_ptr->plmn_info[index].num_frequencies)
    {
      /* add the entry in the database */
      rr_plc_add_entry_to_plmn_list(
        plmn_list_ptr,
        plmn_db_ptr->plmn_info[index].PLMN_id,
        plmn_db_ptr->plmn_info[index].cell_info[cell_index].meas.RXLEV_average,
        ((plmn_db_ptr->plmn_info[index].cell_info[cell_index].GPRS_indicator)
          ? SYS_SRV_CAPABILITY_CS_PS_SERVICE
          : SYS_SRV_CAPABILITY_CS_SERVICE_ONLY)
      );
    }
  }

  /* For manual search, also add serving cell into the list */
  if (rr_is_camped(gas_id) &&
      rr_plmn_list_req_info_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
  {
    rr_l1_freq_power_T meas;
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
    meas = rr_gprs_get_serving_measurements(gas_id);

    MSG_GERAN_HIGH_0_G("Adding serving PLMN to search results");

    rr_plc_add_entry_to_plmn_list(
      plmn_list_ptr,
      scell_info_ptr->gsm.location_area_identification.plmn_id,
      meas.RXLEV_average,
      ((scell_info_ptr->gsm.gprs_indicator)
        ? SYS_SRV_CAPABILITY_CS_PS_SERVICE
        : SYS_SRV_CAPABILITY_CS_SERVICE_ONLY)
    );
  }

  /*Add the other sub's(online) PLMN to PLMN list which might be filtered from BCCH decode list 
  due to BST filter in which if cell is part of CA,BA list are not included for decoding */
  while (other_gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    if (other_gas_id != gas_id)
    {
      if (rr_is_camped(other_gas_id) &&
          rr_plmn_list_req_info_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) 
      {
        rr_l1_freq_power_T meas;
        gprs_scell_info_t *other_scell_info_ptr = rr_gprs_get_scell_info(other_gas_id);
        meas = rr_gprs_get_serving_measurements(other_gas_id);

        RR_NULL_CHECK_FATAL(other_scell_info_ptr);

        MSG_GERAN_HIGH_1_G("Adding GAS_ID = %d PLMN to search results",(int)other_gas_id);
        MSG_GERAN_HIGH_3_G("Adding PLMN 0x%02X-0x%02X-0x%02X into PLMN list",
          other_scell_info_ptr->gsm.location_area_identification.plmn_id.identity[0],
          other_scell_info_ptr->gsm.location_area_identification.plmn_id.identity[1],
          other_scell_info_ptr->gsm.location_area_identification.plmn_id.identity[2]
        );

        rr_plc_add_entry_to_plmn_list(
          plmn_list_ptr,
          other_scell_info_ptr->gsm.location_area_identification.plmn_id,
          meas.RXLEV_average,
          ((other_scell_info_ptr->gsm.gprs_indicator)
            ? SYS_SRV_CAPABILITY_CS_PS_SERVICE
            : SYS_SRV_CAPABILITY_CS_SERVICE_ONLY)
        );
      }
    }
    ++other_gas_id;
  }

  if (plmn_list_ptr->length >= SYS_PLMN_LIST_MAX_LENGTH)
  {
    MSG_GERAN_ERROR_0_G("Number of PLMN overflow!");
    plmn_list_ptr->length = SYS_PLMN_LIST_MAX_LENGTH;
  }

  return;
} /* rr_plc_construct_plmn_list */

/**
 * Function: returns TRUE if the PLMN search is automatic
 *
 * @Params:
 * @Outputs:
 */
boolean rr_plc_is_automatic_search(const gas_id_t gas_id)
{
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plmn_list_req_info_ptr);

  return (rr_plmn_list_req_info_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
}

#if (defined FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
/**
 * Function: gets the search timer value (given by RRC) in WTOG plmn search
 *
 * @Params:
 * @Outputs: the timer value
 */
uint32 rr_plc_get_wtog_search_timer_value(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_plc_data_t *rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  return rr_plc_data_ptr->x2g_search_timer_val;
}
#endif /* FEATURE_WCDMA || FEATURE_LTE || defined (FEATURE_GSM_TDS)*/

/**
 * Function: setup rr_plmn_list_control state machine.
 *           This function should be called each time to start PLMN search
 *
 * @Params:  search_type:   FOREGROUND_SEARCH, BACKGROUND_SEARCH or WTOG_SEARCH
 *           timer_val:     used only for WTOG search
 * @Outputs: updated rr_plc_data
 */
void rr_plc_setup(
  rr_plc_search_type_e search_type,
  uint32 timer_val,
  const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_plc_data_t *rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  MSG_GERAN_HIGH_0_G("Setup PLMN search related data");
  MSG_GERAN_HIGH_2_G("   -- search_type: %d, timer_val: %d", search_type, timer_val);

  rr_plc_data_ptr->search_type = search_type;

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  rr_plc_data_ptr->x2g_search_timer_val = timer_val;
  rr_plc_data_ptr->x2g_update_timer_val = 0UL;
  rr_plc_data_ptr->x2g_srch_start_sclks = timetick_get();
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

  return;
}

#ifdef FEATURE_LTE

boolean rr_plc_l2g_search_in_progress(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_plc_data_t *rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  if (rr_plc_data_ptr->state == RR_PLC_STATE_X2G && rr_plc_data_ptr->search_type == RR_PLC_LTOG_SEARCH)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#endif /* FEATURE_LTE */

/**
 * Function: Setup if ps service is enabled during PLMN search
 *           i.e. allow GPRS resumption at GTOW search interval.
 *           This function should be called by GRR
 *
 * @Params:  ps_service_enable - TRUE if PS service is allowed. FALSE otherwise
 * @Outputs:
 */
void rr_plc_set_ps_service_priority(
  boolean ps_service_enable,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_plc_data_t *rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  if ((ps_service_enable) && (rr_mac_ul_tbf_est_is_allowed(gas_id)))
  {
    MSG_GERAN_HIGH_0_G("ps_service is allowed during background PLMN search");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("ps_service is blocked during background PLMN search");
  }

  rr_plc_data_ptr->ps_service_enable = ps_service_enable;
}

/**
 * Function: To get if ps service is enabled during PLMN search.
 *           This function should be called by RR when it receives
 *           MAC_UL_TBF_EST_REQ in PLMN_LIST_SEARCH state.
 *
 * @Params:  N/A
 * @Outputs: if ps service is enabled
 */
boolean rr_plc_get_ps_service_priority(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_plc_data_t *rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  return rr_plc_data_ptr->ps_service_enable;
}

/**
 * Function: To get RR's action after PLMN search
 *
 * @Params:  N/A
 * @Outputs: GOTO_INACTIVE or START_IDLE
 */
rr_action_after_plc_e rr_plc_get_action_after_plmn_search_complete(const gas_id_t gas_id)
{
  rr_action_after_plc_e  rr_action = RR_PLC_COMPLETE_GOTO_INACTIVE;
  rr_plc_data_t *rr_plc_data_ptr;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  switch(rr_plc_data_ptr->search_type)
  {
#ifdef FEATURE_GSM_TDS
    case RR_PLC_TTOG_SEARCH:
      MSG_GERAN_HIGH_0_G("RR was inactive, now move back");
      break;
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
    case RR_PLC_WTOG_SEARCH:
      MSG_GERAN_HIGH_0_G("RR was inactive, now move back");
      break;
#endif /* FEATURE_WCDMA */

    case RR_PLC_FOREGROUND_SEARCH:

#ifdef FEATURE_SGLTE
    case RR_PLC_SGLTE_X2G_FOREGROUND_SEARCH:
#endif /* FEATURE_SGLTE */
      MSG_GERAN_HIGH_0_G("RR was no service or no_channels, go to inactive");
      break;

    case RR_PLC_BACKGROUND_SEARCH:

#ifdef FEATURE_SGLTE
    case RR_PLC_SGLTE_X2G_BACKGROUND_SEARCH:
#endif /* FEATURE_SGLTE */
      MSG_GERAN_HIGH_0_G("RR was in service, now move back");
      rr_action = RR_PLC_COMPLETE_START_IDLE;
      break;

#ifdef FEATURE_LTE
    case RR_PLC_LTOG_SEARCH:
      MSG_GERAN_HIGH_0_G("RR was inactive, now move back");
      break;
#endif /* FEATURE_LTE */

    default:
      MSG_GERAN_ERROR_1_G("unknown search_type=%d, go to inactive", rr_plc_data_ptr->search_type);
      break;
  }

  return rr_action;

}

/**
 * Function: RR PLMN LIST search state machine entrance
 *
 * Input:    @rr_event    - RR to RR_PLMN_SEARCH commands
 *           @message_ptr - The incoming message
 * Output:   return rr_event_T as RR_PLMN_SEARCH outputs to RR
 */
rr_event_T rr_plmn_list_control(
  rr_event_T event,
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T out_event = EV_NO_EVENT;
  rr_event_T new_event = EV_NO_EVENT;
  byte       message_id  = 0;
  byte       message_set = 0;
  rr_plc_data_t *rr_plc_data_ptr;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  /* reset and initialisation events can be handled immediately */
  if ((EV_RESET_SOFTWARE == event) ||
      (EV_PANIC_RESET    == event))
  {
    rr_plc_reset(rr_plc_data_ptr);
    return(EV_PLMN_SEARCH_COMPLETE);
  }

  event = rr_extract_message_header_details(
    event,
    message_ptr,
    &message_set,
    &message_id
  );

  /* some events are handled in stateless manner */
  switch (event)
  {
    case EV_PLMN_SEARCH_ABORT:
      MSG_GERAN_HIGH_0_G("plmn search start to abort....");
      rr_plc_data_ptr->abort = TRUE;
      break;

    case EV_INPUT_MESSAGE:
      if ((message_set == MS_MM_RR) && ((mm_rr_message_id_enum_T)message_id == RR_PLMN_LIST_REQ))
      {
        MSG_GERAN_ERROR_0_G("PLMN list message passed into");
        out_event = EV_PLMN_SEARCH_COMPLETE;
      }
      break;

    default:
      break;
  }

  while(event != EV_NO_EVENT)
  {
    if (event == EV_INPUT_MESSAGE && message_ptr == NULL)
    {
      MSG_GERAN_ERROR_0_G("message_ptr == NULL");
      event = EV_NO_EVENT;
      continue;
    }

    switch (rr_plc_data_ptr->state)
    {
      case RR_PLC_STATE_NULL:
      {
        switch(event)
        {
          case EV_PLMN_SEARCH_START:
          {
            boolean service_search_proceed = TRUE;
            rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);

#ifdef FEATURE_FEMTO_CSG
            if (rr_plmn_list_req_info_ptr->service_search == SYS_SERVICE_SEARCH_CSG)
            {
              /* We only expect WCDMA to be set in the RAT priority list structure
              in RR_PLMN_LIST_REQ.  If any other RATs are enabled, disable them now. */
              service_search_proceed = rr_passed_service_search_csg_check(gas_id);
            }
#endif /* FEATURE_FEMTO_CSG */

            rr_plc_start_plmn_search(gas_id);

            rr_plc_data_ptr->rat_search_info_current = rr_plc_rat_search_info_next(gas_id);

#ifdef FEATURE_LTE
            if (rr_plmn_list_req_info_ptr->service_search == SYS_SERVICE_SEARCH_IRAT_PLMN)
            {
              service_search_proceed = rr_passed_service_search_irat_plmn_check(gas_id);
            }
#endif /* FEATURE_LTE */

            if (rr_plc_data_ptr->rat_search_info_current != NULL && service_search_proceed)
            {
              uint32 max_search_timeout_ms = 0;

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
              if (rr_plmn_list_req_info_ptr->search_timer_value.MaxManualPLMNTimer > 0)
              {
                max_search_timeout_ms = CONVERT_SECONDS_TO_MILLISECONDS(
                  rr_plmn_list_req_info_ptr->search_timer_value.MaxManualPLMNTimer
                );

                MSG_GERAN_HIGH_1_G(
                  "Max search timer %d secs (MaxManualPLMNTimer)",
                  rr_plmn_list_req_info_ptr->search_timer_value.MaxManualPLMNTimer
                );
              }
#endif // FEATURE_MPLMN_PARTIAL_UPDATING

              if ( (rr_plc_data_ptr->search_type == RR_PLC_SGLTE_X2G_FOREGROUND_SEARCH) || (rr_plc_data_ptr->search_type == RR_PLC_SGLTE_X2G_BACKGROUND_SEARCH) )
              {
                if (rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_time_interval > 0 )
                {
                  max_search_timeout_ms  = CONVERT_SECONDS_TO_MILLISECONDS(
                  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_time_interval
                  );
                }
              }

#ifdef FEATURE_LTE
              if (max_search_timeout_ms == 0)
              {
                if (rr_plc_nv_settings.guard_tmri_val > 0)
                {
                  max_search_timeout_ms = (uint32)rr_plc_nv_settings.guard_tmri_val;

                  MSG_GERAN_HIGH_1_G(
                    "Max search timer %d secs (guard_tmri_val)",
                    CONVERT_MILLISECONDS_TO_SECONDS(max_search_timeout_ms)
                  );
                }
              }
#endif /* FEATURE_LTE */

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
              if (max_search_timeout_ms > 0)
              {
                if (rr_plmn_list_req_info_ptr->search_timer_value.FirstPLMNTimer > 0)
                {
                  rr_timer_start(
                    RR_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER,
                    CONVERT_SECONDS_TO_MILLISECONDS(
                      rr_plmn_list_req_info_ptr->search_timer_value.FirstPLMNTimer
                    ),
                    gas_id
                  );

                  MSG_GERAN_HIGH_1_G(
                    "Partial results timer %d secs (FirstPLMNTimer)",
                    rr_plmn_list_req_info_ptr->search_timer_value.FirstPLMNTimer
                  );
                }
              }
              else
#endif // FEATURE_MPLMN_PARTIAL_UPDATING
              {
                max_search_timeout_ms = RR_PLMN_LIST_SEARCH_GUARD_TIMER_DEFAULT_DURATION;

                MSG_GERAN_HIGH_1_G(
                  "Max search timer %d secs (default)",
                  CONVERT_MILLISECONDS_TO_SECONDS(RR_PLMN_LIST_SEARCH_GUARD_TIMER_DEFAULT_DURATION)
                );
              }

              rr_timer_start(
                RR_PLMN_LIST_SEARCH_GUARD_TIMER,
                max_search_timeout_ms,
                gas_id
              );

              switch (rr_plc_data_ptr->rat_search_info_current->rat)
              {
                case RR_RAT_GSM:
                {
                  new_event = rr_plc_gsm_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                  rr_plc_data_ptr->state = RR_PLC_STATE_GSM;
                  break;
                } /* RR_RAT_GSM */

#ifdef FEATURE_GSM_TDS
                case RR_RAT_UTRAN_TDD:
                {
                  sys_band_cap_u_type band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_TDS, gas_id);
                  rr_update_band_preference_for_tdsrrc(band_cap_u.chgwt_band_cap, gas_id);
                  new_event = rr_plc_g2x_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                  rr_plc_data_ptr->state = RR_PLC_STATE_G2X;
                  break;
                } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
                case RR_RAT_WCDMA:
                {
                  sys_band_cap_u_type band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_WCDMA, gas_id);
                  rr_update_band_preference_for_rrc(band_cap_u.chgwt_band_cap, gas_id);
                  new_event = rr_plc_g2x_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                  rr_plc_data_ptr->state = RR_PLC_STATE_G2X;
                  break;
                } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
                case RR_RAT_LTE:
                {
                  new_event = rr_plc_g2x_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                  rr_plc_data_ptr->state = RR_PLC_STATE_G2X;
                  break;
                } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

                default:
                {
                  MSG_GERAN_HIGH_0_G("No PLMN search is neccesary");
                  rr_plc_search_complete(gas_id);
                  out_event = EV_PLMN_SEARCH_COMPLETE;
                }
              }
            }
            else
            {
              MSG_GERAN_HIGH_0_G("No PLMN search is neccesary");
              rr_plc_search_complete(gas_id);
              out_event = EV_PLMN_SEARCH_COMPLETE;
            }
            break;
          } /* EV_PLMN_SEARCH_START */

#ifdef FEATURE_WCDMA
          case EV_PLMN_SEARCH_START_WTOG:
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
          case EV_PLMN_SEARCH_START_TTOG:
#endif /* FEATURE_GSM_TDS */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
          {
            rr_plc_start_plmn_search(gas_id);
            rr_plc_data_ptr->rat_search_info_current = rr_plc_rat_search_info_next(gas_id);
            if (rr_plc_data_ptr->rat_search_info_current)
            {
              if (rr_plc_data_ptr->rat_search_info_current->rat != RR_RAT_GSM)
              {
                MSG_GERAN_ERROR_2_G("rr_plc_data_ptr->rat_search_info_current->rat == %d (expected %d)",
                          rr_plc_data_ptr->rat_search_info_current->rat,
                          RR_RAT_GSM);
              }
              new_event = rr_plc_x2g_control(EV_PLMN_SEARCH_START, NULL, gas_id);
              rr_plc_data_ptr->state = RR_PLC_STATE_X2G;
            }
            else
            {
              /* If rr_plc_data_ptr->rat_search_info_current is NULL this indicates that
              the GSM search does not need to be performed as RR already has
              an available PLMN database (from a previous, recent cell
              selection attempt). */
              rr_plc_search_complete(gas_id);
              out_event = EV_PLMN_SEARCH_COMPLETE;
              rr_plc_data_ptr->state = RR_PLC_STATE_NULL;
            }
            break;
          } /* EV_PLMN_SEARCH_START_WTOG */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
          case EV_PLMN_SEARCH_START_LTOG:
          {
            rr_plc_start_plmn_search(gas_id);
            rr_plc_data_ptr->rat_search_info_current = rr_plc_rat_search_info_next(gas_id);
            if (rr_plc_data_ptr->rat_search_info_current != NULL)
            {
              if (rr_plc_data_ptr->rat_search_info_current->rat != RR_RAT_GSM)
              {
                MSG_GERAN_ERROR_2_G("rr_plc_data_ptr->rat_search_info_current->rat == %d (expected %d)",
                          rr_plc_data_ptr->rat_search_info_current->rat,
                          RR_RAT_GSM);
              }
              new_event = rr_plc_x2g_control(EV_PLMN_SEARCH_START, NULL, gas_id);
              rr_plc_data_ptr->state = RR_PLC_STATE_X2G;
            }
            else
            {
              /* If rr_plc_data_ptr->rat_search_info_current is NULL this indicates that
              the GSM search does not need to be performed as RR already has
              an available PLMN database (from a previous, recent cell
              selection attempt). */
              rr_plc_search_complete(gas_id);
              out_event = EV_PLMN_SEARCH_COMPLETE;
              rr_plc_data_ptr->state = RR_PLC_STATE_NULL;
            }
            break;
          } /* EV_PLMN_SEARCH_START_LTOG */
#endif /* FEATURE_LTE */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_plc_state_name(rr_plc_data_ptr->state));
            out_event = EV_PLMN_SEARCH_COMPLETE;
          }
        }
        break;
      } /* RR_PLC_STATE_NULL */

      case RR_PLC_STATE_GSM:
      {
        switch (event)
        {
          case EV_PLMN_SEARCH_COMPLETE:
          {
            ASSERT(rr_plc_data_ptr->rat_search_info_current->rat == RR_RAT_GSM);
            rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
            if (rr_plc_data_ptr->abort)
            {
              MSG_GERAN_HIGH_0_G("RR plmn search abort completed");
              rr_plc_search_complete(gas_id);
              out_event = EV_PLMN_SEARCH_COMPLETE;
              rr_plc_data_ptr->state = RR_PLC_STATE_NULL;
            }
            else
            {
              if (rr_plc_data_ptr->rat_search_info_current->hplmn_found)
              {
                rr_plc_search_complete(gas_id);
                out_event = EV_PLMN_SEARCH_COMPLETE;
                rr_plc_data_ptr->state = RR_PLC_STATE_NULL;
              }
              else
              {
                rr_plc_data_ptr->rat_search_info_current = rr_plc_rat_search_info_next(gas_id);

                if (rr_plc_data_ptr->rat_search_info_current != NULL)
                {
                  switch (rr_plc_data_ptr->rat_search_info_current->rat)
                  {
                    case RR_RAT_GSM:
                    {
                      new_event = rr_plc_gsm_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                      rr_plc_data_ptr->state = RR_PLC_STATE_GSM;
                      break;
                  } /* RR_RAT_GSM */

#ifdef FEATURE_GSM_TDS
                    case RR_RAT_UTRAN_TDD:
                    {
                      sys_band_cap_u_type band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_TDS, gas_id);
                      rr_update_band_preference_for_tdsrrc(band_cap_u.chgwt_band_cap, gas_id);
                      new_event = rr_plc_g2x_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                      rr_plc_data_ptr->state = RR_PLC_STATE_G2X;
                      break;
                    } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
                    case RR_RAT_WCDMA:
                    {
                      sys_band_cap_u_type band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_WCDMA, gas_id);
                      rr_update_band_preference_for_rrc(band_cap_u.chgwt_band_cap, gas_id);
                      new_event = rr_plc_g2x_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                      rr_plc_data_ptr->state = RR_PLC_STATE_G2X;
                      break;
                    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
                    case RR_RAT_LTE:
                    {
                      new_event = rr_plc_g2x_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                      rr_plc_data_ptr->state = RR_PLC_STATE_G2X;
                      break;
                    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

                    default:
                    {
                      MSG_GERAN_HIGH_0_G("No PLMN search is neccesary");
                      rr_plc_search_complete(gas_id);
                      out_event = EV_PLMN_SEARCH_COMPLETE;
                    }
                  }
                }
                else
                {
                  rr_plc_search_complete(gas_id);
                  out_event = EV_PLMN_SEARCH_COMPLETE;
                  rr_plc_data_ptr->state = RR_PLC_STATE_NULL;
                }
              }
            }
            break;
          } /* EV_PLMN_SEARCH_COMPLETE */

          case EV_PLMN_LIST_SEARCH_GUARD_TIMER_EXPIRED:
          {
            rr_plc_data_ptr->abort = TRUE;
            new_event = rr_plc_gsm_control(EV_PLMN_SEARCH_ABORT, NULL, gas_id);
            break;
          } /* EV_PLMN_LIST_SEARCH_GUARD_TIMER_EXPIRED */

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
          case EV_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER_EXPIRED:
          {
            sys_detailed_plmn_list_s_type *available_plmn_list_ptr;
            rr_available_plmn_db_T *available_plmn_db_ptr = rr_get_available_plmn_db_ptr(gas_id);

            // Use heap for temporary PLMN list
            available_plmn_list_ptr = GPRS_MEM_CALLOC(sizeof(sys_detailed_plmn_list_s_type));

            // Populate PLMN list based on available PLMN DB entries so far
            rr_plc_construct_plmn_list(
              available_plmn_db_ptr,
              available_plmn_list_ptr,
              NO_CHECK,
              gas_id
            );

            rr_plc_handle_partial_results_timer_expiry(
              available_plmn_list_ptr,
              gas_id);

            GPRS_MEM_FREE(available_plmn_list_ptr);
            break;
          } // EV_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER_EXPIRED
#endif // FEATURE_MPLMN_PARTIAL_UPDATING

          default:
          {
            new_event = rr_plc_gsm_control(event, message_ptr, gas_id);
          }
        }
        break;
      } /* RR_PLC_STATE_GSM */

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
      case RR_PLC_STATE_G2X:
      {
        switch (event)
        {
          case EV_PLMN_SEARCH_COMPLETE:
          {
#if defined(FEATURE_WCDMA) && !defined(FEATURE_LTE)
  #ifdef FEATURE_GSM_TDS
            ASSERT(rr_plc_data_ptr->rat_search_info_current->rat == RR_RAT_WCDMA ||
                   rr_plc_data_ptr->rat_search_info_current->rat == RR_RAT_UTRAN_TDD);
  #else
            ASSERT(rr_plc_data_ptr->rat_search_info_current->rat == RR_RAT_WCDMA);
  #endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_WCDMA && !FEATURE_LTE */
#if defined(FEATURE_WCDMA) && defined(FEATURE_LTE)
  #ifdef FEATURE_GSM_TDS
            ASSERT(rr_plc_data_ptr->rat_search_info_current->rat == RR_RAT_WCDMA ||
                   rr_plc_data_ptr->rat_search_info_current->rat == RR_RAT_LTE ||
                   rr_plc_data_ptr->rat_search_info_current->rat == RR_RAT_UTRAN_TDD);
  #else
            ASSERT(rr_plc_data_ptr->rat_search_info_current->rat == RR_RAT_WCDMA ||
                   rr_plc_data_ptr->rat_search_info_current->rat == RR_RAT_LTE);
  #endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_WCDMA && FEATURE_LTE */

            rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
            if (rr_plc_data_ptr->abort)
            {
              MSG_GERAN_HIGH_0_G("RR plmn search abort completed");
              rr_plc_search_complete(gas_id);
              out_event = EV_PLMN_SEARCH_COMPLETE;
              rr_plc_data_ptr->state = RR_PLC_STATE_NULL;
            }
            else
            {
              if (rr_plc_data_ptr->rat_search_info_current->hplmn_found)
              {
                rr_plc_search_complete(gas_id);
                out_event = EV_PLMN_SEARCH_COMPLETE;
                rr_plc_data_ptr->state = RR_PLC_STATE_NULL;
              }
              else
              {
                rr_plc_data_ptr->rat_search_info_current = rr_plc_rat_search_info_next(gas_id);

                if (rr_plc_data_ptr->rat_search_info_current != NULL)
                {
                  switch (rr_plc_data_ptr->rat_search_info_current->rat)
                  {
                    case RR_RAT_GSM:
                    {
                      new_event = rr_plc_gsm_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                      rr_plc_data_ptr->state = RR_PLC_STATE_GSM;
                      break;
                    } /* RR_RAT_GSM */

#ifdef FEATURE_GSM_TDS
                    case RR_RAT_UTRAN_TDD:
                    {
                      sys_band_cap_u_type band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_TDS, gas_id);
                      rr_update_band_preference_for_tdsrrc(band_cap_u.chgwt_band_cap, gas_id);
                      new_event = rr_plc_g2x_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                      rr_plc_data_ptr->state = RR_PLC_STATE_G2X;
                      break;
                    } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
                    case RR_RAT_WCDMA:
                    {
                      sys_band_cap_u_type band_cap_u = rr_plc_band_pref_sys_mode(SYS_SYS_MODE_WCDMA, gas_id);
                      rr_update_band_preference_for_rrc(band_cap_u.chgwt_band_cap, gas_id);
                      new_event = rr_plc_g2x_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                      rr_plc_data_ptr->state = RR_PLC_STATE_G2X;
                      break;
                    } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
                    case RR_RAT_LTE:
                    {
                      new_event = rr_plc_g2x_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                      rr_plc_data_ptr->state = RR_PLC_STATE_G2X;
                      break;
                    } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

                    default:
                    {
                      MSG_GERAN_HIGH_0_G("No PLMN search is neccesary");
                      rr_plc_search_complete(gas_id);
                      out_event = EV_PLMN_SEARCH_COMPLETE;
                    }
                  }
                }
                else
                {
                  rr_plc_search_complete(gas_id);
                  out_event = EV_PLMN_SEARCH_COMPLETE;
                  rr_plc_data_ptr->state = RR_PLC_STATE_NULL;
                }
              }
            }
            break;
          } /* EV_PLMN_SEARCH_COMPLETE */

          case EV_PLMN_LIST_SEARCH_GUARD_TIMER_EXPIRED:
          {
            rr_plc_data_ptr->abort = TRUE;
            MSG_GERAN_HIGH_0_G("RR_PLMN_LIST_SEARCH_GUARD_TIMER expired, abort search");
            new_event = rr_plc_g2x_control(EV_PLMN_SEARCH_ABORT, NULL, gas_id);
            break;
          } /* EV_PLMN_LIST_SEARCH_GUARD_TIMER_EXPIRED */

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
          case EV_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER_EXPIRED:
          {
            rr_rat_e rat = rr_plc_data_ptr->rat_search_info_current->rat;

#ifdef FEATURE_LTE
            if (rat == RR_RAT_LTE)
            {
              rr_plc_send_lte_rrc_g_get_plmn_prtl_results_req(gas_id);
            }
            else
#endif /* FEATURE_LTE */
            {
              sys_detailed_plmn_list_s_type *partial_plmn_list_ptr;
              boolean partial_plmn_list_valid = FALSE;

              partial_plmn_list_ptr = GPRS_MEM_CALLOC(sizeof(sys_detailed_plmn_list_s_type));

              if (partial_plmn_list_ptr != NULL)
              {
                switch (rat)
                {
#ifdef FEATURE_WCDMA
                  case RR_RAT_WCDMA:
                  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                    partial_plmn_list_valid = wrrc_get_inter_rat_bplmn_prtl_result_sub_id(
                      partial_plmn_list_ptr,              // available_plmn_list
                      geran_map_gas_id_to_nas_id(gas_id)  // as_id
                    );
#else
                    partial_plmn_list_valid = wrrc_get_inter_rat_bplmn_prtl_result(
                      partial_plmn_list_ptr               // available_plmn_list
                    );
#endif // FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM
                    break;
                  }
#endif // FEATURE_WCDMA

#ifdef FEATURE_GSM_TDS
                  case RR_RAT_UTRAN_TDD:
                  {
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
                    partial_plmn_list_valid = tdsrrc_get_inter_rat_bplmn_prtl_result_sub_id(
                      partial_plmn_list_ptr,              // available_plmn_list
                      geran_map_gas_id_to_nas_id(gas_id)  // as_id
                    );
#else
                    partial_plmn_list_valid = tdsrrc_get_inter_rat_bplmn_prtl_result(
                      partial_plmn_list_ptr               // available_plmn_list
                    );
#endif // FEATURE_TDS_SUB_ID_INTERFACE
                    break;
                  }
#endif // FEATURE_GSM_TDS
                  default: ;
                }

                if (partial_plmn_list_valid)
                {
                  rr_plc_handle_partial_results_timer_expiry(
                    partial_plmn_list_ptr,
                    gas_id
                  );
                }
                else
                {
                  rr_plc_handle_partial_results_timer_expiry(
                    NULL,
                    gas_id
                  );
                }

                GPRS_MEM_FREE(partial_plmn_list_ptr);
              }
            }
            break;
          }
#endif // FEATURE_MPLMN_PARTIAL_UPDATING

          default:
          {
            new_event = rr_plc_g2x_control(event, message_ptr, gas_id);
          }
        }
        break;
      } /* RR_PLC_STATE_G2X */

      case RR_PLC_STATE_X2G:
        switch (event)
        {
          case EV_PLMN_SEARCH_COMPLETE:
            ASSERT(rr_plc_data_ptr->rat_search_info_current->rat == RR_RAT_GSM);
            rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
            rr_plc_search_complete(gas_id);
            out_event = EV_PLMN_SEARCH_COMPLETE;
            rr_plc_data_ptr->state = RR_PLC_STATE_NULL;
            break;

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
          case EV_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER_EXPIRED:
          {
            MSG_GERAN_ERROR_0_G("Unexpected partial results timer expiry");
            break;
          }
#endif // FEATURE_MPLMN_PARTIAL_UPDATING

          default:
            new_event = rr_plc_x2g_control(event, message_ptr, gas_id);
            break;
        }
        break;
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

      default:
      {
        if (geran_get_nv_recovery_restart_enabled(gas_id))
        {
          MSG_GERAN_ERROR_1_G("Unexpected state: %d, triggering recovery", rr_plc_data_ptr->state);
          geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
        }
        else
        {
          ERR_GERAN_FATAL_1_G("Unexpected state: %d", rr_plc_data_ptr->state);
        }
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((event != new_event) && (new_event != EV_NO_EVENT)) ||
        (rr_plc_data_ptr->old_state != rr_plc_data_ptr->state))
    {
      rr_store_trace_msg_buf(
        RR_PLMN_LIST_SM,
        new_event,
        (byte) rr_plc_data_ptr->state,
        message_ptr,
        gas_id
      );
    }
#endif

    if (new_event != EV_NO_EVENT)
    {
      event = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event = EV_NO_EVENT;
    }

    if (rr_plc_data_ptr->state != rr_plc_data_ptr->old_state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_plmn_list_control",
        rr_plc_data_ptr->old_state,
        rr_plc_data_ptr->state,
        rr_plc_state_name,
        gas_id
      );

      rr_plc_data_ptr->old_state = rr_plc_data_ptr->state;
    }
  }

  return out_event;
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
/*===========================================================================

FUNCTION     rr_plc_wtog_decoding_bcch

DESCRIPTION  Queries whether the PLMN list state machine is currently decoding
             a BCCH list for a WTOG background PLMN search.


PARAMS       VOID

DEPENDENCIES rr_plc_data_ptr->state, rr_plc_data_ptr->x2g_state

RETURN VALUE TRUE if a W2G BPLMN is running and PLC is decoding the BCCH list

===========================================================================*/
boolean rr_plc_wtog_decoding_bcch(const gas_id_t gas_id)
{
  rr_plc_data_t *rr_plc_data_ptr;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);

  if (rr_plc_data_ptr != NULL)
  {
    /* check the main RR control state as well as the local PLC state */
    return(rr_background_plmn_search_in_progress(gas_id) &&
           (rr_plc_data_ptr->state == RR_PLC_STATE_X2G) &&
           (rr_plc_data_ptr->x2g_state == RR_PLC_X2G_STATE_BCCH_LIST_DECODE) );
  }

  return(FALSE);
}
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
boolean rr_plc_bplmn_pscan_active(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_plc_data_t *rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  rr_PLMN_list_req_info_T * rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);

  if ((rr_plc_data_ptr != NULL) && (rr_plmn_list_req_info_ptr != NULL))
  {
    if (rr_plc_data_ptr->search_type == RR_PLC_BACKGROUND_SEARCH)
    {
      if ((rr_plc_data_ptr->state     == RR_PLC_STATE_GSM) &&
          (rr_plc_data_ptr->gsm_state == RR_PLC_GSM_STATE_PSCAN))
      {
        return(rr_multi_sim_mode_enabled());
      }
      else if ((rr_plc_data_ptr->state  == RR_PLC_STATE_G2X) &&
               (rr_plmn_list_req_info_ptr->service_search != SYS_SERVICE_SEARCH_IRAT_PLMN))
      {
        return(rr_multi_sim_mode_enabled());
      }
    }
  }

  return(FALSE);
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

void rr_plc_set_band_filter(
  sys_band_mask_type band_mask,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_plc_data_t *rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);

  if (rr_plc_data_ptr != NULL)
  {
    rr_plc_data_ptr->band_filter_mask = (band_mask & RR_SYS_BAND_MASK_GSM_SUPPORTED_BANDS);

    MSG_GERAN_HIGH_2_G(
      "PLMN search band filter set 0x%08X (0x%08X)",
      rr_plc_data_ptr->band_filter_mask,
      band_mask
    );
  }
}

sys_band_mask_type rr_plc_get_band_filter(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_plc_data_t *rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);

  if (rr_plc_data_ptr != NULL)
  {
    return rr_plc_data_ptr->band_filter_mask;
  }

  return SYS_BAND_MASK_EMPTY;
}

#ifdef FEATURE_SGLTE

/**
  @brief This function implements a state machine to control PLMN list search
  procedure when SGLTE is active
*/
rr_event_T rr_plc_sglte_g2x_control(rr_event_T rr_event, rr_cmd_bdy_type *message_ptr, const gas_id_t gas_id)
{
  rr_plc_data_t *rr_plc_data_ptr;
  byte           message_id = 0;
  byte           message_set = 0;
  rr_event_T     new_event = EV_NO_EVENT;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  /* extract message_set and message_id (and check null message ptr) */
  rr_event = rr_extract_message_header_details(
    rr_event,
    message_ptr,
    &message_set,
    &message_id
  );

  while (rr_event != EV_NO_EVENT)
  {
    switch (rr_plc_data_ptr->sglte_g2x_state)
    {
      case RR_PLC_SGLTE_G2X_STATE_NULL:
      {
        switch(rr_event)
        {
          case EV_PLMN_SEARCH_START:
          {
            MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN EV_PLMN_SEARCH_START rxd");
            rr_send_mph_ue_mode_change_req(TRUE,gas_id);
            rr_g2wr_reset_wcdma_surrounding_database(gas_id);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
            rr_g2wr_cancel_all_wcdma_five_second_timers(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

            rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_DEACT;
            rr_plc_data_ptr->aborting_no_multirat_activation = FALSE;
            rr_event = EV_NO_EVENT; /*absorb in this SM*/
            break;
          }
          default:; /*handle all other signals in rr_control*/
        }
        break;
      }

      case RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_DEACT:
      {
        switch(rr_event)
        {
          case  EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_UE_MODE_CHANGE_CNF:
                  {
                    MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN MPH_UE_MODE_CHANGE_CNF rxd");

                    rr_plc_start_plmn_search(gas_id);

                    rr_plc_data_ptr->rat_search_info_current = rr_plc_rat_search_info_next(gas_id);

                    if (rr_plc_sglte_g2x_start_search(gas_id))
                    {                      
                      uint32 max_search_timeout_ms = 0;
                      /* Now that we have triggered a request to the other RAT, Start a Tracking timer */
                      max_search_timeout_ms = RR_PLMN_LIST_SEARCH_GUARD_TIMER_DEFAULT_DURATION;

                      MSG_GERAN_HIGH_1_G(
                      "Max search timer %d secs (default)",
                      CONVERT_MILLISECONDS_TO_SECONDS(RR_PLMN_LIST_SEARCH_GUARD_TIMER_DEFAULT_DURATION)
                      );
					
                      rr_timer_start(
                      RR_PLMN_LIST_SEARCH_GUARD_TIMER,
                      max_search_timeout_ms,
                       gas_id
                      );

                      rr_event = EV_NO_EVENT; /*absorb in this SM*/
                      rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS;
                    }
                    else
                    {
                      MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN No RAT's to search");
                      rr_send_mph_ue_mode_change_req(FALSE,gas_id);
                      rr_event = EV_NO_EVENT; /*absorb in this SM*/
                      rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_ACTIVATION;
                    }
                    break;
                  }
                  default:; /*handle all other signals in rr_control*/
                }
                break;
              }
              default:; /*handle all other signals in rr_control*/
            }
            break;
          }

          case EV_PLMN_SEARCH_ABORT:
          case EV_PLMN_SEARCH_ABORT_NO_MULTIRAT_ACT:
          {
            MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN EV_PLMN_SEARCH_ABORT rxd");
            
            rr_timer_cancel(RR_PLMN_LIST_SEARCH_GUARD_TIMER, gas_id);
           
            rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_DEACT_ABORTING;

            if (rr_event == EV_PLMN_SEARCH_ABORT_NO_MULTIRAT_ACT)
            {
              rr_plc_data_ptr->aborting_no_multirat_activation = TRUE;
            }

            rr_event = EV_NO_EVENT; /*absorb in this SM*/
            break;
          }
          default:; /*handle all other signals in rr_control*/
        }
        break;
      }

      case RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch (message_id)
                {
                  case TDSRRC_INTERRAT_PLMN_SRCH_CNF:
                  {
                    tdsrrc_interrat_plmn_srch_cnf_type *plmn_srch_cnf =
                      (tdsrrc_interrat_plmn_srch_cnf_type *)message_ptr;

                    MSG_GERAN_MED_2_G(
                      "SGLTE PLC: received TDSRRC_INTERRAT_PLMN_SRCH_CNF, srch_done=%d, hplmn_found=%d",
                      plmn_srch_cnf->srch_done, plmn_srch_cnf->hplmn_found
                    );

                    if (plmn_srch_cnf->srch_done)
                    {
                      rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
                      rr_plc_data_ptr->rat_search_info_current->available_plmn_list =
                        plmn_srch_cnf->available_plmn_list;

                      if (plmn_srch_cnf->hplmn_found)
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN found");
                        rr_plc_data_ptr->rat_search_info_current->hplmn_found = TRUE;

                        rr_plc_rat_search_set_to_search_done(gas_id); /*HPLMN found stop searching*/
                      }
                      else
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN not found");
                      }

                      new_event = EV_PLMN_SEARCH_START;
                    }
                    else
                    {
                      MSG_GERAN_HIGH_1_G("SGLTE PLC G2T:: aborting due to srch_done= %d received",plmn_srch_cnf->srch_done);
                      rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);
                      rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_RAT_ABORTING;
                      rr_event = EV_NO_EVENT; /*absorb in this SM*/
                    }
                    break;
                  } /* TDSRRC_INTERRAT_PLMN_SRCH_CNF */
                  default:; /*handle all other signals in rr_control*/
                }
                break;
              } /* MS_RRC_RR */
#endif /*FEATURE_GSM_TDS*/

              case MS_MSGR_RR:
              {
                switch (message_ptr->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case LTE_RRC_G_PLMN_SRCH_RSP:
                  {
                    lte_irat_plmn_srch_rsp_s * plmn_srch_rsp = &message_ptr->rr_msgr_msg.message.lte_irat_plmn_srch_rsp;

                    MSG_GERAN_MED_2_G(
                      "SGLTE PLC: received LTE_RRC_G_PLMN_SRCH_RSP, srch_done=%d, hplmn_found=%d",
                      plmn_srch_rsp->srch_done,
                      plmn_srch_rsp->hplmn_found
                    );
                    if (plmn_srch_rsp->srch_done)
                    {
                      rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
                      rr_plc_data_ptr->rat_search_info_current->available_plmn_list = plmn_srch_rsp->available_plmn_list;
                      rr_plc_data_ptr->rat_search_info_current->detected_frequency_list = plmn_srch_rsp->detected_frequency_list;

                      if (plmn_srch_rsp->hplmn_found)
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN found");
                        rr_plc_data_ptr->rat_search_info_current->hplmn_found = TRUE;

                        rr_plc_rat_search_set_to_search_done(gas_id); /*HPLMN found stop searching*/
                      }
                      else
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN not found");
                      }

                      new_event = EV_PLMN_SEARCH_START;
                    }
                    else
                    {
                      MSG_GERAN_HIGH_1_G("SGLTE PLC G2L:: aborting due to srch_done= %d received",plmn_srch_rsp->srch_done);
                      rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);
       
                      rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_RAT_ABORTING;
                      rr_event = EV_NO_EVENT; /*absorb in this SM*/
                    }
                    break;
                  } /* LTE_RRC_G_PLMN_SRCH_CNF */
#endif /* FEATURE_LTE */

                  default:; /*handle all other signals in rr_control*/
                }
                break;
              } /* MS_MSGR_RR */
              default:; /*handle all other signals in rr_control*/
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_START:
          {
            MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN EV_PLMN_SEARCH_START rxd");

            rr_plc_data_ptr->rat_search_info_current = rr_plc_rat_search_info_next(gas_id);

            if (rr_plc_sglte_g2x_start_search(gas_id))
            {
              rr_event = EV_NO_EVENT; /*absorb in this SM*/
            }
            else
            {
              rr_send_mph_ue_mode_change_req(FALSE,gas_id);
              rr_event = EV_NO_EVENT; /*absorb in this SM*/
              rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_ACTIVATION;
            }
            break;
          }

          case EV_PLMN_SEARCH_ABORT:
          case EV_PLMN_SEARCH_ABORT_NO_MULTIRAT_ACT:
          {
            MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN EV_PLMN_SEARCH_ABORT rxd");

            rr_plc_g2x_send_abort_req(rr_plc_data_ptr->rat_search_info_current->rat, gas_id);

            if (rr_event == EV_PLMN_SEARCH_ABORT_NO_MULTIRAT_ACT)
            {
              rr_plc_data_ptr->aborting_no_multirat_activation = TRUE;
            }

            rr_timer_cancel(RR_PLMN_LIST_SEARCH_GUARD_TIMER, gas_id);
            rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS_ABORTING;
            rr_event = EV_NO_EVENT; /*absorb in this SM*/
            break;
          }
          default:; /*handle all other signals in rr_control*/
        }
        break;
      }


      case RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_DEACT_ABORTING:
      {
        switch(rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_UE_MODE_CHANGE_CNF:
                  {
                    MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN MPH_UE_MODE_CHANGE_CNF rxd");

                    rr_send_mph_ue_mode_change_req(FALSE, gas_id);
                    rr_event = EV_NO_EVENT; /*absorb in this SM*/
                    rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_ACTIVATION;
                    break;
                  }
                  default:; /*handle all other signals in rr_control*/
                }
                break;
              }
              default:; /*handle all other signals in rr_control*/
            }
            break;
          }

          case EV_PLMN_SEARCH_ABORT: /*absorb in this state wait for UE mode change cnf*/
          case EV_PLMN_SEARCH_ABORT_NO_MULTIRAT_ACT:
          {
            MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN EV_PLMN_SEARCH_ABORT rxd");

            if (rr_event == EV_PLMN_SEARCH_ABORT_NO_MULTIRAT_ACT)
            {
              rr_plc_data_ptr->aborting_no_multirat_activation = TRUE;
            }
            rr_timer_cancel(RR_PLMN_LIST_SEARCH_GUARD_TIMER, gas_id);            
            break;
          }
          default:; /*handle all other signals in rr_control*/
        }
        break;
      }

      case RR_PLC_SGLTE_G2X_STATE_RAT_ABORTING:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch (message_id)
                {
                  case TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF:
                  {
                    MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF rxd");
                    rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
                    rr_plc_data_ptr->sglte_g2x_state =RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS;              
                    new_event = EV_PLMN_SEARCH_START;
                    break;                    
                  }
                  default:; /*handle all other signals in rr_control*/
                }
                break;
              } /* MS_RRC_RR */
#endif /*FEATURE_GSM_TDS*/

              case MS_MSGR_RR:
              {
                switch (message_ptr->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case LTE_RRC_G_ABORT_PLMN_SRCH_RSP:
                  {
                    MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN LTE_RRC_G_ABORT_PLMN_SRCH_RSP rxd");
                    rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
                    rr_plc_data_ptr->sglte_g2x_state =RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS; 
                    new_event = EV_PLMN_SEARCH_START;
                    break;
                  }
#endif /* FEATURE_LTE */

                  default:; /*handle all other signals in rr_control*/
                }
                break;
              } /* MS_MSGR_RR */
              default:; /*handle all other signals in rr_control*/
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_PLMN_SEARCH_ABORT:
          case EV_PLMN_SEARCH_ABORT_NO_MULTIRAT_ACT:
          {
            MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN EV_PLMN_SEARCH_ABORT rxd");
            MSG_GERAN_HIGH_0_G("Already abort request sent, waiting for ABORT_CNF");
            if (rr_event == EV_PLMN_SEARCH_ABORT_NO_MULTIRAT_ACT)
            {
              rr_plc_data_ptr->aborting_no_multirat_activation = TRUE;
            }

            rr_timer_cancel(RR_PLMN_LIST_SEARCH_GUARD_TIMER, gas_id);
            rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS_ABORTING;
            MSG_GERAN_HIGH_0_G(" wait for ABORT_CNF in RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS_ABORTING, NAS abort received");
            rr_event = EV_NO_EVENT; /*absorb in this SM*/
            break;
          } /* EV_PLMN_SEARCH_ABORT */
          default:; /*handle all other signals in rr_control*/
        }
        break;
      }

      case RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS_ABORTING:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch (message_id)
                {
                  case TDSRRC_INTERRAT_PLMN_SRCH_CNF:
                  {
                    if ((rr_plc_data_ptr->aborting_no_multirat_activation == FALSE) &&
                        (get_rr_state(gas_id) != RR_GOING_INACTIVE))
                    {
                      /* Race condition where RRC send PLMN search CNF just as RR sent the Abort CNF.
                         The abort cnf will be ignored in this case.*/
                      tdsrrc_interrat_plmn_srch_cnf_type *plmn_srch_cnf =
                        (tdsrrc_interrat_plmn_srch_cnf_type *)message_ptr;

                      MSG_GERAN_MED_2_G(
                        "SGLTE PLC: received TDSRRC_INTERRAT_PLMN_SRCH_CNF, srch_done=%d, hplmn_found=%d",
                        plmn_srch_cnf->srch_done,
                        plmn_srch_cnf->hplmn_found
                      );

                      rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
                      rr_plc_data_ptr->rat_search_info_current->available_plmn_list =
                        plmn_srch_cnf->available_plmn_list;

                      if (plmn_srch_cnf->hplmn_found)
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN found");
                        rr_plc_data_ptr->rat_search_info_current->hplmn_found = TRUE;
                      }
                      else
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN not found");
                      }

                      rr_send_mph_ue_mode_change_req(FALSE,gas_id);
                      rr_event = EV_NO_EVENT; /*absorb in this SM*/
                      rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_ACTIVATION;
                    }
                    else
                    {
                      tdsrrc_interrat_plmn_srch_cnf_type *plmn_srch_cnf =
                        (tdsrrc_interrat_plmn_srch_cnf_type *)message_ptr;

                      MSG_GERAN_MED_2_G(
                        "SGLTE PLC: received TDSRRC_INTERRAT_PLMN_SRCH_CNF, srch_done=%d, hplmn_found=%d",
                        plmn_srch_cnf->srch_done,
                        plmn_srch_cnf->hplmn_found
                      );

                      rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_NULL;
                      rr_plc_search_complete(gas_id);
                      rr_event = EV_PLMN_SEARCH_COMPLETE;
                      rr_plc_data_ptr->aborting_no_multirat_activation = FALSE;
                    }
                    break;
                  } /* TDSRRC_INTERRAT_PLMN_SRCH_CNF */

                  case TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF:
                  {
                    if ((rr_plc_data_ptr->aborting_no_multirat_activation == FALSE) &&
                        (get_rr_state(gas_id) != RR_GOING_INACTIVE))
                    {
                      MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF rxd");

                      rr_send_mph_ue_mode_change_req(FALSE,gas_id);
                      rr_event = EV_NO_EVENT; /*absorb in this SM*/
                      rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_ACTIVATION;
                    }
                    else
                    {
                      MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF rxd");

                      rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_NULL;
                      rr_plc_search_complete(gas_id);
                      rr_event = EV_PLMN_SEARCH_COMPLETE;
                      rr_plc_data_ptr->aborting_no_multirat_activation = FALSE;
                    }
                    break;
                  }
                  default:; /*handle all other signals in rr_control*/
                }
                break;
              } /* MS_RRC_RR */
#endif /*FEATURE_GSM_TDS*/

              case MS_MSGR_RR:
              {
                switch (message_ptr->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case LTE_RRC_G_PLMN_SRCH_RSP:
                  {
                    if ((rr_plc_data_ptr->aborting_no_multirat_activation == FALSE) &&
                        (get_rr_state(gas_id) != RR_GOING_INACTIVE))
                    {
                      /* Race condition where RRC send PLMN search CNF just as RR sent the Abort CNF.
                         The abort cnf will be ignored in this case.*/

                      lte_irat_plmn_srch_rsp_s * plmn_srch_rsp = &message_ptr->rr_msgr_msg.message.lte_irat_plmn_srch_rsp;

                      MSG_GERAN_MED_2_G("SGLTE PLMN List Search: received LTE_RRC_G_PLMN_SRCH_CNF, srch_done=%d, hplmn_found=%d",
                                         plmn_srch_rsp->srch_done,
                                         plmn_srch_rsp->hplmn_found);

                      rr_plc_data_ptr->rat_search_info_current->search_done = TRUE;
                      rr_plc_data_ptr->rat_search_info_current->available_plmn_list = plmn_srch_rsp->available_plmn_list;
                      rr_plc_data_ptr->rat_search_info_current->detected_frequency_list = plmn_srch_rsp->detected_frequency_list;

                      if (plmn_srch_rsp->hplmn_found)
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN found");
                        rr_plc_data_ptr->rat_search_info_current->hplmn_found = TRUE;
                      }
                      else
                      {
                        MSG_GERAN_HIGH_0_G("HPLMN not found");
                      }

                      rr_send_mph_ue_mode_change_req(FALSE,gas_id);
                      rr_event = EV_NO_EVENT; /*absorb in this SM*/
                      rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_ACTIVATION;
                    }
                    else
                    {
                      rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_NULL;
                      rr_plc_search_complete(gas_id);
                      rr_event = EV_PLMN_SEARCH_COMPLETE;
                      rr_plc_data_ptr->aborting_no_multirat_activation = FALSE;
                    }
                    break;
                  } /* LTE_RRC_G_PLMN_SRCH_CNF */

                  case LTE_RRC_G_ABORT_PLMN_SRCH_RSP:
                  {
                    if ((rr_plc_data_ptr->aborting_no_multirat_activation == FALSE) &&
                        (get_rr_state(gas_id) != RR_GOING_INACTIVE))
                    {
                      MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN LTE_RRC_G_ABORT_PLMN_SRCH_RSP rxd");

                      rr_send_mph_ue_mode_change_req(FALSE,gas_id);
                      rr_event = EV_NO_EVENT; /*absorb in this SM*/
                      rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_ACTIVATION;
                    }
                    else
                    {
                      MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN LTE_RRC_G_ABORT_PLMN_SRCH_RSP rxd");

                      rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_NULL;
                      rr_plc_search_complete(gas_id);
                      rr_event = EV_PLMN_SEARCH_COMPLETE;
                      rr_plc_data_ptr->aborting_no_multirat_activation = FALSE;
                    }
                    break;
                  }
#endif /* FEATURE_LTE */

                  default:; /*handle all other signals in rr_control*/
                }
                break;
              } /* MS_MSGR_RR */
              default:; /*handle all other signals in rr_control*/
            }
            break;
          } /* EV_INPUT_MESSAGE */
          default:; /*handle all other signals in rr_control*/
        }
        break;
      }

      case RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_ACTIVATION:
      {
        switch(rr_event)
        {
          case  EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_UE_MODE_CHANGE_CNF:
                  {
                    MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN MPH_UE_MODE_CHANGE_CNF rxd");
                    rr_plc_data_ptr->sglte_g2x_state = RR_PLC_SGLTE_G2X_STATE_NULL;

                    rr_plc_search_complete(gas_id);

                      rr_l1_update_wcdma_neighbor_list(gas_id);

                    rr_event = EV_PLMN_SEARCH_COMPLETE;
                    break;
                  }
                  default:; /*handle all other signals in rr_control*/
                }
                break;
              }
              default:; /*handle all other signals in rr_control*/
            }
            break;
          }

          case EV_PLMN_SEARCH_ABORT:
          case EV_PLMN_SEARCH_ABORT_NO_MULTIRAT_ACT:
          {
            /*Wait in state for mode change cnf*/
            if (rr_event == EV_PLMN_SEARCH_ABORT_NO_MULTIRAT_ACT)
            {
              rr_plc_data_ptr->aborting_no_multirat_activation = TRUE;
            }
            
            rr_timer_cancel(RR_PLMN_LIST_SEARCH_GUARD_TIMER, gas_id);            
            break;
          }
          default:; /*handle all other signals in rr_control*/
        }
        break;
      }
      default:; /*handle all other signals in rr_control*/
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((rr_event != new_event) && (new_event != EV_NO_EVENT)) ||
        (rr_plc_data_ptr->old_sglte_g2x_state != rr_plc_data_ptr->sglte_g2x_state))
    {
      rr_store_trace_msg_buf(RR_SGLTE_BPLMN_SM,
                             new_event,
                             (byte) rr_plc_data_ptr->sglte_g2x_state,
                             message_ptr, gas_id);
    }
#endif

    if (rr_plc_data_ptr->old_sglte_g2x_state != rr_plc_data_ptr->sglte_g2x_state)
    {
      RR_LOG_STATE_TRANSITION("rr_plc_sglte_control",
                              rr_plc_data_ptr->old_sglte_g2x_state,
                              rr_plc_data_ptr->sglte_g2x_state,
                              rr_sglte_g2x_plc_state_name,
                              gas_id);

      rr_plc_data_ptr->old_sglte_g2x_state = rr_plc_data_ptr->sglte_g2x_state;
    }

    if (new_event != EV_NO_EVENT)
    {
      rr_event = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      break; /*return the rr_event to rr_control*/
    }
  }

  return rr_event;
}

/**
  @brief Determines if SGLTE PLMN search is ongoing from the
         PLMN search state machine.

  @return TRUE if PLMN search is active in SGLTE mode, FALSE if not.
*/

boolean rr_is_sglte_plmn_srch_active(const gas_id_t gas_id)
{
  rr_plc_data_t *rr_plc_data_ptr;
  boolean        search_active = FALSE;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  switch (rr_plc_data_ptr->sglte_g2x_state)
  {
    case RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_DEACT:
    case RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_DEACT_ABORTING:
    case RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS:
    case RR_PLC_SGLTE_G2X_STATE_SEARCH_IN_PROGRESS_ABORTING:
    case RR_PLC_SGLTE_G2X_STATE_RAT_ABORTING:
    case RR_PLC_SGLTE_G2X_STATE_WAIT_L1_MULTIRAT_ACTIVATION:
    {
      search_active = TRUE;
      break;
    }

    default:
    {
      search_active = FALSE;
      break;
    }
  }

  return search_active;
}

boolean rr_x2g_bplmn_search_active(const gas_id_t gas_id)
{
  rr_plc_data_t *rr_plc_data_ptr;
  boolean search_active = FALSE;

  // Obtain a pointer to the module data
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);

  switch (rr_plc_data_ptr->search_type)
  {
#ifdef FEATURE_WCDMA
    case RR_PLC_WTOG_SEARCH:
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
    case RR_PLC_LTOG_SEARCH:
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
    case RR_PLC_TTOG_SEARCH:
#endif /*FEATURE_GSM_TDS*/
    case RR_PLC_SGLTE_X2G_FOREGROUND_SEARCH:
    case RR_PLC_SGLTE_X2G_BACKGROUND_SEARCH:
      search_active = TRUE;
    break;

    default:
    break;
  }

  return search_active;
}

#endif /*FEATURE_SGLTE*/

/*----------------------------------------------------------------------------
 * Public APIs
 * -------------------------------------------------------------------------*/

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
/*!
 * \brief Multi-SIM API to obtain partial PLMN search results.
 *
 * \param available_plmn_list_ptr (out)
 * \param as_id (in)
 *
 * \return boolean - TRUE if results valid, FALSE otherwise
 */
boolean rr_get_inter_rat_bplmn_prtl_result_sub_id(
  sys_detailed_plmn_list_s_type *available_plmn_list_ptr,
  sys_modem_as_id_e_type as_id
)
{
#ifdef FEATURE_MPLMN_PARTIAL_UPDATING

  sys_detailed_plmn_list_s_type *plmn_list_ptr;
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  rr_available_plmn_db_T *rr_available_plmn_db_ptr = rr_get_available_plmn_db_ptr(gas_id);

  // Sanity-check the input
  RR_NULL_CHECK_RETURN_PARAM(available_plmn_list_ptr, FALSE);

  available_plmn_list_ptr->length = 0;

  RR_NULL_CHECK_RETURN_PARAM(rr_plmn_list_req_info_ptr, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(rr_available_plmn_db_ptr, FALSE);

  // Use temporary storage to construct the list
  plmn_list_ptr = GPRS_MEM_CALLOC(sizeof(sys_detailed_plmn_list_s_type));

  rr_plc_construct_plmn_list(
    rr_available_plmn_db_ptr,      // plmn_db_ptr
    plmn_list_ptr,                 // plmn_list_ptr
    (rr_plmn_list_req_info_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
     ? NO_CHECK
     : CHECK_FORBIDDEN_LAI_LIST,   // check_forbidden_lai
    gas_id                         // gas_id
  );

  // Copy the temporary list into the supplied output list, filtering as necessary
  rr_plc_filter_plmn_list_if_necessary(
    available_plmn_list_ptr,   // filtered_available_plmn_list
    plmn_list_ptr,             // available_plmn_list
    gas_id                     // gas_id
  );

#ifdef PERLUTF
  #error code not present
#endif // PERLUTF

  // Free the temporary storage
  GPRS_MEM_FREE(plmn_list_ptr);

  if (available_plmn_list_ptr->length > 0)
  {
    return TRUE;
  }

#else // FEATURE_MPLMN_PARTIAL_UPDATING
#ifdef FEATURE_DUAL_SIM
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);
#endif /*FEATURE_DUAL_SIM*/
  (void)available_plmn_list_ptr;
  (void)as_id;
  MSG_GERAN_ERROR_0_G("PLMN search partial results feature not enabled");

#endif // FEATURE_MPLMN_PARTIAL_UPDATING

  return FALSE;
}

/*!
 * \brief API to obtain partial PLMN search results.
 *
 * \param available_plmn_list_ptr (out)
 *
 * \return boolean - TRUE if results valid, FALSE otherwise
 */
boolean rr_get_inter_rat_bplmn_prtl_result(
  sys_detailed_plmn_list_s_type *available_plmn_list_ptr
)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  sys_modem_as_id_e_type as_id = geran_get_multi_mode_sub();
  MSG_GERAN_HIGH_1("SingleSIM API called, AS-ID=%d assumed",as_id);
#else
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;
#endif // defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)

  // Redirect to multi-SIM version indicating sub
  return rr_get_inter_rat_bplmn_prtl_result_sub_id(available_plmn_list_ptr, as_id);
}
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*===========================================================================

FUNCTION     rr_ms_set_plc_gsm_aborted

DESCRIPTION  Set the PLC control as aborted

DEPENDENCIES None

RETURN VALUE  None
SIDE EFFECTS 

===========================================================================*/
void rr_ms_set_plc_gsm_aborted(const gas_id_t gas_id)
{
  rr_plc_data_t *rr_plc_data_ptr;
  rr_plc_data_ptr = rr_plc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_plc_data_ptr);
  MSG_GERAN_ERROR_0_G("Set PLMN search as aborted");    
  rr_plc_data_ptr->abort = TRUE;
}

#endif /*FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM*/
/* EOF */

