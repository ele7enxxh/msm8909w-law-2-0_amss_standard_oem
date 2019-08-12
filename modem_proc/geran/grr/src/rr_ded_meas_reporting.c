/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Dedicated Measurement Reporting module

GENERAL DESCRIPTION
   This module handles measurement reporting while in Dedicated

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006-2013 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_ded_meas_reporting.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
16Aug06    DMB     Moved measurement reporting code from rr_cell_reestablish.c

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"  /* Contains FEATURE_... defines */
#include <stdlib.h> /* for qsort() */
#include "amssassert.h"
#include "rr_seg_load.h"
#include "rr_ded_meas_reporting.h"
#include "rr_defs.h"
#include "rr_general.h"
#include "rr_log.h"
#include "gsbitstream.h"
#include "rr_l2_send.h"
#include "rr_sys_info_ncell.h"
#include "geran_eng_mode_read_api.h"
#include "geran_eng_mode_info.h"
#include "time_svc.h"
#include "gprs_mem.h"
#include "rr_multi_sim.h"
#include "rr_l1_send.h" 
#include "stringl/stringl.h"
#ifdef FEATURE_GSM_BAND_AVOIDANCE
#include "rr_band_avoid.h"
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/**
 * This value is used in Measurement Reports to indicate to the network
 * that neighbour cell information is not available (e.g. BA list has not
 * yet been received).
 */
#define NCELL_INFO_NOT_AVAILABLE_FOR_SCELL 7

/* This value is chosen as an effective inifinite value, as the maximum
possible value for a reported quantity (RSCP or Ec/No) is 63, so it can
never meet/exceed 64. */
#define RR_FDD_REPORTING_THRESHOLD_INFINITY  64

#define MAX_RSRP_DIFFERENCE_FOR_PRUNING      12

#ifdef FEATURE_GSM_TDS

/* This value is chosen as an effective inifinite value, as the maximum
possible value for a reported quantity is 63, so it can
never meet/exceed 64. */
#define RR_TDD_REPORTING_THRESHOLD_INFINITY  64
#endif /*FEATURE_GSM_TDS*/

#define RR_LAST_MEAS_BUFFER_SIZE 16

#define RR_REPORTING_THRESHOLD_DEFAULT 0U
#define RR_REPORTING_THRESHOLD_ALWAYS  0U
#define RR_REPORTING_THRESHOLD_NEVER   7U

#define RR_REPORTING_OFFSET_DEFAULT    0U

const byte rr_last_meas_results_init[RR_LAST_MEAS_BUFFER_SIZE] =
  {0x00,0x3F,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

#ifdef FEATURE_LTE

/**
  @brief An instance of this structure is used to store LTE measurements
         received from L1 while in dedicated mode.

  This structure contains one entry per frequency that may be present in the
  LTE neighbour cell list.  Memory for these entries is dynamically allocated
  when RR sends a new IRAT neighbour cell list to L1.
*/
typedef struct
{
  rr_l1_lte_measurements_t * entries[MAX_RR_L1_EUTRAN_FREQUENCIES];
} rr_ded_meas_reporting_lte_measurements_t;

#endif /* FEATURE_LTE */

typedef struct
{
  ARFCN_T arfcn;
  uint8   bsic;
  uint8   rxlev;

  uint8   cell_index;

} rr_dedicated_mode_gsm_measurement_t;

#ifdef FEATURE_WCDMA

typedef struct
{
  uint16  uarfcn;
  uint16  scrambling_code;

  uint8   reported_rssi;

  uint8   cell_index;
  boolean rssi_index_valid;
  uint8   rssi_index;

} rr_dedicated_mode_wcdma_measurement_t;

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS

typedef struct
{
  uint16 uarfcn;
  uint16 cpid;

  uint8  cell_index;

} rr_dedicated_mode_tdscdma_measurement_t;

#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE

typedef struct
{
  lte_earfcn_t earfcn;
  uint16 pcid;

  uint8  frequency_index;

} rr_dedicated_mode_lte_measurement_t;

#endif /* FEATURE_LTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

typedef struct
{
  rr_dedicated_mode_gsm_measurement_t     gsm;  
#ifdef FEATURE_WCDMA
  rr_dedicated_mode_wcdma_measurement_t   wcdma;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  rr_dedicated_mode_tdscdma_measurement_t tdscdma;
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
  rr_dedicated_mode_lte_measurement_t     lte;
#endif /* FEATURE_LTE */
} rr_dedicated_mode_irat_measurement_u;

typedef struct
{
  rr_rat_e                             rat;
  rr_dedicated_mode_irat_measurement_u data;
  uint8                                multirat_reporting;
  uint8                                reported_value;
  int16                                sort_value;  
  boolean                              reported;
} rr_dedicated_mode_irat_measurement_t;

#ifdef FEATURE_WCDMA
#define MAX_WCDMA_MEASUREMENTS (MAX_WCDMA_STORED_DED_MEASUREMENTS)
#else
#define MAX_WCDMA_MEASUREMENTS (0)
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
#define MAX_TDSCDMA_MEASUREMENTS (MAX_IRAT_STORED_DED_MEASUREMENTS)
#else
#define MAX_TDSCDMA_MEASUREMENTS (0)
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
#define MAX_LTE_MEASUREMENTS (16 * 8)
#else
#define MAX_LTE_MEASUREMENTS (0)
#endif /* FEATURE_LTE */

#define RR_DEDICATED_MODE_IRAT_MEASUREMENT_LIST_SIZE (MAX_WCDMA_MEASUREMENTS + MAX_TDSCDMA_MEASUREMENTS + MAX_LTE_MEASUREMENTS)

typedef struct
{
  uint32                               count;
  rr_dedicated_mode_irat_measurement_t entries[RR_DEDICATED_MODE_IRAT_MEASUREMENT_LIST_SIZE];
} rr_dedicated_mode_irat_measurement_list_t;

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

typedef struct
{
  byte                                rr_last_meas_results[RR_LAST_MEAS_BUFFER_SIZE];
  boolean                             invalidate_measurement_reports;
  boolean                             rr_last_serving_ded_meas_valid;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
  boolean                             mph_surround_wcdma_dedicated_meas_ind_received;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/
  mph_serving_ded_meas_ind_T          rr_last_serving_ded_meas_ind;
  surrounding_measurement_T           usat_surrounding_measurement;
  rr_ba_lists_t                      *rr_ba_lists_ptr;

#ifdef FEATURE_LTE
  /* Storage space for LTE neighbour cell measurements in dedicated mode is
  dynamically allocated.

  When MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ is sent to L1, storage space
  is allocated in the lte_measurements structure for each unique LTE frequency
  that appears in the signal.

  On receipt of new LTE measurements (MPH_SURROUND_IRAT_MEAS_IND) from L1, the
  measurements are copied in to the associated structure.

  If the LTE neighbour list is updated, the dynamically allocated storage space
  is also updated.
  */
  rr_ded_meas_reporting_lte_measurements_t lte_measurements;
#endif /* FEATURE_LTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  /* The IRAT measurement list is populated every time a measurement report is
  generated.  From this, IRAT neighbour cells are chosen to be added to the
  measurement report based on the rules in 3GPP 45.008 section 8.4.7. */
  rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

} rr_ded_meas_reporting_data_t;

/* Types used to define a MEASUREMENT REPORT message. */

#define RR_MEASUREMENT_REPORT_MAX_ENTRIES 6

typedef struct
{
  uint8   bcch_freq_ncell;
  uint8   bsic_ncell;
  uint8   rxlev_ncell;

} rr_measurement_report_entry_gsm_t;

#ifdef FEATURE_WCDMA

typedef struct
{
  uint8 reported_value;
  uint8 reported_index;

} rr_measurement_report_entry_wcdma_t;

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS

typedef struct
{
  uint8 reported_value;
  uint8 reported_index;

} rr_measurement_report_entry_tdscdma_t;

#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE

typedef struct
{
  uint8  reported_value;
  uint8  frequency_index;
  uint16 pcid;

} rr_measurement_report_entry_lte_t;

#endif /* FEATURE_LTE */

typedef union
{
  rr_measurement_report_entry_gsm_t     gsm;
#ifdef FEATURE_WCDMA
  rr_measurement_report_entry_wcdma_t   wcdma;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  rr_measurement_report_entry_tdscdma_t tdscdma;
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
  rr_measurement_report_entry_lte_t     lte;
#endif /* FEATURE_LTE */
} rr_measurement_report_entry_u;

typedef struct
{
  rr_rat_e                      rat;
  rr_measurement_report_entry_u data;
} rr_measurement_report_entry_t;

/* Data structure that defines the contents of a MEASUREMENT REPORT message,
ready for encoding. */
typedef struct
{
  uint8                         ba_used;
  uint8                         dtx_used;
  uint8                         rxlev_full_serving_cell;
  uint8                         ba_3g_used;
  uint8                         meas_valid;
  uint8                         rxlev_sub_serving_cell;
  uint8                         rxqual_full_serving_cell;
  uint8                         rxqual_sub_serving_cell;
  uint32                        count;
  rr_measurement_report_entry_t entries[RR_MEASUREMENT_REPORT_MAX_ENTRIES];
} rr_measurement_report_t;

/**
  @defgroup enhanced_measurement_report
*/
#define MAX_WCDMA_CELL_COUNT_EMR 96
#define MAX_WCDMA_CELL_COUNT_NMR 64
#define MAX_CELL_COUNT_FOR_REPORT_TYPE(rt) (REPORT_TYPE_ENHANCED  == (rt) ? MAX_WCDMA_CELL_COUNT_EMR : MAX_WCDMA_CELL_COUNT_NMR)

/**
  @ingroup enhanced_measurement_report
*/
typedef struct
{
  uint8 rr_short_pd;               /* 1 bit */
  uint8 message_type;              /* 5 bits */
  uint8 short_layer_2_header;      /* 2 bits */
  uint8 ba_used;                   /* 1 bit */
  uint8 ba_3g_used;                /* 1 bit */
  uint8 bsic_seen;                 /* 1 bit */
  uint8 scale;                     /* 1 bit */
  boolean serving_cell_data_valid; /* 1 bit */
  struct
  {
    uint8 dtx_used;                /* 1 bit */
    uint8 rxlev_val;               /* 6 bits */
    uint8 rx_qual_full;            /* 3 bits */
    uint8 mean_bep;                /* 5 bits */
    uint8 cv_bep;                  /* 3 bits */
    uint8 nbr_rcvd_blocks;         /* 5 bits */
  } serving_cell_data;

  /* Total number of bits in fixed parameters above = 36 */

  uint32  bits_used; /* Tracks the number of bits currently used by the message. */
  uint8   highest_bitmap_index; /* Tracks the highest index so far used in the reporting index. */

  uint32                                 ncell_count;
  rr_dedicated_mode_irat_measurement_t * ncells[MAX_WCDMA_CELL_COUNT_EMR];

  uint32                                 lte_cell_count;
  rr_dedicated_mode_irat_measurement_t * lte_cells[4];

  uint32                                 invalid_bsic_information_count;
  rr_dedicated_mode_irat_measurement_t * invalid_bsic_information[32];

} rr_enhanced_measurement_report_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA
static rr_log_3g_meas_report_T  log_3g_meas_report[NUM_GERAN_DATA_SPACES];
#endif /* FEATURE_WCDMA */

/*****************************************/
/* Saves last meas results, used by usat */
/* Initialized to a dummy meas results   */
/* that indicates no measurements        */
/*****************************************/
static rr_ded_meas_reporting_data_t rr_ded_meas_reporting_data[NUM_GERAN_DATA_SPACES];

#ifdef FEATURE_GSM_TDS
void rr_irat_store_dedicated_utran_tdd_measurements(
  rr_l1_utran_tdd_measurements_t * new_utran_tdd_measurements,
  const gas_id_t gas_id
);
#endif /*FEATURE_GSM_TDS*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

static void rr_gsm_meas_params_dump(
  rr_gsm_meas_params_t * gsm_meas_params
)
{
  RR_NULL_CHECK_RETURN_VOID(gsm_meas_params);

  MSG_GERAN_MED_0("GSM Measurement Parameters:");
  MSG_GERAN_MED_1("  PWRC:                     %d", gsm_meas_params->pwrc);
  MSG_GERAN_MED_1("  REPORT_TYPE:              %d", gsm_meas_params->report_type);
  MSG_GERAN_MED_1("  REPORTING_RATE:           %d", gsm_meas_params->reporting_rate);
  MSG_GERAN_MED_1("  INVALID_BSIC_REPORTING:   %d", gsm_meas_params->invalid_bsic_reporting);
  MSG_GERAN_MED_1("  SERVING_BAND_REPORTING:   %d", gsm_meas_params->serving_band_reporting);
  MSG_GERAN_MED_1("  MULTIBAND_REPORTING:      %d", gsm_meas_params->multiband_reporting);
  MSG_GERAN_MED_1("  SCALE_ORD:                %d", gsm_meas_params->scale_ord);
  MSG_GERAN_MED_1("  900_REPORTING_OFFSET:     %d", gsm_meas_params->reporting_offset_900);
  MSG_GERAN_MED_1("  900_REPORTING_THRESHOLD:  %d", gsm_meas_params->reporting_threshold_900);
  MSG_GERAN_MED_1("  1800_REPORTING_OFFSET:    %d", gsm_meas_params->reporting_offset_1800);
  MSG_GERAN_MED_1("  1800_REPORTING_THRESHOLD: %d", gsm_meas_params->reporting_threshold_1800);
  MSG_GERAN_MED_1("  1900_REPORTING_OFFSET:    %d", gsm_meas_params->reporting_offset_1900);
  MSG_GERAN_MED_1("  1900_REPORTING_THRESHOLD: %d", gsm_meas_params->reporting_threshold_1900);
  MSG_GERAN_MED_1("  850_REPORTING_OFFSET:     %d", gsm_meas_params->reporting_offset_850);
  MSG_GERAN_MED_1("  850_REPORTING_THRESHOLD:  %d", gsm_meas_params->reporting_threshold_850);
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)

void rr_utran_meas_params_dump(
  rr_utran_meas_params_t * utran_meas_params
)
{
  RR_NULL_CHECK_RETURN_VOID(utran_meas_params);

  MSG_GERAN_MED_0("UTRAN Measurement Parameters:");
  MSG_GERAN_MED_1("  Qsearch_C:                 %d", utran_meas_params->qsearch_c);
  MSG_GERAN_MED_1("  FDD_REP_QUANT:             %d", utran_meas_params->fdd_rep_quant);
  MSG_GERAN_MED_1("  FDD_MULTIRAT_REPORTING:    %d", utran_meas_params->fdd_multirat_reporting);
  MSG_GERAN_MED_1("  FDD_REPORTING_OFFSET:      %d", utran_meas_params->fdd_reporting_offset);
  MSG_GERAN_MED_1("  FDD_REPORTING_THRESHOLD:   %d", utran_meas_params->fdd_reporting_threshold);
  MSG_GERAN_MED_1("  FDD_REPORTING_THRESHOLD_2: %d", utran_meas_params->fdd_reporting_threshold_2);
  MSG_GERAN_MED_1("  TDD_MULTIRAT_REPORTING:    %d", utran_meas_params->tdd_multirat_reporting);
  MSG_GERAN_MED_1("  TDD_REPORTING_OFFSET:      %d", utran_meas_params->tdd_reporting_offset);
  MSG_GERAN_MED_1("  TDD_REPORTING_THRESHOLD:   %d", utran_meas_params->tdd_reporting_threshold);
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#ifdef FEATURE_LTE

static void rr_eutran_meas_params_dump(
  rr_eutran_meas_params_t * eutran_meas_params
)
{
  RR_NULL_CHECK_RETURN_VOID(eutran_meas_params);

  MSG_GERAN_MED_0("E-UTRAN Measurement Parameters:");
  MSG_GERAN_MED_1("  Qsearch_C_E-UTRAN:                     %d", eutran_meas_params->qsearch_c);
  MSG_GERAN_MED_1("  E-UTRAN_REP_QUANT:                     %d", eutran_meas_params->rep_quant);
  MSG_GERAN_MED_1("  E-UTRAN_MULTIRAT_REPORTING:            %d", eutran_meas_params->multirat_reporting);
  MSG_GERAN_MED_1("  E-UTRAN_FDD_REPORTING_THRESHOLD:       %d", eutran_meas_params->fdd.reporting_threshold);
  MSG_GERAN_MED_1("  E-UTRAN_FDD_REPORTING_THRESHOLD_2:     %d", eutran_meas_params->fdd.reporting_threshold_2);
  MSG_GERAN_MED_1("  E-UTRAN_FDD_MEASUREMENT_REPORT_OFFSET: %d", eutran_meas_params->fdd.measurement_report_offset);
  MSG_GERAN_MED_1("  E-UTRAN_FDD_REPORTING_OFFSET:          %d", eutran_meas_params->fdd.reporting_offset);
  MSG_GERAN_MED_1("  E-UTRAN_TDD_REPORTING_THRESHOLD:       %d", eutran_meas_params->tdd.reporting_threshold);
  MSG_GERAN_MED_1("  E-UTRAN_TDD_REPORTING_THRESHOLD_2:     %d", eutran_meas_params->tdd.reporting_threshold_2);
  MSG_GERAN_MED_1("  E-UTRAN_TDD_MEASUREMENT_REPORT_OFFSET: %d", eutran_meas_params->tdd.measurement_report_offset);
  MSG_GERAN_MED_1("  E-UTRAN_TDD_REPORTING_OFFSET:          %d", eutran_meas_params->tdd.reporting_offset);
  MSG_GERAN_MED_1("  REPORTING_GRANULARITY:                 %d", eutran_meas_params->reporting_granularity);
}

#endif /* FEATURE_LTE */

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_ded_meas_reporting_data_t*
 */
static rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_ded_meas_reporting_data[as_index];
}

#ifdef FEATURE_WCDMA
/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_log_3g_meas_report_T*
 */
static rr_log_3g_meas_report_T *rr_log_3g_meas_report_get_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &log_3g_meas_report[as_index];
}
#endif /* #ifdef FEATURE_WCDMA */

/**
  @brief Decode an OTA XXX_REPORTING_THRESHOLD value.

  This function takes an OTA reporting threshold value, in the range of 0 to 7,
  and returns the decoded value.

  The OTA reporting threshold is decoded according to 45.008 Section 9:

  OTA Value:     0 1  2  3  4  5  6 7
  Decoded Value: 0 6 12 18 24 30 36 64

  The value 64 is used when the OTA value is 7, as this indicates a threshold
  of infinity.  The highest reported value (which is a 6 bit number) is 63, so
  a threshold of 64 will never be exceeded.

  @param ota_reporting_threshold The OTA value to be decoded.
  @return The decoded XXX_REPORTING_THRESHOLD value.
*/
static uint8 rr_decode_xxx_reporting_threshold(uint8 ota_reporting_threshold)
{
  RR_ASSERT_RETURN_VALUE(ota_reporting_threshold <= 7, 0);

  if (ota_reporting_threshold == 7)
  {
    return 64;
  }
  else
  {
    return (ota_reporting_threshold * 6);
  }
}

/**
  @brief Decode an OTA XXX_REPORTING_OFFSET value.

  @param ota_reporting_offset The OTA value to be decoded.
  @return The decoded XXX_REPORTING_OFFSET value.
*/
static uint8 rr_decode_xxx_reporting_offset(uint8 ota_reporting_offset)
{
  RR_ASSERT_RETURN_VALUE(ota_reporting_offset <= 7, 0);

  return (ota_reporting_offset * 6);
}

#ifdef FEATURE_LTE

/**
  @brief Decode an OTA E-UTRAN_FDD_MEASUREMENT_REPORT_OFFSET or
         E-UTRAN_TDD_MEASUREMENT_REPORT_OFFSET value.

  The decoding of E-UTRAN measurement report offset depends on the reported
  quantity that the network has configured.

  @note If the reported quantity is configured as RSRQ, the value returned
  by this function is the actual decoded value multiplied by 2.  This is
  because the decoded value increments in 0.5 dB steps, and it is simple
  to avoid floating point operations by just multiplying all relevant
  values by 2.  This does not affect the value that is reported to the
  network.

  @param rep_quant The E-UTRAN reporting quantity that the network has
         configured.
  @param ota_measurement_report_offset The OTA value to be decoded.
*/
static int16 rr_decode_eutran_measurement_report_offset(
  rr_eutran_rep_quant_e rep_quant,
  uint8                 ota_measurement_report_offset
)
{
  if (rep_quant == RR_EUTRAN_REP_QUANT_RSRP)
  {
    RR_ASSERT_RETURN_VALUE(ota_measurement_report_offset <= 63, -140);

    return (ota_measurement_report_offset - 140);
  }
  else /* rep_quant == RR_EUTRAN_REP_QUANT_RSRQ */
  {
    RR_ASSERT_RETURN_VALUE(ota_measurement_report_offset <= 63, -39);

    return (ota_measurement_report_offset - 39);
  }
}

#endif /* FEATURE_LTE */


/**
  @brief Calculate the length of the BA list used in dedicated mode.

  @return The length of the dedicated mode BA list.
*/
uint32 rr_dedicated_mode_ba_list_length(const gas_id_t gas_id)
{
  rr_ba_lists_t *rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

  BCCH_frequency_list_T* SIbis_BA_list = &rr_ba_lists_ptr->SI5_SI5bis_measurement_report_BA_list;
  BCCH_frequency_list_T* SIter_BA_list = &rr_ba_lists_ptr->SI5ter_measurement_report_BA_list;

  return (SIbis_BA_list->no_of_entries + SIter_BA_list->no_of_entries);
}

#ifdef FEATURE_LTE

/**
  @brief Calculate the length of the E-UTRAN neighbour list used in dedicated mode.

  @return The length of the dedicated mode E-UTRAN neighbour list.
*/
static uint32 rr_dedicated_mode_eutran_neighbour_list_length(const gas_id_t gas_id)
{
  gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_RETURN_PARAM(scell, 0);
  RR_NULL_CHECK_RETURN_PARAM(scell->gsm.dedicated_mode_params, 0);

  return scell->gsm.dedicated_mode_params->eutran_params.neighbour_list.count;
}

#endif /* FEATURE_LTE */

#ifdef FEATURE_WCDMA
#ifdef FEATURE_NONBLIND_HANDOVER_DEBUG

#endif /* #ifdef FEATURE_NONBLIND_HANDOVER_DEBUG */
#endif /* #ifdef FEATURE_WCDMA */


static uint8 rr_reporting_threshold(sys_band_T band, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data 
  gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_RETURN_PARAM(scell, RR_REPORTING_THRESHOLD_DEFAULT);
  RR_NULL_CHECK_RETURN_PARAM(scell->gsm.dedicated_mode_params, RR_REPORTING_THRESHOLD_DEFAULT);

  switch (band)
  {
    case SYS_BAND_PGSM_900:
    case SYS_BAND_EGSM_900:
    {
      return scell->gsm.dedicated_mode_params->gsm_params.meas_params.reporting_threshold_900;
    } /* SYS_BAND_PGSM_900 / SYS_BAND_EGSM_900 */

    case SYS_BAND_DCS_1800:
    {
      return scell->gsm.dedicated_mode_params->gsm_params.meas_params.reporting_threshold_1800;
    } /* SYS_BAND_DCS_1800 */

    case SYS_BAND_CELL_850:
    {
      return scell->gsm.dedicated_mode_params->gsm_params.meas_params.reporting_threshold_850;
    } /* SYS_BAND_CELL_850 */

    case SYS_BAND_PCS_1900:
    {
      return scell->gsm.dedicated_mode_params->gsm_params.meas_params.reporting_threshold_1900;
    } /* SYS_BAND_PCS_1900 */

    default:
    {
      MSG_GERAN_ERROR_1("Unexpected band: %d", band);
      return RR_REPORTING_THRESHOLD_DEFAULT;
    }
  }
}

static uint8 rr_reporting_offset(sys_band_T band, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_RETURN_PARAM(scell, RR_REPORTING_OFFSET_DEFAULT);
  RR_NULL_CHECK_RETURN_PARAM(scell->gsm.dedicated_mode_params, RR_REPORTING_OFFSET_DEFAULT);

  switch (band)
  {
    case SYS_BAND_PGSM_900:
    case SYS_BAND_EGSM_900:
    {
      return scell->gsm.dedicated_mode_params->gsm_params.meas_params.reporting_offset_900;
    } /* SYS_BAND_PGSM_900 / SYS_BAND_EGSM_900 */

    case SYS_BAND_DCS_1800:
    {
      return scell->gsm.dedicated_mode_params->gsm_params.meas_params.reporting_offset_1800;
    } /* SYS_BAND_DCS_1800 */

    case SYS_BAND_CELL_850:
    {
      return scell->gsm.dedicated_mode_params->gsm_params.meas_params.reporting_offset_850;
    } /* SYS_BAND_CELL_850 */

    case SYS_BAND_PCS_1900:
    {
      return scell->gsm.dedicated_mode_params->gsm_params.meas_params.reporting_offset_1900;
    } /* SYS_BAND_PCS_1900 */

    default:
    {
      MSG_GERAN_ERROR_1("Unexpected band: %d", band);
      return RR_REPORTING_OFFSET_DEFAULT;
    }
  }
}

/**
 * Finds the given frequency in the given BA list
 *
 * @param BCCH_ARFCN Frequency
 *
 * @param p_list     BA list
 *
 * @param index_ptr  [out] index of frequency in BA list
 *
 * @return TRUE if frequency found, FALSE otherwise
 */
static boolean rr_find_frequency_index_in_ba(
  ARFCN_T                BCCH_ARFCN,
  BCCH_frequency_list_T *p_list,
  byte                  *index_ptr
)
{
  boolean index_found = FALSE;
  byte no_of_entries;
  boolean freq_0_included = FALSE;
  byte index = 0;

  /* see if ARFCN 0 is in the list */
  if (p_list->no_of_entries > 0 && (p_list->BCCH_ARFCN[0].num == 0))
  {
    /* frequency 0 is included */
    freq_0_included = TRUE;
  }

  no_of_entries = p_list->no_of_entries;

  for (index = 0; index < no_of_entries; index++)
  {
    if (p_list->BCCH_ARFCN[index].num == BCCH_ARFCN.num)
    {
      index_found = TRUE;
      break;
    }
  }

  /* if ARFCN 0 is in the list of frequencies */
  if (freq_0_included)
  {
    /*********************************************************************************/
    /* based on Table 10.5.2.20.1/3GPP TS 04.18, ARFCN 0 should be placed at the     */
    /* end of the indexed list if included. Since in this case it is included,       */
    /* the index value must be shifted down by 1 (for all index values other than 0) */
    /* or placed at the end (for index value equal to 0)                             */
    /*********************************************************************************/
    if (index == 0)
    {
      index = no_of_entries - 1;
    }
    else if (index < no_of_entries)
    {
      index -= 1;
    }
    /* otherwise, the index was no_of_entries (indicating the freq was not found), leave as is */
  }
  /* otherwise, ARFCN 0 is not in the list of frequencies, no special action is required */

  /* set the index that was found */
  *index_ptr = index;

  return index_found;
}

/**
 * finds the index of the frequency parameter
 * in the BCCH frequency list. The frequency list used is
 * the one obtained from the SACCH, as per 05.08 8.4.4.
 * The index of the cells in the BCCH frequency list are
 * based on increasing ARFCN, except that ARFCN 0 is placed
 * at the end of the indexed list
 *
 * Indexing of cells is based on the BA(SACCH) list (or BA(BCCH) if not available)
 * but the BA list is divided into two parts, first the BA list based only on SI5/SI5bis
 * (or SI2/SI2bis if using BA(BCCH)), and then BA based only on SI5ter
 * The BA list based on SI5ter is placed after the BA list based on SI5/SI5bis for
 * indexing purposes
 *
 * @param BCCH_ARFCN ARFCN whose index is desired
 *
 * @return index of the ARFCN
 */
static byte rr_find_frequency_index(
  ARFCN_T BCCH_ARFCN,
  boolean *found_ptr,
  boolean in_dedicated,
  const gas_id_t gas_id
)
{
  boolean index_found = FALSE;
  byte                                index = 0;
  BCCH_frequency_list_T              *SIbis_BA_list;
  BCCH_frequency_list_T              *SIter_BA_list;
  gprs_scell_info_t                  *scell_info_ptr = rr_gprs_get_scell_info(gas_id);


  usat_system_information_database_T *usat_system_information_database_ptr;


  usat_system_information_database_ptr = rr_get_usat_system_information_database_ptr(gas_id);

  if (in_dedicated==TRUE)
  {

    rr_ba_lists_t *rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

    /* If in dedicated but the dedicated BAlist has yet to be received then use the idle BAlist */
    if (rr_has_complete_dedicated_BA_list(&scell_info_ptr->gsm))
    {
      SIbis_BA_list = &rr_ba_lists_ptr->SI5_SI5bis_measurement_report_BA_list;
    }
    else
    {
      SIbis_BA_list = &scell_info_ptr->gsm.complete_BA_list;
    }

    SIter_BA_list = &rr_ba_lists_ptr->SI5ter_measurement_report_BA_list;

  }
  else
  {
    /**********************************************************************/
    /* We normally do not generate measurement reports in idle, usat must */
    /* be asking us for measurement reports, in this case we should use   */
    /* the usat si db                                                     */
    /**********************************************************************/
    SIbis_BA_list = &usat_system_information_database_ptr->SIbis_measurement_report_BA_list;
    SIter_BA_list = &usat_system_information_database_ptr->SIter_measurement_report_BA_list;
  }

  /* search both the SI5_SI5bis and SI5ter BA list for measurement reports, with the
     SI5_SI5bis list first */
  index_found = rr_find_frequency_index_in_ba(
    BCCH_ARFCN,
    SIbis_BA_list,
    &index
  );

  if (!index_found)
  {
    /**************************************************************/
    /* Not in the SI5/SI5bis BA list, try the SI5ter BA list next */
    /**************************************************************/
    index_found = rr_find_frequency_index_in_ba(
      BCCH_ARFCN,
      SIter_BA_list,
      &index
    );

    /********************************************************************/
    /* add the length of the SI5_SI5bis BA list to the index to get     */
    /* the correct index for meas report. Remember that SI5ter BA       */
    /* list is placed at the end of the SI5_SI5bis BA list for indexing */
    /* purposes                                                         */
    /********************************************************************/
    index += SIbis_BA_list->no_of_entries;
  }

  if (!index_found)
  {
    /* not in either BA list, set the index to the combined length of the two BA lists */
    index = SIbis_BA_list->no_of_entries + SIter_BA_list->no_of_entries;
  }

  if (found_ptr)
  {
    *found_ptr = index_found;
  }
  /* if found_ptr is NULL, the index is expected to be there... if not */
  else
  {
    if (!index_found)
    {
      MSG_GERAN_MED_1("Expected to find ARFCN=%d in BA list",(int) BCCH_ARFCN.num);
    }
  }

  return index;
}


static byte rr_find_frequency_and_bsic_index(
  ARFCN_T arfcn,
  byte bsic,
  boolean *found_ptr,
  boolean in_dedicated,
  const gas_id_t gas_id
)
{
  boolean            arfcn_found = FALSE;
  BSIC_T             local_bsic;
  byte               arfcn_index = rr_find_frequency_index(arfcn, &arfcn_found, in_dedicated, gas_id);
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  CONVERT_TO_BSIC_T(local_bsic, bsic);

  if (arfcn_found && arfcn_index < MAX_GSM_NCELLS)
  {
    byte i;
    byte j;
    byte arfcn_and_bsic_index = 0;

    /* now we've found the arfcn in the SACCH list.  Need to find the actual index
    that should be reported.  */
    rr_gsm_bsic_description_T *bsic_description = &scell_info_ptr->gsm.gsm_bsic_description;

    if (bsic_description->bsics[arfcn_index].num_of_bsics > GSM_BSIC_DESCRIPTION_MAX_BSICS_PER_ENTRY)
    {
      MSG_GERAN_ERROR_3("bsic_description->bsics[%d].num_of_bsics (%d) out of range, truncating to %d",
                arfcn_index,
                bsic_description->bsics[arfcn_index].num_of_bsics,
                GSM_BSIC_DESCRIPTION_MAX_BSICS_PER_ENTRY);

      bsic_description->bsics[arfcn_index].num_of_bsics = GSM_BSIC_DESCRIPTION_MAX_BSICS_PER_ENTRY;
    }

    for (i = 0; (i < arfcn_index) && (i < MAX_GSM_NCELLS); i++)
    {
      arfcn_and_bsic_index += bsic_description->bsics[i].num_of_bsics;
    }

    /* now need to iterate over the BSICs for the index that we're actually interested
    in, stopping when we get to one that matches the BSIC that we're trying to find. */
    for (j = 0;
         j < bsic_description->bsics[arfcn_index].num_of_bsics;
         j++)
    {
      if (rr_same_BSIC(&bsic_description->bsics[arfcn_index].bsic[j], &local_bsic))
      {
        arfcn_and_bsic_index += j;
        break;
      }
    }

    *found_ptr = TRUE;

    return arfcn_and_bsic_index;
  }
  *found_ptr = FALSE;
  return arfcn_index;
}

#ifdef FEATURE_GSM_TDS
/**************************************************************************************
Function
rr_compare_tds_rscp_measurements

Description
Compare function used by the qsort algorithm for sorting based on RSCP.

Parameters
[IN]
const void *arg1, const void *arg2

Returns
  // <0 no swap
  // 0  don't care
  // >0  swap

**********************************************************************************************/
int rr_compare_tds_rscp_measurements(
  const void *arg1,
  const void *arg2
)
{
  ASSERT (arg1 != NULL);
  ASSERT (arg2 != NULL);

  // <0 no swap
  // 0  don't care
  // >0  swap

  /* sort in descending order since strongest cells need to be reported first */
  if ( ((rr_ded_irat_measurement_t*)arg1)->data.utran_tdd.rscp > ((rr_ded_irat_measurement_t*)arg2)->data.utran_tdd.rscp )
  {
    return -1; /* no swap */
  }
  else if ( ((rr_ded_irat_measurement_t*)arg1)->data.utran_tdd.rscp < ((rr_ded_irat_measurement_t*)arg2)->data.utran_tdd.rscp )
  {
    return 1; /* swap */
  }
  return 0;
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_GSM_TDS
/**************************************************************************************
Function
rr_find_tds_frequency_index

Description
This function loops the complete list to find the highest index that
matches the given uarfcn/cell parameter id. It also checks for the frequency
only slot for the given uarfcn. If found, it returns the index of the frequency
only slot.

Parameters
[IN]
wcdma_neighbor_cell_list_T *wcdma_neighbor_cell_list
3G Neighbor cell list
tds_ded_measurements_t *tds_meas
Cell whose index in the list need to be found out

[OUT]
boolean *found_index
TRUE if the index of the cell is found

boolean *has_freq_only
TRUE if there is a frequency only slot for the given cell

byte *freq_only_index
Index of the frequency only slot

Returns
Index of the input cell in the list

**********************************************************************************************/
static byte rr_find_tds_frequency_index(
  wcdma_ncell_list_data_t *wcdma_ncell_list,
  tds_ded_measurements_t *tds_meas,
  boolean *found_index,
  boolean *has_freq_only,
  byte *freq_only_index
)
{
  byte i;
  byte index = 0xFF;
  *found_index = FALSE;
  *has_freq_only = FALSE;
  *freq_only_index = 0xFF;

  for (i = 0; i < MAX_UTRAN_CELLS + MAX_UTRAN_FREQ; ++i)
  {
    wcdma_neighbor_cell_list_T *cell_index = &wcdma_ncell_list->wcdma_neighbor_cell_list[i];

    if (cell_index->valid_data == TRUE)
    {
      wcdma_cell_T *cell = &cell_index->cell;
      if (tds_meas->uarfcn == cell->uarfcn)
      {
        if (cell_index->frequency_only)
        {
          *has_freq_only = TRUE;
          *freq_only_index = i;
        }
        else /* if not frequency only */
        {
          if (tds_meas->cell_parameter_id == cell->scrambling_code)
          {
            *found_index = TRUE;
            index = i;
            /* Diversity has to be copied from neighbor list to the dedicated measurements as it
            is not received from L1. So that it can be passed to RRC in the IRAT-handover command */
            tds_meas->diversity = cell->diversity;
          }
        } /* if (frequency_only) */
      } /* if uarfcns are equal */
    } /* valid_data = TRUE */
  } /* for */
  return index;
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
/**************************************************************************************
Function
rr_find_wcdma_frequency_index

Description
This function loops the complete list to find the highest index that
matches the given uarfcn/scrambling code. It also checks for the frequency
only slot for the given uarfcn. If found, it returns the index of the frequency
only slot.

Parameters
[IN]
wcdma_neighbor_cell_list_T *wcdma_neighbor_cell_list
3G Neighbor cell list
wcdma_cell_T *input_cell
Cell whose index in the list need to be found out

[OUT]
boolean *found_index
TRUE if the index of the cell is found

boolean *has_freq_only
TRUE if there is a frequency only slot for the given cell

byte *freq_only_index
Index of the frequency only slot

Returns
Index of the input cell in the list

**********************************************************************************************/
static byte rr_find_wcdma_frequency_index(
  wcdma_neighbor_cell_list_T *wcdma_ncell_list,
  wcdma_cell_T *input_cell,
  boolean *found_index,
  boolean *has_freq_only,
  byte *freq_only_index
)
{
  byte i;
  byte index = 0xFF;
  *found_index = FALSE;
  *has_freq_only = FALSE;
  *freq_only_index = 0xFF;

  for (i = 0; i < MAX_UTRAN_CELLS + MAX_UTRAN_FREQ; ++i)
  {
    wcdma_neighbor_cell_list_T *cell_index = &wcdma_ncell_list[i];

    if (cell_index->valid_data == TRUE)
    {
      wcdma_cell_T *cell = &cell_index->cell;
      if (input_cell->uarfcn == cell->uarfcn)
      {
        if (cell_index->frequency_only)
        {
          *has_freq_only = TRUE;
          *freq_only_index = i;
        }
        else /* if not frequency only */
        {
          if (input_cell->scrambling_code == cell->scrambling_code)
          {
            *found_index = TRUE;
            index = i;
            /* Diversity has to be copied from neighbor list to the dedicated measurements as it
            is not received from L1. So that it can be passed to RRC in the IRAT-handover command */
            input_cell->diversity = cell->diversity;
          }
        } /* if (frequency_only) */
      } /* if uarfcns are equal */
    } /* valid_data = TRUE */
  } /* for */
  return index;
}

#endif /* FEATURE_WCDMA */


/*===========================================================================

FUNCTION rr_gsm_bsic_received_for_each_ncell

===========================================================================*/
static boolean rr_gsm_bsic_received_for_each_ncell(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_ba_lists_t     *rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

  if (scell_info_ptr->gsm.valid_dedicated_data_flag ||
      scell_info_ptr->gsm.valid_data_flag)
  {
    byte i;
    byte offset;

    /* if MI has been received */
    rr_gsm_bsic_description_T *gsm_bsic_desc = &scell_info_ptr->gsm.gsm_bsic_description;
    BCCH_frequency_list_T *si5_si5bis_ba_list = &rr_ba_lists_ptr->SI5_SI5bis_measurement_report_BA_list;
    BCCH_frequency_list_T *si5ter_ba_list = &rr_ba_lists_ptr->SI5ter_measurement_report_BA_list;

    for (i = 0; i < si5_si5bis_ba_list->no_of_entries; i++)
    {
      if (gsm_bsic_desc->bsics[i].num_of_bsics == 0)
      {
        MSG_GERAN_HIGH_1_G("BSIC description not received for SI5/SI5bis ncell at index %d", i);
        return FALSE;
      }
    }

    offset = i;

    for (i = 0; i < si5ter_ba_list->no_of_entries; i++)
    {
      if (gsm_bsic_desc->bsics[i+offset].num_of_bsics == 0)
      {
        MSG_GERAN_HIGH_1_G("BSIC description not received for SI5ter ncell at index %d", i);
        return FALSE;
      }
    }

    MSG_GERAN_HIGH_0_G("BSIC description has been received for all ncells");
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================

FUNCTION add_bsic_to_gsm_bsic_description

===========================================================================*/

static void add_bsic_to_gsm_bsic_description(
  rr_gsm_bsic_description_T *dst,
  BSIC_T *bsic_ptr,
  byte index
)
{
  ASSERT(bsic_ptr != NULL);

  if (index >= MAX_SIZE_OF_GSM_NEIGHBOUR_CELL_LIST)
  {
    MSG_GERAN_ERROR_0("GPRS BSIC Description overflow!");
    return;
  }
  else
  {
    gsm_bsic_description_entry_T *entry = &dst->bsics[index];

    if (entry->num_of_bsics < GSM_BSIC_DESCRIPTION_MAX_BSICS_PER_ENTRY)
    {
      entry->bsic[entry->num_of_bsics] = *bsic_ptr;

      MSG_GERAN_MED_3("ADDING BSIC entry (BA index: %d) : (%d, %d)",
        index,
        bsic_ptr->PLMN_colour_code,
        bsic_ptr->BS_colour_code
      );

      entry->num_of_bsics++;
    }
  }
}


/*******************************************************************************************
Function:rr_is_bsic_valid

Description:

Parameters:
  byte bsic,
  ARFCN_T arfcn

*******************************************************************************************/

static boolean rr_is_bsic_valid(
  byte bsic,
  ARFCN_T arfcn,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_ba_lists_t     *rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

  /* use information from the Measurement Information */
  if (scell_info_ptr->gsm.wcdma_ded_neighbor_list.valid_data == TRUE)
  {
    byte i;
    byte max = BA_MAX_LEN; /* gsm_bsic_desc.num_of_cells is not used */
    /* if MI has been received */
    rr_gsm_bsic_description_T *gsm_bsic_desc = &scell_info_ptr->gsm.gsm_bsic_description;
    BCCH_frequency_list_T *gsm_sacch_ncell_list = &rr_ba_lists_ptr->complete_BA_sacch;

    for (i = 0; i < max; ++i)
    {
      if (
          (gsm_sacch_ncell_list->BCCH_ARFCN[i].num == arfcn.num) &&
          (gsm_sacch_ncell_list->BCCH_ARFCN[i].band == arfcn.band)
         )
      {
        byte j;
        gsm_bsic_description_entry_T *entry = &gsm_bsic_desc->bsics[i];
        byte max_per_entry = entry->num_of_bsics;

        for (j = 0; j < max_per_entry; ++j)
        {
          if (bsic == CONVERT_FROM_BSIC_T(entry->bsic[j]))
          {
             return TRUE;
          }
        } /* for j */
      } /* if (gsm_ncell_list->ncells[i].arfcn == arfcn) */
    } /* for i */
  }
  /* use information from the SI2quater if MI was not available yet */
  else if (scell_info_ptr->gsm.utran_neighbor_list.valid_data == TRUE)
  {
    byte i;
    /* if still using idle mode information from SI2quater */
    gprs_ncells_info_t *ncells = rr_gprs_get_ncells_info(gas_id);
    gsm_neighbour_cell_list_t *gsm_ncell_list = &ncells->gsm_neighbour_cell_list;

    for (i = 0; i < gsm_ncell_list->num_of_ncells; i++)
    {
      if (ARFCNS_EQUAL(gsm_ncell_list->ncells[i].arfcn, arfcn))
      {
        if (gsm_ncell_list->ncells[i].bcch_or_pmo == BCCH_FORMAT)
        {
          uint32 j;

          for (j = 0; j < gsm_ncell_list->ncells[i].ncell_data.bcch.bsic_description.num_of_bsics; j++)
          {
            if (bsic == CONVERT_FROM_BSIC_T(gsm_ncell_list->ncells[i].ncell_data.bcch.bsic_description.bsic[j]))
            {
              return TRUE;
            }
          }
        }
        else
        {
          /* It's a PBCCH cell, so the network has given us the BSIC in PSI3.  Check
          that the requested BSIC is the same as this one. */
          if (bsic == CONVERT_FROM_BSIC_T(gsm_ncell_list->ncells[i].ncell_data.pmo_pcco.bsic))
          {
            return TRUE;
          }
        }
      }
    }
  } /* else if (system_information_database_ptr->utran_neighbor_list.valid_data == TRUE) */
  return FALSE;
}

/*******************************************************************************************
Function:rr_is_invalid_bsic_in_top_six

Description:

Parameters:
surrounding_measurement_T *meas_ptr
system_information_database_T *sidb

*******************************************************************************************/

static boolean rr_is_invalid_bsic_in_top_six(
  surrounding_measurement_T *mp,
  const gas_id_t gas_id
)
{
  byte i;
  byte max = mp->no_of_entries;

  for (i = 0; i < max; ++i)
  {
    if (rr_is_bsic_valid(
          mp->measurement[i].BSIC, mp->measurement[i].BCCH_ARFCN,
          gas_id
        ) == FALSE)
    {
      return TRUE;
    }
  }
  return FALSE;
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
/*******************************************************************************************
Function:rr_get_absolute_index_start_emr

Description:

Parameters:

*******************************************************************************************/
static byte rr_get_absolute_index_start_emr(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  if (scell_info_ptr->gsm.wcdma_ded_neighbor_list.valid_data == TRUE)
  {
    if (scell_info_ptr->gsm.wcdma_ded_neighbor_list.common_params.Absolute_Index_Start_EMR_PI)
    {
      return scell_info_ptr->gsm.wcdma_ded_neighbor_list.common_params.Absolute_Index_Start_EMR;
    }
  }
  else if (scell_info_ptr->gsm.utran_neighbor_list.valid_data == TRUE)
  {
    if (scell_info_ptr->gsm.utran_neighbor_list.common_params.Absolute_Index_Start_EMR_PI)
    {
      return scell_info_ptr->gsm.utran_neighbor_list.common_params.Absolute_Index_Start_EMR;
    }
  }
  return 0;
}
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

/*******************************************************************************************
Function:rr_get_scale_for_emr

Description:  !! there is another fucntion rr_get_scale_for_pemr(), which should be used
                 in PEMR
Parameters:

*******************************************************************************************/
static byte rr_get_scale_for_emr(const gas_id_t gas_id)
{
  byte               scale = 0; 
  gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_RETURN_PARAM(scell, 0);
  RR_NULL_CHECK_RETURN_PARAM(scell->gsm.dedicated_mode_params, 0);

  scale = scell->gsm.dedicated_mode_params->gsm_params.meas_params.scale_ord;
 
  if (scale == 2)
  {
    /* 45.008 - if scale_ord is 2 (automatic), the MS shall choose the lowest SCALE value
     * that is sufficient for reporting the strongest signal level within each EMR
     *
     * Since L1 to RR's Rxlev caps at 63, RR will never need to set scale value to 1
     * to accommodate signal stronger than -48dbm, except network explicitly ask to.
     */
    scale = 0;
  }

  return scale;
}

/**
  @brief Determine if the reported value for the GSM band given meets the
         requirements for inclusion in MEASUREMENT REPORTs sent by the UE.

  @param rxlev The RXLEV value to be reported.
  @param band The band of the cell that the RXLEV is measured for.
  @return TRUE if the reporting requirements are met, FALSE otherwise.
*/
static boolean rr_reported_value_equal_or_beyond_threshold(
  uint8 rxlev,
  sys_band_T band,
  const gas_id_t gas_id
)
{
  uint8 reporting_threshold = rr_reporting_threshold(band, gas_id);

  if (reporting_threshold == RR_REPORTING_THRESHOLD_ALWAYS)
  {
    return TRUE;
  }
  else if (reporting_threshold == RR_REPORTING_THRESHOLD_NEVER)
  {
    return FALSE;
  }
  else
  {
    uint8 reporting_threshold_db = reporting_threshold * 6;

    if (rxlev >= reporting_threshold_db)
    {
      MSG_GERAN_HIGH_2("rxlev (%d) >= reporting_threshold_db (%d) == TRUE", rxlev, reporting_threshold_db);
      return TRUE;
    }
    else
    {
      MSG_GERAN_HIGH_2("rxlev (%d) >= reporting_threshold_db (%d) == FALSE", rxlev, reporting_threshold_db);
      return FALSE;
    }
  }
}

/*******************************************************************************************
Function:rr_store_valid_cells_into_surround_meas

Description: This function does the NCC permitted check and if this check passes then store the measurement
into surround measurement otherwise it will discard it

Parameters:
surrounding_measurement_T* meas_ptr

Return: This function also returns the number of valid cells stored in the surrounding measurements
*******************************************************************************************/
static byte rr_store_valid_cells_into_surround_meas(surrounding_measurement_T *meas_ptr,
                                                    gprs_scell_info_t         *scell_info_ptr)
{
  BSIC_T             local_bsic;
  byte               mask;
  byte               cell_index;
  byte               BSIC;
  byte               valid_entry_index = 0;
  byte               NCC_permitted = 0;

  NCC_permitted = scell_info_ptr->gsm.NCC_permitted;

  for ( cell_index = 0; cell_index < meas_ptr->no_of_entries; cell_index++)
  {
    BSIC = meas_ptr->measurement[cell_index].BSIC;

    CONVERT_TO_BSIC_T(local_bsic, BSIC);

    mask = (byte) (0x01 << local_bsic.PLMN_colour_code);

    if ((NCC_permitted & mask) != 0)
    {
      if(cell_index != valid_entry_index)
      {
        meas_ptr->measurement[valid_entry_index].BCCH_ARFCN     = meas_ptr->measurement[cell_index].BCCH_ARFCN;
        meas_ptr->measurement[valid_entry_index].BSIC           = meas_ptr->measurement[cell_index].BSIC;
        meas_ptr->measurement[valid_entry_index].RXLEV_average  = meas_ptr->measurement[cell_index].RXLEV_average;

        MSG_GERAN_LOW_3("NCC : %d, BCC : %d, RXLEV : %d",
          (int) local_bsic.PLMN_colour_code,
          (int) local_bsic.BS_colour_code,
          (int) meas_ptr->measurement[valid_entry_index].RXLEV_average);

        meas_ptr->measurement[cell_index].BCCH_ARFCN.num        = 0;
        meas_ptr->measurement[cell_index].BCCH_ARFCN.band       = SYS_BAND_NONE;
        meas_ptr->measurement[cell_index].BSIC                  = 0;
        meas_ptr->measurement[cell_index].RXLEV_average         = 0;
      }
      valid_entry_index++;
    }
    else
    {
      meas_ptr->measurement[cell_index].BCCH_ARFCN.num          = 0;
      meas_ptr->measurement[cell_index].BCCH_ARFCN.band         = SYS_BAND_NONE;
      meas_ptr->measurement[cell_index].BSIC                    = 0;
      meas_ptr->measurement[cell_index].RXLEV_average           = 0;
    }
  }

  meas_ptr->no_of_entries = valid_entry_index;

  return meas_ptr->no_of_entries;
}

#ifdef FEATURE_LTE

static uint8 rr_get_reported_rsrp_6_bit(
  int16 rsrp
)
{
  if (rsrp < -140) return 0;
  if (rsrp < -138) return 1;
  if (rsrp < -136) return 2;
  if (rsrp < -134) return 3;
  if (rsrp < -132) return 4;
  if (rsrp < -130) return 5;
  if (rsrp < -128) return 6;
  if (rsrp < -126) return 7;
  if (rsrp < -124) return 8;
  if (rsrp < -122) return 9;
  if (rsrp < -120) return 10;
  if (rsrp < -118) return 11;
  if (rsrp < -116) return 12;
  if (rsrp < -114) return 13;
  if (rsrp < -112) return 14;
  if (rsrp < -110) return 15;
  if (rsrp < -108) return 16;
  if (rsrp < -106) return 17;
  if (rsrp < -78)
  {
    return (uint8) (rsrp + 124);
  }
  if (rsrp < -76) return 46;
  if (rsrp < -74) return 47;
  if (rsrp < -72) return 48;
  if (rsrp < -70) return 49;
  if (rsrp < -68) return 50;
  if (rsrp < -66) return 51;
  if (rsrp < -64) return 52;
  if (rsrp < -62) return 53;
  if (rsrp < -60) return 54;
  if (rsrp < -58) return 55;
  if (rsrp < -56) return 56;
  if (rsrp < -54) return 57;
  if (rsrp < -52) return 58;
  if (rsrp < -40) return 59;
  if (rsrp < -48) return 60;
  if (rsrp < -46) return 61;
  if (rsrp < -44) return 62;
  return 63;
}

static uint8 rr_get_reported_rsrp_3_bit(
  int16                     rsrp,
  rr_l1_eutran_type_e       eutran_type,
  rr_eutran_meas_params_t * eutran_meas_params
)
{
  uint8 offset;
  int   offset_dbm;
  uint8 reporting_granularity_db;

  if (eutran_type == RR_L1_EUTRAN_TYPE_FDD)
  {
    offset = eutran_meas_params->fdd.measurement_report_offset;
  }
  else /* eutran_type == RR_L1_EUTRAN_TYPE_TDD */
  {
    offset = eutran_meas_params->tdd.measurement_report_offset;
  }

  if (eutran_meas_params->reporting_granularity == 0)
  {
    reporting_granularity_db = 2;
  }
  else
  {
    reporting_granularity_db = 3;
  }

  /* 3GPP 45.008, Section 9 - Control Parameters:

  For RSRP the mapping is as follows:

  0 = -140 dBm,
  1 = -139 dBm,
  2 = -138 dBm,
  ...,
  62 = -78 dBm,
  63 = -77 dBm
  */
  offset_dbm = offset - 140;

  if (rsrp < (offset_dbm + reporting_granularity_db))       return 0;
  if (rsrp < (offset_dbm + (2 * reporting_granularity_db))) return 1;
  if (rsrp < (offset_dbm + (3 * reporting_granularity_db))) return 2;
  if (rsrp < (offset_dbm + (4 * reporting_granularity_db))) return 3;
  if (rsrp < (offset_dbm + (5 * reporting_granularity_db))) return 4;
  if (rsrp < (offset_dbm + (6 * reporting_granularity_db))) return 5;
  if (rsrp < (offset_dbm + (7 * reporting_granularity_db))) return 6;
  return 7;
}

static uint8 rr_get_reported_rsrq_6_bit(
  int16 rsrq 
)
{
  int16 rsrq_x2 = rsrq * 2; 

  if (rsrq_x2 < -39) return 0;
  if (rsrq_x2 < -38) return 1;
  if (rsrq_x2 < -37) return 2;
  if (rsrq_x2 < -36) return 3;
  if (rsrq_x2 < -35) return 4;
  if (rsrq_x2 < -34) return 5;
  if (rsrq_x2 < -33) return 6;
  if (rsrq_x2 < -32) return 7;
  if (rsrq_x2 < -31) return 8;
  if (rsrq_x2 < -30) return 9;
  if (rsrq_x2 < -29) return 10;
  if (rsrq_x2 < -28) return 11;
  if (rsrq_x2 < -27) return 12;
  if (rsrq_x2 < -26) return 13;
  if (rsrq_x2 < -25) return 14;
  if (rsrq_x2 < -24) return 15;
  if (rsrq_x2 < -23) return 16;
  if (rsrq_x2 < -22) return 17;
  if (rsrq_x2 < -21) return 18;
  if (rsrq_x2 < -20) return 19;
  if (rsrq_x2 < -19) return 20;
  if (rsrq_x2 < -18) return 21;
  if (rsrq_x2 < -17) return 22;
  if (rsrq_x2 < -16) return 23;
  if (rsrq_x2 < -15) return 24;
  if (rsrq_x2 < -14) return 25;
  if (rsrq_x2 < -13) return 26;
  if (rsrq_x2 < -12) return 27;
  if (rsrq_x2 < -11) return 28;
  if (rsrq_x2 < -10) return 29;
  if (rsrq_x2 < -9) return 30;
  if (rsrq_x2 < -8) return 31;
  if (rsrq_x2 < -7) return 32;
  if (rsrq_x2 < -6) return 33;
  return 34;
}

/**
  @brief Calculate the reported value when RSRQ reporting is enabled for 3-bit
         reporting.

  @note Ideally this function would be passed (RSRQ * 2) rather than RSRQ, as the
        mapping for RSRQ increments in steps of 0.5 dB.  Having (RSRQ * 2) would
        negate the need for floating point arithmetic, while allowing the complete
        mapping to be used.  Until L1 reports (RSRQ * 2) we will just double the
        value we receive from L1.
*/

static uint8 rr_get_reported_rsrq_3_bit(
  int16                     rsrq,   
  rr_l1_eutran_type_e       eutran_type,
  rr_eutran_meas_params_t * eutran_meas_params
)
{
  int16 rsrq_x2 = rsrq * 2; 
  uint8 offset;
  int   offset_db_x2;
  uint8 reporting_granularity_db;
  int   reporting_granularity_db_x2;

  if (eutran_type == RR_L1_EUTRAN_TYPE_FDD)
  {
    offset = eutran_meas_params->fdd.measurement_report_offset;
  }
  else /* eutran_type == RR_L1_EUTRAN_TYPE_TDD */
  {
    offset = eutran_meas_params->tdd.measurement_report_offset;
  }

  if (eutran_meas_params->reporting_granularity == 0)
  {
    reporting_granularity_db = 1;
  }
  else
  {
    reporting_granularity_db = 2;
  }

  reporting_granularity_db_x2 = reporting_granularity_db * 2;

  /* 3GPP 45.008, Section 9 - Control Parameters:

  For RSRQ the mapping is as follows:

  0 = -19.5 dB,
  1 = -19 dB,
  2 = -18.5 dB,
  ...,
  29 = -4 dB,
  30 = -3.5 dB

  Note that we are dealing with RSRQx2 to avoid floating point arithmetic, so
  all values in the mapping must be doubled, giving:

  0 = -39 dB,
  1 = -38 dB,
  2 = -37 dB,
  ...,
  29 = -8 dB,
  30 = -7 dB
  */
  offset_db_x2 = offset - 39;

  if (rsrq_x2 < (offset_db_x2 + reporting_granularity_db_x2))       return 0;
  if (rsrq_x2 < (offset_db_x2 + (2 * reporting_granularity_db_x2))) return 1;
  if (rsrq_x2 < (offset_db_x2 + (3 * reporting_granularity_db_x2))) return 2;
  if (rsrq_x2 < (offset_db_x2 + (4 * reporting_granularity_db_x2))) return 3;
  if (rsrq_x2 < (offset_db_x2 + (5 * reporting_granularity_db_x2))) return 4;
  if (rsrq_x2 < (offset_db_x2 + (6 * reporting_granularity_db_x2))) return 5;
  if (rsrq_x2 < (offset_db_x2 + (7 * reporting_granularity_db_x2))) return 6;
  return 7;
}

#endif /* FEATURE_LTE */

static void rr_dedicated_mode_irat_measurement_list_add_gsm_measurements(
  rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list,
  surrounding_measurement_T                 * gsm_measurements,
  rr_gsm_meas_params_t                      * gsm_meas_params,
  boolean                                     in_dedicated,
  const gas_id_t gas_id
)
{
  rr_dedicated_mode_irat_measurement_t * entry = NULL;
  uint32 i;


  RR_NULL_CHECK_RETURN_VOID(irat_measurement_list);
  RR_NULL_CHECK_RETURN_VOID(gsm_measurements);
  RR_NULL_CHECK_RETURN_VOID(gsm_meas_params);

  if (gsm_measurements->no_of_entries > 0)
  {
    MSG_GERAN_HIGH_1_G("Adding %d GSM cells to IRAT measurement list", gsm_measurements->no_of_entries);
  }

  for (i = 0; i < gsm_measurements->no_of_entries && i < ARR_SIZE(gsm_measurements->measurement); ++i)
  {
    boolean cell_index_valid = FALSE;
    uint8   cell_index = 0;

    ARFCN_T arfcn = gsm_measurements->measurement[i].BCCH_ARFCN;
    uint8   bsic  = gsm_measurements->measurement[i].BSIC;

    cell_index = rr_find_frequency_and_bsic_index(
      arfcn,
      bsic,
      &cell_index_valid,
      in_dedicated,
      gas_id
    );

    if (cell_index_valid)
    {
      /* Add the cell to the measurement list if it meets
      xxx_reporting_threshold criteria. */
      uint8 rxlev = MIN(gsm_measurements->measurement[i].RXLEV_average, 63);

      if (rr_reported_value_equal_or_beyond_threshold(rxlev, gsm_measurements->measurement[i].BCCH_ARFCN.band, gas_id))
      {
        if (irat_measurement_list->count < ARR_SIZE(irat_measurement_list->entries))
        {
          entry = &irat_measurement_list->entries[irat_measurement_list->count];

          entry->rat                         = RR_RAT_GSM;

          entry->data.gsm.arfcn              = arfcn;
          entry->data.gsm.bsic               = bsic;
          entry->data.gsm.cell_index         = cell_index;

          entry->multirat_reporting          = 0; /* Not relevant for GSM cells */
          entry->reported_value              = rxlev;
          entry->sort_value                  = rxlev - rr_decode_xxx_reporting_threshold(rr_reporting_threshold(arfcn.band,gas_id)) + rr_decode_xxx_reporting_offset(rr_reporting_offset(arfcn.band,gas_id));

          ++irat_measurement_list->count;
        }
        else
        {
          MSG_GERAN_ERROR_2_G("Unable to add ARFCN %d (band %d), list full",
                              arfcn.num,
                              arfcn.band);
        }
      }
      else
      {
        MSG_GERAN_MED_3_G("Skipping ARFCN %d (band %d), reported value %d below threshold",
                          arfcn.num,
                          arfcn.band,
                          rxlev);
      }
    }
    else
    {
      MSG_GERAN_ERROR_3_G("No index for ARFCN %d (band %d), BSIC %d",
                          arfcn.num,
                          arfcn.band,
                          bsic);
    }
  }
}

#ifdef FEATURE_WCDMA

static void rr_dedicated_mode_irat_measurement_list_add_wcdma_measurements(
  rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list,
  surr_wcdma_ded_measurements_T             * wcdma_measurements,
  rr_utran_meas_params_t                    * utran_meas_params,
  wcdma_neighbor_cell_list_T                * wcdma_neighbour_list,
  report_type_T                               report_type, 
  const gas_id_t gas_id 
)
{
  rr_dedicated_mode_irat_measurement_t * entry = NULL;
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(irat_measurement_list);
  RR_NULL_CHECK_RETURN_VOID(wcdma_measurements);
  RR_NULL_CHECK_RETURN_VOID(utran_meas_params);
  RR_NULL_CHECK_RETURN_VOID(wcdma_neighbour_list);

  for (i = 0;
       i < wcdma_measurements->num_entries &&
       i < ARR_SIZE(wcdma_measurements->meas);
       ++i)
  {
    boolean cell_index_valid = FALSE;
    uint8   cell_index = 0;
    boolean rssi_index_valid = FALSE;
    uint8   rssi_index = 0;

    /* WCDMA neighbour cells can only be included in measurement report
    messages if their reported index (the value used to uniquely identify the
    cell to the network) is in the range 0 - 63. */

    cell_index = rr_find_wcdma_frequency_index(
      wcdma_neighbour_list,
      &wcdma_measurements->meas[i].cell,
      &cell_index_valid,
      &rssi_index_valid,
      &rssi_index
    );

    if (cell_index_valid == FALSE)
    {
      MSG_GERAN_ERROR_2_G("Reporting index not valid for WCDMA cell (UARFCN: %d, SC: %d)",
                          wcdma_measurements->meas[i].cell.uarfcn,
                          wcdma_measurements->meas[i].cell.scrambling_code);
      continue;
    }

    if (report_type == REPORT_TYPE_ENHANCED)
    {
      cell_index += rr_get_absolute_index_start_emr(gas_id);
      if (rssi_index_valid)
      {
        rssi_index += rr_get_absolute_index_start_emr(gas_id);
      }
    }

    if (cell_index < MAX_CELL_COUNT_FOR_REPORT_TYPE(report_type))
    {
      uint8 reported_value;
      uint8 non_reported_value;
 
      uint8 fdd_reporting_threshold = rr_decode_xxx_reporting_threshold(
        utran_meas_params->fdd_reporting_threshold
      );

      uint8 fdd_reporting_offset = rr_decode_xxx_reporting_offset(
        utran_meas_params->fdd_reporting_offset
      );

      /* FDD_REPORTING_THRESHOLD_2 is not encoded, it is a 6-bit value that
      the non-reported value is compared to directly */
      uint8 fdd_reporting_threshold_2 = utran_meas_params->fdd_reporting_threshold_2;

      if (utran_meas_params->fdd_rep_quant == FDD_REP_QUANT_RSCP)
      {
        reported_value = rr_get_reported_RSCP(wcdma_measurements->meas[i].RSCP);
        non_reported_value = rr_get_reported_Ec_No(wcdma_measurements->meas[i].Ec_Nox2);
      }
      else
      {
        reported_value = rr_get_reported_Ec_No(wcdma_measurements->meas[i].Ec_Nox2);
        non_reported_value = rr_get_reported_RSCP(wcdma_measurements->meas[i].RSCP);
      }
 
      if (reported_value >= fdd_reporting_threshold &&
          non_reported_value >= fdd_reporting_threshold_2)
      {
        if (irat_measurement_list->count < ARR_SIZE(irat_measurement_list->entries))
        {
          entry = &irat_measurement_list->entries[irat_measurement_list->count];

          entry->rat                         = RR_RAT_WCDMA;

          entry->data.wcdma.uarfcn           = wcdma_measurements->meas[i].cell.uarfcn;
          entry->data.wcdma.scrambling_code  = wcdma_measurements->meas[i].cell.scrambling_code;
          entry->data.wcdma.reported_rssi    = rr_get_reported_RSSI(wcdma_measurements->meas[i].RSSI);
          entry->data.wcdma.cell_index       = cell_index;
          entry->data.wcdma.rssi_index_valid = rssi_index_valid;
          entry->data.wcdma.rssi_index       = rssi_index;

          entry->multirat_reporting          = utran_meas_params->fdd_multirat_reporting;
          entry->reported_value              = reported_value;
          entry->sort_value                  = (int16) (reported_value - fdd_reporting_threshold + fdd_reporting_offset);  

          ++irat_measurement_list->count;
        }
        else
        {
          MSG_GERAN_ERROR_2_G("Unable to add WCDMA cell UARFCN: %d, SC: %d, list full",
                              wcdma_measurements->meas[i].cell.uarfcn,
                              wcdma_measurements->meas[i].cell.scrambling_code);
        }
      }
      else
      {
        MSG_4(MSG_SSID_DFLT,
              MSG_LEGACY_MED,
              "Skipping WCDMA cell UARFCN: %d, SC: %d, reported_value: %d, non_reported_value: %d,  does not meet thresholds",
              wcdma_measurements->meas[i].cell.uarfcn,
              wcdma_measurements->meas[i].cell.scrambling_code,
              reported_value,
              non_reported_value);
      }
    }
    else
    {
      MSG_GERAN_ERROR_3_G("Skipping WCDMA cell UARFCN: %d, SC: %d, cell_index: %d out of range",
                          wcdma_measurements->meas[i].cell.uarfcn,
                          wcdma_measurements->meas[i].cell.scrambling_code,
                          cell_index);
    }
  }
}

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS

static void rr_dedicated_mode_irat_measurement_list_add_tdscdma_measurements(
  rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list,
  rr_ded_irat_measurements_t                * tdscdma_measurements,
  rr_utran_meas_params_t                    * utran_meas_params,
  wcdma_ncell_list_data_t                   * wcdma_ncell_list_data, 
  report_type_T                               report_type,
  const gas_id_t gas_id
)
{
  rr_dedicated_mode_irat_measurement_t * entry = NULL;
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(irat_measurement_list);
  RR_NULL_CHECK_RETURN_VOID(tdscdma_measurements);
  RR_NULL_CHECK_RETURN_VOID(utran_meas_params);
  RR_NULL_CHECK_RETURN_VOID(wcdma_ncell_list_data);

  for (i = 0;
       i < tdscdma_measurements->no_of_entries &&
       i < ARR_SIZE(tdscdma_measurements->entries);
       ++i)
  {
    boolean cell_index_valid = FALSE;
    uint8   cell_index = 0;
    boolean rssi_index_valid = FALSE;
    uint8   rssi_index = 0;

    /* WCDMA neighbour cells can only be included in measurement report
    messages if their reported index (the value used to uniquely identify the
    cell to the network) is in the range 0 - 63. */

    cell_index = rr_find_tds_frequency_index(
      wcdma_ncell_list_data,
      &tdscdma_measurements->entries[i].data.utran_tdd,
      &cell_index_valid,
      &rssi_index_valid,
      &rssi_index
    );

    if (cell_index_valid == FALSE)
    {
      MSG_GERAN_ERROR_2_G("Reporting index not valid for TDSCDMA cell (UARFCN: %d, CPID: %d)",
                          tdscdma_measurements->entries[i].data.utran_tdd.uarfcn,
                          tdscdma_measurements->entries[i].data.utran_tdd.cell_parameter_id);
      continue;
    }

    if (report_type == REPORT_TYPE_ENHANCED)
    {
      cell_index += rr_get_absolute_index_start_emr(gas_id);
    }

    if (cell_index < MAX_CELL_COUNT_FOR_REPORT_TYPE(report_type))
    {
      uint8 reported_value; 
      uint8 tdd_reporting_threshold = rr_decode_xxx_reporting_threshold(
        utran_meas_params->tdd_reporting_threshold
      );

      uint8 tdd_reporting_offset = rr_decode_xxx_reporting_offset(
        utran_meas_params->tdd_reporting_offset
      );

      reported_value = rr_get_reported_RSCP(tdscdma_measurements->entries[i].data.utran_tdd.rscp);

      if (reported_value >= tdd_reporting_threshold)
      {
        if (irat_measurement_list->count < ARR_SIZE(irat_measurement_list->entries))
        {
          entry = &irat_measurement_list->entries[irat_measurement_list->count];

          entry->rat                         = RR_RAT_UTRAN_TDD;

          entry->data.tdscdma.uarfcn     = tdscdma_measurements->entries[i].data.utran_tdd.uarfcn;
          entry->data.tdscdma.cpid       = tdscdma_measurements->entries[i].data.utran_tdd.cell_parameter_id;
          entry->data.tdscdma.cell_index = cell_index;

          entry->multirat_reporting      = utran_meas_params->tdd_multirat_reporting;
          entry->reported_value          = reported_value;
          entry->sort_value              = (int16) (reported_value - tdd_reporting_threshold + tdd_reporting_offset);

          ++irat_measurement_list->count;
        }
        else
        {
          MSG_GERAN_ERROR_2_G("Unable to add TDSCDMA cell UARFCN: %d, CPID: %d, list full",
                              tdscdma_measurements->entries[i].data.utran_tdd.uarfcn,
                              tdscdma_measurements->entries[i].data.utran_tdd.cell_parameter_id);
        }
      }
      else
      {
        MSG_GERAN_MED_3_G("Skipping TDSCDMA cell UARFCN: %d, CPID: %d, reported_value: %d, does not meet threshold",
                          tdscdma_measurements->entries[i].data.utran_tdd.uarfcn,
                          tdscdma_measurements->entries[i].data.utran_tdd.cell_parameter_id,
                          reported_value);
      }
    }
    else
    {
      MSG_GERAN_ERROR_3_G("Skipping TDSCDMA cell UARFCN: %d, SC: %d, cell_index: %d out of range",
                          tdscdma_measurements->entries[i].data.utran_tdd.uarfcn,
                          tdscdma_measurements->entries[i].data.utran_tdd.cell_parameter_id,
                          cell_index);
    }
  }
}

#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE

/**
  @brief Find the index of an EARFCN in the E-UTRAN Neighbour Cell list.

  3GPP 44.018 section 3.4.1.2.1.1a describes the construction and indexing
  of the E-UTRAN Neighbour Cell list.

  @param earfcn The EARFCN to search for
  @param frequency_index Variable in which to store the frequency index.
  @return TRUE if the frequency_index is valid, FALSE otherwise.
*/
static boolean rr_dedicated_mode_find_lte_frequency_index(
  lte_earfcn_t earfcn,
  uint8 * frequency_index,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);
  boolean             valid = FALSE;
  uint8               i;

  RR_NULL_CHECK_RETURN_PARAM(scell->gsm.dedicated_mode_params, FALSE);

  for (i = 0;
       i < scell->gsm.dedicated_mode_params->eutran_params.neighbour_list.count &&
       i < ARR_SIZE(scell->gsm.dedicated_mode_params->eutran_params.neighbour_list.entries);
       i++)
  {
    if (earfcn == scell->gsm.dedicated_mode_params->eutran_params.neighbour_list.entries[i].frequency.earfcn)
    {
      if (frequency_index != NULL)
      {
        *frequency_index = i;
        valid = TRUE;
      }
    }
  }

  return valid;
}


static boolean rr_dedicated_mode_is_lte_cell_allowed(
  rr_dedicated_mode_eutran_neighbour_list_t * neighbour_list,
  lte_earfcn_t                                earfcn,
  uint16                                      pcid
)
{
  uint32              i;

  RR_NULL_CHECK_RETURN_PARAM(neighbour_list, FALSE);
  RR_ASSERT_RETURN_VALUE(pcid <= RR_LTE_PCID_MAX, FALSE);

  for (i = 0; i < neighbour_list->count && i < RR_EUTRAN_NEIGHBOUR_LIST_LENGTH; ++i)
  {
    /* The PCID is allowed if it is not present in the Not Allowed Cells bitmap. */

    if (neighbour_list->entries[i].frequency.earfcn == earfcn)
    {
      if (RR_PCID_GROUP_IE_GET(neighbour_list->entries[i].not_allowed_cells.bitmap, pcid))
      {
        /* The PCID is in the Not Allowed Cells bitmap, so is not allowed. */
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    }
  }

  MSG_GERAN_ERROR_1("EARFCN %d not found in list, assuming Not Allowed", earfcn);
  return FALSE;
}

static int16 rr_compute_modified_offset_for_earfcn(rr_l1_lte_measurements_t* meas_entry_p, rr_eutran_meas_params_t* eutran_meas_params_p, rr_l1_eutran_type_e eutran_type, const gas_id_t gas_id )
{
  uint32  cell;
  int16 max_rsrp, reported_offset,  received_offset;
  
  max_rsrp = -141;
  reported_offset=0;
/* For a given a Given EARFCN */
/* Loop on all the cells to find the strongest cell */

  for (cell = 0;
     cell < meas_entry_p->no_of_entries &&
     cell < ARR_SIZE(meas_entry_p->entries);
     ++cell)
  {

     /* Compue the MAX RSP for the cell */
     if ( meas_entry_p->entries[cell].rsrp > max_rsrp )
     {
       max_rsrp = meas_entry_p->entries[cell].rsrp;
     }
  }

  /* Now that we have the Max RSRP, lets get the equivalent threshold for (max_rsrp-12) */
  /* Compare the current threshold versus the new threshold */
  /* Return the modified threshold */

  if (eutran_type == RR_L1_EUTRAN_TYPE_FDD  )
  {
    received_offset =  rr_decode_eutran_measurement_report_offset(
                        RR_EUTRAN_REP_QUANT_RSRP,
                        eutran_meas_params_p->fdd.measurement_report_offset);
  }
  else
  {
    received_offset = rr_decode_eutran_measurement_report_offset(
                        RR_EUTRAN_REP_QUANT_RSRP,
                        eutran_meas_params_p->tdd.measurement_report_offset);
  }
  
  MSG_GERAN_HIGH_2_G("rr_compute_modified_offset_for_earfcn, max_rsrp %x and received_offset %x", max_rsrp, received_offset );

  /* Set Offset to the Highest RSRP on the cell -12 dbM */
  reported_offset = (max_rsrp-MAX_RSRP_DIFFERENCE_FOR_PRUNING) ;

  if ( received_offset  > reported_offset )
  {
    /* Retain the offset to the Received offset in this case */
    reported_offset = received_offset;
  }

  MSG_GERAN_HIGH_2_G("rr_compute_modified_offset_for_earfcn, Reported OFfset Offset %x and Received_offset %x", reported_offset, received_offset );

  return (reported_offset);
}


static uint8 rr_compute_modified_threshold_for_earfcn(rr_l1_lte_measurements_t* meas_entry_p, rr_eutran_meas_params_t* eutran_meas_params_p, rr_l1_eutran_type_e eutran_type, const gas_id_t gas_id )
{
  uint32  cell;
  int16 max_rsrp;
  uint8 reporting_threshold, reported_rsrp;
  
  max_rsrp = -141; /* The minimum */
  
  /* For a given a Given EARFCN */
  /* Loop on all the cells to find the strongest cell */

  for (cell = 0;
     cell < meas_entry_p->no_of_entries &&
     cell < ARR_SIZE(meas_entry_p->entries);
     ++cell)
  {
    /* Compue the MAX RSP for the cell */
    if ( meas_entry_p->entries[cell].rsrp > max_rsrp )
    {
      max_rsrp = meas_entry_p->entries[cell].rsrp;
    }
  }

  if (eutran_type == RR_L1_EUTRAN_TYPE_FDD  )
  {
    reporting_threshold =  rr_decode_xxx_reporting_threshold( eutran_meas_params_p->fdd.reporting_threshold);
  }
  else
  {
    reporting_threshold = rr_decode_xxx_reporting_threshold( eutran_meas_params_p->tdd.reporting_threshold);
  }

  MSG_GERAN_HIGH_2_G("rr_compute_modified_threshold_for_earfcn, max_rsrp %x and reporting_threshold %x", max_rsrp, reporting_threshold );


  reported_rsrp = rr_get_reported_rsrp_6_bit( (max_rsrp - MAX_RSRP_DIFFERENCE_FOR_PRUNING) );

  MSG_GERAN_HIGH_2_G("rr_compute_modified_threshold_for_earfcn, reported_rsrp %x and reporting_threshold %x", reported_rsrp, reporting_threshold );

  if ( reported_rsrp > reporting_threshold )
  {
    /* Modify the Network Provided Threshold to the value of Strongest Cell - 12 dbM value */
    reporting_threshold = reported_rsrp ;
  }

  return (reporting_threshold);
}


 
static void rr_dedicated_mode_irat_measurement_list_add_lte_measurements(
  rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list,
  rr_ded_meas_reporting_lte_measurements_t  * lte_measurements,
  rr_dedicated_mode_eutran_params_t         * eutran_params,
  report_type_T                               report_type,
  const gas_id_t gas_id
)
{
 
  rr_eutran_meas_params_t              * eutran_meas_params;
  uint32 f;
  uint32 c;
  uint8                                  modified_threshold;
  int16                                  modified_offset;
  boolean                                filter_cells_based_on_rsrp;

  rr_dedicated_mode_irat_measurement_t * entry = NULL;


  RR_NULL_CHECK_RETURN_VOID(irat_measurement_list);
  RR_NULL_CHECK_RETURN_VOID(lte_measurements);
  RR_NULL_CHECK_RETURN_VOID(eutran_params);

  filter_cells_based_on_rsrp = FALSE;
  modified_threshold = 0;
  modified_offset = 0;
  
  eutran_meas_params = &eutran_params->meas_params;

  if (eutran_meas_params->rep_quant == RR_EUTRAN_REP_QUANT_RSRP)
  {
    filter_cells_based_on_rsrp = TRUE;
  }
  
  for (f = 0;
       f < ARR_SIZE(lte_measurements->entries);
       ++f)
  {
    lte_earfcn_t        earfcn;
    rr_l1_eutran_type_e eutran_type;
    uint8               frequency_index = 0;
    boolean             frequency_index_valid;

    if (lte_measurements->entries[f] == NULL)
    {
      continue;
    }

    earfcn = lte_measurements->entries[f]->earfcn;

    frequency_index_valid = rr_dedicated_mode_find_lte_frequency_index(
      earfcn,
      &frequency_index,
      gas_id
    );

    if (frequency_index_valid == FALSE)
    {
      MSG_GERAN_ERROR_1_G("Invalid frequency index for EARFCN %d", earfcn);
    }

    if (lte_rrc_cell_is_tdd(earfcn))
    {
      eutran_type = RR_L1_EUTRAN_TYPE_TDD;
    }
    else
    {
      eutran_type = RR_L1_EUTRAN_TYPE_FDD;
    }

    if ( filter_cells_based_on_rsrp )
    {
      if ( report_type == REPORT_TYPE_ENHANCED )
      {       
        modified_threshold = rr_compute_modified_threshold_for_earfcn( lte_measurements->entries[f], eutran_meas_params, eutran_type, gas_id );
      }
      else
      {
        modified_offset = rr_compute_modified_offset_for_earfcn( lte_measurements->entries[f], eutran_meas_params, eutran_type, gas_id );
      }
    }
    
    for (c = 0;
         c < lte_measurements->entries[f]->no_of_entries &&
         c < ARR_SIZE(lte_measurements->entries[f]->entries);
         ++c)
    {
      uint8 reported_value;
      uint8 non_reported_value;

      if (rr_dedicated_mode_is_lte_cell_allowed(
            &eutran_params->neighbour_list,
            earfcn,
            lte_measurements->entries[f]->entries[c].pcid) == FALSE)
      {
        MSG_GERAN_MED_2_G("Skipping LTE cell EARFCN %d, PCID %d, Not Allowed",
                          earfcn,
                          lte_measurements->entries[f]->entries[c].pcid);
        continue;
      }

      if (report_type == REPORT_TYPE_ENHANCED) 
      {
        if (eutran_meas_params->rep_quant == RR_EUTRAN_REP_QUANT_RSRP)
        {
          reported_value     = rr_get_reported_rsrp_6_bit(lte_measurements->entries[f]->entries[c].rsrp);
          non_reported_value = rr_get_reported_rsrq_6_bit(lte_measurements->entries[f]->entries[c].rsrq);
        }
        else /* eutran_meas_params->rep_quant == RR_EUTRAN_REP_QUANT_RSRQ */
        {
          reported_value     = rr_get_reported_rsrq_6_bit(lte_measurements->entries[f]->entries[c].rsrq);
          non_reported_value = rr_get_reported_rsrp_6_bit(lte_measurements->entries[f]->entries[c].rsrp);
        }

        /* The checks for cell suitability must also take in to accound FDD or TDD. */
        if (eutran_type == RR_L1_EUTRAN_TYPE_FDD)
        {
          uint8 eutran_fdd_reporting_threshold;
          uint8 eutran_fdd_reporting_offset;
          uint8 eutran_fdd_reporting_threshold_2;

          eutran_fdd_reporting_threshold = rr_decode_xxx_reporting_threshold(
            eutran_meas_params->fdd.reporting_threshold
          );
          eutran_fdd_reporting_offset = rr_decode_xxx_reporting_offset(
            eutran_meas_params->fdd.reporting_offset
          );
          eutran_fdd_reporting_threshold_2 = eutran_meas_params->fdd.reporting_threshold_2;

          /* When filtering of ghost cells is not used use original threshold for adding cells. 
           * When ghost cell filtering is used modified threshold will be used for adding cells. 
           * Modified threshold is computed at begining of this function. */
          if (!filter_cells_based_on_rsrp)
          {
            modified_threshold = eutran_fdd_reporting_threshold;
          }
          
          if (reported_value >= modified_threshold &&
              non_reported_value >= eutran_fdd_reporting_threshold_2)
          {
            if (irat_measurement_list->count < ARR_SIZE(irat_measurement_list->entries))
            {
              entry = &irat_measurement_list->entries[irat_measurement_list->count];

              entry->rat                      = RR_RAT_LTE;
              entry->data.lte.earfcn          = earfcn;
              entry->data.lte.pcid            = lte_measurements->entries[f]->entries[c].pcid;
              entry->data.lte.frequency_index = frequency_index;

              entry->multirat_reporting       = eutran_meas_params->multirat_reporting;
              entry->reported_value           = reported_value;
              entry->sort_value               = (int16) (reported_value - eutran_fdd_reporting_threshold + eutran_fdd_reporting_offset);

              ++irat_measurement_list->count;
            }
            else
            {
              MSG_GERAN_ERROR_2_G("Unable to add LTE cell EARFCN: %d, PCID: %d, list full",
                                  lte_measurements->entries[f]->earfcn,
                                  lte_measurements->entries[f]->entries[c].pcid);
            }
          }
          else
          {
            MSG_6(MSG_SSID_DFLT,
                  MSG_LEGACY_MED,
                  "LTE cell EARFCN: %d, PCID: %d, Reported Value: %d, Non-Reporting Value: %d does not meet modified thresholds %d, %d",
                  lte_measurements->entries[f]->earfcn,
                  lte_measurements->entries[f]->entries[c].pcid,
                  reported_value,
                  non_reported_value,
                  modified_threshold,
                  eutran_fdd_reporting_threshold_2);

          }
        }
        else /* eutran_type == RR_L1_EUTRAN_TYPE_TDD */
        { 
          uint8 eutran_tdd_reporting_threshold;
          uint8 eutran_tdd_reporting_offset;
          uint8 eutran_tdd_reporting_threshold_2;

          eutran_tdd_reporting_threshold = rr_decode_xxx_reporting_threshold(
            eutran_meas_params->tdd.reporting_threshold
          );
          eutran_tdd_reporting_offset = rr_decode_xxx_reporting_offset(
            eutran_meas_params->tdd.reporting_offset
          );
          eutran_tdd_reporting_threshold_2 = eutran_meas_params->tdd.reporting_threshold_2;

          /* When filtering of ghost cells is not used use original threshold for adding cells. 
           * When ghost cell filtering is used modified threshold will be used for adding cells. 
           * Modified threshold is computed at begining of this function. */
          if (!filter_cells_based_on_rsrp)
          {
            modified_threshold = eutran_tdd_reporting_threshold;
          }
          
          if (reported_value >= modified_threshold &&
              non_reported_value >= eutran_tdd_reporting_threshold_2)
          {
            if (irat_measurement_list->count < ARR_SIZE(irat_measurement_list->entries))
            {
              entry = &irat_measurement_list->entries[irat_measurement_list->count];

              entry->rat                      = RR_RAT_LTE;
              entry->data.lte.earfcn          = earfcn;
              entry->data.lte.pcid            = lte_measurements->entries[f]->entries[c].pcid;
              entry->data.lte.frequency_index = frequency_index;

              entry->multirat_reporting       = eutran_meas_params->multirat_reporting;
              entry->reported_value           = reported_value;
              entry->sort_value               = (int16) (reported_value - eutran_tdd_reporting_threshold + eutran_tdd_reporting_offset);

              ++irat_measurement_list->count;
            }
            else
            {
              MSG_GERAN_ERROR_2_G("Unable to add LTE cell EARFCN: %d, PCID: %d, list full",
                                  lte_measurements->entries[f]->earfcn,
                                  lte_measurements->entries[f]->entries[c].pcid);
            }
          }
        }
      }
      else /* report_type == REPORT_TYPE_NORMAL uses 3-bit reporting values */
      {
        uint8 reported_value_6_bit;

        if (eutran_type == RR_L1_EUTRAN_TYPE_FDD)
        {
          int16 eutran_fdd_measurement_report_offset;
          int16 measured_value;
          uint8 eutran_fdd_reporting_offset = rr_decode_xxx_reporting_offset(
            eutran_meas_params->fdd.reporting_offset
          );

          if (eutran_meas_params->rep_quant == RR_EUTRAN_REP_QUANT_RSRP)
          {
            reported_value       = rr_get_reported_rsrp_3_bit(lte_measurements->entries[f]->entries[c].rsrp,
                                                              RR_L1_EUTRAN_TYPE_FDD,
                                                              eutran_meas_params);
            reported_value_6_bit = rr_get_reported_rsrp_6_bit(lte_measurements->entries[f]->entries[c].rsrp);
            non_reported_value   = rr_get_reported_rsrq_3_bit(lte_measurements->entries[f]->entries[c].rsrq,
                                                              RR_L1_EUTRAN_TYPE_FDD,
                                                              eutran_meas_params);
            measured_value       = lte_measurements->entries[f]->entries[c].rsrp;

            eutran_fdd_measurement_report_offset = rr_decode_eutran_measurement_report_offset(
              RR_EUTRAN_REP_QUANT_RSRP,
              eutran_meas_params->fdd.measurement_report_offset
            );
          }
          else /* eutran_meas_params->rep_quant == RR_EUTRAN_REP_QUANT_RSRQ */
          {
            reported_value       = rr_get_reported_rsrq_3_bit(lte_measurements->entries[f]->entries[c].rsrq,
                                                              RR_L1_EUTRAN_TYPE_FDD,
                                                              eutran_meas_params);
            reported_value_6_bit = rr_get_reported_rsrq_6_bit(lte_measurements->entries[f]->entries[c].rsrq);
            non_reported_value   = rr_get_reported_rsrp_3_bit(lte_measurements->entries[f]->entries[c].rsrp,
                                                              RR_L1_EUTRAN_TYPE_FDD,
                                                              eutran_meas_params);
            measured_value       = lte_measurements->entries[f]->entries[c].rsrq * 2;

            eutran_fdd_measurement_report_offset = rr_decode_eutran_measurement_report_offset(
              RR_EUTRAN_REP_QUANT_RSRQ,
              eutran_meas_params->fdd.measurement_report_offset
            );
          }

          /* When filtering of ghost cells is not used use original threshold for adding cells. 
           * When ghost cell filtering is used modified threshold will be used for adding cells. 
           * Modified threshold is computed at begining of this function. */
          if (!filter_cells_based_on_rsrp)
          {
            modified_offset = eutran_fdd_measurement_report_offset;
          }
          
          if (measured_value >= modified_offset &&
              non_reported_value >= eutran_meas_params->fdd.reporting_threshold_2)
          {
            if (irat_measurement_list->count < ARR_SIZE(irat_measurement_list->entries))
            {
              entry = &irat_measurement_list->entries[irat_measurement_list->count];

              entry->rat                      = RR_RAT_LTE;
              entry->data.lte.earfcn          = earfcn;
              entry->data.lte.pcid            = lte_measurements->entries[f]->entries[c].pcid;
              entry->data.lte.frequency_index = frequency_index;

              entry->multirat_reporting       = eutran_meas_params->multirat_reporting;
              entry->reported_value           = reported_value;
              entry->sort_value               = (int16) (reported_value_6_bit - eutran_fdd_measurement_report_offset + eutran_fdd_reporting_offset);

              ++irat_measurement_list->count;
            }
            else
            {
              MSG_GERAN_ERROR_2_G("Unable to add LTE cell EARFCN: %d, PCID: %d, list full",
                                  lte_measurements->entries[f]->earfcn,
                                  lte_measurements->entries[f]->entries[c].pcid);
            }
          }
        }        
        else /* eutran_type == RR_L1_EUTRAN_TYPE_TDD */
        {
          int16 eutran_tdd_measurement_report_offset;
          int16 measured_value;
          uint8 eutran_tdd_reporting_offset = rr_decode_xxx_reporting_offset(
            eutran_meas_params->tdd.reporting_offset
          );

          if (eutran_meas_params->rep_quant == RR_EUTRAN_REP_QUANT_RSRP)
          {
            reported_value       = rr_get_reported_rsrp_3_bit(lte_measurements->entries[f]->entries[c].rsrp,
                                                              RR_L1_EUTRAN_TYPE_TDD,
                                                              eutran_meas_params);
            reported_value_6_bit = rr_get_reported_rsrp_6_bit(lte_measurements->entries[f]->entries[c].rsrp);
            non_reported_value   = rr_get_reported_rsrq_3_bit(lte_measurements->entries[f]->entries[c].rsrq,
                                                              RR_L1_EUTRAN_TYPE_TDD,
                                                              eutran_meas_params);
            measured_value       = lte_measurements->entries[f]->entries[c].rsrp;

            eutran_tdd_measurement_report_offset = rr_decode_eutran_measurement_report_offset(
              RR_EUTRAN_REP_QUANT_RSRP,
              eutran_meas_params->tdd.measurement_report_offset
            );
          }
          else /* eutran_meas_params->rep_quant == RR_EUTRAN_REP_QUANT_RSRQ */
          {
            reported_value       = rr_get_reported_rsrq_3_bit(lte_measurements->entries[f]->entries[c].rsrq,
                                                              RR_L1_EUTRAN_TYPE_TDD,
                                                              eutran_meas_params);
            reported_value_6_bit = rr_get_reported_rsrq_6_bit(lte_measurements->entries[f]->entries[c].rsrq);
            non_reported_value   = rr_get_reported_rsrp_3_bit(lte_measurements->entries[f]->entries[c].rsrp,
                                                              RR_L1_EUTRAN_TYPE_TDD,
                                                              eutran_meas_params);
            measured_value       = lte_measurements->entries[f]->entries[c].rsrq;

            eutran_tdd_measurement_report_offset = rr_decode_eutran_measurement_report_offset(
              RR_EUTRAN_REP_QUANT_RSRQ,
              eutran_meas_params->tdd.measurement_report_offset
            );
          }

          /* When filtering of ghost cells is not used use original threshold for adding cells. 
           * When ghost cell filtering is used modified threshold will be used for adding cells. 
           * Modified threshold is computed at begining of this function. */
          if (!filter_cells_based_on_rsrp)
          {
            modified_offset = eutran_tdd_measurement_report_offset;
          }
          
          if (measured_value >= modified_offset &&
              non_reported_value >= eutran_meas_params->tdd.reporting_threshold_2)
          {
            if (irat_measurement_list->count < ARR_SIZE(irat_measurement_list->entries))
            {
              entry = &irat_measurement_list->entries[irat_measurement_list->count];

              entry->rat                      = RR_RAT_LTE;
              entry->data.lte.earfcn          = earfcn;
              entry->data.lte.pcid            = lte_measurements->entries[f]->entries[c].pcid;
              entry->data.lte.frequency_index = frequency_index;

              entry->multirat_reporting       = eutran_meas_params->multirat_reporting;
              entry->reported_value           = reported_value;
              entry->sort_value               = (int16) (reported_value_6_bit - eutran_tdd_measurement_report_offset + eutran_tdd_reporting_offset);

              ++irat_measurement_list->count;
            }
            else
            {
              MSG_GERAN_ERROR_2_G("Unable to add LTE cell EARFCN: %d, PCID: %d, list full",
                                  lte_measurements->entries[f]->earfcn,
                                  lte_measurements->entries[f]->entries[c].pcid);
            }
          }
        }
      }
    }
  }
}

#endif /* FEATURE_LTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
 
static void rr_dedicated_mode_irat_measurement_list_dump(
  rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(irat_measurement_list);

  MSG_GERAN_HIGH_0("Measurement List:");

  for (i = 0; i < irat_measurement_list->count && i < ARR_SIZE(irat_measurement_list->entries); ++i)
  {
    rr_dedicated_mode_irat_measurement_t * cell = &irat_measurement_list->entries[i];

    switch (cell->rat)
    {
      case RR_RAT_GSM:
      {
        MSG_5(MSG_SSID_DFLT,
              MSG_LEGACY_HIGH,
              "[%02d]: GSM cell, ARFCN %d, Cell Index: %d, Reported Value: %d, Sort Value: %d",
              i,
              cell->data.gsm.arfcn.num,
              cell->data.gsm.cell_index,
              cell->reported_value,
              cell->sort_value);
        break;
      } /* RR_RAT_GSM */

#ifdef FEATURE_WCDMA
      case RR_RAT_WCDMA:
      {
        MSG_6(MSG_SSID_DFLT,
              MSG_LEGACY_HIGH,
              "[%02d]: WCDMA cell, UARFCN %d, SC: %d, Cell Index: %d, Reported Value: %d, Sort Value: %d",
              i,
              cell->data.wcdma.uarfcn,
              cell->data.wcdma.scrambling_code,
              cell->data.wcdma.cell_index,
              cell->reported_value,
              cell->sort_value);
        break;
      } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
      case RR_RAT_UTRAN_TDD:
      {
        MSG_6(MSG_SSID_DFLT,
              MSG_LEGACY_HIGH,
              "[%02d]: TDSCDMA cell, UARFCN %d, CPID: %d, Cell Index: %d, Reported Value: %d, Sort Value: %d",
              i,
              cell->data.tdscdma.uarfcn,
              cell->data.tdscdma.cpid,
              cell->data.tdscdma.cell_index,
              cell->reported_value,
              cell->sort_value);
        break;
      } /* RR_RAT_UTRAN_TDD */
#endif
#ifdef FEATURE_LTE
      case RR_RAT_LTE:
      {
        MSG_6(MSG_SSID_DFLT,
              MSG_LEGACY_HIGH,
              "[%02d]: LTE cell, EARFCN %d, PCID: %d, Frequency Index: %d, Reported Value: %d, Sort Value: %d",
              i,
              cell->data.lte.earfcn,
              cell->data.lte.pcid,
              cell->data.lte.frequency_index,
              cell->reported_value,
              cell->sort_value);
        break;
      } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_LTE */

      default:
      {
        MSG_GERAN_ERROR_1("Unexpected RAT: %d", cell->rat);
      }
    }
  }
}


/**
  @brief Create the IRAT measurement list.

  This function creates the IRAT measurement list, which contains all IRAT
  measurements received from L1, in a unified list suitable.  This list is
  sorted using various functions so that a measurement report can be encoded
  according to the rules defined in 3GPP Sections 8.4.3 and 8.4.7.

  @param irat_measurement_list The destination structure that will store the
         completed list.
  @param wcdma_measurements WCDMA measurements received from L1.
  @param tdscdma_measurements TDSCDMA measurements received from L1.
  @param lte_measurements LTE measurements received from L1.
  @param utran_meas_params The current UTRAN measurement parameters received
         from the network.
  @param eutran_meas_params The current E-UTRAN measurement parameters received
         from the network.
*/

static void rr_dedicated_mode_irat_measurement_list_create(
  rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list,
  surrounding_measurement_T                 * gsm_measurements,
#ifdef FEATURE_WCDMA
  surr_wcdma_ded_measurements_T             * wcdma_measurements,
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  rr_ded_irat_measurements_t                * tdscdma_measurements,
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
  rr_ded_meas_reporting_lte_measurements_t  * lte_measurements,
#endif /* FEATURE_LTE */
  rr_dedicated_mode_params_t                * dedicated_mode_params,
  boolean                                     in_dedicated,
  const                                       gas_id_t gas_id
)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  wcdma_ncell_list_data_t              * wcdma_ncell_list_data_ptr;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

  RR_NULL_CHECK_RETURN_VOID(irat_measurement_list);
  RR_NULL_CHECK_RETURN_VOID(dedicated_mode_params);

  memset(irat_measurement_list, 0, sizeof(*irat_measurement_list));

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  wcdma_ncell_list_data_ptr = rr_get_wcdma_ncell_list_data_ptr(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
 
  if (gsm_measurements != NULL)
  {
    rr_dedicated_mode_irat_measurement_list_add_gsm_measurements(
      irat_measurement_list,
      gsm_measurements,
      &dedicated_mode_params->gsm_params.meas_params,
      in_dedicated,
      gas_id
    );
  }

#ifdef FEATURE_WCDMA
  if (rr_dedicated_mode_is_wcdma_enabled(gas_id) && wcdma_measurements != NULL)
  {
    rr_dedicated_mode_irat_measurement_list_add_wcdma_measurements(
      irat_measurement_list,
      wcdma_measurements,
      &dedicated_mode_params->utran_params.meas_params,
      wcdma_ncell_list_data_ptr->wcdma_neighbor_cell_list,
      dedicated_mode_params->gsm_params.meas_params.report_type,
      gas_id
    );
  }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
  if (rr_dedicated_mode_is_tdscdma_enabled(gas_id) && tdscdma_measurements != NULL)
  {
    rr_dedicated_mode_irat_measurement_list_add_tdscdma_measurements(
      irat_measurement_list,
      tdscdma_measurements,
      &dedicated_mode_params->utran_params.meas_params,
      wcdma_ncell_list_data_ptr,
      dedicated_mode_params->gsm_params.meas_params.report_type,
      gas_id
    );
  }
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
  if (rr_dedicated_mode_is_lte_enabled(gas_id) && lte_measurements != NULL)
  {
    rr_dedicated_mode_irat_measurement_list_add_lte_measurements(
      irat_measurement_list,
      lte_measurements,
      &dedicated_mode_params->eutran_params,
      dedicated_mode_params->gsm_params.meas_params.report_type,
      gas_id
    );
  }
#endif /* FEATURE_LTE */
}

/**
  @brief Comparison function to sort IRAT measurements according to 3GPP 45.008
         Section 8.4.7, Step 1.

  This function sorts IRAT measurements in to descending order of reported value.

  In the case of equal reporting values, entries are sorted in descending order
  of sort value.
*/
static int rr_sort_irat_measurements_descending_reported_value_and_sort_value_fn(
  const void * a,
  const void * b
)
{
  RR_NULL_CHECK_RETURN_PARAM(a, 0);
  RR_NULL_CHECK_RETURN_PARAM(b, 0);

  if (((rr_dedicated_mode_irat_measurement_t *) a)->reported_value >
      ((rr_dedicated_mode_irat_measurement_t *) b)->reported_value)
  {
    return -1;
  }
  else if (((rr_dedicated_mode_irat_measurement_t *) a)->reported_value <
           ((rr_dedicated_mode_irat_measurement_t *) b)->reported_value)
  {
    return 1;
  }
  else
  {
    /* Equal values of Multirat Reporting, so sort based on sort value */
    if (((rr_dedicated_mode_irat_measurement_t *) a)->sort_value >
        ((rr_dedicated_mode_irat_measurement_t *) b)->sort_value)
    {
      return -1;
    }
    else if (((rr_dedicated_mode_irat_measurement_t *) a)->sort_value <
             ((rr_dedicated_mode_irat_measurement_t *) b)->sort_value)
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
  @brief Comparison function to sort IRAT measurements according to 3GPP 45.008
         Section 8.4.7, Step 2.

  This function sorts IRAT measurements in to descending order of
  XXX_MULTIRAT_REPORTING value, and within equal XXX_MULTIRAT_REPORTING, by
  descending order of 'sort' value (6 bit reported value -
  XXX_REPORTING_THRESHOLD + XXX_REPORTING_OFFSET).
*/
static int rr_sort_irat_measurements_descending_multirat_reporting_and_sort_value_fn(
  const void * a,
  const void * b
)
{
  RR_NULL_CHECK_RETURN_PARAM(a, 0);
  RR_NULL_CHECK_RETURN_PARAM(b, 0);

  if (((rr_dedicated_mode_irat_measurement_t *) a)->multirat_reporting >
      ((rr_dedicated_mode_irat_measurement_t *) b)->multirat_reporting)
  {
    return -1;
  }
  else if (((rr_dedicated_mode_irat_measurement_t *) a)->multirat_reporting <
           ((rr_dedicated_mode_irat_measurement_t *) b)->multirat_reporting)
  {
    return 1;
  }
  else
  {
    /* Equal values of Multirat Reporting, so sort based on sort value */
    if (((rr_dedicated_mode_irat_measurement_t *) a)->sort_value >
        ((rr_dedicated_mode_irat_measurement_t *) b)->sort_value)
    {
      return -1;
    }
    else if (((rr_dedicated_mode_irat_measurement_t *) a)->sort_value <
             ((rr_dedicated_mode_irat_measurement_t *) b)->sort_value)
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
  @brief Comparison function to sort IRAT measurements according to 3GPP 45.008
         Section 8.4.7, Step 3.

  This function sorts IRAT measurements in to descending order of 'sort' value
  (6 bit reported value - XXX_REPORTING_THRESHOLD + XXX_REPORTING_OFFSET).
*/
static int rr_sort_irat_measurements_descending_sort_value_fn(
  const void * a,
  const void * b
)
{
  RR_NULL_CHECK_RETURN_PARAM(a, 0);
  RR_NULL_CHECK_RETURN_PARAM(b, 0);

  if (((rr_dedicated_mode_irat_measurement_t *) a)->sort_value >
      ((rr_dedicated_mode_irat_measurement_t *) b)->sort_value)
  {
    return -1;
  }
  else if (((rr_dedicated_mode_irat_measurement_t *) a)->sort_value <
           ((rr_dedicated_mode_irat_measurement_t *) b)->sort_value)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

#ifdef FEATURE_WCDMA

/**
  @brief Clears all RSSI reporting flags for WCDMA cells on the given UARFCN
         in the IRAT measurement list.

  This function is intended to support the following statement in 3GPP 45.008
  Section 8.4.7:

  "If the neighbour cell list contains a UTRAN FDD frequency for RSSI
  reporting, an RSSI report on that frequency shall be included when and only
  when a cell on that frequency is also reported (RSSI shall be reported at
  most once per frequency occurence in the neighbouring cell list).  RSSI
  measurements for frequencies contained in the neighbour cell list shall be
  reported with high priority and shall be reported before measurements on
  valid 3G cell, in case of not available positions."

  Each WCDMA cell in the IRAT measurement list contains a flag that indicates
  whether it's frequency is configured for RSSI reporting.  As soon as an
  RSSI report for a specific UARFCN is included in the measurement report,
  this function should be called to clear all RSSI reporting flags for that
  UARFCN so that it is not reported again.

  @param irat_measurement_list The measurement list to process.
  @param uarfcn The UARFCN to search for.
*/
static void rr_irat_measurement_list_clear_rssi_index_valid_flags_for_wcdma_uarfcn(
  rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list,
  uint16                                      uarfcn
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(irat_measurement_list);

  for (i = 0;
       i < irat_measurement_list->count &&
       i < ARR_SIZE(irat_measurement_list->entries);
       ++i)
  {
    if (irat_measurement_list->entries[i].rat == RR_RAT_WCDMA &&
        irat_measurement_list->entries[i].data.wcdma.uarfcn == uarfcn &&
        irat_measurement_list->entries[i].data.wcdma.rssi_index_valid)
    {
      irat_measurement_list->entries[i].data.wcdma.rssi_index_valid = FALSE;
    }
  }
}

#endif /* FEATURE_WCDMA */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
 
/**
  @brief Returns the number of entries remaining in the IRAT measurement
         whose 'reported' flag is set to FALSE, indicating that the entry has
         not yet been included in a measurement report.

  @return The number of cells that habe not yet been reported.
*/

static uint32 rr_irat_measurement_list_unreported_count_per_rat(
  rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list,
  rr_rat_e                                    rat
)
{
  uint32 i;
  uint32 unreported_count = 0;

  RR_NULL_CHECK_RETURN_PARAM(irat_measurement_list, 0);

  for (i = 0;
       i < irat_measurement_list->count &&
       i < ARR_SIZE(irat_measurement_list->entries);
       ++i)
  {
    if (irat_measurement_list->entries[i].reported == FALSE && irat_measurement_list->entries[i].rat == rat)
    {
      unreported_count++;
    }
  }

  return unreported_count;
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

/**
  @brief Add an entry to a measurement report.

  @param measurement_report Measurement report to add the entry to.
  @param entry Entry to add to the measurement report.
*/
static void rr_measurement_report_add(
  rr_measurement_report_t       * measurement_report,
  rr_measurement_report_entry_t * entry
)
{
  RR_NULL_CHECK_RETURN_VOID(measurement_report);
  RR_NULL_CHECK_RETURN_VOID(entry);

  if (measurement_report->count < RR_MEASUREMENT_REPORT_MAX_ENTRIES)
  {
    measurement_report->entries[measurement_report->count] = *entry;
    ++measurement_report->count;
  }
  else
  {
    MSG_GERAN_ERROR_0("Failed to add entry to measurement report");
  }
}

/**
  @brief Displayes the contents of a measurement report to.

  @param measurement_report The measurement report to display.
*/
static void rr_measurement_report_dump(
  rr_measurement_report_t * measurement_report
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(measurement_report);

  MSG_GERAN_HIGH_0("Measurement Report:");

  for (i = 0;
       i < measurement_report->count && i < ARR_SIZE(measurement_report->entries);
       ++i)
  {
    switch (measurement_report->entries[i].rat)
    {
      case RR_RAT_GSM:
      {
        MSG_4(
          MSG_SSID_DFLT,
          MSG_LEGACY_HIGH,
          "[%d]: GSM Cell, BCCH_FREQ_NCELL: %d, BSIC_NCELL: %d, RXLEV_NCELL: %d",
          i,
          measurement_report->entries[i].data.gsm.bcch_freq_ncell,
          measurement_report->entries[i].data.gsm.bsic_ncell,
          measurement_report->entries[i].data.gsm.rxlev_ncell
        );

        break;
      } /* RR_RAT_GSM */

#ifdef FEATURE_WCDMA
      case RR_RAT_WCDMA:
      {
        MSG_3(
          MSG_SSID_DFLT,
          MSG_LEGACY_HIGH,
          "[%d]: WCDMA Cell, Reported Index: %d, Reported Value: %d",
          i,
          measurement_report->entries[i].data.wcdma.reported_index,
          measurement_report->entries[i].data.wcdma.reported_value
        );
        break;
      } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
      case RR_RAT_UTRAN_TDD:
      {
        MSG_3(
          MSG_SSID_DFLT,
          MSG_LEGACY_HIGH,
          "[%d]: TDSCDMA Cell, Reported Index: %d, Reported Value: %d",
          i,
          measurement_report->entries[i].data.tdscdma.reported_index,
          measurement_report->entries[i].data.tdscdma.reported_value
        );
        break;
      } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
      case RR_RAT_LTE:
      {
        MSG_4(
          MSG_SSID_DFLT,
          MSG_LEGACY_HIGH,
          "[%d]: LTE Cell, Frequency Index: %d, PCID: %d, Reported Value: %d",
          i,
          measurement_report->entries[i].data.lte.frequency_index,
          measurement_report->entries[i].data.lte.pcid,
          measurement_report->entries[i].data.lte.reported_value
        );
        break;
      } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

      default:
      {
        MSG_GERAN_ERROR_2("[%d]: Unexpected RAT: %d",
                            i,
                            measurement_report->entries[i].rat);
      }
    }
  }
}

/**
  @brief Encodes a Measurement Report according to the rules defined in 3GPP
         45.008 sections 8.4.3 and 8.4.7.

  @param layer3_message Pointer to the buffer in to which the encoded
         measurement report should be stored.
  @param measurement_report Pointer to the structure containing the information
         to be encoded.
*/
static void rr_measurement_report_encode(
  uint8                   * layer3_message,
  rr_measurement_report_t * measurement_report,
  boolean invalidate_measurement_reports
)
{
  uint32 i;
  uint32 slot = 0; /* Number of neighbour cells included in the report. */
 
  RR_NULL_CHECK_RETURN_VOID(layer3_message);
  RR_NULL_CHECK_RETURN_VOID(measurement_report);
  
  memset(layer3_message, 0x00, N201_SACCH);

  layer3_message[0] = RADIO_RESSOURCES;
  layer3_message[1] = MEASUREMENT_REPORT;

  /*
  [2] < BA_USED : bit (1) >    < DTX_USED : bit (1) >                 < RXLEV_FULL_SERVING_CELL : bit (6) >
  [3] < 3G_BA_USED : bit (1) > < MEAS_VALID : bit (1) >               < RXLEV_SUB_SERVING_CELL : bit (6) >
  [4] < spare bit >            < RXQUAL_FULL_SERVING_CELL : bit (3) > < RXQUAL_SUB_SERVING_CELL : bit (3) >
  */
  layer3_message[2] = 0;
  if (measurement_report->ba_used == 1)
  {
    layer3_message[2] |= 0x80;
  }
  if (measurement_report->dtx_used == TRUE)
  {
    layer3_message[2] |= 0x40;
  }
  layer3_message[2] |= measurement_report->rxlev_full_serving_cell;

  if (measurement_report->ba_3g_used == 1)
  {
    layer3_message[3] |= 0x80;
  }
  /* NOTE that the valid bit uses reversed logic,
   * i.e. (0 = valid, 1 = invalid)
   */
  if (measurement_report->meas_valid == TRUE)
  {
    layer3_message[3] &= ~0x40;
  }
  else
  {
    /* RXQUAL averages were not completed by L1 during the report period */
    layer3_message[3] |= 0x40;
    MSG_GERAN_HIGH_2("RXQUAL incomplete (0x%02X,0x%02X): invalidate meas report",
                     measurement_report->rxqual_full_serving_cell,
                     measurement_report->rxqual_sub_serving_cell);
  }
  layer3_message[3] |= measurement_report->rxlev_sub_serving_cell;

  layer3_message[4] |= (byte) ( measurement_report->rxqual_full_serving_cell << 4);
  layer3_message[4] |= (byte) ( measurement_report->rxqual_sub_serving_cell << 1);

  if (invalidate_measurement_reports == TRUE)
  {
    /* Spec 44.018 10.5.2.20 NO-NCELL_M = 7 (1 1 1)
    no neighbour cell information available for serving cell*/
    slot = NCELL_INFO_NOT_AVAILABLE_FOR_SCELL;
    MSG_GERAN_MED_1("Setting NO-NCELL-M = %d (SI5/SI6 not yet received after HO)",slot);
  }                      
  else 
  {
    /* Now iterate over the cells in the measurement report. */
    for (i = 0;
         i < measurement_report->count &&
         i < ARR_SIZE(measurement_report->entries) &&
         slot < 6; /* Maximum of 6 entries in measurement report */
         ++i)
    {
      uint8 high_part;
      uint8 low_part;

      uint8 rxlev_ncell;
      uint8 bcch_freq_ncell;
      uint8 bsic_ncell;

      switch (measurement_report->entries[i].rat)
      {
        case RR_RAT_GSM:
        {
          bcch_freq_ncell = measurement_report->entries[i].data.gsm.bcch_freq_ncell;
          rxlev_ncell     = measurement_report->entries[i].data.gsm.rxlev_ncell;
          bsic_ncell      = measurement_report->entries[i].data.gsm.bsic_ncell;
          break;
        } /* RR_RAT_GSM */

#ifdef FEATURE_WCDMA
        case RR_RAT_WCDMA:
        {
          bcch_freq_ncell = 31;
          rxlev_ncell     = measurement_report->entries[i].data.wcdma.reported_value;
          bsic_ncell      = measurement_report->entries[i].data.wcdma.reported_index;
          break;
        } /* RR_RAT_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
        case RR_RAT_UTRAN_TDD:
        {
          bcch_freq_ncell = 31;
          rxlev_ncell     = measurement_report->entries[i].data.tdscdma.reported_value;
          bsic_ncell      = measurement_report->entries[i].data.tdscdma.reported_index;
          break;
        } /* RR_RAT_UTRAN_TDD */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
        case RR_RAT_LTE:
        {
          if (measurement_report->entries[i].data.lte.frequency_index < RR_EUTRAN_NEIGHBOUR_LIST_LENGTH)
          {
            bcch_freq_ncell = 30 - measurement_report->entries[i].data.lte.frequency_index;
            bsic_ncell      = measurement_report->entries[i].data.lte.pcid & 0x3f;
            rxlev_ncell     = ((measurement_report->entries[i].data.lte.reported_value & 0x07) << 3) |
                              ((measurement_report->entries[i].data.lte.pcid & 0x1c0) >> 6);
          }
          else
          {
            MSG_GERAN_ERROR_2("[%d]: Unexpected LTE frequency_index: %d", i, measurement_report->entries[i].data.lte.frequency_index);
            continue;
          }
          break;
        } /* RR_RAT_LTE */
#endif /* FEATURE_LTE */

        default:
        {
          MSG_GERAN_ERROR_2("[%d]: Unexpected RAT: %d", i, measurement_report->entries[i].rat);
          continue;
        }
      }

      if (slot == 0)
      {
        layer3_message[5] |= rxlev_ncell;
        layer3_message[6] = (byte) (bcch_freq_ncell << 3);
        high_part = (byte) (bsic_ncell & 0x38) >> 3;
        low_part = bsic_ncell & 0x07;
        layer3_message[6] |= high_part;
        layer3_message[7] |= (byte) (low_part << 5);
      }
      else if (slot == 1)
      {
        high_part = (byte) (rxlev_ncell & 0x3E) >> 1;
        low_part = rxlev_ncell & 0x01;
        layer3_message[7] |= high_part;
        layer3_message[8] |= (byte) (low_part << 7);
        layer3_message[8] |= (byte) (bcch_freq_ncell << 2);
        high_part = (byte) (bsic_ncell & 0x30) >> 4;
        low_part = bsic_ncell & 0x0F;
        layer3_message[8] |= high_part;
        layer3_message[9] |= (byte) (low_part << 4);
      }
      else if (slot == 2)
      {
        high_part = (byte) (rxlev_ncell & 0x3C) >> 2;
        low_part = rxlev_ncell & 0x03;
        layer3_message[9] |= high_part;
        layer3_message[10] |= (byte) (low_part << 6);
        layer3_message[10] |= (byte) (bcch_freq_ncell << 1);
        high_part = (byte) (bsic_ncell & 0x20) >> 5;
        low_part = bsic_ncell & 0x1F;
        layer3_message[10] |= high_part;
        layer3_message[11] |= (byte) (low_part << 3);
      }
      else if (slot == 3)
      {
        high_part = (byte) (rxlev_ncell & 0x38) >> 3;
        low_part = rxlev_ncell & 0x07;
        layer3_message[11] |= high_part;
        layer3_message[12] |= (byte) (low_part << 5);
        layer3_message[12] |= bcch_freq_ncell;
        layer3_message[13] |= (byte) (bsic_ncell << 2);
      }
      else if (slot == 4)
      {
        high_part = (byte) (rxlev_ncell & 0x30) >> 4;
        low_part = rxlev_ncell & 0x0F;
        layer3_message[13] |= high_part;
        layer3_message[14] |= (byte) (low_part << 4);
        high_part = (byte) (bcch_freq_ncell & 0x1E) >> 1;
        low_part = bcch_freq_ncell & 0x01;
        layer3_message[14] |= high_part;
        layer3_message[15] |= (byte) (low_part << 7);
        layer3_message[15] |= (byte) (bsic_ncell << 1);
      }
      else if (slot == 5)
      {
        high_part = (byte) (rxlev_ncell & 0x20) >> 5;
        low_part = rxlev_ncell & 0x1F;
        layer3_message[15] |= high_part;
        layer3_message[16] |= (byte) (low_part << 3);
        high_part = ((byte)(bcch_freq_ncell & 0x1C) >> 2);
        low_part = bcch_freq_ncell & 0x03;
        layer3_message[16] |= high_part;
        layer3_message[17] |= (byte) (low_part << 6);
        layer3_message[17] |= bsic_ncell;
      }
      ++slot;
    }
  }

  /* Finally, update the NO-NCELL-M value to indicate how many cells are
  present in the measurement report. */
  layer3_message[4] |= ((slot & 0x04) >> 2);
  layer3_message[5] |= ((slot & 0x03) << 6);
}

static void rr_measurement_report_add_irat_ncells_before_gsm_cells(
  rr_measurement_report_t                   * measurement_report,
  rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list,
  uint8                                       fdd_multirat_reporting,
  uint8                                       tdd_multirat_reporting,
  uint8                                       eutran_multirat_reporting
)
{
  rr_measurement_report_entry_t   measurement_report_entry;

  RR_NULL_CHECK_RETURN_VOID(measurement_report);
  RR_NULL_CHECK_RETURN_VOID(irat_measurement_list);

  qsort(
    &irat_measurement_list->entries[0],
    irat_measurement_list->count,
    sizeof(irat_measurement_list->entries[0]),
    rr_sort_irat_measurements_descending_reported_value_and_sort_value_fn
  );

  /*
  If there are more valid non-GSM cells than can be reported, the MS shall
  select the cells for each supported RAT/mode for which XXX_MULTIRAT_REPORTING
  is non-zero according to the following procedure:

  1. For each RAT/mode the best valid cell of each RAT/mode is included in the
     report.  The best cell is the cell with the highest reported value.
  */

#ifdef FEATURE_WCDMA
  if (fdd_multirat_reporting > 0)
  {
    uint32 i;

    /* Search for the strongest WCDMA cell and add it to the report. */
    for (i = 0;
         i < irat_measurement_list->count &&
         i < ARR_SIZE(irat_measurement_list->entries);
         ++i)
    {
      if (irat_measurement_list->entries[i].reported == FALSE &&
          irat_measurement_list->entries[i].rat == RR_RAT_WCDMA)
      {
        /* Mark the cell as reported, so it is only reported once. */
        irat_measurement_list->entries[i].reported = TRUE;
        --fdd_multirat_reporting;

        /* WCDMA cells have a special case for RSSI reporting. */
        if (irat_measurement_list->entries[i].data.wcdma.rssi_index_valid)
        {
          measurement_report_entry.rat = RR_RAT_WCDMA;
          measurement_report_entry.data.wcdma.reported_value = irat_measurement_list->entries[i].data.wcdma.reported_rssi;
          measurement_report_entry.data.wcdma.reported_index = irat_measurement_list->entries[i].data.wcdma.rssi_index;

          rr_measurement_report_add(
            measurement_report,
            &measurement_report_entry
          );

          /* Clear all RSSI reporting flags for this UARFCN. */
          rr_irat_measurement_list_clear_rssi_index_valid_flags_for_wcdma_uarfcn(
            irat_measurement_list,
            irat_measurement_list->entries[i].data.wcdma.uarfcn
          );
        }

        measurement_report_entry.rat = RR_RAT_WCDMA;
        measurement_report_entry.data.wcdma.reported_value = irat_measurement_list->entries[i].reported_value;
        measurement_report_entry.data.wcdma.reported_index = irat_measurement_list->entries[i].data.wcdma.cell_index;

        rr_measurement_report_add(
          measurement_report,
          &measurement_report_entry
        );
        break;
      }
    }
  }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
  if (tdd_multirat_reporting > 0)
  {
    uint32 i;

    /* Search for the strongest TDSCDMA cell and add it to the report. */
    for (i = 0;
         i < irat_measurement_list->count &&
         i < ARR_SIZE(irat_measurement_list->entries);
         ++i)
    {
      if (irat_measurement_list->entries[i].reported == FALSE &&
          irat_measurement_list->entries[i].rat == RR_RAT_UTRAN_TDD)
      {
        /* Mark the cell as reported, so it is only reported once. */
        irat_measurement_list->entries[i].reported = TRUE;
        --tdd_multirat_reporting;

        measurement_report_entry.rat = RR_RAT_UTRAN_TDD;
        measurement_report_entry.data.tdscdma.reported_value = irat_measurement_list->entries[i].reported_value;
        measurement_report_entry.data.tdscdma.reported_index = irat_measurement_list->entries[i].data.tdscdma.cell_index;

        rr_measurement_report_add(
          measurement_report,
          &measurement_report_entry
        );
        break;
      }
    }
  }
#endif /* FEATURE_GSM_TDS */


#ifdef FEATURE_LTE
  if (eutran_multirat_reporting > 0)
  {
    uint32 i;

    /* Search for the strongest LTE cell and add it to the report. */
    for (i = 0;
         i < irat_measurement_list->count &&
         i < ARR_SIZE(irat_measurement_list->entries);
         ++i)
    {
      if (irat_measurement_list->entries[i].reported == FALSE &&
          irat_measurement_list->entries[i].rat == RR_RAT_LTE)
      {
        /* Mark the cell as reported, so it is only reported once. */
        irat_measurement_list->entries[i].reported = TRUE;
        --eutran_multirat_reporting;

        measurement_report_entry.rat = RR_RAT_LTE;
        measurement_report_entry.data.lte.reported_value = irat_measurement_list->entries[i].reported_value;
        measurement_report_entry.data.lte.frequency_index = irat_measurement_list->entries[i].data.lte.frequency_index;
        measurement_report_entry.data.lte.pcid = irat_measurement_list->entries[i].data.lte.pcid;

        rr_measurement_report_add(
          measurement_report,
          &measurement_report_entry
        );
        break;
      }
    }
  }
#endif /* FEATURE_LTE */

  /*
  2. If there are further positions and cells available, the MS shall
     additionally include in the report the next best valid cell on each RAT/
     mode for which XXX_MULTIRAT_REPORTING cells have not yet been included.

     In case this would result in including more cells than there are remaining
     spaces in the measurement report, priority is given to cells from RATs/
     modes with higher XXX_MULTIRAT_REPORTING value.  In case of equal
     XXX_MULTIRAT_REPORTING values, the cell for which (6 bit reported value –
     XXX_REPORTING_THRESHOLD + XXX_REPORTING_OFFSET) is highest is reported.

     6 bit mapping for reported values shall be used in the calculations even
     if 3 bit reporting is used.  In this case, instead of
     E-UTRAN_FDD_REPORTING_THRESHOLD or E-UTRAN_TDD_REPORTING_THRESHOLD the
     mobile station shall use the 6 bit mapped value from
     E-UTRAN_FDD_MEASUREMENT_REPORT_OFFSET or
     E-UTRAN_TDD_MEASUREMENT_REPORT_OFFSET respectively, using the mapping
     table defined in subclause 8.1.5.4 for E-UTRAN FDD or subclause 8.1.5.5
     for E-UTRAN TDD.

  3. Step 2 is repeated until either all positions are used or no more cells
     are to be reported (because, for each RAT/mode, either
     XXX_MULTIRAT_REPORTING cells have been reported, or there are no more
     valid cells to report).
  */

  qsort(
    &irat_measurement_list->entries[0],
    irat_measurement_list->count,
    sizeof(irat_measurement_list->entries[0]),
    rr_sort_irat_measurements_descending_multirat_reporting_and_sort_value_fn
  );

  while (measurement_report->count < RR_MEASUREMENT_REPORT_MAX_ENTRIES)
  {
    uint32  i;
    boolean w_reporting_possible = FALSE;
    boolean t_reporting_possible = FALSE;
    boolean l_reporting_possible = FALSE;

#ifdef FEATURE_WCDMA
    w_reporting_possible = (rr_irat_measurement_list_unreported_count_per_rat(irat_measurement_list, RR_RAT_WCDMA) > 0) && (fdd_multirat_reporting > 0);
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
    t_reporting_possible = (rr_irat_measurement_list_unreported_count_per_rat(irat_measurement_list, RR_RAT_UTRAN_TDD) > 0) && (tdd_multirat_reporting > 0);
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
    l_reporting_possible = (rr_irat_measurement_list_unreported_count_per_rat(irat_measurement_list, RR_RAT_LTE) > 0) && (eutran_multirat_reporting > 0);
#endif /* FEATURE_LTE */

    if (!w_reporting_possible && !t_reporting_possible && !l_reporting_possible)
    {
      break;
    }

    /* IRAT measurement list is already sorted in to the correct priority
    order, so just need to add the best unreported cell to the measurement
    report. */

    for (i = 0;
         i < irat_measurement_list->count &&
         i < ARR_SIZE(irat_measurement_list->entries);
         ++i)
    {
      if (irat_measurement_list->entries[i].reported == FALSE)
      {
#ifdef FEATURE_WCDMA
        if (irat_measurement_list->entries[i].rat == RR_RAT_WCDMA && fdd_multirat_reporting > 0)
        {
          irat_measurement_list->entries[i].reported = TRUE;
          --fdd_multirat_reporting;

          /* WCDMA cells have a special case for RSSI reporting. */
          if (irat_measurement_list->entries[i].data.wcdma.rssi_index_valid)
          {
            measurement_report_entry.rat = RR_RAT_WCDMA;
            measurement_report_entry.data.wcdma.reported_value = irat_measurement_list->entries[i].data.wcdma.reported_rssi;
            measurement_report_entry.data.wcdma.reported_index = irat_measurement_list->entries[i].data.wcdma.rssi_index;

            rr_measurement_report_add(
              measurement_report,
              &measurement_report_entry
            );

            /* Clear all RSSI reporting flags for this UARFCN. */
            rr_irat_measurement_list_clear_rssi_index_valid_flags_for_wcdma_uarfcn(
              irat_measurement_list,
              irat_measurement_list->entries[i].data.wcdma.uarfcn
            );
          }

          measurement_report_entry.rat = RR_RAT_WCDMA;
          measurement_report_entry.data.wcdma.reported_value = irat_measurement_list->entries[i].reported_value;
          measurement_report_entry.data.wcdma.reported_index = irat_measurement_list->entries[i].data.wcdma.cell_index;

          rr_measurement_report_add(
            measurement_report,
            &measurement_report_entry
          );
          break;
        }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
        if (irat_measurement_list->entries[i].rat == RR_RAT_UTRAN_TDD && tdd_multirat_reporting > 0)
        {
          irat_measurement_list->entries[i].reported = TRUE;
          --tdd_multirat_reporting;

          measurement_report_entry.rat = RR_RAT_UTRAN_TDD;
          measurement_report_entry.data.tdscdma.reported_value = irat_measurement_list->entries[i].reported_value;
          measurement_report_entry.data.tdscdma.reported_index = irat_measurement_list->entries[i].data.tdscdma.cell_index;

          rr_measurement_report_add(
            measurement_report,
            &measurement_report_entry
          );
          break;
        }
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
        if (irat_measurement_list->entries[i].rat == RR_RAT_LTE && eutran_multirat_reporting > 0)
        {
          irat_measurement_list->entries[i].reported = TRUE;
          --eutran_multirat_reporting;

          measurement_report_entry.rat = RR_RAT_LTE;
          measurement_report_entry.data.lte.reported_value = irat_measurement_list->entries[i].reported_value;
          measurement_report_entry.data.lte.frequency_index = irat_measurement_list->entries[i].data.lte.frequency_index;
          measurement_report_entry.data.lte.pcid = irat_measurement_list->entries[i].data.lte.pcid;

          rr_measurement_report_add(
            measurement_report,
            &measurement_report_entry
          );
          break;
        }
#endif /* FEATURE_LTE */
      }
    }
  }
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

/**
  @brief Add GSM neighbour cell measurements to a measurement report.

  @param gsm_measurements The GSM measurements received from L1.
  @param in_dedicated TRUE if RR is in dedicated mode, FALSE if not.
*/
static void rr_measurement_report_add_gsm_ncells(
  rr_measurement_report_t   * measurement_report,
  surrounding_measurement_T * gsm_measurements,
  boolean                     in_dedicated,
  const                       gas_id_t gas_id
)
{
  uint32              i;
  uint32              num_slots;
  gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_RETURN_VOID(measurement_report);
  RR_NULL_CHECK_RETURN_VOID(gsm_measurements);

  num_slots = RR_MEASUREMENT_REPORT_MAX_ENTRIES - (MIN(measurement_report->count, RR_MEASUREMENT_REPORT_MAX_ENTRIES));

  /* Now GSM cells must be added to the measurement report.  The number of
  cells to be added is (at most) the number of remaining positions in the
  measurement report. */

  (void) rr_store_valid_cells_into_surround_meas(gsm_measurements, scell);

  MSG_GERAN_HIGH_2_G("Adding up to %d GSM cells (%d measurements received)", num_slots, gsm_measurements->no_of_entries);

  for (i = 0;
       i < gsm_measurements->no_of_entries && i < num_slots;
       ++i)
  {
    boolean found = FALSE;
    uint8 bcch_freq_ncell = rr_find_frequency_index(
      gsm_measurements->measurement[i].BCCH_ARFCN,
      &found,
      in_dedicated,
      gas_id
    );

    if (found)
    {
      measurement_report->entries[measurement_report->count].rat = RR_RAT_GSM;
      measurement_report->entries[measurement_report->count].data.gsm.bcch_freq_ncell = bcch_freq_ncell;
      measurement_report->entries[measurement_report->count].data.gsm.bsic_ncell  = gsm_measurements->measurement[i].BSIC;
      measurement_report->entries[measurement_report->count].data.gsm.rxlev_ncell = MIN(gsm_measurements->measurement[i].RXLEV_average, 63);
      ++measurement_report->count;
    }
  }
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

static void rr_measurement_report_add_irat_ncells_after_gsm_cells(
  rr_measurement_report_t                   * measurement_report,
  rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list,
  uint8                                       fdd_multirat_reporting,
  uint8                                       tdd_multirat_reporting,
  uint8                                       eutran_multirat_reporting
)
{
  rr_measurement_report_entry_t   measurement_report_entry;
  uint32                          i;

  RR_NULL_CHECK_RETURN_VOID(measurement_report);
  RR_NULL_CHECK_RETURN_VOID(irat_measurement_list);

  qsort(
    &irat_measurement_list->entries[0],
    irat_measurement_list->count,
    sizeof(irat_measurement_list->entries[0]),
    rr_sort_irat_measurements_descending_sort_value_fn
  );

  /* Just iterate over the IRAT measurement list, adding cells to the
  measurement report until it is full. */
  for (i = 0;
       i < irat_measurement_list->count &&
       i < ARR_SIZE(irat_measurement_list->entries) &&
       measurement_report->count < ARR_SIZE(measurement_report->entries);
       ++i)
  {
    if (irat_measurement_list->entries[i].reported == FALSE)
    {
 
#ifdef FEATURE_WCDMA
      if (irat_measurement_list->entries[i].rat == RR_RAT_WCDMA && fdd_multirat_reporting > 0)
      {
        irat_measurement_list->entries[i].reported = TRUE;

        /* WCDMA cells have a special case for RSSI reporting. */
        if (irat_measurement_list->entries[i].data.wcdma.rssi_index_valid)
        {
          measurement_report_entry.rat = RR_RAT_WCDMA;
          measurement_report_entry.data.wcdma.reported_value = irat_measurement_list->entries[i].data.wcdma.reported_rssi;
          measurement_report_entry.data.wcdma.reported_index = irat_measurement_list->entries[i].data.wcdma.rssi_index;

          rr_measurement_report_add(
            measurement_report,
            &measurement_report_entry
          );

          /* Clear all RSSI reporting flags for this UARFCN. */
          rr_irat_measurement_list_clear_rssi_index_valid_flags_for_wcdma_uarfcn(
            irat_measurement_list,
            irat_measurement_list->entries[i].data.wcdma.uarfcn
          );
        }

        measurement_report_entry.rat = RR_RAT_WCDMA;
        measurement_report_entry.data.wcdma.reported_value = irat_measurement_list->entries[i].reported_value;
        measurement_report_entry.data.wcdma.reported_index = irat_measurement_list->entries[i].data.wcdma.cell_index;

        rr_measurement_report_add(
          measurement_report,
          &measurement_report_entry
        );
      }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
      if (irat_measurement_list->entries[i].rat == RR_RAT_UTRAN_TDD && tdd_multirat_reporting > 0)
      {
        irat_measurement_list->entries[i].reported = TRUE;

        measurement_report_entry.rat = RR_RAT_UTRAN_TDD;
        measurement_report_entry.data.tdscdma.reported_value = irat_measurement_list->entries[i].reported_value;
        measurement_report_entry.data.tdscdma.reported_index = irat_measurement_list->entries[i].data.tdscdma.cell_index;

        rr_measurement_report_add(
          measurement_report,
          &measurement_report_entry
        );
      }
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
      if (irat_measurement_list->entries[i].rat == RR_RAT_LTE && eutran_multirat_reporting > 0)
      {
        irat_measurement_list->entries[i].reported = TRUE;

        measurement_report_entry.rat = RR_RAT_LTE;
        measurement_report_entry.data.lte.reported_value = irat_measurement_list->entries[i].reported_value;
        measurement_report_entry.data.lte.frequency_index = irat_measurement_list->entries[i].data.lte.frequency_index;
        measurement_report_entry.data.lte.pcid = irat_measurement_list->entries[i].data.lte.pcid;

        rr_measurement_report_add(
          measurement_report,
          &measurement_report_entry
        );
      }
#endif /* FEATURE_LTE */
    }
  }
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

/**
  @brief Encodes a MEASUREMENT REPORT message based on measurements received
         from GL1 according to the rules defined in 3GPP 45.008 section 8.4.7.

  A multi-RAT MS shall report the number of best valid cells, in each supported
  other radio access technology/mode in the neighbour cell list or on
  frequencies contained in the E -UTRAN Neighbour Cell list, according to the
  value of the parameters XXX_MULTIRAT_REPORTING (XXX indicates radio access
  technology/mode).

  For UTRAN FDD, only cells with a reported value (CPICH Ec/No or CPICH RSCP)
  equal or higher than FDD_REPORTING_THRESHOLD and a non-reported value (from
  CPICH Ec/No and CPICH RSCP) equal or higher than FDD_REPORTING_THRESHOLD_2,
  shall be reported.

  For UTRAN TDD, only cells with a reported value (P-CCPCH RSCP) equal or
  higher than TDD_REPORTING_THRESHOLD shall be reported.

  For E-UTRAN FDD or E-UTRAN TDD:

  When 6-bit measurement reporting is used only cells with a reported value
  (RSRP or RSRQ) equal or higher than E-UTRAN_FDD_REPORTING_THRESHOLD or
  E-UTRAN_TDD_REPORTING_THRESHOLD and with a non-reported value (from RSRP
  and RSRQ) equal or higher than E-UTRAN_FDD_REPORTING_THRESHOLD_2 or
  E-UTRAN_TDD_REPORTING_THRESHOLD_2, respectively, shall be reported.

  When 3-bit measurement reporting is used only cells with a measured value
  (RSRP and RSRQ) equal or higher than E -UTRAN_FDD_MEASUREMENT_REPORT_OFFSET
  or E-UTRAN_TDD_MEASUREMENT_REPORT_OFFSET and with a non-reported value (RSRP
  or RSRQ) equal or higher than E-UTRAN_FDD_REPORTING_THRESHOLD_2 or
  E-UTRAN_TDD_REPORTING_THRESHOLD_2, respectively, shall be reported.
*/
static void rr_encode_measurement_report(
  uint8                     layer3_message[N201_SACCH],
  boolean                   in_dedicated,
  boolean                   meas_valid,
  rr_dedicated_mode_params_t * dedicated_mode_params,
  const gas_id_t gas_id
)
{
  rr_measurement_report_t                    measurement_report;
  surrounding_measurement_T                  gsm_measurements;

#ifdef FEATURE_WCDMA
  surr_wcdma_ded_measurements_T            * wcdma_measurements = NULL;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  rr_ded_irat_measurements_t               * tdscdma_measurements = NULL;
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
  rr_ded_meas_reporting_lte_measurements_t * lte_measurements = NULL;
#endif /* FEATURE_LTE */
  uint8                                      fdd_multirat_reporting = 0;
  uint8                                      tdd_multirat_reporting = 0;
  uint8                                      eutran_multirat_reporting = 0;
// Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);

  boolean invalidate_measurement_reports; 

  RR_NULL_CHECK_RETURN_VOID(layer3_message);
  RR_NULL_CHECK_RETURN_VOID(dedicated_mode_params);

  memset(layer3_message, 0, N201_SACCH);
  memset(&measurement_report, 0, sizeof(measurement_report));

  measurement_report.meas_valid = meas_valid;
  /* copying measurement reports status to local variable */
  invalidate_measurement_reports = rr_ded_meas_reporting_data_ptr->invalidate_measurement_reports;
  

  if (in_dedicated)
  {
    gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

    /********************************************************************************************/
    /* always use the value in the SI5_SI5bis BA list for measurement reports. This corresponds */
    /* to the BA list being used by L1 at the time the measurement report is generated          */
    /********************************************************************************************/

    rr_ba_lists_t * rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

    if (invalidate_measurement_reports == TRUE)
    {
      MSG_GERAN_MED_0_G("Using BA_IND, 3G_BA_IND from previous cell (SI5/SI6 not yet received after HO)");
    }

    measurement_report.ba_used                  = rr_ba_lists_ptr->SI5_SI5bis_measurement_report_BA_list.BA_IND;
    measurement_report.dtx_used                 = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.DTX_used;
    measurement_report.rxlev_full_serving_cell  = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXLEV_FULL_average;
    if (scell->gsm.wcdma_ded_neighbor_list.valid_data)
    {
      measurement_report.ba_3g_used             = scell->gsm.wcdma_ded_neighbor_list.common_params.BA_IND_3G;
    }
    else
    {
      measurement_report.ba_3g_used             = scell->gsm.utran_neighbor_list.common_params.BA_IND_3G;
    }
    measurement_report.rxlev_sub_serving_cell   = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXLEV_SUB_average;
    measurement_report.rxqual_full_serving_cell = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXQUAL_FULL_average;
    measurement_report.rxqual_sub_serving_cell  = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXQUAL_SUB_average;

    gsm_measurements                            = *rr_get_surrounding_measurement_ptr(gas_id);

#ifdef FEATURE_GSM_BAND_AVOIDANCE
    rr_band_avoid_modify_gsm_meas_rpt(&gsm_measurements,
                                      scell->gsm.BCCH_ARFCN,
                                      &measurement_report.rxlev_full_serving_cell, 
                                      &measurement_report.rxlev_sub_serving_cell, 
                                      &measurement_report.rxqual_full_serving_cell,
                                      &measurement_report.rxqual_sub_serving_cell,
                                      NULL, /* MEAN BEP */
                                      NULL, /* CV BEP */
                                      NULL, /* NBR RCVD Blocks */
                                       gas_id);
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
  }
  else
  {
    usat_system_information_database_T * usat_system_information_database_ptr = rr_get_usat_system_information_database_ptr(gas_id);

    measurement_report.ba_used                  = usat_system_information_database_ptr->SIbis_measurement_report_BA_list.BA_IND;
    measurement_report.dtx_used                 = usat_system_information_database_ptr->cell_options.DTX;
    measurement_report.rxlev_full_serving_cell  = usat_system_information_database_ptr->reselection_info.RXLEV_average;
    measurement_report.ba_3g_used               = 0;
    measurement_report.rxlev_sub_serving_cell   = 0;
    measurement_report.rxqual_full_serving_cell = 0;
    measurement_report.rxqual_sub_serving_cell  = 0;

    gsm_measurements                            = rr_ded_meas_reporting_data_ptr->usat_surrounding_measurement;
  }
 
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  rr_utran_meas_params_dump(&dedicated_mode_params->utran_params.meas_params);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
  rr_eutran_meas_params_dump(&dedicated_mode_params->eutran_params.meas_params);
#endif /* FEATURE_LTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  /* WCDMA/TDSCDMA cells can only be included in a Measurement Report message if the
  GSM neighbour cell list has less than 32 entries (3GPP 44.018 Section
  10.5.2.20 - Measurement Results: "If no more than 31 (GSM) ARFCN frequencies
  are included in the BA (list), the index BCCH -FREQ-NCELL 31 indicates
  report(s) on 3G cells."

  If no more than (31 - NUM_E-UTRAN_FREQUENCIES) GSM ARFCN frequencies are
  included in the BA (list), the BCCH-FREQ-NCELL indices from (31 -
  NUM_E-UTRAN_FREQUENCIES) to 30 (inclusive) indicate report(s) on E-UTRAN
  cells.

  NUM_E-UTRAN_FREQUENCIES is defined as the number of separate E-UTRAN
  frequencies in the E-UTRAN Neighbour Cell list.
  */
  if (rr_dedicated_mode_ba_list_length(gas_id) <= 31)
  {
#ifdef FEATURE_WCDMA
    wcdma_measurements = rr_get_wcdma_ded_meas_ptr(gas_id);
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
    tdscdma_measurements = rr_get_tds_ded_meas_ptr(gas_id);
#endif /* FEATURE_GSM_TDS */
  }

#ifdef FEATURE_LTE
  if (rr_dedicated_mode_ba_list_length(gas_id) <= (31 - rr_dedicated_mode_eutran_neighbour_list_length(gas_id)))
  {
    lte_measurements = &rr_ded_meas_reporting_data_ptr->lte_measurements;
  }
#endif /* FEATURE_LTE */
 
  rr_dedicated_mode_irat_measurement_list_create(
    rr_ded_meas_reporting_data_ptr->irat_measurement_list,
    NULL, /* GSM measurments not added to list for normal measurement reporting. */
#ifdef FEATURE_WCDMA
    wcdma_measurements,
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
    tdscdma_measurements,
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
    lte_measurements,
#endif /* FEATURE_LTE */
    dedicated_mode_params,
    in_dedicated,
    gas_id
  );

  rr_dedicated_mode_irat_measurement_list_dump(
    rr_ded_meas_reporting_data_ptr->irat_measurement_list
  );

  /* This function adds IRAT cells according to steps 1 to 3 of 3GPP 45.008
  section 8.4.7. */
#ifdef FEATURE_WCDMA
  fdd_multirat_reporting = dedicated_mode_params->utran_params.meas_params.fdd_multirat_reporting;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  tdd_multirat_reporting = dedicated_mode_params->utran_params.meas_params.tdd_multirat_reporting;
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
  eutran_multirat_reporting = dedicated_mode_params->eutran_params.meas_params.multirat_reporting;
#endif /* FEATURE_LTE */

  rr_measurement_report_add_irat_ncells_before_gsm_cells(
    &measurement_report,
    rr_ded_meas_reporting_data_ptr->irat_measurement_list,
    fdd_multirat_reporting,
    tdd_multirat_reporting,
    eutran_multirat_reporting
  );
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  /* The remaining positions in the measurement report shall be used for
  reporting of GSM cells as defined in 3GPP 45.008 section 8.4.3. */
  rr_measurement_report_add_gsm_ncells(
    &measurement_report,
    &gsm_measurements,
    in_dedicated,
    gas_id
  );

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  /* If there are still remaining positions, these shall be used to report the
  next best  valid cells in supported other radio access technologies for which
  XXX_MULTIRAT_REPORTING is non-zero. The best cell is the cell with the
  highest quantity of (6 bit reported value – XXX_REPORTING_THRESHOLD +
  XXX_REPORTING_OFFSET). */
  rr_measurement_report_add_irat_ncells_after_gsm_cells(
    &measurement_report,
    rr_ded_meas_reporting_data_ptr->irat_measurement_list,
    fdd_multirat_reporting,
    tdd_multirat_reporting,
    eutran_multirat_reporting
  );
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  /* Print out the contents of the measurement report for debug purposes. */
  rr_measurement_report_dump(&measurement_report);

  /* And finally, pack the measurement report in to the layer 3 message. */
  rr_measurement_report_encode(
    layer3_message,
    &measurement_report,
    invalidate_measurement_reports
  );

  /* Copy over the meas results IE to rr_last_meas_results for use by USAT */
  if (in_dedicated == TRUE)
  {
    /* skip the first two octets, which are the RR protocol and message type */
    memscpy(
      &rr_ded_meas_reporting_data_ptr->rr_last_meas_results[0],
      sizeof(rr_ded_meas_reporting_data_ptr->rr_last_meas_results),
      layer3_message+2,
      RR_LAST_MEAS_BUFFER_SIZE
    );	
  }
}

/**
  @ingroup dedicated_mode_irat_measurement

  @brief Return the cell_index of the IRAT measurement.

  @param ncell The cell to get the cell_index of.
  @param cell_index Variable in which to store the cell_index.
  @return TRUE if the cell's cell index is valid, FALSE if not.
*/
static boolean rr_dedicated_mode_irat_measurement_cell_index(
  rr_dedicated_mode_irat_measurement_t * ncell,
  boolean                              * cell_index
)
{
  RR_NULL_CHECK_RETURN_PARAM(ncell, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(cell_index, FALSE);

  switch (ncell->rat)
  {
    case RR_RAT_GSM:
    {
      *cell_index = ncell->data.gsm.cell_index;
      return TRUE;
    }

#ifdef FEATURE_WCDMA
    case RR_RAT_WCDMA:
    {
      *cell_index = ncell->data.wcdma.cell_index;
      return TRUE;
    }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case RR_RAT_UTRAN_TDD:
    {
      *cell_index = ncell->data.tdscdma.cell_index;
      return TRUE;
    }
#endif /* FEATURE_GSM_TDS */

    default:
    {
      MSG_GERAN_ERROR_1("Unexpected RAT: %d", ncell->rat);
      return FALSE;
    }
  }
}

/**
  @defgroup enhanced_measurement_report Enhanced Measurement Report
*/

/**
  @ingroup enhanced_measurement_report

  @brief Prints an Enhanced Measurement Report message to diag.

  @param emr The Enhanced Measurement Report to print.
*/
static void rr_enhanced_measurement_report_dump(
  rr_enhanced_measurement_report_t * emr
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(emr);

  MSG_GERAN_MED_0("Enhanced Measurement Report:");
  MSG_GERAN_MED_1("  BA_USED:    %d", emr->ba_used);
  MSG_GERAN_MED_1("  3G_BA_USED: %d", emr->ba_3g_used);
  MSG_GERAN_MED_1("  BSIC_Seen:  %d", emr->bsic_seen);
  MSG_GERAN_MED_1("  SCALE:      %d", emr->scale);
  if (emr->serving_cell_data_valid)
  {
    MSG_GERAN_MED_0("  Serving Cell Data:");
    MSG_GERAN_MED_1("    DTX_USED:        %d", emr->serving_cell_data.dtx_used);
    MSG_GERAN_MED_1("    RXLEV_VAL:       %d", emr->serving_cell_data.rxlev_val);
    MSG_GERAN_MED_1("    RX_QUAL_FULL:    %d", emr->serving_cell_data.rx_qual_full);
    MSG_GERAN_MED_1("    MEAN_BEP:        %d", emr->serving_cell_data.mean_bep);
    MSG_GERAN_MED_1("    CV_BEP:          %d", emr->serving_cell_data.cv_bep);
    MSG_GERAN_MED_1("    NBR_RCVD_BLOCKS: %d", emr->serving_cell_data.nbr_rcvd_blocks);
  }
  else
  {
    MSG_GERAN_MED_0("  Serving Cell Data: Invalid");
  }

  MSG_GERAN_MED_1("  Number of 2G / 3G cells: %d", emr->ncell_count);

  for (i = 0; i <= emr->highest_bitmap_index && i < ARR_SIZE(emr->ncells); ++i)
  {
    if (emr->ncells[i] != NULL)
    {
      MSG_GERAN_MED_2("    [%02d]: REPORTING_QUANTITY: %d", i, emr->ncells[i]->reported_value);
    }
  }

  MSG_GERAN_MED_1("  Number of LTE cells: %d", emr->lte_cell_count);

#ifdef FEATURE_LTE
  for (i = 0; i < emr->lte_cell_count && i < ARR_SIZE(emr->lte_cells); ++i)
  {
    if (emr->lte_cells[i] != NULL)
    {
      MSG_4(MSG_SSID_DFLT,
            MSG_LEGACY_MED,
            "    [%02d]: EARFCN: %d, PCID: %d, REPORTING_QUANTITY: %d",
            i,
            emr->lte_cells[i]->data.lte.earfcn,
            emr->lte_cells[i]->data.lte.pcid,
            emr->lte_cells[i]->reported_value);
    }
  }
#endif /* FEATURE_LTE */
}


/**
  @ingroup enhanced_measurement_report

  @brief Encodes an Enhanced Measurement Report message to be sent OTA.

  @param layer3_message The buffer in to which the message should be encoded.
  @param emr The message to encode.
*/
static void rr_enhanced_measurement_report_encode(
  uint8                              layer3_message[N201_SACCH_BTER],
  rr_enhanced_measurement_report_t * emr
)
{
  gs_bit_stream_T bitstream;

  RR_NULL_CHECK_RETURN_VOID(layer3_message);
  RR_NULL_CHECK_RETURN_VOID(emr);

  gs_bit_stream_initialize(&bitstream, layer3_message, N201_SACCH_BTER);

  /* Zero out the complete bitstream and ensure that it's pad pattern is set
  to 0x2b. */
  bitstream.pad_pattern = 0x2b;
  gs_bit_stream_fill(&bitstream, N201_SACCH_BTER, 0);

  gs_bit_stream_insert_field(&bitstream, 1, emr->rr_short_pd);
  gs_bit_stream_insert_field(&bitstream, 5, emr->message_type);
  gs_bit_stream_insert_field(&bitstream, 2, emr->short_layer_2_header);
  gs_bit_stream_insert_field(&bitstream, 1, emr->ba_used);
  gs_bit_stream_insert_field(&bitstream, 1, emr->ba_3g_used);
  gs_bit_stream_insert_field(&bitstream, 1, emr->bsic_seen);
  gs_bit_stream_insert_field(&bitstream, 1, emr->scale);

  if (emr->serving_cell_data_valid)
  {
    gs_bit_stream_insert_field(&bitstream, 1, 1);
    gs_bit_stream_insert_field(&bitstream, 1, emr->serving_cell_data.dtx_used);
    gs_bit_stream_insert_field(&bitstream, 6, emr->serving_cell_data.rxlev_val);
    gs_bit_stream_insert_field(&bitstream, 3, emr->serving_cell_data.rx_qual_full);
    gs_bit_stream_insert_field(&bitstream, 5, emr->serving_cell_data.mean_bep);
    gs_bit_stream_insert_field(&bitstream, 3, emr->serving_cell_data.cv_bep);
    gs_bit_stream_insert_field(&bitstream, 5, emr->serving_cell_data.nbr_rcvd_blocks);
  }
  else
  {
    gs_bit_stream_insert_field(&bitstream, 1, 0);
  }

  if (emr->invalid_bsic_information_count > 0)
  {
    uint32 i;

    for (i = 0; i < emr->invalid_bsic_information_count && i < ARR_SIZE(emr->invalid_bsic_information); ++i)
    {
      gs_bit_stream_insert_field(&bitstream, 1, 1);
      gs_bit_stream_insert_field(&bitstream, 5, emr->invalid_bsic_information[i]->data.gsm.cell_index);
      gs_bit_stream_insert_field(&bitstream, 6, emr->invalid_bsic_information[i]->data.gsm.bsic);
      gs_bit_stream_insert_field(&bitstream, 6, emr->invalid_bsic_information[i]->reported_value);
    }
  }

  gs_bit_stream_insert_field(&bitstream, 1, 0); // Repeated Invalid BSIC Information not present

  /* If both LTE cells and 2G/3G cells are present, use the Rel8 encoder. */
#ifdef FEATURE_LTE
  if (emr->lte_cell_count > 0)
  {
    uint32 i;

    gs_bit_stream_insert_field(&bitstream, 1, 0); // REPORTING_QUANTITY not present

    gs_bit_stream_insert_HL(&bitstream, TRUE); /* Additions in Rel-8 */

    if (emr->ncell_count > 0)
    {
      /* BITMAP_LENGTH */
      gs_bit_stream_insert_field(&bitstream, 7, emr->highest_bitmap_index);

      /* REPORTING_QUANTITY */
      for (i = 0; i <= emr->highest_bitmap_index; ++i)
      {
        if (emr->ncells[i] != NULL)
        {
          gs_bit_stream_insert_field(&bitstream, 1, 1);
          gs_bit_stream_insert_field(&bitstream, 6, emr->ncells[i]->reported_value);
        }
        else
        {
          gs_bit_stream_insert_field(&bitstream, 1, 0);
        }
      }
    }
    else
    {
      /* BITMAP_LENGTH */
      gs_bit_stream_insert_field(&bitstream, 7, 0);

      /* Empty Reporting Quantity */
      gs_bit_stream_insert_field(&bitstream, 1, 0);
    }

    /* E-UTRAN Measurement Report */
    gs_bit_stream_insert_field(&bitstream, 1, 1);
    gs_bit_stream_insert_field(&bitstream, 2, emr->lte_cell_count - 1);
    for (i = 0; i < emr->lte_cell_count; ++i)
    {
      if (emr->lte_cells[i] != NULL)
      {
        gs_bit_stream_insert_field(&bitstream, 3, emr->lte_cells[i]->data.lte.frequency_index);
        gs_bit_stream_insert_field(&bitstream, 9, emr->lte_cells[i]->data.lte.pcid);
        gs_bit_stream_insert_field(&bitstream, 6, emr->lte_cells[i]->reported_value);
      }
      else
      {
        MSG_GERAN_ERROR_1("emr->lte_cells[%d] == NULL", i);
      }
    }
  }
  else /* Otherwise, use the pre-Rel8 encoder. */
#endif /* FEATURE_LTE */
  {
    if (emr->ncell_count > 0)
    {
      uint32 i;

      gs_bit_stream_insert_field(&bitstream, 1, 1);

      /* REPORTING_QUANTITY */
      for (i = 0; i <= emr->highest_bitmap_index; ++i)
      {
        if (emr->ncells[i] != NULL)
        {
          gs_bit_stream_insert_field(&bitstream, 1, 1);
          gs_bit_stream_insert_field(&bitstream, 6, emr->ncells[i]->reported_value);
        }
        else
        {
          gs_bit_stream_insert_field(&bitstream, 1, 0);
        }
      }
    }
    else
    {
      gs_bit_stream_insert_field(&bitstream, 1, 0);
    }
  }
}

/**
  @ingroup enhanced_measurement_report

  @brief Adds a neighbour cell measurement to an EMR if there is sufficient
         space.

  @param emr The EMR to add the neighbour cell measurement to.
  @param ncell The neighbour cell measurement to add to the EMR.
  @reutrn TRUE if the neighbour cell measurement was added to the EMR, FALSE if
          there was insufficient space to add it.
*/
static boolean rr_enhanced_measurement_report_add(
  rr_enhanced_measurement_report_t     * emr,
  rr_dedicated_mode_irat_measurement_t * ncell
)
{
  RR_NULL_CHECK_RETURN_PARAM(emr, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(ncell, FALSE);

  /* Work out if there is enough room to add the ncell to the EMR. */

#ifdef FEATURE_LTE
  if (ncell->rat == RR_RAT_LTE)
  {
    uint32 bits_required = 0;

    /* If this is the first LTE cell being added to the measurement report
    then it will trigger a switch from pre-Rel8 to Rel8 encoding.  This has
    associated bit requirements (to store the bitmap_length and additional
    indicator bits). */
    if (emr->lte_cell_count == 0)
    {
      /* Account for the H bit to indicate Additions in Rel-8, plus the 7 bits
      for BITMAP_LENGTH. */
      bits_required += (1 + 7);

      /* Account for N_E-UTRAN in E-UTRAN Measurement Report. */
      bits_required += 2;
    }

    /* Account for the bits required to encode an entry in the E-UTRAN
    Measurement Report. */
    bits_required += (3 + 9 + 6);

    /* Now check that bits_required bits are actually available. */
    if (((N201_SACCH_BTER * 8) - emr->bits_used) >= bits_required &&
        emr->lte_cell_count < ARR_SIZE(emr->lte_cells))
    {
      MSG_GERAN_MED_3("Adding LTE cell EARFCN %d, PCID %d, Value %d to EMR",
                        ncell->data.lte.earfcn,
                        ncell->data.lte.pcid,
                        ncell->reported_value);
      emr->lte_cells[emr->lte_cell_count++] = ncell;
      emr->bits_used += bits_required;
      return TRUE;
    }
    else
    {
      MSG_GERAN_MED_0("Unable to add LTE cell to EMR (insufficient space)");
      return FALSE;
    }
  }
  else
#endif /* FEATURE_LTE */
  {
    /* When adding a GSM / WCDMA / TDSCDMA cell we just need to check whether
    there is sufficient room to extend the bitmap.  It is not possible for this
    cell to trigger a switch from pre-Rel8 to Rel8 encoding. */
    uint8  cell_index;

    boolean cell_index_valid = rr_dedicated_mode_irat_measurement_cell_index(
      ncell,
      &cell_index
    );

    if (cell_index_valid)
    {
      uint32 bits_required = 0;

      if (cell_index > emr->highest_bitmap_index)
      {
        /* We need to grow the reporting_bitmap to fit in the new highest index. */
        bits_required += (cell_index - emr->highest_bitmap_index);

        /* And also account for the 6 bits required for the reported value. */
        bits_required += 6;
      }
      else
      {
        /* We do not need to grow the reporting_bitmap, so we just need to
        account for the additional 6 bits for the reported value. */
        bits_required += 6;
      }

      /* Now check that bits_required bits are actually available. */
      if (((N201_SACCH_BTER * 8) - emr->bits_used) >= bits_required &&
          emr->ncell_count < ARR_SIZE(emr->ncells))
      {
        MSG_GERAN_HIGH_3("Adding RXLEV %d at index %d of EMR reporting bitmap (bits_remaining: %d)",
                           ncell->reported_value,
                           cell_index,
                           ((N201_SACCH_BTER * 8) - emr->bits_used));

        emr->ncells[cell_index] = ncell;

        ++emr->ncell_count;

        emr->bits_used += bits_required;

        if (cell_index > emr->highest_bitmap_index)
        {
          emr->highest_bitmap_index = cell_index;
        }
        return TRUE;
      }
      else
      {
        MSG_GERAN_MED_2("Unable to add cell to EMR, bits_available: %d, bits_required: %d",
                          ((N201_SACCH_BTER * 8) - emr->bits_used),
                          bits_required);
        return FALSE;
      }
    }
    else
    {
      MSG_GERAN_ERROR_0("cell_index is not valid for cell");
      return FALSE;
    }
  }
}

static boolean rr_enhanced_measurement_report_add_invalid_bsic_information(
  rr_enhanced_measurement_report_t     * emr,
  rr_dedicated_mode_irat_measurement_t * ncell
)
{
  uint32 bits_required;

  RR_NULL_CHECK_RETURN_PARAM(emr, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(ncell, FALSE);

  /* Work out if there is enough room to add the ncell to the EMR. */

  RR_ASSERT_RETURN_VALUE(ncell->rat == RR_RAT_GSM, FALSE);

  /* 1 bit for the indicator, 5 + 6 + 6 for the cell info. */
  bits_required = (1 + 5 + 6 + 6);

  if (((N201_SACCH_BTER * 8) - emr->bits_used) >= bits_required &&
      emr->invalid_bsic_information_count < ARR_SIZE(emr->invalid_bsic_information))
  {
    MSG_GERAN_HIGH_3("Adding BCCH-FREQ-NCELL: %d, BSIC: %d, RXLEV-NCELL: %d to Invalid BSIC Information",
                     ncell->data.gsm.cell_index,
                     ncell->data.gsm.bsic,
                     ncell->reported_value);

    emr->invalid_bsic_information[emr->invalid_bsic_information_count++] = ncell;
    emr->bits_used += bits_required;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#ifdef FEATURE_WCDMA
/**
  @ingroup enhanced_measurement_report

  @brief Adds a neighbour cell measurement to an EMR if there is sufficient
         space.  The measurement is for a WCDMA RSSI report.

  @param emr The EMR to add the neighbour cell measurement to.
  @param ncell The neighbour cell measurement to add to the EMR.
  @reutrn TRUE if the neighbour cell measurement was added to the EMR, FALSE if
          there was insufficient space to add it.
*/
static boolean rr_enhanced_measurement_report_add_wcdma_rssi(
  rr_enhanced_measurement_report_t     * emr,
  rr_dedicated_mode_irat_measurement_t * ncell
)
{
  RR_NULL_CHECK_RETURN_PARAM(emr, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(ncell, FALSE);

  RR_ASSERT_RETURN_VALUE(ncell->rat == RR_RAT_WCDMA, FALSE);

  /* Work out if there is enough room to add the ncell to the EMR. */

  /* When adding a GSM / WCDMA / TDSCDMA cell we just need to check whether
  there is sufficient room to extend the bitmap.  It is not possible for this
  cell to trigger a switch from pre-Rel8 to Rel8 encoding. */

  if (ncell->data.wcdma.rssi_index_valid)
  {
    uint32 bits_required = 0;

    if (ncell->data.wcdma.rssi_index > emr->highest_bitmap_index)
    {
      /* We need to grow the reporting_bitmap to fit in the new highest index. */
      bits_required += (ncell->data.wcdma.rssi_index - emr->highest_bitmap_index);

      /* And also account for the 6 bits required for the reported value. */
      bits_required += 6;
    }
    else
    {
      /* We do not need to grow the reporting_bitmap, so we just need to
      account for the additional 6 bits for the reported value. */
      bits_required += 6;
    }

    /* Now check that bits_required bits are actually available. */
    if (((N201_SACCH_BTER * 8) - emr->bits_used) >= bits_required &&
        emr->ncell_count < ARR_SIZE(emr->ncells))
    {
      MSG_GERAN_HIGH_2("Adding RSSI %d at index %d of EMR reporting bitmap", ncell->data.wcdma.reported_rssi, ncell->data.wcdma.rssi_index);

      emr->ncells[ncell->data.wcdma.rssi_index] = ncell;

      ++emr->ncell_count;

      emr->bits_used += bits_required;

      if (ncell->data.wcdma.rssi_index > emr->highest_bitmap_index)
      {
        emr->highest_bitmap_index = ncell->data.wcdma.rssi_index;
      }
      return TRUE;
    }
    else
    {
      MSG_GERAN_MED_0("Unable to add cell to EMR");
      return FALSE;
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("cell_index is not valid for cell");
    return FALSE;
  }
}
#endif /* FEATURE_WCDMA */

/**
  @ingroup enhanced_measurement_report

  @brief Generates an Enhanced Measurement Report.

  @param layer3_message The buffer in to which the message should be encoded.
  @param in_dedicated TRUE if the UE is in dedicated mode, FALSE otherwise.
  @param dedicated_mode_params The current dedicated mode parameters.
*/
static void rr_encode_enhanced_measurement_report(
  byte                         layer3_message[N201_SACCH_BTER],
  boolean                      in_dedicated,
  rr_dedicated_mode_params_t * dedicated_mode_params,
  const                        gas_id_t gas_id
)
{
 
  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
 
  rr_enhanced_measurement_report_t           emr;
  surrounding_measurement_T                  gsm_measurements;
#ifdef FEATURE_WCDMA
  surr_wcdma_ded_measurements_T            * wcdma_measurements = rr_get_wcdma_ded_meas_ptr(gas_id);
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  rr_ded_irat_measurements_t               * tdscdma_measurements = rr_get_tds_ded_meas_ptr(gas_id);
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
  rr_ded_meas_reporting_lte_measurements_t * lte_measurements = &rr_ded_meas_reporting_data_ptr->lte_measurements;
#endif /* FEATURE_LTE */
  gprs_scell_info_t                        * scell = rr_gprs_get_scell_info(gas_id);
  uint32                                     slots = 0;
  boolean                                    emr_full = FALSE;

  RR_NULL_CHECK_RETURN_VOID(layer3_message);
  RR_NULL_CHECK_RETURN_VOID(dedicated_mode_params);
  RR_NULL_CHECK_RETURN_VOID(scell);

  MSG_GERAN_HIGH_0_G("Encoding Enhanced Measurement Report:");

  memset(layer3_message, 0, N201_SACCH_BTER);
  memset(&emr, 0, sizeof(emr));

  emr.rr_short_pd = RADIO_RESOURCES_SHORT_PD;
  emr.message_type = ENHANCED_MEASUREMENT_REPORT;
  emr.short_layer_2_header = 0;

  if (in_dedicated)
  {
    gsm_measurements = *rr_get_surrounding_measurement_ptr(gas_id);
  }
  else
  {
    gsm_measurements = rr_ded_meas_reporting_data_ptr->usat_surrounding_measurement;
  }

  if (in_dedicated)
  {
    rr_ba_lists_t * rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);
    emr.ba_used = rr_ba_lists_ptr->SI5_SI5bis_measurement_report_BA_list.BA_IND;
  }
  else
  {
    usat_system_information_database_T * usat_si_db = rr_get_usat_system_information_database_ptr(gas_id);
    emr.ba_used = usat_si_db->SIbis_measurement_report_BA_list.BA_IND;
  }

  if (scell->gsm.wcdma_ded_neighbor_list.valid_data)
  {
    emr.ba_3g_used = scell->gsm.wcdma_ded_neighbor_list.common_params.BA_IND_3G;
  }
  else if (scell->gsm.utran_neighbor_list.valid_data)
  {
    emr.ba_3g_used = scell->gsm.utran_neighbor_list.common_params.BA_IND_3G;
  }
  else
  {
    emr.ba_3g_used = 0;
  }

  emr.bsic_seen = rr_is_invalid_bsic_in_top_six(&gsm_measurements,gas_id);
  emr.scale = rr_get_scale_for_emr(gas_id);

  /* Serving cell data is always valid, as this function is triggered off
  receipt of serving measurements. */
  emr.serving_cell_data_valid = TRUE;

  if (in_dedicated)
  {
    emr.serving_cell_data.dtx_used = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.DTX_used;
    emr.serving_cell_data.rxlev_val = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXLEV_VAL;
    emr.serving_cell_data.rx_qual_full = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXQUAL_FULL_average;
    emr.serving_cell_data.mean_bep = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.MEAN_BEP;
    emr.serving_cell_data.cv_bep = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.CV_BEP;
    emr.serving_cell_data.nbr_rcvd_blocks = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.NBR_RCVD_BLOCKS;

#ifdef FEATURE_GSM_BAND_AVOIDANCE
    rr_band_avoid_modify_gsm_meas_rpt(&gsm_measurements,
                                      scell->gsm.BCCH_ARFCN,
                                      &emr.serving_cell_data.rxlev_val, 
                                       NULL, /* RX lev sub */
                                      &emr.serving_cell_data.rx_qual_full,
                                       NULL, /*RX qual sub*/
                                      &emr.serving_cell_data.mean_bep,
                                      &emr.serving_cell_data.cv_bep,
                                      &emr.serving_cell_data.nbr_rcvd_blocks,
                                       gas_id);
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
  }
  else
  {
    usat_system_information_database_T * usat_si_db = rr_get_usat_system_information_database_ptr(gas_id);

    emr.serving_cell_data.dtx_used = usat_si_db->cell_options.DTX;
    emr.serving_cell_data.rxlev_val = 0;
    emr.serving_cell_data.rx_qual_full = usat_si_db->reselection_info.RXLEV_average;
    emr.serving_cell_data.mean_bep = 0;
    emr.serving_cell_data.cv_bep = 0;
    emr.serving_cell_data.nbr_rcvd_blocks = 0;
  }


   MSG_GERAN_MED_2_G("Scale used is and RXlev for serving cell before applying offset if required is (%d, %d)",
                      emr.scale,
                      emr.serving_cell_data.rxlev_val
          );

  /* need to adjust RxLev according to SCALE, see 45.008 */
   rr_adjust_rxlev_by_scale(emr.scale, &emr.serving_cell_data.rxlev_val);

   MSG_GERAN_MED_2_G("Scale used is and RXlev for serving cell after applying offset is (%d, %d)",
                      emr.scale,
                      emr.serving_cell_data.rxlev_val
          );



  /* 36 bits are required to encode the fixed elements of the message. */
  emr.bits_used = 36;

  /* Take in to account that at least one bit is required to indicate that
  presence (or not) of the Repeated Invalid BSIC Information structure(s). */
  emr.bits_used += 1;

  /* Take in to account that one bit is needed to indicate whether the pre-Rel8
  REPORTING_QUANTITY bitmap is present or not. */
  emr.bits_used += 1;

  rr_dedicated_mode_irat_measurement_list_create(
    rr_ded_meas_reporting_data_ptr->irat_measurement_list,
    &gsm_measurements,
#ifdef FEATURE_WCDMA
    wcdma_measurements,
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
    tdscdma_measurements,
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
    lte_measurements,
#endif /* FEATURE_LTE */
    dedicated_mode_params,
    in_dedicated,
    gas_id
  );

  qsort(
    &rr_ded_meas_reporting_data_ptr->irat_measurement_list->entries[0],
    rr_ded_meas_reporting_data_ptr->irat_measurement_list->count,
    sizeof(rr_ded_meas_reporting_data_ptr->irat_measurement_list->entries[0]),
    rr_sort_irat_measurements_descending_sort_value_fn
  );

  rr_dedicated_mode_irat_measurement_list_dump(
    rr_ded_meas_reporting_data_ptr->irat_measurement_list
  );

  /* Step 1: Add up to SERVING_BAND_REPORTING cells to the EMR. */
  if (dedicated_mode_params->gsm_params.meas_params.serving_band_reporting > 0)
  {
    rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list;
    sys_band_T                                  serving_band = scell->gsm.BCCH_ARFCN.band;

    irat_measurement_list = rr_ded_meas_reporting_data_ptr->irat_measurement_list;

    slots = dedicated_mode_params->gsm_params.meas_params.serving_band_reporting;

    MSG_GERAN_HIGH_0_G("Searching for ncells for reporting priority level 1 (serving band reporting)");

    /* Find the strongest unreported cell in the list that is GSM and matches
    the serving cell band. */
    while (slots > 0 && !emr_full)
    {
      uint32 i;
      rr_dedicated_mode_irat_measurement_t * cell = NULL;

      /* Search for a cell to add. */
      for (i = 0; i < irat_measurement_list->count && i < ARR_SIZE(irat_measurement_list->entries); ++i)
      {
        if (irat_measurement_list->entries[i].reported == FALSE &&
            irat_measurement_list->entries[i].rat == RR_RAT_GSM &&
            irat_measurement_list->entries[i].data.gsm.arfcn.band == serving_band &&
            rr_is_bsic_valid(irat_measurement_list->entries[i].data.gsm.bsic,
                             irat_measurement_list->entries[i].data.gsm.arfcn,gas_id))
        {

          MSG_GERAN_MED_2_G("Scale used is and RXlev for serving band reporting before applying offset if required is (%d, %d)",
                      emr.scale,
                      irat_measurement_list->entries[i].reported_value);

          /* need to adjust RxLev according to SCALE, see 45.008 */
          rr_adjust_rxlev_by_scale(emr.scale, &irat_measurement_list->entries[i].reported_value);

          MSG_GERAN_MED_2_G("Scale used is and RXlev for serving band reporting after applying offset is (%d, %d)",
                       emr.scale,
                       irat_measurement_list->entries[i].reported_value);

          cell = &irat_measurement_list->entries[i];
          break;
        }
      }

      if (cell != NULL)
      {
        boolean added = rr_enhanced_measurement_report_add(
          &emr,
          cell
        );

        if (added)
        {
          cell->reported = TRUE;
          --slots;
        }
        else
        {
          /* Failed to add the cell to the EMR, so it must be full. */
          emr_full = TRUE;
          break;
        }
      }
      else
      {
        /* No more suitable serving band cells found, break out of the loop. */
        break;
      }
    }
  }



  /* Step 2: Add up to MULTIBAND_REPORTING+UNUSED_SLOTS cells to the EMR. */
  if (dedicated_mode_params->gsm_params.meas_params.multiband_reporting > 0)
  {
    rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list;
    sys_band_T                                  serving_band = scell->gsm.BCCH_ARFCN.band;

    irat_measurement_list = rr_ded_meas_reporting_data_ptr->irat_measurement_list;

    slots += dedicated_mode_params->gsm_params.meas_params.multiband_reporting;

    MSG_GERAN_HIGH_0_G("Searching for ncells for reporting priority level 2 (non-serving band reporting)");

    /* Find the strongest unreported cell in the list that is GSM and does not match
    the serving cell band. */
    while (slots > 0 && !emr_full)
    {
      uint32 i;
      rr_dedicated_mode_irat_measurement_t * cell = NULL;

      /* Search for a cell to add. */
      for (i = 0; i < irat_measurement_list->count && i < ARR_SIZE(irat_measurement_list->entries); ++i)
      {
        if (irat_measurement_list->entries[i].reported == FALSE &&
            irat_measurement_list->entries[i].rat == RR_RAT_GSM &&
            irat_measurement_list->entries[i].data.gsm.arfcn.band != serving_band &&
            rr_is_bsic_valid(irat_measurement_list->entries[i].data.gsm.bsic,
                             irat_measurement_list->entries[i].data.gsm.arfcn,gas_id))
        {

          MSG_GERAN_MED_2_G("Scale used is and RXlev for non-serving band reporting before applying offset if required is (%d, %d)",
                      emr.scale,
                      irat_measurement_list->entries[i].reported_value);

          /* need to adjust RxLev according to SCALE, see 45.008 */
          rr_adjust_rxlev_by_scale(emr.scale, &irat_measurement_list->entries[i].reported_value);

          MSG_GERAN_MED_2_G("Scale used is and RXlev for non-serving band reporting after applying offset is (%d, %d)",
                       emr.scale,
                       irat_measurement_list->entries[i].reported_value);

          cell = &irat_measurement_list->entries[i];
          break;
        }
      }

      if (cell != NULL)
      {
        boolean added = rr_enhanced_measurement_report_add(
          &emr,
          cell
        );

        if (added)
        {
          cell->reported = TRUE;
          --slots;
        }
        else
        {
          /* Failed to add the cell to the EMR, so it must be full. */
          emr_full = TRUE;
          break;
        }
      }
      else
      {
        /* No more suitable serving band cells found, break out of the loop. */
        break;
      }
    }
  }

  /* Step 3: The number of best valid cells and with a reported value equal or
  greater than XXX_REPORTING_THRESHOLD, in each supported other radio access
  technology/mode in the 3G neighbour cell list or on frequencies contained in
  the E-UTRAN Neighbour Cell list, according to the value of the parameters
  XXX_MULTIRAT_REPORTING */
  if (!emr_full)
  {
    rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list;

    uint8 fdd_slots = 0;
    uint8 tdd_slots = 0;
    uint8 eutran_slots = 0;
    
#ifdef FEATURE_WCDMA
    fdd_slots = dedicated_mode_params->utran_params.meas_params.fdd_multirat_reporting;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
    tdd_slots = dedicated_mode_params->utran_params.meas_params.tdd_multirat_reporting;
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
    eutran_slots = dedicated_mode_params->eutran_params.meas_params.multirat_reporting;
#endif /* FEATURE_LTE */

    irat_measurement_list = rr_ded_meas_reporting_data_ptr->irat_measurement_list;

    MSG_GERAN_HIGH_0_G("Searching for ncells for reporting priority level 3 (IRAT cells)");

    while (!emr_full && (fdd_slots > 0 || tdd_slots > 0 || eutran_slots > 0))
    {
      uint32 i;
      rr_dedicated_mode_irat_measurement_t * cell = NULL;

      /* Search for a cell to add. */
      for (i = 0; i < irat_measurement_list->count && i < ARR_SIZE(irat_measurement_list->entries); ++i)
      {
        if (irat_measurement_list->entries[i].reported ||
            irat_measurement_list->entries[i].rat == RR_RAT_GSM)
        {
          /* Skip GSM cells and cells that have already been included in the report. */
          continue;
        }
        else
        {
#ifdef FEATURE_WCDMA
          if (irat_measurement_list->entries[i].rat == RR_RAT_WCDMA && fdd_slots > 0)
          {
            cell = &irat_measurement_list->entries[i];
            break;
          }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
          if (irat_measurement_list->entries[i].rat == RR_RAT_UTRAN_TDD && tdd_slots > 0)
          {
            cell = &irat_measurement_list->entries[i];
            break;
          }
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
          if (irat_measurement_list->entries[i].rat == RR_RAT_LTE && eutran_slots > 0)
          {
            cell = &irat_measurement_list->entries[i];
            break;
          }
#endif /* FEATURE_LTE */
        }
      }

      if (cell != NULL)
      {
        boolean added;

#ifdef FEATURE_WCDMA
        /* RSSI reporting for WCDMA cells, if necessary. */
        if (cell->rat == RR_RAT_WCDMA &&
            cell->data.wcdma.rssi_index_valid)
        {
          added = rr_enhanced_measurement_report_add_wcdma_rssi(
            &emr,
            cell
          );

          if (added)
          {
            rr_irat_measurement_list_clear_rssi_index_valid_flags_for_wcdma_uarfcn(
              irat_measurement_list,
              cell->data.wcdma.uarfcn
            );
          }
          else
          {
            /* Failed to add the cell to the EMR, so it must be full. */
            emr_full = TRUE;
            break;
          }
        }
#endif /* FEATURE_WCDMA */

        added = rr_enhanced_measurement_report_add(
          &emr,
          cell
        );

        if (added)
        {
          cell->reported = TRUE;
#ifdef FEATURE_WCDMA
          if (cell->rat == RR_RAT_WCDMA)
          {
            --fdd_slots;
          }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
          if (cell->rat == RR_RAT_UTRAN_TDD)
          {
            --tdd_slots;
          }
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
          if (cell->rat == RR_RAT_LTE)
          {
            --eutran_slots;
          }
#endif /* FEATURE_LTE */
        }
        else
        {
          /* Failed to add the cell to the EMR, so it must be full. */
          emr_full = TRUE;
          break;
        }
      }
      else
      {
        /* No more suitable IRAT cells found, break out of the loop. */
        break;
      }
    }
  }

  /* Step 4: The remaining GSM cells with known and valid BSIC or, if indicated
  by the parameter INVALID_BSIC_REPORTING, with known and allowed NCC part of
  the BSIC in any frequency band and valid cells of other radio access
  technologies for which XXX_MULTIRAT_REPORTING is greater than zero. */

  if (!emr_full)
  {
    rr_dedicated_mode_irat_measurement_list_t * irat_measurement_list;
    
#ifdef FEATURE_WCDMA
    uint8 fdd_slots = dedicated_mode_params->utran_params.meas_params.fdd_multirat_reporting;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
    uint8 tdd_slots = dedicated_mode_params->utran_params.meas_params.tdd_multirat_reporting;
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
    uint8 eutran_slots = dedicated_mode_params->eutran_params.meas_params.multirat_reporting;	
#endif /* FEATURE_LTE */

    irat_measurement_list = rr_ded_meas_reporting_data_ptr->irat_measurement_list;

    MSG_GERAN_HIGH_0_G("Searching for ncells for reporting priority level 4 (other valid cells)");

    /* Find the strongest unreported cell in the list that is GSM and does not match
    the serving cell band. */
    while (!emr_full)
    {
      uint32 i;
      rr_dedicated_mode_irat_measurement_t * cell = NULL;
      rr_dedicated_mode_irat_measurement_t * invalid_bsic_report_cell = NULL;

      /* Search for a cell to add. */
      for (i = 0; i < irat_measurement_list->count && i < ARR_SIZE(irat_measurement_list->entries); ++i)
      {
        if (irat_measurement_list->entries[i].reported == FALSE)
        {
          if (irat_measurement_list->entries[i].rat == RR_RAT_GSM)
          {
            /* If it is a GSM cell, then check if it's BSIC is valid.  If not,
            and if INVALID_BSIC_REPORTING is enabled, then add the cell to the
            Invalid BSIC Information.  If the BSIC is valid, add the cell to
            the report. */
            if (rr_is_bsic_valid(irat_measurement_list->entries[i].data.gsm.bsic,
                                 irat_measurement_list->entries[i].data.gsm.arfcn,gas_id))
            {

        MSG_GERAN_MED_2_G("Scale used is and RXlev for other valid cells before applying offset if required is (%d, %d)",
                emr.scale,
                irat_measurement_list->entries[i].reported_value);

        /* need to adjust RxLev according to SCALE, see 45.008 */
        rr_adjust_rxlev_by_scale(emr.scale, &irat_measurement_list->entries[i].reported_value);

        MSG_GERAN_MED_2_G("Scale used is and RXlev for other valid cells after applying offset is (%d, %d)",
                 emr.scale,
                 irat_measurement_list->entries[i].reported_value);

              cell = &irat_measurement_list->entries[i];
              break;
            }
            else if (dedicated_mode_params->gsm_params.meas_params.invalid_bsic_reporting)
            {
              invalid_bsic_report_cell = &irat_measurement_list->entries[i];
              break;
            }
          }
          else /* IRAT cell */
          {
#ifdef FEATURE_WCDMA
            if ((irat_measurement_list->entries[i].rat == RR_RAT_WCDMA) && (fdd_slots > 0))
            {
              cell = &irat_measurement_list->entries[i];
              break;
            }
            else
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
            if ((irat_measurement_list->entries[i].rat == RR_RAT_UTRAN_TDD) && (tdd_slots > 0))
            {
              cell = &irat_measurement_list->entries[i];
              break;
            }
            else
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
            if ((irat_measurement_list->entries[i].rat == RR_RAT_LTE) && (eutran_slots > 0))
            {
              cell = &irat_measurement_list->entries[i];
              break;
            }
#endif /* FEATURE_LTE */
            break;
          }
        }
      }

      if (cell != NULL)
      {
        boolean added;

#ifdef FEATURE_WCDMA
        /* RSSI reporting for WCDMA cells, if necessary. */
        if (cell->rat == RR_RAT_WCDMA &&
            cell->data.wcdma.rssi_index_valid)
        {
          added = rr_enhanced_measurement_report_add_wcdma_rssi(
            &emr,
            cell
          );

          if (added)
          {
            rr_irat_measurement_list_clear_rssi_index_valid_flags_for_wcdma_uarfcn(
              irat_measurement_list,
              cell->data.wcdma.uarfcn
            );
          }
          else
          {
            /* Failed to add the cell to the EMR, so it must be full. */
            emr_full = TRUE;
            break;
          }
        }
#endif /* FEATURE_WCDMA */

        added = rr_enhanced_measurement_report_add(
          &emr,
          cell
        );

        if (added)
        {
          cell->reported = TRUE;
        }
        else
        {
          /* Failed to add the cell to the EMR, so it must be full. */
          emr_full = TRUE;
          break;
        }
      }
      else if (invalid_bsic_report_cell != NULL)
      {
        boolean added = rr_enhanced_measurement_report_add_invalid_bsic_information(
          &emr,
          invalid_bsic_report_cell
        );

        if (added)
        {
          invalid_bsic_report_cell->reported = TRUE;
          --slots;
        }
        else
        {
          /* Failed to add the cell to the EMR, so it must be full. */
          emr_full = TRUE;
          break;
        }
      }
      else
      {
        /* No more suitable serving band cells found, break out of the loop. */
        break;
      }
    }
  }



  /* Print out the contents of the measurement report for debug purposes. */
  rr_enhanced_measurement_report_dump(&emr);

  /* And finally, pack the measurement report in to the layer 3 message. */
  rr_enhanced_measurement_report_encode(
    layer3_message,
    &emr
  );
}

/**************************************************************************************
Function
rr_generate_measurement_report

Description
Construct and send Measurement Report based on the index list created from SI2quater or MI.

Parameters
None

Returns
FALSE

**********************************************************************************************/
static void rr_generate_measurement_report(
  boolean meas_valid,
  const gas_id_t gas_id
)
{
  surrounding_measurement_T     *surr_meas_ptr;
  boolean                        enhanced_measurement_report = FALSE;
  byte                           layer3_message[N201_SACCH_BTER]; /* buffer large enough for EMR */
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  surrounding_measurement_T     *surrounding_measurement_ptr = rr_get_surrounding_measurement_ptr(gas_id);
  rr_dedicated_mode_params_t    * dedicated_mode_params = scell_info_ptr->gsm.dedicated_mode_params;
  report_type_T                 report_type;
 // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);

  RR_NULL_CHECK_RETURN_VOID(dedicated_mode_params);

  report_type = dedicated_mode_params->gsm_params.meas_params.report_type;

 

  memset(layer3_message, 0, N201_SACCH_BTER);

  /* Verify if normal 3G measurement reports should and can be sent */
  if (report_type == REPORT_TYPE_N0RMAL ||
     (report_type == REPORT_TYPE_ENHANCED && (rr_gsm_bsic_received_for_each_ncell(gas_id) == FALSE)))
  {
    MSG_GERAN_HIGH_0_G("REPORT_TYPE=Normal Measurement Reporting");
    if ((report_type == REPORT_TYPE_ENHANCED) && (rr_gsm_bsic_received_for_each_ncell(gas_id) == FALSE))
    {
      MSG_GERAN_HIGH_0_G("No EMR:1 BSIC per freq cond. failed");
    }
    rr_encode_measurement_report(
      layer3_message,
      TRUE,
      meas_valid,
      scell_info_ptr->gsm.dedicated_mode_params,
      gas_id
    );

  } /* normal reporting */
  else if ((report_type == REPORT_TYPE_ENHANCED) && (rr_gsm_bsic_received_for_each_ncell(gas_id) == TRUE))
  {
    /* The MS shall use the Enhanced Measurement Report message for
    reporting if at least one BSIC is allocated to each   BA (list) frequency.
    Otherwise, the Measurement Report message shall be used.*/
    MSG_GERAN_HIGH_0_G("REPORT_TYPE=Enhanced Measurement Reporting");

    enhanced_measurement_report = TRUE;

    rr_encode_enhanced_measurement_report(
      layer3_message,
      TRUE,
      scell_info_ptr->gsm.dedicated_mode_params,   
      gas_id
    );
  }

  /* if measurement reports are currently being invalidated, don't bother */
  /* passing valid surround measurements to the logging function below    */
  if (rr_ded_meas_reporting_data_ptr->invalidate_measurement_reports == TRUE)
  {
    surr_meas_ptr = NULL;
  }
  else
  {
    surr_meas_ptr = surrounding_measurement_ptr;
  }

  /***************************************/
  /* Send the measurement report message */
  /***************************************/
  rr_log_sacch_rpt(
    &scell_info_ptr->gsm,
    surr_meas_ptr,
    rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXLEV_FULL_average,
    rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXLEV_SUB_average,
    rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXQUAL_FULL_average,
    rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXQUAL_SUB_average,
    gas_id
  );

  if (enhanced_measurement_report == TRUE)
  {
    /* EMR uses short header format, and includes more octets */
    rr_send_dl_unit_data_req(
      layer3_message,
      N201_SACCH_BTER,
      SAPI0,
      SACCH,
      TRUE,
      gas_id
    );
  }
  else
  {
    rr_send_dl_unit_data_req(
      layer3_message,
      N201_SACCH,
      SAPI0,
      SACCH,
      FALSE,
      gas_id
    );
  }

#ifdef FEATURE_GSM_BAND_AVOIDANCE
  rr_band_avoid_meas_rpt_sent_send_imsg(gas_id);
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

  return;
}

#ifdef FEATURE_WCDMA
/******************************************************************************
Function
rr_update_pn_position_validity_count

Description
PN_position_validity_count counter keeps track of how many times L1 has NOT 
reported a previously-reported WCDMA cell.
 
This function increments this counter for all such not reported WCDMA cells.
If counter reaches max allowed then it invalidates measurement of that cell.

Parameters
[IN]
mph_surround_wcdma_dedicated_meas_ind_T *received_message: The input message
const gas_id_t gas_id: GAS ID

[OUT]
void
******************************************************************************/
static void rr_update_pn_position_validity_count(
  mph_surround_wcdma_dedicated_meas_ind_T *received_message,
  const gas_id_t                           gas_id
)
{
  byte                           i, j;
  boolean                        update_count = FALSE;
  surr_wcdma_ded_measurements_T *stored_wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);
  wcdma_ded_measurements_T      *stored_meas = NULL;
  wcdma_dedicated_meas_T        *rcvd_meas = NULL;

  RR_NULL_CHECK_RETURN_VOID(stored_wcdma_ded_meas_ptr);

  /* Loop through each stored WCDMA cell to check if those are reported or not */
  for (i = 0; i < stored_wcdma_ded_meas_ptr->num_entries; ++i)
  {
    stored_meas = &stored_wcdma_ded_meas_ptr->meas[i];

    if (received_message != NULL)
    {
      /* Loop through cells reported by GL1 */
      for (j = 0; j < received_message->num_best_cells; j++)
      {
        rcvd_meas = &received_message->wcdma_ded_meas[j];
        
        if ((rcvd_meas->UARFCN == stored_meas->cell.uarfcn) &&
            (rcvd_meas->Scrambling_Code == stored_meas->cell.scrambling_code))
        {
          /* Cell is reported by GL1 */
          break;
        }
      }
      
      if (j == received_message->num_best_cells)
      {
        /* Cell is not reported by GL1. Increment its counter. */
        update_count = TRUE;
      }
    }
    else
    {
      /* Measurement Ind is not received from GL1. Increment its counter. */
      update_count = TRUE;
    }

    if (update_count == TRUE)
    {
      byte num_uarfcns = rr_get_num_uarfcns_sent_to_l1_in_dedicated(gas_id);

      stored_meas->PN_position_validity_count++;
      MSG_GERAN_MED_3_G("WCDMA cell(%d, %d) is not reported for %d times", 
        stored_meas->cell.uarfcn,
        stored_meas->cell.scrambling_code,
        stored_meas->PN_position_validity_count);

      /* If GL1 reports one UARFCN per measurement then we must get at least 
       * one measurement of a each cell within N number of measurement ind. 
       * Where N is equal to the number of UARFCN given to GL1.
       *
       * Invalidate PN_Offset of the cell if the count is more than or equal to N */
      if (stored_meas->PN_position_validity_count >= num_uarfcns)
      {
        MSG_GERAN_MED_3_G("Invalidating WCDMA cell (%d, %d) as not included in past %d measurement reports",
          stored_meas->cell.uarfcn,
          stored_meas->cell.scrambling_code,
          num_uarfcns);
        
        /* Mark the meas as Invalid by setting PN_Offset to 0xFFFF */
        stored_meas->PN_Offset = 0xFFFF;
      }
    }
  }
}


/******************************************************************************
Function
rr_delete_invalid_wcdma_ded_meas

Description
Delets invalid WCDMA measurements from 3g dedicated measurement list.
 
Parameters
[IN]
const gas_id_t gas_id: GAS ID

[OUT]
void
******************************************************************************/
static void rr_delete_invalid_wcdma_ded_meas(
  const gas_id_t gas_id
)
{
  byte                           i;
  byte                           invalid_index = 0;
  byte                           num_invalidated = 0;
  boolean                        found_invalid_index = FALSE;
  surr_wcdma_ded_measurements_T *stored_wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);
  wcdma_ded_measurements_T      *stored_meas = NULL;

  RR_NULL_CHECK_RETURN_VOID(stored_wcdma_ded_meas_ptr);

  /* move the valid PN_Offsets to the front in the wcdma_ded_meas array */
  for (i = 0; i < stored_wcdma_ded_meas_ptr->num_entries; ++i)
  {
    stored_meas = &stored_wcdma_ded_meas_ptr->meas[i];

    if (found_invalid_index == FALSE)
    {
      if (stored_meas->PN_Offset == 0xFFFF)
      {
        found_invalid_index = TRUE;
        invalid_index = i;
        num_invalidated = 1;
      }
    }
    else
    {
      if (stored_meas->PN_Offset != 0xFFFF)
      {
        stored_wcdma_ded_meas_ptr->meas[invalid_index] = *stored_meas;
        invalid_index++;
      }
      else
      {
        num_invalidated++;
      }
    }
  }

  stored_wcdma_ded_meas_ptr->num_entries -= num_invalidated;
}


static void rr_prepare_3g_cell_list_report(
  rr_log_3g_meas_report_T *log_3g_meas_rpt,
  const gas_id_t gas_id
)
{
  byte                           j, k;
  byte                           report_index = 0, freq_only_index = 0;
  boolean                        found_index = FALSE, has_freq_only = FALSE;
  byte                           num_entries;
  wcdma_ded_measurements_T      *wcdma_ded_meas_ptr;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  wcdma_ncell_list_data_t       *wcdma_ncell_list_data_ptr = rr_get_wcdma_ncell_list_data_ptr(gas_id);
  surr_wcdma_ded_measurements_T *stored_wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);
  wcdma_ded_neighbor_list_T     *wcdma_ded_nlist_ptr = NULL;
  utran_idle_neighbor_list_T    *wcdma_idle_nlist_ptr = NULL;

  wcdma_ded_nlist_ptr = &scell_info_ptr->gsm.wcdma_ded_neighbor_list;
  wcdma_idle_nlist_ptr = &scell_info_ptr->gsm.utran_neighbor_list;
  num_entries = stored_wcdma_ded_meas_ptr->num_entries;
  wcdma_ded_meas_ptr = stored_wcdma_ded_meas_ptr->meas;

  /* if dedicated nlist data is valid */
  if (wcdma_ded_nlist_ptr->valid_data == TRUE)
  {
    byte num_cells = wcdma_ded_nlist_ptr->wcdma_cell_list.num_of_cells;
    /* loop on wcdma_ded_nlist_ptr */
    for (j = 0; j < num_cells; ++j)
    {
      /* loop on ded measurements */
      for (k = 0; ((k < num_entries) && (log_3g_meas_rpt->NUM_CELLS < MAX_WCDMA_STORED_DED_MEASUREMENTS)); ++k)
      {
        /* cells with valid PN offset */
        if (
            (wcdma_ded_nlist_ptr->wcdma_cell_list.cell_list[j].uarfcn.uarfcn == wcdma_ded_meas_ptr[k].cell.uarfcn) &&
            (wcdma_ded_nlist_ptr->wcdma_cell_list.cell_list[j].cell_code == wcdma_ded_meas_ptr[k].cell.scrambling_code) &&
            (wcdma_ded_meas_ptr[k].PN_Offset != 0xFFFF) // use PN_OFFSET_UNKNOWN when defined in RRC
           )
        {
          byte logi = log_3g_meas_rpt->NUM_CELLS;
          byte num_uarfcns = rr_get_num_uarfcns_sent_to_l1_in_dedicated(gas_id);
          log_3g_meas_rpt->rr_3g_report[logi].UARFCN = wcdma_ded_meas_ptr[k].cell.uarfcn;
          log_3g_meas_rpt->rr_3g_report[logi].SC_CODE = wcdma_ded_meas_ptr[k].cell.scrambling_code;
          log_3g_meas_rpt->rr_3g_report[logi].DIVERSITY = wcdma_ded_meas_ptr[k].cell.diversity;
          log_3g_meas_rpt->rr_3g_report[logi].EXPIRES = (word)(104*4.615*(num_uarfcns - wcdma_ded_meas_ptr[k].PN_position_validity_count));
          log_3g_meas_rpt->rr_3g_report[logi].PN_POS = wcdma_ded_meas_ptr[k].PN_Offset;

          /* This function loops the complete list to find the highest index that
             matches the given uarfcn */
          report_index = rr_find_wcdma_frequency_index(
            wcdma_ncell_list_data_ptr->wcdma_neighbor_cell_list,
            &wcdma_ded_meas_ptr[k].cell,
            &found_index,
            &has_freq_only,
            &freq_only_index
          );

          if (found_index == TRUE)
          {
            log_3g_meas_rpt->rr_3g_report[logi].CELL_INDEX =  report_index;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Failed to find index for logging");
          }

          if ((wcdma_ded_nlist_ptr->mi_params.FDD_REP_QUANT == FDD_REP_QUANT_RSCP) && (has_freq_only == FALSE))
          {
            log_3g_meas_rpt->rr_3g_report[logi].REPORT_FIELDS = 0;
            log_3g_meas_rpt->rr_3g_report[logi].ENERGY = (int8)wcdma_ded_meas_ptr[k].RSCP;
          }
          else if ((wcdma_ded_nlist_ptr->mi_params.FDD_REP_QUANT == FDD_REP_QUANT_Ec_No)&& (has_freq_only == FALSE))
          {
            log_3g_meas_rpt->rr_3g_report[logi].REPORT_FIELDS = 1;
            log_3g_meas_rpt->rr_3g_report[logi].ENERGY = (int8)(wcdma_ded_meas_ptr[k].Ec_Nox2/2);
          }

          if ((wcdma_ded_nlist_ptr->mi_params.FDD_REP_QUANT == FDD_REP_QUANT_RSCP) && (has_freq_only == TRUE))
          {
            log_3g_meas_rpt->rr_3g_report[logi].REPORT_FIELDS = 2;
            log_3g_meas_rpt->rr_3g_report[logi].RSSI = (int8)wcdma_ded_meas_ptr[k].RSSI;
            log_3g_meas_rpt->rr_3g_report[logi].ENERGY = (int8)wcdma_ded_meas_ptr[k].RSCP;
          }
          else if ((wcdma_ded_nlist_ptr->mi_params.FDD_REP_QUANT == FDD_REP_QUANT_Ec_No) && (has_freq_only == TRUE))
          {
            log_3g_meas_rpt->rr_3g_report[logi].REPORT_FIELDS = 3;
            log_3g_meas_rpt->rr_3g_report[logi].RSSI = (int8)wcdma_ded_meas_ptr[k].RSSI;
            log_3g_meas_rpt->rr_3g_report[logi].ENERGY = (int8)(wcdma_ded_meas_ptr[k].Ec_Nox2/2);
          }

          log_3g_meas_rpt->NUM_CELLS++;
        } /* matched measurement */
      } /* loop on ded measurements */

      if (k >= num_entries) /* no match can happen when measurements of not received yet; The log packet will have
        default values */
      {
        /* emit a MSG_HIGH ? */
      }
    } /* loop on wcdma_ded_nlist_ptr */
  } /* if (wcdma_ded_nlist_ptr->valid_data == TRUE) */
  else if (wcdma_idle_nlist_ptr->valid_data)
  {
    byte num_cells = wcdma_idle_nlist_ptr->wcdma_cell_list.num_of_cells;

    /* loop on wcdma_idle_nlist_ptr */
    for (j = 0; j < num_cells; ++j)
    {
      /* loop on ded measurements */
      for (k = 0; ((k < num_entries) && (log_3g_meas_rpt->NUM_CELLS < MAX_WCDMA_STORED_DED_MEASUREMENTS)); ++k)
      {
        /* matched measurement */
        if (
            (wcdma_idle_nlist_ptr->wcdma_cell_list.cell_list[j].uarfcn.uarfcn == wcdma_ded_meas_ptr[k].cell.uarfcn) &&
            (wcdma_idle_nlist_ptr->wcdma_cell_list.cell_list[j].cell_code == wcdma_ded_meas_ptr[k].cell.scrambling_code) &&
            (wcdma_ded_meas_ptr[k].PN_Offset != 0xFFFF) // use PN_OFFSET_UNKNOWN when defined in RRC
           )
        {
          byte logi = log_3g_meas_rpt->NUM_CELLS;
          byte num_uarfcns = rr_get_num_uarfcns_sent_to_l1_in_dedicated(gas_id);
          log_3g_meas_rpt->rr_3g_report[logi].UARFCN = wcdma_ded_meas_ptr[k].cell.uarfcn;
          log_3g_meas_rpt->rr_3g_report[logi].SC_CODE = wcdma_ded_meas_ptr[k].cell.scrambling_code;
          log_3g_meas_rpt->rr_3g_report[logi].DIVERSITY = wcdma_ded_meas_ptr[k].cell.diversity;
          log_3g_meas_rpt->rr_3g_report[logi].EXPIRES = (word)(104*4.615*(num_uarfcns - wcdma_ded_meas_ptr[k].PN_position_validity_count));
          log_3g_meas_rpt->rr_3g_report[logi].PN_POS = wcdma_ded_meas_ptr[k].PN_Offset;

          /* This function loops the complete list to find the highest index that
             matches the given uarfcn */
          report_index = rr_find_wcdma_frequency_index(
            wcdma_ncell_list_data_ptr->wcdma_neighbor_cell_list,
            &wcdma_ded_meas_ptr[k].cell,
            &found_index,
            &has_freq_only,
            &freq_only_index
          );

          if (found_index == TRUE)
          {
            log_3g_meas_rpt->rr_3g_report[logi].CELL_INDEX =  report_index;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Failed to find index for logging");
          }

          if ((wcdma_idle_nlist_ptr->si2q_params.FDD_REP_QUANT == FDD_REP_QUANT_RSCP) && (has_freq_only == FALSE))
          {
            log_3g_meas_rpt->rr_3g_report[logi].REPORT_FIELDS = 0;
            log_3g_meas_rpt->rr_3g_report[logi].ENERGY = (int8)wcdma_ded_meas_ptr[k].RSCP;
          }
          else if ((wcdma_idle_nlist_ptr->si2q_params.FDD_REP_QUANT== FDD_REP_QUANT_Ec_No)&& (has_freq_only == FALSE))
          {
            log_3g_meas_rpt->rr_3g_report[logi].REPORT_FIELDS = 1;
            log_3g_meas_rpt->rr_3g_report[logi].ENERGY = (int8)(wcdma_ded_meas_ptr[k].Ec_Nox2/2);
          }

          if ((wcdma_idle_nlist_ptr->si2q_params.FDD_REP_QUANT == FDD_REP_QUANT_RSCP) && (has_freq_only == TRUE))
          {
            log_3g_meas_rpt->rr_3g_report[logi].REPORT_FIELDS = 2;
            log_3g_meas_rpt->rr_3g_report[logi].RSSI = (int8)wcdma_ded_meas_ptr[k].RSSI;
            log_3g_meas_rpt->rr_3g_report[logi].ENERGY = (int8)wcdma_ded_meas_ptr[k].RSCP;
          }
          else if ((wcdma_idle_nlist_ptr->si2q_params.FDD_REP_QUANT == FDD_REP_QUANT_Ec_No) && (has_freq_only == TRUE))
          {
            log_3g_meas_rpt->rr_3g_report[logi].REPORT_FIELDS = 3;
            log_3g_meas_rpt->rr_3g_report[logi].RSSI = (int8)wcdma_ded_meas_ptr[k].RSSI;
            log_3g_meas_rpt->rr_3g_report[logi].ENERGY = (int8)(wcdma_ded_meas_ptr[k].Ec_Nox2/2);
          }

          log_3g_meas_rpt->NUM_CELLS++;
        } /* matched measurement */
      } /* loop on ded measurements */

      if (k >= num_entries) /* no match can happen when measurements of not received yet; The log packet will have
        default values */
      {
        /* emit a MSG_HIGH ? */
      }
    } /* loop on wcdma_idle_nlist_ptr */
  } /* else if (wcdma_idle_nlist_ptr->valid_data) */
}

#endif /* FEATURE_WCDMA */



/*
 *   Function name:  rr_validate_measurement_data
 *   -------------------------------------
 *
 *   Description: Determines whether a MPH_SERVING_DED_MEAS_IND is valid
 *   ------------
 *
 *
 *   Parameters: ded_meas_ptr - the latest data received from Layer 1
 *   -----------
 *
 *
 *   Returns: Void
 *   --------
 *
 *
 *
 */
static boolean rr_validate_measurement_data(
  mph_serving_ded_meas_ind_T *ded_meas_ptr
)
{
  boolean bValid = TRUE;

  /* L1 will use an error value that exceeds 7 if full RXQUAL was not calculated */
  if (ded_meas_ptr->RXQUAL_FULL_average > RR_L1_MAXIMUM_RXQUAL)
  {
    ded_meas_ptr->RXQUAL_FULL_average = RR_L1_MAXIMUM_RXQUAL;
    bValid = FALSE;
  }

  /* L1 will use an error value that exceeds 7 if sub RXQUAL was not calculated  */
  if (ded_meas_ptr->RXQUAL_SUB_average > RR_L1_MAXIMUM_RXQUAL)
  {
    ded_meas_ptr->RXQUAL_SUB_average = RR_L1_MAXIMUM_RXQUAL;
    bValid = FALSE;
  }

  return(bValid);
}

#ifdef FEATURE_LTE
static void rr_irat_store_dedicated_lte_measurements(
  rr_l1_lte_measurements_t * lte_measurements,
  const gas_id_t gas_id
)
{
  rr_l1_lte_measurements_t * dst = NULL;
  uint32 i; 

  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  RR_NULL_CHECK_RETURN_VOID(lte_measurements);

  MSG_GERAN_HIGH_2_G("Received %d LTE measurements for EARFCN %d:",
                     lte_measurements->no_of_entries,
                     lte_measurements->earfcn);

  for (i = 0; i < lte_measurements->no_of_entries && i < MAX_LTE_MEASUREMENTS_PER_FREQUENCY; ++i)
  {
    MSG_GERAN_HIGH_3_G("  PCID: %d, RSRP: %d, RSRQ: %d",
                       lte_measurements->entries[i].pcid,
                       lte_measurements->entries[i].rsrp,
                       lte_measurements->entries[i].rsrq);
  }

  /* Search for a storage entry for the EARFCN. */
  for (i = 0; i < MAX_RR_L1_EUTRAN_FREQUENCIES; ++i)
  {
    if (rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i] != NULL &&
        rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i]->earfcn == lte_measurements->earfcn)
    {
      dst = rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i];
      break;
    }
  }

  if (dst == NULL)
  {
    MSG_GERAN_ERROR_0_G("Failed to find storage slot for LTE measurements");
  }
  else
  {
    *dst = *lte_measurements;
  }
}
#endif /* FEATURE_LTE */

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* --------------------------------------------------------------------------*/

void rr_reset_ded_meas_reporting(
  const gas_id_t gas_id
)
{
#ifdef FEATURE_LTE
  uint32 i; 
#endif /* FEATURE_LTE */

  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_valid = FALSE;

#ifdef FEATURE_WCDMA
  rr_ded_meas_reporting_data_ptr->mph_surround_wcdma_dedicated_meas_ind_received = FALSE;
#endif /* #ifdef FEATURE_WCDMA */

#ifdef FEATURE_LTE
  for (i = 0; i < MAX_RR_L1_EUTRAN_FREQUENCIES; ++i)
  {
    if (rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i] != NULL)
    {
      GPRS_MEM_FREE(rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i]);
      rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i] = NULL;
    }
  }
#endif /* FEATURE_LTE */
}

#ifdef FEATURE_LTE
void rr_ded_meas_reporting_allocate_lte_measurements_storage(
  mph_dedicated_wcdma_cell_update_list_req_T * update_list_req,
  const gas_id_t gas_id
)
{
  uint32 i;

  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  RR_NULL_CHECK_RETURN_VOID(update_list_req);

  /* Iterate over the allocated storage space, checking that each EARFCN that
  has storage allocated is present in the new list sent to L1.  If it is not
  present, RR will receive no measurements for this EARFCN, so it's storage
  should be freed. */
  for (i = 0; i < MAX_RR_L1_EUTRAN_FREQUENCIES; ++i)
  {
    if (rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i] != NULL)
    {
      boolean earfcn_in_neighbour_list = rr_l1_dedicated_wcdma_cell_update_list_req_contains_earfcn(
        update_list_req,
        rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i]->earfcn
      );

      if (earfcn_in_neighbour_list)
      {
        MSG_GERAN_HIGH_1_G("EARFCN %d still in neighbour list",
                           rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i]->earfcn);
      }
      else
      {
        MSG_GERAN_HIGH_1_G("EARFCN %d no longer in neighbour list, freeing dynamic storage",
                           rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i]->earfcn);
        GPRS_MEM_FREE(rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i]);
        rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i] = NULL;
      }
    }
  }

  /* Now need to iterate over all entries in the
  MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ, allocating storage for any that do
  not already have storage allocated. */
  for (i = 0; i < update_list_req->eutran_frequency_count && i < MAX_RR_L1_EUTRAN_FREQUENCIES; ++i)
  {
    lte_earfcn_t earfcn = update_list_req->eutran_frequencies[i].earfcn;
    boolean earfcn_found = FALSE;
    uint32 j;

    for (j = 0; j < MAX_RR_L1_EUTRAN_FREQUENCIES; ++j)
    {
      if (rr_ded_meas_reporting_data_ptr->lte_measurements.entries[j] != NULL &&
          rr_ded_meas_reporting_data_ptr->lte_measurements.entries[j]->earfcn == earfcn)
      {
        earfcn_found = TRUE;
        break;
      }
    }

    if (earfcn_found == FALSE)
    {
      for (j = 0; j < MAX_RR_L1_EUTRAN_FREQUENCIES; ++j)
      {
        if (rr_ded_meas_reporting_data_ptr->lte_measurements.entries[j] == NULL)
        {
          rr_ded_meas_reporting_data_ptr->lte_measurements.entries[j] = GPRS_MEM_CALLOC(sizeof(rr_l1_lte_measurements_t));

          if (rr_ded_meas_reporting_data_ptr->lte_measurements.entries[j] != NULL)
          {
            rr_ded_meas_reporting_data_ptr->lte_measurements.entries[j]->earfcn = earfcn;
          }
          else
          {
            MSG_GERAN_ERROR_2_G("Failed to allocate %d bytes LTE EARFCN %d", sizeof(rr_l1_lte_measurements_t), earfcn);
          }
          break;
        }
      }
    }
  }
}
#endif /* FEATURE_LTE */

void rr_ded_set_measurement_reports_invalid(
  boolean invalid_reports,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  rr_ded_meas_reporting_data_ptr->invalidate_measurement_reports = invalid_reports;
}

void rr_process_mph_serving_ded_meas_ind(
  mph_serving_ded_meas_ind_T *mph_serving_ded_meas_ind,
  gas_id_t gas_id
)
{
  boolean                    meas_valid;
  gprs_scell_info_t         *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_measured_signal_data_t *rr_measured_signal_data_ptr = rr_get_measured_signal_data_ptr(gas_id);
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr;
#ifdef FEATURE_WCDMA
  rr_log_3g_meas_report_T *log_3g_meas_report_ptr;
#endif

  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

#ifdef FEATURE_WCDMA
  log_3g_meas_report_ptr = rr_log_3g_meas_report_get_ptr(gas_id);
  RR_NULL_CHECK_FATAL(log_3g_meas_report_ptr);
#endif

  /* copy the incoming measurements into the global structure */

  rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind = *mph_serving_ded_meas_ind;
  /* and then validate them, based on RXQUAL values */
  meas_valid = rr_validate_measurement_data(&rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind);

  /* set a global flag to indicate validity */
  rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_valid = meas_valid;

  MSG_GERAN_HIGH_2_G("RXLEV_VAL =%d, NBR_RCVD_BLOCKS =%d", rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXLEV_VAL, rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.NBR_RCVD_BLOCKS);
  MSG_GERAN_HIGH_2_G("MEAN_BEP =%d, CV_BEP =%d", rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.MEAN_BEP, rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.CV_BEP);

  /* trk_lo_adj for a cell is independent of whether the UE is in dedicated or
  idle mode; */
  scell_info_ptr->gsm.trk_lo_adj = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.trk_lo_adj;

#ifdef FEATURE_WCDMA
  if (rr_ded_meas_reporting_data_ptr->mph_surround_wcdma_dedicated_meas_ind_received == FALSE)
  {
    /* No WCDMA measurement report has been received since the last serving meas
    ind.  This could be an indication that L1 has stopped reporting cells because
    they are too weak.  Increment the PN_position_validity counter for each WCDMA
    cell that we know about and delete any that have become invalid. */
    rr_update_pn_position_validity_count(NULL, gas_id);
    rr_delete_invalid_wcdma_ded_meas(gas_id);
  }
  else
  {
    rr_ded_meas_reporting_data_ptr->mph_surround_wcdma_dedicated_meas_ind_received = FALSE;
  }
#endif /* FEATURE_WCDMA */

  rr_generate_measurement_report(meas_valid, gas_id);

#ifdef FEATURE_WCDMA
  memset (log_3g_meas_report_ptr, 0, sizeof(rr_log_3g_meas_report_T));
  rr_prepare_3g_cell_list_report(log_3g_meas_report_ptr, gas_id);
  (void) rr_log_3g_cell_list_report (log_3g_meas_report_ptr, gas_id);
#endif /* FEATURE_WCDMA */

  {
    eng_mode_scell_measurements_t eng_mode_scell_meas = {0};

    eng_mode_scell_meas.C1 = 0;
    eng_mode_scell_meas.C2 = 0;
    eng_mode_scell_meas.rx_lev = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXLEV_SUB_average;
    eng_mode_scell_meas.rx_qual_sub = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXQUAL_SUB_average;
    eng_mode_scell_meas.channel_type = ENG_MODE_RX_LEVEL_TCH_PDTCH;
    eng_mode_scell_meas.rx_lev_sub = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXLEV_SUB_average;
    eng_mode_scell_meas.rx_lev_full = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXLEV_FULL_average;
    eng_mode_scell_meas.rx_qual_full = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXQUAL_FULL_average;
    eng_mode_scell_meas.C31 = 0;
    eng_mode_scell_meas.C32 = 0;


    geran_eng_mode_data_write(
      ENG_MODE_SCELL_MEASUREMENTS,    // geran_eng_mode_info_type
      &eng_mode_scell_meas,           // param_copy
      TRUE,                           // valid_flag
      gas_id                          // gas_id
    );
  }

  /* store the current signal strength and quality for reporting to CM/UI */
  rr_measured_signal_data_ptr->current_signal_quality  = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXQUAL_SUB_average;
  rr_measured_signal_data_ptr->current_signal_strength = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXLEV_SUB_average;

  scell_info_ptr->gsm.reselection_info.RXLEV_average = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXLEV_SUB_average;
  scell_info_ptr->gsm.reselection_info.measurements_valid = TRUE;
}

/*
 *   Function name:  rr_store_ded_meas_while_releasing_l2
 *   ----------------------------------------------------
 *
 *   Description: Handles the serving dedicated measurement report and
 *   ------------ stores it. Used whilst in connection release waiting for
 *                L2 to release, No meas rpts required.
 *
 *   Parameters: mph_serving_ded_meas_ind_T
 *   -----------
 *
 *
 *   Returns: none
 *   --------
 */

void rr_store_ded_meas_while_releasing_l2(
  mph_serving_ded_meas_ind_T *rr_current_serving_ded_meas_ind_ptr,
  const gas_id_t gas_id
)
{
  if (rr_current_serving_ded_meas_ind_ptr)
  {
    /* The contents of the measurement report during connection release are printed out, but are ignored */
    /* It is often the case that these measurements are unreliable when made during the release */

    MSG_GERAN_MED_2_G("releasing RXLEV_FULL=%d RXLEV_SUB=%d",rr_current_serving_ded_meas_ind_ptr->RXLEV_FULL_average, rr_current_serving_ded_meas_ind_ptr->RXLEV_SUB_average);
    MSG_GERAN_MED_2_G("releasing RXQUAL_FULL=%d RXQUAL_SUB=%d",rr_current_serving_ded_meas_ind_ptr->RXQUAL_FULL_average, rr_current_serving_ded_meas_ind_ptr->RXQUAL_SUB_average);
  }
}

void rr_ded_meas_add_eng_mode_nmr_info(
  ARFCN_T                    arfcn,
  byte                       bsic,
  byte                       rxlev,
  gsm_ncell_list_member_t   *opt_ncell_info_ptr,
  eng_mode_nmr_info_t       *eng_mode_nmr,
  gprs_reselection_values_T *curr_resel_entry_ptr
)
{
  if (eng_mode_nmr != NULL)
  {
    if (eng_mode_nmr->num_of_items < ENG_MODE_MAX_NMR_ITEMS)
    {
      nmr_element_t *nmr_ptr = &eng_mode_nmr->nmr[eng_mode_nmr->num_of_items];

      nmr_ptr->arfcn = arfcn.num;
      nmr_ptr->bsic = bsic;
      nmr_ptr->rxlev = rxlev;

      /****populate optional info ****/

      /*set default cell id to invalid, overwrite if valid cell id found*/
      nmr_ptr->optional_info.cell_id = GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;

      if (opt_ncell_info_ptr != NULL)
      {
        information_T *gsm_info_ptr = &opt_ncell_info_ptr->ncell_data.bcch.gsm;

        /*PBCCH opt info not supported*/
        if ( (opt_ncell_info_ptr->bcch_or_pmo == BCCH_FORMAT) &&
             (gsm_info_ptr->valid_data_flag == TRUE) &&
             (gsm_info_ptr->cell_id_valid == TRUE) )
        {
          nmr_ptr->optional_info.cell_id = gsm_info_ptr->cell_id;
          nmr_ptr->optional_info.lac = gsm_info_ptr->location_area_identification.location_area_code;
          nmr_ptr->optional_info.plmn.plmn_id[0] = gsm_info_ptr->location_area_identification.plmn_id.identity[0];
          nmr_ptr->optional_info.plmn.plmn_id[1] = gsm_info_ptr->location_area_identification.plmn_id.identity[1];
          nmr_ptr->optional_info.plmn.plmn_id[2] = gsm_info_ptr->location_area_identification.plmn_id.identity[2];
        }
      }

      if (curr_resel_entry_ptr != NULL)
      {
        nmr_ptr->C1 = curr_resel_entry_ptr->C1;
        nmr_ptr->C2 = curr_resel_entry_ptr->C2;
        nmr_ptr->C31 = curr_resel_entry_ptr->C31;
        nmr_ptr->C32 = curr_resel_entry_ptr->C32;
      }

      eng_mode_nmr->num_of_items++;
    }
    else
    {
      MSG_GERAN_MED_0("MAX number of NMR items reached");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("NULL PTR eng_mode_nmr");
  }
}

void rr_store_surround_meas(
  best_six_info_T * p_best_six_info,
  boolean in_dedicated,
  const gas_id_t gas_id
)
{
  byte                           no_of_entries;
  byte                           cell_index;
  byte                           measurement_index = 0;
  boolean                        authorized_reporting = FALSE;
  surrounding_measurement_T     *meas_ptr;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  surrounding_measurement_T     *surrounding_measurement_ptr = rr_get_surrounding_measurement_ptr(gas_id);
  eng_mode_nmr_info_t            nmr_info;

  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  memset(&nmr_info, 0, sizeof(eng_mode_nmr_info_t));

  if (in_dedicated==TRUE)
  {
    meas_ptr = surrounding_measurement_ptr;
  }
  else
  {
    meas_ptr = &rr_ded_meas_reporting_data_ptr->usat_surrounding_measurement;
  }

  meas_ptr->no_of_entries = 0;

  if (p_best_six_info==NULL)
  {
    no_of_entries = 0;
  }
  else
  {
    no_of_entries = p_best_six_info->no_of_entries;
  }


  for (cell_index = 0; cell_index < no_of_entries; cell_index++)
  {

    if (p_best_six_info->surrounding_channel_info[cell_index].valid_info == TRUE)
    {
      authorized_reporting = FALSE;

      /* to be reported, the cell must be in the current BA list supported by the serving cell */
      /* otherwise, the index will not be computed */
      (void) rr_find_frequency_index(
        p_best_six_info->surrounding_channel_info[cell_index].BCCH_ARFCN,
        &authorized_reporting,
        in_dedicated,
        gas_id
      );


      if (authorized_reporting == TRUE)
      {
        authorized_reporting = FALSE;

        meas_ptr->measurement[measurement_index].BCCH_ARFCN =
          p_best_six_info->surrounding_channel_info[cell_index].BCCH_ARFCN;

        meas_ptr->measurement[measurement_index].RXLEV_average =
          p_best_six_info->surrounding_channel_info[cell_index].RXLEV_average;

        meas_ptr->measurement[measurement_index].BSIC =
          (byte ) p_best_six_info->surrounding_channel_info[cell_index].SCH_block[0] >> 2;

        MSG_GERAN_LOW_1_G("Measurement on channel %d :",
          (int) meas_ptr->measurement[measurement_index].BCCH_ARFCN.num);

        measurement_index++;
        meas_ptr->no_of_entries++;

      }

      /* save NMR info for AGPS */
      rr_ded_meas_add_eng_mode_nmr_info(
        p_best_six_info->surrounding_channel_info[cell_index].BCCH_ARFCN,
        ((p_best_six_info->surrounding_channel_info[cell_index].SCH_block[0] & 0xfc) >> 2),
        p_best_six_info->surrounding_channel_info[cell_index].RXLEV_average,
        NULL, /*no optional info available in dedicated mode*/
        &nmr_info,
        NULL /*C values not calculated in dedicated mode*/
      );
    }

    if (scell_info_ptr->gsm.BCCH_ARFCN.num ==
        p_best_six_info->surrounding_channel_info[cell_index].BCCH_ARFCN.num
       )
    {
      rr_measured_signal_data_t *rr_measured_signal_data_ptr = rr_get_measured_signal_data_ptr(gas_id);

      /* Measurement is for serving cell ??*/
      MSG_GERAN_HIGH_0_G("Received serv cell in surround meas?");

      rr_measured_signal_data_ptr->current_signal_strength = p_best_six_info->surrounding_channel_info[cell_index].RXLEV_average;

      scell_info_ptr->gsm.reselection_info.RXLEV_average = rr_measured_signal_data_ptr->current_signal_strength;
      scell_info_ptr->gsm.reselection_info.measurements_valid = TRUE;
      /* We will calculate C1 later, if we need to do a reestablishment */
    }
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

#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)

void rr_irat_process_surround_irat_meas_ind_in_dedicated(
  mph_surround_irat_meas_ind_t * surround_irat_meas_ind,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(surround_irat_meas_ind);

  switch (surround_irat_meas_ind->rat)
  {
    case RR_L1_IRAT_RAT_UNDEFINED:
    {
      break;
    } /* RR_L1_IRAT_RAT_UNDEFINED */

    case RR_L1_IRAT_RAT_WCDMA:
    {
      MSG_GERAN_ERROR_0_G("RR IRAT Meas Ind not expected for WCDMA");
      break;
    } /* RR_L1_IRAT_RAT_WCDMA */

#ifdef FEATURE_LTE
    case RR_L1_IRAT_RAT_LTE:
    {
      rr_irat_store_dedicated_lte_measurements(
        &surround_irat_meas_ind->measurements.lte,
        gas_id
      );
      break;
    } /* RR_L1_IRAT_RAT_LTE */
#endif /*FEATURE_LTE*/

#ifdef FEATURE_GSM_TDS
    case RR_L1_IRAT_RAT_UTRAN_TDD:
    {
      rr_irat_store_dedicated_utran_tdd_measurements(
        &surround_irat_meas_ind->measurements.utran_tdd,
        gas_id
      );
      break;
    }
#endif /*FEATURE_GSM_TDS*/

    default:
    {
      MSG_GERAN_ERROR_1_G("Unknown RAT: %d", surround_irat_meas_ind->rat);
    }
  }
}

#endif /* FEATURE_LTE || FEATURE_GSM_TDS */

#ifdef FEATURE_GSM_TDS
typedef boolean (* rr_ded_irat_measurement_match_fn)(rr_ded_irat_measurement_t *, void *);

static boolean rr_irat_measurement_match_tdd_uarfcn(
  rr_ded_irat_measurement_t * measurement,
  void                      * value
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

/**
  @brief Deletes the entry at the specified index from the specified
         list of measurements.

  @param ncells The list to modify.
  @param index The index of the element that should be removed.
*/
static void rr_ded_irat_measurements_delete_index(
  rr_ded_irat_measurements_t * measurements,
  uint32                   i
)
{
  uint32 remaining_index;

  if (i < measurements->no_of_entries && i < MAX_IRAT_STORED_DED_MEASUREMENTS)
  {
    /*******************************************************************/
    /* Delete the current entry in the surrounding cell database.  */
    /* Shift the remainder of the list down 1 place to fill in the gap */
    /*******************************************************************/
    for (remaining_index = i + 1;
         remaining_index < measurements->no_of_entries &&
         remaining_index < MAX_IRAT_STORED_DED_MEASUREMENTS;
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
      MSG_GERAN_ERROR_2("Invalid entry=%d in surround DB=%d", i, measurements->no_of_entries);
    }

    memset (&measurements->entries[measurements->no_of_entries].data, 0, sizeof measurements->entries[measurements->no_of_entries].data );
  }
  else
  {
    MSG_GERAN_ERROR_2("invalid entry: %d , size = %d", i, measurements->no_of_entries);
  }
}


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
static void rr_ded_irat_measurements_delete_matching_entries(
  rr_ded_irat_measurements_t       * measurements,
  rr_ded_irat_measurement_match_fn   match_fn,
  void                             * match_value
)
{
  uint32 i;

  RR_ASSERT_RETURN_VOID(measurements != NULL);
  RR_ASSERT_RETURN_VOID(match_fn != NULL);
  RR_ASSERT_RETURN_VOID(match_value != NULL);

  RR_ASSERT_RETURN_VOID(measurements->no_of_entries < MAX_IRAT_STORED_DED_MEASUREMENTS);

  for (i = measurements->no_of_entries; i > 0; --i)
  {
    if (match_fn(&measurements->entries[i-1], match_value))
    {
      rr_ded_irat_measurements_delete_index(measurements, i-1);
    }
  }
}

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
/*************************************************************************
Function
rr_tds_pseudo_fr_set_freq_to_measured

Description
Sets the UARFCN in the pseudo fr stored frequency list to measured.

Parameters
[IN]
uint16 uarfcn
***************************************************************************/
static void rr_tds_pseudo_fr_set_freq_to_measured(
  uint16 uarfcn,
  const gas_id_t gas_id

)
{
  rr_dedicated_data_T *dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  rr_pseudo_fr_info_t *pfr_ptr   = &dedic_ptr->tds_pseudo_fr;
  uint8 i = 0;

  for (i=0; i<pfr_ptr->num_freq; i++)
  {
    if (pfr_ptr->freq_list[i].uarfcn == uarfcn)
    {
      if (pfr_ptr->freq_list[i].measured == FALSE)
      {
        pfr_ptr->freq_list[i].measured = TRUE;
        MSG_GERAN_HIGH_1_G("RR TDS Pseudo FR UARFCN %d measured",uarfcn);
        break;
      }
    }
  }
}
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

/*************************************************************************
Function
rr_irat_store_dedicated_utran_tdd_measurements

Description
Process MPH_SURROUND_IRAT_MEAS_IND message from L1 containing
measurement on TDS cells

Parameters
[IN]
rr_l1_utran_tdd_measurements_t *new_utran_tdd_measurements: The input message

Rules
1) Only known cells are reported by L1

***************************************************************************/

void rr_irat_store_dedicated_utran_tdd_measurements(
  rr_l1_utran_tdd_measurements_t * new_utran_tdd_measurements,
  const gas_id_t gas_id
)
{
  byte                           i, k, num_best_cells;
  tds_ded_measurements_t        *tds_ded_meas_ptr;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_ded_irat_measurements_t    *stored_tds_ded_meas_ptr = rr_get_tds_ded_meas_ptr(gas_id);
#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
  rr_dedicated_data_T           *dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/
  wcdma_ded_neighbor_list_T     *wcdma_ded_nlist_ptr = NULL;
  utran_idle_neighbor_list_T    *wcdma_idle_nlist_ptr = NULL;

  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  /* First check if the received measurementss are for the cells that were given to L1 */

  wcdma_ded_nlist_ptr = &scell_info_ptr->gsm.wcdma_ded_neighbor_list;
  wcdma_idle_nlist_ptr = &scell_info_ptr->gsm.utran_neighbor_list;

  ASSERT (new_utran_tdd_measurements != NULL);

  MSG_GERAN_HIGH_2_G("Received %d UTRAN TDD measurements for UARFCN %d",
           new_utran_tdd_measurements->no_of_entries,
           new_utran_tdd_measurements->uarfcn);

  if (new_utran_tdd_measurements->no_of_entries > MAX_UTRAN_TDD_MEASUREMENTS_PER_FREQUENCY)
  {
    MSG_GERAN_ERROR_2_G("RR number of TDS ded meas out of range:%d, max:%d",new_utran_tdd_measurements->no_of_entries,MAX_UTRAN_TDD_MEASUREMENTS_PER_FREQUENCY);
    return;
  }

  for (i = 0; i < new_utran_tdd_measurements->no_of_entries; ++i)
  {
    MSG_GERAN_HIGH_3_G("TDS CPID: %d, RSCP: %d",
             new_utran_tdd_measurements->entries[i].cell_parameter_id,
             new_utran_tdd_measurements->entries[i].rscp,
             0);
  }

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
  if (dedic_ptr->tds_pseudo_fr.active)
  {
    rr_tds_pseudo_fr_set_freq_to_measured(new_utran_tdd_measurements->uarfcn, gas_id);
  }
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

  rr_ded_meas_reporting_data_ptr->mph_surround_wcdma_dedicated_meas_ind_received = TRUE;

  num_best_cells = (byte)new_utran_tdd_measurements->no_of_entries;

  /* Delete all cells in specified ARFCN, no cells found on this frequency */
  if (num_best_cells == 0)
  {
    rr_ded_irat_measurements_delete_matching_entries(
      stored_tds_ded_meas_ptr,
      rr_irat_measurement_match_tdd_uarfcn,
      &new_utran_tdd_measurements->uarfcn
    );
  }

  if (stored_tds_ded_meas_ptr != NULL)
  {
    /* Now, delete any entries in the ncells store for the UARFCN in the new
      measurements if the entry is not included in the new measurements. */
    for (i = stored_tds_ded_meas_ptr->no_of_entries; i > 0; --i)
    {
      if (stored_tds_ded_meas_ptr->entries[i-1].data.utran_tdd.uarfcn == new_utran_tdd_measurements->uarfcn)
      {
        uint32 j;

        for (j = 0; j < new_utran_tdd_measurements->no_of_entries && j < MAX_UTRAN_TDD_MEASUREMENTS_PER_FREQUENCY; ++j)
        {
          if (stored_tds_ded_meas_ptr->entries[i-1].data.utran_tdd.cell_parameter_id ==
              new_utran_tdd_measurements->entries[j].cell_parameter_id)
          {
            break;
          }
        }

        if (j == new_utran_tdd_measurements->no_of_entries)
        {
          /* The ncell is not present in the new measurements from L1, so
          should be deleted from the store. */
          rr_ded_irat_measurements_delete_index(stored_tds_ded_meas_ptr, i-1);
        }
      }
    }
  }

  /* Loop through each of the received measurement */
  for (k = 0; k < num_best_cells; ++k)
  {
    /* Check if the received measurement was for a cell that was originally sent to L1*/
    rr_l1_utran_tdd_measurement_t *message = &new_utran_tdd_measurements->entries[k];

    if (wcdma_ded_nlist_ptr->valid_data == TRUE)
    {
      byte j;
      byte num_cells = wcdma_ded_nlist_ptr->wcdma_cell_list.num_of_cells;

      for (j = 0; j < num_cells; ++j)
      {
        if (
            (wcdma_ded_nlist_ptr->wcdma_cell_list.cell_list[j].uarfcn.uarfcn == new_utran_tdd_measurements->uarfcn) &&
            (wcdma_ded_nlist_ptr->wcdma_cell_list.cell_list[j].cell_code == message->cell_parameter_id)
           )
        {
          break;
        }
      }
      if (j == num_cells) /* not found */
      {
        /* Error: ignore this measurement */
        MSG_GERAN_HIGH_2_G("Received TDS Dedicated Measurements for an unlisted cell(%d, %d)", new_utran_tdd_measurements->uarfcn, message->cell_parameter_id);
        continue;
      }
    }
    else if (wcdma_idle_nlist_ptr->valid_data)
    {
      byte j;
      byte num_cells = wcdma_idle_nlist_ptr->wcdma_cell_list.num_of_cells;
      for (j = 0; j < num_cells; ++j)
      {
        if (
            (wcdma_idle_nlist_ptr->wcdma_cell_list.cell_list[j].uarfcn.uarfcn == new_utran_tdd_measurements->uarfcn) &&
            (wcdma_idle_nlist_ptr->wcdma_cell_list.cell_list[j].cell_code == message->cell_parameter_id)
           )
        {
          break;
        }
      }
      if (j == num_cells) /* not found */
      {
        /* Error: ignore this measurement */
        MSG_GERAN_HIGH_2_G("Received TDS Dedicated Measurements for an unlisted cell(%d, %d)", new_utran_tdd_measurements->uarfcn, message->cell_parameter_id);
        continue;
      }
    }
#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
    else if (dedic_ptr->tds_pseudo_fr.active)
    {
      uint8 i = 0;

      /*check that measured UARFCN is in requested Pseudo FR freq list*/
      for (i=0; i<dedic_ptr->tds_pseudo_fr.num_freq; i++)
      {
        if (dedic_ptr->tds_pseudo_fr.freq_list[i].uarfcn == new_utran_tdd_measurements->uarfcn)
        {
          break;
        }
      }

      if (i == dedic_ptr->tds_pseudo_fr.num_freq)
      {
        MSG_GERAN_ERROR_1_G("RR ignoring UARFCN %d not expected in Pseudo FR meas results",new_utran_tdd_measurements->uarfcn);
        continue;
      }
    }
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/
    else /* both the lists are invalid so all the measurements should be based on old information */
    {
      /* ignore report and return */
      return;
    }

    if (stored_tds_ded_meas_ptr != NULL)
    {
      /* check if an entry is present already (i.e if it has a measurement already) */
      for (i = 0; i < stored_tds_ded_meas_ptr->no_of_entries; ++i)
      {
        tds_ded_measurements_t *cell = &stored_tds_ded_meas_ptr->entries[i].data.utran_tdd;

        if ((cell->uarfcn == new_utran_tdd_measurements->uarfcn) && (cell->cell_parameter_id == message->cell_parameter_id))
        {
          break;
        }
      }

      tds_ded_meas_ptr = &stored_tds_ded_meas_ptr->entries[i].data.utran_tdd;

      /* if not present already */
      if (i == stored_tds_ded_meas_ptr->no_of_entries)
      {
        if (stored_tds_ded_meas_ptr->no_of_entries >= MAX_IRAT_STORED_DED_MEASUREMENTS)
        {
          MSG_GERAN_ERROR_3_G("RR Cant add TDS uarfcn:%d, cpid:%d in ded meas store, max size:%d", new_utran_tdd_measurements->uarfcn,
                                                                                         message->cell_parameter_id,
                                                                                         MAX_IRAT_STORED_DED_MEASUREMENTS);
          continue;
        }

        stored_tds_ded_meas_ptr->no_of_entries++;
        stored_tds_ded_meas_ptr->entries[i].rat = RR_RAT_UTRAN_TDD;

        tds_ded_meas_ptr->uarfcn = new_utran_tdd_measurements->uarfcn;
        tds_ded_meas_ptr->cell_parameter_id = message->cell_parameter_id;
        tds_ded_meas_ptr->diversity = 0;
      }

      /* All the following will be updated even if the cell was present already */
      tds_ded_meas_ptr->ts0_pos = new_utran_tdd_measurements->ts0_pos;

      tds_ded_meas_ptr->rscp = message->rscp;
    }

  } /* loop on each cell of the best reported by L1 */
}
#endif /* #ifdef FEATURE_GSM_TDS */


#ifdef FEATURE_WCDMA
/******************************************************************************
Function
rr_get_index_to_replace_wcdma_ded_meas

Description
This functions checks if meas recieved for a W cell can replace another W cell meas
if received cell's RCSP is better than than RSCP of cell which is going to be replaced.

This function will get called when there is a overflow detected while storing W meas.

Parameters
[IN]
wcdma_dedicated_meas_T *rcvd_meas: meas data for received cell
byte                   *found_index: index to the meas of W cell whose data is going to be replaced
const gas_id_t gas_id: GAS ID

[OUT]
TRUE: index found and data can be replaced
FALSE: index not found, ignore meas of this cell
******************************************************************************/
static boolean rr_get_index_to_replace_wcdma_ded_meas(
  wcdma_dedicated_meas_T *rcvd_meas,
  byte                   *found_index,
  const gas_id_t          gas_id
)
{
  byte                           i;
  boolean                        index_found = FALSE;
  surr_wcdma_ded_measurements_T *stored_wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);
  wcdma_ded_measurements_T      *stored_meas = NULL;
  wcdma_dedicated_meas_T        *poor_meas = rcvd_meas;

  RR_NULL_CHECK_RETURN_PARAM(stored_wcdma_ded_meas_ptr, FALSE);

  /* find a cell whose RSCP is less than this received measurement */
  for (i = 0; i < stored_wcdma_ded_meas_ptr->num_entries; ++i)
  {
    stored_meas = &stored_wcdma_ded_meas_ptr->meas[i];

    if (stored_meas->RSCP < poor_meas->RSCP)
    {
      index_found = TRUE;
      *found_index = i;
      poor_meas->RSCP = stored_meas->RSCP;
    }
  }

  return index_found;
}


/******************************************************************************
Function
rr_validate_mph_surround_wcdma_dedicated_meas_ind

Description
Validates mph_surround_wcdma_dedicated_meas_ind message by checking if cells 
reported by GL1 are part of current 3g ncell list. 

Parameters
[IN]
mph_surround_wcdma_dedicated_meas_ind_T *received_message: The input message
const gas_id_t gas_id: GAS ID

[OUT]
TRUE: If only known cells are reported by L1
FALSE: If any cell is unkown
******************************************************************************/
static boolean rr_validate_mph_surround_wcdma_dedicated_meas_ind(
  mph_surround_wcdma_dedicated_meas_ind_T *received_message,
  const gas_id_t                           gas_id
)
{
  byte                           i, j, num_best_cells, num_ncells;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_utran_cell_list_T          *wcdma_ncell_list = NULL;
  wcdma_dedicated_meas_T        *wcdma_ded_meas = NULL;
  boolean                        valid = TRUE;

  RR_NULL_CHECK_RETURN_PARAM(received_message, FALSE);

  /* Get pointers to WCDMA ncell list */
  if (scell_info_ptr->gsm.wcdma_ded_neighbor_list.valid_data == TRUE)
  {
    wcdma_ncell_list = &scell_info_ptr->gsm.wcdma_ded_neighbor_list.wcdma_cell_list;
  }
  else if (scell_info_ptr->gsm.utran_neighbor_list.valid_data == TRUE)
  {
    wcdma_ncell_list = &scell_info_ptr->gsm.utran_neighbor_list.wcdma_cell_list;
  }
  else
  {
    /* 3g ncell list is not valid. Invalidate this meas ind. */
    MSG_GERAN_ERROR_0_G("WCDMA neighour list is not valid");
    return FALSE;
  }
  
  num_best_cells = received_message->num_best_cells;
  num_ncells = wcdma_ncell_list->num_of_cells;

  /* Loop through each of the received measurement */
  for (i = 0; i < num_best_cells; ++i)
  {
    /* Check if the received measurement was for a cell that was originally sent to L1 */
    wcdma_ded_meas = &received_message->wcdma_ded_meas[i];

    for (j = 0; j < num_ncells; ++j)
    {
      if ((wcdma_ncell_list->cell_list[j].uarfcn.uarfcn == wcdma_ded_meas->UARFCN) &&
          (wcdma_ncell_list->cell_list[j].cell_code == wcdma_ded_meas->Scrambling_Code))
      {
        break;
      }
    }
    
    if (j == num_ncells) /* not found */
    {
      /* Cell not found in 3g ncell list. Invalidate this meas ind. */
      MSG_GERAN_ERROR_2_G("Received WCDMA Dedicated Measurements for an unlisted cell(%d, %d)", 
        wcdma_ded_meas->UARFCN, 
        wcdma_ded_meas->Scrambling_Code);
      
      valid = FALSE;
      break;
    }
  }

  return valid;
}


/*************************************************************************
Function
rr_process_mph_surround_wcdma_dedicated_meas_ind

Description
Process mph_surround_wcdma_dedicated_meas_ind message from L1 containing
measurement on WCDMA cells

Parameters
[IN]
mph_surround_wcdma_dedicated_meas_ind_T *message: The input message

Rules
1) Only known cells are reported by L1

***************************************************************************/
void rr_process_mph_surround_wcdma_dedicated_meas_ind(
  mph_surround_wcdma_dedicated_meas_ind_T *received_message,
  const gas_id_t                           gas_id
)
{
  byte                           i, k, num_best_cells;
  boolean                        index_found = TRUE;
  wcdma_ded_measurements_T      *stored_meas;
  surr_wcdma_ded_measurements_T *stored_wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  
  ASSERT (received_message != NULL);
  RR_NULL_CHECK_FATAL(stored_wcdma_ded_meas_ptr);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  /* Check for validty of cells reported by GL1.
   * Do not process if valid cells are not reported */
  if (!rr_validate_mph_surround_wcdma_dedicated_meas_ind(received_message, gas_id))
  {
    return;
  }

  rr_ded_meas_reporting_data_ptr->mph_surround_wcdma_dedicated_meas_ind_received = TRUE;

  /* Increment pn_position counter for cells those are not reported by GL1 */
  rr_update_pn_position_validity_count(received_message, gas_id);

  /* Delete invalid cells from wcdma_ded_meas list*/
  rr_delete_invalid_wcdma_ded_meas(gas_id);

  num_best_cells = received_message->num_best_cells;

  /* Loop through each of the received measurement */
  for (k = 0; k < num_best_cells; ++k)
  {
    wcdma_dedicated_meas_T *rcvd_meas = &received_message->wcdma_ded_meas[k];

    MSG_GERAN_MED_3_G("Rcvd meas for WCDMA cell(%d, %d), RSCP=%d", 
      rcvd_meas->UARFCN,
      rcvd_meas->Scrambling_Code,
      rcvd_meas->RSCP);
    
    /* check if an entry is present already (i.e if it has a measurement already) */
    for (i = 0; i < stored_wcdma_ded_meas_ptr->num_entries; ++i)
    {
      wcdma_cell_T *cell = &stored_wcdma_ded_meas_ptr->meas[i].cell;

      if ((cell->uarfcn == rcvd_meas->UARFCN) && (cell->scrambling_code == rcvd_meas->Scrambling_Code))
      {
        break;
      }
    }

    /* if not present create a new entry */
    if (i == stored_wcdma_ded_meas_ptr->num_entries)
    {
      if (i < ARR_SIZE(stored_wcdma_ded_meas_ptr->meas))
      {
        stored_wcdma_ded_meas_ptr->num_entries++;
      }
      else
      {
       /* In case of overflow update the an entry whose meas is poor */
        MSG_GERAN_ERROR_2_G("Error:: Overflow Detected Adding W cell(%d, %d) to WCDMA Ded Meas List", 
          rcvd_meas->UARFCN,
          rcvd_meas->Scrambling_Code);
        
        index_found = rr_get_index_to_replace_wcdma_ded_meas(rcvd_meas, &i, gas_id);

        if (index_found)
        {
          /* meas stored at index "i" will be replaced. */
          MSG_GERAN_ERROR_2_G("This meas will replace meas of W cell(%d, %d)", 
            stored_wcdma_ded_meas_ptr->meas[i].cell.uarfcn,
            stored_wcdma_ded_meas_ptr->meas[i].cell.scrambling_code);
        }
        else
        {
          /* meas cannot be replaced. Drop this meas. */
          MSG_GERAN_HIGH_0_G("This meas will be dropped.");
        }
      }
      
      if (index_found)
      {
        stored_meas = &stored_wcdma_ded_meas_ptr->meas[i];
        stored_meas->cell.uarfcn = rcvd_meas->UARFCN;
        stored_meas->cell.scrambling_code = rcvd_meas->Scrambling_Code;
        stored_meas->cell.diversity = 0;
      }
    }
    else
    {
      stored_meas = &stored_wcdma_ded_meas_ptr->meas[i];
    }

    if (index_found)
    {
      /* All the following will be updated even if the cell was present already */
      stored_meas->Ec_Nox2 = rcvd_meas->Ec_Nox2;
      stored_meas->PN_Offset = rcvd_meas->PN_Offset;
      stored_meas->RSCP = rcvd_meas->RSCP;
      stored_meas->RSSI = rcvd_meas->RSSI;
  
      /* reset */
      stored_meas->PN_position_validity_count = 0;
    }
  } /* loop on each cell of the best 8 reported by L1 */
}
#endif /* #ifdef FEATURE_WCDMA */


/**
 * Used for getting the meas results sent in the last meas report.
 * In non dedicated mode we generate a measurement report based on the last
 * idle mode measurements
 *
 * @param meas_results - ptr where meas results should be copied to
 * @param in_dedicated - TRUE if in dedicated mode
 */
void rr_get_last_meas_results(
  byte* meas_results,
  uint8 sizeofmeas_result,
  boolean in_dedicated,
  const gas_id_t gas_id
)
{
  usat_system_information_database_T *usat_system_information_database_ptr;

  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  usat_system_information_database_ptr = rr_get_usat_system_information_database_ptr(gas_id);
  RR_NULL_CHECK_FATAL(usat_system_information_database_ptr);

  if (!in_dedicated)
  {
    byte layer3_message[N201_SACCH];

    /**************************************/
    /* we do not send measurement reports */
    /* in idle, so we need to generate it */
    /**************************************/

    /* update surrounding measurements database */
    rr_store_surround_meas(
      &usat_system_information_database_ptr->best_six_info,
      in_dedicated,
      gas_id
    );

    /* Stores the meas report into the layer 3 message and also updates rr_last_meas_results */
    rr_encode_measurement_report(
      layer3_message,
      in_dedicated,
      TRUE, /* measurement report flagged as valid */
      NULL,  
      gas_id
    );
  }

  /* copies last meas results IE into the output parameter */
  memscpy(meas_results, sizeofmeas_result,rr_ded_meas_reporting_data_ptr->rr_last_meas_results, RR_LAST_MEAS_BUFFER_SIZE);
}

boolean rr_get_last_dedicated_sub_averages(
  byte *rxlev_sub_ptr,
  byte *rxqual_sub_ptr,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  if (rxlev_sub_ptr != NULL)
  {
    *rxlev_sub_ptr = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXLEV_SUB_average;
  }

  if (rxqual_sub_ptr != NULL)
  {
    *rxqual_sub_ptr = rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_ind.RXQUAL_SUB_average;
  }

  return(rr_ded_meas_reporting_data_ptr->rr_last_serving_ded_meas_valid);
}

/**
  @brief Updates the main BSIC Description store, based on MEASUREMENT
  INFORMATION messages received from the network.

  @param I Pointer to MEASUREMENT INFORMATION structure.
*/

void rr_gsm_update_gsm_bsic_description_from_mi_instances(
  instances_T *I,
  const gas_id_t gas_id
)
{
  int i;
  byte index = 0;
  uint8 bsic_count = 0;

  rr_gsm_bsic_description_T  bsic_description;
  gprs_scell_info_t         *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_ba_lists_t             *rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

  memset(&bsic_description, 0, sizeof(rr_gsm_bsic_description_T));

  for (i = 0; i < MAX_INSTANCES; i++)
  {
    if (!I->instance[i].valid_data)
    {
      continue;
    }

    if (I->instance[i].common_params.BA_IND == rr_ba_lists_ptr->complete_BA_sacch.BA_IND)
    {
      if (I->instance[i].si2q_mi.mi_params.BSIC_Desc_PI == TRUE)
      {
        byte j;
        BSIC_Desc_T *bsic_desc = &I->instance[i].si2q_mi.mi_params.BSIC_Desc;
        BSIC_T tmp_bsic;

        if (bsic_desc->BA_Index_Start_BSIC_PI)
        {
          index = bsic_desc->BA_Index_Start_BSIC;
        }

        CONVERT_TO_BSIC_T(tmp_bsic, bsic_desc->BSIC);

        add_bsic_to_gsm_bsic_description(&bsic_description, &tmp_bsic, index);
        ++bsic_count;

        for (j = 0; j < bsic_desc->Number_Remaining_BSIC; j++)
        {
          if (bsic_desc->mi_bsic[j].Frequency_Scrolling)
          {
            index++;
          }
          CONVERT_TO_BSIC_T(tmp_bsic, bsic_desc->mi_bsic[j].BSIC);
          add_bsic_to_gsm_bsic_description(&bsic_description, &tmp_bsic, index);
          ++bsic_count;
        } /* end for j */
      } /* end if */
    } /* if (I->instance[i].common_params.BA_IND == system_information_database_ptr->complete_BA_sacch.BA_IND) */
  } /* end for i */

  if (bsic_count)
  {
    MSG_GERAN_HIGH_0_G("Updating BSIC Description from MEASUREMENT INFORMATION");
    scell_info_ptr->gsm.gsm_bsic_description = bsic_description;
  }
}


rr_ba_lists_t *rr_get_ba_lists_ptr(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr);
  return rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr;
}


void rr_ded_meas_reporting_l1_in_dedicated(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  RR_NULL_CHECK_FATAL(scell_info_ptr);
  RR_ASSERT_FATAL(rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr == NULL);

  rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr = GPRS_MEM_CALLOC(sizeof(rr_ba_lists_t));
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr);

  // If the GSM SI2 data is available, then use this to initialise the SACCH BA list used for
  // measurement reporting
  if (scell_info_ptr->gsm.valid_data_flag)
  {
    // SI5/SI5bis list is based on SI2/SI2bis list
    rr_ded_meas_reporting_data_ptr->
      rr_ba_lists_ptr->SI5_SI5bis_measurement_report_BA_list = scell_info_ptr->gsm.SI2_BA_list;

    /* add the SI2bis BA list information to this one */
    if (scell_info_ptr->gsm.decode_SI2bis == RR_SI_REQUIRED)
    {
      rr_add_ba_list(
        &rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr->SI5_SI5bis_measurement_report_BA_list,
        &scell_info_ptr->gsm.SI2bis_BA_list,
        gas_id
      );
    }

    /* SI5ter is based on SI2ter BA list */
    rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr->SI5ter_BA_list = scell_info_ptr->gsm.SI2ter_BA_list;

    /* flag the dedicated measurement reports as valid */
    rr_ded_set_measurement_reports_invalid(FALSE, gas_id);
  }

  rr_ded_meas_reporting_data_ptr->irat_measurement_list = GPRS_MEM_CALLOC(sizeof(*rr_ded_meas_reporting_data_ptr->irat_measurement_list));

  RR_NULL_CHECK_RETURN_VOID(rr_ded_meas_reporting_data_ptr->irat_measurement_list );
 
}


boolean rr_ded_ba_sacch_is_valid(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  return ((rr_ded_meas_reporting_data_ptr != NULL) &&
          (rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr != NULL) &&
          (rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr->complete_BA_sacch.no_of_entries != 0));
}


boolean rr_ded_get_copy_of_ba_sacch(
  inter_task_BCCH_allocation_T *ba_sacch_copy_ptr,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  if ((ba_sacch_copy_ptr != NULL) &&
      (rr_ded_meas_reporting_data_ptr != NULL) &&
      (rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr != NULL))
  {
    int i;
    BCCH_frequency_list_T *ba_sacch_ptr = &rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr->complete_BA_sacch;

    ba_sacch_copy_ptr->no_of_entries = ba_sacch_ptr->no_of_entries;

    for (i = 0; i < BA_MAX_LEN; i++)
    {
      ba_sacch_copy_ptr->BCCH_ARFCN[i] = ba_sacch_ptr->BCCH_ARFCN[i];
    }

    return TRUE;
  }

  return FALSE;
}


void rr_ded_meas_reporting_discard_data(
  const gas_id_t gas_id
)
{
#ifdef FEATURE_LTE
  uint32 i; 
#endif /* FEATURE_LTE */

  // Obtain a pointer to the module data
  rr_ded_meas_reporting_data_t *rr_ded_meas_reporting_data_ptr = rr_ded_meas_reporting_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_ded_meas_reporting_data_ptr);

  if (rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr != NULL)
  {
    GPRS_MEM_FREE(rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr);
    rr_ded_meas_reporting_data_ptr->rr_ba_lists_ptr = NULL;
  }

#ifdef FEATURE_LTE
  for (i = 0; i < MAX_RR_L1_EUTRAN_FREQUENCIES; ++i)
  {
    if (rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i] != NULL)
    {
      GPRS_MEM_FREE(rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i]);
      rr_ded_meas_reporting_data_ptr->lte_measurements.entries[i] = NULL;
    }
  }
#endif /* FEATURE_LTE */

  if (rr_ded_meas_reporting_data_ptr->irat_measurement_list != NULL)
  {
    MSG_GERAN_HIGH_1_G("Freeing %d bytes for irat_measurement_list",
                       sizeof(rr_dedicated_mode_irat_measurement_list_t));
    GPRS_MEM_FREE(rr_ded_meas_reporting_data_ptr->irat_measurement_list);
    rr_ded_meas_reporting_data_ptr->irat_measurement_list = NULL;
  }

}


/**
 * Initialisation function called when RR task is started.
 * Note: The primary purpose of this function is to initialise
 * pointers to control/data structures. This should also take into
 * account any second instance of data, as this is a one-off
 * initialisation.
 */
void rr_ded_meas_reporting_task_start_init(
  void
)
{
  // Zero the control/data structure(s)
  memset(rr_ded_meas_reporting_data, 0, sizeof(rr_ded_meas_reporting_data));

  // Setup any specific values

  memscpy(
    &rr_ded_meas_reporting_data[GERAN_DATA_SPACE_INDEX_1].rr_last_meas_results[0],
    sizeof(rr_ded_meas_reporting_data[GERAN_DATA_SPACE_INDEX_1].rr_last_meas_results),
          rr_last_meas_results_init,
    RR_LAST_MEAS_BUFFER_SIZE
  );

#ifdef FEATURE_DUAL_SIM
  memscpy(
    &rr_ded_meas_reporting_data[GERAN_DATA_SPACE_INDEX_2].rr_last_meas_results[0],
    sizeof(rr_ded_meas_reporting_data[GERAN_DATA_SPACE_INDEX_2].rr_last_meas_results),
	   rr_last_meas_results_init,
    RR_LAST_MEAS_BUFFER_SIZE
  );

#ifdef FEATURE_TRIPLE_SIM
  memscpy(
    &rr_ded_meas_reporting_data[GERAN_DATA_SPACE_INDEX_3].rr_last_meas_results[0],
    sizeof(rr_ded_meas_reporting_data[GERAN_DATA_SPACE_INDEX_3].rr_last_meas_results),
    rr_last_meas_results_init,
    RR_LAST_MEAS_BUFFER_SIZE
  );
#endif /* FEATURE_TRIPLE_SIM */
#endif /* FEATURE_DUAL_SIM */
}

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

void rr_dedicated_mode_process_initial_gsm_params(
  gprs_scell_info_t * scell,
  instances_T       * si2quater_instances,
  const             gas_id_t gas_id
)
{
  rr_dedicated_mode_gsm_params_t * dst = NULL;

  SI2q_params_T                  * si2quater_meas_params_instance = NULL;
  SI2q_params_T                  * si2quater_gprs_meas_params_instance = NULL;
  uint32                           i;

  RR_NULL_CHECK_RETURN_VOID(scell);
  RR_NULL_CHECK_RETURN_VOID(scell->gsm.dedicated_mode_params);

  memset(&scell->gsm.dedicated_mode_params->gsm_params, 0, sizeof(rr_dedicated_mode_gsm_params_t));

  dst = &scell->gsm.dedicated_mode_params->gsm_params;

  dst->meas_params.report_type = RR_DEFAULT_REPORT_TYPE;
  dst->meas_params.serving_band_reporting = RR_DEFAULT_SERVING_BAND_REPORTING;

  /* Search the SI2quater instances for the highest index number that contains
  each of "3G Measurement Parameters Description", "GPRS 3G Measurement
  Parameters Description" and "3G Additional Measurement Paramters Description"
  structures. */

  si2quater_instances = rr_get_instances_ptr(SI2Q_INSTANCE,gas_id);

  for (i = 0; i < si2quater_instances->num_instances && i < ARR_SIZE(si2quater_instances->instance); ++i)
  {
    if (si2quater_instances->instance[i].valid_data)
    {
      if (si2quater_instances->instance[i].si2q_mi.si2q_params.MP_Desc_PI)
      {
        si2quater_meas_params_instance = &si2quater_instances->instance[i].si2q_mi.si2q_params;
      }

      if (si2quater_instances->instance[i].si2q_mi.si2q_params.gprs_meas_params.valid)
      {
        si2quater_gprs_meas_params_instance = &si2quater_instances->instance[i].si2q_mi.si2q_params;
      }
    }
  }

  /* Measurement Parameters Description provides:

  REPORT_TYPE
  SERVING_BAND_REPORTING
  */
  if (si2quater_meas_params_instance != NULL)
  {
    dst->meas_params.report_type = si2quater_meas_params_instance->REPORT_TYPE;
    dst->meas_params.serving_band_reporting = si2quater_meas_params_instance->SERVING_BAND_REPORTING;
  }

  /* GPRS Measurement Parameters Description provides:

  REPORT_TYPE
  REPORTING_RATE
  INVALID_BSIC_REPORTING
  MULTIBAND_REPORTING
  SERVING_BAND_REPORTING
  SCALE_ORD
  900_REPORTING_OFFSET
  900_REPORTING_THRESHOLD
  1800_REPORTING_OFFSET
  1800_REPORTING_THRESHOLD
  400_REPORTING_OFFSET     - unsupported
  400_REPORTING_THRESHOLD  - unsupported
  1900_REPORTING_OFFSET
  1900_REPORTING_THRESHOLD
  850_REPORTING_OFFSET
  850_REPORTING_THRESHOLD

  These parameters are not used in dedicated mode.  TBD whether they should be
  used.
  */
  if (si2quater_gprs_meas_params_instance != NULL)
  {

  }

  rr_gsm_meas_params_dump(&dst->meas_params);
}

/**
  @brief Processes the GSM Measurement Parameters contained in a set of
         MEASUREMENT INFORMATION messages.

  This function combines the GSM Measurement Parameters elements from a set
  of MEASUREMENT INFORMATION messages, and stores the resulting information in
  the serving cell database.
*/
static void rr_dedicated_mode_process_gsm_params(
  gprs_scell_info_t * scell,
  instances_T       * mi_instances
)
{
  rr_dedicated_mode_gsm_params_t * dst = NULL;
  MI_params_T                    * mi_common_params_instance = NULL;
  MI_params_T                    * mi_meas_params_instance = NULL;
  uint32                           i;

  RR_NULL_CHECK_RETURN_VOID(mi_instances);
  RR_NULL_CHECK_RETURN_VOID(scell);

  RR_ASSERT_RETURN_VOID(mi_instances->si2q_or_mi == MI_INSTANCE);

  memset(&scell->gsm.dedicated_mode_params->gsm_params, 0, sizeof(rr_dedicated_mode_gsm_params_t));

  dst = &scell->gsm.dedicated_mode_params->gsm_params;

  dst->meas_params.report_type = RR_DEFAULT_REPORT_TYPE;
  dst->meas_params.serving_band_reporting = RR_DEFAULT_SERVING_BAND_REPORTING;

  /* Search the Measurement Information instances for the highest indexed
  message containing "3G Measurement Parameters Description" and "3G Additional
  Measurement Parameters Description 2". */

  for (i = 0; i < mi_instances->num_instances && i < ARR_SIZE(mi_instances->instance); ++i)
  {
    if (mi_instances->instance[i].valid_data)
    {
      mi_common_params_instance = &mi_instances->instance[i].si2q_mi.mi_params;

      if (mi_instances->instance[i].si2q_mi.mi_params.MP_Desc_PI)
      {
        mi_meas_params_instance = &mi_instances->instance[i].si2q_mi.mi_params;
      }
    }
  }

  if (mi_common_params_instance != NULL)
  {
    dst->meas_params.pwrc = mi_common_params_instance->PWRC;
    dst->meas_params.report_type = mi_common_params_instance->REPORT_TYPE;
    dst->meas_params.reporting_rate = mi_common_params_instance->REPORTING_RATE;
    dst->meas_params.invalid_bsic_reporting = mi_common_params_instance->INVALID_BSIC_REPORTING;
  }

  if (mi_meas_params_instance != NULL)
  {
    if (mi_meas_params_instance->MP_Desc.SERVING_BAND_REPORTING_PI)
    {
      dst->meas_params.serving_band_reporting = mi_meas_params_instance->MP_Desc.SERVING_BAND_REPORTING;
    }
    if (mi_meas_params_instance->MP_Desc.MULTIBAND_REPORTING_PI)
    {
      dst->meas_params.multiband_reporting = mi_meas_params_instance->MP_Desc.MULTIBAND_REPORTING;
    }
    dst->meas_params.scale_ord = mi_meas_params_instance->MP_Desc.SCALE_ORD;
    if (mi_meas_params_instance->MP_Desc.REPORTING_900_PI)
    {
      dst->meas_params.reporting_offset_900 = mi_meas_params_instance->MP_Desc.REPORTING_OFFSET_900;
      dst->meas_params.reporting_threshold_900 = mi_meas_params_instance->MP_Desc.REPORTING_THRESHOLD_900;
    }
    if (mi_meas_params_instance->MP_Desc.REPORTING_1800_PI)
    {
      dst->meas_params.reporting_offset_1800 = mi_meas_params_instance->MP_Desc.REPORTING_OFFSET_1800;
      dst->meas_params.reporting_threshold_1800 = mi_meas_params_instance->MP_Desc.REPORTING_THRESHOLD_1800;
    }
    if (mi_meas_params_instance->MP_Desc.REPORTING_1900_PI)
    {
      dst->meas_params.reporting_offset_1900 = mi_meas_params_instance->MP_Desc.REPORTING_OFFSET_1900;
      dst->meas_params.reporting_threshold_1900 = mi_meas_params_instance->MP_Desc.REPORTING_THRESHOLD_1900;
    }
    if (mi_meas_params_instance->MP_Desc.REPORTING_850_PI)
    {
      dst->meas_params.reporting_offset_850 = mi_meas_params_instance->MP_Desc.REPORTING_OFFSET_850;
      dst->meas_params.reporting_threshold_850 = mi_meas_params_instance->MP_Desc.REPORTING_THRESHOLD_850;
    }
  }

  rr_gsm_meas_params_dump(&dst->meas_params);
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)

/**
  @brief Copy idle mode 3G Measurement Parameters to dedicated mode store for
         use on entry to dedicated mode, before Measurement Information is
         received.
*/
static void rr_dedicated_mode_process_initial_utran_params(
  gprs_scell_info_t * scell,
  instances_T       * si2quater_instances, 
  const gas_id_t gas_id
)
{
  rr_dedicated_mode_utran_params_t * dst = NULL;
  SI2q_params_T                    * si2quater_3g_meas_params_instance = NULL;
  SI2q_params_T                    * si2quater_gprs_3g_meas_params_instance = NULL;
  SI2q_params_T                    * si2quater_3g_addl_meas_params_2_instance = NULL;
  uint32                             i;

  RR_NULL_CHECK_RETURN_VOID(scell);
  RR_NULL_CHECK_RETURN_VOID(si2quater_instances);
  RR_NULL_CHECK_RETURN_VOID(scell->gsm.dedicated_mode_params);

  memset(&scell->gsm.dedicated_mode_params->utran_params, 0, sizeof(rr_dedicated_mode_utran_params_t));
  dst = &scell->gsm.dedicated_mode_params->utran_params;

  dst->meas_params.qsearch_c = RR_DEFAULT_QSEARCH_C;

  /* Search the SI2quater instances for the highest index number that contains
  each of "3G Measurement Parameters Description", "GPRS 3G Measurement
  Parameters Description" and "3G Additional Measurement Paramters Description"
  structures. */

  for (i = 0; i < si2quater_instances->num_instances && i < ARR_SIZE(si2quater_instances->instance); ++i)
  {
    if (si2quater_instances->instance[i].valid_data)
    {
      if (si2quater_instances->instance[i].si2q_mi.si2q_params.WCDMA_3G_MP_Desc_PI)
      {
        si2quater_3g_meas_params_instance = &si2quater_instances->instance[i].si2q_mi.si2q_params;
      }

      if (si2quater_instances->instance[i].si2q_mi.si2q_params.gprs_3g_meas_params.valid)
      {
        si2quater_gprs_3g_meas_params_instance = &si2quater_instances->instance[i].si2q_mi.si2q_params;
      }

      if (si2quater_instances->instance[i].si2q_mi.si2q_params.FDD_additional_MPs_2_PI)
      {
        si2quater_3g_addl_meas_params_2_instance = &si2quater_instances->instance[i].si2q_mi.si2q_params;
      }
    }
  }
 
  /* 3G MEASUREMENT PARAMETERS Description provides:

  Qsearch_I               - assigns Qsearch_C, dependent on Qsearch_C_Initial
  Qsearch_C_Initial
  FDD_Qoffset             - not applicable in dedicated mode
  FDD_REP_QUANT
  FDD_MULTIRAT_REPORTING
  FDD_Qmin                - not applicable in dedicated mode
  TDD_Qoffset             - not applicable in dedicated mode
  TDD_MULTIRAT_REPORTING
  */
  if (si2quater_3g_meas_params_instance != NULL)
  {
    if (si2quater_3g_meas_params_instance->QSearch_C_Initial == 0)
    {
      dst->meas_params.qsearch_c = si2quater_3g_meas_params_instance->QSearch_I;
    }
    else
    {
      dst->meas_params.qsearch_c = QSEARCH_I_ALWAYS;
    }

    if (si2quater_3g_meas_params_instance->FDD_Info_PI)
    {
      dst->meas_params.fdd_rep_quant = si2quater_3g_meas_params_instance->FDD_REP_QUANT;
      dst->meas_params.fdd_multirat_reporting = si2quater_3g_meas_params_instance->FDD_MULTIRAT_REPORTING;
    }

    if (si2quater_3g_meas_params_instance->TDD_Info_PI)
    {
      dst->meas_params.tdd_multirat_reporting = si2quater_3g_meas_params_instance->TDD_MULTIRAT_REPORTING;
    }
  }

  /* GPRS 3G MEASUREMENT PARAMETERS Description provides:

  Qsearch_P               - not applicable in dedicated mode
  FDD_REP_QUANT           - does this override the value in "3G Measurement Parameters Description"?
  FDD_MULTIRAT_REPORTING  - does this override the value in "3G Measurement Parameters Description"?
  FDD_REPORTING_OFFSET    - does this override the value in "3G Measurement Parameters Description"?
  FDD_REPORTING_THRESHOLD - does this override the value in "3G Measurement Parameters Description"?
  TDD_MULTIRAT_REPORTING  - does this override the value in "3G Measurement Parameters Description"?
  TDD_REPORTING_OFFSET    - does this override the value in "3G Measurement Parameters Description"?
  TDD_REPORTING_THRESHOLD - does this override the value in "3G Measurement Parameters Description"?
  */
  if (si2quater_gprs_3g_meas_params_instance != NULL)
  {
    /* TODO: Check if any of these parameters should be used in dedicated mode,
    e.g. FDD_REPORTING_OFFSET / FDD_REPORTING_THRESHOLD, which otherwise would
    not be available until Measurement Information is received. */
  }

  /* 3G Additional Measurement Parameters Description 2 provides:

  FDD_REPORTING_THRESHOLD_2
  */
  if (si2quater_3g_addl_meas_params_2_instance != NULL)
  {
    dst->meas_params.fdd_reporting_threshold_2 = si2quater_3g_addl_meas_params_2_instance->FDD_REPORTING_THRESHOLD_2;
  }

  rr_utran_meas_params_dump(&dst->meas_params);
}

/**
  @brief Processes the UTRAN Parameters Description structure contained in
         a set of MEASUREMENT INFORMATION messages.

  This function combines the UTRAN Parameters Description elements from a set
  of MEASUREMENT INFORMATION messages, and stores the resulting information in
  the serving cell database.

  This information is stored in dynamically allocated memory in the system
  information database, this memory should be freed when RR leaves dedicated
  mode.
*/
void rr_dedicated_mode_process_utran_params(
  gprs_scell_info_t * scell,
  instances_T       * mi_instances
)
{
  rr_dedicated_mode_utran_params_t * dst = NULL;
  MI_params_T                      * mi_3g_meas_params_instance = NULL;
  MI_params_T                      * mi_3g_addl_meas_params_2_instance = NULL;
  uint32                             i;

  RR_NULL_CHECK_RETURN_VOID(mi_instances);
  RR_NULL_CHECK_RETURN_VOID(scell);
  RR_NULL_CHECK_RETURN_VOID(scell->gsm.dedicated_mode_params);

  RR_ASSERT_RETURN_VOID(mi_instances->si2q_or_mi == MI_INSTANCE);

  memset(&scell->gsm.dedicated_mode_params->utran_params, 0, sizeof(rr_dedicated_mode_utran_params_t));

  dst = &scell->gsm.dedicated_mode_params->utran_params;

  dst->meas_params.qsearch_c = RR_DEFAULT_QSEARCH_C;

  /* Search the Measurement Information instances for the highest indexed
  message containing "3G Measurement Parameters Description" and "3G Additional
  Measurement Parameters Description 2". */

  for (i = 0; i < mi_instances->num_instances && i < ARR_SIZE(mi_instances->instance); ++i)
  {
    if (mi_instances->instance[i].si2q_mi.mi_params.MI_3G_MP_Desc_PI)
    {
      mi_3g_meas_params_instance = &mi_instances->instance[i].si2q_mi.mi_params;
    }

    if (mi_instances->instance[i].si2q_mi.mi_params.fdd_reporting_threshold_2_valid)
    {
      mi_3g_addl_meas_params_2_instance = &mi_instances->instance[i].si2q_mi.mi_params;
    }
  }

  if (mi_3g_meas_params_instance != NULL)
  {
    dst->meas_params.qsearch_c = mi_3g_meas_params_instance->QSearch_C;
    dst->meas_params.fdd_rep_quant = mi_3g_meas_params_instance->FDD_REP_QUANT;

    if (mi_3g_meas_params_instance->FDD_MULTIRAT_REPORTING_PI)
    {
      dst->meas_params.fdd_multirat_reporting = mi_3g_meas_params_instance->FDD_MULTIRAT_REPORTING;
    }

    if (mi_3g_meas_params_instance->FDD_REP_PI)
    {
      dst->meas_params.fdd_reporting_offset = mi_3g_meas_params_instance->FDD_REP_OFFSET;
      dst->meas_params.fdd_reporting_threshold = mi_3g_meas_params_instance->FDD_REP_THRESHOLD;
    }

    if (mi_3g_meas_params_instance->TDD_MULTIRAT_REPORTING_PI)
    {
      dst->meas_params.tdd_multirat_reporting = mi_3g_meas_params_instance->TDD_MULTIRAT_REPORTING;
    }

    if (mi_3g_meas_params_instance->TDD_REP_PI)
    {
      dst->meas_params.tdd_reporting_offset = mi_3g_meas_params_instance->TDD_REP_OFFSET;
      dst->meas_params.tdd_reporting_threshold = mi_3g_meas_params_instance->TDD_REP_THRESHOLD;
    }
  }

  if (mi_3g_addl_meas_params_2_instance != NULL)
  {
    dst->meas_params.fdd_reporting_threshold_2 = mi_3g_addl_meas_params_2_instance->fdd_reporting_threshold_2;
  }

  rr_utran_meas_params_dump(&dst->meas_params);
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#ifdef FEATURE_LTE

/**
  @brief Copy idle mode E-UTRAN Parameters to dedicted mode store for use on
         entry to dedicated mode, before Measurement Information is received.

  @param meas_params Idle mode E-UTRAN Measurement Parameters.
  @param neighbour_list Idle mode E-UTRAN Neighbour List.
  @param scell The current serving cell structure in to which the dedicated mode
         information is to be populated.
*/
void rr_dedicated_mode_process_initial_eutran_params(
  rr_eutran_meas_params_t * meas_params,
  rr_lte_neighbour_list_t * lte_neighbour_list,
  gprs_scell_info_t       * scell
)
{
  rr_dedicated_mode_eutran_params_t * dst = NULL;

  RR_NULL_CHECK_RETURN_VOID(scell);
  RR_NULL_CHECK_RETURN_VOID(scell->gsm.dedicated_mode_params);

  memset(&scell->gsm.dedicated_mode_params->eutran_params, 0, sizeof(rr_dedicated_mode_eutran_params_t));

  dst = &scell->gsm.dedicated_mode_params->eutran_params;

  if (meas_params != NULL)
  {
    dst->meas_params = *meas_params;
  }
  else
  {
    /* No E-UTRAN Measurement Parameters are available, so configure the
    default Qsearch_C_E-UTRAN value to be sent to L1. */
    dst->meas_params.qsearch_c = RR_DEFAULT_QSEARCH_C_EUTRAN;
  }

  if (lte_neighbour_list != NULL)
  {
    uint32 i;

    for (i = 0;
         i < lte_neighbour_list->count &&
         i < ARR_SIZE(lte_neighbour_list->entries) &&
         i < ARR_SIZE(dst->neighbour_list.entries);
         ++i)
    {
      if (dst->neighbour_list.count < ARR_SIZE(dst->neighbour_list.entries))
      {
        dst->neighbour_list.entries[i].frequency = lte_neighbour_list->entries[i].frequency;
        dst->neighbour_list.entries[i].not_allowed_cells = lte_neighbour_list->entries[i].not_allowed_cells;
        dst->neighbour_list.entries[i].measurement_control = lte_neighbour_list->entries[i].measurement_control;
        ++dst->neighbour_list.count;
      }
    }
  }

  rr_eutran_meas_params_dump(&dst->meas_params);
}

/**
  @brief Processes the E-UTRAN Parameters Description structure contained in
         a set of MEASUREMENT INFORMATION messages.

  This function combines the E-UTRAN Parameters Description elements from a set
  of MEASUREMENT INFORMATION messages, and stores the resulting information in
  the serving cell database.

  This information is stored in dynamically allocated memory in the system
  information database, this memory should be freed when RR leaves dedicated
  mode.
*/

void rr_dedicated_mode_process_eutran_params(
  gprs_scell_info_t * scell,
  instances_T       * mi_instances
)
{
  rr_dedicated_mode_eutran_params_t * dst = NULL;
  boolean                             meas_control_params_valid = FALSE;
  rr_eutran_meas_control_params_t     meas_control_params;
  uint32                              i;

  RR_NULL_CHECK_RETURN_VOID(mi_instances);
  RR_NULL_CHECK_RETURN_VOID(scell);
  RR_NULL_CHECK_RETURN_VOID(scell->gsm.dedicated_mode_params);

  RR_ASSERT_RETURN_VOID(mi_instances->si2q_or_mi == MI_INSTANCE);

  memset(&meas_control_params, 0, sizeof(meas_control_params));

  memset(&scell->gsm.dedicated_mode_params->eutran_params, 0, sizeof(rr_dedicated_mode_eutran_params_t));

  dst = &scell->gsm.dedicated_mode_params->eutran_params;
  dst->meas_params.qsearch_c = RR_DEFAULT_QSEARCH_C_EUTRAN;

  for (i = 0; i < mi_instances->num_instances && i < MAX_INSTANCES; ++i)
  {
    if (mi_instances->instance[i].valid_data)
    {
      SI2q_MI_instance_T * instance = &mi_instances->instance[i];

      if (instance->si2q_mi.mi_params.eutran_params.meas_params_valid)
      {
        dst->meas_params = instance->si2q_mi.mi_params.eutran_params.meas_params;
      }

      if (instance->si2q_mi.mi_params.eutran_params.neighbour_cells_count > 0)
      {
        uint32 j;

        for (j = 0;
             j < instance->si2q_mi.mi_params.eutran_params.neighbour_cells_count &&
             j < RR_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS;
             ++j)
        {
          if (dst->neighbour_list.count < ARR_SIZE(dst->neighbour_list.entries))
          {
            dst->neighbour_list.entries[dst->neighbour_list.count].frequency =
              instance->si2q_mi.mi_params.eutran_params.neighbour_cells[j];

            ++dst->neighbour_list.count;
          }
          else
          {
            MSG_GERAN_ERROR_2("Error: overflow detected adding EARFCN %d to dedicated E-UTRAN Neighbour List (MI %d)",
                              instance->si2q_mi.mi_params.eutran_params.neighbour_cells[j].earfcn,
                              i);
          }
        }
      }

      /* Only the Measurement Control params from the highest indexed MI are
      required, so we grab a copy here.  These will be merged in to the
      neighbour list later in this function. */
      if (instance->si2q_mi.mi_params.eutran_params.meas_control_params_valid)
      {
        meas_control_params_valid = TRUE;
        meas_control_params = instance->si2q_mi.mi_params.eutran_params.meas_control_params;
      }
    }
  }

  /* Now iterate over the instances again, this time processing the Not Allowed
  Cells, which should be applied to the relevant EUTRAN Neighbour List entries. */

  for (i = 0; i < mi_instances->num_instances && i < MAX_INSTANCES; ++i)
  {
    if (mi_instances->instance[i].valid_data)
    {
      SI2q_MI_instance_T * instance = &mi_instances->instance[i];

      if (instance->si2q_mi.mi_params.eutran_params.not_allowed_cells_count > 0)
      {
        uint32 j;

        for (j = 0;
             j < instance->si2q_mi.mi_params.eutran_params.not_allowed_cells_count &&
             j < RR_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS;
             ++j)
        {
          rr_si2quater_ro_eutran_not_allowed_cells_t * src =
            &instance->si2q_mi.mi_params.eutran_params.not_allowed_cells[j];

          /* Each Not Allowed Cells IE refers to zero or more frequencies,
          identified by their frequency index. */
          uint32 k;

          for (k = 0;
               k < src->eutran_frequency_indices_count &&
               k < MAX_EUTRAN_NOT_ALLOWED_CELLS_EUTRAN_FREQUENCY_INDICES;
               ++k)
          {
            uint8 frequency_index = src->eutran_frequency_indices[k];

            if (frequency_index < RR_EUTRAN_NEIGHBOUR_LIST_LENGTH)
            {
              if (frequency_index < dst->neighbour_list.count)
              {
                dst->neighbour_list.entries[frequency_index].not_allowed_cells = src->not_allowed_cells;
              }
              else
              {
                MSG_GERAN_ERROR_2("Error: received Not Allowed Cells for index %d, %d in list",
                                  frequency_index,
                                  dst->neighbour_list.count);
              }
            }
            else
            {
              MSG_GERAN_ERROR_1("Error: frequency_index %d out of range", frequency_index);
            }
          }
        }
      }
    }
  }

  /* Finally, merge the Measurement Control parameters in to the neighbour
  list (if provided by the network). */
  if (meas_control_params_valid)
  {
    /* Iterate over each entry in the E-UTRAN Neighbour List - for each entry,
    get the Measurement_Control_E-UTRAN value from the MI structure. */
    for (i = 0;
         i < dst->neighbour_list.count &&
         i < ARR_SIZE(dst->neighbour_list.entries);
         ++i)
    {
      if (IS_BIT_NUM_SET(meas_control_params.validity_bitmap, i))
      {
        if (IS_BIT_NUM_SET(meas_control_params.value_bitmap, i))
        {
          dst->neighbour_list.entries[i].measurement_control = TRUE;
        }
        else
        {
          dst->neighbour_list.entries[i].measurement_control = FALSE;
        }
      }
      else if (meas_control_params.default_meas_control_valid)
      {
        dst->neighbour_list.entries[i].measurement_control =
          meas_control_params.default_meas_control;
      }
      else
      {
        dst->neighbour_list.entries[i].measurement_control = RR_DEFAULT_MEASUREMENT_CONTROL_EUTRAN;
      }
    }
  }
  else
  {
    /* Assign default Measurement_Control_E-UTRAN value (1) */
    for (i = 0;
         i < dst->neighbour_list.count &&
         i < ARR_SIZE(dst->neighbour_list.entries);
         ++i)
    {
      dst->neighbour_list.entries[i].measurement_control = RR_DEFAULT_MEASUREMENT_CONTROL_EUTRAN;
    }
  }

  rr_eutran_meas_params_dump(&dst->meas_params);
}

#endif /* FEATURE_LTE */


void rr_dedicated_mode_process_params(
  gprs_scell_info_t * scell,
  instances_T       * mi_instances
)
{
  if (scell->gsm.dedicated_mode_params == NULL)
  {
    /* allocate and initialise the required memory */
    scell->gsm.dedicated_mode_params = GPRS_MEM_CALLOC(sizeof(rr_dedicated_mode_params_t));
    if (scell->gsm.dedicated_mode_params)
    {
      MSG_GERAN_HIGH_1("Allocated %d bytes for dedicated mode parameters", sizeof(rr_dedicated_mode_params_t));
    }
  }
  else
  {
    /* re-initialise the existing memory allocation */
    memset(scell->gsm.dedicated_mode_params, 0, sizeof(rr_dedicated_mode_params_t));
    MSG_GERAN_HIGH_0("Using previously allocated memory for dedicated mode parameters");
  }

  if (scell->gsm.dedicated_mode_params == NULL)
  {
    MSG_GERAN_ERROR_0("Failed to allocate memory for dedicated mode parameters");
    return;
  }

  rr_dedicated_mode_process_gsm_params(scell, mi_instances);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  rr_dedicated_mode_process_utran_params(scell, mi_instances);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
  rr_dedicated_mode_process_eutran_params(scell, mi_instances);
#endif /* FEATURE_LTE */
}

void rr_dedicated_mode_process_initial_params(
  gprs_scell_info_t * scell,
  instances_T       * si2quater_instances,
  const             gas_id_t gas_id
)
{
  if (scell->gsm.dedicated_mode_params == NULL)
  {
    /* allocate and initialise the required memory */
    scell->gsm.dedicated_mode_params = GPRS_MEM_CALLOC(sizeof(rr_dedicated_mode_params_t));
    if (scell->gsm.dedicated_mode_params)
    {
      MSG_GERAN_HIGH_1_G("Allocated %d bytes for dedicated mode parameters", sizeof(rr_dedicated_mode_params_t));
    }
  }
  else
  {
    /* re-initialise the existing memory allocation */
    memset(scell->gsm.dedicated_mode_params, 0, sizeof(rr_dedicated_mode_params_t));
    MSG_GERAN_HIGH_0_G("Using previously allocated memory for dedicated mode parameters");
  }

  if (scell->gsm.dedicated_mode_params == NULL)
  {
    MSG_GERAN_ERROR_0_G("Failed to allocate memory for dedicated mode parameters");
    return;
  }

  if (si2quater_instances == NULL)
  {
    scell->gsm.dedicated_mode_params->gsm_params.meas_params.report_type = RR_DEFAULT_REPORT_TYPE;
    scell->gsm.dedicated_mode_params->gsm_params.meas_params.serving_band_reporting = RR_DEFAULT_SERVING_BAND_REPORTING;
  }
  else
  {
    rr_dedicated_mode_process_initial_gsm_params(scell, si2quater_instances,gas_id);
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  if (si2quater_instances == NULL)
  {
    scell->gsm.dedicated_mode_params->utran_params.meas_params.qsearch_c = RR_DEFAULT_QSEARCH_C;
  }
  else
  {
    rr_dedicated_mode_process_initial_utran_params(scell, si2quater_instances,gas_id);
  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
  if (si2quater_instances == NULL)
  {
    scell->gsm.dedicated_mode_params->eutran_params.meas_params.qsearch_c = RR_DEFAULT_QSEARCH_C_EUTRAN;
  }
  else
  {
    rr_dedicated_mode_process_initial_eutran_params(
    &scell->eutran_params.meas_params,
    &scell->lte_neighbour_list,
    scell
    );
  }
#endif /* FEATURE_LTE */
}

boolean rr_dedicated_mode_is_wcdma_enabled(const gas_id_t gas_id)
{
#ifdef FEATURE_WCDMA
#ifdef FEATURE_SGLTE
  if (rr_is_sglte_mode_active(gas_id))
  {
    return FALSE;
  }
  else
  {
    return rr_is_wcdma_enabled(gas_id);
  }
#else
  return rr_is_wcdma_enabled(gas_id);
#endif /* FEATURE_SGLTE */
#else
  return FALSE;
#endif /* FEATURE_WCDMA */
}

boolean rr_dedicated_mode_is_tdscdma_enabled(const gas_id_t gas_id)
{
#ifdef FEATURE_GSM_TDS
#ifdef FEATURE_SGLTE
  if (rr_is_sglte_mode_active(gas_id))
  {
    return FALSE;
  }
  else
  {
    return rr_is_utran_tdd_enabled(gas_id);
  }
#else
  return rr_is_utran_tdd_enabled(gas_id);
#endif /* FEATURE_SGLTE */
#else
  return FALSE;
#endif /* FEATURE_GSM_TDS */
}

boolean rr_dedicated_mode_is_lte_enabled(const gas_id_t gas_id)
{
#ifdef FEATURE_LTE
#ifdef FEATURE_SGLTE
  if (rr_is_sglte_mode_active(gas_id))
  {
    return FALSE;
  }
  else
  {
    return rr_is_lte_enabled(gas_id);
  }
#else
  return rr_is_lte_enabled(gas_id);
#endif /* FEATURE_SGLTE */
#else
  return FALSE;
#endif /* FEATURE_LTE */
}

/* EOF */

