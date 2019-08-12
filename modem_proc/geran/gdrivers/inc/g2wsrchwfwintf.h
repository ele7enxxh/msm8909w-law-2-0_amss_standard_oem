#ifndef G2WSRCHWFWINTF_H
#define G2WSRCHWFWINTF_H

/*===========================================================================
                 G2W Search WCDMA Firmware Interface

GENERAL DESCRIPTION
This module implements low level interface support with WCDMA firmware to
support G2W searching.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2000 - 2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/g2wsrchwfwintf.h#1 $
  $DateTime: 2016/12/13 08:00:21 $
  $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/07/14   cc      G2W direct stage 2 search
18/08/14   ks      New function to send channel ID to WFW for G2W searches
02/07/14   mc      CR.687340 : BOLT 2.0 : COEX : Mass Merge of COEX changes from DI4.0
06/01/14   ks      Bolt FED API checkin
12/08/13   og      CR522046 Configure WFW for COEX management in DSDA
06/12/12   cja     CR383165 Remove compiler warnings
31/10/12   pg      CR409233: updates for G2W on Dime modem 
03/15/12   mm      CR 344531 G2W drivers new design
26/10/11   mm      CR313554 X2W_MODE_CFG_CMD is issued only after WFW ack'ed start
04/10/11   cja     CR310265 Disable WFW after suspend WFW
08/06/11   cja     Nikel G2W changes (so WFW does startup)
01/05/11   yh      Added get function for abort status
30/11/10   og      G2W dedicated timeline fix. CR266049 
10/06/10   scm     Implement g2w_srch_abort_then_wfw_sleep() function.
06/05/09   scm     Extern g2w_srch_initiate_search().
21/04/09   scm     Extern g2w_srch_start_wfw() and small API mod.
16/04/09   nf      MUSTANG needs to use g2w_issue_command_to_fw()
15/04/09   scm     Extern g2w_srch_unfreeze_sample_server().
25/03/09   scm     Modify g2w_store_stage1/2_result() comment headers.
03/03/09   scm     Run-time variables to adjust amount of debug messaging.
30/03/09   ws      Added FEATURE_GSM_GPRS_QDSP6 and FEATURE_GSM_GPRS_POSEIDON
                   to replace FEATURE_GSM_GPRS_MUSTANG
23/01/09   cs      Added extra featurisation to allow WPLT to build
23/12/08   agv     Changed Enum define WfwG2WTypeEnum to align with new WFW.
12/12/08   scm     Implement idle and dedicated reconfirmation PN search.
10/12/08   agv     Added a new I/F for Mustang WFW.
12/10/08   scm     Extern g2w_store_stage1_result()/g2w_store_stage2_result().
04/12/08   og      Mustang G2W IRAT integration.
                   SCM Replace g2w_srch_stop_wfw() with g2w_srch_sleep_wfw().
09/25/08   scm     More mods to support FEATURE_GSM_GPRS_MUSTANG feature.
09/23/08   scm     Mods to write Stage1/2 PN search params to FW.
09/22/08   scm     More G2W FEATURE_GSM_GPRS_MUSTANG mods.
09/16/08   scm     Initial revision. 
===========================================================================*/
#include "geran_variation.h"
#include "comdef.h"
#include "queue.h"

#ifdef FEATURE_G2W_UNIT_TEST
#include "offline_test.h"
#endif

/* This file only applies to builds with inter-RAT. */
#if defined(FEATURE_INTERRAT_HANDOVER_GTOW) || defined(FEATURE_INTERRAT_CELL_RESELECTION_GTOW)

#include "g2wsrchparm.h"
#include "wfw_sw_intf.h"
#include "wfw_sw_cmd_proc_intf.h"
#include "gtmrs.h"
#include "rfcom.h"


/*=============================================================
                  DEFINITIONS
==============================================================*/
/* Maximum length of mDSP command queue */
#define G2W_MDSP_SYNC_DSP_CMD_Q_LEN_MAX 10

/* Maximum length of free command buffers queue */
#define G2W_MDSP_SYNC_FREE_CMD_BUF_Q_LEN 40

/* Max length of Trace debug buffer */
#define G2W_MDSP_SYNC_TRACE_BUF_CMD_MAX 6*2*5

/* Max number of Step1 peaks to keep */
#define G2W_SRCH_MAX_PSC_PEAKS_TO_KEEP 8

/* Max cells for ID search */
#define G2W_SRCH_MAX_ID_CELLS 32

/* Maximum number of Step 1 peaks we pass to FW for NAS-TT search. */
#define G2W_MAX_NASTT_PEAK_COUNT                6

#define G2W_SRCH_NASTT_PEAKS_PER_TASK           4

#define G2W_SRCH_MAX_NASTT_RESULTS              \
        (G2W_MAX_NASTT_PEAK_COUNT * G2W_SRCH_NASTT_PEAKS_PER_TASK)

/* Max peaks per task for PN after NAS-TT search */
#define G2W_SRCH_PN_AFTER_NASTT_PEAKS_PER_TASK  6

#define G2W_MAX_PN_AFTER_NASTT_PEAK_COUNT       6

/* Dedicated Mode Stage1 peaks per task */
#define G2W_SRCH_STAGE1_PN_PEAKS_PER_TASK       G2W_SRCH_LIST_PEAKS_PER_TASK

/* Max number of Stage1 Peaks */
#define G2W_SRCH_STAGE1_MAX_TOTAL_PEAKS         \
        (30 * G2W_SRCH_STAGE1_PN_PEAKS_PER_TASK)

/* Max tasks for Dedicated Mode Stage2 */
#define G2W_MDSP_STAGE2_NUM_TASKS 35 /* +1 may be added */

/* Sometimes extra Stage2 search task gets added on, which is why add one here.
** Number of Stage2 PN search tasks from WFW should never be bigger than this. */
#define G2W_SRCH_STAGE2_MAX_TASKS_FINISHED  (G2W_SRCH_STAGE2_NUM_TASKS + 1)

/* Max PN peaks per task in idle */
#define G2W_SRCH_NON_STAGED_PN_PEAKS_PER_TASK   6

/* Max tasks for Reconf List Search */
#define G2W_MDSP_LIST_SRCH_MAX_TASKS G2W_SRCH_LPQ_MAX_TASKS

/* Max Number of list search tasks on LPQ */
#define G2W_SRCH_LIST_SRCH_LPQ_MAX_TASKS    G2W_SRCH_LPQ_MAX_TASKS

/* Window Size for PN Search */
#define G2W_SRCH_PN_WIN_SIZE_CHIPX1           128
#define G2W_SRCH_PN_WIN_SIZE_CHIPX8          1024

/* Ec/Io Table size entries */
#define G2W_SRCH_ECIO_TAB_SIZ 64

/* Convert from chipx1 units to 64-chipx1 units, and subtract 1, because
** that's what WFW wants. */
#define G2W_SRCH_COH_INT_LEN_TO_WFW_UNITS(coh_int_len)         \
        ((coh_int_len >> 6) - 1)

/* Does opposite of previous macro. */
#define G2W_SRCH_COH_INT_LEN_TO_SW_UNITS(coh_int_len)          \
        ((coh_int_len + 1) << 6)

/* Subtract 1 from number of slots, because that's what WFW wants. */
#define G2W_SRCH_NON_COH_INT_LEN_TO_WFW_UNITS(non_coh_int_len) \
        (non_coh_int_len - 1)

/* Does opposite of previous macro. */
#define G2W_SRCH_NON_COH_INT_LEN_TO_SW_UNITS(non_coh_int_len)  \
        (non_coh_int_len + 1)

#define G2W_SRCH_STAGE1_PN_COH_INT_LEN          \
        G2W_SRCH_COH_INT_LEN_TO_WFW_UNITS(1024)
#define G2W_SRCH_STAGE1_PN_NON_COH_INT_LEN      \
        G2W_SRCH_NON_COH_INT_LEN_TO_WFW_UNITS(1)

#define G2W_SRCH_PN_COH_INT_LEN                 \
        G2W_SRCH_COH_INT_LEN_TO_WFW_UNITS(2048)
#define G2W_SRCH_PN_NON_COH_INT_LEN             \
        G2W_SRCH_NON_COH_INT_LEN_TO_WFW_UNITS(1)

/* code_group is psc >> 3 or scr >> 7   */
#define SCR_CODE_TO_CODE_GROUP(scr_code) (scr_code >> 7)

#define g2w_mdsp_start_mode_type WfwCmdStartWfwCfgEnum

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_GSM_COEX)
#define g2w_mdsp_start_mode_ue_type WfwStartCmdUeTypeEnum
#define g2w_mdsp_cxm_param_cfg_type WfwCxmParamCfgCmdStruct
#endif

/* G2W_SRCH_STAGE1_PN_NUM_TASKS must not exceed 30, to avoid array overrun.  
** Also G2W_SRCH_STAGE1_PN_NUM_TASKS
** times G2W_SRCH_STAGE1_PN_WIN_SIZE_CHIPX1 must be 38400. */
#define  G2W_SRCH_STAGE1_PN_NUM_TASKS        30
#define  G2W_SRCH_STAGE1_PN_WIN_SIZE_CHIPX1  1280

#define  G2W_SRCH_STAGE2_DIRECT_PN_NUM_TASKS 25
#define  G2W_SRCH_STAGE2_DIRECT_PN_WIN_SIZE_CHIPX1 1536

/* Discard direct stage 2 PN( 2048x1) peaks below 81, which corresponds to -20.8 dB. */
#define  G2W_SRCH_DIRECT_STAGE2_PN_THRESHOLD 81
/* Discard Idle Step1 peaks below 71 (Step-1 256x45, No RxD) which corresponds to -25 dB. */
#define  G2W_SRCH_STEP1_THRESHOLD      71

/*================================================================================
                           Enumerations
================================================================================*/
/* ------------------ Firmware parameters -------------------------------------*/

/* Peak Detector Mode */
typedef enum 
{
  /* "0x00": Peak Detection enabled. */
  SRCH_PD_NORMAL,
  /* "0x01": Peak Detection Disabled. */
  SRCH_PD_BYPASS,
  /* "0x02": used for DRX mode search. */  
  SRCH_PD_SHOULDER
} g2w_srch_pd_enum_type;

/* Carrier to search, primary or secondary.  G2W only uses primary. */
typedef enum
{
  G2W_SRCH_CARRIER_PRIMARY   = 0,
  G2W_SRCH_CARRIER_SECONDARY = 1,
  G2W_SRCH_CARRIER_MAX
} g2w_srch_carrier;


/* RxD value returned from firmware:
** - 0: Search results NOT obtained through RxD.
** - 1: Search results obtained through RxD separate mode.
** - 2: Search results obtained through RxD add mode. */
typedef enum
{
  G2W_SRCH_RXD_MODE_NO_RXD,
  G2W_SRCH_RXD_MODE_SEPARATE,
  G2W_SRCH_RXD_MODE_ADD,
  G2W_SRCH_RXD_MODE_MAX
} g2w_srch_rxd_mode_enum_type;

/* Firmware parameters */
typedef enum
{
  G2W_SRCH_ANTENNA_0,
  G2W_SRCH_ANTENNA_1,
  G2W_SRCH_ANTENNA_MAX
} g2w_srch_antenna_select;


/* Search Alignment */
typedef enum 
{
  /* "0x00": No alignment needed */
  G2W_SRCH_ALIGN_NONE = 0,
  /* "0x01": 256-chip alignment */
  G2W_SRCH_ALIGN_256,
  /* "0x02": 512-chip alignment */
  G2W_SRCH_ALIGN_512,
  /* "0x03": 1024-chip alignment */
  G2W_SRCH_ALIGN_1024
} g2w_srch_align_enum_type;

/* Step1/NASTT/PN search task mode */
typedef enum
{
  G2W_SRCH_TASK_MODE_NO_RXD_ADD_NO_STTD_NO_RXD_SEPARATE,
  G2W_SRCH_TASK_MODE_YES_RXD_ADD_NO_STTD_NO_RXD_SEPARATE,
  G2W_SRCH_TASK_MODE_NO_RXD_ADD_YES_STTD_NO_RXD_SEPARATE,
  G2W_SRCH_TASK_MODE_YES_RXD_ADD_YES_STTD_NO_RXD_SEPARATE,
  G2W_SRCH_TASK_MODE_NO_RXD_ADD_NO_STTD_YES_RXD_SEPARATE,
  G2W_SRCH_TASK_MODE_INVALID,
  G2W_SRCH_TASK_MODE_NO_RXD_ADD_YES_STTD_YES_RXD_SEPARATE,
  G2W_SRCH_TASK_MODE_MAX
} g2w_srch_task_mode;

/* FW parameters, srch_ss_freeze_flag */
typedef enum
{
  G2W_SRCH_SS_FREEZE_FLAG_NO_ACTION,
  G2W_SRCH_SS_FREEZE_FLAG_STEP1_SEARCH_FROZEN,
  G2W_SRCH_SS_FREEZE_FLAG_NASTT_SEARCH_UNFROZEN,
  G2W_SRCH_SS_FREEZE_FLAG_PN_SEARCH_UNFROZEN_BEFORE,
  G2W_SRCH_SS_FREEZE_FLAG_PN_SEARCH_UNFROZEN_AFTER,
  G2W_SRCH_SS_FREEZE_FLAG_MAX
} g2w_srch_ss_freeze_flag_enum_type;

/* FW parameters, srch_use_freeze_flag */
typedef enum
{
  G2W_SRCH_USE_FREEZE_FLAG_NO_ACTION,
  G2W_SRCH_USE_FREEZE_FLAG_NASTT_USES_FROZEN_SAMPLES,
  G2W_SRCH_USE_FREEZE_FLAG_PN_USES_FROZEN_SAMPLES,
  G2W_SRCH_USE_FREEZE_FLAG_MAX
} g2w_srch_use_freeze_flag_enum_type;

/*------------------------G2W drivers operations enums-----------------------*/

typedef enum
{
  /* IRAT G2W Idle Mode */
  G2W_IDLE_MODE = WFW_IRAT_G2W_IDLE_MODE,
  /* IRAT G2W Dedicated Mode */
  G2W_DEDICATED_MODE = WFW_IRAT_G2W_DEDICATED_MODE
} g2w_mdsp_mode_enum_type;

/* FW to SW ISR events */
typedef enum
{
  G2W_MDSP_SW_COMMAND_DONE,
  G2W_MDSP_SW_SEARCH_DONE,
  G2W_MDSP_SW_RF_ON,
  G2W_MDSP_SW_AGC_TUNING_DONE,
  G2W_MDSP_SW_X2W_SAMPLE_RAM_DONE,
  G2W_MDSP_SW_CRASH_IND,
  G2W_MDSP_SW_NUM_EVENTS  
} g2w_mdsp_event_type;

/*========================================================================
                           Typedef Callback functions
=========================================================================*/
/* Callback function type for pre-cb and post-cb for a SW-FW cmd */
typedef void (*g2w_mdsp_post_cb_fn)(gas_id_t gas_id);
typedef void (*g2w_mdsp_pre_cb_fn)(gas_id_t gas_id);

/* Generic Callback function type for FW-to-SW event done */
typedef void (*g2w_event_done_cb_fn) (g2w_mdsp_event_type,gas_id_t);

/*========================================================================== 
                           Structs 
===========================================================================*/
/*  Input into G2W id search from GL1 */

/* ID search cell type */
typedef struct 
{
  uint16 scr_code;
  uint16 code_group;
  boolean cell_diversity;
} g2w_mdsp_id_cell_type;

/* ID search main input struct */
typedef struct 
{
  uint16 freq;
  uint16 num_cells;
  g2w_mdsp_id_cell_type id_cell[G2W_SRCH_MAX_ID_CELLS];
} g2w_mdsp_srchdrv_id_parms_type;


/* input into list search from GL1 */
typedef struct 
{
  uint16 scr_code;
  boolean cell_diversity;
  uint32 pn_pos;
  uint16 nc;
  uint32 win_size;
} g2w_mdsp_list_cell_struct_type;

/*---------------------Dedicated mode Stage 1/2 cell structure----------------*/

/* Dedicated stage1/2 current cell search parameters */
typedef struct 
{
  uint16 freq;
  /* Scrambling Code index */
  uint16  scr_code;
  /* original pn_pos */  
  uint16 pn_pos ;
  /* Number of tasks */
  uint8  num_tasks;
  /* Window starting PN Position in chipx1 */
  uint16  task_pos_hi[G2W_SRCH_STAGE2_NUM_TASKS+1];
} g2w_srch_ded_mode_srch_q_parm_struct_type;


/*------------------------LIST SEARCH PARAMETERS STRUCTURE----------------*/

/* List search task struct */
typedef struct 
{
  /* Scrambling Code index */
  uint16  scr_code;
  /* OVSF for Primary CPICH */
  uint16  ovsf;
  /* Window starting PN Position in chipx8 */
  uint32  pn_pos;
  /* Search Window Size in chipx8 */
  uint16  win_siz;
  /* Peak Detector Mode */
  g2w_srch_pd_enum_type  pd;
  /* Alignment for coherent integration */
  g2w_srch_align_enum_type align;
  /* Coherent integration length */
  uint16  nc;
  /* Non-coherent integration length */
  uint16  nn;
} g2w_mdsp_list_srch_task_struct_type;

/* List search parameters struct*/
typedef struct 
{
  uint16 freq;
  /* Search from Sample RAM? */
  boolean sr;
  /* 3 common LSB's */
  uint16  pn_pos_lo;
  /* Number of tasks */
  uint16  num_tasks;
  /* List search tasks */
  g2w_mdsp_list_srch_task_struct_type  task[G2W_MDSP_LIST_SRCH_MAX_TASKS];
} g2w_mdsp_list_srch_q_parm_struct_type;

/*---------------------------------Search Results struct--------------------------*/

/* NAS-TT Results struct */
typedef struct
{
  uint16  psc;
  uint32  max_pos;
  uint32  max_eng;
} g2w_srch_peak_struct_type;

typedef struct
{
  uint16 num_peaks;
  g2w_srch_peak_struct_type nastt_peak[G2W_SRCH_MAX_NASTT_RESULTS];
} g2w_srch_nastt_result_struct_type;

/* PN after NAS-TT results struct type */
typedef struct 
{
  uint16 scr_code;
  uint32 max_pos;
  uint32 max_eng;
  int16  rscp;
  int16  ecio;
} g2w_mdsp_pn_peak_struct_type ;

/*  PN after NAS-TT Results to be passed back */
typedef struct 
{
  uint16 num_peaks;
  uint16 frequency;
  int16 rssi;
  g2w_mdsp_pn_peak_struct_type srch_pn_pilots[G2W_SRCH_MAX_PN_PILOTS];
} g2w_mdsp_pn_after_nastt_results_type;

/* List search Results struct */
typedef struct 
{
  uint16 scr_code;
  uint32 max_pos;
  uint32 max_eng;
} g2w_mdsp_list_srch_peak_struct_type;

typedef struct 
{
  uint16 num_tasks;
  uint16 frequency;
  g2w_mdsp_list_srch_peak_struct_type cell[G2W_MDSP_LIST_SRCH_MAX_TASKS];
} g2w_mdsp_list_srch_struct_type;

/* Dedicated Mode Stage1 Result Struct */
typedef struct
{
  uint16 pn_pos;
  uint32 eng;
} g2w_ded_mode_task_result_type;

/* Following structure is for storing the stage1 result   */
typedef struct
{
  uint16 freq;
  /* Scrambling Code index */
  uint16 scr_code;  
  /* Input PN position */   
  uint16 in_pn_pos;  
  uint16 num_results;
  g2w_ded_mode_task_result_type task[G2W_SRCH_STAGE1_NUM_TASKS];
} g2w_ded_mode_srch_result_struct_type;

/* Peak structure type */
typedef struct
{
  /* position, or offset to the Slot Reference Counter,
     of this slot peak in chipx8 resolution */
  uint16  pos;
  /* Energy of this slot peak */
  uint16  eng;
} g2w_srch_slot_peak_struct_type;

/* Struct for search type for logging purposes */
#ifdef FEATURE_G2W_SRCH_LOGGING_ENABLED

typedef enum
{
  SRCH_STEP_NONE              = 0,   /* 0 means no search is active           */
  SRCH_STEP1                  = 1,   /* Step_1 search                         */
  SRCH_STEP_LIST              = 4,   /* List Search                           */
  /* Special search types for off-line searcher.                                */
  SRCH_OFFLINE_NASTT          = 8,   /* NAS-TT search                         */
  SRCH_OFFLINE_PN_AFTER_NASTT = 9,   /* PN search after NAS-TT search         */
  SRCH_OFFLINE_STEP1          = 10,  /* Step_1 search                         */
  SRCH_OFFLINE_IDLE_RECONF_PN = 11,  /* Reconfirmation PN search in idle      */
  SRCH_OFFLINE_DED_RECONF_PN  = 12,  /* Reconfirmation PN search in dedicated */
  SRCH_OFFLINE_STAGE1         = 13,  /* Dedicated mode stage 1                */
  SRCH_OFFLINE_STAGE2         = 14,  /* Dedicated mode stage 2                */
  SRCH_STEP_ENUM_MAX          = 15
} g2w_srch_step_enum_type;

#endif

/*-----------------------------FW cmd sync params struct type----------------------*/

/* Start WFW cmd params */
typedef struct
{
  g2w_mdsp_start_mode_type start_mode;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_GSM_COEX)
  g2w_mdsp_start_mode_ue_type ue_type;
#endif
} g2w_start_wfw_cmd_param_list_type;

/* Sleep WFW cmd params */
typedef struct
{
  uint16 dummy;
} g2w_sleep_wfw_cmd_param_list_type;

/* Step 1 ID search cmd params */
typedef struct
{
  g2w_srch_carrier carrier_sel;
} g2w_step1_search_cmd_param_list_type;

/* NAS-TT search cmd params */
typedef struct
{
  boolean async_nastt;
  g2w_srch_carrier carrier_sel;
  uint8 peak_count;
  uint16 peak_pos_LSW[G2W_MAX_NASTT_PEAK_COUNT];
  uint16 peak_pos_MSW[G2W_MAX_NASTT_PEAK_COUNT];
} g2w_nastt_search_cmd_param_list_type;

/* PN search cmd params */
typedef struct
{
  boolean stage1_srch;
  boolean stage2_srch;  
} g2w_pn_search_cmd_param_list_type;

/* X2W mode RF cfg cmd params */
typedef struct
{
  /* Idle or Dedicated mode */
  g2w_mdsp_mode_enum_type mode;
  boolean ftm_mode;
  uint16 pll_settling_time;
  /* Preload offset times for FED */
  uint16 home_preload_offset_time;
  uint16 nbr_preload_offset_time;
  /* RF action time when the gap starts, in Cx8 */
  uint32 rf_action_time;
  /* RxLM buffer index */
  uint32 rxlm_buf_idx;
  /* Freq error in Q6 Hz format */
  int32 freq_err_q6_hz;
  /* Inverse W DL freq */
  uint32 inv_dlo_freq;  
} g2w_mode_rf_cfg_cmd_param_list_type;

/* Stop RF cmd params */
typedef struct
{
  uint16 dummy;
} g2w_stop_rf_cmd_param_list_type;

/* Abort cmd params */
typedef struct
{
  uint16 dummy;
} g2w_abort_cmd_param_list_type;

/*----------------UNION: G2W_MDSP_SYNC_CMD_PARAMS_TYPE-------------------*/

/* This is a union of the various mDSP command structures.*/

typedef union {
  /* Start WFW params */
  g2w_start_wfw_cmd_param_list_type start_wfw_params;
  /* Sleep WFW params, would be a dummy */
  g2w_sleep_wfw_cmd_param_list_type sleep_wfw_params;
  /* Step 1 search params */
  g2w_step1_search_cmd_param_list_type step1_search_params;
  /* NAS-TT search params */
  g2w_nastt_search_cmd_param_list_type nastt_search_params;
  /* Stage 1, Stage 2 and non-staged PN search params */
  g2w_pn_search_cmd_param_list_type pn_search_params;
  /* X2W mode and RF cfg params */
  g2w_mode_rf_cfg_cmd_param_list_type mode_rf_cfg_params;
  g2w_stop_rf_cmd_param_list_type stop_rf_params;
  g2w_abort_cmd_param_list_type abort_cmd_params;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_GSM_COEX)
  g2w_mdsp_cxm_param_cfg_type coex_param_cfg;
#endif
} g2w_mdsp_sync_cmd_params_type;


/*-------------------------STRUCTURE: MDSP_SYNC_CMD_TYPE-----------------*/

/* This structure is used for sending commands to the mDSP via the L1 sync
interface. */

typedef struct 
{
  /* which command is being sent */
  uint16 cmd_title;
  /* GSM Frame number at which the cmd is sent to FW */
  uint16 gfn; 
  /* cmd parameters to be put in the the data area of the buffer */
  g2w_mdsp_sync_cmd_params_type sync_params;
  /* This is a callback function which will be called by the L1 Sync
     module before delivering the command to the DSP. */
  g2w_mdsp_pre_cb_fn pre_proc_callback;
  /* This callback function will be executed after the command submitted
     to the DSP is executed and CMD DONE Interrupt is asserted */
  g2w_mdsp_post_cb_fn post_proc_callback;

  /* WFW->G command meaning WFW is done processing the command. */
  WfwSWCmdDoneFlagsEnum cmd_done_flag;

  /* How long until we consider command timed out. */
  timetick_type          cmd_done_timeout_msec;
} g2w_mdsp_sync_cmd_struct_type;


/* MDSP cmd queue control info struct */
typedef struct
{
  /* head index of the cmd queue */
  uint8 queue_head_idx;
  /* tail index of the cmd queue */
  uint8 queue_tail_idx;
  /* the actual free buffers in the queue */
  g2w_mdsp_sync_cmd_struct_type g2w_mdsp_sync_cmd_buffs[G2W_MDSP_SYNC_DSP_CMD_Q_LEN_MAX];
} g2w_mdsp_sync_queue_ctrl_struct_type;


/* MDSP free cmd buffer queue struct */
typedef struct
{
  /* current index of the free cmd buffer queue */
  uint8 cur_index;
  /* the actual free cmd buffers */
  g2w_mdsp_sync_cmd_struct_type g2w_free_cmd_buffs[G2W_MDSP_SYNC_FREE_CMD_BUF_Q_LEN];
} g2w_mdsp_sync_free_cmd_bufs_queue_struct_type;

/*=============================================================================
                     Extern Variables
==============================================================================*/

/* Flag for mDSP busy */
extern boolean g2w_mdsp_sync_dsp_busy[];

/* Dedicated Mode Stage1/2 result objects */
extern g2w_ded_mode_srch_result_struct_type g2w_stage1_result[], g2w_stage2_result[];

/* Dedicated Mode Stage 1 and 2 result cell indices */
extern uint16 g2w_srch_cur_stage1_result_idx[], g2w_srch_cur_stage2_result_idx[];

/* List Search Results sent back to GL1 */
extern g2w_mdsp_list_srch_struct_type g2w_srch_list_srch_result[];

/* List Search result count */
extern uint16 g2w_srch_lpq_cnt[];

/*=======================================================================
           F U N C T I O N    P R O T O T Y P E S
========================================================================*/

/*=======================================================================   
   
FUNCTION :      G2W_MDSP_SYNC_INIT

DESCRIPTION :   This function initializes the command queue, logging buffer, 
                pointers to FW interface and registers the callback handlers
                for ISRs from FW.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.   
 
===========================================================================*/
extern void g2w_mdsp_sync_init(gas_id_t gas_id);

/*===========================================================================

FUNCTION :      G2W_SET_EVENT_DONE_CB

DESCRIPTION :   This function sets the callback function for event done ISRs.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    Callback function is set to be g2w_srch_drv_continue_srch.

===========================================================================*/
extern void g2w_set_event_done_cb(g2w_event_done_cb_fn g2w_event_cb,gas_id_t gas_id);

/*===========================================================================
FUNCTION G2W_MDSP_SYNC_EVENT_HANDLER

DESCRIPTION
 	This function handles the MDSP to SW event and executes the callback function
 	registered for that event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void g2w_mdsp_sync_event_handler(gas_id_t gas_id);

/*===========================================================================
FUNCTION        G2W_MDSP_SYNC_SEND_START_CMD

DESCRIPTION     This function sends START command to mDSP

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    Pre-cb function will be executed, if registered
===========================================================================*/
extern void g2w_mdsp_sync_send_start_cmd(
  /* mode to start the firmware in */
  g2w_mdsp_start_mode_type start_mode
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_GSM_COEX)
  ,g2w_mdsp_start_mode_ue_type ue_type
#endif
  /* pre call back to be executed before the command is sent to fw */
  ,g2w_mdsp_pre_cb_fn pre_cb,
  /* post call back to be executed after the command is sent to fw */
  g2w_mdsp_post_cb_fn post_cb,
  gas_id_t gas_id
);

/*===========================================================================

FUNCTION        G2W_MDSP_SYNC_SEND_MODE_RF_CFG_CMD

DESCRIPTION     This function formats and sends a X2W_MODE_CFG command to the
                mDSP by way of the mDSP command queue.  

DEPENDENCIES    None

RETURN VALUE    None 
 
SIDE EFFECTS   Pre-cb function will be executed, if registered
===========================================================================*/
extern void g2w_mdsp_sync_send_mode_rf_cfg_cmd(
  /* G2W mode: idle or dedicated */
  g2w_mdsp_mode_enum_type mode,
  /* RxLM buffer index */
  uint32 g2w_rxlm_buf_idx,
  /* RF action time in WSTMR cx8 */
  uint32 rf_action_time,
  /* Freq error in Q6 Hz format */
  int32 freq_err_q6_hz,
  /* Inverse W DL freq */
  uint32 inv_dlo_freq,
  /* pre call back function */
  g2w_mdsp_pre_cb_fn pre_cb,
  /* post call back function */
  g2w_mdsp_post_cb_fn post_cb,
  gas_id_t gas_id);

/*===========================================================================

FUNCTION        G2W_MDSP_SYNC_SEND_STEP1_SRCH_CMD

DESCRIPTION     This function formats and sends a Step1 search command to the
                mDSP by way the the mDSP command queue.  

DEPENDENCIES    None

RETURN VALUE   None

SIDE EFFECTS    Pre-cb function will be executed, if registered
===========================================================================*/
extern void g2w_mdsp_sync_send_step1_srch_cmd(
  /* pre-command callback */
  g2w_mdsp_pre_cb_fn pre_cb,
  /* post-command callback */
  g2w_mdsp_post_cb_fn post_cb,
  gas_id_t gas_id);

/*===========================================================================

FUNCTION        G2W_MDSP_SYNC_SEND_NASTT_SRCH_CMD

DESCRIPTION     This function formats and sends a NAS-TT search command to the
                mDSP by way the the mDSP command queue.  

DEPENDENCIES    None

RETURN VALUE    None
 
SIDE EFFECTS    Pre-cb function will be executed, if registered

===========================================================================*/
extern void g2w_mdsp_sync_send_nastt_srch_cmd(
  /* pre-command callback */
  g2w_mdsp_pre_cb_fn pre_cb,
  /* post-command callback */
  g2w_mdsp_post_cb_fn post_cb,
  gas_id_t gas_id);

/*===========================================================================

FUNCTION        G2W_MDSP_SYNC_SEND_PN_SRCH_CMD

DESCRIPTION     This function formats and sends a PN search command to the
                mDSP by way the the mDSP command queue.  

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    Pre-cb function will be executed, if registered

===========================================================================*/
extern void g2w_mdsp_sync_send_pn_srch_cmd(
  /* PN search parameters for Stage1/Stage2 searches */
  g2w_srch_ded_mode_srch_q_parm_struct_type* g2w_pn_srch_ptr,
  /* PN after NAS-TT/Reconf search parameters */
  g2w_mdsp_list_srch_q_parm_struct_type* g2w_srch_list_srch_q_parms,
  /* Trigger Stage1 search */
  boolean stage1_srch,
  /* Trigger Stage2 search; if both flags are FALSE, trigger Reconf/PN after NASTT search */
  boolean stage2_srch,
  /* pre-command callback */
  g2w_mdsp_pre_cb_fn pre_cb,
  /* post-command callback */
  g2w_mdsp_post_cb_fn post_cb,
  gas_id_t gas_id);

/*===========================================================================

FUNCTION        G2W_MDSP_SYNC_SEND_ABORT_CMD

DESCRIPTION     This function sends an ABORT search command to the
                mDSP by way of the mDSP command queue.  

DEPENDENCIES    None
 
RETURN VALUE    None 
 
SIDE EFFECTS    Pre-cb function will be executed, if registered
===========================================================================*/
extern void g2w_mdsp_sync_send_abort_cmd(
  /* pre-command callback */
  g2w_mdsp_pre_cb_fn pre_cb,
  /* post-command callback */
  g2w_mdsp_post_cb_fn post_cb,
  gas_id_t gas_id);

/*===========================================================================

FUNCTION        G2W_MDSP_SYNC_SEND_STOP_RF_CMD

DESCRIPTION     This function sends a STOP RF command to the
                mDSP by way of the mDSP command queue.  

DEPENDENCIES    None
 
RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void g2w_mdsp_sync_send_stop_rf_cmd(
  /* pre-command callback */
  g2w_mdsp_pre_cb_fn pre_cb,
  /* post-command callback */
  g2w_mdsp_post_cb_fn post_cb,
  gas_id_t gas_id);

/*===========================================================================

FUNCTION        G2W_MDSP_SYNC_SEND_SLEEP_WFW_CMD

DESCRIPTION     This function sends a sleep wfw command to the mDSP by way of 
 				the mDSP command queue when a search completes or G2W drivers is
 				shut down by GL1 abort cmd.

DEPENDENCIES    None
 
RETURN VALUE    None

SIDE EFFECTS    Pre-cb function will be executed, if registered
===========================================================================*/
extern void g2w_mdsp_sync_send_sleep_wfw_cmd(
  /* pre-command callback */
  g2w_mdsp_pre_cb_fn pre_cb,
  /* post-command callback */
  g2w_mdsp_post_cb_fn post_cb,
  gas_id_t gas_id);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_GSM_COEX)
#ifdef FEATURE_JOLOKIA_MODEM
#else
/*===========================================================================

FUNCTION        G2W_MDSP_SYNC_SEND_CXM_CHANNEL_ID_TO_WFW

DESCRIPTION     This function sends the CXM priority table to the mDSP. Used
                to configure DSDA COEX management for G2W.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS
===========================================================================*/
void g2w_mdsp_sync_send_cxm_channel_id_to_wfw(
  /* pre-command callback */
  g2w_mdsp_pre_cb_fn pre_cb,
  /* post-command callback */
  g2w_mdsp_post_cb_fn post_cb,
  gas_id_t gas_id);
#endif

/*===========================================================================

FUNCTION        G2W_MDSP_SYNC_SEND_CXM_PRIORITY_TBL_TO_WFW

DESCRIPTION     This function sends the CXM priority table to the mDSP. Used
                to configure DSDA COEX management for G2W.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS
===========================================================================*/
void g2w_mdsp_sync_send_cxm_priority_tbl_to_wfw(
  /* pre-command callback */
  g2w_mdsp_pre_cb_fn pre_cb,
  /* post-command callback */
  g2w_mdsp_post_cb_fn post_cb,
  gas_id_t gas_id);
#endif
/*===========================================================================

FUNCTION        G2W_MDSP_SYNC_SEND_FW_PANIC_CMD

DESCRIPTION     This function sends Panic cmd to FW. Mostly used for debugging   
                purposes. 

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    Panic cmd sent to FW incase SW error fatals and for debugging both
                SW and FW states are required.

===========================================================================*/
extern boolean g2w_mdsp_sync_send_fw_panic_cmd(
  /* the pre-command callback function */
  g2w_mdsp_pre_cb_fn pre_cb,
  /* the post-commadn callback function */
  g2w_mdsp_post_cb_fn post_cb);

/*===========================================================================
FUNCTION       G2W_SRCH_STEP1_DUMP

DESCRIPTION    This function retrieves Step 1 search results from WCDMA firmware and
               stores that data in g2w_srch_step1_peak_cnt and g2w_srch_psc_peaks[].

DEPENDENCIES   None.

PARAMETERS     None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
extern void g2w_mdsp_step1_dump(gas_id_t gas_id);

/*===========================================================================
FUNCTION      G2W_MDSP_NASTT_DUMP

DESCRIPTION   This function retrieves NAS-TT search results from WCDMA firmware and
              stores the results in g2w_srch_nastt_result structure.

DEPENDENCIES  None.

PARAMETERS    None.

RETURN VALUE  uint16 - Number of NAS-TT peaks detected.

SIDE EFFECTS  None.
===========================================================================*/
extern uint16 g2w_mdsp_nastt_dump(gas_id_t gas_id);

/*===========================================================================
FUNCTION     G2W_SRCH_PN_AFTER_NASTT_DUMP

DESCRIPTION  This function retrieves PN search results from WCDMA firmware and
             stores the results in g2w_srch_step3_srch_result structure.

DEPENDENCIES None.

PARAMETERS   None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
extern void g2w_mdsp_pn_after_nastt_dump(gas_id_t gas_id);

/*===========================================================================
FUNCTION     G2W_MDSP_STORE_STAGE1_RESULT

DESCRIPTION  This function processes the results obtained from Stage 1 PN search in
 	         dedicated mode.
 
DEPENDENCIES None.

PARAMETERS   None

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
extern void g2w_mdsp_store_stage1_result(gas_id_t gas_id);

/*===========================================================================
FUNCTION     G2W_MDSP_STORE_STAGE2_RESULT

DESCRIPTION  This function processes the results obtained from Stage 2 PN search in
             dedicated mode.

DEPENDENCIES None.

PARAMETERS   None
 
RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
extern void g2w_mdsp_store_stage2_result(
  /* Indicate if we program for direct Stage 2 search */
  boolean stage2_direct,
  gas_id_t gas_id);

/*===========================================================================
FUNCTION     G2W_MDSP_STORE_RECONF_RESULT

DESCRIPTION  This function processes the results obtained from Reconfirmation PN search in
             dedicated mode.

DEPENDENCIES None.

PARAMETERS   None
 
RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
extern void g2w_mdsp_store_reconf_result(gas_id_t gas_id);

/*===========================================================================

FUNCTION      G2W_MDSP_GET_RSSI

DESCRIPTION   This function returns the instantaneous RSSI value from the FW variable.
 
DEPENDENCIES  None.

RETURN VALUE  int16.

SIDE EFFECTS  None

===========================================================================*/
extern int16 g2w_mdsp_get_rssi(gas_id_t gas_id);

/*===========================================================================

FUNCTION     G2W_SRCH_DRV_MUTEX_LOCK

DESCRIPTION
  This function locks the mutex for G2W search drivers shared between task and
  ISR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void g2w_srch_drv_mutex_lock(void);

/*===========================================================================

FUNCTION     G2W_SRCH_DRV_MUTEX_UNLOCK

DESCRIPTION
  This function unlocks the mutex for G2W search drivers shared between task and
  ISR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void g2w_srch_drv_mutex_unlock(void);
#endif /*(FEATURE_INTERRAT_HANDOVER_GTOW) || defined(FEATURE_INTERRAT_CELL_RESELECTION_GTOW) */
#endif /*#ifndef G2WSRCHWFWINTF_H */


