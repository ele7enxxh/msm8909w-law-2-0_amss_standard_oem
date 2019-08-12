#ifndef SRCHACQ_H
#define SRCHACQ_H

/*===========================================================================

       EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchpch.c_v   1.12   11 Jul 2002 21:20:38   jdatar  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/srchacq.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/08/16   rn      Changes to implement the algorithm to optimize the performance for 
                   x2w resume acq post LTA cases
03/02/15   da      Changes to support idle-resume scenarios
02/11/15   da      Fix to ensure six unique step1 peaks are picked for ABPLMN acq and
                   changed the incorrect conditional logic for checking the other hypothesis 
                   during NASTT failure in ACQ state.
01/21/15   da      Changes to read NV 5458 only once during wl1m_init()
10/29/14   vn      NBR FET coexistence changes.
08/23/14   bj      Added split acq across DRX support in BPLMN state.
07/15/14   bj      WCDMA Narrow Band Rejection for System Search.
06/17/14   jd      Use pn pos from rrc in cx8 during x2w resel bplmn.
06/05/14   jd      Fix to not use freq info from acq req command during x2w resume.
05/15/14   km      Srch API to identify power up acq
05/09/14   km      New srchacq api to retrieve freq field from srchacq db
05/07/14   km      Pn position should be 32bit
04/16/14   bj      Split ACQ code cleanup and BPLMN split ACQ code changes
04/14/14   km      Fix compilation errors
12/16/13   as      Porting G2W TA from Triton to Dime.
12/16/13   bj      split ACQ feature Porting changes.
11/27/13   jd      Fix to convert pn position to cx8 only during rat switch acq.
11/25/13   da      Enabled 6 step 1 peaks to be picked up for srchacq type L1_ABPLMN_SRCH_TYPE
11/12/13   jd      Fix to reset sample server freeze flag after acquisition failure.
11/11/13   gj      Converting reference position from cx8 to cx1 while sending acq confirmation to RRC
10/31/13   jd      Fix to update pn pos of the cell in cx8 during limited window pn .

===========================================================================*/

#include "srchsched.h"
#include "comdef.h"
#if defined FEATURE_QSH_DUMP || defined FEATURE_QSH_EVENT_METRIC
#include "wl1_qsh_ext.h"
#endif /* defined FEATURE_QSH_DUMP || defined FEATURE_QSH_EVENT_METRIC*/

/*=============================================================================
   PREPROCESSOR MACROS
  ===========================================================================*/
#define MAX_PEAKS_FOR_NASTT             6
#define MAX_STEP1_PEAKS_FOR_POWER_UP    2
/* this is the max step 1 peaks to be picked up for search type ABPLMN */
#define MAX_STEP1_PEAKS_ACQ_ABPLMN_TYPE 6 

/* The maximum difference between two parallel frequency hypotheses, in ppm
   x100 */
#define MAX_FREQ_ERROR_DIFFERENCE 700

#ifdef FEATURE_WCDMA_TCXO_RESOURCE_SUPPORT
/* If a VCTCXO freeze is in effect (like by GPS), this is the maximum variation
** we will tolerate without slamming a new trk_lo.  This mostly affects OOS,
** where we ask TCXO MGR for a new trk_lo at each acquisition attempt.  Small
** temperature variations can result in different trk_lo values returned.
** Instead of forcing GPS to lose its freeze, just don't bother using the new
** trk_lo if it's not much different from the old one.  Going with a
** conservative 0.25 ppm here: 2.14 GHz (DL freq) * 0.25 ppm = 535 Hz.  mDSP
** resolution is 7.3242 Hz.  535 / 7.3242 ~= 73. */
#define TRK_LO_PDM_DELTA_MAX  73
#endif /* FEATURE_WCDMA_TCXO_RESOURCE_SUPPORT */

#define SRCHACQ_VALID_COUNT_FOR_SECOND_STEP2 2

/* Check if this acquisition mode is for power-up or OOS and if srchacq_type is ABPLMN*/
#define SRCHACQ_POWER_UP_OOS_ACQ_MODE(srchacq_db) \
          (((srchacq_db.mode == L1_WCDMA_FULL) || \
           (srchacq_db.mode == L1_WCDMA_ACQ_DET_LIST)) && \
           (srchacq_db.srchacq_type != L1_ABPLMN_SRCH_TYPE))

/* Check if a second step2 search is required */
#define SRCHACQ_LIMITED_SERVICE_SRCH_SECOND_STEP2_REQD(step3_task_cnt)    \
         ((srch_step1_peak_cnt > SRCHACQ_VALID_COUNT_FOR_SECOND_STEP2) && \
          (step3_task_cnt < SRCH_STEP3_MAX_TASKS)                      && \
          (srchsched_db.step1_nastt_params.cur_psc_peak_indx[CARR_0] <= SRCHACQ_VALID_COUNT_FOR_SECOND_STEP2))

/* How many search strategies to cycle through before we start over again. */
#define SRCHACQ_BIN_TABLE_ENTRIES         2

/* How long to run through all search strategies before we start over again. */
#define SRCHACQ_BIN_TABLE_DURATION_SECS  14400

/* Number of attempts before ACQ finally fails */
#define SRCHACQ_MAX_ATTEMPT_CNT 4

/* Convert PN postion provided in cx1 to cx8 if acq type is rat switch. For other acq types, position is provided in cx8 */
#define SRCHACQ_GET_CX8_PN_POS(pn_pos) ( ((srchacq_db.proc == SRCHACQPROC_X2W_RESEL_HO) && (srchacq_db.cmd.srch!= L1_BPLMN_SRCH_TYPE)) ? (pn_pos * 8) : pn_pos)


/*=============================================================================
   MODULE-SPECIFIC TYPE DEFINITIONS
  ===========================================================================*/
/* Supported high-level acquisition procedures */
typedef enum
{
  SRCHACQPROC_POWERUP,
  SRCHACQPROC_SPLIT_ACQ_POWERUP,
  SRCHACQPROC_HHO,
  SRCHACQPROC_X2W_RESUME,
  SRCHACQPROC_X2W_RESEL_HO,
  SRCHACQPROC_FACH_DCH_TO_IDLE,
  SRCHACQPROC_BPLMN,
  SRCHACQPROC_SPLIT_ACQ_BPLMN,
  SRCHACQPROC_BPLMN_KNOWN_PN_POS,
  SRCHACQPROC_SPLIT_ACQ_BPLMN_RESUME,
  SRCHACQPROC_MAX
} srchacqproc_enum_type;

typedef enum
{
  SRCHACQ_STEP_STEP1_NASTT,               /* Step 1 followed by NAS-TT search                 */
  SRCHACQ_STEP_STEP1,                     /* Step 1 search                                              */    
  SRCHACQ_STEP_NASTT,                     /* NAS-TT search                                            */
  SRCHACQ_STEP_PN,                        /* PN search, either limited or full window          */
  SRCHACQ_STEP_PULLIN,                    /* Pullin                                                         */
  SRCHACQ_STEP_FRAME_AMBIG_RES,           /* Frame ambiguity resolution and TxD detection */
  SRCHACQ_STEP_LOCK_EXTEND,               /* Lock Extension                                             */  
  SRCHACQ_STEP_PCCPCH_SETUP_CONT,         /* PCCPCH Setup and TxD detection                  */
  SRCHACQ_STEP_PCCPCH_SETUP_AFTER_GAP,    /* PCCPCH Setup and TxD detection after GAP    */
  SRCHACQ_STEP_MAX
} srchacq_step_enum_type;

typedef struct
{
  uint16 scr_code;

  /* indicates if the initial limited window search is pass/fail */
  boolean list_search_success;
} srchacq_resume_set_struct_type;

typedef struct
{
  uint32 max_pos[SRCH_PN_NUM_RESULT_PER_TASK];
  uint32 max_eng[SRCH_PN_NUM_RESULT_PER_TASK];
  uint32 max_pos_div[SRCH_PN_NUM_RESULT_PER_TASK];
  uint32 max_eng_div[SRCH_PN_NUM_RESULT_PER_TASK];
  boolean eng_valid[MAX_NUM_ANTENNA];
  boolean sttd;
} srchacq_temp_result_struct_type;

typedef struct
{
  uint32 max_eng;
  uint32 max_pos;
} srchacq_cell_peak_struct_type;

/* Different search strategies to use during extended OOS recovery.
** Sometimes ignore RGS and/or temperature table during acq attempts. */
typedef enum
{
  SRCHACQ_BIN_NORMAL,
  SRCHACQ_BIN_IGNORE_RGS,
  SRCHACQ_BIN_IGNORE_RGS_AND_TEMP_TABLE,
  SRCHACQ_BIN_NUM_TYPES
} srchacq_bin_type;

/* This feature kicks in during extended OOS.  Its goal is to recover from
 * bad TCXO Manager RGS (Recent Good System) and trk_lo vs. temperature table
 * data.  It employs different combinations of the following three techniques:
 * (1) Ignore RGS.
 * (2) Ignore RGS and trk_lo vs. temperature table.
 * (3) Widen frequency search bins.
 *
 * For now expanded frequency bin search feature only works with TCXO
 * Manager 3.0, and not the older versions.
 *
 * For now this doesn't work for XO, only TCXO targets.
 *
 * Define an expanded frequency bin search entry with this structure:
 *   bin_type - SRCHACQ_BIN_NORMAL, SRCHACQ_BIN_IGNORE_RGS, or
 *              SRCHACQ_BIN_IGNORE_RGS_AND_TEMP_TABLE.  So it's either normal
 *              (legacy) behavior, ignore RGS, or ignore both RGS and trk_lo
 *              vs. temperature table.
 *   bin_duration_sec - OOS duration to use given search strategy.  Value is
 *                      max OOS duration for given search strategy, before
 *                      moving to next entry in the table.
 *   bin_offset_ppm - Use +/- this value for frequency bins.  Units are
 *                    ppm * 100.  So 700 means 7.00 ppm.
 */
typedef struct
{
  srchacq_bin_type  bin_type;
  uint32            bin_duration_sec;
} srchacq_bin_entry_type;

/* Structure that contains the header of the RF scan
 * result buffer */
typedef struct
{
  uint16 rat_type;
  uint16 num_scan_results;
} wcdma_rf_scan_result_header;

/* Structure to report result from an RF scan */
typedef struct
{
  uint16 uarfcn;
  uint8 band;
  int8 rssi;
} wcdma_rf_scan_result;


/* The following structure will hold the freq bin data for different type of
   freq binning mechanisms. This is like a freq bin LUT and will be statically
   2 initialised with the systems recommended data. */
typedef struct
{
  /* Max number of freq bins for this type of binning mechanisms */ 
  uint32 max_bins;

  /* Freq bins in 100*ppm units for this type of mechanism */
  int16 freq_bins[SRCH_FREQ_HYPO_MAX];            
} srchacq_freq_bin_table_struct;

typedef enum
{
  /* acq start step1 - config max parallel freq hypo possible */
  SRCH_FREQ_HYPO_CONFIG_REASON_START_STEP1,

  /* subsequent step1 - config remaining freq hypo not yet step1 searched*/
  SRCH_FREQ_HYPO_CONFIG_REASON_SUBSEQUENT_STEP1,

  /* NASTT -config freq hypo from the step1 peaks */
  SRCH_FREQ_HYPO_CONFIG_REASON_START_NASTT
} srchacq_freq_hypo_config_purpose;

typedef struct
{
  /* actual freq hypo */
  int32 freq_hypo;

  srchsched_step1_nastt_status_enum_type step1_status;
  srchsched_step1_nastt_status_enum_type nastt_status;

  /* this freq hypo is programmed to which idx in fw. needed to
  associate step1 srch result to a particular freq hypo */
  uint8 fw_idx;
  /********************** 
    tcxofreqhypo values written*/
  int32 tcxofreq_hypo;
} srch_freq_hypo_struct_type;


typedef enum 
{
  FREQ_BIN_MECH_RGS_VALID,
  FREQ_BIN_MECH_175_PPM,
  FREQ_BIN_MECH_175_PPM_OOS,
  FREQ_BIN_MECH_M1,
  FREQ_BIN_MECH_M2,
  FREQ_BIN_MECH_M3,
  FREQ_BIN_MECH_M4,
  FREQ_BIN_MECH_M5,
  FREQ_BIN_MECH_M6,
  FREQ_BIN_MECH_M1_OOS,
  FREQ_BIN_MECH_M2_OOS,
  FREQ_BIN_MECH_M3_OOS,
  FREQ_BIN_MECH_M4_OOS,
  FREQ_BIN_MECH_M5_OOS,
  FREQ_BIN_MECH_M6_OOS,
  FREQ_BIN_MECH_MAX
} srch_freq_bin_mech_enum_type;

#define SRCHACQ_MAX_PARALLEL_STEP1_HYPO 2

typedef struct
{
  /* Complete frequency bin database */
  srch_freq_hypo_struct_type freq_offset[SRCH_FREQ_HYPO_MAX];
  uint8 passed_hypo_idx;
  uint8 freq_hypo_size;

  /* Indices of current hypotheses in freq_offset to search */
  uint8  cur_idx[SRCHACQ_MAX_PARALLEL_STEP1_HYPO];
  uint8 idx_size;
  
  srch_freq_bin_mech_enum_type freq_bin_mech;

  srchacq_freq_hypo_config_purpose purpose;
} srch_freq_hypo_db_struct_type;

/* Internal state information */
typedef struct
{
  /* TRUE if a procedure is ongoing, FALSE otherwise. Note that this value is
     different than checking if the procedure bitmask is nonzero, since this
     value will be set to FALSE in between acquisition procedures. */
  boolean busy;

  /* Bitmask of remaining procedures in the sequence. Bit position is defined
     by srchacq_step_enum_type */
  uint16 bmsk;


  /* Acquisition sequence being performed */
  srchacqproc_enum_type proc;

  /* The number of times the current procedure has been attempted */
  uint8 attempts;

  /* Status of the last Step 1/NAS-TT search to run in this sequence */
  srchsched_step1_nastt_status_struct_type s1_nastt_status;

  uint16 scr_code_acq_passed; // PSC that ACQ passed on

  l1_acq_cmd_type cmd;
  l1_wcdma_acq_mode_type mode; // was srchacq_mode
  l1_wcdma_acq_mech_type type; // was srchacq_mech 
  l1_wcdma_srch_enum_type srchacq_type; //Type of search requested by RRC
  int32 srchacq_cur_freq_offset;
  int16 srchacq_cur_freq_bin;

  boolean rgs_valid; // was srchacq_rgs_data_valid
  boolean powerup_full_srch_in_prog; // was srchacq_full_srch_at_pwrup_in_progress

  uint16 pullin_scr_code; // scr code that pullin passed on

  uint8 cur_cell_idx;
  uint8 num_cells;

  struct
  {
    uint16 scr_code;
    uint32 pos;
    boolean sttd;
    uint32 tot_eng;      /* might be needed in ranking (or) for filtering later to be used as prev_filt_eng  */
    int16 ecio_2x;          /* EcIo converted from tot_eng */   
    int16 rscp;          /* RSCP calculated from EcIo and RxAGC */ 
  } cell_list[L1_WCDMA_ACQ_DET_LIST_MAX]; // what is the ideal size?

   boolean sttd_nw_informed;
   boolean sttd; // was srchacq_ratchange_sttd_indicator and srchacq_wcdma_hho_sttd_indicator
   uint32 pos; // was srchacq_pn_pos
  
   boolean full_service;
   boolean reset_xo; // see srch_reset_vctcxo_param_during_acquisition
   
   uint8 step; // for plt only

   srch_freq_hypo_db_struct_type freq_hypo_db;
   srchacq_bin_entry_type bin_table[SRCHACQ_BIN_TABLE_ENTRIES]; // tells which bin to use based on OOS time

   boolean use_full_win; // was srchacq_attempt_full_srch_at_pwrup, for debugging
   uint16 dbg_scr_code; // was srch_debug_scr_code

   boolean aset_dereg_pending; // was srchacq_pending_dereg_aset_cell

   /* Flag set to true only from CMI when a WCDMA Scan is to be
    * initialized */
   boolean ss_start_scan;

   /* Variable to log the current number of results reported
    *  back to CMI */
   uint16 cmi_res_cnt; // was num_logged_results

   /* Variable that determines the maximum number of results
    *  requested by CMI for a WCDMA RF Scan */
   uint16 cmi_max_res; // was wcdma_max_num_results

   /* Local array of WCDMA RF frequency scan result, was rf_scan_result */
   wcdma_rf_scan_result* rf_scan_rslt;

   /* Local WCDMA RF Scan result header, was rf_scan_result_header */
   wcdma_rf_scan_result_header* rf_scan_rslt_hdr;  

   // HHO
   boolean forward_hho;
   
   // X2W RESUME
   srchacq_resume_set_struct_type resume_reacq_set[L1_MAX_RL];
   uint16 resume_reacq_set_cnt;

   int64 rx_rot; // Needed for centering Acq search
} srchacq_db_struct_type;

boolean srch_acq_cleanup(l1m_state_enum_type);
void srchacq_hho_resume_on_cell_cfg(void);
boolean srchacq_process_hho_pn_srch(void);
void srchacq_resume_processing_failed_sttd_done(void);
int8 srchacq_sort_pullin_set_cmp_fn(const void* left, const void* right);
boolean srchacq_verify_step1_false_peak(uint16 step1_peak_cnt);
void srchacq_reset_freq_hypo_db(void);
void srchacq_update_hho_cell_pos(srch_cell_struct_type *cell_ptr, uint32 dl_expected_pn_pos);
void srchacq_unfreeze_tcxo(void);

extern srchacq_step_enum_type srchacqproc_get_step(void);

/*=============================================================================
  FUNCTION
    srchacq_handle_acq_req_cmd
  
  DESCRIPTION
    This function handles an acquisition request from RRC. It imports all
    necessary parameters and starts the appropriate procedure.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    An acquisition sequence will be started
=============================================================================*/
void srchacq_handle_acq_req_cmd(l1_acq_cmd_type* cmd);

/* Imports the RL list from the supplied ACQ command into the internal retry state */
void srchacq_retry_set_rls(const l1_acq_cmd_type* cmd);

/* Determine if forward or reverse HHO is going on */
void srchacq_update_hho_dir(void);


/*===========================================================================

FUNCTION SRCH_ACQ_PULLIN_DONE

DESCRIPTION
  This function is called by Demod module once pull-in operation is finished.

DEPENDENCIES
  None.

RETURN VALUE
  NONE.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_acq_pullin_done(dl_acq_pullin_done_cmd_type* cmd);

/* END OLD CODE */

void srchacq_init(void);

/*===========================================================================

FUNCTION SRCH_ACQ_CONFIG_MULTIPLE_FREQ_HYPO

DESCRIPTION:
  configure diffrent freq hypothesis to FW for step1
  'curr_rotator' will be passed if we get some data from tcxo etc. It
  will be used at the start of the acq search

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srchacq_config_multiple_freq_hypo(void);

boolean srchacq_start_step1_nastt(void);
void srchacq_start_nastt(void);
void srchacq_start_pn(void);
boolean srchacq_start_pullin(srch_cell_struct_type** cell_pptr, uint8 cell_cnt);
void srchacq_start_frame_ambig_res(uint16 freq, uint16 scr_code);

boolean srchacq_handle_step1_nastt_done(void);
boolean srchacq_handle_pn_done(void);
boolean srchacq_handle_pullin_done(dl_acq_pullin_done_cmd_type* cmd);
boolean srchacq_handle_frame_ambig_res_done(boolean status, boolean sttd_used);

extern srchacq_db_struct_type srchacq_db;

/*===========================================================================
  FUNCTION SRCHACQ_SET_CUR_RL_IDX
  
  DESCRIPTION
    This function sets the internal RL index (of srchacq_hho_retry_state.rls).
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
    
  SIDE EFFECTS
    None.
===========================================================================*/
extern void srchacq_set_cur_rl_idx(uint8 idx);

/*===========================================================================
  FUNCTION SRCHACQ_GET_CUR_RL_IDX

DESCRIPTION
    This function returns the index of the current RL that is attempting to
    be acquired. The index can be used with the srchacq_retry_acq_rls array.

DEPENDENCIES
    None.

RETURN VALUE
    Current RL index.

SIDE EFFECTS
    None.
===========================================================================*/
extern uint8 srchacq_get_cur_rl_idx(void);

/*===========================================================================
  FUNCTION SRCHACQ_IS_FORWARD_HHO

DESCRIPTION
    This function returns true if the current HHO state is forward, false
    if reverse.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
extern boolean srchacq_is_forward_hho(void);

extern void srchacq_set_sttd(boolean sttd);
extern void srchacq_set_mode(l1_wcdma_acq_mode_type mode);
extern void srchacq_set_pos(uint32 pos);

extern boolean srchacq_get_sttd(void);
extern uint16 srchacq_get_freq(void);
extern l1_wcdma_acq_mode_type srchacq_get_mode(void);
extern uint8 srchacq_get_attempts(void);
extern srchacqproc_enum_type srchacq_get_proc(void);
extern srch_freq_hypo_db_struct_type* srchacq_get_freq_hypo_db(void);
extern uint32 srchacq_get_pos(void);
extern boolean srchacq_get_use_full_win(void);
extern l1_wcdma_acq_mech_type srchacq_get_type(void);
extern void srchacq_prepare_nb_rej_proc(void);

/*=============================================================================
  FUNCTION
    SRCHACQ_GET_STEP
  
  DESCRIPTION
    This function determines the enum inferred by the most significant bit
    set in srchacq_db.bmsk. Since the bitmask is set once when the 
    sequence begins and bits are cleared when their corresponding procedures
    complete, this value is the procedure that is currently ongoing.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    The acquisition procedure that is currently being handled if one is
    ongoing, SRCHACQ_STEP_MAX otherwise.
  
  SIDE EFFECTS
    None
=============================================================================*/
extern srchacq_step_enum_type srchacq_get_step(void);

/*=============================================================================
  FUNCTION
    SRCHACQ_HHO_CLEANUP
  
  DESCRIPTION
    This function sets the appropriate acquisition state variables after
    at the end of hard handover.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    none
  
  SIDE EFFECTS
    None
=============================================================================*/
extern void srchacq_hho_cleanup(boolean success);

/*===========================================================================
  FUNCTION SRCHACQ_GET_TUNING_FREQ

DESCRIPTION
    This function returns the frequecy which needs to be tuned

RETURN VALUE
    uint16.

SIDE EFFECTS
    NONE.
===========================================================================*/
extern uint16 srchacq_get_tuning_freq( void );

extern void srchacq_handle_pullin_failure(void);
extern void srchacq_decfg_all(void);


/*split ACQ function declaration */

extern void srchacq_lock_extend(uint16 duration_ms);
extern void srchacq_rf_tune_proc(void);
extern void srchacq_common_preproc(void);
extern void srchacq_release_alt_client_resource(void);
extern void srchacq_release_resource(void);
extern void srchacq_request_resource(void);
extern boolean srchacq_is_split_acq_possible(l1_acq_cmd_type *cmd);
extern void srchacq_samp_serv_unfreeze(void);
extern void srchacq_notify_nastt_done(void);
extern void srchacq_action_done_cb(boolean result);
extern srchacq_step_enum_type srchacq_next_primary_step(srchacq_step_enum_type prev_step,
                                                                 boolean prim_step_result);
extern void srchacq_setup_acq_sttd_step(uint16 scr_code);
#ifdef FEATURE_DUAL_SIM
extern void srchacq_resume_processing_successful_sttd_done(void);
#endif

extern boolean srchacq_is_mode_powerup(void);
extern void srchacq_rf_tune_for_fet_cfg(void);

/*===========================================================================
  FUNCTION srchacq_nv_5458_read_psc_lock

DESCRIPTION
    This function is used to read the test NV 5458 during wl1m_init.
    The value entered from the NV will be stored in a global variable which
    will be used to determine whether we need to lock to a particular PSC
    for acquisition.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
extern void srchacq_nv_5458_read_psc_lock(void);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
 
  FUNCTION SRCHACQ_QSH_GET_ACQ_CMD_INFO
 
DESCRIPTION
    This function collects srchacq_cmd info needed for QSH module during crash dump

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/

void srchacq_qsh_get_acq_cmd_info(wl1_debug_l1_acq_cmd_type *acq_cmd_t);

/*===========================================================================
  FUNCTION SRCHACQ_QSH_DUMP_CB

DESCRIPTION
    This function collects srchacq info needed for QSH module during crash dump

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/

void srchacq_qsh_dump_cb(wl1_srcher_mini_debug_info_struct_type *addr_ptr_t);
#endif
/*===========================================================================
  FUNCTION srchacq_handle_idle_resume_pn_done

DESCRIPTION
    This function is called to handle the pn search done in idle-resume acq
    case.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
void srchacq_handle_idle_resume_pn_done(srch_cell_struct_type* cell_ptr);

/*===========================================================================
  FUNCTION srchacq_idle_resume_on_cell_cfg

DESCRIPTION
    This function is called upon cell config in idle-resume acq case.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
void srchacq_idle_resume_on_cell_cfg(void);

/*===========================================================================
 
 FUNCTION        SRCH_HANDLE_LTA_PN_DONE
 
 DESCRIPTION
   This function handles a pn search done as a part of x2w resume acq
   after LTA gap. It will check whether for the previous serving cell,
   the pullin threshold needs to lowered or we need to re-issue the
   PN search in ACQ.
 
 DEPENDENCIES
 
 RETURN VALUE
 None
 
 SIDE EFFECTS
 
 ===========================================================================*/
boolean srch_handle_lta_pn_done(srch_cell_struct_type *cell_ptr);

/*===========================================================================
 
 FUNCTION        SRCH_GET_PULLIN_THRESH_FOR_X2W_RESUME_AFTER_LTA
 
 DESCRIPTION
   This function computes the pullin thresh for all the cells during
   x2w resume acq after LTA gap.
 
 DEPENDENCIES
 
 RETURN VALUE
 uint16 - pullin_thresh
 
 SIDE EFFECTS
 
 ===========================================================================*/
uint16 srch_get_pullin_thresh_for_x2w_resume_after_lta(srch_cell_struct_type* cell_ptr);

#endif /* !SRCHACQ_H */
