/*! \file rr_pscan.c

  This module implements RR functionality related to requesting and processing the results of
  power-scans by L1.

  A set of functions are provided to encapsulate sending a variety of different power-scan requests
  to L1 and perform generic processing when L1 indicates that it has completed the power scan.

                Copyright (c) 2007-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_pscan.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_pscan.h"
#include "rr_acq_db.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_l1_send.h"
#include "rr_log.h"
#include "rr_general.h"
#include "rr_arfcn.h"
#include "rr_cell_selection.h"
#include "cmapi.h"
#include "rr_multi_sim.h"
#include "gprs_mem.h"
#include "rr_mode.h"
#include "rr_nv.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/* Even though we are a quad-band UE, we split the bands we support up in to
6 separate bands: PGSM 900, EGSM 900 (lower range), EGSM 900 (upper range),
DCS 1800, PCS 1900, Cellular 850 */
#define RR_PSCAN_BAND_DATA_SIZE 6

/* marker value for ARFCNs which should not be filtered after ACQ DB scan */
#define RR_MS_BLOCKED_BCCH_RXLEV_OFFSET 0xBB

#define RR_BAND_BITMAP_NONE ((rr_l1_band_bitmap_T)0x00)

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef struct
{
  /* Name used in debug strings. */
  char *                    name;

  /* The value set in ARFCN.band for this band. */
  sys_band_T                sys_band;

  /* The value used in band_pref to indicate support for this band. */
  sys_band_mask_type        sys_band_mask;

  /* The mask that should be added to the band bitmap when sending power scan
  request to L1 if this band is included. */
  rr_l1_band_bitmap_bands_T l1_band;

  /* The minimum value of ARFCN for this band. */
  uint16                    channel_min;

  /* The maximum value of ARFCN for this band. */
  uint16                    channel_max;
} rr_pscan_band_data_t;

/**
 * PSCAN states, possible state transitions:
 * INACTIVE -> IN_PROGRESS -> INACTIVE;
 * INACTIVE -> IN_PROGRESS -> ABORT -> INACTIVE;
 */
typedef enum
{
  RR_PSCAN_INACTIVE,
  RR_PSCAN_IN_PROGRESS,
  RR_PSCAN_ABORT
} rr_pscan_state_e;

/**
 * PSCAN related data
 */
typedef struct
{
  rr_pscan_state_e   state;
  rr_pscan_state_e   old_state;
  rr_search_mode_T   search_mode;
  sys_band_mask_type band_pref;
} rr_pscan_control_data_t;

typedef struct
{
  boolean              pending_request;
  word                 num_frequencies;
  word                 storage_limit;
  rr_l1_freq_power_T  *meas;
} rr_common_results_T;

typedef struct
{
  rr_pscan_control_data_t rr_pscan_control_data;
  rr_pscan_results_db_T   rr_pscan_results_db;
  boolean                 rr_pscan_last_acq_db_freqs_valid;
  rr_l1_pscan_results_T   rr_pscan_last_acq_db_freqs;
  rr_common_results_T     common_results;
} rr_pscan_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

// Note: PSCAN data capture is only supported for Sub1
struct
{
  boolean  enabled;
  uint32  *buffer_ptr;
  uint16   max_num_of_entries;
  uint16   num_of_entries;
  gas_id_t enabled_gas_id;
} rr_pscan_data_capture_data;

static rr_pscan_data_t rr_pscan_data[NUM_GERAN_DATA_SPACES];

static const rr_pscan_band_data_t rr_pscan_band_data[RR_PSCAN_BAND_DATA_SIZE] =
{
  {
    "PGSM 900",
    SYS_BAND_PGSM_900,
    SYS_BAND_MASK_GSM_PGSM_900,
    RR_L1_BAND_BITMAP_PGSM,
    RR_ARFCN_PGSM_900_MIN,
    RR_ARFCN_PGSM_900_MAX
  },
  {
    "EGSM 900 (lower range)",
    SYS_BAND_EGSM_900,
    SYS_BAND_MASK_GSM_EGSM_900,
    RR_L1_BAND_BITMAP_EGSM,
    RR_ARFCN_EGSM_900_LOWER_RANGE_MIN,
    RR_ARFCN_EGSM_900_LOWER_RANGE_MAX
  },
  {
    "EGSM 900 (upper range)",
    SYS_BAND_EGSM_900,
    SYS_BAND_MASK_GSM_EGSM_900,
    RR_L1_BAND_BITMAP_EGSM,
    RR_ARFCN_EGSM_900_UPPER_RANGE_MIN,
    RR_ARFCN_EGSM_900_UPPER_RANGE_MAX
  },
  {
    "DCS 1800",
    SYS_BAND_DCS_1800,
    SYS_BAND_MASK_GSM_DCS_1800,
    RR_L1_BAND_BITMAP_DCS_1800,
    RR_ARFCN_DCS_1800_MIN,
    RR_ARFCN_DCS_1800_MAX
  },
  {
    "PCS 1900",
    SYS_BAND_PCS_1900,
    SYS_BAND_MASK_GSM_PCS_1900,
    RR_L1_BAND_BITMAP_PCS_1900,
    RR_ARFCN_PCS_1900_MIN,
    RR_ARFCN_PCS_1900_MAX
  },
  {
    "Cellular 850",
    SYS_BAND_CELL_850,
    SYS_BAND_MASK_GSM_850,
    RR_L1_BAND_BITMAP_GSM_850,
    RR_ARFCN_CELL_850_MIN,
    RR_ARFCN_CELL_850_MAX
  },
};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_pscan_data_t*
 */
rr_pscan_data_t *rr_pscan_data_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_pscan_data[as_index];
}

/*!
 * \brief Returns the max number of freq to monitor based on the bands being searched.
 *
 * \param current_band_group
 * \param gas_id
 *
 * \return word
 */
static word rr_get_num_freq_monitor(
  sys_band_mask_type current_band_group,
  const gas_id_t gas_id
)
{
  /*Returns the number of freq to monitor depending on which bands are searched*/
  word num_freq = 0;
  rr_pscan_data_t *rr_pscan_data_ptr;

  // Obtain a pointer to the module data
  rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  /* PGSM or EGSM 900 */
  if((current_band_group & (sys_band_mask_e_type)SYS_BAND_MASK_GSM_PGSM_900) ||
     (current_band_group & (sys_band_mask_e_type)SYS_BAND_MASK_GSM_EGSM_900))
  {
    num_freq += RR_TOT_FREQ_MONITOR_GSM_900;
  }

  /* DCS 1800 */
  if(current_band_group & (sys_band_mask_e_type) SYS_BAND_MASK_GSM_DCS_1800)
  {
    num_freq += RR_TOT_FREQ_MONITOR_DCS_1800;
  }

  /* PCS 1900 */
  if(current_band_group & (sys_band_mask_e_type) SYS_BAND_MASK_GSM_PCS_1900)
  {
    num_freq += RR_TOT_FREQ_MONITOR_PCS_1900;
  }

  /* Cellular 850 */
  if(current_band_group & (sys_band_mask_e_type) SYS_BAND_MASK_GSM_850)
  {
    num_freq += RR_TOT_FREQ_MONITOR_GSM_850;
  }

  /* If the count goes beyond 150  */
  if (num_freq > RR_NUM_FREQ_MONITOR)
  {
    /* max=0;
      if GSM900, max+=35;
      if GSM850, max+=35;
      if GSM1800, max+=40;
      if GSM1900, max+=40;
      max is bound to 150
    */
    num_freq = RR_NUM_FREQ_MONITOR;
  }

  return num_freq;
} /* rr_get_num_freq_monitor */

/*!
 * \brief Returns a pointer to a text string representing the given state
 *
 * \param pscan_state
 *
 * \return char*
 */
static char * rr_pscan_state_name(
  rr_pscan_state_e pscan_state
)
{
  char * s = "";

  switch( pscan_state )
  {
    case RR_PSCAN_INACTIVE:
      s = "Inactive";
      break;

    case RR_PSCAN_IN_PROGRESS:
      s = "InProgress";
      break;

    case RR_PSCAN_ABORT:
      s = "Abort";
      break;

    default:
      s = "Unknown";
      break;
  }

  return s;
} /* rr_pscan_state_name */

/*!
 * \brief Update the state of the PSCAN control state machine.
 *
 * \param new_state
 * \param gas_id
 */
static void rr_pscan_state_transition(
  rr_pscan_state_e new_state,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_pscan_data_t *rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  MSG_GERAN_HIGH_2_G("RR_PSCAN state: from %s to %s",
    rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state),
    rr_pscan_state_name(new_state));

  rr_pscan_data_ptr->rr_pscan_control_data.state = new_state;
}

/*!
 * \brief Initialisation function.
 *
 * \param gas_id
 */
static void rr_pscan_init(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_pscan_data_t *rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  memset(&rr_pscan_data_ptr->rr_pscan_control_data, 0, sizeof(rr_pscan_control_data_t));
  memset(&rr_pscan_data_ptr->rr_pscan_results_db, 0, sizeof(rr_pscan_data_ptr->rr_pscan_results_db));
  memset(&rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs, 0, sizeof(rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs));

  rr_pscan_data_ptr->rr_pscan_control_data.state = RR_PSCAN_INACTIVE;

  return;
}

/*!
 * \brief called when PSCAN data capture is enabled by CM. The buffer supplied by CM is initialised.
 *
 * \param memptr - pointer to the buffer where the PSCAN data should be stored
 * \param max_num_of_entries max_num_of_entries - max num of entries that can be stored in the buffer
 */
static void rr_pscan_enable_pscan_data_capture(
  uint32 *memptr,
  uint16 max_num_of_entries,
  gas_id_t gas_id
)
{
  int i;

  MSG_GERAN_HIGH_0_G("PSCAN data capture enabled");

  rr_pscan_data_capture_data.enabled            = TRUE;
  rr_pscan_data_capture_data.buffer_ptr         = memptr;
  rr_pscan_data_capture_data.max_num_of_entries = max_num_of_entries;
  rr_pscan_data_capture_data.num_of_entries     = 0;
  rr_pscan_data_capture_data.enabled_gas_id     = gas_id;

  /*
  1st 32-bit word is a header
    0-15 : CMAPI_SYS_MODE_GSM
   16-31 : num of freqs
  */
  (*memptr) = (uint16) CMAPI_SYS_MODE_GSM;  // Intialise with num_of_freqs=0
  ++memptr;

  /*
  Remaining buffer is array of 32-bit words containing scan result
    0-15 : arfcn
   16-23 : band
   24-31 : rssi
  */
  for (i = 0; i < max_num_of_entries; i++)
  {
    memptr[i] = 0x0000ffff;  // Initialise entry to invalid ARFCN
  }

  return;
}

/*!
 * \brief Called when PSCAN data capture is disabled by CM. Local data related to this feature are reset.
 */
static void rr_pscan_disable_pscan_data_capture(void)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  gas_id_t gas_id;

  gas_id = rr_pscan_data_capture_data.enabled_gas_id;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  MSG_GERAN_HIGH_0_G("PSCAN data capture disabled");

  rr_pscan_data_capture_data.enabled            = FALSE;
  rr_pscan_data_capture_data.buffer_ptr         = NULL;
  rr_pscan_data_capture_data.max_num_of_entries = 0;
  rr_pscan_data_capture_data.num_of_entries     = 0;
  rr_pscan_data_capture_data.enabled_gas_id     = GERAN_ACCESS_STRATUM_ID_UNDEFINED;

  return;
}

/*!
 * \brief If PSCAN data capture is enabled, the data is copied to the buffer provided by CM.
 *
 *  Called when PSCAN data is available. If PSCAN data capture is enabled, the data is copied to the buffer
 *  provided by CM.
 *
 * \param pscan_db_ptr - PSCAN database
 * \param pscan_db_offset - PSCAN database offset
 */
static void rr_pscan_data_capture(
  rr_pscan_results_db_T *pscan_db_ptr,
  uint16 pscan_db_offset,
  gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(pscan_db_ptr);

  if (rr_pscan_data_capture_enabled(gas_id) &&
      (rr_pscan_data_capture_data.buffer_ptr != NULL))
  {
    uint16 arfcn;
    uint8 band;
    int8 rssi;
    uint16 num_of_freqs;
    uint16 pscan_db_index;
    uint32 *capture_buffer_ptr;
    uint16 capture_buffer_index;
    boolean arfcn_found;

    /*
    rr_pscan_data_capture_data.buffer_ptr is the buffer provided by CM to place the scan results.

    Format is as follows:

    First 32 bits of scan data memory (common to all RATs)
      1st 16 bits: cmapi_sys_mode_e RAT type typecast to uint16 (e.g. set to CMAPI_SYS_MODE_GSM)
      2nd 16 bits: number of following 32 bit words filled by the RAT
    */
    struct capture_buffer_header_s
    {
      uint16 sys_mode;
      uint16 num_of_freqs;
    } *header_ptr;

    /*
    Remaining 32 bit words contain the scan data
    */
    struct capture_buffer_entry_s
    {
      uint16 arfcn;
      uint8 band;
      int8 rssi;
    } *capture_buffer_entry_ptr;

    // Pointer to the 32-bit word 'header'
    header_ptr = (struct capture_buffer_header_s *)rr_pscan_data_capture_data.buffer_ptr;

    // Pointer to initial position to store scan data
    capture_buffer_ptr = rr_pscan_data_capture_data.buffer_ptr + 1;

    // Loop thru the frequencies in the pscan database, starting at the offset indicated
    pscan_db_index = pscan_db_offset;
    num_of_freqs = pscan_db_ptr->pscan_results.num_frequencies;

    while (pscan_db_index < num_of_freqs)
    {
      arfcn = (uint16) pscan_db_ptr->pscan_results.meas[pscan_db_index].arfcn.num;
      band = (uint8) pscan_db_ptr->pscan_results.meas[pscan_db_index].arfcn.band;
      rssi = (int8) rxlev_to_dbm(pscan_db_ptr->pscan_results.meas[pscan_db_index].RXLEV_average);

      MSG_GERAN_MED_3("PSCAN data capture entry: arfcn=%d band=%d rssi=%d",(int)arfcn,(int)band,(int)rssi);

      arfcn_found = FALSE;

      // Scan through the data capture buffer to check if there is an entry for this ARFCN already
      for (capture_buffer_index = 0;
           capture_buffer_index < rr_pscan_data_capture_data.num_of_entries;
           capture_buffer_index++)
      {
        capture_buffer_entry_ptr =
          (struct capture_buffer_entry_s *)capture_buffer_ptr + capture_buffer_index;

        if ((capture_buffer_entry_ptr->arfcn == arfcn) &&
            (capture_buffer_entry_ptr->band == band))
        {
          MSG_GERAN_MED_2("arfcn=%d band=%d already captured, updating rssi",(int)arfcn,(int)band);

          // Update the RSSI for this ARFCN
          capture_buffer_entry_ptr->rssi = rssi;

          // Mark ARFCN as found and exit loop
          arfcn_found = TRUE;
          break;
        }
      }

      if (!arfcn_found)
      {
        // This is a new ARFCN, not present in the capture buffer

        // Check if there is any room left in the capture buffer
        if (rr_pscan_data_capture_data.num_of_entries < rr_pscan_data_capture_data.max_num_of_entries)
        {
          capture_buffer_entry_ptr =
            (struct capture_buffer_entry_s *)capture_buffer_ptr + rr_pscan_data_capture_data.num_of_entries;

          // Store an entry in the capture buffer
          capture_buffer_entry_ptr->arfcn = arfcn;
          capture_buffer_entry_ptr->band = band;
          capture_buffer_entry_ptr->rssi = rssi;

          // Increment the number of entries
          ++rr_pscan_data_capture_data.num_of_entries;
        }
        else
        {
          MSG_GERAN_HIGH_0("PSCAN data capture buffer full");
          break;
        }
      }

      ++pscan_db_index;
    }

    // Update the number of entries into the header
    header_ptr->num_of_freqs = rr_pscan_data_capture_data.num_of_entries;
  }
}

/**
 * Detect whether an entry in the acquisition DB results is marked as "blocked".
 * @return TRUE if the ARFCN can be safely filtered from the next scan process
 */
static boolean rr_pscan_ok_to_filter(
  rr_l1_freq_power_T *arfcn_meas_ptr
)
{
#ifdef FEATURE_DUAL_SIM
  if ((arfcn_meas_ptr->RXLEV_average >= RR_MS_BLOCKED_BCCH_RXLEV_OFFSET+0) &&
      (arfcn_meas_ptr->RXLEV_average <= RR_MS_BLOCKED_BCCH_RXLEV_OFFSET+63))
  {
    /* DSDS controller has decided to prevent filtering of the ARFCN */
    /* because BCCH decode attempts were blocked by another activity */
    /* restore the original RXLEV measurement here before returning  */
    arfcn_meas_ptr->RXLEV_average -= RR_MS_BLOCKED_BCCH_RXLEV_OFFSET;
    return(FALSE);
  }
#endif /* FEATURE_DUAL_SIM */

  return(TRUE);
}

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Mark an entry in the acquisition DB results as "blocked".
 *
 *  An entry marked as "blocked" will NOT be filtered from any subsequent scans.
 *
 * \param blocked_arfcn
 * \param gas_id
 */
void rr_ms_pscan_mark_arfcn_blocked(
  ARFCN_T blocked_arfcn,
  const gas_id_t gas_id
)
{
  uint16 j;
  rr_pscan_data_t *rr_pscan_data_ptr;

  // Obtain a pointer to the module data
  rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  if (rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs_valid == FALSE)
  {
    /* no valid scan results for acquisition database, exit now */
    return;
  }

  for (j = 0; j < rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs.num_frequencies; j++)
  {
    if (rr_arfcn_compare_freq(blocked_arfcn, rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs.meas[j].arfcn))
    {
      rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs.meas[j].RXLEV_average += RR_MS_BLOCKED_BCCH_RXLEV_OFFSET;
      break;
    }
  }

  MSG_GERAN_HIGH_2_G("ARFCN(%d,%d) marked as blocked",
                     (int)blocked_arfcn.num, (int)blocked_arfcn.band);
  return;
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*!
 * \brief Determines L1 band bitmap for a specified ARFCN
 *
 * \param arfcn (in)
 */
static rr_l1_band_bitmap_T rr_pscan_get_band_bitmap_for_arfcn(ARFCN_T arfcn)
{
  switch(arfcn.band)
  {
    case SYS_BAND_PGSM_900:
      return((byte)RR_L1_BAND_BITMAP_PGSM);

    case SYS_BAND_EGSM_900:
      return((byte)RR_L1_BAND_BITMAP_EGSM);

    case SYS_BAND_DCS_1800:
      return((byte)RR_L1_BAND_BITMAP_DCS_1800);

    case SYS_BAND_PCS_1900:
      return((byte)RR_L1_BAND_BITMAP_PCS_1900);

    case SYS_BAND_CELL_850:
      return((byte)RR_L1_BAND_BITMAP_GSM_850);

    default:
      MSG_GERAN_ERROR_1("Unknown band %d in power scan setup", arfcn.band);
      break;
  }

  return(0);
}

/*!
 * \brief Clears the power-scan database.
 *
 * \param pscan_db_ptr
 * \param gas_id
 */
void rr_pscan_clear_results(
  rr_pscan_results_db_T *pscan_db_ptr
)
{
  pscan_db_ptr->pscan_results.num_frequencies=0;
} /* rr_pscan_clear_results */

/*!
 * \brief Adds a list of frequences to the power-scan database.
 *
 * \param arfcn_list
 * \param no_of_entries
 * \param gas_id
 */
void rr_pscan_add_frequencies(
  ARFCN_T *arfcn_list,
  uint16 no_of_entries,
  const gas_id_t gas_id
)
{
  rr_l1_band_bitmap_T    band_bitmap = 0;
  uint16                 index;
  rr_pscan_results_db_T *pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);

  for (index = 0; index < no_of_entries; index++)
  {
    pscan_results_db_ptr->pscan_results.meas[index].arfcn = arfcn_list[index];
    band_bitmap |= rr_pscan_get_band_bitmap_for_arfcn(arfcn_list[index]);
  }

  pscan_results_db_ptr->bands_searched = band_bitmap;
  pscan_results_db_ptr->pscan_results.num_frequencies = no_of_entries;

  return;
}

/*!
 * \brief Returns the number of entries in the power scan results database.
 *
 * \param pscan_db_ptr - pointer to power-scan database
 * \param gas_id
 *
 * \return word - Number of frequencies in the database
 */
word rr_pscan_get_num_frequencies(
  rr_pscan_results_db_T *pscan_db_ptr
)
{
  /* process the request even if the results have been invalidated */
  return pscan_db_ptr->pscan_results.num_frequencies;
} /* rr_pscan_get_num_frequencies */


/**
 * Stores the strongest N frequencies in the power scan starting from the
 * given offset into the given candidate cell database (which is completely
 * cleared before storing) in order of decreasing signal strength
 *
 * @param pscan_db_ptr      pointer to power scan database
 *
 * @param candidate_cell_db_ptr
 *                          pointer to candidate cell database
 *
 * @param power_scan_offset power scan offset (number of strongest frequencies to skip)
 *
 * @param num_frequencies   number of frequencies to extract from power scan
 *
 * @return actual number of frequencies that were extracted from power scan results
 */
word rr_pscan_store_strongest_frequencies_in_candidate_db(
  rr_pscan_results_db_T *pscan_db_ptr,
  rr_candidate_cell_db_T *candidate_cell_db_ptr,
  word power_scan_offset,
  word num_frequencies,
  rr_search_order_e search_order,
  const gas_id_t gas_id
)
{
  word pscan_num_frequencies = pscan_db_ptr->pscan_results.num_frequencies;
  word counter               = 0;
  byte gsm_counter           = 0;
  byte dcs_counter           = 0;
  byte pcs_counter           = 0;
  byte g850_counter          = 0;
  byte acq_db_intersperse_counter = 0;

  /* first clear the candidate database */
  rr_candidate_db_clear(candidate_cell_db_ptr);


  for (counter = 0;
       (counter + power_scan_offset) < pscan_num_frequencies;
       counter++)
  {

    if ((gsm_counter + dcs_counter + pcs_counter + g850_counter + acq_db_intersperse_counter) < num_frequencies)
    {
      word num = pscan_db_ptr->pscan_results.meas[counter + power_scan_offset].arfcn.num;
      rr_internal_band_T band = GET_ARFCN_BAND(pscan_db_ptr->pscan_results.meas[counter + power_scan_offset].arfcn);

      if( (((band == BAND_PGSM) && (num != 0)) || (band == BAND_EGSM))
          && (gsm_counter < RR_MAX_FREQ_MONITOR_GSM_900))
      {
        /* add the entry into the database */
        rr_candidate_db_add_frequency(candidate_cell_db_ptr,&pscan_db_ptr->pscan_results.meas[counter + power_scan_offset]);
        gsm_counter++;
      }
      else if((band == BAND_DCS) && (dcs_counter < RR_MAX_FREQ_MONITOR_DCS_1800))
      {
        /* add the entry into the database */
        rr_candidate_db_add_frequency(candidate_cell_db_ptr,&pscan_db_ptr->pscan_results.meas[counter + power_scan_offset]);
        dcs_counter++;
      }
      else if((band == BAND_PCS) && (pcs_counter < RR_MAX_FREQ_MONITOR_PCS_1900))
      {
        /* add the entry into the database */
        rr_candidate_db_add_frequency(candidate_cell_db_ptr,&pscan_db_ptr->pscan_results.meas[counter + power_scan_offset]);
        pcs_counter++;
      }
      else if((band == BAND_GSM_850) && (g850_counter < RR_MAX_FREQ_MONITOR_GSM_850))
      {
        /* add the entry into the database */
        rr_candidate_db_add_frequency(candidate_cell_db_ptr,&pscan_db_ptr->pscan_results.meas[counter + power_scan_offset]);
        g850_counter++;
      }
    }
    else /* once num_frequencies is reached */
    {
      break;
    }
  }

  MSG_GERAN_HIGH_3_G("#G900=%d, #G1800=%d, #G1900=%d added to candidate db", gsm_counter, dcs_counter, pcs_counter);
  MSG_GERAN_HIGH_1_G("#G850=%d added to candidate db", g850_counter);

  /* store the new power scan offset in the database */
  candidate_cell_db_ptr->power_scan_offset = power_scan_offset + counter;

  /* set the bands searched based on the power scan values */
  candidate_cell_db_ptr->bands_searched = pscan_db_ptr->bands_searched;

  /* return the total */
  return(gsm_counter + dcs_counter + pcs_counter + g850_counter + acq_db_intersperse_counter);
} /* rr_pscan_store_strongest_frequencies_in_candidate_db */

/**
 * Returns the strongest N frequencies in the power scan database that are
 * also in the given frequency list. The resulting frequencies are returned
 * in the given output buffer
 *
 * @param pscan_db_ptr         pointer to power scan database
 *
 * @param num_frequencies      number of frequencies to extract from the power scan results
 *
 * @param list_num_frequencies number of frequencies in the provided list
 *
 * @param frequency_list       array of frequencies to search for in power scan
 *
 * @param result_buffer_ptr    Result where the frequencies/powers found are stored. Should have enough space
 *                             for the number of frequencies requested
 *
 * @return number of frequencies found
 */
int rr_pscan_get_strongest_frequencies_in_list(
  rr_pscan_results_db_T *pscan_db_ptr,
  int                   num_frequencies,
  int                   list_num_frequencies,
  ARFCN_T               *frequency_list,
  rr_l1_freq_power_T    *result_buffer_ptr
)
{
  int index;
  int foundIndex = 0;
  int listIndex;
  int no_of_entries = pscan_db_ptr->pscan_results.num_frequencies;

  /* search the power scan from start to bottom */
  for (index = 0; index < no_of_entries; ++index)
  {
    /* if the current entry is in the requested list */
    boolean found_in_list = FALSE;

    /* if the result buffer is full (all requested frequencies found) */
    if (foundIndex >= num_frequencies)
    {
      /* done with loop */
      break;
    }

    for (listIndex = 0; listIndex < list_num_frequencies; ++listIndex)
    {
      if (((pscan_db_ptr->pscan_results.meas[index].arfcn.num) ==
          frequency_list[listIndex].num)
          && ((pscan_db_ptr->pscan_results.meas[index].arfcn.band) ==
          frequency_list[listIndex].band)
         )
      {
        /* match was found */
        found_in_list = TRUE;
        break;
      }
    } /* foreach frequency in provided list */

    if (found_in_list)
    {
      /* add it to the results buffer */
      result_buffer_ptr[foundIndex] = pscan_db_ptr->pscan_results.meas[index];
      ++foundIndex;
    }
  } /* foreach entry in power scan database */

  return foundIndex;
} /* rr_pscan_get_strongest_frequencies_in_list */

/**
 * Invalidates the power scan results without clearing them
 */
void rr_invalidate_power_scan_results(const gas_id_t gas_id)
{
  if (rr_timer_cancel(RR_INVALIDATE_PSCAN_RESULTS_TIMER, gas_id))
  {
    MSG_GERAN_HIGH_0_G("Power scan results have been invalidated");
  }

  return;
} /* rr_invalidate_power_scan_results */

/**
 * Functionality: Discards storage for common power scan results
 *
 */
void rr_pscan_discard_common_results(const gas_id_t gas_id)
{
  rr_pscan_data_t *rr_pscan_data_ptr;

  // Obtain a pointer to the module data
  rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  if (rr_pscan_data_ptr->common_results.meas != NULL)
  {
    /* free up the memory for shared results */
    GPRS_MEM_FREE(rr_pscan_data_ptr->common_results.meas);
    rr_pscan_data_ptr->common_results.meas = NULL;
  }

  /* clear the corresponding flag and counts */
  rr_pscan_data_ptr->common_results.pending_request = FALSE;
  rr_pscan_data_ptr->common_results.num_frequencies = 0;
  rr_pscan_data_ptr->common_results.storage_limit   = 0;

  return;
}

/**
 * Functionality: Adds a single measurement result to a list of power measure
 *
 * @Params:  list_ptr - the list of measurements
 *           result_ptr - the new result
 *
 */
static rr_l1_band_bitmap_T rr_pscan_add_result(rr_l1_pscan_results_T *list_ptr, rr_l1_freq_power_T *result_ptr)
{
  if (list_ptr->num_frequencies < RR_MAX_PSCAN_FREQ_SCAN)
  {
    list_ptr->meas[list_ptr->num_frequencies] = *result_ptr;
    list_ptr->num_frequencies++;
    return(rr_pscan_get_band_bitmap_for_arfcn(result_ptr->arfcn));
  }

  return(RR_BAND_BITMAP_NONE);
}

/**
 * Functionality: Adds a single measurement result to the common result storage
 *
 * @Params:  rr_pscan_data_ptr - module data pointer
 *           result_ptr - the new result
 *
 */
static void rr_pscan_store_common_result(rr_pscan_data_t *pscan_ptr, rr_l1_freq_power_T *result_ptr)
{
  if (pscan_ptr->common_results.num_frequencies < pscan_ptr->common_results.storage_limit)
  {
    pscan_ptr->common_results.meas[pscan_ptr->common_results.num_frequencies] = *result_ptr;
    pscan_ptr->common_results.num_frequencies++;
  }
  else
  {
    MSG_GERAN_ERROR_0("Exceeded storage limit for shared power scan results");
  }

  return;
}

/**
 * Functionality: Inserts shared results into a power scan database
 *
 * @Params:  cnf_ptr - pointer to MPH_POWER_SCAN_CNF (for GAS ID determination)
 *
 */
static boolean rr_pscan_include_common_results(mph_power_scan_cnf_T *cnf_ptr)
{
  rr_pscan_data_t       *rr_pscan_data_ptr;
  rr_l1_freq_power_T    *common_data_ptr;
  rr_pscan_results_db_T *results_db_ptr;
  boolean                used_common;
  gas_id_t               gas_id;

  /* sanity check input parameters */
  if (cnf_ptr == NULL)
  {
    return(FALSE);
  }

  // Obtain a pointer to the module data
  gas_id = cnf_ptr->gas_id;
  rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);
  used_common = FALSE;

  common_data_ptr = rr_pscan_data_ptr->common_results.meas;
  results_db_ptr = &(rr_pscan_data_ptr->rr_pscan_results_db);

  /* if memory was available, process the list of common results */
  if (common_data_ptr != NULL)
  {
    if (results_db_ptr != NULL)
    {
      word arfcn_count;
      word arfcn_index;

      arfcn_count = rr_pscan_data_ptr->common_results.num_frequencies;
      for (arfcn_index = 0; arfcn_index < arfcn_count; arfcn_index++)
      {
        if (RR_BAND_BITMAP_NONE != rr_pscan_add_result(&(results_db_ptr->pscan_results), &(common_data_ptr[arfcn_index])))
        {
          /* the results will have at least one common entry in them */
          used_common = TRUE;
        }
      }

      /* finally, sort the combined list in order of signal strength, strongest first*/
      qsort((void *)results_db_ptr->pscan_results.meas,
            results_db_ptr->pscan_results.num_frequencies,
            sizeof(rr_l1_freq_power_T),
            rr_compare_freq_power);

      MSG_GERAN_HIGH_1_G("Power scan using %d shared results", arfcn_count);
    }

    /* free up the shared results */
    GPRS_MEM_FREE(common_data_ptr);
  }

  /* clear the corresponding flag, frequency count, and pointer */
  rr_pscan_data_ptr->common_results.pending_request = FALSE;
  rr_pscan_data_ptr->common_results.num_frequencies = 0;
  rr_pscan_data_ptr->common_results.storage_limit   = 0;
  rr_pscan_data_ptr->common_results.meas            = NULL;

  return(used_common);
}

/*!
 * \brief Starts RR_INVALIDATE_PSCAN_RESULTS_TIMER for power scan aging
 *
 * \param gas_id
 *
 */
static void rr_start_pscan_validity_timer(const gas_id_t gas_id)
{
  rex_timer_cnt_type timeout_ms = rr_nv_get_pscan_results_reuse_time_millisecs(RR_GAS_ID_TO_AS_ID);

  if (timeout_ms != 0)
  {
    MSG_GERAN_HIGH_1_G("Power scan results will be valid for %d ms", timeout_ms);

    rr_timer_start(RR_INVALIDATE_PSCAN_RESULTS_TIMER,   // timer_id
                   timeout_ms,                          // millisecs
                   gas_id);                             // gas_id
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Re-use of power scan results disabled via EFS");
  }

  return;
}

/**
 * Perform generic processing when a MPH_POWER_SCAN_CNF message is received
 */
void rr_process_mph_power_scan_cnf(mph_power_scan_cnf_T *msg_ptr)
{
  if (msg_ptr != NULL)
  {
    /* log the end of the power scan */
    rr_log_power_scan_completed(msg_ptr->gas_id);

    /* if the message indicates that the results are NOT invalid */
    /* start the timer that will allow them to be reused if required */
    if (msg_ptr->invalidate)
    {
      rr_pscan_discard_common_results(msg_ptr->gas_id);
      rr_invalidate_power_scan_results(msg_ptr->gas_id);
    }
    else
    {
      if (rr_pscan_include_common_results(msg_ptr))
      {
        /* common results were included, so ensure that data does not */
        /* continue to be reused by other subscriptions, ad infinitum */
        rr_invalidate_power_scan_results(msg_ptr->gas_id);
      }
      else
      {
        /* no common results were included, so start the validity timer */
        /* so that these real OTA results are available for reuse later */
        rr_start_pscan_validity_timer(msg_ptr->gas_id);
      }
    }
  }

  return;
} /* rr_process_mph_power_scan_cnf */

/*!
 * \brief This function must be called when receiving MPH_POWER_SCAN_CNF.
 *
 *  It manipulates power scan results from L1 (based on FEATUREs) and then populates the Candidate Cell DB
 *  with the results.
 *
 * \param search_order
 * \param gas_id
 *
 * \return word - number of frequencies found
 */
word rr_pscan_process_power_scan_results(
  rr_search_order_e search_order,
  const gas_id_t gas_id
)
{
  rr_candidate_cell_db_T *candidate_db_ptr = rr_get_candidate_cell_db_ptr(gas_id);
  uint16                  num_freq_to_monitor;
  uint16                  pscan_result_offset;
  uint16                  num_frequencies_found;
  uint16                  pscan_result_num_frequencies;
  rr_pscan_results_db_T  *pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);
  rr_pscan_data_t        *rr_pscan_data_ptr;

  // Obtain a pointer to the module data
  rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  if (candidate_db_ptr != NULL)
  {
    /* Attempt to do a parallel decode      */
    num_freq_to_monitor = rr_get_num_freq_monitor(rr_pscan_data_ptr->rr_pscan_control_data.band_pref, gas_id);
    pscan_result_offset = rr_candidate_db_get_power_scan_offset(candidate_db_ptr);
    pscan_result_num_frequencies = rr_pscan_get_num_frequencies(pscan_results_db_ptr);

    /* the offset in the candidate cell DB indicates where in the power scan results to search */
    MSG_GERAN_HIGH_3_G("Storing str.=%d, off=%d, pscan=%d",
             (int)num_freq_to_monitor,
             (int)pscan_result_offset,
             (int)pscan_result_num_frequencies);

    // PSCAN data capture is only supported for one subscription
    rr_pscan_data_capture(pscan_results_db_ptr, pscan_result_offset, gas_id);

    /* store the next strongest set of frequencies from the pscan into the candidate database */
    num_frequencies_found = rr_pscan_store_strongest_frequencies_in_candidate_db(
      pscan_results_db_ptr,
      candidate_db_ptr,
      pscan_result_offset,
      num_freq_to_monitor,
      search_order,
      gas_id
    );
  }
  else
  {
    num_frequencies_found = 0;
  }

  return(num_frequencies_found);
}

/*!
 * \brief Sends a power scan request to L1 for all frequencies in each band specified in 'band_pref'.
 *
 * \param search_mode
 * \param band_pref
 * \param filter_acq_db_freqs
 * \param gas_id
 *
 * \return rr_event_T - RR_EV_PERFORM_POWER_SCAN if a foreground power scan request is sent to L1
 *                    - RR_EV_BACKGROUND_PLC_STARTED if a background power scan request is sent to L1
 */
rr_event_T rr_perform_power_scan(
  rr_search_mode_T search_mode,
  sys_band_mask_type band_group,
  boolean filter_acq_db_freqs,
  const gas_id_t gas_id
)
{
  rr_candidate_cell_db_T *candidate_db_ptr = rr_get_candidate_cell_db_ptr(gas_id);
  rr_pscan_results_db_T  *pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);
  rr_l1_band_bitmap_T     band_bitmap = 0;
  ARFCN_T                 arfcn;
  uint16                  count = 0;
  uint16                  i;
  rr_pscan_data_t        *rr_pscan_data_ptr;

  // Obtain a pointer to the module data
  rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  /* clear the candidate database since a new power scan is being started from
  scratch and all the old results would be invalid */
  rr_pscan_clear_results(
    pscan_results_db_ptr   // pscan_db_ptr
  );

  if (candidate_db_ptr != NULL)
  {
    rr_candidate_db_clear(
      candidate_db_ptr    // candidate_cell_db
    );
  }

  /* ensure the validity timer is NOT running yet */
  rr_invalidate_power_scan_results(gas_id);

  // If "capture scan" is enabled, then ensure all frequencies are scanned
  if (rr_pscan_data_capture_enabled(gas_id))
  {
    rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs_valid = FALSE;
  }

  /* Do some sanity checking - if both EGSM and PGSM are set in the band_group
  then remove PGSM as it is a subset of EGSM. */
  if (ARE_SET(SYS_BAND_MASK_GSM_EGSM_900 | SYS_BAND_MASK_GSM_PGSM_900, band_group))
  {
    MSG_GERAN_MED_0_G("EGSM 900 and PGSM 900 specified in band_group, removing PGSM 900");
    CLEAR_BIT(SYS_BAND_MASK_GSM_PGSM_900, band_group);
  }

  MSG_GERAN_HIGH_3_G(
    "Performing power scan, band_pref: %08x, filter_acq_db_freqs: %d, last_acq_db_freqs_valid: %d",
    band_group,
    filter_acq_db_freqs,
    rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs_valid
  );

  for (i = 0; i < RR_PSCAN_BAND_DATA_SIZE; i++)
  {
    const rr_pscan_band_data_t *band_data = &rr_pscan_band_data[i];

    if (band_group & band_data->sys_band_mask)
    {
      MSG_SPRINTF_1(MSG_SSID_DFLT,
                    MSG_LEGACY_HIGH,
                    "Adding frequencies for %s to power scan request",
                    band_data->name);

      band_bitmap |= (int) band_data->l1_band;

      arfcn.band = band_data->sys_band;

      for (arfcn.num = band_data->channel_min;
           arfcn.num <= band_data->channel_max;
           arfcn.num++)
      {
        if (filter_acq_db_freqs && rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs_valid)
        {
          uint16 j;
          boolean filter_freq = FALSE;

          /* Need to check that this ARFCN was not scanned in the ACQ-DB power
          scan - if it was, we should skip it. */
          for (j = 0; j < rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs.num_frequencies; j++)
          {
            if (rr_arfcn_compare_freq(arfcn, rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs.meas[j].arfcn))
            {
              filter_freq = rr_pscan_ok_to_filter(&(rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs.meas[j]));
              break;
            }
          }

          if (filter_freq)
          {
            MSG_GERAN_MED_2_G("Filtering ARFCN (%d, %d) from power scan request (frequency scanned during ACQ-DB search)", arfcn.num, arfcn.band);
            continue;
          }
        }

        if (count < RR_MAX_PSCAN_FREQ_SCAN)
        {
          pscan_results_db_ptr->pscan_results.meas[count].arfcn = arfcn;
          count++;
        }
        else
        {
          MSG_GERAN_ERROR_2_G("Overflow while adding ARFCN (%d, %d) to power scan request",
            arfcn.num,
            arfcn.band);
        }
      }
    }
  }

  MSG_GERAN_MED_1_G("Number of frequencies in power scan request: %d", count);

  rr_log_power_scan_started(gas_id);

  rr_pscan_data_ptr->rr_pscan_control_data.band_pref = band_group;

  pscan_results_db_ptr->bands_searched = band_bitmap;
  pscan_results_db_ptr->pscan_results.num_frequencies = count;

  rr_send_mph_power_scan_req(
    &pscan_results_db_ptr->pscan_results,
    band_bitmap,
    search_mode,
    gas_id
  );

  /* Reset this flag as any information stored in the associated structure is
  now out of date. */
  rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs_valid = FALSE;

  return RR_EV_PERFORM_POWER_SCAN;
}

/*!
 * \brief Perform a power scan based on information in the acquisition database.
 *
 *  Sends a power scan request to L1 including all ARFCNs on all bands included
 *  in the specified band-pref, but only if they match entries in the acquisition
 *  database.
 *
 * \param pscan_db - Pointer to the power scan results database to use.
 * \param band_pref - Bitmask of the bands that should be included in the search.
 * \param plmn_id - The PLMN to search for.
 * \param requested_plmn_only - TRUE if RR should include on ARFCNs that are known to be associated with the
 *                              PLMN to search for
 *                            - FALSE if RR should include ARFCNs from other known PLMNs.
 * \param gas_id
 *
 * \return rr_event_T - EV_NO_EVENT if a power scan request is sent to L1 and RR should wait for the
 *                      power scan confirm to be received.
 *                    - EV_PSCAN_FAILURE if no frequencies were available to send to L1.
 */
rr_event_T rr_pscan_perform_pscan_acq_db(
  rr_pscan_results_db_T *pscan_db,
  sys_band_mask_type band_pref,
  sys_plmn_id_s_type plmn_id,
  boolean requested_plmn_only,
  rr_search_mode_T search_mode,
  rr_pscan_request_type_e req_type,
  const gas_id_t gas_id
)
{
  rr_pscan_data_t *rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  rr_acq_db_populate_pscan_db(
    &pscan_db->pscan_results,   // pscan_request
    band_pref,                  // band_pref
    plmn_id,                    // plmn_id
    requested_plmn_only,        // requested_plmn_only
    req_type,
    gas_id                      // gas_id
  );

  /* Save the frequencies being searched so that they can be excluded from a
  later full search if necessary. */
  rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs_valid = TRUE;
  rr_pscan_data_ptr->rr_pscan_last_acq_db_freqs = pscan_db->pscan_results;

  rr_pscan_data_ptr->rr_pscan_control_data.band_pref = band_pref;

  if (pscan_db->pscan_results.num_frequencies > 0)
  {
    rr_l1_band_bitmap_T band_bitmap = RR_BAND_BITMAP_NONE;
    uint16              i;

    /* generate the band bitmap that matches the request being sent to GL1 */
    for (i = 0; i < pscan_db->pscan_results.num_frequencies; i++)
    {
      band_bitmap |= rr_pscan_get_band_bitmap_for_arfcn(pscan_db->pscan_results.meas[i].arfcn);
    }

    MSG_GERAN_HIGH_1_G("Band_Bitmap Searched while acq_db_scan is %x",band_bitmap);
    pscan_db->bands_searched = band_bitmap;
	
    rr_send_mph_power_scan_req(
      &pscan_db->pscan_results,   // pscan_results_buffer_ptr
      pscan_db->bands_searched,   // requested_bands
      search_mode,           // search_mode
      gas_id                      // gas_id
    );

    rr_log_power_scan_started(gas_id);


    return RR_EV_PERFORM_ACQ_DB_POWER_SCAN;
  }
  else
  {
    return EV_NO_BCCH_FOUND;
  }
}

/*!
 * \brief Must be called before the RR-PSCAN module is used to perform a power-scan
 *
 * \param gas_id
 */
void rr_pscan_setup(
  const gas_id_t gas_id
)
{
  rr_pscan_results_db_T *pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);
  rr_pscan_data_t *rr_pscan_data_ptr;

  // Obtain a pointer to the module data
  rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  rr_pscan_data_ptr->rr_pscan_control_data.state = RR_PSCAN_INACTIVE;
  rr_pscan_clear_results(pscan_results_db_ptr);
}

/*!
 * \brief Sets the band preference to be used for a power-scan.
 *
 * \param band_pref
 * \param gas_id
 */
void rr_pscan_set_band_pref(
  sys_band_mask_type band_pref,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_pscan_data_t *rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  MSG_GERAN_MED_1_G("rr_pscan_set_band_pref(band_pref => 0x%08x)", band_pref);
  rr_pscan_data_ptr->rr_pscan_control_data.band_pref = band_pref;
}

/**
 * \brief  Determines which data spaces are viable sources for sharing their power scan results
 *
 * \param  data_sources_ptr - pointer to list of TRUE/FALSE values
 * \param  exclude_gas_id - data space which should not be searched
 * \return boolean - TRUE if any valid sources were identified
 *
 */
static boolean rr_pscan_find_valid_data_sources(boolean *data_sources_ptr, const gas_id_t exclude_gas_id)
{
  boolean any_valid = FALSE;

  if (data_sources_ptr != NULL)
  {
    gas_id_t gas_id;
    int      as_index;

    for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
    {
      as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

      if (gas_id == exclude_gas_id)
      {
        /* never consider the specified GAS ID as a valid source */
        data_sources_ptr[as_index] = FALSE;
      }
      else
      {
        if (rr_pscan_data[as_index].common_results.pending_request)
        {
        /* data space is not a valid source if it is currently performing a scan */
          data_sources_ptr[as_index] = FALSE;
          MSG_GERAN_HIGH_0_G("Power scan in progress, cannot share results");
        }
        else if (!rr_timer_is_running(RR_INVALIDATE_PSCAN_RESULTS_TIMER, gas_id))
        {
          /* data space is not a valid source if it has stale results */
          data_sources_ptr[as_index] = FALSE;
          MSG_GERAN_HIGH_0_G("Power scan results are stale");
        }
        else
        {
          /* assume the source is valid */
          data_sources_ptr[as_index] = TRUE;
          MSG_GERAN_HIGH_0_G("Power scan results can be shared");
        }
      }

      /* update the flag that indicates whether any sources are valid */
      any_valid = (any_valid || data_sources_ptr[as_index]);
    }
  }

  return(any_valid);
}



/**
 * Functionality: Determines if an ARFCN is in the supplied
 * pscan request
 *
 * @Params:  pscan_request_ptr - pointer to pscan req 
 *           num_freq - num freq in pscan_req
 *           arfcn - ARFCN to find in the list gas_id
 * 
 * @Return: TRUE - ARFCN present, FALSE - ARFCN not present
 *
 */
boolean rr_pscan_arfcn_present(rr_l1_pscan_results_T *pscan_request_ptr, uint16 num_freq, ARFCN_T arfcn, const gas_id_t gas_id)
{  
  word meas_index = 0;
  boolean match_found = FALSE;

  RR_NULL_CHECK_RETURN_PARAM(pscan_request_ptr, FALSE);

  for (meas_index = 0; (meas_index < num_freq) && (meas_index < ARR_SIZE(pscan_request_ptr->meas)); meas_index++)
  {
    if (ARFCNS_EQUAL(arfcn, pscan_request_ptr->meas[meas_index].arfcn))
    {
      match_found = TRUE;
      break;
    }
  }

  return match_found;
}

/**
 * Functionality: Finds any existing power measurement for a specified ARFCN
 *
 * @Params:  meas_ptr - pointer to ARFCN/power structure
 *           valid_data_sources - list of TRUE/FALSE values, one per subscription
 *
 */
static boolean rr_pscan_find_meas(rr_l1_freq_power_T *meas_ptr, boolean *valid_data_sources)
{
  rr_pscan_results_db_T *results_db_ptr;
  int                    as_index;
  int                    meas_index;
  int                    meas_count;

  /* search through all data spaces marked as valid sources */
  for (as_index = 0; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    if (TRUE == valid_data_sources[as_index])
    {
      /* get the results database pointer for this space and search it */
      results_db_ptr = &(rr_pscan_data[as_index].rr_pscan_results_db);

      if (results_db_ptr != NULL)
      {
        meas_count = results_db_ptr->pscan_results.num_frequencies;
        for (meas_index = 0; meas_index < meas_count; meas_index++)
        {
          if (ARFCNS_EQUAL(meas_ptr->arfcn, results_db_ptr->pscan_results.meas[meas_index].arfcn))
          {
            meas_ptr->RXLEV_average = results_db_ptr->pscan_results.meas[meas_index].RXLEV_average;
            return(TRUE);
          }
        }
      }
    }
  }

  return(FALSE);
}

/**
 * Functionality: Prepares shared results before requesting a power scan Layer 1
 *
 * @Params:  req_ptr - pointer to MPH_POWER_SCAN_REQ (for GAS ID determination)
 *
 */
void rr_pscan_prepare_common_results(mph_power_scan_req_T *req_ptr)
{
  rr_pscan_data_t *rr_pscan_data_ptr;
  boolean          valid_data_sources[NUM_GERAN_DATA_SPACES] = {0};
  gas_id_t         gas_id;
  word             arfcn_count;

  /* sanity check input parameters */
  if (req_ptr == NULL)
  {
    return;
  }

  if (req_ptr->pscan_results_buffer_ptr->num_frequencies == 0)
  {
    return;
  }

  /* Obtain a pointer to the module data */
  gas_id = req_ptr->gas_id;
  rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  /* a call to this function implies that a new scan is starting */
  rr_pscan_data_ptr->common_results.pending_request = TRUE;

  /* determine whether any other data spaces are viable sources of common results */
  if (!rr_pscan_find_valid_data_sources(valid_data_sources, gas_id))
  {
    MSG_GERAN_HIGH_0_G("No power scan results available for sharing");
    return;
  }

  /* determine the number of frequencies that this scan was intended for */
  arfcn_count = req_ptr->pscan_results_buffer_ptr->num_frequencies;

  /* deallocate any existing memory for common results */
  rr_pscan_data_ptr->common_results.storage_limit = 0;
  if (rr_pscan_data_ptr->common_results.meas != NULL)
  {
    GPRS_MEM_FREE(rr_pscan_data_ptr->common_results.meas);
    rr_pscan_data_ptr->common_results.meas = NULL;
  }

  /* and then allocate for the current request */
  rr_pscan_data_ptr->common_results.meas = (rr_l1_freq_power_T *)GPRS_MEM_CALLOC(arfcn_count * sizeof(rr_l1_freq_power_T));
  rr_pscan_data_ptr->common_results.num_frequencies = 0;

  /* if memory was available, begin the preparation */
  if (rr_pscan_data_ptr->common_results.meas != NULL)
  {
    rr_l1_freq_power_T arfcn_result;
    word               arfcn_index;

    /* leave the first requested entry intact, so that L1 still transitions */
    /* through FIND_BCCH state as normal for the MPH_POWER_SCAN_REQ message */
    req_ptr->pscan_results_buffer_ptr->num_frequencies = 1;
    arfcn_result = req_ptr->pscan_results_buffer_ptr->meas[0];
    req_ptr->requested_bands = rr_pscan_get_band_bitmap_for_arfcn(arfcn_result.arfcn);

    /* record the maxmium number of entries for which storage was */
    /* allocated, this will be one more than required by the loop */
    rr_pscan_data_ptr->common_results.storage_limit = arfcn_count;

    for (arfcn_index = 1; arfcn_index < arfcn_count; arfcn_index++)
    {
      /* loop through each ARCFN that is being requested, trying */
      /* to find a measurement for it in any OTHER stack context */
      arfcn_result = req_ptr->pscan_results_buffer_ptr->meas[arfcn_index];

      if (rr_pscan_find_meas(&arfcn_result, valid_data_sources))
      {
        /* available elsewhere, store in the common result list */
        rr_pscan_store_common_result(rr_pscan_data_ptr, &arfcn_result);
      }
      else
      {
        /* not available, restore the entry in the request to GL1 */
        req_ptr->requested_bands |= rr_pscan_add_result(req_ptr->pscan_results_buffer_ptr, &arfcn_result);
      }
    }

    MSG_GERAN_HIGH_2_G("Power scan: %d common and %d new frequencies",
                       (int)(rr_pscan_data_ptr->common_results.num_frequencies),
                       (int)(req_ptr->pscan_results_buffer_ptr->num_frequencies));
  }

  return;
}

/*!
 * \brief The power-scan control state-machine
 *
 * \param event
 * \param message_ptr
 * \param gas_id
 */
void rr_pscan_control(
  rr_event_T event,
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T             new_event = EV_NO_EVENT;
  byte                   message_id   = 0;
  byte                   message_set  = 0;
  rr_pscan_data_t       *rr_pscan_data_ptr;

  // Obtain a pointer to the module data
  rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  event = rr_extract_message_header_details(
    event,
    message_ptr,
    &message_set,
    &message_id
  );

  /* EV_RESET_SOFTWARE and EV_PANIC_RESET are handled in stateless manner. */
  if ((event == EV_RESET_SOFTWARE) || (event == EV_PANIC_RESET))
  {
    rr_pscan_init(gas_id);
    event = EV_NO_EVENT;
    return;
  }

  if (message_ptr != NULL)
  {
    if (message_set == MS_RR_RR)
    {
      if (message_ptr->rr.header.rr_message_set == RR_PSCAN_IMSG &&
          message_id == (uint8) RR_PSCAN_IMSG_START_IND)
      {
        if (rr_pscan_data_ptr->rr_pscan_control_data.state != RR_PSCAN_INACTIVE)
        {
          MSG_GERAN_ERROR_1_G("RR_PSCAN_IMSG_START_IND received in state %d, resetting state to INACTIVE to allow processing", rr_pscan_data_ptr->rr_pscan_control_data.state);
          rr_pscan_data_ptr->rr_pscan_control_data.state = RR_PSCAN_INACTIVE;
        }
      }
    }
  }

  while (event != EV_NO_EVENT)
  {
    switch (rr_pscan_data_ptr->rr_pscan_control_data.state)
    {
      case RR_PSCAN_INACTIVE:
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
                  case RR_PSCAN_IMSG:
                  {
                    switch (message_ptr->rr.pscan.imsg.header.imh.message_id)
                    {
                      case RR_PSCAN_IMSG_START_IND:
                      {
                        rr_pscan_data_ptr->rr_pscan_control_data.band_pref = message_ptr->rr.pscan.imsg.pscan_start_ind.band_pref;
                        rr_pscan_data_ptr->rr_pscan_control_data.search_mode = message_ptr->rr.pscan.imsg.pscan_start_ind.search_mode;

                        if(rr_pscan_data_ptr->rr_pscan_control_data.search_mode == RR_SEARCH_RESELECTION)
                        {
                          rr_pscan_results_db_T *pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);

                          rr_send_mph_power_scan_req(
                            &pscan_results_db_ptr->pscan_results,
                            pscan_results_db_ptr->bands_searched,
                            RR_SEARCH_RESELECTION,
                            gas_id
                          );
                        }
                        else
                        {
                          rr_PLMN_list_req_info_T   *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
                          rr_pscan_results_db_T     *rr_pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);
                          if (ARE_SET(SYS_BAND_MASK_GSM_EGSM_900 | SYS_BAND_MASK_GSM_PGSM_900, rr_pscan_data_ptr->rr_pscan_control_data.band_pref))
                          {
                            MSG_GERAN_MED_0_G("EGSM 900 and PGSM 900 specified in band_group, removing PGSM 900");
                            CLEAR_BIT(SYS_BAND_MASK_GSM_PGSM_900, rr_pscan_data_ptr->rr_pscan_control_data.band_pref);
                          }
                          if (rr_plmn_list_req_info_ptr->scan_scope == SYS_SCAN_SCOPE_ACQ_DB)
                          {
                            MSG_GERAN_HIGH_0_G("acq db scan for plmn list");
                            (void) rr_pscan_perform_pscan_acq_db(
                                                          rr_pscan_results_db_ptr,
                                                          rr_pscan_data_ptr->rr_pscan_control_data.band_pref,
                                                          rr_plmn_list_req_info_ptr->plmn_list.info[0].plmn, /* actually this PLMN will not affect the frequency to be scaned. */
                                                          FALSE, /* requested plmn only */
                                                          rr_pscan_data_ptr->rr_pscan_control_data.search_mode,
                                                          RR_PSCAN_REQ_NONE,
                                                          gas_id
                                                          );
                          }
                          else
                          {
                            (void) rr_perform_power_scan(
                            rr_pscan_data_ptr->rr_pscan_control_data.search_mode,
                            rr_pscan_data_ptr->rr_pscan_control_data.band_pref,
                            FALSE,
                            gas_id
                            );
                          }
                          
                        }

                        rr_pscan_state_transition(
                          RR_PSCAN_IN_PROGRESS,   // new_state
                          gas_id                  // gas_id
                        );
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message_ptr,rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state));
                      }
                    }
                    break;
                  } /* RR_PSCAN_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr,rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state));
                  }
                }
                break;
              } /*MS_RR_RR*/

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr,rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state));
              }
            }
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state));
          }
        }
        break;
      }

      case RR_PSCAN_IN_PROGRESS:
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
                  case RR_PSCAN_IMSG:
                  {
                    switch (message_ptr->rr.pscan.imsg.header.imh.message_id)
                    {
                      case RR_PSCAN_IMSG_ABORT_REQ:
                      {
                        if (rr_pscan_data_ptr->rr_pscan_control_data.search_mode == RR_SEARCH_NORMAL)
                        {
                          MSG_GERAN_HIGH_0_G("Force abort foreground PSCAN!");
                          rr_pscan_send_abort_cnf(gas_id);

                          rr_pscan_state_transition(
                            RR_PSCAN_INACTIVE,    // new_state
                            gas_id                // gas_id
                          );
                        }
                        else if (rr_pscan_data_ptr->rr_pscan_control_data.search_mode == RR_SEARCH_BACKGROUND)
                        {
                          MSG_GERAN_HIGH_0_G("Abort a background PSCAN!");
                          rr_send_mph_power_scan_abort_req(gas_id);

                          rr_pscan_state_transition(
                            RR_PSCAN_ABORT,   // new_state
                            gas_id            // gas_id
                          );
                        }
                        else if (rr_pscan_data_ptr->rr_pscan_control_data.search_mode == RR_SEARCH_RESELECTION)
                        {
                          MSG_GERAN_HIGH_0_G("Abort a RPS PSCAN!");

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                          /* advise the multi-SIM controller of this abort */
                          /* since GL1 may need to be prompted to complete */
                          rr_ms_power_scan_aborted(gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

                          rr_pscan_state_transition(
                            RR_PSCAN_ABORT,   // new_state
                            gas_id            // gas_id
                          );
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message_ptr,rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state));
                      }
                    }
                    break;
                  } /* RR_PSCAN_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr,rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state));
                  }
                }
                break;
              } /*MS_RR_RR*/

              case MS_RR_L1:
              {
                switch ((rr_l1_message_id_enum_T) message_id)
                {
                  case MPH_POWER_SCAN_CNF:
                  {
                    // Insert any pscan results common to multiple subs
                    rr_process_mph_power_scan_cnf(&message_ptr->mph_power_scan_cnf);

                    // Inform caller of completion
                    rr_pscan_send_finish_ind(
                      message_ptr->mph_power_scan_cnf.suspending,   // l1_suspended
                      gas_id                                        // gas_id
                    );

                    /* processing of the pscan results should happen in in the parent state machine*/
                    rr_pscan_state_transition(
                      RR_PSCAN_INACTIVE,    // new_state
                      gas_id                // gas_id
                    );
                    break;
                  } /* RR_PSCAN_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr,rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state));
                  }
                }
                break;
              } /*MS_RR_L1*/

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr,rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state));
              }
            }
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state));
          }
        }
        break;
      }

      case RR_PSCAN_ABORT:
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
              case MS_RR_L1:
              {
                switch ((rr_l1_message_id_enum_T) message_id)
                {
                  case MPH_POWER_SCAN_CNF:
                  {
                    MSG_GERAN_HIGH_0_G("Received MPH_POWER_SCAN_CNF while in PSCAN_ABORT");

                    /* free memory for any shared results */
                    rr_pscan_discard_common_results(gas_id);

                    /* and cancel the validity timer (because GL1 may not have finished the scan) */
                    rr_invalidate_power_scan_results(gas_id);

                    rr_pscan_send_abort_cnf(gas_id);

                    rr_pscan_state_transition(
                      RR_PSCAN_INACTIVE,    // new_state
                      gas_id                // gas_id
                    );
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr,rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state));
                  }
                }
                break;
              } /*MS_RR_L1*/

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr,rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state));
              }
            }
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_pscan_state_name(rr_pscan_data_ptr->rr_pscan_control_data.state));
          }
        }
        break;
      }

      default:
        MSG_GERAN_ERROR_2_G("UnexEV. Ev(%d), State(%d)",(int) event,(int)rr_pscan_data_ptr->rr_pscan_control_data.state);
        break;
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((event != new_event) && (new_event != EV_NO_EVENT)) ||
        (rr_pscan_data_ptr->rr_pscan_control_data.old_state != rr_pscan_data_ptr->rr_pscan_control_data.state))
    {
      rr_store_trace_msg_buf(
        RR_PSCAN_SM,
        new_event,
        (byte) rr_pscan_data_ptr->rr_pscan_control_data.state,
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

    if (rr_pscan_data_ptr->rr_pscan_control_data.old_state != rr_pscan_data_ptr->rr_pscan_control_data.state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_pscan_control",
        rr_pscan_data_ptr->rr_pscan_control_data.old_state,
        rr_pscan_data_ptr->rr_pscan_control_data.state,
        rr_pscan_state_name,
        gas_id
      );

      rr_pscan_data_ptr->rr_pscan_control_data.old_state = rr_pscan_data_ptr->rr_pscan_control_data.state;
    }
  }
}

/*!
 * \brief Returns a pointer to rr_pscan_results_db.
 *
 * \param gas_id
 *
 * \return rr_pscan_results_db_T*
 */
rr_pscan_results_db_T *rr_get_pscan_results_db_ptr(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_pscan_data_t *rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

  return(&rr_pscan_data_ptr->rr_pscan_results_db);
}

/*!
 * \brief Returns TRUE if power-scan data capture is enabled for this GAS ID; FALSE otherwise.
 *
 * \param gas_id
 *
 * \return boolean
 */
boolean rr_pscan_data_capture_enabled(const gas_id_t gas_id)
{
  return(rr_pscan_data_capture_data.enabled && (gas_id == rr_pscan_data_capture_data.enabled_gas_id));
}

/*!
 * \brief Returns TRUE if prior power-scan results are reusable for the given band_pref.
 *
 * \param gas_id
 *
 * \return boolean - TRUE if previous power-scan results are available, FALSE otherwise
 */
boolean rr_pscan_results_are_reusable(
  sys_band_mask_type band_pref,
  const gas_id_t gas_id
)
{
  if (rr_pscan_data_capture_enabled(gas_id))
  {
    return(FALSE);
  }

  if (rr_timer_is_running(RR_INVALIDATE_PSCAN_RESULTS_TIMER, gas_id))
  {
    rr_pscan_results_db_T *pscan_results_db_ptr;

    // Obtain a pointer to the module data
    rr_pscan_data_t *rr_pscan_data_ptr = rr_pscan_data_get_data_ptr(gas_id);
    RR_NULL_CHECK_FATAL(rr_pscan_data_ptr);

    // Obtain a pointer to the pscan results
    pscan_results_db_ptr = rr_get_pscan_results_db_ptr(gas_id);

    // if both EGSM and PGSM are set in the band_pref then remove PGSM as it is a subset of EGSM
    if (ARE_SET(SYS_BAND_MASK_GSM_EGSM_900 | SYS_BAND_MASK_GSM_PGSM_900, band_pref))
    {
      CLEAR_BIT(SYS_BAND_MASK_GSM_PGSM_900, band_pref);
    }

    if ((rr_pscan_data_ptr->rr_pscan_control_data.band_pref == band_pref) &&
        (pscan_results_db_ptr != NULL) &&
        (pscan_results_db_ptr->pscan_results.num_frequencies > 0))
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*!
 * \brief Initialisation function called at task start.
 */
void rr_pscan_task_start_init(void)
{
  memset(&rr_pscan_data, 0, sizeof(rr_pscan_data));

} /* end rr_pscan_task_start_init() */

/*!
 * \brief Starts capture of power-scan data into the provided buffer. Note: Called by CM.
 *
 * \param memptr - Pointer to a buffer to place the results
 * \param memsize - Size (bytes) of the buffer
 *
 * \return boolean - TRUE if successful; FALSE otherwise
 */
boolean geran_start_capture_scan_data(uint32 *memptr, uint16 memsize)
{
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;
  int16  max_num_of_entries;

  /*
  memsize is size of the buffer, in bytes
  Each scan entry is 32-bits, so maximum number of entries is (memsize / 4)
  But the first 32-bit entry in the buffer is a header, so subtract 1.
  */
  max_num_of_entries = (memsize / 4) - 1;

  if ((max_num_of_entries > 0) && (memptr != NULL))
  {
    rr_pscan_enable_pscan_data_capture(memptr, max_num_of_entries, geran_map_nas_id_to_gas_id(as_id));

    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*!
 * \brief Stops capture of power-scan data. Note: Called by CM.
 *
 * \return boolean - TRUE if successful; FALSE otherwise
 */
boolean geran_stop_capture_scan_data(void)
{
  rr_pscan_disable_pscan_data_capture();

  return TRUE;
}


#ifdef PERLUTF
#error code not present
#endif // PERLUTF

/* EOF */

