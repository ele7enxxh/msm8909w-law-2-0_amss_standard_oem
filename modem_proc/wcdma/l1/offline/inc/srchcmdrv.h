#ifndef SRCHCMDRV_H
#define SRCHCMDRV_H

/*============================================================================
                            S R C H C M D R V . H
DESCRIPTION

Copyright (c) 2000-2011 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchcmdrv.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
12/23/14   mk      Updated thresholds for IRAT searches (Applicable only to Jolokia/tobasco modem) 
12/05/14   cc      Reduce array size of unused elements
08/04/14   rs      Fixed compiler warnings
06/19/14   cc      IRAT DR-DSDS check in
05/30/14   dp      W2W restructure with mult step1 peak search in pn-acq gaps
05/29/14   mm      Update IRAT/CM driver initialization and RxLM buffer allocation call flow
04/07/14   cc      Protecting shared global variables
04/02/14   mm      3C IRAT/CM support
12/20/13   dp      Allow partial task completion from FW for DSDA
12/11/13   mm      3C CM/IRAT support
10/15/13   mm      Initial FE-FACH check-in
09/30/13   cc      DBDC IRAT support check in
09/12/13   jd      Searcher redesign
12/17/12   mm      Interface changes for passing AS ID to GL1 functions
12/11/12   cc      0x417A log pacekt improvement
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12   dp      Made it so W2W cleanup waits for FW searches to finish
12/08/11   dp      Change search tasks scheduled in CM with different
                   offline clock speeds.  Also fixed tabs.
11/11/11   yh      Added more GSM search states to allow concurrent searches in CM
10/26/11   yh      Fix calculation of available search window in a gap
09/12/11   yh      Add function to calculate number of pwr meas
09/02/11   dp      Fixed FW task math macros so that math is done in int32 type
                   instead of uint32 type, which was causing a bug.
05/26/11   dp      Added changes to make tuning time value dynamic
04/14/10   vsr     Enhancements to reduce IF false alarms
03/30/10   rvs     IRAT sample-server config changes for Chetak and
                   general clean-up.
08/31/09   yh      Change step1 thres for NN 30 to -23.7db (70 linear)
08/26/09   yh      Support for Interf searches when in low offline clock
06/23/09   yh      Make interF NN to be 30 instead of 45.
05/13/09   yh      Change step1 threshold to -24.5db because NN is 45, not 30.
04/20/09   yh      Compiler warning fixes.
03/27/09   yh      Modify SRCHCM_PN_TASKS_PER_SLOT to accommodate change 
                   in wcdma offline clk for cx28
01/21/09   sv      Fixed compile error.
01/20/09   sv      Updated PN search thresholds for CM.
11/13/08   yh      Add CM support
10/03/08   hk      Bringing in 7k Mailine fixes
06/08/07   yh      Set step1 results to saturation if exceeds 16 bits.
10/11/06   yh      Added Interf. rescheduling when DM is delayed
09/19/06   yh      Change PSC ACQ early abort threshold to -20dB.
08/02/06   yh      Added DM delay check.
04/05/06   bbd     Added frequency offset to BSIC id parm and BSIC reconf struct
01/16/06   gs      Updated RFAPI calls from common header file
12/05/05   bbd     Added support for CM STTD searches.
04/12/05   rc      Reverted back the SRCHCM_LIST_SRCH_EGY_THRESH and 
                   SRCHCM_LIST_SRCH_EARLY_ABORT_EGY_THRESH values. 
01/27/05   cpe     Updated a few search parameters based on latest memo
10/06/04   gs/bbd  Make SRCHCM_LIST_SRCH_WINSIZE dependent on length of gap.
                   Shorten SRCHCM_INTERF_TUNE_TIME_CX8 to 29184 (950 micro sec).
                   Modify SRCHCM_LIST_SRCH_MAX_TASK_1_NN to accommodate more
                   list search tasks per gap.
                   Modify srchcm_list_srch_set_srch_params() and 
                   srchcm_list_srch_for_psc_peaks_set_srch_params() to accept 
                   gap length in slots.
09/20/04   gs      Disabled code under FEATURE_SRCHCM_NO_SRCH_TILL_RESULT_PENDING
09/13/04   gs      Removed var srchcm_interf_step1_log_struct_in_use
                   Changed step 1 buffer to array that is accessed using queue
                   index of step 1 search used.
                   Function srchcm_step1_cmd now returns queue index used.
07/21/04   gs      Enabled code under FEATURE_SRCHCM_NO_SRCH_TILL_RESULT_PENDING
05/10/04   gs      Redid the macro definition SRCHCM_LIST_SRCH_MAX_TASK_1_NN.
                   With existing definition it was using worst case F/W trigger
                   time (assuming all tasks are triggered) to compute the number
                   of tasks doable. With number of max task increased to 16 it
                   was resulting in no search schedulable in gap of 7 slots.
04/08/04   gs      Moved macro SRCHCM_GSM_SCH_DEC_CMD_MAX_PER_GAP from file
                   srchcmmeas.h to this file.
                   Added structure member extra_qs_uncertainty_win to BSIC
                   id/reconf SCH decode parameter structure.
                   Changed prototype of function
                   srchcm_update_gsm_bsic_id_sch_dec_cmd_params
02/19/04   gs      Removed reference to FEATURE_SRCH_BITWIDTH_MULTI_FACTOR
                   and code under that.
01/16/04   gs      Changed PSC peak separation to 4 Cx1 (32 Cx8)
12/16/03   gs      Support for GSM Quadband
11/11/03   yus     Replace FEATURE_6250_COMPILE with FEATURE_SRCH_BITWIDTH_MULTI_FACTOR
11/06/03   gs      Removed duplicate declaration of 2 functions.
10/24/03   gs      Updated functionality for compressed mode inter-freq
                   search under FEATURE_CM_FOR_INTER_FREQUENCY_MEASUREMENTS
07/24/03   yus     Add support for MSM6250.
08/09/02   gs      Created this file
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "mdspsync.h"
#include "wl1rfmif.h"
#include "srchparm.h"
#include "srchgsmdrv.h"
#include "srchcmmeasparms.h"
#include "l1mathutil.h"
#include "mcalwcdma_srch.h"
#include "l1interflog.h"
#include "mcalwcdma_cm.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* WCDMA inter frequency search related macro */
/* ------------------------------------------ */
  /* CM search related parameters */
  /* ---------------------------- */
/* Theoretical num of Max tasks that can be scheduled for each search 
 * using a lowest 72MHz clock rate 
 */
#define SRCHCM_PN_SRCH_MAX_TASK 64
/* Max mum of PSC that can be scheduled per gap */
#define SRCHCM_MAX_NUM_PSC_ACQ_SRCH_PER_GAP 4
/* Number of PN search peaks to be reported by mDSP */
#define SRCHCM_NUM_PN_PEAKS_PER_TASK 6
/* Non coherent integration length to cover in step 1 substaks */
#define SRCHCM_STEP1_SRCH_NON_COH_INT_LEN 30
/* Step1 energy threshold -25.0  for SRCHCM_STEP1_SRCH_NON_COH_INT_LEN = 30 */
#define SRCHCM_STEP1_EGY_THRESH 47
/* Step1 energy threshold -10.5 dB for SRCHCM_STEP1_SRCH_NON_COH_INT_LEN = 30 */
#define SRCHCM_STEP1_PEAK_ENERGY_THRESH_MED 796
/* Step1 energy threshold -8.5 dB for SRCHCM_STEP1_SRCH_NON_COH_INT_LEN = 30 */
#define SRCHCM_STEP1_PEAK_ENERGY_THRESH_HIGH 1260


/* minimum step 1 PSC peak to LIST search PN pos PSC position seperation required
   This is refered to as D-chips in systems memo */
#define SRCHCM_STEP1_LIST_SRCH_PEAK_PSC_PN_SEP_CX8 32

/* CM PN search parameters */
/* coherent int len (31 + 1) * 64 = 2048 Cx1 */
#define SRCHCM_PN_SRCH_NC               31
#define SRCHCM_PN_SRCH_NN               0
#define SRCHCM_RECONF_SRCH_NN           1

/* Index into table to determine no TD threshold. Idx 44 means -22db */
#define SRCHCM_PN_SRCH_NO_TD_THRESH_IDX 44
/* Index into table to determine TD threshold. Idx 42 means -21db */
#define SRCHCM_PN_SRCH_TD_THRESH_IDX    42
/* Look up table */
#define SRCHCM_PN_SRCH_EGY_THRESH(sttd) ((sttd) ? SRCH_GET_PN_ENG(SRCHCM_PN_SRCH_TD_THRESH_IDX): \
                                                  SRCH_GET_PN_ENG(SRCHCM_PN_SRCH_NO_TD_THRESH_IDX))
                                                  
/* Early abort threshold for no TD search is 40 or -20db */
#define SRCHCM_PN_SRCH_EARLY_ABRT_NO_TD_THRESH_IDX 40
/* Early abort threshold for TD search is 39 or -19.5 db */
#define SRCHCM_PN_SRCH_EARLY_ABRT_TD_THRESH_IDX 39
/* Look up table */
#define SRCHCM_PN_SRCH_EARLY_ABRT_EGY_THRES(sttd) ((sttd) ? SRCH_GET_PN_ENG(SRCHCM_PN_SRCH_EARLY_ABRT_TD_THRESH_IDX): \
                                                            SRCH_GET_PN_ENG(SRCHCM_PN_SRCH_EARLY_ABRT_NO_TD_THRESH_IDX))

/* Threshold for detected pn noise floor (Reconfirmation) */
/* Index into table to determine TD threshold. Idx 44 means -22db */
#define SRCHCM_STTD_DETECTED_PN_SRCH_NOISE_FLOOR_IDX_LOW 44
/* Index into table to determine TD threshold. Idx 42 means -21db */
#define SRCHCM_STTD_DETECTED_PN_SRCH_NOISE_FLOOR_IDX_MED 42
/* Index into table to determine TD threshold. Idx 40 means -20db */
#define SRCHCM_STTD_DETECTED_PN_SRCH_NOISE_FLOOR_IDX_HIGH 40
/* Index into table to determine Non TD threshold. Idx 46 means -23db */
#define SRCHCM_NOTD_DETECTED_PN_SRCH_NOISE_FLOOR_IDX 46
/* Look up table */
#define SRCHCM_DETECTED_PN_SRCH_NOISE_FLOOR(sttd, idx) SRCH_GET_ENG_FRM_ECIO_LUT(sttd,idx)

  /* CM search state machine related macros */
  /* -------------------------------------- */
/* |----------------|_________________//________________|----------------
                    <---> 1024 Chips Tx continue
                                                    <---> 1024 chips possible Rx start
                         <---> Interf RF Tune in
                              <---> Sample collection
                                   <---------------> offline searches
                                               <---> Interf RF Tune out
                                               (tuning can occur in parallel with offline searches)
                              <======//=======> Usable interf search gap */
#define SRCHCM_INTERF_USABLE_GAP_CX8(num_gap_slots) \
  (((num_gap_slots) * CHIP_PER_SLOT * 8) - (1024 * 8 * 2) \
   - ((int32)mcalwcdma_cm_w2w_tune_away_time_cx8)  \
   - ((int32)mcalwcdma_cm_w2w_samples_collection_time_cx8))
   /* note: 1) casted as int32 so arithmetic happens as integers, not unsigned
            2) Interf RF Tune out is not considered. Instead, WFW needs time
               to collect the samples. Samples processing can be used when
               RF is tuned back.
   */

/* Number of slots usable for an interf measurement */
#define SRCHCM_INTERF_USABLE_GAP_SLOTS(num_gap_slots) \
  (SRCHCM_INTERF_USABLE_GAP_CX8(num_gap_slots) / (8 * CHIP_PER_SLOT))

#define SRCHCM_STEP1_NN_IN_GAP(num_gap_slots) (SRCHCM_INTERF_USABLE_GAP_SLOTS(num_gap_slots))

/* Number of PCS peaks to process in list serch tasks */
#define SRCHCM_MAX_STEP1_PEAKS_TO_PROC 32

/* How much stuff can be computed in one slot's worth of time at a given clock 
   speed. Higher clock speeds give higher numbers because more stuff can be 
   accomplished in one slot's worth of time).  Not sure what the units are. 
   Used for calculating number of tasks we can schedule per slot.*/
#define SRCHCM_COMP_UNITS_AVAIL_PER_SLOT_L_CLK  1440
#define SRCHCM_COMP_UNITS_AVAIL_PER_SLOT_M_CLK  2160
#define SRCHCM_COMP_UNITS_AVAIL_PER_SLOT_H_CLK  2700

/* PN SEARCH TASK SCHEDULING DEFINES AND MACROS */
#define SRCHCM_PN_SRCH_WINSIZE_CX1 (192)
#define SRCHCM_COST_PER_WFW_PN_SRCH_TASK (223)
#define SRCHCM_PN_SRCH_MAX_TASK_1_NN_LOW_CLK(num_gap_slots) \
  (SRCHCM_INTERF_USABLE_GAP_SLOTS(num_gap_slots) * \
  (FLOOR(SRCHCM_COMP_UNITS_AVAIL_PER_SLOT_L_CLK, SRCHCM_COST_PER_WFW_PN_SRCH_TASK)))
#define SRCHCM_PN_SRCH_MAX_TASK_1_NN_MED_CLK(num_gap_slots) \
  (SRCHCM_INTERF_USABLE_GAP_SLOTS(num_gap_slots) * \
  (FLOOR(SRCHCM_COMP_UNITS_AVAIL_PER_SLOT_M_CLK, SRCHCM_COST_PER_WFW_PN_SRCH_TASK)))
#define SRCHCM_PN_SRCH_MAX_TASK_1_NN_HIGH_CLK(num_gap_slots) \
  (SRCHCM_INTERF_USABLE_GAP_SLOTS(num_gap_slots) * \
  (FLOOR(SRCHCM_COMP_UNITS_AVAIL_PER_SLOT_H_CLK, SRCHCM_COST_PER_WFW_PN_SRCH_TASK)))

/* RECONF SEARCH TASK SCHEDULING DEFINES AND MACROS */
#define SRCHCM_RECONF_GAP_SIZE_CUTTOFF_FOR_WINSIZE 7  /* in units of slots */
#define SRCHCM_RECONF_NORMAL_WINSIZE_CX1 (192)
#define SRCHCM_RECONF_SMALL_WINSIZE_CX1 (128)
#define SRCHCM_RECONF_SRCH_WINSIZE_CX1(num_gap_slots) \
  (((num_gap_slots) <= SRCHCM_RECONF_GAP_SIZE_CUTTOFF_FOR_WINSIZE) ? \
   SRCHCM_RECONF_SMALL_WINSIZE_CX1 : SRCHCM_RECONF_NORMAL_WINSIZE_CX1)
#define SRCHCM_COST_PER_WFW_RECONF_SRCH_TASK(winsize) (((winsize) == SRCHCM_RECONF_NORMAL_WINSIZE_CX1) ? (415) : (282))
#define SRCHCM_RECONF_SRCH_MAX_TASK_2_NN_LOW_CLK(num_gap_slots) \
  (SRCHCM_INTERF_USABLE_GAP_SLOTS(num_gap_slots - 1) * \
  (FLOOR(SRCHCM_COMP_UNITS_AVAIL_PER_SLOT_L_CLK, SRCHCM_COST_PER_WFW_RECONF_SRCH_TASK(SRCHCM_RECONF_SRCH_WINSIZE_CX1(num_gap_slots)))))
#define SRCHCM_RECONF_SRCH_MAX_TASK_2_NN_MED_CLK(num_gap_slots) \
  (SRCHCM_INTERF_USABLE_GAP_SLOTS(num_gap_slots - 1) * \
  (FLOOR(SRCHCM_COMP_UNITS_AVAIL_PER_SLOT_M_CLK, SRCHCM_COST_PER_WFW_RECONF_SRCH_TASK(SRCHCM_RECONF_SRCH_WINSIZE_CX1(num_gap_slots)))))
#define SRCHCM_RECONF_SRCH_MAX_TASK_2_NN_HIGH_CLK(num_gap_slots) \
  (SRCHCM_INTERF_USABLE_GAP_SLOTS(num_gap_slots - 1) * \
  (FLOOR(SRCHCM_COMP_UNITS_AVAIL_PER_SLOT_H_CLK, SRCHCM_COST_PER_WFW_RECONF_SRCH_TASK(SRCHCM_RECONF_SRCH_WINSIZE_CX1(num_gap_slots)))))

/* The list search non coherent length */
#define SRCHCM_PN_SRCH_NON_COH_INT_LEN ((SRCHCM_PN_SRCH_NN) + 1)

/* The Reconf search non coherent length */
#define SRCHCM_RECONF_SRCH_NON_COH_INT_LEN ((SRCHCM_RECONF_SRCH_NN) + 1)

/* Number of PCS hypothesis to cover for list search. This is the number
   of slots in a frame */
#define SRCHCM_LIST_SRCH_MAX_PSC_HYPO 15

/* Num of PN peaks per PSC. each PSC has a max 15 tasks, each task has 6 peaks */
#define SRCHCM_NUM_PN_PEAKS_PER_PSC (SRCHCM_LIST_SRCH_MAX_PSC_HYPO) * (SRCHCM_NUM_PN_PEAKS_PER_TASK)

  /* CM search driver related macros */
  /* ------------------------------- */
/* Non coherent search state for a search sub-task. */
#define SRCHCM_SRCH_NON_COH_INT_START 0x01
#define SRCHCM_SRCH_NON_COH_INT_CONT  0x02
#define SRCHCM_SRCH_NON_COH_INT_END   0x04

/* Maximum STEP 1 search peaks reported */
#define SRCHCM_STEP1_MAX_PEAKS SRCH_MAX_NUM_STEP1_PEAKS
  
/* Inter RAT measurement related macro */
/* ----------------------------------- */
#define SRCHCM_GSM_MDSP_CMD_FTSM_QS  FRAME_TICK_SETUP_MARGIN_QS
/* number of SCH decode cmd in a GAP */
#define SRCHCM_GSM_SCH_DEC_CMD_MAX_PER_GAP 2

#ifndef MAX_NUM_ANTENNA
#define MAX_NUM_ANTENNA 2
#endif

#define SRCHCM_MAX_PILOT_PEAKS SRCH_PN_NUM_RESULT_PER_TASK

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* CM search state machine related enum definitions */
/* ------------------------------------------------ */
typedef enum
{
  /* "0x00": Peak Detection enabled. */
  SRCHCM_PD_NORMAL,
  /* "0x01": Peak Detection Disabled. */
  SRCHCM_PD_BYPASS,
  /* "0x02": used for DRX mode search. */
  SRCHCM_PD_SHOULDER
} srchcm_pd_enum_type;

/* Alignment for coherent integration */
typedef enum
{
  /* "0x00": No alignment needed */
  SRCHCM_ALIGN_NONE,
  /* "0x01": 256-chip alignment */
  SRCHCM_ALIGN_256,
  /* "0x02": 512-chip alignment */
  SRCHCM_ALIGN_512,
  /* "0x03": 1024-chip alignment */
  SRCHCM_ALIGN_1024
} srchcm_align_enum_type;


/* CM search driver related structure definitions */
/* ---------------------------------------------- */
/* This structure defines the PCS peak parameters */
typedef struct
{
  /* Step 1 PCS prak energy */
  uint16 egy;
  /* step 1 PCS peak position 0..2560x8 */
  uint16 pcs_pos_cx8;
  /* indicate if this peak has got the cell identified in list search */
  boolean cell_exists;
} srchcm_step1_srch_peak_struct_type;

/* This structure defines the Step 1 peak result */
typedef struct
{
  /* the number of non coh int length performed in the step1 search */
  uint8 num_nn_completed;
  /* number of peaks reported by step1 result */
  uint8 num_peaks;
  /* number of un identified peaks */
  uint8 num_peaks_no_cell_exists;
  /* Each step 1 peak result */
  srchcm_step1_srch_peak_struct_type peak[SRCHCM_STEP1_MAX_PEAKS];
  /* RRSI at step 1 done */
  int16 rssi_at_step1_dbmx10;
} srchcm_step1_srch_result_struct_type;

/* This structure defines the list search peak result parameters */
typedef struct
{
  /* PSC index scheduled to the search to determine the PSC of the each peak */
  uint8 psc_index;
  /* pn search peak energy */
  uint16 egy;
  /* PN position for pn search task */
  uint32 pn_pos_cx8;
} srchcm_pn_srch_peak_struct_type;

/* This structure defines all PN search peak result */
typedef struct
{
  /* number of the tasks finished in the PN search */
  uint8 num_tasks;
  /* result of peak parameter for all tasks. Each tasks report the
     number of peaks as per the async parameter programed in to mDSP */
  srchcm_pn_srch_peak_struct_type peak[SRCHCM_PN_SRCH_MAX_TASK][SRCHCM_MAX_PILOT_PEAKS];
  srchcm_pn_srch_peak_struct_type peak_div[SRCHCM_PN_SRCH_MAX_TASK][SRCHCM_MAX_PILOT_PEAKS];
  boolean  srch_result_valid[MAX_NUM_ANTENNA];
} srchcm_pn_srch_result_struct_type;

typedef void (*SRCHCM_SRCH_DONE_CB_FUNC_TYPE)(uint8);


/* Measurement purpose for GSM cells */
typedef enum
{
  SRCHCMDRV_GSM_RSSI_SCAN,
  SRCHCMDRV_GSM_BSIC_IDENTIFY,
  SRCHCMDRV_GSM_BSIC_RECONF
} srchcmdrv_gsm_meas_purpose_enum_type;

/* band list update action */
typedef enum
{
  SRCHCMDRV_BANDLIST_RESET,
  SRCHCMDRV_BANDLIST_ADD,
  SRCHCMDRV_BANDLIST_RESET_AND_ADD,
  SRCHCMDRV_BADNLIST_VALID
} srchcmdrv_bandlist_update_action_enum_type;

typedef struct
{
  /* start frame number at which pwr scan can be started */
  uint8 start_frame_num;
  /* This is the starting offset in the start frame */
  uint16 start_offset_qs;
  /* window size in which the power scan can be done.
     Note that this can be more than 1 frame length */
  uint16 win_size_qs;
} srchcm_gsm_pwr_scan_params_struct_type;

typedef struct
{
  /* cell channel frequency */
  W_ARFCN_T arfcn;
  /* timing valid. If valid then SCH decode otherwise FCCH detect */
  boolean timing_valid;
  /* first GSM frame number in which to start FCCH detect or SCH decode*/
  uint8 first_frame;
  /* offset in frame where to start SCH decode or FCCH detect*/
  uint16 offset_qs;
  /* for SCH decode */
  /* frequency offset for this cell from the fcch tone detect */
  int16 frequency_offset;
  /* for FCCH detect */
  /* window size for FCCH detect */
  uint16 fcch_win_size;
  /* for SCH decode */
  /* number of frame available for SCH decode */
  uint8 num_frames;
  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  sys_modem_as_id_e_type as_id;
  #endif
} srchcm_gsm_bsic_id_params_struct_type;

typedef struct
{
  /* cell channel frequency */
  W_ARFCN_T arfcn;
  /* GSM frame number in which to do SCH decode*/
  uint8 sch_decode_fr_num;
  /* offset in frame where to start SCH decode */
  uint16 sch_decode_qs_offset;
  /* frequency offset for the SCH decode */
  int16 frequency_offset;
  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  sys_modem_as_id_e_type as_id;
  #endif
} srchcm_gsm_bsic_reconf_params_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
  /* number of peaks configured per task for the PN srch.
      for pd=normal --->  6 peaks (non RxD,combined)  12 peaks (RxD separate)
      for pd=shoulder --->  6*3 peaks (non RxD,combined)  12*3 peaks (RxD separate)    
  */
extern uint16 srchcm_pn_peaks_per_task;

/* Call back function for CM step 1 result proceesing. This can
   be set to NULL if no processing is required */
extern SRCHCM_SRCH_DONE_CB_FUNC_TYPE srch_cm_step1_done_cb_func[MCALWCDMA_SRCH_CM_MAX_NUM_CMQ];
/* This is the call back function for list search result processing.
   This can be set to NULL if not processing is required */
extern SRCHCM_SRCH_DONE_CB_FUNC_TYPE srch_cm_list_srch_done_cb_func[MCALWCDMA_SRCH_CM_MAX_NUM_CMQ];

/* CM step 1 search results */
extern srchcm_step1_srch_result_struct_type *srchcm_step1_srch_result;

/* Result from the CM list search (PSC_ACQ/RECONF LIST SRCH)  */
extern srchcm_pn_srch_result_struct_type srchcm_pn_srch_result;

typedef void SRCHCM_CLEANUP_CB_FUNC_TYPE(void);
/* The cb srchcmdrv should call in cleanup scenarios where srchcmmeas is 
   waiting for all configured/ongoing searches in FW to be done.  Whether this
   is NULL or not also determines whether srchcmmeas is waiting.  (This is set 
   directly by srchcmmeas. */
extern SRCHCM_CLEANUP_CB_FUNC_TYPE *srchcm_cleanup_pending_cb;

/* variables for interf logging */
/* ---------------------------- */
extern L1_WCDMA_INTERF_STEP1_SRCH_LOG_PKT_type srchcm_interf_step1_log_pkt[MCALWCDMA_SRCH_CM_MAX_NUM_CMQ];
extern L1_WCDMA_INTERF_LIST_SRCH_LOG_PKT_type *srchcm_interf_list_srch_log_pkt[MCALWCDMA_SRCH_CM_MAX_NUM_CMQ];
extern uint8 srchcm_interf_list_srch_log_pkt_buf_idx[MCALWCDMA_SRCH_CM_MAX_NUM_CMQ];
extern boolean srchcm_interf_srch_log_pkt_in_use[MCALWCDMA_SRCH_CM_MAX_NUM_CMQ];
extern L1_WCDMA_INTERF_LIST_SRCH_LOG_PKT_type *srchcm_interf_list_srch_log_pkt_buf_ptr[MCALWCDMA_SRCH_CM_MAX_NUM_CMQ];
extern L1_WCDMA_INTERF_LIST_SRCH_LOG_PKT_type *srchcm_interf_cur_list_srch_log_pkt;

/* Log the num of pwr scan cmds before SCH window */
extern uint8 srchcm_gsm_num_pwr_scan_cmds_before_sch;

extern sys_band_class_e_type srchcm_gsm_band_info_list[MCALWCDMA_MAX_NUM_BAND];
/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*===========================================================================
FUNCTION srchcm_state_init

DESCRIPTION
  This function initializes the CM state machine state related variables.
  For inter frequency search it does the following:
  - Clear the search paneind flag.
  - Set the non coherent length for step 1 done to 0
  - Set the step 1 and list search done call back function to NULL
  - Set the inter frequency search state to IDLE
  - Initialize the mDSP async writes for step 1 search energy threshold
    and list search peaks.

  Note that list search related variables are initialized when inter frequency
  search state transitions from STEP1_DONE to LIST SEARCH
  This function should be called at the begining of DCH state.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_drv_init(void);

/*===========================================================================
FUNCTION srchcm_async_parm_init

DESCRIPTION
  This function initializes the async parameters for step1/pn search
  to be issued for inter freq search i.e using CM queues 0 and 1

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_async_parm_init( void );

/*===========================================================================
FUNCTION srchcm_step1_cmd

DESCRIPTION
  This function writes the asynchronous variables to mDSP for CM step 1 search.
  It also sets the Step 1 done call back function and then sends the CM step 1
  command to mdsp sync interface.

DEPENDENCIES
  None

RETURN VALUE
  Q index used for step 1

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 srchcm_step1_cmd(
  /* non coherent int length */
  uint8 non_coh_int_len,
  /* non coherent int state */
  uint8 non_coh_int_state_bf,
  /* call back function for step 1 done */
  SRCHCM_SRCH_DONE_CB_FUNC_TYPE step1_done_handler);

/*===========================================================================
FUNCTION srchcm_list_srch_set_srch_params

DESCRIPTION
  This function sets various list search parameters based on the input
  parameters passed in function argument. This is for list search peaks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void srchcm_pn_srch_set_srch_params(
  /* The search window size of the task, in chipx1 */
  uint16 search_winsize_cx1,
  /* number of list search task */
  uint8 num_pn_search_task,
  /* cell scrambling code list */
  uint16 *scr_code,
  /* PCS position from step1 result */
  uint32 *pn_pos_cx8,
  /* list search STTD enable */
  boolean *pn_srch_sttd,
  /* non coherent accumulation start.end state bit field */
  uint8 pn_srch_non_coh_int_state_bf,
  /* Non Coherent Integ length */
  uint16 nn_int_len);

/*===========================================================================
FUNCTION srchcm_pn_srch_cmd

DESCRIPTION
  This function writes the list search parameter to mDSP. To do this it gets
  the free Q for list seqrch parameters. then it writes the parameters to this
  Q and then sends the CM list search command to mdsp sync interface.

DEPENDENCIES
  None

RETURN VALUE
  Q index used for list search

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 srchcm_pn_srch_cmd(
  /* list search done call back function */
  SRCHCM_SRCH_DONE_CB_FUNC_TYPE pn_srch_done_handler);

/*===========================================================================
FUNCTION srchcm_get_step1_srch_result

DESCRIPTION
  This function gets the CM step 1 results from the result Q buffer passed as
  function argument and save them in global variable srchcm_step1_srch_result.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_get_step1_srch_result(
  /* CM result Q index */
  uint8 step1_queue_idx);

/*===========================================================================
FUNCTION srchcm_get_list_srch_result

DESCRIPTION
  This function gets the CM list search results from the result Q buffer passed
  as function argument and save them in global variable srchcm_list_srch_result.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_get_pn_srch_result(
  /* CM result Q index */
  uint8 pn_queue_idx);

/*===========================================================================

FUNCTION SRCHCM_MCAL_WHL1_STEP1_SRCH_STATUS_CB

DESCRIPTION:
  - callback function registered with MCAL and will be called by MCAL
     - on CMD_DONE interrupt
     -on SRCHCM_DONE interrupt


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_mcal_whl1_step1_srch_status_cb(
  /* Status returned by mDSP */
  mcalwcdma_srch_status_type status,
  /* Queue in which this callback is coming for */
  uint8 queue_idx);

/*===========================================================================

FUNCTION SRCH_MCAL_WHL1_PN_SRCH_STATUS_CB

DESCRIPTION:
  - callback function registered with MCAL and will be called by MCAL
     - on CMD_DONE interrupt
     -on SRCHCM_DONE interrupt


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_mcal_whl1_pn_srch_status_cb(
  /* Status returned by mDSP */
  mcalwcdma_srch_status_type status, 
  /* Queue in which this callback is coming for */
  uint8 queue_idx);

/*===========================================================================
FUNCTION srchcm_interf_get_rssi_dbmx10

DESCRIPTION
  This functin calculates the RSSI inside the CM Gap in dBm10 for Taxis.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern int16 srchcm_interf_get_rssi_dbmx10(void);

/*===========================================================================
FUNCTION srchcm_update_cm_startup_cmd_params

DESCRIPTION
  This function updates the CM startup parameters. It puts the CM start up
  command SRCHCM_GSM_CM_START_UP_TIME_QS before the gap startup time. It
  also adjusts the command frame and QS offset based of GSM FTSM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_update_cm_startup_cmd_params(
  /* gap parameters in GSM time units */
  schcm_cm_gap_param_gsm_struct_type *gap_param_for_gsm,
  /* CM startup command parameters */
  srchcm_gsm_startup_cmd_param_struct_type *startup_cmd_params
  #ifdef FEATURE_DUAL_SIM
  , sys_modem_as_id_e_type as_id
  #endif
  );

/*===========================================================================
FUNCTION srchcm_update_cm_cleanup_cmd_params

DESCRIPTION
  This function updates the CM cleanup parameters. It set the cleanup command
  to be sent at the clean up GFN passed to the function and adjust the command
  frame and QS offset for FTMS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_update_cm_cleanup_cmd_params(
  /* GFN at which cleanup command should go  */
  uint8 cleanup_frame_num,
  /* CM cleanup offset. This can be larger than QS per frame */
  uint16 cleanup_offset,
  /* clean up command parameters */
  srchcm_gsm_cleanup_cmd_param_struct_type *cleanup_cmd_params
  #ifdef FEATURE_DUAL_SIM
  , sys_modem_as_id_e_type as_id
  #endif
  );

/*===========================================================================
FUNCTION srchcm_interf_query_cfgd_srchs_still_ongoing

DESCRIPTION
  This function indicates whether individual (per-gap) searches are still 
  configured and ongoing in FW.  Currently it's intended for use during cleanup
  of W2W module, as a way to tell when it's safe to say W2W is cleaned up.

DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating whether configured searches are still in progress

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srchcm_interf_query_cfgd_srchs_still_ongoing(void);



/*===========================================================================
FUNCTION srchcm_calculate_num_gsm_rssi_meas

DESCRIPTION
  This function figures out how many rssi burst measurements can be performed 
  in a gap.

DEPENDENCIES
  None

RETURN VALUE
  The number of bursts we can do in the gap.

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 srchcm_calculate_num_gsm_rssi_meas(
  /* The qs offset to the start of the search gap, with respect to the previous GSM frame boundary */
  uint16 pwr_meas_start_offset_qs, 
  /* the size of the search window in qs */
  uint16 gap_win_size_qs);

/*===========================================================================
FUNCTION srchcm_update_gsm_rssi_cmd_params

DESCRIPTION
  This function update the RSSI scan command buffre. The required number of RSSI
  scan may span multiple GSM frames. This function checks the location of each
  burst in GAP and put the burst in their respective command buffer.

DEPENDENCIES
  None

RETURN VALUE
  Number of RSSI scan command updated.

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 srchcm_update_gsm_rssi_cmd_params(
  /* Number of burst to be programmed in this GAP */
  uint8 *num_bursts,
  /* RSSI scan measurement parameters info */
  srchcm_gsm_pwr_scan_params_struct_type *pwr_scan_params,
  /* RSSI scan GSM command parameters */
  srchcm_gsm_pwr_scan_cmd_param_struct_type *pwr_cmd_params,
  /* This gap is used for BSIC ID (SCH) or BSIC RC */
  boolean is_bsic_gap
  #ifdef FEATURE_DUAL_SIM
  , sys_modem_as_id_e_type as_id
  #endif
  );



/*===========================================================================
FUNCTION srchcm_update_gsm_bsic_id_acq_cmd_params

DESCRIPTION
  This function sets the FCCH ACQ parameters. It takes ACQ start frame and
  ACQ window size for FCCH detect and formulates the ACQ start and stop command.
  ACQ start connabd is set for doing ACQ at the start of ACQ window and ACQ stop
  is at the end of ACQ window.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_update_gsm_bsic_id_acq_cmd_params(
  /* BSIC Id parameters */
  srchcm_gsm_bsic_id_params_struct_type *bsic_id_params,
  /* FCCH ACQ start cmd params */
  srchcm_gsm_start_acq_cmd_param_struct_type *acq_start_cmd_params,
  /* FCCH ACQ stop cmd params */
  srchcm_gsm_stop_acq_cmd_param_struct_type *acq_stop_cmd_params);

/*===========================================================================
FUNCTION srchcm_update_gsm_bsic_id_sch_dec_cmd_params

DESCRIPTION
  This function sets the SCH decode command parameter.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_update_gsm_bsic_id_sch_dec_cmd_params(
  /* BSIC Id parameters */
  srchcm_gsm_bsic_id_params_struct_type *bsic_id_params,
  /* SCH decode cmd params */
  srchcm_gsm_sch_decode_cmd_param_struct_type *sch_decode_params,
  /* sch decode command index */
  uint8 command_idx);



/*===========================================================================
FUNCTION srchcm_update_gsm_bsic_reconf_sch_dec_cmd_params

DESCRIPTION
  This function sets the SCH decode command parameter for BSIC reconf.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcm_update_gsm_bsic_reconf_sch_dec_cmd_params(
  /* BSIC Id parameters */
  srchcm_gsm_bsic_reconf_params_struct_type *bsic_reconf_params,
  /* SCH decode cmd params */
  srchcm_gsm_sch_decode_cmd_param_struct_type *sch_decode_params);

/*===========================================================================
FUNCTION srchcm_update_gsm_band_info

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchcm_update_gsm_band_info_list(
  srchcmdrv_bandlist_update_action_enum_type action,
  W_ARFCN_T *arfcn);

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

#endif /* SRCHCMDRV_H */

