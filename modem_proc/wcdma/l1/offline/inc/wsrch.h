#ifndef SRCH_H
#define SRCH_H
/*===========================================================================
                S E A R C H      H E A D E R      F I L E

GENERAL DESCRIPTION:
  This file contains basic definitions used all other srch files or 
  external modules.

EXTERNALIZED FUNCTIONS

 srch_hpq_isr
  This function is the interrupt handler for Searcher HPQ interrupt when HPQ
  search is done.

 srch_lpq_isr
  This function is the interrupt handler for Searcher LPQ interrupt when LPQ
  search is done.

 srch_get_ref_cell_scr_code
  This function retruns active cell's primary scrambling code. 

 srch_get_cpich_rscp
  This function retruns serving cell's RSCP.

 srch_get_curr_freq
  This function returns current frequency on which UE is camped.

 srch_get_filtered_rscp
  This function returns an unsigned value representing the current,
  filtered RSCP value in P(AGC) + Ec/Io form for all ASET cells. 
  
  The returned can then be arbitrarily mapped to UI display space 
  using an equation, table, or case statement.  
  
  The RSCP is filtered using an (N-1)/N IIR filter for a smoother signal
  strength display. 

 srch_compute_rscp
  This function calculates the RSCP given the ecIo and the RxAGC..
  The calculated RSCP value will be returned in uint16 units. The actual RSCP
  is -ve of the returned value.

 srch_async_parm_init
  This function initializes searcher asynchronous parameters depending
	on the current state.

 srch_event_timer_elapsed
  This function returns the time elapsed from start_ts passed to it.

 nset_process_log_on_demand
  This funtion creates a NSET log packet and submits it to QXDM

 aset_process_log_on_demand
  This funtion creates a ASET log packet and submits it to QXDM

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright(c) 2000-2012 by Qualcomm Technologies, Inc.
All Rights Reserved.
      
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srch.h_v   1.11   29 May 2002 11:28:34   msatyana  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wsrch.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/16   vn      Initial check-in for FR 30856
02/24/15   ap      Initial check-in for WL1 QSH framework 
12/24/15   vn      Handled recovery window,FET scenario in utilization calculation
10/20/15   vn      Changes for Step1 Peak Pruner algorithm.
09/14/15   rn      Added Rumi framework enhancement changes for searcher module.
07/17/15   vn      Externing variable which store maximum TX power inside PCH_SLEEP and BPLMN.
04/08/15   skk     Send Cell_selection_CNF with failure to RRC if lock in not available to W for 5 sec
01/16/15   ag      Added Searcher debug code
12/31/14   amj     call MCVS API to update srch peiodicity always in DCH
12/23/14   bj      Adding debug command in WPLT for PWRUP ACQ full window PN search.
12/02/14   amj     Allow parallel Step1 when inter-f step1 is ongoing.
11/24/14   gsk     SRCHZZ wakeup cleanup changes
12/2/14    vn      Added macro checks whether cell selection req is received.
11/19/14   jd      Fix for compiler warning.
11/19/14   da      Searcher changes to support FR23776 : LTE to WCDMA CSFB delay optimizations
11/17/14   svh     Added new global for storing idle mode rxagc for ASDIV.
11/06/14   amj     Search Utilization formula update.
10/29/14   vn      Inorder to have NBR FET coexistance a global to check 
                   whether nbrej has performed or not is introduced
10/17/14   amj     DSDA search utilization adjustment.
10/10/14   amj     Code Cleanup.
10/07/14   jk      Fixing the compiler warnings by adding forward 
                   declaration for 'wsrch_nv_read()'.
09/10/14   amj     Nastt Power Optimization changes.
09/24/14   amj     Request/Release diversity chain for NBR.
09/19/14   da      Rumi specific fix related to frequency error in ACQ
09/17/14   ac      Changed WSRCH_NBREJ and QTA_ROTA NVs to use WNV API
05/20/14   bj      Added support for Pause and Resume searches in QTA gap and disable RxD searches in QTA gap
08/21/14   da      Fixed compiler warnings 
08/07/14   jk      Fixed search compiler warnings.
07/28/14   amj     Jolokia Updated searcher tables and RUMI database
07/18/14   sad     RF api changes and inter freq tune changes
07/15/14   bj      WCDMA Narrow Band Rejection for System Search.
07/15/14   ac      NV reorganization
06/17/14   rsr     Added Check to Ensure that NBR is not Supported in Div. Only 
                   Mode.
05/30/14   ar      Remove DUAL SIM featurization for srch_is_wkup_srch
05/28/14   amj     Mainline FEATURE_WCDMA_GET_RAT_MEAS_INFO.
05/28/14   amj     MCVS searcher changes for VPE clock
05/29/14   ag      R10 periodic measurement support on second carrier.
03/31/14   jd      Fix for conflict between scch deactivation and parallel step1.
05/21/14   jk      Moved the periodic measurements handling from srch_ops_cmd to a 
                   new local command.
05/13/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
03/17/14   jd      Fix to use mutex in scheduler to avoid race condition during mode change.
04/14/14   gsk     Fix for compilation errors
02/12/14   sad     DB-3C API check in
04/01/14   km      RxD/ACQ changes
07/01/14   amj     Clean up old ASET Update code
01/06/14   sr      Suspending periodic ops before CPHY req processing
12/16/13   as      Porting G2W TA from Triton to Dime.
11/25/13   jk      FR 15721: HSPA+: Call performance improvement when selecting the best cell.
11/21/13   jd      FET 2.0 Searcher two queue approach.
10/30/13   jd      Changes to disable parallel step1 search when offline clk is 72mhz and rxd is active.
10/31/13   amj     Define mutex for RXD callback
10/17/13   bs      Preliminary 3C Support
09/12/13   jd      Searcher redesign
07/26/13   da      Replaced 8 logging functions with a single generic one 
07/24/13   jd      Changes for Dual band Dual carrier
07/23/13   vs      Added support for ASDiv feature.
07/15/13   gsk     FR 3672: CTCH enhancement changes
06/06/13   amj     Added API for changing the Step1 threshold when CPC DRX mode starts/stops
05/31/13   sad     Support for enahnced Layer-1 manager
05/21/13   sad     Addded function srchdc_active
04/11/13   gsk     Idle Mode FR 2741/2743, reducing WL1 wait time between rf_tune_to_chan
                   and search cmd to FW. Also adding support for interf offline PN
03/27/13   km      Removed logging for all cell search
01/21/13   amj     Make abort srch function return boolean for offline clk change
01/03/13   zr      Moving PN bypass parameters to heap memory
11/28/12   amj     MCVS feature related changes  
11/19/12   amj     Supporting imbalanced RxD step 1 search
10/24/12   amj     Cleaned up srch reset/restore state functionality.
09/24/12   zr      Adding API to determine the dual carrier frequency
08/15/12   zr      Adding debug search option
08/14/12   zr      Adding log packets to reduce F3 logging from searcher
08/03/12   sr      Delay the CM config till DPCH reslam
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12   pv      Offline PICH changes - add 2 parameters to legacy sample server cmd.
07/05/12   gsk     Idle mode FRs 1782/1705/1947/2152/1950
05/23/12   kcm     L/G to W handover optimizations
04/18/12   zr      Added prototypes to revert and restore the DCH substate
04/03/12   zr      Added dummy cell creation function for testing 
03/27/12   hk      Added support for FEATURE_WCDMA_HS_FACH_DRX
03/14/12   zr      Added PN bypass peak info log packet
03/06/12   zr      Adding API to reschedule aborted PN search after offline clock speed change01/30/12   vs      Feature cleanup.
01/20/12   bs      free floating changes.
12/19/11   sv      Increased the array size for "valid" element to correct size 
                   in search bypass structure.
12/07/11   sv      Added changes to support Fatpath algorithm in connected mode.
11/06/11   nd      Added structure definitions to capture event 1A and evt2D
                   info to implement 2D delay algorithm.
08/11/11   mmr     Externed wsrch_tune_to_channel()
07/25/11   mmr     Changed the rotator value from int16 to int32
06/17/11   sa      Added all cell search count for logging all cells after
                   every 1 sec.
01/31/11   sa\nd   Moved processing of INTER-RAT periodic measurements from 
                   ISR to task context.
10/19/10   sv      Added changes to support full search for DC and QICE support.
07/15/10   vsr     (Genesis) Moved CME call to triage
05/12/10   sv      Fixed prototype for full window PN search.
03/30/10   rvs     Remove FEATURE_WCDMA_CME_CONTROLLER.
02/26/10   rvs     Fixes for Mustang.
08/11/09   sv      Fix compiler warning.
07/16/09   hk      Reorganized the code to remove INC/SRC violations
07/15/09   rvs     Edited feature flags to include
                   FEATURE_WCDMA_QICE_CONTROLLER.
05/26/09   sv      Added support for sending freeze\unfreeze sample server command to FW.
05/25/09   hk      Fixed compiler warnings
05/05/09   sv      Modified prototype for srch_mcal_handle_no_srch_cb.
04/29/09   rvs     Moved CME txd change cb functions to wl1srchdlif.h.
04/24/09   stk     Compiler warning fixes
04/20/09   rvs     Add CME txd change cb prototype and reg/dereg functions.
03/13/09   sv      Externed the "srch_async_parm_init_in_pch_sleep" function.
01/12/09   sv      Externed the "srch_mcal_handle_no_srch_cb" function.
12/09/08   sv      Mainlined FEATURE_WCDMA_OFFLINE_SEARCHER, FEATURE_RUMI_BRINGUP 
                   and FEATURE_WCDMA_DL_ENHANCED.
11/15/08   vsr     Moved CME srch callback call to task context
11/09/08   sv      Externed the "srch_mcal_whl1_status_cb" function.
10/06/08   sv      Externed some functions 
08/27/08   vsr     Added CME capability under feature FEATURE_WCDMA_CME_CONTROLLER
10/26/07   sk      Checking in MBMS branch code into mainline with MBMS feature disabled
09/26/07   nd      added extern for the new ISR for freq scan done from mDSP
05/23/07  ub/sk    Intergrate GSM meas and code cleanup
03/21/07   bbd     Removed srch timing adjust feature.
11/01/06   cpe     Mainlined the following features:
                    -FEATURE_OPTIMIZE_SEARCH_FOR_HHO
06/20/06   bd      Renamed global vars/functions to avoid conflict with 1x names
03/20/06   sv      Changes to support FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE.
07/01/05   bbd     Add proto for srch_set_timing_adj_value(). 
04/14/05   vb      Externed srch_hset_srch_cmd
04/14/04   sh      Added proto for srch_compute_rscp().
03/26/04   cpe     Added two more fields to srch_ops_code_struct_type
09/09/03   m       Modifications to support Inter Freq HHO
06/27/03   sh      Added function srch_event_timer_elapsed().
05/28/03   scm     Remove srch_prepare_for_suspend/resume().
05/23/03   cpe     Added the filed meas_id to the structure
                   srch_ops_code_struct_type
08/22/02   scm     Add srch_prepare_for_suspend() and srch_prepare_for_resume().
05/29/02   m       added prototype to support LOD for NSET and ASET updates
05/18/02   sh      Added function srch_async_parm_init() to initialize srch
                   async parameters based on the current state.
03/12/02   cpe     Added periodic_meas_rpt field to srch_ops_code_struct_type
09/12/01   wjn     Added the prototype of function "srch_get_filtered_rscp()"
                   for WCDMA RSSI display by UI.
06/26/01   cpe     Added srch_get_cpich_rscp() prototype
07/05/00   wjn     Created file.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "srchset.h"
#include "l1task.h"
#include "l1def.h"
#include "cmapi.h"
#include "rxdiv_v.h"
#include "wnv_l1.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define SRCH_RESEND_THROUGH_SCHED() ((l1m_state == L1M_ACQ) || (l1m_state == L1M_BCH) || (l1m_state == L1M_DCH) || ((l1m_state == L1M_FACH) && !(srchfachdrx_query_edrx_active())))
#define SRCH_MAX_TYPES 0x100

#define SRCH_MAX_VPE_CLK_SPEED 5
#define SRCH_MAX_NUM_GSM_TRANS_SLOTS 5

#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
/* typedef for the searcher callback function type */
typedef void ANT_SWITCH_SEARCHER_CB_FUNC_TYPE(void);
/* searcher callback register function  */
extern void ant_switch_register_with_search_done_cb(ANT_SWITCH_SEARCHER_CB_FUNC_TYPE *ant_switch_searcher_cb);
/* searcher callbacl deregister function */
extern void ant_switch_deregister_with_search_done_cb(void);
/* srch_rxagc_prim uses SRCH_RXAGC_VAL_FOR_RSCP and srch_rxagc_prim_idle uses SRCH_RXAGC_VAL_DBM. */
extern int16 srch_rxagc_prim, srch_rxagc_div, srch_rxagc_prim_idle;
#endif

/*Whether a cell selection request received or not */
#define WSRCH_IS_CELL_SELECTION_REQ_RECEIVED() (srch_cphy_cell_selection_recvd)

/*--------------------------------------------------------------------------
                            SEARCHER QUEUE TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  SRCH_S1_NASTT_PN_Q,           /* step1-nastt-pn srch*/
  SRCH_OTDOA_Q,            /* OTDOA srch  */
}
srch_queue_enum_type;

/*--------------------------------------------------------------------------
                         SEARCHER OPERATION TYPE
--------------------------------------------------------------------------*/
/** 
 * Enum that holds all the various types of searches supported
 */
typedef enum
{
  NO_SRCH = 0x0,
  ASET_PN_SRCH = 0x01,
  NSET_PN_SRCH = 0x02,
  STEP1_SRCH = 0x04,
  NASTT_SRCH = 0x08,
  DC_STEP1_SRCH = 0x10,
  DC_NASTT_SRCH = 0x20,
  PARALLEL_STEP1_SRCH = 0x40,
  PARALLEL_NASTT_SRCH = 0x80,
  W_3C_STEP1_SRCH = 0x100,
  W_3C_NASTT_SRCH = 0x200,
  #ifdef FEATURE_DUAL_SIM
  WKUP_PN_SRCH = 0x400,
  #endif
  SCCH_ACT_SRCH = 0x800
} srch_ops_code_enum_type;

typedef struct
{
  boolean aset_srch;    /* Acive and Candidate Set Search */
  boolean nset_srch;    /* Neighbor set search  */
  boolean rssi_update;  /* RSSI update */
  boolean fing_assign;  /* Finger Assignment */
  boolean step1_srch;    /* step1 search  */
  boolean nastt_srch;    /* NASTT search */
  boolean dc_step1_srch; /* DC step1 search  */
  boolean dc_nastt_srch; /* DC NASTT search */
  boolean log_srch;     /* Log search events */
  boolean srch_dbg;     /* Multi-purpose search debug option */
  srch_ops_code_enum_type srch_type;         /* Search type enum for new Scheduler */
}
srch_ops_code_struct_type;

/*--------------------------------------------------------------------------
                       SEARCHER LOST_SRCH STRUCT TYPE
--------------------------------------------------------------------------*/
typedef struct
{
  boolean  s1_nastt_pn_srch_lost;    /* lost step1/nastt/pn search */
  boolean  otdoa_srch_lost;    /* lost OTDOA search */
  srch_intraf_queue_enum_type srch_queue_idx;   /* Specifies the queue on which search is lost */
}
srch_lost_srch_struct_type;

typedef struct
{
  uint16 psc;
  uint16 freq;
  uint32 bmask;
  boolean status;
} srch_wakeup_set_struct_type;

typedef enum
{
  /* when no full srch is going on*/
  SRCH_NO_SRCH,
  /* every 360 ms, this full srch is attempted, Step1-NASTT (async nbrs)*/
  SRCH_STEP1_NASTT_ASYNC,
  /* every 360 ms, this full srch is attempted, Step1-NASTT (async nbrs)*/
  SRCH_STEP1_NASTT_UNMON
} srch_full_srch_type_enum_type;

/*--------------------------------------------------------------------------
                         SEARCH STEP ENUMERATION

--------------------------------------------------------------------------*/

typedef enum
{
  SRCH_STEP_NONE,     /* 0 means no search is active */
  SRCH_STEP1,         /* Step_1 search */
  SRCH_STEP_NASTT,    /* NASTT search */
  SRCH_STEP_PN        /* PN search */
} srch_step_enum_type;

#define SRCH_MAP_STEP_TO_QUEUE(srch_step) ( (srch_step == SRCH_STEP_PN ) ? SRCH_PN_Q : SRCH_S1_NASTT_Q )

/* type of search - intra OR inter */
typedef enum
{
  INTRA_F,
  INTER_F,
} srch_cell_type_enum_type;

typedef struct
{
 uint8 meas_id;
 uint8 carr_idx;
 uint16 periodic_meas_rpt_cnt;
}srch_periodic_meas_struct_type;

typedef struct
{
  uint32 evt_idx;
} l1_interrat_periodic_meas_struct_type;

typedef struct
{
  boolean active;
  uint16 ts;

  boolean freeze;
  uint16 freeze_ts;
  uint16 new_ttt;
  uint16 ota_ttt;
}srchinterf_2d_info_struct_type;

typedef struct
{
 boolean active;
 uint16 ts;
}srchdch_evt1a_info_type;

extern srchdch_evt1a_info_type evt_1a;

typedef enum
{
  SRCH_FILTER_ECIO,
  SRCH_FILTER_RSCP,
  SRCH_FILTER_MAX
} srch_filter_type_enum;

typedef struct
{
  /* path energy for primary antenna search */
  uint32 max_eng[SRCH_MAX_NUM_PEAKS_BYPASS_SRCH];
  /* path position for primary antenna search */
  uint32 max_pos[SRCH_MAX_NUM_PEAKS_BYPASS_SRCH];
  /* path energy for secondry antenna search */
  uint32 max_eng_div[SRCH_MAX_NUM_PEAKS_BYPASS_SRCH];
  /* path position for secondry antenna search */
  uint32 max_pos_div[SRCH_MAX_NUM_PEAKS_BYPASS_SRCH];
  /* number of peaks returned by FW for bypass mode search */
  uint16 num_bypass_peaks;
  /* number of peaks returned by FW for normal  mode search */
  uint16 num_peaks;
  /* total number of peaks returned by FW for normal and bypass mode search */
  uint16 tot_num_peaks;
  boolean valid[2*SRCH_MAX_NUM_PEAKS_BYPASS_SRCH];  
}srch_bypass_pn_srch_results_struct_type;

typedef struct
{
  /* flag to enable bypass  mode search */
  boolean enable;
  boolean  srch_result_valid[MAX_NUM_ANTENNA];
  /* results per task */
  srch_bypass_pn_srch_results_struct_type task_rslt[SRCH_BYPASS_MODE_MAX_PN_TASKS];
  
}srch_pn_srch_bypass_mode_params_struct_type;

extern srch_pn_srch_bypass_mode_params_struct_type* srch_pn_bypass_srch_params;

/* Database for rumi bringup flags and globals */
typedef struct
{
  /*Flag to enable rumi bringup*/
  boolean bringup_enabled;
  
  /*variable to do full window PN during ACQ*/
  boolean acq_use_full_win_pn_srch;

  /*To try Full window PN, scrambling code is given as input*/
  uint16 full_win_pn_scr_code;

  /* Enable rumi recovery win_size boolean */
  boolean recover_win_size;

  /* Enable rumi recovery win_size for hho fail 128*36*/
  boolean recover_win_size_hho_fail;

  /*Enable reacq pn search*/
  boolean enable_reacq_pn;

  /* Enable testing with frequency error */
  boolean test_with_freq_err;

}srch_rumi_db_struct_type;

extern srch_rumi_db_struct_type srch_rumi_db;

#define SRCH_MAX_NUM_CYCLE_TYPE  3
#define SRCH_MAX_NUM_RXD_VALUES  2
#define SRCH_MCVS_Q16_VALUE_NOTATION  65536

/* this number is for 1.5/4 = 0.375 into Q16 */
#define SRCH_MCVS_Q16_0375 24576

/* this number is for 0.6666667 into Q16 */
#define SRCH_MCVS_Q16_0667 43691

/* this number is for 0.8333 into Q16 - PN cmd processing delay */
#define SRCH_MCVS_Q16_1500 54611

/* this number is for 31.5 into Q16 used for Step1 */
#define SRCH_MCVS_Q16_STEP1_315 2064384

/* this number is for 1080 * 100 into Q16 used for final utilization formula */
#define SRCH_MCVS_Q16_UTIL_1080 6068

/* this number is checking whether number of PSCs are too much so that FW has problem */
/* to finish search in expected timeline */
#define WSRCH_UTIL_MAX_NUM_PSC_THRESHOLD 20

/* this number in percentage is for adjusting util calculation when FET is enabled */
#define WSRCH_UTIL_FET_ADJUST 14



typedef struct
{
  /** Silver Cycles {LOW, MEDIUM, HIGH} */
  uint8 cycles;
  /** Full search period for carrier 0 {180, 160 ms} */
  uint16 T_fs0;
  /** Full search period for carrier 1 {180, 160 ms} */
  uint16 T_fs1;
  /** Indicates whether compressed mode is active {0, 1} */
  uint8 I_cm;
  /** Number of active/candidate set searches per 1080 ms {54} */
  uint8 N_ac;
  /** Number of sync searches per 1080 ms {14} */
  uint8 N_s;
  /** Number of active and cadidate cells on carrier 0 {1 to 12^5} */
  uint32 X_ac0;
  /** Number of active and cadidate cells on carrier 1 {1 to 12^5} */
  uint32 X_ac1;
  /** Number of sync cells on carrier 0  {0 to 28} */
  uint32 X_s0;
  /** Number of sync cells on carrier 1  {0 to 28} */
  uint32 X_s1;
  /** Indicates whether UE has a secondary downlink {0, 1} */
  uint8 I_1;
  /** Indicates whether RxD is enabled {0, 1} */
  uint8 I_RxD;
    /** Indicates whether recovery window is enabled {0, 1} */
  uint8 I_RecW;
}srch_mcvs_srch_util_params_struct_type;

typedef struct
{

  /* this array is to maintain the NASTT tasks/NASTT slot values for each type of search cycles */
  uint8 srch_nastt_tasks_per_nastt_slot[SRCH_MAX_NUM_CYCLE_TYPE];

  /* this array is going to maintain the PN tasks/ slot values for each type of search cycles, as a function of RxD */
  uint8 srch_pn_tasks_per_slot[SRCH_MAX_NUM_RXD_VALUES][SRCH_MAX_NUM_CYCLE_TYPE];

  /* this array is going to maintain the PN tasks/ slot values for each type of search cycles, when recovery window PN is issued */
  uint8 srch_pn_tasks_per_slot_recovery_window[SRCH_MAX_NUM_RXD_VALUES][SRCH_MAX_NUM_CYCLE_TYPE];

} srch_mcvs_srch_util_struct_type;

extern srch_full_srch_type_enum_type srch_async_full_srch_type;

extern boolean srch_reset_ops_sanity_vars_needed;
extern int16 srch_saved_max_tx_pwr_nv;



#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
/* Mutex to perform Inter-Frequency measurement Parameters */
extern rex_crit_sect_type wl1_srch_interf_store_meas_db_mutex;

/* Mutex to perform Intra-Frequency measurement Parameters */
extern rex_crit_sect_type wl1_srch_meas_db_init_mutex;

#define WL1_SRCH_INTERF_STORE_MEAS_DB_INTLOCK() REX_ISR_LOCK(&wl1_srch_interf_store_meas_db_mutex)
#define WL1_SRCH_INTERF_STORE_MEAS_DB_INTFREE() REX_ISR_UNLOCK(&wl1_srch_interf_store_meas_db_mutex)

#define WL1_SRCH_MEAS_DB_INIT_INTLOCK() REX_ISR_LOCK(&wl1_srch_meas_db_init_mutex)
#define WL1_SRCH_MEAS_DB_INIT_INTFREE() REX_ISR_UNLOCK(&wl1_srch_meas_db_init_mutex)
#endif /*FEATURE_WCDMA_FREE_FLOATING_TASK*/

#ifdef FEATURE_WCDMA_DC_HSUPA
/* Boolean to give status of whether DC configuration in Uplink or not*/
extern boolean srch_sec_ul_freq_configured;
#endif

#define SRCH_FILT_ENG_SCALING_FACTOR  512
typedef void (*SRCH_FULL_PN_SRCH_DONE_CB_FUNC_TYPE)(uint16,uint16);

extern SRCH_FULL_PN_SRCH_DONE_CB_FUNC_TYPE srch_full_pn_srch_cb_func ;

extern rex_crit_sect_type wl1_rxd_cb_crit_sect;

#define WL1_RXD_CB_INTLOCK() REX_ISR_LOCK(&wl1_rxd_cb_crit_sect);
#define WL1_RXD_CB_INTFREE() REX_ISR_UNLOCK(&wl1_rxd_cb_crit_sect);

/*NBR processing states */
typedef enum
{
  WSRCH_NBR_PROC_INACTIVE,
  WSRCH_NBR_PROC_IN_PROG,
  WSRCH_NBR_PROC_COMPLETED,  
}wsrch_nbrej_proc_state_enum_type;

 /*! NBR tunable parameters through NV item also the async parameters written to fw*/
typedef struct
{
 boolean enabled;
 uint32 rotatorFreq;  /* rotator frequency to be programmed to RXFE_NB_CSR_FREQ_OFFSET_NB0 */
 uint8 accumBpg;     /*! num of bpgs to accumulate for NBEE */
 int8 threshold1;    /* threshold1 - XXX */
 int8 threshold2;    /* threshold1 - XXX */

}wsrch_nbrej_config_params;

 /*NBR result states */
 typedef enum
 {
   WSRCH_SRCH_NBR_RSLT_W_SIGNAL,
   WSRCH_SRCH_NBR_RSLT_G_SIGNAL,
   WSRCH_SRCH_NBR_RSLT_INVALID
 }wsrch_srch_nbrej_result_enum_type;

/* NBR database */
typedef struct
{
  /*NBR current state */
  wsrch_nbrej_proc_state_enum_type state;
  /* info whether RxDiv module is using the div chain or not. if using then NBR cant use it*/
  boolean rxd_enabled;
  /* keep track of the time when the NBR was started. Keeps track if f/w
  responding in time */
  uint16 start_ts;

  wsrch_srch_nbrej_result_enum_type result;
  /* if for some reason we decide to stop ACQ after NBR has been triggered, it helps to
  send the ACQ_CNF at the end of NBR done. e.g RxAGC < thresh scenario*/
  boolean pending_acq_cnf;
  /* saves the RxLM buffer index allocated for NBR */
  uint32 rxlm_idx;
  /* NBR NV parameters and also written to f/w async interface */
  wsrch_nbrej_config_params config_params;
  /* A global to check whether nbrej has performed or not */
  /*This will be set when we do NBR and reset ONLY on ACQ success*/
  /*After ACQ success based on this LM settings are configured for FET mode*/
  boolean nbrej_performed;
}wsrch_nbrej_db_struct_type;

extern wsrch_nbrej_db_struct_type wsrch_nbrej_db;
extern boolean wsrch_nbr_debug_msg_enable;

/* Minimum time for an NBR WRM Request in SCLK time set for 6ms*/
#define WSRCH_NBR_MIN_REQUEST_TIME_IN_SCLK   ((6 * TIMETICK_NOMINAL_FREQ_HZ)/1000)

/*whether NBR in progress or not*/
#define WSRCH_NB_REJ_PROC_ACTIVE()  (wsrch_nbrej_db.state == WSRCH_NBR_PROC_IN_PROG)

/*whether NBR is in inactive or not*/
#define WSRCH_NB_REJ_PROC_INACTIVE()  (wsrch_nbrej_db.state == WSRCH_NBR_PROC_INACTIVE)


/*Whether NBR can be enabled in these states or not */
#define WSRCH_IS_NB_REJ_BE_ALLOWED() ((wsrch_nbrej_db.config_params.enabled) &&              \
   ((search_state == SRCH_ACQ_STATE) ||(search_state == SRCH_BPLMN_STATE)) &&                \
    (!wl1_rxd_in_div_only_mode())) 

extern srch_nv_struct_type wl1_srch_nv;
extern mcalwcdma_srch_step1_result_buffer_type srch_step1_result_buf;

#ifdef FEATURE_QSH_EVENT_METRIC 

#define SRCH_PRIMARY_ANTENNA 0
#define SRCH_DIVERSITY_ANTENNA 1
#define SRCH_MAX_ANTENNA 2

typedef struct
{
  int32 rssi[SRCH_MAX_ANTENNA];
  int32 ecio[SRCH_MAX_ANTENNA];
  int32 rscp[SRCH_MAX_ANTENNA];
} srch_cell_info_qsh_data_struct_type;

  srch_cell_info_qsh_data_struct_type srch_cell_info_qsh_data;
#endif /* FEATURE_WCDMA_QSH_EVENT_METRIC  */


/*============================================================================
                          CELL SELECTION 
============================================================================*/

/* Cell list structure for Cell Selection evaluation. */
typedef struct
{
  /* Cell Selection frequency requested by RRC */
  uint16   freq;

  /* Primary Scrambling Code */
  uint16   scr_code;

  /* Flag indicating if the cell is searched or not.
   * Currently, if the cell timing is unknown, the cell
   * will not be searched for cell selection evaluation
   */
  boolean  evaluated;

  /* Index (or pointer) to the corresponding cell table entry */
  srch_cell_struct_type *cell_ptr;

} srch_cell_select_list_type;

/* Data structure to store cell list for cell selection */
extern srch_cell_select_list_type  srch_cell_select_list;

/*The number of list searches issued since the CPHY_CELL_SELECTION_REQ has been 
  received by Layer 1. */
extern uint16 srch_cell_selection_list_search_cnt ;

/*A flag used to indicate to the Layer 1 code that indicates whether 
  CPHY_CELL_SELECTION_REQ has been received from RRC.*/
extern boolean srch_cphy_cell_selection_recvd ;

/* Define for Cell Selection TimeOut in number of frames -- This is the
   duration of the time for which list search will be issued periodically
   using the search heart beat event (ASETsearch will be issued) for
   calculating the S criteria..*/

#define SRCH_CELL_SELECTION_TIMEOUT 20

#define SRCH_SCRIT_FAIL_CNT_RXD_TRIG 2 

/* Default minimum values for cell selection S values */
#define SRCH_SQUAL_DEFAULT_MIN (-32767)
#define SRCH_SRXLEV_DEFAULT_MIN (-32767)


#define CELL_SEL_TIMEOUT( )                                     \
  ( srch_cell_selection_list_search_cnt*SRCH_ASET_SRCH_PERIOD \
      >= SRCH_CELL_SELECTION_TIMEOUT )

typedef enum
{
  SRCH_NV_E1D_LONE_CELL       =  0x0 , //0
  SRCH_NV_STEP1_PEAK_PRUNER   =  0x1 , //0
  // MAX cannot exceed 16 since the var is uint16
  SRCH_NV_MAX                 =  0x2
}srch_enhnc_ctrl_enums;


/*===========================================================================
                          FUNCTION PROTOTYPES
                          
The followings are prototypes of functions exported to external modules.
===========================================================================*/


/*===========================================================================

FUNCTION SRCH_HPQ_ISR

DESCRIPTION
  This function is the interrupt handler for Searcher HPQ interrupt when HPQ
  search is done.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_hpq_isr ( void );

/*===========================================================================

FUNCTION SRCH_LPQ_ISR

DESCRIPTION
  This function is the interrupt handler for Searcher LPQ interrupt when LPQ
  search is done.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_lpq_isr ( void );

/*===========================================================================

FUNCTION SRCH_HEAP_MEM_FREE

DESCRIPTION
  This function performs frees searcher heap memory initializations. 

DEPENDENCIES
  srch_heap_mem_alloc must have been called beforehand, or this function
  will do nothing.

RETURN VALUE
  None.

SIDE EFFECTS
  srch_pn_bypass_srch_params and srch_pn_result_buf will be set to NULL.

===========================================================================*/
extern void srch_heap_mem_free(void);

/*===========================================================================

FUNCTION SRCH_HEAP_MEM_ALLOC

DESCRIPTION
  This function performs allocates searcher heap memory initializations. 

DEPENDENCIES
  srch_heap_mem_allocated but be FALSE, or this functiom will do nothing.

RETURN VALUE
  None.

SIDE EFFECTS
  srch_pn_bypass_srch_params and srch_pn_result_buf will be set to NULL.

===========================================================================*/
extern void srch_heap_mem_alloc(void);

/*===========================================================================

FUNCTION SRCH_GET_REF_CELL_SCR_CODE

DESCRIPTION
  This function retruns active cell's primary scrambling code. 

DEPENDENCIES
  None.

RETURN VALUE
  Active cell's primary scrambling code.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 srch_get_ref_cell_scr_code ( void );

/*===========================================================================

FUNCTION SRCH_GET_CPICH_RSCP

DESCRIPTION
  This function retruns serving cell's RSCP.

DEPENDENCIES
  None.

RETURN VALUE
  Serving cell's RSCP

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 srch_get_cpich_rscp( void );

/*===========================================================================

FUNCTION SRCH_GET_SAVED_RSCP

DESCRIPTION
  This function returns current frequency on which UE is camped.

DEPENDENCIES
  None.

RETURN VALUE
  Current frequency

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 srch_get_curr_freq(void) ;

#ifdef FEATURE_WCDMA_DC_HSDPA
/*===========================================================================

FUNCTION SRCH_GET_CURR_FREQ_SEC_CARR

DESCRIPTION
  This function returns current frequency of the secondary carrier.

DEPENDENCIES
  None.

RETURN VALUE
  Current frequency of secondary carrier

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 srch_get_curr_freq_sec_carr(void);
#endif /* FEATURE_WCDMA_DC_HSDPA */

/* Check_again : Write the description of this function*/
extern uint16 srch_clamp_filt_eng(uint16 old_val, uint16 new_val, uint16 clamp_limit);

/*===========================================================================

FUNCTION SRCH_GET_FILTERED_RSCP

DESCRIPTION    
  This function returns an unsigned value representing the current,
  filtered RSCP value in P(AGC) + Ec/Io form for all ASET cells. 
  
  The returned can then be arbitrarily mapped to UI display space 
  using an equation, table, or case statement.  
  
  The RSCP is filtered using an (N-1)/N IIR filter for a smoother signal
  strength display. 

DEPENDENCIES
  
    
RETURN VALUE
  An unsigned filtered RSCP value, in 0.5 dBm steps. The actual RSCP is
  -(Returned-unsigned-RSCP-value/2)dBm.

SIDE EFFECTS
  Filtered RSCP is updated.

===========================================================================*/
extern uint16 srch_get_filtered_rscp(void);

/*===========================================================================

FUNCTION SRCH_COMPUTE_RSCP

DESCRIPTION
  This function calculates the RSCP given the ecIo and the RxAGC..
  The calculated RSCP value will be returned in uint16 units. The actual RSCP
  is -ve of the returned value.

DEPENDENCIES
  The cell's ecio have been computed.

RETURN VALUE
  uint16.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 srch_compute_rscp( uint16 ecio, int16 rx_agc);

/*===========================================================================

FUNCTION      SRCH_ASYNC_PARM_INIT

DESCRIPTION
  This function initializes searcher asynchronous parameters depending
	on the current state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  srch_step1_parms, srch_step2_parms, srch_step3_parms variables
	are initialized.

===========================================================================*/
extern void srch_async_parm_init( void );

/*===========================================================================

FUNCTION      SRCH_ASYNC_PARM_INIT_IN_PCH_SLEEP

DESCRIPTION
        This function initializes searcher asynchronous parameters in PCH_SLEEP state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void srch_async_parm_init_in_pch_sleep( void );

/*===========================================================================
FUNCTION      SRCH_ASYNC_PARM_UPDATE_FOR_PN_IN_PCH_SLEEP

DESCRIPTION
      This function updates the flag srchPNIdleInterfFlag in
      srch PN async param FW interface to value

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
extern void srch_async_parm_update_for_pn_in_pch_sleep(uint8 value);

/*===========================================================================
FUNCTION      SRCH_QUERY_ASYNC_PARM_VAL_FOR_INTERF_PN

DESCRIPTION
      This function returns the value of flag srchPNIdleInterfFlag in
      srch PN async param FW interface. 

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
extern uint8 srch_query_async_parm_val_for_interf_pn(void);

/*===========================================================================

FUNCTION SRCH_EVENT_TIMER_ELAPSED

DESCRIPTION
  This function returns the time elapsed from start_ts passed to it.

DEPENDENCIES
  start_ts should be initialized and updated correctly.

RETURN VALUE
  Time elapsed from start_ts.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 srch_event_timer_elapsed( uint16 start_ts );

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION        NSET_PROCESS_LOG_ON_DEMAND

FILE NAME       srch.c

DESCRIPTION     This funtion creates a NSET log packet and submits it
                to QXDM

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void nset_process_log_on_demand(void);
#endif

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION        ASET_PROCESS_LOG_ON_DEMAND

FILE NAME       srch.c

DESCRIPTION     This funtion creates a ASET log packet and submits it
                to QXDM

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void aset_process_log_on_demand(void);
#endif

/*===========================================================================

FUNCTION SRCH_HSET_SRCH_CMD

DESCRIPTION
  This function updates searcher HPQ, if needed, based on aset information and 
  issues a search command to DSP. 

DEPENDENCIES
  ASET size is greater than 0, which should be always the case when this
  function is called. Otherwise, DSP behavior is unpredicable.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean  srch_hset_srch_cmd(void);

#ifdef FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE
/*===========================================================================

FUNCTION SRCH_CHANGE_ANTENNA

DESCRIPTION:
  Allows RxDiv controller to change the antenna

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Next step 1/2/3 search commands take place on new antenna
===========================================================================*/
extern void srch_change_antenna(uint8 new_ant_id);
#endif /* FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE */

/*===========================================================================

FUNCTION SRCH_WAKEUP_CELLS

DESCRIPTION:
  API called by DRX manager to wakeup cells. This API can be called in any
  L1 state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Wakeup list search may get triggered in certain states
===========================================================================*/
extern void srch_wakeup_cells(srch_wakeup_set_struct_type wakeup_set_info);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION SRCH_WAKEUP_CELLS_NON_DRX_STATE

DESCRIPTION:
  API called by DRX manager to wakeup cells. This API can be called in any
  L1 states except PCH/PCH_SLEEP.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Wakeup list search may get triggered in certain states
===========================================================================*/
extern void srch_wakeup_cells_non_drx_state(srch_wakeup_set_struct_type wakeup_set_info);
#endif

/*===========================================================================

FUNCTION SRCH_WAKEUP_CELLS_DONE

DESCRIPTION:
  API called by SRCH to respond to wakeup request from DRX
  L1 state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_wakeup_cells_done(srch_wakeup_set_struct_type wakeup_set_info);

/*===========================================================================

FUNCTION SRCH_ENTITY_DROP

DESCRIPTION:
  API called by DRX to drop an entity

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_entity_drop(uint32 bmask_rem_entities,int16 dropped_handle);


#ifdef FEATURE_WCDMA_FS_FALSE_PEAK_ELIM_OPT
/*===========================================================================

FUNCTION SRCH_FS_PROC_DONE_ISR

DESCRIPTION
  This function handles the FS_PROC_DONE interrupt from mDSP

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_fs_proc_done_isr ( void );
#endif/*FEATURE_WCDMA_FS_FALSE_PEAK_ELIM_OPT*/

/*===========================================================================

FUNCTION SRCH_STEP1_SRCH_CMD

DESCRIPTION:
  - fill all the parameters to srch_step1_sync_parms
  - call the MCAL api to issue srch
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srch_step1_srch_cmd(boolean updt_sync_parms, boolean sec_carrier);


/*===========================================================================

FUNCTION SRCH_NASTT_UNMONITORED_SRCH_CMD

DESCRIPTION:
    issues a NASTT unmonitored search cmd on the 'pos_arr' passed. 'pos_arr' will 
    be the positions of the step1 peaks on which NASTT search will be issued.
    
  - fill all the parameters to srch_nastt_sync_parms
  - call the MCAL api to issue srch
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srch_nastt_unmonitored_srch_cmd( uint32 (*pos_arr)[2], uint16 num_peaks, boolean sec_carrier);

/*===========================================================================

FUNCTION SRCH_NASTT_ASYNC_ADD_CELLS_FOR_SRCH

DESCRIPTION:
  add the cells which are going to be detected in NASTT async
  search. the PSCs from these cells are going to be passed on
  to fw aongwith the search cmd.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern void srch_nastt_async_add_cells_for_srch
(
  srch_cell_struct_type  **cell_pptr, 
  uint16 cell_cnt, 
  boolean reset
);

/*===========================================================================

FUNCTION SRCH_NASTT_ASYNC_ADD_CELLS_FOR_SRCH

DESCRIPTION:
  add the cells which are going to be detected in NASTT async
  search. the PSCs from these cells are going to be passed on
  to fw aongwith the search cmd.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

/*
extern void srch_nastt_async_add_interf_cells_for_srch
(
  srch_interf_cell_struct_type  **cell_pptr, 
  uint16 cell_cnt, 
  boolean reset
);
*/
/*===========================================================================

FUNCTION SRCH_NASTT_ASYNC_SRCH_CMD

DESCRIPTION:
  - fill all the parameters to srch_step1_sync_parms
  - call the MCAL api to issue srch
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srch_nastt_async_srch_cmd( uint32 *pos_arr, uint16 num_peaks);

/*===========================================================================

FUNCTION SRCH_NASTT_SRCH_CMD

DESCRIPTION
  This function issues a NASTT search cmd (both async & Unmonitored). details below
  1) pick the num_step1_peaks for NASTT serch
  2) Advance srchsched_db.step1_nastt_params.cur_psc_peak_indx by the number of peaks currently searched
  3) if its a NASTT(async) then use the async_cell_mask to pick the psc
     which are sent the NASTT cmd. 
  4)For unmonitored NASTT we dont use asyn_cell_mask
  
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - indicates srch cmd was sent successfully.
  FALSE - indicates srch cmd could not be sent successfully.
  
SIDE EFFECTS
  srchsched_db.step1_nastt_params.cur_psc_peak_indx is advanced by the number of step1 peaks which are being sent for
  NASTT search here

===========================================================================*/
extern boolean srch_nastt_srch_cmd
( 
  srch_full_srch_type_enum_type nastt_type, 
  uint16 async_cell_mask,
  uint16 num_step1_peaks,
  boolean sec_carrier  
);

/*===========================================================================

FUNCTION SRCH_PN_ADD_INTERF_CELLS_FOR_SRCH

DESCRIPTION:
  Adds the inter freq cells for PN search. This PN search will be used in DRX state
  when the RF will be tuned to a diffrerent freq and normal searches will be issued
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

//extern void srch_pn_add_interf_cells_for_srch
//(
//  srch_interf_cell_struct_type  **cell_pptr,
//  uint16 cell_cnt,
//  uint16 max_tasks,
//  boolean reset
//);

/*===========================================================================

FUNCTION SRCH_UPDATE_PN_SRCH_CMD

DESCRIPTION:
    This function issues a PN search cmd on the cells given by 'cell_mask'
  - process cell_mask and add the corresponding cells to the srch_pn_srch_set_ptr
  - call function srch_pn_srch_cmd() to set sync params for the pn srch cmd.
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srch_pn_send_srch_cmd(uint32 cell_mask);

extern boolean srch_pn_send_srch_cmd_for_asu(uint32 cell_mask);

/*=========================================================================

FUNCTION     : SRCHACQ_PROGRAM_FULL_PN_SRCH

DESCRIPTION  : This function  programs a full PN circle search.

DEPENDENCIES : None

RETURN VALUE : None.

SIDE EFFECTS :

=========================================================================*/
extern void srch_pn_program_full_pn_srch 
(
  uint16 freq,
  uint16 scr_code, 
  boolean txd,
  srch_cell_txd_det_status txd_status,
  uint16 num_task,
  SRCH_FULL_PN_SRCH_DONE_CB_FUNC_TYPE full_pn_srch_cb
);


/*===========================================================================

FUNCTION SRCH_UPDATE_PN_SRCH_CMD

DESCRIPTION:
    This function send a PN search cmd for the inter freq cells
  - fill all the parameters to srch_pn_sync_parms
  - call the MCAL api to issue srch
  
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srch_pn_srch_cmd_interf_cells(boolean update,uint8 pd, uint8 rxd);


/*===========================================================================

FUNCTION SRCH_STEP1_GET_RESULT

DESCRIPTION
  call MCAL SRCH api to get the result in srch_step1_result_buf
  in case the RxD mode 1 is used - combine the sorted peaks from
  both the primary and diversity antenna and make a single sorted 
  list in srch_psc_peaks[]

  run duplicate peak detection mechanism to remove duplicate peaks

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_step1_get_result (void);


/*===========================================================================

FUNCTION SRCH_NASTT_GET_RESULT

DESCRIPTION:
 
 get the NAS-TT peaks from fw through MCAL in srch_nastt_result_buf[]. 
 get the BEST energy peak for a scr code and store that in srch_nastt_pilots[]
 (peaks can be from both Rx antennas in case of RxD mode 1. Pick the strongest 
 among antennas and use it as ref pos for PN srch next time)
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern void srch_nastt_get_result(void);

/*===========================================================================

FUNCTION SRCH_PN_GET_RESULT

DESCRIPTION:
 
 - get the PN peaks from fw through MCAL in srch_pn_result_buf[]. 
 - process srch_pn_result_buf[] and populate the cell structure with
   the results
   (peaks can be from both Rx antennas in case of RxD mode 1)
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern void srch_pn_get_result(void);

/*===========================================================================

FUNCTION SRCH_DETERMINE_RXD_MODE

DESCRIPTION:
  - get the RxD mode as per the current reccomendation 
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern uint16 srch_determine_rxd_mode(srch_step_enum_type srch_step);

/*===========================================================================

FUNCTION SRCH_CHECK_IF_FULL_SRCH_WITH_NASTT_ASYNC_NEEDED

DESCRIPTION:
 check if we want to schedule a full search with NASTT async or can we skip this
 in case we dont have any neighbors.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srch_check_if_full_srch_with_nastt_async_neeeded(void);

/*===========================================================================

FUNCTION srch_check_if_any_srch_ongoing

DESCRIPTION
  Depending on the srch step it will check the corresponding que if any srch ongoing.
  This function is here to hide the que level informaition in a single function rather
  than distributed across all srch* files. Later if non OTDOA PN que become separate we need
  to change the logic at this place 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srch_check_if_any_srch_ongoing(srch_step_enum_type srch_step, boolean otdoa);

/*===========================================================================

FUNCTION      SRCH_INTRAF_UPDATE_ENG_PARAMS_POST_FULL_SRCH

DESCRIPTION
  This function updates pos/eng paramsafter a full search. Also calculates
  the ECIO and filt ECIO of the cells that were discovered during full search .

DEPENDENCIES
  This should be called after a NASTT result has already been populated to
  cell structures..

RETURN VALUE
  None.

SIDE EFFECTS
  
===========================================================================*/
extern void srch_intraf_update_eng_params_post_full_srch
(
  /* Cell pointer to intra-frequency cell */
  srch_cell_struct_type *cell_ptr,
  uint16 pilot_idx
);

/*===========================================================================

FUNCTION SRCH_NONACQ_CONFIG_SEARCHER_ROTATOR

DESCRIPTION:
  Configure searcher rotator in non ACQ state through this function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_nonacq_config_searcher_rotator(int16 rotator);

/*===========================================================================

FUNCTION SRCH_LOG_SRCH_RESULT_STEP1

DESCRIPTION:
  Step 1 log packet version 10

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_log_srch_result_step1(void);

/*===========================================================================

FUNCTION SRCH_LOG_SRCH_RESULT_NASTT

DESCRIPTION:
  Log the NASTT packet

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_log_srch_result_nastt(void);

/*===========================================================================

FUNCTION srch_log_srch_result_pn

DESCRIPTION:
  Log the PN log packet for intra-freq searches

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_log_srch_result_pn(void);

/*===========================================================================

FUNCTION srch_log_pn_no_rxd_normal_srch_result

DESCRIPTION:
  Log the PN log packet (No RxD + normal search)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_log_pn_no_rxd_normal_srch_result(srch_cell_type_enum_type srch_type);

/*===========================================================================

FUNCTION srch_log_pn_rxd_normal_srch_result

DESCRIPTION:
  Log the PN log packet (RxD + normal search)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_log_pn_rxd_normal_srch_result(srch_cell_type_enum_type srch_type);

/*===========================================================================

FUNCTION srch_log_pn_no_rxd_shoulder_srch_result

DESCRIPTION:
  Log the PN log packet (No RxD + shoulder search)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_log_pn_no_rxd_shoulder_srch_result(srch_cell_type_enum_type srch_type);

/*===========================================================================

FUNCTION srch_log_pn_no_rxd_normal_srch_result

DESCRIPTION:
  Log the PN log packet (RxD + shoulder search)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_log_pn_rxd_shoulder_srch_result(srch_cell_type_enum_type srch_type);

/*===========================================================================

FUNCTION srch_log_interf_srch_result_pn

DESCRIPTION:
  Log the PN log packet for inter-freq searches

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_log_interf_srch_result_pn(void);

#ifdef FEATURE_DC_HSDPA_LOGGING
/*===========================================================================
FUNCTION SRCH_LOG_PN_BYPASS_PEAK_SRCH_RESULT

DESCRIPTION:
  Log the PN bypass peak log packet 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_log_pn_bypass_peak_srch_result(void);
#endif /* FEATURE_DC_HSDPA_LOGGING */

/*===========================================================================

FUNCTION SRCH_PN_GET_RESULT_INTERF_CELLS

DESCRIPTION:
 
 - get the PN peaks from fw through MCAL in srch_pn_result_buf[]. 
 - process srch_pn_result_buf[] and populate the cell structure with
   the results
   (peaks can be from both Rx antennas in case of RxD mode 1)
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_pn_get_result_interf_cells(void);

/*===========================================================================

FUNCTION srch_nastt_async_process_peaks

DESCRIPTION:
 
 get the NAS-TT peaks from fw through MCAL in srch_nastt_result_buf[]. 
 get the BEST energy peak for a scr code and store that in srch_nastt_pilots[]
 (peaks can be from both Rx antennas in case of RxD mode 1. Pick the strongest 
 among antennas and use it as ref pos for PN srch next time)
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_nastt_async_process_peaks(void);

/*===========================================================================

FUNCTION SRCH_PN_SRCH_CMD

DESCRIPTION:
  - fill all the parameters to srch_pn_sync_parms
  - call the MCAL api to issue srch
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_pn_srch_cmd(boolean update,uint8 pd, uint8 rxd);
extern void srch_set_pn_sync_params(uint8 pd, uint8 rxd);
extern void srch_pn_add_unique_cells_for_srch(srch_cell_struct_type  **cell_pptr, uint16 cnt, uint16 max_tasks);

extern int16 srch_get_freq_offset(void);

/*===========================================================================
FUNCTION WSRCH_GET_PN_RESULTS_TASKS_CNT

DESCRIPTION
    This function will be used to return the number of tasks from the last PN search issued

INPUT
    None

DEPENDENCIES
    None.

RETURN VALUE
    Number of tasks

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 wsrch_get_pn_result_tasks_cnt(void);

/*===========================================================================
FUNCTION SRCH_GET_FRAME_PEAK_DIFF

DESCRIPTION
    Given 2 frame peaks, this function computes the PN position(in cx1) between
    the two peaks.

INPUT
    PN position (in cx1) of the 2 frame peaks.

DEPENDENCIES
    PN position passed to this function MUST BE in cx1 units!!.

RETURN VALUE
   PN position (in cx1) between the two peaks.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint32 srch_get_frame_peak_diff(uint32 pn1, uint32 pn2);


/*===========================================================================
FUNCTION WSRCH_GET_PN_RESULTS_PEAKS_CNT

DESCRIPTION
    This function will be used to return the number of peaks per task from the last PN search issued

INPUT
    Task index

DEPENDENCIES
    None.

RETURN VALUE
    Number of peaks

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 wsrch_get_pn_result_peaks_cnt(uint8 task_idx);


/*===========================================================================

FUNCTION SRCH_CREATE_DUMMY_CELL

DESCRIPTION
  This function creates a dummy cell that can be added to a search set for
  debugging purposes.

DEPENDENCIES
  None.

RETURN VALUE
  A pointer to the newly-created dummy cell on success, NULL on failure.

SIDE EFFECTS
  None.

===========================================================================*/
extern srch_cell_struct_type* srch_create_dummy_cell(void);

/*===========================================================================

FUNCTION SRCH_RESET_SRCH_SUBSTATE

DESCRIPTION
  This function resets the search DCH substate when the search state is DCH.

DEPENDENCIES
  None.

RETURN VALUE
  True if the substate was reset, false otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_reset_srch_substate(void);

/*===========================================================================

FUNCTION SRCH_RESTORE_SRCH_SUBSTATE

DESCRIPTION
  This function restores the search DCH substate to the value stored in          
  srch_dch_reset_substate for the purpose of resuming a search.  

DEPENDENCIES
  None.

RETURN VALUE
  True if the UE is in DCH and the state was not inactive, false otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srch_restore_srch_substate(void);

/*===========================================================================
FUNCTION SRCH_ACQ_CHECK_UE_IN_X2W_HHO

DESCRIPTION
  This function calls the rrc api to determine if ue is in x2w hho.

DEPENDENCIES
  None.

RETURN VALUE
 TRUE if UE is in X2W HHO, FALSE if not.
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srch_acq_check_ue_in_x2w_hho(void);

/*===========================================================================
FUNCTION SRCH_ASYNC_PARM_UPDATE_IN_PCH_SLEEP

DESCRIPTION
  This function updates the search parms

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srch_async_parm_update_in_pch_sleep(uint16 freeze_flag, boolean usefreeze_flag, boolean rf_flag, srch_intraf_queue_enum_type queue_to_update);

/*===========================================================================
FUNCTION SRCH_ACQ_CHECK_UE_IN_X2W_HHO

DESCRIPTION
  This function calls the rrc api to determine if ue is in x2w hho.

DEPENDENCIES
  None.

RETURN VALUE
 TRUE if UE is in X2W HHO, FALSE if not.
SIDE EFFECTS
  None.
===========================================================================*/

extern boolean srch_acq_check_ue_in_x2w_hho(void);

/*===========================================================================
FUNCTION        SRCH_GET_SRCH_UTILIZATION_FOR_MCVS

FILE NAME       WSRCH.C

DESCRIPTION     This function calculates the searcher utilization based on the parameters passed in and based on other
                          parameters present and it returns the searcher utilization in percentage form.

DEPENDENCIES    

RETURN VALUE    uint8 searcher utilization in percentage (acceptable values 0 - 100)

SIDE EFFECTS    
===========================================================================*/
extern uint8 srch_get_srch_utilization_for_mcvs(
  uint16 c0_srch_period,
  uint32 clk
);

/*===========================================================================
FUNCTION        SRCH_GET_FS_PERIOD_FOR_C0

FILE NAME       WSRCH.C

DESCRIPTION     This function returns the full search period for carrier 0.

DEPENDENCIES    

RETURN VALUE    full search period for c0

SIDE EFFECTS    
===========================================================================*/
extern uint16 srch_get_fs_period_for_c0(void);

/*===========================================================================

FUNCTION SRCH_CHECK_RXD_IMBALANCE_THRESH_CONDITIONS

DESCRIPTION
  This function will get called every full DCH cycle in order to set the Step1
  energy threshold correctly. This function will return True if either one of
  the below conditions is met, and False otherwise:
  1. the searcher EcIo's from all ASET cells are below -18 dB
  2. the RxAGC level from either Rx is less than -95 dBm and their RxAGC level
  difference is greater than 5 dB.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srch_check_rxd_imbalance_thresh_conditions(void);

/*===========================================================================

FUNCTION SRCH_SET_STEP1_ENG_THRESH

DESCRIPTION
  This function sets the Step1 energy threshold based on the boolean flag.
  If the flag is set to true, the threshold will be -24.6 dB, otherwise it
  will be -23.3 dB.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_set_step1_eng_thresh(boolean lower_thresh);

/*===========================================================================

FUNCTION      SRCH_NASTT_ASYNC_PARM_INIT_DURING_CM

DESCRIPTION
  writes to fw that step1 for intra freq outside CM gaps will be using RxD mode 2 during CM. All
  other step1 async params remain same

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void srch_nastt_async_parm_init_during_cm(void);

extern void srch_prepare_abort_all(void);
extern void srch_complete_abort_all (void);

/*===========================================================================

FUNCTION SRCH_PN_ADD_CELLS_FOR_PN_SRCH

DESCRIPTION:
    This function issues a PN search cmd on the cells given by 'cell_mask'
  - process cell_mask and add the corresponding cells to the srch_pn_srch_cell_ptr[]
  - call function srch_pn_srch_cmd() to set sync params for the pn srch cmd.
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_pn_add_cells_for_pn_srch(uint32 cell_mask);

/*===========================================================================

FUNCTION SRCH_USE_RGS_ROTATOR

DESCRIPTION:
  Returns if RGS values can be used/discarded

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_use_rgs_rotator(tcxomgr_vco_indicator_type vco_type);

/*===========================================================================

FUNCTION SRCH_CARR_ACTIVE

DESCRIPTION
  This function returns true when the specified carrier is in active state

DEPENDENCIES
  State is determined by SRCH ASET count of the carrier

RETURN VALUE
  TRUE: Carrier is active
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srch_carr_active( uint8 carr_idx );

/*===========================================================================
FUNCTION      SRCH_ASYNC_PARM_INIT_STEP1_THRESH_ON_CPC_START

DESCRIPTION

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
extern void srch_async_parm_init_step1_thresh_on_cpc_start(void);

/*===========================================================================
FUNCTION      SRCH_ASYNC_PARM_INIT_ON_CPC_STOP

DESCRIPTION

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
extern void srch_async_parm_init_step1_thresh_on_cpc_stop(void);

/*===========================================================================

FUNCTION   SRCH_GET_CARR_IDX_BASED_ON_FREQ

DESCRIPTION
  This function will return the carrier index of DL frequency specified.

DEPENDENCIES
  None.

PARAMETERS
  UARFCN to find the carrier index of

RETURN VALUE
  Carrier Index of DL UARFCN specified.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 srch_get_carr_idx_based_on_freq(uint16 freq);

/*===========================================================================

FUNCTION     SRCH_GET_CARR_FREQ

DESCRIPTION
  This function will return the DL UARFCN of the carrier specified

DEPENDENCIES
  None.

PARAMETERS
  Carrier index to resolve into frequency (UARFCN)
 
RETURN VALUE
  Downlink UARFCN of carrier.

SIDE EFFECTS
  None
===========================================================================*/
uint16 srch_get_carr_freq(uint8 carr_idx);

/*===========================================================================

FUNCTION     SRCH_IS_WKUP_SEARCH

DESCRIPTION
  This function is called by WSRCH before issuing a PN search to check if this 
  is a reacq search in which case a HPQ search is used

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  Boolean 
===========================================================================*/
extern boolean srch_is_wkup_search(void);

/*===========================================================================

FUNCTION     MCALWCDMA_CHECK_IF_SRCH_IS_ISSUED

DESCRIPTION
  This function will return if the search is issued in one of the queues

DEPENDENCIES
  None.

PARAMETERS
  srch type
 
RETURN VALUE
  boolean
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_check_if_srch_is_issued( srch_step_enum_type srch_type );

/*===========================================================================

FUNCTION     MCALWCDMA_SRCH_CHECK_IF_QUEUE_IS_BUSY

DESCRIPTION
  This function will return if the search is issued in the queue

DEPENDENCIES
  None.

PARAMETERS
  srch type
 
RETURN VALUE
  boolean
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_srch_check_if_queue_is_busy( srch_intraf_queue_enum_type queue );

/*===========================================================================

FUNCTION     MCALWCDMA_GET_SRCH_ISSUED

DESCRIPTION
  This function will return the search which is ongoing when they are single queue at  any point.

DEPENDENCIES
  None.

PARAMETERS
  srch type
 
RETURN VALUE
  boolean
  
SIDE EFFECTS
  None
===========================================================================*/
extern srch_step_enum_type mcalwcdma_get_srch_issued( void );

/*===========================================================================

FUNCTION     WL1_SRCH_E1D_ENHANCEMENT_NV_READ

DESCRIPTION
  This function will return the DL UARFCN of the carrier specified

DEPENDENCIES
  None.

PARAMETERS
  None
 
RETURN VALUE
  Downlink UARFCN of carrier.

SIDE EFFECTS
  None
===========================================================================*/
void wl1_srch_e1d_enhancement_nv_read(void);
/*===========================================================================

FUNCTION wl1_srch_print_e1d_enhancement_vars

DESCRIPTION


DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void wl1_srch_print_e1d_enhancement_vars(void);

/*===========================================================================

FUNCTION srch_handle_pn_srch_rxd_acq_bch

DESCRIPTION
  After Rxd turns ON, sched is moved to CONFIG_MODE to issue an ad-hoc PN search.
  After the PN search is done, move it back to INTRAF mode

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
extern void srch_pn_srch_done_rxd_acq_bch(void);

/*===========================================================================

FUNCTION        SRCH_CHECK_ANY_MCVS_SRCH_UTIL_PARAMS_CHANGED

FILE NAME       WSRCH.C

DESCRIPTION     This function checks if any of the parameters required to calculate the search utilization have changed
                         since the last full search.

DEPENDENCIES

RETURN VALUE    

SIDE EFFECTS
===========================================================================*/
extern void srch_check_any_mcvs_srch_util_params_changed(void);

/*===========================================================================
FUNCTION        SRCH_SET_SILVER_SRCH_CYCLES

FILE NAME       WSRCH.C

DESCRIPTION     This function checks if any of the parameters required to calculate the search utilization have changed 
                         since the last full search.

DEPENDENCIES    

RETURN VALUE    TRUE if at least one parameter has changed in the last search cycle, FALSE otherwise.

SIDE EFFECTS    
===========================================================================*/
extern void srch_set_silver_srch_cycles(uint8 new_cycles);

/*===========================================================================
FUNCTION        SRCH_GET_SILVER_SRCH_CYCLES

FILE NAME       WSRCH.C

DESCRIPTION     This function checks if any of the parameters required to calculate the search utilization have changed
                         since the last full search.

DEPENDENCIES

RETURN VALUE    TRUE if at least one parameter has changed in the last search cycle, FALSE otherwise.

SIDE EFFECTS
===========================================================================*/
extern uint8 srch_get_silver_srch_cycles(void);

/*===========================================================================

FUNCTION      wcdma_get_rat_meas_info

DESCRIPTION
This is the API function that CM that will call to get WCDMA serving and neighbor cell info

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void wcdma_get_rat_meas_info(cmapi_wcdma_signal_info_struct_type* wcdma_signal_info);

/*===========================================================================

FUNCTION      wcdma_is_rat_meas_info_ready

DESCRIPTION
This function returns the status of "wcdma_rat_meas_info_ready" variable

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
boolean wcdma_is_rat_meas_info_ready(void);

extern boolean srch_unlock_cb_for_meas(wrm_unlock_event_enum_type event);
extern boolean srch_unlock_cb_for_nbr(wrm_unlock_event_enum_type event);



/*===========================================================================

FUNCTION wsrch_nbrej_async_parm_init

DESCRIPTION
  This function writes NBR async parameters to f/w interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void wsrch_nbrej_async_parm_init(void);

/*===========================================================================

FUNCTION wsrch_nbrej_proc_check_fw_unresponsiveness

DESCRIPTION
  This function implements a guard timer mechanism through HB timer
  if f/w doesnt respond to NBR processing .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean wsrch_nbrej_proc_check_fw_unresponsiveness(void);

/*===========================================================================

FUNCTION WSRCH_NBREJ_RESET_DB

DESCRIPTION
  This function reset all the NBREJ parameters except the items obtained from NV
  and dont reset the Rxd info.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void wsrch_nbrej_reset_db(void);

/*===========================================================================

FUNCTION WSRCH_GET_NBREJ_RXLM_IDX

DESCRIPTION
  This function is a way to check if NBR proc is active and also returns
  the RxLM index.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint32 wsrch_get_nbrej_rxlm_idx(void);

/*===========================================================================

FUNCTION wsrch_nbrej_read_nv_params

DESCRIPTION
  This function reads the NBR NV params. It reads 5 parameters.
  NBR tunable parameters through NV item
 boolean enabled; -- NBR can be enabled or disabled.
 uint32 rotatorFreq  -- rotator frequency to be programmed to RXFE_NB_CSR_FREQ_OFFSET_NB0
 uint8 accumBpg -- num of bpgs to accumulate for NBEE
 uint8 threshold1 -- threshold1
 uint8 threshold2; -- threshold1

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void wsrch_nbrej_read_nv_params(void);

/*===========================================================================

FUNCTION wsrch_nbrej_proc_done_cmd_handler

DESCRIPTION
  This function handles the NBR done local cmd and process the rest of 
  the NBR done ISR from task context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void wsrch_nbrej_proc_done_cmd_handler();

/*===========================================================================

FUNCTION      srch_qta_start_cmd

DESCRIPTION
Initialize RxD search parmeteres on qta start command

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

extern void srch_qta_start_cmd(void);

/*===========================================================================

FUNCTION      srch_qta_stop_cmd

DESCRIPTION
Initialize RxD search parmeteres on qta stop command

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

extern void srch_qta_stop_cmd(void);

/*===========================================================================
 
 FUNCTION        WSRCH_NV_READ
 
 DESCRIPTION
   This function reads the Searcher NV
 
 DEPENDENCIES
 
 RETURN VALUE
 None
 
 SIDE EFFECTS
 
 ===========================================================================*/
extern void wsrch_nv_read();

 /*===========================================================================
 FUNCTION WSRCH_RETUNE_TO_CHANNEL
 
 DESCRIPTION
     This change is required only for Jolokia Modem.
     This function will be used in the case where NBR and FET coexists .This function will re tune RF
     to the channel input by 'freq'.After releasing RxLM setting on doing NBR this retune enable RxLM settings ready for FET
 
 INPUT
     target freq to which RF should be tuned to.
 
 DEPENDENCIES
     None.
 
 RETURN VALUE
    .
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
extern void wsrch_retune_to_channel(uint16 freq);

/*===========================================================================

FUNCTION SRCH_CELL_SEL_RPT

DESCRIPTION
  This function sends a report CPHY_CELL_SELECTION_CNF to RRC on cell
  selection search results.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srch_cell_sel_rpt (boolean lock_failure);

/*===========================================================================

FUNCTION SRCH_CELL_SELECT_SRCH_DONE

DESCRIPTION
  This function processes list search dump for cell selection in BCH state.
  This function retrieves the search results from the LPQ and computes
  the s_qual and s_rxlev Cell Selection criteria. It then calls a function
  to send the results to RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srch_cell_select_srch_done(void);

/*===========================================================================
 
 FUNCTION        SRCH_UPDATE_RUMI_FLAG_PWRUP_ACQ_FULL_WIN_PN
 
 DESCRIPTION
   This function is used to set full win pn search flag  and psc during RUMI bringup 
   through WPLT command
 
 DEPENDENCIES
 
 RETURN VALUE
 None
 
 SIDE EFFECTS
 
 ===========================================================================*/

extern void srch_update_rumi_flag_pwrup_acq_full_win_pn(uint16 scr_code);

/*===========================================================================
 
 FUNCTION        SRCH_ADD_DEBUG_EVENT
 
 DESCRIPTION
   This function can be used to add srch related debug information into mcalwcdma_srch_hist 
   queue
 
 DEPENDENCIES
 
 RETURN VALUE
 None
 
 SIDE EFFECTS
 
 ===========================================================================*/
extern void srch_add_debug_event(l1_local_cmd_enum_type  cmd_code, boolean discard);

#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================
 
 FUNCTION        WL1_SRCHER_METRIC_CB
 
 DESCRIPTION
  This function collects searcher info for QSH module.
 
 DEPENDENCIES
  None
 
 RETURN VALUE
  boolean
 
 SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_srcher_metric_cb(wl1_qsh_metric_e metric_id, uint8* addr_ptr, wl1_qsh_metric_action_enum_type action);
#endif

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
 
 FUNCTION        WL1_SRCHER_DUMP_CB
 
 DESCRIPTION
   This function collects searcher info during crash dump for QSH module
 
 DEPENDENCIES
  None
 RETURN VALUE
  None
 
 SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_srcher_dump_cb(void* addr_ptr);
#endif

#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================
 
 FUNCTION 		SRCH_GET_AVG_RSSI
 
 DESCRIPTION
   This function returns avg rssi
 
 DEPENDENCIES
 
 RETURN VALUE
 None
 
 SIDE EFFECTS
 
 ===========================================================================*/

extern int16 srch_get_avg_rssi(void);
 /*===========================================================================
 
 FUNCTION        SRCH_LOG_RSSI_FOR_AVG 
  
 DESCRIPTION
   This function uses circular buffer to log last 5 rssi.
 
 DEPENDENCIES
 
 RETURN VALUE
 None
 
 SIDE EFFECTS
 
 ===========================================================================*/

extern void srch_log_rssi_for_avg(void);
#endif

#ifdef FEATURE_WCDMA_QSH_EVENT_METRIC 
/*===========================================================================

FUNCTION srch_get_metric_for_cell_info

DESCRIPTION
  This function gets metrics for QSH

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_get_metric_for_cell_info(void);
#endif /* FEATURE_WCDMA_QSH_EVENT_METRIC  */

#endif /* SRCH_H */
