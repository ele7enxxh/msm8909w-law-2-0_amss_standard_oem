/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              rr_general.h

GENERAL DESCRIPTION
   This module contains general rr functionality.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2002-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_general.h_v   1.12   17 Jul 2002 11:48:52   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_general.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/16/12   sjw     Force PLMN search to foreground if both stacks in forbidden LAI
08/18/09   sjw     Added support for RXQUAL reporting to CM during 2G PS calls
03/09/09   sjw     Relocated rr_channel_mode_supported() to rr_channel_mode.c
02/13/09   sjw     Reworked local storage for ciphering module data
05/07/04   gc      Enhanced Trace Buffer Code
Feb/12/04   VT     QuadBand: Added new function rr_set_supported_bands()
Jan/21/04   VT     QuadBand: Added new function rr_set_allowed_bands()
06/30/03   sbs     Added support for Location Services related APDU handling.
05/08/03   he      Enabled debug string logging.
02/27/03   RG      Updated SAPI3 handling
02/14/03   RG      Added support for G2W blind Handover
12/12/02   JAC     unfeaturized rr_update_timing_advance_info
11/20/02   SBS     Added rr_update_timing_advance_info function for use when
                   FEATURE_MMGPS defined.
08/06/02   rmc     Cell Selection Optimization Changes
07/31/02   rmc     Adding RR timer information related functions
07/17/02   rmc     Removed rr_update_global_ba_list function
07/25/02   Rao     Added a general util function to send L3 messages to L2
06/19/02   JAC     rr_start_idle interface change
05/30/02   rmc     Removed rr_debug_str declaration
05/22/02   rmc     Changed common mm_rr types to mm_as...
03/05/02   rmc     Dual Mode Cell Selection changes
03/18/02   SB      Dual Mode Activation / Deactivation changes
02/20/02   rmc     current_paging_mode accessor (CR20636)
02/15/02   rmc     QXDM Changes
02/06/02   rao     Added RR local SIM info accesor function prototypes.
01/30/02   RMC     Cell Reselection Cleanup

===========================================================================*/

#ifndef RR_GENERAL_H
#define RR_GENERAL_H


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include <stdio.h>
#include "comdef.h"
#include "sys_cnst.h"
#include "environ.h"
#include "libprot.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_l2.h"
#include "mm_rr.h"
#include "rr_defs.h"
#include "msg.h"
#include "err.h"
#include "rr_usat_if.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"     /* For rr_internal_band_T */
#include "rr_task.h"
#include "rr_gprs_debug.h"
#include "rr_control.h"
#include "rr_grr.h"
#include "rr_dtm.h"
#include "rr_gprs_main.h"
#include "rr_candidate_cell_db.h"

#include "sys_eplmn_list.h"

#include "rr_hz.h"

#include "geran_eng_mode_info.h"
#include "rr_gprs_defs.h"
#include "geran_multi_sim.h"
#include "rr_ded_config.h"

#if defined(FEATURE_LTE)
#include "lte_rrc_ext_api.h"
#endif /* FEATURE_LTE */

/*===========================================================================

                         DEFINITIONS

===========================================================================*/
/* (X,Y) pairs for candidate cell processing - see paragraph below */
/* For each band define a pair of numbers (X,Y) where the default numbers are
 *     (140,140) for 900 band,
 *     (140,140) for 1800 band,
 *     (140,140) for 850 band, and
 *     (140,140) for 1900 band.
 *
 * The total number of cells should be less than or equal to the maximum
 * number that will fit in the MPH_DECODE_BCCH_LIST_REQ, 140.
 *
 * By setting all of these values to 140 we allow RR to decode ARFCNs from
 * the power scan results strictly based on signal strength, rather than
 * giving any weighting to specific bands.
 */

#define RR_TOT_FREQ_MONITOR_GSM_900                140
#define RR_MAX_FREQ_MONITOR_GSM_900                140

#define RR_TOT_FREQ_MONITOR_DCS_1800               140
#define RR_MAX_FREQ_MONITOR_DCS_1800               140

#define RR_TOT_FREQ_MONITOR_GSM_850                140
#define RR_MAX_FREQ_MONITOR_GSM_850                140

#define RR_TOT_FREQ_MONITOR_PCS_1900               140
#define RR_MAX_FREQ_MONITOR_PCS_1900               140

#define RR_NUM_FREQ_MONITOR                        140

#define IGNORE_RCVD_SI_MASK                     0x0000

#ifdef FEATURE_GSM_AMR_WB

  #ifdef FEATURE_VAMOS_II
    #define IS_AMR_CHANNEL_MODE(cm) (((cm) == SPEECH_V3) || \
                                     ((cm) == SPEECH_V5) || \
                                     ((cm) == SPEECH_V3_VAMOS2) ||\
                                     ((cm) == SPEECH_V5_VAMOS2))
 #else  /*FEATURE_VAMOS_II*/
#define IS_AMR_CHANNEL_MODE(cm) (((cm) == SPEECH_V3) || ((cm) == SPEECH_V5))
 #endif /*FEATURE_VAMOS_II*/

#else /* FEATURE_GSM_AMR_WB */

  #ifdef FEATURE_VAMOS_II
   #define IS_AMR_CHANNEL_MODE(cm) (((cm) == SPEECH_V3) || ((cm) == SPEECH_V3_VAMOS2))
  #else  /*FEATURE_VAMOS_II*/
#define IS_AMR_CHANNEL_MODE(cm) ((cm) == SPEECH_V3)
  #endif /*FEATURE_VAMOS_II*/

#endif /* FEATURE_GSM_AMR_WB */

#define RR_GET_BAND_CAP_CHGWT(bc) ((bc).chgwt_band_cap)
#define RR_GET_BAND_CAP_LTE(bc)   ((bc).lte_band_cap)

#define NELEMS(array) (sizeof(array) / sizeof(array[0]))

/* min, max, and default values for RADIO-LINK-TIMEOUT from 3GPP 44.018 */
#define RR_MIN_RLT_COUNTER 4
#define RR_MAX_RLT_COUNTER 64
#define RR_DEF_RLT_COUNTER 16 /* arbitrarily chose a default value */

#define RR_INVALID_RXQUAL_VALUE 0xFF

#define RR_MAX_GREATEST_POWER_DATA 33

extern word greatest_power_imp(word index);

/*Index must be > 0*/
#define greatest_power(index) \
(((index) <= RR_MAX_GREATEST_POWER_DATA) ? (word) greatest_power_data[(index)] : greatest_power_imp((index)))

#ifndef NO_ERROR      /* Also defined in winerror.h */
#define NO_ERROR 0    /* moved from api/rr_l2.h where ifdefs are not allowed. */
#endif

typedef enum
{
  RR_NW_SUPP_UTRAN_NONE,
  RR_NW_SUPP_UTRAN_FDD,
  RR_NW_SUPP_UTRAN_TDD,
  RR_NW_SUPP_UTRAN_TDD_FDD
} rr_nw_supp_utran_rats_t;

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/
extern const unsigned int greatest_power_data[];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

void rr_update_idle_mode(const gas_id_t gas_id);
void rr_l1_update_dedicated_mode(system_information_database_T *si_db_ptr, const gas_id_t gas_id);

void rr_add_ba_list(
  BCCH_frequency_list_T* BCCH_frequency_list,
  BCCH_frequency_list_T* information_database_ba,
  const gas_id_t gas_id
);

boolean rr_check_BA_list_change(
  BCCH_frequency_list_T *new_entries,
  BCCH_frequency_list_T *old_entries,
  const gas_id_t gas_id
);

rr_internal_band_T rr_gapi_get_current_band(
  const gas_id_t gas_id
);

rr_internal_band_T rr_get_band_using_band_indicator(
  word local_arfcn,
  si_psi_band_ind_T band_ind,
  const gas_id_t gas_id
);

si_psi_band_ind_T  rr_get_current_band_indicator(boolean is_camped, const gas_id_t gas_id);

/* Access/Maintenance functions for barred cell list. Cells are barred
 * due to authentication or authentication and ciphering failure.
 */
void rr_init_barred_cell_list(const gas_id_t gas_id);
boolean rr_is_cell_in_barred_cell_list(ARFCN_T arfcn, BSIC_T bsic, rr_csi_cgi_t *cgi, uint8 *barred_cell_index, const gas_id_t gas_id);
uint8 rr_find_oldest_barred_cell(const gas_id_t gas_id);
uint8 rr_get_barred_cell_count(const gas_id_t gas_id);
boolean rr_is_cell_in_barred_cell_list(ARFCN_T arfcn, BSIC_T bsic, rr_csi_cgi_t *cgi, uint8 *barred_cell_index, const gas_id_t gas_id);
void rr_update_barred_cell_remaining_time(const gas_id_t gas_id);
void rr_process_barred_cell_timer_expiry(const gas_id_t gas_id);
void rr_authentication_failed_bar_cell(uint32 cell_bar_timer_value, const gas_id_t gas_id);
void rr_authn_fail_bar_cell_basedon_ARFCN_BSIC(uint32 cell_bar_timer_value, ARFCN_T arfcn, uint8 bsic, const gas_id_t gas_id);

/**
 * Used when sorting cells based on rxlev.
 * @arg1   First cell  @arg2   Second cell
 */
int rr_compare_freq_power( const void *arg1, const void *arg2 );
void rr_clear_si_sacch(const gas_id_t gas_id);
boolean rr_get_DTX_indicator(const gas_id_t gas_id);

void rr_reset_S_counter(const gas_id_t gas_id);
uint8 rr_increment_S_counter(const gas_id_t gas_id);
uint8 rr_decrement_S_counter(const gas_id_t gas_id);
boolean rr_retrieve_S_counter(byte *now_ptr, byte *max_ptr, const gas_id_t gas_id);
boolean rr_same_lai(LAI_T *lai1,LAI_T *lai2);

void rr_compute_BSIC_from_SCH_block(BSIC_T *BSIC_result_ptr,byte *SCH_block);
boolean rr_same_BSIC(BSIC_T *bsic1, BSIC_T *bsic2);

boolean rr_check_frequency_list(
  frequency_list_T* frequency_list,
  const gas_id_t gas_id
);

void rr_notify_deep_sleep_to_other_rats(boolean in_deep_sleep, const gas_id_t gas_id);

void rr_start_idle_mode
(
  gprs_scell_info_t             *cell_info,
  boolean                        notify_MM,
  boolean                        early_camping_performed,
  const gas_id_t                 gas_id
);

/**
 * Check the access to the given cell for the given establishment reason
 *
 * @param establishment_cause reason the cell is being accessed
 * @param plmn_id PLMN id of the cell
 * @param access_control the access control parameters of the cell
 * @param CELL_BAR_ACCESS  the Cell is Barred or not for Access
 * @param CELL_BAR_QUALIFY controls the cell selection and reselection.
 *
 * @return the type of access allowed for the cell for the given establishment cause
 */
extern access_result_T rr_check_access_class
(
  establishment_cause_T  establishment_cause,
  PLMN_id_T  plmn_id,
  byte  access_control[2],
  boolean  cell_bar_access,
  boolean  cell_bar_qualify,
  const gas_id_t gas_id
);

extern access_result_T rr_check_access_class_for_w2g_oos_search(
  PLMN_id_T  plmn_id,
  byte  access_control[2],
  boolean  cell_bar_access,
  boolean  cell_bar_qualify,
  const gas_id_t gas_id
);

extern access_result_T rr_check_access_class_for_raching(
  establishment_cause_T  establishment_cause,
  const gas_id_t gas_id
);

/*+452757 ASR*/
extern boolean rr_si2quater_all_instances_received(
  instances_T * si2quater_instances
);
/*-452757 ASR*/

extern access_result_T rr_check_access_class_for_raching_for_tbf(const gas_id_t gas_id);

extern access_result_T rr_check_access_class_for_camping(
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
);

void rr_add_forbidden_cell(ARFCN_T channel_no, const gas_id_t gas_id);

void rr_start_forbidden_timer(const gas_id_t gas_id);

int rr_get_C1(
  ARFCN_T arfcn,
  byte RXLEV_average,
  byte RXLEV_ACCESS_MIN,
  byte power_control_level,
  byte POWER_OFFSET,
  const gas_id_t gas_id
);

rr_cell_type_T rr_get_cell_priority(
  byte CELL_BAR_QUALIFY,
  byte CELL_BAR_ACCESS,
  LAI_T *location_area_identification,
  byte *access_control,
  boolean *cell_test_operation_ptr,
  const gas_id_t gas_id
);

void rr_clear_pending_channel_data(const gas_id_t gas_id);

boolean rr_cell_forbidden(ARFCN_T channel_no, const gas_id_t gas_id);

void rr_clear_system_information(system_information_database_T *information_database);
void rr_prepare_sidb_for_camping(
  system_information_database_T *si_db_ptr,
  ARFCN_T                       arfcn,
  BSIC_T                        BSIC,
  byte                          RXLEV_average,
  const gas_id_t                gas_id
);


void  rr_initialise_forbidden_cell_list(const gas_id_t gas_id);

void rr_reset_software(const gas_id_t gas_id);

void rr_clear_forbidden_cell_timer(rr_timer_id_T timer_id, const gas_id_t gas_id);

void rr_reset_forbidden_lai_list(const gas_id_t gas_id);
void rr_store_forbidden_lai_list(
  rr_forbidden_lai_update_req_T *forbidden_lai_update_req,
  const gas_id_t gas_id
);

void rr_process_rr_data_req(
  rr_data_req_T *rr_data_req,
  const gas_id_t gas_id
);

/* rr prototypes */
extern boolean rr_is_camped(
  const gas_id_t gas_id
);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
extern boolean rr_ms_active_stacks_in_forbidden_LAI(void);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/* external prototypes */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
extern void ghdi_ds_read_imeisv(
  byte *,
  sys_modem_as_id_e_type
);
#endif /* FEATURE_DUAL_SIM && FEATURE_DUAL_IMEI */

extern void rr_cancel_all_forbidden_cell_timers(
  const gas_id_t gas_id
);

extern PLMN_id_T rr_lai_get_PLMN(
  LAI_T lai
);

extern boolean rr_is_forbidden_LAI(
  LAI_T *lai_ptr,
  const gas_id_t gas_id
);

extern int rxlev_to_dbm(
  byte rxlev
);

extern byte rr_convert_rxlex_to_absolute_dBm(
  byte rxlev
);

extern mm_as_cell_access_status_T rr_get_cell_access_status(
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
);

extern sys_srv_capability_e_type rr_get_cell_service_capability(
  system_information_database_T *si_db_ptr
);

extern boolean rr_channel_type_supported(
  channel_type_T channel_type,
  const gas_id_t gas_id
);

extern boolean rr_set_channel_frequency_list_from_mobile_allocation(
  channel_information_T       *channel_info_ptr,
  mobile_allocation_T         *mobile_allocation_ptr,
  cell_channel_description_T  *cell_channel_description_ptr,
  const gas_id_t               gas_id
);

extern boolean rr_check_frequency_lists_supported(
  channel_spec_T *channel_spec_ptr,
  const gas_id_t gas_id
);
extern boolean rr_check_assignment_frequency_lists_supported(
  assignment_details_T *assignment_details_ptr,
  const gas_id_t gas_id
);

/*****************************/
/* BA list related functions */
/*****************************/
void rr_reset_idle_BA_lists(
  system_information_database_T *si_db_ptr
);

void rr_set_BA_list(
  BCCH_frequency_list_T *dest_ba_list_ptr,
  BCCH_frequency_list_T *src_ba_list_ptr
);

void rr_generate_complete_idle_BA_list(
  system_information_database_T *si_db_ptr,
  uint16 rcvd_si_mask,
  const gas_id_t gas_id
);

boolean rr_has_complete_dedicated_BA_list(
  system_information_database_T *si_db_ptr
);

void rr_reset_dedicated_BA_lists(
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
);

void rr_generate_complete_dedicated_BA_list(
  system_information_database_T *si_db_ptr,
  const gas_id_t gas_id
);

void rr_update_3g_frequency_list(
  rr_utran_arfcn_t *utran_freq_list,
  byte *num_frequencies,
  word uarfcn,
  rr_utran_rat_type_t utran_rat
);

void rr_update_3g_cell_list(
  rr_utran_cell_T *cell_list,
  byte *num_cells,
  word uarfcn,
  word cell_code,
  boolean diversity,
  uint8 sync_case_tstd,
  rr_utran_rat_type_t utran_rat
);

void rr_initialize_wcdma_idle_neighbor_list(
  wcdma_neighbor_list_T *wcdma_neighbor_list
);

void rr_populate_idle_neighbor_list(
  utran_idle_neighbor_list_T *wcdma_idle_neighbor_list,
  instances_T *SI2quater_instances
);

void rr_populate_idle_neighbor_list_si2ter(
  wcdma_idle_neighbor_list_si2ter_T *wcdma_idle_neighbor_list_si2ter,
  si2ter_instances_T *si2ter_instances
);

#ifdef FEATURE_GSM_TDS
void rr_initialize_tds_ded_measurements(
  rr_ded_irat_measurements_t *tds_ded_meas_ptr
);
#endif /*FEATURE_GSM_TDS*/

void rr_initialize_wcdma_ded_neighbor_list(wcdma_ded_neighbor_list_T *wcdma_ded_neighbor_list);

void rr_initialize_wcdma_ded_measurements(surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr);

void rr_initialize_wcdma_neighbor_cell_list(wcdma_neighbor_cell_list_T *wcdma_cell_list_ptr);

void rr_create_wcdma_neighbor_cell_index_list(
  instances_T *I,
  wcdma_neighbor_cell_list_T *wcdma_neighbor_cell_list,
  boolean source /* 0 = SOURCE_SI2_QUATER, 1 = SOURCE_MEAS_INFO*/
);

void rr_populate_ded_neighbor_list (
  wcdma_ded_neighbor_list_T *wcdma_ded_neighbor_list,
  instances_T *MI_instances,
  const gas_id_t gas_id
);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined (FEATURE_LTE)
void rr_l1_update_wcdma_neighbor_list(const gas_id_t gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
void rr_purge_wcdma_ded_measurements(
  surr_wcdma_ded_measurements_T *surr_wcdma_ded_meas_ptr,
  wcdma_ded_neighbor_list_T *wcdma_ded_nlist_ptr
);

byte rr_get_reported_RSCP(int16 RSCP);
byte rr_get_reported_Ec_No(int16 Ec_Nox2);
byte rr_get_reported_RSSI(int16 RSSI);

void rr_set_num_uarfcns_sent_to_l1(uint8 num, const gas_id_t gas_id);
uint8 rr_get_num_uarfcns_sent_to_l1(const gas_id_t gas_id);
uint8 rr_count_num_unique_uarfcns(mph_wcdma_cell_update_list_req_T *);

void rr_set_num_uarfcns_sent_to_l1_in_dedicated(uint8 num, const gas_id_t gas_id);
uint8 rr_get_num_uarfcns_sent_to_l1_in_dedicated(const gas_id_t gas_id);
uint8 rr_count_num_unique_uarfcns_in_dedicated(
  mph_dedicated_wcdma_cell_update_list_req_T *
);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
void rr_send_buffered_nas_messages(const gas_id_t gas_id);
#endif /* FEATURE_WCDMA */

void rr_process_dl_data_ack_ind(dl_data_ack_ind_T *new_message, const gas_id_t gas_id);
void rr_initialize_nas_message_buffer(const gas_id_t gas_id);
byte rr_add_to_nas_message_buffer(
  SAPI_T SAPI,
  byte l3_message_length,
  byte *l3_message,
  const gas_id_t gas_id
);
boolean rr_remove_from_nas_message_buffer(byte l3_message_number, const gas_id_t gas_id);

l2_channel_type_T rr_get_l2_channel_type(const gas_id_t gas_id);

void rr_initialize_SI2q_MI_instances(instances_T *instances, si2q_mi_instance_e_type type);
void rr_initialize_SI2q_instances(const gas_id_t gas_id);
void rr_initialize_MI_instances(const gas_id_t gas_id);
void rr_initialize_SI2ter_instances(const gas_id_t gas_id);
void rr_reset_SI2ter_instances(si2ter_instances_T *inst_ptr);
boolean rr_is_first_received_si2ter_instance(si2ter_instances_T *instances);
sys_band_mask_e_type rr_convert_band_info(sys_band_T band);
void rr_set_preferred_bands(sys_band_mask_type  band_pref, const gas_id_t gas_id);
void rr_move_zero_in_frequency_list(ARFCN_T*, byte, ARFCN_T*);

rr_event_T rr_extract_message_header_details(
  rr_event_T rr_event,
  rr_cmd_bdy_type * message_ptr,
  byte * message_set,
  byte * message_id );


#define RR_HZ_NOTIFY_ON_CHANGE TRUE
#define RR_HZ_UPDATE_INFO_ONLY FALSE

typedef struct
{
  rr_hz_state_cb_fn_T state_change_cb_ptr;
  rr_hz_state_ind_T   last_state;
  PLMN_id_T           plmn;
  word                lac;
  word                cell_id;
} rr_hz_info_T;

void rr_hz_notify_state_change(
  rr_hz_state_ind_T,
  const gas_id_t gas_id
);

void rr_hz_update_zone_info(
  boolean,
  const gas_id_t gas_id
);

void rr_hz_notify_ps_status(
  boolean,
  const gas_id_t gas_id
);


/**
 * Returns TRUE if new_band is equal to old_band, FALSE otherwise.
 * For the purposes of this function, PGSM and EGSM are considered equal.
 */
extern boolean rr_bands_equal(rr_internal_band_T new_band, rr_internal_band_T old_band);

extern boolean rr_is_any_cell_selection_mode(const gas_id_t gas_id);
extern boolean rr_is_any_cell_reselection_mode(const gas_id_t gas_id);
extern boolean rr_is_automatic_plmn_selection_mode(const gas_id_t gas_id);
extern boolean rr_is_limited_plmn_selection_mode(const gas_id_t gas_id);

extern void rr_send_handover_sync_ind(rr_dedicated_data_T *, const gas_id_t gas_id);

extern void rr_adjust_rxlev_by_scale(byte scale, byte *rxlev);

#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
extern boolean rr_cell_is_in_plmn_lac_range(LAI_T lai);
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */

extern void rr_dtx_override_reset(const gas_id_t gas_id);
extern void rr_dtx_rr_override_on(const gas_id_t gas_id);
extern void rr_dtx_rr_override_off(const gas_id_t gas_id);
extern void rr_dtx_update_l1_if_required(const gas_id_t gas_id);

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
extern void rr_eng_mode_update_dtx_ul_status(boolean dtx_status, const gas_id_t gas_id);

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
extern void rr_eng_mode_update_nw_params(const gas_id_t gas_id);

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
extern void rr_eng_mode_update_scell_config(const gas_id_t gas_id);

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
  gprs_scell_info_t * scell_info_ptr,
  eng_mode_cell_cb_cause_t cause,
  const gas_id_t gas_id
);

#ifdef FEATURE_LTE

extern boolean rr_create_lte_neighbour_list(
  rr_lte_neighbour_list_t * eutran_neighbour_list,
  instances_T * si2quater_instances
);

#endif /* FEATURE_LTE */

/**
  @brief Returns a pointer to the RAT Priority List stored in the
         rr_PLMN_request_details structure.
*/
extern sys_rat_pri_list_info_s_type * rr_rat_pri_list_info(const gas_id_t gas_id);

/**
  @brief Compares if two LTE band masks are equal.
*/
extern boolean rr_chk_if_equal_lte_band_mask(sys_lte_band_mask_e_type mask1, sys_lte_band_mask_e_type mask2);

/**
  @brief Checks if a LTE band mask is empty or not.
*/
extern boolean rr_is_lte_band_mask_empty(sys_lte_band_mask_e_type band_mask);

/**
  @brief Checks if a band mask is empty or not.
*/
extern boolean rr_is_band_mask_empty(sys_band_cap_u_type band_mask, sys_sys_mode_e_type sys_mode);

/**
 * Finds the requested sys_mode entry in priority_list_info
 * array.  If an entry can't be found NULL is returned
 *
 * @param rat_pri_list_info_ptr
 *        sys_mode - requested sys type
 *
 * @return sys_priority_list_info_s_type*
 */
extern sys_priority_list_info_s_type* rr_rat_pri_list_info_get_sys_mode_entry(
  sys_rat_pri_list_info_s_type * rat_pri_list_info_ptr,
  sys_sys_mode_e_type            sys_mode
);

/**
  @brief Returns an indication of whether a specific system mode is included
         in a RAT Priority List.
  @return TRUE if sys_mode is included in the RAT Priority List, FALSE if not.
*/
extern boolean rr_rat_pri_list_info_is_sys_mode_present(
  sys_rat_pri_list_info_s_type * rat_pri_list_info,
  sys_sys_mode_e_type            sys_mode
);

extern boolean rr_rat_pri_list_info_gsm_only(
  sys_rat_pri_list_info_s_type * rat_pri_list_info
);

/**
  @brief Searches the RAT priority list for the bands that should be searched
         when acquiring service on the specified technology.

  @return The bands that should be searched when acquiring service on the
          specified technology, or SYS_BAND_MASK_EMPTY if the technology is
          not present in the RAT priority list.
*/
extern sys_band_cap_u_type rr_rat_pri_list_info_search_band_pref(
  sys_rat_pri_list_info_s_type * rat_pri_list_info,
  sys_sys_mode_e_type            sys_mode
);

/**
  @brief Searches the RAT priority list for the bands that are supported by the
         specified technology.

  @return The bands which are supported on the specified technology, or
          SYS_BAND_MASK_EMPTY if no bands are supported.
*/
extern sys_band_cap_u_type rr_rat_pri_list_info_band_pref(
  sys_rat_pri_list_info_s_type * rat_pri_list_info,
  sys_sys_mode_e_type            sys_mode
);

/**
  @brief Returns an indication of whether the UE is currently operating in
         GSM-only mode or not.

  @return TRUE if the the UE is in GSM-only mode, FALSE otherwise.
*/
extern boolean rr_gsm_only(const gas_id_t gas_id);

/**
  @brief Returns the current band_pref that RR has been provided.

  @return The current band_pref.
*/
extern sys_band_mask_type rr_band_pref(const gas_id_t gas_id);

/**
  @brief Returns the current band mask that RR should use during PLMN
         selection.

  @return The band mask that RR should use during PLMN selection.
*/
extern sys_band_mask_type rr_search_band_pref(const gas_id_t gas_id);

/**
  @brief Returns the band_pref associated with WCMDA.
*/
extern sys_band_mask_type rr_band_pref_wcdma(const gas_id_t gas_id);


/**
  @brief Returns the band_pref associated with UTRAN_TDD.
*/
extern sys_band_mask_type rr_band_pref_utran_tdd(const gas_id_t gas_id);

/**
  @brief Determines if UTRAN TDD support is currently enabled in
         RR and the current PLMN is in the known list of TDS
         PLMN's.

  @return TRUE if UTRAN TDD support is enabled, FALSE if not.
*/
extern boolean rr_is_utran_tdd_enabled_in_nw_plmn(const gas_id_t gas_id);

/**
  @brief Determines if UTRAN TDD support is currently enabled in
         RR.

  @return TRUE if UTRAN TDD support is enabled, FALSE if not.
*/
extern boolean rr_is_utran_tdd_enabled(const gas_id_t gas_id);

/**
  @brief Determines if WCDMA support is currently enabled in
         RR.

  @return TRUE if WCDMA support is enabled, FALSE if not.
*/
extern boolean rr_is_wcdma_enabled(const gas_id_t gas_id);

#ifdef FEATURE_LTE

/**
  @brief Returns the band_pref associated with LTE.
*/
extern sys_lte_band_mask_e_type rr_band_pref_lte(const gas_id_t gas_id);

/**
  @brief Determines if LTE support is currently enabled in RR.

  @return TRUE if LTE support is enabled, FALSE if not.
*/
extern boolean rr_is_lte_enabled(const gas_id_t gas_id);

extern boolean rr_priority_and_threshold_information_available(
  instances_T * si2quater_instances,
  const gas_id_t  gas_id //tlx
);

extern boolean rr_serving_cell_priority_parameters_ie_received(
  gprs_scell_info_t * scell
);

extern rr_resel_priority_e rr_get_resel_priority_for_uarfcn(
  uint16 uarfcn,
  rr_utran_rat_type_t rat_type,
  const gas_id_t gas_id
);

extern boolean rr_utran_priority_parameters_ie_received(
  instances_T * si2quater_instances,
  const gas_id_t  gas_id
);

extern int rr_get_qrxlevmin_for_uarfcn(
  uint16 uarfcn,
  rr_utran_rat_type_t rat_type,
  const gas_id_t gas_id
);

extern uint8 rr_get_thresh_utran_high_for_uarfcn(
  uint16 uarfcn,
  rr_utran_rat_type_t rat_type,
  const gas_id_t gas_id
);

extern uint8 rr_get_thresh_utran_low_for_uarfcn(
  uint16 uarfcn,
  rr_utran_rat_type_t rat_type,
  const gas_id_t gas_id
);

/**
  @brief Get the priority level of the specified E-ARFCN.

  @param earfcn The E-ARFCN to get the QRXLEVMIN for.
  @return The priority level for the E-ARFCN specified.
*/
extern rr_resel_priority_e rr_get_resel_priority_for_earfcn(
  lte_earfcn_t earfcn,
  const gas_id_t gas_id
);

/**
  @brief Get the QRXLEVMIN value for the specified E-ARFCN.

  This function returns the QRXLEVMIN value for the specified E-ARFCN.  This
  is a value in the range of -140 .. -78 dBm.

  @param earfcn The E-ARFCN to get the QRXLEVMIN for.
  @return The QRXLEVMIN value for the E-ARFCN specified in dBm.
*/
extern int16 rr_get_qrxlevmin_for_earfcn(
  lte_earfcn_t earfcn,
  const gas_id_t gas_id
);

/**
  @brief Get the THRESH_HIGH value for the specified E-ARFCN.

  This function returns the THRESH_HIGH value for the specified E-ARFCN.  This
  is a value in the range of 0 - 62 dB.

  @param earfcn The E-ARFCN to get the THRESH_HIGH for.
  @return The THRESH_HIGH value for the E-ARFCN specified in dB.
*/
extern int16 rr_get_thresh_high_for_earfcn(
  lte_earfcn_t earfcn,
  const gas_id_t gas_id
);

/**
  @brief Get the THRESH_LOW value for the specified E-ARFCN.

  This function returns the THRESH_LOW value for the specified E-ARFCN.  This
  is a value in the range of 0 - 62 dB.

  @param earfcn The E-ARFCN to get the THRESH_LOW for.
  @return The THRESH_LOW value for the E-ARFCN specified in dB.
*/
extern int16 rr_get_thresh_low_for_earfcn(
  lte_earfcn_t earfcn,
  const gas_id_t gas_id
);

#endif /* FEATURE_LTE */

extern sys_band_T rr_map_sys_band_class_to_sys_band(
  sys_sband_e_type sys_band_class
);

extern sys_sband_e_type rr_map_sys_band_to_sys_band_class(
  sys_band_T sys_band
);

#ifdef FEATURE_LTE

extern void rr_individual_priorities_store(
  rr_individual_priorities_ie_t * individual_priorities_ie,
  ARFCN_T                         scell_arfcn,
  sys_plmn_id_s_type              plmn,
  const gas_id_t gas_id  //tlx
);

extern void rr_individual_priorities_clear(
  const gas_id_t gas_id
);

extern void rr_dedicated_priority_info_get(
  lte_irat_dedicated_priority_info_s * dst,
  const gas_id_t gas_id
);

extern sys_plmn_id_s_type rr_dedicated_priority_info_get_plmn(
  const gas_id_t gas_id
);

/**
  @brief Dumps the Information in the Individual Priority database.
*/
extern void rr_individual_priorities_dump(
  const gas_id_t gas_id
);

/**
  @brief Checks whether the dedicated priority information is valid
         or not.
  
  @param dedicated_priority_info The dedicated priority information to
         check the validity of.
  @return TRUE if the dedicated priority information is valid, FALSE if
          it is not.
*/
extern boolean rr_dedicated_priority_info_valid(
  lte_irat_dedicated_priority_info_s * dedicated_priority_info
);

/**
  @brief Stores Dedicated Priority information received from another RAT during
         an IRAT procedure.

  @param dedicated_priority_info The Dedicated Priority Information received
         from the other RAT.
  @param arfcn The ARFCN of the current GSM cell which should be used to determine
               the GERAN priority.
  @param plmn The PLMN that the Priority Parameters are valid for.
*/
extern void rr_dedicated_priority_info_store(
  lte_irat_dedicated_priority_info_s * dedicated_priority_info,
  ARFCN_T                            * arfcn,
  sys_plmn_id_s_type                   plmn,
  const gas_id_t                       gas_id
);

/**
  @brief Get the current effective GERAN priority.

*/
extern rr_resel_priority_e rr_geran_priority(
  const gas_id_t gas_id
);

/**
  @brief Determine whether Individual Priorities have been provided to the UE.

  @return TRUE if the UE has valid individual priorities, FALSE otherwise.
*/
extern boolean rr_individual_priorities_valid(
  const gas_id_t gas_id
);

/**
  @brief Assumes ownership of the Dedicated Priorities, and sends an indication
         to the previous owner to clear out their dedicated priorities.

  This function should be called on completion of an X2G procedure, in order to
  information the other Access Stratums in the system that GERAN is now the owner
  of Dedicated Priorities.
*/
extern void rr_dedicated_priorities_take_ownership(
  const gas_id_t gas_id
);

/**
  @brief If dedicated priorities are currently owned by some RAT other than
         GERAN, this function sends a request to that RAT for a copy of
         the dedicated priorities.
*/
extern void rr_dedicated_priorities_request_from_owner(
  const gas_id_t gas_id
);

/**
  @brief Request the specified RAT to clear it's dedicated priorities.

  If owner is LTE_RRC_DEDICATED_PRI_OWNER_LTE, sends LTE_RRC_CLEAR_DEDICATED_PRI_IND.
  If owner is LTE_RRC_DEDICATED_PRI_OWNER_WCDMA, sends XXX.

  @param owner The RAT to send the request to.
*/
extern void rr_dedicated_priorities_request_owner_clears(
  lte_rrc_dedicated_pri_owner_e owner,
  const gas_id_t gas_id
);

#endif /* FEATURE_LTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

/**
  @brief Sends all of the messages in the system information set to RR.
*/
extern void rr_x2g_system_information_send(
  ARFCN_T                       arfcn,
  rr_x2g_system_information_t * system_information,
  const gas_id_t                gas_id
);

/**
  @brief Checks whether L2 Pseudo Length and Protocol Discriminator
         bytes are present in the given system information store.
*/
extern boolean rr_x2g_system_information_headers_present(
  rr_x2g_system_information_t * system_information
);

/**
  @brief Prepends 2 header octets to each message in the system information set.
*/
extern void rr_x2g_system_information_prepend_headers(
  rr_x2g_system_information_t * system_information,
  const gas_id_t                gas_id //tlx
);

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

extern void rr_general_task_start_init(
  void
);

extern void rr_plmn_selection_triggered(const gas_id_t);

extern void rr_cell_selection_triggered(const gas_id_t);

/**
  @brief Wrapper for sys_eplmn_list_equivalent_plmn.
*/
extern boolean rr_eplmn_list_equivalent_plmn(sys_plmn_id_s_type plmn, const gas_id_t gas_id);

/**
  @brief Wrapper for sys_ehplmn_list_equivalent_plmn.
*/
extern boolean rr_ehplmn_list_equivalent_plmn(sys_plmn_id_s_type plmn, const gas_id_t gas_id);

/*!
 * \brief checks if the supplied PLMN is the HPLMN or equivalent to it (EHPLMN)
 * 
 * \param plmn - plmn id received in the SI3 from cell
 *
 * \return boolean - TRUE if the PLMN is in the EHPLMN list or same as HPLMN , FALSE otherwise
 */
extern boolean rr_is_ehplmn_or_hplmn(sys_plmn_id_s_type plmn, const gas_id_t gas_id);

#ifdef FEATURE_VAMOS_II
extern channel_mode_T  rr_remove_vamos2_indication_from_channel_mode(
  channel_mode_T channel_mode
);


extern boolean rr_vamos2_rcvd_chan_mode_matches_curr_chan_mode(
channel_mode_T rcvd_channel_mode,
channel_mode_T current_chan_mode
);

#endif /*FEATURE_VAMOS_II*/


extern void rr_store_service_domain(sys_srv_domain_e_type service_domain,const gas_id_t gas_id);

rr_nw_supp_utran_rats_t rr_nw_supported_utran_rats(gprs_scell_info_t * scell);

#ifdef FEATURE_SGLTE

extern void rr_sglte_dedicated_priority_info_store(
  sglte_l2g_resel_req_ded_pri_info_t * sglte_dedicated_pri_info_ptr,
  const gas_id_t gas_id
);

#ifdef FEATURE_GSM_TDS
extern boolean rr_sglte_is_l1_tds_measurements_configured(const gas_id_t gas_id);

extern void rr_sglte_set_l1_tds_measurements_configured(boolean is_configured, const gas_id_t gas_id);
#endif

#endif /* FEATURE_SGLTE */

#ifdef FEATURE_LTE
extern boolean rr_is_lte_l1_measurements_configured(const gas_id_t gas_id);
extern void rr_set_lte_l1_measurements_configured(boolean is_configured, const gas_id_t gas_id);
#endif

extern rr_internal_band_T rr_determine_actual_band(
  word local_arfcn,
  si_psi_band_ind_T band_ind
);

extern void rr_invalidate_csg_info(sys_detailed_plmn_list_s_type *info_ptr);

/*!
 * \brief Returns the ARFCN of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return ARFCN_T
 */
extern ARFCN_T rr_scell_arfcn(const gas_id_t gas_id);

/*!
 * \brief Returns the BSIC of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return BSIC_T
 */
extern BSIC_T rr_scell_bsic(const gas_id_t gas_id);

/*!
 * \brief Returns the RxLev (average) of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return uint8
 */
extern uint8 rr_scell_rxlev_average(const gas_id_t gas_id);

/*!
 * \brief Returns the PLMN ID of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return PLMN_id_T
 */
extern PLMN_id_T rr_scell_plmn_id(const gas_id_t gas_id);

/*!
 * \brief Returns the LAI of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return LAI_T
 */
extern LAI_T rr_scell_lai(const gas_id_t gas_id);

/*!
 * \brief Returns the RAC of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return uint8
 */
extern uint8 rr_scell_rac(const gas_id_t gas_id);

/*!
 * \brief Returns the Cell Identity of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return uint16
 */
extern uint16 rr_scell_cell_identity(const gas_id_t gas_id);

/*!
 * \brief Returns the service capability of the serving cell.
 * 
 * \param gas_id (in)
 * 
 * \return sys_srv_capability_e_type
 */
extern sys_srv_capability_e_type rr_scell_service_capability(const gas_id_t gas_id);

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
extern boolean rr_check_and_populate_default_irat_prior_params(const gas_id_t gas_id );
extern void    rr_set_rr_using_pseudo_si2q_data(boolean rr_using_pseudo_si2q_data, const gas_id_t gas_id);
extern boolean rr_is_pseudo_si2q_data_used(const gas_id_t gas_id);
extern boolean rr_default_irat_params_update_required(const gas_id_t gas_id);
extern void  rr_restore_default_serv_utran_pseudo_si2q_params(gprs_scell_info_t *serving_cell_ptr, const gas_id_t gas_id);
extern void rr_flush_default_irat_prior_params(const gas_id_t gas_id );

#endif /* FEATURE_FAST_G2L_PSEUDO_SI2Q */

#ifdef FEATURE_LTE
extern void rr_add_gsm_cell_to_depriotized_list(ARFCN_T arfcn, const gas_id_t gas_id);
extern boolean rr_is_gsm_cell_in_depriotized_list(ARFCN_T arfcn, const gas_id_t gas_id);
extern void rr_init_depriotized_list(const gas_id_t gas_id);
#endif /*FEATURE_LTE*/
#ifdef FEATURE_GERAN_HST_OPT
/*===========================================================================
FUNCTION  rr_is_hst_cell_freq_list_present

DESCRIPTION Checks if IRAT acq DB is valid AND is for a high speed cell

RETURN VALUE - TRUE - high speed freq list valid, FALSE - opposite
===========================================================================*/
extern boolean rr_is_hst_cell_freq_list_present(const gas_id_t gas_id);

/*!
 * \brief Returns if High Speed Train (HST) optimization
 *        is allowed
 *
 * \param gas_id (in)
 *
 * \return TRUE - HST opt allowed, FALSE - HST opt not allowed
 */
extern boolean rr_high_speed_train_opt_allowed(const gas_id_t gas_id);
#endif /*FEATURE_GERAN_HST_OPT*/

#endif /* RR_GENERAL_H */

/* EOF */

