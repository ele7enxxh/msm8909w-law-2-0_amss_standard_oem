#ifndef SRCHCMMEAS_H
#define SRCHCMMEAS_H

/*============================================================================
                            S R C H C M D R V . H
DESCRIPTION

Copyright (c) 2000-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchcmmeas.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
02/24/15    ap      Initial check-in for WL1 QSH framework 
09/20/15    wz/ar   Add prototypes to query if W2G/W2L CM/FMO searches are ongoing
03/17/15    mk      W2W IRAT threshold update for JOLOKIA
11/24/14    sr      Changes to issue suspended step1 srch by FMO scheduler during EDRX call
11/24/14    mm      Extern prototype of init IRAT cell DB
11/19/14    gm      To implement changes to inform TRM if div is needed for measurement.
11/17/14    cc      Send gap start action time in CM/FMO for FW to get a reference frame
11/14/14    wz      reset step1 srch when CM is deactivated 
11/13/14    psr     Added function prototypes for srchcm_query_interf_fmo_srch_type and srchcm_query_interf_fmo_freq
09/08/14    ks      Update macro to check that we do not pass in a NULL ptr
08/23/14    gv      Featurization to support W compiled out
06/24/14    sr      Changes to not enter CPC lite sleep if CM interf step1 is in progress
06/23/14    gj      DCHSUPA Featurization code changes for searcher
06/19/14    cc      IRAT DR-DSDS check in
06/11/14    cc      Mods to support back to back W2W Interf registrations
                    and de-registrations with CXM in DSDA mode
05/30/14    mm      Initialize driver cell database at DPCH setup
05/30/14    mm       Init/reset driver flag support for IRAT
05/30/14    dp      W2W restructure with mult step1 peak search in pn-acq gaps
05/29/14    mm      Update IRAT/CM driver initialization and RxLM buffer allocation call flow
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
05/01/14    ar      Add enum type for InterF panic mode
04/30/14    mm      Result handling for interf searches in FACH
04/17/14    db      Increase the allowed number of W2G CM scheduled commands to 6 in a CM gap
04/15/14    mm      Field for detect search done in inter-freq searches
04/14/14    ks      Remove sending XO values to WFW
04/04/14    gsk      Priority timers for inter-freq search in EDRX
04/02/14    mm      3C IRAT/CM support
03/10/14    ks      Bolt FED API checkin
12/20/13    dp      Allow partial task completion from FW for DSDA
12/20/13    jk      Added an API that tells us, if compressed mode measurements is performed on
                    the frequency passed as an argument.
12/09/13    ar      Modify prototype for srchcm_update_freq_srch_allowed
11/27/13    hk      Unification of non-intra module cleanup procedures
11/05/13    dp      Fixed W2W early-frequency-switch algorithm
10/28/13    ar      Moving GSM module cleanup to L1M
10/17/13    bs      Preliminary 3C Support
10/15/13    mm      Initial FE-FACH check-in
10/03/13    cc      Fix the cleanup decision for RSSI in SCH
09/30/13    cc      DBDC IRAT support check in
09/23/13    as      Porting Dual Sim from Triton to Dime.
09/12/13    jd      Searcher redesign
08/12/13    cc      DBDC CM W2W feature check in
06/20/13    mm      Update for RF carrier type and secondary uarfn
05/16/13    mm      CM GSM cmd sequence max cmd macro
04/23/13    mm      Passing neighbor RxLM information for W2W inter-freq measurements every CM/FACH gap.
12/17/12    mm      Interface changes for AS ID passed from RRC to GL1 functions
12/11/12    cc      0x417A log pacekt improvement
11/17/12    cc      W2W interF and CM feature for Dime
10/15/12    jd      Memory Optimization Changes
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/21/12    sr      Added support to inform edrx inter freq when searches are done
06/07/12    vs      MBMS feature cleanup.
02/03/12    dp      Get RSSI Threshold from common external #define
01/27/12    vs      Nikel feature cleanup.
11/11/11    yh      Optimization to allow concurrent GSM searches in CM
05/26/11    dp      Added support for W2W XO Parameter passing to FW
04/27/11    dp      Added RXLM support for IRAT and W2W searches
02/25/11    mr      Added INTLOCK and INTFREE around _pn_srch_status_cb and 
                   _step1_srch_status_cb to prevent a race-condition
10/15/10    yh      Added support for W2L Compressed mode
08/30/10    yh      Make carrier_spacing uint16
08/08/10    sj      Added unified PLT support
06/21/10    mr      Compiler warning fixes
04/14/10    vsr     Enhancements to reduce IF false alarms
03/19/10    yh      Add DC+CM support.
04/20/09    yh      Compiler warning fixes.
04/17/09    yh/uk   Add W2G mutex into W2G drivers code
11/13/08    yh      Add CM support
10/03/08    hk      Bringing in 7k Mailine fixes
10/26/07    sk      Checking in MBMS branch code into mainline with MBMS feature disabled
10/16/07    yh      Do not count gaps taken by intraf toward ID abort cnt.
09/05/07    rgn     Pass in intra frequency hho indicator to cm init drv function
07/30/07    rgn     Created a new function to perform all gsm measurement 
                    cleanups
05/23/07  ub/sk     code cleanup
05/14/07    sk      Defined extern srch_meas_dummy_process_rssi_result()
01/05/06    yh      Mainline FEATURE_CM_GSM_SEARCH_RUNNING_RSSI_AVE and
                    FEATURE_CM_GSM_SEARCH_GAP_USE_OPTIMIZE
10/11/06    yh      Added Interf. rescheduling when DM is delayed
10/06/06    yh      Support for FEATURE_CM_GSM_UNLOCK_BSIC_ID_CELL which unlocks
                    the cell doing BSIC ID if this cell is worse than -100dB
                    and there is another cell in the BSIC list which is stronger
                    than -100dB and this cell is 10dB stronger from the unlocking
                    cell.
10/06/06    yh      Support for FEATURE_CM_GSM_RSSI_REPORT_TIMER which limits
                    RSSI meas. reporting to 160ms if we are identifying
                    the first BSIC (when reconf. list is empty).
08/24/06    yh      Eliminate last_meas_rssi_dbmx16 field from cell struct.
06/15/06    bbd     Added thresholds to support multiple freq optimization for
                    interF CM measurements.
05/12/06    bbd     Changed SRCHCM_INTERF_MAX_RECONF_FAILURE to 10.
04/05/06    sk/bbd  Added frequency offset to srchcm_gsm_cell_struct_type
03/20/06    bbd     Added force_head parameter to srchcm_interf_seek_next_undetect_cell().
01/16/06    gs      Updated RFAPI calls from common header file
12/06/05    bbd     Added support for FEATURE_RF_HAS_L1_INTERFACE.
12/05/05    bbd     Added support for CM STTD searches.
09/14/05    gs      Added correct RF compressed mode support include file
                    based on target
10/10/05    gs      Added srchcm_interf_abort_reconf_list_srch().
09/07/05    bbd     Increased SRCHCM_GSM_MAX_PWR_SCAN_PER_MEAS_PRD to 20 to prevent
                    first samples from being pushed out of the buffer when RSSI gaps
                    are dense.
09/07/05    gs      Added meas info override boolean to meas_info structure
                    Added rssi_valid flag to ARFCN info structure
08/31/05    gs      Defined enum srchcm_interf_search_type_enum_type
08/18/05    bbd     Define SRCHCM_GSM_NUM_WORD16_SCH_RX_HDR_MDSP as MDSP_RX_HDR_SIZE.
08/16/05    bbd     Added support for dynamic switching of gap meas purpose under
                    FEATURE_CM_GSM_SEARCH_GAP_USE_OPTIMIZE.
08/03/05    bbd     Added support for run-time determination of MDSP RF setup buffer
                    size under FEATURE_RUNTIME_RFSETUP_BUF_SIZE.
07/18/05    gs      Added macros related to RSSI sample timestamp
                    Moved few macros from source file to header file
                    Added ARFCN info structure and externalized new functions
                    added for ARFCN info maintainance
06/15/05    gs      Added detected cell list search floor threshold
                    Added macro for number of failure to move cell to
                    undetected if field perforce mode is disabled
05/24/05    vp      The changes were provided by Gurdeep. Added structure 
                    element last_meas_gap to inter-freq info structure. The code is 
                    not added under the flag FEATURE_FILTER_INTERF_MEASUREMENTS
                    to aid CM module unit testing
05/19/05    bbd     Changed data type of tone_offset in
                    srchcm_gsm_fcch_detect_meas_result_struct_type to resolve lint errors.
04/28/05    bbd     Fixes for passing GL1 HW tags parameters during sending of GSM commands
                    and retrieval of results.
04/20/05    bbd     Changed FEATURE_TWO_PARAM_ASYNC_RX_METRICS to
                    FEATURE_USE_LATEST_GL1_HW_API.
                    Added support for GL1 HW abstraction to pass parameters needed
                    for RF driver. These parameters are encapsulated in the "tags" field.                    
04/12/05    rc      Removed field num_opt_success_cnt from the structure
                    srchcm_interf_cell_struct_type.
                    Removed SRCHCM_INTERF_MAX_NUM_OPT_SUCCESS_CNT.
03/15/05    gs      Added new macro SRCHCM_INTERF_MAX_RECONF_FAILURE
                    Added structure member num_reconf_failure in cell
                    Added structure member done_with_step1_peak in
                    inter-freq info structure and macros to access it.
01/27/05    cpe     Added a new field num_opt_success_cnt to the structure
                    srchcm_interf_cell_struct_type
                    Added SRCHCM_INTERF_MAX_NUM_OPT_SUCCESS_CNT.
01/21/05    bbd     Support for removing burst_matric_meas under feature name
                    FEATURE_TWO_PARAM_ASYNC_RX_METRICS. Defining this feature 
                    removes burst_matric_meas usage.
12/08/04    bbd     Added SRCHCM_GSM_BSIC_VER_MEAS_AGE_MAX
11/30/04    bbd     Added in_use field to srchcm_gsm_meas_info_struct_type.
11/19/04    bbd     Include "stdlib.h" to fix compiler warnings about labs().
09/22/04    eh      Added and modified RFWCDMACM_* constants.
07/23/04    gs      Added SRCHCM_GSM_BSIC_ID_FORBID_AFTER_F_RECONF_ABOR_TIMER
                    macro
                    Added member forbid_bsic_id and forbid_bsic_id_timer
                    to structure srchcm_gsm_cell_struct_type
06/07/04    gs      Externalixed following vars/function
                    Var srchcm_meas_cell_bsic_ver_status_update_pending
                    Fn srchcm_gsm_update_bsic_ver_list and
                    srchcm_gsm_update_bsic_ver_status
05/10/04    gs      Added enum definition for srchcm_interf_mode_enum_type
                    Externalized global variable srchcm_interf_mode
04/13/04    gs      Replaced struct member forbidden_qs_offset_valid with
                    num_forbidden_offset
                    Forbidden offsets are now array.
                    Added macro SRCHCM_GSM_NUM_MAX_FORBIDDEN_OFFSET
                    Externalized global variable srchcm_gsm_rssi_meas_age
                    Externalized function srchcm_meas_query_known_cell_qs_offset
04/08/04    gs      Reduced SRCHCM_GSM_MAX_BSIC_ID_MEAS_UPDATE_INFO to 1 to
                    save some MIPS.
                    Added SRCHCM_GSM_MAX_BSIC_RECONF_MEAS_UPDATE_INFO to
                    restrict number of cells for reconf.
                    Removed macro SRCHCM_GSM_SCH_DEC_CMD_MAX_PER_GAP. It is
                    moved to srchcmdrv.c
                    Added structure member srchcm_gsm_cell_struct_type and
                    member for keep forbidden offset info to strcuture
                    srchcm_gsm_cell_struct_type.
                    Removed unused strcture member cell_index from GSM
                    meas info for cell reconf.
                    Added boolean structure member purpose_bsic_id to structure
                    srchcm_gsm_sch_decode_result_struct_type to identify
                    the type of SCH decode being polled.
                    Externalized function srchcm_meas_force_reconf_abort
03/06/04    gs      Added structure element mdsp_buffer status to structure
                    srchcm_interf_rf_set_info_struct_type
                    Added type definition for interf search done callback
                    Externalized fuction srchcm_interf_cleanup_interf_meas.
03/03/04    gs      Externalized function srchcm_gsm_query_gsm_meas_unused
02/19/04    gs      Removed reference to FEATURE_SRCH_BITWIDTH_MULTI_FACTOR
                    and code under that.
02/11/04    gs      Added macro SRCHCM_GSM_BSIC_VER_MIN_RSSI_DBMX16
01/26/04    gs      Added macros to support step 1 result expiry
12/16/03    gs      Support for GSM Quadband
10/24/03    gs      Updated functionality for compressed mode inter-freq
                    search under FEATURE_CM_FOR_INTER_FREQUENCY_MEASUREMENTS
10/03/03    gs      Added the structure member result_fetch_timeout_unit_wframe
                    in RSSI result.
                    Externalized function srchcm_meas_gsm_meas_gap_unusable
08/15/03    gs      Replaced macro SRCHCM_GSM_PWR_SCAN_PROC_FR_TIME with
                    SRCHCM_GSM_PWR_SCAN_RESULT_READ_FR_DELAY
                    Added macro SRCHCM_GSM_ACQ_CLEANUP_MIN_FR_DELAY
                    Added lna_state in srchcm_gsm_rssi_meas_result_struct_type
                    Added acq_cleanup_req_sent and cleanup_req_frame_num
                    in structure srchcm_gsm_fcch_detect_meas_result_struct_type
02/14/03    scm     Make srchcm_cleanup_gsm_pending_meas() return boolean.
01/30/03    gs      Added strcuture members to result info struct to keep
                    track of number of commands sent to mDSP to help GSM RF
                    driver windup.
                    Correceted function prototype as per definition in
                    srchcmmeas.c and externalized the new functions added to
                    srchcmmeas.c
01/16/03    gs      Changed macro values for SRCHCM_GSM_NUM_WORD16_SCH_RX_HDR_MDSP
                    and SRCHCM_GSM_GL1_HW_MDSP_RX_HDR_DIFF from hard coded to
                    size of actual structures defined.
11/14/02    gs      Changed prototype of function srchcm_meas_update_rssi_meas_prd
                    to pass RSSI TGMP active value.
10/21/02    gs      Changed the initial RSSI of the cell to -70 dBm
                    Added structure member burst_matric_valid in SCH decode
                    result info
10/17/02    gs      Added GSM meas state for deleted cell.
10/07/02    gs      Added the macro SRCH_GSM_SCH_DEC_BURST_MATRIX_OFFSET_CMD_FRAME
                    for SCH burst read offset from SCH decode command.
                    Added structure member to SCH decode burst matric.
09/27/02    gs      Changed the rssi_db to rssi_dbmx16
                    Externalized the variables for use in CM meas reporting
                    module to access the measured values.
09/19/02    gs      Externalized the function to update the BSIC Id abort count
08/22/02    djm     ifdef include when FEATURE_CM_MEASUREMENT specified
08/22/02    djm     srch_gsm_cell_struct_type changed to srchcm_gsm_cell_struct_type
                    to resolve conflict with srchset.h defn'
08/09/02    gs      Created this file
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "stdlib.h"
#include "wl1rfmif.h"
#include "dlinklist.h"
#include "srchcmdrv.h"
#include "srchgsmdrv.h"
#include "srchcmmeasparms.h"
#include "mcalwcdma_cm.h"

#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_CM_LTE_SEARCH)
#include "wsrchltedrv.h"
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Create SRCHCMMEAS critical section */
extern rex_crit_sect_type srchcmmeas_crit_sect;
#define SRCHCMMEAS_INTLOCK()  REX_ISR_LOCK(&srchcmmeas_crit_sect)
#define SRCHCMMEAS_INTFREE()  REX_ISR_UNLOCK(&srchcmmeas_crit_sect)

#define SRCHCM_SET_IDX_INVALID 0xFF

/* Frame increment for inter-freq timers for non-DRX state */
#define SRCHCM_FRAME_INC 1

/* Macros for time stamp diff */
#define SRCHCM_MAX_TIME_COUNT_VAL 0xFFFF
#define SRCHCM_TIME_DIFF(this_ts, ref_ts) \
  ((ref_ts) < (this_ts)) ? \
  (((ref_ts) + SRCHCM_MAX_TIME_COUNT_VAL) - (this_ts)) : \
  ((ref_ts) - (this_ts))

/* WCDMA Inter frequency measurement related macro */
/* ----------------------------------------------- */
#define SRCHCM_SET_INTERF_MAX 2
#define SRCHCM_SET_INTERF_CELL_MAX 32
/* step 1 expiry timer (in units of WCDMA frames) */
#define SRCHCM_INTERF_STEP1_EXPIRY_TIMER 1500
/* step 1 expiry timer (in units of WCDMA frames) when multiple frequencies are present */
#define SRCHCM_INTERF_STEP1_MULT_FREQ_EXPIRY_TIMER 500
/* if the number of inter-f PSC UE needs to find is larger than
   SRCHCM_INTERF_STEP1_PEAKS_REMAINING_THRESH, stop list
   search for each PSC on the next step1 peak unless it is above
   SRCHCM_INTERF_STEP1_EXPIRY_EGY_THRESH. */
/*Step-1 256x30, No RxD,  Db value -16.5, linear value 213*/
#define SRCHCM_INTERF_STEP1_EXPIRY_EGY_THRESH 213
/* step 1 peaks remaining threshold 
   if the number of remaining step1 peaks is less than this threshold, 
   try to finish list searches on all step1 peaks found in this freq. */
#define SRCHCM_INTERF_STEP1_PEAKS_REMAINING_THRESH 3

#define SRCHCM_INTERF_VERIFY_MAX_AGE 0xFFFE
/* This is the age set for cell that is set to inital detect */
#define SRCHCM_INTERF_VERIFY_AGE_AT_INITIAL_DETECT 0xFFFF

/* number of list search peaks to be stored for cell */
#define SRCHCM_NUM_LIST_SRCH_PEAKS 4

/* value for side lobe detection */
#define SRCHCM_INTERF_MIN_DIST_CX8_FOR_SIDELOBE 10
#define SRCHCM_INTERF_MAX_DIST_CX8_FOR_SIDELOBE 14
#define SRCHCM_INTERF_SIDELOBE_RATIO_LINEAR_UNIT 10 /* this corresponds to 10 dB */
/* the get smaller side of magnitude of difference */
#define SRCHCM_INTERF_PN_POS_CX8_DIFF(x, y)  \
   ((labs((int32)((int32)(x) - (int32)(y))) < CHIPX4_PER_FRAME) ? \
    labs((int32)((int32)x - (int32)y)) : \
    (CHIPX8_PER_FRAME - labs((int32)((int32)x - (int32)y))) \
   )

/* Number of consecutive reconfirm failure before cell is moved to undetected state */
#define SRCHCM_INTERF_MAX_RECONF_FAILURE 10

/* link list node for macro use */
extern dll_node_struct_type *srchcm_list_macro_use;

#define SRCHCM_INTERF_GET_CUR_CELL(dll_ptr) \
  ((dll_ptr != NULL) ? (((srchcm_list_macro_use = dll_get_cur_node(dll_ptr)) != NULL) ? \
   ((srchcm_interf_cell_struct_type*) srchcm_list_macro_use->data_ptr) : NULL) : NULL)
#define SRCHCM_INTERF_GET_NEXT_CELL(dll_ptr, seek) \
  ((dll_ptr != NULL) ? (((srchcm_list_macro_use = dll_get_next_node(dll_ptr, seek)) != NULL) ? \
   ((srchcm_interf_cell_struct_type*) srchcm_list_macro_use->data_ptr) : NULL) : NULL)
#define SRCHCM_INTERF_GET_PREV_CELL(dll_ptr, seek) \
  ((dll_ptr != NULL) ? (((srchcm_list_macro_use = dll_get_prev_node(dll_ptr, seek)) != NULL) ? \
   ((srchcm_interf_cell_struct_type*) srchcm_list_macro_use->data_ptr) : NULL) : NULL)
   
/* Maro for host use to extract information given cell pointer */
/* ----------------------------------------------------------- */
/* Cell frequency */
#define SRCHCM_INTERF_CELL_FREQ(cell_ptr) \
  ((cell_ptr)->freq)
  /* Cell scrambling code */
#define SRCHCM_INTERF_CELL_SCR_CODE(cell_ptr) \
  ((cell_ptr)->scr_code)

/* Cell last measurement gap w.r.t. to its previous measurement */
#define SRCHCM_INTERF_CELL_LAST_MEAS_GAP(cell_ptr) \
  ((cell_ptr)->last_meas_gap)
#define SRCHCM_INTERF_CELL_FIRST_MEAS(cell_ptr) \
  (SRCHCM_INTERF_CELL_LAST_MEAS_GAP(cell_ptr) == \
  SRCHCM_INTERF_VERIFY_AGE_AT_INITIAL_DETECT)

  /* cell total energy of all paths above threshold */
#define SRCHCM_INTERF_CELL_EGY(cell_ptr) \
  ((cell_ptr)->total_egy)
/* PN position of strongest peak (first) */
#define SRCHCM_INTERF_CELL_PN_POS_CX8(cell_ptr) \
  ((cell_ptr)->pn_pos_cx8)
/* RSSI in dBmx12 measured in gap when this cell was searched */
#define SRCHCM_INTERF_CELL_RSSI_DBMX10_AT_SEARCH(cell_ptr) \
  ((cell_ptr)->rssi_dbmx10)
/* indicate cell state is detected */
#define SRCHCM_INTERF_CELL_STATE_IS_DETECTED(cell_ptr) \
  ((cell_ptr)->detection_state == SRCHCM_INTERF_CELL_STATE_DETECTED)
/* freq info index of this cell */
#define SRCHCM_INTERF_CELL_FREQ_INFO_IDX(cell_ptr) \
  ((cell_ptr)->interf_info->info_idx)
/* All cell in this freq has been reconfirmed at least once */  
#define SRCHCM_INTERF_CELL_FREQ_ALL_CELLS_RECONF(cell_ptr) \
  ((cell_ptr)->interf_info->last_cell_in_reconf_list_hit)
/* Is this cell the last cell to be reconfirmed */  
#define SRCHCM_INTERF_CELL_IS_LAST_IN_RECONF_LIST(cell_ptr) \
  ((cell_ptr) == (cell_ptr)->interf_info->last_marker_cell)
/* This indicate that last cell marker was in list waiting host update
   but got pushed out of the list bacause either all cells before the
   last cell marker went undetected or were deleted */
#define SRCHCM_INTERF_CELL_FREQ_EVAL_RPT_IMMEDIATE(cell_ptr) \
  ((cell_ptr)->interf_info->eval_rpt_before_proc_waiting_host_update_list)
#define SRCHCM_INTERF_CELL_FREQ_ALL_RECONF_PERIOD(cell_ptr) \
  ((cell_ptr)->interf_info->last_reconf_period_all_cells_w_frame)
  
/* indicate that freq info at index is valid */
#define SRCHCM_INTERF_FREQ_IDX_VALID(freq_idx) \
  (srchcm_interf_info[freq_idx].valid)
/* same as SRCHCM_INTERF_CELL_FREQ_EVAL_RPT_IMMEDIATE bit with freq index */
#define SRCHCM_INTERF_FREQ_EVAL_RPT_IMMEDIATE(freq_idx) \
  (srchcm_interf_info[freq_idx].eval_rpt_before_proc_waiting_host_update_list)
#define SRCHCM_INTERF_FREQ_ALL_RECONF_PERIOD(freq_idx) \
  (srchcm_interf_info[freq_idx].last_reconf_period_all_cells_w_frame)
/* freq of freq index info */
#define SRCHCM_INTERF_FREQ_AT_INFO_IDX(freq_idx) \
  (srchcm_interf_info[freq_idx].freq)

#define SRCHCM_INTER_FREQ_STEP1_NEEDS_REPORTING_CHECK(freq_idx) \
  (srchcm_interf_info[freq_idx].step1_needs_reporting_check)
  
/* Inter RAT measurement related macro */
/* ----------------------------------- */

/* Max inter-RAT cells */
#define SRCH_SET_INTER_RAT_CELL_MAX 32

  /* GSM CM measurement related macro */
  /* -------------------------------- */
/* Max GSM cells */
#define SRCH_SET_GSM_CELL_MAX SRCH_SET_INTER_RAT_CELL_MAX

/* Max BSIC Id and verify cells */
#define SRCHCM_SET_GSM_CELL_BSIC_IDENTIFY_MAX 8
#define SRCHCM_SET_GSM_CELL_BSIC_VERIFY_MAX   8

#define SRCHCM_GSM_NUM_MAX_FORBIDDEN_OFFSET 3

  /* GSM RSSI scan related macros */

/* number of frame required to do power meas and process in frame */
#define SRCHCM_GSM_PWR_SCAN_RESULT_READ_FR_DELAY     3
/* maximum number of carrier that can be measured in max TGL 14 */
#define SRCHCM_SET_GSM_RSSI_MAX_CARRIER 15
/* Number of power scan command possible per GAP */
#define SRCHCM_GSM_PWR_SCAN_CMD_MAX_PER_GAP 3
/* Min GSM RSSI samples required */
#define SRCHCM_GSM_MIN_PWR_SCAN_SAMPLES 1
/* Max power scan in one measurement period */
#define SRCHCM_GSM_MAX_PWR_SCAN_PER_MEAS_PRD 20
/* measurement period in WCDMA frame units */
#define SRCHCM_GSM_MEAS_PRD_UNIT_WFRAME 48 /* 480 ms */
/* initial RSSI value for LNA gain select */
#define SRCHCM_GSM_INIT_RSSI_VAL (-70 * 16)
/* RSSI must be above this level for cell to be in BSIC verification list */
#define SRCHCM_GSM_BSIC_VER_MIN_RSSI_DBMX16 SRCHGSM_MIN_ACCEPTABLE_RX_LVL_dBm_x16
/* Timeout and max timer values for RSSI scans */
#define SRCHCM_GSM_MAX_RSSI_MEAS_PRD_W_FR 1000 /* 10 sec */
#define SRCHCM_GSM_RSSI_RESULT_FETCH_TIMEOUT_W_FR 50 /* .5 sec */

/* Maximum consecutive RSSI gaps that can be used for BSIC ID */
#define SRCHCM_GSM_MAX_RSSI_GAPS_FOR_BSIC_ID 15 

/* max time allowed to ignore processing of rssi results */
#define SRCHCM_GSM_RSSI_MEAS_REPORTED_TIMER 16 /* 160 ms */

#define SRCHCM_GSM_STARTUP_CLEANUP_CMDS 2

/* Number of SCH command possible per W2G CM GAP */
#define SRCHCM_GSM_ASYNC_SCH_CMD_MAX_PER_GAP 1

#define SRCHCM_GSM_SEQ_MAX_CMD SRCHCM_GSM_PWR_SCAN_CMD_MAX_PER_GAP \
                               + SRCHCM_GSM_STARTUP_CLEANUP_CMDS \
                               + SRCHCM_GSM_ASYNC_SCH_CMD_MAX_PER_GAP

  /* BSIC Id related macros */

/* this the maximum number of cell for which BSIC Id meas info 
   is created in update function.
   One is the minimum required. Second is if the first one is not
   valid for BSIC Id (deleted/identified) */
#define SRCHCM_GSM_MAX_BSIC_ID_MEAS_UPDATE_INFO 1
#define SRCHCM_GSM_MAX_BSIC_RECONF_MEAS_UPDATE_INFO 1
/* number of frame after ACQ start cmd frame, cleanup can be sent */
#define SRCHCM_GSM_ACQ_CLEANUP_MIN_FR_DELAY     3

#define SRCHCM_GSM_BSIC_ID_FORBID_AFTER_F_RECONF_ABOR_TIMER 100 /* in unit W frames 1 second */

/* bsic_ver_meas_age absolute max value */
#define SRCHCM_GSM_BSIC_VER_MEAS_AGE_MAX 0xFFFF

/* Threshold to unlock current BSIC ID cell (10dB)*/
#define SRCHCM_GSM_BSIC_VER_MIN_POWER_TO_UNLOCK (10 * 16)
/* Min power of candidate cell to fit in the unlock category (-100dB)*/
#define SRCHCM_GSM_BSIC_VER_UNLOCK_THRESHOLD (-100 * 16)

/* SCH decode related macros */
/* number of time the BSIC is allowed to be decoded in a GAP
   eligible to decode the SCH in cell reconfirmation state */
#define SRCHCM_GSM_MAX_BSIC_DEC_ATTEMPTS 2
/* Number of maximum SCH decode result.
   2 SCH decode can be programmed back to back for BSIC Id.
   1 extra that can possibly come immediate after these for BSIC re-conf */
#define SRCHCM_GSM_MAX_SCH_RESULT 3
/* Number of frames needed to produce SCH decode burst matrix */
#define SRCH_GSM_SCH_DEC_BURST_MATRIX_OFFSET_CMD_FRAME 3
/* Number of frames required for SCH decode result processing after
   the end of burst */
#define SRCH_GSM_SCH_DECODE_TIME_UNIT_FRAME 3

/* SCH mDSP header length */
#define SRCHCM_GSM_NUM_WORD16_SCH_RX_HDR_MDSP  MDSP_RX_HDR_SIZE
/* SCH decoded data length in mDSP */
#define SRCHCM_GSM_SCH_PAYLOAD_SIZE_MDSP       2
/* SCH decode result buffer.
   5 header word16 + 2 word16 for SCH payload 25 bits */
#define SRCHCM_GSM_NUM_WORD16_SCH_RX_DATA_MDSP \
  (SRCHCM_GSM_NUM_WORD16_SCH_RX_HDR_MDSP + SRCHCM_GSM_SCH_PAYLOAD_SIZE_MDSP)
/* Difference between the mDSP and GL1 HW header size */
#define SRCHCM_GSM_GL1_HW_MDSP_RX_HDR_DIFF     ((sizeof(gl1_defs_rx_hdr_struct)/sizeof(uint16)) - SRCHCM_GSM_NUM_WORD16_SCH_RX_HDR_MDSP)
/* Total SCH reault buffer size */
#define SRCHCM_GSM_NUM_SCH_RESULT_BUF_WORD16 \
  (SRCHCM_GSM_NUM_WORD16_SCH_RX_DATA_MDSP + SRCHCM_GSM_GL1_HW_MDSP_RX_HDR_DIFF)
/* SCH decoded data offset from the */
#define SRCHCM_GSM_SCH_PAYLOAD_OFFSET \
  (SRCHCM_GSM_NUM_WORD16_SCH_RX_HDR_MDSP + SRCHCM_GSM_GL1_HW_MDSP_RX_HDR_DIFF)
  
#define SRCHCM_GSM_SCH_GET_BSIC(word16_1, word16_2) \
  ((word16_1 >> 2) & 0x3F)
#define SRCHCM_GSM_SCH_GET_RFN_T1(word16_1, word16_2) \
  (((word16_2 & 0x80) >> 7) | \
   ((word16_1 & 0xFF00) >> (8-1)) | \
   ((word16_1 & 0x3) << 9))
#define SRCHCM_GSM_SCH_GET_RFN_T2(word16_1, word16_2) \
  ((word16_2 >> 2) & 0x1F)
#define SRCHCM_GSM_SCH_GET_RFN_T3_PRIME(word16_1, word16_2) \
  (((word16_2 & 0x100) >> 8) | \
   ((word16_2 & 0x3) << 1))
#define SRCHCM_GSM_SCH_RFN_T3_PRIME_TO_T3(rfn_t3_prime) \
  ((rfn_t3_prime * 10) + 1)
#define SRCHCM_GSM_SCH_RFN_TO_HYPER_FRAME_NUMBER(rfn_t1, rfn_t2, rfn_t3) \
  ((rfn_t1 * 26 * 51)  + (((26 + rfn_t3 - rfn_t2) % 26) * 51) + rfn_t3)
  
  /* GSM CM meas info update related macros */
  
/* invalid GSM meas info related index */
#define SRCHCM_GSM_MEAS_INFO_IDX_INVALID 0xFF
/* This is the number of frame offset in which the compressed
   mode GAP is there. 0 means that gap is in next WCDMA frame
   1 means GAP is after 1 frame after the next frame */
#define SRCHCM_GSM_ADV_INFO_COUNT 1
/* GSM advance info buffer count.
   Keep 1 extra buffer as the GSM mDSP command buffer pointers
   are held in this in the measurement info structure */
#define SRCHCM_GSM_ADV_INFO_BUF_COUNT (SRCHCM_GSM_ADV_INFO_COUNT + 1)

#define SRCHCM_GSM_OLD_RECONF_STRONG_ID_RSSI_DELTA_DBMX16 (3 * 16)

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Inter-frequency measurement related type definition */
/* --------------------------------------------------- */

/* Inter frequency cell state type */
typedef enum
{
  SRCH_SET_INTERF_UNUSED,
  SRCH_SET_INTERF_USED,
} srch_set_interf_cell_state_enum_type;

/* This enum defines the CM search state for a given inter frequency */
typedef enum
{
  SRCHCM_INTERF_STATE_SRCH_IDLE,
  SRCHCM_INTERF_STATE_READY_TO_START_STEP1_SRCH,
  SRCHCM_INTERF_STATE_STEP1_SRCH,
  SRCHCM_INTERF_STATE_STEP1_SRCH_SUSPENDED,
  SRCHCM_INTERF_STATE_STEP1_SRCH_DONE,
  SRCHCM_INTERF_STATE_TRIGGERED_PSC_ACQ_LIST_SRCH,
  SRCHCM_INTERF_STATE_READY_TO_START_RECONF_LIST_SRCH,
  SRCHCM_INTERF_STATE_RECONF_LIST_SRCH,
  SRCHCM_INTERF_STATE_LIST_SRCH_NN_COMPLETE,
  SRCHCM_INTERF_STATE_SRCH_DONE
} srchcm_srch_state_enum_type;

/* Inter frequency search doability enum definition */
typedef enum
{
  SRCHCM_INTERF_SRCH_NOT_POSSIBLE,
  SRCHCM_INTERF_SRCH_POSSIBLE
} srchcm_interf_srch_doability_enum_type;

/* Interfreq cell states */
typedef enum
{
  SRCHCM_INTERF_CELL_STATE_UNDETECTED,
  SRCHCM_INTERF_CELL_STATE_INITIAL_DETECT,
  SRCHCM_INTERF_CELL_STATE_DETECTED
} srchcm_interf_cell_detect_state_enum_type;

/* inter freq compressed mode interface search modes */
typedef enum
{
  SRCHCM_INTERF_MODE_FACH,
  SRCHCM_INTERF_MODE_DCH
} srchcm_interf_mode_enum_type;

/* Type of inter-freq search */
typedef enum
{
  SRCHCM_INTERF_SRCH_TYPE_NONE,
  SRCHCM_INTERF_SRCH_TYPE_STEP1,
  SRCHCM_INTERF_SRCH_TYPE_PSC_ACQ_LIST,
  SRCHCM_INTERF_SRCH_TYPE_RECONF_LIST
} srchcm_interf_search_type_enum_type;

/* Defines a "working unit" to be used in the "working set". A working unit is
   simply information on the 15 hypotheses that are undergoing searches now or 
   in the near future for a specific [step1-peak, psc] pair. */
typedef struct
{
  /* used for linking in a linked list */
  dll_node_struct_type ll_node;

  /* the step1 peak index that this unit searches */
  uint8 step1_peak_idx;
  /* the psc that this unit searches */
  struct srchcm_interf_cell_struct *psc_p;

  /* the tasks in this unit for which searches have been completed */
  uint16 completed_bfld;
  /* The tasks in this unit for which searches have been scheduled in FW and 
     results are pending. Note we store this information on a per-queue (gap) 
     basis */
  uint16 requested_bfld[MCALWCDMA_SRCH_CM_MAX_NUM_CMQ];

  /* These 2 fields keep track of the best peak found in the results back from 
     this working unit (needed because results can come back across multiple 
     gaps) */
  /* psc acq best peak energy */
  uint16 best_peak_egy;
  /* psc acq best peak PN position */
  uint32 best_peak_pn_pos_cx8;

} srchcm_interf_psc_acq_working_unit_type;

#define SRCHCM_INTERF_NUM_WORKING_UNITS 9
/* Structure to store the current state of psc acq search for a single frequency */
typedef struct
{
  /* determines if the step1_peak_idx_of_pending_working_unit and 
     undet_psc_node_of_pending_working_unit fields are valid */
  boolean pending_fields_valid;

  /* These mark the next working unit that will be pulled into the working set 
     as units get completed through the many hypothesis searches. */
  uint8                 step1_peak_idx_of_pending_working_unit;
  dll_node_struct_type *undet_psc_node_of_pending_working_unit;

  /* -------------------------------------------------------------------------- */
  /* these fields are used to describe the "working set" (of hypothesis 
     "working units"), which describes a subset of all the hypotheses that will
     be searched for PSC acq on a frequency. The purpose of this subset is to 
     store more detailed information for the psc acq hypotheses that are about 
     to be searched or are currently being searched. The units in the set will 
     form an queue-like structure that helps with scheduling and results 
     processing. */

  /* linked list of working units that are in the working set */
  dll_struct_type used_working_units;
  /* linked list of unused working units. Data in working units in this list is
     considered invalid */
  dll_struct_type unused_working_units;

  /* static allocation of our working units. These units will usually be 
     accessed from the above linked lists. */
  srchcm_interf_psc_acq_working_unit_type working_units[SRCHCM_INTERF_NUM_WORKING_UNITS];

} srchcm_interf_psc_acq_state_type;

/* stores info specific to undetected cell search */
typedef struct
{
  /* The frequency that is currently targeted for searches.*/
  uint8 freq_idx;

  /* Indicates how much time an undetected cell search can take before it's 
     time to give up and try again on another frequency (or start over on the
     same frequency if there is only one to search).  The age of an undetected 
     cell search can be determined by the step1 result age, which is set to 0
     when step1 search is started. */
  uint16 detection_timer_expiry_value;

  /* Indicates the number of cells in the undetected cell list that have been 
     detected once in psc acq searches and need initial-detect reconf searches
     to confirm them. Also used like a boolean flag.  If it's greater than zero
     the scheduler won't schedule more detection (step1/psc acq) searches */
  uint8 num_init_detect_verif_cells;

  /* all info related to current psc_acq search state */
  srchcm_interf_psc_acq_state_type psc_acq;
  
} srchcm_interf_undet_cell_srch_state_type;

/* declare our cell struct here so we can point to it in structure definitions
   and define it later */
struct srchcm_interf_cell_struct;

/* stores state related to currently active step1 search results */
typedef struct
{
  /* amount of time since step 1 started being active 
     (set to 0 when search started and incremented every frame thereafter) */
  uint32 active_age;

  /* step 1 for this frequency done or not */
  boolean result_valid;

  /* step 1 result for this frequency */
  srchcm_step1_srch_result_struct_type result;

} srchcm_interf_step1_state_type;

typedef struct
{
  boolean interf_drv_init;
  boolean gsm_drv_init;
  boolean lte_drv_init;
} srchcm_irat_drv_init_status_struct_type;

/* This structure defines the inter frequency info */
typedef struct
{
  /* indicate if this frequency is active in the interf cell list */
  boolean valid;

  /* indicate if freq can be searched, set to TRUE by default in CM */
  boolean srch_allowed;

  /* interf info index in array */
  uint8 info_idx;

  /* the arfcn of this WCDMA frequency*/
  uint16  freq;

  /* number of cells in the interf cell list belonging to this frequency */
  int8 num_cells;

  dll_struct_type undetected_cell_list;
  dll_struct_type cell_list_waiting_host_update;
  dll_struct_type detected_cell_list;

  /* information about the progress of step1 search for this freq */
  srchcm_interf_step1_state_type step1_state;

  /* this is the last cell in detected cell list after which
     event evaluation can be signalled to host */
  void *last_marker_cell;
  /* indicate last cell in reconf list has hit */
  boolean last_cell_in_reconf_list_hit;
  /* indicate that host should do eval/rpt before processing waiting host update list */
  boolean eval_rpt_before_proc_waiting_host_update_list;
  /* time taken for last reconf iteration for all cells */
  uint32 last_reconf_period_all_cells_w_frame;
  uint32 cur_reconf_period_all_cells_w_frame;
  uint16 reconf_period_threshold;

  /* TRUE if the frequency has been searched at least once since cells/freq has
     been added/modified */
  boolean searched_at_least_once;

  /* this boolean is used for indicating to meas reporting module that
     this freq is done with all step 1 all undetected cells should be reported */
  boolean step1_needs_reporting_check;

  /* If TRUE, the next search cycle on this freq should use the stored step1 
     state rather than performing a new step1 search. Should only be set TRUE
     if step1_result_valid is TRUE*/
  boolean use_saved_step1_state;

  /* latest RSSI measured value for this freq */
  int16 latest_rssi_dbmx10;

  /* Reconf Threshold for non TD Cells */
  uint8 notd_reconf_threshold_idx;

  /* Reconf Threshold for STTD Cells */
  uint8 sttd_reconf_threshold_idx;
} srchcm_interf_info_struct_type;

/* inter frequency cell structure definition */
typedef struct srchcm_interf_cell_struct
{
  /* link list node */
  dll_node_struct_type ll_node;

  /* cell index in reconf list. */
  uint8 index;

  /* cell state */
  srch_set_interf_cell_state_enum_type cell_state;

  boolean  srch_result_valid[MAX_NUM_ANTENNA];

  /* host info pointer (used by higher layers to store state) */
  void* host_info_ptr;

  /* WCDMA Cell Scrambling Code */
  uint16  scr_code;
  /* WCDMA frequency assignment for the cell */
  uint16  freq;
  /* STTD enable or not */
  boolean sttd;

  /* cell detection state. */
  srchcm_interf_cell_detect_state_enum_type detection_state;
  /* how long it's been since the cell has been reconfirmed (in w frames) */
  uint16 verf_age;
    /* this variable describes how long the cell went without search 
       (the verf_age) prior to the most recent reconfirmation search results */
  uint16 last_meas_gap;
  /* Number of consecutive failure after cell was detected */
  uint8 num_reconf_failure;
  /* indicate if this cell is sync cell type, that is its position
     was provided when added */
  boolean cell_sync_type;

  /* PN search peak result -Primary ant peaks */
  uint8 num_peaks;
  /* PN search peak result -diversity ant peaks*/
  uint8 num_div_peaks;
  srchcm_pn_srch_peak_struct_type peaks[SRCHCM_NUM_PN_PEAKS_PER_TASK];
  srchcm_pn_srch_peak_struct_type peaks_div[SRCHCM_NUM_PN_PEAKS_PER_TASK];

  /* PN position for this cell */
  uint32 pn_pos_cx8;
  /* total energy of all path above threshold of this cell */
  uint32 total_egy;
  /* RSSI value after list search */
  int16 rssi_dbmx10;

  /* This field is for sync-cells only.  It stores the originally given 
     cell position when we replace the above "pn_pos_cx8" field with a found 
     position. */
  uint32 save_pre_inital_detect_pn_pos_cx8;

  /* Stores what the cell was ahead of this cell in the undetected cell list at
     the time of it's removal and insertion into the detected cell list (for 
     initial detection reconf search). This field is only valid when the cell's
     detection state is INITIAL_DETECT. */
  dll_node_struct_type *prev_node_to_initial_detect_node;

  /* used to store the step1 peak index that gave this cell it's initial 
     detection.  Only valid if detection state is INITIAL_DETECT */
  uint8 initial_detect_step1_idx;

  /* inter_frequency cell info. */
  srchcm_interf_info_struct_type *interf_info;
} srchcm_interf_cell_struct_type;

/* structure definition for inter frequency RF setup information */
typedef struct
{
  /* Number of RF source items, 1 for single band 2 for Dual Band */
  uint8 num_source_item;
  #ifdef FEATURE_WCDMA
  /* Source params for RF header struct */
  wl1_rf_source_params_struct_type source_params[WL1_RF_HEADER_DB_NUM_SOURCE];
  #endif
  /* Last triggered search Inter-freq neighbor freq info */
  uint16 interf_freq_info;
  /* Pointer to the RF buffer allocated for the most recently triggered search */
  mcalwcdma_cm_irat_rf_buf_ctrl_struct_type* rf_buf;
  /* pointer to RF buffer to be used for cleanup after search */
  void *rf_buf_cleanup_ptr[MCALWCDMA_SRCH_CM_MAX_NUM_CMQ];
} srchcm_interf_rf_set_info_struct_type;

#ifdef FEATURE_WCDMA_DC_HSUPA
/* structure definition for specifying step1 results from DC measurements */
typedef struct
{ 
  /* RSSI in dbmx10 */
  int16 rssi_dbx10;
  /* Total no. of step 1 peaks on DC frequency searched in DC mode */
  uint16 num_peaks;
  /* PSC Peaks array, provide the position and energy of all the peaks found in step 1*/
  srchcm_step1_srch_peak_struct_type peaks[SRCH_MAX_NUM_STEP1_PEAKS];
  /* CFN at which last step1 was scheduled  */
  uint8 last_step1_cfn;
} srchcm_interf_step1_info_struct_type;
#endif

typedef void SRCHCM_INTERF_SEARCH_DONE_CB_FUNC(void);

/* Inter RAT measurement related type definition */
/* --------------------------------------------- */

/* Inter RAT cell type enum definition */
typedef enum
{
  SRCH_INTER_RAT_CELL_UNUSED_TYPE,
  SRCH_INTER_RAT_CELL_GSM_TYPE,
  SRCH_INTER_RAT_CELL_IS95_TYPE,
  SRCH_INTER_RAT_CELL_IS2000_TYPE
} srch_inter_rat_cell_type_enum_type;

  /* Inter RAT GSM measurement related type definition */
  /* --------------------------------------------- */

/* GSM cell state */
typedef enum
{
  SRCH_SET_GSM_UNUSED,
  SRCH_SET_GSM_DELETED,
  SRCH_SET_GSM_USED
} srch_set_gsm_cell_state_enum_type;

/* GSM cell measurement state */
typedef enum
{
  SRCH_GSM_MEAS_NA,
  SRCH_GSM_MEAS_DELETED,
  SRCH_GSM_MEAS_RSSI,
  SRCH_GSM_BSIC_IDENTIFY_ABORTED,
  SRCH_GSM_BSIC_IDENTIFY,
  SRCH_GSM_BSIC_RECONF
} srch_gsm_srch_meas_state;

/* GSM search trigger result */
typedef enum
{
  SRCHCM_GSM_SRCH_TRIGGER_FAILED,
  SRCHCM_GSM_SRCH_TRM_RF_REQUEST_FAILED,
  SRCHCM_GSM_SRCH_TRIGGER_SUCCESS
} srch_gsm_search_trigger_enum_type;

/* GSM cell BSIC Id structure definition */
typedef struct
{
  /* Only lower 3 bits are valid */

  /* network colour code */
  uint8 ncc;
  /* base station colour code */
  uint8 bcc;
} srchcm_gsm_bsic_struct_type;

/* GSM cell for CM measurement info structure definition */
typedef struct
{
  /* This points to the inter-RAT cell strcuture pointer that
     include this cell info */
  void* cell_inter_rat_self_ptr;

  /* frequency channel number */
  W_ARFCN_T arfcn;

  /* Indicate if BSIC verification required or NOT */
  boolean bsic_verify_reqd;

  /* measurement state/result */
  /* ------------------------ */

  /* GSM cell state */
  srch_set_gsm_cell_state_enum_type cell_state;

  /* GSM search/meas state */
  srch_gsm_srch_meas_state meas_state;

  /* RSSI measured from power scan */
  boolean rssi_valid;
  int16 rssi_dbmx16;

  /* indicate that this cell is under BSIC Id */
  boolean bsic_id_active;

  /* timing information for FCCH aquisition */
  boolean timing_valid;
  uint8 frame_offset;
  uint16 qs_offset;

  uint32 hyper_frame_offset;

  /* this information is for a forbidden QS offset for this cell
     It is reset at entry in to BSIC Id state and also reset
     when coming back from BSIC Id aborted */
  uint8  num_forbidden_offset;
  uint8  forbidden_frame_offset[SRCHCM_GSM_NUM_MAX_FORBIDDEN_OFFSET];
  uint16 forbidden_qs_offset[SRCHCM_GSM_NUM_MAX_FORBIDDEN_OFFSET];

  /* Frequency error for this cell */
  int16  frequency_offset;

  /* timer for barring cell to get RSSI sorted and be involved in
     BSIC Id process after reconfirm abort. Desired position of
     forbidden cell is at end of list. All cell sort and move function
     looks at end of BSIC Id list and excludes cells that have this
     flag set. Also timer update function looks at same place to
     update timer and update timer and reset forbid_bsic_id flag */
  boolean forbid_bsic_id;
  uint16  forbid_bsic_id_timer;

  /* BSIC for the cell found in SCH decode */
  boolean bsic_valid;
  srchcm_gsm_bsic_struct_type bsic;
  /* indicate for how long the BSIC was last verified,
     in units of WCDMA frame */
  uint16 bsic_ver_meas_age;
  /* number of times BSIC is failed to be decoded */
  uint8 bsic_decode_failed_attempt;

  /* number of gaps used to BSIC identify */
  uint16 num_gaps_used_bsic_id;
} srchcm_gsm_cell_struct_type;

/* ARFCN RSSI sample information strcuture */
typedef struct
{
  /* indicate that this ARFCN info is valid */
  boolean valid;

  /* ARFCN value */
  W_ARFCN_T arfcn;

  /* RSSI sample head and tail. At start head = tail = 0 */
  uint8 sample_head;
  uint8 sample_tail;

  /* last measured RSSI value */
  int16 last_meas_rssi_dbmx16;
  /* indicate that RSSI has been measured at least once averaged over 3
     samples in a measurement period */
  boolean rssi_valid;

  /* RSSI samples for averaging */
  int16 rssi_dbmx16_samples[SRCHCM_GSM_MAX_PWR_SCAN_PER_MEAS_PRD];
  /* sample timestamp */
  uint16 sample_ts[SRCHCM_GSM_MAX_PWR_SCAN_PER_MEAS_PRD];
} srchcm_gsm_arfcn_info_struct_type;

/* GSM measurement info struct definition.
   This structure keeps the parameters for various type of measurement and
   the GSM command parameters. The meas info is the UNION of all types of
   possible measurement. The measurement type is indicated by GSM measurement
   type. */
typedef struct
{
  /* indicate if this measurement info is valid. The info is Invalid
     to start with. If gets updates with each GSM advance info update */
  boolean valid;

  /* indicate that this information is in use and shall not be over-written */
  boolean in_use;

  /* indicate the CFN at which this info is applicable */
  uint8 cfn;

  /* GSM measurement type. indicate the type of measurement info kept
     in the union below */
  srchcmdrv_gsm_meas_purpose_enum_type meas_purpose;

  /* indicate that gap is used with overridden or alternate purpose */
  boolean gap_purpose_is_oride;

  /* GSM GAP parameters. indicate GFN, QS offset etc. */
  schcm_cm_gap_param_gsm_struct_type gsm_gap_params;
  /* CM startup and cleaup command parameters. This is applicable to any type of
     GSM measurement */
  srchcm_gsm_startup_cmd_param_struct_type cm_startup_cmd_params;
  srchcm_gsm_cleanup_cmd_param_struct_type cm_cleanup_cmd_params;

  struct
  {
    /* RSSI scan info */
    /* ------------- */

    /* RSSI scan GSM measurement and command parameters */
    struct
    {
      /* RSSI scan measurement parameters */
      /* Max num of cells can be scheduled */
      uint8 num_max_cells;
      /* Real num of cells that we scheduled */
      uint8 num_cells_for_meas;
      srchcm_gsm_pwr_scan_params_struct_type pwr_scan_params;

      /* RSSI scan GSM command parameters */
      uint8 num_pwr_scan_cmd;
      srchcm_gsm_pwr_scan_cmd_param_struct_type cmd_params[SRCHCM_GSM_PWR_SCAN_CMD_MAX_PER_GAP];
    } pwr_meas;

    /* BSIC Id parameter */
    /* ----------------- */

    struct
    {
      /* trigger function set this value to save the cell index in the meas params array
         that is a candidate for this measurement */
      uint8 meas_cell_info_idx;
      /* number of cells in the candidate array */
      uint8 num_cell_info;
      struct
      {
        /* BSIC Id measurement parameters */
        srchcm_gsm_cell_struct_type *cell_ptr;
        srchcm_gsm_bsic_id_params_struct_type bsic_id_params;

        /* FCCH ACQ detect OR SCG decode command parameters */
        union
        {
          struct
          {
            srchcm_gsm_start_acq_cmd_param_struct_type acq_start;
            srchcm_gsm_stop_acq_cmd_param_struct_type  acq_stop;
          } acq_cmd;

          srchcm_gsm_sch_decode_cmd_param_struct_type sch_decode[SRCHCM_GSM_SCH_DEC_CMD_MAX_PER_GAP];
        } cmd_params;
      } bsic_id_info[SRCHCM_GSM_MAX_BSIC_ID_MEAS_UPDATE_INFO];
    } bsic_id;

    /* BSIC reconf parameter */
    /* --------------------- */

    struct
    {
      /* trigger function set this value to save the cell index in the meas params array
         that is a candidate for this measurement */
      uint8 meas_cell_info_idx;
      /* Number of cell in the candidate measurement parameter list */
      uint8 num_cell_info;
      struct
      {
        /* SCH decode measurement parameters */
        srchcm_gsm_cell_struct_type *cell_ptr;
        srchcm_gsm_bsic_reconf_params_struct_type bsic_reconf_params;

        /* SCH decode GSM command parameters */
        srchcm_gsm_sch_decode_cmd_param_struct_type cmd_params;
      } bsic_reconf_info[SRCHCM_SET_GSM_CELL_BSIC_VERIFY_MAX];
    } bsic_reconf;

  } meas_info;
} srchcm_gsm_meas_info_struct_type;

  /* GSM CM measurement result poll info structures */
  /* ---------------------------------------------- */

/* RSSI scan result poll info struct definition */
typedef struct
{
  /* Number of RSSI scan command in this GAP */
  uint8 num_cmds;
  /* Number of command result received */
  uint8 num_result_rece;
  /* number of commands sent to mDSP */
  uint8 num_cmds_sent;
  /* timeout after number after commands are scheduled */
  uint8 result_fetch_timeout_unit_wframe;

  /* Each command result info */
  struct
  {
    /* GFN when the result of this command is expected */
    uint8 result_frame_num;
    /* Number of GSM bursts in this command */
    uint8 num_bursts;
    /* GSM cell array indicies for each bursts */
    uint8 cell_idx[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    /* measured RSSI for each burst */
    int16 rssi_dbmx16[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    /* LNA state of each burst */
    uint8 lna_state[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    gl1_hw_rx_tags_type tags[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR];

    #ifdef FEATURE_DUAL_SIM
    /* as_id received from RRC. Passed to GSM L1 for DSDS */
    sys_modem_as_id_e_type as_id;
    #endif
  } cmd_result[SRCHCM_GSM_PWR_SCAN_CMD_MAX_PER_GAP];
} srchcm_gsm_rssi_meas_result_struct_type;

/* FCCH tone detect poll struct information */
typedef struct
{
  /* indicate that command for FCCH detect has been sent to mDSP */
  boolean acq_start_cmd_sent;
  boolean acq_stop_cmd_sent;
  boolean acq_cleanup_req_sent;

  /* cell pointer for which the FCCH ACQ is being done */
  srchcm_gsm_cell_struct_type *cell_ptr;
  /* ACQ start number. Used to establish initial frame offset */
  uint8 acq_start_frame;
  /* ACQ start window offset. Used to establish QS offset */
  uint16 acq_win_start_offset;

  /* GSN at which the result is expected */
  uint8 result_frame_num;
  /* Number of frames for which the FCCH ACQ result need to be polled.
     This is based on the ACQ windoe length */
  uint8 num_results;
  /* frame number in which to send cleanup */
  uint8 cleanup_req_frame_num;

  /* Number of frames for which result has been polled */
  uint8 num_result_polled;
  /* indicate if tone is detected in polled frames */
  boolean tone_is_detected;
  /* SNR of last strongest tone detected */
  uint16 tone_snr;
  /* Tone offset w.r.t. ACQ window for last strongest tone detected */
  int16 tone_offset;
  /* Frequency offset for the tone */
  int16 tone_freq_offset;
  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  sys_modem_as_id_e_type as_id;
  #endif
} srchcm_gsm_fcch_detect_meas_result_struct_type;

/* SCH decode poll info structure definition */
typedef struct
{
  /* indicate that command for SCH decode has been sent to mDSP */
  boolean cmd_sent;
  /* indicate this result was meant for bsic Id */
  boolean purpose_bsic_id;
  /* cell pointer for which the SCH decode is being done */
  srchcm_gsm_cell_struct_type *cell_ptr;
  /* Current timeline GFN at which SCH is being decoded. This is the frame
     burst at which SCH burst starts */
  uint8 decode_frame_num;
  /* Hyper frame number in current timeline at which SCH burst start */
  uint32 decode_hyper_frame_num;

  /* GFM at which to get the SCH decode burst matrix */
  uint8 burst_matric_frame_num;
  /* GFN at which result is expected */
  uint8 result_frame_num;

  /* SCH decode burst matric contents */
  boolean burst_matric_valid;
  gl1_defs_metrics_rpt burst_matric_rpt;

  /* SCH decode status. TRUE is burst is received and CRC passes */
  boolean sch_decode_status;
  /* SCH decode header and data buffer */
  uint16 buffer[SRCHCM_GSM_NUM_SCH_RESULT_BUF_WORD16];  
  /* GL1 HW abstraction to allow RF parameters to be passed back to GL1 HW driver
     This is transparent to WL1. */
  gl1_hw_rx_tags_type tags;
  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  sys_modem_as_id_e_type as_id;
  #endif
} srchcm_gsm_sch_decode_result_struct_type;

/* Inter RAT cell structure definiiton.
   Only GSM cell definition is included */
typedef struct srchcm_inter_rat_cell_struct_tag_type
{
  /* indicate the cell is in use of not */
  boolean cell_used;
  /* Cell type */
  srch_inter_rat_cell_type_enum_type cell_type;
  /* Host info pointer */
  void* host_info_ptr;

  union
  {
    /* cell self pointer. This MUST be the first cell in the
       cell structure definition */
    struct srchcm_inter_rat_cell_struct_tag_type* self_ptr;
    /* GSM calls */
    srchcm_gsm_cell_struct_type gsm;
  } cell;
}srchcm_inter_rat_cell_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* this variable indicate the inter freq search mode using compressed mode
   searh interface in mDSP. There are 2 modes. FACH and DCH.
   This is used for following purpose
   - Logging the mode dependent CFN/SFN etc information
   - Setting signal for processing all cell in a freq done in task context */
extern srchcm_interf_mode_enum_type srchcm_interf_mode;

extern boolean srchcm_interf_drv_initialized;

/* inter-frequency info */
extern srchcm_interf_info_struct_type srchcm_interf_info[SRCHCM_SET_INTERF_MAX];

/* this indicate that CM interfreq search is done */
extern boolean srchcm_interf_search_done_status;
/* this indicate that some freq has detected cells in host update list
   and has hit the last cell in current reconf list */
extern boolean srchcm_interf_search_last_cell_in_reconf_list_hit;

/* number of the GSM cells */
extern uint8 srch_set_gsm_cell_cnt;
/* All GSM cells participating in RSSI measurement */
extern srchcm_gsm_cell_struct_type *srch_gsm_cell_ptr[SRCH_SET_GSM_CELL_MAX];

/* current cell index that is under BSIC confirmation */
extern srchcm_gsm_cell_struct_type *srchcm_gsm_cell_bsic_verified;

/* RSSI measurement age in units of WDMA frame (10 ms) */
extern uint32 srchcm_gsm_rssi_meas_age;

/* this indicate that CM interRAT search is done */
extern boolean srchcm_gsm_rssi_meas_done_status;
extern boolean srchcm_gsm_bsic_verified_status;
extern boolean srchcm_meas_cell_bsic_ver_status_update_pending;

/* Prepare for sch window frame num in advance to use for scheduling RSSI bursts in SCH */
extern uint8 srchcm_gsm_prep_sch_decode_frame_for_rssi;
/* Prepare for sch window offset of the beginning from the frame start to use for scheduling 
   RSSI bursts in SCH*/
extern uint16 srchcm_gsm_prep_sch_decode_offset_qs_for_rssi;

/* Log the estimation num of RSSI meas cells based on the length of the GAP before/after SCH window */
extern uint8 srchcm_num_rssi_meas_before_sch;
extern uint8 srchcm_num_rssi_meas_after_sch; 

#ifdef FEATURE_WCDMA_DC_HSUPA
extern srchcm_interf_step1_info_struct_type srchcm_dc_step1_result;
#endif

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION SRCHCM_QUERY_INTERF_FMO_SRCH_TYPE

DESCRIPTION
  This function returns the search type of the inter frequency FMO. This info
  is used to update the fmo(0x4721) log packet
DEPENDENCIES
  
RETURN VALUE
  Returns FMO search type

SIDE EFFECTS
  None.
===========================================================================*/
extern srchcm_interf_search_type_enum_type srchcm_query_interf_fmo_srch_type(void);

/*===========================================================================
FUNCTION SRCHCM_QUERY_INTERF_FMO_FREQ

DESCRIPTION
  This function returns the Frequency tuned  for the inter frequency FMO. This info
  is used to update the fmo(0x4721) log packet
DEPENDENCIES
  
RETURN VALUE
  Returns FMO freq

SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 srchcm_query_interf_fmo_freq(void);


/*==========================================================================
 
 FUNCTION  srchcm_get_dirat_value
 
 DESCRIPTION  Gets the dirat time for CM/FACH measurements
 
 DEPENDENCIES  Unknown
 
 RETURN VALUE  None
 
 SIDE EFFECTS  None
 
 ===========================================================================*/
extern uint32 srchcm_get_dirat_value(uint8 num_tg_slots, uint8 tgsn);

/*==========================================================================
 
 FUNCTION  srchcm_interf_list_srch_log_packet_alloc
 
 DESCRIPTION  This function allocates the dynamic memory needed for the Inter-freq List Srch
              log packets and related logging structures
 
 DEPENDENCIES  Unknown
 
 RETURN VALUE  None
 
 SIDE EFFECTS  None
 
 ===========================================================================*/
/* Changes for Memory Reduction using LI->MemHeap */
extern void srchcm_interf_list_srch_log_packet_alloc(void);

/*==========================================================================
 
 FUNCTION  srchcm_interf_list_srch_log_packet_dealloc
 
 DESCRIPTION  This function de allocates the dynamic memory needed for the Inter-freq List Srch
              log packets and related logging structures
 
 DEPENDENCIES  Unknown
 
 RETURN VALUE  None
 
 SIDE EFFECTS  None
 
 ===========================================================================*/
extern void srchcm_interf_list_srch_log_packet_dealloc(void);
/*===========================================================================
FUNCTION srchcm_meas_init

DESCRIPTION
  This function initializes the CM search and measurement related database,
  variables and states. The following initializations are done:

  WCDMA Inter-Frequency:
  - None (Most of then are done in WCDMA search module as the inter-frequency
    cells shares the same structures as the intra-frequenct cell do.)

DEPENDENCIES
  This function must be called after the CM search drivers are initialized
  otherwise the result processing call back function will be over written.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_meas_init(void);
/*===========================================================================
FUNCTION srchcm_meas_reset_interf_step1_srch

DESCRIPTION
  This function reset interf step1 meas state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_meas_reset_interf_step1_srch(void);
/*===========================================================================
FUNCTION srchcm_update_time_counter

DESCRIPTION
  This function increments time counter

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_update_time_counter(void);

/*===========================================================================
FUNCTION srchcm_init_irat_cell_db

DESCRIPTION
  This function initializes the cell database for interf/GSM/LTE. This will be
  called at DPCH set up and L1 Idle init.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_init_irat_cell_db(void);

/*===========================================================================
FUNCTION srchcm_meas_interf_get_cells_to_reconf

DESCRIPTION
  This function gets the list of detected cells to reconfirm. All the cells
  in list shall belong to same frequency and there count will be less or
  equal to value set in global srchcm_interf_num_reconf_srch_cells_doable.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to cell list to reconfirm. This is the same pointer saved in global
  srchcm_interf_reconf_cell_list_to_trigger. NULL if there is no cell to
  reconfirm.

SIDE EFFECTS
  Following global varibales are updated
    srchcm_interf_reconf_cell_list_to_trigger
    srchcm_interf_num_reconf_srch_cells_doable
===========================================================================*/
extern dll_struct_type* srchcm_meas_interf_get_cells_to_reconf(void);

/*===========================================================================
FUNCTION srchcm_meas_get_next_inter_f_srch_type

DESCRIPTION
  This function returns to the caller what kind of search would be performed 
  right now given current srchcmmeas state.  Note that if mutex locks aren't 
  used, it's possible this state may change between the call to this function 
  and the next search event.

DEPENDENCIES
  None

RETURN VALUE
  Returns a search type (srchcm_interf_search_type_enum_type).  If no search
  can be performed (or when relentless_search is FALSE and no search needs to 
  be performed), the function will return SRCHCM_INTERF_SRCH_TYPE_NONE.

SIDE EFFECTS
  None
===========================================================================*/

extern srchcm_interf_search_type_enum_type srchcm_meas_get_next_inter_f_srch_type(
  /* When FALSE, detect searches won't schedule if all frequencies have 
     already been fully searched recently.  TRUE means we won't consider how 
     recently they've been searched. */
  boolean relentless_detect,
  /* guarenteed minimum number of slots in the GAP */
  uint8 min_num_tg_slots);

/*===========================================================================
FUNCTION srchcm_meas_trigger_inter_f_srch

DESCRIPTION
  This function is called by L1 compressed mode manager to trigger interf
  search, it returns trigger status and if search can be triggered it updates
  current search state to perform search.

DEPENDENCIES
  None

RETURN VALUE
  Search doability status.

SIDE EFFECTS
  Updates following global variables
    srchcm_interf_step1_srch_state
    srchcm_interf_list_srch_state
===========================================================================*/

extern srchcm_interf_srch_doability_enum_type srchcm_meas_trigger_inter_f_srch(
  /* When FALSE, detect searches won't schedule if all frequencies have 
     already been fully searched recently.  TRUE means we won't consider this. */
  boolean relentless_detect,
  /* Number of slots in the GAP */
  uint8 num_tg_slots,
  /* Starting W slot number */
  uint8 tgsn,
  /* WTR index info */
  uint8 *wtr_index,
  /* Interf neighbor RxLM buffer index */
  uint32* nbr_rxlm_buf_index);

/*===========================================================================
FUNCTION srchcm_do_inter_f_search

DESCRIPTION
  This function is called by L1 compressed mode manager to do the interf
  search based on states and other global set by calling trigger function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates following global variables
    srchcm_interf_step1_srch_state
    srchcm_interf_list_srch_state
===========================================================================*/

extern void srchcm_do_inter_f_search(
  /* gap start CFN */
  uint8 gap_start_cfn,
  /* Number of slots in compressed mode gap */
  uint8 num_gap_slots,
  /* Inter-frequency neighbor RxLM buffer index */
  uint32 nbr_rxlm_buf_index);

/*===========================================================================
FUNCTION srchcm_update_freq_srch_allowed

DESCRIPTION
  This function is called to block the searching of specific frequencies by
  the measurement layer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates whether a frequency can be searched
===========================================================================*/
extern void srchcm_update_freq_srch_allowed(
  uint8 srch_allowed);

/*===========================================================================
FUNCTION srchcm_update_all_freq_cell_reconf_timer_expiry_vals

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
extern void srchcm_update_all_freq_cell_reconf_timer_expiry_vals(
  /* expiry value for interf PSC reconf searches */
  uint32 reconf_timer_val);

/*===========================================================================
FUNCTION srchcm_update_cell_detect_timer_expiry_val

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
extern void srchcm_update_cell_detect_timer_expiry_val(
  /* New inter-freq step1 timer value */
  uint32 detection_timer_val);

/*===========================================================================
FUNCTION srchcm_interf_set_cm_step1_age_expiry_val

DESCRIPTION
  This function is called when a CM pattern is being activated to ensure that
  the Step1 expiry timers are set to the default values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_interf_set_cm_step1_age_expiry_val(void);

/*===========================================================================
FUNCTION srchcm_eliminate_detected_psc_step1_peaks

DESCRIPTION
  This function eliminates the step 1 peaks that matches the PSC position
  corresponding the cells PN position with in range of few chips.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates the step 1 result step 1 peaks available for PSC ACQ list search.
===========================================================================*/

extern void srchcm_eliminate_detected_psc_step1_peaks(
/* pointer to cell link list */
dll_struct_type* cell_list_ptr,
/* the step1 results to check against the cell list */
  srchcm_step1_srch_result_struct_type *step1_results);

/*===========================================================================
FUNCTION srchcm_list_srch_part_done_handler

DESCRIPTION
  This function are call back function on partial list search done. Any code
  to debug the result at partial search can be put in these function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_list_srch_part_done_handler(
  /* q index in which result is received */
  uint8 q_idx);

/*===========================================================================
FUNCTION srchcm_step1_srch_done_handler

DESCRIPTION
  This function is a call back function for CM step 1 search done. This is
  set as a call back function by srchcm_do_inter_f_search funtion when the
  non coherent integration length is complete. This function sets the interf
  state to DONE so that update function can proceed to LIST search.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_step1_srch_done_handler(
  /* q index of list search done */
  uint8 q_index);

/*===========================================================================
FUNCTION srchcm_perform_sidelobe_detection

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchcm_perform_sidelobe_detection(
  /* Interf cell pointer */
  srchcm_interf_cell_struct_type* cell_ptr,
  /* Peak index */
  uint8 peak_idx,
  /* Antenna index */
  uint8 ant_index);

/*===========================================================================
FUNCTION srchcm_pn_srch_for_step1_peaks_done_handler

DESCRIPTION
  This function is a call back function for CM pn search done. This is
  set as a call back function by srchcm_do_inter_f_search funtion when the
  non coherent integration length of current step1 peak position hypothesis is done.
  
  It do the following actions:
  - Saves the list search peak results if the energy is above threshold
  - If all the hypothesis are done for the current step 1 peak and all the
    peaks are done then it sorts the peaks and call the inter_f search done
    function. The search state at this point is set to SRCHCM_INTERF_STATE_SRCH_DONE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void srchcm_pn_srch_for_step1_peaks_done_handler(
  /* q index of list search done */
  uint8 trans_id);

/*===========================================================================
FUNCTION srchcm_inter_f_psc_acq_list_srch_done

DESCRIPTION
  This function handles the part of PSC ACQ list search done when all the
  required result from various hypothesis have been collected. It moved cell to
  detected list and cell search state and verification age for immediate list
  search if it has found a peak. Also the PSC peaks is marked as cell exists.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Undetected cell list is impacted if cell has a peak.
  Cell info gets a update
===========================================================================*/

void srchcm_inter_f_psc_acq_pn_srch_done(
  srchcm_pn_srch_peak_struct_type *psc_acq_peak,
  uint8 psc_idx);

/*===========================================================================
FUNCTION srchcm_list_srch_reconf_done_handler

DESCRIPTION
  This function is a call back function for CM list search done. This is
  set as a call back function by srchcm_do_inter_f_search funtion when the
  non coherent integration length is done.
  
  It do the following actions:
  - Saves the list search peak results if the energy is above threshold

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_list_srch_reconf_done_handler(
  /* q index of list search done */
  uint8 q_index);

/*===========================================================================
FUNCTION srchcm_list_srch_egy_sort_comp_fn

DESCRIPTION
  Peak sort function for Qsort

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern int8 srchcm_list_srch_egy_sort_comp_fn(
  const void *p1,
  const void *p2);

/*===========================================================================
FUNCTION srchcm_set_int_f_srch_step1_result_buffer

DESCRIPTION
  This function sets the Step 1 result buffer pointer from which either the
  step 1 result will be written after step 1 search or will be used for
  list search.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_set_int_f_srch_step1_result_buffer(
  /* step1 result buffer pointer */
  srchcm_step1_srch_result_struct_type *step1_srch_result);

/*===========================================================================
FUNCTION srchcm_interf_abort_step1_srch

DESCRIPTION
  This function aborts step 1 search. Global srchcm_interf_step1_srch_state
  is set to IDLE and any call back function is cleared

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_interf_abort_step1_srch(uint8 idx);

/*===========================================================================
FUNCTION srchcm_interf_abort_psc_acq_list_srch

DESCRIPTION
  This function aborts PSC ACQ list search. It checks if list search is
  suspended or not. If suspended it clears this flag otherwise sets
  list search state to IDLE.
  Clears call back function registered at the list search Q index.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_interf_abort_psc_acq_list_srch(uint8 idx);

/*===========================================================================
FUNCTION srchcm_interf_abort_reconf_list_srch

DESCRIPTION
  This function aborts reconf list search. It restores cell in reconf list at
  CM search Q index to its inter-freq detected cell list. Reset list pointer
  at that Q index, reset cell state to USED, clears call back function for
  that Q.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_interf_abort_reconf_list_srch(uint8 index_q);

/*===========================================================================
FUNCTION srchcm_interf_increment_timers

DESCRIPTION
  This function is called peridically by L1 compressed mode manager in DCH and
  heartbeat handler in EDRX for incrementing the step1 and reconf active timers.
  This updates the step1 active timer and age of all detected cells in all frequencies.
  Cell verify age is maxed out at certain number so that it does not roll off.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_interf_increment_timers(uint16 frame_inc);

/*===========================================================================
FUNCTION srchcm_interf_update_rf_setup_info

DESCRIPTION
  This function gets RF setup data for mDSP inter freq operation from
  RF driver and updates to mDSP.

DEPENDENCIES
  None

RETURN VALUE
  RF setup info index (This is not the mDSP buffer index)

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 srchcm_interf_update_rf_setup_info(
  /* ARFCNs for which buffer needs update */
  uint16 arfcn0
  #ifdef FEATURE_WCDMA_DC_HSDPA
  , uint16 arfcn1
  #endif
  );

/*===========================================================================
FUNCTION srchcm_interf_get_cur_rf_setup_mdsp_buf_idx

DESCRIPTION
  This function returns the mDSP RF setup buffer index for current RF setup
  info.

DEPENDENCIES
  None

RETURN VALUE
  RF setup mDSP buffer index

SIDE EFFECTS
  None
===========================================================================*/

extern uint16 srchcm_interf_get_cur_rf_setup_mdsp_buf_idx(void);

/*===========================================================================
FUNCTION srchcm_meas_release_w2w_rf_buf

DESCRIPTION
  This function releases the mcalwcdma RF buffers used for W2W measurements

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_meas_release_w2w_rf_buf(
  /* Index for RF buffer cleanup ptr */
  uint8 cleanup_ptr_index);

/*===========================================================================
FUNCTION srchcm_interf_query_srch_in_progress

DESCRIPTION
  This function indicates whether interf searches are ongoing in fw in the near
  future.  

  Due to the way the srchcmmeas state machine works, if CM gaps with interf 
  searches are in CFNs X and Y, and this function is called at time Z:

  if Z < [X-1, slot 2]: result is valid for frames before X
  if [X-1, slot 2] < Z < [Y-1, slot 2]: result is valid for frames X+1 through Y-1.
  if [Y-1, slot 2] < Z: result is valid for frames after Y
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating whether configured searches are still in progress

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchcm_interf_query_srch_in_progress(void);

/* --------------------------------------------------------------------------- */
/* Funtions to be used by inter freq measurement control and reporting modules */
/* --------------------------------------------------------------------------- */

/*===========================================================================
FUNCTION srchcm_interf_get_next_cell_to_update

DESCRIPTION
  This function is for use by inter freq meas reporting code for getting the
  cell that are waiting in waiting for host update list.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to cell structure type srchcm_interf_cell_struct_type.
  NULL if no further cell is available.

SIDE EFFECTS
  None
===========================================================================*/

extern srchcm_interf_cell_struct_type* srchcm_interf_get_next_cell_to_update(void);

/*===========================================================================
FUNCTION srchcm_interf_release_cell_for_meas

DESCRIPTION
  This function is for use by inter freq meas reporting code for releasing
  cell for measurement again. Cell is put back to undetected or detected
  list based on cell srch state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_interf_release_cell_for_meas(
  /* cell pointer to be release for measurement after host update */
  srchcm_interf_cell_struct_type *cell_ptr);

/*===========================================================================
FUNCTION srchcm_interf_cleanup_interf_meas

DESCRIPTION
  This function cleanup any ongoing inter-freq search. This function is
  intended to be called in task context
  - If step1 search is in progress, it is aborted.
  - If PSC ACQ list search in progress it is aborted
  - If Reconf list search is going then post reconf search call back function
    is set and cleanup done status is marked FALSE.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on cleanup done status

SIDE EFFECTS
  Aborts step1 and PSC ACQ list search.
===========================================================================*/

extern boolean srchcm_interf_cleanup_interf_meas(
  /* Post search done call back function */
  SRCHCM_INTERF_SEARCH_DONE_CB_FUNC *cb_func);

/*===========================================================================
FUNCTION srchcm_meas_get_cm_gap_start_action_time_for_inter_f_srch

DESCRIPTION
  This function is to get CM gap start action time for interF search in FRC time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 srchcm_meas_get_cm_gap_start_action_time(
  /* The starting slot of the gap */
  uint8 tgsn,
  /* CFN for which CM GAP starts */
  uint8 tg_cfn);

#if defined(FEATURE_WPLT) || defined(FEATURE_WCDMA_PLT_MODE)

/*===========================================================================
FUNCTION srchcm_interf_update_info

DESCRIPTION
  This function is for WPLT use. In the absense of any real measurement
  control and reporting code this function can be called from task context
  to servive waiting for host update list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_interf_update_info(void);

#endif

/* Inter frequency cell add/delete management */
/* ------------------------------------------ */

/*===========================================================================
FUNCTION srchcm_meas_add_interf_cells

DESCRIPTION
  This function adds the WCDMA inter frequency cells to the CM measurement.
  The cells can be added any time. Cells are added at the end of the measurement
  list. Only one frequency cell can be added to the drivers. If there are cell
  existing in the drivers meas list then the cells added must be of the same
  frequency.
  If measurement control wants to add different frequency cells then all
  cells must be deleted.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on the all cell add sucess. If any cell add fails then
  it return FALSE.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchcm_meas_add_interf_cells(
  /* inter frequency cell list frequency */
  uint16 cell_freq,
  /* number of cells to be added */
  uint8 num_cells,
  /* scermabling code list */
  uint16 *scr_code_array,
  /* cell PN position array, value 0xFFFF in unknown */
  uint32 *pn_pos_cx8,
  /* indicate cell has STTD enabled or not */
  boolean *sttd_enable,
  /* interf cell info pointer array */
  srchcm_interf_cell_struct_type *interf_cell_info[]
  #ifdef FEATURE_WCDMA_DC_HSUPA
  /* If a step1 update on this frequency is also desired, this points to the 
     info.  Otherwise leave NULL */
  , srchcm_interf_step1_info_struct_type *step1_info
  #endif
  );

/*===========================================================================
FUNCTION srchcm_meas_remove_interf_cells

DESCRIPTION
  This function remove the WCDMA inter frequency cells from the CM measurement.
  The cells can be removed any time. Cell frequency is checked for deletion.
  When any cell(s) are deleted the meas cell list is moved up. The current
  meas cell index is also adjusted if it is effected. If the cell being removed
  is the cell that is currently under measurement then current meas cell pointer
  is set to NULL.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on the all cell remove sucess. If any cell add fails then
  it return FALSE.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchcm_meas_remove_interf_cells(
  /* number of cells to be added */
  uint8 num_cells,
  /* scermabling code list */
  srchcm_interf_cell_struct_type *del_cell_ptr[]);

/*===========================================================================
FUNCTION srchcm_meas_remove_all_interf_cells

DESCRIPTION
  This function remove all WCDMA inter frequency cells from the CM measurement.
  The cells can be removed any time. The cell pointer currently under measurement
  is set to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_remove_all_interf_cells(void);

/*===========================================================================
FUNCTION srchcm_meas_adjust_reconf_period_for_ttt

DESCRIPTION
  This function sets dynamic values to the reconfirmation time based on any 
  triggered TTTs for cell reporting.  It should be called by measurement layer
  anytime a TTT is started or reset.

  Internally we'll adjust our reconfirmation threshold to 
                 Min(Max(ttt_min, 100ms), 320ms).

  In other words, our reconfirmation timer will match ttt_min within the range 
  of 100-320 ms.

  This will speed up the reporting process, and help prevent dropped calls in 
  quickly deteriorating channel conditions.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - True if succesfully configured. False otherwise.

SIDE EFFECTS
  Changes reconfirm period threshold in global srchcm_interf_info for the given
  frequency.
===========================================================================*/
extern boolean srchcm_meas_adjust_reconf_period_for_ttt(
  /* the frequency to adjust the reconf period on */
  uint16 arfcn, 
  /* True if the start of any TTT on the freq has been triggered, and we’re waiting 
     for the TTT to complete for a report to be sent to the network */
  boolean any_ttt_active, 
  /* If any_TTT_active is TRUE, this should be the smallest TTT value of those
     active TTTs (in ms) */
  uint16 ttt_min);


/* --------------- */
/* GSM measurement */
/* --------------- */

/*===========================================================================
FUNCTION srchcm_gsm_get_meas_info

DESCRIPTION
  This function gets the GSM measurement info for the given CFN and
  measurement purpose. For GSM the measurement info is maintained in advance
  by N number of WCDMA frames in order to reduce the processing of WCDMA GAP
  information for GSM measurement. It looks into the advance GSM measurement
  info array, checeks the CFN and measurement purpose at the current info
  index. If both of these items match it returns the measurement info.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to measurement info structure of type srchcm_gsm_meas_info_struct_type.
  NULL if there is no measurement info present for the given parameters.

SIDE EFFECTS
  None
===========================================================================*/
extern srchcm_gsm_meas_info_struct_type* srchcm_gsm_get_meas_info(
  /* CFN of WCDMA frame in which gap is starting */
  uint8 cfn);

/*===========================================================================
FUNCTION srchcm_gsm_query_gsm_meas_unused

DESCRIPTION
  This function finds the GSM advanced information and if it exists then
  check the GSM purpose and see if that has any thing that can use that GAP.
  If that gap has no GSM cell to act upon for a given purpose then it returns
  TRUE otherwise FALSE.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchcm_gsm_query_gsm_meas_unused(
  /* WCDMA CFN in which gap start */
  uint8 cfn);

/*===========================================================================
FUNCTION srchcm_gsm_update_adv_meas_info

DESCRIPTION
  This function updates the advanced GSM information. L1 always maintains the
  advanced information of coming gap, at least a 1 WCDMA frame in advance so
  that required commands command can be sent ASAP without enoccuring the
  overhead of validating the gap and calulating when and where the required
  command can be sent.
  It gets the advanced info pointer and call the CM search drivers with advanced
  measurement info pointer and GAP information to get the information for a
  given measurement purpose.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_gsm_update_adv_meas_info(
  /* WCDMA CFN in which gap start */
  uint8 cfn,
  /* GSM measurement purpose */
  srchcmdrv_gsm_meas_purpose_enum_type meas_purpose,
  /* gap length in wcdma slots */
  uint8 gap_length_slots,
  /* transmission gap parameter */
  schcm_cm_gap_param_gsm_struct_type *gap_param_for_gsm);

/*===========================================================================
FUNCTION srchcm_meas_trigger_gsm_meas

DESCRIPTION
  This function triggers the GSM measurement for the upcoming GAP. It checks
  if the GSM measurement can be done. If it can be done it updates the required
  variables in the measurement info and returns the trigger status as TRUE.

  If measurement can't be done, it returns the status FALSE.

  It calls various sub function to trigger appropriate measurement.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE as per trigger status.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchcm_meas_trigger_gsm_meas(
  /* WCDMA CFN in which gap start */
  uint8 cfn,
  /* GAP length in WCDMA slot units */
  uint8 gap_length,
  /* GAP start inf slot number in WCDMA slot unit */
  uint8 gap_start_slot_num,
  /* GSM measurement purpose */
  srchcmdrv_gsm_meas_purpose_enum_type meas_purpose,
  /* BSIC Id abort count. Valid only for BSIC ID purpose */
  uint16 num_gaps_bsic_id_abort);

/*===========================================================================
FUNCTION srchcm_meas_gsm_meas_gap_unusable

DESCRIPTION
  This function is called for unusable gap. In this case the it just update
  meas info buffer and update other global as per meas purpose.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_gsm_meas_gap_unusable(
    /* WCDMA CFN in which gap start */
  uint8 cfn,
  /* BSIC Id abort count. Valid only for BSIC ID purpose */
  uint16 num_gaps_bsic_id_abort,
  /* Flag to indicate if gap was blocked for intraf. meas. */
  boolean gap_blocked_by_intraf);


/*===========================================================================
FUNCTION srchcm_meas_update_num_cmd_sent
         srchcm_meas_update_acq_start_cmd_sent
         srchcm_meas_update_acq_stop_cmd_sent
         srchcm_meas_update_sch_decode_cmd_sent

DESCRIPTION
  This function updates the number of command(s) sent to mDSP. This is used
  for unwinding the RF drivers at the time of cleanup.

  The caller of these functions must lock the W2G mutex.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_meas_update_rssi_num_cmd_sent(void);
extern void srchcm_meas_update_acq_start_cmd_sent(void);
extern void srchcm_meas_update_acq_stop_cmd_sent(void);
extern void srchcm_meas_update_sch_decode_cmd_sent(void);
extern void srchcm_meas_cleanup_cmd_cb_0(void);
extern void srchcm_meas_cleanup_cmd_cb_1(void);

/*===========================================================================
FUNCTION srchcm_do_gsm_meas

DESCRIPTION
  This function actually performs the GSM measurements. It uses the measurement
  info pointer saved by trigger function and calls the appropriate function
  based on purpose to do the measurement.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_do_gsm_meas(
  /* WCDMA CFN in which gap start */
  uint8 cfn,
  /* GSM measurement purpose */
  srchcmdrv_gsm_meas_purpose_enum_type meas_purpose);

/*===========================================================================
FUNCTION srch_meas_dummy_process_rssi_result

DESCRIPTION
  This function sorts out the bsic verification list once the RSSI result is
  done and all the states are changed to IDLE.

  This function is called from GSMTR ISR as well as from TASK when a cleanup
  is happening. The caller must get the W2G mutex. This function does not
  grab it!!!

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_meas_dummy_process_rssi_result(void);

/*===========================================================================
FUNCTION srchcm_meas_trigger_gsm_rssi_meas

DESCRIPTION
  This function triggers the GSM RSSI (power scan) measurements. It updates
  the cell ARFCN to the mDSP commands for power scan. It returns the trigger
  status based on power scan commands required for mDSP. False is returned
  on following conditions:
  - No GSM cell present
  - Number of cells that can be measured in this GAP is 0.
  - Number of doable cells left in this RSSI measurement cycle is 0. This
    condition can only happen if the remaining GSM cell are all deleted one
    with measurement purpose NA.
  It also updates the result processing related information so that result
  can be read and processed.

DEPENDENCIES
  None

RETURN VALUE
  SRCHCM_GSM_SRCH_TRIGGER_FAILED,
  SRCHCM_GSM_SRCH_TRM_RF_REQUEST_FAILED,
  SRCHCM_GSM_SRCH_TRIGGER_SUCCESS

SIDE EFFECTS
  None
===========================================================================*/

extern srch_gsm_search_trigger_enum_type srchcm_meas_trigger_gsm_rssi_meas(
  /* measurement info for RSSI measurement */
  srchcm_gsm_meas_info_struct_type *meas_info,
  /* Duration of IRAT RF usage  */
  uint32 dirat);

/*===========================================================================
FUNCTION srchcm_meas_update_gsm_rssi_meas

DESCRIPTION
  This function update the RSSI measurement related information based on the
  compressed mode GAP information. This function assumes that F/W can process
  the number of RSSI measurement as specified in Spec. It distributes the
  measurement position for the possible cell count in the available gap.
  While calculating the available gap space it excludes the time required
  for GSM start up and cleanup. These are defined as macros and can be tuned.
    SRCHCM_GSM_T_START_QS_PWR_SCAN
    SRCHCM_GSM_T_CLEAN_QS_PWR_SCAN
  These are specified in GSM quater symbol resolution.

  It calls the CM driver function to get the burst window position and actual
  burst start offset for the commands.

  Burst window: This include the burst setup time and actual burst sample
  accumulation time. Actual burst is always at the end of burst window.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_update_gsm_rssi_meas(
  /* gap length in wcdma slots */
  uint8 gap_length_slots,
  /* transmission gap parameter */
  schcm_cm_gap_param_gsm_struct_type *gap_param_for_gsm,
  /* GSM meas info struct pointer */
  srchcm_gsm_meas_info_struct_type *meas_info
  #ifdef FEATURE_DUAL_SIM
  /* AS ID to be passed to GL1 functions */
  , sys_modem_as_id_e_type as_id
  #endif
  );

/*===========================================================================
FUNCTION srchcm_meas_update_gsm_rssi_meas_in_sch

DESCRIPTION
  This function update the RSSI measurement related information inside SCH decode gap. 
  This function assumes that F/W can process the number of RSSI measurement with SCH 
  decode as specified in Spec. It distributes the measurement position for the possible 
  cell count in the remaining available gap out of SCH window. While calculating the 
  available gap space it excludes the time required for GSM start up, cleanup, and SCH 
  also decode window. These are defined as macros and can be tuned.
    SRCHCM_GSM_T_START_QS_PWR_SCAN
    SRCHCM_GSM_T_CLEAN_QS_PWR_SCAN
    SRCHCM_GSM_BSIC_ID_SCH_BURST_DEC_SZ
  These are specified in GSM quater symbol resolution.

  It calls the CM driver function to get the burst window position and actual
  burst start offset for the commands.

  Burst window: This include the burst setup time and actual burst sample
  accumulation time. Actual burst is always at the end of burst window.

  Num bursts: This function also calculates the number of bursts can be fit before/after 
  SCH decode window.

  This function is called by srchcm_gsm_update_adv_meas_info() which locks
  the W2G mutex prior to this function.


DEPENDENCIES
  None

RETURN VALUE
  TRUE: RSSI bursts inside SCH gap

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchcm_meas_update_gsm_rssi_meas_in_sch(
  /* gap length in wcdma slots */
  uint8 gap_length_slots,
  /* transmission gap parameter */
  schcm_cm_gap_param_gsm_struct_type *gap_param_for_gsm,
  /* GSM meas info struct pointer */
  srchcm_gsm_meas_info_struct_type *meas_info
  #ifdef FEATURE_DUAL_SIM
  /* AS ID to be passed to GL1 functions */
  , sys_modem_as_id_e_type as_id
  #endif
  );

/*===========================================================================
FUNCTION srchcm_meas_update_rssi_meas_prd

DESCRIPTION
  This function increments the RSSI measurement period counter if the
  RSSI measurement is in progress.
  The RSSI TGMP exist parameter is passed. The reason for that is
  that in case the RSSI measurement is in progess and RSSI TGMP is not
  existing then there is no use of increment this unnecessarily. It can
  overflow if the RSSI TGMP pattern is not activated soon enough.
  However in case enough samples have been already collected then
  RSSI meas measurement should be processed and sorted at the end of 480 ms.
  For this reason in case RSSI meas is in progress and RSSI TGMP is not
  existing now then RSSI meas period should be advanced upto 480 ms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_update_rssi_meas_prd(
  /* indicate if TGMP RSSI meas purpose is existing */
  boolean rssi_tgmp_exist);

/*===========================================================================
FUNCTION srchcm_meas_poll_rssi_meas_result

DESCRIPTION
  This function is a call back function. GSM frame tick handler calls this
  function to poll the RSSI measurement result. WHile triggering the RSSI
  measurement the result processing parameters are updated. In that GFN
  of the current command to poll indicate the GFN when the results are available.
  Once the GFN mactes the GFN for which the command is to be read, it calls the
  GSM driver function to read the RSSI result for that command. If all the
  command results are available it then calls the function to process the RSSI
  reault.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_poll_rssi_meas_result(
  /* current frame number */
  uint8 cur_gfn);

/*===========================================================================
FUNCTION srchcm_meas_process_rssi_meas_result

DESCRIPTION
  This function processes the RSSI result. It updates the RSSI to cell
  structure.

  It also checks if all the GSM cells have been measured for RSSI,it triggers
  the RSSI processing that sorts the cells as per RSSI value and updates the
  BSIC verification list. The result processing also includes the addition of
  new cells being added and the removal of cells being deleted. Cells are not
  dynamically added/removed from list but are done in this post processing.
  All this is done in the L1 task context. Signal is set if all the measurement
  process RSSI measurement (which is surely done by now here), BSIC verification
  (ID and reconf) are done and in the idle state. If any of the measurement
  state is busy then it sets the global for each busy task so that it can set
  the L1 task to post process the result when it is done.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_process_rssi_meas_result(void);

/*===========================================================================
FUNCTION srchcm_meas_sort_rssi_update_bsic_ver_list

DESCRIPTION
  This function must be called in task context. After calling this function
  the measurement control and report module may copy the necessary result variable
  to their local copy for evaluation and free the measurement state to IDLE so that
  measurement can proceed.
  
  This function do the following things in the order mentioned below.
  - Remove any deleted cell from the list
  - Do RSSI averaging of the all the samples received
  - Do the RSSI sorting only if there is cell left after processing the delete list
  - Create the Cell verification list. These are with highest N RSSI
  - Remove unwanted cell from BSIC verification list. Cell may be deleted or may have
    dropped from highest N RSSI cell that need BSIC verification
  - Add the cell that are not in BSIC verification list for initial BSIC identification
  - Add new cell to RSSI meas

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_meas_sort_rssi_update_bsic_ver_list(void);

/*===========================================================================
FUNCTION srchcm_gsm_check_reset_bsic_id_abort_cell_list

DESCRIPTION
  This function check if all teh cells have been aborted in BSIC Id phase.
  In this case it reset the state of all cells to BSIC Id and resorts the cells.

  It is called
  1. When from BSIC ver list refesh (task context)
     The covers if all BSIC ver list has aborted cells as result of all BSIC Id
     cells getting deleted.

  OR

  2. When any BSIC Id cell transitions to BSIC Id abort
     2.1 GSM gap with BSIC Id trigger
     2.2 At BSIC Id result poll
  
  1 and 2.2 may not be mutully exclusive but 1 and 2.1 are not. So WCDMA_INTLOCK
  the whole duration

  This function locks W2G mutex internally.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_gsm_check_reset_bsic_id_abort_cell_list(void);

/*===========================================================================
FUNCTION srchcm_list_srch_egy_sort_comp_fn

DESCRIPTION
  RSSI sort function for Qsort

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern int8 srchcm_meas_gsm_rssi_sort_comp_fn(
  const void *p1,
  const void *p2);

/*===========================================================================
FUNCTION srchcm_gsm_update_bsic_ver_list

DESCRIPTION
  This function updates BSIC verification list. This isintended to be called
  in task context. Typical usage will be to call after every RSSI measurement
  done signal or can be called any time tehre is change in BSIC verification
  required status of GSM cell list.
  It look for global srchcm_meas_cell_bsic_ver_status_update_pending and if
  TRUE it updates bsic_verify_reqd member in all cell structure with value
  of global srchcm_gsm_bsic_ver_status_to_update.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_gsm_update_bsic_ver_list(void);

/*===========================================================================
FUNCTION srchcm_gsm_update_bsic_ver_status

DESCRIPTION
  This function updates the BSIC verification required status for all GSM
  cells. It put the BSIC Id and BSIC reconf state in PROC state so that
  it can iterate thro' the list.
  If it can't put the BSIC id or BSIC reconf to PROC state it will set their
  respective global flag
  
  BSIC Id: srchcm_meas_cell_bsic_ver_status_update_after_bsic_id_done
  BSIC reconf: srchcm_meas_cell_bsic_ver_status_update_after_bsic_reconf_done
  
  BSIC verfication required state is saved to global variable
  srchcm_gsm_bsic_ver_status_to_update

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_gsm_update_bsic_ver_status(
  /* indicate BSIC verify status to update */
  boolean bsic_ver_reqd);

/*===========================================================================
FUNCTION srchcm_meas_bsic_id_abort_cell

DESCRIPTION
  This function do BSIC Id abort for a cell

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_meas_bsic_id_abort_cell(
  /* GSM cell structure pointer */
  srchcm_gsm_cell_struct_type *cell_ptr);

/*===========================================================================
FUNCTION srchcm_meas_query_known_cell_qs_offset

DESCRIPTION
  This function queries the known cell offset in BSIC reconfirmation list,
  if it exists returns the offset in function argument frame offset and
  qs_offset passed as pointer.

DEPENDENCIES
  None

RETURN VALUE
  Number of cells found forbidden

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 srchcm_meas_query_known_cell_qs_offset(
  /* cell ARFCN to cehck in reconf list */
  W_ARFCN_T arfcn,
  /* number of max cell to find out */
  uint8 num_max_cell,
  /* if cell exists, frame_offset is returned in this */
  uint8 *frame_offset,
  /* if cell exists, qs_offset is returned in this */
  uint16 *qs_offset);

/*===========================================================================
FUNCTION srchcm_meas_trigger_gsm_bsic_id_meas

DESCRIPTION
  This function triggers the BSIC Id measurement. It scanns through the list of
  cells in the measurement info. It assumes that cells will be put in the order
  if priority of their measurement state. It just goes thro' the list and grab
  the first available one.

DEPENDENCIES
  None

RETURN VALUE
  SRCHCM_GSM_SRCH_TRIGGER_FAILED,
  SRCHCM_GSM_SRCH_TRM_RF_REQUEST_FAILED,
  SRCHCM_GSM_SRCH_TRIGGER_SUCCESS

SIDE EFFECTS
  None
===========================================================================*/

extern srch_gsm_search_trigger_enum_type srchcm_meas_trigger_gsm_bsic_id_meas(
  /* BSIC id abort count */
  uint16 num_gaps_bsic_id_abort,
  /* GSM measurement info */
  srchcm_gsm_meas_info_struct_type *meas_info,
  /* Duration of IRAT RF usage  */
  uint32 dirat);

/*===========================================================================
FUNCTION srchcm_meas_update_gsm_bsic_id_abort_count

DESCRIPTION
  This function updates the BSIC Id abort count. This is used only when there
  is a GAP for BSIC Id measurement purpose but is not used to trigger the
  measurement. In that case the BSIC Id abort count must be updated

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_update_gsm_bsic_id_abort_count(
  /* BSIC id abort count */
  uint16 num_gaps_bsic_id_abort);

/*===========================================================================
FUNCTION srchcm_meas_update_gsm_bsic_id_meas

DESCRIPTION
  This function updates the BSIC Id parameters of all cells that can do BSIC
  identification. It receives the gap parameters and calculate the search
  parameters. In the BSIC Id there are 2 parts.
  FCCH timing detect: If the timing is not known then it calculates the FCCH
                      aquisition window start position and window size.
  SCH decode: If the FCCH timing is known then it checks if the SCH decode can
              done in the GAP. If it can be done it updates the SCH decode
              position in the gap in measurement info.

  It scans through the list of cells that can do BSIC Id. These are the cell
  with timing invalid and/or BSIC invalid. Each cell can have two measurement
  states
    BSIC Identify (This is bSIC identify for the first time)
    BSIC Id aborted (This cell has timed out for number of gaps to be utilzed
                     for identifying the BSIC)
  Former one is given preference over the latter. The first cell that satisfy
  this criteria is chosen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_update_gsm_bsic_id_meas(
  /* transmission gap parameter */
  schcm_cm_gap_param_gsm_struct_type *gap_param_for_gsm,
  /* GSM meas info struct pointer */
  srchcm_gsm_meas_info_struct_type *meas_info
  #ifdef FEATURE_DUAL_SIM
  /* AS ID to be passed to GL1 functions */
  , sys_modem_as_id_e_type as_id
  #endif
  );

/*===========================================================================
FUNCTION srchcm_meas_poll_fcch_detect_result

DESCRIPTION
  This function is a call back function. GSM frame tick handler calls this
  function to poll the FCCH detect result. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_poll_fcch_detect_result(
  /* current frame number */
  uint8 cur_gfn);

/*===========================================================================
FUNCTION srchcm_meas_poll_sch_decode_result

DESCRIPTION
  This function is a call back function. GSM frame tick handler calls this
  function to poll the SCH decode result. This is a common poll function for
  SCH decode on BSIC Id and BSIC reconfirmation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_poll_sch_decode_result(
  /* current frame number */
  uint8 cur_gfn);

/*===========================================================================
FUNCTION srchcm_meas_update_bsic_id_result_after_sch_decode

DESCRIPTION
  This function processes the SCH decode resulr for BSIC identification.
  It do the appropriate measurement state transition as needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchcm_meas_update_bsic_id_result_after_sch_decode(
  /* GSM cell pointer info */
  srchcm_gsm_cell_struct_type *cell_ptr,
  /* SCH decode result */
  boolean sch_decode_result);

/*===========================================================================
FUNCTION srchcm_meas_update_bsic_reconf_result_after_sch_decode

DESCRIPTION
  This function processes the SCH decode resulr for BSIC re-confirmation.
  It do the appropriate measurement state transition as needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchcm_meas_update_bsic_reconf_result_after_sch_decode(
  /* GSM cell pointer info */
  srchcm_gsm_cell_struct_type *cell_ptr,
  /* SCH decode result */
  boolean sch_decode_result);

/*===========================================================================
FUNCTION srchcm_meas_gsm_bsic_reconf_abort_timer_update

DESCRIPTION
  This function update the reconf abort update timer. This is called every
  WCDMA frame by L1 CM manager to check the BSIC reconfirm timer and BSIC
  decode attemptfor the cells that are there for BSIC reconfirmation.
  If any cell has expired its timer for BSIC reconfirmation then its timing
  and BSIC is assumed invalid. It is put back for BSIC identification.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_gsm_bsic_reconf_abort_timer_update(
  /* number of frames (corresponding to Tre-confirm_abort time) */
  uint16 num_wcdma_frame_reconf_abort);

/*===========================================================================
FUNCTION srchcm_meas_force_reconf_abort

DESCRIPTION
  This function forces the cell out from reconf list. The typical usage of
  this function is when meas eval and report module finds that cell with BSIC
  found doesn't exists in the cell list then it force aborts the cell and put
  it back to BSIC Id list with that cell timing marked as forbidden.
  
  This function is intended to be called from lower priority ISR or task context.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on trigger result.

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_force_reconf_abort(
  /* cell pointer of the cell to do reconf abort */
  srchcm_gsm_cell_struct_type *cell_ptr,
  /* last QS ofset to be marked forbidden */
  boolean mark_last_qs_forbiden);

/*===========================================================================
FUNCTION srchcm_meas_trigger_gsm_bsic_reconf_meas

DESCRIPTION
  This function triggers the BSIC reconf measurement. It iterate through cells
  that can do BSIC reconf and find the cell with maximum BSIC verification age.
  If it finds a cell it saves the cell index in to global and return TRUE other
  wise it sends FALSE for no trigger.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on trigger result.

SIDE EFFECTS
  None
===========================================================================*/

extern srch_gsm_search_trigger_enum_type srchcm_meas_trigger_gsm_bsic_reconf_meas(
  /* GSM meas info struct pointer */
  srchcm_gsm_meas_info_struct_type *meas_info);

/*===========================================================================
FUNCTION srchcm_meas_update_gsm_bsic_reconf_meas

DESCRIPTION
  This function updates the BSIC reconfirmation info in the maesurement info.
  It iterates thro the cell in BSIC verifivation set and based on the GAP and
  GSM cell timing info available it finds the cell that can do the BSIC
  reconformation. It doesn't check for the BSIC verification age. It updates
  the BSIC reconf related info to meas info structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_meas_update_gsm_bsic_reconf_meas(
  /* GSM meas info struct pointer */
  srchcm_gsm_meas_info_struct_type *meas_info,
  /* transmission gap parameter */
  schcm_cm_gap_param_gsm_struct_type *gap_param_for_gsm
  #ifdef FEATURE_DUAL_SIM
  /* AS ID to be passed to GL1 functions */
  , sys_modem_as_id_e_type as_id
  #endif
  );

/*===========================================================================
FUNCTION srchcm_meas_gsm_sch_decode_possible

DESCRIPTION
  This function checks if the SCH decode is possible or not. This cjecks for
  back to back SCH for BSIC Id and BSIC re-conf. Back to Back SCh are only
  in BSID Id in a gap.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based SCH decodable.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchcm_meas_gsm_sch_decode_possible(
  /* SCH decode GFN */
  uint8 sch_decode_frame_num);

/*===========================================================================
FUNCTION srchcm_gsm_cmd_cleanup_done

DESCRIPTION
  This function is a callback function that is set by function
  srchcm_cleanup_gsm_pending_meas when it calls srchgsm_clean_pending_cmd_q
  function to cleanup the GSM pending Q.
  It waits for N gsm frame as defined by SRCHCM_GSM_NUM_FR_WAIT_FOR_CLEANUP
  macro after all the commands have been issued to mDSP

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

typedef void SRCHCM_GSM_CMD_CLEANUP_DONE_CB_FUNC_TYPE(void);

extern void srchcm_gsm_cmd_cleanup_done(uint8 num_cmd_pending);

/*===========================================================================
FUNCTION srchcm_gsm_cmd_pre_cleanup

DESCRIPTION
  This function cleans up is called in GSTMR and unwinds RF stack.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_gsm_cmd_pre_cleanup(void);

/*===========================================================================
FUNCTION srchcm_cleanup_gsm_pending_meas

DESCRIPTION
  This function cleanup and GSM command in pending Q and unwinds RF stack
  based on commands that have been or will sent will cleanup is done.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if cleanup completes during function invocation.
          \ FALSE if cleanup is to finish later in time.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchcm_cleanup_gsm_pending_meas(
  /* call back function when GSM cmd has finished in mDSP in worst case */
  SRCHCM_GSM_CMD_CLEANUP_DONE_CB_FUNC_TYPE* cb_func);

/*===========================================================================
FUNCTION srchcm_gsm_add_cell_to_ver_list

DESCRIPTION
  This function adds the cell to the BSIC verification list.The BSIC
  verifcation list is organized as following
  
  BSIC verification list start
   - First index cell for BSIC Id measurement state
  |
  |    Cells with initial BSIC Id
  |
  |
  |- First index cell for BSIC Id aborted state
  |
  |    Cells with BSIC Id aborted due to N_id_abort
  |
  |-First index cell for BSIC reconfirmation state
  |
  |    Cells for BSIC reconfirmation 
  |
  |--------------------------------------------

  This function takes the cell pointer and put the cell in the BSIC
  verification list in the area as specified by measurement state.
  If the RSSI sort is asked for BSIC Id and BSIC Id aborted it put
  the cell based on RSSI in front of the cell whose RSSI is less than
  its own other wise it is added to the end of that area.
  It updates the various counters and first indicies for various parts
  of the cell BSIC verification.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_gsm_add_cell_to_ver_list(
  /* GSM cell pointer to add */
  srchcm_gsm_cell_struct_type *cell_ptr,
  /* Cell BSIC verification state based on which cell is added */
  srch_gsm_srch_meas_state meas_state,
  /* indicate if the cell is added based on the RSSI it is added
     at the end of list */
  boolean sort_by_rssi);

/*===========================================================================
FUNCTION srchcm_gsm_remove_cell_from_ver_list

DESCRIPTION
  This function removes the cell to the BSIC verification list. It check the
  meas state of the cell and updates the first index and movers the cells up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_gsm_remove_cell_from_ver_list(
  /* cell index that need to be removed */
  uint8 cell_idx);

/*===========================================================================
FUNCTION srchcm_gsm_move_cell_in_ver_list

DESCRIPTION
  This function moves the cell within the cell verification list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_gsm_move_cell_in_ver_list(
  /* GSM cell pointer to add */
  srchcm_gsm_cell_struct_type *cell_ptr,
  /* Cell BSIC verification state based on which cell is moved */
  srch_gsm_srch_meas_state new_meas_state,
  /* indicate if the cell is added based on the RSSI it is added
     at the end of list */
  boolean sort_by_rssi);


/*===========================================================================
FUNCTION srchcm_log_info_update_submit

DESCRIPTION
  This function submit the CM meas log. It manages the log buffers also. The
  CM log packets are double buffered. the reson for the double buffereing is that
  once a log is completely filled in trigger functions, the log info is still
  pending waiting for th eresult to be available.
  In the meas time the second log buffer is used to fill the info.
  
  This function is called periodically every WCDMA frame.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based SCH decodable.

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_log_info_update_submit(void);


/*===========================================================================
FUNCTION srchcm_get_empty_inter_rat_cell

DESCRIPTION
  This function gets pointer from inter-RAT empty set. It gets the pointer
  from end of the list and adjust the empty set count.

DEPENDENCIES
  None

RETURN VALUE
  Cell pointer or NULL if not able to get any.

SIDE EFFECTS
  None
===========================================================================*/

extern srchcm_inter_rat_cell_struct_type* srchcm_get_empty_inter_rat_cell(void);

/*===========================================================================
FUNCTION srchcm_get_empty_inter_rat_cell

DESCRIPTION
  This function puts the cell pointer to appropriate cell list based on the
  cell set type. At this moment only GSM cell set type is supported.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchcm_put_inter_rat_cell_to_set(
  srchcm_inter_rat_cell_struct_type *cell_ptr,
  srch_inter_rat_cell_type_enum_type set_type);

/*===========================================================================
FUNCTION srchcm_delete_inter_rat_cell_to_set

DESCRIPTION
  This function delete cell pointer from appropriate cell list based on the
  cell set type. The deleted cell is not put to the empty cell set. The caller
  of this function must the cell pointer to the empty set.
  At this moment only GSM cell set type is supported.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_delete_inter_rat_cell_to_set(
  srchcm_inter_rat_cell_struct_type *cell_ptr,
  srch_inter_rat_cell_type_enum_type set_type);

/*===========================================================================
FUNCTION srchcm_add_gsm_cell_for_meas

DESCRIPTION
  This function adds a single GSM cell from inter-RAT cell set list.

DEPENDENCIES
  None

RETURN VALUE
  Inter-RAT Cell pointer added. Null if not able to get an empty one.

SIDE EFFECTS
  None
===========================================================================*/

extern srchcm_inter_rat_cell_struct_type* srchcm_add_gsm_cell_for_meas(
#if defined(FEATURE_WPLT) || defined(FEATURE_WCDMA_PLT_MODE)
  /* ARFCN for GSM cell */
  W_ARFCN_T arfcn,
  /* BSIC verification required for this cell */
  boolean bsic_verify_reqd,
  /* expected RSSI */
  int16 expected_rssi_dbmx16
#else
  /* ARFCN for GSM cell */
  W_ARFCN_T arfcn,
  /* BSIC verification required for this cell */
  boolean bsic_verify_reqd
#endif
);

/*===========================================================================
FUNCTION srchcm_meas_gsm_meas_gap_trigger_failed

DESCRIPTION
  This function is called for trigger failed gap. In this case the it marks
  meas info buffer in use to FALSE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void srchcm_meas_gsm_meas_gap_trigger_failed(
    /* WCDMA CFN in which gap start */
  uint8 cfn
);

/*===========================================================================
FUNCTION srchcm_gsm_add_arfcn_in_unique_list

DESCRIPTION
  This function adds ARFCN to unique ARFCN list. It it already exists then
  it just return with message print.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_gsm_add_arfcn_in_unique_list(
  /* ARFCN to add */
  W_ARFCN_T *arfcn);

/*===========================================================================
FUNCTION srchcm_gsm_query_cell_exist_with_arfcn

DESCRIPTION
  This function scans thro' GSM cell list and check if it has ARFCN same as
  passed in function argument

DEPENDENCIES
  None

RETURN VALUE
  Number of cells with that ARFCN

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 srchcm_gsm_query_cell_exist_with_arfcn(
  /* ARFCN to add */
  W_ARFCN_T *arfcn,
  /* indicated all cell to scan or the one that are noe deleted ones */
  boolean scan_all_cells);

/*===========================================================================
FUNCTION srchcm_gsm_get_arfcn_info_ptr

DESCRIPTION
  This function scans thro' GSN ARFCN list and returns the pointer to ARFCN
  info with same ARFCN as passed in function argument otherwise return NULL

DEPENDENCIES
  None

RETURN VALUE
  pointer to srchcm_gsm_arfcn_info_struct_type structure type or NULL

SIDE EFFECTS
  None
===========================================================================*/

extern srchcm_gsm_arfcn_info_struct_type* srchcm_gsm_get_arfcn_info_ptr(
  /* ARFCN to add */
  W_ARFCN_T *arfcn);

/*===========================================================================
FUNCTION srchcm_gsm_del_arfcn_in_unique_list

DESCRIPTION
  This function removes ARFCN to unique ARFCN list. This function expect that
  ARFCN do exists in list otherwise it prints error message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_gsm_del_arfcn_in_unique_list(
  /* ARFCN to delete */
  W_ARFCN_T *arfcn);

/*===========================================================================
FUNCTION srchcm_gsm_trim_arfcn_sample_tail_to_max_age

DESCRIPTION
  This function trims the sample accumulation tail with in the max age w.r.t.
  current time to avoid stale sample accumulation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_gsm_trim_arfcn_sample_tail_to_max_age(
  /* Maximum age of RSSI sample allowed */
  uint16 max_age);

/*===========================================================================
FUNCTION srchcm_gsm_trim_arfcn_sample_tail_to_max_age

DESCRIPTION
  This function gets the average of at least 3 samples and at least 480 ms
  worth of samples and average them out

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE indicating is average RSSI value is valid or not

SIDE EFFECTS
  If successful then RSSI value pointer passed asargument to function is
  updated
===========================================================================*/

extern boolean srchcm_gsm_get_arfcn_rssi(
  /* ARFCN to get RSSI value for */
  W_ARFCN_T *arfcn,
  /* Average RSSI value */
  int16 *ave_rssi_dbmx16,
  /* This ignores minimum requirement of 480 ms period, 3 samples is met.
     it is assumed that this requirement is being maintained by measurement control */
  boolean ignore_min_requirement);

/*===========================================================================
FUNCTION srchcm_gsm_proceed_arfcn_rssi_meas_processing

DESCRIPTION
  This function scans ARFCN list for to see if any ARFCN meets minimum
  requirement of 480 ms and 3 samples.
  OR if there is any valid cell for RSSI processing

  If there is atleast one cell that can do RSSI processing or NO cell at all
  that is valid it returns TRUE.

  This function is intended to be called at RSSI processing function at
  callback RSSI done when last ARFCN index has been hit

  Check done in this function are simple and don't check for any RSSI sample
  age to eliminate any aged sample.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srchcm_gsm_proceed_arfcn_rssi_meas_processing(
  /* number of ARFCN valid */
  uint8 *num_valid_arfcn,
  /* number of minimum samples in valid ARFCN */
  uint8 *min_sample,
  /* min oldest sample age w.r.t. to current time */
  uint16 *min_oldest_rssi_sample_age);

/*===========================================================================
FUNCTION srchcm_gsm_cleanup_gsm_meas

DESCRIPTION
  This function cleans up all pending gsm measurements. This is typically called
  after cm has been suspended. If the gsm frame tick handler is not deregistered 
  in this routine it will get deregistered by posting a local command in the 
  post callback

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchcm_gsm_cleanup_gsm_meas(
  /* call back function when GSM cmd has finished in mDSP in worst case */
  SRCHCM_GSM_CMD_CLEANUP_DONE_CB_FUNC_TYPE* cb_func);

/*===========================================================================
FUNCTION srchcm_meas_init_drv_status

DESCRIPTION
  This function initializes the interf driver initialized status. This API is exported
  to higher modules. It's called at WL1 init, DPCH setup and idle init.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_meas_init_drv_status(void);

/*===========================================================================
FUNCTION srchcm_meas_reset_drv_status

DESCRIPTION
  This function resets the interf driver initialized status. This API is exported
  to higher modules. This is called at WL1 suspend.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_meas_reset_drv_status(void);

/*============================================================================
FUNCTION srchcm_interf_prep_xo_values_for_gap

DESCRIPTION
  This function updates the shared memory between FW and SW with the most 
  recent XO parameters translated for the neighbor.

DEPENDENCIES
  Make sure this is called after the rf buffers are updated with 
  mcalwcdma_cm_write_rf_buf_info, since this function can overwrite the values.  

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
void srchcm_interf_prep_xo_values_for_gap(
     /* the W2W Buffer Index corresponding to the searched neighbor cell */
     uint8 buf_idx
     #ifdef FEATURE_WCDMA_DC_HSDPA 
     /* Carrier spacing between f1/f2 */
     , uint16 carrier_spacing
     #endif 
     );
 
/*===========================================================================
FUNCTION srchcm_meas_allocate_w2w_rf_buf

DESCRIPTION
  This function allocates the mcalwcdma RF buffers for W2W measurements

DEPENDENCIES
  None

RETURN VALUE
  TRUE, if buffer allocated
  FALSE, otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchcm_meas_allocate_w2w_rf_buf(void);

/*===========================================================================
FUNCTION srchcm_gsm_meas_init


DESCRIPTION
  This function initializes the CM search and measurement related database,
  variables and states. The following initializations are done:

    RSSI measurement
  - Initializes the cell pointers to NULL.
  - Set the GSM cell (active and deleted) counts to 0
  - RSSI measurement last GSM cell index to INVALID

    BSIC verification
  - Set the BSIC verification cell count to 0
  - Set the cell pointers to NULL
  - Set the GSM BSIC id and reconf cell index to INVALID

    Result processing
  - Set the RSSI measurement call back function

  This function grabs the W2G mutex, so callers of it do not need to wrap
  this function inside W2G mutex.

DEPENDENCIES
  This function must be called after the CM search drivers are initialized
  otherwise the result processing call back function will be over written.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_gsm_meas_init(void);

/*===========================================================================
FUNCTION srchcm_init_cm_irat_drv_on_resume

DESCRIPTION
  This function initialises all IRAT (W2W, W2G and W2L) module data structure 
  and drivers needed for compressed mode in case of a suspend-resume WCDMA because of
  HHO or forced sync A if IRAT neighbors are present.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_init_cm_irat_drv_on_resume(void);

/*===========================================================================
FUNCTION srchcm_get_interf_srch_in_progress

DESCRIPTION
  This function returns whether a PSC ACQ/Reconf List/Step1 search is currently in progress with CM

DEPENDENCIES
  None

RETURN VALUE
  TRUE if List/Step1 search is currently in progress 
  FALSE if List/Step1 search is currently NOT in progress 

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchcm_get_interf_srch_in_progress(void);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION srchcm_get_rssi_meas_in_progress

DESCRIPTION
  This function check if CM W2G searches is in progress and there is no GSM 
  cell in BSIC ID state yet

DEPENDENCIES
  None

RETURN VALUE
  True if W2G srch is in progress and number of cells for BSIC verification
  is 0.
SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchcm_get_rssi_meas_in_progress(void);
#endif

#ifdef FEATURE_WCDMA_COEXISTENCE_SW
/*===========================================================================
FUNCTION srchcm_report_w2w_interf_meas_info_to_cxm

DESCRIPTION
  This function registers the frequency info(uarfcn) of the scheduled
  W2W Interfrequency measurement with WL1CXM and de-registers the
  frequency info(uarfcn) upon measurement completion

  Note:
  =====
  The following Static local variables
  **static uint8 pending_gap_idx;
  **static uint16 pending_gap_uarfcn;
  **static boolean gap_info_registered[MCALWCDMA_SRCH_CM_MAX_NUM_CMQ];
  **static boolean pending_gap_reg;
  have been introduced to handle back to back W2W Interf CM gaps
  The idea is retain/register the frequency of the second gap only
  after the frequency info of the first gap has been de-registered

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcm_report_w2w_interf_meas_info_to_cxm(uint16 uarfcn, boolean is_cleanup, uint8 index);
#endif /* FEATURE_WCDMA_COEXISTENCE_SW */

/*===========================================================================
FUNCTION SRCHCM_IS_STEP1_SRCH_SUSPENDED

DESCRIPTION
  This function checks if Step1 srch is pending

DEPENDENCIES
  None

RETURN VALUE
  True if Step1 search is pending
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchcm_is_step1_srch_suspended(void);

/*===========================================================================
FUNCTION srchcm_meas_get_cm_gap_start_action_time_for_inter_f_srch

DESCRIPTION
  This function is to get CM gap start action time for interF search in FRC time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 srchcm_meas_get_gap_action_time(
  /* The starting slot of the gap */
  uint8 tgsn,
  /* CFN for which CM GAP starts */
  uint8 tg_cfn
);

/*===========================================================================
FUNCTION srchcm_query_w2x_dch_fach_meas_ongoing

DESCRIPTION
  This function is to determine if W2G or W2L DCH or FACH meas is in the progress
  
RETURN VALUE
  NONE
  
SIDE EFFECTS
=============================================================================*/
boolean srchcm_query_w2x_dch_fach_meas_ongoing(void);

/*===========================================================================
FUNCTION srchcm_query_w2w_detected_cell_condition

DESCRIPTION
  This function is to check detected cell condition of W2W measurement
  
RETURN VALUE
  NONE
  
SIDE EFFECTS
=============================================================================*/
boolean srchcm_query_w2w_detected_cell_condition(void);
#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION  srchcm_get_irat_interf_info

DESCRIPTION
  This function is to collect the irat interf meas info for debugging. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_get_irat_interf_info(void *debug_buf);

/*===========================================================================
FUNCTION  srchcm_get_cm_gsm_meas_info

DESCRIPTION
  This function is to collect the cm w2g meas info for debugging. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_get_cm_gsm_meas_info(void *debug_buf);
#endif /* FEATURE_QSH_DUMP */

#endif /* SRCHCMMEAS_H */
