/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        GRR Meas Reporting Module

GENERAL DESCRIPTION
  This module contains control and individual state machines for GRR NC and Extended
  measurement reporting.

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_meas_rpt_control.c#1 $
  $History: rr_gprs_meas_rpt_control.c $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
23/07/12  SJW   Allow for variability of first T3158 period during NC2 mode
24/05/12  SJW   Improved accuracy of T3158 duration calculations
23/05/03  NT    Added multiband reporting
23/04/03  NT    Added code for HCS reselection on BCCH to clear the parameters
          after a reselection/dsf/empty list from another PMO
===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"
#include "rr_seg_load.h"
#include "rr_gprs_meas_rpt_control.h"
#include "rr_sys_info_ncell.h"
#include "rr_sys_info_si.h"
#include "rr_process_pmo_pcco.h"
#include "ms.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_general.h"
#include "rr_mac_send.h"
#include "rr_mm_send.h"
#include "gmacgrrsig.h"
#include "mac_grr.h"
#include "rr_gprs_defs.h"
#include "rr_sys_info_3g.h"
#include "rr_control.h"
#include "rr_resel_calcs.h"
#include "rr_l1_send.h"
#include "rr_resel.h"
#include "rr_resel_utils.h"
#include <stdlib.h>    /* for qsort function */
#include "rr_ps_domain.h"
#include "rr_multi_sim.h" 
#include "gprs_mem.h"
#include "rr_process_pmo_pcco.h"
#include "rr_defs.h"
#include "stringl/stringl.h"
#ifdef FEATURE_IDLE_DRX_SCALING
#include "rr_idle_drx.h"
#endif /* FEATURE_IDLE_DRX_SCALING */


/*===========================================================================
                         LOCAL DATA DECLARATIONS/TYPES
===========================================================================*/

#define NC_MEAS_REP_PERIOD_MS_SIZE 8
#define NC_MEAS_NUM_PAGING_CYCLES  8
#define EARFCN_INVALID_VAL         0xFFFF

typedef enum
{
  t3158_restarted,
  t3158_allow_to_run
} t3158_action_t;

typedef enum
{
  DSF_CAUSE,
/* TODO:  CELL_RESELECTION_CAUSE, */
/* TODO:  PCCO_CELL_RESELECTION_CAUSE, */
  GMM_STANDBY_CAUSE,
  PMO_RESET_CAUSE,
  OLD_SCELL_CAUSE

}cause_t;

typedef enum
{
  send_to_mac,
  send_to_mac_forbidden

} pmr_send_t;

/*Structure to hold the recieved nc measurement data*/
typedef struct {
    ARFCN_T bcch_arfcn;
    uint16 rxlev_total;
    uint16 rxlev_count;
    boolean     valid_info;
    BSIC_T      bsic;
}nc_measurement_element_t;

typedef struct {

  /* ncell info*/
  uint32 no_of_ncell_entries;
  nc_measurement_element_t nc_measurements[BA_MAX_LEN];
  /* scell info */
  ARFCN_T scell_arfcn;
  uint16 scell_rxlev_total;
  uint16 scell_rxlev_count;
  uint16 scell_ilevel_total;
  uint16 scell_ilevel_count;

}nc_measurement_data_t;

/* structure to hold the measurement report if MAC is in the wrong mode upon T3158 expiry */
typedef struct
{
  boolean       valid;
  pmr_or_pemr_e msg_type;
  pmr_or_pemr_u msg;
  boolean       standby_before_cnf;
} pending_pmr_store_t;


/*This reporting period is used to see when to try to hold onto meas reports*/
/*to be sent when MAC returns to the IDLE or TRANSFER states - see in code*/
#define THRESHOLD_REPORTING_PERIOD 5
/* Extract the NCC from a pointer to the SCH data block */
#define DECODE_NCC_DIRECT(sch_block)  (byte)((((sch_block[0] & 0xfc) >> 2) & 0x38) >> 3)
/* Extract the BCC from a pointer to the SCH data block */
#define DECODE_BCC_DIRECT(sch_block)  (byte)(((sch_block[0] & 0xfc) >> 2) & 0x07)

#define MAX_GRR_3G_FDD_MEASUREMENTS 6

#ifdef FEATURE_GSM_TDS
#define MAX_GRR_3G_TDD_MEASUREMENTS 6
#endif /*FEATURE_GSM_TDS*/

typedef struct
{
  uint16 uarfcn;
  uint16 scrambling_code;
  int16 rscp_total;
  int16 rscp_average;
  int16 ecio_total;
  int16 ecio_average;
  int16 rssi_total;
  int16 rssi_average;
  uint16 noof_measurements;

} fdd_measurement_t;

#ifdef FEATURE_GSM_TDS
typedef struct
{
  uint16 uarfcn;
  uint16 cpid;
  int16  rscp_total;
  int16  rscp_average;
  uint16 noof_measurements;
} tdd_measurement_t;
#endif /*FEATURE_GSM_TDS*/

typedef struct
{
  uint8 fdd_count;
  fdd_measurement_t fdd_entries[MAX_GRR_3G_FDD_MEASUREMENTS];

#ifdef FEATURE_GSM_TDS
  uint8 tdd_count;
  tdd_measurement_t tdd_entries[MAX_GRR_3G_TDD_MEASUREMENTS];
#endif /*FEATURE_GSM_TDS*/
} grr_3g_measurements_t;

typedef struct
{
  nc_meas_state_T       state;
  nc_meas_state_T       old_state;
  nc_measurement_data_t nc_measurement_data;

  /*meas_rpt_sent_in_txfer flag is used to remember whether we sent atleast one PMR successfully in transfer*/
  /*This will be used to decide whether we need to restart the T3158 timer with idle value or not*/
  /*when we move back to idle mode*/
  boolean               meas_rpt_sent_in_txfer;
  pending_pmr_store_t   pending_pmr_store;

  nc_measurement_element_t *pcs_cells[6];
  nc_measurement_element_t *dcs_cells[6];
  nc_measurement_element_t *pgsm_cells[6];
  nc_measurement_element_t *egsm_cells[6];
  nc_measurement_element_t *g850_cells[6];

  grr_3g_measurements_t grr_3g_measurements;

  pmr_or_pemr_u                         msg_pemr;
  packet_enhanced_measurement_report_t *pemr;
  pmr_or_pemr_u                         msg_pmr;
} rr_gprs_meas_rpt_control_data_t;


/*===========================================================================
                         LOCAL FUNCTIONS
===========================================================================*/
static rr_event_T rr_gprs_read_meas_rpt_sysinfo(const gas_id_t gas_id);
static rr_event_T rr_gprs_process_nc_measurement_params(const gas_id_t gas_id);
static boolean rr_gprs_format_meas_report(pmr_send_t, const gas_id_t gas_id);
static void rr_gprs_start_nc_measurement_process(boolean, const gas_id_t gas_id);
static void rr_gprs_store_nc_measurements(mph_nc_measurement_ind_t *nc_measurements, const gas_id_t gas_id);
static void rr_gprs_store_serving_measurements(mph_serving_meas_ind_t *serving_measurements, const gas_id_t gas_id);
static void rr_gprs_stop_nc_measurement_process(const gas_id_t gas_id);
static rr_event_T rr_gprs_change_nc_measurement_process(cause_t, const gas_id_t gas_id);
static t3158_action_t rr_gprs_start_t3158(boolean mac_status_changed, const gas_id_t gas_id);
static void rr_gprs_meas_rpt_control_force_t3158_restart(const gas_id_t gas_id);
static void rr_gprs_load_best_six_for_each_band(nc_measurement_data_t *, const gas_id_t gas_id);
static void rr_gprs_insert_meas_entry(nc_measurement_element_t *, nc_measurement_element_t **);

static void rr_nc_meas_params_merge_mods(nc_meas_params_mods_t *dst, nc_meas_params_mods_t *mods);
static void rr_gprs_meas_params_merge_mods(gprs_meas_params_mods_t *dst, gprs_meas_params_mods_t *mods);

#ifdef FEATURE_WCDMA
static void rr_gprs_3g_meas_params_merge_mods(gprs_3g_meas_params_mods_t *dst, gprs_3g_meas_params_mods_t *mods);
static void rr_gprs_store_3g_measurements_idle(mph_surround_wcdma_idle_meas_ind_T * meas, const gas_id_t gas_id);
static void rr_gprs_store_3g_measurements_xfer(mph_surround_wcdma_xfer_meas_ind_T * meas, const gas_id_t gas_id);
static void rr_gprs_3g_measurements_sort(const gas_id_t gas_id);
static uint32 grr_add_fdd_cells_to_pemr(packet_enhanced_measurement_report_t *pemr,
                                        const gprs_scell_info_t *scell_ptr,
                                        const gas_id_t gas_id);
#endif /* FEATURE_WCDMA */

static void rr_gprs_nacc_params_merge_mods( pmo_pcco_nacc_params_t *dst , pmo_pcco_nacc_params_t *src );

#ifdef FEATURE_GSM_TDS
static void rr_gprs_tdd_3g_measurements_sort(const gas_id_t gas_id);
static uint32 grr_add_tdd_cells_to_pemr(packet_enhanced_measurement_report_t *pemr,
                                        const gprs_scell_info_t *scell_ptr,
                                        const gas_id_t gas_id);
#endif /*FEATURE_GSM_TDS*/

/*===========================================================================
                         LOCAL VARIABLES
===========================================================================*/
static rr_gprs_meas_rpt_control_data_t  rr_gprs_meas_rpt_control_data[NUM_GERAN_DATA_SPACES];

static const uint16 nc_meas_rep_period_ms[NC_MEAS_REP_PERIOD_MS_SIZE] = {480, 960, 1920, 3840, 7680, 15360, 30720, 61440 };
static const uint16 paging_cycle_lengths[NC_MEAS_NUM_PAGING_CYCLES]   = {471, 706,  942, 1177, 1412, 1648,   1883,  2118};

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_gprs_meas_rpt_control_data_t*
 */
static rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_gprs_meas_rpt_control_data[as_index];
  }


static void rr_gsm_priority_params_merge_mods(
  csn_gsm_priority_params_ie_t * dst,
  csn_gsm_priority_params_ie_t * mods
)
{
  RR_NULL_CHECK_RETURN_VOID(dst);
  RR_NULL_CHECK_RETURN_VOID(mods);

  /* No merging to do, just overwrite the destination with the
  modifications. */
  *dst = *mods;
}


static void rr_gprs_meas_rpt_control_reset_measurements(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  MSG_GERAN_MED_0_G("Resetting NC / 3G measurement data");
  memset(&rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data, 0 , sizeof(nc_measurement_data_t));
#ifdef FEATURE_WCDMA
  memset(&rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements, 0, sizeof(grr_3g_measurements_t));
#endif /* FEATURE_WCDMA */
}

static boolean grr_nc_params_set_by_pmo_pcco(const gas_id_t gas_id)
{

  rr_params_mods_t ** params_mods = rr_params_mods(gas_id);

  if (params_mods != NULL && *params_mods != NULL)
  {
    return (*params_mods)->nc_meas_params.valid;
  }
  else
  {
    return FALSE;
  }

}

byte rr_gprs_get_network_control_order(const gas_id_t gas_id)
{
  gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

  return scell->nc_meas_params.network_control_order;
}

/**
  @brief Check whether the provided network control order is valid.

  Determines whether the provided network control order is valid.  Valid values
  are NC_MEAS_NC0, NC_MEAS_NC1 and NC_MEAS_NC2.

  @return TRUE if the specified network control order is valid, FALSE otherwise.
*/
static boolean rr_network_control_order_valid(uint8 network_control_order)
{
  switch (network_control_order)
  {
    case NC_MEAS_NC0:
    case NC_MEAS_NC1:
    case NC_MEAS_NC2:
    {
      return TRUE;
    }

    default:
    {
      return FALSE;
    }
  }
}


/**
  @brief Reset the current network control order to NC0.

  This function should be used if an invalid network control order is detected.
  It will reset the network control order to zero.
*/
static void rr_network_control_order_reset(const gas_id_t gas_id)
{
  gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_RETURN_VOID(scell);

  MSG_GERAN_HIGH_0_G("Resetting network_control_order to 0");
  scell->nc_meas_params.network_control_order = NC_MEAS_NC0;
}

/* Enhancement for implicit NACC rejection via PCCO
 * Remove the old NACC target from the best six so that it
 * will not be included in the pseudo BA list on the new cell
 * Note that this only affects NC1 mode
 */
void rr_gprs_nacc_check_pcco_reject(
  ARFCN_T pcco_arfcn,
  ARFCN_T nacc_arfcn,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

  /* check whether the PCCO cell is the same as our NACC target */
  if (!ARFCNS_EQUAL(pcco_arfcn, nacc_arfcn))
  {
    byte nc_mode = rr_gprs_get_network_control_order(gas_id);

    MSG_GERAN_HIGH_3_G("PCCO target %d differs from NACC target %d (scell %d)",
             (int)pcco_arfcn.num,
             (int)nacc_arfcn.num,
             (int)scell_info_ptr->gsm.BCCH_ARFCN.num);

    if ((nc_mode == NC_MEAS_NC1) && (best_six_ncells_ptr->no_of_entries > 1))
    {
      bcch_ncell_info_t *ncell_db_ptr = rr_gprs_get_surround_ncell_sys_info(nacc_arfcn, gas_id);

      if (ncell_db_ptr != NULL)
      {
        MSG_GERAN_HIGH_0_G("Implicit NACC reject in NC1 mode");
        rr_gprs_remove_from_best_six(
          ncell_db_ptr->gsm.BCCH_ARFCN,
          ncell_db_ptr->gsm.BSIC,
          gas_id
        );
      }
      else
      {
        MSG_GERAN_ERROR_0_G("NACC target not in surround cell database");
      }
    }
    else
    {
      MSG_GERAN_HIGH_2_G("Avoid NACC reject optimisation NC=%1d, best six=%d",
               (int)nc_mode,(int)best_six_ncells_ptr->no_of_entries);
    }
  }

  return;
}

static byte rr_gprs_get_nc_non_drx_period(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  return scell_info_ptr->nc_meas_params.nc_non_drx_period;
}

static byte rr_gprs_get_nc_reporting_period_i(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  uint8 nc_reporting_period_i = scell_info_ptr->nc_meas_params.nc_reporting_period_i;

  if (nc_reporting_period_i < NC_MEAS_REP_PERIOD_MS_SIZE)
  {
    return nc_reporting_period_i;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("nc_reporting_period_i out of range, returning 0");
    return 0;
  }
}

static byte rr_gprs_get_nc_reporting_period_t(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  uint8              nc_reporting_period_t;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  /* check for uninitialised (or out of range) values */
  nc_reporting_period_t = scell_info_ptr->nc_meas_params.nc_reporting_period_t;
  if (nc_reporting_period_t >= MAX_NC_REPORTING_PERIOD_INDEX)
  {
    /* update the serving cell store to the default mandated by 3GPP 44.018 (3.84 seconds) */
    scell_info_ptr->nc_meas_params.nc_reporting_period_t = DEFAULT_NC_REPORTING_PERIOD_T;

    /* check whether default parameters are in effect because SI2q has not been received */
    if ((rr_gprs_meas_rpt_control_data_ptr->state == NC_MEAS_NULL)            &&
        (scell_info_ptr->nc_meas_params.network_control_order == NC_MEAS_NC2) &&
        (scell_info_ptr->gsm.two_quater_position != SI2Q_POSITION_NONE))
  {
      /* See 3GPP 45.008, para 10.1.4.1, "Measurement reporting" */
      /* "The start of the first reporting period may be random" */
      /* This adjustment caters for the initial reporting cycle, */
      /* which relies on SI2q, GMM ready state and transfer mode */
      nc_reporting_period_t = INITIAL_NC_REPORTING_PERIOD_T;
      MSG_GERAN_HIGH_1_G("Using initial NC_REPORTING_PERIOD_T=%d", (int)nc_reporting_period_t);
  }
  else
  {
      nc_reporting_period_t = DEFAULT_NC_REPORTING_PERIOD_T;
    }
  }

  return(nc_reporting_period_t);
}


static void rr_params_mods_dump(rr_params_mods_t * params_mods,const gas_id_t gas_id)
{
  RR_NULL_CHECK_RETURN_VOID(params_mods);

  MSG_GERAN_MED_0_G("RR parameter modifications:");
  if (params_mods->nc_meas_params.valid)
  {
    MSG_GERAN_MED_0_G("  NC Measurement Parameters:");
    MSG_GERAN_MED_1_G("    network_control_order: %d", params_mods->nc_meas_params.network_control_order);
    if (params_mods->nc_meas_params.nc_period_valid)
    {
      MSG_GERAN_MED_1_G("    nc_non_drx_period: %d", params_mods->nc_meas_params.nc_non_drx_period);
      MSG_GERAN_MED_1_G("    nc_reporting_period_i: %d", params_mods->nc_meas_params.nc_reporting_period_i);
      MSG_GERAN_MED_1_G("    nc_reporting_period_t: %d", params_mods->nc_meas_params.nc_reporting_period_i);
    }
  }
  else
  {
    MSG_GERAN_MED_0_G("  NC Measurement Parameters: not present");
  }

  if (params_mods->serving_cell_priority_params_valid)
  {
    MSG_GERAN_MED_0_G("  Serving Cell Priority Parameters:");
    MSG_GERAN_MED_3_G("    geran_priority: %d, thresh_priority_search: %d, thresh_gsm_low: %d",
                      params_mods->serving_cell_priority_params.geran_priority,
                      params_mods->serving_cell_priority_params.thresh_priority_search,
                      params_mods->serving_cell_priority_params.thresh_gsm_low);
    MSG_GERAN_MED_2_G("    h_prio: %d, t_reselection: %d",
                      params_mods->serving_cell_priority_params.h_prio,
                      params_mods->serving_cell_priority_params.t_reselection);
  }
  else
  {
    MSG_GERAN_MED_0_G("  Serving Cell Priority Parameters: not present");
  }

  if (params_mods->utran_priority_params_valid)
  {
    uint32 i;

    MSG_GERAN_MED_0_G("  UTRAN Priority Parameters:");
    if (params_mods->utran_priority_params.default_params_valid)
    {
      MSG_GERAN_MED_0_G("    Default Parameters:");
      MSG_GERAN_MED_3_G("      priority: %d, thresh: %d, qrxlevmin: %d",
                        params_mods->utran_priority_params.default_params.priority,
                        params_mods->utran_priority_params.default_params.thresh,
                        params_mods->utran_priority_params.default_params.qrxlevmin);
    }
    else
    {
      MSG_GERAN_MED_0_G("    Default Parameters: not present");
    }
    for (i = 0;
         i < params_mods->utran_priority_params.utran_priority_params_count &&
         i < CSN_UTRAN_PRIORITY_PARAMS_DESC_MAX;
         ++i)
    {
      csn_utran_priority_params_t * item = &params_mods->utran_priority_params.utran_priority_params[i];

      MSG_GERAN_MED_0_G("    Repeated UTRAN Priority Parameters:");
      MSG_GERAN_MED_2_G("      [%02d]: frequency_index_bitmap: %#04x", i, item->frequency_index_bitmap);
      if (item->priority_valid)
      {
        MSG_GERAN_MED_1_G("            priority: %d", item->priority);
      }
      else
      {
        MSG_GERAN_MED_0_G("            priority: not present");
      }
      MSG_GERAN_MED_1_G("            thresh_high: %d", item->thresh_high);
      if (item->thresh_low_valid)
      {
        MSG_GERAN_MED_1_G("            thresh_low: %d", item->thresh_low);
      }
      else
      {
        MSG_GERAN_MED_0_G("            thresh_low: not present");
      }
      if (item->qrxlevmin_valid)
      {
        MSG_GERAN_MED_1_G("            qrxlevmin: %d", item->qrxlevmin);
      }
      else
      {
        MSG_GERAN_MED_0_G("            qrxlevmin: not present");
      }
    }
  }
  else
  {
    MSG_GERAN_MED_0_G("  UTRAN Priority Parameters: not present");
  }

  if (params_mods->eutran_params_valid)
  {
    uint32 i;

    MSG_GERAN_MED_0_G("  E-UTRAN Priority Parameters:");
    MSG_GERAN_MED_1_G("    E-UTRAN_CCN_ACTIVE: %d", params_mods->eutran_params.ccn_active);
    if (params_mods->eutran_params.gprs_meas_params_valid)
    {
      MSG_GERAN_MED_0_G("    GPRS E-UTRAN Measurement Parameters:");
      MSG_GERAN_MED_3_G("      Qsearch_P: %d, REP_QUANT: %d, MULTIRAT_REPORTING: %d",
                        params_mods->eutran_params.gprs_meas_params.qsearch_p,
                        params_mods->eutran_params.gprs_meas_params.rep_quant,
                        params_mods->eutran_params.gprs_meas_params.multirat_reporting);
      if (params_mods->eutran_params.gprs_meas_params.fdd_valid)
      {
        MSG_GERAN_MED_0_G("      FDD Parameters:");
        MSG_GERAN_MED_1_G("        fdd_reporting_threshold: %d", params_mods->eutran_params.gprs_meas_params.fdd_valid);
        MSG_GERAN_MED_1_G("        fdd_reporting_threshold_2_valid: %d", params_mods->eutran_params.gprs_meas_params.fdd_reporting_threshold_2_valid);
        MSG_GERAN_MED_1_G("        fdd_reporting_threshold_2: %d", params_mods->eutran_params.gprs_meas_params.fdd_reporting_threshold_2);
        MSG_GERAN_MED_1_G("        fdd_reporting_offset_valid: %d", params_mods->eutran_params.gprs_meas_params.fdd_reporting_offset_valid);
        MSG_GERAN_MED_1_G("        fdd_reporting_offset: %d", params_mods->eutran_params.gprs_meas_params.fdd_reporting_offset);
      }
      else
      {
        MSG_GERAN_MED_0_G("      FDD Parameters: not present");
      }
      if (params_mods->eutran_params.gprs_meas_params.tdd_valid)
      {
        MSG_GERAN_MED_0_G("      TDD Parameters:");
        MSG_GERAN_MED_1_G("        tdd_reporting_threshold: %d", params_mods->eutran_params.gprs_meas_params.tdd_valid);
        MSG_GERAN_MED_1_G("        tdd_reporting_threshold_2_valid: %d", params_mods->eutran_params.gprs_meas_params.tdd_reporting_threshold_2_valid);
        MSG_GERAN_MED_1_G("        tdd_reporting_threshold_2: %d", params_mods->eutran_params.gprs_meas_params.tdd_reporting_threshold_2);
        MSG_GERAN_MED_1_G("        tdd_reporting_offset_valid: %d", params_mods->eutran_params.gprs_meas_params.tdd_reporting_offset_valid);
        MSG_GERAN_MED_1_G("        tdd_reporting_offset: %d", params_mods->eutran_params.gprs_meas_params.tdd_reporting_offset);
      }
      else
      {
        MSG_GERAN_MED_0_G("      TDD Parameters: not present");
      }
    }
    else
    {
      MSG_GERAN_MED_0_G("    GPRS E-UTRAN Measurement Parameters: not present");
    }

    for (i = 0;
         i < params_mods->eutran_params.neighbour_cells_count &&
         i < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS;
         ++i)
    {
      csn_eutran_neighbour_cells_t * item = &params_mods->eutran_params.neighbour_cells[i];
      uint32                         j;

      MSG_GERAN_MED_0_G("    Repeated E-UTRAN Neighbour Cells:");
      MSG_GERAN_MED_1_G("    [%02d]:", i);

      for (j = 0;
           j < item->earfcn_count &&
           j < CSN_EUTRAN_NEIGHBOUR_CELLS_MAX_EARFCNS;
           ++j)
      {
        MSG_GERAN_MED_2_G("      EARFCN: %d, Measurement Bandwidth: %d", item->earfcns[j].earfcn, item->earfcns[j].measurement_bandwidth);
      }

      if (item->priority_valid)
      {
        MSG_GERAN_MED_1_G("      priority: %d", item->priority);
      }
      else
      {
        MSG_GERAN_MED_0_G("      priority: not present");
      }
      MSG_GERAN_MED_1_G("      thresh_high: %d", item->thresh_high);
      if (item->thresh_low_valid)
      {
        MSG_GERAN_MED_1_G("      thresh_low: %d", item->thresh_low);
      }
      else
      {
        MSG_GERAN_MED_0_G("      thresh_low: not present");
      }
      if (item->qrxlevmin_valid)
      {
        MSG_GERAN_MED_1_G("      qrxlevmin: %d", item->qrxlevmin);
      }
      else
      {
        MSG_GERAN_MED_0_G("      qrxlevmin: not present");
      }
    }
  }
  else
  {
    MSG_GERAN_MED_0_G("  E-UTRAN Priority Parameters: not present");
  }
}

/*=============================================================================

  FUNCTION rr_gprs_meas_rpt_control_handle_pmo_message

  DESCRIPTION
    Updates NC Measurement Parameters when it is indicated that a PMO (or set
    of) has been received.

  PARAMETERS
    None

  RETURNS
    rr_event_T RR_EV_MEASUREMENT_PARAMS_UPDATED if NC Measurement Params have
    been modified by the PMO, EV_NO_EVENT if not.

=============================================================================*/
static rr_event_T rr_gprs_meas_rpt_control_handle_pmo_message(const gas_id_t gas_id)
{
  rr_event_T                        output_event;
  rr_params_mods_t               ** params_mods = rr_params_mods(gas_id);
  csn_gsm_priority_params_ie_t      serving_cell_priority_params;
  csn_utran_priority_params_ie_t    utran_priority_params;
  csn_eutran_params_ie_t            eutran_params;

  RR_NULL_CHECK_RETURN_PARAM(params_mods, EV_NO_EVENT);

  memset(&serving_cell_priority_params, 0, sizeof(csn_gsm_priority_params_ie_t));
  memset(&utran_priority_params, 0, sizeof(csn_utran_priority_params_ie_t));
  memset(&eutran_params, 0, sizeof(csn_eutran_params_ie_t));

  if (*params_mods != NULL)
  {
    /* There are already modifications to the broadcast system information,
    so we just merge in the new values. */
  }
  else
  {
    /* Allocate some memory for the global RR parameters modifications
    store. */
    *params_mods = GPRS_MEM_CALLOC(sizeof(rr_params_mods_t));
    RR_NULL_CHECK_RETURN_PARAM(*params_mods, EV_NO_EVENT);
  }

  MSG_GERAN_HIGH_0_G("RR parameter modifications before PMO:");
  rr_params_mods_dump(*params_mods,gas_id);

  /* merge in any changes from the PMO to our current set of modifications */
  rr_nc_meas_params_merge_mods(&(*params_mods)->nc_meas_params, rr_gprs_get_pmo_nc_meas_params(gas_id));
  rr_gprs_meas_params_merge_mods(&(*params_mods)->gprs_meas_params, rr_gprs_get_pmo_gprs_meas_params(gas_id));

  rr_gprs_nacc_params_merge_mods(&(*params_mods)->nacc_params, rr_gprs_get_pmo_nacc_params(gas_id));

#ifdef FEATURE_WCDMA
  rr_gprs_3g_meas_params_merge_mods(&(*params_mods)->gprs_3g_meas_params, rr_gprs_get_pmo_gprs_3g_meas_params(gas_id));
#endif /* FEATURE_WCDMA */

  if (rr_get_pmo_priority_and_eutran_params_valid(gas_id))
  {
    if (rr_get_pmo_serving_cell_priority_params(&serving_cell_priority_params,gas_id))
    {
      (*params_mods)->serving_cell_priority_params_valid = TRUE;
      rr_gsm_priority_params_merge_mods(
        &(*params_mods)->serving_cell_priority_params,
        &serving_cell_priority_params
      );
    }
    else
    {
      (*params_mods)->serving_cell_priority_params_valid = FALSE;
    }

    if (rr_get_pmo_utran_priority_params(&utran_priority_params,gas_id))
    {
      (*params_mods)->utran_priority_params_valid = TRUE;
      rr_utran_priority_params_merge_mods(
        &(*params_mods)->utran_priority_params,
        &utran_priority_params
      );
    }
    else
    {
      (*params_mods)->utran_priority_params_valid = FALSE;
    }

    if (rr_get_pmo_eutran_params(&eutran_params,gas_id))
    {
      (*params_mods)->eutran_params_valid = TRUE;
      csn_eutran_params_merge_csn_mods(
        &(*params_mods)->eutran_params,
        &eutran_params
      );
    }
    else
    {
      (*params_mods)->eutran_params_valid = FALSE;
    }
  }

  /* rebuild the global measurement params from system information, modifications and
  temporary PCCO modifications */
  rr_gprs_update_meas_params(gas_id);

  /* set our NC mode based on the new global measurement params */
  output_event = rr_gprs_process_nc_measurement_params(gas_id);

  /* clear the PMO accumulated data store in rr_sys_info_psi.c ready for the next set */
  rr_psi_discard_pmo_data(gas_id);

  MSG_GERAN_HIGH_0_G("RR parameter modifications after PMO:");
  rr_params_mods_dump(*params_mods,gas_id); 

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  if (!rr_gsm_only(gas_id))
  {
    rr_l1_update_wcdma_neighbor_list(gas_id);
  }
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

  return output_event;
}

/*=============================================================================

  FUNCTION rr_gprs_meas_rpt_control_handle_pcco_message

  DESCRIPTION
    Stores parameters received in a PCCO for use after camping on the target
    cell.

  PARAMETERS
    None

  RETURNS
    EV_NO_EVENT

=============================================================================*/
static rr_event_T rr_gprs_meas_rpt_control_handle_pcco_message(const gas_id_t gas_id)
{
 

  rr_event_T          output_event = EV_NO_EVENT;
  rr_params_mods_t ** pcco_params_mods = rr_pcco_params_mods(gas_id);

  RR_NULL_CHECK_RETURN_PARAM(pcco_params_mods, EV_NO_EVENT);

  if (*pcco_params_mods != NULL)
  {
    memset(*pcco_params_mods, 0, sizeof(rr_params_mods_t));
  }
  else
  {
    *pcco_params_mods = GPRS_MEM_CALLOC(sizeof(rr_params_mods_t));
    RR_NULL_CHECK_RETURN_PARAM(*pcco_params_mods, EV_NO_EVENT);
  }

  rr_nc_meas_params_merge_mods(&(*pcco_params_mods)->nc_meas_params, rr_gprs_get_pcco_nc_meas_params(gas_id));
  rr_gprs_meas_params_merge_mods(&(*pcco_params_mods)->gprs_meas_params, rr_gprs_get_pcco_gprs_meas_params(gas_id));

  rr_gprs_nacc_params_merge_mods(&(*pcco_params_mods)->nacc_params, rr_gprs_get_pcco_nacc_params(gas_id));

#ifdef FEATURE_WCDMA
  rr_gprs_3g_meas_params_merge_mods(&(*pcco_params_mods)->gprs_3g_meas_params, rr_gprs_get_pcco_gprs_3g_meas_params(gas_id));
#endif /* FEATURE_WCDMA */

  /* clear the PCCO data store in rr_sys_info_psi.c ready for the one */
  rr_psi_discard_pcco_data(gas_id);

  return output_event;
}


void rr_gprs_meas_rpt_control_handle_l2g_cco_network_control_order(
  uint8 network_control_order,
  const gas_id_t gas_id
)
{
  rr_params_mods_t ** pcco_params_mods = rr_pcco_params_mods(gas_id);

  MSG_GERAN_HIGH_1_G("Received L2G CCO NCO value %d", network_control_order);

  RR_NULL_CHECK_RETURN_VOID(pcco_params_mods);

  if (*pcco_params_mods != NULL)
  {
    memset(*pcco_params_mods, 0, sizeof(rr_params_mods_t));
  }
  else
  {
    *pcco_params_mods = GPRS_MEM_CALLOC(sizeof(rr_params_mods_t));
    RR_NULL_CHECK_RETURN_VOID(*pcco_params_mods);
  }

  (*pcco_params_mods)->nc_meas_params.valid = TRUE;
  (*pcco_params_mods)->nc_meas_params.network_control_order = network_control_order;
}

/*=============================================================================

  FUNCTION rr_gprs_meas_rpt_control_handle_mac_grr_cell_resel_status

  DESCRIPTION
    Takes action when a grr_mac_cell_resel_status is received.  If this a
    successful PCCO completion, merges PCCO parameter modifications in to
    current set of modifications.  If it indicates a failed PCCO attempt, clear
    the PCCO parameter modifications.

  PARAMETERS
    None

  RETURNS
    EV_NO_EVENT

=============================================================================*/
static void rr_gprs_meas_rpt_control_handle_mac_grr_cell_resel_status(
  pcco_status_t status,
  const gas_id_t gas_id
)
{
  rr_params_mods_t ** params_mods = rr_params_mods(gas_id);
  rr_params_mods_t ** pcco_params_mods = rr_pcco_params_mods(gas_id);

  RR_NULL_CHECK_RETURN_VOID(params_mods);
  RR_NULL_CHECK_RETURN_VOID(pcco_params_mods);

  if (*params_mods != NULL)
  {
    MSG_GERAN_ERROR_0_G("Old parameter modifications still present, clearing");
    memset(*params_mods, 0, sizeof(rr_params_mods_t));
  }
  else
  {
    *params_mods = GPRS_MEM_CALLOC(sizeof(rr_params_mods_t));
    RR_NULL_CHECK_RETURN_VOID(params_mods);
  }

  if (*pcco_params_mods != NULL)
  {
    if (status == PCCO_SUCCESS)
    {
      /* need to merge the PCCO modifications in to the stored modifications */
      rr_nc_meas_params_merge_mods(&(*params_mods)->nc_meas_params, &(*pcco_params_mods)->nc_meas_params);
      rr_gprs_meas_params_merge_mods(&(*params_mods)->gprs_meas_params, &(*pcco_params_mods)->gprs_meas_params);
#ifdef FEATURE_WCDMA
      rr_gprs_3g_meas_params_merge_mods(&(*params_mods)->gprs_3g_meas_params, &(*pcco_params_mods)->gprs_3g_meas_params);
#endif /* FEATURE_WCDMA */
      rr_gprs_nacc_params_merge_mods(&(*params_mods)->nacc_params, &(*pcco_params_mods)->nacc_params);
    }

    /* PCCO data is now finished with, so can be freed. */
    GPRS_MEM_FREE(*pcco_params_mods);
    *pcco_params_mods = NULL;
  }
}

/****************************************************************************
 *   Function name:  rr_gprs_get_nc_mode
 *   --------------
 *
 *   Description: This function is an access function to the nc mode in operation
 *   ------------
 *
 *
 *   Parameters: none
 *   -----------
 *
 *
 *   Returns:  NC_MEAS_NC0, NC_MEAS_NC1, NC_MEAS_NC2, NC_MEAS_RESET
 *   --------
 *
 *
 ****************************************************************************/
uint8 rr_gprs_get_nc_mode(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);
  uint8              output_nc_mode;

  switch (scell->nc_meas_params.network_control_order)
  {
  case NC_MEAS_NC0:
    {
      output_nc_mode = NC_MEAS_NC0;
    }
    break;

  case NC_MEAS_NC1:
    {
      if (rr_gmminfo_get_gmm_status(gas_id) == GMM_READY_STATE)
      {
        output_nc_mode = NC_MEAS_NC1;
      }
      else
      {
        output_nc_mode = NC_MEAS_NC0;
      }
    }
    break;

  case NC_MEAS_NC2:
    {
      if (rr_gmminfo_get_gmm_status(gas_id) == GMM_READY_STATE)
      {
        output_nc_mode = NC_MEAS_NC2;
      }
      else
      {
        output_nc_mode = NC_MEAS_NC0;
      }
    }
    break;

  case NC_MEAS_RESET:
    {
      output_nc_mode = NC_MEAS_RESET;
    }
    break;

  default:
    MSG_GERAN_ERROR_0_G("Unknown nc mode: %d");
    output_nc_mode = NC_MEAS_NC0;
  }
  return output_nc_mode;
}

static void rr_gprs_meas_rpt_control_reset_modifications(
  const gas_id_t gas_id
)
{
#ifdef FEATURE_WCDMA
  rr_sys_info_3g_data_t *rr_sys_info_3g_data_ptr = rr_get_sys_info_3g_data_ptr(gas_id);
#endif /* FEATURE_WCDMA */
  rr_params_mods_t      ** params_mods = rr_params_mods(gas_id);
  rr_params_mods_t      ** pcco_params_mods = rr_pcco_params_mods(gas_id);

  MSG_GERAN_MED_0_G("Resetting all stored measurement parameter modifications");

  if (params_mods != NULL && *params_mods != NULL)
  {
    GPRS_MEM_FREE(*params_mods);
    *params_mods = NULL;
  }

  if (pcco_params_mods != NULL && *pcco_params_mods != NULL)
  {
    GPRS_MEM_FREE(*pcco_params_mods);
    *pcco_params_mods = NULL;
  }

#ifdef FEATURE_WCDMA
  /* Reset the 3G nlist to the broadcast list in case it's been changed */
  rr_sys_info_3g_data_ptr->grr_3g_nlist = &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast;
#endif /* FEATURE_WCDMA */
}

static void rr_gprs_meas_rpt_control_reset(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_ptr = rr_gprs_get_scell_info(gas_id);

  /* clear pcco/pmo modifications to meas params */
  rr_gprs_meas_rpt_control_reset_modifications(gas_id);

  if (rr_gprs_camped_on_ccch(gas_id) || rr_gsm_camped_on_ccch(gas_id))
  {
    MSG_GERAN_HIGH_0_G("Reverting to BCCH NC meas params and BA");
    rr_gprs_revert_to_broadcast_meas_params_51(scell_ptr, &scell_ptr->gsm, gas_id);
    rr_gprs_revert_ba_gprs(gas_id);
  }
  else
  {
    /* clear the current parameters (setting to defaults other than 0 where appropriate) */
    nc_measurement_params_t *nc_meas_params      = &scell_ptr->nc_meas_params;
    gprs_meas_params_t      *gprs_meas_params    = &scell_ptr->gprs_meas_params;
    gprs_3g_meas_params_t   *gprs_3g_meas_params = &scell_ptr->gprs_3g_meas_params;

    MSG_GERAN_HIGH_0_G("Resetting NC Measurement Control");

    memset(nc_meas_params, 0, sizeof(nc_measurement_params_t));
    nc_meas_params->nc_reporting_period_i = DEFAULT_NC_REPORTING_PERIOD_I;
    nc_meas_params->nc_reporting_period_t = DEFAULT_NC_REPORTING_PERIOD_T;
//was    nc_meas_params->nc_reporting_period_t = MAX_NC_REPORTING_PERIOD_INDEX;
    nc_meas_params->nc_non_drx_period     = DEFAULT_NON_DRX_PERIOD;

    memset(gprs_meas_params, 0, sizeof(gprs_meas_params_t));
    memset(gprs_3g_meas_params, 0, sizeof(gprs_3g_meas_params_t));
    gprs_3g_meas_params->FDD_GPRS_Qoffset = DEFAULT_FDD_GPRS_QOFFSET;
    gprs_3g_meas_params->FDD_Qmin         = DEFAULT_FDD_QMIN;
    gprs_3g_meas_params->Qsearch_P        = DEFAULT_QSEARCH_P;
    gprs_3g_meas_params->TDD_GPRS_Qoffset = DEFAULT_TDD_GPRS_QOFFSET;
  }
}

/****************************************************************************
 *   Function name:  rr_gprs_meas_report_can_proceed
 *   --------------
 *
 *   Description:    Checks whether a PMR can be sent
 *   ------------
 *
 *
 *   Parameters:     None
 *   -----------
 *
 *
 *   Returns:        TRUE if normal PMR procedures allowed, FALSE otherwise
 *   --------
 *
 *
 ****************************************************************************/
static boolean rr_gprs_meas_report_can_proceed(
  const gas_id_t gas_id
)
{
  boolean proceed;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  if (rr_is_control_state(RR_GRR, gas_id))
  {
    if (rr_mac_ps_access_blocked(gas_id))
    {
      MSG_GERAN_HIGH_0_G("PMR hold off: MAC suspended");
      proceed = FALSE;
     (void)rr_gprs_start_t3158(FALSE, gas_id);
    }
    else
    {
      /* allow measurement report to go out as normal */
      proceed = TRUE;
    }
  }
  else
  {
    /* RR currently has control, so determine the exact scenario */
    proceed = FALSE;

    /* Camped on CCCH - no access to RACH at this point, since RR connection */
    /* establishment is shared between packet and circuit switched services  */
    /* Best we can do is restart the measurement timer for the next period   */
    MSG_GERAN_MED_3_G("PMR blocked %d",
            scell_info_ptr->gsm.camped_on_flag,
            0,
            0);
    (void)rr_gprs_start_t3158(FALSE, gas_id);
  }

  return(proceed);
}

#ifdef FEATURE_WCDMA

/*! rr_gprs_store_3g_measurement

Stores the measurements for the specified cell in the 3G measurements
database for measurement reporting.

*/
static void rr_gprs_store_3g_measurement(
  uint16 uarfcn,
  uint16 scrambling_code,
  int16 rscp,
  int16 ecio,
  int16 rssi,
  const gas_id_t gas_id
)
{
  int i;
  boolean found = FALSE;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  /* search for the uarfcn/scrambling code in the database,
  if present, update it's measurements */
  for (i = 0; i < rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count && !found; i++)
  {
    if (rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].uarfcn == uarfcn &&
        rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].scrambling_code == scrambling_code)
    {
      MSG_GERAN_MED_3_G("Updating WCDMA cell %d, %d, %d", uarfcn, scrambling_code, rscp);
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].rscp_total += rscp;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].ecio_total += ecio;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].rssi_total += rssi;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].noof_measurements += 1;
      found = TRUE;
    }
  }

  if (!found)
  {
    if (rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count > MAX_GRR_3G_FDD_MEASUREMENTS)
    {
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count = MAX_GRR_3G_FDD_MEASUREMENTS;
    }

    if (rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count < MAX_GRR_3G_FDD_MEASUREMENTS)
    {
      MSG_GERAN_MED_3_G("Inserting WCDMA cell %d, %d, %d", uarfcn, scrambling_code, rscp);
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count].uarfcn = uarfcn;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count].scrambling_code = scrambling_code;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count].rscp_total = rscp;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count].ecio_total = ecio;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count].rssi_total = rssi;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count].noof_measurements = 1;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count++;
    }
    else
    {
      fdd_measurement_t *last_entry;
      boolean replace_last_entry = FALSE;
      gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);


      /* sort the results database according to FDD_REP_QUANT criteria, then
      see if the current cell is better than the weakest stored cell.  If it is,
      replace it with the new one. */
      rr_gprs_3g_measurements_sort(gas_id);

      last_entry = &rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count - 1];

      if (last_entry->noof_measurements == 0)
      {
        /* This shouldn't happen, so flag an error and replace the invalid entry */
        MSG_GERAN_ERROR_3_G("grr_3g_measurements.fdd_entries[%d] has 0 measurements, replacing",
          rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count - 1,
          0,
          0);

        replace_last_entry = TRUE;
      }
      else if (scell->gprs_3g_meas_params.FDD_REP_QUANT == 0)
      {
        if (rscp > (last_entry->rscp_total / last_entry->noof_measurements))
        {
          replace_last_entry = TRUE;
        }
      }
      else
      {
        if (ecio > (last_entry->ecio_total / last_entry->noof_measurements))
        {
          replace_last_entry = TRUE;
        }
      }
      if (replace_last_entry)
      {
        MSG_GERAN_MED_3_G("Replacing weakest WCDMA cell with %d, %d, %d", uarfcn, scrambling_code, rscp);
        last_entry->uarfcn = uarfcn;
        last_entry->scrambling_code = scrambling_code;
        last_entry->rscp_total = rscp;
        last_entry->ecio_total = ecio;
        last_entry->rssi_total = rssi;
        last_entry->noof_measurements = 1;
      }
    }
  }
}

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
/*! rr_gprs_store_tdd_3g_measurement

Stores the measurements for the specified cell in the 3G measurements
database for measurement reporting.

*/
static void rr_gprs_store_tdd_3g_measurement(
  uint16 uarfcn,
  uint16 cpid,
  int16 rscp,
  const gas_id_t gas_id
)
{
  int i;
  boolean found = FALSE;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  /* search for the uarfcn/cpid in the database,
  if present, update it's measurements */
  for (i = 0; i < rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count && !found; i++)
  {
    if (rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[i].uarfcn == uarfcn &&
        rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[i].cpid == cpid)
    {
      MSG_GERAN_MED_3_G("Updating TDSCDMA cell %d, %d, %d", uarfcn, cpid, rscp);
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[i].rscp_total += rscp;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[i].noof_measurements += 1;
      found = TRUE;
    }
  }

  if (!found)
  {
    if (rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count > MAX_GRR_3G_TDD_MEASUREMENTS)
    {
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count = MAX_GRR_3G_TDD_MEASUREMENTS;
    }

    if (rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count < MAX_GRR_3G_TDD_MEASUREMENTS)
    {
      MSG_GERAN_MED_3_G("Inserting TDSCDMA cell %d, %d, %d", uarfcn, cpid, rscp);
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count].uarfcn = uarfcn;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count].cpid = cpid;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count].rscp_total = rscp;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count].noof_measurements = 1;
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count++;
    }
    else
    {
      tdd_measurement_t *last_entry;
      boolean replace_last_entry = FALSE;

      /* sort the results database according to FDD_REP_QUANT criteria, then
      see if the current cell is better than the weakest stored cell.  If it is,
      replace it with the new one. */
      rr_gprs_tdd_3g_measurements_sort(gas_id);

      last_entry = &rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count - 1];

      if (last_entry->noof_measurements == 0)
      {
        /* This shouldn't happen, so flag an error and replace the invalid entry */
        MSG_GERAN_ERROR_3_G("grr_3g_measurements.tdd_entries[%d] has 0 measurements, replacing",
          rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count - 1,
          0,
          0);

        replace_last_entry = TRUE;
      }
      else
      if (rscp > (last_entry->rscp_total / last_entry->noof_measurements))
      {
        replace_last_entry = TRUE;
      }
      
      if (replace_last_entry)
      {
        MSG_GERAN_MED_3_G("Replacing weakest TDSCDMA cell with %d, %d, %d", uarfcn, cpid, rscp);
        last_entry->uarfcn = uarfcn;
        last_entry->cpid = cpid;
        last_entry->rscp_total = rscp;
        last_entry->noof_measurements = 1;
      }
    }
  }
}
#endif /*FEATURE_GSM_TDS*/

static char *get_nc_measurement_control_state_name( nc_meas_state_T state )
{
  char * s;

  switch ( state )
  {
    case NC_MEAS_NULL:
      s = "NULL";
      break;

    case NC_MEAS_ACQUIRE:
      s = "ACQUIRE";
      break;

    case NC_MEAS_SEND:
      s = "SEND";
      break;

    case NC_MEAS_SUSPEND:
      s = "SUSPEND";
      break;

    default:
      s = "?";
  }

  return s;

} /* end get_nc_measurement_control_state_name() */

#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)

/**
  @brief Deletes an FDD entry from the measurements store indicated.

  @param measurements The measurements store to modify.
  @param i The index of the entry in the measurements store to delete.
*/
void grr_3g_measurements_delete_fdd_index(
  grr_3g_measurements_t * measurements,
  uint8 i,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(measurements);

  if (measurements->fdd_count > 0)
  {
    if (measurements->fdd_count <= MAX_GRR_3G_FDD_MEASUREMENTS ) // Check for FDD count to be lessa than max assigned.
    {
       /* Check that the index we've been asked to delete is actually present
             in the database. */
       if (i < measurements->fdd_count)
       {
         /* If the index that we've been asked to delete is not the last index
                in the database then we need to move all entries after the index to
                be deleted down by one, to fill in the gap in the database. */
         uint8 last_entry_index = measurements->fdd_count - 1;

         if (i < last_entry_index)
         {
          uint8 entries_to_move = last_entry_index - i;
          //Replace memmove with memsmove
		  memsmove(&measurements->fdd_entries[i],
		          (sizeof(measurements->fdd_entries)-(i*sizeof(measurements->fdd_entries[0]))), 
		          &measurements->fdd_entries[i+1], 
		          sizeof(fdd_measurement_t) * entries_to_move);
         }
        --measurements->fdd_count;
       }
     }	
  }
}

static void rr_gprs_meas_rpt_control_process_surround_irat_meas_ind(
  mph_surround_irat_meas_ind_t * new_measurements,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  RR_NULL_CHECK_RETURN_VOID(new_measurements);

#ifdef FEATURE_WCDMA
  if (new_measurements->rat == RR_L1_IRAT_RAT_WCDMA)
  {
    rr_l1_wcdma_measurements_t * rr_l1_wcdma_measurements = &new_measurements->measurements.wcdma;

    grr_3g_measurements_t * grr_3g_measurements = &rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements;

    uint8 i;

    /* MPH_SURROUND_IRAT_MEAS_IND contains measurements for a specific
    frequency.  If the new signal does not contain measurements for any
    previously reported cells then they should first be removed from 
    the grr_3g_measurements store. */

    /* Iterate over the measurements in the current store, deleting
    any entries that are not included in the measurement report from L1
    for the same frequency. */
    uint16 uarfcn = rr_l1_wcdma_measurements->uarfcn;

    for (i = grr_3g_measurements->fdd_count; i > 0; --i)
    {
      fdd_measurement_t * entry = &grr_3g_measurements->fdd_entries[i-1];
      boolean             found = FALSE;

      if (entry->uarfcn == uarfcn)
      {
        /* If the entry is not in the new measurement report then remove
        it as it is no longer being reported by L1. */
        uint8 j;

        for (j = 0;
             j < rr_l1_wcdma_measurements->no_of_entries &&
             j < MAX_WCDMA_MEASUREMENTS_PER_FREQUENCY;
             ++j)
        {
          if (entry->scrambling_code == rr_l1_wcdma_measurements->entries[j].scrambling_code)
          {
            found = TRUE;
            break;
          }
        }

        if (!found)
        {
          MSG_GERAN_MED_2_G("WCDMA cell UARFCN %d, Scrambling Code %d no longer reported by L1, deleting",
                   entry->uarfcn,
                   entry->scrambling_code);
          grr_3g_measurements_delete_fdd_index(grr_3g_measurements, i-1, gas_id);
        }
      }
    }

    for (i = 0; i < new_measurements->measurements.wcdma.no_of_entries; ++i)
    {
      rr_gprs_store_3g_measurement(
        new_measurements->measurements.wcdma.uarfcn,
        new_measurements->measurements.wcdma.entries[i].scrambling_code,
        new_measurements->measurements.wcdma.entries[i].rscp,
        new_measurements->measurements.wcdma.entries[i].ecno,
        new_measurements->measurements.wcdma.entries[i].rssi,
        gas_id
      );
    }
  }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  if (new_measurements->rat == RR_L1_IRAT_RAT_UTRAN_TDD)
  {
    uint32 i;

    memset(&rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements, 0, sizeof(grr_3g_measurements_t));

    for (i = 0; i < new_measurements->measurements.utran_tdd.no_of_entries; ++i)
    {
      rr_gprs_store_tdd_3g_measurement(
        new_measurements->measurements.utran_tdd.uarfcn,
        new_measurements->measurements.utran_tdd.entries[i].cell_parameter_id,
        new_measurements->measurements.utran_tdd.entries[i].rscp,
        gas_id
      );
    }
  }
#endif /*FEATURE_GSM_TDS*/
}

#endif /* FEATURE_LTE || FEATURE_GSM_TDS*/

void rr_utran_priority_params_merge_mods(
  csn_utran_priority_params_ie_t * dst,
  csn_utran_priority_params_ie_t * mods
)
{
  uint32 i;
  uint32 j;
  uint32 index_bitmap = 0;
  uint32 complete_bitmap = 0;

  RR_NULL_CHECK_RETURN_VOID(dst);
  RR_NULL_CHECK_RETURN_VOID(mods);

  if (mods->default_params_valid)
  {
    dst->default_params_valid = TRUE;
    dst->default_params = mods->default_params;
  }

  /* Remove old values before storing modified values */
  for (i = 0;
       i < mods->utran_priority_params_count &&
       i < CSN_UTRAN_PRIORITY_PARAMS_DESC_MAX;
       ++i)
  {
    complete_bitmap = 0;
    
    for (j = 0; j < dst->utran_priority_params_count; j++)
    {
      /* get frequency indexes present both in old and new data. */
      index_bitmap = dst->utran_priority_params[j].frequency_index_bitmap &
                     mods->utran_priority_params[i].frequency_index_bitmap;

      /* A new priority information is received for some freq indexes. Hence remove those freq indexes from old data. */
      if (index_bitmap != 0)
      {
        dst->utran_priority_params[j].frequency_index_bitmap ^= index_bitmap;
      }

      complete_bitmap |= index_bitmap;

      /* All indexes in new data are removed from older data. No need to go through loop again. */
      if (complete_bitmap == mods->utran_priority_params[i].frequency_index_bitmap)
      {
        break;
      }
    }
  }

  /* If all frequency indexes are removed from a structure then remove the structure */
  for (i = 0, j = 0; j < dst->utran_priority_params_count; j++)
  {
    if (dst->utran_priority_params[j].frequency_index_bitmap != 0)
    {
      dst->utran_priority_params[i++] = dst->utran_priority_params[j];
    }
  }
  dst->utran_priority_params_count = i;
  
  for (i = 0;
       i < mods->utran_priority_params_count &&
       i < CSN_UTRAN_PRIORITY_PARAMS_DESC_MAX;
       ++i)
  {
    if (dst->utran_priority_params_count < CSN_UTRAN_PRIORITY_PARAMS_DESC_MAX)
    {
      dst->utran_priority_params[dst->utran_priority_params_count] = mods->utran_priority_params[i];
      ++dst->utran_priority_params_count;
    }
    else
    {
      MSG_GERAN_ERROR_1("Overflow merging UTRAN Priority Parameters (%d)", i);
    }
  }
}

void rr_copy_csn_eutran_neighbour_cells_to_rr_eutran_neighbour_cells(
  rr_eutran_neighbour_cells_t * dst,
  csn_eutran_neighbour_cells_t * src
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(dst);
  RR_NULL_CHECK_RETURN_VOID(src);

  memset(dst, 0, sizeof(rr_eutran_neighbour_cells_t));

  for (i = 0; i < src->earfcn_count && i < CSN_EUTRAN_NEIGHBOUR_CELLS_MAX_EARFCNS; ++i)
  {
    if (dst->eutran_frequency_list_count < MAX_EUTRAN_NEIGHBOUR_CELLS_EARFCNS)
    {
      dst->eutran_frequency_list[dst->eutran_frequency_list_count].earfcn = (lte_earfcn_t)src->earfcns[i].earfcn;
      dst->eutran_frequency_list[dst->eutran_frequency_list_count].measurement_bandwidth = src->earfcns[i].measurement_bandwidth;
      ++dst->eutran_frequency_list_count;
    }
  }

  if (src->priority_valid)
  {
    dst->eutran_priority_valid = TRUE;
    dst->eutran_priority = src->priority;
  }
  dst->thresh_eutran_high = src->thresh_high;
  if (src->thresh_low_valid)
  {
    dst->thresh_eutran_low_valid = TRUE;
    dst->thresh_eutran_low = src->thresh_low;
  }
  if (src->qrxlevmin_valid)
  {
    dst->eutran_qrxlevmin_valid = TRUE;
    dst->eutran_qrxlevmin = src->qrxlevmin;
  }
}

void rr_remove_old_entries_from_repeated_eutran_ncells(
  rr_eutran_params_t * dst, 
  csn_eutran_params_ie_t * src
)
{
  uint8   src_i;
  uint8   src_j;
  uint8   dst_i;
  uint8   dst_j;
  uint16  src_earfcn;
  uint16  dst_earfcn;
  boolean found             = FALSE;
  boolean remove_entry      = FALSE;
  boolean move_entry        = FALSE;
  uint8   remove_index      = 0;
  uint8   remove_count      = 0;
  boolean remove_list_entry = FALSE;
  boolean move_list_entry   = FALSE;
  uint8   remove_list_index = 0;
  uint8   remove_list_count = 0;
  csn_eutran_neighbour_cells_t * src_list;
  rr_eutran_neighbour_cells_t  * dst_list;

  for (src_i = 0; src_i < src->neighbour_cells_count && src_i < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS; ++src_i)
  {
    src_list = &src->neighbour_cells[src_i];
      
    for (src_j = 0; src_j < src_list->earfcn_count && src_j < CSN_EUTRAN_NEIGHBOUR_CELLS_MAX_EARFCNS; ++src_j)
    {
      found = FALSE;
      src_earfcn = src_list->earfcns[src_j].earfcn;

      for (dst_i = 0; dst_i < dst->neighbour_cells_count && dst_i < RR_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS; ++dst_i)
      {
        dst_list = &dst->neighbour_cells[dst_i];
        
        for (dst_j = 0; dst_j < dst_list->eutran_frequency_list_count && dst_j < MAX_EUTRAN_NEIGHBOUR_CELLS_EARFCNS; ++dst_j)
        {
          dst_earfcn = dst_list->eutran_frequency_list[dst_j].earfcn;

          /* Invalidate old entry */
          if (dst_earfcn == src_earfcn)
          {
            dst_list->eutran_frequency_list[dst_j].earfcn = EARFCN_INVALID_VAL;
            found = TRUE;
            break;
          }
        }

        if (found)
        {
          break;
        }
      }
    }
  }

  for (dst_i = 0; dst_i < dst->neighbour_cells_count && dst_i < RR_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS; ++dst_i)
  {
    remove_entry = FALSE;
    move_entry   = FALSE;
    remove_index = 0;
    remove_count = 0;

    dst_list = &dst->neighbour_cells[dst_i];
      
    for (dst_j = 0; dst_j < dst_list->eutran_frequency_list_count && dst_j < MAX_EUTRAN_NEIGHBOUR_CELLS_EARFCNS; ++dst_j)
    {
      /* Remove invalidated entry */
      if (dst_list->eutran_frequency_list[dst_j].earfcn == EARFCN_INVALID_VAL)
      {
        remove_entry = TRUE;
        remove_count++;
      }
      else
      {
        remove_entry = FALSE;
      }

      /* move valid entries to the front of array */
      if ((move_entry == FALSE) && (remove_entry == TRUE))
      {
        move_entry = TRUE;
        remove_index = dst_j;
      }
      else if ((move_entry == TRUE) && (remove_entry == FALSE))
      {
        dst_list->eutran_frequency_list[remove_index] = dst_list->eutran_frequency_list[dst_j];
        remove_index++;
      }
    }

    dst_list->eutran_frequency_list_count -= remove_count;

    /* If all earfcns are removed then remove the repeated eutran list entry. */
    if (dst_list->eutran_frequency_list_count == 0)
    {
      remove_list_entry = TRUE;
      remove_list_count++;
    }
    else
    {
      remove_list_entry = FALSE;
    }
      
    /* move non-zero entries to the front */
    if ((move_list_entry == FALSE) && (remove_list_entry == TRUE))
    {
      move_list_entry = TRUE;
      remove_list_index = dst_i;
    }
    else if ((move_list_entry == TRUE) && (remove_list_entry == FALSE))
    {
      dst->neighbour_cells[remove_list_index] = dst->neighbour_cells[dst_i];
      remove_list_index++;
    }
  }
  
  dst->neighbour_cells_count -= remove_list_count;
}

void rr_eutran_params_merge_csn_mods(
  rr_eutran_params_t * dst,
  csn_eutran_params_ie_t * mods
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(dst);
  RR_NULL_CHECK_RETURN_VOID(mods);

  dst->ccn_active = mods->ccn_active;

  /* 3GPP 45.008 section 10.4, Table 3, Note 2 states:

  "In case an optional parameter is not included in a point-to-point signalling
  message, the default value of that parameter shall replace any previously
  broadcast value, where applicable."

  This means that if GPRS E-UTRAN Measurement Parameters are included in the
  PMO, we must clear out the broadcast values before copying in the PMO values.
  */
  if (mods->gprs_meas_params_valid)
  {

    /* Copy the CSN.1 structure to the RR structure. */
    dst->gprs_meas_params.qsearch_p = mods->gprs_meas_params.qsearch_p;
    dst->gprs_meas_params.rep_quant = mods->gprs_meas_params.rep_quant;
    dst->gprs_meas_params.multirat_reporting = mods->gprs_meas_params.multirat_reporting;


    if (mods->gprs_meas_params.fdd_valid)
    {
      dst->gprs_meas_params.fdd.reporting_threshold_2 = mods->gprs_meas_params.fdd_reporting_threshold;
      if (mods->gprs_meas_params.fdd_reporting_threshold_2_valid)
      {
        dst->gprs_meas_params.fdd.reporting_threshold_2 = mods->gprs_meas_params.fdd_reporting_threshold_2; 
      }
      if (mods->gprs_meas_params.fdd_reporting_offset_valid)
      {
        dst->gprs_meas_params.fdd.reporting_offset = mods->gprs_meas_params.fdd_reporting_offset;
      }
    }

    if (mods->gprs_meas_params.tdd_valid)
    {
      dst->gprs_meas_params.tdd.reporting_threshold = mods->gprs_meas_params.tdd_reporting_threshold;
      if (mods->gprs_meas_params.tdd_reporting_threshold_2_valid)
      {
        dst->gprs_meas_params.tdd.reporting_threshold_2 = mods->gprs_meas_params.tdd_reporting_threshold_2;
      }
      if (mods->gprs_meas_params.tdd_reporting_offset_valid)
      {
        dst->gprs_meas_params.tdd.reporting_offset = mods->gprs_meas_params.tdd_reporting_offset;
      }
    }
  }

  /* If we receive updated parameters for an eutran cell, we need to keep the latest info in cell list.
   * If an eutran cell is already present remove its entry. 
   * This will avoid having both old and updated entries for same cell. */
  rr_remove_old_entries_from_repeated_eutran_ncells(dst, mods);
  
  /* Now copy latest cell info. */
  for (i = 0;
       i < mods->neighbour_cells_count &&
       i < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS;
       ++i)
  {
    if (dst->neighbour_cells_count < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS)
    {
      rr_copy_csn_eutran_neighbour_cells_to_rr_eutran_neighbour_cells(
        &dst->neighbour_cells[dst->neighbour_cells_count],
        &mods->neighbour_cells[i]
      );
      ++dst->neighbour_cells_count;
    }
    else
    {
      MSG_GERAN_ERROR_1("Overflow merging E-UTRAN Neighbour Cells (%d)", i);
    }
  }

  for (i = 0;
       i < mods->not_allowed_cells_count &&
       i < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS;
       ++i)
  {
    if (dst->not_allowed_cells_count < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS)
    {
      dst->not_allowed_cells[dst->not_allowed_cells_count] = mods->not_allowed_cells[i];
      ++dst->not_allowed_cells_count;
    }
    else
    {
      MSG_GERAN_ERROR_1("Overflow merging E-UTRAN Not Allowed Cells (%d)", i);
    }
  }
}

void csn_eutran_params_merge_csn_mods(
  csn_eutran_params_ie_t * dst,
  csn_eutran_params_ie_t * mods
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(dst);
  RR_NULL_CHECK_RETURN_VOID(mods);

  dst->ccn_active = mods->ccn_active;

  if (mods->gprs_meas_params_valid)
  {
    dst->gprs_meas_params_valid = TRUE;
    dst->gprs_meas_params = mods->gprs_meas_params;
  }

  for (i = 0;
       i < mods->neighbour_cells_count &&
       i < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS;
       ++i)
  {
    if (dst->neighbour_cells_count < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS)
    {
      dst->neighbour_cells[dst->neighbour_cells_count] = mods->neighbour_cells[i];
      ++dst->neighbour_cells_count;
    }
    else
    {
      MSG_GERAN_ERROR_1("Overflow merging E-UTRAN Neighbour Cells (%d)", i);
    }
  }

  for (i = 0;
       i < mods->not_allowed_cells_count &&
       i < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS;
       ++i)
  {
    if (dst->not_allowed_cells_count < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS)
    {
      dst->not_allowed_cells[dst->not_allowed_cells_count] = mods->not_allowed_cells[i];
      ++dst->not_allowed_cells_count;
    }
    else
    {
      MSG_GERAN_ERROR_1("Overflow merging E-UTRAN Not Allowed Cells (%d)", i);
    }
  }
}



/****************************************************************************
 *   Function name:  rr_gprs_nc_measurement_control
 *   --------------
 *
 *   Description: This function is the control of the NC meas state machine
 *   ------------
 *
 *
 *   Parameters: An event and a possible message
 *   -----------
 *
 *
 *   Returns:  an event to calling FSM/function
 *   --------
 *
 *
 ****************************************************************************/
rr_event_T rr_gprs_nc_measurement_control(
  rr_event_T nc_meas_event,
  void *new_message,
  const gas_id_t gas_id
)
{
  rr_params_mods_t ** params_mods = rr_params_mods(gas_id);
  rr_event_T          new_nc_meas_event = EV_NO_EVENT;
  rr_event_T          output_event = EV_NO_EVENT;
  byte                message_set, message_id;

// Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;

  RR_NULL_CHECK_RETURN_PARAM(params_mods, EV_NO_EVENT);

  
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  nc_meas_event = rr_extract_message_header_details(
                    nc_meas_event,
                    new_message,
                    &message_set,
    &message_id
  );

  /************************************************************/
  /* Upon a reset command reinitialize the state machine and  */
  /* variables. This will in effect switch off any nc         */
  /* measurement reporting coming from L1. If the state is    */
  /* NC_MEAS_NULL then there is no need to reset              */
  /************************************************************/
  if (nc_meas_event == EV_RESET_SOFTWARE ||
      nc_meas_event == RR_EV_MM_REQ_RECEIVED_ABORT)
  {
    if (rr_gprs_meas_rpt_control_data_ptr->state != NC_MEAS_NULL)
    {
      rr_gprs_stop_nc_measurement_process(gas_id);
      rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_NULL;
    }
    rr_gprs_meas_rpt_control_reset(gas_id);
    nc_meas_event = EV_NO_EVENT;
  }

#ifdef FEATURE_SGLTE
  if (rr_is_sglte_mode_active(gas_id))
  {
    if (nc_meas_event == EV_NCO_MODE_1)        
    {
      MSG_HIGH("SGLTE Mode active when NCO_MODE_1 set, inhibit measurements to NW",0,0,0);
      nc_meas_event = EV_NO_EVENT;
      output_event = EV_CELL_RESELECTION_ACT;
    }
    else if (nc_meas_event == EV_NCO_MODE_2)
    {
      MSG_HIGH("SGLTE Mode active when NCO_MODE_2 set, inhibit measurements to NW",0,0,0);
      nc_meas_event = EV_NO_EVENT;
      output_event = EV_CELL_RESELECTION_DEACT; /*Difficult case as we not sending measurement reports*/
    }
  }
#endif /*FEATURE_SGLTE*/

  while (nc_meas_event != EV_NO_EVENT)
  {
    switch(rr_gprs_meas_rpt_control_data_ptr->state)
    {
  /***************************************************************/
  /* NC_MEAS_NULL - Measurement reporting is inactive. There is  */
  /* no requirement to report to the network. This state is left */
  /* if                                                          */
  /* 1.PMO is recieved that requires measurement reporting       */
  /* 2.PCCO on the previous cell requires reports in the new cell*/
  /* 3.system information on the current scell reuires it.       */
  /***************************************************************/
    case NC_MEAS_NULL:

      /***************************************************************/
      /* INPUT EVENT handlers                                        */
      /***************************************************************/
      if ((nc_meas_event == EV_INPUT_MESSAGE) && (new_message != NULL))
      {
        if (message_set == MS_MM_RR)
        {
          mm_rr_message_id_enum_T mm_rr_message_id =
                  (mm_rr_message_id_enum_T) message_id;

          if (mm_rr_message_id == RR_GMM_GPRS_STATE_CHANGE_REQ)
          {
            rr_gmm_gprs_state_change_req_T *rr_gmm_gprs_state_change_req =
                               (rr_gmm_gprs_state_change_req_T*) new_message;

            if(rr_gmm_gprs_state_change_req->new_state == GMM_READY_STATE)
            {
              /* GMM has moved into the ready state so see if the system info */
              /* requires measurement reporting to be switched on */
              new_nc_meas_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
            }
            else
            {
              new_nc_meas_event = rr_gprs_change_nc_measurement_process(GMM_STANDBY_CAUSE, gas_id);
            }
          }
          else
          {
            MSG_GERAN_ERROR_1_G("Unexpected message from MM: %d", mm_rr_message_id);
          }
        }

        else if (message_set == MS_RR_L1)
        {
           rr_l1_message_id_enum_T rr_l1_message_id =
                   (rr_l1_message_id_enum_T) message_id;

           if (rr_l1_message_id == MPH_SURROUND_MEAS_IND ||
               rr_l1_message_id == MPH_SERVING_MEAS_IND ||
               rr_l1_message_id == MPH_SERVING_IDLE_MEAS_IND ||
               rr_l1_message_id == MPH_NC_MEASUREMENT_IND ||
               rr_l1_message_id == MPH_SERVING_AUX_MEAS_IND
               )
           {
             /* ignore measurement reports in this state as measurement reporting */
             /* is inactive */
             ;
           }
#ifdef FEATURE_WCDMA
           /* save 3g measurements to keep grr_3g_measurements up to date,
            * which could be used if MS go to NC1/NC2
            */
           else if (rr_l1_message_id == MPH_SURROUND_WCDMA_IDLE_MEAS_IND)
           {
             (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);
             rr_gprs_store_3g_measurements_idle(
               (mph_surround_wcdma_idle_meas_ind_T *) new_message,
               gas_id
             );
           }
           else if (rr_l1_message_id == MPH_SURROUND_WCDMA_XFER_MEAS_IND)
           {
             (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);
             rr_gprs_store_3g_measurements_xfer(
               (mph_surround_wcdma_xfer_meas_ind_T *) new_message,
               gas_id
             );
           }
#endif /* FEATURE_WCDMA */
#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
           else if (rr_l1_message_id == MPH_SURROUND_IRAT_MEAS_IND)
           {
             (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);
             rr_gprs_meas_rpt_control_process_surround_irat_meas_ind(
               (mph_surround_irat_meas_ind_t *) new_message,
               gas_id
             );
           }
#endif /* FEATURE_LTE || FEATURE_GSM_TDS */
           else
           {
             MSG_GERAN_ERROR_1_G("Unknown message from L1: %d", rr_l1_message_id);
           }
        }
        else if (message_set == MS_MAC_RR)
        {
          mac_grr_sig_id_t mac_grr_sig_id = (mac_grr_sig_id_t) message_id;

          if (mac_grr_sig_id == MAC_GRR_CELL_RESEL_STATUS)
          {
            mac_grr_cell_resel_status_t *mac_grr_cell_resel_status =
                            (mac_grr_cell_resel_status_t*) new_message;

            rr_gprs_meas_rpt_control_handle_mac_grr_cell_resel_status(
              mac_grr_cell_resel_status->status,
              gas_id
            );
          }
          else if (mac_grr_sig_id == MAC_GRR_MEAS_RPT_CNF)
          {
            MSG_GERAN_HIGH_1_G("Late arrival of MAC_GRR_MEAS_RPT_CNF (expected=%1d)",
            rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.standby_before_cnf);
            rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.standby_before_cnf = FALSE;
          }
          else
          {
            MSG_GERAN_ERROR_1_G("Unknown message from MAC: %d", mac_grr_sig_id);
          }
        }
        else
        {
          MSG_GERAN_ERROR_2_G("Unknown message from unknown entity: %d, %d",
              (int) message_set, (int) message_id);
        }
      }

      /**
       * A complete set of PMO messages has been received and are ready for
       * processing.
       */
      else if (nc_meas_event == EV_PMO_MESSAGE)
      {
        new_nc_meas_event = rr_gprs_meas_rpt_control_handle_pmo_message(gas_id);
      }

      /**
       * A PCCO has been received and is ready for processing.
       */
      else if ( nc_meas_event == EV_PCCO_MESSAGE )
      {
        new_nc_meas_event = rr_gprs_meas_rpt_control_handle_pcco_message(gas_id);
      }

      /***************************************************************************/
      /* EV_ABANDON_RESELECTION                                                  */
      /* Do nothing with this event in this state. Just swallow it !             */
      /***************************************************************************/
      else if ( nc_meas_event == EV_ABANDON_RESELECTION )
      {
        ;
      }

      /***************************************************************************/
      /* RR_EV_MEASUREMENT_PARAMS_UPDATED                                        */
      /* System information that is relevant to measurement reporting has changed*/
      /* This must be checked to see if nc measurement reporting is effected     */
      /***************************************************************************/
      else if ( nc_meas_event == RR_EV_MEASUREMENT_PARAMS_UPDATED )
      {
        /*The measurement reporting system information has changed*/
        new_nc_meas_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
      }

      /***************************************************************************/
      /* EV_NCO_MODE_0                                                           */
      /* A PMO may have given us extra frequencies to monitor so this is needed  */
      /* to get cell reselection to check if this the case                       */
      /***************************************************************************/
      else if (nc_meas_event == EV_NCO_MODE_0)
      {
        /*Set the output event that will alert cell reselection of the current setting*/
        output_event = EV_CELL_RESELECTION_ACT;
      }

      /***************************************************************************/
      /* EV_NCO_MODE_1                                                           */
      /* NCO has been set to 1 so measurement reporting is required but with     */
      /* autonomous cell reselection. Start the measurement reporting and notify */
      /* cell reselection that a PMO has been recieved as GPRS parameters may be */
      /* available for some cells                                                */
      /***************************************************************************/
      else if (nc_meas_event == EV_NCO_MODE_1)
      {
        MSG_GERAN_HIGH_0_G("Going to NC1 in NC_MEAS_NULL");

        /*send the command to layer 1 to begin the measurement reporting*/
        rr_gprs_start_nc_measurement_process(FALSE, gas_id);

        /*Set the state to acquire, measurement reporting is now active*/
        rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_ACQUIRE;

        /*Set the output event that will alert cell reselection of the PMO*/
        output_event = EV_CELL_RESELECTION_ACT;
        
#ifdef FEATURE_IDLE_DRX_SCALING
        rr_idle_drx_control(EV_NCO_MODE_1, NULL, gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */
      }

      /***************************************************************************/
      /* EV_NCO_MODE_2                                                           */
      /* NCO has been set to 2 so measurement reporting is required but with     */
      /* network controlled cell reselection. Start the measurement reporting    */
      /* the notify cell reselection.                                            */
      /***************************************************************************/
      else if (nc_meas_event == EV_NCO_MODE_2)
      {
        MSG_GERAN_HIGH_0_G("Going to NC2 in NC_MEAS_NULL");

        /*send the command to layer 1 to begin the measurement reporting*/
        rr_gprs_start_nc_measurement_process(FALSE, gas_id);

        /*Set the state to acquire, measurement reporting is now active*/
        rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_ACQUIRE;

        /*Set the output event that will deactivate cell reselection*/
        output_event = EV_CELL_RESELECTION_DEACT;
        
#ifdef FEATURE_IDLE_DRX_SCALING
        rr_idle_drx_control(EV_NCO_MODE_2, NULL, gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */
      }

      /*****************************************************************/
      /* EV_CAMP_ON_GRR5n_PCCF_OLD_SCELL                               */
      /* A pcco cell reselection has failed and we have camped back on */
      /* the old scell. restore the nc setup on this scell.            */
      /* If we are in NuLL then the cell that the PCCO targeted had NC0*/
      /*****************************************************************/
      else if (nc_meas_event == EV_CAMP_ON_PCCF_OLD_SCELL)
      {
        /* restore the old values for this old scell */
        new_nc_meas_event = rr_gprs_change_nc_measurement_process(OLD_SCELL_CAUSE, gas_id);
      }

      else if (nc_meas_event == RR_EV_CAMPED_ON_CELL)
      {
        MSG_GERAN_HIGH_0_G("Camped on cell, reading system info...");

        /* Camped on a new cell, so clear out any previous PMO modifications. */
        if (*params_mods != NULL)
        {
          GPRS_MEM_FREE(*params_mods);
          *params_mods = NULL;
        }

        rr_gprs_update_meas_params(gas_id);

        new_nc_meas_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
      }

      else if (nc_meas_event == RR_EV_DEACTIVATE_MEASUREMENTS)
      {
        ; /* safe to ignore this here as measurements are already deactivated */
      }

      else if (nc_meas_event == RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13)
      {
        rr_gprs_revert_ba_gprs(gas_id);
        rr_gprs_meas_rpt_control_reset(gas_id);
      }

      else if (nc_meas_event == EV_RESET_SOFTWARE)
      {
        ; /* don't need to do anything here as reporting is inactive */
      }

      else if ((nc_meas_event == EV_MAC_TRANSFER) ||
               (nc_meas_event == EV_MAC_IDLE)     ||
               (nc_meas_event == EV_T3158_TIMEOUT))
      {
        ; /* safe to ignore these events here */
      }

      else
      {
        MSG_GERAN_ERROR_1_G("Unexpected Event: %d in state NC_MEAS_NULL", nc_meas_event);
      }

      break;

  /************************************************************/
  /* NC_MEAS_ACQUIRE - This state collects the L1 measurement */
  /* reports and averages the measurments until the timer     */
  /* expires T3158                                            */
  /************************************************************/
    case NC_MEAS_ACQUIRE:

      /*********************************************************/
      /* Message handlers                                      */
      /*********************************************************/
      if ((nc_meas_event == EV_INPUT_MESSAGE) && (new_message != NULL))
      {
        if (message_set == MS_MM_RR)
        {
          mm_rr_message_id_enum_T mm_message_id = (mm_rr_message_id_enum_T) message_id;

          if (mm_message_id == RR_GMM_GPRS_STATE_CHANGE_REQ)
          {
            rr_gmm_gprs_state_change_req_T *rr_gmm_gprs_state_change_req =
                               (rr_gmm_gprs_state_change_req_T*) new_message;

            if(rr_gmm_gprs_state_change_req->new_state != GMM_READY_STATE)
            {
              /**
               * GMM has left the READY state so measurement reporting should
               * be deactivated.
               */
              new_nc_meas_event = rr_gprs_change_nc_measurement_process(GMM_STANDBY_CAUSE, gas_id);
            }
          }
          else
          {
            MSG_GERAN_ERROR_1_G("Unexpected message from MM: %d", message_id);
          }
        }

        else if (message_set == MS_MAC_RR)
        {
          mac_grr_sig_id_t mac_message_id = (mac_grr_sig_id_t) message_id;

          /**
           * A MAC_GRR_CELL_RESEL_STATUS indicates the success or failure of
           * the first cell access attempt on a cell after a PCCO.
           */
          if (mac_message_id == MAC_GRR_CELL_RESEL_STATUS)
          {
            mac_grr_cell_resel_status_t *mac_grr_cell_resel_status =
                            (mac_grr_cell_resel_status_t*) new_message;

            rr_gprs_meas_rpt_control_handle_mac_grr_cell_resel_status(
              mac_grr_cell_resel_status->status,
              gas_id
            );
          }
          else
          {
            MSG_GERAN_ERROR_1_G("Unexpected message from MAC: %d", message_id);
          }
        }

        /**
         * Handle messages from L1.
         */
        else if (message_set == MS_RR_L1)
        {
          rr_l1_message_id_enum_T l1_message_id = (rr_l1_message_id_enum_T) message_id;

          if (l1_message_id == MPH_NC_MEASUREMENT_IND)
          {
            rr_gprs_store_nc_measurements((mph_nc_measurement_ind_t*) new_message, gas_id);
          }
          else if (l1_message_id == MPH_SERVING_MEAS_IND)
          {
            rr_gprs_store_serving_measurements((mph_serving_meas_ind_t*) new_message, gas_id);
          }
          else if (l1_message_id == MPH_SERVING_IDLE_MEAS_IND)
          {
            mph_serving_meas_ind_t mph_serving_meas_ind;
            mph_serving_idle_meas_ind_T *mph_serving_idle_meas_ind =
                            (mph_serving_idle_meas_ind_T*) new_message;
            mph_serving_meas_ind.rxlev_average =
              mph_serving_idle_meas_ind->RXLEV_average;
            mph_serving_meas_ind.i_level_valid = FALSE;

            rr_gprs_store_serving_measurements(&mph_serving_meas_ind, gas_id);
          }
#ifdef FEATURE_WCDMA
          else if (l1_message_id == MPH_SURROUND_WCDMA_IDLE_MEAS_IND)
          {
            (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);
            rr_gprs_store_3g_measurements_idle(
              (mph_surround_wcdma_idle_meas_ind_T *) new_message,
              gas_id
            );
          }
          else if (l1_message_id == MPH_SURROUND_WCDMA_XFER_MEAS_IND)
          {
            (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);
            rr_gprs_store_3g_measurements_xfer(
              (mph_surround_wcdma_xfer_meas_ind_T *) new_message,
              gas_id
            );
          }
#endif /* FEATURE_WCDMA */
#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
          else if (l1_message_id == MPH_SURROUND_IRAT_MEAS_IND)
          {
            (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);
            rr_gprs_meas_rpt_control_process_surround_irat_meas_ind(
              (mph_surround_irat_meas_ind_t *) new_message,
              gas_id
            );
          }
#endif /* FEATURE_LTE || FEATURE_GSM_TDS */
          else
          {
            MSG_GERAN_ERROR_1_G("Unexpected message from L1: %d", (int) message_id);
          }
        }
        else
        {
          MSG_GERAN_ERROR_2_G("Unexpected message: %d, %d",
              (int) message_set, (int) message_id);
        }
      }

      /**
       * A set of PMO messages have been received and decoded.
       */
      else if (nc_meas_event == EV_PMO_MESSAGE)
      {
        new_nc_meas_event = rr_gprs_meas_rpt_control_handle_pmo_message(gas_id);
      }

      /**
       * A PCCO messages has been received.
       */
      else if ( nc_meas_event == EV_PCCO_MESSAGE )
      {
        new_nc_meas_event = rr_gprs_meas_rpt_control_handle_pcco_message(gas_id);
      }

      /**
       * Timer T3158 has expired, send a Packet Measurement Report or
       * Packet Enhanced Measurement Report.
       */
      else if (nc_meas_event == EV_T3158_TIMEOUT)
      {
        if (rr_gprs_meas_report_can_proceed(gas_id))
        {
          /* compile the measurement report */
          if (rr_gprs_format_meas_report(send_to_mac, gas_id))
          {
            /* change the state to send */
            rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_SEND;
          }

          /* Start the nc measurement timer again */
          (void) rr_gprs_start_t3158(FALSE, gas_id);
        }
      }

      /**
       * RR_EV_MEAUREMENT_PARAMS_UPDATED
       * System information containing NC Measurement Parameters has been received,
       * so recheck it.  This will also take into account applying PMO/PCCO modifications.
       */
      else if (nc_meas_event == RR_EV_MEASUREMENT_PARAMS_UPDATED)
      {
        if (!grr_nc_params_set_by_pmo_pcco(gas_id))
        {
          /*The measurement reporting system information has changed,
          and PMO/PCCO params are not in operation at the moment */
          new_nc_meas_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
        }
      }

      /***************************************************************************/
      /* EV_NCO_MODE_0                                                           */
      /* Measurement reporting is currently active, but a PMO has been recieved  */
      /* with the NCO set to 0. Cell reselection is autonomous and measurement   */
      /* turned off                                                              */
      /***************************************************************************/
      else if (nc_meas_event == EV_NCO_MODE_0)
      {
        /* turn off the nc measurements */
        rr_gprs_stop_nc_measurement_process(gas_id);

        /*Reset meas_rpt_sent_in_txfer flag*/
        rr_gprs_meas_rpt_control_data_ptr->meas_rpt_sent_in_txfer = FALSE;

        /*Set the state to NULL, measurement reporting is now switched off*/
        rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_NULL;

        /*Set the output event that will alert cell reselection of the PMO*/
        output_event = EV_CELL_RESELECTION_ACT;
        
#ifdef FEATURE_IDLE_DRX_SCALING
        rr_idle_drx_control(EV_NCO_MODE_0, NULL, gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */
      }

      /***************************************************************************/
      /* EV_NCO_MODE_1                                                           */
      /* Measurement reporting is currently active, but a PMO has been recieved  */
      /* with the NCO set to 1 therefore cell reselection is autonomous with nc  */
      /* measurement reporting on                                                */
      /***************************************************************************/
      else if (nc_meas_event == EV_NCO_MODE_1)
      {
        MSG_GERAN_HIGH_0_G("Going to NC1 in NC_MEAS_ACQUIRE");

        /*send the command to layer 1 to begin the measurement reporting*/
        rr_gprs_start_nc_measurement_process(TRUE, gas_id);

        /*Set the output event that will alert cell reselection of the PMO*/
        output_event = EV_CELL_RESELECTION_ACT;
      }

      /***************************************************************************/
      /* EV_NCO_MODE_2                                                           */
      /* Measurement reporting is currently active, but a PMO has been recieved  */
      /* with the NCO set to 2 therefore cell reselection is network controlled  */
      /* autonomous with nc measurement reporting on                             */
      /***************************************************************************/
      else if (nc_meas_event == EV_NCO_MODE_2)
      {
        MSG_GERAN_HIGH_0_G("Going to NC2 in NC_MEAS_ACQUIRE");

        /*send the command to layer 1 to begin the measurement reporting*/
        rr_gprs_start_nc_measurement_process(TRUE, gas_id);

        /*Set the output event that will deactivate cell reselection*/
        output_event = EV_CELL_RESELECTION_DEACT;
      }

      /************************************************************************/
      /* EV_MAC_IDLE || EV_MAC_TRANSFER                                       */
      /* Measurement reports may only be sent when mac is in idle or transfer */
      /* If t3158 has previously expired there may be a stored measurement    */
      /* report waiting to be sent.                                           */
      /************************************************************************/
      else if ( (nc_meas_event == EV_MAC_IDLE) ||
                (nc_meas_event == EV_MAC_TRANSFER) )
      {
        /**********************************************************************/
        /* a state transition has taken place. If in transfer use the         */
        /* nc_reporting_period_t and if in idle use the nc_reporting_period_i */
        /**********************************************************************/
        (void)rr_gprs_start_t3158(TRUE, gas_id);

        if(nc_meas_event == EV_MAC_IDLE)
        {
          /*Reset meas_rpt_sent_in_txfer flag*/
          rr_gprs_meas_rpt_control_data_ptr->meas_rpt_sent_in_txfer = FALSE;
        }

        /*****************************************************************************/
        /* check to see if there is a stored measurement report that can now be sent */
        /* because MAC is not in the IDLE_BLOCKED or TRANSFER_BLOCKED states         */
        /*****************************************************************************/
        if ( rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.valid )
        {
          /*****************************************************************/
          /* If a pending measurment report is stored then only send it if */
          /* the time to the next available report is greater than the set */
          /* threshold THRESHOLD_REPORTING_PERIOD. There is no point in    */
          /* trying to hold onto and send measurement reports if the freq  */
          /* between them is small, and another is due soon.               */
          /*****************************************************************/
          if ( ((nc_meas_event == EV_MAC_IDLE) &&
                (rr_gprs_get_nc_reporting_period_i(gas_id) > THRESHOLD_REPORTING_PERIOD))
               ||
               ((nc_meas_event == EV_MAC_TRANSFER) &&
                (rr_gprs_get_nc_reporting_period_t(gas_id) > THRESHOLD_REPORTING_PERIOD)) )
          {
            /* send the report to mac */
            rr_send_grr_mac_send_meas_rpt(
              rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.msg_type,
                                          &rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.msg,
              rr_gprs_get_network_control_order(gas_id),
              rr_gprs_get_nc_non_drx_period(gas_id),
              gas_id
            );

            /* change the state to send */
            rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_SEND;

            /* It is possible that the call to rr_gprs_start_t3158() above 
            left the timer running rather than restarting it.  As we have just
            sent a PMR, we should force a restart of T3158 here to ensure that
            the next PMR is sent at the appropriate time. */
            rr_gprs_meas_rpt_control_force_t3158_restart(gas_id);
          }
          else
          {
            /* Don't bother to send the pending measurement report as another is due very soon */
            MSG_GERAN_MED_0_G("clear out pending measurement store, next report due soon");
          }
          /* invalidate the stored report */
          rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.valid = FALSE;
        }
      }

      /***************************************************************************/
      /* RR_EV_DEACTIVATE_MEASUREMENTS                                                      */
      /* deactivate the measurement reporting, a cell reselection is taking place*/
      /* In this send state GRR is awaiting confirmation that the measurments    */
      /* have been sent up to the network. If a cell reselection is actioned then*/
      /* MAC will fail the PMR send, so upon return from the SUSPEND state the   */
      /* we never return to this SEND state !                                    */
      /***************************************************************************/
      else if (nc_meas_event == RR_EV_DEACTIVATE_MEASUREMENTS)
      {
        /*Reset meas_rpt_sent_in_txfer flag*/
        rr_gprs_meas_rpt_control_data_ptr->meas_rpt_sent_in_txfer = FALSE;
        /* go to the suspend state */
        rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_SUSPEND;
      }

      /***************************************************************************/
      /* RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13                                   */
      /* SI13 data is no longer valid, so assume GSM-only operation              */
      /***************************************************************************/
      else if (nc_meas_event == RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13)
      {
        rr_gprs_revert_ba_gprs(gas_id);
        rr_gprs_meas_rpt_control_reset(gas_id);

        new_nc_meas_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
      }

      break;

  /****************************************************************/
  /* NC_MEAS_SEND - The measurement report for the last period    */
  /* has been sent to the network. This state waits for a         */
  /* mac_grr_meas_rpt_cnf before moving back to the acquire state */
  /***************************************************************/
    case NC_MEAS_SEND:

      /*********************************************************/
      /* Message handlers                                      */
      /*********************************************************/
      if ((nc_meas_event == EV_INPUT_MESSAGE) && (new_message != NULL))
      {
        /**
         * Handle messages from MM.
         */
        if (message_set == MS_MM_RR)
        {
          mm_rr_message_id_enum_T mm_rr_message_id =
                  (mm_rr_message_id_enum_T) message_id;

          if (mm_rr_message_id == RR_GMM_GPRS_STATE_CHANGE_REQ)
          {
            rr_gmm_gprs_state_change_req_T *rr_gmm_gprs_state_change_req =
                                (rr_gmm_gprs_state_change_req_T*)new_message;

            if (rr_gmm_gprs_state_change_req->new_state == GMM_READY_STATE)
            {
              /* GMM has moved into the ready state so see if the system info */
              /* requires measurement reporting to be switched on */
              new_nc_meas_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
            }
            else
            {
              /**
               * GMM has left the READY state so measurement reporting should
               * be deactivated.
               */
              MSG_GERAN_MED_0_G("GMM left ready state while waiting for meas rpt cnf");
              rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.standby_before_cnf = TRUE;
              new_nc_meas_event =
                rr_gprs_change_nc_measurement_process(GMM_STANDBY_CAUSE, gas_id);
            }
          }
          else
          {
            MSG_GERAN_ERROR_1_G("Unexpected message from MM: %d", mm_rr_message_id);
          }
        }

        /**
         * Handle messages from L1.
         */
        else if (message_set == MS_RR_L1)
        {
          rr_l1_message_id_enum_T l1_message_id = (rr_l1_message_id_enum_T) message_id;

          if (l1_message_id == MPH_NC_MEASUREMENT_IND)
          {
            rr_gprs_store_nc_measurements((mph_nc_measurement_ind_t*) new_message, gas_id);
          }
          else if (l1_message_id == MPH_SERVING_MEAS_IND)
          {
            rr_gprs_store_serving_measurements((mph_serving_meas_ind_t*) new_message, gas_id);
          }
          else if (l1_message_id == MPH_SERVING_IDLE_MEAS_IND)
          {
            mph_serving_meas_ind_t mph_serving_meas_ind;
            mph_serving_idle_meas_ind_T *mph_serving_idle_meas_ind;
            /*convert the serving_idle_meas primitive to the serving_meas primitive*/
            mph_serving_idle_meas_ind = (mph_serving_idle_meas_ind_T*)new_message;
            mph_serving_meas_ind.rxlev_average = mph_serving_idle_meas_ind->RXLEV_average;
            mph_serving_meas_ind.i_level_valid = FALSE;

            rr_gprs_store_serving_measurements(&mph_serving_meas_ind, gas_id);
          }
#ifdef FEATURE_WCDMA
          else if (l1_message_id == MPH_SURROUND_WCDMA_IDLE_MEAS_IND)
          {
            (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);
            rr_gprs_store_3g_measurements_idle(
              (mph_surround_wcdma_idle_meas_ind_T *) new_message,
              gas_id
            );
          }
          else if (l1_message_id == MPH_SURROUND_WCDMA_XFER_MEAS_IND)
          {
            (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);
            rr_gprs_store_3g_measurements_xfer(
              (mph_surround_wcdma_xfer_meas_ind_T *) new_message,
              gas_id
            );
          }
#endif /* FEATURE_WCDMA */
#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
          else if (l1_message_id == MPH_SURROUND_IRAT_MEAS_IND)
          {
            (void)rr_timer_restart(GRR_SURROUND_MEAS_TIMER, GRR_SURROUND_MEAS_TIMEOUT, gas_id);
            rr_gprs_meas_rpt_control_process_surround_irat_meas_ind(
              (mph_surround_irat_meas_ind_t *) new_message,
              gas_id
            );
          }
#endif /* FEATURE_LTE || FEATURE_GSM_TDS*/
          else
          {
            MSG_GERAN_ERROR_1_G("Unexpected message from L1: %d", message_id);
          }
        }

        /**
         * Handle messages from MAC.
         */
        else if (message_set == MS_MAC_RR)
        {
          mac_grr_sig_id_t mac_message_id = (mac_grr_sig_id_t) message_id;

          if (mac_message_id == MAC_GRR_MEAS_RPT_CNF)
          {
            rr_mac_status_t rr_mac_status;
            mac_grr_meas_rpt_cnf_t *mac_grr_meas_rpt_cnf;

            rr_mac_status = rr_macinfo_get_mac_status(gas_id);

            mac_grr_meas_rpt_cnf = (mac_grr_meas_rpt_cnf_t *) new_message;

            if (mac_grr_meas_rpt_cnf->status == MAC_GRR_SUCCESS)
            {
              if(rr_mac_status == MAC_TRANSFER)
              {
                /* if we received the first successful PMR confirmation in transfer then*/
                /*set meas_rpt_sent_in_txfer flag*/
                if(rr_gprs_meas_rpt_control_data_ptr->meas_rpt_sent_in_txfer == FALSE)
                {
                  rr_gprs_meas_rpt_control_data_ptr->meas_rpt_sent_in_txfer = TRUE;
                }
              }
              /****************************************************/
              /* The last measurement repot has been successfully */
              /* sent by MAC, return to the acquire state         */
              /****************************************************/
              MSG_GERAN_LOW_0_G("PMR send has been successful");
            }
            else
            {
              /* to do - define action upon send failed */
              MSG_GERAN_MED_0_G("PMR send has failed");
            }

            /* change state - this may be altered below if a pending report is stored*/
            rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_ACQUIRE;

            /*****************************************************************/
            /* If a pending measurment report is stored then only send it if */
            /* the time to the next available report is greater than the set */
            /* threshold THRESHOLD_REPORTING_PERIOD. There is no point in    */
            /* trying to hold onto and send measurement reports if the freq  */
            /* between them is small, and another is due soon.               */
            /*****************************************************************/
            if ( rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.valid )
            {
              if ( ((rr_mac_status == MAC_IDLE) &&
                    (rr_gprs_get_nc_reporting_period_i(gas_id) > THRESHOLD_REPORTING_PERIOD)) ||
                   ((rr_mac_status == MAC_TRANSFER) &&
                    (rr_gprs_get_nc_reporting_period_t(gas_id) > THRESHOLD_REPORTING_PERIOD)))
              {
                /* send the report to mac */
                rr_send_grr_mac_send_meas_rpt(
                  rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.msg_type,
                                              &rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.msg,
                  rr_gprs_get_network_control_order(gas_id),
                  rr_gprs_get_nc_non_drx_period(gas_id),
                  gas_id
                );
                /* change the state to send */
                rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_SEND;
              }
              else
              {
                /* Don't bother to send the pending measurement report as another is due very soon */
                MSG_GERAN_LOW_0_G("clear out pending measurement store, next report due soon");
              }
              /* invalidate the stored report */
              rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.valid = FALSE;
            }
          }
          else
          {
            MSG_GERAN_ERROR_1_G("Unexpected message from MAC: %d", (int) message_id);
          }
        }
        else
        {
          MSG_GERAN_ERROR_2_G("Unexpected message: %d, %d", (int) message_set, (int) message_id);
        }
      }

      /**
       * Input event handlers.
       */

      /**
       * A set of PMO messages has been received.
       */
      else if (nc_meas_event == EV_PMO_MESSAGE)
      {
        new_nc_meas_event = rr_gprs_meas_rpt_control_handle_pmo_message(gas_id);
      }

      /**
       * A PCCO message has been received.
       */
      else if ( nc_meas_event == EV_PCCO_MESSAGE )
      {
        new_nc_meas_event = rr_gprs_meas_rpt_control_handle_pcco_message(gas_id);
      }

      /**
       * Timer T3158 has expired.  A Packet Measurement Report or Packet
       * Enhanced Measurement Report should be sent and T3158 restarted.
       */
      else if (nc_meas_event == EV_T3158_TIMEOUT)
      {
        /* compile the measurement report - this will be stored and not sent to MAC */
        (void)rr_gprs_format_meas_report(send_to_mac_forbidden, gas_id);
        /* Start the nc measurement timer again */
        (void)rr_gprs_start_t3158(FALSE, gas_id);
      }

      /**
       * New measurement parameters have been received in system information.
       */
      else if (nc_meas_event == RR_EV_MEASUREMENT_PARAMS_UPDATED)
      {
        if (!grr_nc_params_set_by_pmo_pcco(gas_id))
        {
          /*The measurement reporting system information has changed,
          and PMO/PCCO params are not in operation at the moment */
          new_nc_meas_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
        }
      }

      /***************************************************************************/
      /* EV_NCO_MODE_0                                                           */
      /* Measurement reporting is currently active, but a PMO has been recieved  */
      /* with the NCO set to 0. Cell reselection is autonomous and measurement   */
      /* turned off                                                              */
      /***************************************************************************/
      else if (nc_meas_event == EV_NCO_MODE_0)
      {
        /* turn off the nc measurements */
        rr_gprs_stop_nc_measurement_process(gas_id);
        /*Reset meas_rpt_sent_in_txfer flag*/
        rr_gprs_meas_rpt_control_data_ptr->meas_rpt_sent_in_txfer = FALSE;

        /*Set the state to NULL, measurement reporting is now switched off*/
        rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_NULL;

        /*Set the output event that will alert cell reselection of the PMO*/
        output_event = EV_CELL_RESELECTION_ACT;
        
#ifdef FEATURE_IDLE_DRX_SCALING
        rr_idle_drx_control(EV_NCO_MODE_0, NULL, gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */
      }

      /***************************************************************************/
      /* EV_NCO_MODE_1                                                           */
      /* Measurement reporting is currently active, but a PMO has been recieved  */
      /* with the NCO set to 1 therefore cell reselection is autonomous with nc  */
      /* measurement reporting on                                                */
      /***************************************************************************/
      else if (nc_meas_event == EV_NCO_MODE_1)
      {
        MSG_GERAN_HIGH_0_G("Going to NC1 in NC_MEAS_SEND");

        /*send the command to layer 1 to begin the measurement reporting*/
        rr_gprs_start_nc_measurement_process(TRUE, gas_id);

        /*Set the output event that will alert cell reselection of the PMO*/
        output_event = EV_CELL_RESELECTION_ACT;
      }

      /***************************************************************************/
      /* EV_NCO_MODE_2                                                           */
      /* Measurement reporting is currently active, but a PMO has been recieved  */
      /* with the NCO set to 2 therefore cell reselection is network controlled  */
      /* autonomous with nc measurement reporting on                             */
      /***************************************************************************/
      else if (nc_meas_event == EV_NCO_MODE_2)
      {
        MSG_GERAN_HIGH_0_G("Going to NC2 in NC_MEAS_SEND");
        /*send the command to layer 1 to begin the measurement reporting*/
        rr_gprs_start_nc_measurement_process(TRUE, gas_id);

        /*Set the output event that will deactivate cell reselection*/
        output_event = EV_CELL_RESELECTION_DEACT;
      }

      /***************************************************************************/
      /* RR_EV_DEACTIVATE_MEASUREMENTS                                                      */
      /* deactivate the measurement reporting, a cell reselection is taking place*/
      /***************************************************************************/
      else if (nc_meas_event == RR_EV_DEACTIVATE_MEASUREMENTS)
      {
        /* go to the suspend state */
        rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_SUSPEND;
        /*Reset meas_rpt_sent_in_txfer flag*/
        rr_gprs_meas_rpt_control_data_ptr->meas_rpt_sent_in_txfer = FALSE;
      }

      /***************************************************************************/
      /* RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13                                   */
      /* SI13 data is no longer valid, so assume GSM-only operation              */
      /***************************************************************************/
      else if (nc_meas_event == RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13)
      {
        rr_gprs_revert_ba_gprs(gas_id);
        rr_gprs_meas_rpt_control_reset(gas_id);

        new_nc_meas_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
      }
      break;

  /***************************************************************/
  /* NC_MEAS_SUSPEND a cell reselection is taking place so the   */
  /* measurement reporting is temporarily suspended              */
  /***************************************************************/
    case NC_MEAS_SUSPEND:
      /****************************************************************/
      /* EV_CAMP_ON_GPRS                                              */
      /* Cell reseleciton has completed. Check parameters for action. */
      /* then return to the correct state                             */
      /****************************************************************/
      if (nc_meas_event == RR_EV_MEASUREMENT_PARAMS_UPDATED)
      {
        MSG_GERAN_HIGH_0_G("Camped on cell after reselection, reading system info...");

        new_nc_meas_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
      }

      else if (nc_meas_event == RR_EV_CAMPED_ON_CELL)
      {
        MSG_GERAN_HIGH_0_G("Camped on cell after reselection, reading system info...");

        /* Camped on a new cell, so clear out any previous PMO modifications. */
        if (*params_mods != NULL)
        {
          GPRS_MEM_FREE(*params_mods);
          *params_mods = NULL;
        }

        rr_gprs_update_meas_params(gas_id);

        new_nc_meas_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
      }

      /*****************************************************************/
      /* EV_CAMP_ON_GRR52_PCCF_OLD_SCELL                               */
      /* A pcco cell reselection has failed and we have camped back on */
      /* the old scell. restore the nc setup on this scell             */
      /*****************************************************************/
      else if (nc_meas_event == EV_CAMP_ON_PCCF_OLD_SCELL ||
               nc_meas_event == EV_ABANDON_RESELECTION )
      {
        /* restore the old values for this old scell */
        new_nc_meas_event = rr_gprs_change_nc_measurement_process(OLD_SCELL_CAUSE, gas_id);
      }

      /***************************************************************************/
      /* EV_NCO_MODE_0                                                           */
      /* Measurement reporting was active on the old scell but the parameters for*/
      /* this scell require it to be off                                         */
      /***************************************************************************/
      else if (nc_meas_event == EV_NCO_MODE_0)
      {
        /* turn off the nc measurements */
        rr_gprs_stop_nc_measurement_process(gas_id);

        /*Set the state to NULL, measurement reporting is now switched off*/
        rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_NULL;

        /*Set the output event that will alert cell reselection of the current setting*/
        output_event = EV_CELL_RESELECTION_ACT;
        
#ifdef FEATURE_IDLE_DRX_SCALING
        rr_idle_drx_control(EV_NCO_MODE_0, NULL, gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */
      }

      /***************************************************************************/
      /* EV_NCO_MODE_1                                                           */
      /* NCO has been set to 1 so measurement reporting is required but with     */
      /* autonomous cell reselection. Start the measurement reporting and notify */
      /* cell reselection that a PMO has been recieved as GPRS parameters may be */
      /* available for some cells                                                */
      /***************************************************************************/
      else if (nc_meas_event == EV_NCO_MODE_1)
      {
        MSG_GERAN_HIGH_0_G("Going to NC1 in NC_MEAS_SUSPEND");

        /*send the command to layer 1 to begin the measurement reporting*/
        rr_gprs_start_nc_measurement_process(FALSE, gas_id);

        /*Set the state to acquire, measurement reporting is now active*/
        rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_ACQUIRE;

        /*Set the output event that will alert cell reselection of the PMO*/
        output_event = EV_CELL_RESELECTION_ACT;
      }

      /***************************************************************************/
      /* EV_NCO_MODE_2                                                           */
      /* NCO has been set to 2 so measurement reporting is required but with     */
      /* network controlled cell reselection. Start the measurement reporting    */
      /* the notify cell reselection.                                            */
      /***************************************************************************/
      else if (nc_meas_event == EV_NCO_MODE_2)
      {
        MSG_GERAN_HIGH_0_G("Going to NC2 in NC_MEAS_SUSPEND");
        /*send the command to layer 1 to begin the measurement reporting*/
        rr_gprs_start_nc_measurement_process(TRUE, gas_id);

        /*Set the state to acquire, measurement reporting is now active*/
        rr_gprs_meas_rpt_control_data_ptr->state = NC_MEAS_ACQUIRE;

        /*Set the output event that will deactivate cell reselection*/
        output_event = EV_CELL_RESELECTION_DEACT;
      }

      else if (nc_meas_event == RR_EV_DEACTIVATE_MEASUREMENTS)
      {
        ; /* safe to ignore this here as measurements are suspended */
      }

      /***************************************************************************/
      /* RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13                                   */
      /* SI13 data is no longer valid, so assume GSM-only operation              */
      /***************************************************************************/
      else if (nc_meas_event == RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13)
      {
        rr_gprs_revert_ba_gprs(gas_id);
        rr_gprs_meas_rpt_control_reset(gas_id);

        new_nc_meas_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
      }

      else if (nc_meas_event == EV_PMO_MESSAGE)
      {
        /* if a NACC reselection is in progress, this PMO may */
        /* change the NC mode, and so it must be handled here */
        if (rr_resel_is_in_ccn_reselection_state(gas_id))
        {
          new_nc_meas_event = rr_gprs_meas_rpt_control_handle_pmo_message(gas_id);
        }
      }
      else if (nc_meas_event == EV_PCCO_MESSAGE)
      {
        MSG_GERAN_LOW_0_G("PCCO in progress");
      }
      else if (nc_meas_event == EV_T3158_TIMEOUT)
      {
        /* Start the nc measurement timer again */
        (void)rr_gprs_start_t3158(FALSE, gas_id);
      }
      else
      {
        MSG_GERAN_ERROR_1_G("Unexpected Event: %d in NC_MEAS_SUSPEND", nc_meas_event);
      }

      break;

  /***************************************************************/
  /* INPUT EVENT handlers                                        */
  /***************************************************************/
    default:
      ;

      nc_meas_event = EV_NO_EVENT;
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((nc_meas_event != new_nc_meas_event) &&
         (new_nc_meas_event != EV_NO_EVENT)) ||
        (rr_gprs_meas_rpt_control_data_ptr->old_state != rr_gprs_meas_rpt_control_data_ptr->state))
    {
      rr_store_trace_msg_buf(
        GRR_NC_MMT_SM,
                             new_nc_meas_event,
        (byte)rr_gprs_meas_rpt_control_data_ptr->state,
        new_message,
        gas_id
      );
    }
#endif

    /* Check for a new event */
    if (new_nc_meas_event != EV_NO_EVENT)
    {
      nc_meas_event = new_nc_meas_event;
      new_nc_meas_event = EV_NO_EVENT;
    }
    else
    {
      nc_meas_event = EV_NO_EVENT;
    }

    if ( rr_gprs_meas_rpt_control_data_ptr->state != rr_gprs_meas_rpt_control_data_ptr->old_state )
    {
      RR_LOG_STATE_TRANSITION(
        "rr_gprs_nc_measurement_control",
        rr_gprs_meas_rpt_control_data_ptr->old_state,
        rr_gprs_meas_rpt_control_data_ptr->state,
        get_nc_measurement_control_state_name,
        gas_id
      );
    }
    rr_gprs_meas_rpt_control_data_ptr->old_state = rr_gprs_meas_rpt_control_data_ptr->state;
  }

  return output_event;
}


#if 0  // not used at present 
void rr_nc_meas_params_apply_mods(nc_measurement_params_t *dst, nc_meas_params_mods_t *mods)
{
  if (mods->valid)
  {
    dst->network_control_order = mods->network_control_order;
    if (mods->nc_period_valid)
    {
      dst->nc_non_drx_period = mods->nc_non_drx_period;
      dst->nc_reporting_period_i = mods->nc_reporting_period_i;
      dst->nc_reporting_period_t = mods->nc_reporting_period_t;
    }
  }
}
#endif // #if 0


#if 0  // not used at present 
void rr_gprs_meas_params_apply_mods(gprs_meas_params_t *dst, gprs_meas_params_mods_t *mods)
{
  if (mods->valid)
  {
    if (mods->multiband_reporting_valid)
    {
      dst->multiband_reporting = mods->multiband_reporting;
    }
    if (mods->serving_band_reporting_valid)
    {
      dst->serving_band_reporting = mods->serving_band_reporting;
    }
    dst->scale_ord = mods->scale_ord;
    if (mods->reporting_900_valid)
    {
      dst->reporting_900_offset = mods->reporting_900_offset;
      dst->reporting_900_threshold = mods->reporting_900_threshold;
    }
    if (mods->reporting_1800_valid)
    {
      dst->reporting_1800_offset = mods->reporting_1800_offset;
      dst->reporting_1800_threshold = mods->reporting_1800_threshold;
    }
    if (mods->reporting_450_valid)
    {
      dst->reporting_450_offset = mods->reporting_450_offset;
      dst->reporting_450_threshold = mods->reporting_450_threshold;
    }
    if (mods->reporting_1900_valid)
    {
      dst->reporting_1900_offset = mods->reporting_1900_offset;
      dst->reporting_1900_threshold = mods->reporting_1900_threshold;
    }
    if (mods->reporting_850_valid)
    {
      dst->reporting_850_offset = mods->reporting_850_offset;
      dst->reporting_850_threshold = mods->reporting_850_threshold;
    }
  }
}
#endif // #if 0


/****************************************************************************
 *   Function name:  rr_gprs_read_meas_rpt_sysinfo
 *   --------------
 *
 *   Description: This function reads the measurement report system info
 *   ------------ to decide whether to activate/ deactivate meas reporting
 *
 *
 *   Parameters: none
 *   -----------
 *
 *
 *   Returns:  an event to calling FSM/function
 *   --------
 *
 *
 ****************************************************************************/
static rr_event_T rr_gprs_read_meas_rpt_sysinfo(const gas_id_t gas_id)
{
  return rr_gprs_process_nc_measurement_params(gas_id);
}


/****************************************************************************
 *   Function name:  rr_gprs_process_nc_measurement_params
 *   --------------
 *
 *   Description: This function looks at the nco to decide if measurement
 *   ------------ reporting should be performed or stopped etc
 *
 *
 *   Parameters: none
 *   -----------
 *
 *
 *   Returns:  an event to calling FSM/function
 *   --------
 *
 *
 ****************************************************************************/
static rr_event_T rr_gprs_process_nc_measurement_params(const gas_id_t gas_id)
{
  gmm_llc_if_state_type gmm_state;
  rr_event_T            output_event = EV_NO_EVENT;

  switch (rr_gprs_get_network_control_order(gas_id))
  {
  case NC_MEAS_NC0:
    /**********************************************************************/
    /* The Network Control Order is set to 0 No measurement reporting and */
    /* autonomous cell reselection                                        */
    /**********************************************************************/
    output_event = EV_NCO_MODE_0;
    break;

  case NC_MEAS_NC1:
    /**********************************************************************/
    /* The Network Control Order is set to 1 if GMM is in the ready state */
    /* then activate measurement reporting.                               */
    /**********************************************************************/
    gmm_state = rr_gmminfo_get_gmm_status(gas_id);
    if (gmm_state == GMM_READY_STATE)
    {
      output_event = EV_NCO_MODE_1;
    }
    else
    {
      MSG_GERAN_MED_1_G("GMM state is not READY(%d): unable to change to NC1 mode",
              (int)gmm_state);
      output_event = EV_NCO_MODE_0;
    }
    break;

  case NC_MEAS_NC2:
    /**********************************************************************/
    /* The Network Control Order is set to 2 if GMM is in the ready state */
    /* then activate measurement reporting, and deactivate MS controlled  */
    /* cell reselection                                                   */
    /**********************************************************************/
    gmm_state = rr_gmminfo_get_gmm_status(gas_id);
    if (gmm_state == GMM_READY_STATE)
    {
      output_event = EV_NCO_MODE_2;
    }
    else
    {
      MSG_GERAN_MED_1_G("GMM state is not READY(%d): unable to change to NC2 mode",
              (int)gmm_state);
      output_event = EV_NCO_MODE_0;
    }
    break;

  case NC_MEAS_RESET:
    MSG_GERAN_HIGH_0_G("Revert to broadcast parameters");
    /*************************************************************************/
    /* The Network Control Order is set to RESET so go back to the broadcast */
    /* parameters in PSI's or SI13                                           */
    /*************************************************************************/

    /* clear pcco/pmo modifications to meas params */
    rr_gprs_meas_rpt_control_reset_modifications(gas_id);

    /* generate meas params from system info */
    rr_gprs_update_meas_params(gas_id);

    /* Validate that the new nco is valid. */
    if (!rr_network_control_order_valid(rr_gprs_get_network_control_order(gas_id)))
    {
      MSG_GERAN_ERROR_1_G("Unexpected network_control_order %d, resetting to NC0",
                rr_gprs_get_network_control_order(gas_id));

      rr_network_control_order_reset(gas_id);
    }

    /* call this function again as network_control_order is now guaranteed
    to be either 0, 1 or 2. */
    output_event = rr_gprs_process_nc_measurement_params(gas_id);
    break;

  default:
    MSG_GERAN_HIGH_0_G("NC mode unrecognised !!");
  }

  return output_event;
}


/****************************************************************************
 *   Function name:  rr_gprs_start_nc_measurement_process
 *   --------------
 *
 *   Description: This function starts the nc measurement process by starting
 *   ------------ the timer T3158 and sending the request to L1
 *
 *
 *   Parameters: none
 *   -----------
 *
 *
 *   Returns:  none
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_start_nc_measurement_process(boolean bActive, const gas_id_t gas_id)
{
  uint8                          nc_index, ba_index;
  t3158_action_t                 t3158_action;
  inter_task_BCCH_allocation_T * ba_list = rr_gprs_get_ba_gprs(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  /* Start the nc measurement timer t3158 */
  t3158_action = rr_gprs_start_t3158(FALSE, gas_id);

  if ( t3158_action == t3158_restarted )
  {
    /* Send the request to L1 */
    rr_send_mph_nc_measurements_req(ba_list, gas_id);

    /* clean out the nc_measurement data structure */
    rr_gprs_meas_rpt_control_reset_measurements(gas_id);
  }
  else if ( t3158_action == t3158_allow_to_run &&
            rr_timer_is_running(T3174, gas_id)
          )
  {
    nc_measurement_data_t old_nc_measurement_data;
    ARFCN_T               scell_arfcn = rr_gprs_get_scell_arfcn(gas_id);

    old_nc_measurement_data = rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data;
    

    rr_gprs_meas_rpt_control_reset_measurements(gas_id);

    /***********************************************************************/
    /* If a PCCO has ordered a reselection, and the T3158 timer is allowed */
    /* to carry on running then the measurements taken on the old cell are */
    /* used on the new cell if the arfcns are in the new cells ba list.    */
    /*                                                                     */
    /* old scell BAlist = 10,20,30,40,50,60,66                             */
    /* new scell BAlist = 20,30,40,50,60,85,77                             */
    /*                                                                     */
    /*                                                                     */
    /* All measurements for ARFCNs 10 and 66 are removed from the ncell    */
    /* measurment store. The measurements taken for arfcn 10 which is now  */
    /* the scell are placed into the scell measurement store               */
    /***********************************************************************/
    
    /* Find the scell measurements in the ncell database and move them over to the temp store */
    for (nc_index = 0;
         (nc_index < old_nc_measurement_data.no_of_ncell_entries)&& (nc_index < BA_MAX_LEN);
         ++nc_index)
    {
      if (ARFCNS_EQUAL(old_nc_measurement_data.nc_measurements[nc_index].bcch_arfcn, scell_arfcn))
      {
        /* Copy over the ncell data to the scell area. */
        rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_arfcn = old_nc_measurement_data.nc_measurements[nc_index].bcch_arfcn;
        rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_total = old_nc_measurement_data.nc_measurements[nc_index].rxlev_total;
        rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count = old_nc_measurement_data.nc_measurements[nc_index].rxlev_count;
        rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_ilevel_count = 0;
        rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_ilevel_total = 0;

        /* Move the old serving cell data in to the old neighbour cell
        measurements store so that it can be re-used if the old serving cell
        is in the new serving cell's BA list (as checked for after this for-loop). */
        old_nc_measurement_data.nc_measurements[nc_index].rxlev_total = old_nc_measurement_data.scell_rxlev_total;
        old_nc_measurement_data.nc_measurements[nc_index].rxlev_count = old_nc_measurement_data.scell_rxlev_count;
        old_nc_measurement_data.nc_measurements[nc_index].bcch_arfcn = old_nc_measurement_data.scell_arfcn;

        /* Don't need to clean out the bsic just set valid info to FALSE. */
        old_nc_measurement_data.nc_measurements[nc_index].valid_info = FALSE;
        break;
      }
    }

    /* Iterate over the old measurements taken while on the previous serving
    cell.  Restore any measurements for cells which are present in the BA list
    of the new serving cell. */
    for (nc_index = 0;
         (nc_index < old_nc_measurement_data.no_of_ncell_entries) && (nc_index < BA_MAX_LEN);
         ++nc_index)
    {
      /* there may be empty elements so don't bother with these*/
      if (old_nc_measurement_data.nc_measurements[nc_index].bcch_arfcn.num == 0)
      {
        break;
      }

      for (ba_index = 0;
           (ba_index < ba_list->no_of_entries) && (ba_index < BA_MAX_LEN);
           ++ba_index)
      {
        if (ARFCNS_EQUAL(ba_list->BCCH_ARFCN[ba_index], old_nc_measurement_data.nc_measurements[nc_index].bcch_arfcn))
        {
          rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.no_of_ncell_entries] =
            old_nc_measurement_data.nc_measurements[nc_index];

          rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.no_of_ncell_entries++;
          break;
        }
      }
    }
  }
  else if (bActive)
  {
    /* force an update of the NC measurement list in L1 */
    MSG_GERAN_MED_0_G("Updating NC measurement request list");
    rr_send_mph_nc_measurements_req(ba_list, gas_id);
  }
}




/****************************************************************************
 *   Function name:  rr_gprs_change_nc_measurement_process
 *   --------------
 *
 *   Description: invalidates the individual nc parameters given in a pmo, or
 *   ------------ changes the BA list depending upon the cause value. This is
 *                as defined in 05.08 - 10.1.4
 *                The individual parameters...
 *
 *                NC_REPORTING_PERIOD_I
 *                NC_REPORTING_PERIOD_T
 *                NC_NON_DEX_PERIOD
 *                NETWORK_CONTROL_ORDER
 *
 *                become invalid upon a DSF, gmm moving out of ready state or
 *                another pmo with RESET as the NCO value.
 *
 *                The broadcast BA list is used again if there is a DSF, another
 *                PMO is recieved with an empty list or the MS selects another cell.
 *
 *
 *   Parameters: cause value
 *   -----------
 *
 *
 *   Returns:  rr_event
 *   --------
 *
 *
 ****************************************************************************/
static rr_event_T rr_gprs_change_nc_measurement_process(cause_t cause, const gas_id_t gas_id)
{
  rr_event_T output_event = EV_NO_EVENT;

  /*************************************************************/
  /* Depending upon the cause the ba list or the nc parameters */
  /* sent to the ms in a pmo may be invalidated                */
  /*************************************************************/
  switch ( cause )
  {
    /**
     * A DSF will cause all NC Measurement Parameters an BA(GPRS)
     * to be reset to default values.
     */
    case DSF_CAUSE:
    {
      rr_gprs_revert_ba_gprs(gas_id);
      rr_gprs_meas_rpt_control_reset_modifications(gas_id);
      output_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
    }
    break;

    /**
     * A PMO with NCO == RESET will cause all parameters and BA list
     * to be set to broadcast values.
     */
    case PMO_RESET_CAUSE:
    {
      rr_gprs_revert_ba_gprs(gas_id);
      rr_gprs_meas_rpt_control_reset_modifications(gas_id);
      output_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
    }
    break;

    /**
     * GMM entering STANDBY state will cause NC Measurement Parameters
     * to be returned to broadcast values.  BA(GPRS) is left modified?
     */
    case GMM_STANDBY_CAUSE:
    {
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
      rr_params_mods_t ** params_mods = rr_params_mods(gas_id);
      /*CR550108*/  
      pmo_pcco_data_t *pmo_store_ptr = rr_get_pmo_data(gas_id);
      /*CR550108*/
      boolean             update_l1 = FALSE;

      if (params_mods != NULL && *params_mods != NULL)
      {
        if ((*params_mods)->gprs_3g_meas_params.valid)
        {
          update_l1 = TRUE;
        }
      }
      if( (!rr_is_pmo_nc_frequency_list_valid(gas_id)) &&
	  	  ((pmo_store_ptr->nc_meas_params.valid))
	  	 ) 
      {
        MSG_GERAN_MED_0_G("In NC2 mode: priority reselection  was not applicable hence when moving to normal mode update L1 with the list");
        update_l1 = TRUE;	

      }
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

      rr_gprs_meas_rpt_control_reset_modifications(gas_id);
      output_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
      if (!rr_gsm_only(gas_id))
      {
        if (update_l1)
        {
          rr_l1_update_wcdma_neighbor_list(gas_id);
        }
      }

#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */
    }
    break;

    /**
     * OLD_SCELL_CAUSE - should only do this if reselection has been
     * abandoned.  Just need to wipe out the PCCO modifications.
     */
    case OLD_SCELL_CAUSE:
    {
      rr_params_mods_t ** pcco_params_mods = rr_pcco_params_mods(gas_id);

      if (pcco_params_mods != NULL && *pcco_params_mods != NULL)
      {
        GPRS_MEM_FREE(*pcco_params_mods);
        *pcco_params_mods = NULL;
      }
      output_event = rr_gprs_read_meas_rpt_sysinfo(gas_id);
    }
    break;

    default:
      MSG_GERAN_ERROR_1_G("Unknown cause: %d", cause);
  }
  return output_event;
}


/****************************************************************************
 *   Function name:  rr_gprs_stop_nc_measurement_process
 *   --------------
 *
 *   Description: This function starts the nc measurement process by starting
 *   ------------ the timer T3158 and sending the request to L1
 *
 *
 *   Parameters: none
 *   -----------
 *
 *
 *   Returns:  none
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_stop_nc_measurement_process(const gas_id_t gas_id)
{
  inter_task_BCCH_allocation_T * ba_list = rr_gprs_get_ba_gprs(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  /* Stop the timer t3158 */
  (void)rr_timer_cancel(T3158, gas_id);
  MSG_GERAN_LOW_0_G("Stop the NC timer T3158");

  /* Send the stop request to L1 */
  rr_send_mph_stop_nc_measurements_req(ba_list, gas_id);

  /* invalidate any pending measurment report */
  rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.valid = FALSE;
}


/****************************************************************************
 *   Function name:  rr_gprs_store_nc_measurements
 *   --------------
 *
 *   Description: This function stores the measurements that have been made by
 *   ------------ L1 on the nc frequencies requested. The results must be avaraged
 *                over the reporting period, this average is done on the timer expiry.
 *                All incoming measurements will have the bsic validated either by comparing
 *                with the given bsic if on pbcch, or by checking against the ncc_permitted
 *                bitmap if on bcch.
 *
 *   Parameters: pointer to mph_nc_measurement_ind_t
 *   -----------
 *
 *
 *   Returns:  none
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_store_nc_measurements(
  mph_nc_measurement_ind_t *nc_measurement_ind,
  const gas_id_t gas_id
)
{
  uint8 index, element_index;
  nc_measurement_element_t *nc_measurement_element = NULL;
  ARFCN_T bcch_arfcn;
  uint8 ncc_mask,ncc_permitted;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  nc_measurement_data_t * nc_measurement_data;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  nc_measurement_data = &rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data;

    /***********************************************************************/
    /* Set all of the entries to have valid info as false. This means that */
    /* none of the entries are currently sync'ed up to by L1. When the nc  */
    /* measurements are processed the valid info will be set to its correct*/
    /* value. The idea is that it is assumed that no cells are sync'ed to  */
    /* and the values in the lastest measurement report will correct this  */
    /* giving the lastest picture.                                         */
    /***********************************************************************/
    for (element_index = 0;
         (element_index < nc_measurement_data->no_of_ncell_entries) &&
         (element_index < BA_MAX_LEN);
         ++element_index)
    {
      nc_measurement_data->nc_measurements[element_index].valid_info = FALSE;
    }

    for (index = 0;
         (index < nc_measurement_ind->no_of_entries) &&
         (index < BA_MAX_LEN);
         ++index)
    {
      bcch_arfcn = nc_measurement_ind->surrounding_channel_info[index].BCCH_ARFCN;

      /* Locate the arfcn within the nc_measurement_data structure*/
      for (element_index = 0;
           (element_index < nc_measurement_data->no_of_ncell_entries) &&
           (element_index < BA_MAX_LEN);
           ++element_index)
      {
        if (ARFCNS_EQUAL(nc_measurement_data->nc_measurements[element_index].bcch_arfcn, bcch_arfcn))
        {
          nc_measurement_element = &nc_measurement_data->nc_measurements[element_index];
          break;
        }
        else
        {
          /*Reset the pointer to NULL there is no entry for this yet*/
          nc_measurement_element = NULL;
        }
      }

      /* Create a new entry for this arfcn */
      if (nc_measurement_element == NULL)
      {
        if (nc_measurement_data->no_of_ncell_entries < BA_MAX_LEN)
        {
          /* There is no entry for the current arfcn this add this entry to the end */
          nc_measurement_data->nc_measurements[nc_measurement_data->no_of_ncell_entries].bcch_arfcn = bcch_arfcn;
          nc_measurement_element = &nc_measurement_data->nc_measurements[nc_measurement_data->no_of_ncell_entries];
          nc_measurement_data->no_of_ncell_entries++;
        }
        else
        {
          MSG_GERAN_ERROR_0("Overflow detected while storing NC measurements");
        }
      }

      if (nc_measurement_element != NULL)
      {
        /****************************************************************/
        /* Begin storing the results into the nc measurement store area */
        /****************************************************************/
  
        /* Add the rxlev into the structure */
        nc_measurement_element->rxlev_total += nc_measurement_ind->surrounding_channel_info[index].RXLEV_average;
  
        /* Increment the count that will be used for averaging */
        nc_measurement_element->rxlev_count++;
  
        /* If BSIC has been decoded store this data */
        if (nc_measurement_ind->surrounding_channel_info[index].valid_info)
        {
          /**************************************************************************/
          /* Store the BSIC then mark it a valid or invalid using check that follow */
          /**************************************************************************/
          /* Load up the BSIC into the nc_data element */
          nc_measurement_element->bsic.PLMN_colour_code =
                    DECODE_NCC_DIRECT((nc_measurement_ind->surrounding_channel_info[index].SCH_block));
          nc_measurement_element->bsic.BS_colour_code =
                    DECODE_BCC_DIRECT((nc_measurement_ind->surrounding_channel_info[index].SCH_block));
  
          /* Check for the BSIC match using the ncc permitted */
          ncc_mask = (uint8) (0x01 << nc_measurement_element->bsic.PLMN_colour_code);
          ncc_permitted = scell_info_ptr->gsm.NCC_permitted;
          if (( ncc_permitted & ncc_mask ) != 0)
          {
            nc_measurement_element->valid_info = TRUE;
          }
          else
          {
            MSG_GERAN_HIGH_0_G("NCC permitted mismatch");
            nc_measurement_element->valid_info = FALSE;
          }
        }
        else
        {
          MSG_GERAN_HIGH_0_G("NO BSIC to compare");
          nc_measurement_element->valid_info = FALSE;
        }
      }
    }
}

/****************************************************************************
 *   Function name:  rr_gprs_store_serving_measurements
 *   --------------
 *
 *   Description: This function stores the measurements that have been made by
 *   ------------ L1 on the serving frequency. The results must be averaged
 *                over the reporting period.
 *
 *
 *   Parameters: pointer to mph_serving_meas_ind_t
 *   -----------
 *
 *
 *   Returns:  none
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_store_serving_measurements(
  mph_serving_meas_ind_t *serving_measurements,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  /* store the ARFCN of the serving cell */
  rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_arfcn = scell_info_ptr->gsm.BCCH_ARFCN;

  /* save the serving measurements in the global NC structure */
  if (rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count == 0)
  {
    /* overwrite whatever is in the store already */
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_total = serving_measurements->rxlev_average;
  }
  else
  {
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_total += serving_measurements->rxlev_average;
  }
  /* increment the RXLEV count */
  rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count++;

  if (serving_measurements->i_level_valid)
  {
    if (rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_ilevel_count == 0)
    {
      /* overwrite whatever is in the store already */
      rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_ilevel_total = serving_measurements->i_level;
    }
    else
    {
      rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_ilevel_total += serving_measurements->i_level;
    }
    /* increment the interference level count */
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_ilevel_count++;
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_get_serving_measurements
 *   --------------
 *
 *   Description: This function returns the measurements that have been made by
 *   ------------ L1 on the serving frequency. The results is averaged
 *                over the reporting period.
 *
 *
 *   Parameters: N/A
 *   -----------
 *
 *
 *   Returns:  serving cell's measurement in rr_l1_freq_power_T
 *   --------
 *
 *
 ****************************************************************************/
rr_l1_freq_power_T rr_gprs_get_serving_measurements(const gas_id_t gas_id)
{
  rr_l1_freq_power_T            meas;
  gprs_reselection_structure_T *gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  meas.arfcn = rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_arfcn;

  if (rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count > 0)
  {
    meas.RXLEV_average = (byte) (rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_total / rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count);
  }
  else if (gprs_reselection_list_ptr->serving_values_valid)
  {
    meas.RXLEV_average = gprs_reselection_list_ptr->serving_values.RXLEV_average;
  }
  else
  {
    /* There have no valid serving cell measurement, assuming -80dbm */
    meas.RXLEV_average = 30;
  }

  return meas;
}


static boolean rr_gprs_complete_gprs_bsic_description_received(const gas_id_t gas_id)
{
  gprs_ncells_info_t *ncells = rr_gprs_get_ncells_info(gas_id);
  uint32 i;

  for (i = 0; i < ncells->gsm_neighbour_cell_list.num_of_ncells; i++)
  {
    /* Removed / duplicate cells are not considered as they are not reported.
    Added cells (from PCCO / PMO) are not checked because they always have
    a BSIC allocated, it is mandatory in the message structures. */
    if (ncells->gsm_neighbour_cell_list.ncells[i].status != NCELL_REMOVED &&
        ncells->gsm_neighbour_cell_list.ncells[i].status != NCELL_DUPLICATE &&
        ncells->gsm_neighbour_cell_list.ncells[i].status != NCELL_ADDED)
    {
      if (ncells->gsm_neighbour_cell_list.ncells[i].ncell_data.bcch.bsic_description.num_of_bsics == 0)
      {
        /* If we've received no BSIC for this cell, return FALSE */
        MSG_GERAN_ERROR_1_G("Not received GPRS BSIC Description for NCELL at index %d", i);
        return FALSE;
      }
    }
  }
  return TRUE;
}

/**
 * Returns the network-specified Reporting Threshold for the specified band.
 */
static byte get_reporting_threshold(sys_band_T band, const gas_id_t gas_id)
{
  gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);

  switch (band)
  {
  case SYS_BAND_EGSM_900:
  case SYS_BAND_PGSM_900:
    return (byte) (scell->gprs_meas_params.reporting_900_threshold * 6);
  case SYS_BAND_PCS_1900:
    return (byte) (scell->gprs_meas_params.reporting_1900_threshold * 6);
  case SYS_BAND_DCS_1800:
    return (byte) (scell->gprs_meas_params.reporting_1800_threshold * 6);
  case SYS_BAND_CELL_850:
    return (byte) (scell->gprs_meas_params.reporting_850_threshold * 6);
  default:
    MSG_GERAN_ERROR_1_G("get_reporting_threshold(): unknown band %d", band);
    return 0;
  }
}

/**
 * Add up to max_cells best GSM cells in band to the PEMR struct.  Returns
 * the number of cells added.
 */
static uint32 add_cells_to_pemr(
  packet_enhanced_measurement_report_t *pemr,
                                sys_band_T band,
                                uint32 max_cells,
  byte threshold,
  const gas_id_t gas_id
)
{
  uint32 num_of_cells_added = 0;
  uint32 i;
  nc_measurement_data_t * nc_measurement_data;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  MSG_GERAN_HIGH_3_G("Adding up to %d cells in band %d to PEMR, threshold: %d", max_cells, band, threshold);

  nc_measurement_data = &rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data;

  for (i = 0;
       (i < nc_measurement_data->no_of_ncell_entries) &&
       (num_of_cells_added < max_cells);
       ++i)
  {
    boolean band_matches = FALSE;

    if (!nc_measurement_data->nc_measurements[i].valid_info)
    {
      continue;
    }

    MSG_GERAN_MED_3_G("Checking if neighbour cell index %d (ARFCN %d) is in band %d",
      i,
      nc_measurement_data->nc_measurements[i].bcch_arfcn.num,
      band);

    if (nc_measurement_data->nc_measurements[i].bcch_arfcn.band == band)
    {
      band_matches = TRUE;
      MSG_GERAN_MED_0_G("yes");
    }
    else
    {
      MSG_GERAN_MED_0_G("no");
    }

    if (band_matches == FALSE && band == SYS_BAND_EGSM_900)
    {
      MSG_GERAN_MED_3_G("Checking if neighbour cell index %d (ARFCN %d) is in band %d",
        i,
        nc_measurement_data->nc_measurements[i].bcch_arfcn.num,
        SYS_BAND_PGSM_900);

      if (nc_measurement_data->nc_measurements[i].bcch_arfcn.band == SYS_BAND_PGSM_900)
      {
        MSG_GERAN_MED_0_G("yes");
        band_matches = TRUE;
      }
      else
      {
        MSG_GERAN_MED_0_G("no");
      }
    }

    if (band_matches)
    {
      if (nc_measurement_data->nc_measurements[i].rxlev_total >= threshold)
      {
        byte ba_index;

        MSG_GERAN_MED_0_G("band matches and rxlev_total meets threshold criteria, add cell to report");
        /**
         * WARNING :: rr_gprs_find_ba_gprs_index doesn't give us a useful way
         * of determining whether it actually found the cell or not.  It will
         * end up returning the size of the GSM ncell list if it doesn't find
         * the cell that we're looking for.
         *
         * For the time being, we're just going to report this anyway.  The function
         * should be rewritten at some point to return a better value in case of error.
         */
        ba_index = rr_gprs_find_ba_gprs_index(
          nc_measurement_data->nc_measurements[i].bcch_arfcn,
          NULL,
          gas_id
        );

        if (ba_index < MAX_REPORTING_QUANTITY)
        {
          pemr->reporting_quantity[ba_index].valid = TRUE;
          pemr->reporting_quantity[ba_index].value = (byte) nc_measurement_data->nc_measurements[i].rxlev_total;
          rr_adjust_rxlev_by_scale(pemr->scale, &pemr->reporting_quantity[ba_index].value);
          num_of_cells_added++;
        }
        else
        {
          MSG_GERAN_ERROR_1_G("ba_index out of range: %d", ba_index);
        }
      }
      else
      {
        MSG_GERAN_MED_3_G("band matches, but no rxlev_total %d does not exceed threshold %d",
          nc_measurement_data->nc_measurements[i].rxlev_total,
          threshold,
          0
        );
      }
    }
  }

  return num_of_cells_added;
}

static int compare_nc_measurement_element_by_rxlev_total(const void *element1, const void *element2)
{
  return ((nc_measurement_element_t *)(element1))->rxlev_total <
         ((nc_measurement_element_t *)(element2))->rxlev_total;
}

static boolean rr_gprs_band_in_ba_list(sys_band_T band, const gas_id_t gas_id)
{
  gprs_ncells_info_t *ncells = rr_gprs_get_ncells_info(gas_id);
  uint32 i;

  for (i = 0; i < ncells->gsm_neighbour_cell_list.num_of_ncells; i++)
  {
    if ((ncells->gsm_neighbour_cell_list.ncells[i].status == NCELL_ORIGINAL) ||
        (ncells->gsm_neighbour_cell_list.ncells[i].status == NCELL_ADDED)    ||
        (ncells->gsm_neighbour_cell_list.ncells[i].status == NCELL_ADDED_NC2_ONLY))
    {
      if (ncells->gsm_neighbour_cell_list.ncells[i].arfcn.band == band)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

static byte rr_get_scale_for_pemr(const gas_id_t gas_id)
{
  byte scale = 0;
  gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);

  scale = scell->gprs_meas_params.scale_ord;

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
  @brief Determines if the specified BSIC is valid for the specified ARFCN.

  @warning This function is only expected to be called when camped on a BCCH,
  not on a PBCCH.
  
  @return TRUE if the BSIC is valid for the ARFCN, FALSE otherwise.
*/
static boolean rr_pemr_is_bsic_valid(
  ARFCN_T arfcn,
  BSIC_T  bsic,
  const gas_id_t gas_id
)
{
  gsm_ncell_list_member_t * ncell;
  
  ncell = rr_gprs_get_ncell_info(arfcn, NULL, gas_id);

  if (ncell)
  {
    if (ncell->bcch_or_pmo == BCCH_FORMAT)
    {
      bcch_ncell_info_t * bcch_ncell_info = &ncell->ncell_data.bcch;

      uint32 i;
      for (i = 0;
           i < bcch_ncell_info->bsic_description.num_of_bsics &&
           i < GSM_BSIC_DESCRIPTION_MAX_BSICS_PER_ENTRY;
           ++i)
      {
        if (SAME_BSIC(bsic, bcch_ncell_info->bsic_description.bsic[i]))
        {
          return TRUE;
        }
      }

      return FALSE;
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Cell is PBCCH, BSIC check fails");
      return FALSE;
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("ncell_info not found");
    return FALSE;
  }
}

/**
  @brief Determines if any of the six strongest GSM cells has an invalid BSIC.

  This function assumes that the neighbour cell measurements in the measurement
  data are sorted in descending order of RXLEV.

  @return TRUE if any of the six strongest GSM cells has an invalid BSIC,
          FALSE otherwise.
*/
static boolean rr_pemr_invalid_bsic_seen(
  nc_measurement_data_t * measurement_data,
  const gas_id_t gas_id
)
{
  uint32 i;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  for (i = 0;
       i < measurement_data->no_of_ncell_entries &&
       i < 6; /* Check the six strongest cells only. */
       ++i)
  {
    if (measurement_data->nc_measurements[i].valid_info)
    {
      if (!rr_pemr_is_bsic_valid(
             measurement_data->nc_measurements[i].bcch_arfcn,
             measurement_data->nc_measurements[i].bsic,
             gas_id
           ))
      {
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
 * Encode a Packet Enhanced Measurement Report.
 */
static boolean encode_pemr(
  const pmr_send_t          pmr_send_instruction,
  const gprs_scell_info_t * scell,
  const gas_id_t            gas_id
)
{
  uint32                         i;
  uint32                         unused_positions = 0;
  uint32                         total_num_cells_added = 0;
  gprs_reselection_structure_T * gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  boolean gsm_850_in_ba_list = rr_gprs_band_in_ba_list(SYS_BAND_CELL_850, gas_id);
  boolean gsm_900_in_ba_list = rr_gprs_band_in_ba_list(SYS_BAND_EGSM_900, gas_id) ||
                               rr_gprs_band_in_ba_list(SYS_BAND_PGSM_900, gas_id);
  boolean gsm_1800_in_ba_list = rr_gprs_band_in_ba_list(SYS_BAND_DCS_1800, gas_id);
  boolean gsm_1900_in_ba_list = rr_gprs_band_in_ba_list(SYS_BAND_PCS_1900, gas_id);

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  rr_gprs_meas_rpt_control_data_ptr->pemr = &rr_gprs_meas_rpt_control_data_ptr->msg_pemr.pemr;

  memset(rr_gprs_meas_rpt_control_data_ptr->pemr, 0, sizeof(packet_enhanced_measurement_report_t));

  /* If serving cell information is valid, average its measurements and
  insert in to the measurement report. */
  if (rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count > 0)
  {
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_total /= rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count;
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count = 1;
  }
  else if (gprs_reselection_list_ptr->serving_values_valid)
  {
    MSG_GERAN_MED_0_G("Using serving cell measurements received before NC2...");
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_total = gprs_reselection_list_ptr->serving_values.RXLEV_average;
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count = 1;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("No serving cell rxlev available, not including scell in PEMR");
  }

  if (rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count > 0)
  {
    rr_gprs_meas_rpt_control_data_ptr->pemr->serving_cell_data_valid = TRUE;
    rr_gprs_meas_rpt_control_data_ptr->pemr->serving_cell_data.rxlev = (uint8) rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_total;
    rr_adjust_rxlev_by_scale(rr_gprs_meas_rpt_control_data_ptr->pemr->scale, &rr_gprs_meas_rpt_control_data_ptr->pemr->serving_cell_data.rxlev);
  }

  /**
   * Average the neighbour cell measurement data and filter out any serving cell
   * measurements that are present.
   */
  for (i = 0; (i < rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.no_of_ncell_entries) && (i < BA_MAX_LEN); i++)
  {
    if ((rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[i].valid_info) &&
        (rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[i].rxlev_count > 0))
    {
      rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[i].rxlev_total /= rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[i].rxlev_count;
      rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[i].rxlev_count = 1;
    }

    if (ARFCNS_EQUAL(rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[i].bcch_arfcn, scell->gsm.BCCH_ARFCN))
    {
      MSG_GERAN_HIGH_0_G("Serving cell entry removed from neighbour cell measurement data!");
      rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[i].valid_info = FALSE;
    }
  }

  /* sort the cells in the nc_measurement_data structure */
  qsort(
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements,
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.no_of_ncell_entries,
    sizeof(nc_measurement_element_t),
    compare_nc_measurement_element_by_rxlev_total
  );

  rr_gprs_meas_rpt_control_data_ptr->pemr->bsic_seen = rr_pemr_invalid_bsic_seen(
    &rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data,
    gas_id
  );
  rr_gprs_meas_rpt_control_data_ptr->pemr->scale = rr_get_scale_for_pemr(gas_id);

  /* 8.4.8.1 - Report Priority */

  /**
   * 1) the number of strongest GSM cells with known and valid BSIC and with
   * a reported value equal or greater than XXX_REPORTING_THRESHOLD, in the
   * frequency band of the serving cell, according to the value of the
   * parameter SERVING_BAND_REPORTING.
   */
  if (scell->gprs_meas_params.serving_band_reporting > 0)
  {
    sys_band_T serving_band = rr_gprs_get_scell_arfcn(gas_id).band;

    uint32 num_of_cells_added = add_cells_to_pemr(
      rr_gprs_meas_rpt_control_data_ptr->pemr,
                                                  serving_band,
                                                  scell->gprs_meas_params.serving_band_reporting,
      get_reporting_threshold(serving_band, gas_id),
      gas_id
    );

    total_num_cells_added += num_of_cells_added;

    MSG_GERAN_MED_3_G("Added %d cells from serving band (serving_band_reporting: %d)",
            num_of_cells_added,
            scell->gprs_meas_params.serving_band_reporting,
            0);

    unused_positions += scell->gprs_meas_params.serving_band_reporting - num_of_cells_added;

    switch (serving_band)
    {
    case SYS_BAND_CELL_850:
      gsm_850_in_ba_list = FALSE;
      break;
    case SYS_BAND_EGSM_900:
    case SYS_BAND_PGSM_900:
      gsm_900_in_ba_list = FALSE;
      break;
    case SYS_BAND_DCS_1800:
      gsm_1800_in_ba_list = FALSE;
      break;
    case SYS_BAND_PCS_1900:
      gsm_1900_in_ba_list = FALSE;
      break;
    default:
      MSG_GERAN_ERROR_1_G("Unknown serving cell band: %d", serving_band);
    }
  }

  /**
   * 2) the number of strongest GSM cells with known and valid BSIC and with
   * a reported value equal or greater than XXX_REPORTING_THRESHOLD, in each
   * of the frequency bands in the BA list, excluding the frequency band of
   * the serving cell, according to the value of the parameter
   * MULTIBAND_REPORTING;
   */

  if (gsm_850_in_ba_list)
  {
    uint32 num_of_cells_added = add_cells_to_pemr(
      rr_gprs_meas_rpt_control_data_ptr->pemr,
                             SYS_BAND_CELL_850,
                             scell->gprs_meas_params.multiband_reporting,
      get_reporting_threshold(SYS_BAND_CELL_850, gas_id),
      gas_id
    );

    total_num_cells_added += num_of_cells_added;

    unused_positions += scell->gprs_meas_params.multiband_reporting - num_of_cells_added;
  }

  if (gsm_900_in_ba_list)
  {
    uint32 num_of_cells_added = add_cells_to_pemr(
      rr_gprs_meas_rpt_control_data_ptr->pemr,
                               SYS_BAND_EGSM_900,
                               scell->gprs_meas_params.multiband_reporting + unused_positions,
      get_reporting_threshold(SYS_BAND_EGSM_900, gas_id),
      gas_id
    );

    total_num_cells_added += num_of_cells_added;

    unused_positions += scell->gprs_meas_params.multiband_reporting - num_of_cells_added;
  }

  if (gsm_1800_in_ba_list)
  {
    uint32 num_of_cells_added = add_cells_to_pemr(
      rr_gprs_meas_rpt_control_data_ptr->pemr,
                             SYS_BAND_DCS_1800,
                             scell->gprs_meas_params.multiband_reporting + unused_positions,
      get_reporting_threshold(SYS_BAND_DCS_1800, gas_id),
      gas_id
    );

    total_num_cells_added += num_of_cells_added;

    unused_positions += scell->gprs_meas_params.multiband_reporting - num_of_cells_added;
  }

  if (gsm_1900_in_ba_list)
  {
    uint32 num_of_cells_added = add_cells_to_pemr(
      rr_gprs_meas_rpt_control_data_ptr->pemr,
                             SYS_BAND_PCS_1900,
                             scell->gprs_meas_params.multiband_reporting + unused_positions,
      get_reporting_threshold(SYS_BAND_PCS_1900, gas_id),
      gas_id
    );

    total_num_cells_added += num_of_cells_added;

    unused_positions += scell->gprs_meas_params.multiband_reporting - num_of_cells_added;
  }

#ifdef FEATURE_GSM_TDS
  if ( (rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count > 0) &&
       (rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count > 0) )
  {
    MSG_GERAN_ERROR_0_G("RR WCDMA and TDSCDMA dedicated measurements INVALID COMBINATION");
    return FALSE;
  }

  /* 3) the number of best valid cells and with a reported value equal or greater
   * than XXX_REPORTING_THRESHOLD, in each supported other radio access technology/mode
   * in the 3G neighbour cell list, according to the value of the parameters
   * XXX_MULTIRAT_REPORTING. When the radio access technology/mode is UTRAN FDD,
   * then additionally the non-reported value (from CPICH Ec/No and CPICH RSCP) ]
   * shall be equal or greater than FDD_REPORTING_THRESHOLD_2.
   * A valid cell is defined in subclause 8.4.7
   */
  MSG_GERAN_HIGH_0_G("Reporting priority level 3 (TDD 3G cells)");

  if (!rr_gsm_only(gas_id) && rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count > 0)
  {
    uint32 num_of_tdd_cells_added;

    rr_gprs_tdd_3g_measurements_sort(gas_id);

    num_of_tdd_cells_added = grr_add_tdd_cells_to_pemr(rr_gprs_meas_rpt_control_data_ptr->pemr, scell, gas_id);
    MSG_GERAN_MED_1_G("number %d 3G cells added to PEMR", (int)num_of_tdd_cells_added);

    total_num_cells_added += num_of_tdd_cells_added;
  }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
  /* 3) the number of best valid cells and with a reported value equal or greater
   * than XXX_REPORTING_THRESHOLD, in each supported other radio access technology/mode
   * in the 3G neighbour cell list, according to the value of the parameters
   * XXX_MULTIRAT_REPORTING. When the radio access technology/mode is UTRAN FDD,
   * then additionally the non-reported value (from CPICH Ec/No and CPICH RSCP) ]
   * shall be equal or greater than FDD_REPORTING_THRESHOLD_2.
   * A valid cell is defined in subclause 8.4.7
   */

  MSG_GERAN_HIGH_0_G("Reporting priority level 3 (3G cells)");

  if (!rr_gsm_only(gas_id) && rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count > 0)
  {
    uint32 num_of_fdd_cells_added;

    rr_gprs_3g_measurements_sort(gas_id);

    num_of_fdd_cells_added = grr_add_fdd_cells_to_pemr(rr_gprs_meas_rpt_control_data_ptr->pemr, scell, gas_id);
    MSG_GERAN_MED_1_G("number %d 3G cells added to PEMR", (int)num_of_fdd_cells_added);

    total_num_cells_added += num_of_fdd_cells_added;
  }
#endif /* FEATURE_WCDMA */

  /****************/

  if (total_num_cells_added > 0)
  {
    rr_gprs_meas_rpt_control_data_ptr->pemr->reporting_quantity_valid = TRUE;
  }

  if (rr_gprs_get_network_control_order(gas_id) == NC_MEAS_NC1)
  {
    rr_gprs_meas_rpt_control_data_ptr->pemr->nc_mode = 0;
  }
  else
  {
    rr_gprs_meas_rpt_control_data_ptr->pemr->nc_mode = 1;
  }

  rr_gprs_meas_rpt_control_data_ptr->pemr->tlli = rr_gmminfo_get_tlli(gas_id);

  if (rr_gprs_camped_on_ccch(gas_id))
  {
#ifdef FEATURE_WCDMA
    rr_sys_info_3g_data_t *rr_sys_info_3g_data_ptr = rr_get_sys_info_3g_data_ptr(gas_id);
#endif /* FEATURE_WCDMA */

    rr_gprs_meas_rpt_control_data_ptr->pemr->ba_used_and_3g_ba_used_valid = TRUE;
    rr_gprs_meas_rpt_control_data_ptr->pemr->ba_used = scell->gsm.complete_BA_list.BA_IND;
#ifdef FEATURE_WCDMA
    rr_gprs_meas_rpt_control_data_ptr->pemr->ba_3g_used = rr_sys_info_3g_data_ptr->grr_3g_nlist->source.ba_ind_3g;
#endif /* FEATURE_WCDMA */
  }

  if (scell->gprs_meas_params.pmo_ind_valid)
  {
    rr_gprs_meas_rpt_control_data_ptr->pemr->pmo_used = scell->gprs_meas_params.pmo_ind;
  }

  /* Clear the measurements store as the measurements in it should not be used
  for the next PEMR. */
  rr_gprs_meas_rpt_control_reset_measurements(gas_id);

  if ((rr_macinfo_get_mac_status(gas_id) == MAC_IDLE || rr_macinfo_get_mac_status(gas_id) == MAC_TRANSFER) &&
      (pmr_send_instruction == send_to_mac))
  {
    rr_send_grr_mac_send_meas_rpt(
      REPORT_TYPE_PEMR,
                                  &rr_gprs_meas_rpt_control_data_ptr->msg_pemr,
      rr_gprs_get_network_control_order(gas_id),
      rr_gprs_get_nc_non_drx_period(gas_id),
      gas_id
    );

    return TRUE;
  }
  else
  {
    rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.valid = TRUE;
    rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.msg_type = REPORT_TYPE_PEMR;
    rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.msg = rr_gprs_meas_rpt_control_data_ptr->msg_pemr;

    return FALSE;
  }
}


/****************************************************************************
 *   Function name:  pmr_find_next_best_meas
 *   --------------
 *
 *   Description: This function searches for the highest valid measurement in
 *                the global NC measurement data structure
 *
 *
 *   Parameters:  None
 *   -----------
 *
 *
 *   Returns:     Pointer to the measurement element with highest RXLEV, or NULL
 *   --------
 *
 *
 ****************************************************************************/
static nc_measurement_element_t * pmr_find_next_best_meas(
  const gas_id_t gas_id
)
{
  nc_measurement_element_t *result_ptr = NULL;
  nc_measurement_element_t *meas_ptr;
  uint32                    meas_index;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  /* loop through all the NC measurements in the global store */
  meas_ptr = rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements;
  for (meas_index = 0; meas_index < rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.no_of_ncell_entries; meas_index++, meas_ptr++)
  {
    if (meas_ptr->valid_info)
    {
      /* valid measurement */
      if (result_ptr)
      {
        if (meas_ptr->rxlev_total > result_ptr->rxlev_total)
        {
          result_ptr = meas_ptr;
        }
      }
      else
      {
        /* first time a valid result has been found, so it becomes the default */
        result_ptr = meas_ptr;
      }
    }
  }

  return(result_ptr);
}


/****************************************************************************
 *   Function name:  encode_pmr
 *   --------------
 *
 *   Description: This function averages the measurements gathered and compiles a
 *   ------------ measurement report that contains the best 6 cells ( or less )
 *
 *
 *   Parameters: none
 *   -----------
 *
 *
 *   Returns:  rr_event_T
 *   --------
 *
 *
 ****************************************************************************/
static boolean encode_pmr(
  const pmr_send_t          pmr_send_instruction,
  const gprs_scell_info_t * scell,
  const gas_id_t            gas_id
)
{
  nc_measurement_element_t     *ptr_to_best_cell = NULL;
  packet_measurement_report_t  *packet_measurement_report;
  boolean                       send_meas_rpt = FALSE;
  uint8                         meas_index;
  uint8                         array_index;
  uint8                         pcs_count  = 0;
  uint8                         dcs_count  = 0;
  uint8                         egsm_count = 0;
  uint8                         pgsm_count = 0;
  uint8                         g850_count = 0;
  gprs_reselection_structure_T *gprs_reselection_list_ptr = rr_get_gprs_reselection_list_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  packet_measurement_report = &rr_gprs_meas_rpt_control_data_ptr->msg_pmr.pmr;

  memset(&rr_gprs_meas_rpt_control_data_ptr->msg_pmr, 0, sizeof(pmr_or_pemr_u));

  /* clean out the pending measurement report store as this is now invalid */
  rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.valid = FALSE;

  /* average the ncell results gathered */
  for (meas_index = 0; (meas_index < rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.no_of_ncell_entries) && (meas_index < BA_MAX_LEN ); meas_index++)
  {
    /* only average the entries with valid data as the best 6 must have BSIC decoded anyway */
    if ((rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[meas_index].valid_info) &&
        (rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[meas_index].rxlev_count > 0))
    {
      rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[meas_index].rxlev_total =
                rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[meas_index].rxlev_total/rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[meas_index].rxlev_count;
    }

    /* Check to make sure that no entry exists for the scell in the ncell array */
    if (ARFCNS_EQUAL(rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[meas_index].bcch_arfcn, scell->gsm.BCCH_ARFCN))
    {
      MSG_GERAN_HIGH_0_G("Scell entry removed from PMR");
      rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.nc_measurements[meas_index].valid_info = FALSE;
    }
  }

  packet_measurement_report->tlli = rr_gmminfo_get_tlli(gas_id);

  {
    ARFCN_T            scell_bcch_arfcn = scell->gsm.BCCH_ARFCN;
    rr_internal_band_T scell_band;
    uint8              multiband_reporting;
    uint8              num_meas_left;

    scell_band = GET_ARFCN_BAND(scell_bcch_arfcn);

    /* load up the pointers to the best in each band that is supported */
    rr_gprs_load_best_six_for_each_band(
      &rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data,
      gas_id
    );

    multiband_reporting = scell->gprs_meas_params.multiband_reporting;

    switch (multiband_reporting)
    {
    case 1:
      /* Strongest cell from each band in the BA list - excluding the scell band */
    case 2:
      /* Strongest 2 cells from each band in the BA list - excluding the scell band */
    case 3:
      /* Strongest 3 cells from each band in the BA list - excluding the scell band */
      for ( meas_index = 0; meas_index < 6; meas_index++ )
      {
        if( (rr_gprs_meas_rpt_control_data_ptr->pcs_cells[pcs_count] != NULL) && (scell_band != BAND_PCS) )
        {
          ptr_to_best_cell = rr_gprs_meas_rpt_control_data_ptr->pcs_cells[pcs_count++];
          /* don't go through this path again */
          if (pcs_count == multiband_reporting)
          {
            for(array_index=0;array_index<6;array_index++)
            {
              rr_gprs_meas_rpt_control_data_ptr->pcs_cells[array_index]=NULL;
            }
          }
        }
        else if( (rr_gprs_meas_rpt_control_data_ptr->dcs_cells[dcs_count] != NULL) && (scell_band != BAND_DCS) )
        {
          ptr_to_best_cell = rr_gprs_meas_rpt_control_data_ptr->dcs_cells[dcs_count++];
          /* don't go through this path again */
          if (dcs_count == multiband_reporting)
          {
            for(array_index=0;array_index<6;array_index++)
            {
              rr_gprs_meas_rpt_control_data_ptr->dcs_cells[array_index]=NULL;
            }
          }
        }
        else if( (rr_gprs_meas_rpt_control_data_ptr->pgsm_cells[pgsm_count] != NULL) && (scell_band != BAND_PGSM) )
        {
          ptr_to_best_cell = rr_gprs_meas_rpt_control_data_ptr->pgsm_cells[pgsm_count++];
          /* don't go through this path again */
          if (pgsm_count == multiband_reporting)
          {
            for(array_index=0;array_index<6;array_index++)
            {
              rr_gprs_meas_rpt_control_data_ptr->pgsm_cells[array_index]=NULL;
            }
          }
        }
        else if( (rr_gprs_meas_rpt_control_data_ptr->egsm_cells[egsm_count] != NULL) && (scell_band != BAND_EGSM) )
        {
          ptr_to_best_cell = rr_gprs_meas_rpt_control_data_ptr->egsm_cells[egsm_count++];
          /* don't go through this path again */
          if (egsm_count == multiband_reporting)
          {
            for(array_index=0;array_index<6;array_index++)
            {
              rr_gprs_meas_rpt_control_data_ptr->egsm_cells[array_index]=NULL;
            }
          }
        }
        else if( (rr_gprs_meas_rpt_control_data_ptr->g850_cells[g850_count] != NULL) && (scell_band != BAND_GSM_850) )
        {
          ptr_to_best_cell = rr_gprs_meas_rpt_control_data_ptr->g850_cells[g850_count++];
          /* don't go through this path again */
          if (g850_count == multiband_reporting)
          {
            for(array_index=0;array_index<6;array_index++)
            {
              rr_gprs_meas_rpt_control_data_ptr->g850_cells[array_index]=NULL;
            }
          }
        }
        else
        {
          /* Fill remaining entries with cells in the scells band */
          switch (scell_band)
          {
          case BAND_PGSM:
            ptr_to_best_cell = rr_gprs_meas_rpt_control_data_ptr->pgsm_cells[pgsm_count++];
            break;
          case BAND_EGSM:
            ptr_to_best_cell = rr_gprs_meas_rpt_control_data_ptr->egsm_cells[egsm_count++];
            break;
          case BAND_DCS:
            ptr_to_best_cell = rr_gprs_meas_rpt_control_data_ptr->dcs_cells[dcs_count++];
            break;
          case BAND_PCS:
            ptr_to_best_cell = rr_gprs_meas_rpt_control_data_ptr->pcs_cells[pcs_count++];
            break;
          case BAND_GSM_850:
            ptr_to_best_cell = rr_gprs_meas_rpt_control_data_ptr->g850_cells[g850_count++];
            break;
          default:
            MSG_GERAN_ERROR_1_G("Invalid band (%d) in format_meas_report", scell_band);
          }
        }

        if (ptr_to_best_cell != NULL)
        {
          /* load the data into the meas report */
          packet_measurement_report->enc_nc_meas_rprt_strct.num_nc_meas++;
          packet_measurement_report->enc_nc_meas_rprt_strct.num_meas[meas_index].freq_n =
            rr_gprs_find_ba_gprs_index(ptr_to_best_cell->bcch_arfcn, &ptr_to_best_cell->bsic, gas_id);
          packet_measurement_report->enc_nc_meas_rprt_strct.num_meas[meas_index].rxlev_n = (uint8)ptr_to_best_cell->rxlev_total;
          packet_measurement_report->enc_nc_meas_rprt_strct.num_meas[meas_index].bsic_n = CONVERT_BSIC_TO_BYTE(ptr_to_best_cell->bsic);
          packet_measurement_report->enc_nc_meas_rprt_strct.num_meas[meas_index].bsic_n_flag = valid;
          /* make sure that the entry is not used again by setting the valid_info flag to FALSE */
          ptr_to_best_cell->valid_info = FALSE;
        }
        else
          break;
      } /* end of for loop */

      /* fill the remaining positions with whatever measurements are left */
      meas_index = (uint8)packet_measurement_report->enc_nc_meas_rprt_strct.num_nc_meas;
      num_meas_left = MAX_NC_MEAS - meas_index;

      while (num_meas_left)
      {
        ptr_to_best_cell = pmr_find_next_best_meas(gas_id);
        if (ptr_to_best_cell)
        {
          /* load the data into the meas report */
          packet_measurement_report->enc_nc_meas_rprt_strct.num_nc_meas++;
          packet_measurement_report->enc_nc_meas_rprt_strct.num_meas[meas_index].freq_n =
            rr_gprs_find_ba_gprs_index(
               ptr_to_best_cell->bcch_arfcn,
               &ptr_to_best_cell->bsic,
               gas_id
             );
          packet_measurement_report->enc_nc_meas_rprt_strct.num_meas[meas_index].rxlev_n = (uint8)ptr_to_best_cell->rxlev_total;
          packet_measurement_report->enc_nc_meas_rprt_strct.num_meas[meas_index].bsic_n = CONVERT_BSIC_TO_BYTE(ptr_to_best_cell->bsic);
          packet_measurement_report->enc_nc_meas_rprt_strct.num_meas[meas_index].bsic_n_flag = valid;
          /* make sure that the entry is not used again by setting the valid_info flag to FALSE */
          ptr_to_best_cell->valid_info = FALSE;
          num_meas_left--;
          meas_index++;
        }
        else
        {
          /* drop out of the loop now */
          num_meas_left = 0;
        }
      }
      break;

    default:
      /* Six strongest from any band */
      for (meas_index = 0; meas_index < 6; meas_index++)
      {
        ptr_to_best_cell = pmr_find_next_best_meas(gas_id);

        if (ptr_to_best_cell == NULL)
        {
          /* no more valid entries can be found, so stop now */
          break;
        }
        else
        {
          /* load the data into the meas report */
          packet_measurement_report->enc_nc_meas_rprt_strct.num_nc_meas++;
          packet_measurement_report->enc_nc_meas_rprt_strct.num_meas[meas_index].freq_n =
            rr_gprs_find_ba_gprs_index(
               ptr_to_best_cell->bcch_arfcn,
               &ptr_to_best_cell->bsic,
               gas_id
             );
          packet_measurement_report->enc_nc_meas_rprt_strct.num_meas[meas_index].rxlev_n = (uint8)ptr_to_best_cell->rxlev_total;
          packet_measurement_report->enc_nc_meas_rprt_strct.num_meas[meas_index].bsic_n = CONVERT_BSIC_TO_BYTE(ptr_to_best_cell->bsic);
          packet_measurement_report->enc_nc_meas_rprt_strct.num_meas[meas_index].bsic_n_flag = valid;

        /* make sure that the entry is not used again by setting the valid_info flag to FALSE */
          ptr_to_best_cell->valid_info = FALSE;
        }
      } /* end of for loop */
      break;
    }

    /* clean out the temporary best 6 arrays */
    for (array_index = 0; array_index < 6; array_index++)
    {
      rr_gprs_meas_rpt_control_data_ptr->egsm_cells[array_index] = NULL;
      rr_gprs_meas_rpt_control_data_ptr->pgsm_cells[array_index] = NULL;
      rr_gprs_meas_rpt_control_data_ptr->pcs_cells[array_index]  = NULL;
      rr_gprs_meas_rpt_control_data_ptr->dcs_cells[array_index]  = NULL;
      rr_gprs_meas_rpt_control_data_ptr->g850_cells[array_index] = NULL;
    }
  } /* end of block for FEATURE_GSM_MULTIBAND */


  /* set up the NC_MODE */
  if (rr_gprs_get_network_control_order(gas_id) == NC_MEAS_NC1)
  {
    packet_measurement_report->enc_nc_meas_rprt_strct.nc_mode = 0;
  }
  else
  {
    packet_measurement_report->enc_nc_meas_rprt_strct.nc_mode = 1;
  }

  packet_measurement_report->enc_nc_meas_rprt_strct.intrfrnce_srvg_cell_flag = not_valid;


  /* average and load the RXLEV measurements for the scell into the meas report */
  if (rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count > 0)
  {
    /* avoid division by zero */
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_total /= rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count;
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count = 1;
  }
  else if (gprs_reselection_list_ptr->serving_values_valid)
  {
    MSG_GERAN_MED_0_G("Using serving cell measurements received before NC2...");
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_total = gprs_reselection_list_ptr->serving_values.RXLEV_average;
    rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count = 1;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("No serving cell rxlev available, not including scell in PMR");
  }
  packet_measurement_report->enc_nc_meas_rprt_strct.rxlev_srvg_cell = (uint8)rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_total;

  /* Load the psi5 change mark value is not being used instead of a PMO */
  packet_measurement_report->psi5_chng_mrk_flag = not_valid;
  /*Add R99 extension only in the following cases
    1. Camped on PBCCH
    2. SI2Quarter is received
    3. PMO IND valid is set. This means RR received PMO which might have changed NCELL list*/

  if((scell->gsm.two_quater_position == SI2Q_POSITION_BCCH_Norm) ||
    (scell->gsm.two_quater_position == SI2Q_POSITION_BCCH_Ext))
  {
    if(ARE_SET(MASK_SI_2_QUATER,scell->gsm.update_message_received))
    {
      packet_measurement_report->ba_or_psi3_and_pmo_included = valid;
    }
  }
  else if(scell->gprs_meas_params.pmo_ind_valid)
  {
    packet_measurement_report->ba_or_psi3_and_pmo_included = valid;
  }

  if (packet_measurement_report->ba_or_psi3_and_pmo_included == valid)
  {
    /* RELEASE 99 extensions */
    packet_measurement_report->ba_or_psi3_change_mark = ba_present;
    packet_measurement_report->ba_used = scell->gsm.complete_BA_list.BA_IND;
#ifdef FEATURE_WCDMA
    {
      rr_sys_info_3g_data_t *rr_sys_info_3g_data_ptr = rr_get_sys_info_3g_data_ptr(gas_id);

      packet_measurement_report->three_g_ba_used = rr_sys_info_3g_data_ptr->grr_3g_nlist->source.ba_ind_3g;
    }
#endif /* FEATURE_WCDMA */

    if (scell->gprs_meas_params.pmo_ind_valid)
    {
      packet_measurement_report->pmo_used = scell->gprs_meas_params.pmo_ind;
    }
    else
    {
      packet_measurement_report->pmo_used = 0;
    }

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
    if (rr_gprs_3g_meas_report_encode(&packet_measurement_report->three_g_measurement_report, gas_id))
    {
      packet_measurement_report->three_g_measurement_report_flag = valid;
    }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/
  }

  /* clean out the nc_measurement data structure */
  rr_gprs_meas_rpt_control_reset_measurements(gas_id);

  /* restore a serving cell measurement in case we have to report again without new data */
  rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_total = packet_measurement_report->enc_nc_meas_rprt_strct.rxlev_srvg_cell;
  rr_gprs_meas_rpt_control_data_ptr->nc_measurement_data.scell_rxlev_count = 0;

  /* send report to MAC if in IDLE or TRANSFER */
  if (((rr_macinfo_get_mac_status(gas_id) == MAC_IDLE) ||
       (rr_macinfo_get_mac_status(gas_id) == MAC_TRANSFER))
      && (pmr_send_instruction == send_to_mac))
  {
    rr_send_grr_mac_send_meas_rpt(
      REPORT_TYPE_PMR,
                                  &rr_gprs_meas_rpt_control_data_ptr->msg_pmr,
      rr_gprs_get_network_control_order(gas_id),
      rr_gprs_get_nc_non_drx_period(gas_id),
      gas_id
    );

    /* flag that the measurement has been sent */
    send_meas_rpt = TRUE;
  }
  else
  {
    /****************************************************************************/
    /* put the measurement report into the pending report store to be sent when */
    /* MAC moves into the correct mode                                          */
    /****************************************************************************/
    rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.msg_type = REPORT_TYPE_PMR;
    rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.msg      = rr_gprs_meas_rpt_control_data_ptr->msg_pmr;
    rr_gprs_meas_rpt_control_data_ptr->pending_pmr_store.valid    = TRUE;

    if (pmr_send_instruction == send_to_mac)
    {
      MSG_GERAN_MED_0_G("Storing NC meas rpt - MAC not in IDLE or TRANSFER");
    }
    else
    {
      MSG_GERAN_MED_0_G("Storing NC meas rpt - waiting for response to last request");
    }

    send_meas_rpt = FALSE;
  }

  return send_meas_rpt;
}


/**
 * rr_gprs_format_meas_report
 *
 * This function will encode and send either a PEMR (if the network indicates
 * normal measurement reporting AND RR has received a complete GPRS BSIC
 * description) or a PMR otherwise.
 */
static boolean rr_gprs_format_meas_report(pmr_send_t pmr_send_instruction, const gas_id_t gas_id)
{
  gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

  if (scell->gprs_meas_params.report_type == REPORT_TYPE_ENHANCED)
  {
    if (rr_gprs_complete_gprs_bsic_description_received(gas_id))
    {
      MSG_GERAN_MED_0_G("System information indicates enhanced measurement reporting, complete GPRS BSIC Description received, send PEMR");
      return
        encode_pemr(
          pmr_send_instruction,
          scell,
          gas_id
        );
    }
    else
    {
      MSG_GERAN_MED_0_G("System information indicates enhanced measurement reporting, complete GPRS BSIC Description NOT received, send PMR");
      return
        encode_pmr(
          pmr_send_instruction,
          scell,
          gas_id
        );
    }
  }
  else
  {
    MSG_GERAN_MED_0_G("System information indicates normal measurement reporting, send PMR");
    return
      encode_pmr(
        pmr_send_instruction,
        scell,
        gas_id
      );
  }
}





/****************************************************************************
 *   Function name:  rr_gprs_start_t3158
 *   --------------
 *
 *   Description: This function starts the timer t3158 with the correct value.
 *   ------------ it will also check to see if the timer is currently running.
 *                (this will occur if measurment reporting is active already)
 *                In such a case the time to expiry is compared against the
 *                value to be loaded. If the time to expiry is less than the
 *                timeout value to be loaded it is left to expire, otherwise
 *                the new value is used.
 *
 *
 *   Parameters: none
 *   -----------
 *
 *
 *   Returns:  none
 *   --------
 *
 *
 ****************************************************************************/
static t3158_action_t rr_gprs_start_t3158(boolean mac_status_changed, const gas_id_t gas_id)
{
  t3158_action_t timer_action;
  uint16         drx_period_ms;
  uint16         timer_setting_ms;
  boolean        is_running;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  /* Start the timer with the correct value - anything other than transfer has the IDLE timeout*/
  if (rr_macinfo_get_mac_status(gas_id) == MAC_TRANSFER)
  {
    timer_setting_ms = nc_meas_rep_period_ms[rr_gprs_get_nc_reporting_period_t(gas_id)];
  }
  else
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
    uint8              bs_pa_mfrms;

    if (scell_info_ptr != NULL)
  {
      bs_pa_mfrms = scell_info_ptr->gsm.control_channel_description.BS_PA_MFRMS;
  }
  else
  {
      /* trigger error handler in subsequent checking below */
      bs_pa_mfrms = 0;
    }

    if ((bs_pa_mfrms >= BS_PA_MFRMS_MIN) && (bs_pa_mfrms <= BS_PA_MFRMS_MAX))
    {
      /* use the lookup table to determine the paging cycle (table calculated offline for better accuracy) */
      drx_period_ms = paging_cycle_lengths[bs_pa_mfrms-BS_PA_MFRMS_MIN];
    }
    else
    {
      drx_period_ms = 0;
      if (geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_0_G("BS_PA_MFRMS is zero (invalid value),Triggering PANIC RESET");
        geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
      } 
      else
      {            
        ERR_GERAN_FATAL_0_G("BS_PA_MFRMS is zero (invalid value)");
      }
      return(t3158_allow_to_run);
    }

    /************************************************************************/
    /* Calculate the idle timer setting - In packet idle mode the reporting */
    /* period is nc_reporting_period_i rounded off to the nearest smaller   */
    /* integer multiple of DRX period if nc_reporting_period_i is greater   */
    /* than DRX period                                                      */
    /*                                                                      */
    /* e.g. nc_reporting_period_i = 6144ms                                  */
    /*      drx_period_ms = 958ms                                           */
    /*                                                                      */
    /*            nc_reporting_period_i/drx_period_ms = 6                   */
    /*                                                                      */
    /* ....the reporting period is set to 6 * drx_period_ms therefore       */
    /*                                                                      */
    /*            nc_reporting_period_i = 5748ms                            */
    /*                                                                      */
    /* If the nc_reporting_period_i/drx_period_ms gave an answer of 0 then  */
    /* the nc_reporting_period_i must be smaller than the DRX period so the */
    /* nc_reporting_period_i is set to equal the DRX period.                */
    /************************************************************************/
    timer_setting_ms = (uint16)
      ((nc_meas_rep_period_ms[rr_gprs_get_nc_reporting_period_i(gas_id)]/drx_period_ms) * drx_period_ms);

    /* If the setting is zero then the nc_reporting_period_i is smaller than the DRX period so
       use the DRX period as the reporting period - 05.08 - 10.1.4.1 */
    if ( timer_setting_ms == 0 )
    {
      timer_setting_ms = drx_period_ms;
    }
  }

  /************************************************************************/
  /* According to the spec 04.60 - 5.6.1 para4.  If the ms recieves a new */
  /* timer setting or require a change due to a change of packet mode then*/
  /* if already running only use the new timer setting if it is shorter   */
  /* than the timer to expiry of the current timer count.                 */
  /************************************************************************/
  is_running = rr_timer_is_running(T3158, gas_id);

  if ((mac_status_changed == TRUE) && (rr_macinfo_get_mac_status(gas_id) == MAC_IDLE))
  {
    /*if mac status changed from transfer to IDLE and we transmitted one PMR in transfer then*/
    /*restart the T3158 with idle value otherwise preserve the current timer*/
    if ((is_running == TRUE) && (rr_gprs_meas_rpt_control_data_ptr->meas_rpt_sent_in_txfer == TRUE))
    {
      (void)rr_timer_cancel(T3158, gas_id);
      /* load the timer with the new value */
      (void)rr_timer_start(T3158, timer_setting_ms, gas_id);
      MSG_GERAN_MED_1_G("Start the NC timer T3158 with reporting period: %d",timer_setting_ms);
      timer_action = t3158_restarted;
    }
    else
    {
      if(rr_gprs_meas_rpt_control_data_ptr->meas_rpt_sent_in_txfer == TRUE)
      {
        MSG_GERAN_MED_0_G("Preserve the current timer as no measurement report sent in transfer mode");
      }
      timer_action = t3158_allow_to_run;
    }
  }
  else
  {
    uint16 time_left;

    time_left = (uint16)rr_timer_get_time_remaining(T3158, gas_id);
    if (is_running && (time_left <= timer_setting_ms))
    {
      /* allow the timer to run with its existing value */
      MSG_GERAN_MED_2_G("Preserve the T3158 setting it is the shortest: %d <= %d ",(int)time_left, (int)timer_setting_ms);
      timer_action = t3158_allow_to_run;
    }
    else
    {
      /* new T3158 setting is shorter than time remaining */
      /* so just restart the timer now with the new value */
      (void)rr_timer_start(T3158, timer_setting_ms, gas_id);
      MSG_GERAN_MED_1_G("Start the NC timer T3158 with reporting period: %d",(int)timer_setting_ms);
      timer_action = t3158_restarted;
    }
  }

  return timer_action;
}

/**
  @brief Forces a restart of T3158, regardless of whether the timer is
         currently running or not.
*/
void rr_gprs_meas_rpt_control_force_t3158_restart(const gas_id_t gas_id)
{
  uint16         timer_setting_ms;

  /* Start the timer with the correct value - anything other than transfer has the IDLE timeout*/
  if (rr_macinfo_get_mac_status(gas_id) == MAC_TRANSFER)
  {
    timer_setting_ms = nc_meas_rep_period_ms[rr_gprs_get_nc_reporting_period_t(gas_id)];
  }
  else
  {
    /* Split paging is not supported on ccch */
    gprs_scell_info_t * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
    uint16              drx_period_ms;

    if (scell_info_ptr->gsm.control_channel_description.BS_PA_MFRMS == 0)
    {
      if (geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_0_G("BS_PA_MFRMS is zero (invalid value),Triggering PANIC RESET");
        geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
      } 
      else
      {            
        ERR_GERAN_FATAL_0_G("BS_PA_MFRMS is zero (invalid value),Triggering PANIC RESET");
      }
      return;
    }

    #define BCCH_51_MULTIFRAME_DURATION 235

    /* Calculate the DRX period */
    drx_period_ms = (uint16)(BCCH_51_MULTIFRAME_DURATION * scell_info_ptr->gsm.control_channel_description.BS_PA_MFRMS);

    /************************************************************************/
    /* Calculate the idle timer setting - In packet idle mode the reporting */
    /* period is nc_reporting_period_i rounded off to the nearest smaller   */
    /* integer multiple of DRX period if nc_reporting_period_i is greater   */
    /* than DRX period                                                      */
    /*                                                                      */
    /* e.g. nc_reporting_period_i = 6144ms                                  */
    /*      drx_period_ms = 958ms                                           */
    /*                                                                      */
    /*            nc_reporting_period_i/drx_period_ms = 6                   */
    /*                                                                      */
    /* ....the reporting period is set to 6 * drx_period_ms therefore       */
    /*                                                                      */
    /*            nc_reporting_period_i = 5748ms                            */
    /*                                                                      */
    /* If the nc_reporting_period_i/drx_period_ms gave an answer of 0 then  */
    /* the nc_reporting_period_i must be smaller than the DRX period so the */
    /* nc_reporting_period_i is set to equal the DRX period.                */
    /************************************************************************/
    timer_setting_ms = (uint16)
      ((nc_meas_rep_period_ms[rr_gprs_get_nc_reporting_period_i(gas_id)]/drx_period_ms) * drx_period_ms);

    /* If the setting is zero then the nc_reporting_period_i is smaller than the DRX period so
       use the DRX period as the reporting period - 05.08 - 10.1.4.1 */
    if ( timer_setting_ms == 0 )
    {
      timer_setting_ms = drx_period_ms;
    }
  }

  (void) rr_timer_cancel(T3158, gas_id);
  MSG_GERAN_HIGH_1_G("Forcing restart of T3158, %d ms", timer_setting_ms);
  rr_timer_start(T3158, timer_setting_ms, gas_id);
}

/****************************************************************************
 *   Function name:  rr_gprs_load_best_six_for_each_band
 *   --------------
 *
 *   Description: looks through all the collected measurements and loads up the
 *   ------------ results into best 6 lists for each band.
 *
 *   Parameters: nc_measurement_data_t - contains current results.
 *   -----------
 *
 *   Returns:  none
 *   --------
 *
 *
 ****************************************************************************/
static void rr_gprs_load_best_six_for_each_band(
  nc_measurement_data_t *results_src,
  const gas_id_t gas_id
)
{
nc_measurement_element_t *meas_ptr;
rr_internal_band_T        band;
uint8                     index;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  /* average the ncell results gathered */
  for (index = 0; index < results_src->no_of_ncell_entries; index++)
  {
    meas_ptr = &(results_src->nc_measurements[index]);

    /* only average the entries with valid data as the best 6 must have BSIC decoded anyway */
    if ((meas_ptr->valid_info) && (meas_ptr->rxlev_count > 0))
    {
      band = GET_ARFCN_BAND(meas_ptr->bcch_arfcn);

      switch(band)
      {
      case BAND_EGSM:
        rr_gprs_insert_meas_entry(meas_ptr, rr_gprs_meas_rpt_control_data_ptr->egsm_cells);
        break;

      case BAND_PGSM:
        rr_gprs_insert_meas_entry(meas_ptr, rr_gprs_meas_rpt_control_data_ptr->pgsm_cells);
        break;

      case BAND_PCS:
        rr_gprs_insert_meas_entry(meas_ptr, rr_gprs_meas_rpt_control_data_ptr->pcs_cells);
        break;

      case BAND_DCS:
        rr_gprs_insert_meas_entry(meas_ptr, rr_gprs_meas_rpt_control_data_ptr->dcs_cells);
        break;

      case BAND_GSM_850:
        rr_gprs_insert_meas_entry(meas_ptr, rr_gprs_meas_rpt_control_data_ptr->g850_cells);
        break;

      default:
        MSG_GERAN_ERROR_1_G("Invalid band (%d) in load_best_six", band);
        break;
      }  /* end switch */
    } /* end if */
  } /* end for */
} /* end of rr_gprs_load_best_six_for_each_band() */


/****************************************************************************
 *   Function name:  rr_gprs_insert_meas_entry
 *   --------------
 *
 *   Description: Inserts a measurement entry in the given list
 *   ------------ if space allows, and measurement is better than one of the others
 *
 *   Parameters: nc_measurement_element_t - specific entry
 *   ----------- nc_measurement_element_t - array of entries where ptr should be stored.
 *
 *   Returns:  none
 *   --------
 *
 *
 ****************************************************************************/
#define NUM_MEAS_ENTRIES_IN_ARRAY 6
static void rr_gprs_insert_meas_entry(nc_measurement_element_t *entry, nc_measurement_element_t **band_array)
{
uint8 index;
uint8 move_index;

  /* scan the entire array */
  for (index = 0; index < NUM_MEAS_ENTRIES_IN_ARRAY; index++)
  {
    if (band_array[index] == NULL)
    {
      /* this entry in the array is empty, so store the ptr at this index */
      band_array[index] = entry;
      MSG_GERAN_LOW_2("Added (%d,%d) to MR list", entry->bcch_arfcn.num, entry->bcch_arfcn.band);
      break;
    }
    else if (entry->rxlev_total > band_array[index]->rxlev_total)
    {
      /* new entry has greater RXLEV total than current element,insert at this index */
      /* all entries in the array move down one place,so the last entry will be lost */
      for (move_index = (NUM_MEAS_ENTRIES_IN_ARRAY-1); move_index > index; move_index--)
      {
        band_array[move_index] = band_array[move_index-1];
      }
      band_array[index] = entry;
      MSG_GERAN_LOW_2("Added (%d,%d) to MR list", entry->bcch_arfcn.num, entry->bcch_arfcn.band);
      break;
    }
  }
}

#ifdef FEATURE_WCDMA
/*! rr_gprs_3g_measurements_average

Calculates the average values for each 3G cell measurements are kept for.
*/
static void rr_gprs_average_3g_measurements(
  const gas_id_t gas_id
)
{
  int i;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  for (i = 0; i < rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count; i++)
  {
    rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].ecio_average =
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].ecio_total / rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].noof_measurements;

    rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].rscp_average =
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].rscp_total / rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].noof_measurements;

    rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].rssi_average =
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].rssi_total / rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i].noof_measurements;
  }
}

static void rr_gprs_store_3g_measurements_idle(
  mph_surround_wcdma_idle_meas_ind_T * meas,
  const gas_id_t gas_id
)
{
  int i;
  surrounding_wcdma_channel_info_T *entry;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  memset(&rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements, 0, sizeof(grr_3g_measurements_t));

  for (i = 0; i < meas->best_six_wcdma_info.no_of_entries; i++)
  {
    entry = &meas->best_six_wcdma_info.surrounding_wcdma_channel_info[i];

    rr_gprs_store_3g_measurement(
      entry->UARFCN,
      entry->Scrambling_code,
      entry->RSCP,
      entry->EcNo,
      entry->RSSI,
      gas_id
    );
  }
}

static void rr_gprs_store_3g_measurements_xfer(
  mph_surround_wcdma_xfer_meas_ind_T * meas,
  const gas_id_t gas_id
)
{
  int i;
  wcdma_dedicated_meas_T *entry;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  memset(&rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements, 0, sizeof(grr_3g_measurements_t));

  for (i = 0; i < meas->num_best_cells; i++)
  {
    entry = &meas->wcdma_xfer_meas[i];
    rr_gprs_store_3g_measurement(
      entry->UARFCN,
      entry->Scrambling_Code,
      entry->RSCP,
      entry->Ec_Nox2,
      entry->RSSI,
      gas_id
    );
  }
}

/** Returns the REP_QUANT value for a specified UARFCN measurement index

@param meas_index - the measurement index of the required UARFCN (0 is best WCDMA cell)
*/
byte rr_gprs_3g_meas_get_rep_quant(int meas_index, const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_ptr = rr_gprs_get_scell_info(gas_id);
  uint16             noof_meas;
  byte               rep_quant;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  noof_meas = rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[meas_index].noof_measurements;
  if (noof_meas == 0)
  {
    /* avoid possibility of div-by-zero */
    noof_meas = 1;
  }

  if (scell_ptr->gprs_3g_meas_params.FDD_REP_QUANT == 0)
  {
    int16 rep_quant_db = rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[meas_index].rscp_total / noof_meas;

    rep_quant = rr_get_reported_RSCP(rep_quant_db);
  }
  else
  {
    rep_quant = rr_get_reported_Ec_No((int16)(rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[meas_index].ecio_total / noof_meas));
  }

  return(rep_quant);
}

/** Adds up to FDD_MULTIRAT_REPORTING UTRAN measurement reports to the 3G Measurement Report

@param meas_report the CSN structure to add the cells to
@param FDD_MULTIRAT_REPORTING the number of entries to add to the measurement report
@param noof_entries pointer to the number of entries currently in the measurement report
*/
static uint8 rr_gprs_3g_meas_report_encode_fdd(
  three_g_measurement_report_t *meas_report,
  uint8 FDD_MULTIRAT_REPORTING,
  uint8 *noof_entries,
  const gas_id_t gas_id
)
{
  uint8                  noof_fdd_entries = 0; /* the number of entries that have been added to the measurement report */
  int                    fdd_measurements_index = 0; /* the first/strongest cell in the fdd_measurements database */
  rr_sys_info_3g_data_t *rr_sys_info_3g_data_ptr = rr_get_sys_info_3g_data_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  MSG_GERAN_MED_1_G("FDD_MULTIRAT_REPORTING: %d", FDD_MULTIRAT_REPORTING);
  MSG_GERAN_MED_3_G("grr_3g_measurements.fdd_count: %d, %d, %d", rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count,
                                                     rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[0].rscp_total,
                             rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[0].noof_measurements);

  while ((noof_fdd_entries < FDD_MULTIRAT_REPORTING) && (fdd_measurements_index < rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count))
  {
    uint16 uarfcn = rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[fdd_measurements_index].uarfcn;
    uint16 scrambling_code = rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[fdd_measurements_index].scrambling_code;

    uint8 rssi_index = (uint8)rr_gprs_3g_nlist_get_fdd_freq_index(rr_sys_info_3g_data_ptr->grr_3g_nlist, uarfcn);

    if (rssi_index != MAX_GRR_3G_NLIST_SIZE)
    {
      /* need to report the rssi for this frequency, add it only if it has not already been included */
      boolean rssi_already_reported = FALSE;
      uint8   i;

      MSG_GERAN_MED_2_G("Reporting RSSI for uarfcn %d, %d", uarfcn, rssi_index);

      /* check to see that its rssi_index is not already in the measurement report */
      for (i = *noof_entries; i < noof_fdd_entries; i++)
      {
        if (meas_report->three_g_cell_list[i].three_g_cell_list_index == rssi_index)
        {
          rssi_already_reported = TRUE;
        }
      }

      if (rssi_already_reported == FALSE)
      {
        meas_report->three_g_cell_list[*noof_entries].rep_quality = rr_get_reported_RSSI((int16)(rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[fdd_measurements_index].rssi_total /
          rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[fdd_measurements_index].noof_measurements));
        meas_report->three_g_cell_list[*noof_entries].three_g_cell_list_index = rssi_index;
        *noof_entries += 1;
      }
    }

    /* RSSI report has now been added if it was required, so now need to add the cell rscp or ec/io if it will fit */
    if (noof_fdd_entries < FDD_MULTIRAT_REPORTING)
    {
      byte rep_quant;
      gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);

      byte reported_index = (uint8)rr_gprs_3g_nlist_get_fdd_cell_index(rr_sys_info_3g_data_ptr->grr_3g_nlist, uarfcn, scrambling_code);

      if (reported_index != MAX_GRR_3G_NLIST_SIZE)
      {
        MSG_GERAN_MED_3_G("Adding %d, %d, %d to PMR", uarfcn, scrambling_code, reported_index);
        if (scell->gprs_3g_meas_params.FDD_REP_QUANT == 0)
        {
          int16 rep_quant_db = rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[fdd_measurements_index].rscp_total /
            rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[fdd_measurements_index].noof_measurements;
          rep_quant = rr_get_reported_RSCP(rep_quant_db);
        }
        else
        {
          rep_quant = rr_get_reported_Ec_No((int16)(rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[fdd_measurements_index].ecio_total /
            rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[fdd_measurements_index].noof_measurements));
        }

        meas_report->three_g_cell_list[*noof_entries].rep_quality = rep_quant;
        meas_report->three_g_cell_list[*noof_entries].three_g_cell_list_index = reported_index;

        *noof_entries += 1;
        noof_fdd_entries += 1;
      }
      else
      {
        MSG_GERAN_ERROR_2_G("Cell reported by L1 is not in 3G neighbour list, %d, %d, %d", uarfcn, scrambling_code);
      }
    }

    fdd_measurements_index++;
  }

  return noof_fdd_entries;
}

static int compare_fdd_measurements_by_rscp(const void *arg1, const void *arg2)
{
  int rscp1, rscp2;

  ASSERT(arg1 != NULL);
  ASSERT(arg2 != NULL);

  rscp1 = ((fdd_measurement_t *) arg1)->rscp_average;
  rscp2 = ((fdd_measurement_t *) arg2)->rscp_average;


  if (rscp1 > rscp2)
  {
    return -1;
  }
  else if (rscp1 < rscp2)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

static int compare_fdd_measurements_by_ecio(const void *arg1, const void *arg2)
{
  int ecio1, ecio2;

  ASSERT(arg1 != NULL);
  ASSERT(arg2 != NULL);

  ecio1 = ((fdd_measurement_t *) arg1)->ecio_average;
  ecio2 = ((fdd_measurement_t *) arg2)->ecio_average;

  if (ecio1 > ecio2)
  {
    return -1;
  }
  else if (ecio1 < ecio2)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


static void rr_gprs_3g_measurements_sort_fdd(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  if (rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count > MAX_GRR_3G_FDD_MEASUREMENTS)
  {
    rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count = MAX_GRR_3G_FDD_MEASUREMENTS;
  }

  rr_gprs_average_3g_measurements(gas_id);

  if (scell->gprs_3g_meas_params.FDD_REP_QUANT == 0)
  {
    qsort(
      (void *) &rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[0],
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count,
      sizeof(fdd_measurement_t),
      compare_fdd_measurements_by_rscp
    );
  }
  else
  {
    qsort(
      (void *) &rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[0],
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count,
      sizeof(fdd_measurement_t),
      compare_fdd_measurements_by_ecio
    );
  }
}

static void rr_gprs_3g_measurements_sort(const gas_id_t gas_id)
{
  rr_gprs_3g_measurements_sort_fdd(gas_id);
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
/** Adds up to TDD_MULTIRAT_REPORTING UTRAN measurement reports to the 3G Measurement Report

@param meas_report the CSN structure to add the cells to
@param TDD_MULTIRAT_REPORTING the number of entries to add to the measurement report
@param noof_entries pointer to the number of entries currently in the measurement report
*/
static uint8 rr_gprs_3g_meas_report_encode_tdd(
  three_g_measurement_report_t *meas_report,
  uint8 TDD_MULTIRAT_REPORTING,
  uint8 *noof_entries,
  const gas_id_t gas_id
)
{
  uint8                  noof_tdd_entries = 0; /* the number of entries that have been added to the measurement report */
  int                    tdd_measurements_index = 0; /* the first/strongest cell in the tdd_measurements database */
  rr_sys_info_3g_data_t *rr_sys_info_3g_data_ptr = rr_get_sys_info_3g_data_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  MSG_GERAN_MED_1_G("TDD_MULTIRAT_REPORTING: %d", TDD_MULTIRAT_REPORTING);
  MSG_GERAN_MED_3_G("grr_3g_measurements.tdd_count: %d, %d, %d", rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count,
                                                       rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[0].rscp_total,
                                                       rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[0].noof_measurements);

  while ((noof_tdd_entries < TDD_MULTIRAT_REPORTING) && (tdd_measurements_index < rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count))
  {
    uint16 uarfcn = rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[tdd_measurements_index].uarfcn;
    uint16 cpid = rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[tdd_measurements_index].cpid;

    /* RSSI report has now been added if it was required, so now need to add the cell rscp or ec/io if it will fit */
    if (noof_tdd_entries < TDD_MULTIRAT_REPORTING)
    {
      byte rep_quant;

      byte reported_index = (uint8)rr_gprs_3g_nlist_get_tdd_cell_index(rr_sys_info_3g_data_ptr->grr_3g_nlist, uarfcn, cpid);

      if (reported_index != MAX_GRR_3G_NLIST_SIZE)
      {
        int16 rep_quant_db = rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[tdd_measurements_index].rscp_total /
          rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[tdd_measurements_index].noof_measurements;

        rep_quant = rr_get_reported_RSCP(rep_quant_db);

        MSG_GERAN_MED_3_G("Adding %d, %d, %d to PMR", uarfcn, cpid, reported_index);

        meas_report->three_g_cell_list[*noof_entries].rep_quality = rep_quant;
        meas_report->three_g_cell_list[*noof_entries].three_g_cell_list_index = reported_index;

        *noof_entries += 1;
        noof_tdd_entries += 1;
      }
      else
      {
        MSG_GERAN_ERROR_2_G("Cell reported by L1 is not in 3G neighbour list, %d, %d, %d", uarfcn, cpid);
      }
    }

    tdd_measurements_index++;
  }

  return noof_tdd_entries;
}

/*! rr_gprs_tdd_3g_measurements_average

Calculates the average values for each 3G cell measurements are kept for.
*/
static void rr_gprs_average_tdd_3g_measurements(
  const gas_id_t gas_id
)
{
  int i;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  for (i = 0; i < rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count; i++)
  {
    rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[i].rscp_average =
      rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[i].rscp_total / rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[i].noof_measurements;
  }
}

static int compare_tdd_measurements_by_rscp(const void *arg1, const void *arg2)
{
  int rscp1, rscp2;

  ASSERT(arg1 != NULL);
  ASSERT(arg2 != NULL);

  rscp1 = ((tdd_measurement_t *) arg1)->rscp_average;
  rscp2 = ((tdd_measurement_t *) arg2)->rscp_average;


  if (rscp1 > rscp2)
  {
    return -1;
  }
  else if (rscp1 < rscp2)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

static void rr_gprs_3g_measurements_sort_tdd(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  if (rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count > MAX_GRR_3G_TDD_MEASUREMENTS)
  {
    rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count = MAX_GRR_3G_TDD_MEASUREMENTS;
  }

  rr_gprs_average_tdd_3g_measurements(gas_id);

  qsort(
    (void *) &rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[0],
    rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count,
    sizeof(tdd_measurement_t),
    compare_tdd_measurements_by_rscp
  );
}

static void rr_gprs_tdd_3g_measurements_sort(const gas_id_t gas_id)
{
  rr_gprs_3g_measurements_sort_tdd(gas_id);
}

static uint32 grr_add_tdd_cells_to_pemr(
  packet_enhanced_measurement_report_t *pemr,
  const gprs_scell_info_t *scell_ptr,
  const gas_id_t gas_id
)
{
  uint32                 num_of_cells_added = 0;
  uint32                 i;
  int16                  report_value;
  rr_sys_info_3g_data_t *rr_sys_info_3g_data_ptr = rr_get_sys_info_3g_data_ptr(gas_id);

  byte max_cells = scell_ptr->gprs_3g_meas_params.TDD_MULTIRAT_REPORTING;
  byte threshold = scell_ptr->gprs_3g_meas_params.TDD_REPORTING_THRESHOLD;

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  if (threshold <=6)
  {
    threshold = (byte) (threshold * 6);
  }
  else
  {
    threshold = 255; //infinite
  }


  MSG_GERAN_HIGH_2_G("Adding up to %d tdd cells to PEMR, threshold: %d", max_cells, threshold);

  for (i = 0; i < rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count && num_of_cells_added < max_cells; i++)
  {
    tdd_measurement_t *p_meas_entry = &(rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_entries[i]);

    report_value     = rr_get_reported_RSCP(p_meas_entry->rscp_average);

    if (report_value >= threshold)
    {
      /* get tdd cell's index */
      uint32 cell_index = (uint32)rr_gprs_3g_nlist_get_tdd_cell_index(rr_sys_info_3g_data_ptr->grr_3g_nlist,
                               p_meas_entry->uarfcn,
                               p_meas_entry->cpid);

      if (cell_index != MAX_GRR_3G_NLIST_SIZE)
      {
        MSG_GERAN_MED_0_G("Cell meets threshold criteria, add it to report");
        if (rr_sys_info_3g_data_ptr->grr_3g_nlist->absolute_index_start_EMR_valid)
        {
          cell_index += rr_sys_info_3g_data_ptr->grr_3g_nlist->absolute_index_start_EMR;
        }

        if (cell_index < MAX_REPORTING_QUANTITY)
        {
          MSG_GERAN_MED_3_G("Adding cell (%d, %d), %d to PEMR", p_meas_entry->uarfcn,
            p_meas_entry->cpid, cell_index);

          pemr->reporting_quantity[cell_index].valid = TRUE;
          pemr->reporting_quantity[cell_index].value = (byte)report_value;

          num_of_cells_added ++;
        }
        else
        {
          MSG_GERAN_ERROR_1_G("out of PEMR's capicity, %d", cell_index);
        }
      }
      else
      {
        MSG_GERAN_ERROR_3_G("Cell reported by L1 is not in 3G neighbour list, %d, %d, %d",
          p_meas_entry->uarfcn, p_meas_entry->cpid, cell_index);
      }

    }//end of if (better_than_threshold = TRUE)
  }//end of for

  return num_of_cells_added;
}
#endif /*FEATURE_GSM_TDS*/


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
/** Encodes a 3G measurement report if required.
*/
boolean rr_gprs_3g_meas_report_encode(
  three_g_measurement_report_t *meas_report,
  const gas_id_t gas_id
)
{
  uint8 noof_entries = 0;
#ifdef FEATURE_WCDMA
  uint8 noof_fdd_entries;
#endif /* FEATURE_WCDMA */
  boolean include_3g_meas_report = FALSE;
  gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);
#ifdef FEATURE_GSM_TDS
  uint8 noof_tdd_entries = 0;
#endif /*FEATURE_GSM_TDS*/

  uint8 WCDMA_QSearch_value = 15;
  
  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);


  /* 3GPP 45.008 Section 10, Table 3, Note 1:
  
  If PBCCH does not exist, the MS shall perform cell re-selection according to
  the idle mode procedures defined in clause 6 (i.e. use parameter Qsearch_I).
  In case parameter Qsearch_P is broadcast on BCCH it shall be used according
  to clause 10 only if GPRS cell re-selection parameters for one or more cells
  are provided to the MS in a Packet Cell Change Order or Packet Measurement 
  Order message. */
  if (rr_gprs_pmo_pcco_ncells_are_present(gas_id))
  {
    WCDMA_QSearch_value = scell->gprs_3g_meas_params.Qsearch_P;   
  }
  else
  {
    WCDMA_QSearch_value = scell->gsm.utran_neighbor_list.si2q_params.QSearch_I;
  }

  if (WCDMA_QSearch_value == 15)
  {
    MSG_GERAN_MED_0_G("Qsearch_P = 15, not including 3G cells in PMR");
  }
  else
  {
#ifdef FEATURE_GSM_TDS
    if (rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count &&
        rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count)
    {
      MSG_GERAN_ERROR_2_G("RR WCDMA and TDSCDMA measurements INVALID COMBINATION, FDD:%d, TDD:%d",
                rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count,
                rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.tdd_count);
      return FALSE;
    }

    rr_gprs_tdd_3g_measurements_sort(gas_id);

    noof_tdd_entries = rr_gprs_3g_meas_report_encode_tdd(
      meas_report,
      scell->gprs_3g_meas_params.TDD_MULTIRAT_REPORTING,
      &noof_entries,
      gas_id
    );

    if (noof_tdd_entries > 0)
    {
      include_3g_meas_report = TRUE;
      meas_report->n3g = noof_entries - 1;
    }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
    rr_gprs_3g_measurements_sort(gas_id);

    noof_fdd_entries = rr_gprs_3g_meas_report_encode_fdd(
      meas_report,
      scell->gprs_3g_meas_params.FDD_MULTIRAT_REPORTING,
      &noof_entries,
      gas_id
    );

    if (noof_fdd_entries > 0)
    {
      include_3g_meas_report = TRUE;
      meas_report->n3g = noof_entries - 1;
    }
#endif /* FEATURE_WCDMA */
  }
  return include_3g_meas_report;
}
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
/*=============================================================================

  FUNCTION rr_gprs_3g_meas_params_merge_mods

  DESCRIPTION
    Merges modifications to GPRS 3G Measurement Parameters received in a PCCO or
    PMO with an existing set of GPRS Measurement Parameters.

  PARAMETERS
    dst - Destination parameters
    src - Source parameters to merge in to destination

  RETURNS
    void

=============================================================================*/
static void rr_gprs_3g_meas_params_merge_mods(gprs_3g_meas_params_mods_t *dst, gprs_3g_meas_params_mods_t *src)
{
  if ((dst == NULL) || (src == NULL))
  {
    return;
  }

  if (src->valid)
  {
    dst->valid = TRUE;
    if (src->Qsearch_P_valid)
    {
      dst->Qsearch_P_valid = TRUE;
      dst->Qsearch_P = src->Qsearch_P;
    }

    if (src->_3G_SEARCH_PRIO_valid)
    {
      dst->_3G_SEARCH_PRIO_valid = TRUE;
      dst->_3G_SEARCH_PRIO = src->_3G_SEARCH_PRIO;
    }

    if (src->FDD_reporting_valid)
    {
      dst->FDD_reporting_valid = TRUE;
      dst->FDD_REP_QUANT = src->FDD_REP_QUANT;
      dst->FDD_MULTIRAT_REPORTING = src->FDD_MULTIRAT_REPORTING;
    }

    if (src->FDD_off_thresh_valid)
    {
      dst->FDD_off_thresh_valid = TRUE;
      dst->FDD_REPORTING_OFFSET = src->FDD_REPORTING_OFFSET;
      dst->FDD_REPORTING_THRESHOLD = src->FDD_REPORTING_THRESHOLD;
    }

    if (src->TDD_reporting_valid)
    {
      dst->TDD_reporting_valid = TRUE;
      dst->TDD_MULTIRAT_REPORTING = src->TDD_MULTIRAT_REPORTING;
    }

    if (src->TDD_off_thresh_valid)
    {
      dst->TDD_off_thresh_valid = TRUE;
      dst->TDD_REPORTING_OFFSET = src->TDD_REPORTING_OFFSET;
      dst->TDD_REPORTING_THRESHOLD = src->TDD_REPORTING_THRESHOLD;
    }

    if (src->CDMA2000_reporting_valid)
    {
      dst->CDMA2000_reporting_valid = TRUE;
      dst->CDMA2000_MULTIRAT_REPORTING = src->CDMA2000_MULTIRAT_REPORTING;
    }

    if (src->CDMA2000_off_thresh_valid)
    {
      dst->CDMA2000_off_thresh_valid = TRUE;
      dst->CDMA2000_REPORTING_OFFSET = src->CDMA2000_REPORTING_OFFSET;
      dst->CDMA2000_REPORTING_THRESHOLD = src->CDMA2000_REPORTING_THRESHOLD;
    }
  }
}

static uint32 grr_add_fdd_cells_to_pemr(
  packet_enhanced_measurement_report_t *pemr,
  const gprs_scell_info_t *scell_ptr,
  const gas_id_t gas_id
)
{
  uint32                 num_of_cells_added = 0;
  uint32                 i;
  int16                  report_value;
  int16                  non_report_value;
  rr_sys_info_3g_data_t *rr_sys_info_3g_data_ptr = rr_get_sys_info_3g_data_ptr(gas_id);

  byte max_cells = scell_ptr->gprs_3g_meas_params.FDD_MULTIRAT_REPORTING;
  byte threshold = scell_ptr->gprs_3g_meas_params.FDD_REPORTING_THRESHOLD;
  byte threshold2 = 0; //set to default value from now, disabled

  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  if (threshold <=6)
  {
    threshold = (byte) (threshold * 6);
  }
  else
  {
    threshold = 255; //infinite
  }


  MSG_GERAN_HIGH_3_G("Adding up to %d fdd cells to PEMR, threshold: %d, threshold2: %d", \
    max_cells, threshold, threshold2);

  for (i = 0; i < rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_count && num_of_cells_added < max_cells; i++)
  {
    fdd_measurement_t *p_meas_entry = &(rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements.fdd_entries[i]);

    if (scell_ptr->gprs_3g_meas_params.FDD_REP_QUANT == (int)FDD_REP_QUANT_RSCP)
    {
      report_value     = p_meas_entry->rscp_average;
      non_report_value = p_meas_entry->ecio_average;
      report_value     = rr_get_reported_RSCP(report_value);
      non_report_value = rr_get_reported_Ec_No(non_report_value);
    }
    else if (scell_ptr->gprs_3g_meas_params.FDD_REP_QUANT == (int)FDD_REP_QUANT_Ec_No)
    {
      report_value     = p_meas_entry->ecio_average;
      non_report_value = p_meas_entry->rscp_average;
      report_value     = rr_get_reported_Ec_No(report_value);
      non_report_value = rr_get_reported_RSCP(non_report_value);
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Unknown FDD report quant");
      continue;
    }

    if ((report_value >= threshold) && (non_report_value >= threshold2))
    {
      /* get fdd cell's index */
      uint32 cell_index = (uint32)rr_gprs_3g_nlist_get_fdd_cell_index(rr_sys_info_3g_data_ptr->grr_3g_nlist,
                               p_meas_entry->uarfcn,
                               p_meas_entry->scrambling_code);

      if (cell_index != MAX_GRR_3G_NLIST_SIZE)
      {
        /* The RSSI for the frequency should be reported first
         * The parameter FDD_MULTIRAT_REPORTING indicates a number of cells
         * to be reported and does not include the number of places taken by
         * RSSI reporting in the measurement report message
         */
        uint32 freq_index = (uint32)rr_gprs_3g_nlist_get_fdd_freq_index(rr_sys_info_3g_data_ptr->grr_3g_nlist,
                              p_meas_entry->uarfcn);
        if (freq_index != MAX_GRR_3G_NLIST_SIZE)
        {
          if (rr_sys_info_3g_data_ptr->grr_3g_nlist->absolute_index_start_EMR_valid)
          {
            freq_index += rr_sys_info_3g_data_ptr->grr_3g_nlist->absolute_index_start_EMR;
          }

          if (freq_index < MAX_REPORTING_QUANTITY)
          {
            if (pemr->reporting_quantity[freq_index].valid == TRUE)
            {
              MSG_GERAN_MED_2_G("RSSI on frequency %d has been included, index %d",
                p_meas_entry->uarfcn, freq_index);
            }
            else
            {
              MSG_GERAN_MED_2_G("Adding frequency %d, %d to PEMR", p_meas_entry->uarfcn, freq_index);
              pemr->reporting_quantity[freq_index].valid = TRUE;
              pemr->reporting_quantity[freq_index].value = rr_get_reported_RSSI(p_meas_entry->rssi_average);
            }
          }
          else
          {
            MSG_GERAN_ERROR_1_G("out of PEMR's capicity, %d", freq_index);
          }
        }

        MSG_GERAN_MED_0_G("cell meets threshold criteria, add it to report");
        if (rr_sys_info_3g_data_ptr->grr_3g_nlist->absolute_index_start_EMR_valid)
        {
          cell_index += rr_sys_info_3g_data_ptr->grr_3g_nlist->absolute_index_start_EMR;
        }

        if (cell_index < MAX_REPORTING_QUANTITY)
        {
          MSG_GERAN_MED_3_G("Adding cell (%d, %d), %d to PEMR", p_meas_entry->uarfcn,
            p_meas_entry->scrambling_code, cell_index);

          pemr->reporting_quantity[cell_index].valid = TRUE;
          pemr->reporting_quantity[cell_index].value = (byte)report_value;

          num_of_cells_added ++;
        }
        else
        {
          MSG_GERAN_ERROR_1_G("out of PEMR's capicity, %d", cell_index);
        }
      }
      else
      {
        MSG_GERAN_ERROR_3_G("Cell reported by L1 is not in 3G neighbour list, %d, %d, %d",
          p_meas_entry->uarfcn, p_meas_entry->scrambling_code, cell_index);
      }

    }//end of if (better_than_threshold = TRUE)
  }//end of for

  return num_of_cells_added;
}
void rr_gprs_clear_3g_measurement(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_meas_rpt_control_data_ptr);

  memset(&rr_gprs_meas_rpt_control_data_ptr->grr_3g_measurements, 0, sizeof(grr_3g_measurements_t));
}
#endif /* FEATURE_WCDMA */

/*=============================================================================

  FUNCTION rr_nc_meas_params_merge_mods

  DESCRIPTION
    Merges modifications to NC Measurement Parameters received in a PCCO or
    PMO with an existing set of NC Measurement Parameters.  This takes in to
    account that NC Measurement Parameters may or may not include the Period
    information.  If it is not included in the src NC Measurement Parameters,
    the values in the dst (existing) parameters will be used.

  PARAMETERS
    dst - Destination parameters
    src - Source parameters to merge in to destination

  RETURNS
    void

=============================================================================*/
static void rr_nc_meas_params_merge_mods(nc_meas_params_mods_t *dst, nc_meas_params_mods_t *src)
{
  if ((dst == NULL) || (src == NULL))
  {
    return;
  }

  if (src->valid)
  {
    dst->valid = TRUE;
    dst->network_control_order = src->network_control_order;

    if (src->nc_period_valid)
    {
      dst->nc_period_valid = TRUE;
      dst->nc_non_drx_period = src->nc_non_drx_period;
      dst->nc_reporting_period_i = src->nc_reporting_period_i;
      dst->nc_reporting_period_t = src->nc_reporting_period_t;
    }
  }
}

/*=============================================================================

  FUNCTION rr_gprs_meas_params_merge_mods

  DESCRIPTION
    Merges modifications to GPRS Measurement Parameters.

  PARAMETERS
    dst - Destination parameters
    src - Source parameters to merge in to destination

  RETURNS
    void

=============================================================================*/
static void rr_gprs_meas_params_merge_mods(gprs_meas_params_mods_t *dst, gprs_meas_params_mods_t *src)
{
  if ((dst == NULL) || (src == NULL))
  {
    return;
  }

  if (src->valid)
  {
    dst->valid = TRUE;

    if (src->pmo_ind_valid)
    {
      dst->pmo_ind_valid = src->pmo_ind_valid;
      dst->pmo_ind = src->pmo_ind;
    }
    if (src->report_type_valid)
    {
      dst->report_type_valid = TRUE;
      dst->report_type = src->report_type;
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

    dst->scale_ord_valid = TRUE;
    dst->scale_ord = src->scale_ord;

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
}



static void rr_gprs_nacc_params_merge_mods( pmo_pcco_nacc_params_t *dst , pmo_pcco_nacc_params_t *src )
{
  if ((dst == NULL) || (src == NULL))
  {
    return;
  }

  if ( src->valid_flag )
  {
    *dst = *src;
  }
}

void rr_gprs_merge_nc_nacc_into_cell_nacc_params(
  gprs_cell_options_t *gprs_cell_options,
  pmo_pcco_nacc_params_t *local_final_pmo_pcco_nacc_params_ptr,
  const gas_id_t gas_id
)
{
  if (local_final_pmo_pcco_nacc_params_ptr->valid_flag)
  {
    /* 3GPP 44.060 V4.19.0, Table 11.2.4.1 states:
     * CCN_ACTIVE (1 bit field)
     * This field indicates whether CCN is enabled for the mobile station in the cell
     * It is coded as follows:
     *   0  The broadcast CCN_ACTIVE parameter shall apply if available. Otherwise, CCN is disabled for the mobile station.
     *   1  CCN is enabled for the mobile station.
     */
    if (local_final_pmo_pcco_nacc_params_ptr->ccn_active == 1)
    {
      /* CCN definitely enabled */
      gprs_cell_options->optional_extension_info_flag = valid;
      gprs_cell_options->optional_extension_info.ccn_active = 1;
    }

    if (local_final_pmo_pcco_nacc_params_ptr->ccn_support_flag)
    {
      rr_gprs_merge_ccn_support_into_ba_list(
        &local_final_pmo_pcco_nacc_params_ptr->ccn_supported_desc,
        gas_id
      );
    }
  }
}



boolean rr_meas_reporting_is_active(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_gprs_meas_rpt_control_data_t *rr_gprs_meas_rpt_control_data_ptr;
  rr_gprs_meas_rpt_control_data_ptr = rr_gprs_meas_rpt_control_get_data_ptr(gas_id);

  if (rr_gprs_meas_rpt_control_data_ptr != NULL)
  {
  if ((rr_gprs_meas_rpt_control_data_ptr->state == NC_MEAS_ACQUIRE) ||
      (rr_gprs_meas_rpt_control_data_ptr->state == NC_MEAS_SEND))
  {
      return TRUE;
    }
  }

  return FALSE;
}

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */


/**
  @brief Initialisation function called at RR task start
 
  Note: The primary purpose of this function is to initialise
  pointers to control/data structures. This should also take
  into account any second instance of data, as this is a
  one-off initialisation.
*/ 
void rr_gprs_meas_rpt_control_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset((void *)rr_gprs_meas_rpt_control_data, 0, sizeof(rr_gprs_meas_rpt_control_data));

  // Setup any specific values
  rr_gprs_meas_rpt_control_data[GERAN_DATA_SPACE_INDEX_1].state     = NC_MEAS_NULL;
  rr_gprs_meas_rpt_control_data[GERAN_DATA_SPACE_INDEX_1].old_state = NC_MEAS_NULL;
#ifdef FEATURE_DUAL_SIM
  rr_gprs_meas_rpt_control_data[GERAN_DATA_SPACE_INDEX_2].state = NC_MEAS_NULL;
  rr_gprs_meas_rpt_control_data[GERAN_DATA_SPACE_INDEX_2].old_state = NC_MEAS_NULL;
#endif /* FEATURE_DUAL_SIM */

} /* end rr_gprs_meas_rpt_control_task_start_init() */

/* EOF */

