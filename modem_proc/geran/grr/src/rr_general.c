/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR General

GENERAL DESCRIPTION
   This module contains general/auxiliary functions used by the other RR modules

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_general.c_v   1.22   17 Jul 2002 11:50:36   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_general.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
May/16/12    sjw    Force PLMN search to foreground if both stacks in forbidden LAI
Apr/16/12    sjw    Added GAS ID tags to paging mode change debug
Feb/27/12    sjw    Update for FEATURE_UMTS_VOICE_CIPHERING_IND after handover
Feb/21/11    sjw    Validate the length octet when retrieving stored IMSI
Aug/18/09    sjw    Added support for RXQUAL reporting to CM during 2G PS calls
May/13/09    sjw    Start relocation of dedicated mode data to a single structure
Mar/18/09    sjw    Relocated timer definitions to new rr_timer module
Mar/11/09    sjw    Moved global voice and data support flags into rr_channel_mode.c
Mar/09/09    sjw    Relocated rr_channel_mode_supported() to rr_channel_mode.c
Feb/13/09    sjw    Reworked local storage for ciphering module data
May/17/07     ld    Defined rr_get_current_bs_info() for all of CGPS
Dec/16/04    sbs    Corrected polarity of Command / Response flag when constructing LCS
                      APDU.
Aug/19/04    ld     Added code to populate the bsic and arfcn
                      in rr_get_current_bs_info() function.
Jun/01/04    VT     In rr_check_in_band(), treat BAND_PGSM to be in-band with
                      BAND_EGSM, if the phone supports BAND_EGSM.
May/26/04    sbs    Added missing call to lsm_get_free_cmd() before calling
                    lsm_cmd_q_put() in the function rr_process_apdu_data_seg().
May/26/04    DMB    Remove unused events from rr_event_name()
May/18/04    VT     In rr_perform_early_camping(), changed the invalid check for
                      Control Chn Description in SysInfo DB, from ASSERT to ERR_FATAL.
                    In rr_is_allowed_band(), removed an unwanted ASSERT.
May/18/04    VT     In rr_plmn_list_add(), while adding a new entry in the list,
                      checking if the PLMN Id is valid by calling sys_plmn_id_is_valid().
May/14/04    VT     In rr_set_allowed_bands() and rr_set_supported_bands(), setting
                      BAND_PGSM also whenever we set BAND_EGSM.
05/07/04     gc     Enhanced Trace Buffer Code
Apr/29/04    VT     In rr_get_maximum_power_output(): Changed the initialization
                      value for maximum_rf_power_output from 0 to 22dBm. Replaced
                      an ASSERT for band check with a MSG_ERROR.
Apr/19/04    VT     QuadBand: In rr_get_num_freq_monitor(), use the band group
                      provided in the PLMN_LIST_REQ to search alternative bands.
                      Otherwise, use band group specified in PLMN_SELECT_REQ.
Apr/19/04   dmb     rr_start_idle_mode() always now called with serving cell data; Copy from pending store
                    made in cell selection code before starting idle
Apr/06/04    VT     In rr_perform_bcch_list_decode_from_candidate_db(), added a check
                      for num_frequencies == 0 in candidate DB.
Apr/1/2004  SBS     Cleaned up LINT warnings in FEATURE_MMGPS related code
April/01/02 NT      Added sending of rr_send_mph_read_sys_info_req after idle_update on ccch
                    to stop L1 from carrying on reading sys info.
Mar/24/2004 VT      Removed the unused #define RR_FIRST_SYS_INFO_SUSPEND_TIMEOUT
Mar/09/04   VT      In rr_cancel_timer(): Even if the timer has expired or
                      doesn't exist, set the RR-internal timer state to
                      STOPPED, as it might be in EXPIRED state.
Mar/05/04    VT     QuadBand: In rr_set_allowed_bands() and rr_set_supported_bands(),
                      resetting bands that are not set in band_pref.
Mar/05/04    VT     In rr_get_band(): Initializing band to BAND_NONE instead of 0.
Mar/02/04    VT     In rr_cancel_timer(): Timer doesn't exist is a MSG_LOW now.
Feb/24/04    VT     In rr_cancel_timer(): Added INTLOCK and INTFREE.
                    In rr_cancel_timer(): Added call to gs_timer_exists(), to
                      check if the timer is created.
Feb/12/04    VT     QuadBand: Added new function rr_set_supported_bands()
Jan/26/04    VT     QuadBand: In rr_set_allowed_bands(), if band_pref bit-mask
                      has both PCS and DCS enabled, Set EGSM and DCS as allowed
                      (Temporary).
Jan/21/04    VT     QuadBand: Added new function rr_set_allowed_bands()
Jan/09/04    VT     QuadBand: To get the correct Band in the PCS1900/DCS1800 overlap range,
                      modified rr_get_band() and rr_is_allowed_band().
01/07/04    gc      Fixes for SI Optimisation
Dec/16/03    VT     QuadBand: Re-architected rr_perform_power_scan(), in the process removing bugs.
Oct/21/03    VT     In rr_set_channel_frequency_list_from_mobile_allocation(),
                      removed ASSERT(channel_info_ptr->frequency_list.no_of_items == 1)
                      but, printing a MSG_ERROR instead.
06/30/03    sbs     Added support for Location Services related APDU handling.
05/08/03    he      Featurized debug string logging (RR_DIAG_STR)
05/02/03    He      Removed clear pscan results from reset function.
02/27/03    RG      Added routine to return L2 channel type based on the channel configuration information.
02/19/03    Rao     Added GSM Deep Sleep support.
02/18/02    JAC     Added rr_candidate_db_find_strongest and rr_candidate_db_sort_rxlev, lint
02/14/03    RG      Added support for G2W blind Handover
02/07/03    he      Added EV_CELL_REEST_TRIGGERED to rr_show_event function.
12/02/02    JAC     Moved timing advance saving outside of FEATURE_MMGPS
                    Added rr_move_zero_in_frequency_list
                    Added generation of idle meas reports for USAT
12/4/02     RG      Fix for problem found when unit testing WCDMA reject and threshold
                    database functionality.
11/20/02    SBS     Added rr_get_current_timing_advance_info,
                    rr_update_current_timing_advance_info, and rr_get_current_bs_info
                    to allow LSM task to retrieve TA, Cell ID, and LAI from RR
                    when FEATURE_MMGPS is defined.
11/18/02    RG      Fixes for problems found during unit testing GSM to WCDMA reselection.
10/29/02    Rao     Added InterRAT Reselection related Events.
10/28/02    Rao     Added sending RESET_SOFTWARE to W2G Reselection SM.
10/28/02    JAC     Added arfcn 0 fix, put in front in
                    rr_set_channel_frequency_list_from_mobile_allocation
10/16/02    JAC     Modified number of freqs to decode to 35(gsm)/50(dcs)/85(combined)
10/08/02    JAC     Changed rr_check_frequency_list to explicitly check for BAND_NONE
09/25/02    rmc     Removing use of "rr_globals.h" which is not required
09/23/02    JAC     Added check to see if we need to send logging info during a reset
08/26/02    rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
08/16/02    Rao     Removed Extra InterRAT HO Abort Event.
08/15/02    Rao     Fixed the problem of accessing unitialized BS_PA_MFRMS for InterRAT HO.
08/07/02    he      Reseting TC counter in system information database structure.
08/07/02    he      Reseting TC counter in system information database structure.
07/31/02    rmc     Added RR timer state management functions
07/26/02    JAC     Added functions for power scan result db, candidate cell db,
                   and available plmn db
07/25/02    Rao     Added a general util function to send L3 messages to L2
07/11/02    JAC     Added reset of control_channel_description_valid in clear functions
07/08/02    rmc     Removed CBQ2 use in RR
06/19/02    JAC     Added support for early camping
06/13/02    rmc     Use multiband accessors rather than multiband_info directly
06/12/02    rmc     Change in inter_task_lai_T
05/30/02    Rao     Getting rid of Path Loss timer - Reselection Optimizations
05/30/02    rmc     Changes to enable MULTIBAND_SUPPORT flag
05/22/02    rmc     Changes to MM-RR interface for dual mode
04/29/02    rmc     Fix for sending MPH_READ_SYS_INFO_REQ to soon after START_IDLE_REQ
04/10/02    rmc     Dual Mode Cell Selection changes
03/28/02    rmc     Temporary fix for CR21280, reset SIM info for software reset
03/18/02    SB      Dual Mode Activation / Deactivation changes
03/06/02    rmc     Temporary fix for new MM cipher key sequence number change
03/01/02    ATM     Hack until RR upgrades to new MM for cipher_sequence_number
02/28/02    JAC     added events for immediate cell reselection
02/27/02    rao     Added Dual Mode Events for rr_event_name.
02/20/02    rmc     Added accessors for current_page_mode (CR20636)
02/08/02    rmc     QXDM Cleanup + Logging
02/04/02    rao     Added New Accessor functions for SIM info access
01/30/02    RMC     Cell Reselection Cleanup
12/11/01    RMC     Fixed bug in rr_start_idle_mode. Now correctly uses data source
                    to initialize system information database
11/09/01    RMC     Fixed bug in rr_get_cell_priority, reset CellChannelDescription list when
                    resetting software
12/11/00    TS      Initial Revision.

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include <stdlib.h>
#include <string.h>
#include "cause.h"
#include "rr_seg_load.h"
#include "rr_general.h"
#include "rr_procedure.h"
#include "rr_decode_ie.h"
#include "rr_l1_send.h"
#include "rr_mm_send.h"
#include "rr_sys_info.h"
#include "rr_cell_selection.h"
#include "rr_shared_cell_selection.h"
#include "rr_ded_meas_reporting.h"
#include "rr_conn_establish.h"
#include "rr_conn_release.h"
#include "rr_control.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "uim.h"
#include "uim_v.h"
#include "amssassert.h"
#include "rr_l2_send.h"
#include "rr_log.h"
#include "ms.h"
#include "rr_channel_mode.h"
#include "rr_plmn_list.h"
#include "rr_mac_send.h"
#include "rr_inter_ho.h"
#include "rr_sapi3.h"
#include "rr_mode.h"
#include "rr_cipher.h"
#include "rr_acq_db.h"
#include "gsbitstream.h"
#include "rr_sys_info_ncell.h"
#include "rr_gprs_defs.h"
#include "rr_w2g_service_redirection.h"
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
#include "rr_x2g_resel.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "rr_multiband_config.h"
#include "sd.h"
#include "sd_v.h"
#include "rr_g2w_handover.h"
#include "rr_rrc_send.h"
#include "sys.h"
#include "sys_v.h"
#include "reg_sim.h"
#include "rr_main.h"
#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
#include "gprs_odm_info.h"
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */
#include "rr_candidate_cell_db.h"
#include "rr_arfcn.h"
#include "rr_cell_reestablish.h"
#include "geran_eng_mode_info.h"
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
#include "rr_callback.h"
#include "rr_resel_calcs.h"
#endif /*FEATURE_GSM_RR_CELL_CALLBACK*/
#include "grr_mutex.h"
#include "gprs_mem.h"
#include "rr_init.h"
#include "rr_resel_utils.h"
#include "rr_sys_info_pcell.h"
#ifdef FEATURE_GSM_DTM
#include "rr_dtm.h"
#endif /* FEATURE_GSM_DTM */
#include "rr_nv.h"
#ifdef FEATURE_WCDMA
#include "rr_g2w.h"
#endif /* FEATURE_WCDMA */
#include "rr_diag_if.h"
#ifdef FEATURE_LTE
#include "lte_irat_types.h"
#include "lte_rrc_ext_api.h"
#endif /* FEATURE_LTE */
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
#include "rr_g2x.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
#include "rr_multi_sim.h"
#ifdef FEATURE_DUAL_SIM
extern sd_ss_band_pref_e_type sd_misc_get_subs_supp_band_pref(sys_modem_as_id_e_type);
#endif /* FEATURE_DUAL_SIM */
#include "rr_ded_config.h"
#include "rr_sim.h"
#ifdef FEATURE_GSM_TDS
#include "tds_rrc_api.h"
#endif /*FEATURE_GSM_TDS*/
#include "rr_msgr.h"
#include "rr_sys_info_cache.h"
#include "rr_nv.h" 
#include "rr_gprs_meas_rpt_control.h" 
#include "stringl/stringl.h"
#ifdef FEATURE_CGPS_UMTS_CP_GSM
#include "rr_cgps.h"
#endif /* FEATURE_CGPS_UMTS_CP_GSM */
#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
#include "rr_tds_send.h"
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/
#ifdef FEATURE_QSH_EVENT_METRIC
#include "rr_qsh_metric.h"
#endif // FEATURE_QSH_EVENT_METRIC


#ifdef FEATURE_LTE
#include "mm_rr.h"
#endif /*FEATURE_LTE*/
extern void gl1_set_band_group (sys_band_mask_type sys_band_mask);
/* mmcp_sd_task_running **************************************************/
/* this function will be provided by MMCP but for now it's in rr_main.c */
extern boolean mmcp_sd_task_running(void);
/* mmcp_sd_task_running **************************************************/

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

typedef enum
{
  DTX_OVERRIDE_INACTIVE,                     /* Use value broadcast by network */
  DTX_NAS_OVERRIDE_ACTIVE,                   /* Override value to FALSE */
  DTX_RR_OVERRIDE_ACTIVE,                    /* Override value to FALSE */
  DTX_OVERRIDE_INACTIVE_UPDATE_L1_PENDING    /* Override disabled - L1 not yet updated */

} dtx_override_status_t;

typedef struct
{
  /**
   * Used to detect Radio Link Failures (RLF) in dedicated mode based on
   * decodes of SACCH frames.
   */
  uint8                      S_counter;

  /* Keep a copy of the current timing advance as well as a validity flag */
  boolean                    rr_current_timing_advance_valid;

  /* for the other task to be able to query when needed  */
  byte                       rr_current_timing_advance;

  /* List of cells that are currently forbidden for C1 based reselection */
  rr_forbidden_cell_list_T   rr_forbidden_cell_list;

  /* Barred cell list. Cells barred due to authentication or authentication and ciphering failure */
  rr_barred_cell_info_T      rr_barred_cell_list[RR_MAX_NUM_OF_BARRED_CELLS];

  #if defined FEATURE_WCDMA || defined FEATURE_GSM_TDS
  uint8                      num_uarfcns_sent_to_l1;
  uint8                      num_uarfcns_sent_to_l1_in_dedicated;
  #endif /* FEATURE_WCDMA */

  /**
   * Contains dynamic information related to the current GSM band in use (based on current serving cell),
   * supported bands (can change as a triband MS moves between EGSM/DCS1800
   * and EGSM/PCS1900 modes), and power class associated with each band
   * (can change as MS is attached to car kits, etc.)
   */
  multiband_info_T           multiband_database;

  /* Bands allowed (band preference + band_capability) */
  sys_band_mask_type         bands_allowed;

  #ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
  boolean                    rr_plmn_lac_range_valid;
  gprs_odm_plmn_lac_ranges_t rr_plmn_lac_range;
  #endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */

  dtx_override_status_t      rr_dtx_override_status;

  rr_hz_info_T               rr_hz_info;

  #ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
  boolean rr_using_pseudo_si2q_data;
  #endif
  
  #ifdef FEATURE_SGLTE
  #ifdef FEATURE_GSM_TDS
  boolean rr_sglte_l1_tds_meas_configured;
  #endif
  #endif

  #ifdef FEATURE_LTE
  boolean rr_l1_lte_meas_configured;
  rr_depriotized_gsm_cell_list_T depriotized_gsm_cell_list;
  #endif /* FEATURE_LTE */

} rr_general_data_t;

static rr_general_data_t  rr_general_data[NUM_GERAN_DATA_SPACES];

static usat_system_information_database_T  usat_system_information_database[NUM_GERAN_DATA_SPACES];

#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
static const PLMN_id_T ODM_PLMN_ID = {0x31, 0x10, 0x70};
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */

const unsigned int greatest_power_data[] =
{
  0,    /* 0 */
  1,    /* 1 */
  2,    /* 2 */
  2,    /* 3 */
  4,    /* 4 */
  4,    /* 5 */
  4,    /* 6 */
  4,    /* 7 */
  8,    /* 8 */
  8,    /* 9 */
  8,    /* 10 */
  8,    /* 11 */
  8,    /* 12 */
  8,    /* 13 */
  8,    /* 14 */
  8,    /* 15 */
  16,    /* 16 */
  16,    /* 17 */
  16,    /* 18 */
  16,    /* 19 */
  16,    /* 20 */
  16,    /* 21 */
  16,    /* 22 */
  16,    /* 23 */
  16,    /* 24 */
  16,    /* 25 */
  16,    /* 26 */
  16,    /* 27 */
  16,    /* 28 */
  16,    /* 29 */
  16,    /* 30 */
  16,    /* 31 */
  32,    /* 32 */
  32     /* 33 */
};

#ifdef FEATURE_LTE
  #define RR_WCDMA_DED_MEAS_NUM_SACCH_BLOCKS_PER_LTE_FREQ 5
#endif /* FEATURE_LTE */

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
  #define GRR_PSEUDO_SI2Q_MAX_EARFCN 8
#endif

/**
 * List of forbidden LAIs provided by the MM. Used to determine cell suitability
 * during cell selection/reselection. (Move to shared cell selection)
 */
#ifdef PERLUTF
#error code not present
#else
static forbidden_lai_list_T forbidden_lai_list[NUM_GERAN_DATA_SPACES];
#endif /*PERLUTF*/

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
/* This array defines the order in which tunnelled SI messages will be sorted,
and hence sent in to RR after choosing a cell.  This order is chosen to ensure
that early camping will not be performed as a result of tunnelled system
information.  Also, as SI3 is sent last this means that system information
acquisition will not be completed until all tunnelled system information has
been sent in to RR.  SI1 is always put at the front of the list if it is
included as this eases later checking for it's presence - only the first entry
need be examined, rather than iterating over the whole list. */
static const uint8 rr_x2g_si_message_order[] = {
  SYSTEM_INFORMATION_TYPE_1,
  SYSTEM_INFORMATION_TYPE_13,
  SYSTEM_INFORMATION_TYPE_2,
  SYSTEM_INFORMATION_TYPE_2_BIS,
  SYSTEM_INFORMATION_TYPE_2_TER,
  SYSTEM_INFORMATION_TYPE_2_QUATER,
  SYSTEM_INFORMATION_TYPE_4,
  SYSTEM_INFORMATION_TYPE_3,
};
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

/*+452757 ASR */
/**
  @brief Checks whether all SI2quater instances have been received.

  @return TRUE if all SI2quater instances have been received, FALSE
          otherwise.  Also returns FALSE if no SI2quater instances have been
          received, which means either that none are broadcast or none have
          been received.
*/
boolean rr_si2quater_all_instances_received(
  instances_T * si2quater_instances
)
{
  RR_NULL_CHECK_RETURN_PARAM(si2quater_instances, FALSE);

  if (si2quater_instances->num_instances > 0)
  {
    if (si2quater_instances->instance[0].valid_data)
    {
      if (si2quater_instances->instance[0].common_params.COUNT ==
          (si2quater_instances->num_instances - 1))
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
  else
  {
    return FALSE;
  }
}

/*-452757 ASR */

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_general_data_t*
 */
static rr_general_data_t *rr_general_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_general_data[as_index];
}

/*!
 * \brief Returns a pointer to the forbidden LAI list 
 *
 * \param as_id - NAS subscription ID
 *
 * \return forbidden_lai_list_T*
 */
static forbidden_lai_list_T *rr_get_forbidden_lai_list_ptr(const sys_modem_as_id_e_type as_id)
{
  /* N.B. There is no ASID to GAS ID mapping here: storage is deliberately per ASID */
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
#ifdef FEATURE_TRIPLE_SIM
  if (SYS_MODEM_AS_ID_3 == as_id)
  {
    return(&(forbidden_lai_list[GERAN_DATA_SPACE_INDEX_3]));
  }
  else
#endif /* FEATURE_TRIPLE_SIM */
  if (SYS_MODEM_AS_ID_2 == as_id)
  {
    return(&(forbidden_lai_list[GERAN_DATA_SPACE_INDEX_2]));
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(&(forbidden_lai_list[GERAN_DATA_SPACE_INDEX_1]));
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)

/*===========================================================================

FUNCTION  rr_camped_in_forbidden_LAI
  Returns TRUE if the specified access stratum is camped in a forbidden LAI

PARAMETERS
  gas_id - Dual Standby GERAN Access Stratum Identifier

Returns:
  TRUE if camped in a forbidden LAI, FALSE otherwise

===========================================================================*/

static boolean rr_camped_in_forbidden_LAI(const gas_id_t gas_id)
{
  gprs_scell_info_t    *scell_ptr;
  forbidden_lai_list_T *lai_list_ptr;
  int                   index;

  scell_ptr = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_ptr);

  // Obtain a pointer to the forbidden LAI list (stored by ASID)
  lai_list_ptr = rr_get_forbidden_lai_list_ptr(geran_map_gas_id_to_nas_id(gas_id));
  RR_NULL_CHECK_FATAL(lai_list_ptr);

  for (index = 0; index < lai_list_ptr->no_of_entries; index++)
  {
    if (scell_ptr != NULL)
    {
      /* see if it matches any of the LAIs in the list */
      if (rr_same_lai(&(scell_ptr->gsm.location_area_identification), &(lai_list_ptr->lai[index])))
      {
        /* found a match, the LAI is forbidden */
        return(scell_ptr->gsm.camped_on_flag);
      }
    }
  }

  return(FALSE);
} /* rr_camped_in_forbidden_LAI */

/*===========================================================================

FUNCTION rr_ms_active_stacks_in_forbidden_LAI

DESCRIPTION  Checks whether all active GERAN stacks are in a forbidden LAI

PARAMS       VOID

DEPENDENCIES None

RETURN VALUE TRUE if all active stacks are camped on a forbidden LAI

===========================================================================*/
boolean rr_ms_active_stacks_in_forbidden_LAI(void)
{
  gas_id_t gas_id;
  int      active_count = 0;
  int      forbid_count = 0;

  for (gas_id = GERAN_ACCESS_STRATUM_ID_1; gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; gas_id++)
  {
    if (rr_mode_gsm_active_or_starting(gas_id))
    {
      active_count++;
      if (rr_camped_in_forbidden_LAI(gas_id))
      {
        forbid_count++;
      }
    }
  }

  return((active_count != 0) && (active_count == forbid_count));
} /* rr_ms_active_stacks_in_forbidden_LAI */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q

/*===========================================================================

FUNCTION  rr_set_rr_using_pseudo_si2q_data
  This function sets the value of variable, within rr_general_data 
  rr_using_pseudo_si2q_data to TRUE or FALSE (based on the input arguments)
  Value TRUE is set by the caller, when RR is using Pesudo SI2Q data for LTE config.
  Value FALSE is set by the caller when RR no longer uses Pesudo SI2Q data for LTE config.

PARAMETERS
  rr_using_pseudo_si2q_data - boolean value to set TRUE/FALSE
  gas_id - Dual Standby GERAN Access Stratum Identifier

Returns:
  void 

===========================================================================*/

void rr_set_rr_using_pseudo_si2q_data( boolean rr_using_pseudo_si2q_data, const gas_id_t gas_id )
{
    // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  if (rr_using_pseudo_si2q_data != rr_general_data_ptr->rr_using_pseudo_si2q_data)
  { 
    MSG_GERAN_HIGH_1_G("rr_set_rr_using_pseudo_si2q_data, setting new value= %d", (int)rr_using_pseudo_si2q_data );
  }

  rr_general_data_ptr->rr_using_pseudo_si2q_data = rr_using_pseudo_si2q_data;

}



/*===========================================================================

FUNCTION  rr_is_pseudo_si2q_data_used
  Returns TRUE when the variable rr_using_pseudo_si2q_data in rr_general is set to TRUE
  This happens ONLY under following two conditions:
  When NAS responds successfully to valid EARFCN on cells.
  EFS has default data set for LTE EARFCN's.

PARAMETERS
  gas_id - Dual Standby GERAN Access Stratum Identifier

Returns:
  TRUE if camped in a forbidden LAI, FALSE otherwise

===========================================================================*/

boolean rr_is_pseudo_si2q_data_used( const gas_id_t gas_id )
{
    // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  
  return ( rr_general_data_ptr->rr_using_pseudo_si2q_data );

}

#endif

void rr_dtx_override_reset(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  MSG_GERAN_HIGH_0_G("DTX override reset");
  rr_general_data_ptr->rr_dtx_override_status = DTX_OVERRIDE_INACTIVE;
}

// N.B. Public API (Called from NAS)
/**
  @brief Turn DTM override on.

  This public function is called from NAS at certain times when a call
  is established to avoid a problem where noise was heard on the audio
  which was found to be related to DTX operation.

  This function allows NAS to override the DTX setting and force it
  OFF during certain signalling procedures.

  rr_dtx_override_off() then allows DTX to be turned on again.
**/
void geran_grr_dtx_override_on_per_subs(
  sys_modem_as_id_e_type as_id
)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);

  dtx_override_status_t  dtx_override_status;
  gprs_scell_info_t     *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  dtx_override_status = rr_general_data_ptr->rr_dtx_override_status;

  MSG_GERAN_HIGH_0_G("DTX override (NAS) on");
  rr_general_data_ptr->rr_dtx_override_status = DTX_NAS_OVERRIDE_ACTIVE;

  if (rr_in_dedicated(gas_id) &&
      (dtx_override_status != DTX_RR_OVERRIDE_ACTIVE))
  {
    rr_l1_update_dedicated_mode(&scell_info_ptr->gsm, gas_id);
  }
}

// N.B. Public API (Called from NAS)
void geran_grr_dtx_override_on(
  void
)
{
  // Assume SingleSIM API is for Sub1
  geran_grr_dtx_override_on_per_subs(SYS_MODEM_AS_ID_1);
}

void rr_dtx_override_on( void )
{
  // Assume legacy API is for Sub1
  geran_grr_dtx_override_on_per_subs(SYS_MODEM_AS_ID_1);
}

void geran_grr_dtx_override_off_per_subs(
  sys_modem_as_id_e_type as_id
)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);

  MSG_GERAN_HIGH_0_G("DTX override off");
  rr_general_data_ptr->rr_dtx_override_status = DTX_OVERRIDE_INACTIVE_UPDATE_L1_PENDING;
}

void geran_grr_dtx_override_off(
  void
)
{
  // Assume SingleSIM API is for Sub1
  geran_grr_dtx_override_off_per_subs(SYS_MODEM_AS_ID_1);
}

void rr_dtx_override_off( void )
{
  // Assume legacy API is for Sub1
  geran_grr_dtx_override_off_per_subs(SYS_MODEM_AS_ID_1);
}

void rr_dtx_rr_override_on(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  if (rr_general_data_ptr->rr_dtx_override_status != DTX_NAS_OVERRIDE_ACTIVE)
  {
    MSG_GERAN_HIGH_0_G("DTX override (RR) on");

    rr_general_data_ptr->rr_dtx_override_status = DTX_RR_OVERRIDE_ACTIVE;
  }
  else
  {
    MSG_GERAN_HIGH_0_G("DTX override (NAS) overrules RR");
  }
}

void rr_dtx_rr_override_off(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  if (rr_general_data_ptr->rr_dtx_override_status == DTX_RR_OVERRIDE_ACTIVE)
  {
    MSG_GERAN_HIGH_0_G("DTX override (RR) off");
    rr_general_data_ptr->rr_dtx_override_status = DTX_OVERRIDE_INACTIVE_UPDATE_L1_PENDING;
  }
  else
  if (rr_general_data_ptr->rr_dtx_override_status == DTX_NAS_OVERRIDE_ACTIVE)
  {
    MSG_GERAN_HIGH_0_G("DTX override (NAS) overrules RR");
  }
}

void rr_dtx_update_l1_if_required(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  if (rr_general_data_ptr->rr_dtx_override_status == DTX_OVERRIDE_INACTIVE_UPDATE_L1_PENDING)
  {
    MSG_GERAN_HIGH_0_G("DTX update L1");
    rr_l1_update_dedicated_mode(&scell_info_ptr->gsm, gas_id);
    rr_general_data_ptr->rr_dtx_override_status = DTX_OVERRIDE_INACTIVE;
  }
}

/**
 *
 * @return TRUE if RR is camped on the serving cell, FALSE otherwise
 */
boolean rr_is_camped(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  /* used the camped on flag in the SI DB to make the determination */
  return ((scell_info_ptr->gsm.camped_on_flag) ? TRUE : FALSE);
} /* rr_is_camped */

/**
 * Gets the maximum power output of the mobile
 *
 * @param rf_power_capability RF power capability of the mobile (defines the class
 *                            of the mobile)
 *
 * @param band                Band for which to get the max power output
 *
 * @return Max output power in dBm
 */
static byte rr_get_maximum_power_output(
  byte rf_power_capability,
  rr_internal_band_T band,
  const gas_id_t gas_id
)
{
  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);

  /* Changed from 0 to lowest maximum_rf_power_output for all bands *
   *   (including GMSK and 8-PSK Modulation), as specified in       *
   *   3GPP TS 05.05 Sec: 4.1.1                                     */
  byte maximum_rf_power_output = 22; /* dBm */

  if (rf_power_capability < RR_BAND_NUM_POWER_CLASSES)
  {
    /* ASSERT(band < MAXNO_BANDS); */
    if (band < MAXNO_BANDS)
    {
      /* use intlock since multiband_info can be changed by multiple tasks */
      GRR_GENERAL_LOCK();
      maximum_rf_power_output =
        multiband_info->band[band].max_power[rf_power_capability];
      GRR_GENERAL_UNLOCK();
    }
    else
    {
      MSG_GERAN_ERROR_1_G("Invalid band (%d)",(int)band);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Invalid rf_power_capability (%d)",(int)rf_power_capability);
  }

  return maximum_rf_power_output;
} /* rr_get_maximum_power_output */

/**
 * Gets the maximum power output of the mobile for a given frequency
 * Note: Called from WCDMA (WL1)
 *
 * @frequency - frequency required
 *
 * @return Max output power in dBm
 */
byte rr_get_maximum_power_output_for_frequency(
  ARFCN_T frequency
)
{
  return
    rr_get_maximum_power_output(
       rr_get_band_rf_power_class(GET_ARFCN_BAND(frequency)),
       GET_ARFCN_BAND(frequency),
       GERAN_ACCESS_STRATUM_ID_1
     );

} /* end rr_get_maximum_power_output_for_frequency() */

/**
 * Converts a RXLEV value into a dBm value
 *
 * @param rxlev  rxlev value
 *
 * @return dBm value
 */
int rxlev_to_dbm(
  byte rxlev
)
{
  int result;

  result = (int) rxlev - 111;

  return result;
}

/* retrieve the default RADIO-LINK-TIMEOUT for the current cell */
static uint8 rr_get_radio_link_timeout(
  const gas_id_t gas_id
)
{
  byte               radio_link_timeout;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  radio_link_timeout = scell_info_ptr->gsm.cell_options.RADIO_LINK_TIMEOUT;

  /* sanity check the value from the serving cell database */
  if ((radio_link_timeout < RR_MIN_RLT_COUNTER) ||
      (radio_link_timeout > RR_MAX_RLT_COUNTER))
  {
    /* value is out of range, use a default and flag the counter as invalid */
    radio_link_timeout = RR_DEF_RLT_COUNTER;
    scell_info_ptr->gsm.RLT_valid = FALSE;

    MSG_GERAN_ERROR_1_G("RLT counter out of range, using %d", (int)radio_link_timeout);
    (void)rr_log_cell_options(&(scell_info_ptr->gsm.cell_options), gas_id);
  }

  return((uint8)radio_link_timeout);
}/* end rr_get_radio_link_timeout() */

/* Reset the S_counter to the value of RADIO-LINK-TIMEOUT broadcast in sys info */
void rr_reset_S_counter(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  rr_general_data_ptr->S_counter = rr_get_radio_link_timeout(gas_id);

  return;
} /* end rr_reset_S_counter() */


/* Increment the S_counter, and return the new value */
uint8 rr_increment_S_counter(
  const gas_id_t gas_id
)
{
  uint8 max_rlt_counter = rr_get_radio_link_timeout(gas_id);

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  /* Increment the S counter by 2: see 3GPP 45.008 section 5.2 */
  rr_general_data_ptr->S_counter += 2;

  /* S counter is capped at RADIO-LINK-TIMEOUT from Si3 or Si6 Cell Options */
  if (rr_general_data_ptr->S_counter > max_rlt_counter)
  {
    rr_general_data_ptr->S_counter = max_rlt_counter;
  }

  return(rr_general_data_ptr->S_counter);
} /* end rr_increment_S_counter() */


/* Decrement the S_counter, and return the new value */
uint8 rr_decrement_S_counter(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  /* Decrement the S counter, if it is non-zero */
  if (rr_general_data_ptr->S_counter != 0)
  {
    rr_general_data_ptr->S_counter -= 1;
  }

  return(rr_general_data_ptr->S_counter);
} /* end rr_decrement_S_counter() */

/* Retrieve the current S_counter, and the maximum value */
/* Returns an indication of the validity of this maximum */
boolean rr_retrieve_S_counter(
  byte *now_ptr,
  byte *max_ptr,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  boolean            rlt_valid;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  if ((now_ptr != NULL) && (rr_general_data_ptr != NULL))
  {
    *now_ptr = rr_general_data_ptr->S_counter;
  }

  if (scell_info_ptr != NULL)
  {
    rlt_valid = scell_info_ptr->gsm.RLT_valid;
  }
  else
  {
    rlt_valid = FALSE;
  }

  if (max_ptr != NULL)
  {
    if (rlt_valid)
    {
      *max_ptr = scell_info_ptr->gsm.cell_options.RADIO_LINK_TIMEOUT;
    }
    else
    {
      *max_ptr = RR_MAX_RLT_COUNTER;
    }
  }

  return(rlt_valid);
}

/**
 * Compares two LAI values, returning TRUE if they are the same
 *
 * @param lai1   LAI1 value
 *
 * @param lai2   LAI2 value
 *
 * @return TRUE if the LAIs are the same, FALSE otherwise
 */
boolean rr_same_lai(
  LAI_T *lai1,
  LAI_T *lai2
)
{
  if ((lai1->plmn_id.identity[0] == lai2->plmn_id.identity[0]) &&
       (lai1->plmn_id.identity[1] == lai2->plmn_id.identity[1]) &&
       (lai1->plmn_id.identity[2] == lai2->plmn_id.identity[2]) &&
       (lai1->location_area_code == lai2->location_area_code)
     )
  {
    return TRUE;
  }

  return FALSE;
} /* rr_same_lai */


/**
 * Clears the pending_channel_data global store
 */
void rr_clear_pending_channel_data(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  rr_clear_system_information(&pcell_info_ptr->gsm);
}

#ifdef FEATURE_GERAN_HST_OPT
/*===========================================================================

FUNCTION  rr_is_hst_cell_freq_list_present

DESCRIPTION Checks if IRAT acq DB is valid AND is for a high speed cell

RETURN VALUE - TRUE - high speed freq list valid, FALSE - opposite

===========================================================================*/
boolean rr_is_hst_cell_freq_list_present(const gas_id_t gas_id)
{
  if ((rr_high_speed_ind_set()) && (rr_irat_acq_db_valid(gas_id)))
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
}
#endif /*FEATURE_GERAN_HST_OPT*/

/*===========================================================================

FUNCTION  rr_get_DTX_indicator

DESCRIPTION

Returns TRUE if DTX should be used, FALSE otherwise. Based on the value
of CellOptions received in SI3 and SI6. Uses global channel_configuration to
determine the channel type

See GSM 04.08 10.5.2.3 (CellOptions BCCH) and 10.5.2.3a (CellOptions SACCH)
for mapping.

RETURN VALUE

===========================================================================*/

boolean rr_get_DTX_indicator(
  const gas_id_t gas_id
)
{
  boolean            DTX_indicator = FALSE;
  byte               DTX_value     = 0;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  DTX_value = scell_info_ptr->gsm.cell_options.DTX;

  if (DTX_value > 7)
  {
    MSG_GERAN_ERROR_1_G("Invalid DTX value (%d)",(int)DTX_value);
  }
  else
  {
    channel_spec_T *chan_spec_ptr = rr_get_channel_spec(NULL, gas_id);

    if (chan_spec_ptr != NULL)
    {
      boolean half_rate_map[] = {FALSE,FALSE,FALSE,TRUE,TRUE,TRUE,TRUE,TRUE};
      boolean full_rate_map[] = {TRUE,TRUE,FALSE,TRUE,TRUE,TRUE,FALSE,TRUE};

      if (chan_spec_ptr->channel_info_1_after.channel_type == HALF_RATE_TRAFFIC)
      {
        DTX_indicator = half_rate_map[DTX_value];
      }
      else if (chan_spec_ptr->channel_info_1_after.channel_type == FULL_RATE_TRAFFIC)
      {
        DTX_indicator = full_rate_map[DTX_value];
      }
    }
  }

  /* Force the DTX indication to FALSE if overriding is active */
  if ( (rr_general_data_ptr->rr_dtx_override_status == DTX_NAS_OVERRIDE_ACTIVE) ||
       (rr_general_data_ptr->rr_dtx_override_status == DTX_RR_OVERRIDE_ACTIVE) )
  {
    DTX_indicator = FALSE;
  }

  return DTX_indicator;
}

/**
 * Determines the priority of a cell
 *
 * @param CELL_BAR_QUALIFY CBQ value for the cell
 *
 * @param CELL_BAR_ACCESS  cell bar access value received for the cell
 *
 * @param location_area_identification
 *                         For test mode of operation, the PLMN_id may be involved in priority determination
 *
 * @param cell_test_operation_ptr
 *                         [out]Set to TRUE if cell is only suitable because of cell test mode (may be
 *                         set to NULL if information is not needed)
 *
 * @return One of:<BR>
 *         LOW_PRIORITY  - Cell is a low priority cell<BR>
 *         HIGH_PRIORITY - Cell is a normal priority cell<BR>
 *         CELL_BARRED   - Cell is barred
 */
rr_cell_type_T rr_get_cell_priority(
  byte      CELL_BAR_QUALIFY,
  byte      CELL_BAR_ACCESS,
  LAI_T     *location_area_identification,
  byte      *access_control,
  boolean   *cell_test_operation_ptr,
  const gas_id_t gas_id
)
{
  rr_cell_type_T cell_type;
  ms_operation_mode_T ms_operation_mode = rr_siminfo_get_ms_opmode(gas_id);

  // assume initially that we are not in cell_test mode... set flag to FALSE
  if (cell_test_operation_ptr != NULL)
  {
    *cell_test_operation_ptr = FALSE;
  }

  if (CELL_BAR_QUALIFY == 1)
  {
    /* by definition, cells with CBQ set to 1 are low priority cells */
    cell_type = RR_LOW_PRIORITY_CELL;
  }
  else /* cell is not low priority */
  {
    if (ms_operation_mode == CELL_TEST_OPERATION)
    {
      /*****************************************/
      /* The MS is in cell test mode operation */
      /*****************************************/
      if (CELL_BAR_ACCESS == 1)
      {
        boolean  home_plmn;

        boolean  ACC_15_barred = FALSE;
		
#ifdef FEATURE_DUAL_SIM        
        sys_modem_as_id_e_type as_id = geran_map_gas_id_to_nas_id(gas_id);		 
#endif /* FEATURE_DUAL_SIM */
		
#ifndef FEATURE_EQUIVALENT_HPLMN
       sys_plmn_id_s_type	  hplmn;
#endif /* FEATURE_EQUIVALENT_HPLMN */
			
       /* As per 3GPP Spec. 45.008 - 9. Control parameters, UE should trigger to camp on with following conditions.
        * if the MS is A/Gb mode capable, if all the following conditions are met, then the "Cell selection priority" and 
        * the "Status for cell reselection" shall be set to normal (A/Gb mode):
        * - the cell belongs to the MS HPLMN if no EHPLMN list is available or is empty, or the cell belongs to any PLMN defined in the EHPLMN list,
        * - the MS is in cell test operation mode,
        * - the CELL_BAR_ACCESS is set to "1",
        * - the CELL_BAR_QUALIFY is set to "0",
        * - the Access Control class 15 is barred */
				 
		
		
#ifdef FEATURE_EQUIVALENT_HPLMN
		
		
#ifdef FEATURE_DUAL_SIM
       home_plmn = reg_sim_per_subs_is_ehplmn(location_area_identification->plmn_id, as_id);
#else
       home_plmn = reg_sim_is_ehplmn(location_area_identification->plmn_id);
#endif /* FEATURE_DUAL_SIM */
		
#else /* FEATURE_EQUIVALENT_HPLMN */
		
#ifdef FEATURE_DUAL_SIM
       hplmn = reg_sim_per_subs_hplmn_get(as_id);
#else
       hplmn = reg_sim_hplmn_get();		  
#endif /* FEATURE_DUAL_SIM */
		
       home_plmn = sys_plmn_match(hplmn,location_area_identification->plmn_id);
				
#endif /* FEATURE_EQUIVALENT_HPLMN */


        /***********************************/
        /* Is the access class 15 barred ? */
        /***********************************/
        ACC_15_barred = FALSE;

        if ((access_control[0] & 0x80) == 0x80)
        {
          ACC_15_barred = TRUE;
        }

        /* If SIM Access Control Class includes 15 and Cell Access Control indicates
        that Class 15 is barred and cell PLMN == hplmn then camping on the cell is
        allowed and cell access is allowed. */

        if ((home_plmn == TRUE) && (ACC_15_barred == TRUE))
        {
          /* the cell would normally be barred, but it should be considered normal for this mode */
          cell_type = RR_HIGH_PRIORITY_CELL;

          /* indicate that cell considered normal only because of cell test operation */
          if (cell_test_operation_ptr != NULL)
          {
            *cell_test_operation_ptr = TRUE;
          }
        }
        else
        {
          cell_type = RR_CELL_BARRED;
        }
      }
      else
      {
        cell_type = RR_HIGH_PRIORITY_CELL;
      }
    }
    else /* MS is not in cell test operation mode */
    {
      if (CELL_BAR_ACCESS == 1)
      {
        cell_type = RR_CELL_BARRED;
      }
      else
      {
        cell_type = RR_HIGH_PRIORITY_CELL;
      }
    }
  } /* cell is not low priority */

  return cell_type;
} // rr_get_cell_priority

/**
 * Function: rr_determine_actual_band
 *
 * @param local_arfcn  channel number (0..1023)
 * @param band_ind     band indicator (0..1) to use to resolve PCS/DCS conflicts
 *
 * @return band associated with the given channel or
 *         BAND_NONE if the ARFCN is unknown or not supported
 */
rr_internal_band_T rr_determine_actual_band(
  word              local_arfcn,
  si_psi_band_ind_T band_ind
)
{
  rr_internal_band_T     actual_band;

  /****************************************************************
   * First of all, determine the actual band that the ARFCN is in *
   ****************************************************************/
  if (local_arfcn == 0)
  {
    actual_band = BAND_EGSM;
  }
  else if ((local_arfcn >= RR_ARFCN_EGSM_900_UPPER_RANGE_MIN) &&
           (local_arfcn <= RR_ARFCN_EGSM_900_UPPER_RANGE_MAX))
  {
    actual_band = BAND_EGSM;
  }
  else if ((local_arfcn >= RR_ARFCN_PGSM_900_MIN) &&
           (local_arfcn <= RR_ARFCN_PGSM_900_MAX))
  {
    actual_band = BAND_PGSM;
  }
  else if ((local_arfcn >= RR_ARFCN_DCS_1800_MIN) &&
           (local_arfcn <= RR_ARFCN_DCS_1800_MAX))
  {
    if (local_arfcn <= RR_ARFCN_PCS_1900_MAX)
    {
      /* use the supplied band indicator to resolve the DCS/PCS overlap */
      if (band_ind == SI_PSI_BAND_IND_PCS1900)
      {
        actual_band = BAND_PCS;
      }
      else
      {
        actual_band = BAND_DCS;
      }
    }
    else
    {
      actual_band = BAND_DCS;
    }
  }
  else if ((local_arfcn >= RR_ARFCN_CELL_850_MIN) &&
           (local_arfcn <= RR_ARFCN_CELL_850_MAX))
  {
    actual_band = BAND_GSM_850;
  }
  else
  {
    if (local_arfcn != INVALID_ARFCN)
    {
      MSG_GERAN_LOW_1("Unknown ARFCN %d, unable to determine band", (int)local_arfcn);
    }
    actual_band = BAND_NONE;
  }

  return actual_band;
}

/**
 *
 * Returns a pointer to the multband info database
 *
 */
multiband_info_T *rr_get_multiband_info(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  return &rr_general_data_ptr->multiband_database;

} /* end rr_get_multiband_info() */

/**
 *
 * Query SD for current band support, and update RR information
 *
 */
static void rr_update_band_support_info(
  const gas_id_t gas_id
)
{
  sd_ss_band_pref_e_type user_band_cap;
  sys_band_mask_type bands_allowed;
  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  // bands_allowed = system band preference + user band capability

  // Set the initial set of bands allowed from last signalled band_pref
  bands_allowed = rr_band_pref(gas_id);

	if (mmcp_sd_task_running())
	{

    // Now add in any other bands indicated as supported from SD
    // sd_misc_get_supp_band_pref() returns bands which the RF supports, but removes bands
    //  - which are not selected on the UI
    //  - which are not supported by the current subscription
#ifdef FEATURE_DUAL_SIM
    user_band_cap = sd_misc_get_subs_supp_band_pref(geran_map_gas_id_to_nas_id(gas_id));
#else
    user_band_cap = sd_misc_get_supp_band_pref();
#endif /* FEATURE_DUAL_SIM */
	}
	else /* SD task not able to give a result yet, provide default until it can */
	{
		user_band_cap = SD_SS_BAND_PREF_NONE;
	}

  if (SD_BAND_CONTAIN(user_band_cap, SD_SS_BAND_PREF_GSM_RGSM_900))
  {
    bands_allowed |= SYS_BAND_MASK_GSM_RGSM_900;
  }
  else
  if (SD_BAND_CONTAIN(user_band_cap, SD_SS_BAND_PREF_GSM_EGSM_900))
  {
    bands_allowed |= SYS_BAND_MASK_GSM_EGSM_900;
  }
  else
  if (SD_BAND_CONTAIN(user_band_cap, SD_SS_BAND_PREF_GSM_PGSM_900))
  {
    bands_allowed |= SYS_BAND_MASK_GSM_PGSM_900;
  }

  if (SD_BAND_CONTAIN(user_band_cap, SD_SS_BAND_PREF_GSM_DCS_1800))
  {
    bands_allowed |= SYS_BAND_MASK_GSM_DCS_1800;
  }

  if (SD_BAND_CONTAIN(user_band_cap, SD_SS_BAND_PREF_GSM_PCS_1900))
  {
    bands_allowed |= SYS_BAND_MASK_GSM_PCS_1900;
  }

  if (SD_BAND_CONTAIN(user_band_cap, SD_SS_BAND_PREF_GSM_850))
  {
    bands_allowed |= SYS_BAND_MASK_GSM_850;
  }

  /* When building the Radio Access Capabilities IE, this must contain at least one GSM */
  /* band. 24.008 suggests that P-GSM should be always included as a minimum. (See also */
  /* spec CR734) */
  /* When in W-only, SD will return no GSM bands, but RR is still queried during the */
  /* construction of the Radio Access Capabilities IE */

  /*lint -save -e655  */
  if ( SYS_BAND_MASK_EMPTY ==
       (bands_allowed &
        (SYS_BAND_MASK_GSM_RGSM_900 | SYS_BAND_MASK_GSM_PGSM_900 |
         SYS_BAND_MASK_GSM_EGSM_900 | SYS_BAND_MASK_GSM_DCS_1800 |
         SYS_BAND_MASK_GSM_PCS_1900 | SYS_BAND_MASK_GSM_850))
     )
  {
#ifdef FEATURE_LTE
    /* If an L2G Manual PLMN List Search is initiated the SD API returns
    no bands.  The band_pref in the RAT priority list received from LTE
    is used to determine which ARFCNs to search, but when decoding BCCHs
    the cell suitability check uses the SD band API to determine if the
    cell's band is actually supported.  We work around the SD API returning
    no GSM bands here by just enabling all GSM bands temporarily.

    This change will not result in RR searching more bands than are necessary,
    as this is limited by the bands set in RAT priority list.
    */
    if (rr_plc_l2g_search_in_progress(gas_id))
    {
      bands_allowed |= SYS_BAND_MASK_GSM_EGSM_900 |
                       SYS_BAND_MASK_GSM_DCS_1800 |
                       SYS_BAND_MASK_GSM_PCS_1900 |
                       SYS_BAND_MASK_GSM_850;
    }
    else
    {
      bands_allowed |= SYS_BAND_MASK_GSM_PGSM_900;
    }
#else
    bands_allowed |= SYS_BAND_MASK_GSM_PGSM_900;
#endif /* FEATURE_LTE */
  }

  if ( bands_allowed != rr_general_data_ptr->bands_allowed )
  {
    rr_general_data_ptr->bands_allowed = bands_allowed;

    /* Reset all bands supported to unsupported */
    multiband_info->band[BAND_EGSM].supported = FALSE;
    multiband_info->band[BAND_PGSM].supported = FALSE;
    multiband_info->band[BAND_PCS].supported = FALSE;
    multiband_info->band[BAND_DCS].supported = FALSE;
    multiband_info->band[BAND_GSM_850].supported = FALSE;

    /* RGSM900 - if RGSM900 set, also include EGSM900 and PGSM900 */
    /* Note: There is no specific support for RGSM900 in RR */
    if (bands_allowed & SYS_BAND_MASK_GSM_RGSM_900)
    {
      multiband_info->band[BAND_EGSM].supported = TRUE;

      multiband_info->band[BAND_PGSM].supported = TRUE;
      MSG_GERAN_HIGH_0_G("No specific support for RGSM900 - UE supports EGSM900 and PGSM900");
    }
    else
    /* EGSM900 - if EGSM900 set, also include PGSM900 */
    if (bands_allowed & SYS_BAND_MASK_GSM_EGSM_900)
    {
      multiband_info->band[BAND_EGSM].supported = TRUE; /* UE capability - doesn't change */

      multiband_info->band[BAND_PGSM].supported = TRUE;
      MSG_GERAN_HIGH_0_G("UE supports EGSM900 and PGSM900");
    }
    else
    /* PGSM900 */
    if(bands_allowed & SYS_BAND_MASK_GSM_PGSM_900)
    {
      multiband_info->band[BAND_PGSM].supported = TRUE;
      MSG_GERAN_HIGH_0_G("UE supports PGSM900");
    }

    /* DCS1800 */
    if (bands_allowed & SYS_BAND_MASK_GSM_DCS_1800)
    {
      multiband_info->band[BAND_DCS].supported = TRUE;
      MSG_GERAN_HIGH_0_G("UE supports DCS1800");
    }

    /* PCS1900 */
    if (bands_allowed & SYS_BAND_MASK_GSM_PCS_1900)
    {
      multiband_info->band[BAND_PCS].supported = TRUE;
      MSG_GERAN_HIGH_0_G("UE supports PCS1900");
    }

    /* GSM850 */
    if(bands_allowed & SYS_BAND_MASK_GSM_850)
    {
      multiband_info->band[BAND_GSM_850].supported = TRUE;
      MSG_GERAN_HIGH_0_G("UE supports GSM850");
    }
  }
  /*lint -restore */

} /* end rr_update_band_support_info() */


/* TODO: Remove calls to this from gsclassmark.c */
void mb_initialise(
  void
)
{
  ;
} /* end mb_initialise() */


/**
 *
 * Initialise band support information
 *
 */
void rr_init_band_support_info(
  const gas_id_t gas_id
)
{
  const multiband_info_T default_multiband_info =
  {
    {
      {
        /*************/
        /* E-GSM-900 */
        /*************/
        FALSE,             /* filled in later */
        FALSE,             /* filled in later */
        975,              /* min ARFCN */
        124,              /* max ARFCN */
        { 0, 39, 37, 33, 29}, /* max power by class - see 05.05 4.4.1 */
        { 39, 39, 39, 37, 35, 33, 31, 29, 27, 25,  /* output power */
          23, 21, 19, 17, 15, 13, 11, 9, 7, 5, 5,  /* see 05.05 4.1.1 */
          5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
        (byte) RR_POWER_CLASS_E900,
      },
      {
        /*************/
        /* P-GSM-900 */
        /*************/
        FALSE,             /* filled in later */
        FALSE,             /* filled in later */
        1,             /* min ARFCN */
        124,           /* max ARFCN */
        { 0, 39, 37, 33, 29}, /* max power by class - see 05.05 4.4.1 */
        { 39, 39, 39, 37, 35, 33, 31, 29, 27, 25,  /* output power */
          23, 21, 19, 17, 15, 13, 11, 9, 7, 5, 5,  /* see 05.05 4.1.1 */
          5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
        (byte) RR_POWER_CLASS_P900,
      },
      {
        /************/
        /* PCS-1900 */
        /************/
        /* For configuration values, see 3GPP TS 05.05 Section 4.1.1 */
        FALSE,             /* filled in later */
        FALSE,             /* filled in later */
        512,              /* min ARFCN */
        810,              /* max ARFCN */
        { 30, 24, 33, 0, 0}, /* max power by class */
        { 30, 28, 26, 24, 22, 20, 18, 16, 14, 12,  /* output power */
          10, 8, 6, 4, 2, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* These values reserved*/
          33, 32},
        (byte) RR_POWER_CLASS_1900,
      },
      {
        /************/
        /* DCS-1800 */
        /************/
        FALSE,             /* filled in later */
        FALSE,             /* filled in later */
        512,              /* min ARFCN */
        885,              /* max ARFCN */
        { 30, 24, 36, 0, 0}, /* max power by class - see 05.05 4.4.1 */
        { 30, 28, 26, 24, 22, 20, 18, 16, 14, 12,  /* output power */
          10, 8, 6, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0,  /* see 05.05 4.1.1 */
          0, 0, 0, 0, 0, 0, 36, 34, 32},
        (byte) RR_POWER_CLASS_1800,
      },
      {
        /************/
        /* GSM-850  */
        /************/
        FALSE,             /* filled in later */
        FALSE,             /* filled in later */
        128,              /* min ARFCN */
        251,              /* max ARFCN */
        { 0, 39, 37, 33, 29}, /* max power by class - see 05.05 4.4.1 */
        { 39, 39, 39, 37, 35, 33, 31, 29, 27, 25,  /* output power */
          23, 21, 19, 17, 15, 13, 11, 9, 7, 5, 5,  /* see 05.05 4.1.1 */
          5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
        (byte) RR_POWER_CLASS_850,
      }
    },
    BAND_NONE, /* current_band */
    BAND_NONE  /* current_dedicated_band */
  };

  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  /* Copy over the above default info into the multiband info database */
  (* multiband_info) = default_multiband_info;

  rr_general_data_ptr->bands_allowed = SYS_BAND_MASK_EMPTY;

  rr_update_band_support_info(gas_id);

} /* end rr_init_band_support_info() */

/**
 * Function: rr_get_band_using_band_indicator
 *
 * @param local_arfcn  channel number (0..1023)
 * @param band_ind     band indicator (0..1) to use to resolve PCS/DCS conflicts
 *
 * @return band associated with the given channel or
 *         BAND_NONE if the ARFCN is unknown or not supported
 */
rr_internal_band_T rr_get_band_using_band_indicator(
  word local_arfcn,
  si_psi_band_ind_T band_ind,
  const gas_id_t gas_id
)
{
  rr_internal_band_T actual_band;

  actual_band = rr_determine_actual_band(local_arfcn, band_ind);

  if (actual_band == BAND_NONE)
  {
    return BAND_NONE;
  }

  /****************************************************************
   * EGSM is a superset of PGSM so can be used if it is supported *
   ****************************************************************/
  if (actual_band == BAND_PGSM)
  {
    if (rr_gapi_is_supported_band(SYS_BAND_EGSM_900, gas_id))
    {
      return(BAND_EGSM);
    }
  }

  /****************************************************************
   * If the actual band is not supported, return error value here *
   ****************************************************************/
  if (rr_gapi_is_supported_band((sys_band_T)actual_band, gas_id) == FALSE)
  {
    MSG_GERAN_LOW_2_G("Unsupported band %d for ARFCN %d", (int)actual_band, (int)local_arfcn);
    actual_band = BAND_NONE;
  }

  return(actual_band);
}

/**
 * Function: rr_get_current_band_indicator
 *
 * @param is_camped TRUE/FALSE - determines which sys info DB is used
 *
 * @return band indicator that can be use to resolve DCS/PCS conflicts
 */
si_psi_band_ind_T rr_get_current_band_indicator(
  boolean is_camped,
  const gas_id_t gas_id
)
{
  si_psi_band_ind_T   band_ind;
  gprs_scell_info_t * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_scell_info_t * pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

  if (rr_is_control_state(RR_W2G_INTERRAT_HANDOVER_PROGRESS, gas_id))
  {
    MSG_GERAN_HIGH_1_G("rr_get_current_band_indicator: %d", pcell_info_ptr->gsm.band_indicator);
    band_ind = pcell_info_ptr->gsm.band_indicator;
  }
  else
  {
    rr_internal_band_T  curr_band;

    /* determine whether DCS or PCS are currently in use */
    if (rr_in_dedicated(gas_id))
    {
      curr_band = rr_get_current_dedicated_band(gas_id);
      if ((curr_band != BAND_DCS) && (curr_band != BAND_PCS))
      {
        /* dedicated band does not help with resolution, so use serving band */
        curr_band = rr_gapi_get_current_band(gas_id);
      }
    }
    else if (grr_get_l1_in_test_mode(gas_id))
    {
      /* during GPRS Test Mode, GRR's current band may be out of date */
      curr_band = grr_get_test_mode_band(gas_id);
    }
    else
    {
      curr_band = rr_gapi_get_current_band(gas_id);
    }

    /* use the current band to derive the band indicator if possible */
    if (curr_band == BAND_PCS)
    {
      band_ind = SI_PSI_BAND_IND_PCS1900;
    }
    else if (curr_band == BAND_DCS)
    {
      band_ind = SI_PSI_BAND_IND_DCS1800;
    }
    else
    {
      /* current band is not relevant, so use band indicator from sys info DB */
      if (is_camped == TRUE)
      {
        band_ind = scell_info_ptr->gsm.band_indicator;
      }
      else
      {
        band_ind = pcell_info_ptr->gsm.band_indicator;
      }
    }
  }

  return(band_ind);
}

/**
 * Function: rr_get_band
 *
 * @param local_arfcn  channel number (0..1023)
 *
 * @return band associated with the given channel or
 *         BAND_NONE if the ARFCN is unknown or not supported
 */
rr_internal_band_T rr_get_band(
  word local_arfcn,
  const gas_id_t gas_id
)
{
  rr_internal_band_T result;
  si_psi_band_ind_T  band_ind;

  /* do an initial check for ARFCNs in the DCS/PCS overlap range */
  if ((local_arfcn >= RR_ARFCN_PCS_1900_MIN) &&
      (local_arfcn <= RR_ARFCN_PCS_1900_MAX))
  {
    boolean is_camped = rr_is_camped(gas_id);

    /* determine the current band indicator */
    band_ind = rr_get_current_band_indicator(is_camped, gas_id);
  }
  else
  {
    /* band indicator is not relevant, but must be set to something */
    band_ind = SI_PSI_BAND_IND_DCS1800;
  }

  /* pass the ARFCN and the band indicator to the worker function */
  result = rr_get_band_using_band_indicator(local_arfcn, band_ind,gas_id);

  /* during GPRS Test Mode, the CSN decoder will call this function */
  /* but GRR will not have been able to set the current band itself */
  if ((result < MAXNO_BANDS) && grr_get_l1_in_test_mode(gas_id))
  {
    /* this may look like it will get called many times, but it is likely */
    /* that only the decoding of packet assignments will occur during the */
    /* GPRS Test Mode, since GRR is idle and SI acquisition is disabled.  */
    grr_set_test_mode_band(result, gas_id);
  }

  return(result);
}

/**
 * determines if a channel number is within the
 * specified band
 *
 * @param band   band to check
 *
 * @param arfcn  channel number
 *
 * @return TRUE if the given channel number is in the given band,
 *         FALSE otherwise
 */
boolean rr_check_in_band(
  rr_internal_band_T band,
  word local_arfcn,
  const gas_id_t gas_id
)
{
  word  range;
  boolean in_band = FALSE;
  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);

  if (rr_gapi_is_supported_band((sys_band_T)band, gas_id) && (band < MAXNO_BANDS))
  {
    /*
     * band is being used - get the number of frequencies being used
     * in this band - 1
     * The modulo 1024 is used so that for E-GSM, with channels
     * from 875 to 1023, and 0 to 124, this is equivalent to a
     * single range from 875 to 124 modulo 1024
     */

    /***********************************************************************************/
    /* NOTE that it is not necessary to use INTLOCK to protect access to max/min arfcn */
    /* since the max/min information does not change dynamically.                      */
    /***********************************************************************************/
    range = (multiband_info->band[band].max_arfcn + 1024
            - multiband_info->band[band].min_arfcn) % 1024;

    if (((local_arfcn - multiband_info->band[band].min_arfcn + 1024) %1024) <= range )
    {
      /*
       * arfcn is in this band
       */

      in_band = TRUE;
    }
    /* If BAND_EGSM is enabled, SD never enables BAND_PGSM. Internally in RR
     *   if BAND_EGSM is enabled, we enable BAND_PGSM also since it is a sub-set
     *   of BAND_EGSM. If the phone supports BAND_EGSM, the following is to treat
     *   BAND_PGSM to be in-band with BAND_EGSM, which is the desired behavior.
     */
    if ((rr_get_band(local_arfcn, gas_id) == BAND_EGSM) && (band == BAND_PGSM))
    {
      if (rr_gapi_is_supported_band(SYS_BAND_EGSM_900, gas_id))
      {
        in_band = TRUE;
      }
    }
  }

  return in_band;
} /* rr_check_in_band */

/**
 * Function to Check that all the entries in the frequency list are in the same supported band
 *
 * @param frequency_list frequency list to check
 *
 * @return TRUE if the frequency list contains entries from different bands, FALSE otherwise
 */
boolean rr_check_frequency_list(
  frequency_list_T *frequency_list,
  const gas_id_t gas_id
)
{
  rr_internal_band_T    band = BAND_NONE;
  rr_internal_band_T     this_band;
  boolean  mixed_or_unsupported_bands = FALSE;
  int      i;


  for (i = 0; i < frequency_list->no_of_items; i++)
  {
    if (band == BAND_NONE)
    {
      /* determine band of the current channel */
      this_band = GET_ARFCN_BAND(frequency_list->channel[i]);

      /* store band for comparison */
      band = this_band;

      /* check if band is supported */
      if (this_band == BAND_NONE)
      {
        /* Band not supported */
        mixed_or_unsupported_bands = TRUE;
        break;
      }
    }
    else
    {
      /*
       * check that all other valid bands are in the same band
       */

      if (!rr_check_in_band(band,frequency_list->channel[i].num, gas_id))
      {
        mixed_or_unsupported_bands = TRUE;
        break;
      }
    }
  }

  return mixed_or_unsupported_bands;
} /* rr_check_frequency_list */


/**
 * determines if the given band is currently preferred or not. May be called from
 * multiple tasks
 *
 * @param band   band
 *
 * @return TRUE if the band is preferred, FALSE otherwise
 */
boolean rr_is_preferred_band(
  rr_internal_band_T band,
  const gas_id_t gas_id
)
{
  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);
  boolean preferred = FALSE;

  if (band == BAND_NONE)
  {
    return FALSE;
  }

  if (band >= MAXNO_BANDS)
  {
    MSG_GERAN_ERROR_1_G("Invalid band (%d)",(int)band);
    return FALSE;
  }

  /* access to multiband_info should be INTLOCK protected */
  GRR_GENERAL_LOCK();
  preferred = multiband_info->band[band].preferred;
  GRR_GENERAL_UNLOCK();

  return preferred;
} /* rr_is_preferred_band */

/**
 * Sets the flag indicating whether the given band is currently preferred or not
 * to the given value. May be accessed from multiple tasks.
 *
 * @param band    band
 *
 * @param preferred value (TRUE if allowed, FALSE otherwise)
 */
void rr_set_preferred_band(
  rr_internal_band_T band,
  boolean preferred,
  const gas_id_t gas_id
)
{
  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);

  if (band >= MAXNO_BANDS)
  {
    MSG_GERAN_ERROR_1_G("Invalid band (%d)",(int)band);
    return;
  }

  /* access to multiband_info should be INTLOCK protected */
  GRR_GENERAL_LOCK();
  multiband_info->band[band].preferred = preferred;
  GRR_GENERAL_UNLOCK();
} /* rr_set_preferred_band */

/**
 * determines if the given band is supported or not. May be called from
 * multiple tasks
 *
 * @param band   band
 *
 * @return TRUE if the band is allowed, FALSE otherwise
 */
boolean rr_gapi_is_supported_band(
  sys_band_T band,
  const gas_id_t gas_id
)
{
  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);
  boolean            supported = FALSE;
  rr_internal_band_T rr_band = (rr_internal_band_T)band;

  if (rr_band == BAND_NONE)
  {
    return FALSE;
  }

  if (rr_band >= MAXNO_BANDS)
  {
    MSG_GERAN_ERROR_1_G("Invalid band (%d)", (int)rr_band);
    return FALSE;
  }

  /* Query SD for latest band support info */
  rr_update_band_support_info(gas_id);

  /* access to multiband_info should be INTLOCK protected */
  GRR_GENERAL_LOCK();
  supported = multiband_info->band[rr_band].supported;
  GRR_GENERAL_UNLOCK();

  return supported;
} /* rr_is_supported_band */

boolean rr_is_supported_band(
  rr_internal_band_T band
)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(SYS_MODEM_AS_ID_1);

  return rr_gapi_is_supported_band((sys_band_T)band, gas_id);
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/**
 * determines if the given band is supported or not. May be called from
 * multiple tasks
 *
 * @param band   band
 *
 * @return TRUE if the band is allowed, FALSE otherwise
 */
boolean rr_ds_is_supported_band(
  rr_internal_band_T band,
  sys_modem_as_id_e_type as_id
)
{
  return rr_gapi_is_supported_band((sys_band_T)band, geran_map_nas_id_to_gas_id(as_id));
} /* rr_ds_is_supported_band */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/**
 * May be accessed from multiple tasks
 *
 * @return the current band associated with the last serving cell
 */
rr_internal_band_T rr_gapi_get_current_band(
  const gas_id_t gas_id
)
{
  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);
  rr_internal_band_T current_band;

  GRR_GENERAL_LOCK();
  current_band = multiband_info->current_band;
  GRR_GENERAL_UNLOCK();

  if (!RR_IS_VALID_BAND(current_band))
  {
    MSG_GERAN_ERROR_1_G("Invalid band (%d)",(int) current_band);
    current_band = BAND_PGSM;
  }

  return current_band;
} /* rr_get_current_band */

rr_internal_band_T rr_get_current_band(
  void
)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(SYS_MODEM_AS_ID_1);

  return rr_gapi_get_current_band(gas_id);
} /* rr_ds_get_current_band */

#ifdef FEATURE_DUAL_SIM
/**
 * May be accessed from multiple tasks
 *
 * @return the current band associated with the last serving cell
 */
rr_internal_band_T rr_ds_get_current_band(
  sys_modem_as_id_e_type as_id
)
{
  return rr_gapi_get_current_band(geran_map_nas_id_to_gas_id(as_id));
} /* rr_ds_get_current_band */
#endif /* FEATURE_DUAL_SIM */

/**
 * Set the current band to the given value. May be accessed by multiple tasks
 *
 * @param band   new current band value
 */
void rr_set_current_band(
  rr_internal_band_T band,
  const gas_id_t gas_id
)
{
  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);

  if (!RR_IS_VALID_BAND(band))
  {
    MSG_GERAN_ERROR_1_G("Invalid band (%d)",(int) band);
    band = BAND_PGSM;
  }

  GRR_GENERAL_LOCK();
  multiband_info->current_band = band;
  GRR_GENERAL_UNLOCK();

  MSG_GERAN_HIGH_1_G("current band = %d",band);

} /* rr_set_current_band */

/**
 * Resets the current band value to the first
 * supported band that is found. If no bands are
 * supported, it is set to BAND_NONE
 */
void rr_reset_current_band(
  const gas_id_t gas_id
)
{
  if (rr_gapi_is_supported_band(SYS_BAND_PGSM_900, gas_id))
  {
    rr_set_current_band(BAND_PGSM, gas_id);
  }
  else if (rr_gapi_is_supported_band(SYS_BAND_EGSM_900, gas_id))
  {
    rr_set_current_band(BAND_EGSM, gas_id);
  }
  else if (rr_gapi_is_supported_band(SYS_BAND_DCS_1800, gas_id))
  {
    rr_set_current_band(BAND_DCS, gas_id);
  }
  else if (rr_gapi_is_supported_band(SYS_BAND_PCS_1900, gas_id))
  {
    rr_set_current_band(BAND_PCS, gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("No GSM bands are supported");
    rr_set_current_band(BAND_NONE, gas_id);
  }
} /* rr_reset_current_band */


/*
 Get current_dedicated_band stored from multiband database
 */

rr_internal_band_T rr_get_current_dedicated_band(
  const gas_id_t gas_id
)
{
  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);
  rr_internal_band_T current_ded_band;

  GRR_GENERAL_LOCK();
  current_ded_band = multiband_info->current_dedicated_band;
  GRR_GENERAL_UNLOCK();

  if (!RR_IS_VALID_BAND(current_ded_band))
  {
    MSG_GERAN_ERROR_1_G("Invalid ded band (%d)",(int) current_ded_band);
  }

  return current_ded_band;
} /* rr_get_current_dedicated_band */

/*
 Set current_dedicated_band parameter in multiband_info database
 */

void rr_set_current_dedicated_band(
  rr_internal_band_T band,
  const gas_id_t gas_id
)
{
  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);

  if (!RR_IS_VALID_BAND(band))
  {
    MSG_GERAN_ERROR_1_G("Invalid ded band (%d)",(int) band);
  }

  GRR_GENERAL_LOCK();
  multiband_info->current_dedicated_band = band;
  GRR_GENERAL_UNLOCK();

  MSG_GERAN_HIGH_1_G("current dedicated band = %d",band);
}/* rr_set_current_dedicated_band */


/**
 * May be called by multiple tasks.
 *
 * @return RF power class of the current band
 */
byte rr_gapi_get_current_rf_power_class(
  const gas_id_t gas_id
)
{
  byte power_class;
  rr_internal_band_T current_band = rr_gapi_get_current_band(gas_id);
  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);

  GRR_GENERAL_LOCK();
  power_class = multiband_info->band[current_band].rf_power_class;
  GRR_GENERAL_UNLOCK();

  return power_class;
} /* rr_get_current_rf_power_class */

byte rr_get_current_rf_power_class(
  void
)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(SYS_MODEM_AS_ID_1);

  return rr_gapi_get_current_rf_power_class(gas_id);
}

byte rr_gapi_get_band_rf_power_class(
  rr_internal_band_T band,
  const gas_id_t gas_id
)
{
  byte power_class;

  // Obtain a pointer to the multi-band information
  multiband_info_T * multiband_info = rr_get_multiband_info(gas_id);

  if (!RR_IS_VALID_BAND(band))
  {
    MSG_GERAN_ERROR_1_G("Invalid band (%d)",(int)band);
    band = BAND_PGSM;
  }

  GRR_GENERAL_LOCK();
  power_class = multiband_info->band[band].rf_power_class;
  GRR_GENERAL_UNLOCK();

  return power_class;
}

/**
 * May be called by multiple tasks
 *
 * @param band   band for which the current RF power class is desired
 *
 * @return current power class associated with the given band
 */
byte rr_get_band_rf_power_class(
  rr_internal_band_T band
)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(SYS_MODEM_AS_ID_1);

  return rr_gapi_get_band_rf_power_class(band, gas_id);
} /* rr_get_band_rf_power_class */

#ifdef FEATURE_DUAL_SIM
/**
 * May be called by multiple tasks.
 *
 * @return RF power class of the current band
 */
byte rr_ds_get_current_rf_power_class(
  sys_modem_as_id_e_type as_id
)
{
  return rr_gapi_get_current_rf_power_class(geran_map_nas_id_to_gas_id(as_id));
} /* rr_get_current_rf_power_class */
#endif /* FEATURE_DUAL_SIM */

#ifdef FEATURE_DUAL_SIM
/**
 * May be called by multiple tasks
 *
 * @param band   band for which the current RF power class is desired
 *
 * @return current power class associated with the given band
 */
byte rr_ds_get_band_rf_power_class(
  rr_internal_band_T band,
  sys_modem_as_id_e_type as_id
)
{
  return rr_gapi_get_band_rf_power_class(band, geran_map_nas_id_to_gas_id(as_id));
} /* rr_get_band_rf_power_class */
#endif /* FEATURE_DUAL_SIM */

byte rr_gapi_get_supported_band_count(
  const gas_id_t gas_id
)
{
  uint8 count = 0;
  multiband_info_T *multiband_info_ptr = rr_get_multiband_info(gas_id);

  if (multiband_info_ptr != NULL)
  {
    uint8 index = 0;

    GRR_GENERAL_LOCK();
    while (index < MAXNO_BANDS)
    {
      if (multiband_info_ptr->band[index].supported == TRUE)
      {
        count++;
      }
      index++;
    }
    GRR_GENERAL_UNLOCK();
  }

  return (byte)count;
}

/**
 * May be called from multiple tasks.
 *
 * @return number of bands currently supported
 */
byte rr_get_supported_band_count(
  void
)
{
  return rr_gapi_get_supported_band_count(GERAN_ACCESS_STRATUM_ID_1);
} /* rr_get_supported_band_count */

/**
 * May be called from multiple tasks.  Referenced by as_id.
 *
 * @return number of bands currently supported
 */
byte rr_ds_get_supported_band_count(
  sys_modem_as_id_e_type as_id
)
{
  return rr_gapi_get_supported_band_count(geran_map_nas_id_to_gas_id(as_id));
} /* rr_get_supported_band_count */

sys_band_mask_e_type rr_convert_band_info(
  sys_band_T band
)
{
  sys_band_mask_e_type ret_val;

  switch(band)
  {
    case SYS_BAND_PGSM_900:
      ret_val = (sys_band_mask_e_type) SYS_BAND_MASK_GSM_PGSM_900;
      break;

    case SYS_BAND_EGSM_900:
      ret_val = (sys_band_mask_e_type) SYS_BAND_MASK_GSM_EGSM_900;
      break;

    case SYS_BAND_DCS_1800:
      ret_val = (sys_band_mask_e_type) SYS_BAND_MASK_GSM_DCS_1800;
      break;

    case SYS_BAND_PCS_1900:
      ret_val = (sys_band_mask_e_type) SYS_BAND_MASK_GSM_PCS_1900;
      break;

    case SYS_BAND_CELL_850:
      ret_val = (sys_band_mask_e_type) SYS_BAND_MASK_GSM_850;
      break;

    default:
      MSG_GERAN_ERROR_1("RR does not support band %d",band);
      ret_val = SYS_BAND_MASK_EMPTY;
      break;
  }

  return(ret_val);
} /* rr_convert_band_info */

/**
 * Sets the preferred bands, which is derived from the band_pref received
 * in a PLMN SELECT REQ, and affects the bands searched during a power scan
 */
void rr_set_preferred_bands(
  sys_band_mask_type band_pref,
  const gas_id_t gas_id
)
{
  if (band_pref & (sys_band_mask_type) SYS_BAND_MASK_GSM_PCS_1900)
  {
    rr_set_preferred_band(BAND_PCS, TRUE, gas_id);
    MSG_GERAN_HIGH_0_G("band_pref bit-mask for PCS1900 is allowed");
  }
  else
  {
    rr_set_preferred_band(BAND_PCS, FALSE, gas_id);
    MSG_GERAN_HIGH_0_G("band_pref bit-mask for PCS1900 is not allowed");
  }

  if (band_pref & (sys_band_mask_type) SYS_BAND_MASK_GSM_DCS_1800)
  {
    rr_set_preferred_band(BAND_DCS, TRUE, gas_id);
    MSG_GERAN_HIGH_0_G("band_pref bit-mask for DCS1800 is allowed");
  }
  else
  {
    rr_set_preferred_band(BAND_DCS, FALSE, gas_id);
    MSG_GERAN_HIGH_0_G("band_pref bit-mask for DCS1800 is not allowed");
  }

  if (band_pref & (sys_band_mask_type) SYS_BAND_MASK_GSM_850)
  {
    rr_set_preferred_band(BAND_GSM_850, TRUE, gas_id);
    MSG_GERAN_HIGH_0_G("band_pref bit-mask for GSM850 is allowed");
  }
  else
  {
    rr_set_preferred_band(BAND_GSM_850, FALSE, gas_id);
    MSG_GERAN_HIGH_0_G("band_pref bit-mask for GSM850 is not allowed");
  }

  if (band_pref & (sys_band_mask_type) SYS_BAND_MASK_GSM_EGSM_900)
  {
    rr_set_preferred_band(BAND_EGSM, TRUE, gas_id);
    rr_set_preferred_band(BAND_PGSM, TRUE, gas_id);
    MSG_GERAN_HIGH_0_G("band_pref bit-masks for EGSM and PGSM900 are allowed");
  }
  else
  {
    rr_set_preferred_band(BAND_EGSM, FALSE, gas_id);
    MSG_GERAN_HIGH_0_G("band_pref bit-mask for EGSM900 is not allowed");
    if (band_pref & (sys_band_mask_type) SYS_BAND_MASK_GSM_PGSM_900)
    {
      rr_set_preferred_band(BAND_PGSM, TRUE, gas_id);
      MSG_GERAN_HIGH_0_G("band_pref bit-mask for PGSM900 is allowed");
    }
    else
    {
      rr_set_preferred_band(BAND_PGSM, FALSE, gas_id);
      MSG_GERAN_HIGH_0_G("band_pref bit-mask for PGSM900 is not allowed");
    }
  }


}  /* rr_set_preferred_bands() */

/*
 *   Function name:  rr_check_BA_list_change
 *   ------------------------------
 *
 *   Description: Checks the BA list to see that if it has changed
 *
 *
 *   Parameters:  BCCH_frequency_list new_entries - new list that was broadcasted
 *   -----------  BCCH_frequency_list old_entries - old list
 *
 *
 *   Returns:  boolean TRUE - if ba list has changed
 *   --------
 *
 */
boolean rr_check_BA_list_change(
  BCCH_frequency_list_T *new_entries,
  BCCH_frequency_list_T *old_entries,
  const gas_id_t gas_id
)
{
  BCCH_frequency_list_T temp_entries=*new_entries;
  int index=0;

  qsort_arfcn(
    temp_entries.BCCH_ARFCN,
    temp_entries.no_of_entries,
    gas_id
  );

  if (temp_entries.no_of_entries == old_entries->no_of_entries)
  {
    for (index = 0; index < temp_entries.no_of_entries; index++)
    {
      if (temp_entries.BCCH_ARFCN[index].num != old_entries->BCCH_ARFCN[index].num)
      {
        return TRUE;
      }
    }
  }
  else
  {
    /* different number of entries */
    return TRUE;
  }

  return FALSE;
}

/**
 * Add unique frequencies to the BA list is specified
 * information_database, and reorders the list
 *
 * @param target_ba_list_ptr BA list where the frequencies should be added
 *
 * @param source_ba_list_ptr source from which the BA list should be constructed
 *
 * @param ba_list_name       name of the ba list being added
 */
void rr_add_ba_list(
  BCCH_frequency_list_T *target_ba_list_ptr,
  BCCH_frequency_list_T *source_ba_list_ptr,
  const gas_id_t gas_id
)
{
  int     no_of_entries;
  int     cell_index_1;
  int     cell_index_2;
  boolean  present_flag;
  int     fl_length;

  no_of_entries = source_ba_list_ptr->no_of_entries;
  fl_length = target_ba_list_ptr->no_of_entries;

  for (cell_index_1 = 0; cell_index_1 < no_of_entries; cell_index_1++)
  {
    present_flag = FALSE;

    for (cell_index_2 = 0; cell_index_2 < fl_length; cell_index_2++)
    {
      if (source_ba_list_ptr->BCCH_ARFCN[cell_index_1].num ==
          target_ba_list_ptr->BCCH_ARFCN[cell_index_2].num)
      {
        present_flag = TRUE;
        break;
      }
    }

    if (!present_flag)
    {
      if (target_ba_list_ptr->no_of_entries < BA_MAX_LEN)
      {
        target_ba_list_ptr->BCCH_ARFCN[fl_length] =
        source_ba_list_ptr->BCCH_ARFCN[cell_index_1];
        target_ba_list_ptr->no_of_entries++;
        fl_length++;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("Too many neighbour cell entries, rest ignored");
        break;
      }
    }
    /* otherwise, already present */
  }

  /* set the BA_IND of the target equal to the source */
  target_ba_list_ptr->BA_IND = source_ba_list_ptr->BA_IND;

  qsort_arfcn(target_ba_list_ptr->BCCH_ARFCN, target_ba_list_ptr->no_of_entries, gas_id);
} /* rr_add_ba_list */


/**
 * Add unique frequencies to the BA list is specified
 * information_database, and reorders the list
 *
 * @param target_ba_list_ptr BA list where the frequencies should be added
 *
 * @param source_ba_list_ptr source from which the BA list should be constructed
 *
 * @param ba_list_name       name of the ba list being added
 */
static void rr_add_ba_list_without_sorting(
  BCCH_frequency_list_T *target_ba_list_ptr,
  BCCH_frequency_list_T *source_ba_list_ptr
)
{
  int     no_of_entries;
  int     cell_index_1;
  int     cell_index_2;
  boolean  present_flag;
  int     fl_length;

  no_of_entries = source_ba_list_ptr->no_of_entries;
  fl_length = target_ba_list_ptr->no_of_entries;

  for (cell_index_1 = 0; cell_index_1 < no_of_entries; cell_index_1++)
  {
    present_flag = FALSE;

    for (cell_index_2 = 0; cell_index_2 < fl_length; cell_index_2++)
    {
      if (source_ba_list_ptr->BCCH_ARFCN[cell_index_1].num ==
          target_ba_list_ptr->BCCH_ARFCN[cell_index_2].num)
      {
        MSG_GERAN_HIGH_1("ARFCN %d already in the idle BA list",source_ba_list_ptr->BCCH_ARFCN[cell_index_1].num);
        present_flag = TRUE;
        break;
      }
    }

    if (!present_flag)
    {
      if (target_ba_list_ptr->no_of_entries < BA_MAX_LEN)
      {
        target_ba_list_ptr->BCCH_ARFCN[fl_length] =
        source_ba_list_ptr->BCCH_ARFCN[cell_index_1];
        target_ba_list_ptr->no_of_entries++;
        fl_length++;
      }
      else
      {
        MSG_GERAN_ERROR_0("Too many neighbour cell entries, rest ignored");
        break;
      }
    }
    /* otherwise, already present */
  }

  /* set the BA_IND of the target equal to the source */
  target_ba_list_ptr->BA_IND = source_ba_list_ptr->BA_IND;
} /* rr_add_ba_list_without_sorting */


/**
 * Mark the corresponding database as invalid, and reset
 * the number of BAS entries to zero
 *
 * @param information_database pointer to SI DB to clear
 */
void rr_clear_system_information(
  system_information_database_T *sys_info_db_ptr
)
{

  if (sys_info_db_ptr->dedicated_mode_params != NULL)
  {
    MSG_GERAN_MED_1("Freeing %d bytes for dedicated mode parameters", sizeof(rr_dedicated_mode_params_t));
    GPRS_MEM_FREE(sys_info_db_ptr->dedicated_mode_params);
  }

  memset((void *)sys_info_db_ptr, 0, sizeof(system_information_database_T));

  rr_clear_reselection_info(&sys_info_db_ptr->reselection_info);

  /* set the BCCH_ARFCN to an invalid value */
  sys_info_db_ptr->BCCH_ARFCN.num = INVALID_ARFCN;
  sys_info_db_ptr->BCCH_ARFCN.band = SYS_BAND_NONE;
  sys_info_db_ptr->gprs_indicator = FALSE;

  sys_info_db_ptr->band_indicator = SI_PSI_BAND_IND_DCS1800;

  /* Set PLMN id to an invalid value. */
  sys_plmn_undefine_plmn_id(&sys_info_db_ptr->location_area_identification.plmn_id);

#ifdef FEATURE_WCDMA
  sys_info_db_ptr->utran_neighbor_list.FDD_GPRS_Qoffset = DEFAULT_FDD_GPRS_QOFFSET;
  sys_info_db_ptr->utran_neighbor_list.TDD_GPRS_Qoffset = DEFAULT_TDD_GPRS_QOFFSET;

  sys_info_db_ptr->utran_neighbor_list.si2q_params.FDD_Qoffset = DEFAULT_FDD_QOFFSET;
  sys_info_db_ptr->utran_neighbor_list.si2q_params.TDD_Qoffset = DEFAULT_TDD_QOFFSET;
  sys_info_db_ptr->utran_neighbor_list.si2q_params.QSearch_I = DEFAULT_QSEARCH_I;
  sys_info_db_ptr->utran_neighbor_list.si2q_params.FDD_Qmin = DEFAULT_FDD_QMIN;
  sys_info_db_ptr->utran_neighbor_list.si2q_params.FDD_Qmin_offset = DEFAULT_FDD_QMIN_OFFSET;
  sys_info_db_ptr->utran_neighbor_list.si2q_params.FDD_RSCPmin = DEFAULT_FDD_RSCPMIN;

  sys_info_db_ptr->wcdma_neighbor_list_si2ter.FDD_Qoffset = DEFAULT_FDD_QOFFSET;
  sys_info_db_ptr->wcdma_neighbor_list_si2ter.TDD_Qoffset = DEFAULT_TDD_QOFFSET;
  sys_info_db_ptr->wcdma_neighbor_list_si2ter.Qsearch_I = DEFAULT_QSEARCH_I;
  sys_info_db_ptr->wcdma_neighbor_list_si2ter.FDD_Qmin = DEFAULT_FDD_QMIN;
  sys_info_db_ptr->wcdma_neighbor_list_si2ter.FDD_Qmin_offset = DEFAULT_FDD_QMIN_OFFSET;
  sys_info_db_ptr->wcdma_neighbor_list_si2ter.FDD_RSCPmin = DEFAULT_FDD_RSCPMIN;
#endif /* FEATURE_WCDMA */

} /* rr_clear_system_information */

/**
 * Prepares the given SI DB for a camping attempt (to collect
 * all the system information required for camping)
 *
 * @param si_db_ptr      pointer to system information database
 *
 * @param channel_number channel number of cell where camping is being attempted
 *
 * @param BSIC           BSIC of cell where camping is being attempted
 *
 * @param RXLEV_average  last RXLEV value computed for the cell
 */
void rr_prepare_sidb_for_camping(
  system_information_database_T *si_db_ptr,
  ARFCN_T                       loc_arfcn,
  BSIC_T                        BSIC,
  byte                          RXLEV_average,
  const gas_id_t                gas_id
)
{
  rr_clear_system_information(si_db_ptr);

  rr_initialize_SI2q_instances(gas_id);
  rr_initialize_SI2ter_instances(gas_id);

  si_db_ptr->BCCH_ARFCN.num = loc_arfcn.num;
  si_db_ptr->BCCH_ARFCN.band = loc_arfcn.band;

  si_db_ptr->BSIC       = BSIC;
  si_db_ptr->reselection_info.RXLEV_average       = RXLEV_average;

  /* indicate that the SI received event should be generated for this cell */
  si_db_ptr->generate_si_received_event = TRUE;

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
  rr_set_rr_using_pseudo_si2q_data(FALSE, gas_id);
#endif

  
} /* rr_prepare_sidb_for_camping */

/**
 * Clear/invalidate the dedicated data associated with the given SIDB
 *
 * @param information_database pointer to information database
 */
void rr_clear_si_sacch(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t   *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_ba_lists_t       *rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_ba_lists_ptr);

  MSG_GERAN_HIGH_0_G("Clearing SI SACCH");

  if (scell_info_ptr->gsm.valid_dedicated_data_flag)
  {
    rr_update_old_si5_ba_lists(
      &rr_ba_lists_ptr->SI5_BA_list,
      &rr_ba_lists_ptr->SI5bis_BA_list,
      &rr_ba_lists_ptr->SI5ter_BA_list,
      gas_id
    );
    rr_set_status_of_old_cell_si5_ba_lists(TRUE, gas_id);
  }

  scell_info_ptr->gsm.valid_dedicated_data_flag = FALSE;

  rr_reset_dedicated_BA_lists(&scell_info_ptr->gsm, gas_id);

  /* mark all the dedicated SIs as not received */
  CLEAR_BIT( MASK_SI_5 | MASK_SI_5_BIS | MASK_SI_5_TER | MASK_SI_6,
             scell_info_ptr->gsm.update_message_received);

  rr_initialize_MI_instances(gas_id);

  /* generate new S counter value next time the RTL value is received */
  scell_info_ptr->gsm.RLT_valid = FALSE;

  rr_reset_si5_count(gas_id);
} /* rr_clear_si_sacch */

/**
 * Function to calculate C1
 *
 * @param channel_no          ARFCN of the cell
 *
 * @param RXLEV_average       signal strength (RXLEV) for cell
 *
 * @param RXLEV_ACCESS_MIN    minimum signal strength (in RXLEV) to access cell
 *
 * @param power_control_level power control level for cell
 *
 * @param POWER_OFFSET        power offset for cell
 *
 * @return computed C1 value
 */
int rr_get_C1(
  ARFCN_T loc_arfcn,
  byte RXLEV_average,
  byte RXLEV_ACCESS_MIN,
  byte power_control_level,
  byte POWER_OFFSET,
  const gas_id_t gas_id
)
{
  int C1;
  int received_level_average= 0;
  int RXLEV_ACCESS_MIN_DBM  = 0;
  byte MS_TXPWR_MAX_CCH_DBM = 0;
  byte maximum_rf_power_output = 0;
  byte rf_power_capability = 0;
  rr_internal_band_T band;
  multiband_info_T * multiband_info_ptr = rr_get_multiband_info(gas_id);

  band = GET_ARFCN_BAND(loc_arfcn);

  if (band < MAXNO_BANDS)
  {
    received_level_average = rxlev_to_dbm(RXLEV_average);
    RXLEV_ACCESS_MIN_DBM = rxlev_to_dbm(RXLEV_ACCESS_MIN);

    /* get the RF power class (same as capability) of the band associated with given channel */
    rf_power_capability = rr_gapi_get_band_rf_power_class(band, gas_id);

    /* if band is not DCS or not class 3, then ignore POWER_OFFSET */
    if ((band == BAND_DCS) && (rf_power_capability == (byte) CLASS_3))
    {
      POWER_OFFSET *= 2;         /* map OTA value to offset in dBs */
    }
    else
    {
      POWER_OFFSET = 0;
    }

    if (power_control_level < RR_BAND_NUM_POWER_LEVELS)
    {
      /****************************************************************************************/
      /* NOTE that it is not necessary to use INTLOCK to protect access to output_power array */
      /* since this information does not change dynamically.                                  */
      /****************************************************************************************/
      MS_TXPWR_MAX_CCH_DBM = multiband_info_ptr->band[band].output_power[power_control_level];
    }
    else
    {
      MSG_GERAN_ERROR_1_G("Invalid power_control_level: %d",(int)power_control_level);
    }

    maximum_rf_power_output = rr_get_maximum_power_output(rf_power_capability, band, gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Invalid band: %d", (int)band);
  }

  /*************************/
  /* C1 formula from 05.08 */
  /*************************/
  C1 = (received_level_average - RXLEV_ACCESS_MIN_DBM)
       - MAX(MS_TXPWR_MAX_CCH_DBM + POWER_OFFSET - maximum_rf_power_output,0);

  return C1;
} /* rr_get_C1 */

/**
 * Function to determine if the specified cell is currently
 * forbidden, due to it being less than 5 seconds since it
 * was last selected in a cell reselection.
 *
 * @param channel_no channel number to check if forbidden
 *
 * @return TRUE if cell is in forbidden list, FALSE otherwise
 */
boolean rr_cell_forbidden(
  ARFCN_T loc_arfcn,
  const gas_id_t gas_id
)
{
  int i;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  for (i=0; i < RR_MAX_NUMBER_OF_FORBIDDEN_CELLS; ++i)
  {
    if ((rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[i].valid_entry)         &&
        (rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[i].ARFCN.num == loc_arfcn.num)
        && (rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[i].ARFCN.band == loc_arfcn.band)
       )
    {
      return TRUE;
    }
  }

  return FALSE;
} /* rr_cell_forbidden */

/**
 * Function to start all the forbidden cell timers for cells in the
 * forbidden cell list that are not already running
 */
void rr_start_forbidden_timer(
  const gas_id_t gas_id
)
{
  int i;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  for (i=0; i< RR_MAX_NUMBER_OF_FORBIDDEN_CELLS; ++i)
  {
    if (rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[i].valid_entry)
    {
      if (!rr_timer_is_running(
             rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[i].timer_id,
             gas_id
           )
          )
      {
        MSG_GERAN_HIGH_3_G("Cell(%d): started forbidden cell timer",
                (int) rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[i].ARFCN.num,
                 0,
                 0);
        (void)rr_timer_start(
          rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[i].timer_id,
          RR_RESET_FORBIDDEN_CELL_TIMEOUT,
          gas_id
        );
      }
      /* timer is already running */
    }
    /* skip invalid entries */
  }
}

/**
 * Function to remove the entry corresponding to the timer
 * which has just matured, from the forbidden_cell list
 *
 * @param timer_id forbidden timer id that just expired
 */
void rr_clear_forbidden_cell_timer(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  int index;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  for (index = 0; index < RR_MAX_NUMBER_OF_FORBIDDEN_CELLS; ++index)
  {
    if (rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[index].timer_id == timer_id)
    {
      MSG_GERAN_HIGH_3_G("Cell(%d): expired forbidden cell timer",
              (int) rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[index].ARFCN.num,
              0,
              0);

      /* indicate the entry is now invalid */
      rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[index].valid_entry = FALSE;

      /* entry found, no need to search further */
      break;
    }
  }
}

/**
 * Cancels all forbidden cell timers
 */
void rr_cancel_all_forbidden_cell_timers(
  const gas_id_t gas_id
)
{
  int i;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  for (i=0; i < RR_MAX_NUMBER_OF_FORBIDDEN_CELLS; i++)
  {
    if (rr_timer_cancel(
          rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[i].timer_id,
          gas_id
        )
       )
    {
      MSG_GERAN_HIGH_1_G("Cell=%d: Stopped forbidden cell timer",(int) rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[i].ARFCN.num);
    }
  }
} /* rr_cancel_all_forbidden_cell_timers */

/**
 * Function to add a channel to the currently forbidden cell list
 *
 * @param channel_no channel number to add to the forbidden cell list
 */
void rr_add_forbidden_cell(
  ARFCN_T loc_arfcn,
  const gas_id_t gas_id
)
{
  int i;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  /* don't add if the cell is already in the list */
  if (!rr_cell_forbidden(loc_arfcn, gas_id))
  {
    /* add it in the first available entry */
    for (i = 0; i < RR_MAX_NUMBER_OF_FORBIDDEN_CELLS; ++i)
    {
      if (!rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[i].valid_entry)
      {
        /* add it here */
        rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[i].valid_entry = TRUE;
        rr_general_data_ptr->rr_forbidden_cell_list.forbidden_cells[i].ARFCN = loc_arfcn;
        /* no need to continue */
        break;
      }
    }
  }
  /* already in the list, don't add it again */
} /* rr_add_forbidden_cell */

/**
 * Function to initialise the forbidden cell list. Must be called before any
 * cells are added or any timers are started
 */
void rr_initialise_forbidden_cell_list(const gas_id_t gas_id)
{
  rr_forbidden_cell_list_T * forbidden_cell_list;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  forbidden_cell_list = &rr_general_data_ptr->rr_forbidden_cell_list;

  memset(forbidden_cell_list, 0, sizeof(rr_forbidden_cell_list_T));

  forbidden_cell_list->forbidden_cells[0].timer_id = RR_RESET_FORBIDDEN_CELL_TIMER_1;
  forbidden_cell_list->forbidden_cells[1].timer_id = RR_RESET_FORBIDDEN_CELL_TIMER_2;
  forbidden_cell_list->forbidden_cells[2].timer_id = RR_RESET_FORBIDDEN_CELL_TIMER_3;
  forbidden_cell_list->forbidden_cells[3].timer_id = RR_RESET_FORBIDDEN_CELL_TIMER_4;
  forbidden_cell_list->forbidden_cells[4].timer_id = RR_RESET_FORBIDDEN_CELL_TIMER_5;
  forbidden_cell_list->forbidden_cells[5].timer_id = RR_RESET_FORBIDDEN_CELL_TIMER_6;
  forbidden_cell_list->forbidden_cells[6].timer_id = RR_RESET_FORBIDDEN_CELL_TIMER_7;

  /*
  This function is sometimes called to clean out the forbidden list
  so timers may be running.
  */
  rr_cancel_all_forbidden_cell_timers(gas_id);

} /* rr_initialise_forbidden_cell_list */



/**
 * Initialise barred cell list. Cell barring here is due to authentication or
 * authentication and ciphering failure.
 */
void rr_init_barred_cell_list(
  const gas_id_t gas_id
)
{
  uint8 index;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  /* initialise barred cell list */
  for (index =0; index < RR_MAX_NUM_OF_BARRED_CELLS; index++)
  {
    rr_general_data_ptr->rr_barred_cell_list[index].remaining_time = RR_INVALID_REMAINING_TIME;
    rr_general_data_ptr->rr_barred_cell_list[index].arfcn.num = INVALID_ARFCN;
    rr_general_data_ptr->rr_barred_cell_list[index].arfcn.band = INVALID_BAND;
    rr_general_data_ptr->rr_barred_cell_list[index].bsic.BS_colour_code = INVALID_BSIC_CC;
    rr_general_data_ptr->rr_barred_cell_list[index].bsic.PLMN_colour_code = INVALID_PLMN_CC;
#ifdef FEATURE_FAKE_BTS_CELL_BARRING
    rr_general_data_ptr->rr_barred_cell_list[index].cgi_valid = FALSE;
#endif /* FEATURE_FAKE_BTS_CELL_BARRING */
  }
} /* rr_init_barred_cell_list() */



/**
 * Add a cell to barred cell list. Cell barring here is due to authentication
 * or authentication and ciphering failure.
 */
void rr_add_to_barred_cell_list(
  rr_barred_cell_info_T *barred_cell_ptr,
  const gas_id_t gas_id
)
{
  uint8   index;
  boolean cell_added = FALSE;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  /* check if cell is in barred cell list. If in it, do not add */
  if (!rr_is_cell_in_barred_cell_list(
         barred_cell_ptr->arfcn,
         barred_cell_ptr->bsic,
         NULL,
         &index,
         gas_id
       )
     )
  {
    /* Find an unused location in barred cell list */
    for (index = 0; index < RR_MAX_NUM_OF_BARRED_CELLS; index++)
    {
      if (rr_general_data_ptr->rr_barred_cell_list[index].remaining_time == RR_INVALID_REMAINING_TIME)
      {
        rr_general_data_ptr->rr_barred_cell_list[index].arfcn = barred_cell_ptr->arfcn;
        rr_general_data_ptr->rr_barred_cell_list[index].bsic = barred_cell_ptr->bsic;
#ifdef FEATURE_FAKE_BTS_CELL_BARRING 
        rr_general_data_ptr->rr_barred_cell_list[index].cgi = barred_cell_ptr->cgi; 
        rr_general_data_ptr->rr_barred_cell_list[index].cgi_valid = barred_cell_ptr->cgi_valid;
#endif
        rr_general_data_ptr->rr_barred_cell_list[index].remaining_time = barred_cell_ptr->remaining_time;
        cell_added = TRUE;
        break;
      }
    }

    /* List is full, cell is not added. Remove the oldest barred cell from the
       list, i.e. cell with shortest remaining time and add new cell in it's
       place */
    if (cell_added == FALSE)
    {
      index = rr_find_oldest_barred_cell(gas_id);
      if (index < RR_MAX_NUM_OF_BARRED_CELLS)
      {
        rr_general_data_ptr->rr_barred_cell_list[index].arfcn = barred_cell_ptr->arfcn;
        rr_general_data_ptr->rr_barred_cell_list[index].bsic = barred_cell_ptr->bsic;
        rr_general_data_ptr->rr_barred_cell_list[index].remaining_time = barred_cell_ptr->remaining_time;
#ifdef FEATURE_FAKE_BTS_CELL_BARRING 
        rr_general_data_ptr->rr_barred_cell_list[index].cgi = barred_cell_ptr->cgi; 
        rr_general_data_ptr->rr_barred_cell_list[index].cgi_valid = barred_cell_ptr->cgi_valid;
#endif
        cell_added = TRUE;
      }
    }

    if (cell_added)
    {
      /* Start barred cell timer */
      rr_start_cell_barred_timer(gas_id);

#ifndef FEATURE_FAKE_BTS_CELL_BARRING
      MSG_GERAN_HIGH_3_G("Cell added to barred cell list, arfcn: %d, ncc: %d,  bcc: %d",
               barred_cell_ptr->arfcn.num,
               barred_cell_ptr->bsic.PLMN_colour_code,
               barred_cell_ptr->bsic.BS_colour_code);
#else
      MSG_GERAN_HIGH_5_G("Cell added to barred cell list, arfcn: %d, ncc: %d,  bcc: %d, lac %d, ci %d",
               barred_cell_ptr->arfcn.num,
               barred_cell_ptr->bsic.PLMN_colour_code,
               barred_cell_ptr->bsic.BS_colour_code,
               barred_cell_ptr->cgi.lai.location_area_code,
               barred_cell_ptr->cgi.ci);
#endif

      MSG_GERAN_HIGH_1_G("Cell barred for: %d mS",
               barred_cell_ptr->remaining_time);
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Failed to add cell to barred cell list");
    }
  }
  else
  {
#ifdef FEATURE_FAKE_BTS_CELL_BARRING
    rr_general_data_ptr->rr_barred_cell_list[index].arfcn = barred_cell_ptr->arfcn;
    rr_general_data_ptr->rr_barred_cell_list[index].bsic = barred_cell_ptr->bsic;
    rr_general_data_ptr->rr_barred_cell_list[index].remaining_time = barred_cell_ptr->remaining_time;
    rr_general_data_ptr->rr_barred_cell_list[index].cgi = barred_cell_ptr->cgi; 
    rr_general_data_ptr->rr_barred_cell_list[index].cgi_valid = barred_cell_ptr->cgi_valid;

#endif
    MSG_GERAN_HIGH_3_G("Cell already in barred cell list, arfcn: %d, ncc: %d, bcc: %d",
                                         barred_cell_ptr->arfcn.num,
                                         barred_cell_ptr->bsic.PLMN_colour_code,
                                         barred_cell_ptr->bsic.BS_colour_code);
  }

}/* rr_add_to_barred_cell_list() */


#ifdef PERLUTF
#error code not present
#endif // PERLUTF


/**
 * Find the barred cell with shortest remaining time. This cell will be removed
 * from the list in case list is full and a new cell needs adding.
 */
uint8 rr_find_oldest_barred_cell(
  const gas_id_t gas_id
)
{
  uint8  index;
  uint32 shortest_remaining_time = RR_INVALID_REMAINING_TIME;
  uint8  index_of_shortest_time = 0;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  for(index =0; index < RR_MAX_NUM_OF_BARRED_CELLS; index++)
  {
    if(rr_general_data_ptr->rr_barred_cell_list[index].remaining_time <= shortest_remaining_time)
    {
      shortest_remaining_time = rr_general_data_ptr->rr_barred_cell_list[index].remaining_time;
      index_of_shortest_time = index;
    }
  }

  MSG_GERAN_HIGH_3_G("Oldest barred cell,arfcn: %d, ncc: %d, bcc: %d",
              rr_general_data_ptr->rr_barred_cell_list[index_of_shortest_time].arfcn.num,
              rr_general_data_ptr->rr_barred_cell_list[index_of_shortest_time].bsic.PLMN_colour_code,
              rr_general_data_ptr->rr_barred_cell_list[index_of_shortest_time].bsic.BS_colour_code);

  return index_of_shortest_time;
} /* rr_find_oldest_barred_cell() */



/**
 * Find out number of barred cells in the list.
 */
uint8 rr_get_barred_cell_count(
  const gas_id_t gas_id
)
{
  uint8 index;
  uint8 barred_cell_count = 0;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  for(index =0; index < RR_MAX_NUM_OF_BARRED_CELLS; index++)
  {
    if(rr_general_data_ptr->rr_barred_cell_list[index].remaining_time != RR_INVALID_REMAINING_TIME)
    {
      barred_cell_count++;
    }
  }

  MSG_GERAN_HIGH_1_G("Barred cell count: %d",barred_cell_count);

  return barred_cell_count;
}/* rr_get_barred_cell_count() */



/**
 * Search barred cell list for a cell. Cell barring here is due to authentication
 * or authentication and ciphering failure.
 */
boolean rr_is_cell_in_barred_cell_list(
  ARFCN_T arfcn,
  BSIC_T bsic,
  rr_csi_cgi_t *cgi,
  uint8 *barred_cell_index,
  const gas_id_t gas_id
)
{
  uint8   index;
  boolean return_value = FALSE;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  for(index =0; index < RR_MAX_NUM_OF_BARRED_CELLS; index++)
  {
    if(ARFCNS_EQUAL(rr_general_data_ptr->rr_barred_cell_list[index].arfcn, arfcn) &&
       rr_same_BSIC(&rr_general_data_ptr->rr_barred_cell_list[index].bsic, &bsic) &&
       (rr_general_data_ptr->rr_barred_cell_list[index].remaining_time != RR_INVALID_REMAINING_TIME))
    {
#ifdef FEATURE_FAKE_BTS_CELL_BARRING
      if ((cgi == NULL) ||
          (rr_same_lai(&rr_general_data_ptr->rr_barred_cell_list[index].cgi.lai, &cgi->lai) &&
          (rr_general_data_ptr->rr_barred_cell_list[index].cgi.ci == cgi->ci)))
#endif
     {
        MSG_GERAN_HIGH_3_G("Cell in barred list, arfcn: %d, ncc: %d, bcc: %d",
                                 rr_general_data_ptr->rr_barred_cell_list[index].arfcn.num,
                                 rr_general_data_ptr->rr_barred_cell_list[index].bsic.PLMN_colour_code,
                                 rr_general_data_ptr->rr_barred_cell_list[index].bsic.BS_colour_code);
        if (barred_cell_index != NULL)
        {
          *barred_cell_index = index;
        }
        return_value = TRUE;
        break;
      }
    }
  }
  return return_value;
} /* rr_is_cell_in_barred_cell_list() */



/**
 * Update remaining time of each cell in barred cell list. If remaining time
 * is zero for any cell, then remove the cell from the list
 */
void rr_update_barred_cell_remaining_time(
  const gas_id_t gas_id
)
{
  uint8 index;

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  for(index =0; index < RR_MAX_NUM_OF_BARRED_CELLS; index++)
  {
    if(rr_general_data_ptr->rr_barred_cell_list[index].remaining_time != RR_INVALID_REMAINING_TIME)
    {
      /* Deduct timeout value from remaining time */
      if(rr_general_data_ptr->rr_barred_cell_list[index].remaining_time > RR_CELL_BARRED_TIMEOUT)
      {
        rr_general_data_ptr->rr_barred_cell_list[index].remaining_time -= RR_CELL_BARRED_TIMEOUT;
      }
      /* Otherwise, remove the cell from list, i.e. invalidate remaining time,
         arfcn, and bsic */
      else
      {
        rr_general_data_ptr->rr_barred_cell_list[index].arfcn.num = INVALID_ARFCN;
        rr_general_data_ptr->rr_barred_cell_list[index].arfcn.band = INVALID_BAND;
        rr_general_data_ptr->rr_barred_cell_list[index].bsic.BS_colour_code = INVALID_BSIC_CC;
        rr_general_data_ptr->rr_barred_cell_list[index].bsic.PLMN_colour_code = INVALID_PLMN_CC;
        rr_general_data_ptr->rr_barred_cell_list[index].remaining_time = RR_INVALID_REMAINING_TIME;
      }
    }
  }
}/* rr_update_barred_cell_remaining_time() */

/**
 * RR_CELL_BARRED_TIMER expired. Update barred cell remaining time. Remove any
 * barred cells whose remaining time is zero. Start RR_CELL_BARRED_TIMER if
 * there are barred cells in the list.
 */
void rr_process_barred_cell_timer_expiry(
  const gas_id_t gas_id
)
{
  /* Firstly, deduct RR_CELL_BARED_TIMEOUT from remaining_time of each
   * cell in barred cell list. Following this deduction, if remaining
   * time is zero for any cell, then remove that cell from the barred
   * cell list */

  rr_update_barred_cell_remaining_time(gas_id);

  MSG_GERAN_HIGH_0_G("Barred cell timer expired");

  /* Restart barred cell timer if there are barred cells in the list */
  if(rr_get_barred_cell_count(gas_id) != 0)
  {
    rr_start_cell_barred_timer(gas_id);
  }
} /* rr_process_barred_cell_timer_expiry() */



/**
 * Authentication or authentication and ciphering failure has occured. Bar the
 * serving cell.
 */

void rr_authentication_failed_bar_cell(
  uint32 cell_bar_timer_value,
  const gas_id_t gas_id
)
{
  rr_barred_cell_info_T  barred_cell;
  gprs_scell_info_t     *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  if((cell_bar_timer_value >= RR_CELL_BARRED_TIMEOUT) &&
      (cell_bar_timer_value <= CONVERT_HOURS_TO_MILLISECONDS(12)))
  {
    /* Add cell to barred cell list. */
    barred_cell.arfcn = scell_info_ptr->gsm.BCCH_ARFCN;
    barred_cell.bsic = scell_info_ptr->gsm.BSIC;
    barred_cell.remaining_time = cell_bar_timer_value;
#ifdef FEATURE_FAKE_BTS_CELL_BARRING
    barred_cell.cgi_valid = FALSE;
#endif /* FEATURE_FAKE_BTS_CELL_BARRING */

    rr_add_to_barred_cell_list(&barred_cell, gas_id);
  }
  else
  {
    MSG_GERAN_HIGH_1_G("Cell barred period %dms is invalid", cell_bar_timer_value);
  }
} /* rr_authentication_failed_bar_cell() */

void rr_authn_fail_bar_cell_basedon_ARFCN_BSIC(uint32 cell_bar_timer_value, ARFCN_T arfcn, uint8 bsic, const gas_id_t gas_id)
{
  rr_barred_cell_info_T barred_cell;
  BSIC_T                local_bsic;

  CONVERT_TO_BSIC_T(local_bsic, bsic);
  
  if ((cell_bar_timer_value >= RR_CELL_BARRED_TIMEOUT) &&
      (cell_bar_timer_value <= CONVERT_HOURS_TO_MILLISECONDS(12)))
  {
    /* Add cell to barred cell list */
    barred_cell.arfcn = arfcn;
    barred_cell.bsic  = local_bsic;
    barred_cell.remaining_time = cell_bar_timer_value;
#ifdef FEATURE_FAKE_BTS_CELL_BARRING
    barred_cell.cgi_valid = FALSE;
#endif /* FEATURE_FAKE_BTS_CELL_BARRING */

    rr_add_to_barred_cell_list(&barred_cell, gas_id);
    MSG_GERAN_HIGH_2_G("Cell (%d,%d) barred", arfcn.num, arfcn.band);
  }
  else
  {
    MSG_GERAN_HIGH_1_G("Cell barred period %dms is invalid", cell_bar_timer_value);
  }

  return;
}

/**
 * Computes the BSIC value from a cell using the SCH block information
 * See GSM05.02 3.3.2.2 & GSM 03.03 4.3.2
 *
 * @param BSIC_result_ptr [out] Set to the computed BSIC value
 *
 * @param SCH_block       SCH block from which the BSIC is computed
 */
void rr_compute_BSIC_from_SCH_block(
  BSIC_T  *BSIC_result_ptr,
  byte    *SCH_block
)
{
  BSIC_result_ptr->PLMN_colour_code = (byte) ((((SCH_block[0] & 0xFC) >> 2) & 0x38) >> 3);
  BSIC_result_ptr->BS_colour_code = (byte) (((SCH_block[0] & 0xFC) >> 2) & 0x07);
} /* rr_compute_BSIC_from_SCH_block */

/**
 * Compares two BSIC values and returns TRUE if they are a match, FALSE otherwise
 *
 * @param bsic1  first BSIC value
 *
 * @param bsic2  second BSIC value
 *
 * @return TRUE if BSICs match, FALSE otherwise
 */
boolean rr_same_BSIC(BSIC_T *bsic1, BSIC_T *bsic2)
{
  return ((bsic1->BS_colour_code == bsic2->BS_colour_code) &&
          (bsic1->PLMN_colour_code == bsic2->PLMN_colour_code)
         );
}

/**
 * Sets the BA list in the given L1 parameters structure
 * based on the given BCCH allocation (should correspond
 * to the complete BA list). Filters the BA list to remove
 * the serving cell and any cells from unsupported bands
 *
 * @param l1params_ba_list   Target list
 *
 * @param freq_list_ptr      source list
 *
 * @param serving_cell_arfcn ARFCN of the serving cell
 */
static void rr_set_l1params_ba_list(
  inter_task_BCCH_allocation_T *l1params_ba_list,
  BCCH_frequency_list_T *freq_list_ptr,
  ARFCN_T serving_cell_arfcn,
  boolean remove_serving_cell,
  const gas_id_t gas_id
)
{
  int cell_index;
  int no_of_entries = freq_list_ptr->no_of_entries;
  int write_index = 0;

  RR_NULL_CHECK_RETURN_VOID(freq_list_ptr);
  RR_NULL_CHECK_RETURN_VOID(l1params_ba_list);

  /* initialize the target BA list to no entries */
  l1params_ba_list->no_of_entries = 0;

  /* start with the BA list (whatever is in the complete BA list so far) */
  for (cell_index = 0; (cell_index < no_of_entries) && (cell_index < BA_MAX_LEN); ++cell_index)
  {
    ARFCN_T BA_ARFCN;
    BA_ARFCN = freq_list_ptr->BCCH_ARFCN[cell_index];

    /* Dont include the serving cell ARFCN in BA list */
    if (!remove_serving_cell || (serving_cell_arfcn.num != BA_ARFCN.num))
    {
      /* don't include cells from unsupported bands */
      boolean band_supported = TRUE;

      /* WARNING: Temporary derive band from ARFCN but it should be obtained from the SI
       * byte band = BA_ARFCN.band;
       * make use of GET_ARFCN_BAND() here ?
       */
      rr_internal_band_T band = rr_get_band(BA_ARFCN.num, gas_id);

      if (!rr_gapi_is_supported_band((sys_band_T)band, gas_id))
      {
        MSG_GERAN_MED_1_G("BA entry=%d not included (unsupported band)",(int) BA_ARFCN.num);
        band_supported = FALSE;
      }

      if (band_supported)
      {
        l1params_ba_list->BCCH_ARFCN[write_index] = BA_ARFCN;
        l1params_ba_list->no_of_entries++;
        write_index++;
      }
      /* otherwise, the band is not supported and the entry is not included */
    }
    else
    {
      MSG_GERAN_MED_1_G("BA entry=%d not included because it matches scell",(int) BA_ARFCN.num);
    }
  } /* for all cells in SI source BA list */
} /* rr_set_l1params_ba_list */

/**
 * Notify RRC layer of other RATs of deep sleep status within GERAN access stratum
 *
 * @param in_deep_sleep   TRUE if GERAN is in deep sleep, FALSE otherwise
 *                        for MM/L1)
 *
 * @param gas_id          GERAN access stratum identifier
 *
 */
void rr_notify_deep_sleep_to_other_rats(boolean in_deep_sleep, const gas_id_t gas_id)
{
  /* only the multimode subscription needs to perform this operation */
  if (geran_is_multi_mode_gas_id(gas_id))
  {
#ifdef FEATURE_WCDMA
    if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA))
    {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      rrc_set_ue_in_deep_sleep_sub_id(in_deep_sleep, RR_GAS_ID_TO_AS_ID);
#else
      rrc_set_ue_in_deep_sleep(in_deep_sleep);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
    }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA))
    {
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
      tdsrrc_set_ue_in_deep_sleep(in_deep_sleep, RR_GAS_ID_TO_AS_ID);
#else
      tdsrrc_set_ue_in_deep_sleep(in_deep_sleep);
#endif /* FEATURE_TDS_SUB_ID_INTERFACE */
    }
#endif /* FEATURE_GSM_TDS */
  }

  return;
}

/**
 * Start idle mode on the cell specified in the sys_info_source.
 *
 * @param sys_info_source cell on which to start idle mode (also contains relevant SI
 *                        for MM/L1)
 *
 * @param notify_MM       indicates whether to send a rr_service_ind to MM or not
 *
 * @param early_camping_performed
 *                        indicates if early camping was performed on the cell or not.
 *                        If early camping was performed, an mph_idle_update_req is used
 *                        instead of a mph_start_idle_req
 */
void rr_start_idle_mode(
  gprs_scell_info_t *cell_info,
  boolean notify_MM,
  boolean early_camping_performed,
  const gas_id_t gas_id
)
{
  system_information_database_T *sys_info_source;
  layer1_sys_info_T              layer1_sys_info;
  boolean                        cbch_channel_information_valid;
  boolean                        imsi_valid;
  word                           imsi_mod_1000 = 0;
  inter_task_BCCH_allocation_T  *ba_list = rr_gprs_get_ba_gprs(gas_id);
  surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);
  rr_measured_signal_data_t     *rr_measured_signal_data_ptr = rr_get_measured_signal_data_ptr(gas_id);
#ifdef FEATURE_GSM_TDS
  rr_ded_irat_measurements_t    *tds_ded_meas_ptr = rr_get_tds_ded_meas_ptr(gas_id);
#endif /*FEATURE_GSM_TDS*/

  RR_NULL_CHECK_FATAL(cell_info);
  RR_NULL_CHECK_FATAL(ba_list);
  RR_NULL_CHECK_FATAL(wcdma_ded_meas_ptr);
  RR_NULL_CHECK_FATAL(rr_measured_signal_data_ptr);
  RR_ZERO_STRUCTURE(layer1_sys_info);

  sys_info_source = &cell_info->gsm;

  /* Dedicated data should no longer be considered valid, otherwise old
  SI5/5bis/5ter BA lists may be used if a new call is set up. */
  sys_info_source->valid_dedicated_data_flag = FALSE;

  /* Copy the BA(GPRS) list (which is always constructed, regardless of whether */
  /* operating as GSM-only) as the BA list for L1 */
  layer1_sys_info.BA_list = (* ba_list);

  cbch_channel_information_valid = sys_info_source->cbch_channel_information_valid;
  if (cbch_channel_information_valid)
  {
    /* If SI1 is received after SI4 then the following code will ensure the   */
    /* CBCH frequency list is populated from the SI1 cell channel description */
    if (!rr_set_channel_frequency_list_from_mobile_allocation(
           &sys_info_source->cbch_channel_information,
           &sys_info_source->cbch_mobile_allocation,
           &sys_info_source->cell_channel_description,
           gas_id
         )
       )
    {
      /* if the channel list setup fails, flag the info as invalid */
      MSG_GERAN_ERROR_0_G("CBCH channel information could not be populated");
      /* note that this is a LOCAL flag, the system info DB still contains TRUE */
      cbch_channel_information_valid = FALSE;
    }
  }

  layer1_sys_info.BS_PA_MFRMS = sys_info_source->control_channel_description.BS_PA_MFRMS;
  layer1_sys_info.BS_CC_CHANS = sys_info_source->control_channel_description.BS_CC_CHANS;
  layer1_sys_info.BS_AG_BLKS_RES = sys_info_source->control_channel_description.BS_AG_BLKS_RES;
  layer1_sys_info.BS_CCCH_SDCCH_COMB = sys_info_source->control_channel_description.BS_CCCH_SDCCH_COMB;
  layer1_sys_info.MS_TXPWR_MAX_CCH = sys_info_source->cell_selection_parameters.MS_TXPWR_MAX_CCH;
  layer1_sys_info.PWRC = sys_info_source->cell_options.PWRC;
  layer1_sys_info.NCC_permitted = sys_info_source->NCC_permitted;
  layer1_sys_info.TX_integer = sys_info_source->RACH_control_parameters.tx_integer;
  layer1_sys_info.cbch_channel_information_valid = cbch_channel_information_valid;
  layer1_sys_info.cbch_channel_information = sys_info_source->cbch_channel_information;

  if (cell_info->bcch_sys_info.si13_data_valid_flag)
  {
    layer1_sys_info.LB_MS_TXPWR_MAX_CCH_valid = cell_info->bcch_sys_info.LB_MS_TXPWR_MAX_CCH_valid;
    layer1_sys_info.LB_MS_TXPWR_MAX_CCH = cell_info->bcch_sys_info.LB_MS_TXPWR_MAX_CCH;
  }
  else
  {
    layer1_sys_info.LB_MS_TXPWR_MAX_CCH_valid = FALSE;
    layer1_sys_info.LB_MS_TXPWR_MAX_CCH = 0;
  }

  layer1_sys_info.multiband_reporting = sys_info_source->multiband_reporting;

  /******************************/
  /* Read the IMSI from the SIM */
  /* or from the local sim struct*/
  /******************************/
  imsi_valid = rr_get_imsi_mod_1000(&imsi_mod_1000, gas_id);

  if (early_camping_performed)
  {
    /*****************************************************************/
    /* Prior to this we performed early camping to listen to pages   */
    /* We do not need to send a start idle since L1 is already       */
    /* partially in idle mode but we need to update them with        */
    /* new information we didn't have before. Previously we didn't   */
    /* have the full SI                                              */
    /*****************************************************************/
    MSG_GERAN_HIGH_1_G("Cell(%d): starting idle, early camp performed",(int) sys_info_source->BCCH_ARFCN.num);
    rr_send_mph_idle_update_req(
      &layer1_sys_info,
      imsi_valid,
      imsi_mod_1000,
      gas_id
    );
    /* stop L1 from carrying on reading sys info */
    rr_send_mph_read_sys_info_req(RR_L1_SI_TYPE_NONE, gas_id);
  }
  else
  {
    /* start IDLE mode in L1 */
    MSG_GERAN_HIGH_1_G("Cell(%d): starting idle mode",(int) sys_info_source->BCCH_ARFCN.num);

    rr_send_mph_start_idle_req(
      &layer1_sys_info,
      imsi_valid,
      imsi_mod_1000,
      FALSE,
      gas_id
    );
  }

  #ifdef FEATURE_WCDMA
  /* (Re)initialize the wcdma measurements received in the dedicated mode */
  rr_initialize_wcdma_ded_measurements(wcdma_ded_meas_ptr);
  #endif /* FEATURE_WCDMA */

  #ifdef FEATURE_GSM_TDS
  rr_initialize_tds_ded_measurements(tds_ded_meas_ptr);
  #endif /*FEATURE_GSM_TDS*/

  /************************************************************************/
  /* If early camping was performed, L1 could have been told to perform   */
  /* PAGING_REORGANIZATION. If this is the case, L1 should go into normal */
  /* paging now.                                                          */
  /************************************************************************/
  if (early_camping_performed)
  {
    // Early Camping sets the Page Mode to Normal Paging. But in some scenarios where no paging information
    // is available, Paging Reorg is used. Ensure Normal Paging is used now
    (void) rr_ce_compare_and_update_l1_page_mode(
      PAGING_REORGANIZATION,
      NORMAL_PAGING,
      gas_id
    );
  }
  else
  {
    // L1 is using Normal Paging, so set the OTA Page Mode to match
    rr_ce_set_ota_page_mode(
      NORMAL_PAGING,
      gas_id
    );
  }

  if (!rr_timer_is_running(SIGNAL_STRENGTH_TIMER, gas_id))
  {
    MSG_GERAN_MED_0_G("Start signal strength timer");
    (void)rr_timer_start(SIGNAL_STRENGTH_TIMER, SIGNAL_STRENGTH_TIMEOUT, gas_id);
  }

  /* Send the Signal Strength Indication to UI */
  rr_send_cm_status_ind(gas_id);

  /* always set the five second timer id to the serving five second timer */
  sys_info_source->reselection_info.five_second_timer_id = RR_SERV_FIVE_SEC_C1_TIMER;


  /*****************************************************/
  /* Advise Cell Broadcast task whether CBCH messages  */
  /* are actually being broadcast in the cell          */
  /*****************************************************/
  rr_send_cell_change_ind(
    sys_info_source->location_area_identification,
    sys_info_source->cell_identity,
    sys_info_source->cbch_channel_information_valid,
    gas_id
  );

  rr_send_cb_info(
    sys_info_source->cbch_channel_information_valid,
    gas_id
  );

  /* Log the serving cell system information + cell information values */
  (void) rr_log_cell_info(sys_info_source, gas_id);
  (void) rr_log_cell_options(&(sys_info_source->cell_options), gas_id);
  (void) rr_log_cell_selection_parameters(&(sys_info_source->cell_selection_parameters), gas_id);
  (void) rr_log_control_channel_description(&(sys_info_source->control_channel_description), gas_id);
  (void) rr_log_rach_control_parameters(&(sys_info_source->RACH_control_parameters), gas_id);

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  {
    best_six_ncells_t *best_six_ncells_ptr = rr_get_best_six_ncells_ptr(gas_id);

    rr_notify_cell_id_change(
      sys_info_source->cell_identity,
      *best_six_ncells_ptr,
      sys_info_source->location_area_identification,
      gas_id
    );
  }
#endif /*FEATURE_GSM_RR_CELL_CALLBACK*/

  {
    eng_mode_scell_info_t scell_info = {0};
    gprs_scell_info_t *serving_cell_ptr = NULL;

    serving_cell_ptr = rr_gprs_get_scell_info(gas_id);

    scell_info = geran_eng_mode_format_scell_info(
      serving_cell_ptr,
      ENG_MODE_NO_CAUSE,
      gas_id
    );

    geran_eng_mode_data_write(
      ENG_MODE_SCELL_INFO,
      (void*)&scell_info,
      TRUE,
      gas_id
    );

    rr_eng_mode_update_scell_config(gas_id);
    rr_eng_mode_update_nw_params(gas_id);
  }

#ifdef FEATURE_QSH_EVENT_METRIC
  rr_qsh_metric_log_cell_info(gas_id);
#endif // FEATURE_QSH_EVENT_METRIC

  /* if MM should be notified */
  if (notify_MM)
  {
    /* send RR_SERVICE_IND primitive to MM to provide new system information values */
    rr_send_service_ind(
      MM_AS_SERVICE_AVAILABLE,
      &(sys_info_source->location_area_identification),
      sys_info_source,
      gas_id
    );
  }

  /*****************************************************************************/
  /* reset curent signal quality to the undefined value, as we are now in idle */
  /*****************************************************************************/
  rr_measured_signal_data_ptr->current_signal_quality = RR_INVALID_RXQUAL_VALUE;

  /* reset the list of cells which L1 is assumed to be synchronised with */
  rr_gprs_clear_sync_cell_list(gas_id);

  if (!rr_is_any_cell_selection_mode(gas_id))
  {
    uint8 i;

    (void)rr_acq_db_add_arfcn(
      rr_gprs_get_scell_arfcn(gas_id),
      rr_gprs_get_scell_plmn_id(gas_id),
      gas_id);

    for (i = 0; i < ba_list->no_of_entries; i++)
    {
      (void)rr_acq_db_add_arfcn(
        ba_list->BCCH_ARFCN[i],
        rr_gprs_get_scell_plmn_id(gas_id),
        gas_id
      );
    }
  }


  rr_notify_deep_sleep_to_other_rats(FALSE, gas_id);

  return;
} /* rr_start_idle_mode */

/**
 * Called to update L1 while in idle with the latest SI
 */
void rr_update_idle_mode(
  const gas_id_t gas_id
)
{
  layer1_sys_info_T  layer1_sys_info;
  boolean            imsi_valid;
  word               imsi_mod_1000   = 0;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(scell_info_ptr);
  RR_ZERO_STRUCTURE(layer1_sys_info);

  /* set the BA list in the L1 parameters structure (removing serving cell) */
  rr_set_l1params_ba_list(
    &layer1_sys_info.BA_list,
    &scell_info_ptr->gsm.complete_BA_list,
    scell_info_ptr->gsm.BCCH_ARFCN,
    TRUE,
    gas_id
  );

  layer1_sys_info.BS_PA_MFRMS =
    scell_info_ptr->gsm.control_channel_description.BS_PA_MFRMS;
  layer1_sys_info.BS_CC_CHANS =
    scell_info_ptr->gsm.control_channel_description.BS_CC_CHANS;
  layer1_sys_info.BS_AG_BLKS_RES =
    scell_info_ptr->gsm.control_channel_description.BS_AG_BLKS_RES;
  layer1_sys_info.BS_CCCH_SDCCH_COMB =
    scell_info_ptr->gsm.control_channel_description.BS_CCCH_SDCCH_COMB;

  layer1_sys_info.MS_TXPWR_MAX_CCH =
    scell_info_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH;
  layer1_sys_info.PWRC =
    scell_info_ptr->gsm.cell_options.PWRC;
  layer1_sys_info.NCC_permitted =
    scell_info_ptr->gsm.NCC_permitted;
  layer1_sys_info.TX_integer =
    scell_info_ptr->gsm.RACH_control_parameters.tx_integer;
  layer1_sys_info.cbch_channel_information_valid =
    scell_info_ptr->gsm.cbch_channel_information_valid;
  layer1_sys_info.cbch_channel_information =
    scell_info_ptr->gsm.cbch_channel_information;

  if (scell_info_ptr->bcch_sys_info.si13_data_valid_flag)
  {
    layer1_sys_info.LB_MS_TXPWR_MAX_CCH_valid = scell_info_ptr->bcch_sys_info.LB_MS_TXPWR_MAX_CCH_valid;
    layer1_sys_info.LB_MS_TXPWR_MAX_CCH = scell_info_ptr->bcch_sys_info.LB_MS_TXPWR_MAX_CCH;
  }
  else
  {
    layer1_sys_info.LB_MS_TXPWR_MAX_CCH_valid = FALSE;
    layer1_sys_info.LB_MS_TXPWR_MAX_CCH = 0;
  }

  layer1_sys_info.multiband_reporting = scell_info_ptr->gsm.multiband_reporting;

  imsi_valid = rr_get_imsi_mod_1000(&imsi_mod_1000, gas_id);

  rr_send_mph_idle_update_req(
    &layer1_sys_info,
    imsi_valid,
    imsi_mod_1000,
    gas_id
  );

  /*****************************************************/
  /* Advise Cell Broadcast task whether CBCH messages  */
  /* are actually being broadcast in the cell          */
  /*****************************************************/
  rr_send_cell_change_ind(
    scell_info_ptr->gsm.location_area_identification,
    scell_info_ptr->gsm.cell_identity,
    scell_info_ptr->gsm.cbch_channel_information_valid,
    gas_id
  );

  rr_send_cb_info(
    scell_info_ptr->gsm.cbch_channel_information_valid,
    gas_id
  );

} /* rr_update_idle_mode */

/**************************************************************************************
Function
rr_copy_common_neighbors_list_params

Description
Copy the source common parameters to the destination common parameters.
This routine will be called per instance. The final values stored will
be from the instance with the highest numbered index.

Parameters
[OUT]
SI2q_MI_params_T *dest_common_params
[IN]
SI2q_MI_params_T *source_common_params

Returns
None

**********************************************************************************************/

static void rr_copy_common_neighbors_list_params(
  SI2q_MI_params_T *dest_common_params,
  SI2q_MI_params_T *source_common_params
)
{
  /* The copy is done only if the parameter was present in the instance */
  /* This is done so that the valid information already stored is not
  wiped out by the information that was not present in the next instance */
  if (source_common_params->Absolute_Index_Start_EMR_PI)
  {
    dest_common_params->Absolute_Index_Start_EMR_PI = source_common_params->Absolute_Index_Start_EMR_PI;
    dest_common_params->Absolute_Index_Start_EMR = source_common_params->Absolute_Index_Start_EMR;
  }
  if (source_common_params->Index_Start_3G_PI)
  {
    dest_common_params->Index_Start_3G_PI = source_common_params->Index_Start_3G_PI;
    dest_common_params->Index_Start_3G = source_common_params->Index_Start_3G;
  }

  dest_common_params->MP_CHANGE_MARK = source_common_params->MP_CHANGE_MARK;
  dest_common_params->BA_IND = source_common_params->BA_IND;
  dest_common_params->BA_IND_3G = source_common_params->BA_IND_3G;
  dest_common_params->COUNT = source_common_params->COUNT;
  dest_common_params->INDEX = source_common_params->INDEX;

  if (source_common_params->UTRAN_FDD_Desc_PI)
  {
    dest_common_params->UTRAN_FDD_Desc_PI = source_common_params->UTRAN_FDD_Desc_PI;
  }
  if (source_common_params->WCDMA_3G_NC_Desc_PI)
  {
    dest_common_params->WCDMA_3G_NC_Desc_PI = source_common_params->WCDMA_3G_NC_Desc_PI;
  }

  dest_common_params->utran_tdd_bandwidth = source_common_params->utran_tdd_bandwidth;
}

/**************************************************************************************
Function
rr_copy_si2q_idle_neighbors_list_params

Description
Copy the source SI2quater parameters to the destination SI2quater parameters.
This routine will be called per instance. The final values stored will
be from the instance with the highest numbered index.

Parameters
[OUT]
SI2q_params_T *dest_si2q_params
[IN]
SI2q_params_T *source_si2q_params

Returns
None

**********************************************************************************************/
static void rr_copy_si2q_idle_neighbors_list_params(
  SI2q_params_T *dest_si2q_params,
  SI2q_params_T *source_si2q_params
)
{
  /* The copy is done only if the parameter was present in the instance */
  /* This is done so that the valid information already stored is not
  wiped out by the information that was not present in the next instance */
  if (source_si2q_params->WCDMA_3G_MP_Desc_PI)
  {
    dest_si2q_params->WCDMA_3G_MP_Desc_PI = source_si2q_params->WCDMA_3G_MP_Desc_PI;
    dest_si2q_params->QSearch_I = source_si2q_params->QSearch_I;
    dest_si2q_params->QSearch_C_Initial = source_si2q_params->QSearch_C_Initial;

    if (source_si2q_params->FDD_Info_PI)
    {
      dest_si2q_params->FDD_Info_PI = source_si2q_params->FDD_Info_PI;
      dest_si2q_params->FDD_Qoffset = source_si2q_params->FDD_Qoffset;
      dest_si2q_params->FDD_REP_QUANT = source_si2q_params->FDD_REP_QUANT;
      dest_si2q_params->FDD_MULTIRAT_REPORTING = source_si2q_params->FDD_MULTIRAT_REPORTING;
      dest_si2q_params->FDD_Qmin = source_si2q_params->FDD_Qmin;
    }
    else
    {
      /* if the dest_si2q_params is not filled in yet (by previous instances) */
      if (dest_si2q_params->FDD_Info_PI == FALSE)
      {
        dest_si2q_params->FDD_Qoffset = 8;
      }
    }

    if (source_si2q_params->TDD_Info_PI)
    {
      dest_si2q_params->TDD_Info_PI = source_si2q_params->TDD_Info_PI;
      dest_si2q_params->TDD_Qoffset = source_si2q_params->TDD_Qoffset;
      dest_si2q_params->TDD_MULTIRAT_REPORTING = source_si2q_params->TDD_MULTIRAT_REPORTING;
    }
    else
    {
      /* if the dest_si2q_params is not filled in yet (by previous instances) */
      if (dest_si2q_params->TDD_Info_PI == FALSE)
      {
        dest_si2q_params->TDD_Qoffset = 5; /*default -90dBm*/
      }
    }
  }
  else
  {
    /* if the dest_si2q_params is not filled in yet (by previous instances) */
    if (dest_si2q_params->FDD_Info_PI == FALSE)
    {
      dest_si2q_params->FDD_Qoffset = 8;
    }

    if (dest_si2q_params->TDD_Info_PI == FALSE)
    {
      dest_si2q_params->TDD_Qoffset = 5; /*default -90dBm*/
    }
  }

  if (source_si2q_params->MP_Desc_PI)
  {
    dest_si2q_params->MP_Desc_PI = source_si2q_params->MP_Desc_PI;
    dest_si2q_params->REPORT_TYPE = source_si2q_params->REPORT_TYPE;
    dest_si2q_params->SERVING_BAND_REPORTING = source_si2q_params->SERVING_BAND_REPORTING;
  }
  else /* if REPORT_TYPE is not given, set to the default value which is REPORT_TYPE_N0RMAL */
  {
    /* if the dest_si2q_params is not filled in yet (by previous instances) */
    if (dest_si2q_params->MP_Desc_PI == FALSE)
    {
      dest_si2q_params->REPORT_TYPE = REPORT_TYPE_N0RMAL;
    }
  }

  if (source_si2q_params->FDD_additional_MPs_PI)
  {
    dest_si2q_params->FDD_additional_MPs_PI = TRUE;
    dest_si2q_params->FDD_Qmin_offset = source_si2q_params->FDD_Qmin_offset;
    dest_si2q_params->FDD_RSCPmin = source_si2q_params->FDD_RSCPmin;
  }
  else
  {
    /* !! Don't set the FDD_additional_MPs_PI = FALSE, because no FDD_additional_MPs
     * one instance doesn't mean FDD_additional_MPs is not present in the whole set.
     */
    if (dest_si2q_params->FDD_additional_MPs_PI == FALSE)
    {
      dest_si2q_params->FDD_RSCPmin = DEFAULT_FDD_RSCPMIN;
    }
  }

  if (source_si2q_params->FDD_additional_MPs_2_PI)
  {
    dest_si2q_params->FDD_additional_MPs_2_PI = TRUE;
    if (source_si2q_params->fdd_reporting_threshold_2_valid)
    {
      dest_si2q_params->fdd_reporting_threshold_2_valid = TRUE;
      dest_si2q_params->FDD_REPORTING_THRESHOLD_2 = source_si2q_params->FDD_REPORTING_THRESHOLD_2;
    }
  }
  else
  {
    if (dest_si2q_params->FDD_additional_MPs_2_PI == FALSE)
    {
      dest_si2q_params->FDD_REPORTING_THRESHOLD_2 = DEFAULT_FDD_REPORTING_THRESHOLD_2;
    }
  }
}

/**************************************************************************************
Function
rr_copy_mi_ded_neighbors_list_params

Description
Copy the source MI parameters to the destination MI parameters.
This routine will be called per instance. The final values stored will
be from the instance with the highest numbered index.

Parameters
[OUT]
MI_params_T *dest_mi_params
[IN]
MI_params_T *source_mi_params

Returns
None

**********************************************************************************************/

static void rr_copy_mi_ded_neighbors_list_params(
  MI_params_T *dest_mi_params,
  MI_params_T *source_mi_params
)
{
  byte i;

  dest_mi_params->PWRC = source_mi_params->PWRC;
  dest_mi_params->REPORT_TYPE = source_mi_params->REPORT_TYPE;
  dest_mi_params->REPORTING_RATE = source_mi_params->REPORTING_RATE;
  dest_mi_params->INVALID_BSIC_REPORTING = source_mi_params->INVALID_BSIC_REPORTING;

  if (source_mi_params->BSIC_Desc_PI)
  {
    dest_mi_params->BSIC_Desc_PI = source_mi_params->BSIC_Desc_PI;
    dest_mi_params->BSIC_Desc = source_mi_params->BSIC_Desc;
  }

  /* REPORT_PRIORITY is not sent to L1 in ded_neighbor_list but copy it anyway */
  if (source_mi_params->REPORT_PRIORITY_Desc_PI)
  {
    dest_mi_params->REPORT_PRIORITY_Desc_PI = source_mi_params->REPORT_PRIORITY_Desc_PI;
    for (i = 0; i < MAX_REP_PRIO_BITS; ++i)
    {
      dest_mi_params->REP_PRIORITY[i] = source_mi_params->REP_PRIORITY[i];
    }
  }
  if (source_mi_params->MP_Desc_PI)
  {
    dest_mi_params->MP_Desc_PI = source_mi_params->MP_Desc_PI;
    dest_mi_params->MP_Desc = source_mi_params->MP_Desc;
  }
  if (source_mi_params->MI_3G_WAIT_PI)
  {
    dest_mi_params->MI_3G_WAIT_PI = source_mi_params->MI_3G_WAIT_PI;
    dest_mi_params->MI_3G_WAIT = source_mi_params->MI_3G_WAIT;
  }
  if (source_mi_params->MI_3G_MP_Desc_PI)
  {
    dest_mi_params->MI_3G_MP_Desc_PI = source_mi_params->MI_3G_MP_Desc_PI;
    dest_mi_params->QSearch_C = source_mi_params->QSearch_C;
    dest_mi_params->FDD_REP_QUANT = source_mi_params->FDD_REP_QUANT;
    if (source_mi_params->FDD_MULTIRAT_REPORTING_PI)
    {
      dest_mi_params->FDD_MULTIRAT_REPORTING_PI = source_mi_params->FDD_MULTIRAT_REPORTING_PI;
      dest_mi_params->FDD_MULTIRAT_REPORTING = source_mi_params->FDD_MULTIRAT_REPORTING;
    }
    if (source_mi_params->FDD_REP_PI)
    {
      dest_mi_params->FDD_REP_PI = source_mi_params->FDD_REP_PI;
      dest_mi_params->FDD_REP_OFFSET = source_mi_params->FDD_REP_OFFSET;
      dest_mi_params->FDD_REP_THRESHOLD = source_mi_params->FDD_REP_THRESHOLD;
    }
    if (source_mi_params->TDD_MULTIRAT_REPORTING_PI)
    {
      dest_mi_params->TDD_MULTIRAT_REPORTING_PI = source_mi_params->TDD_MULTIRAT_REPORTING_PI;
      dest_mi_params->TDD_MULTIRAT_REPORTING = source_mi_params->TDD_MULTIRAT_REPORTING;
    }
    if (source_mi_params->TDD_REP_PI)
    {
      dest_mi_params->TDD_REP_PI = source_mi_params->TDD_REP_PI;
      dest_mi_params->TDD_REP_OFFSET = source_mi_params->TDD_REP_OFFSET;
      dest_mi_params->TDD_REP_THRESHOLD = source_mi_params->TDD_REP_THRESHOLD;
    }
  }

  if (source_mi_params->fdd_reporting_threshold_2_valid)
  {
    dest_mi_params->fdd_reporting_threshold_2_valid = TRUE;
    dest_mi_params->fdd_reporting_threshold_2 = source_mi_params->fdd_reporting_threshold_2;
  }
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined (FEATURE_LTE)

void rr_l1_update_wcdma_neighbor_list(
  const gas_id_t gas_id
)
{
  if (rr_x2g_cco_in_progress(gas_id))
  {
    MSG_GERAN_HIGH_0_G("X2G CCO in progress, block IRAT measurement request");
  }
  else
  {
    gprs_scell_info_t          * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
    rr_params_mods_t           ** params_mods = rr_params_mods(gas_id);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
    rr_sys_info_3g_data_t      * rr_sys_info_3g_data_ptr = rr_get_sys_info_3g_data_ptr(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

    if (rr_gsm_only(gas_id))
    {
      MSG_GERAN_MED_0_G("GSM-only, sending empty 3G cell reselection list to L1");
      rr_send_mph_wcdma_cell_update_list_req(NULL, gas_id);
    }
    else
    {

      if (rr_gprs_get_nc_mode(gas_id) == NC_MEAS_NC2 ||
          (params_mods != NULL && *params_mods != NULL && (*params_mods)->gprs_3g_meas_params.valid)
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
          || rr_sys_info_3g_data_ptr->grr_3g_nlist == &rr_sys_info_3g_data_ptr->grr_3g_nlist_modified
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
         )
      {
        MSG_GERAN_MED_0_G("Sending 3G cell reselection list (PBCCH/NC2) to L1");
        rr_gprs_send_mph_wcdma_cell_update_list_req(gas_id);
      }
      else
      {
        if (scell_info_ptr->gsm.utran_neighbor_list.valid_data ||
            scell_info_ptr->gsm.wcdma_neighbor_list_si2ter.valid_data)
        {
          MSG_GERAN_MED_0_G("Sending 3G cell reselection list (BCCH) to L1");
          rr_send_mph_wcdma_cell_update_list_req(scell_info_ptr, gas_id);
        }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
        else if (rr_is_pseudo_si2q_data_used(gas_id) == TRUE )
        {
            MSG_GERAN_MED_0_G("Sending Pseudo SI2Q data to L1");
            rr_send_mph_wcdma_cell_update_list_req(scell_info_ptr, gas_id);            
        }
#endif
        else
        {
          MSG_GERAN_MED_0_G("3G cell reselection list (BCCH) not received");
          rr_send_mph_wcdma_cell_update_list_req(NULL, gas_id);
        }
      }
    }
  }
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

/*
 *   Function name:  rr_update_3g_frequency_list
 *   -------------------------------------------------
 *
 *   Description: Adds the given uarfcn to the frequency list if its not already present in the
 *   ------------
 *                frequency list.
 *
 *   Parameters:
 *   -----------
          [IN] : utran_freq_list - utran neighbor frequency list in the system information database
                             or in the pending_channel_data
                 uarfcn - the frequency to be added
                 utran_rat - RAT type FDD, TDD
          [IN-OUT] : num_cells - the number of entries in the frequency list that will
                     be updated if the given frequency was added to the list.
 *
 *   Returns: none
 *   --------
 *
 */

void rr_update_3g_frequency_list (
  rr_utran_arfcn_t *utran_freq_list,
  byte *num_frequencies,
  word uarfcn,
  rr_utran_rat_type_t utran_rat
)
{
  byte i;
  /* Check if the given uarfcn is in the frequency list */
  for (i = 0; i < *num_frequencies; ++i)
  {
    if (utran_freq_list[i].uarfcn == uarfcn)
    {
      break;
    }
  }
  /* If it is not, add to the cell list and increment num_frequencies */
  if (i == *num_frequencies)
  {
    utran_freq_list[i].uarfcn = uarfcn;
    utran_freq_list[i].rat_type = utran_rat;
    (*num_frequencies)++;
  }
  else
  {
    MSG_GERAN_HIGH_0("Frequency already in 3G cell list");
  }
}

/**************************************************************************************
Function
rr_populate_idle_neighbor_list

Description
Populate the wcdma_idle_neighbor_list structure using information received in the
SI2quater instances.

Parameters
[OUT]
utran_idle_neighbor_list_T *wcdma_idle_neighbor_list,
[IN]
instances_T *SI2quater_instances

Returns
None

**********************************************************************************************/
void  rr_populate_idle_neighbor_list (
  utran_idle_neighbor_list_T *utran_idle_neighbor_list_ptr,
  instances_T *SI2quater_instances
)
{
  byte  i, fdd_cell_index, cell_index, tdd_cell_index, sync_case_tstd;

  // temporary variable
  word wcdma_cell, tdd_cell = 0;

  boolean diversity = FALSE;
  word scrambling_code, cpid = 0;
  rr_utran_cell_T *cell_list;
  rr_utran_arfcn_t *freq_list;
  byte *num_cells, *num_freq;


  /* Initialize the utran_idle_neighbor_list */
  rr_initialize_wcdma_idle_neighbor_list(utran_idle_neighbor_list_ptr);

  /* alias */
  freq_list = utran_idle_neighbor_list_ptr->wcdma_freq_only_list.uarfcn;
  num_freq = &utran_idle_neighbor_list_ptr->wcdma_freq_only_list.num_of_frequencies;
  cell_list = utran_idle_neighbor_list_ptr->wcdma_cell_list.cell_list;
  num_cells = &utran_idle_neighbor_list_ptr->wcdma_cell_list.num_of_cells;

  /* populate the cell list */
  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    SI2q_MI_instance_T *instance = &SI2quater_instances->instance[i];
    byte num_fdd_info, num_tdd_info;

    if (!instance->valid_data)
    {
      continue;
    }
    /* copy parameters : called per instance in order to get the parameters
    from the instance with the highest numbered index */
    rr_copy_common_neighbors_list_params(&utran_idle_neighbor_list_ptr->common_params,
      &instance->common_params);

    rr_copy_si2q_idle_neighbors_list_params(&utran_idle_neighbor_list_ptr->si2q_params,
      &instance->si2q_mi.si2q_params);


    /***************** Populate FDD **************************************************/

    /* alias */
    num_fdd_info = instance->common_params.num_repeated_FDD_info;

    for (fdd_cell_index = 0; fdd_cell_index < num_fdd_info; ++fdd_cell_index)
    {
      FDD_Info_T *fdd_info;

      /* alias */
      fdd_info = &instance->FDD_Info[fdd_cell_index];

      /* If FDD_Indic0 is 1, then Frequency 0 should be included in the set. It was excluded during
      the encoding process. */
      if (fdd_info->FDD_Indic0 == 1)
      {
        /* Add only if there is space left */
        if (*num_cells < MAX_RR_MPH_CELL_UPDATE_LIST)
        {
          /* Add the frequency 0 for the given uarfcn */
          rr_update_3g_cell_list (cell_list, num_cells, fdd_info->uarfcn, 0, 0, 0, RR_UTRAN_FDD);
        }
      }

      if (fdd_info->NR_OF_FDD_CELLS == 31)
      {
        /* Add only if there is space left */
        if (*num_freq < MAX_UTRAN_FREQ_UPDATE_LIST)
        {
          /* Add the uarfcn to the frequency only list */
          rr_update_3g_frequency_list (freq_list, num_freq, fdd_info->uarfcn, RR_UTRAN_FDD);
        }
      }
      else /* fdd_info->num_cells > 0 */
      {
        /* Note cell_index starts from 1 */
        for (cell_index = 1;
             cell_index <= fdd_info->num_cells && cell_index < MAX_UTRAN_CELLS_PER_INSTANCE;
             ++cell_index)
        {
          /* break as soon as the MAX_RR_MPH_CELL_UPDATE_LIST cells have been populated */
          if (*num_cells >= MAX_RR_MPH_CELL_UPDATE_LIST)
          {
            break;
          }
          /* For each (10-bit-long) decoded Parameter, bits 1-9 are the Scrambling Code and
          bit 10 is the corresponding Diversity Parameter. */

          wcdma_cell = fdd_info->decoded_and_sorted_cell_values[cell_index];

          /* find if the diversity (bit10) is set */
          if (wcdma_cell & 0x0200)
          {
            diversity = TRUE;
          }
          else
          {
            diversity = FALSE;
          }

          /* The next  bit9 to bit1 are the scrambling code */
          scrambling_code = wcdma_cell & 0x01FF;

          // Now we have the Primary SC, but since L1 needs plain Old SC ( 13 bit )
          // Convert accordingly.
          scrambling_code <<= 4;

          rr_update_3g_cell_list (
            cell_list,
            num_cells,
            fdd_info->uarfcn,
            scrambling_code,
            diversity,
            0,
            RR_UTRAN_FDD
          );
        } // for (cell_index = 0; cell_index < fdd_info->num_cells; ++cell_index)
      } // fdd_info->num_cells > 0
    } // for (fdd_cell_index = 0; fdd_cell_index < instance->num_FDD_Info; ++fdd_cell_index)

    /***************** Populate TDD **************************************************/

    /* alias */
    num_tdd_info = instance->common_params.num_repeated_TDD_info;

    for (tdd_cell_index = 0; tdd_cell_index < num_tdd_info; ++tdd_cell_index)
    {
      TDD_Info_T *tdd_info;

      /* alias */
      tdd_info = &instance->TDD_Info[tdd_cell_index];

      /* If FDD_Indic0 is 1, then Frequency 0 should be included in the set. It was excluded during
      the encoding process. */
      if (tdd_info->TDD_Indic0 == 1)
      {
        /* Add only if there is space left */
        if (*num_cells < MAX_RR_MPH_CELL_UPDATE_LIST)
        {
          /* Add the frequency 0 for the given uarfcn */
          rr_update_3g_cell_list (cell_list, num_cells, tdd_info->uarfcn, 0, 0, 0, RR_UTRAN_TDD);
        }
      }

      if (tdd_info->NR_OF_TDD_CELLS == 31)
      {
        /* Add only if there is space left */
        if (*num_freq < MAX_UTRAN_FREQ_UPDATE_LIST)
        {
          /* Add the uarfcn to the frequency only list */
          rr_update_3g_frequency_list (freq_list, num_freq, tdd_info->uarfcn, RR_UTRAN_TDD);
        }
      }
      else /* tdd_info->num_cells > 0 */
      {
        /* Note cell_index starts from 1 */
        for (cell_index = 1;
             cell_index <= tdd_info->num_cells && cell_index < MAX_UTRAN_CELLS_PER_INSTANCE;
             ++cell_index)
        {
          /* break as soon as the MAX_RR_MPH_CELL_UPDATE_LIST cells have been populated */
          if (*num_cells >= MAX_RR_MPH_CELL_UPDATE_LIST)
          {
            break;
          }
          /* For each (9-bit-long) decoded Parameter, bits 1-7 are the Cell Parameter ID and
          bit 8 is the Sync Case TSTD, bit 9 is the diversity bit*/

          tdd_cell = tdd_info->decoded_and_sorted_cell_values[cell_index];

          /* find if the diversity (bit9) is set */
          if (tdd_cell & 0x0100)
          {
            diversity = TRUE;
          }
          else
          {
            diversity = FALSE;
          }

          /* Sync Case TSTD is bit 8*/
          sync_case_tstd = (tdd_cell & 0x0080) >> 7;

          /* The next  bit7 to bit1 are the cell parameter id */
          cpid = tdd_cell & 0x007F;

          rr_update_3g_cell_list (
            cell_list,
            num_cells,
            tdd_info->uarfcn,
            cpid,
            diversity,
            sync_case_tstd,
            RR_UTRAN_TDD
          );
        } // for (cell_index = 0; cell_index < tdd_info->num_cells; ++cell_index)
      } // tdd_info->num_cells > 0
    } // for (tdd_cell_index = 0; tdd_cell_index < instance->num_TDD_Info; ++tdd_cell_index)
  } // for (i = 0; i < MAX_INSTANCES; ++i)

  /* set valid_data to true */
  utran_idle_neighbor_list_ptr->valid_data = TRUE;
}

/**************************************************************************************
Function
rr_populate_idle_neighbor_list_si2ter

Description
Populate the wcdma_idle_neighbor_list structure using information received in the
SI2ter instances.

Parameters
[OUT]
utran_idle_neighbor_list_T *wcdma_idle_neighbor_list,
[IN]
instances_T *instances

Returns
None

**********************************************************************************************/
void  rr_populate_idle_neighbor_list_si2ter (
  wcdma_idle_neighbor_list_si2ter_T *wcdma_idle_neighbor_list_si2ter,
  si2ter_instances_T *instances
)
{
  word *freq_list;
  byte *num_freq;
  byte  i;
  uint8 *num_tdd_freq;

  ASSERT(wcdma_idle_neighbor_list_si2ter != NULL);
  ASSERT(instances != NULL);

  memset(wcdma_idle_neighbor_list_si2ter, 0, sizeof(wcdma_idle_neighbor_list_si2ter_T));

  /* alias */
  freq_list = wcdma_idle_neighbor_list_si2ter->wcdma_freq_only_list.uarfcn;
  num_freq = &wcdma_idle_neighbor_list_si2ter->wcdma_freq_only_list.num_of_frequencies;
  num_tdd_freq = &wcdma_idle_neighbor_list_si2ter->tds_freq_only_list.num_of_frequencies;

  /* populate the cell list */
  for (i = 0; i < MAX_SI2TER_INSTANCES; ++i)
  {
    si2ter_rest_T *instance = &(instances->instance[i]);

    if (!instance->valid_data)
    {
      continue;
    }

    /* copy */
    if (instance->FDD_INFO_PI == TRUE)
    {
      freq_list[*num_freq] = instance->FDD_arfcn;
      (*num_freq)++;
    }

    if (instance->TDD_INFO_PI == TRUE)
    {
      wcdma_idle_neighbor_list_si2ter->tds_freq_only_list.uarfcn[*num_tdd_freq] = instance->TDD_arfcn;
      (*num_tdd_freq)++;
    }

    if (instance->TDD_BANDWIDTH_PI == TRUE)
    {
       wcdma_idle_neighbor_list_si2ter->TDD_BANDWIDTH_valid = TRUE;
       wcdma_idle_neighbor_list_si2ter->TDD_BANDWIDTH = instance->TDD_BANDWIDTH;
    }

    if (instance->WCDMA_3G_MP_Desc_PI)
    {
      wcdma_idle_neighbor_list_si2ter->Qsearch_I = instance->Qsearch_I;
      if (instance->WCDMA_3G_MP_FDD_Params_PI)
      {
        wcdma_idle_neighbor_list_si2ter->FDD_Qmin = instance->FDD_Qmin;
        wcdma_idle_neighbor_list_si2ter->FDD_Qoffset = instance->FDD_Qoffset;
      } /* if (instance->WCDMA_3G_MP_FDD_Params_PI) */

      if (instance->WCDMA_3G_MP_TDD_Params_PI)
      {
        wcdma_idle_neighbor_list_si2ter->TDD_Qoffset = instance->TDD_Qoffset;
      } /* if (instance->WCDMA_3G_MP_TDD_Params_PI) */
    } /* if (instance->WCDMA_3G_MP_Desc_PI) */

    if (instance->FDD_additional_MPs_valid)
    {
      wcdma_idle_neighbor_list_si2ter->FDD_additional_MPs_valid = TRUE;
      wcdma_idle_neighbor_list_si2ter->FDD_Qmin_offset = instance->FDD_Qmin_offset;
      wcdma_idle_neighbor_list_si2ter->FDD_RSCPmin = instance->FDD_RSCPmin;
    }
    else
    {
      wcdma_idle_neighbor_list_si2ter->FDD_additional_MPs_valid = FALSE;
    }
  } /* end for (i = 0; i < MAX_SI2TER_INSTANCES; ++i) */

  /* set valid_data to true */
  wcdma_idle_neighbor_list_si2ter->valid_data = TRUE;
}

/**************************************************************************************
Function
rr_update_3g_cell_list

Description
Add the cell specified by the uarfcn, scrambling_code, diversity into the cell_list if
the cell is not already present in the cell_list. The *num_cells is incremented if the
cell is added to the cell_list.

Parameters
[IN/OUT]
wcdma_cell_T *cell_list
List to which the specified cell is added to
byte *num_cells
Number of cells in the cell_list
[IN]
word uarfcn
word scrambling_code
boolean diversity
Cell that need to be added to the cell list.

Returns
None

**********************************************************************************************/
void rr_update_3g_cell_list (
  rr_utran_cell_T *cell_list,
  byte *num_cells,
  word uarfcn,
  word cell_code,
  boolean diversity,
  uint8 sync_case_tstd,
  rr_utran_rat_type_t utran_rat
)
{
  byte i;

  ASSERT (cell_list != NULL);
  ASSERT (num_cells != NULL);

  /* Check if the given cell is in the cell list */
  for (i = 0; i < *num_cells; ++i)
  {
    if (
        (cell_list[i].uarfcn.rat_type == utran_rat) &&
        (cell_list[i].uarfcn.uarfcn == uarfcn) &&
        (cell_list[i].cell_code == cell_code) &&
        (cell_list[i].diversity == diversity)
       )
    {
      break;
    }
  }
  /* If it is not, add to the cell list and increment numcells */
  if (i == *num_cells)
  {
    cell_list[i].uarfcn.rat_type = utran_rat;
    cell_list[i].uarfcn.uarfcn = uarfcn;
    cell_list[i].cell_code = cell_code;
    cell_list[i].diversity = diversity;
    cell_list[i].sync_case_tstd = sync_case_tstd;
    (*num_cells)++;
  }
  else
  {
    MSG_GERAN_HIGH_0("Cell already in 3G cell list");
  }
}

/**************************************************************************************
Function
rr_purge_wcdma_ded_measurements

Description
Purges the old entries (if) present in the wcdma_ded_meas structure using the 3G NC list sent
to L1 in dedicated mode.

Parameters
[IN]
surr_wcdma_ded_measurements_T *surr_wcdma_ded_meas_ptr
wcdma_ded_neighbor_list_T *wcdma_ded_nlist_ptr

Returns
None

**********************************************************************************************/

void rr_purge_wcdma_ded_measurements(
  surr_wcdma_ded_measurements_T *surr_wcdma_ded_meas_ptr,
  wcdma_ded_neighbor_list_T *wcdma_ded_nlist_ptr
)
{
  byte num_meas_entries = surr_wcdma_ded_meas_ptr->num_entries;

  if ((num_meas_entries == 0) || (wcdma_ded_nlist_ptr->valid_data == FALSE))
  {
    return;
  }
  else
  {
    byte i;
    for (i = 0; i < num_meas_entries; ++i)
    {
      byte j;
      byte num_ded_ncells = wcdma_ded_nlist_ptr->wcdma_cell_list.num_of_cells;
      wcdma_cell_T *meas_cell = &surr_wcdma_ded_meas_ptr->meas[i].cell;
      for (j = 0; j < num_ded_ncells; ++j)
      {
        rr_utran_cell_T *list_cell = &wcdma_ded_nlist_ptr->wcdma_cell_list.cell_list[j];
        if (
            (meas_cell->uarfcn == list_cell->uarfcn.uarfcn) &&
            (meas_cell->scrambling_code == list_cell->cell_code) &&
            (meas_cell->diversity == list_cell->diversity)
           )
        {
          break;
        }
      } /* loop on the ncell list */
      if (j == num_ded_ncells) /* not found */
      {
        memset((void *)&surr_wcdma_ded_meas_ptr->meas[i], 0, sizeof(wcdma_ded_measurements_T));
        surr_wcdma_ded_meas_ptr->num_entries--;
      } /* if cell in wcdma_ded_meas not found in the list  */
    } /* loop on the ded measurements */
  } /* end else */
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
/**************************************************************************************
Function
rr_get_reported_RSCP

Description
See 04.18 section 9.1.1.3 CPICH RSCP measurement report mapping

Parameters
[IN]
int16 RSCP received from L1

Returns
mapped value

**********************************************************************************************/
byte rr_get_reported_RSCP(
  int16 RSCP
)
{
  int reported_rscp = RSCP + 116;

  if (reported_rscp < 0)
  {
    return 0;
  }
  else if (reported_rscp > 63)
  {
    return 63;
  }
  else
  {
    return (uint8) reported_rscp;
  }
}

/**************************************************************************************
Function
rr_get_reported_Ec_No

Description
See 04.18 section 9.1.2.3 CPICH Ec/Io measurement report mapping

Parameters
[IN]
int16 Ec_Nox2: Ec_No times 2, received from L1

Returns
mapped value

**********************************************************************************************/
byte rr_get_reported_Ec_No(
  int16 Ec_Nox2
)
{
  if (Ec_Nox2 < -48)
  {
    return 0;
  }
  if (Ec_Nox2 > 0)
  {
    return 49;
  }
  return (byte)(Ec_Nox2 + 49);
}

/**************************************************************************************
Function
rr_get_reported_RSSI

Description
See 04.18 section 9.1.3.3 UTRA Carrier RSSI measurement report mapping

Parameters
[IN]
int16 RSSI: RSSI received from L1

Returns
mapped value

**********************************************************************************************/
byte rr_get_reported_RSSI(
  int16 RSSI
)
{
  if (RSSI < -100)
  {
    return 0;
  }
  if (RSSI > -25)
  {
    return 76;
  }
  return (byte)(RSSI + 101);
}
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

/**************************************************************************************
Function
rr_initialize_wcdma_idle_neighbor_list

Description
Initialize the wcdma idle neighbor list.

Parameters
[IN]
wcdma_neighbor_list_T *wcdma_neighbor_list

Returns
None

**********************************************************************************************/
void rr_initialize_wcdma_idle_neighbor_list(
  wcdma_neighbor_list_T *wcdma_neighbor_list
)
{
  memset ((void *)wcdma_neighbor_list, 0, sizeof (wcdma_neighbor_list_T));
  wcdma_neighbor_list->si2q_params.FDD_additional_MPs_PI   = FALSE;
  wcdma_neighbor_list->si2q_params.FDD_additional_MPs_2_PI = FALSE;
  wcdma_neighbor_list->si2q_params.FDD_Qoffset = DEFAULT_FDD_QOFFSET;
  wcdma_neighbor_list->si2q_params.TDD_Qoffset = DEFAULT_TDD_QOFFSET;
  wcdma_neighbor_list->si2q_params.QSearch_I = DEFAULT_QSEARCH_I;
  wcdma_neighbor_list->si2q_params.FDD_Qmin = DEFAULT_FDD_QMIN;
  wcdma_neighbor_list->si2q_params.FDD_RSCPmin = DEFAULT_FDD_RSCPMIN;
  wcdma_neighbor_list->si2q_params.FDD_Qmin_offset = DEFAULT_FDD_QMIN_OFFSET;
  wcdma_neighbor_list->si2q_params.REPORT_TYPE = REPORT_TYPE_N0RMAL;
}


/**************************************************************************************
Function
rr_populate_ded_neighbor_list

Description
Populate the wcdma_ded_neighbor_list structure using information received in the
MI instances.

Parameters
[OUT]
wcdma_ded_neighbor_list_T *wcdma_ded_neighbor_list
[IN]
instances_T *MI_instances

Returns
None

**********************************************************************************************/
void rr_populate_ded_neighbor_list(
  wcdma_ded_neighbor_list_T *wcdma_ded_neighbor_list_ptr,
  instances_T *MI_instances,
  const gas_id_t gas_id
)
{
  byte               i, fdd_cell_index, tdd_cell_index, cell_index;

  // temporary variable
  word               wcdma_cell, tdd_cell = 0;

  boolean            diversity = FALSE;
  word               scrambling_code, cpid = 0;
  byte               sync_case_tstd = 0;
  rr_utran_cell_T   *cell_list;
  rr_utran_arfcn_t  *freq_list;
  byte              *num_cells, *num_freq;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);


  /* Initialize the wcdma_idle_neighbor_list */
  memset (wcdma_ded_neighbor_list_ptr, 0, sizeof(wcdma_ded_neighbor_list_T));

  /* alias */
  freq_list = wcdma_ded_neighbor_list_ptr->wcdma_freq_only_list.uarfcn;
  num_freq = &wcdma_ded_neighbor_list_ptr->wcdma_freq_only_list.num_of_frequencies;

  cell_list = wcdma_ded_neighbor_list_ptr->wcdma_cell_list.cell_list;
  num_cells = &wcdma_ded_neighbor_list_ptr->wcdma_cell_list.num_of_cells;

  /* populate the cell list */
  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    // alias
    SI2q_MI_instance_T *instance = &MI_instances->instance[i];
    byte num_fdd_info;
    byte num_tdd_info;

    if (!instance->valid_data)
    {
      continue;
    }

    /* set valid_data to true if any instance has been received */
    wcdma_ded_neighbor_list_ptr->valid_data = TRUE;

    /* copy parameters : called per instance in order to get the parameters
    from the instance with the highest numbered index */
    rr_copy_common_neighbors_list_params(&wcdma_ded_neighbor_list_ptr->common_params,
      &instance->common_params);

    rr_copy_mi_ded_neighbors_list_params(&wcdma_ded_neighbor_list_ptr->mi_params,
      &instance->si2q_mi.mi_params);

    /***************** Populate FDD **************************************************/

    num_fdd_info = instance->common_params.num_repeated_FDD_info;

    for (fdd_cell_index = 0; fdd_cell_index < num_fdd_info; ++fdd_cell_index)
    {
      // alias
      FDD_Info_T *fdd_info = &instance->FDD_Info[fdd_cell_index];

      /* If FDD_Indic0 is 1, then Frequency 0 should be included in the set. It was excluded during
      the encoding process. */
      if (fdd_info->FDD_Indic0 == 1)
      {
        /* Add only if there is space left */
        if (*num_cells < MAX_RR_MPH_CELL_UPDATE_LIST)
        {
          /* Add the frequency 0 for the given uarfcn */
          rr_update_3g_cell_list (cell_list, num_cells, fdd_info->uarfcn, 0, 0, 0, RR_UTRAN_FDD);
        }
      }

      if (fdd_info->num_cells == 0)
      {
        /* Add only if there is space left */
        if (*num_freq < MAX_UTRAN_FREQ_UPDATE_LIST)
        {
          /* Add the uarfcn to the frequency only list */
          rr_update_3g_frequency_list (freq_list, num_freq, fdd_info->uarfcn, RR_UTRAN_FDD);
        }
      }
      else /* fdd_info->num_cells > 0 */
      {
        for (cell_index = 1;
             cell_index <= fdd_info->num_cells && cell_index < MAX_UTRAN_CELLS_PER_INSTANCE;
             ++cell_index)
        {
          /*  as soon as the MAX_RR_MPH_CELL_UPDATE_LIST cells have been populated */
          if (*num_cells >= MAX_RR_MPH_CELL_UPDATE_LIST)
          {
            break;
          }
          /* For each (10-bit-long) decoded Parameter, bits 1-9 are the Scrambling Code and
          bit 10 is the corresponding Diversity Parameter. */

          wcdma_cell = fdd_info->decoded_and_sorted_cell_values[cell_index];

          /* find if the diversity (bit10) is set */
          if (wcdma_cell & 0x0200)
          {
            diversity = TRUE;
          }
          else
          {
            diversity = FALSE;
          }

          /* The next  bit9 to bit1 are the scrambling code */
          scrambling_code = wcdma_cell & 0x01FF;

          // Now we have the Primary SC, but since L1 needs plain Old SC ( 13 bit )
          // Convert accordingly.
          scrambling_code <<= 4;

          rr_update_3g_cell_list (
            cell_list,
            num_cells,
            fdd_info->uarfcn,
            scrambling_code,
            diversity,
            0,
            RR_UTRAN_FDD
          );
        } // for (cell_index = 0; cell_index < fdd_info->num_cells; ++cell_index)
      } /* fdd_info->num_cells > 0 */
    } // for (fdd_cell_index = 0; fdd_cell_index < instance->num_FDD_Info; ++fdd_cell_index)

  /***************** Populate TDD **************************************************/

    num_tdd_info = instance->common_params.num_repeated_TDD_info;

    for (tdd_cell_index = 0; tdd_cell_index < num_tdd_info; ++tdd_cell_index)
    {
      // alias
      TDD_Info_T *tdd_info = &instance->TDD_Info[tdd_cell_index];

      /* If FDD_Indic0 is 1, then Frequency 0 should be included in the set. It was excluded during
      the encoding process. */
      if (tdd_info->TDD_Indic0 == 1)
      {
        /* Add only if there is space left */
        if (*num_cells < MAX_RR_MPH_CELL_UPDATE_LIST)
        {
          /* Add the frequency 0 for the given uarfcn */
          rr_update_3g_cell_list (cell_list, num_cells, tdd_info->uarfcn, 0, 0, 0, RR_UTRAN_TDD);
        }
      }

      if (tdd_info->NR_OF_TDD_CELLS >= 21)
      {
        /* Add only if there is space left */
        if (*num_freq < MAX_UTRAN_FREQ_UPDATE_LIST)
        {
          /* Add the uarfcn to the frequency only list */
          rr_update_3g_frequency_list (freq_list, num_freq, tdd_info->uarfcn, RR_UTRAN_TDD);
        }
      }
      else /* tdd_info->num_cells > 0 */
      {
        for (cell_index = 1;
             cell_index <= tdd_info->num_cells && cell_index < MAX_UTRAN_CELLS_PER_INSTANCE;
             ++cell_index)
        {
          /*  as soon as the MAX_RR_MPH_CELL_UPDATE_LIST cells have been populated */
          if (*num_cells >= MAX_RR_MPH_CELL_UPDATE_LIST)
          {
            break;
          }

          /* For each (9-bit-long) decoded Parameter, bits 1-7 are the Cell Parameter ID and
          bit 8 is the Sync Case TSTD, bit 9 is the diversity bit*/

          tdd_cell = tdd_info->decoded_and_sorted_cell_values[cell_index];

          /* find if the diversity (bit9) is set */
          if (tdd_cell & 0x0100)
          {
            diversity = TRUE;
          }
          else
          {
            diversity = FALSE;
          }

          /* Sync Case TSTD is bit 8*/
          sync_case_tstd = (tdd_cell & 0x0080) >> 7;

          /* The next  bit7 to bit1 are the cell parameter id */
          cpid = tdd_cell & 0x007F;

          rr_update_3g_cell_list (
            cell_list,
            num_cells,
            tdd_info->uarfcn,
            cpid,
            diversity,
            sync_case_tstd,
            RR_UTRAN_TDD
          );
        } // for (cell_index = 0; cell_index < tdd_info->num_cells; ++cell_index)
      } /* tdd_info->num_cells > 0 */
    } // for (tdd_cell_index = 0; tdd_cell_index < instance->num_TDD_Info; ++tdd_cell_index)
  } // for (i = 0; i < MAX_INSTANCES; ++i)

  /* if there have no 3G_MP from MI, we need take Qsearch_xx from SI2quater/SI2ter into account */
  if (wcdma_ded_neighbor_list_ptr->mi_params.MI_3G_MP_Desc_PI == FALSE)
  {
    SI2q_params_T *si2q_params = &(scell_info_ptr->gsm.utran_neighbor_list.si2q_params);
    if (si2q_params->WCDMA_3G_MP_Desc_PI)
    {
      if (si2q_params->QSearch_C_Initial)
      {
        wcdma_ded_neighbor_list_ptr->mi_params.QSearch_C = QSEARCH_I_ALWAYS;
      }
      else
      {
        MSG_GERAN_HIGH_1_G("Use Qsearch_I for Qsearch_C %d", si2q_params->QSearch_I);
        wcdma_ded_neighbor_list_ptr->mi_params.QSearch_C = si2q_params->QSearch_I;
      }
    }
    else
    {
      /*+452757- ASR */
      if (rr_si2quater_all_instances_received(rr_get_instances_ptr(SI2Q_INSTANCE,gas_id)))//All SI2qs  received
      {
      /*-452757- ASR */
        wcdma_ded_neighbor_list_ptr->mi_params.QSearch_C = QSEARCH_I_ALWAYS;

      /*+452757- ASR */
      }
      else
      {
        wcdma_ded_neighbor_list_ptr->mi_params.QSearch_C = 15;//Never According to  44.018, section 3.4.1.2.1
      }
      /*-452757- ASR */
    }
  }

}

/**************************************************************************************
Function
rr_initialize_wcdma_ded_neighbor_list

Description
Initialize the wcdma dedicated neighbor list.

Parameters
[IN]
wcdma_ded_neighbor_list_T *wcdma_ded_neighbor_list

Returns
None

**********************************************************************************************/
void rr_initialize_wcdma_ded_neighbor_list(
  wcdma_ded_neighbor_list_T *wcdma_ded_neighbor_list_ptr
)
{
  memset ((void *)wcdma_ded_neighbor_list_ptr, 0, sizeof (wcdma_ded_neighbor_list_T));
}

#ifdef FEATURE_GSM_TDS
/**************************************************************************************
Function
rr_initialize_tds_ded_measurements

Description
Initialize the tds dedicated measurements

Parameters
[IN]
rr_ded_irat_measurements_t *tds_ded_meas_ptr

Returns
None

**********************************************************************************************/
void rr_initialize_tds_ded_measurements(
  rr_ded_irat_measurements_t *tds_ded_meas_ptr
)
{
  int num_elements;
  int i;

  memset((void *)tds_ded_meas_ptr, 0, sizeof(rr_ded_irat_measurements_t));

  num_elements = (int)(sizeof(tds_ded_meas_ptr->entries) / sizeof(tds_ded_meas_ptr->entries[0]));

  /* Initialize the ts0_pos to 0xFFFF */
  for (i = 0; i < num_elements; i++)
  {
    tds_ded_meas_ptr->entries[i].data.utran_tdd.ts0_pos = 0xFFFF;
  }
}

#endif /*FEATURE_GSM_TDS*/

/**************************************************************************************
Function
rr_initialize_wcdma_ded_measurements

Description
Initialize the wcdma dedicated measurements

Parameters
[IN]
surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr

Returns
None

**********************************************************************************************/
void rr_initialize_wcdma_ded_measurements(
  surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr
)
{
  int num_elements;
  int i;

  memset((void *)wcdma_ded_meas_ptr, 0, sizeof(surr_wcdma_ded_measurements_T));

  num_elements = (int)(sizeof(wcdma_ded_meas_ptr->meas) / sizeof(wcdma_ded_meas_ptr->meas[0]));

  /* Initialize the PN_offset to 0xFFFF */
  for (i = 0; i < num_elements; i++)
  {
    wcdma_ded_meas_ptr->meas[i].PN_Offset = 0xFFFF;
  }
}

/**************************************************************************************
Function
rr_initialize_wcdma_neighbor_cell_list

Description
Initialize the wcdma neighbor cell list that maintains the indices to be reported in
the Measurement Report.

Parameters
[IN]
wcdma_neighbor_cell_list_T *wcdma_cell_list_ptr

Returns
None

**********************************************************************************************/
void rr_initialize_wcdma_neighbor_cell_list(
  wcdma_neighbor_cell_list_T *wcdma_cell_list_ptr
)
{
  memset ((void *)wcdma_cell_list_ptr, 0, sizeof (wcdma_neighbor_cell_list_T) * (MAX_UTRAN_CELLS + MAX_UTRAN_FREQ));
}

#if 0 // not used
#ifdef FEATURE_NONBLIND_HANDOVER_DEBUG
/**************************************************************************************
Function
rr_show_known_wcdma_cells

Description
Show surrounding WCDMA known cells

Parameters
[IN]
surr_wcdma_ded_measurements_T *surr_wcdma_ded_meas

Returns
None

**********************************************************************************************/

void rr_show_known_wcdma_cells(surr_wcdma_ded_measurements_T *surr_wcdma_ded_meas)
{
  byte i;
  MSG_GERAN_HIGH_0_G("Surrounding WCDMA known cells");
  MSG_GERAN_HIGH_0_G("-----------------------------");
  for (i = 0; i < surr_wcdma_ded_meas->num_entries; ++i)
  {
    if (surr_wcdma_ded_meas->meas[i].PN_Offset != 0xFFFF)
    {
      MSG_GERAN_HIGH_3_G("UARFCN=%d, SC=%d, DIV=%d",\
              (int)surr_wcdma_ded_meas->meas[i].cell.uarfcn,\
              (int)surr_wcdma_ded_meas->meas[i].cell.scrambling_code,\
              (int)surr_wcdma_ded_meas->meas[i].cell.diversity);
    }
  }

  if (i == 0)
  {
    MSG_GERAN_HIGH_0_G("------ No known cells -----");
  }
}


void rr_show_idle_neighbor_list(utran_idle_neighbor_list_T *wcdma_neighbor_list)
{
  int num_freq_only = 0, num_cells = 0;
  int i, j;

  ASSERT(wcdma_neighbor_list != NULL);

  num_freq_only = wcdma_neighbor_list->wcdma_freq_only_list.num_of_frequencies;
  num_cells = wcdma_neighbor_list->wcdma_cell_list.num_of_cells;

  MSG_GERAN_HIGH_0_G("---WCDMA IDLE NCELL LIST---");
  MSG_GERAN_HIGH_2_G("NumFreqOnly=%d, NumCells=%d",(int)num_freq_only, (int)num_cells);

  for (i = 0; i < num_freq_only; ++i)
  {
    MSG_GERAN_HIGH_2_G("UARFCN_ONLY[%d]=%d", (int)i, (int)wcdma_neighbor_list->wcdma_freq_only_list.uarfcn[i]);
  }

  for (j = 0; j < num_cells; ++j)
  {
    MSG_GERAN_HIGH_1_G("WCDMA NCELL[%d]", (int)j);
    MSG_GERAN_HIGH_3_G("UARFCN=%d, SC=%d, DIV=%d", (int) wcdma_neighbor_list->wcdma_cell_list.cell_list[j].uarfcn, (int) wcdma_neighbor_list->wcdma_cell_list.cell_list[j].scrambling_code, (int) wcdma_neighbor_list->wcdma_cell_list.cell_list[j].diversity);
  }
}

void rr_show_ded_neighbor_list(wcdma_ded_neighbor_list_T *wcdma_neighbor_list)
{
  int num_freq_only = 0, num_cells = 0;
  int i, j;

  ASSERT(wcdma_neighbor_list != NULL);

  num_freq_only = wcdma_neighbor_list->wcdma_freq_only_list.num_of_frequencies;
  num_cells = wcdma_neighbor_list->wcdma_cell_list.num_of_cells;

  MSG_GERAN_HIGH_0_G("---WCDMA IDLE NCELL LIST---");
  MSG_GERAN_HIGH_2_G("NumFreqOnly=%d, NumCells=%d",(int)num_freq_only, (int)num_cells);

  for (i = 0; i < num_freq_only; ++i)
  {
    MSG_GERAN_HIGH_2_G("UARFCN_ONLY[%d]=%d", (int)i, (int)wcdma_neighbor_list->wcdma_freq_only_list.uarfcn[i]);
  }

  for (j = 0; j < num_cells; ++j)
  {
    MSG_GERAN_HIGH_1_G("WCDMA NCELL[%d]", (int)j);
    MSG_GERAN_HIGH_3_G("UARFCN=%d, SC=%d, DIV=%d", (int) wcdma_neighbor_list->wcdma_cell_list.cell_list[j].uarfcn, (int) wcdma_neighbor_list->wcdma_cell_list.cell_list[j].scrambling_code, (int) wcdma_neighbor_list->wcdma_cell_list.cell_list[j].diversity);
  }
}


void rr_show_rr_rrc_interrat_measurement_data(rr_rrc_interrat_measurement_data_type *rr_rrc_interrat_measurement_data)
{
  if (rr_rrc_interrat_measurement_data != NULL)
  {
    wcdma_cell_meas_type *wcdma_meas_ptr;

    wcdma_meas_ptr = rr_rrc_interrat_measurement_data->wcdma_cell_measurements;

    if (wcdma_meas_ptr != NULL)
    {
      int num_cells;
      int i;

      num_cells = rr_rrc_interrat_measurement_data->number_of_cells;

      MSG_GERAN_HIGH_1_G("Sending RRC_INTERRAT_HANDOVER_REQ with %d WCDMA measurements",(int)num_cells);

      for (i = 0; (i < num_cells) && (i < RR_RRC_MAX_INTERRAT_MEASUREMENTS); i++, wcdma_meas_ptr++)
      {
        MSG_GERAN_HIGH_3_G("%d, UARFCN=%d, PSC=%d", (int) i, (int)wcdma_meas_ptr->uarfcn, (int)wcdma_meas_ptr->primary_scrambling_code);
        MSG_GERAN_HIGH_3_G("%d, DIVERSITY=%d, PN_OFFSET=%d", (int)i, (int)wcdma_meas_ptr->diversity, (int)wcdma_meas_ptr->PN_Offset);
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Invalid measurement pointer passed to function");
  }

  return;
}

#endif /* FEATURE_NONBLIND_HANDOVER_DEBUG */
#endif // #if 0

/**************************************************************************************
Function
rr_get_rep_priority

Description
Extract the report priority bit from the array at the specified index.

Parameters
[IN]
uint32 *rep_priority
Input report priority array

byte index
index to the bit that needs to be extracted.

Returns
report_priority at the specified index.

**********************************************************************************************/
static boolean rr_get_rep_priority(
  uint32 *rep_priority,
  byte index
)
{
  uint32 bit_position = 0;

  if (index <= 31)
  {
    bit_position = 0x0001 << (31-index);
    return (boolean)(rep_priority[0] & bit_position);
  }
  if (index >= 32 && index <= 63)
  {
    bit_position = 0x0001 << (63-index);
    return (boolean)(rep_priority[1] & bit_position);
  }
  if (index >= 64 && index <= 95)
  {
    bit_position = 0x0001 << (95-index);
    return (boolean)(rep_priority[2] & bit_position);
  }

  /* Indices exceeding the value 95 shall be ignored. */
  return 0;

}

/****************************************************************************************

Function:
rr_create_wcdma_neighbor_cell_index_list

Description:
Creates the 3G neighbor cell list; indices from this list are used for measurement reporting.
This could be called with either the SI2quater or MI instances as identified by the source
parameter.

Parameters:
[IN]
instances_T *I
boolean source:
0 = SOURCE_SI2_QUATER, 1 = SOURCE_MEAS_INFO
[OUT]
wcdma_neighbor_cell_list_T *wcdma_neighbor_cell_list

Returns:
None

Rules:
Rules for creating the 3G neighbor cell list (04.18) :

1)
The 3G Neighbour Cell list may contain up to 96 3G Neighbour Cells and/or UTRAN frequencies for RSSI reporting.

2)
Each 3G Neighbour Cell Description received is added to the 3G Neighbour
Cell list, starting with the index equal to the parameter Index_Start_3G. If
this parameter is not present then the value 0 shall be used.

3)
For each 3G Neighbour Cell Description, the cells / UTRAN frequencies are indexed in the following order:
UTRAN FDD cells / UTRAN FDD frequencies: FDD ARFCNs are indexed in the order of occurrence in the
3G Neighbour Cell description. For each FDD ARFCN indicating UTRAN FDD cells, the cells are
indexed in the order of increasing values of the decoded FDD_CELL_INFORMATION parameters.

4)
For each 3G Neighbour Cell Description, the cells / UTRAN frequencies are indexed in the following order:
UTRAN TDD cells / UTRAN TDD frequencies: TDD ARFCNs are indexed in the order of occurrence in the
3G Neighbour Cell description. For each TDD ARFCN indicating UTRAN TDD cells, the cells are
indexed in the order of increasing values of the decoded TDD_CELL_INFORMATION parameters.

5)
If a 3G Neighbour Cell Description includes non-supported frequencies or Radio Access Technologies,
this shall not be considered as an error; indices in the 3G Neighbour Cell list shall be incremented
accordingly. [THIS RULE IS NOT ACCOUNTED FOR YET]

6)
If more than one cell/UTRAN frequency with the same index in the 3G Neighbour Cell list are
provided by different instances of 3G Neighbour Cell Descriptions, the cell / UTRAN frequency
from the message instance with the highest index shall be used.

7)
If a cell / UTRAN frequency is provided for an index higher than 95 in the 3G Neighbour Cell list,
this shall not be considered as an error; the cell / UTRAN frequency shall not be included in the
3G Neighbour Cell list.

8)
Rules for both SI2quater and Measurement Information (44.018 table 9.1.54.1b, 10.5.2.33b)
a) If NR_OF_FDD_CELS =0 and FDD_Indic0=0, this indicates the 3G Neighbour Cell list index for report on RSSI, see 3GPP TS 05.08.
b) If NR_OF_FDD_CELS is equal or greater than 17, this shall not be considered as an error; the corresponding index in the 3G
Neighbour Cell list shall be incremented by one.
c) If NR_OF_TDD_CELS is equal or greater than 21 OR NR_OF_TDD_CELS is equal to 0 and TDD_Indic0=0 this shall not be considered as an error; the corresponding index in the 3G
Neighbour Cell list shall be incremented by one.

9)
Rule for SI2quater only
If parameter NR_OF_TDD_CELS or NR_OF_FDD_CELS in table 9.1.54.1a. is equal to 31, this indicates that the corresponding UARFCN
shall be included in the 3G Cell Reselection list (see clause 3.4.1.2.1.7); no index shall be
allocated in the 3G Neighbour Cell list.

***********************************************************************************************/

void rr_create_wcdma_neighbor_cell_index_list(
  instances_T *I,
  wcdma_neighbor_cell_list_T *wcdma_neighbor_cell_list_ptr,
  boolean source /* 0 = SOURCE_SI2_QUATER, 1 = SOURCE_MEAS_INFO*/
)
{
  byte i;
  byte rep_FDD_index;
#ifdef FEATURE_GSM_TDS
  byte rep_TDD_index;
#endif /*FEATURE_GSM_TDS*/

  ASSERT (I != NULL);
  ASSERT (wcdma_neighbor_cell_list_ptr != NULL);

  /* Initialize the wcdma_neighbor_cell_list */
  memset ((void *)wcdma_neighbor_cell_list_ptr, 0, sizeof(wcdma_neighbor_cell_list_T)*(MAX_UTRAN_CELLS + MAX_UTRAN_FREQ));

  /* for (each instance I in the wcdma_neighbours_info.instances array) */
  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    /* temporary alias */
    SI2q_MI_instance_T *instance = &I->instance[i];
    /* temp alias */
    byte num_repeated_FDD_info = instance->common_params.num_repeated_FDD_info;

#ifdef FEATURE_GSM_TDS
    byte num_repeated_TDD_info = instance->common_params.num_repeated_TDD_info;
#endif /*FEATURE_GSM_TDS*/

    /* Rule 2: Starting index for each instance is Index_Start_3G */
    /* Rule 5: The instances are looped in ascending order and
    the cell/UTRAN frequency from the instance with the highest index will overwrite
    any information in  the 3G neighbour cell index that could be present already. */
    byte curr_wcdma_cell_list_index = instance->common_params.Index_Start_3G;

    if (!instance->valid_data)
    {
      continue;
    }

    /********************************************/
    /******** Handle FDD frequencies  ***********/
    /********************************************/

    /* for (each frequency F in I) */
    for (
          rep_FDD_index = 0;
          (rep_FDD_index < num_repeated_FDD_info) &&
          (curr_wcdma_cell_list_index < MAX_UTRAN_CELLS); /* Rule 1 & Rule 7 */
          ++rep_FDD_index
        )
    {
      /* temporary alias */
      FDD_Info_T *fdd_info = &instance->FDD_Info[rep_FDD_index];

      /* If FDD_Indic0 is set, the cell with scrambling code 0 should be included for this frequency. */
      if (fdd_info->FDD_Indic0)
      {
        wcdma_neighbor_cell_list_T *cell_list = &wcdma_neighbor_cell_list_ptr[curr_wcdma_cell_list_index];
        wcdma_cell_T *cell = &cell_list->cell;

        cell->uarfcn = fdd_info->uarfcn;
        cell->diversity = FALSE;
        cell->scrambling_code = 0;
        cell->diversity = FALSE;

        /* REP_PRIORITY is received only in the measurement information */
        if (source == SOURCE_MEAS_INFO)
        {
          cell_list->MI_REP_PRIORITY = rr_get_rep_priority(instance->si2q_mi.mi_params.REP_PRIORITY,
            curr_wcdma_cell_list_index);
        }
        cell_list->frequency_only = FREQ_SC;
        cell_list->valid_data = TRUE;

        cell_list->rat_type = RR_UTRAN_FDD;

        curr_wcdma_cell_list_index++;
      }

      /* if (num of cells present for F is greater than 0 )*/
      if (fdd_info->num_cells > 0)
      {
        /* for (each cell Fsc associated with F ) */
        byte cell_index;
        byte num_cells = fdd_info->num_cells;

        for (
             cell_index = 1;
             (cell_index <= num_cells) &&
             (cell_index < MAX_UTRAN_CELLS_PER_INSTANCE) &&
             (curr_wcdma_cell_list_index < MAX_UTRAN_CELLS); /* Rule 7 */
             ++cell_index
            )
        {
          /* Rule 3: The fdd_info has decoded_and_sorted_cell_values */
          /* alias for source */
          word wcdma_cell = fdd_info->decoded_and_sorted_cell_values[cell_index];
          /* alias for destination */
          wcdma_neighbor_cell_list_T *cell_list = &wcdma_neighbor_cell_list_ptr[curr_wcdma_cell_list_index];
          wcdma_cell_T *cell = &cell_list->cell;

          /* copy */
          cell->uarfcn = fdd_info->uarfcn;

          cell->diversity = FALSE;

          /* find if the diversity (bit10) is set */
          if (wcdma_cell & 0x0200)
          {
            cell->diversity = TRUE;
          }

          /* The next  bit9 to bit1 are the scrambling code */
          cell->scrambling_code = wcdma_cell & 0x01FF;

          /* change to plain old SC */
          cell->scrambling_code <<= 4;

          /* REP_PRIORITY is received only in the measurement information */
          if (source == SOURCE_MEAS_INFO)
          {
            cell_list->MI_REP_PRIORITY =
              rr_get_rep_priority(instance->si2q_mi.mi_params.REP_PRIORITY, curr_wcdma_cell_list_index);
          }

          cell_list->frequency_only = FREQ_SC;
          cell_list->valid_data = TRUE;

          cell_list->rat_type = RR_UTRAN_FDD;

          /* Rule 5 */
          /* indices are incremented for every cell that is added to the list */
          curr_wcdma_cell_list_index++;
        } /* end for (cell_index) */
      } /* num_cells > 0 */
      else if (fdd_info->num_cells == 0)
      {
        /* Rule 8a: If NR_OF_FDD_CELLS=0 and FDD_Indic0=0, this indicates the 3G Neighbour Cell
        list index for report on RSSI */
        /* Rule 8b: If NR_OF_FDD_CELLS is equal or greater than 17, this shall not be considered
        as an error; the corresponding index in the 3G Neighbour Cell list shall be incremented
        by one. */
        /* Rule 9: For NR_OF_FDD_CELLS=31 in SI2quater, no index shall be allocated in the
        3G Neighbour Cell list. */
        if (
             ((fdd_info->NR_OF_FDD_CELLS == 0) && (fdd_info->FDD_Indic0 == 0)) || // Rule 8a
             ((fdd_info->NR_OF_FDD_CELLS >= 17) && (fdd_info->NR_OF_FDD_CELLS <= 30)) || // Rule 8b
             ((fdd_info->NR_OF_FDD_CELLS == 31) && (source == SOURCE_MEAS_INFO)) // Rule 9
           )
        {
          wcdma_neighbor_cell_list_T *cell_list = &wcdma_neighbor_cell_list_ptr[curr_wcdma_cell_list_index];
          wcdma_cell_T *cell = &cell_list->cell;
          /* For frequency only cases indices are incremented only when the list is being created
          from the Measurement Information */
          cell->uarfcn = fdd_info->uarfcn;
          cell_list->frequency_only = FREQ_ONLY_UNREPORTED;
          cell_list->valid_data = TRUE;

          cell_list->rat_type = RR_UTRAN_FDD;

          /* increment curr_wcdma_cell_list_index */
          curr_wcdma_cell_list_index++;
        }
      } /* else if (fdd_info->num_cells == 0) */
    } /* for (rep_FDD_index = 0; rep_FDD_index < num_repeated_FDD_info; ++rep_FDD_index) */

#ifdef FEATURE_GSM_TDS
    /********************************************/
    /******** Handle TDD frequencies  ***********/
    /********************************************/

    /* for (each frequency F in I) */
    for (
          rep_TDD_index = 0;
          (rep_TDD_index < num_repeated_TDD_info) &&
          (curr_wcdma_cell_list_index < MAX_UTRAN_CELLS); /* Rule 1 & Rule 7 */
          ++rep_TDD_index
        )
    {
      /* temporary alias */
      TDD_Info_T *tdd_info = &instance->TDD_Info[rep_TDD_index];

      /* If TDD_Indic0 is set, the cell with cell parameter id 0 should be included for this frequency. */
      if (tdd_info->TDD_Indic0)
      {
        wcdma_neighbor_cell_list_T *cell_list = &wcdma_neighbor_cell_list_ptr[curr_wcdma_cell_list_index];
        wcdma_cell_T *cell = &cell_list->cell;

        cell->uarfcn = tdd_info->uarfcn;
        cell->diversity = FALSE;
        cell->scrambling_code = 0; /*cell parameter id*/
        cell->diversity = FALSE;

        /* REP_PRIORITY is received only in the measurement information */
        if (source == SOURCE_MEAS_INFO)
        {
          cell_list->MI_REP_PRIORITY = rr_get_rep_priority(instance->si2q_mi.mi_params.REP_PRIORITY,
            curr_wcdma_cell_list_index);
        }
        cell_list->frequency_only = FREQ_SC;
        cell_list->valid_data = TRUE;

        cell_list->rat_type = RR_UTRAN_TDD;

        curr_wcdma_cell_list_index++;
      }

      /* if (num of cells present for F is greater than 0 )*/
      if (tdd_info->num_cells > 0)
      {
        /* for (each cell Fsc associated with F ) */
        byte cell_index;
        byte num_cells = tdd_info->num_cells;

        for (
             cell_index = 1;
             (cell_index <= num_cells) &&
             (cell_index < MAX_UTRAN_CELLS_PER_INSTANCE) &&
             (curr_wcdma_cell_list_index < MAX_UTRAN_CELLS); /* Rule 7 */
             ++cell_index
            )
        {
          /* Rule 3: The tdd_info has decoded_and_sorted_cell_values */
          /* alias for source */
          word tds_cell = tdd_info->decoded_and_sorted_cell_values[cell_index];
          /* alias for destination */
          wcdma_neighbor_cell_list_T *cell_list = &wcdma_neighbor_cell_list_ptr[curr_wcdma_cell_list_index];
          wcdma_cell_T *cell = &cell_list->cell;

          /* copy */
          cell->uarfcn = tdd_info->uarfcn;

          cell->diversity = FALSE;

          /* find if the diversity (bit9) is set */
          if (tds_cell & 0x0100)
          {
            cell->diversity = TRUE;
          }

          /* The next  bit7 to bit1 are the cell parameter id */
          cell->scrambling_code = tds_cell & 0x007F;

          /* change to plain old SC */
          /*cell->scrambling_code <<= 4; Not required for TDS*/

          /* REP_PRIORITY is received only in the measurement information */
          if (source == SOURCE_MEAS_INFO)
          {
            cell_list->MI_REP_PRIORITY =
              rr_get_rep_priority(instance->si2q_mi.mi_params.REP_PRIORITY, curr_wcdma_cell_list_index);
          }

          cell_list->frequency_only = FREQ_SC;
          cell_list->valid_data = TRUE;

          cell_list->rat_type = RR_UTRAN_TDD;

          /* Rule 5 */
          /* indices are incremented for every cell that is added to the list */
          curr_wcdma_cell_list_index++;
        } /* end for (cell_index) */
      } /* num_cells > 0 */
      else if (tdd_info->num_cells == 0)
      {
        /*
        Rule 8)
        Rules for both SI2quater and Measurement Information (44.018 table 9.1.54.1b, 10.5.2.33b)
        a) If NR_OF_FDD_CELS =0 and FDD_Indic0=0, this indicates the 3G Neighbour Cell list index for report on RSSI, see 3GPP TS 05.08.
        b) If NR_OF_FDD_CELS is equal or greater than 17, this shall not be considered as an error; the corresponding index in the 3G
        Neighbour Cell list shall be incremented by one.
        c) If NR_OF_TDD_CELS is equal or greater than 21 OR NR_OF_TDD_CELS is equal to 0 and TDD_Indic0=0 this shall not be considered as an error; the corresponding index in the 3G
        Neighbour Cell list shall be incremented by one.

        Rule 9)
        Rule for SI2quater only
        If parameter NR_OF_TDD_CELS or NR_OF_FDD_CELS in table 9.1.54.1a. is equal to 31, this indicates that the corresponding UARFCN
        shall be included in the 3G Cell Reselection list (see clause 3.4.1.2.1.7); no index shall be
        allocated in the 3G Neighbour Cell list.

        */
        if (
             ((tdd_info->NR_OF_TDD_CELLS == 0) && (tdd_info->TDD_Indic0 == 0)) || // Rule 8c
             ((tdd_info->NR_OF_TDD_CELLS >= 21) && (tdd_info->NR_OF_TDD_CELLS <= 30)) || // Rule 8c
             ((tdd_info->NR_OF_TDD_CELLS == 31) && (source == SOURCE_MEAS_INFO)) // Rule 9
           )
        {
          wcdma_neighbor_cell_list_T *cell_list = &wcdma_neighbor_cell_list_ptr[curr_wcdma_cell_list_index];
          wcdma_cell_T *cell = &cell_list->cell;
          /* For frequency only cases indices are incremented only when the list is being created
          from the Measurement Information */
          cell->uarfcn = tdd_info->uarfcn;
          cell_list->frequency_only = FREQ_ONLY_UNREPORTED;
          cell_list->valid_data = TRUE;

          cell_list->rat_type = RR_UTRAN_TDD;

          /* increment curr_wcdma_cell_list_index */
          curr_wcdma_cell_list_index++;
        }
      } /* else if (tdd_info->num_cells == 0) */
    } /* for (rep_TDD_index = 0; rep_TDD_index < num_repeated_TDD_info; ++rep_TDD_index) */
#endif /*FEATURE_GSM_TDS*/
  }/* for (i = 0; i < MAX_INSTANCES; ++i) */
}

#ifdef FEATURE_SGLTE
#ifdef FEATURE_GSM_TDS
boolean rr_sglte_is_l1_tds_measurements_configured(const gas_id_t gas_id)
{
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  return (rr_general_data_ptr->rr_sglte_l1_tds_meas_configured);
}

void rr_sglte_set_l1_tds_measurements_configured(boolean is_configured, const gas_id_t gas_id)
{
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  rr_general_data_ptr->rr_sglte_l1_tds_meas_configured = is_configured;
}
#endif
#endif

#ifdef FEATURE_LTE
boolean rr_is_lte_l1_measurements_configured(const gas_id_t gas_id)
{
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  return (rr_general_data_ptr->rr_l1_lte_meas_configured);
}

void rr_set_lte_l1_measurements_configured(boolean is_configured, const gas_id_t gas_id)
{
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  rr_general_data_ptr->rr_l1_lte_meas_configured = is_configured;
}
#endif

#if defined FEATURE_WCDMA || defined FEATURE_GSM_TDS

void rr_set_num_uarfcns_sent_to_l1(
  uint8 num,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  rr_general_data_ptr->num_uarfcns_sent_to_l1 = num;
}

uint8 rr_get_num_uarfcns_sent_to_l1(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  return rr_general_data_ptr->num_uarfcns_sent_to_l1;
}

/**
 * Returns the number of unique ARFCNs present in the cell list
 * and frequency list fields of the message specified.
 */
uint8 rr_count_num_unique_uarfcns(
  mph_wcdma_cell_update_list_req_T *msg
)
{
#define MAX_FOUND_LIST_SIZE 16

  uint8 found_count = 0;
  uint16 found_list[MAX_FOUND_LIST_SIZE] = {0};
  uint32 i, j;

  for (i = 0; i < msg->wcdma_cell_list.num_of_cells; i++)
  {
    uint16 uarfcn = msg->wcdma_cell_list.cell_list[i].uarfcn;

    /* Check if this uarfcn is already in the found_list.  If it isn't, add it */
    for (j = 0; j < found_count && j < MAX_FOUND_LIST_SIZE; j++)
    {
      if (uarfcn == found_list[j])
      {
        break;
      }
    }

    if (j == found_count)
    {
      /* the frequency was not found in the list, so add it if there is space */
      if (found_count < MAX_FOUND_LIST_SIZE)
      {
        found_list[found_count] = uarfcn;
        found_count++;
      }
      else
      {
        MSG_GERAN_ERROR_0("Overflow in number of unique frequencies!");
      }
    }
  }

  for (i = 0; i < msg->wcdma_reselection_freq_list.num_of_frequencies; i++)
  {
    uint16 uarfcn = msg->wcdma_reselection_freq_list.uarfcn[i];

    for (j = 0; j < found_count; j++)
    {
      if (uarfcn == found_list[j])
      {
        break;
      }
    }

    if (j == found_count)
    {
      /* the frequency was not found in the list, so add it if there is space */
      if (found_count < MAX_FOUND_LIST_SIZE)
      {
        found_list[found_count] = uarfcn;
        found_count++;
      }
      else
      {
        MSG_GERAN_ERROR_0("Overflow in number of unique frequencies!");
      }
    }
  }

  return found_count;
}

void rr_set_num_uarfcns_sent_to_l1_in_dedicated(
  uint8 num,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  rr_general_data_ptr->num_uarfcns_sent_to_l1_in_dedicated = num;
}

uint8 rr_get_num_uarfcns_sent_to_l1_in_dedicated(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  return rr_general_data_ptr->num_uarfcns_sent_to_l1_in_dedicated;
}

/**
 * Returns the number of unique ARFCNs present in the cell list
 * and frequency list fields of the message specified.
 *  
 * LTE frequencies are also included in this calculation to 
 * ensure that W dedicated measurements are deleted with the 
 * correct period when L is also being measured. 
 *  
 */
uint8 rr_count_num_unique_uarfcns_in_dedicated(
  mph_dedicated_wcdma_cell_update_list_req_T *msg
)
{
#define MAX_FOUND_LIST_SIZE 16

  uint8 found_count = 0;
  uint16 found_list[MAX_FOUND_LIST_SIZE] = {0};
  uint32 i, j;

  /*Add in UTRAN FDD frequencies*/
  for (i = 0; i < msg->wcdma_cell_list.num_of_cells; i++)
  {
    uint16 uarfcn = msg->wcdma_cell_list.cell_list[i].uarfcn;

    /* Check if this uarfcn is already in the found_list.  If it isn't, add it */
    for (j = 0; j < found_count && j < MAX_FOUND_LIST_SIZE; j++)
    {
      if (uarfcn == found_list[j])
      {
        break;
      }
    }

    if (j == found_count)
    {
      /* the frequency was not found in the list, so add it if there is space */
      if (found_count < MAX_FOUND_LIST_SIZE)
      {
        found_list[found_count] = uarfcn;
        found_count++;
      }
      else
      {
        MSG_GERAN_ERROR_0("Overflow in number of unique frequencies!");
      }
    }
  }
#ifdef FEATURE_GSM_TDS
  /*Add in UTRAN TDD frequencies*/
  for (i = 0; i < msg->utran_tdd_cell_list.num_of_cells; i++)
  {
    uint16 uarfcn = msg->utran_tdd_cell_list.cell_list[i].uarfcn;

    /* Check if this uarfcn is already in the found_list.  If it isn't, add it */
    for (j = 0; j < found_count && j < MAX_FOUND_LIST_SIZE; j++)
    {
      if (uarfcn == found_list[j])
      {
        break;
      }
    }

    if (j == found_count)
    {
      /* the frequency was not found in the list, so add it if there is space */
      if (found_count < MAX_FOUND_LIST_SIZE)
      {
        found_list[found_count] = uarfcn;
        found_count++;
      }
      else
      {
        MSG_GERAN_ERROR_0("Overflow in number of unique frequencies!");
      }
    }
  }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
  if ( (msg->eutran_frequency_count > 0) &&
       (msg->eutran_frequency_count <= MAX_RR_L1_EUTRAN_FREQUENCIES) )
  {    
    /* LTE frequencies are all unique in the EUTRAN frequency list no need to check for double entries.*/
    /* LTE frequencies take RR_WCDMA_DED_MEAS_NUM_SACCH_BLOCKS_PER_LTE_FREQ longer than a single
       UTRAN UARFCN to scan*/
    found_count += (msg->eutran_frequency_count * RR_WCDMA_DED_MEAS_NUM_SACCH_BLOCKS_PER_LTE_FREQ);

    MSG_HIGH("%d EUTRAN frequencies added to frequency count: %d",
              msg->eutran_frequency_count, found_count,0
            );
  }
#endif /* FEATURE_LTE */

  return found_count;
}

#endif /* FEATURE_WCDMA */

boolean rr_is_first_received_si2ter_instance(
  si2ter_instances_T *instances
)
{
  return (instances->num_instances == 0);
}

/**
 * Update L1 with dedicated system information
 *
 * @param si_db_ptr Pointer to the system information database that should be used
 */
void rr_l1_update_dedicated_mode(
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
)
{
  inter_task_BCCH_allocation_T BCCH_allocation;
  boolean                      DTX_indicator;
  rr_ba_lists_t               *rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  RR_NULL_CHECK_FATAL(si_db_ptr);
  RR_ZERO_STRUCTURE(BCCH_allocation);

  rr_generate_complete_dedicated_BA_list(si_db_ptr, gas_id);

  rr_set_l1params_ba_list(
    &BCCH_allocation,
    &rr_ba_lists_ptr->complete_BA_sacch,
    si_db_ptr->BCCH_ARFCN,
    FALSE,
    gas_id
  );

  DTX_indicator = rr_get_DTX_indicator(gas_id);

  MSG_GERAN_HIGH_1_G("Dedicated L1 update: DTX=%d", DTX_indicator);

  /* send the dedicated update message to L1 */
  rr_send_mph_dedicated_update_req(
    &BCCH_allocation,
    DTX_indicator,
    si_db_ptr->cell_options.PWRC,
    0xFF,
    si_db_ptr->multiband_reporting,
    gas_id
  );

  /* As L1 was updated, reset the DTX override status */
  if (rr_general_data_ptr->rr_dtx_override_status == DTX_OVERRIDE_INACTIVE_UPDATE_L1_PENDING)
  {
    rr_dtx_override_reset(gas_id);
  }

} /* rr_l1_update_dedicated_mode */


/**
 * Resets the forbidden LAI list
 */
void rr_reset_forbidden_lai_list(const gas_id_t gas_id)
{
  int cell_index;

  // Obtain a pointer to the forbidden LAI list (stored by ASID)
  forbidden_lai_list_T *lai_list_ptr = rr_get_forbidden_lai_list_ptr(geran_map_gas_id_to_nas_id(gas_id));
  RR_NULL_CHECK_FATAL(lai_list_ptr);

  lai_list_ptr->no_of_entries = 0;

  for (cell_index = 0; cell_index < MAX_FORBIDDEN_LAIS; cell_index++)
  {
    lai_list_ptr->lai[cell_index].location_area_code = 0;
    lai_list_ptr->lai[cell_index].plmn_id.identity[0] = 0;
    lai_list_ptr->lai[cell_index].plmn_id.identity[1] = 0;
    lai_list_ptr->lai[cell_index].plmn_id.identity[2] = 0;
  }

  return;
} /* rr_reset_forbidden_lai_list */

/**
 * Called when the RR software is being reset.
 *
 * This function does not cancel/reset the RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER
 * as this needs to be kept running across GSM->WCDMA->GSM mode changes.
 *
 * This function does not cancel/reset the RR_DUAL_SIM_HEARTBEAT_TIMER
 * as this needs to be managed exclusively by the GRR Dual SIM control module
 */
static void rr_reset_most_timers(
  const gas_id_t gas_id
)
{
  rr_timer_id_T ignored_timers[5];
  int           num_entries;

  /* initialise the array */
  memset(ignored_timers, 0, sizeof(ignored_timers));

  /* create an array of timer IDs that should be left undisturbed */
  ignored_timers[0] = RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER;
  num_entries       = 1;

  ignored_timers[num_entries] = RR_INVALIDATE_PSCAN_RESULTS_TIMER;
  num_entries++;

  ignored_timers[num_entries] = RR_MM_ACTIVATION_RSP_TIMER;
  num_entries++;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* Multi-SIM timers are controlled externally */
  ignored_timers[num_entries] = RR_DUAL_SIM_HEARTBEAT_TIMER;
  num_entries++;
  ignored_timers[num_entries] = RR_ACQ_DUTY_CYCLE_TIMER;
  num_entries++;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  /* pass the array to a function that will reset all timers except these */
  rr_timer_reset_all(ignored_timers, num_entries, gas_id);

  return;
} /* rr_reset_most_timers */

/**
 * Reset the software ( at RR level) to initial conditions as much
 * as practical
 */
void rr_reset_software(
  const gas_id_t gas_id
)
{
  PLMN_id_T                      empty_plmn;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  gprs_scell_info_t             *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
  wcdma_ncell_list_data_t       *wcdma_ncell_list_data_ptr = rr_get_wcdma_ncell_list_data_ptr(gas_id);
#ifdef FEATURE_WCDMA
  surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  rr_ded_irat_measurements_t   *tds_ded_meas_ptr = rr_get_tds_ded_meas_ptr(gas_id);
#endif /*FEATURE_GSM_TDS*/

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  RR_NULL_CHECK_FATAL(scell_info_ptr);
  RR_NULL_CHECK_FATAL(pcell_info_ptr);
#ifdef FEATURE_WCDMA
  RR_NULL_CHECK_FATAL(wcdma_ncell_list_data_ptr);
  RR_NULL_CHECK_FATAL(wcdma_ded_meas_ptr);
#endif /* FEATURE_WCDMA */
  RR_ZERO_STRUCTURE(empty_plmn);

  MSG_GERAN_HIGH_0_G("Resetting RR software");

  /* Flush the pending message queue - any messages in here will be lost... */
  rr_flush_pending_messages(gas_id);

  /**
   * Initialise cell information for RR_SERVICE_IND/GRR_MAC_PS_ACCESS_REQ.
   */
  rr_mm_send_reset_last_cell_info(gas_id);
  rr_mac_send_reset_last_cell_info(gas_id);

  /*************************************************/
  /* Reinitialise the system_information_database  */
  /*************************************************/
  rr_clear_system_information(&scell_info_ptr->gsm);
  rr_clear_system_information(&pcell_info_ptr->gsm);

  rr_initialize_SI2q_instances(gas_id);
  rr_initialize_SI2ter_instances(gas_id);

  /*****************************************************************/
  /* Reset power scan, candidate cell, and available PLMN database */
  /*****************************************************************/
  rr_candidate_db_clear(rr_get_candidate_cell_db_ptr(gas_id));

  /* reset all RR timers (cancel in GS if running) */
  rr_reset_most_timers(gas_id);

  /* reset the surrounding database */
  rr_reset_surrounding_database(gas_id);

  /* reset the current band */
  rr_reset_current_band(gas_id);

  if (rr_l1_gsm_deact_reason_get(gas_id) != RR_L1_MODE_CHANGE)
  {
    /* reset the forbidden LAI list */
    rr_reset_forbidden_lai_list(gas_id);
  }

  if (!rr_log_reselection_log_complete(gas_id))
  {
    rr_log_reselect_failure(RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ, gas_id);
  }

  if (!rr_log_cell_selection_log_complete(gas_id))
  {
    rr_log_cell_selection_end(RR_LOG_CELL_SELECTION_END_ABORT_MM_REQUEST, gas_id);
  }

  (void) rr_selection_control(EV_RESET_SOFTWARE, NULL, gas_id);
  rr_reset_ded_meas_reporting(gas_id);
  rr_initialise_forbidden_cell_list(gas_id);

  (void) rr_cell_reest_control(EV_RESET_SOFTWARE, NULL, gas_id);
  (void) rr_connection_establishment_control(EV_RESET_SOFTWARE, NULL, gas_id);
  (void) rr_connection_release(EV_RESET_SOFTWARE, NULL, gas_id);
  (void) rr_mode_control(EV_RESET_SOFTWARE,NULL,gas_id);
  (void) rr_inter_cell_handover_control(EV_RESET_SOFTWARE,NULL,gas_id);
  (void) rr_plmn_list_control(EV_RESET_SOFTWARE,NULL, gas_id);
  (void) rr_handle_procedures(EV_RESET_SOFTWARE, NULL, gas_id);

  rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);
  rr_l1_gsm_deact_reason_set(RR_L1_MODE_CHANGE, gas_id);

  rr_initialize_wcdma_neighbor_cell_list(wcdma_ncell_list_data_ptr->wcdma_neighbor_cell_list);

#ifdef FEATURE_GSM_TDS
  rr_initialize_tds_ded_measurements(tds_ded_meas_ptr);
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
  rr_initialize_wcdma_ded_measurements(wcdma_ded_meas_ptr);
#endif /* FEATURE_WCDMA */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  rr_g2wr_reset_wcdma_surrounding_database(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
  rr_g2wr_cancel_all_wcdma_five_second_timers(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  rr_g2x_barred_frequency_db_clear(gas_id);
  rr_g2x_barred_cell_db_clear(gas_id);
  rr_g2x_cell_quality_data_clear(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
  (void) rr_g2w_handover_control(EV_RESET_SOFTWARE, NULL, gas_id);
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_CGPS_UMTS_CP_GSM
  rr_cgps_init(gas_id);
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

  rr_dtx_override_reset(gas_id);

  geran_eng_mode_data_write(
    ENG_MODE_ALL_INFORMATION,
    NULL,
    FALSE,
    gas_id
  );

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
  rr_set_rr_using_pseudo_si2q_data(FALSE, gas_id);
#endif


} /* rr_reset_software */


/**
 * Stores the forbidden LAI list
 *
 * @param forbidden_lai_update_req
 *               Message containing the new forbidden LAI list
 */
void rr_store_forbidden_lai_list(rr_forbidden_lai_update_req_T *forbidden_lai_update_req, const gas_id_t gas_id)
{
  int lai_index;
  int no_of_entries;
  forbidden_lai_list_T *lai_list_ptr;

  RR_NULL_CHECK_RETURN_VOID(forbidden_lai_update_req);

  // Obtain a pointer to the forbidden LAI list (stored by ASID)
  lai_list_ptr = rr_get_forbidden_lai_list_ptr(forbidden_lai_update_req->as_id);
  RR_NULL_CHECK_FATAL(lai_list_ptr);

  no_of_entries = forbidden_lai_update_req->forbidden_lai_list.no_of_entries;
  lai_list_ptr->no_of_entries = (byte) no_of_entries;

  for (lai_index = 0;
       (lai_index < no_of_entries) && (lai_index < MAX_FORBIDDEN_LAIS);
       lai_index++)
  {
    inter_task_lai_T *lai_ptr =
      &(forbidden_lai_update_req->forbidden_lai_list.location_area_identification[lai_index]);

    lai_list_ptr->lai[lai_index].plmn_id.identity[0] = lai_ptr->PLMN_id.identity[0];
    lai_list_ptr->lai[lai_index].plmn_id.identity[1] = lai_ptr->PLMN_id.identity[1];
    lai_list_ptr->lai[lai_index].plmn_id.identity[2] = lai_ptr->PLMN_id.identity[2];

    lai_list_ptr->lai[lai_index].location_area_code =
      (word) ((lai_ptr->location_area_code[0] * 256) + (lai_ptr->location_area_code[1]));
  }

  // reset the rest of the entries in the list to 0 (for unit test consistency)
  for (;lai_index < MAX_FORBIDDEN_LAIS;lai_index++)
  {
    lai_list_ptr->lai[lai_index].location_area_code = 0;
    lai_list_ptr->lai[lai_index].plmn_id.identity[0] = 0;
    lai_list_ptr->lai[lai_index].plmn_id.identity[1] = 0;
    lai_list_ptr->lai[lai_index].plmn_id.identity[2] = 0;
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  if (geran_is_multi_mode_gas_id(gas_id))
  {
    /* Delete any IRAT cells from the Forbidden LAI database if they are
    in an LAI that is no longer forbidden. */
    rr_g2x_forbidden_lai_list_updated(gas_id);
  }
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

  return;
} /* rr_store_forbidden_lai_list */


/**
 * Check the access to the given cell for the given establishment reason
 *
 * @param establishment_cause reason the cell is being accessed
 *
 * @param si_db_ptr           Pointer to system information database for the cell
 *
 * @return the type of access allowed for the cell for the given establishment cause
 */
access_result_T rr_check_access_class(
  establishment_cause_T  establishment_cause,
  PLMN_id_T  plmn_id,
  byte  access_control[2],
  boolean  cell_bar_access,
  boolean  cell_bar_qualify,
  const gas_id_t gas_id
)
{
  byte                 access_class_membership[2];
  access_result_T      access_result;
  boolean              normal_access_allowed;
  boolean              special_access_allowed;
  boolean              emergency_access_allowed;

  /* Assume no normal or special access to start with */
  normal_access_allowed = FALSE;
  special_access_allowed = FALSE;

  /*
  SIM/cell access control bit-mapping:

  octet 0         b8 b7 b6 b5 b4 b3 b2 b1
  access class =  15 14 13 12 11 10  9  8

  octet 1         b8 b7 b6 b5 b4 b3 b2 b1
  access class =   7  6  5  4  3  2  1  0
  */

  /* AC10 indicates if emegency calls are allowed */
  emergency_access_allowed = ((access_control[0] & 0x04) == 0);

  /* Get the access class membership from the SIM. This returns FALSE if not available */
  if (rr_siminfo_get_access_class(access_class_membership, sizeof(access_class_membership),gas_id))
  {
    boolean is_hplmn;
    sys_plmn_id_s_type hplmn_id;
    ms_operation_mode_T ms_operation_mode = rr_siminfo_get_ms_opmode(gas_id);

    /* Get the IMSI-derived HPLMN. Note: This takes no account of EHPLMNs */
#ifdef FEATURE_DUAL_SIM
    hplmn_id = reg_sim_per_subs_hplmn_get(geran_map_gas_id_to_nas_id(gas_id));
#else
    hplmn_id = reg_sim_hplmn_get();
#endif /* FEATURE_DUAL_SIM */

    /* If the two match, then the current cell belongs to the HPLMN */
    is_hplmn = sys_plmn_match(hplmn_id, plmn_id);

    /*
    Check for CELL TEST OPERATION special case first

    05.08:
    If all the following conditions are met, then the "Cell selection priority" and the
    "Status for cell reselection" shall be set to normal:

    - the cell belongs to the MS HPLMN;
    - the MS is in cell test operation mode;
    - the CELL_BAR_ACCESS is set to "1";
    - the CELL_BAR_QUALIFY is set to "0";
    - the Access Control class 15 is barred.

    It seems that under these conditions, network would be expecting UE to register on it.
    */
    if (is_hplmn &&
        (ms_operation_mode == CELL_TEST_OPERATION) &&
        (cell_bar_access == TRUE) &&
        (cell_bar_qualify == FALSE) &&
        ((access_control[0] & 0x80) == 0x80))
    {
      MSG_GERAN_MED_0_G("HPLMN = 1, CTO = 1, Cell Barred = 1, CBQ = 0, ACCESS CLASS 15 BARRED, allowing all calls");
      normal_access_allowed = TRUE;
      emergency_access_allowed = TRUE;
    }
    else
    {
      uint8 access_class[2];

      /* Find out what the overall access class is, by removing the classes in the SIM */
      /* Access Class Membership which are barred by the cell's Access Control */
      access_class[0] = (~access_control[0]) & access_class_membership[0];
      access_class[1] = (~access_control[1]) & access_class_membership[1];

      /* Check the Normal Access classes */

      /* Normal access is allowed for AC0-AC9 */
      normal_access_allowed = (access_class[1] != 0) ||
                              ((access_class[0] & 0x01) == 0x01) ||
                              ((access_class[0] & 0x02) == 0x02);

      /* Check the Special Access Classes */
      /* Note: Even if the checking of the Normal Access classes has determined that */
      /* access if allowed, it is still necessary to check the Special Access classes */
      /* as if one of these allows access, this also always Emergency Access which will */
      /* override the setting if barred in conjunction with Normal Access */

      /* AC11 and/or AC15 indicate that the HPLMN may be accessed */
      if ( ((access_class[0] & 0x08) == 0x08) ||
           ((access_class[0] & 0x80) == 0x80) )
      {
        #ifdef FEATURE_EQUIVALENT_HPLMN
        /* Make a new assessment of whether the cell belongs to the HPLMN, this time */
        /* including EHPLMNs */
        is_hplmn = reg_sim_is_ehplmn(plmn_id);
        #endif /* FEATURE_EQUIVALENT_HPLMN */

        special_access_allowed = is_hplmn;

        /* If this special access is ok, then emergency access is ok regardless of the */
        /* AC10 indication */
        emergency_access_allowed |= is_hplmn;
      }
      else
      /* AC12-AC14 indicate that any PLMN matching the home country may be accessed */
      /* (Home country is defined as a matching MCC) */
      if ( ((access_class[0] & 0x10) == 0x10) ||
           ((access_class[0] & 0x20) == 0x20) ||
           ((access_class[0] & 0x40) == 0x40) )
      {
        uint32 imsi_mcc, imsi_mnc, cell_mcc, cell_mnc;
        boolean is_home_country, unused_flag;

        sys_plmn_get_mcc_mnc(
          hplmn_id,
          &unused_flag,
          &unused_flag,
          &imsi_mcc,
          &imsi_mnc
        );

        sys_plmn_get_mcc_mnc(
          plmn_id,
          &unused_flag,
          &unused_flag,
          &cell_mcc,
          &cell_mnc
        );

        is_home_country = sys_mcc_same_country(imsi_mcc, cell_mcc);

        special_access_allowed = is_home_country;

        /* If this special access is ok, then emergency access is ok regardless of the */
        /* AC10 indication */
        emergency_access_allowed |= is_home_country;
      }
    }
  }
  else
  {
    if (establishment_cause == MM_RR_IMSI_DETACH)
    {
      normal_access_allowed = TRUE;
    }
  }

  if ((normal_access_allowed || special_access_allowed) &&
      emergency_access_allowed)
  {
    access_result = ALL_CALLS_ALLOWED;
  }
  else
  if (normal_access_allowed || special_access_allowed)
  {
    access_result = NORMAL_CALLS_ALLOWED;
  }
  else
  if (emergency_access_allowed)
  {
    access_result = EMERGENCY_CALLS_ALLOWED;
  }
  else
  {
    access_result = NO_CALLS_ALLOWED;
  }

  return access_result;
} /* rr_check_access_class */


access_result_T rr_check_access_class_for_w2g_oos_search(
  PLMN_id_T  plmn_id,
  byte  access_control[2],
  boolean  cell_bar_access,
  boolean  cell_bar_qualify,
  const gas_id_t gas_id
)
{
  return
    rr_check_access_class(
      MM_RR_LOCATION_UPDATE,
      plmn_id,
      access_control,
      cell_bar_access,
      cell_bar_qualify,
      gas_id
    );
}


/* Function to verify that the cell's Access Control parameters allow RACHing */
/* on this cell */
access_result_T rr_check_access_class_for_raching(
  establishment_cause_T establishment_cause,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  return
    rr_check_access_class(
      establishment_cause,
      scell_info_ptr->gsm.location_area_identification.plmn_id,
      scell_info_ptr->gsm.RACH_control_parameters.access_control,
      scell_info_ptr->gsm.RACH_control_parameters.CELL_BAR_ACCESS,
      scell_info_ptr->gsm.cell_selection_parameters.CELL_BAR_QUALIFY,
      gas_id
    );

} /* end rr_check_access_class_for_raching() */


/* Function to verify that the cell's Access Control parameters allow RACHing */
/* on this cell (for a TBF) */
access_result_T rr_check_access_class_for_raching_for_tbf(const gas_id_t gas_id)
{
  return rr_check_access_class_for_raching(MM_RR_MO_FULL_SPEECH, gas_id);

} /* end rr_check_access_class_for_raching_for_tbf() */


access_result_T rr_check_access_class_for_camping(
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
)
{
  return
    rr_check_access_class(
      MM_RR_MO_FULL_SPEECH,
      si_db_ptr->location_area_identification.plmn_id,
      si_db_ptr->RACH_control_parameters.access_control,
      si_db_ptr->RACH_control_parameters.CELL_BAR_ACCESS,
      si_db_ptr->cell_selection_parameters.CELL_BAR_QUALIFY,
      gas_id
    );

} /* end rr_check_access_class_for_camping() */


/**
 * Determines the cell access status for the given cell based on the
 * access class values in the SIM. Uses mobile originated call as the
 * access reason for the determination.
 *
 * @param si_db_ptr Pointer to system information database for the serving cell
 *
 * @return Cell access status for the cell
 */
mm_as_cell_access_status_T rr_get_cell_access_status(
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
)
{
  access_result_T access_result;
  mm_as_cell_access_status_T mm_as_access;

  /* check the access class of the given cell and map to appropriate MM type */
  access_result = rr_check_access_class_for_camping(si_db_ptr, gas_id);

  switch (access_result)
  {
    case NO_CALLS_ALLOWED:
      mm_as_access = MM_AS_ACCESS_NO_CALLS;
      break;

    case NORMAL_CALLS_ALLOWED:
      mm_as_access = MM_AS_ACCESS_NORMAL_ONLY;
      break;

    case EMERGENCY_CALLS_ALLOWED:
      mm_as_access = MM_AS_ACCESS_EMERGENCY_ONLY;
      break;

    case ALL_CALLS_ALLOWED:
      mm_as_access = MM_AS_ACCESS_ALL_CALLS;
      break;

    default:
      MSG_GERAN_ERROR_1_G("Unexpected value=%d for access result",(int) access_result);
      mm_as_access = MM_AS_ACCESS_ALL_CALLS;
      break;
  }

  return mm_as_access;
} /* rr_get_cell_access_status */

/*===========================================================================

FUNCTION  rr_lai_get_PLMN
Returns the PLMN portion of the given LAI in PLMN_id_T format

PARAMETERS

  lai - The LAI from which the PLMN should be extracted

Returns:
  PLMN_id_T - The PLMN extracted from the LAI

===========================================================================*/

PLMN_id_T rr_lai_get_PLMN(
  LAI_T lai
)
{
  return lai.plmn_id;
} // rr_lai_get_PLMN

/*===========================================================================

FUNCTION  rr_is_forbidden_LAI
Returns TRUE if the given LAI matches any of the LAIs in the forbidden LAI list

PARAMETERS
  lai_ptr - Pointer to the LAI to verify against forbidden LAI list

Returns:
  TRUE if LAI is forbidden, FALSE otherwise

===========================================================================*/

boolean rr_is_forbidden_LAI(LAI_T *lai_ptr, const gas_id_t gas_id)
{
  int index;
  int no_of_entries;
  forbidden_lai_list_T *lai_list_ptr;

  // Sanity check the input parameter
  RR_NULL_CHECK_RETURN_PARAM(lai_ptr, FALSE);

  // Obtain a pointer to the stored forbidden LAI list
  lai_list_ptr = rr_get_forbidden_lai_list_ptr(geran_map_gas_id_to_nas_id(gas_id));
  RR_NULL_CHECK_FATAL(lai_list_ptr);

  no_of_entries = lai_list_ptr->no_of_entries;

  for (index = 0; index < no_of_entries;++index)
  {
    // see if it matches any of the LAIs in the list
    if (rr_same_lai(lai_ptr,&(lai_list_ptr->lai[index])))
    {
      // they are a match, the LAI is forbidden
      return TRUE;
    }
  }

  return FALSE;
} // rr_is_forbidden_LAI

/**
  @brief Converts the given RXLEV value into absolute dBm units (positive).

  The range used is 111 to 48.  The mapping is based on that described in 3GPP
  45.008:

  RXLEV  0  =  111
  RXLEV  1  =  110
  RXLEV  2  =  109
        :
        :
  RXLEV  62  =  47
  RXLEV  63  =  48

  The mapping from dBm

  @param rxlev  RXLEV value to convert (must be in range 0-63)
  @return Absolute dBm value corresponding to given RXLEV
*/
byte rr_convert_rxlex_to_absolute_dBm(byte rxlev)
{
  /* since rxlev is byte, it has to be >= 0 */
  if (rxlev > 63)
  {
    MSG_GERAN_ERROR_1("Invalid signal strength=%d",(int) rxlev);
    rxlev = 63;
  }

  return (byte) ((byte) 111 - rxlev);
} // rr_convert_rxlex_to_absolute_dBm


/*!
 * \brief Sends MM or RRC L3 messages to L2.
 * 
 * \param rr_data_req (in)
 * \param gas_id (in)
 */
void rr_process_rr_data_req(rr_data_req_T *rr_data_req, const gas_id_t gas_id)
{
  byte protocol_discriminator;

  protocol_discriminator = rr_data_req->layer3_message[0] & 0x0F;

  if (protocol_discriminator == SMS_MESSAGES)
  {
    /********************************************/
    /* Pass the message to the SAPI3 controller */
    /********************************************/
    (void) rr_control_SAPI3_link(rr_data_req, gas_id);
  }
  else
  {
    (void) rr_send_dl_data_req(
      rr_data_req->layer3_message,
      rr_data_req->l3_message_length,
      SAPI0,
      DCCH,
      gas_id
    );
  }
}

/**
 * Determines the service capability of the given cell based on the
 * associated system information
 *
 * @param si_db_ptr Pointer to the system information store for the cell
 *
 * @return service capability of the cell
 */
sys_srv_capability_e_type rr_get_cell_service_capability(
  system_information_database_T *si_db_ptr
)
{
  /***********************************************************************************/
  /* service capability depends on the GPRS Indicator (packet service supported)     */
  /* assumes that voice service is always supported (no way to indicate otherwise)   */
  /***********************************************************************************/
  sys_srv_capability_e_type capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;

  ASSERT(si_db_ptr != NULL);

  /* if packet service is provided */
  if (si_db_ptr->gprs_indicator == TRUE)
  {
    // both voice and packet
    capability = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
  }
  else // packet service not supported
  {
    /* voice is supported by default */
    capability = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
  }

  return capability;
} /* rr_get_cell_service_capability */

/**
 * determines if the given channel type is currently supported
 *
 * @param channel_type channel type value
 * @param gas_id
 *
 * @return TRUE if the channel type is supported, FALSE otherwise
 */
boolean rr_channel_type_supported(
  channel_type_T channel_type,
  const gas_id_t gas_id
)
{
  byte rr_ms_capability;

  /* query the current capabilities */
  rr_get_ms_capabilities(&rr_ms_capability, NULL, gas_id);

  // Check that the channel_type given is compatible with the MS capability.
  // Note: It is allowable to receive FULL RATE TRAFFIC if the MS capability is advertised as SDCCH_ONLY

  if ((channel_type == HALF_RATE_TRAFFIC) && (rr_ms_capability != DUAL_RATE_SUPPORTED))
  {
    return(FALSE);
  }

  return(TRUE);
} /* rr_channel_type_supported */


/**
 * Checks if the frequency list contains arfcn 0, if it does
 * moves it to the back of the list
 *
 * @param original_list_ptr pointer to a word array, with sorted entries
 *
 * @param length of original list
 *
 * @param modified_list_ptr pointer to modified list
 *
 */
void rr_move_zero_in_frequency_list(
  ARFCN_T*    original_list_ptr,
  byte     length,
  ARFCN_T*    modified_list_ptr
)
{
  int offset=0;
  int index;


  /*************************************************/
  /* Check if arfcn zero is present and if we need */
  /* to move it, if we do adjust the first entry   */
  /*************************************************/
  if (
       length>1 &&
       original_list_ptr[0].num == 0
     )
  {
    /* We need to move the zero entry to the back of the list */
    modified_list_ptr[length-1] = original_list_ptr[0];
    offset=1;
  }
  else if (length>0)
  {
    /* Since arfcn zero is not present the last entry is the same */
    modified_list_ptr[length-1]=original_list_ptr[length-1];
  }

  /* Copy over the rest of the array */
  for (index = 0; index < length-1; index++)
  {
    modified_list_ptr[index] = original_list_ptr[index+offset];
  }

} /* rr_move_zero_in_frequency_list */




/**
 * Sets the frequency list in the channel_info_ptr to the appropriate values
 * using the mobile allocation if the channel is a hopping channel.
 *
 * If not, it is assumed that the frequency list already contains the single
 * entry corresponding to the channel's frequency
 *
 * @param channel_info_ptr pointer to the channel information structure
 *
 * @param mobile_allocation_ptr
 *                         pointer to the mobile allocation IE
 *
 * @param cell_channel_description
 *                         cell_channel_description associated with the mobile allocation IE
 *
 * @return TRUE if the list was set, FALSE if there was an error while setting the list
 */
boolean rr_set_channel_frequency_list_from_mobile_allocation(
  channel_information_T       *channel_info_ptr,
  mobile_allocation_T         *mobile_allocation_ptr,
  cell_channel_description_T  *cell_channel_description_ptr,
  const gas_id_t               gas_id
)
{
  byte index;
  byte ca_index;
  ARFCN_T channel_number;
  cell_channel_description_T cell_allocation_temp;
  byte no_of_entries;
  boolean result = TRUE;
  rr_internal_band_T band = BAND_NONE;

  ASSERT(channel_info_ptr != NULL);
  ASSERT(mobile_allocation_ptr != NULL);
  ASSERT(cell_channel_description_ptr != NULL);

  cell_allocation_temp = *cell_channel_description_ptr;
  no_of_entries = mobile_allocation_ptr->no_of_entries;

  /* Validate that no_of_entries is in the correct range. */
  if (no_of_entries > CA_MAX_LEN)
  {
    no_of_entries = CA_MAX_LEN;
  }

  /*****************************************************************/
  /* Only need to get frequency list from mobile allocation if the */
  /* channel is a hopping channel                                  */
  /*****************************************************************/
  if (channel_info_ptr->hopping_flag == TRUE)
  {
    /********************************************************************************/
    /* Our cell allocation list has arfcn stored in increasing order. 4.08 10.5.2.1 */
    /* says we should make an exception for arfcn 0 which should be stored at the   */
    /* the end of the list.                                                         */
    /********************************************************************************/
    channel_info_ptr->frequency_list.no_of_items = no_of_entries;

    rr_move_zero_in_frequency_list(
      cell_channel_description_ptr->channel_number,
      MIN(cell_channel_description_ptr->no_of_entries,CA_MAX_LEN),
      cell_allocation_temp.channel_number
    );

    /* for each frequency index in the mobile allocation */
    for (index = 0; index < no_of_entries; index++)
    {
      /* Get the cell allocation index */
      ca_index = (byte) (mobile_allocation_ptr->MA_C[index] - 1);

      /****************************************************************************/
      /* get the associated channel number from the cell_channel_description_ptr  */
      /****************************************************************************/
      if (ca_index < cell_allocation_temp.no_of_entries)
      {
        channel_number = cell_allocation_temp.channel_number[ca_index];
      }
      else
      {
        /* error in the mobile allocation */
        MSG_GERAN_ERROR_2_G("CAIndex=%d out of range=%d",(int) ca_index, (int) cell_allocation_temp.no_of_entries);
        return FALSE;
      }

      /* store it in the channel description's frequency list */
      channel_info_ptr->frequency_list.channel[index] = channel_number;

      /*******************************************************/
      /* Must check the band the channel is in for multiband */
      /*******************************************************/
      if (band == BAND_NONE)
      {
        /********************************************************************/
        /* First time through the loop, get the band of the current channel */
        /********************************************************************/
        band = GET_ARFCN_BAND(channel_number);

        if (band == BAND_NONE)
        {
          /************************************************/
          /* This band is not suported, reject assignment */
          /************************************************/
          result = FALSE;
          MSG_GERAN_ERROR_1_G("Band not supported for chan: %d",(int) channel_number.num);

          /* exit from loop, no point in continuing since IA will be aborted */
          break;
        }
      }
      else /* not first time through loop */
      {
        /*****************************************************/
        /* For subsequent iterations, check that all entries */
        /* are in the same band                              */
        /*****************************************************/
        if (!rr_check_in_band(band,channel_number.num, gas_id))
        {
          /*****************************************************/
          /* this entry is not in the same band, so connection */
          /* establishment failure has occurred in RR          */
          /*****************************************************/
          result = FALSE;
          MSG_GERAN_ERROR_2_G("Chan=%d, Band=%d: Chan in other band",(int) channel_number.num, (int) band);
          break;
        }
      }
    } /* for each entry in mobile allocation */

    if ((result == TRUE) && (no_of_entries > 1))
    {
      /***************************************************************************/
      /* Check for arfcn 0 it is stored in the back of the cell allocation list   */
      /* it needs to be put in the front when given to L1 5.02 section 6.2.3      */
      /****************************************************************************/
      if (channel_info_ptr->frequency_list.channel[no_of_entries - 1].num == 0)
      {
         uint32 ch_index;

         for (ch_index = no_of_entries - 1; ch_index > 0; --ch_index)
         {
           channel_info_ptr->frequency_list.channel[ch_index] = channel_info_ptr->frequency_list.channel[ch_index-1];
         }

         channel_info_ptr->frequency_list.channel[0].num = 0;
      }
    }
  } /* channel's hopping flag is true */
  else
  {
    /**************************************************************/
    /* No hoppping, channel to use is in first entry of           */
    /* channel_info's frequency_list, with no_of_items            */
    /* set to 1. This was set up when the channel description was */
    /* decoded                                                    */
    /**************************************************************/

    /* First, check if channel_info_ptr->frequency_list.no_of_items !=1  */
    /*    then, it is equal to 0, which is an error                      */
    if (channel_info_ptr->frequency_list.no_of_items != 1)
    {
       /* ASSERT(channel_info_ptr->frequency_list.no_of_items == 1); */

       MSG_GERAN_ERROR_1_G("Number of frequencies in frequency_list is: %d",
                    (int) channel_info_ptr->frequency_list.no_of_items);
       result = FALSE;
    }  /* if (channel_info_ptr->frequency_list.no_of_items  */
    else
    {
      /*******************************************************/
      /* check that requested channel is in a supported band */
      /*******************************************************/
      channel_number = channel_info_ptr->frequency_list.channel[0];

      if (GET_ARFCN_BAND(channel_number) == BAND_NONE)
      {
        result = FALSE;
        MSG_GERAN_ERROR_1_G("Band not supported for chan: %d",(int) channel_number.num);
      }
    } /* else channel_info_ptr->frequency_list.no_of_items is equal to 1 */
  } /* channel's hopping flag is false */

  return result;
} /* rr_set_channel_frequency_list_from_mobile_allocation */

/**
 * checks the given frequency list to verify that all frequencies are supported
 * and belong to the same band
 *
 * @param frequency_list_ptr pointer to frequency list
 *
 * @return TRUE if frequency list checks out, FALSE otherwise
 */
static boolean rr_check_frequency_list_supported(
  frequency_list_T *frequency_list_ptr,
  const gas_id_t gas_id
)
{
  int index;
  int no_of_items;
  rr_internal_band_T band = BAND_NONE;

  ASSERT(frequency_list_ptr != NULL);

  no_of_items = frequency_list_ptr->no_of_items;

  /* check each entry in the frequency list */
  for (index=0; index < no_of_items;++index)
  {
    ARFCN_T channel_number = frequency_list_ptr->channel[index];

    /* if the first band has not been established */
    if (band == BAND_NONE)
    {
      /* get the band of the first channel */
      band = GET_ARFCN_BAND(channel_number);

      /* if the channel is not in a supported band, just return false */
      if (band == BAND_NONE)
      {
        MSG_GERAN_ERROR_1_G("Frequency=%d not implemented",(int) channel_number.num);
        return FALSE;
      }
    }
    else /* otherwise, the band of the frequency list has been established, all others should match */
    {
      if (rr_check_in_band(band,channel_number.num, gas_id) == FALSE)
      {
        MSG_GERAN_ERROR_2_G("Frequency=%d not in band=%d",(int) channel_number.num,(int) band);
        /* the frequency is not in the same band */
        return FALSE;
      }
    }
  } /* foreach frequency in the list */

  /* all frequencies check out */
  return TRUE;
} /* rr_check_frequency_list_supported */

/**
 * checks all the frequency lists in the given channel spec to verify that they are all
 * in supported bands and that all the frequencies in a given list belong to the
 * same band
 *
 * @param channel_spec_ptr pointer to the channel_spec that should be checked
 *
 * @return TRUE if all the frequencies check out, FALSE otherwise
 */
boolean rr_check_frequency_lists_supported(
  channel_spec_T *channel_spec_ptr,
  const gas_id_t gas_id
)
{
  /* Check the channel_info_1_after */
  if (rr_check_frequency_list_supported(
        &(channel_spec_ptr->channel_info_1_after.frequency_list),
        gas_id
      ) == FALSE)
  {
    MSG_GERAN_HIGH_0_G("Channel1After freq. list not supported");
    return FALSE;
  }

  /* check channel_info_2_after (if required) */
  if (channel_spec_ptr->number_of_ded_channels == 2)
  {
    if (rr_check_frequency_list_supported(
          &(channel_spec_ptr->channel_info_2_after.frequency_list),
          gas_id
        ) == FALSE)
    {
      MSG_GERAN_HIGH_0_G("Channel2After freq. list not supported");
      return FALSE;
    }
  }

  /* check channel infos before (if required) */
  if (channel_spec_ptr->starting_time_valid == TRUE)
  {
    /* Check the channel_info_1_before */
    if (rr_check_frequency_list_supported(
          &(channel_spec_ptr->channel_info_1_before.frequency_list),
          gas_id
        ) == FALSE)
    {
      MSG_GERAN_HIGH_0_G("Channel1Before freq. list not supported");
      return FALSE;
    }

    /* check channel_info_2_before (if required) */
    if (channel_spec_ptr->number_of_ded_channels == 2)
    {
      if (rr_check_frequency_list_supported(
            &(channel_spec_ptr->channel_info_2_before.frequency_list),
            gas_id
          ) == FALSE)
      {
        MSG_GERAN_HIGH_0_G("Channel2Before freq. list not supported");
        return FALSE;
      }
    }
  }

  /* all frequency lists check out */
  return TRUE;
} /* rr_check_frequency_lists_supported */

boolean rr_check_assignment_frequency_lists_supported(
  assignment_details_T *assignment_details_ptr,
  const gas_id_t gas_id
)
{
  /* Check the channel_info_1_after */
  if (rr_check_frequency_list_supported(
        &assignment_details_ptr->channel_info_1_after.frequency_list,
        gas_id
      ) == FALSE)
  {
    MSG_GERAN_HIGH_0_G("Channel1After freq. list not supported");
    return FALSE;
  }

  /* check channel_info_2_after (if required) */
  if (assignment_details_ptr->number_of_ded_channels == 2)
  {
    if (rr_check_frequency_list_supported(
          &assignment_details_ptr->channel_info_2_after.frequency_list,
          gas_id
        ) == FALSE)
    {
      MSG_GERAN_HIGH_0_G("Channel2After freq. list not supported");
      return FALSE;
    }
  }

  /* check channel infos before (if required) */
  if (assignment_details_ptr->starting_time_valid)
  {
    /* Check the channel_info_1_before */
    if (rr_check_frequency_list_supported(
          &assignment_details_ptr->channel_info_1_before.frequency_list,
          gas_id
        ) == FALSE)
    {
      MSG_GERAN_HIGH_0_G("Channel1Before freq. list not supported");
      return FALSE;
    }

    /* check channel_info_2_before (if required) */
    if (assignment_details_ptr->number_of_ded_channels == 2)
    {
      if (rr_check_frequency_list_supported(
            &assignment_details_ptr->channel_info_2_before.frequency_list,
            gas_id
          ) == FALSE)
      {
        MSG_GERAN_HIGH_0_G("Channel2Before freq. list not supported");
        return FALSE;
      }
    }
  }

  /* all frequency lists check out */
  return TRUE;
} /* rr_check_assignment_frequency_lists_supported */

/**
 * Used when sorting cells based on rxlev.
 *
 * @param arg1   First freq
 * @param arg2   Second freq
 *
 * @return <0 if Second freq < First freq
 *         >0 if Second freq > First freq
 *         0 if equal
 */
int rr_compare_freq_power( const void *arg1, const void *arg2 )
{
  return ( ((rr_l1_freq_power_T*)arg2)->RXLEV_average - ((rr_l1_freq_power_T*)arg1)->RXLEV_average );
} /* rr_compare_freq_power */


/*****************************/
/* BA list related functions */
/*****************************/

/**
 * Resets all the idle BA lists (and associated flags) on the given database
 *
 * @param si_db_ptr pointer to the SIDB to reset
 */
void rr_reset_idle_BA_lists(
  system_information_database_T *si_db_ptr
)
{
  if (si_db_ptr == NULL)
  {
    MSG_GERAN_ERROR_0("si_db_ptr = NULL");
    return;
  }

  memset(&si_db_ptr->complete_BA_list, 0, sizeof(BCCH_frequency_list_T));
  memset(&si_db_ptr->SI2_BA_list, 0, sizeof(BCCH_frequency_list_T));
  memset(&si_db_ptr->SI2bis_BA_list, 0, sizeof(BCCH_frequency_list_T));
  memset(&si_db_ptr->SI2ter_BA_list, 0, sizeof(BCCH_frequency_list_T));

  si_db_ptr->complete_idle_BA_list_received = FALSE;
  si_db_ptr->decode_SI2bis  = RR_SI_REQUIRE_UNKNOWN;
  si_db_ptr->decode_SI2ter  = RR_SI_REQUIRE_UNKNOWN;

} /* rr_reset_idle_BA_lists */

/**
 * Sets the destination BA list from the source BA list
 *
 * @param dest_ba_list_ptr pointer to destination BA list
 *
 * @param src_ba_list_ptr  pointer to source BA list
 */
void rr_set_BA_list(BCCH_frequency_list_T *dest_ba_list_ptr, BCCH_frequency_list_T *src_ba_list_ptr)
{
  *dest_ba_list_ptr = *src_ba_list_ptr;
} /* rr_set_BA_list */

/**
 * Generates the complete BA list using the information in the various
 * BA lists
 *
 * @param si_db_ptr pointer to the SIDB where the complete BA list should be generated
 * @param rcvd_si_mask indication of any SI just received, to be used for the new list
 */
void rr_generate_complete_idle_BA_list(
  system_information_database_T *si_db_ptr,
  uint16 rcvd_si_mask,
  const gas_id_t gas_id
)
{
  BCCH_frequency_list_T  empty_ba_list;
  boolean                camped_on_flag;
  gprs_scell_info_t     *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  memset(&empty_ba_list,0,sizeof(BCCH_frequency_list_T));

  if (ARFCNS_EQUAL(si_db_ptr->BCCH_ARFCN,scell_info_ptr->gsm.BCCH_ARFCN))
  {
    camped_on_flag = TRUE;
  }
  else
  {
    camped_on_flag = FALSE;
  }

  MSG_GERAN_MED_1_G("Generating complete BA list (%1d)",(int)camped_on_flag);

  /* clear the BA list */
  si_db_ptr->complete_BA_list = empty_ba_list;

  /* copy over the SI2 BA list */
  rr_add_ba_list(&si_db_ptr->complete_BA_list,&si_db_ptr->SI2_BA_list, gas_id);

  /* if SI2bis is required (or the override parameter is set) and has been received */
  if (((si_db_ptr->decode_SI2bis == RR_SI_REQUIRED) || (rcvd_si_mask & MASK_SI_2_BIS)) &&
      (ARE_SET(MASK_SI_2_BIS,si_db_ptr->update_message_received))
     )
  {
    /* copy over the SI2bis BA list */
    MSG_GERAN_MED_1_G("Adding SI2bis to BA list (override = %d)",rcvd_si_mask);
    rr_add_ba_list(&si_db_ptr->complete_BA_list,&si_db_ptr->SI2bis_BA_list, gas_id);
  }

  /* if SI2ter is required (or the override parameter is set) and has been received */
  if (((si_db_ptr->decode_SI2ter == RR_SI_REQUIRED) || (rcvd_si_mask & MASK_SI_2_TER)) &&
      (ARE_SET(MASK_SI_2_TER,si_db_ptr->update_message_received))
     )
  {
    MSG_GERAN_MED_1_G("Adding SI2ter to BA list (override = %d)",rcvd_si_mask);

    /* SI2ter BA list should always be concatentated on to the end of the
    SI2/2bis list WITHOUT any further sorting. */
    rr_add_ba_list_without_sorting(&si_db_ptr->complete_BA_list,&si_db_ptr->SI2ter_BA_list);
  }

#ifdef FEATURE_GERAN_HST_OPT

  if (rr_high_speed_train_opt_allowed(gas_id))
  {  
    ARFCN_T               arfcn;
    rr_irat_acq_db_t     *irat_acq_db_ptr;
    uint8                 i;
    BCCH_frequency_list_T irat_acq_db_freq_list;

    irat_acq_db_ptr = rr_irat_acq_db_get_data_ptr();

    RR_NULL_CHECK_FATAL(irat_acq_db_ptr);

    irat_acq_db_freq_list.no_of_entries = 0;
    irat_acq_db_freq_list.BA_IND = si_db_ptr->complete_BA_list.BA_IND;

    /* make up a freq list */
    for ( i=0; i < irat_acq_db_ptr->freq_list.num_arfcns; i++ )
    {
      arfcn = irat_acq_db_ptr->freq_list.arfcn[i];

      if ( rr_ds_is_supported_band((rr_internal_band_T)arfcn.band, RR_GAS_ID_TO_AS_ID) )
      {
        MSG_GERAN_HIGH_1_G("Adding IRAT acq DB ARFCN %d to the idle BA list",arfcn.num);
        irat_acq_db_freq_list.BCCH_ARFCN[irat_acq_db_freq_list.no_of_entries] = arfcn;
        irat_acq_db_freq_list.no_of_entries++;
      }
    }

    rr_add_ba_list_without_sorting(&si_db_ptr->complete_BA_list,&irat_acq_db_freq_list);
  }
#endif /*FEATURE_GERAN_HST_OPT*/

  /* set the BA_IND of the complete BA list to match SI2 BA_IND */
  si_db_ptr->complete_BA_list.BA_IND = si_db_ptr->SI2_BA_list.BA_IND;

  /* make sure that the ARFCNs are updated with correct band information */
  rr_gprs_reassign_band(IDLE_BA_FREQ_LIST, camped_on_flag, gas_id);
} /* rr_generate_complete_idle_BA_list */

/**
 *
 * @param si_db_ptr pointer to system information database
 *
 * @return TRUE if the database has valid data and a complete dedicated BA list
 */
boolean rr_has_complete_dedicated_BA_list(
  system_information_database_T *si_db_ptr
)
{
  return (si_db_ptr->valid_dedicated_data_flag && si_db_ptr->complete_BA_sacch_received);
} /* rr_has_complete_dedicated_BA_list */

/**
 * Resets all the dedicated BA lists (and associated flags) on the given database
 *
 * @param si_db_ptr pointer to the SIDB to reset
 */
void rr_reset_dedicated_BA_lists(
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
)
{
  rr_ba_lists_t *rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

  RR_NULL_CHECK_FATAL(si_db_ptr);
  RR_NULL_CHECK_FATAL(rr_ba_lists_ptr);

  memset(&rr_ba_lists_ptr->complete_BA_sacch, 0, sizeof(BCCH_frequency_list_T));
  si_db_ptr->complete_BA_sacch_received = FALSE;
  si_db_ptr->decode_SI5bis  = RR_SI_REQUIRE_UNKNOWN;
  si_db_ptr->decode_SI5ter  = RR_SI_REQUIRE_UNKNOWN;
  memset(&rr_ba_lists_ptr->SI5_BA_list, 0, sizeof(BCCH_frequency_list_T));
  memset(&rr_ba_lists_ptr->SI5bis_BA_list, 0, sizeof(BCCH_frequency_list_T));
  memset(&rr_ba_lists_ptr->SI5ter_BA_list, 0, sizeof(BCCH_frequency_list_T));

} /* rr_reset_dedicated_BA_lists */

/**
 * Generates the complete BA list using the information in the various
 * BA lists
 *
 * @param si_db_ptr pointer to the SIDB where the complete BA list should be generated
 */
void rr_generate_complete_dedicated_BA_list(
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
)
{
  boolean        old_si5_ba_lists_valid = rr_get_status_of_old_cell_si5_ba_lists(gas_id);
  rr_ba_lists_t *rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

  MSG_GERAN_MED_0_G("Generating complete dedicated BA list");

  /* clear the BA list */
  memset(&rr_ba_lists_ptr->complete_BA_sacch, 0, sizeof(BCCH_frequency_list_T));

  /* Use SI5 from the current cell if it has been received, otherwise use the
  old one if available. */
  if (ARE_SET(MASK_SI_5, si_db_ptr->update_message_received))
  {
    MSG_GERAN_MED_0_G("Adding new SI5 to BA list");
    rr_add_ba_list(&rr_ba_lists_ptr->complete_BA_sacch, &rr_ba_lists_ptr->SI5_BA_list, gas_id);
  }
  else if (old_si5_ba_lists_valid)
  {
    BCCH_frequency_list_T old_list = rr_get_old_si5_ba_list(MASK_SI_5, gas_id);
    /* We do not want to put an old BA_IND in the BA list as it might trigger
    detection of a BA_IND change which will cause problems. */
    old_list.BA_IND = rr_ba_lists_ptr->complete_BA_sacch.BA_IND;

    MSG_GERAN_MED_0_G("Adding old SI5 to BA list");
    rr_add_ba_list(&rr_ba_lists_ptr->complete_BA_sacch, &old_list, gas_id);
  }

  /* Use SI5bis from the current cell if it has been received, otherwise use the
  old one if available. */
  /* Need to check the 'required' flag as well as the SI5bis bit in the bitmask
  as if the SI is not required it is marked as received in rr_sys_info.c */
  if (ARE_SET(MASK_SI_5_BIS, si_db_ptr->update_message_received) &&
      si_db_ptr->decode_SI5bis == RR_SI_REQUIRED)
  {
    MSG_GERAN_MED_0_G("Adding new SI5bis to BA list");
    rr_add_ba_list(&rr_ba_lists_ptr->complete_BA_sacch, &rr_ba_lists_ptr->SI5bis_BA_list, gas_id);
  }
  else if (old_si5_ba_lists_valid)
  {
    BCCH_frequency_list_T old_list = rr_get_old_si5_ba_list(MASK_SI_5_BIS, gas_id);

    /* We do not want to put an old BA_IND in the BA list as it might trigger
    detection of a BA_IND change which will cause problems. */
    old_list.BA_IND = rr_ba_lists_ptr->complete_BA_sacch.BA_IND;

    MSG_GERAN_MED_0_G("Adding old SI5bis to BA list");
    rr_add_ba_list(&rr_ba_lists_ptr->complete_BA_sacch, &old_list, gas_id);
  }

  /* if SI5ter has been received (NOTE: no check for required because it is optional) */
  if (ARE_SET(MASK_SI_5_TER, si_db_ptr->update_message_received))
  {
    MSG_GERAN_MED_0_G("Adding new SI5ter to BA list");

    /* SI5ter BA list should always be concatentated on to the end of the
    SI5/5bis list WITHOUT any further sorting. */
    rr_add_ba_list_without_sorting(&rr_ba_lists_ptr->complete_BA_sacch, &rr_ba_lists_ptr->SI5ter_BA_list);
  }
  else
  {
    if (old_si5_ba_lists_valid)
    {
      BCCH_frequency_list_T old_list = rr_get_old_si5_ba_list(MASK_SI_5_TER, gas_id);

      /* We do not want to put an old BA_IND in the BA list as it might trigger
      detection of a BA_IND change which will cause problems. */
      old_list.BA_IND = rr_ba_lists_ptr->complete_BA_sacch.BA_IND;

      MSG_GERAN_MED_0_G("Adding old SI5ter to BA list");

      rr_add_ba_list_without_sorting(&rr_ba_lists_ptr->complete_BA_sacch,&old_list);
    }
  }

  /**
   * Construct the BA lists used for Measurement Reporting if all all SI5
   * variants have been received.
   */
  if (si_db_ptr->complete_BA_sacch_received)
  {
    /* Add the SI5 BA list */
    rr_ba_lists_ptr->SI5_SI5bis_measurement_report_BA_list = rr_ba_lists_ptr->SI5_BA_list;

    /* Add the SI5bis BA list if received/required */
    if (ARE_SET(MASK_SI_5_BIS, si_db_ptr->update_message_received) &&
        si_db_ptr->decode_SI5bis == RR_SI_REQUIRED)
    {
      rr_add_ba_list(
        &rr_ba_lists_ptr->SI5_SI5bis_measurement_report_BA_list,
        &rr_ba_lists_ptr->SI5bis_BA_list,
        gas_id
      );
    }

    /* Add the SI5ter BA list if received */
    if (ARE_SET(MASK_SI_5_TER, si_db_ptr->update_message_received))
    {
      rr_ba_lists_ptr->SI5ter_measurement_report_BA_list = rr_ba_lists_ptr->SI5ter_BA_list;
    }
    /* Otherwise, make sure that it is cleared. */
    else
    {
      memset(&rr_ba_lists_ptr->SI5ter_measurement_report_BA_list, 0, sizeof(BCCH_frequency_list_T));
    }
  }
} /* rr_generate_complete_dedicated_BA_list */

/**
 * Generates a ba list in the format specified in 31.111 section 8.29. Used
 * to generate a ba list for USAT
 *
 * @param bcch_list - ptr where ba list should be copied to
 * @param in_dedicated - TRUE if we are in dedicated mode, used to determine
 *   which ba lists we should use, SI2 or SI5
 */
static void rr_get_ba_list_for_measurement_report(
  rr_bcch_list_type *bcch_list,
  boolean in_dedicated,
  const gas_id_t gas_id
)
{
  ARFCN_T                             modified_list1[BA_MAX_LEN];
  ARFCN_T                             modified_list2[BA_MAX_LEN];
  usat_system_information_database_T *usat_system_info_database_ptr;
  BCCH_frequency_list_T              *first_list;
  BCCH_frequency_list_T              *second_list;

  usat_system_info_database_ptr = rr_get_usat_system_information_database_ptr(gas_id);

  first_list = &usat_system_info_database_ptr->SIbis_measurement_report_BA_list;
  second_list = &usat_system_info_database_ptr->SIter_measurement_report_BA_list;

  /*********************************************************************/
  /* We need to move index 0 to the back of the list so the indexes in */
  /* the measurement report correspond to those in the ba list         */
  /*********************************************************************/
  rr_move_zero_in_frequency_list(
    first_list->BCCH_ARFCN,
    MIN(first_list->no_of_entries,BA_MAX_LEN),
    modified_list1
  );

  rr_move_zero_in_frequency_list(
    second_list->BCCH_ARFCN,
    MIN(second_list->no_of_entries,BA_MAX_LEN),
    modified_list2
  );

  if ((first_list->no_of_entries+second_list->no_of_entries)>BA_MAX_LEN)
  {
    MSG_GERAN_ERROR_2_G("BA list too large len%d ded%d",bcch_list->length,in_dedicated);
    bcch_list->length = 0;
  }
  else
  {
    /*Start encoding bcch list*/
    gs_bit_stream_T bitstream;
    int index;

    /* Initialize the list to 0 */
    memset(bcch_list->bcch_list,0,RR_USAT_SIZE_BCCH_LIST);

    gs_bit_stream_initialize(&bitstream, bcch_list->bcch_list, RR_USAT_SIZE_BCCH_LIST);

    /* Store list 1*/
    for (index=0;index<first_list->no_of_entries;index++)
    {
      gs_bit_stream_insert_field(&bitstream,10,modified_list1[index].num);
    }

    /*Store list 2*/
    for (index=0;index<second_list->no_of_entries;index++)
    {
      gs_bit_stream_insert_field(&bitstream,10,modified_list2[index].num);
    }

    bcch_list->length = (byte) gs_bit_stream_byte_count(&bitstream);
  }
}

/**
 * Externalized function used by usat to get measurement results and ba list
 * for GSM cells. When called in dedicated, the measurement results are from
 * the last meas results IE. If not in dedicated a meas results is generated
 * based on the last idle mode measurement.
 *
 * @param nmr - ptr where last measurement results should be copied to.
 *   Uses the same format as the measurement results IE sent in dedicated
 *   meas reports.
 * @param bcch_list - ba list
 */
boolean rr_ds_get_nmr_bcch(uint8 *nmr,
                           rr_bcch_list_type *bcch_list,
                           sys_modem_as_id_e_type as_id)
{
 gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);
 MSG_GERAN_HIGH_1_G("Depreciated API will be removed for gasid = %d",gas_id);
 return FALSE;

}
boolean rr_ds_api_get_nmr_bcch(uint8 *nmr,
                               uint8 len_of_nmr,
                               rr_bcch_list_type *bcch_list,
                               sys_modem_as_id_e_type as_id)
{
  usat_system_information_database_T *usat_system_info_database_ptr;
  gprs_scell_info_t                  *scell_info_ptr;
  boolean                             dedicated = FALSE;
  gas_id_t                            gas_id;

  /* convert incoming subscription ID to GAS ID */
  gas_id = geran_map_nas_id_to_gas_id(as_id);

  /* determine database pointers based on the GAS ID */
  usat_system_info_database_ptr = rr_get_usat_system_information_database_ptr(gas_id);
  scell_info_ptr                = rr_gprs_get_scell_info(gas_id);

  GRR_GENERAL_LOCK();

  if (rr_in_dedicated(gas_id))
  {
    rr_ba_lists_t *rr_ba_lists_ptr = rr_get_ba_lists_ptr(gas_id);

    usat_system_info_database_ptr->SIbis_measurement_report_BA_list = rr_ba_lists_ptr->SI5_SI5bis_measurement_report_BA_list;
    usat_system_info_database_ptr->SIter_measurement_report_BA_list = rr_ba_lists_ptr->SI5ter_measurement_report_BA_list;
    usat_system_info_database_ptr->cell_options     = scell_info_ptr->gsm.cell_options;
    usat_system_info_database_ptr->reselection_info = scell_info_ptr->gsm.reselection_info;
    /***************************************************************/
    /* no need to copy best six since we do not need to generate a */
    /* measurement report, we just use the last one sent           */
    /***************************************************************/
    usat_system_info_database_ptr->best_six_info.no_of_entries=0;
    dedicated = TRUE;
  }
  else
  {
    // For USAT purposes SI2 BA list is good Enough in Idle mode
    usat_system_info_database_ptr->SIbis_measurement_report_BA_list = scell_info_ptr->gsm.SI2_BA_list;
    //usat_system_info_database_ptr->SIter_measurement_report_BA_list = system_information_database_ptr->SI2ter_BA_list;
    usat_system_info_database_ptr->cell_options     = scell_info_ptr->gsm.cell_options;
    usat_system_info_database_ptr->reselection_info = scell_info_ptr->gsm.reselection_info;
    usat_system_info_database_ptr->best_six_info    = *rr_get_last_best_six(gas_id);
    dedicated = FALSE;
  }

  GRR_GENERAL_UNLOCK();

  rr_get_ba_list_for_measurement_report(bcch_list, dedicated, gas_id);
  rr_get_last_meas_results(nmr, len_of_nmr, dedicated, gas_id);

  return(TRUE);
}

/**
 * Externalized function used by usat to get measurement results and ba list
 * for GSM cells. When called in dedicated, the measurement results are from
 * the last meas results IE. If not in dedicated a meas results is generated
 * based on the last idle mode measurement.
 *
 * @param nmr - ptr where last measurement results should be copied to.
 *   Uses the same format as the measurement results IE sent in dedicated
 *   meas reports.
 * @param bcch_list - ba list
 */
 /* rr_get_nmr_bcch */

boolean rr_get_nmr_bcch(uint8 *nmr, rr_bcch_list_type *bcch_list)
{
 MSG_GERAN_HIGH_0("Depreciated API will be removed");
 return FALSE;
}
boolean rr_api_get_nmr_bcch(uint8 *nmr, uint8 len_of_nmr,  rr_bcch_list_type *bcch_list)
{
 return rr_ds_api_get_nmr_bcch(nmr, len_of_nmr, bcch_list, SYS_MODEM_AS_ID_1);
}
/**
 * Externalized function used by usat to get cell identity,
 * referenced by as_id.
 *
 * @param cell_id - pointer where cell id is returned.
 * @param as_id - access stratum id.
 */
boolean rr_ds_get_cell_id(
  uint8                  *cell_id,
  sys_modem_as_id_e_type  as_id
)
{
  gas_id_t gas_id = geran_map_nas_id_to_gas_id(as_id);
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  GRR_GENERAL_LOCK();

  /************************************************************************/
  /* Note the cell id may not be valid if we have not obtained service or */
  /* are in dedicated mode, it is up to the calling function to make sure */
  /* we are in idle                                                       */
  /************************************************************************/
  cell_id[0] = (byte) (scell_info_ptr->gsm.cell_identity >> 8);
  cell_id[1] = (byte) (scell_info_ptr->gsm.cell_identity & 0xFF);

  GRR_GENERAL_UNLOCK();

  return scell_info_ptr->gsm.cell_identity_valid_flag;
}

/**
 * Externalized function used by usat to get cell identity
 *
 * @param cell_id - pointer where cell id is returned
 */
boolean rr_get_cell_id(
  uint8 *cell_id
)
{
  return rr_ds_get_cell_id(cell_id, SYS_MODEM_AS_ID_1);
}

/**
 * Externalized function used by usat to get timing advance
 * for GSM cells, referenced by access stratum id.
 *
 * @param me_status - pointer where mobile status is returned.
 * @param timing_advance - pointer where timing advance
 *                         returned.
 * @param as_id - access stratum id.
 */

boolean rr_ds_get_timing_advance(
  uint8                  *me_status,
  uint8                  *timing_advance,
  sys_modem_as_id_e_type  as_id
)
{
  gas_id_t gas_id;

  GRR_GENERAL_LOCK();

  gas_id = geran_map_nas_id_to_gas_id(as_id);

  if (rr_in_dedicated(gas_id))
  {
    *me_status = 0x01; /* non idle*/
  }
  else
  {
    *me_status = 0x00; /*idle*/
  }

  /**************************************************************************/
  /* Note that the specs don't seem to have a parameter to indicate when no */
  /* timing advance was recorded, ie no call was made. In that case we just */
  /* return whatever it was initialized to                                  */
  /**************************************************************************/
  timing_advance[0] = rr_general_data[gas_id].rr_current_timing_advance>>4;
  timing_advance[1] = rr_general_data[gas_id].rr_current_timing_advance & 0x0F;

  GRR_GENERAL_UNLOCK();

  return TRUE;
}

/**
 * Externalized function used by usat to get timing advance
 * for GSM cells
 *
 * @param me_status - pointer where mobile status is returned
 * @param timing_advance - pointer where timing advance returned
 */

boolean rr_get_timing_advance(
  uint8 *me_status,
  uint8* timing_advance
)
{
  return rr_ds_get_timing_advance(me_status, timing_advance, SYS_MODEM_AS_ID_1);
}

void rr_process_dl_data_ack_ind(
  dl_data_ack_ind_T *new_message,
  const gas_id_t gas_id
)
{
  byte  l3_message_number = new_message->l3_message_number;

  if (new_message->SAPI == SAPI3)
  {
    rr_sapi3_process_dl_data_ack_ind(l3_message_number, gas_id);
    return;
  }

  if (IS_RR_SIGNALLING_ACK(new_message->SAPI, l3_message_number))
  {
    rr_handle_signalling_acknowledgement(l3_message_number, gas_id);
    return;
  }

#ifdef FEATURE_GSM_DTM
  if (IS_GTTP_SIGNALLING_ACK(new_message->SAPI, l3_message_number))
  {
    rr_dtm_send_imsg_l2_gttp_ack_ind(gas_id);
    return;
  }
#endif // FEATURE_GSM_DTM

  /* SAPI0 NAS message - remove the L3 message from the NAS message buffer */
  if (rr_remove_from_nas_message_buffer(l3_message_number, gas_id) == FALSE)
  {
    MSG_GERAN_HIGH_1_G("Failed removing NAS msg buf entry (%d)", (int) l3_message_number);
  }
  else
  {
    MSG_GERAN_LOW_1_G("Removed NAS msg buf for (%d)", (int) l3_message_number);
  }
}


/* functions that work on the rr_nas_message_buffer */

void rr_initialize_nas_message_buffer(
  const gas_id_t gas_id
)
{
  uint32 i;
  rr_nas_message_buffer_T **rr_nas_msg_buf_ptr_ptr = rr_get_nas_message_buffer_ptr_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_nas_msg_buf_ptr_ptr);

  for (i=0; i<MAX_NAS_MESSAGE_BUFFER_SIZE; i++)
  {
    if (rr_nas_msg_buf_ptr_ptr[i] != NULL)
    {
      GPRS_MEM_FREE(rr_nas_msg_buf_ptr_ptr[i]);
      rr_nas_msg_buf_ptr_ptr[i] = NULL;
    }
  }
}

/* Add element to the nas message buffer and return the l3_message_number */
byte rr_add_to_nas_message_buffer(
  SAPI_T SAPI,
  byte l3_message_length,
  byte *l3_message,
  const gas_id_t gas_id
)
{
  byte index;
  rr_nas_message_buffer_T **rr_nas_msg_buf_ptr_ptr = rr_get_nas_message_buffer_ptr_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_nas_msg_buf_ptr_ptr);

  for (index = 0; index < MAX_NAS_MESSAGE_BUFFER_SIZE; ++index)
  {
    if (rr_nas_msg_buf_ptr_ptr[index] == NULL)
    {
      break;
    }
  }

  if (index == MAX_NAS_MESSAGE_BUFFER_SIZE)
  {
    MSG_GERAN_HIGH_1_G("Exceeded NAS Message Buffer limit (%d)", (int) MAX_NAS_MESSAGE_BUFFER_SIZE);
    return MAX_NAS_MESSAGE_BUFFER_SIZE+1;
  }

  rr_nas_msg_buf_ptr_ptr[index] = GPRS_MEM_MALLOC(sizeof(rr_nas_message_buffer_T));

  if (rr_nas_msg_buf_ptr_ptr[index] == NULL)
  {
    MSG_GERAN_ERROR_0_G("failed to alloc memory");
    return MAX_NAS_MESSAGE_BUFFER_SIZE+1;
  }

  rr_nas_msg_buf_ptr_ptr[index]->SAPI = SAPI;
  rr_nas_msg_buf_ptr_ptr[index]->l3_message_length = l3_message_length;
    memscpy(rr_nas_msg_buf_ptr_ptr[index]->layer3_message,
  	      sizeof(rr_nas_msg_buf_ptr_ptr[index]->layer3_message), 
  	      l3_message, 
  	      l3_message_length);


  MSG_GERAN_MED_1_G("Added NAS message to buffer at (%d)", (int) index);

  /* l3_message_number is used to correlate the (dl_data_req)requests to L2
   with (dl_data_ack_ind) responses from L2. The index to the rr_nas_message_buffer
   is between 0 to MAX_NAS_MESSAGE_BUFFER_SIZE-1. Since the l3_message_number
   cannot go beyond MAX_NAS_MESSAGE_BUFFER_SIZE, index+1 can be used
   as the l3_message_number. This is because l3_message_number 0 is reserved
   for RR messages */
  return index+1;
}

boolean rr_remove_from_nas_message_buffer(
  byte l3_message_number,
  const gas_id_t gas_id
)
{
  int index = l3_message_number - 1;
  rr_nas_message_buffer_T **rr_nas_msg_buf_ptr_ptr = rr_get_nas_message_buffer_ptr_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_nas_msg_buf_ptr_ptr);

  if (index < 0 || index >= MAX_NAS_MESSAGE_BUFFER_SIZE)
  {
    MSG_GERAN_HIGH_1_G("Invalid l3_message_number(%d)", (int) l3_message_number);
    return FALSE;
  }

  if (rr_nas_msg_buf_ptr_ptr[index] == NULL)
  {
    MSG_GERAN_HIGH_1_G("No data buffered at index(%d)", (int) index);
    return FALSE;
  }

  GPRS_MEM_FREE(rr_nas_msg_buf_ptr_ptr[index]);
  rr_nas_msg_buf_ptr_ptr[index] = NULL;

  MSG_GERAN_MED_1_G("Removed NAS message from buffer at (%d)", (int) index);

  return TRUE;
}

#ifdef FEATURE_WCDMA
/* Sends buffered msg to RRC */
void rr_send_buffered_nas_messages(
  const gas_id_t gas_id
)
{
  byte i;
  rr_nas_message_buffer_T **rr_nas_msg_buf_ptr_ptr = rr_get_nas_message_buffer_ptr_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_nas_msg_buf_ptr_ptr);

  /* Send all non ack msg */
  for (i = 0; i < MAX_NAS_MESSAGE_BUFFER_SIZE; ++i)
  {
    if (rr_nas_msg_buf_ptr_ptr[i] != NULL)
    {
      rr_send_nas_data_ind(
        rr_nas_msg_buf_ptr_ptr[i]->SAPI,
        rr_nas_msg_buf_ptr_ptr[i]->l3_message_length,
        rr_nas_msg_buf_ptr_ptr[i]->layer3_message,
        gas_id
      );
    }
  }
  /* Send last message with NULL l3_message here */
  rr_send_nas_data_ind(0, 0, NULL, gas_id);
}
#endif /* FEATURE_WCDMA */

/* Determines and returns the l2_channel_type based on the channel_configuration information */
l2_channel_type_T rr_get_l2_channel_type(
  const gas_id_t gas_id
)
{
  channel_spec_T    *chan_spec_ptr = rr_get_channel_spec(NULL, gas_id);
  l2_channel_type_T  l2_channel_type;

  if (chan_spec_ptr != NULL)
  {
    if ((chan_spec_ptr->channel_info_1_after.channel_type == HALF_RATE_TRAFFIC) ||
        (chan_spec_ptr->channel_info_1_after.channel_type == FULL_RATE_TRAFFIC))
    {
      l2_channel_type = SACCH;
    }
    else
    {
      l2_channel_type = DCCH;
    }
  }
  else
  {
    l2_channel_type = DCCH;
  }

  return(l2_channel_type);
}

static int rr_compare_arfcn( const void *arg1, const void *arg2 )
{
  return ( ((ARFCN_T*)arg1)->num - ((ARFCN_T*)arg2)->num );
} /* rr_compare_arfcn */

void qsort_arfcn(
  ARFCN_T *loc_arfcn,
  size_t num,
  const gas_id_t gas_id
)
{
  ARFCN_T *loop_ptr = loc_arfcn;
  byte     index;

  qsort((void *)loc_arfcn, num, sizeof(ARFCN_T), rr_compare_arfcn);

  /* Restore correct band information */
  for (index = 0; index < num; index++, loop_ptr++)
  {
    /* WARNING: derive band info from ARFCN number for now */
    loop_ptr->band = (sys_band_T) rr_get_band(loop_ptr->num, gas_id);
  }
}

void rr_reset_SI2ter_instances(
  si2ter_instances_T *inst_ptr
)
{
  if (inst_ptr != NULL)
  {
    int i;

    memset(inst_ptr, 0, sizeof(si2ter_instances_T));

    /* initialize some parameters to their non-zero default values */
    for (i = 0; i < MAX_SI2TER_INSTANCES; i++)
    {
      inst_ptr->instance[i].Qsearch_I       = DEFAULT_QSEARCH_I;
      inst_ptr->instance[i].FDD_Qmin        = DEFAULT_FDD_QMIN;
      inst_ptr->instance[i].FDD_Qoffset     = DEFAULT_FDD_QOFFSET;
      inst_ptr->instance[i].FDD_Qmin_offset = DEFAULT_FDD_QMIN_OFFSET;
      inst_ptr->instance[i].FDD_RSCPmin     = DEFAULT_FDD_RSCPMIN;
      inst_ptr->instance[i].TDD_Qoffset     = DEFAULT_TDD_QOFFSET;
    }
  }
}

void rr_initialize_SI2ter_instances(const gas_id_t gas_id)
{
  si2ter_instances_T *si2ter_inst_ptr;

  si2ter_inst_ptr = rr_get_SI2ter_instances_ptr(gas_id);
  rr_reset_SI2ter_instances(si2ter_inst_ptr);
}

/**************************************************************************************
Function
rr_initialize_SI2q_MI_instances

Description
Initialize the SI2q_MI_instances array to their default values.

Parameters
[IN]
instances_T           *instances:
Pointer to global storage for the instances.

Returns
None

**********************************************************************************************/
void rr_initialize_SI2q_MI_instances(
  instances_T *instances,
  si2q_mi_instance_e_type si2q_or_mi
)
{
  byte i;

  ASSERT (instances != NULL);

  if (si2q_or_mi == SI2Q_INSTANCE)
  {
    MSG_GERAN_HIGH_0("Clearing SI2quater instances!");
  }
  else if (si2q_or_mi == MI_INSTANCE)
  {
    MSG_GERAN_HIGH_0("Clearing Measurement Information instances!");
  }

  /* initialize some parameters to their default values
  which is different from 0 */
  if (si2q_or_mi == SI2Q_INSTANCE)
  {
    for (i = 0; i < MAX_INSTANCES; ++i)
    {
      rr_si2q_params_free_children(&instances->instance[i].si2q_mi.si2q_params);
    }

    memset((void *) instances, 0, sizeof(instances_T));
    instances->si2q_or_mi = si2q_or_mi;

    for (i = 0; i < MAX_INSTANCES; ++i)
    {
      instances->instance[i].si2q_mi.si2q_params.FDD_Qmin  = DEFAULT_FDD_QMIN;
      instances->instance[i].si2q_mi.si2q_params.QSearch_I = DEFAULT_QSEARCH_I;
      instances->instance[i].si2q_mi.si2q_params.REPORT_TYPE = REPORT_TYPE_N0RMAL;
      instances->instance[i].si2q_mi.si2q_params.FDD_Qoffset = DEFAULT_FDD_QOFFSET;
      instances->instance[i].si2q_mi.si2q_params.TDD_Qoffset = DEFAULT_TDD_QOFFSET;
      instances->instance[i].si2q_mi.si2q_params.FDD_Qmin_offset = DEFAULT_FDD_QMIN_OFFSET;
      instances->instance[i].si2q_mi.si2q_params.FDD_RSCPmin = DEFAULT_FDD_RSCPMIN;
    }
  }
  else
  {
    memset((void *) instances, 0, sizeof(instances_T));
    instances->si2q_or_mi = si2q_or_mi;

    for (i = 0; i < MAX_INSTANCES; ++i)
    {
      instances->instance[i].si2q_mi.mi_params.REPORT_TYPE = REPORT_TYPE_N0RMAL;
    }
  }
}

void rr_initialize_SI2q_instances(
  const gas_id_t gas_id
)
{
  instances_T *si2q_inst_ptr;

  /* obtain pointer to global SI2q instances store */
  si2q_inst_ptr = rr_get_instances_ptr(SI2Q_INSTANCE, gas_id);
  rr_initialize_SI2q_MI_instances(si2q_inst_ptr, SI2Q_INSTANCE);

  return;
}

void rr_initialize_MI_instances(
  const gas_id_t gas_id
)
{
  instances_T *mi_inst_ptr;

  /* obtain pointer to global MI instances store */
  mi_inst_ptr = rr_get_instances_ptr(MI_INSTANCE, gas_id);
  rr_initialize_SI2q_MI_instances(mi_inst_ptr, MI_INSTANCE);

  return;
}

/**
 * Extracts the message header & id from the input message, if the input event is
 * EV_INPUT_MESSAGE.
 * If the input event is not EV_INPUT_MESSAGE, this event is just returned.
 * Note: If the input event is EV_INPUT_MESSAGE, but the input message is NULL,
 * then EV_NO_EVENT is returned
 */
rr_event_T rr_extract_message_header_details(
  rr_event_T rr_event,
  rr_cmd_bdy_type * message_ptr,
  byte * message_set,
  byte * message_id )
{
  if ( rr_event == EV_INPUT_MESSAGE )
  {
    if ( message_ptr && message_set && message_id )
    {
      (* message_set) = message_ptr->message_header.message_set;
      (* message_id) = message_ptr->message_header.message_id;
    }
    else
    {
      MSG_GERAN_ERROR_0("NULL parameters to rr_extract_message_header_details()");
      rr_event = EV_NO_EVENT;
    }
  }

  return rr_event;

} /* rr_extract_message_header_details() */

void rr_send_handover_sync_ind(
  rr_dedicated_data_T *dedic_ptr,
  const gas_id_t gas_id
)
{
  if (dedic_ptr != NULL)
  {
    sync_reason_T sync_reason;

    /* determine the appropriate reason for this handover sync signal */
    if ((dedic_ptr->prev_channel_spec.channel_mode_1 != SIGNALLING_ONLY) &&
        (dedic_ptr->curr_channel_spec.channel_mode_1 != SIGNALLING_ONLY))
    {
      sync_reason = MM_RR_HANDOVER;
    }
    else
    {
      sync_reason = MM_RR_RES_ASS;
    }

    /* update the ciphering status before sending the RR_SYNC_IND */
    rr_ciphering_has_started(dedic_ptr->curr_channel_spec.cipher_flag, gas_id);
#ifdef FEATURE_LTE
    if (rr_ce_access_reason(gas_id) == MM_RR_MO_FULL_SPEECH ||
        rr_ce_access_reason(gas_id) == MM_RR_MO_HALF_SPEECH ||
        rr_ce_access_reason(gas_id) == MM_RR_SUPPLEMENTARY_SERVICE ||
        rr_ce_access_reason(gas_id) == MM_RR_CALL_REEST ||
        rr_ce_is_periodic_lau(gas_id) == TRUE)
    {
      if ((sync_reason == MM_RR_RES_ASS) && (rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(gas_id)))
      {
        rr_connection_release_set_g2l_blind_redir_flag(gas_id);
      }
    }
#endif /* FEATURE_LTE */

    /* and then send it to NAS */
    rr_send_sync_ind(
      sync_reason,
      dedic_ptr->curr_channel_spec.channel_info_1_after.channel_type,
      dedic_ptr->curr_channel_spec.channel_mode_1,
      dedic_ptr->curr_channel_spec.channel_info_1_after.subchannel,
      gas_id
    );
  }

  return;
}


void rr_init_hz_info(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  rr_general_data_ptr->rr_hz_info.state_change_cb_ptr = NULL;
  rr_general_data_ptr->rr_hz_info.last_state = RR_HZ_STATE_NO_SERVICE;
  rr_general_data_ptr->rr_hz_info.plmn.identity[0] = INVALID_PLMN_ID;
  rr_general_data_ptr->rr_hz_info.plmn.identity[1] = INVALID_PLMN_ID;
  rr_general_data_ptr->rr_hz_info.plmn.identity[2] = INVALID_PLMN_ID;
  rr_general_data_ptr->rr_hz_info.lac = INVALID_LAC_WORD;
  rr_general_data_ptr->rr_hz_info.cell_id = 0xFFFF;
}

/**
 * De-registers the callback function for Home Zone info
 */
void rr_hz_deregister_state_change_cb(
  void
)
{
  // Obtain a pointer to the module data
  // TODO - HZ registration is only supported for GERAN_ACCESS_STRATUM_ID_1
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(GERAN_ACCESS_STRATUM_ID_1);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  MSG_GERAN_HIGH_0("De-registering Home Zone callback function");
  rr_general_data_ptr->rr_hz_info.state_change_cb_ptr = NULL;

  return;
}

#ifndef PERLUTF
/**
 * Registers the callback function for Home Zone info
 */
boolean rr_hz_register_state_change_cb(
  rr_hz_state_cb_fn_T new_fn_ptr
)
{
  // Obtain a pointer to the module data
  // TODO - HZ registration is only supported for GERAN_ACCESS_STRATUM_ID_1
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(GERAN_ACCESS_STRATUM_ID_1);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  if (rr_general_data_ptr->rr_hz_info.state_change_cb_ptr != NULL)
  {
    /* a function is already registered, so reject the new request */
    MSG_GERAN_HIGH_0("ERROR: Home Zone callback is already registered");
    return(FALSE);
  }

  MSG_GERAN_HIGH_0("Registering Home Zone callback function");
  rr_general_data_ptr->rr_hz_info.state_change_cb_ptr = new_fn_ptr;

  return(TRUE);
}
#else
#error code not present
#endif /* PERLUTF */

/**
 * Call the registered callback function for Home Zone Service
 */
void rr_hz_notify_state_change(
  rr_hz_state_ind_T new_state,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  if (rr_general_data_ptr->rr_hz_info.state_change_cb_ptr != NULL)
  {
    /* use the callback function that has been registered */
    (* rr_general_data_ptr->rr_hz_info.state_change_cb_ptr)(
      rr_general_data_ptr->rr_hz_info.last_state,
      new_state,
      rr_general_data_ptr->rr_hz_info.plmn,
      rr_general_data_ptr->rr_hz_info.lac,
      rr_general_data_ptr->rr_hz_info.cell_id
    );
  }
  else
  {
    /* no callback function is currently registered */
#ifndef PERLUTF
    MSG_GERAN_LOW_0_G("RR Home Zone state change indication has no callback");
#else
    #error code not present
#endif /* PERLUTF */
  }

  rr_general_data_ptr->rr_hz_info.last_state = new_state;

  return;
}

/**
 * Update locally stored PLMN, LAC and Cell Identity with those in the sys info DB
 * If the contents have changed, may call the notification function with IDLE state
 */
void rr_hz_update_zone_info(
  boolean bNotify,
  const gas_id_t gas_id
)
{
  boolean            bChanged = FALSE;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  /* check the contents of the PLMN identity array one element at a time */
  if ((scell_info_ptr->gsm.location_area_identification.plmn_id.identity[0] != rr_general_data_ptr->rr_hz_info.plmn.identity[0]) ||
      (scell_info_ptr->gsm.location_area_identification.plmn_id.identity[1] != rr_general_data_ptr->rr_hz_info.plmn.identity[1]) ||
      (scell_info_ptr->gsm.location_area_identification.plmn_id.identity[2] != rr_general_data_ptr->rr_hz_info.plmn.identity[2]))
  {
    bChanged = TRUE;
    rr_general_data_ptr->rr_hz_info.plmn = scell_info_ptr->gsm.location_area_identification.plmn_id;
  }

  /* check the LAC */
  if (scell_info_ptr->gsm.location_area_identification.location_area_code != rr_general_data_ptr->rr_hz_info.lac)
  {
    bChanged = TRUE;
    rr_general_data_ptr->rr_hz_info.lac = scell_info_ptr->gsm.location_area_identification.location_area_code;
  }

  /* check the cell ID */
  if (scell_info_ptr->gsm.cell_identity != rr_general_data_ptr->rr_hz_info.cell_id)
  {
    bChanged = TRUE;
    rr_general_data_ptr->rr_hz_info.cell_id = scell_info_ptr->gsm.cell_identity;
  }

  if (bChanged && bNotify)
  {
    rr_hz_notify_state_change(RR_HZ_STATE_IDLE, gas_id);
  }

  return;
}

/**
 * Handles a change in GMAC state and notifies change of PS activity if necessary
 */
void rr_hz_notify_ps_status(
  boolean bPSActive,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  if (bPSActive)
  {
    /* GMAC is definitely active with a TBF established */
    if (rr_general_data_ptr->rr_hz_info.last_state != RR_HZ_STATE_PS_CONNECTION)
    {
      rr_hz_notify_state_change(RR_HZ_STATE_PS_CONNECTION, gas_id);
    }
  }
  else
  {
    /* MAC is idle: if last state notification was PS connection, indicate idle now */
    if (rr_general_data_ptr->rr_hz_info.last_state == RR_HZ_STATE_PS_CONNECTION)
    {
      rr_hz_notify_state_change(RR_HZ_STATE_IDLE, gas_id);
    }
  }

  return;
}


boolean rr_bands_equal(
  rr_internal_band_T new_band,
  rr_internal_band_T old_band
)
{
  if (new_band == old_band)
  {
    return TRUE;
  }
  else if ((new_band == BAND_EGSM && old_band == BAND_PGSM) ||
           (new_band == BAND_PGSM && old_band == BAND_EGSM))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
 * Returns TRUE or FALSE depending on whether RR is allowed to select any cell
 * of if the PLMN of the selected cell must match (or be equivalent to) the
 * PLMN requested by NAS.
 *
 * This is derived from the network select mode, but may also be affected if
 * there is an RR_EST_REQ for emergency call pending.
 */
boolean rr_is_any_cell_selection_mode(
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  #ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
  if (rr_x2g_redir_search_for_emergency_call_in_progress(gas_id))
  {
    MSG_GERAN_MED_0_G("any_cell_selection allowed for W2G service redirection (emergency call)");
    return TRUE;
  }
  #endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

  if (rr_saved_msg_is_rr_est_req_emergency_call(gas_id))
  {
    MSG_GERAN_MED_0_G("any_cell_selection allowed for emergency call");
    return TRUE;
  }

  if ((rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      || (rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL))
  {
    return FALSE;
  }

  if ((rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
      || (rr_PLMN_request_details_ptr->network_select_mode ==
          SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)
      || (rr_PLMN_request_details_ptr->network_select_mode ==
          SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  {
    return TRUE;
  }

  MSG_GERAN_ERROR_1_G("Invalid Network Selection Mode: %d", rr_PLMN_request_details_ptr->network_select_mode);
  return FALSE;
}

boolean rr_is_any_cell_reselection_mode(
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  if (rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED ||
      rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION ||
      rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
  {
    return TRUE;
  }

  return FALSE;
}

boolean rr_is_automatic_plmn_selection_mode(
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  if (rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
  {
    return TRUE;
  }

  return FALSE;
}

boolean rr_is_limited_plmn_selection_mode(
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  return ( (rr_PLMN_request_details_ptr->network_select_mode ==
              SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) ||
            (rr_PLMN_request_details_ptr->network_select_mode ==
              SYS_NETWORK_SELECTION_MODE_LIMITED) ||
            (rr_PLMN_request_details_ptr->network_select_mode ==
              SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION) );
}

/*******************************************************************************************
Function:rr_adjust_rxlev_by_scale

Description:

Parameters:

*******************************************************************************************/
void rr_adjust_rxlev_by_scale(
  byte scale,
  byte *rxlev
)
{
  /* scale is an offset, which applies for the reported RXLEV values.
   * 0 = 0 dB, 1 = +10 dB, see 45.008 for details
   */

  if (scale == 1)
  {
    *rxlev = (*rxlev > 10) ? (*rxlev - 10) : 0;
  }
}

#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
/*
** Populates the given structure with the list of location area codes
** for the indicated PLMN (i.e. caller sets 'plmn_id' and then calls
** function)
**
** Input:
**    plmn_lac_list.PLMN_id
**
** Output:
**    plmn_lac_list.num_of_lacs  - number of LACs available and belong to that PLMN
**    plmn_lac_list.lacs[]       - LACs available and belong to that PLMN
*/
void gprs_odm_get_plmn_lac_list(
  gprs_odm_plmn_lacs_t *plmn_lac_list
)
{
  uint16 i, j;
  PLMN_id_T odm_plmn_id;

  rr_candidate_cell_db_T *rr_candidate_cell_db_ptr;

  if (plmn_lac_list == NULL)
  {
    MSG_GERAN_ERROR_0_G("plmn_lac_list NULL");
    return;
  }

  // Obtain a pointer to the Candidate Cell DB
  // TODO - only GERAN_ACCESS_STRATUM_ID_1 is supported
  rr_candidate_cell_db_ptr = rr_get_candidate_cell_db_ptr(GERAN_ACCESS_STRATUM_ID_1);

  odm_plmn_id = *(PLMN_id_T *)&plmn_lac_list->plmn_id;
  plmn_lac_list->num_of_lacs = 0;
  memset(plmn_lac_list->lacs, 0, sizeof(gprs_odm_lac_t)*GPRS_ODM_MAX_NUM_OF_PLMN_LACS);

  /* add all LACs below to the given PLMN-id into the plmn_lac_list */
  for (i=0; i < rr_candidate_cell_db_ptr->num_frequencies; i++)
  {
    if ( sys_plmn_match(odm_plmn_id,
           rr_candidate_cell_db_ptr->candidate_cells[i].si_info.lai.plmn_id) )
    {
      for (j=0; j < plmn_lac_list->num_of_lacs; j++)
      {
        if (plmn_lac_list->lacs[j] ==
            rr_candidate_cell_db_ptr->candidate_cells[i].si_info.lai.location_area_code)
        {
          break;
        }
      }

      if (j == plmn_lac_list->num_of_lacs)
      {
        plmn_lac_list->lacs[plmn_lac_list->num_of_lacs] =
          rr_candidate_cell_db_ptr->candidate_cells[i].si_info.lai.location_area_code;
        plmn_lac_list->num_of_lacs++;

        MSG_GERAN_HIGH_1_G("New LAC %d added into plmn_lac_list",
          rr_candidate_cell_db_ptr->candidate_cells[i].si_info.lai.location_area_code);
      }
    }
  }

  MSG_GERAN_HIGH_1_G("lac_list get from RR, num_of_lacs %d", plmn_lac_list->num_of_lacs);
}

 /*
 ** Set the range of LACs belongs to user selected network
 **
 ** Input:
 **    odm_lac_range_list  -  LAC ranges for the user selected PLMN, read from SIM
 **    gprs_odm_lac_cat_t  -  is LAC range allowed (LAC_DIRECT) or not (LAC_INVERT)
 **
 ** Output:
 **    rr_plmn_lac_range   -  RR's local static copy of odm_lac_range_list
 */
void gprs_odm_set_plmn_lac_list(
  const gprs_odm_plmn_lac_ranges_t *odm_lac_range_list,
  gprs_odm_lac_cat_t               range_cat,
  const gas_id_t                   gas_id
)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  if (odm_lac_range_list != NULL)
  {
    rr_general_data_ptr->rr_plmn_lac_range_valid = TRUE;
    rr_general_data_ptr->rr_plmn_lac_range = *odm_lac_range_list;
    rr_general_data_ptr->rr_plmn_lac_range.lac_range_cat = range_cat;

    MSG_GERAN_HIGH_1_G("LAC range is set by UI, number of lac ranges %d ",
      rr_general_data_ptr->rr_plmn_lac_range.num_of_lac_ranges);
  }
  else
  {
    rr_general_data_ptr->rr_plmn_lac_range_valid = FALSE;
  }
}

boolean rr_cell_is_in_plmn_lac_range(
  LAI_T lai,
  const gas_id_t gas_id
)
{
  uint8              i;
  boolean            lac_found = FALSE;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  MSG_GERAN_HIGH_0_G("Check if a cell belongs to rr_plmn_lac_list");

  if (!rr_general_data_ptr->rr_plmn_lac_range_valid)
  {
    MSG_GERAN_HIGH_0_G("rr_plmn_lac_range_valid = FALSE, no need to check");
    return TRUE;
  }

  if (!sys_plmn_match(lai.plmn_id, ODM_PLMN_ID))
  {
    MSG_GERAN_HIGH_0_G("Not the ODM PLMN, no need to check");
    return TRUE;
  }

  for (i=0; i < rr_general_data_ptr->rr_plmn_lac_range.num_of_lac_ranges; i++)
  {
    if ((lai.location_area_code >= rr_general_data_ptr->rr_plmn_lac_range.lac_ranges[i].start) &&
        (lai.location_area_code <= rr_general_data_ptr->rr_plmn_lac_range.lac_ranges[i].end))
    {
      MSG_GERAN_HIGH_2_G("lac %x is in lac range, lac cat is %d",
        lai.location_area_code, rr_general_data_ptr->rr_plmn_lac_range.lac_range_cat);
      lac_found = TRUE;
      break;
    }
  }

  return ((rr_general_data_ptr->rr_plmn_lac_range.lac_range_cat == LAC_DIRECT) ? lac_found : (!lac_found));
}

#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */


/* ----------    Functions to provide data to DIAG   ----------- */

#if !defined(__GNUC__)
void rr_diag_get_lai(
  __packed uint8 *lai
)
#else
void rr_diag_get_lai(
  uint8 *lai
)
#endif /* ! __GNUC__ */
{

} /* end rr_diag_get_lai() */

#if !defined(__GNUC__)
void rr_diag_get_cell_id(__packed uint16 *cell_id)
#else
void rr_diag_get_cell_id(uint16 *cell_id)
#endif /* ! __GNUC__ */
{

} /* end rr_diag_get_cell_id() */

/*==========================================================================

  FUNCTION    geran_eng_mode_format_scell_info

  DESCRIPTION

    Formats the scell info data from the sys info database provided.


  PARAMETERS

    sys_info - sys info database

    cause - scell info cause

  DEPENDENCIES
    None.

  RETURN VALUE
    eng_mode_scell_info_t

  SIDE EFFECTS
    None.

===========================================================================*/
eng_mode_scell_info_t geran_eng_mode_format_scell_info(
  gprs_scell_info_t *scell_info_ptr,
  eng_mode_cell_cb_cause_t cause,
  const gas_id_t gas_id
)
{
  eng_mode_scell_info_t scell_info = {0};

  scell_info.arfcn = scell_info_ptr->gsm.BCCH_ARFCN.num;
  scell_info.bsic = scell_info_ptr->gsm.BSIC.BS_colour_code + ( scell_info_ptr->gsm.BSIC.PLMN_colour_code << 3 );
  scell_info.cell_id = scell_info_ptr->gsm.cell_identity;
  scell_info.lac = scell_info_ptr->gsm.location_area_identification.location_area_code;
  scell_info.plmn.plmn_id[0] = scell_info_ptr->gsm.location_area_identification.plmn_id.identity[0];
  scell_info.plmn.plmn_id[1] = scell_info_ptr->gsm.location_area_identification.plmn_id.identity[1];
  scell_info.plmn.plmn_id[2] = scell_info_ptr->gsm.location_area_identification.plmn_id.identity[2];

  /*Update RAC*/
  if (scell_info_ptr->gsm.gprs_indicator)
  {
    if (scell_info_ptr->bcch_sys_info.si13_data_valid_flag)
    {
      scell_info.rac = (uint16)scell_info_ptr->bcch_sys_info.routing_area_code;
    }
    else
    {
      scell_info.rac = 0xffff;
    }
  }
  else
  {
    scell_info.rac = 0xffff;
  }


  scell_info.cell_info_cause = cause;
  scell_info.band = (eng_mode_band_t)rr_get_band(scell_info_ptr->gsm.BCCH_ARFCN.num, gas_id);

  return scell_info;
}

/*==========================================================================

  FUNCTION    rr_eng_mode_update_dtx_ul_status

  DESCRIPTION

    Retrieves and formats the data for ENG_MODE_DTX_UL_STATUS.  Calls eng write
    to update the eng mode store.


  PARAMETERS

    boolean - dtx_status

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None.

===========================================================================*/
void rr_eng_mode_update_dtx_ul_status(
  boolean dtx_status,
  const gas_id_t gas_id
)
{
  geran_eng_mode_data_write(
    ENG_MODE_DTX_UL_STATUS,   // geran_eng_mode_info_type
    &dtx_status,              // param_copy
    TRUE,                     // valid_flag
    gas_id                    // gas_id
  );
}

/*==========================================================================

  FUNCTION    rr_eng_mode_update_nw_params

  DESCRIPTION

    Retrieves and formats the data for ENG_MODE_NW_PARAMS.  Calls eng write
    to update the eng mode store.


  PARAMETERS

    None

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None.

===========================================================================*/
void rr_eng_mode_update_nw_params(
  const gas_id_t gas_id
)
{
#define ENG_MODE_SGSNR_REV_98  0
#define ENG_MODE_SGSNR_REV_99  1

  eng_mode_nw_params_t nw_params = {0};
  gprs_scell_info_t * cell_info_ptr = rr_gprs_get_scell_info(gas_id);
  boolean               gprs_params_available = FALSE;

  RR_NULL_CHECK_FATAL(cell_info_ptr);

  memset(&nw_params, 0xff, sizeof (nw_params)); /* set all values to invalid */

  nw_params.nc_mode = rr_gprs_get_network_control_order(gas_id);
  nw_params.nmo = rr_si_get_nmo(gas_id);

  if (nw_params.nmo == PS_DOMAIN_SYS_INFO_UNAVAILABLE)
  {
    nw_params.nmo = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  }

#ifdef FEATURE_GSM_DTM
  if (rr_in_dedicated(gas_id))
  {
    gprs_params_available = rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID) && rr_cell_supports_dtm(gas_id);

    if (gprs_params_available)
    {
      /* Always assume SGSNR R99 in DTM (3GPP CR G2-050457) */
      nw_params.sgsn_rev = ENG_MODE_SGSNR_REV_99;
    }
  }
  else
#endif /* FEATURE_GSM_DTM */
  {
    /*
    SI13 is acquired if SI3 indicates GPRS is supported - therefore all the GPRS SI should be available.
    However, during cell reestablishment, only minimal SIs are received, which doesn't include SI13.
    So need to check whether the GPRS paramters are available
    */
    gprs_params_available =
      rr_cell_supports_gprs(gas_id) && cell_info_ptr->bcch_sys_info.si13_data_valid_flag;

    if (gprs_params_available)
    {
      if (cell_info_ptr->bcch_sys_info.sgsnr == SGSNR_REV_99)
      {
        nw_params.sgsn_rev = ENG_MODE_SGSNR_REV_99;
      }
      else
      {
        nw_params.sgsn_rev = ENG_MODE_SGSNR_REV_98;
      }
    }
  }

  if (cell_info_ptr->gsm.control_channel_description_valid)
  {
    nw_params.msc_rev = cell_info_ptr->gsm.control_channel_description.MSCR;
  }

  geran_eng_mode_data_write(
    ENG_MODE_NW_PARAMS,    // geran_eng_mode_info_type
    (void*)(&nw_params),   // param_copy
    TRUE,                  // valid_flag
    gas_id                 // gas_id
  );
}

/*==========================================================================

  FUNCTION    rr_eng_mode_update_scell_config

  DESCRIPTION

    Retrieves and formats the data for ENG_MODE_SCELL_CONFIG.  Calls eng write
    to update the eng mode store.


  PARAMETERS

    None

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None.

===========================================================================*/
void rr_eng_mode_update_scell_config(
  const gas_id_t gas_id
)
{
  eng_mode_scell_config_t scell_config = {0};
  gprs_scell_info_t       *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  /*default values*/
  scell_config.pbcch_present = FALSE; /*PBCCH has now been removed from the code base*/
  scell_config.gprs_ms_txpwr_max_cch = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;
  scell_config.gprs_rxlev_access_min = (uint8)GERAN_ENG_MODE_INVALID_VALUE_OF_PARAMETER;

  if (scell_info_ptr->gsm.camped_on_flag &&
      scell_info_ptr->bcch_sys_info.si13_data_valid_flag)
  {
    /*GPRS data valid on cell*/
    if (scell_info_ptr->bcch_sys_info.pmo_scell_params_valid)
    {
      scell_config.gprs_ms_txpwr_max_cch = scell_info_ptr->bcch_sys_info.pmo_gprs_serving_cell_params.gprs_ms_txpwr_max_cch;
      scell_config.gprs_rxlev_access_min = scell_info_ptr->bcch_sys_info.pmo_gprs_serving_cell_params.gprs_rxlev_access_min;
    }
  }

  geran_eng_mode_data_write(
    ENG_MODE_SCELL_CONFIG,    // geran_eng_mode_info_type
    &scell_config,            // param_copy
    TRUE,                     // valid_flag
    gas_id                    // gas_id
  );
}

/*!
 * \brief Returns a pointer to the USAT system informaton.
 *
 * \param gas_id
 *
 * \return usat_system_information_database_T*
 */
usat_system_information_database_T *rr_get_usat_system_information_database_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &usat_system_information_database[as_index];
}

#ifdef FEATURE_LTE

rr_resel_priority_e rr_resel_priority_get(uint8 i)
{
  if (i < RR_RESEL_PRIORITY_MAX)
  {
    return (rr_resel_priority_e) i;
  }
  else
  {
    return RR_RESEL_PRIORITY_UNDEFINED;
  }
}

boolean rr_si2quater_eutran_parameters_received(
  instances_T * si2quater_instances
)
{
  boolean eutran_parameters_received = FALSE;
  uint32 i, j;

  RR_NULL_CHECK_RETURN_PARAM(si2quater_instances, FALSE);

  /* First, a quick check on whether all SI2quater instances have been
  received - this is to handle the case where Fast Acquisition is not supported
  by the network.  If this is the case, the EUTRAN_Start and EUTRAN_Stop bits
  may never be set correctly. */
  if (rr_si2quater_all_instances_received(si2quater_instances))
  {
    return TRUE;
  }

  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    /* Search for E-UTRAN Start */
    if (si2quater_instances->instance[i].valid_data &&
        si2quater_instances->instance[i].si2q_mi.si2q_params.priority_and_eutran_params_desc != NULL &&
        si2quater_instances->instance[i].si2q_mi.si2q_params.priority_and_eutran_params_desc->eutran_params_desc != NULL &&
        si2quater_instances->instance[i].si2q_mi.si2q_params.priority_and_eutran_params_desc->eutran_params_desc->eutran_start)
    {
      for (j = i; j < MAX_INSTANCES; ++j)
      {
        /* Now search for E-UTRAN Stop - only if it is found before we
        encounter a missing instance can we declare that all E-UTRAN
        parameters are received. */
        if (si2quater_instances->instance[j].valid_data)
        {
          if (si2quater_instances->instance[j].si2q_mi.si2q_params.priority_and_eutran_params_desc != NULL &&
              si2quater_instances->instance[j].si2q_mi.si2q_params.priority_and_eutran_params_desc->eutran_params_desc != NULL &&
              si2quater_instances->instance[j].si2q_mi.si2q_params.priority_and_eutran_params_desc->eutran_params_desc->eutran_stop)
          {
            eutran_parameters_received = TRUE;
            break;
          }
        }
        else
        {
          break;
        }
      }
      break;
    }
  }

  return eutran_parameters_received;
}

boolean rr_create_lte_neighbour_list(
  rr_lte_neighbour_list_t * lte_neighbour_list,
  instances_T             * si2quater_instances
)
{
  uint32 i;
  boolean lte_list_created = FALSE;

  ASSERT(lte_neighbour_list != NULL);
  ASSERT(si2quater_instances != NULL);

  memset(lte_neighbour_list, 0, sizeof(rr_lte_neighbour_list_t));

  if (rr_si2quater_eutran_parameters_received(si2quater_instances) == FALSE)
  {
    MSG_GERAN_HIGH_0("EUTRAN_Start->EUTRAN_Stop instances not yet received");
    return lte_list_created;
  }

  MSG_GERAN_HIGH_0("All SI2quater instances containing LTE parameter have been received");
  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    rr_si2quater_ro_eutran_params_desc_t * eutran_params_desc;

    if (si2quater_instances->instance[i].valid_data &&
        si2quater_instances->instance[i].si2q_mi.si2q_params.priority_and_eutran_params_desc != NULL &&
        si2quater_instances->instance[i].si2q_mi.si2q_params.priority_and_eutran_params_desc->eutran_params_desc != NULL)
    {
      uint32 j;

      eutran_params_desc = si2quater_instances->instance[i].si2q_mi.si2q_params.priority_and_eutran_params_desc->eutran_params_desc;

      /* eutran_params_desc->eutran_ccn_active */
      /* eutran_params_desc->eutran_start */
      /* eutran_params_desc->eutran_stop */

      for (j = 0;
           j < MAX_REPEATED_EUTRAN_NEIGHBOUR_CELLS;
           ++j)
      {
        if (eutran_params_desc->eutran_neighbour_cells[j] != NULL)
        {
          rr_lte_neighbour_list_entry_t * dst;
          uint32 k;

          for (k = 0;
               k < eutran_params_desc->eutran_neighbour_cells[j]->eutran_frequency_list_count &&
               k < MAX_EUTRAN_NEIGHBOUR_CELLS_EARFCNS;
               ++k)
          {
            if (lte_neighbour_list->count < MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE)
            {
              dst = &lte_neighbour_list->entries[lte_neighbour_list->count];

              ASSERT(dst != NULL);

              dst->frequency = eutran_params_desc->eutran_neighbour_cells[j]->eutran_frequency_list[k];

              if (eutran_params_desc->eutran_neighbour_cells[j]->eutran_priority_valid)
              {
                dst->priority = rr_resel_priority_get(eutran_params_desc->eutran_neighbour_cells[j]->eutran_priority);
              }
              else
              {
                dst->priority = RR_RESEL_PRIORITY_UNDEFINED;
              }

              dst->thresh_high = eutran_params_desc->eutran_neighbour_cells[j]->thresh_eutran_high;

              if (eutran_params_desc->eutran_neighbour_cells[j]->thresh_eutran_low_valid)
              {
                dst->thresh_low = eutran_params_desc->eutran_neighbour_cells[j]->thresh_eutran_low;
              }
              else
              {
                dst->thresh_low = dst->thresh_high;
              }

              if (eutran_params_desc->eutran_neighbour_cells[j]->eutran_qrxlevmin_valid)
              {
                dst->qrxlevmin = eutran_params_desc->eutran_neighbour_cells[j]->eutran_qrxlevmin;
              }
              else
              {
                dst->qrxlevmin = RR_DEFAULT_EUTRAN_QRXLEVMIN;
              }

              dst->measurement_control = RR_DEFAULT_MEASUREMENT_CONTROL_EUTRAN; 

              ++lte_neighbour_list->count;
              lte_list_created = TRUE;
            }
            else
            {
              MSG_GERAN_ERROR_0("LTE neighbour list truncated");
              lte_list_created = TRUE;
              break;
            }
          }
        }
      }

      /* At this point, we should have an entry in the lte_neighbour_list
      structure for each frequency in the LTE neighbour list.  We now need
      to iterate over the Repeated E-UTRAN Not Allowed Cells structures,
      populating the appropriate entries in the LTE neighbour list with the
      appropriate Not Allowed Cells bitmaps. */
      for (j = 0;
           j < eutran_params_desc->eutran_not_allowed_cells_count &&
           j < MAX_REPEATED_EUTRAN_NOT_ALLOWED_CELLS;
           ++j)
      {
        if (eutran_params_desc->eutran_not_allowed_cells[j] != NULL)
        {
          uint32 k;

          for (k = 0;
               k < eutran_params_desc->eutran_not_allowed_cells[j]->eutran_frequency_indices_count &&
               k < MAX_EUTRAN_NOT_ALLOWED_CELLS_EUTRAN_FREQUENCY_INDICES;
               ++k)
          {
            uint8 eutran_frequency_index = eutran_params_desc->eutran_not_allowed_cells[j]->eutran_frequency_indices[k];
            if (eutran_frequency_index < lte_neighbour_list->count)
            {
              lte_neighbour_list->entries[eutran_frequency_index].not_allowed_cells
                = eutran_params_desc->eutran_not_allowed_cells[j]->not_allowed_cells;
            }
            else
            {
              MSG_GERAN_ERROR_1("Not Allowed Cells structure present for invalid frequency index: %d",
                        eutran_frequency_index);
            }
          }
        }
      }

      /* Repeated E-UTRAN PCID to TA mapping */
      for (j = 0;
           j < eutran_params_desc->eutran_pcid_to_ta_mapping_count &&
           lte_neighbour_list->eutran_pcid_to_ta_mapping_count < MAX_REPEATED_EUTRAN_PCID_TO_TA_MAPPING;
           ++j)
      {
        lte_neighbour_list->eutran_pcid_to_ta_mapping[lte_neighbour_list->eutran_pcid_to_ta_mapping_count] 
            = *eutran_params_desc->eutran_pcid_to_ta_mapping[j];
        ++lte_neighbour_list->eutran_pcid_to_ta_mapping_count;
      }
    }
  }
  return (lte_list_created);  
}

void rr_utran_frequency_list_add_from_head(
  rr_utran_frequency_list_t * utran_frequency_list,
  rr_utran_frequency_list_entry_type_e type,
  uint16 uarfcn
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(utran_frequency_list);

  /* Search from the front of the list for the first undefined entry. */
  for (i = 0; i < RR_UTRAN_FREQUENCY_LIST_SIZE; ++i)
  {
    if (utran_frequency_list->entries[i].type == RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_UNDEFINED)
    {
      utran_frequency_list->entries[i].type = type;
      utran_frequency_list->entries[i].uarfcn = uarfcn;
      break;
    }
  }
}

void rr_utran_frequency_list_add_from_tail(
  rr_utran_frequency_list_t * utran_frequency_list,
  rr_utran_frequency_list_entry_type_e type,
  uint16 uarfcn
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(utran_frequency_list);

  /* Search from the front of the list for the first undefined entry. */
  for (i = RR_UTRAN_FREQUENCY_LIST_SIZE; i > 0; --i)
  {
    if (utran_frequency_list->entries[i - 1].type == RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_UNDEFINED)
    {
      utran_frequency_list->entries[i - 1].type = type;
      utran_frequency_list->entries[i - 1].uarfcn = uarfcn;
      break;
    }
  }
}

boolean rr_utran_frequency_list_contains(
  rr_utran_frequency_list_t * utran_frequency_list,
  rr_utran_frequency_list_entry_type_e type,
  uint16 uarfcn
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_PARAM(utran_frequency_list, FALSE);

  for (i = 0; i < RR_UTRAN_FREQUENCY_LIST_SIZE; ++i)
  {
    if (utran_frequency_list->entries[i].type == type &&
        utran_frequency_list->entries[i].uarfcn == uarfcn)
    {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  @brief Converts rr_utran_rat_type_t to rr_utran_frequency_list_entry_type_e
*/
static rr_utran_frequency_list_entry_type_e rr_convert_utran_rat(
  rr_utran_rat_type_t rat
)
{
  rr_utran_frequency_list_entry_type_e converted_rat = RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_UNDEFINED;

  switch (rat)
  {
    case RR_UTRAN_NONE:
      converted_rat = RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_UNDEFINED;
      MSG_GERAN_ERROR_0("RR unexpected UTRAN RAT type: RR_UTRAN_NONE");
    break;

    case RR_UTRAN_FDD:
      converted_rat = RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_FDD;
    break;

    case RR_UTRAN_TDD:
      converted_rat = RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_TDD;
    break;
  }

  return converted_rat;
}


/**
  @brief This function creates the 3G Frequency List according to the rules
  defined in 3GPP 44.018 section 3.4.1.2.1.7c.

  Indices for the 3G Frequency list are constructed in the following steps.
  When indicates are allocated using the 3G Neighbour Cell list derived from
  the MEASUREMENT INFORMATION message, steps 2 and 3 shall be ignored.

  - Frequencies contained in the 3G Neighbour Cell list are allocated indices
    that start from 0 and are incremented for each frequency.  An index value
    of 0 refers to the first UTRAN frequency (TDD-ARFCN or FDD-ARFCN) contained
    in the 3G Neighbour Cell list; an index value of 1 refers to the second
    frequency contained in the 3G Neighbour Cell list; and so on.  Indices to
    the 3G Neighbour Cell list are allocated according to the order in which 3G
    cells appear in the 3G Neighbour Cell list.  An entry in the 3G Neighbour
    Cell list indicating a frequency for RSSI reporting shall not result in the
    allocation of an index.

  - UTRAN frequencies on their own provided by the SI2quater message (i.e.
    frequencies for which the parameter NR_OF_FDD_CELLS or NR_OF_TDD_CELLS is
    set to 31) are allocated indices that start from 31 and continue in
    descending order.  Indices are first allocated according to increasing
    order of the SI2quater_INDEX field within a consistent set of SI2quater
    messages then according to their order of occurrence within the 3G
    Neighbour Cell description IE for a given SI2quater_INDEX.

  - UTRAN frequencies on their own provided by the SI2ter message are allocated
    indices that start from (31 - N_UTRAN_OWN_SI2QUATER) and continue in
    descending order. Indices are first allocated according to increasing order
    of the SI2ter_INDEX field within a consistent set of SI2ter messages then
    according to their order of occurrence within the UTRAN FDD Description
    and/or UTRAN TDD Description IEs for a given SI2ter_INDEX.
    N_UTRAN_OWN_SI2QUATER is defined as the number of the UTRAN frequencies on
    their own provided by the SI2quater message.

*/
void rr_utran_frequency_list_create(
  rr_utran_frequency_list_t * dst,
  const gas_id_t gas_id
)
{
  wcdma_neighbor_cell_list_T *utran_neighbour_cell_list;
  gprs_scell_info_t          *scell = rr_gprs_get_scell_info(gas_id);
  wcdma_ncell_list_data_t    *wcdma_ncell_list_data_ptr = rr_get_wcdma_ncell_list_data_ptr(gas_id);
  uint32                      i;

  RR_NULL_CHECK_RETURN_VOID(dst);
  RR_NULL_CHECK_RETURN_VOID(wcdma_ncell_list_data_ptr);

  memset(dst, 0, sizeof(rr_utran_frequency_list_t));
  utran_neighbour_cell_list = wcdma_ncell_list_data_ptr->wcdma_neighbor_cell_list;

  /* Stage 1 */
  for (i = 0; i < MAX_UTRAN_CELLS + MAX_UTRAN_FREQ; ++i)
  {
    wcdma_neighbor_cell_list_T * entry = &utran_neighbour_cell_list[i];

    if (entry->valid_data)
    {
      if ( (!rr_utran_frequency_list_contains(dst, RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_FDD, entry->cell.uarfcn)) &&
           (!rr_utran_frequency_list_contains(dst, RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_TDD, entry->cell.uarfcn))
         )
      {
        /* frequency_only flag is TRUE for entries in the neighbour cell list
        for RSSI reporting, so we don't add those to the UTRAN frequency list. */
        if (!entry->frequency_only)
        {
          rr_utran_frequency_list_add_from_head(
            dst,
            rr_convert_utran_rat(entry->rat_type),
            entry->cell.uarfcn
          );
        }
      }
    }
  }

  /* Stage 2 */
  for (i = 0;
       i < scell->gsm.utran_neighbor_list.wcdma_freq_only_list.num_of_frequencies &&
       i < MAX_UTRAN_FREQ_UPDATE_LIST;
       ++i)
  {
    rr_utran_frequency_list_add_from_tail(
      dst,
      rr_convert_utran_rat(scell->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].rat_type),
      scell->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].uarfcn
    );
  }

  /* Stage 3 */
  for (i = 0;
       i < scell->gsm.wcdma_neighbor_list_si2ter.wcdma_freq_only_list.num_of_frequencies &&
       i < MAX_RR_MPH_FREQ_UPDATE_LIST; /*mod to max array size*/
       ++i)
  {
    rr_utran_frequency_list_add_from_tail(
      dst,
      RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_FDD,
      scell->gsm.wcdma_neighbor_list_si2ter.wcdma_freq_only_list.uarfcn[i]
    );
  }

  /*TDD is after FDD in SI2ter message*/
  for (i = 0;
       i < scell->gsm.wcdma_neighbor_list_si2ter.tds_freq_only_list.num_of_frequencies &&
       i < MAX_RR_MPH_FREQ_UPDATE_LIST; /*mod to max array size*/
       ++i)
  {
    rr_utran_frequency_list_add_from_tail(
      dst,
      RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_TDD,
      scell->gsm.wcdma_neighbor_list_si2ter.tds_freq_only_list.uarfcn[i]
    );
  }
}

/**
  @brief Searches the UTRAN Frequency List for an entry that matches the given
         UARFCN / RAT.

  @return the index at which the UARFCN / RAT were found, or RR_UTRAN_FREQUENCY_LIST_SIZE
          if the UARFCN / RAT are not found.
*/
uint32 rr_utran_frequency_list_search(
  rr_utran_frequency_list_t * utran_frequency_list,
  rr_utran_frequency_list_entry_type_e type,
  uint16 uarfcn
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_PARAM(utran_frequency_list, FALSE);

  for (i = 0; i < RR_UTRAN_FREQUENCY_LIST_SIZE; ++i)
  {
    if (utran_frequency_list->entries[i].type == type &&
        utran_frequency_list->entries[i].uarfcn == uarfcn)
    {
      return i;
    }
  }
  return RR_UTRAN_FREQUENCY_LIST_SIZE;
}

boolean rr_resel_priority_is_defined(
  rr_resel_priority_e resel_priority
)
{
  switch (resel_priority)
  {
    case RR_RESEL_PRIORITY_0:
    case RR_RESEL_PRIORITY_1:
    case RR_RESEL_PRIORITY_2:
    case RR_RESEL_PRIORITY_3:
    case RR_RESEL_PRIORITY_4:
    case RR_RESEL_PRIORITY_5:
    case RR_RESEL_PRIORITY_6:
    case RR_RESEL_PRIORITY_7:
      return TRUE;
    default:
      return FALSE;
  }
}

csn_utran_priority_params_t * rr_utran_priority_params_ie_find_entry_for_index(
  csn_utran_priority_params_ie_t * utran_priority_params_ie,
  uint32                           utran_frequency_index
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_PARAM(utran_priority_params_ie, NULL);

  for (i = 0;
       i < utran_priority_params_ie->utran_priority_params_count &&
       i < CSN_UTRAN_PRIORITY_PARAMS_DESC_MAX;
       ++i)
  {
    if (IS_BIT_NUM_SET(utran_priority_params_ie->utran_priority_params[i].frequency_index_bitmap, utran_frequency_index))
    {
      return &utran_priority_params_ie->utran_priority_params[i];
    }
  }
  
  return NULL;
}


rr_resel_priority_e rr_get_resel_priority_for_uarfcn(
  uint16 uarfcn,
  rr_utran_rat_type_t rat_type,
  const gas_id_t gas_id
)
{
  if (rr_is_camped(gas_id))
  {
    /* First, check for Individual Priority information. */
    rr_individual_priorities_t ** individual_priorities = rr_individual_priorities(gas_id);
    if (individual_priorities != NULL && *individual_priorities != NULL)
    {
      /* Individual Priority information is present, check for priority. */
      lte_irat_dedicated_priority_list_s * dedicated_priority_list;
      uint32 i;

      dedicated_priority_list = &(*individual_priorities)->dedicated_priority_list;

      for (i = 0; i < dedicated_priority_list->num_utra_fdd_freq && i < LTE_IRAT_MAX_UTRAN_FREQ; ++i)
      {
        if (dedicated_priority_list->utra_fdd_freq_list[i].arfcn == uarfcn)
        {
          return dedicated_priority_list->utra_fdd_freq_list[i].priority;
        }
      }

#ifdef FEATURE_GSM_TDS
      for (i = 0; i < dedicated_priority_list->num_utra_tdd_freq && i < LTE_IRAT_MAX_UTRAN_FREQ; ++i)
      {
        if (dedicated_priority_list->utra_tdd_freq_list[i].arfcn == uarfcn)
        {
          return dedicated_priority_list->utra_tdd_freq_list[i].priority;
        }
      }
#endif /*FEATURE_GSM_TDS*/

      if ((*individual_priorities)->default_utran_priority_valid)
      {
        return (*individual_priorities)->default_utran_priority;
      }
    }
    else
    {
      rr_utran_frequency_list_t          utran_frequency_list;
      gprs_scell_info_t                * scell = rr_gprs_get_scell_info(gas_id);
      uint32                             utran_frequency_index;

      /* Create the UTRAN Frequency List dynamically, then search it for the
      UARFCN that we are interested in.  This gives the UTRAN Frequency Index,
      which is then searched for in the scell->utran_priority_params structure
      (which is populated whenever SI2quater / PMO is received that modifies
      the system information). */
      rr_utran_frequency_list_create(&utran_frequency_list, gas_id);

      utran_frequency_index = rr_utran_frequency_list_search(
        &utran_frequency_list,
        rr_convert_utran_rat(rat_type),
        uarfcn
      );

      MSG_GERAN_MED_2_G("UARFCN %d, UTRAN Frequency List index: %d", uarfcn, utran_frequency_index);

      if (utran_frequency_index < RR_UTRAN_FREQUENCY_LIST_SIZE)
      {
        /* Search the global UTRAN Priority Parameters structure for an entry
        for the UTRAN Frequency Index that we are concerned with. */
        csn_utran_priority_params_t * utran_priority_params = rr_utran_priority_params_ie_find_entry_for_index(
          &scell->utran_priority_params,
          utran_frequency_index
        );

        if (utran_priority_params != NULL)
        {
          if (utran_priority_params->priority_valid)
          {
            return utran_priority_params->priority;
          }
          else
          {
            /* No specific priority provided, check for a default priority. */
            if (scell->utran_priority_params.default_params_valid)
            {
              return scell->utran_priority_params.default_params.priority;
            }
          }
        }
        else
        {
          /* No specific entry for the frequency that we are interested in, so
          return the default value if present. */
          if (scell->utran_priority_params.default_params_valid)
          {
            return scell->utran_priority_params.default_params.priority;
          }
        }
      }
      else
      {
        MSG_GERAN_ERROR_2_G("Failed to find UTRAN RAT type:%d, UARFCN %d in 3G Frequency List", rat_type, uarfcn);
      }
    }
  }

  return RR_RESEL_PRIORITY_UNDEFINED;
}


static int rr_map_utran_qrxlevmin(
  uint8 qrxlevmin
)
{
  if (qrxlevmin > 31)
  {
    MSG_GERAN_ERROR_2("qrxlevmin %d out of range, capping at %d", qrxlevmin, 31);
    qrxlevmin = 31;
  }

  return -119 + (qrxlevmin * 2);
}

int rr_get_qrxlevmin_for_uarfcn(
  uint16 uarfcn,
  rr_utran_rat_type_t rat_type,
  const gas_id_t gas_id
)
{
  if (rr_is_camped(gas_id))
  {

    rr_utran_frequency_list_t          utran_frequency_list;
    gprs_scell_info_t                * scell = rr_gprs_get_scell_info(gas_id);
    uint32                              utran_frequency_index;

    rr_utran_frequency_list_create(&utran_frequency_list,gas_id);

    utran_frequency_index = rr_utran_frequency_list_search(
      &utran_frequency_list,
      rr_convert_utran_rat(rat_type),
      uarfcn
    );

    if (utran_frequency_index < RR_UTRAN_FREQUENCY_LIST_SIZE)
    {
      /* Search the global UTRAN Priority Parameters structure for an entry
      for the UTRAN Frequency Index that we are concerned with. */
      csn_utran_priority_params_t * utran_priority_params = rr_utran_priority_params_ie_find_entry_for_index(
        &scell->utran_priority_params,
        utran_frequency_index
      );

      if (utran_priority_params != NULL)
      {
        if (utran_priority_params->qrxlevmin_valid)
        {
          return rr_map_utran_qrxlevmin(utran_priority_params->qrxlevmin);
        }
        else
        {
          /* No specific priority provided, check for a default priority. */
          if (scell->utran_priority_params.default_params_valid)
          {
            return rr_map_utran_qrxlevmin(scell->utran_priority_params.default_params.qrxlevmin);
          }
        }
      }
      else
      {
        /* No specific entry for the frequency that we are interested in, so
        return the default value if present. */
        if (scell->utran_priority_params.default_params_valid)
        {
          return rr_map_utran_qrxlevmin(scell->utran_priority_params.default_params.qrxlevmin);
        }
      }
    }
    else
    {
      MSG_GERAN_ERROR_2_G("Failed to find UTRAN RAT type:%d UARFCN %d in 3G Frequency List", rat_type, uarfcn);
    }
  }

  return rr_map_utran_qrxlevmin(RR_DEFAULT_UTRAN_QRXLEVMIN);
}

static uint8 rr_map_thresh_utran(
  uint8 thresh_utran
)
{
  if (thresh_utran > 31)
  {
    MSG_GERAN_ERROR_1("thresh_utran %d out of range, capping at 31", thresh_utran);
    thresh_utran = 31;
  }

  return (thresh_utran * 2);
}

uint8 rr_get_thresh_utran_high_for_uarfcn(
  uint16 uarfcn,
  rr_utran_rat_type_t rat_type,
  const gas_id_t gas_id
)
{
  if (rr_is_camped(gas_id))
  {
    rr_utran_frequency_list_t          utran_frequency_list;
    gprs_scell_info_t                * scell = rr_gprs_get_scell_info(gas_id);
    uint32                              utran_frequency_index;

    rr_utran_frequency_list_create(&utran_frequency_list, gas_id);

    utran_frequency_index = rr_utran_frequency_list_search(
      &utran_frequency_list,
      rr_convert_utran_rat(rat_type),
      uarfcn
    );

    if (utran_frequency_index < RR_UTRAN_FREQUENCY_LIST_SIZE)
    {
      /* Search the global UTRAN Priority Parameters structure for an entry
      for the UTRAN Frequency Index that we are concerned with. */
      csn_utran_priority_params_t * utran_priority_params = rr_utran_priority_params_ie_find_entry_for_index(
        &scell->utran_priority_params,
        utran_frequency_index
      );

      if (utran_priority_params != NULL)
      {
        return rr_map_thresh_utran(utran_priority_params->thresh_high);
      }
      else
      {
        /* No specific entry for the frequency that we are interested in, so
        return the default value if present. */
        if (scell->utran_priority_params.default_params_valid)
        {
          return rr_map_thresh_utran(scell->utran_priority_params.default_params.thresh);
        }
        else
        {
          MSG_GERAN_HIGH_2_G("THRESH_HIGH not available for UARFCN %d (RAT %d)", uarfcn, rat_type);
          return rr_map_thresh_utran(RR_DEFAULT_THRESH_UTRAN_HIGH);
        }
      }
    }
    else
    {
      MSG_GERAN_ERROR_2_G("UTRAN RAT type:%d Frequency Index %d out of range", rat_type, utran_frequency_index);
      return rr_map_thresh_utran(RR_DEFAULT_THRESH_UTRAN_HIGH);
    }
  }

  MSG_HIGH("Not camped, returning default value %d", RR_DEFAULT_THRESH_UTRAN_HIGH, 0, 0);
  return rr_map_thresh_utran(RR_DEFAULT_THRESH_UTRAN_HIGH);
}

uint8 rr_get_thresh_utran_low_for_uarfcn(
  uint16 uarfcn,
  rr_utran_rat_type_t rat_type,
  const gas_id_t gas_id
)
{
  if (rr_is_camped(gas_id))
  { 
    rr_utran_frequency_list_t          utran_frequency_list;
    gprs_scell_info_t                * scell = rr_gprs_get_scell_info(gas_id);
    uint32                              utran_frequency_index;

    rr_utran_frequency_list_create(&utran_frequency_list, gas_id);

    utran_frequency_index = rr_utran_frequency_list_search(
      &utran_frequency_list,
      rr_convert_utran_rat(rat_type),
      uarfcn
    );

    if (utran_frequency_index < RR_UTRAN_FREQUENCY_LIST_SIZE)
    {
      /* Search the global UTRAN Priority Parameters structure for an entry
      for the UTRAN Frequency Index that we are concerned with. */
      csn_utran_priority_params_t * utran_priority_params = rr_utran_priority_params_ie_find_entry_for_index(
        &scell->utran_priority_params,
        utran_frequency_index
      );

      if (utran_priority_params != NULL)
      {
        if (utran_priority_params->thresh_low_valid)
        {
          return rr_map_thresh_utran(utran_priority_params->thresh_low);
        }
        else
        {
          return rr_map_thresh_utran(utran_priority_params->thresh_high);
        }
      }
      else
      {
        /* No specific entry for the frequency that we are interested in, so
        return the default value if present. */
        if (scell->utran_priority_params.default_params_valid)
        {
          return rr_map_thresh_utran(scell->utran_priority_params.default_params.thresh);
        }
      }
    }
    else
    {
      MSG_GERAN_ERROR_2_G("Failed to find UTRAN RAT type:%d, UARFCN %d in 3G Frequency List", rat_type, uarfcn);
    }
  }

  return rr_map_thresh_utran(RR_DEFAULT_THRESH_UTRAN_LOW);
}

rr_resel_priority_e rr_get_resel_priority_for_earfcn(
  lte_earfcn_t earfcn,
  const gas_id_t gas_id
)
{
  if (rr_is_camped(gas_id))
  {
    /* First, check for Individual Priority information. */
    rr_individual_priorities_t ** individual_priorities = rr_individual_priorities(gas_id);
    if (individual_priorities != NULL && *individual_priorities != NULL)
    {
      /* Individual Priority information is present, check for priority. */
      lte_irat_dedicated_priority_list_s * dedicated_priority_list;
      uint32 i;

      dedicated_priority_list = &(*individual_priorities)->dedicated_priority_list;

      for (i = 0; i < dedicated_priority_list->num_eutra_freq && i < LTE_IRAT_MAX_EUTRAN_FREQ; ++i)
      {
        if (dedicated_priority_list->eutra_freq_list[i].arfcn == earfcn)
        {
          return dedicated_priority_list->eutra_freq_list[i].priority;
        }
      }

      if ((*individual_priorities)->default_eutran_priority_valid)
      {
        return (*individual_priorities)->default_eutran_priority;
      }
    }
    else
    {
      gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);
      uint32 i;

      /* Search through the E-UTRAN neighbour list for the E-ARFCN and return
      it's priority. */
      for (i = 0;
           i < scell->lte_neighbour_list.count &&
           i < MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE;
           ++i)
      {
        if (scell->lte_neighbour_list.entries[i].frequency.earfcn == earfcn)
        {
          return scell->lte_neighbour_list.entries[i].priority;
        }
      }
    }
  }
  return RR_RESEL_PRIORITY_UNDEFINED;
}


int16 rr_get_qrxlevmin_for_earfcn(
  lte_earfcn_t earfcn,
  const gas_id_t gas_id
)
{
  if (rr_is_camped(gas_id))
  {
    gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);
    uint32              i;

    for (i = 0;
         i < scell->lte_neighbour_list.count &&
         i < MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE;
         ++i)
    {
      if (scell->lte_neighbour_list.entries[i].frequency.earfcn == earfcn)
      {
        ASSERT(scell->lte_neighbour_list.entries[i].qrxlevmin <= 31);

        return (-140 + (scell->lte_neighbour_list.entries[i].qrxlevmin * 2));
      }
    }
  }

  MSG_GERAN_ERROR_1_G("QRXLEVMIN for E-ARFCN %d not found, using 0", earfcn);
  return -140;
}

int16 rr_get_thresh_high_for_earfcn(
  lte_earfcn_t earfcn,
  const gas_id_t gas_id
)
{
  if (rr_is_camped(gas_id))
  {
    gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);
    uint32              i;

    for (i = 0;
         i < scell->lte_neighbour_list.count &&
         i < MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE;
         ++i)
    {
      if (scell->lte_neighbour_list.entries[i].frequency.earfcn == earfcn)
      {
        return scell->lte_neighbour_list.entries[i].thresh_high * 2;
      }
    }
  }

  MSG_GERAN_ERROR_1_G("THRESH_high for E-ARFCN %d not found, using 0", earfcn);
  return 0;
}

int16 rr_get_thresh_low_for_earfcn(
  lte_earfcn_t earfcn,
  const gas_id_t gas_id
)
{
  if (rr_is_camped(gas_id))
  {
    gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);
    uint32              i;

    for (i = 0;
         i < scell->lte_neighbour_list.count &&
         i < MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE;
         ++i)
    {
      if (scell->lte_neighbour_list.entries[i].frequency.earfcn == earfcn)
      {
        return (scell->lte_neighbour_list.entries[i].thresh_low * 2);
      }
    }
  }

  MSG_GERAN_ERROR_1_G("THRESH_low for E-ARFCN %d not found, using 0", earfcn);
  return 0;
}

boolean rr_utran_priority_parameters_ie_received(
  instances_T * si2quater_instances,
  const gas_id_t  gas_id
)
{
  uint32 i;
  SI2q_MI_instance_T * instance;

  ASSERT(si2quater_instances != NULL);

  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    instance = &si2quater_instances->instance[i];

    if (instance->valid_data)
    {
      SI2q_params_T * si2quater = &instance->si2q_mi.si2q_params;

      if (si2quater->priority_and_eutran_params_desc != NULL)
      {
        if (si2quater->priority_and_eutran_params_desc->utran_priority_params != NULL)
        {
          return TRUE; 
        }        
      }
    }
  }
  return FALSE;
}

boolean rr_priority_and_threshold_information_available(
  instances_T * si2quater_instances,
  const gas_id_t  gas_id //tlx
)
{
  uint32 i;
  SI2q_MI_instance_T * instance;

  ASSERT(si2quater_instances != NULL);

  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    instance = &si2quater_instances->instance[i];

    if (instance->valid_data)
    {
      SI2q_params_T * si2quater = &instance->si2q_mi.si2q_params;

      if (si2quater->priority_and_eutran_params_desc != NULL)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}


boolean rr_serving_cell_priority_parameters_ie_received(
  gprs_scell_info_t * scell
)
{
  RR_NULL_CHECK_RETURN_PARAM(scell, FALSE);

  return scell->serving_cell_priority_params_valid;
} 


#endif /* FEATURE_LTE */


boolean rr_rat_pri_list_info_gsm_only(
  sys_rat_pri_list_info_s_type * rat_pri_list_info
)
{
  uint32 i;

  ASSERT(rat_pri_list_info != NULL);

  for (i = 0; i < rat_pri_list_info->num_items && i < SYS_MAX_ACQ_PREF_RAT_LIST_NUM; ++i)
  {
    if (rat_pri_list_info->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_WCDMA ||
        rat_pri_list_info->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE ||
        rat_pri_list_info->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_TDS )
    {
      return FALSE;
    }
  }

  return TRUE;
}

boolean rr_rat_pri_list_info_is_sys_mode_present(
  sys_rat_pri_list_info_s_type * rat_pri_list_info,
  sys_sys_mode_e_type            sys_mode
)
{
  uint32 i;

  ASSERT(rat_pri_list_info != NULL);

  for (i = 0; i < rat_pri_list_info->num_items && i < SYS_MAX_ACQ_PREF_RAT_LIST_NUM; ++i)
  {
    if (rat_pri_list_info->priority_list_info[i].acq_sys_mode == sys_mode)
    {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * Finds the requested sys_mode entry in priority_list_info
 * array.  If an entry can't be found NULL is returned
 *
 * @param rat_pri_list_info_ptr
 *        sys_mode - requested sys type
 *
 * @return sys_priority_list_info_s_type*
 */
sys_priority_list_info_s_type* rr_rat_pri_list_info_get_sys_mode_entry(
  sys_rat_pri_list_info_s_type * rat_pri_list_info_ptr,
  sys_sys_mode_e_type            sys_mode
)
{
  uint32 i;

  ASSERT(rat_pri_list_info_ptr != NULL);

  for (i = 0; i < rat_pri_list_info_ptr->num_items && i < SYS_MAX_ACQ_PREF_RAT_LIST_NUM; ++i)
  {
    if (rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == sys_mode)
    {
      return &rat_pri_list_info_ptr->priority_list_info[i];
    }
  }

  return NULL;
}

sys_band_cap_u_type rr_rat_pri_list_info_search_band_pref(
  sys_rat_pri_list_info_s_type * rat_pri_list_info,
  sys_sys_mode_e_type            sys_mode
)
{
  sys_band_cap_u_type band_cap_u = {0};
  uint32 i;

  ASSERT(rat_pri_list_info != NULL);

  for (i = 0; i < rat_pri_list_info->num_items && i < SYS_MAX_ACQ_PREF_RAT_LIST_NUM; ++i)
  {
    if (rat_pri_list_info->priority_list_info[i].acq_sys_mode == sys_mode)
    {
      if (rat_pri_list_info->priority_list_info[i].bst_rat_acq_required)
      {
        band_cap_u = rat_pri_list_info->priority_list_info[i].bst_band_cap;
      }
      else
      {
        band_cap_u = rat_pri_list_info->priority_list_info[i].band_cap;
      }
      
      break;
    }
  }

  return band_cap_u;
}

sys_band_cap_u_type rr_rat_pri_list_info_band_pref(
  sys_rat_pri_list_info_s_type * rat_pri_list_info,
  sys_sys_mode_e_type            sys_mode
)
{
  sys_band_cap_u_type band_cap_u = {0};
  uint32 i;

  ASSERT(rat_pri_list_info != NULL);

  for (i = 0; i < rat_pri_list_info->num_items && i < SYS_MAX_ACQ_PREF_RAT_LIST_NUM; ++i)
  {
    if (rat_pri_list_info->priority_list_info[i].acq_sys_mode == sys_mode)
    {
      band_cap_u = rat_pri_list_info->priority_list_info[i].band_cap;
      break;
    }
  }

  return band_cap_u;
}

sys_rat_pri_list_info_s_type * rr_rat_pri_list_info(
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  return &plmn_request_details->rat_pri_list_info;
}

boolean rr_chk_if_equal_lte_band_mask(sys_lte_band_mask_e_type mask1, sys_lte_band_mask_e_type mask2)
{
  return SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&mask1, &mask2);
}

boolean rr_is_lte_band_mask_empty(sys_lte_band_mask_e_type band_mask)
{
  return SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&band_mask);
}

boolean rr_is_band_mask_empty(sys_band_cap_u_type band_mask, sys_sys_mode_e_type sys_mode)
{
  boolean is_empty = FALSE;
  
  if (sys_mode == SYS_SYS_MODE_LTE)
  {
    is_empty = rr_is_lte_band_mask_empty(band_mask.lte_band_cap);
  }
  else
  {
    is_empty = (band_mask.chgwt_band_cap == SYS_BAND_MASK_EMPTY);
  }

  return is_empty;
}

boolean rr_gsm_only(
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  ASSERT(plmn_request_details != NULL);
  return rr_rat_pri_list_info_gsm_only(&plmn_request_details->rat_pri_list_info);
}

sys_band_mask_type rr_band_pref(const gas_id_t gas_id)
{
  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  sys_band_cap_u_type band_cap_u = {0};
  
  RR_NULL_CHECK_FATAL(plmn_request_details);
  
  band_cap_u = rr_rat_pri_list_info_band_pref(
    &plmn_request_details->rat_pri_list_info,
    SYS_SYS_MODE_GSM
  );

  return (band_cap_u.chgwt_band_cap & RR_SYS_BAND_MASK_GSM_SUPPORTED_BANDS);
}

sys_band_mask_type rr_search_band_pref(const gas_id_t gas_id)
{
  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  sys_band_cap_u_type band_cap_u = {0};

  RR_NULL_CHECK_FATAL(plmn_request_details);

  band_cap_u = rr_rat_pri_list_info_search_band_pref(
    &plmn_request_details->rat_pri_list_info,
    SYS_SYS_MODE_GSM
  );

  return (band_cap_u.chgwt_band_cap & RR_SYS_BAND_MASK_GSM_SUPPORTED_BANDS);
}

sys_band_mask_type rr_band_pref_wcdma(const gas_id_t gas_id)
{
  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  sys_band_cap_u_type band_cap_u = {0};
  
  RR_NULL_CHECK_FATAL(plmn_request_details);
  
  band_cap_u = rr_rat_pri_list_info_band_pref(
    &plmn_request_details->rat_pri_list_info,
    SYS_SYS_MODE_WCDMA
  );

  return band_cap_u.chgwt_band_cap;
}

sys_band_mask_type rr_band_pref_utran_tdd(const gas_id_t gas_id)
{
  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  sys_band_cap_u_type band_cap_u = {0};
  
  RR_NULL_CHECK_FATAL(plmn_request_details);
  
  band_cap_u = rr_rat_pri_list_info_band_pref(
    &plmn_request_details->rat_pri_list_info,
    SYS_SYS_MODE_TDS
  );

  return band_cap_u.chgwt_band_cap;
}

boolean rr_is_utran_tdd_enabled_in_nw_plmn(
  const gas_id_t gas_id
)
{
#ifdef FEATURE_GSM_TDS
  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  boolean tdd_enabled = FALSE;
  ASSERT(plmn_request_details != NULL);
  ASSERT(scell_info_ptr != NULL);

  if ( rr_rat_pri_list_info_is_sys_mode_present(
       &plmn_request_details->rat_pri_list_info,
        SYS_SYS_MODE_TDS
     ) == TRUE )
  {
    if (scell_info_ptr->gsm.valid_data_flag ||
        scell_info_ptr->gsm.valid_dedicated_data_flag)
    {
      if (tds_rrc_rat_is_supported(scell_info_ptr->gsm.location_area_identification.plmn_id))
      {
        tdd_enabled = TRUE;
      }
    }
    else
    {
      tdd_enabled = TRUE;
    }
  }

  return tdd_enabled;
#else
  return FALSE;
#endif /*FEATURE_GSM_TDS*/
}


boolean rr_is_utran_tdd_enabled(
  const gas_id_t gas_id
)
{
  boolean tdd_enabled = FALSE;

#ifdef FEATURE_GSM_TDS
  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  ASSERT(plmn_request_details != NULL);

  tdd_enabled =  rr_rat_pri_list_info_is_sys_mode_present(
       &plmn_request_details->rat_pri_list_info,
        SYS_SYS_MODE_TDS
  );
#else
  tdd_enabled = FALSE;
#endif /*FEATURE_GSM_TDS*/

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA) == FALSE)
  {
    MSG_MED("TDSCDMA segment not loaded",0,0,0);
    tdd_enabled = FALSE; 
  }
  
  return tdd_enabled;
}

boolean rr_is_wcdma_enabled(const gas_id_t gas_id)
{
  boolean wcdma_enabled = FALSE;

  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  ASSERT(plmn_request_details != NULL);

  wcdma_enabled = rr_rat_pri_list_info_is_sys_mode_present(
    &plmn_request_details->rat_pri_list_info,
    SYS_SYS_MODE_WCDMA
  );

  if (rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA) == FALSE)
  {
    MSG_MED("WCDMA segment not loaded",0,0,0);
    wcdma_enabled = FALSE; 
  }

  return wcdma_enabled;
}

#ifdef FEATURE_LTE
sys_lte_band_mask_e_type rr_band_pref_lte(const gas_id_t gas_id)
{
  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  sys_band_cap_u_type band_cap_u = {0};
  
  RR_NULL_CHECK_FATAL(plmn_request_details);
  
  band_cap_u = rr_rat_pri_list_info_band_pref(
    &plmn_request_details->rat_pri_list_info,
    SYS_SYS_MODE_LTE
  );

  return band_cap_u.lte_band_cap;
}

boolean rr_is_lte_enabled(
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T * plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
  ASSERT(plmn_request_details != NULL);

  return rr_rat_pri_list_info_is_sys_mode_present(
    &plmn_request_details->rat_pri_list_info,
    SYS_SYS_MODE_LTE
  );
}

#endif /* FEATURE_LTE */

sys_band_T rr_map_sys_band_class_to_sys_band(
  sys_sband_e_type sys_sband
)
{
  switch (sys_sband)
  {
    case SYS_SBAND_GSM_850:      return SYS_BAND_CELL_850;
    case SYS_SBAND_GSM_EGSM_900: return SYS_BAND_EGSM_900;
    case SYS_SBAND_GSM_PGSM_900: return SYS_BAND_PGSM_900;
    case SYS_SBAND_GSM_DCS_1800: return SYS_BAND_DCS_1800;
    case SYS_SBAND_GSM_PCS_1900: return SYS_BAND_PCS_1900;
    default:
      MSG_GERAN_ERROR_1("Unknown sys_sband: 0x%08x", sys_sband);
      return SYS_BAND_NONE;
  }
}

sys_sband_e_type rr_map_sys_band_to_sys_band_class(
  sys_band_T sys_band
)
{
  switch (sys_band)
  {
    case SYS_BAND_CELL_850: return SYS_SBAND_GSM_850;
    case SYS_BAND_EGSM_900: return SYS_SBAND_GSM_EGSM_900;
    case SYS_BAND_PGSM_900: return SYS_SBAND_GSM_PGSM_900;
    case SYS_BAND_DCS_1800: return SYS_SBAND_GSM_DCS_1800;
    case SYS_BAND_PCS_1900: return SYS_SBAND_GSM_PCS_1900;
    default:
      MSG_GERAN_ERROR_1("Unknown sys_band_T: 0x%08x", sys_band);
      return SYS_SBAND_BC0_A;
  }
}

#ifdef FEATURE_LTE

static uint32 rr_t3230_timeout_ms(uint8 t3230_timeout_value)
{
  switch (t3230_timeout_value)
  {
    case 0:  return 5 * 60 * 1000;
    case 1:  return 10 * 60 * 1000;
    case 2:  return 20 * 60 * 1000;
    case 3:  return 30 * 60 * 1000;
    case 4:  return 60 * 60 * 1000;
    case 5:  return 120 * 60 * 1000;
    case 6:  return 180 * 60 * 1000;
    default: return 180 * 60 * 1000;
  }
}

/**
  @brief Stores the Individual Priorities IE received in CHANNEL RELEASE
         message and starts T3230 if appropriate.

  @param individual_priorities_ie Pointer to the Individual Priorities IE.
  @param scell_arfcn The GSM ARFCN that the Individual Priorities IE was
                     received on.
*/
void rr_individual_priorities_store(
  rr_individual_priorities_ie_t * individual_priorities_ie,
  ARFCN_T                         scell_arfcn,
  sys_plmn_id_s_type              plmn,
  const gas_id_t gas_id //tlx
)
{
  rr_individual_priorities_t ** individual_priorities = rr_individual_priorities(gas_id);

  /* Validate input parameters. */
  RR_NULL_CHECK_RETURN_VOID(individual_priorities_ie);
  RR_NULL_CHECK_RETURN_VOID(individual_priorities);

  MSG_GERAN_HIGH_0("Storing Individual Priorities");

  if (*individual_priorities != NULL)
  {
    memset(*individual_priorities, 0, sizeof(rr_individual_priorities_t));
  }
  else
  {
    *individual_priorities = GPRS_MEM_CALLOC(sizeof(rr_individual_priorities_t));
    RR_NULL_CHECK_RETURN_VOID(*individual_priorities);
  }

  (*individual_priorities)->plmn = plmn;

  /* Set up the GERAN priorities based on the serving cell ARFCN. */
  (*individual_priorities)->dedicated_priority_list.num_geran_freq = 1;
  (*individual_priorities)->dedicated_priority_list.geran_freq_list[0].band = rr_map_sys_band_to_sys_band_class(scell_arfcn.band);
  (*individual_priorities)->dedicated_priority_list.geran_freq_list[0].arfcn = scell_arfcn.num;
  (*individual_priorities)->dedicated_priority_list.geran_freq_list[0].priority = individual_priorities_ie->geran_priority;

  /* Store UTRAN priorities if present. */
  if (individual_priorities_ie->utran_individual_priority_parameters_description != NULL)
  {
    uint32  i;

    for (i = 0;
         i < individual_priorities_ie->utran_individual_priority_parameters_description->num_utra_fdd_freq &&
         i < LTE_IRAT_MAX_UTRAN_FREQ;
         ++i)
    {
      (*individual_priorities)->dedicated_priority_list.utra_fdd_freq_list[i] =
        individual_priorities_ie->utran_individual_priority_parameters_description->utra_fdd_freq_list[i];
      }
    (*individual_priorities)->dedicated_priority_list.num_utra_fdd_freq = i;

    for (i = 0;
         i < individual_priorities_ie->utran_individual_priority_parameters_description->num_utra_tdd_freq &&
         i < LTE_IRAT_MAX_UTRAN_FREQ;
         ++i)
    {
      (*individual_priorities)->dedicated_priority_list.utra_tdd_freq_list[i] =
        individual_priorities_ie->utran_individual_priority_parameters_description->utra_tdd_freq_list[i];
    }
    (*individual_priorities)->dedicated_priority_list.num_utra_tdd_freq = i;

    if (individual_priorities_ie->utran_individual_priority_parameters_description->default_utran_priority_valid)
    {
      (*individual_priorities)->default_utran_priority_valid = TRUE;
      (*individual_priorities)->default_utran_priority = individual_priorities_ie->utran_individual_priority_parameters_description->default_utran_priority;
    }
  }

  /* Store E-UTRAN priorities if present. */
  if (individual_priorities_ie->eutran_individual_priority_parameters_description != NULL)
  {
    uint32  i;

    for (i = 0;
         i < individual_priorities_ie->eutran_individual_priority_parameters_description->num_eutra_freq &&
         i < LTE_IRAT_MAX_UTRAN_FREQ;
         ++i)
    {
      (*individual_priorities)->dedicated_priority_list.eutra_freq_list[i] =
        individual_priorities_ie->eutran_individual_priority_parameters_description->eutra_freq_list[i];
      }
    (*individual_priorities)->dedicated_priority_list.num_eutra_freq = i;

    if (individual_priorities_ie->eutran_individual_priority_parameters_description->default_eutran_priority_valid)
    {
      (*individual_priorities)->default_eutran_priority_valid = TRUE;
      (*individual_priorities)->default_eutran_priority = individual_priorities_ie->eutran_individual_priority_parameters_description->default_eutran_priority;
    }
  }

  if (individual_priorities_ie->t3230_timeout_value_valid)
  {
    MSG_GERAN_HIGH_1("Starting T3230 (%d ms)", rr_t3230_timeout_ms(individual_priorities_ie->t3230_timeout_value));
    (void)rr_timer_start(T3230, rr_t3230_timeout_ms(individual_priorities_ie->t3230_timeout_value),gas_id);
  }

  lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_GERAN);
  rr_individual_priorities_dump(gas_id);
}

void rr_individual_priorities_clear(
  const gas_id_t gas_id
)
{
  if (geran_is_multi_mode_gas_id(gas_id))
  {
    rr_individual_priorities_t ** individual_priorities = rr_individual_priorities(gas_id);

    MSG_GERAN_HIGH_0_G("Clearing Individual Priorities");

    if (*individual_priorities != NULL)
    {
      GPRS_MEM_FREE(*individual_priorities);
      *individual_priorities = NULL;
    }

    (void)rr_timer_cancel(T3230, gas_id);

    /* If GERAN is the current owner of the dedicated priorities then LTE RRC
    API should be informed that they are no longer valid.  If GERAN is not the
    current owner this indicates that GERAN only had a temporary copy of the
    priorities (e.g. received at the start of an X2G procedure that subsequently
    failed) so the priorities were never properly owned by GERAN, so the
    ownership does not need updating. */
    if (lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_GERAN)
    {
      lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_NONE);
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("GERAN AS not multimode: request to Clearing Individual Priorities N/A ");               
  }

  return;
}

boolean rr_individual_priorities_valid(const gas_id_t gas_id)
{
  rr_individual_priorities_t ** individual_priorities = rr_individual_priorities(gas_id);

  if (individual_priorities != NULL && *individual_priorities != NULL)
  {
    MSG_GERAN_MED_0_G("Individual priorities valid");
    return TRUE;
  }
  else
  {
    MSG_GERAN_MED_0_G("Individual priorities not valid");
    return FALSE;
  }
}

/**
  @brief Checks whether the dedicated priority information is valid
         or not.
*/
boolean rr_dedicated_priority_info_valid(
  lte_irat_dedicated_priority_info_s * dedicated_priority_info
)
{
  /* Validate input parameters. */
  RR_NULL_CHECK_RETURN_PARAM(dedicated_priority_info, FALSE);

  if (dedicated_priority_info->rem_validity_timer_is_present == FALSE &&
      dedicated_priority_info->priority_list.num_1x_band_class == 0 &&
      dedicated_priority_info->priority_list.num_eutra_freq == 0 &&
      dedicated_priority_info->priority_list.num_geran_freq == 0 &&
      dedicated_priority_info->priority_list.num_hrdp_band_class == 0 &&
      dedicated_priority_info->priority_list.num_utra_fdd_freq == 0 &&
      dedicated_priority_info->priority_list.num_utra_tdd_freq == 0)
  {
    MSG_GERAN_MED_0("dedicated priority info not valid");
    return FALSE;
  }
  else
  {
    MSG_GERAN_MED_0("dedicated priority info valid");
    return TRUE;
  }
}

/**
  @brief Dumps the Information in the Individual Priority database.
*/
void rr_individual_priorities_dump(const gas_id_t gas_id)
{
  rr_individual_priorities_t ** individual_priorities = rr_individual_priorities(gas_id);
  lte_irat_dedicated_priority_list_s priority_list;// = (*individual_priorities)->dedicated_priority_list;
  PLMN_id_T plmn_id;
  uint32 i, max_elems_to_dump = 5; /* dump only the first five entries in each RAT*/
  
  RR_NULL_CHECK_RETURN_VOID(individual_priorities);

  if (*individual_priorities == NULL)
  {
    MSG_GERAN_MED_0_G("Individual priorities are not valid");
    return;
  }
  else
  {
    priority_list = (*individual_priorities)->dedicated_priority_list;
  }
  MSG_GERAN_HIGH_0_G("Individual Priorities db contents(only first 5 entries max. in each RAT):");

  plmn_id = (*individual_priorities)->plmn;
  MSG_GERAN_MED_1_G("PLMN Id:=%6x",((plmn_id.identity[0]<<16) +(plmn_id.identity[1]<<8) + (plmn_id.identity[2])));
  MSG_GERAN_MED_3_G("Default priority Validity GERAN:%d UTRAN:%d EUTRAN:%d",
                                  (*individual_priorities)->default_geran_priority_valid,
                                  (*individual_priorities)->default_utran_priority_valid,
                                  (*individual_priorities)->default_eutran_priority_valid);

  MSG_GERAN_MED_3_G("Default priority GERAN:%d UTRAN:%d EUTRAN:%d",
                                  (*individual_priorities)->default_geran_priority,
                                  (*individual_priorities)->default_utran_priority,
                                  (*individual_priorities)->default_eutran_priority);
  
  MSG_GERAN_MED_4_G("Num of freq: GERAN:%d UTRAN_FDD:%d UTRAN_TDD:%d EUTRAN:%d",
                                  priority_list.num_geran_freq, priority_list.num_utra_fdd_freq,
                                  priority_list.num_utra_tdd_freq, priority_list.num_eutra_freq);

  for(i=0;i<priority_list.num_geran_freq && i < max_elems_to_dump;i++)
  {
    MSG_GERAN_MED_4_G("  GERAN: %d: ARFCN=%d, Band=%d, Priority=%d",i,
      priority_list.geran_freq_list[i].arfcn,priority_list.geran_freq_list[i].band,priority_list.geran_freq_list[i].priority);
  }

  for(i=0;i<priority_list.num_utra_fdd_freq && i < max_elems_to_dump;i++)
  {
    MSG_GERAN_MED_4_G("  UTRA_FDD: %d: ARFCN=%d, Band=%d, Priority=%d",i,
      priority_list.utra_fdd_freq_list[i].arfcn,priority_list.utra_fdd_freq_list[i].band,priority_list.utra_fdd_freq_list[i].priority);
  }

  for(i=0;i<priority_list.num_utra_tdd_freq && i < max_elems_to_dump;i++)
  {
    MSG_GERAN_MED_4_G("  UTRA_TDD: %d: ARFCN=%d, Band=%d, Priority=%d",i,
      priority_list.utra_tdd_freq_list[i].arfcn,priority_list.utra_tdd_freq_list[i].band,priority_list.utra_tdd_freq_list[i].priority);
  }

  for(i=0;i<priority_list.num_eutra_freq && i < max_elems_to_dump;i++)
  {
    MSG_GERAN_MED_4_G("  EUTRA: %d: ARFCN=%d, Band=%d, Priority=%d",i,
      priority_list.eutra_freq_list[i].arfcn,priority_list.eutra_freq_list[i].band,priority_list.eutra_freq_list[i].priority);
  }
}

/**
  @brief Stores Dedicated Priority Information received from another RAT
         as Individual Priority Information.
*/
void rr_dedicated_priority_info_store(
  lte_irat_dedicated_priority_info_s * dedicated_priority_info,
  ARFCN_T                            * arfcn,
  sys_plmn_id_s_type                   plmn,
  const gas_id_t                       gas_id
)
{
  rr_individual_priorities_t ** individual_priorities = rr_individual_priorities(gas_id);

  /* Validate input parameters. */
  RR_NULL_CHECK_RETURN_VOID(dedicated_priority_info);
  RR_NULL_CHECK_RETURN_VOID(individual_priorities);

  if (rr_dedicated_priority_info_valid(dedicated_priority_info))
  {
    MSG_GERAN_HIGH_0_G("Storing Dedicated Priorities");

    if (*individual_priorities != NULL)
    {
      memset(*individual_priorities, 0, sizeof(rr_individual_priorities_t));
    }
    else
    {
      *individual_priorities = GPRS_MEM_CALLOC(sizeof(rr_individual_priorities_t));
      RR_NULL_CHECK_RETURN_VOID(*individual_priorities);
    }

    (*individual_priorities)->plmn = plmn;
    (*individual_priorities)->dedicated_priority_list = dedicated_priority_info->priority_list;

    if (arfcn != NULL)
    {
      uint32 i;

      /* We need to search the list of GERAN priorities for one that matches
      the given ARFCN, then use that priority as the priority for all GERAN
      cells. */
      for (i = 0;
           i < dedicated_priority_info->priority_list.num_geran_freq &&
           i < LTE_IRAT_MAX_GERAN_FREQ;
           ++i)
      {
        ARFCN_T tmp;

        tmp.num = dedicated_priority_info->priority_list.geran_freq_list[i].arfcn;
        tmp.band = rr_map_sys_band_class_to_sys_band(
          dedicated_priority_info->priority_list.geran_freq_list[i].band
        );

        if (rr_arfcn_compare_freq(tmp, *arfcn))
        {
          (*individual_priorities)->default_geran_priority_valid = TRUE;
          (*individual_priorities)->default_geran_priority = dedicated_priority_info->priority_list.geran_freq_list[i].priority;
        }
      }
    }

    if (dedicated_priority_info->rem_validity_timer_is_present)
    {
      MSG_GERAN_HIGH_1_G("Starting T3230 (%d ms)", dedicated_priority_info->rem_validity_timer);
      (void) rr_timer_start(T3230, (uint32) dedicated_priority_info->rem_validity_timer, gas_id);
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Dedicated Priorities not present");
  }

  rr_individual_priorities_dump(gas_id);
}

sys_plmn_id_s_type rr_dedicated_priority_info_get_plmn(
  const gas_id_t gas_id
)
{
  rr_individual_priorities_t ** individual_priorities = rr_individual_priorities(gas_id);
  sys_plmn_id_s_type            plmn;

  if (*individual_priorities != NULL)
  {
    plmn = (*individual_priorities)->plmn;
  }
  else
  {
    sys_plmn_undefine_plmn_id(&plmn);
  }
  return plmn;
}

void rr_dedicated_priority_info_get(lte_irat_dedicated_priority_info_s * dst, const gas_id_t gas_id)
{
  rr_individual_priorities_t ** individual_priorities = rr_individual_priorities(gas_id);

  /* Validate input parameters. */
  RR_NULL_CHECK_RETURN_VOID(dst);

  memset(dst, 0, sizeof(lte_irat_dedicated_priority_info_s));

  if (*individual_priorities != NULL)
  {
   dst->priority_list = (*individual_priorities)->dedicated_priority_list;

    if (rr_timer_is_running(T3230, gas_id))
    {
      dst->rem_validity_timer_is_present = TRUE;
      dst->rem_validity_timer = rr_timer_get_time_remaining(T3230, gas_id);
    }
  }
}

/**
  @brief Request the specified RAT to clear it's dedicated priorities.

  If owner is LTE_RRC_DEDICATED_PRI_OWNER_LTE, sends LTE_RRC_CLEAR_DEDICATED_PRI_IND.
  If owner is LTE_RRC_DEDICATED_PRI_OWNER_WCDMA, sends XXX.

  @param owner The RAT to send the request to.
*/
void rr_dedicated_priorities_request_owner_clears(
  lte_rrc_dedicated_pri_owner_e owner,
  const gas_id_t gas_id
)
{
  MSG_GERAN_MED_1_G("rr_dedicated_priorities_owner_clears: owner=%d", owner);

  switch (owner)
  {
    case LTE_RRC_DEDICATED_PRI_OWNER_LTE:
    {
      lte_rrc_clear_dedicated_pri_ind_s ind;
      errno_enum_type                   result;

      memset(&ind, 0, sizeof(lte_rrc_clear_dedicated_pri_ind_s));

      msgr_init_hdr(
        &ind.msg_hdr,
        MSGR_GERAN_GRR,
        LTE_RRC_CLEAR_DEDICATED_PRI_IND
      );

      result = rr_msgr_send(&ind.msg_hdr, sizeof(lte_rrc_clear_dedicated_pri_ind_s), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      break;
    } /* LTE_RRC_DEDICATED_PRI_OWNER_LTE */

#ifdef FEATURE_WCDMA
    case LTE_RRC_DEDICATED_PRI_OWNER_WCDMA:
    {
      rr_send_rrc_clear_dedicated_priorities_ind(gas_id);
      break;
    } /* LTE_RRC_DEDICATED_PRI_OWNER_WCDMA */
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
    case LTE_RRC_DEDICATED_PRI_OWNER_TDSCDMA:
    {
      rr_send_tds_clear_dedicated_priorities_ind(gas_id);
      break;
    }
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/
    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected dedicated_pri_owner: %d", owner);
    }
  }
}

void rr_dedicated_priorities_take_ownership(
  const gas_id_t gas_id
)
{
  if (geran_is_multi_mode_gas_id(gas_id))
  {
    if (rr_individual_priorities_valid(gas_id))
    {
      lte_rrc_dedicated_pri_owner_e dedicated_pri_owner = lte_rrc_dedicated_pri_get_owner();

      MSG_GERAN_MED_1_G("rr_dedicated_priorities_take_ownership: Existing owner=%d",dedicated_pri_owner);

      if (dedicated_pri_owner != LTE_RRC_DEDICATED_PRI_OWNER_GERAN)
      {
        lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_GERAN);

        rr_dedicated_priorities_request_owner_clears(dedicated_pri_owner, gas_id);
      }
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("GERAN AS not multimode: request to Dedicated priorities N/A ");               
  }

  return;
}

void rr_dedicated_priorities_request_from_owner(
  const gas_id_t gas_id
)
{
  if (geran_is_multi_mode_gas_id(gas_id))
  {
    lte_rrc_dedicated_pri_owner_e dedicated_pri_owner = lte_rrc_dedicated_pri_get_owner();

    if (dedicated_pri_owner != LTE_RRC_DEDICATED_PRI_OWNER_GERAN)
    {
      switch (dedicated_pri_owner)
      {
        case LTE_RRC_DEDICATED_PRI_OWNER_LTE:
        {
          lte_rrc_get_dedicated_pri_req_s req;
          errno_enum_type                 result;

          memset(&req, 0, sizeof(lte_rrc_get_dedicated_pri_req_s));

          msgr_init_hdr(
            &req.msg_hdr,
            MSGR_GERAN_GRR,
            LTE_RRC_GET_DEDICATED_PRI_REQ
          );

          MSG_GERAN_MED_0_G("Requesting dedicated priorities from LTE");

          result = rr_msgr_send(&req.msg_hdr, sizeof(lte_rrc_get_dedicated_pri_req_s), gas_id);
          if (result != E_SUCCESS)
          {
            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
          }
          break;
        } /* LTE_RRC_DEDICATED_PRI_OWNER_LTE */

#ifdef FEATURE_WCDMA
        case LTE_RRC_DEDICATED_PRI_OWNER_WCDMA:
        {
          MSG_GERAN_MED_0_G("Requesting dedicated priorities from WCDMA");
          rr_send_rrc_dedicated_priorities_req(gas_id);
          break;
        } /* LTE_RRC_DEDICATED_PRI_OWNER_WCDMA */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
        case LTE_RRC_DEDICATED_PRI_OWNER_TDSCDMA:
        {
          MSG_GERAN_MED_0_G("Requesting dedicated priorities from TDSCDMA");
          rr_send_tds_dedicated_priorities_req(gas_id);
          break;
        }
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/
        default:
        {
          MSG_GERAN_MED_0_G("No dedicated priorities");
        }
      }
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("GERAN AS not multimode: Requesting dedicated priorities N/A ");               
  }

  return;
}

rr_resel_priority_e rr_geran_priority(
  const gas_id_t gas_id
)
{
  if (rr_is_camped(gas_id))
  {
    gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

    /* First, check for Individual Priority information. */
    rr_individual_priorities_t ** individual_priorities = rr_individual_priorities(gas_id);

    if (individual_priorities != NULL && *individual_priorities != NULL)
    {
      /* Individual Priority information is present, check for priority. */
      if ((*individual_priorities)->default_geran_priority_valid)
      {
        return (*individual_priorities)->default_geran_priority;
      }
      else
      {
        ARFCN_T arfcn = rr_gprs_get_scell_arfcn(gas_id);
        uint32 i;

        for (i = 0; i < (*individual_priorities)->dedicated_priority_list.num_geran_freq && i < LTE_IRAT_MAX_GERAN_FREQ; ++i)
        {
          if ((*individual_priorities)->dedicated_priority_list.geran_freq_list[i].arfcn == arfcn.num &&
              (*individual_priorities)->dedicated_priority_list.geran_freq_list[i].band == rr_map_sys_band_to_sys_band_class(arfcn.band))
          {
            return (*individual_priorities)->dedicated_priority_list.geran_freq_list[i].priority;
          }
        }
      }
    }

    /* If Individual Priorities are not valid or do not contain an entry for
    the serving cell just return the broadcast value for now. */
    return scell->serving_cell_priority_params.geran_priority; 
  }
  else
  {
    return RR_RESEL_PRIORITY_UNDEFINED;
  }
}

#endif /* FEATURE_LTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

/**
  @brief Determine the type of the SI message.

  @param si_message The SI message (layer 3 message) to get the type of.
  @param si_message_type Pointer to the variable in which the message
         type should be stored.

  @return TRUE if the value of si_message_type is valid, FALSE if it is
          not.
*/
boolean rr_x2g_si_message_type(
  geran_grr_si_message_t * si_message,
  uint8                  * si_message_type
)
{
  RR_NULL_CHECK_RETURN_PARAM(si_message, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(si_message_type, FALSE);

  /* For BCCH messages (the only type we get in this container), message type
  is stored in index 2. */
  if (si_message->length >= 3)
  {
    *si_message_type = si_message->data[2];
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
  @brief Determine the order value for the specified SI message.

  This value is used to determine the sort order of SI messages received
  in SI containers and sent in to RR.

  @param si_message the message to get the order value for.

  @return the order value for the message.
*/
static unsigned int rr_x2g_si_message_order_value(
  geran_grr_si_message_t * si_message
)
{
  uint8 si_message_type = 0;
  RR_NULL_CHECK_RETURN_PARAM(si_message, 0);

  if (rr_x2g_si_message_type(si_message, &si_message_type))
  {
    uint32 i;
    for (i = 0; i < sizeof(rr_x2g_si_message_order); ++i)
    {
      if (rr_x2g_si_message_order[i] == si_message_type)
      {
        return i;
      }
    }
  }

  return 0;
}




/**
  @brief Comparison function for qsort.

  Sorts SI messages based on their relative order, defined by the function
  rr_x2g_si_message_order_value().
*/
static int rr_x2g_si_message_cmp_fn(
  const void * a,
  const void * b
)
{
  geran_grr_si_message_t * aa;
  geran_grr_si_message_t * bb;

  RR_NULL_CHECK_RETURN_PARAM(a, 0);
  RR_NULL_CHECK_RETURN_PARAM(b, 0);

  aa = (geran_grr_si_message_t *) a;
  bb = (geran_grr_si_message_t *) b;

  if (rr_x2g_si_message_order_value(aa) <= rr_x2g_si_message_order_value(bb))
  {
    return -1;
  }
  else
  {
    return 1;
  }
}

static void rr_x2g_system_information_sort(
  rr_x2g_system_information_t * system_information
)
{
  RR_NULL_CHECK_RETURN_VOID(system_information);

  qsort(&system_information->messages[0],
        MIN(system_information->count, RR_X2G_SYSTEM_INFORMATION_MESSAGES_MAX),
        sizeof(geran_grr_si_message_t),
        rr_x2g_si_message_cmp_fn);
}

void rr_x2g_system_information_send(
  ARFCN_T                       arfcn,
  rr_x2g_system_information_t * system_information,
  const gas_id_t                gas_id
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(system_information);

  if (system_information->count > 0)
  {
    /* By default, we use TC=8 for all tunnelled SIs, however if SI1 has
    not been included in the SI container then we are allowed to assume
    that the cell does not broadcast it.  If this is the case, we set
    all TCs to 0 - this will trigger the SI handlers to detect that SI1
    is not broadcast. */
    uint8 tc = 8;
    uint8 first_si_message_type = 0;

    /* Firstly, we sort the SI messages in to a known order. */
    rr_x2g_system_information_sort(system_information);

    /* Now we know that if SI1 is available it should be in the first element
    of the array. */
    if (rr_x2g_si_message_type(&system_information->messages[0], &first_si_message_type))
    {
      if (first_si_message_type == SYSTEM_INFORMATION_TYPE_1)
      {
        MSG_GERAN_HIGH_0_G("SI1 is broadcast");
      }
      else
      {
        MSG_GERAN_HIGH_0_G("SI1 not broadcast");
        tc = 0;
      }
    }

    for (i = 0;
         i < system_information->count &&
         i < RR_X2G_SYSTEM_INFORMATION_MESSAGES_MAX;
         ++i)
    {
      /* Send the message in a DL_UNIT_DATA_IND, as if it arrived on the
      appropriate channel. */
      dl_unit_data_ind_T dl_unit_data_ind;
      dl_unit_data_ind.message_header.message_set = MS_RR_L2;
      dl_unit_data_ind.message_header.message_id = DL_UNIT_DATA_IND;
      dl_unit_data_ind.SAPI = SAPI0;
      dl_unit_data_ind.l2_channel_type = BCCH;
      dl_unit_data_ind.own_paging_block = FALSE;
      dl_unit_data_ind.tc = tc;
      dl_unit_data_ind.channel_number = arfcn;
      /* L2 always sets the length to 23 bytes for BCCH messages. */
      dl_unit_data_ind.l3_message_length = N201_MAX;
      dl_unit_data_ind.short_header = FALSE;
      dl_unit_data_ind.gas_id = gas_id;
      memset(&dl_unit_data_ind.layer3_message, 0x2b, N201_MAX);
      memscpy(
         &dl_unit_data_ind.layer3_message,
         sizeof(dl_unit_data_ind.layer3_message),
         system_information->messages[i].data,
         MIN(system_information->messages[i].length, N201_MAX)
       );
	  

      PUT_IMH_LEN(sizeof(dl_unit_data_ind) - sizeof(IMH_T), &dl_unit_data_ind.message_header)

      (void) gs_send_message(GS_QUEUE_RR, &dl_unit_data_ind, TRUE);
    }
  }
}

boolean rr_x2g_system_information_headers_present(
  rr_x2g_system_information_t * system_information
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_PARAM(system_information, FALSE);

  /* Iterate over the system information messages provided.  Check
  for:

  Octet 1: Least significant 2 bits should equal 01b to indicate L2 Pseudo
           Length IEI
  Octet 2: This should equal 0x06 - RR Protocol Discriminator

  If either of these conditions are not met for all entries in the store,
  RR will assume that headers need to be prepended to all entries. */

  for (i = 0;
       i < system_information->count && i < RR_X2G_SYSTEM_INFORMATION_MESSAGES_MAX;
       ++i)
  {
    geran_grr_si_message_t * si_message = &system_information->messages[i];

    /* Check that the message is at least long enough to contain the header
    octets - if it is not, assume they must be added. */
    if (si_message->length > 2)
    {
      if (((si_message->data[0] & 0x03) != 0x01) ||
          (si_message->data[1] != 0x06))
      {
        return FALSE;
      }
    }
    else
    {
      return FALSE;
    }
  }

  return TRUE;
}

void rr_x2g_system_information_prepend_headers(
  rr_x2g_system_information_t * system_information,
  const gas_id_t                gas_id
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(system_information);

  for (i = 0;
       i < system_information->count && i < RR_X2G_SYSTEM_INFORMATION_MESSAGES_MAX;
       ++i)
  {
    geran_grr_si_message_t * si_message = &system_information->messages[i];
    uint8 l2_pseudo_length;

    if (si_message->length == 0)
    {
      MSG_GERAN_ERROR_0_G("SI message %d length 0, skipping");
      continue;
    }

    /* Assume that the first octet is the message type. */
    switch (si_message->data[0])
    {
      case SYSTEM_INFORMATION_TYPE_1:
      {
        l2_pseudo_length = (21 << 2) | 0x01;
        break;
      } /* SYSTEM_INFORMATION_TYPE_1 */

      case SYSTEM_INFORMATION_TYPE_2:
      {
        l2_pseudo_length = (22 << 2) | 0x01;
        break;
      } /* SYSTEM_INFORMATION_TYPE_2 */

      case SYSTEM_INFORMATION_TYPE_2_BIS:
      {
        l2_pseudo_length = (21 << 2) | 0x01;
        break;
      } /* SYSTEM_INFORMATION_TYPE_2_BIS */

      case SYSTEM_INFORMATION_TYPE_2_TER:
      {
        l2_pseudo_length = (18 << 2) | 0x01;
        break;
      } /* SYSTEM_INFORMATION_TYPE_2_TER */

      case SYSTEM_INFORMATION_TYPE_2_QUATER:
      {
        l2_pseudo_length = (1 << 2) | 0x01;
        break;
      } /* SYSTEM_INFORMATION_TYPE_2_QUATER */

      case SYSTEM_INFORMATION_TYPE_3:
      {
        l2_pseudo_length = (18 << 2) | 0x01;
        break;
      } /* SYSTEM_INFORMATION_TYPE_3 */

      case SYSTEM_INFORMATION_TYPE_4:
      {
        /* L2 Pseudo Length for SI4 is a little difficult to calculate as
        we need to know whether CBCH Channel Description and CBCH Mobile
        Allocation are present in the message.

        CBCH Channel Description is a TV IE with IEI 0x64.
        CBCH Mobile Allocation is a TLV with IEI 0x72.

        3GPP 44.018 states section 10.5.2.35 states:

        NOTE:  In the future evolution of this standard the values 64h and
        72h shall not be used as values of the first octet when this
        information element is used in the SYSTEM INFORMATION TYPE 4 message.
        This will prevent mobile stations misinterpreting this information as
        the CBCH IEIs.

        This means that we can just check whether the IEs are included based
        on the values of the SI message at the appropriate positions.
        */
        l2_pseudo_length = 11; /* Minimum length of the mandatory IEs. */
        if (si_message->data[l2_pseudo_length] == 0x64)
        {
          l2_pseudo_length += 4;
        }
        if (si_message->data[l2_pseudo_length] == 0x72)
        {
          l2_pseudo_length += si_message->data[l2_pseudo_length + 1];
        }
        l2_pseudo_length = (l2_pseudo_length << 2) | 0x01;
        break;
      } /* SYSTEM_INFORMATION_TYPE_4 */

      case SYSTEM_INFORMATION_TYPE_7:
      case SYSTEM_INFORMATION_TYPE_8:
      {
        l2_pseudo_length = (1 << 2) | 0x01;
        break;
      } /* SYSTEM_INFORMATION_TYPE_7 */

      case SYSTEM_INFORMATION_TYPE_13:
      {
        l2_pseudo_length = 0x01;
        break;
      } /* SYSTEM_INFORMATION_TYPE_13 */

      default:
      {
        MSG_GERAN_ERROR_1_G("Unknown message type: %d", si_message->data[0]);
        continue;
      }
    }

    /* If we've got this far, we know that the message is of a known type.  Now
    we need to move the first 21 bytes of the message along to make room for the
    two header bytes.  This may cause message truncation if the source message is
    (incorrectly) greater than 21 bytes long.  This is not a problem, as protocol
    checks when the message is processed will take care of this. */
    if (si_message->length > 21)
    {
      si_message->length = 21;
    }

    memsmove(&si_message->data[2],sizeof(si_message->data)-2, &si_message->data[0], 21);	

    si_message->data[0] = l2_pseudo_length;
    si_message->data[1] = 0x06; /* RR Protocol Discriminator */
    si_message->length += 2;
  }
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */


void rr_mm_send_message(
  IMH_T * p_message,
  gs_queue_id_T queue,
  const gas_id_t gas_id
)
{
  gs_status_T status;

  status = gs_send_message( queue, (void *)p_message, TRUE );

  if (status != GS_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("gs_send_message() failed (%d)", status);
  }
}

/*!
 * \brief Initialisation function. Called once at task start.
 */
void rr_general_task_start_init(
  void
)
{
  uint8 as_index;

  // Zero the control/data structure(s) (across all data-spaces)
  memset(rr_general_data, 0, sizeof(rr_general_data));
  memset(usat_system_information_database, 0, sizeof(usat_system_information_database));
  memset(forbidden_lai_list, 0, sizeof(forbidden_lai_list));

  // Setup any specific values not setup by the general module initialisation function

  for (as_index = 0; as_index < NUM_GERAN_DATA_SPACES; as_index++)
  {
    rr_general_data[as_index].S_counter = RR_DEF_RLT_COUNTER;
  }
}

void rr_plmn_selection_triggered(const gas_id_t gas_id)
{
  rr_params_mods_t ** params_mods = rr_params_mods(gas_id);
  rr_params_mods_t ** pcco_params_mods = rr_pcco_params_mods(gas_id);

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

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  rr_g2x_plmn_selection_triggered(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
}

void rr_cell_selection_triggered(const gas_id_t gas_id)
{
  rr_params_mods_t ** params_mods = rr_params_mods(gas_id);
  rr_params_mods_t ** pcco_params_mods = rr_pcco_params_mods(gas_id);

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

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  rr_g2x_cell_selection_triggered(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
}

/*!
 * \brief Wrapper function for EPLMN API. 
 */
boolean rr_eplmn_list_equivalent_plmn(sys_plmn_id_s_type plmn, const gas_id_t gas_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  sys_modem_as_id_e_type as_id = geran_map_gas_id_to_nas_id(gas_id);

#ifdef FEATURE_SGLTE
  if (rr_is_sglte_mode_active(gas_id))
  {
    return(sys_eplmn_list_equivalent_plmn_with_rat(plmn, as_id, SYS_RAT_GSM_RADIO_ACCESS));
  }
#endif /* FEATURE_SGLTE */
#else
#ifdef FEATURE_SGLTE
  if (rr_is_sglte_mode_active(gas_id))
  {
    return(sys_eplmn_list_equivalent_plmn_sglte(plmn, SYS_RAT_GSM_RADIO_ACCESS));
  }
#endif /* FEATURE_SGLTE */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  /* execution continues here if SGLTE is not active */
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  return(sys_eplmn_per_subs_list_equivalent_plmn(plmn, as_id));
#else
  return(sys_eplmn_list_equivalent_plmn(plmn));
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
}

/*!
 * \brief Wrapper function for EHPLMN API. 
 */
boolean rr_ehplmn_list_equivalent_plmn(sys_plmn_id_s_type plmn, const gas_id_t gas_id)
{
#ifdef FEATURE_EQUIVALENT_HPLMN
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  {
    sys_modem_as_id_e_type as_id = geran_map_gas_id_to_nas_id(gas_id);

    /* If the EHPLMN list is present, and not empty,then received SI3 
    PLMN id is searched in the EHPLMN list.if it is in the EHPLMN 	
    list then consider that cell as suitable and camp on it	*/
    return reg_sim_per_subs_is_ehplmn(plmn, as_id);
  }
#else /*defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)*/
  return reg_sim_is_ehplmn(plmn);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
#else /* FEATURE_EQUIVALENT_HPLMN */
  return FALSE;
#endif /* FEATURE_EQUIVALENT_HPLMN */
}

/*!
 * \brief Wrapper function for EHPLMN API. Checks if supplied PLMN id is in the EHPLMN list or not
 * 
 * \param plmn - plmn id received in the SI3 from cell
 *
 * \return boolean - TRUE if the PLMN is in the EHPLMN list or same as HPLMN , FALSE otherwise
 */
boolean rr_is_ehplmn_or_hplmn(sys_plmn_id_s_type plmn, const gas_id_t gas_id)
{
  boolean plmn_ok = FALSE;
	
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  sys_modem_as_id_e_type as_id = geran_map_gas_id_to_nas_id(gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef FEATURE_EQUIVALENT_HPLMN	
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* If the EHPLMN list is present, and not empty,then received SI3 
  PLMN id is searched in the EHPLMN list.if it is in the EHPLMN 	
  list then consider that cell as suitable and camp on it	*/
  plmn_ok = reg_sim_per_subs_is_ehplmn(plmn, as_id);
#else 
  plmn_ok = reg_sim_is_ehplmn(plmn);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  MSG_GERAN_HIGH_1_G("SI3 PLMN id is in EHPLMN list = %d ", (int)plmn_ok);
#else /* FEATURE_EQUIVALENT_HPLMN */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /*Get the IMSI-derived HPLMN from NAS and then compare it with the received 
  SI3 PLMN. If it matches then camp on it.The reason for doing is that 
  NAS can't read the EHPLMN as FEATURE_EQUIVALENT_HPLMN is not defined
  then GRR needs to get HPLMN from NAS.*/
	
  sys_plmn_id_s_type  hplmn = reg_sim_per_subs_hplmn_get(as_id);
#else 
  sys_plmn_id_s_type  hplmn = reg_sim_hplmn_get();
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM*/ 

  /* comparing retrieved hplmn with SI3 PLMN id */
  plmn_ok = sys_plmn_match(hplmn, plmn); 
  
  MSG_GERAN_HIGH_1_G("SI3 PLMN id is same as HPLMN = %d ",(int)plmn_ok);
#endif /* FEATURE_EQUIVALENT_HPLMN */
  return plmn_ok;
}

#ifdef FEATURE_VAMOS_II
channel_mode_T  rr_remove_vamos2_indication_from_channel_mode(channel_mode_T channel_mode)
{
  channel_mode_T converted_channel_mode = 0;

  switch (channel_mode)
  {
     case SPEECH_V1_VAMOS2:
       converted_channel_mode =  SPEECH_V1;
       break;

     case SPEECH_V2_VAMOS2:
       converted_channel_mode =  SPEECH_V2;
       break;

     case SPEECH_V3_VAMOS2:
       converted_channel_mode =  SPEECH_V3;
       break;

     case SPEECH_V5_VAMOS2:
       converted_channel_mode =  SPEECH_V5;
       break;

     default:
       converted_channel_mode =  channel_mode;
       break;
  }

  return converted_channel_mode;
}


boolean rr_vamos2_rcvd_chan_mode_matches_curr_chan_mode(
channel_mode_T rcvd_channel_mode,
channel_mode_T current_chan_mode
)
{

  boolean channel_modes_match = FALSE;
  channel_mode_T   rcvd_channel_mode_1 = rr_remove_vamos2_indication_from_channel_mode(rcvd_channel_mode);
  channel_mode_T   current_channel_mode_1 = rr_remove_vamos2_indication_from_channel_mode(current_chan_mode);

  MSG_MED("rcvd  AMR type = %d rcvd converted AMR type = %d",(int) rcvd_channel_mode,
            (int)rcvd_channel_mode_1,0);

  MSG_MED("current AMR type = %d current converted  AMR type = %d",(int) current_chan_mode,
            (int)current_channel_mode_1,0);

  if (rcvd_channel_mode_1 == current_channel_mode_1)  
  {
    /* multirate config can be inherited from the dedicated data */
    channel_modes_match = TRUE;
  }

  return(channel_modes_match);
}

#endif /*FEATURE_VAMOS_II*/

void rr_store_service_domain(
  sys_srv_domain_e_type service_domain,
  const gas_id_t  gas_id
)
{
  rr_PLMN_request_details_T * rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  if (rr_PLMN_request_details_ptr->service_domain != service_domain)
  {
    MSG_GERAN_MED_2_G("Service domain changed %d->%d",
                      rr_PLMN_request_details_ptr->service_domain,
                      service_domain);

    rr_PLMN_request_details_ptr->service_domain = service_domain;
  }
}

rr_nw_supp_utran_rats_t rr_nw_supported_utran_rats(gprs_scell_info_t * scell)
{
   rr_nw_supp_utran_rats_t nw_rats = RR_NW_SUPP_UTRAN_NONE;
   boolean fdd_found = FALSE;
   boolean tdd_found = FALSE;
   uint8 i = 0;

   if (scell->gsm.utran_neighbor_list.valid_data == TRUE)
   {
      /* Check SI2quater Frequency List. */
      for (i = 0;
             i < scell->gsm.utran_neighbor_list.wcdma_freq_only_list.num_of_frequencies &&
             i < MAX_UTRAN_FREQ_UPDATE_LIST;
             ++i)
      {
        if (scell->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].rat_type == RR_UTRAN_FDD)
        {
          fdd_found = TRUE;
        }

        if (scell->gsm.utran_neighbor_list.wcdma_freq_only_list.uarfcn[i].rat_type == RR_UTRAN_TDD)
        {
          tdd_found = TRUE;
        }
      }

      /* Check SI2quater Cell List. */
      for (i = 0;
           i < scell->gsm.utran_neighbor_list.wcdma_cell_list.num_of_cells &&
           i < MAX_RR_MPH_CELL_UPDATE_LIST;
           ++i)
      {
        if (scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i].uarfcn.rat_type == RR_UTRAN_FDD)
        {
          fdd_found = TRUE;
        }

        if (scell->gsm.utran_neighbor_list.wcdma_cell_list.cell_list[i].uarfcn.rat_type == RR_UTRAN_TDD)
        {
          tdd_found = TRUE;
        }
      }
    }


    if (scell->gsm.wcdma_neighbor_list_si2ter.valid_data == TRUE)
    {
      /* Add FDD frequencies from SI2ter. */
      for (i = 0;
           i < scell->gsm.wcdma_neighbor_list_si2ter.wcdma_freq_only_list.num_of_frequencies &&
           i < MAX_RR_MPH_FREQ_UPDATE_LIST;
           ++i)
      {
        fdd_found = TRUE;
      }

      for (i = 0;
             i < scell->gsm.wcdma_neighbor_list_si2ter.tds_freq_only_list.num_of_frequencies &&
             i < MAX_RR_MPH_FREQ_UPDATE_LIST;
             ++i)
      {
        tdd_found = TRUE;
      }
    }

    if (tdd_found && fdd_found)
    {
      MSG_GERAN_HIGH_0("RR NW supports UTRAN FDD and TDD");
      nw_rats = RR_NW_SUPP_UTRAN_TDD_FDD;
    }
    else if (tdd_found)
    {
      MSG_GERAN_HIGH_0("RR NW supports UTRAN TDD only");
      nw_rats = RR_NW_SUPP_UTRAN_TDD;
    }
    else if (fdd_found)
    {
      MSG_GERAN_HIGH_0("RR NW supports UTRAN FDD only");
      nw_rats = RR_NW_SUPP_UTRAN_FDD;
    }
    else
    {
      MSG_GERAN_HIGH_0("RR NW no UTRAN support");
      nw_rats = RR_NW_SUPP_UTRAN_NONE;
    }

    return nw_rats;
}

#ifdef FEATURE_SGLTE
void rr_sglte_dedicated_priority_info_store(sglte_l2g_resel_req_ded_pri_info_t * sglte_dedicated_pri_info_ptr, const gas_id_t gas_id)
{

  /*NOTE In GERAN_GRR_LTE_RESEL_REQ, the is no dedicated_priorities_valid   */
  /*flag. Assumption is that it's always present. Since generic handler for */
  /*dedicated priority info flag has been add                               */
  /*for sglte since UE is already on S cell PLMN is known, but check        */
  /*for sglte ARFCN is going to be that of the serving cell as UE is already*/
  /*camped this is contrary to normal lte operation where UE is NOT         */ 

 
  /*update SGLTE dedicated priority info rcvd in GERAN_GRR_LTE_RESEL_REQ*/
  /*and reset temp stored info                                          */
  rr_PLMN_request_details_T * rr_plmn_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  sys_plmn_id_s_type requested_PLMN_id = rr_plmn_request_details_ptr->requested_PLMN_id;

  boolean  plmn_ok = sys_plmn_match(requested_PLMN_id,sglte_dedicated_pri_info_ptr->requested_PLMN);

  if (!plmn_ok)
  {
    plmn_ok = rr_eplmn_list_equivalent_plmn(requested_PLMN_id, gas_id) &&
              rr_eplmn_list_equivalent_plmn(sglte_dedicated_pri_info_ptr->requested_PLMN, gas_id);
  }

  if (plmn_ok)
  {
    if (sglte_dedicated_pri_info_ptr->dedicated_priorities_valid)

    {
      ARFCN_T arfcn = rr_gprs_get_scell_arfcn(gas_id);

      rr_dedicated_priority_info_store(
        &sglte_dedicated_pri_info_ptr->dedicated_priority_info,
        &arfcn,
        requested_PLMN_id,
        gas_id
      );

      rr_dedicated_priorities_take_ownership(gas_id);
    }
    else
    {
      /* No action necessary, dedicated priorities are no longer valid.  Likely
      to be due to timer expiry between request and response. */
      MSG_GERAN_HIGH_0_G("Dedicated priorities not valid");
    }
  }
  else
  {
    lte_rrc_dedicated_pri_owner_e owner;

    MSG_GERAN_HIGH_0_G("Dedicated priorities not valid for current PLMN, discarding");

    owner = lte_rrc_dedicated_pri_get_owner();

    lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_NONE);

    rr_dedicated_priorities_request_owner_clears(owner, gas_id);
  }

  /*reset temp stored SGLTE dedicated_priorities info*/
  sglte_dedicated_pri_info_ptr->dedicated_priorities_valid = FALSE;

}

#endif /* FEATURE_SGLTE */

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q

/*===========================================================================

FUNCTION  rr_check_and_populate_default_irat_prior_params

DESCRIPTION  This fucntion checks, if LTE Frequencies are present in the NAS data base by querying the NAS Database.
If LTE Frequencies are present and EFS contains Default Data Items for UTRAN and LTE information
this function will populate the same into Serving Cell Desription structure and return SUCCESS(i.e. TRUE).
If NAS Data base has no LTE Information for this specific Serving Cell, OR the Default Parameters are not set in EFS, then
this function returns FALSE


DEPENDENCIES None

RETURN VALUE boolean ( TRUE/FALSE) Depdning upon whether information is present and populated or it is absent.

SIDE EFFECTS None

===========================================================================*/

boolean rr_check_and_populate_default_irat_prior_params(const gas_id_t gas_id )
{
  uint32 earfcn[GRR_PSEUDO_SI2Q_MAX_EARFCN];
  earfcn_nas_type nas_earfcn;
  inter_task_cell_id_type rr_inter_task_cell_id; 
  uint8 i = 0;
  uint8  num_freq = 0;
  gprs_scell_info_t *serving_cell_ptr = NULL;
  rr_efs_pseudo_si2q_def_data_t* pseudo_si2q_data_p = NULL;
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;

  serving_cell_ptr = rr_gprs_get_scell_info(gas_id);
  pseudo_si2q_data_p = rr_get_efs_nv_fast_g2l_pseudo_si2q_data(as_id);

  if (serving_cell_ptr == NULL) 
  {
    MSG_GERAN_ERROR_0_G("No Valid Serving Cell to Save the Data");
    return FALSE;   
  }

  /* First Check oif the Feature is Enabled and Saved EFS Data is Valid or not?? */
  if ( !(rr_is_efs_nv_fast_g2l_using_pseudo_si2q_enabled(as_id) || rr_is_efs_nv_fast_g2l_using_pseudo_si2q_data_valid(as_id) ))
  {
    MSG_GERAN_HIGH_0_G("GRR Fast G2L Reselection Feature NOT enabled or EFS Data NOT VALID");    
    return FALSE;
  }

  if ( rr_is_pseudo_si2q_data_used(gas_id) && (serving_cell_ptr->lte_neighbour_list.count > 0) )
  {
    MSG_GERAN_HIGH_0_G("rr_check_and_populate_default_irat_prior_params. Invoked when PSEUDO SI2Q data already set in DB and Valid LTE cell List Exists");
    return TRUE;
  }

  rr_inter_task_cell_id.rat = SYS_RAT_GSM_RADIO_ACCESS;
  rr_inter_task_cell_id.plmn = serving_cell_ptr->gsm.location_area_identification.plmn_id;
  rr_inter_task_cell_id.lac = serving_cell_ptr->gsm.location_area_identification.location_area_code;
  rr_inter_task_cell_id.cell_id = serving_cell_ptr->gsm.cell_identity;
  

  if (!get_lte_earfcn_list_from_nas(&rr_inter_task_cell_id, &nas_earfcn))
  {
     MSG_GERAN_HIGH_0_G("rr_check_and_populate_default_irat_prior_params: NAS get_lte_earfcn_list_from_nas() returns FALSE");
     return FALSE;
  }


  if ((nas_earfcn.count == 0) || (nas_earfcn.count > NAS_MAX_EARFCN_COUNT) || (nas_earfcn.count > GRR_PSEUDO_SI2Q_MAX_EARFCN))
  {
     MSG_GERAN_ERROR_1_G("rr_check_and_populate_default_irat_prior_params: NAS get_lte_earfcn_list_from_nas() returns invalid num of freq. %d", nas_earfcn.count );
     return FALSE;
  }

  for (i = 0; i < nas_earfcn.count; i++)
  {
    /* Check for the frequencies which do not fall on Supported Bands */
    if (lte_rrc_earfcn_is_supported(nas_earfcn.earfcn_nas[i], rr_band_pref_lte(gas_id)))
    {
        earfcn[num_freq++] = nas_earfcn.earfcn_nas[i];
    }
    else
    {
      MSG_GERAN_ERROR_1_G("INVALID EARFCN FROM NAS get_lte_earfcn_list_from_nas() %d", nas_earfcn.earfcn_nas[i] );

    }
  }

  if (num_freq == 0)
  {
     MSG_GERAN_ERROR_1_G("No Valid EARFCN FROM NAS get_lte_earfcn_list_from_nas() %d", nas_earfcn.count );
     return FALSE;
  }


  if ( serving_cell_ptr->gsm.camped_on_flag == FALSE)
  {
    MSG_GERAN_ERROR_0_G("rr_check_and_populate_default_irat_prior_params called when GRR NOT CAMPED");
  }

  if ( pseudo_si2q_data_p != NULL ) 
  {
    /* Construct the GERAN, LTE and TDS/WCDMA Priority Frequency Information now */

    rr_restore_default_serv_utran_pseudo_si2q_params( serving_cell_ptr, gas_id );

    if ( (serving_cell_ptr->lte_neighbour_list.count == 0) || (rr_is_pseudo_si2q_data_used(gas_id) == FALSE ) )
    {
      rr_lte_neighbour_list_entry_t * dst;

      /* Now the LTE Part */
      for ( i =0; ((i < num_freq) && (i < GRR_PSEUDO_SI2Q_MAX_EARFCN)); i++ )
      {

        MSG_GERAN_HIGH_1_G("rr_check_and_populate.. Saving Data for LTE EARFCN %d=", earfcn[i]);
          
        dst = &serving_cell_ptr->lte_neighbour_list.entries[i];
        dst->frequency.earfcn = earfcn[i];
        dst->frequency.measurement_bandwidth = pseudo_si2q_data_p->rr_lte_def_data.meas_bandwidth;
        dst->priority = pseudo_si2q_data_p->rr_lte_def_data.priority;
        dst->qrxlevmin = pseudo_si2q_data_p->rr_lte_def_data.qrxlevmin;
        dst->thresh_high = pseudo_si2q_data_p->rr_lte_def_data.thresh_high;
        dst->thresh_low = pseudo_si2q_data_p->rr_lte_def_data.thresh_low;
      }

      serving_cell_ptr->lte_neighbour_list.count = num_freq;

      rr_set_rr_using_pseudo_si2q_data(TRUE, gas_id);         
      return TRUE;               
    }
  }

  MSG_GERAN_ERROR_1_G("rr_check_and_populate_default_irat_prior_params() Returns FALSE from End of Function", 0 );     

  return FALSE;

}

/*===========================================================================

FUNCTION     rr_default_irat_params_update_required

DESCRIPTION  Determines whether an update to the parameters is required

DEPENDENCIES None

RETURN VALUE TRUE if rr_check_and_populate_default_irat_prior_params needs to be called

SIDE EFFECTS None

===========================================================================*/
boolean rr_default_irat_params_update_required(const gas_id_t gas_id)
{
  gprs_scell_info_t *cell_info   = rr_gprs_get_scell_info(gas_id);
  boolean            update_reqd = FALSE;

  if (cell_info != NULL)
  {
    if ((cell_info->gsm.two_ti == RR_SI2TER_NOT_EXPECTED)    &&
        (cell_info->gsm.decode_SI2ter == RR_SI_NOT_REQUIRED) &&
        (cell_info->gsm.two_quater_position == SI2Q_POSITION_NONE))
    { 
      /* NO SI2-TER and NO SI2Q broadcasted on cell */
      update_reqd = TRUE;
    }
    else if (rr_is_camped(gas_id))
    {
      if ((cell_info->gsm.complete_idle_3G_neighbor_list_received_si2ter) &&
          (cell_info->gsm.two_quater_position == SI2Q_POSITION_NONE))
      {
        /* UE is camped on the cell and there is no SI2Q going to arrive, caters for the case when SI2ter arrives prior
           to full camp and UE does not send WCDMA Cell Update List Request from rr_sys_info.c, while decoding SI2-ter */
        update_reqd = TRUE;
      } 
      else if ((cell_info->gsm.complete_idle_3G_neighbor_list_received) &&
               (cell_info->gsm.lte_neighbour_list.count == 0))
      {
        /* Caters for the case SI2Q arrives prior to Full Camp on the cell */
        update_reqd = TRUE;      
      }
    }
  }

  return(update_reqd);
}

/*===========================================================================

FUNCTION     rr_restore_default_serv_utran_pseudo_si2q_params

DESCRIPTION  Stores Serving Cell and UTRAN Priority Params from NV Data Base

DEPENDENCIES None

RETURN Void

SIDE EFFECTS None

===========================================================================*/
void rr_restore_default_serv_utran_pseudo_si2q_params(gprs_scell_info_t *serving_cell_ptr, const gas_id_t gas_id)
{
  rr_efs_pseudo_si2q_def_data_t *pseudo_si2q_data_p = NULL;
  pseudo_si2q_data_p = rr_get_efs_nv_fast_g2l_pseudo_si2q_data(RR_GAS_ID_TO_AS_ID);

  if ((serving_cell_ptr == NULL) || (pseudo_si2q_data_p == NULL))
  {
    MSG_GERAN_ERROR_0_G("rr_restore_default_serv_utran_pseudo_si2q_params: No Valid Serving Cell OR Default NV Data Store to set");
    return;
  }

  if (FALSE == serving_cell_ptr->serving_cell_priority_params_valid)
  {
    serving_cell_ptr->serving_cell_priority_params_valid = TRUE;
    serving_cell_ptr->serving_cell_priority_params.geran_priority = pseudo_si2q_data_p->rr_geran_def_data.geran_priority;
    serving_cell_ptr->serving_cell_priority_params.thresh_priority_search = pseudo_si2q_data_p->rr_geran_def_data.thresh_priority_search;
    serving_cell_ptr->serving_cell_priority_params.thresh_gsm_low = pseudo_si2q_data_p->rr_geran_def_data.thresh_gsm_low;
    serving_cell_ptr->serving_cell_priority_params.t_reselection = pseudo_si2q_data_p->rr_geran_def_data.t_reselection;
    serving_cell_ptr->serving_cell_priority_params.h_prio = pseudo_si2q_data_p->rr_geran_def_data.h_prio;
    serving_cell_ptr->eutran_params.meas_params.qsearch_c = 0x07;
  }
  else
  {
    MSG_GERAN_HIGH_0_G("rr_restore_default_serv_utran_pseudo_si2q_params, Serving Priority Params already Valid ");
  }

  if (!(serving_cell_ptr->utran_priority_params.default_params_valid) &&
       (serving_cell_ptr->utran_priority_params.utran_priority_params_count == 0))
  {
    /* This indicates that there are NO Default UTRAN Priority params or specific UTRAN Priority Params */
    serving_cell_ptr->utran_priority_params.default_params_valid = TRUE;
    serving_cell_ptr->utran_priority_params.default_params.priority = pseudo_si2q_data_p->rr_utran_def_data.priority;

    serving_cell_ptr->utran_priority_params.default_params.thresh = pseudo_si2q_data_p->rr_utran_def_data.thresh_high;
    serving_cell_ptr->utran_priority_params.default_params.qrxlevmin = pseudo_si2q_data_p->rr_utran_def_data.qrxlevmin;
  }
  else
  {
    MSG_GERAN_HIGH_0_G("rr_restore_default_serv_utran_pseudo_si2q_params, UTRAN Priority Parameters NOT SET from Default EFS Data ");
  }

  return;
}

/*===========================================================================

FUNCTION     rr_flush_default_irat_prior_params

DESCRIPTION  Clears Default Priority Parameters set in the Serving Cell DB, when pseudo is used

DEPENDENCIES None

RETURN Void

SIDE EFFECTS None

===========================================================================*/
void rr_flush_default_irat_prior_params(const gas_id_t gas_id )
{

  gprs_scell_info_t *serving_cell_ptr = rr_gprs_get_scell_info(gas_id);
 
  RR_NULL_CHECK_RETURN_VOID(serving_cell_ptr); 
  
  serving_cell_ptr->serving_cell_priority_params_valid = FALSE;
  
  memset( &serving_cell_ptr->lte_neighbour_list, 0, sizeof(rr_lte_neighbour_list_t));
  serving_cell_ptr->utran_priority_params.default_params_valid = FALSE;
}

#endif /* FEATURE_FAST_G2L_PSEUDO_SI2Q */

/*===========================================================================

FUNCTION  rr_invalidate_csg_info

DESCRIPTION  Sets the csg_id to an invalid value.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

===========================================================================*/
void rr_invalidate_csg_info(sys_detailed_plmn_list_s_type *info_ptr)
{
  uint8 i = 0;
  RR_NULL_CHECK_RETURN_VOID(info_ptr)
  
  for (i=0; (i < info_ptr->length) && (i < SYS_PLMN_LIST_MAX_LENGTH); i++)
  {
    info_ptr->info[i].csg_info.csg_id = SYS_CSG_ID_INVALID;
  }
}

/*!
 * \brief Returns the ARFCN of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return ARFCN_T
 */
ARFCN_T rr_scell_arfcn(
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(scell_info_ptr);

  return scell_info_ptr->gsm.BCCH_ARFCN;
}

/*!
 * \brief Returns the BSIC of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return BSIC_T
 */
BSIC_T rr_scell_bsic(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  return scell_info_ptr->gsm.BSIC;
}

/*!
 * \brief Returns the RxLev (average) of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return uint8
 */
uint8 rr_scell_rxlev_average(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  return (uint8)scell_info_ptr->gsm.reselection_info.RXLEV_average;

} /* end rr_scell_rxlev_average() */

/*!
 * \brief Returns the PLMN ID of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return PLMN_id_T
 */
PLMN_id_T rr_scell_plmn_id(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(scell_info_ptr);

  return scell_info_ptr->gsm.location_area_identification.plmn_id;
}

/*!
 * \brief Returns the LAI of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return LAI_T
 */
LAI_T rr_scell_lai(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  return scell_info_ptr->gsm.location_area_identification;
}

/*!
 * \brief Returns the RAC of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return uint8
 */
uint8 rr_scell_rac(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  return (uint8)scell_info_ptr->bcch_sys_info.routing_area_code;
}

/*!
 * \brief Returns the Cell Identity of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return uint16
 */
uint16 rr_scell_cell_identity(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  return (uint16)scell_info_ptr->gsm.cell_identity;
}

/*!
 * \brief Returns the PLMN ID of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return PLMN_id_T
 */
sys_srv_capability_e_type rr_scell_service_capability(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(scell_info_ptr);

  if (scell_info_ptr->gsm.gprs_indicator)
  {
    return SYS_SRV_CAPABILITY_CS_PS_SERVICE;
  }

  return SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
}

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION  rr_add_gsm_cell_to_depriotized_list

PARAMETERS
   ARFCN_T - ARFCN which needs to be added 
  gas_id - Dual Standby GERAN Access Stratum Identifier

Returns:
  void

===========================================================================*/

void rr_add_gsm_cell_to_depriotized_list(ARFCN_T arfcn, const gas_id_t gas_id)
{
  inter_task_lai_T lte_lai;
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  if (mm_get_last_updated_lai_in_lte(&lte_lai, as_id))
  { 
    uint8 index;
    if (rr_general_data_ptr->depriotized_gsm_cell_list.index >= RR_MAX_NUMBER_OF_DEPRIOTIZED_CELLS)
    {
      /*Start over the list*/
      rr_general_data_ptr->depriotized_gsm_cell_list.index = 0;
    }

    index = rr_general_data_ptr->depriotized_gsm_cell_list.index;
    /*Consistency check */
    if (rr_general_data_ptr->depriotized_gsm_cell_list.num_of_cell >= RR_MAX_NUMBER_OF_DEPRIOTIZED_CELLS)
    {
      rr_general_data_ptr->depriotized_gsm_cell_list.num_of_cell = RR_MAX_NUMBER_OF_DEPRIOTIZED_CELLS;
      rr_general_data_ptr->depriotized_gsm_cell_list.num_of_cell--;  
      MSG_GERAN_HIGH_0_G("Overwrite the oldest entry");
    }
    rr_general_data_ptr->depriotized_gsm_cell_list.depriotized_cell_info[index].ARFCN = arfcn;
    rr_general_data_ptr->depriotized_gsm_cell_list.depriotized_cell_info[index].lte_lai = lte_lai;
    rr_general_data_ptr->depriotized_gsm_cell_list.num_of_cell++;
    rr_general_data_ptr->depriotized_gsm_cell_list.index++;

    MSG_GERAN_HIGH_3_G( "GSM cell = %d, added at index= %d,num of cells = %d",
                         rr_general_data_ptr->depriotized_gsm_cell_list.depriotized_cell_info[index].ARFCN.num,
                         index,
                         rr_general_data_ptr->depriotized_gsm_cell_list.num_of_cell);
  }
  else
  {
    MSG_GERAN_HIGH_0_G( "LAI not avaiable from LTE, no GSM cell added");
  }
}


/*===========================================================================

FUNCTION  rr_is_gsm_cell_in_depriotized_list

PARAMETERS
  ARFCN_T - ARFCN which needs to be checked 
  gas_id - Dual Standby GERAN Access Stratum Identifier

Returns: TRUE if cell is found FALSE otherwise

===========================================================================*/

boolean rr_is_gsm_cell_in_depriotized_list(ARFCN_T arfcn, const gas_id_t gas_id)
{
  inter_task_lai_T lte_lai;
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);

  if (mm_get_last_updated_lai_in_lte(&lte_lai, as_id))
  {
    uint8 i;
    for (i = 0; (i< RR_MAX_NUMBER_OF_DEPRIOTIZED_CELLS) && (i < rr_general_data_ptr->depriotized_gsm_cell_list.num_of_cell); i++)
    {
      if (ARFCNS_EQUAL(rr_general_data_ptr->depriotized_gsm_cell_list.depriotized_cell_info[i].ARFCN,arfcn)) 
      {
        boolean plmn_ok = FALSE;
        boolean lac_ok = FALSE;
        plmn_ok = sys_plmn_match(rr_general_data_ptr->depriotized_gsm_cell_list.depriotized_cell_info[i].lte_lai.PLMN_id, lte_lai.PLMN_id);

        if (plmn_ok)
        {
          uint16 lac1 = (uint16) ((lte_lai.location_area_code[0] * 256) + (lte_lai.location_area_code[1]));
          uint16 lac2 = (uint16) ((rr_general_data_ptr->depriotized_gsm_cell_list.depriotized_cell_info[i].lte_lai.location_area_code[0] * 256) + 
                                  (rr_general_data_ptr->depriotized_gsm_cell_list.depriotized_cell_info[i].lte_lai.location_area_code[1]));
          if (lac1 == lac2)
          {
            MSG_GERAN_HIGH_1_G( "Cell= %d in de-prioritized list, now removed", arfcn.num);
            return TRUE;
          }
        } //PLMN mismatch
      } //ARFCN mismatch
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G( "LAI not avaiable from LTE, no GSM cell added" );
  }

  return FALSE;
}

/*===========================================================================

FUNCTION  rr_init_depriotized_list

PARAMETERS
  gas_id - Dual Standby GERAN Access Stratum Identifier

Returns:
  void

===========================================================================*/
void rr_init_depriotized_list(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_general_data_t *rr_general_data_ptr = rr_general_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_general_data_ptr);
  rr_general_data_ptr->depriotized_gsm_cell_list.num_of_cell = 0;
}

#endif /*FEATURE_LTE*/

#ifdef FEATURE_GERAN_HST_OPT
/*!
 * \brief Returns if High Speed Train (HST) optimization
 *        is allowed
 *
 * \param gas_id (in)
 *
 * \return TRUE - HST opt allowed, FALSE - HST opt not allowed
 */
boolean rr_high_speed_train_opt_allowed(const gas_id_t gas_id)
{
  boolean hst_opt_allowed = FALSE;
  
  if (TRUE == rr_is_hst_cell_freq_list_present(gas_id))
  {
    rr_irat_acq_db_t *irat_acq_db_ptr = rr_irat_acq_db_get_data_ptr();
    RR_NULL_CHECK_FATAL(irat_acq_db_ptr);

    if ( TRUE == rr_nv_hst_opt_is_plmn_match(irat_acq_db_ptr->plmn_id, gas_id) && /*HST opt only used for customer specified white list*/
         TRUE == geran_is_multi_mode_gas_id(gas_id)  /*HST opt is only used on non LTE sub*/   
       )
    {
      hst_opt_allowed = TRUE;
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Frequency List for HST not valid");
  }

  return hst_opt_allowed;
}
#endif /*FEATURE_GERAN_HST_OPT*/

/* EOF */

