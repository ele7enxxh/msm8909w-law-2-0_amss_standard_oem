#ifndef GPL1_GRM_INTF_H
#define GPL1_GRM_INTF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      L 1 GRM GENERAL HEADER FILE

GENERAL DESCRIPTION
This module handles GERAN-to-TRM general purpose functions

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2015 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gpl1_grm_intf.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   --------  ----------------------------------------------------------
23/05/16   db       CR921073 Fix a limitation with PCH in transfer
21/09/15   nm        CR767743 Using actual duration in trm unlock cb for unlock by event
30/03/15   nm        CR765891 Allowing TA for TDS during PS RACH 
20/02/15   sjw       CR793380: adaptive TRM durations for acquisition mode
04/02/15   nk        CR767734 Allowing TA during 2-phase access
02/02/15   br        CR787859:In TRM TBF callback function, calling in task GRM release without requesting GL1 ISR lock
06/01/15   jk        CR773695:DRx device Id not requested for all bands
07/01/15   sp        CR772306: Update TRM of gbta grant for PBR to work
22/10/14   cah       CR737592: optimisation for persistent denial based on Scell RSSI and SnR
04/12/14   sjv       CR766784 Backing out CR759568 
25/11/14   zc/zf/sj  CR759568 Delay TRM release if exiting QBTA 
13/11/14   am        CR754217 Keep GRM internal reason aligned with latest TRM function
03/11/14   ss        CR746380 Set TRM extention flag outside of grm intialisation function.  
31/10/14   cs        CR735918 Assign supported bands for initial ACQ based on UE support
16/10/14   aga       CR735726  Remove obsolete gta_set_wcdma_blacklisted functionality
02/10/14   sjv       CR733766 Get RF Device from grm_info instead of querying TRM
26/09/14   cah       CR729408 Reduce number of TRM API calls in GL1 to reduce impact of processing delays on GL1
22/09/14   ap        CR720862: FR 20912: SGLTE+G DSDA with WTR asymmetric GSM support
17/09/14   sjv       CR724558 For PM, TRM band-reg and release at frame level granularity for perf. optimization
12/09/14   nm        CR723570 GL1 changes for FR21036 - Read own PCH in transfer to BO2.0(to be leveraged for TH/JO)
22/09/14   cah       CR723144: Bolt 2.0 DRDSDS - W+G - G2G interband ncell monitor activity denied while in W+G FC mode
10/09/14   cs        CR722344 Remove old deprecated TRM API usage
09/09/14   pa        CR719024: Implementation of Hybrid Tuneaway Fallback Algorithm
28/08/14   sjv       CR717166 pending activities are organized in an array for trm band release to avoid overwrite in case of diff. activities
26/08/14   pa        CR715814 IFS measured client id is not updated from GL1 before passing to TRM
21/08/14   dv        CR711417 Ensure DR mode is enabled when camping on cell
12/08/14   cah       CR707445 Overlap between PM's in PTM mode causes transaction id corruption
06/08/14   ka        CR699964 Implement Idle RXD support in GL1
31/07/14   cos       CR697355 Bolt 2.0 GL1 updates required for enabling RxD cal with the new DR-DSDS TRM apis
29/07/14   ap        CR695234: Changes for GL1's New Rach interface
24/07/14   ka        CR692214 TRM APS feature changes required with new TRM api's
27/07/14   dv        CR700582 Ensure peending IFS/IRAT activity cleared if trm extension denied
25/07/14   cs        CR697245 New GRM API support for WTR Hopping
23/07/14   cah       CR697586  trm_client.cpp:4539   Assertion  search_activity_q.cnt == 0 failed
16/07/14   ssh       CR689277: On wake up cancel do trm release only if TRM status is denied and TRM/GRM reserve reasons match
14/07/14   cs        CR693967 Get DR-DSDS mode during startup
02/07/14   cs        CR688003 First version

===========================================================================*/

/*===========================================================================

           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "comdef.h"
#include "ms.h"
#include "l1_fm.h"

#include "rfm_device_types.h"

#include "sys_type.h"

#include "l1i.h"

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
#include "trm.h"
#include "geran_dual_sim.h"
#include "geran_multi_sim.h"
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

#include "gl1_drdsds.h"

//TRM AND RFSW are not aligned to use 5 bands. RFSW uses PGSM/EGSM as EGSM only. Therefore, we will still have only 4 bands.
#define MAX_BAND_INFO_GRM 4
/*
 * For a particular target what is the maximum valid RF device id
 * that can be assigned to GERAN WAN/WCDMA WAN and LTE WAN
 * these can be different
 */
#define MAX_VALID_GERAN_RFM_DEVICE         RFM_DEVICE_5
#define MAX_VALID_WCDMA_RFM_DEVICE         RFM_DEVICE_5
#define MAX_VALID_LTE_RFM_DEVICE           RFM_DEVICE_5

#define TRM_BAND_TUNE_DENIED 0x1
#define TRM_BAND_TUNE_OK  0x0
#define DR_TA_FALLBACK_DSC_THRESHOLD (50) /*% of max DSC*/

typedef enum
{
 GRM_START_PERSISTENT_DENIAL_MONITORING,
 GRM_CHECK_PERSISTENT_DENIAL_FLAG,
 GRM_DECREMENT_DENIED_REG_COUNT,
 GRM_RESET_PERSISTENT_DENIAL_MONITORING,
 GRM_ADD_DENIED_BANDS_TO_FREQ_INFO,
 GRM_COUNT_DENIED_ARFCNS,
 GRM_PROCESS_DENIED_ARFCNS,
 GRM_GOOD_SERVING_CELL_CHECK,
 GRM_INIT_SERVING_CELL_LEVELS
}grm_persistent_denial_sm_actions_t;


/* Threshold in dBmx16 -70dBm = -70*16*/
#define GRM_PERSISTENT_DENIAL_RSSI_THRESHOLD					(-1120)
/*6dB => 10 dBx16*/
#define GRM_PERSISTENT_DENIAL_SNR_THRESHOLD					(160)

/*worst case number of paging blocks required to PM on all bands in BA list*/
#define DEFAULT_DEMOD_PAGE_COUNT 4

/*======================TRM DURATIONS========================*/
#define GRM_PSCN_MIN_REQUEST_MS              (45) /* approx 10 frames */
#define GRM_PSCN_MAX_REQUEST_MS             (100)
#define GRM_BCCH_MIN_REQUEST_MS              (65) /* approx 14 frames */
#define GRM_BCCH_MAX_REQUEST_MS             (100)

#define GRM_ACQUISITION_LOW (TRM_ACQUISITION)     /* this can be toggled HI by TRM internally */
#define GRM_ACQUISITION_MED (TRM_ACQUISITION_INV) /* this can be toggled HI by TRM internally */
#define GRM_ACQUISITION_MAX (TRM_ACQUISITION_MAX) /* this is NOT affected by HI/LO settings   */

#define GRM_ACQUISITION_REASON(rsn) (((rsn) == GRM_ACQUISITION_LOW) || ((rsn) == GRM_ACQUISITION_MED) || \
                                     ((rsn) == GRM_ACQUISITION_MAX))

#define CHECK_INCLUDES_WCDMA_ACQUISITION (TRM_ACQUISITION)
#define CHECK_EXCLUDES_WCDMA_ACQUISITION (TRM_ACQUISITION_INV_HI) /* non-GSM clients never use this reason */

#define GRM_PAGING_REASON(pr) (((pr) == TRM_DEMOD_PAGE) || ((pr) == TRM_DEMOD_PAGE_MAX_SENS)|| \
                               ((pr) == TRM_DEMOD_PAGE_CONTINUATION)|| ((pr) == TRM_DEMOD_PAGE_INV))

/*=====Power Monitors IFM Activities======*/
#define MAX_PM_SCHED  12   /*Maximum PM that can be scheduled in one set (set = 3 block in transfer mode, and similar)*/
#define PM_QHEAD      0
#define PM_QTAIL      1



typedef enum {
  /* GRM General */
  GRM_REQUEST          = 0x10,
  GRM_RELEASE,
  GRM_RESERVE_AT,
  GRM_RETAIN_LOCK,

  /* GRM Request and Notify */
  GRM_RAN_ACCESS_RETRY,

  /* GRM Request and Notify Enhanced */
  GRM_RANE_DENIED,
  GRM_RANE_ACQUISITION,
  GRM_RANE_RESELECTION,
  GRM_RANE_TBF_SUSPEND,
  GRM_RANE_GPRS_PRIORITY_IND,
  GRM_RANE_PANIC_RESET,

  /* GRM Get Info */
  GRM_GET_GRANTED,
  GRM_GET_AS_ID_MAPPING,
  GRM_GET_REASON,
  GRM_GET_CHAIN_HOLDER,
  GRM_GET_DEVICE_MAPPING,
  GRM_GET_DR_CAPABILITY,
  GRM_GET_DEVICE_CONFIG,

  /* GRM Modify Chain State */
  GRM_MODIFY_BAND,
  GRM_MODIFY_DURATION,
  GRM_MODIFY_REASON,

  /* GRM Dual Receive Info */
  GRM_DR_INFO,

  /* Inform GRM to share same Chain*/
  GRM_INFORM_GRANT_EVENT_INFO,
  GRM_TAG_MAX
} grm_tag_t;

typedef enum
{
    PENDING_DR_DISABLE,
    PENDING_DR_ENABLE,
    PENDING_DR_NULL,
}grm_pending_dr_state;


typedef enum
{
  SC_BCCH,
  SC_PM,
  SC_FCH_SCH,
  SC_IRAT,
  SC_MAX_ACTIVITIES
}gl1_sc_activities_t;

typedef struct
{
  boolean                          update_in_progress;
  boolean                          pending_meas_release;
  uint32                           start_frame;
  uint32                           duration;
  trm_set_client_state_input_type  client_state_info;
  trm_set_client_state_output_data trm_return_data;
} grm_sc_activity_state_info_t; /*stores the TRM state information per GL1 SC activity.*/

typedef struct
{
  uint8 band_tune_result;
  uint8 arfcn_count;
}grm_denied_band_info_t;

/*~~~~~~~~~~~~~~~grm_info_t~~~~~~~~~~~~~~~~*/
typedef struct
{
  boolean                            next_page_inverted;
  int8                               next_page_inv_cnt;
  uint8                              curr_bs_pa_mfrms;
  uint32                             when;
  uint32                             duration;
  boolean                            valid_trm_booking;
  uint32                             start_frame;
  uint32                             end_frame;
  uint32                             req_n_notify_duration;
  trm_reason_enum_t                  reason;
  trm_reason_enum_t                  rsv_reason;
  uint8                              pch_ticks_to_include_denied_bands;
  grm_denied_band_info_t             denied_band_info[INVALID_BAND]; /*array holding all bands which G can not tune RF to (indixed by SYS_BAND type)*/
  uint8                              denied_arfcn_count;
  uint8                              dsc_threshold;
  boolean                            dr_enabled;
  boolean                            persistent_denial;
  trm_freq_input_type                current_band_info; /*list of frequency bands currently registered with TRM*/
  trm_freq_input_type                irat_band_registered;
  trm_freq_input_type                irat_denied_bands; /*list of IRAT bands we can not tune the RF to*/
  grm_sc_activity_state_info_t       trm_reg_activity[SC_MAX_ACTIVITIES];
  grm_sc_activity_state_info_t       pending_reg_activity[SC_MAX_ACTIVITIES];/*have the pending activity info structure as array as well*/
  /*power_monitor_activity_buffer is a Circular FIFO Queue |_out|_|_in|_|_|_|_|_|_|_|_|
    pm_queue_idx[0] will point to in, that is point at which we can write data next  (PM_QHEAD)
    pm_queue_idx[1] will point to out, that is point at which we can read data from (PM_QTAIL)*/
  grm_sc_activity_state_info_t       power_monitor_activity_buf[MAX_PM_SCHED];/*parallel buffer structure to hold PM related IFM activities*/
  boolean                            pm_valid[MAX_PM_SCHED];
  int8                               pm_queue_idx[2];/*index 0 will be in, index 1 will be out, to track the FIFO Array above*/

  trm_resource_enum_t                resource;
  uint32                             frame_duration;
  uint32                             wup_duration;
  boolean                            wreconf;

  trm_freq_input_type                grm_freq_info;

  trm_request_return_data            grm_request_data;

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
  trm_request_input_info             grm_req_n_not_input_data;
  trm_req_n_not_return_data          grm_req_n_not_data;

  trm_request_notify_enh_input_info  grm_req_n_not_enh_input_data;
  trm_req_n_not_enh_return_data      grm_req_n_not_enh_data;

  trm_dual_rec_info_return_data      grm_dual_receive_info_data;
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

  trm_get_info_input_type            grm_get_info_input_data;
  trm_get_info_return_type           grm_get_info_data;

  trm_modify_data                    grm_modify_chain_state_data;
  trm_modify_chain_state_return_data grm_modify_state_info;
  boolean                            grm_modify_state_info_valid;

#ifdef FEATURE_GSM_WTR_HOP
  trm_connected_mode_hop_callback_data grm_conn_mode_hop_info;
#endif /* FEATURE_GSM_WTR_HOP */

  trm_unlock_callback_data           grm_unlock_cb_data;
  grm_unlock_callback_t              grm_unlock_callback;
  /* RSSI: Calculate from MPH_DSC_THRESHOLD_IND*/
  dBx16_T                            rssi;
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
  /* SNR: Tuneaway Mode: On 4th PB*/
  dBx16_T                            dualRx_SNR;

  /*SNR of paging block decode. Used for persistent denial activation/deactivation*/
  dBx16_T                            Scell_SNR;

  /*good serving cell indicator based on RSSI and Scell_SnR*/
  boolean good_serving_cell;
  
  /* TRUE: when request dual Rx in tuneaway Mode: 1 in 4 PB*/
  boolean                            dualRx_request;
  /* Tuneaway state machine states*/
  gl1_drdsds_hybrid_tuneaway_state   hybrid_tuneaway_state;
  gl1_drdsds_drmode_rxd_priority     rxd_priority;
  /* Range 0-4, on 3rd, disbale DRDSDS, 4th Enable DRDSDS*/
  uint8                              paging_block_count;
  /*Range 0-3, consecutive bad pch blocks*/
  uint8                              bad_paging_block_count;
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */
} grm_info_t;

extern trm_band_t grm_valid_gsm_sys_bands[][MAX_BAND_INFO_GRM];
extern uint16   grm_valid_gsm_sys_bands_cnt[];

extern trm_band_t grm_valid_gsm_sys_bands_divrx[][MAX_BAND_INFO_GRM];
extern uint16   grm_valid_gsm_sys_bands_cnt_divrx[];

extern rfm_device_enum_type grm_get_max_valid_rf_device_for_client(
                              trm_client_enum_t client_id );

/*Pending DR state transition */
extern grm_pending_dr_state               pending_dr_state;

extern void grm_clear_freq_info(
              const trm_client_enum_t client_id );

/*helper functions*/
trm_reason_enum_t grm_gl1_get_current_trm_reason(gas_id_t gas_id);

void grm_gl1_set_current_trm_reason(trm_reason_enum_t reason, gas_id_t gas_id);

boolean grm_gl1_get_dr_status(gas_id_t gas_id);

/*===========================================================================

FUNCTION grm_add_sys_band_type_to_trm_freq_input_type

DESCRIPTION
  Adds new freq_info band information to to active list for client if required

PARAMS
  None

DEPENDENCIES

RETURN VALUE

===========================================================================*/
void grm_add_trm_band_to_freq_input_type(   const trm_band_t          band,
                                                    const trm_client_enum_t   client_id);

/*===========================================================================

FUNCTION grm_get_trm_band_from_g_pri

DESCRIPTION

Takes the priority and gas id and works out what TRM band will be used for activity.

PARAMS
l1_fm_priority_T pri
gas_id_t gas_id


DEPENDENCIES

RETURN VALUE
TRM band to be used for channel maintainence activity.

===========================================================================*/

sys_band_class_e_type grm_get_trm_band_from_g_pri (l1_fm_priority_T pri,gas_id_t gas_id);

/*to be mainlined under enhancement CR*/
#ifdef FEATURE_DENIED_ARFCN_PROCESSING
/*===========================================================================

FUNCTION grm_denied_arfcn_processing

DESCRIPTION
  handles actions associated with denied ARFCN monitoring

PARAMS
  None

DEPENDENCIES

RETURN VALUE boolean to indicate if persistant denial monitoring is enabled.

===========================================================================*/
void  grm_denied_arfcn_processing (ARFCN_T *arfcn, uint32 count, grm_persistent_denial_sm_actions_t action, gas_id_t gas_id);
#endif
/*===========================================================================

FUNCTION grm_persistent_denial_monitoring

DESCRIPTION
  handles actions associated with persistant TRM denial monitoring

PARAMS
  None

DEPENDENCIES

RETURN VALUE boolean to indicate if persistant denial monitoring is enabled.

===========================================================================*/

extern boolean grm_persistent_denial_monitoring(gas_id_t gas_id , grm_persistent_denial_sm_actions_t action);

/*===========================================================================

FUNCTION grm_add_arfcn_to_trm_freq_input_type

DESCRIPTION
  Adds new freq_info band to active list for client if required

PARAMS
  None

DEPENDENCIES

RETURN VALUE boolean to indicate if freq store was updated

===========================================================================*/
extern boolean grm_add_arfcn_to_trm_freq_input_type(
                 const ARFCN_T           gl1_freq_info,
                 const trm_client_enum_t client_id );

/*===========================================================================

FUNCTION      gl1_map_gsm_band_to_sys_band

DESCRIPTION   Maps sys_band_t to sys_band_class_e_type

RETURN VALUE  sys_band_class_e_type

===========================================================================*/
extern sys_band_class_e_type gl1_map_gsm_band_to_sys_band(
                               const sys_band_T band );

/*===========================================================================

FUNCTION      gl1_map_sys_band_to_gsm_band

DESCRIPTION   Maps sys_band_class_e_type to sys_band_T

RETURN VALUE  sys_band_T

===========================================================================*/
extern sys_band_T gl1_map_sys_band_to_gsm_band(
                    const sys_band_class_e_type band_e );

extern grm_info_t *grm_get_info_ptr_from_client(
                     const trm_client_enum_t client_id );
extern grm_info_t *grm_get_info_ptr_from_gas_id(
                     const gas_id_t gas_id,
                     const boolean rx_div );

extern gas_id_t          gl1_map_client_id_to_gas_id(
                           const trm_client_enum_t client_id );
extern trm_client_enum_t gl1_map_gas_id_to_client_id(
                           const gas_id_t          gas_id,
                           const boolean           rx_div);

extern rfm_device_enum_type grm_get_rf_device(
                              trm_client_enum_t client_id,
                              gas_id_t          gas_id );

extern rfm_device_enum_type grm_get_device_mapping(
                              trm_client_enum_t  requested_client_id,
                              trm_get_info_data *input_data_p,
                              gas_id_t           gas_id );

extern trm_grant_return_enum_type grm_request(
         trm_client_enum_t   client_id,
         trm_duration_t      duration,
         trm_reason_enum_t   reason,
         trm_resource_enum_t resource );

extern trm_grant_return_enum_type  grm_trm_request_api(
                     trm_client_enum_t   client_id,
                     trm_duration_t      duration,
                     trm_reason_enum_t   reason,
                     trm_resource_enum_t resource);
extern void grm_inform_grant_event(
              trm_client_enum_t   client_id,
              trm_resource_enum_t resource,
              trm_duration_t      duration,
              trm_reason_enum_t   reason,
              trm_grant_event_enum_t     grant_event);   
extern trm_grant_return_enum_type grm_get_granted(
         trm_client_enum_t         client_id,
         trm_get_info_return_type *output_data_p,
         gas_id_t                  gas_id );

idle_data_T *get_ptr_to_idle_data(
               gas_id_t gas_id);

extern void grm_change_reason(
              trm_client_enum_t client_id,
              trm_reason_enum_t new_reason,
              gas_id_t          gas_id );
extern void grm_change_priority(
              trm_client_enum_t client_id,
              trm_reason_enum_t new_priority,
              gas_id_t          gas_id );

extern void grm_modify_chain_state(
              trm_client_enum_t client_id,
              grm_tag_t         grm_tag,
              gas_id_t          gas_id );

extern void grm_retain_lock(
              trm_client_enum_t     client_id,
              grm_unlock_callback_t unlock_callback,
              gas_id_t              gas_id );

extern trm_reason_enum_t grm_get_acquisition_reason(
                           const gas_id_t gas_id);

extern trm_client_priority_enum_t grm_get_client_priority(
                                    trm_client_enum_t client_id );

extern boolean gl1_drds_debug(
                 const gas_id_t gas_id );

extern void grm_init_valid_gsm_sys_bands(
                 const gas_id_t gas_id );

extern void grm_set_valid_acquisition_bands(
                 const gas_id_t gas_id );

extern void grm_initialise(
              const trm_client_enum_t client_id );

extern void grm_set_extension_flags(
              const trm_client_enum_t client_id );

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )

/*----------------Call in task definitions----------------*/
/* This defines the structures and function prototypes
for the TRM interaction within the task, called via ISR */

/*~~~~~~~~~~~grm_freq_request_and_notify_enh~~~~~~~~~~~*/
typedef struct{
  IMH_T                           header;
  trm_client_enum_t               client_id;
  trm_resource_enum_t             resource;
  trm_duration_t                  duration;
  trm_reason_enum_t               reason;
  grm_tag_t                       grm_tag;
  gas_id_t                        gas_id;
} grm_request_and_notify_enhanced_data_t;

/*~~~~~~~~~~~grm_release~~~~~~~~~~~*/
typedef struct{
  IMH_T                           header;
  trm_client_enum_t               client_id;
  grm_tag_t                       grm_tag;
  gas_id_t                        gas_id;
} grm_release_data_t;

/*~~~~~~~~~~~grm_reserve_at~~~~~~~~~~~*/
typedef struct{
  IMH_T                           header;
  trm_client_enum_t               client_id;
  trm_time_t                      when;
  trm_duration_t                  duration;
  uint32                          frames_required;
  uint32                          frame_duration;
  trm_reason_enum_t               reason;
  trm_resource_enum_t             resource;
  gas_id_t                        gas_id;
} grm_reserve_at_data_t;

/*~~~~~~~~~~~~~~grm_abort_callback_t~~~~~~~~~~~~~*/
typedef struct
{
  IMH_T                   header;
  trm_client_enum_t       client;
  trm_unlock_event_enum_t event;
  uint32                  unlock_by_sclk;
  gas_id_t                gas_id;
} gl1_grm_abort_callback_t;

#ifdef FEATURE_G2X_TUNEAWAY
/*~~~~~~~~~~~~~~~~gl1_trm_abort_callback_t~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~ used by the new G2X re-worker ~~~~~~~~~~~~~~~*/
typedef struct
{
  IMH_T                   header;
  trm_client_enum_t       tbf_client_id;
  trm_client_enum_t       winning_client_id;
  trm_reason_enum_t       winning_client_reason;
  trm_unlock_event_enum_t event;
  uint32                  unlock_in_sclk;
  uint32                  winning_client_duration;
  gas_id_t                gas_id;
} gl1_grm_preemption_abort_callback_t;
#endif

#ifdef FEATURE_G2T_2PHASE_ACCESS
typedef struct
{
  IMH_T                   header;
  trm_client_enum_t       tbf_client_id;
  trm_client_enum_t       winning_client_id;
  trm_unlock_event_enum_t event;
  uint32                  unlock_in_sclk;
  gas_id_t                gas_id;
} gl1_gta_for_tds_abort_callback_t;
#endif // end of FEATURE_G2T_2PHASE_ACCESS

#ifdef FEATURE_GTA_DURING_PS_RACH
typedef struct
{
  IMH_T                   header;
  trm_client_enum_t       winning_client_id;
  trm_reason_enum_t       winning_client_reason;  
  trm_unlock_event_enum_t event;
  uint32                  unlock_in_sclk;
  gas_id_t                gas_id;
} gl1_grm_rach_ps_gta_abort_callback_t;
#endif

/*~~~~~~~~~~~~~~grm_grant_callback_t~~~~~~~~~~~~~*/
typedef struct
{
  IMH_T                      header;
  trm_client_enum_t          client;
  trm_grant_return_enum_type grant_status;
  trm_duration_t             duration;
  gas_id_t                   gas_id;
} gl1_grm_grant_callback_t;

extern trm_dual_receive_enum_type grm_get_dr_capability(
                                    trm_client_enum_t client_id,
                                    gas_id_t          gas_id );

extern trm_asid_mapping_return_data grm_get_as_id_mapping(
                                      trm_client_enum_t requested_client_id,
                                      gas_id_t          gas_id );

extern boolean grm_extend_duration(
                 trm_client_enum_t client_id,
                 trm_duration_t    duration,
                 gas_id_t          gas_id );
extern boolean grm_change_duration(
                 trm_client_enum_t client_id,
                 trm_duration_t    duration,
                 gas_id_t          gas_id );

extern boolean gl1_drdsds_enabled(
                 const gas_id_t gas_id );

extern void grm_send_band_release_to_trm(
              gas_id_t            gas_id,
              gl1_sc_activities_t activity,
              boolean             clear_irat_bands );

extern void grm_send_band_release_if_required(
       gas_id_t gas_id);

extern void grm_set_drx_cycle(
              uint32 paging_mframes, gas_id_t gas_id );

extern void grm_set_dr_enabled_status(
              const gas_id_t gas_id );

extern void grm_exchange_lock(
                 trm_client_enum_t dest_client_id,
                 trm_client_enum_t source_client_id,
                 gas_id_t          gas_id );

/*===========================================================================

FUNCTION  gl1_handle_grm_requests

DESCRIPTION
  Handles the issuing of TRM requests for sudden changes of state
  to access mode, dedicated mode, transfer mode. The issuing of the fw
  and rf loading is also contained within this state machine.

DEPENDENCIES
  l1_tskisr_blk,l1_isr_data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean gl1_handle_grm_requests(
                 trm_reason_enum_t reason,     boolean  reset_state,
                 ARFCN_T           arfcn_info, gas_id_t gas_id );

/*===========================================================================

FUNCTION   call_in_task_grm_request_and_notify_enhanced

DESCRIPTION
   Sends a message to the layer1 task telling it to call the function
   grm_request_and_notify_enhanced().

   IN - data - The data to be passed to the function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void call_in_task_grm_request_and_notify_enhanced(
              grm_request_and_notify_enhanced_data_t *params_p );

/*===========================================================================

FUNCTION   call_in_task_grm_release

DESCRIPTION
   Sends a message to the layer1 task telling it to call the function
   trm_release().

   IN - data - The data to be passed to the function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void call_in_task_grm_release( trm_client_enum_t client_id );

/*===========================================================================

FUNCTION   call_in_task_grm_reserve_at

DESCRIPTION
   Sends a message to the layer1 task telling it to call the function
   trm_release().

   IN - data - The data to be passed to the function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void call_in_task_grm_reserve_at( grm_reserve_at_data_t data);

/*===========================================================================

FUNCTION  grm_abort_callback

DESCRIPTION
  This function handles the trm abort callback for both GSM clients.
  And changes the trm_status within the clients global data, this is picked
  up by the ISR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void grm_abort_callback( trm_unlock_callback_data *abort_cb_data_p );

typedef enum
{
  GRM_IGNORE_UNLOCK_REQUIRED,  /* unlock callbacks will ignore UNLOCK_REQUIRED events */
  GRM_ALLOWS_UNLOCK_REQUIRED   /* unlock callbacks will handle UNLOCK_REQUIRED events */
} grm_unlock_mode_t;

/*===========================================================================

FUNCTION  grm_retain_lock_for_acquisition

DESCRIPTION
  Installs an appropriate unlock callback based on current acquisition priority

DEPENDENCIES
  Must only be called when client already holds a lock

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void grm_retain_lock_for_acquisition(       trm_client_enum_t acq_client,
                                             const grm_unlock_mode_t unlock_mode,
                                             const gas_id_t          gas_id );

extern timetick_type grm_get_acquisition_duration(const gas_id_t gas_id);

/*===========================================================================

FUNCTION  grm_enh_grant_callback

DESCRIPTION
  This function handles the enhanced GRANT callback for both GSM clients. It changes
  the trm_status within the clients global data, this is picked up by the ISR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void grm_enh_grant_callback(
              trm_client_enum_t          granted_client,
              trm_grant_return_enum_type grant_status,
              trm_duration_t             duration );


/*===========================================================================

FUNCTION  grm_tbf_enh_grant_callback

DESCRIPTION
  This function handles the enhanced GRANT callback for the GSM client
  that is doing the Packet transfer . It changes
  the trm_status within the clients global data, this is picked up by the ISR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void grm_tbf_enh_grant_callback(
       trm_client_enum_t               client,
       trm_grant_return_enum_type      grant_status,
       trm_duration_t                  duration );

/*===========================================================================

FUNCTION  gpl1_multi_sim_calculate_grm_reservation_time

DESCRIPTION
  This function determines the required TRM request period in sclcks,
  factoring in the time during the frame at which this is called.

DEPENDENCIES
  None

RETURN VALUE
  TRM period request in sclcks.

SIDE EFFECTS
===========================================================================*/
uint32 gpl1_multi_sim_calculate_grm_reservation_time(
         uint32   requested_FN,
         gas_id_t gas_id );

/* GRM definitions start here */
boolean grm_send_trm_band_registration_start(const trm_band_t band, gl1_sc_activities_t sc_activity, uint32 frames_required,gas_id_t gas_id,  grm_info_t *info_ptr);

boolean grm_check_idle_sc_activity_ok(l1_fm_priority_T pri, gas_id_t gas_id, uint32 frames_required, uint32 reserved_fn);

void grm_add_freq_type_to_trm_freq_input_type(
       const trm_frequency_type_t gl1_freq_info,
       const trm_client_enum_t    client_id );

void grm_add_sys_band_type_to_trm_freq_input_type(
       const trm_band_t band,
       const trm_client_enum_t    client_id,
       gl1_sc_activities_t sc_activity);

void grm_clear_stale_client_state_information(const trm_client_enum_t    client_id , gl1_sc_activities_t sc_activity);

void grm_update_client_state_info(grm_sc_activity_state_info_t * activity_info, gl1_sc_activities_t activity);

boolean grm_enable_disable_dual_receive( boolean enable, gas_id_t gas_id );

#ifdef DEBUG_DRDSDS
char* map_client_info_state_to_debug_string (trm_client_state_enum_type trm_state) ;
char* map_gsm_sys_band_to_debug_string(sys_band_T gsm_band);
char* map_trm_state_op_to_debug_string (trm_state_oper_enum_type state_type);
char* map_transaction_id_to_gsm_sc_activity_debug_string(uint8 id);
char* map_measured_client_to_debug_string(trm_client_enum_t    measured_client);
#endif

/* GRM definitions start here */
extern void grm_set_slot_cycle(
              const trm_client_enum_t client_id,
              const uint8             bs_pa_mfrms);

extern void grm_set_acquisition_reason(
              const trm_reason_enum_t acq_reason,
              const gas_id_t          gas_id );
extern trm_reason_enum_t grm_get_acquisition_reason(
                           const gas_id_t gas_id);

extern void grm_reserve_at(
                    trm_client_enum_t   client_id,
                    trm_time_t          when,
                    trm_duration_t      duration,
                    trm_reason_enum_t   reason,
                    trm_resource_enum_t resource );

extern void grm_reserve_at_enhanced(
                    trm_client_enum_t   client_id,
                    trm_time_t          when,
                    trm_duration_t      duration,
                    trm_reason_enum_t   reason,
                    uint32              required_frame,
                    uint32              frame_duration,
                    gas_id_t            gas_id,
                    trm_resource_enum_t resource );

#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER
extern void grm_reserve_at_ccch_in_trans(
                    trm_client_enum_t   client_id,
                    trm_time_t          when,
                    trm_duration_t      duration,
                    trm_reason_enum_t   reason,
                    uint32              required_frame,
                    uint32              frame_duration,
                    gas_id_t            gas_id,
                    trm_resource_enum_t resource );
boolean grm_release_pchxfer(trm_client_enum_t client_id, gas_id_t gas_id );
#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER*/

extern void grm_release_or_exchange(
                    trm_client_enum_t   client_id,
                    gas_id_t            gas_id );

extern void grm_register_pbr_info( trm_client_enum_t client_id,
                                   uint32            drx_cycle );

void grm_register_aps_info( gas_id_t gas_id );

extern void grm_reserve_at_gts( gas_id_t gas_id, int32 wup_duration, uint32 wakeup_fn );
extern boolean gl1_check_trm_grm_reasons_match(gas_id_t gas_id);

extern void gl1_update_grm_reservation_for_ta( gas_id_t gas_id );

extern void gpl1_multi_sim_setup_afc_tt( gas_id_t gas_id );

extern void grm_request_at_warmup( gas_id_t gas_id );

extern void grm_request_and_notify(
              trm_client_enum_t   client_id,
              trm_resource_enum_t resource,
              trm_duration_t      duration,
              trm_reason_enum_t   reason,
              grm_tag_t           grm_tag );

extern void grm_request_and_notify_enhanced(
              trm_client_enum_t   client_id,
              trm_resource_enum_t resource,
              trm_duration_t      duration,
              trm_reason_enum_t   reason,
              grm_tag_t           grm_tag );

extern void gl1_msim_reset_reserve_at_count( const gas_id_t gas_id );

#ifdef FEATURE_GSM_WTR_HOP
extern void grm_send_wtr_hop_async_rsp(
              const boolean  wtr_hop_success,
              const gas_id_t gas_id );
#endif /* FEATURE_GSM_WTR_HOP */

boolean grm_check_valid_freq_info( grm_info_t *grm_info_p,
                                   gas_id_t    gas_id );

boolean grm_is_denied_band(const sys_band_T band, gas_id_t gas_id);

void grm_set_device_configuration( gas_id_t gas_id );
boolean grm_device_config_is_drdsds( void );

/*---Power Monitor Circular FIFO Queue Management Functions---*/
void pm_activity_queue_init(gas_id_t gas_id);
int8 pm_activity_add_to_buffer (boolean valid, gas_id_t gas_id);
grm_sc_activity_state_info_t* pm_activity_remove_from_buffer(gas_id_t gas_id);
void pm_activity_band_release(boolean flush, gas_id_t gas_id);

trm_set_irat_band_device_info grm_get_irat_rf_dev_from_grm_info(sys_band_class_e_type bandin, gas_id_t gas_id);
/*===========================================================================

FUNCTION   call_in_task_grm_release_in_callback

DESCRIPTION
   Sends a message to the layer1 task telling it to call the function
   trm_release().
   This is a streamlined version call_in_task_grm_release(). It does not
   contain any function requires GL1 lock, can be called from TRM callback
   functions to avoid mutex deadlock.

   IN - data - The data to be passed to the function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void call_in_task_grm_release_in_callback( trm_client_enum_t client_id );

#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

#endif /* GPL1_GRM_INTF_H */
