#ifndef DRX_H
#define DRX_H
/*===========================================================================

            L 1   D R X   R E L A T E D   D E C L A R A T I O N S

DESCRIPTION

EXTERNALIZED FUNCTIONS

drx_cmd
  This function is called by L1M when RRC commands L1 to begin DRX
  operation. This function calculates the Paging Occasion and the
  Paging Indicator (PI) value and calls the function to calculate
  the parameters that are needed to send to the mDSP to demodulate
  the PICH.

drx_rel 
  This function is called to release DRX operation (called when sccpch
  containing pich is released)
  
drx_pich_done_isr
  This ISR is called by tramp services when the MDSP_INT5 occurs.
  This function will read the piValue register from the mDSP to ascertain
  whether the PICH was successfully demodulated and if there was a page
  for the UE.
  If the UE was paged, the ISR will be de-registered.
  If the UE was not paged, the mDSP will be commanded to again demodulate
  the PICH after the next drx cycle.

drx_sleep_abort
  Aborts Sleep mode if in progress
  
drx_terminate
  This function is called when DRX mode is terminated.
  
drx_process_cmd
  This function processes the DRX sleep local command and implements 
  the DRX state machine
  
drx_sleep_period_complete
   This function is called to signal that the sleep period is complete and
   its time to wake up.

INTERNALIZED FUNCTIONS
drx_mdsp_parms
  This function calculates the parameters that are needed by the mDSP
  for PICH demodulation:
  piThreshold
  bitsPerPi
  piPnCount
  
  It also calculates the actual SFN and the slot when the mDSP command
  to demodulate the PI bits has to be sent.
  
drx_get_pi_pn_count
  This function calculates the pnCount (in units of 256 chips) in a frame
  where the UE is to begin demodulating the PICH. Range 0...143.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  PICH should be set up before DRX operation.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) (2002-2013) by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/drx.h_v   1.16   11 Jul 2002 21:26:56   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/drx.h#1 $ 
$DateTime: 2016/12/13 08:00:12 $ 
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/24/15   ap      Initial check-in for WL1 QSH framework 
10/08/15   nga      FR 23268: Configurable scaling of IDLE DRX for M2M devices (WCDMA)
                    FR 23280: Skipping IDLE mode measurements for M2M devices (WCDMA)
06/08/15   kr      Reset W2G BPLMN flag at the start of sleep
02/10/15   skk     Timeline fix for long <-> short QXT transisions during Intra f resel.
02/06/15   ar      Change to extern function for posting fake drx
01/22/15   skk     Wakeup 6 slots early during reselections to facilitate offline PICH to online PICH transitiion timeline.
01/08/15   skk     Fixing Compilation errors.
12/9/14    sks     Cleanup of outer loop adjustment for RxD.
12/11/14    sks     Log the time of prep wakeup done at the end of the call and not at power clock on.
12/05/14   rs      DRX PICH version 4 log pkt changes 
11/11/14   rs      Code changes for DRDSDS fallback mechanism - part 2
06/20/14   skk     FR19218: ReacqLessPICH(RLP) porting for JO.
10/09/14   sl      Added utility API functions to access Rxd mode
09/23/14   pv      APIs for supporting TRM swapping of chains for call setup in DR mode.
08/27/14   skk     Use uint32 for drx_delayed_timer_val variable.
08/12/14   sd      Fixed DSDS Medium and Low compiler warnings in WCDMA.
08/11/14   rs      Created a macro to indicate if panic search conditions are met           
08/04/14   rs      Fixed compiler warnings
07/03/14   sks     Changes for FCDC.
06/30/14   skk     Fixed bugs in DRX PICH log packet (0x41AA)
06/24/14   sks     Initialize ticks_rem_minus_target to 0 inside adjust_outer_loop.
04/01/14   skk     Donot adjust outerloop if RxD is enabled while go to sleep but FOFF at wakeup.
05/20/14   skk     Making drx_delayed_timer as cb based timer from signal based.
04/02/14   skk     Bringing back drx_delayed_timer.
05/22/14   gsk     Update rxd prev mode to NO DIV in case of AGC imbalance, to avoid triage operations
                   on rxd in parallel reacq cases
05/13/14   skk     Removing all includes of "tracer_event_ids.h" and replaced with local file.
05/05/14   rs      FR-3672 CTCH log packet changes. 
03/27/14   sm      OMRD changes to make RxD trm compliant
03/27/14   sks     Add a false page indication field to 0x41AA.
01/23/14   rs      Added Support to include FW parameters for pich log pkt
01/20/14   sks     Do not setup PICH if reacq failed.
12/16/13   skk     Call RRC api can l1 go to sleep in G2W TA mode as well.
12/19/13   rs      Prioritize searches unconditionally when S criteria or reacq fails
12/16/13   as      Porting G2W TA from Triton to Dime.
11/25/13   pv       API for computing mdsp parms after we know the next paging occasion.
11/12/13   sks     Increase the sample capture size by 9 bpgs for DS mode for offline PICH.
09/17/13   jkb     Porting Dual Sim from Triton to Dime
08/14/13   rs      Increase the CTCH gap index to accomodate 512 gaps
08/11/13   gsk     Externing a fucntion
08/06/13   gsk     Code cleanup: Removing unnecessary globals from Enhanced CTCH DB
07/29/13   sks     Inclusion of QDSS profile points for timelining.
07/15/13   gsk     FR 3672: CTCH enhancement changes
07/12/13   ar      Idle RxD enhancement changes for extended 3g coverage
06/24/13   pv      Remove PICH delay timer.
02/15/13   gsk     Definition for PICH demod timer of 30ms 
02/20/13   hk      Removed FEATURE_INTER_SIB_SLEEP
02/20/13   hk      Mainlined FEATURE_WCDMA_DL_ENHANCED
02/11/13   gsk/sr  EDRX Full Sleep initial check in.
02/11/13   pv      Removed unussed API.
01/21/13   pv      Featurization to take care of Triton compile.
01/21/13   pv      DRX PICH log pkt - Offline PICH support for Dime.
14/12/12   kr      Fixing Klwrk Error.
09/23/12   pv      Added DRX PICH log pkt support.
09/11/12   abs     Ported DSDS features
08/29/12   pv      Added new enums/APIs for PICH timeline opts and prev_mode for RXD PCH.
08/27/12   pv      Externed a variable.
08/01/12   pv      New externs.
08/01/12   pv      Externed a function.
07/27/12   pv      Backing out previous checkin.
07/22/12   pv      Added externs.
07/17/12   pv      Warning fixes.
07/16/12   pv      Externs for a function and a timer.
07/13/11   pv      Offline PICH/Parallel Reacq fixes.
11/28/12   pv      Fixed compiler warnings.  
11/28/12   gsk     Fixes for compiler warnings
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/11/12   pv      Declared an extern. 
07/05/12   pv      Offline PICH changes.
06/08/12   gv      Feature HS RACH Initial code check in
04/23/12   gsk     Fixed compilation warnings.
02/27/12   stk     Added support for AGC settling done incase of Early RF Shutdown and GTS Failed. 
01/26/12   zr      Mainlined FEATURE_DETECT_STMR_SLP_CLK_MISMATCH, FEATURE_ENHANCED_CELL_RESELECTION, 
                   and FEATURE_WCDMA_GENESIS_SW
01/23/12   uk      Remove extern for gpio strobe init fn
01/20/12   sb      free floating changes.
01/05/12   pv      Created API for drx outer loop adjustment flag.
12/19/11   stk     Added filt_eng to the drx_rxd_pch struct, to keep track of filtering-1
12/02/11   stk     Added support for FEATURE_WCDMA_RXD_PCH and FEATURE_IDLEMODE_FILTER
11/04/11   zr      Adding power profiling control functionality
09/07/11   pv      Fixed compilation warning.
09/07/11   pv      Define rex timers for drx use.
04/28/11   mmr     Added support for Feature XO.
10/19/10   pv      Added a variable for FWS suspend for Genesis.
02/10/10   ms      Included tlm.h
04/17/09   rmak    Added drx_ensure_rf_is_awake to wake up RF in W-W BPLMN under FEATURE_WCDMA_EARLY_RF_SLEEP
04/17/09   mg      Extern Add drx_sleep_timer_1_isr
03/25/09   scm     Extern boolean drx_pch_delivery_enabled.
02/27/08   sv      Handle BPLMN with 320ms DRX.
10/22/08   rmak    Remove FEATURE_6275_COMPILE and use MODEM_CLK_HALT directly
10/09/08   rmak    Delete drx_terminate()
09/05/08   scm     Extern drx_delay_sleep_abort, like drx_sleep_aborted.
07/15/08   scm     Enable CTCH scheduling with new DL.
06/13/08   scm     Really fix compile problem without FEATURE_WCDMA_DL_ENHANCED.
06/11/08   scm     Fix compile problem without FEATURE_WCDMA_DL_ENHANCED.
06/11/08   scm     drx_send_mdsp_cmds_for_pich_setup() returns boolean now.
06/06/08   scm     Port RRC nudge feature to new DL.
05/23/08   ub      Added macro name for 80ms DRX cycle in num frames
03/19/08   rmak    Removed drx_set_sleep_period_complete_sig
                   and drx_clr_sleep_period_complete_sig
01/29/08   scm     Change API to drx_calc_pi_offset_from_pich().
12/11/07   scm     Extern drx_wake_up().
10/29/07   scm     Remove extern of "drx_just_paged".
10/05/07   scm     Remove featurization of types in drx_state_type.
08/20/07   vsr     Put new DL feature flag around drx_snapshot_sfn_bef_cell_resel()
                   Also moved in some macros.
06/13/07   scm     Extern drx_chk_l1_go_to_sleep().
05/02/07   scm     Extern drx_qxdm_dipsw_disables_sleep().
04/04/07   scm     Extern drx_just_paged and set_drx_state().
03/01/07   scm     Extern some functions for new DRX Manager/MBMS.
09/24/07   rmak    Added MIN_DXC_FOR_CONSEC_S2L_PULLBACKS
                   Added drx_parms.num_drx_cycles_at_last_s2l_pullback
09/04/07   mg      Add extern drx_recover_stmr
07/02/07   kps     Don't reset DRX cycle count display on entering DRX mode.
04/12/07   kps     Add ability to output Paging counters.
12/08/06   rmak    Mainlining FEATURE MODEM SLEEP CONTROLLER INSTANCE 1
                   and FEATURE INTER SYSTEM TIMING MAINT THROUGH SLEEP CTLR
11/24/06   sk      Added externs for detecting SFN mismatch during cell reselection 
07/24/06   rmak    Introduced new drx_parms member num_po_overruns
07/06/06   kps     Add declaration for drx_sibs_timer_expired_cmd()
05/02/06   mg      6280: No need to wait for CC slam sig
03/31/06   mg      Add num_pisum_zero and num_pisum_non_zero to drx_parms
02/15/06   scm     Inter-SIB sleep support inside FEATURE_INTER_SIB_SLEEP.
02/10/06   mg      Change drx_process_command declaration for lint.
01/12/06   mg      6275: Change MODEM_CLK_STATUS_RD to MODEM_CLK_HALT
11/10/05   mg      Added externs for boolean drx_nudging_rrc and drx_nudge_rrc()
                   FEATURE_DETECT_STMR_SLP_CLK_MISMATCH.
11/08/05   ub      Added extern definition for drx_sleep_aborted global
11/08/05  gv/nd    Added code for WtoW BPLMN feature.
11/03/04   vp      Added declaration for drx_ok_for_concurrent_pccpch().
07/14/05   kps     make drx_ctch_expired_timer an extern so we can expire the 
                   timer when we receive the frame.
06/20/05   mg      Change drx delayed timeout form 30s to 60s
06/01/05   mg      Extern tcxo_was_shut_down and stmr_ref_count
05/23/05   scm     Extern wake_up_before_pich, for only 1 CTCH reacq attempt.
05/12/05   scm     Goto sleep between CTCH occasions: FEATURE_UMTS_BMC_SLEEP_OPT.
03/21/05   scm     Modify enums in drx_state_type for BMC support.
03/15/05   scm     Add DRX_WAITING_ON_BMC_READY_FOR_WAKE_UP to drx_state_type.
02/18/05   mg      Add definiton for MSM_CLK_STATUS0
12/30/04   mg      Format the file. No code modifcations
11/03/04   asr     Renamed drx_send_mdsp_cmd() to drx_send_mdsp_cmds_for_pich_setup();
                   Eliminated the external interface drx_pich_sfn_arrived().
08/10/04   src     Added a hash-defined symbol to represent a certain DRX cycle
                   length.
07/08/04   scm     Extern drx_is_awake_for_tcxo_settling().
02/16/04   asr     Added #define for DRX_DELAYED_TIMER_VAL_MS.
10/17/03   asr     Declared drx_sleep_timer_1_clr().
09/03/03   asr     Declared new function drx_delayed()
07/30/03   asr     Declared new function drx_pich_demod_enabled_too_late()
04/01/03   scm     drx_parms_struct_type.min_paging_occasion from uint32 to uint16.
02/12/03   asr     Added act_time_256_chips to structure pich_demod_activation_time;
                   Added pich_sfn to structure pich_parms.
01/20/03   scm     Add drx_state_at_suspend to remember DRX mode at SUSPEND time.
                   Also extern drx_cmd_saved for RESUME.
10/21/02   asr     added num_drx_cmds, changed num_drx_cycles changed to uint32 
                   (from uint16) in drx_parms_struct_type
09/09/02   asr     Declared drx_bch_on()
07/27/02   asr     Declared drx_terminate()
07/11/02   sh      Added drx_cmd_fach() to validate drx command in fach.
06/11/02   asr     Declared several helper functions to perform rf_wakeup() 
                   in L1 task context
05/20/02   asr     Added function drx_restart() to restart the DRX state machine
                   with the previous DRX command parameters
05/18/02   sh      Added flag drx_cell_reselect_enabled for cell reselection.
05/12/02   asr     declarations of functions drx_pich_sfn_arrived() and 
                   drx_bch_setup() and drx_bch_tear_down(). New parameter 
                   in drx_parms to store the 256 chip count for pich tear down
04/25/02   asr     include new parameter mdsp_sccpch_setup_cfn in 
                   drx_parms_struct_type used in "fast" sccpch setup
03/20/02   asr     Changed type of mdsp_pi_threshold from uint16 to int16
03/14/02   asr     Moved drx_state_type and drx_state into this file from drx.c
02/12/02   asr     changed some function names
01/31/02   asr     support for Sleep : initial cut (function prototypes)
12/13/01   asr     logging support; also added PICh "pwr_offset" to drx_parms
12/11/01   asr     "imsi" parameters in drx_parms changed from uint32 to uint64
12/06/01   asr     Changes for 2001-06 compliance: adding imsi_div_8192 to drx_parms
11/16/01   asr     change of variable/function names for clarity
09/20/01   asr     Modified drx_parms, added prototypes for helper functions
09/04/01   sh      Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "l1rrcif.h"
#include "drxcmd.h"
#include "tlm.h"

#include "wl1api.h"

#include "wl1drxmanager.h"
#include "rxdiv_v.h"

#include "mdspsync.h"
#include "drxofflineprocmgr.h"

#include "wsrchlog.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define MAX_ALLOWED_SC_STMR_ERROR_MS (7)

/* Macro that holds the number of frames needed for pccpch setup, crc 
 * evaluation and tearing down of atleast one of the channels The frame
 * level breakup for a worst case scenario is:
 * Frame N: CPHY_SETUP for PCCPCH received
 * N+1: Setup config evt for hwch1 and hwch2
 * N+2: config evt for hwch1 and hwch2 received
 * N+3: frame boundary evt for hwch1 and hwch2 received
 * N+4: idle
 * N+5: CRC report-1 received for hwch1
 * N+6: CRC report-1 received for hwch2
 * N+7: CRC report-2 received for hwch1
 * N+8: CRC report-2 received for hwch2
 * ..
 * ..
 * N+5+DL_BCH_CONC_RPT_CNT_MAX-1: CRC report-DL_BCH_CONC_RPT_CNT_MAX received for hwch2
 * N+5+DL_BCH_CONC_RPT_CNT_MAX: CRC report-(DL_BCH_CONC_RPT_CNT_MAX+1) received for hwch1
 * Failure is detected and the hw channels are marked free
 *.N+5+DL_BCH_CONC_RPT_CNT_MAX+1:The freed hwch is available for some other purpose.
 *.Add one more frame to this to be safe.
 */
#define FRAMES_FOR_CONCURRENT_PCCPCH_SETUP 15

/* Convenience symbols to hold various DRX cycle length values in frames */
#define DRX_CYCLE_LENGTH_80MS  8

#ifdef FEATURE_DUAL_SIM
#define DRX_CYCLE_LENGTH_320MS  32
#endif

#define DRX_CYCLE_LENGTH_640MS 64

/* Structure to hold all the parameters necessary for calculating
 * and saving the parameters necessary for PICH demodulation.
 */
#ifdef FEATURE_WTOW_BACKGROUND_PLMN_SEARCH
#ifdef FEATURE_BPLMN_SEARCH_320MS
/* bplmn srch is supported for a drx cycle length of 32 frames and more */
#define MIN_DRX_CYCLE_LENGTH_FOR_WTOW_BPLMN 32
#else
/* bplmn srch is supported for a drx cycle length of 64 frames and more */
#define MIN_DRX_CYCLE_LENGTH_FOR_WTOW_BPLMN 64
#endif
#endif

/* Minimum number of consequetive DRX Cycles between short-to-long-qxt transitions */
#define MIN_DXC_FOR_CONSEC_S2L_PULLBACKS 10

/* 
 * The worst  case setup time should stay 2 frames, as CTCH wakeup time is usually 8ms.
 * But in case of turbo coded channel, setup time is incremented by 6-7 ms causing the worst case CTCH wakeup time as 1.5ms.
 * So for the enhanced CTCH algo, we decided to have 2 frames as the worst case CTCH setup time.
*/
#define DRX_CTCH_SETUP_TIME 2
#define DRX_CTCH_DROP_TIME 1
/* Taking the minimum possible CTCH periodicity as 8 and MAX possible DRX cycle length as 512
 * Array size = 512/8 = 64
 */
/* Log packet table will always have 2 extra rows than the num_ctch_oksn 
 for printing current and PICH sfn in that DRX cycle */ 
#define DRX_MAX_CTCH_BTW_PICH (CTCH_LOG_TABLE_SIZE + 2)

/* FW interfact still does not have support. Will include once FW support is ready.*/
/* Indices of FW populated params for PICH log pkt.*/
typedef enum
{
  DRX_AGC_DONE_IDX           = WFW_DEMOD_PICH_ASYNC_READ_RXAGC_DONE_IDX,
  DRX_PICH_CFG_IDX           = WFW_DEMOD_PICH_ASYNC_READ_PICH_CFG_IDX,
  DRX_OFFL_START_IDX         = WFW_DEMOD_PICH_ASYNC_READ_OFFL_START_IDX,
  DRX_SS_FRZ_TIME_IDX        = WFW_DEMOD_PICH_ASYNC_READ_SS_FRZ_TIME_IDX,
  //DRX_OFFL_TO_ONL_IDX        = WFW_DEMOD_PICH_ASYNC_READ_OFFL_TO_ONL_IDX,
  DRX_OFFL_PROC_DONE_IDX     = WFW_DEMOD_PICH_ASYNC_READ_OFFL_PROC_DONE_IDX,
  DRX_PI_DONE_IDX            = WFW_DEMOD_PICH_ASYNC_READ_PI_DONE_IDX,
  DRX_PI_BITS_START_POS_IDX  = WFW_DEMOD_PICH_ASYNC_READ_PI_BITS_START_POS_IDX,
  DRX_SCCPCH_POS_IDX         = WFW_DEMOD_PICH_ASYNC_READ_SCCPCH_POS_IDX,
  DRX_SS_WNW_END_POS_IDX     = WFW_DEMOD_PICH_ASYNC_READ_SS_WNW_END_POS_IDX,
  DRX_QXT_DONE_IDX           = WFW_DEMOD_PICH_ASYNC_READ_QXT_DONE_IDX,
  DRX_SS_UNFRZ_TIME_IDX      = WFW_DEMOD_PICH_ASYNC_READ_SS_UNFRZ_TIME_IDX,
  DRX_UNFRZ_RXAGC_DONE_IDX   = WFW_DEMOD_PICH_ASYNC_READ_UNFRZ_RXAGC_DONE_IDX,
  DRX_SCCPCH_ON_IDX          = WFW_DEMOD_PICH_ASYNC_READ_SCCPCH_ON,
  DRX_SS_WNW_START_IDX       = WFW_DEMOD_PICH_ASYNC_READ_SS_WNW_ONL_RPOC_START_IDX,
  DRX_ONL_OFFL_IDX           = WFW_DEMOD_PICH_ASYNC_READ_ON_OFF_DECISION_IDX,
  DRX_SS_WNW_REAL_START_IDX  = WFW_DEMOD_PICH_ASYNC_READ_SS_WNW_REAL_START_POS_IDX,
  
  DRX_NUM_ELEM_FW_PICH_ASYNC_READ = WFW_DEMOD_PICH_ASYNC_READ_NUM_ELEM
} drx_fw_pich_async_read_idx_type;

/* FW pupulated params for PICH log pkt.*/
typedef PACKED struct PACKED_POST
{
  uint32 agc_done_time;
  uint32 pich_cfg_time;
  uint32 ss_frz_act_time;
  uint32 online_offline_decision_time;
  uint32 offl_proc_st_time;
  uint32 offl_proc_done_time;
  uint32 pi_decode_done_time;
  uint32 pi_bits_st;
  uint32 ss_window_start_pos;
  uint32 ss_window_end_pos;
  uint32 ss_window_real_start_time;
  uint32 qxt_done_time;
  uint32 pich_ack_ss_unfrz_time;
  uint32 pich_ack_unfrz_agc_tune_done_time;
  uint32 sccpch_on_time;
  uint32 sccpch_fr_start_time;
}drx_fw_pich_async_read_struct_type;
/* Fing stats stuct.*/
typedef PACKED struct PACKED_POST
{ 
  uint16 init_rssi;
  uint16 tcpfSq0;
  uint16 tcpfSq1;
} fing_stats_struct_type;

/* Per finger stats info.*/
typedef PACKED struct PACKED_POST
{
  uint8 num_fings;
  fing_stats_struct_type per_fing_stats[MSMHW_MAX_NUM_FINGERS/2]; 
} drx_fing_stats_struct_type;

/* Global struct for the PICH log pkt.*/
typedef PACKED struct PACKED_POST
{
  /* PARAMETERS*/
  /* Parallel reacq done this DRX cycle.*/
  boolean prll_reacq_this_cycle; 

  /* Reacq less PICH cycle */
  boolean reacq_less_opts;

  /* Offline PICH mode this DRX cycle.*/
  boolean offl_pich_this_cycle;  
           
  int32   piSum;                /* PI SUM value.*/
  uint32  rssiSum;              /* RSSI SUM value.*/
  uint8   piVal;                /* PI Val - UE paged or not.*/  

  uint32 num_pisum_zeros;        /* No.of cumulative PI SUM 0s.*/
  uint32 num_drx_cycles;        /* No.of DRX cycles since last DRX cmd.*/
  uint32 total_drx_cycles;      /* No.of total DRX cycles since bootup.*/
  uint32 num_po_overruns;       /* No.of cumulative PO overran occasions.*/

  int32 outer_loop_freq_error; /* Outer loop frequency error.*/
  int32 tot_freq_err;          /* Total frequency error.*/ 
  int32 freq_common;           /* Freqcommon value obtained from FW.*/

  uint32  abs_slew;              /* Absolute slew.*/
  uint32  serv_cell_tot_eng;     /* Serving cell instantaneous energy.*/
  uint32  serv_cell_filt_eng;    /* Serving cell filtered energy.*/  
  uint32  cell_ref_pos_at_wkup;  /* Cell ref pos at wakeup.*/
  uint32  cell_pos_bef_slp;      /* Cell ref pos at slp */
  uint32  num_false_pages;       /* Cumulative number of false pages*/

  uint16 drx_cycle_length;
  /* Offset of PI bits from the PICH frame start.
     Bits 0:18 - Offset of PICH frame.
     Bits 19:31 - Offset of PI bits within PICH frame.
  */
  uint32  pi_offset; 

  drx_fing_stats_struct_type drx_fing_stats;

  /* TIMESTAMPS */
  uint32  sleep_ending_isr_st;  /* On-line star timestamp.*/

  uint32  first_reacq_st_time;   /* 1st Reacq issuing time.*/
  uint32  first_reacq_end_time;  /* 1st Reacq end time.*/
  uint32  second_reacq_st_time;  /* 1st Reacq end time.*/
  uint32  second_reacq_end_time; /* 2nd Reacq end time.*/
  uint32  cell_cfg_st_time;      /* 2nd Reacq end time.*/
  uint32  cell_cfg_end_time;     /* Cell/fing cfg preparation start time.*/
  uint32  triage_st_time;        /* Triage start time.*/
  uint32  triage_end_time;       /* Triage start time.*/  
  uint32  rf_slp_time;           /* RF sleep time.*/    

  drx_fw_pich_async_read_struct_type fw_parms_info;

} drx_pich_log_pkt_struct_type;

LOG_RECORD_DEFINE(DRX_PICH_LOG_PKT)
  /* Version number of this log packet.*/
  uint8 version;

  drx_pich_log_pkt_struct_type log_info;   
LOG_RECORD_END

#define DRX_PICH_LOG_PKT_SIZE() (sizeof(DRX_PICH_LOG_PKT_type))

typedef struct
{
  /* DRX Cycle Length in 10 ms radio-frame units */
  uint16 drx_cycle_len;

  /* log to base 2 of the drx_cycle_len (for logging) */
  uint8 log_drx_cycle_len;

  /* number of elapsed DRX Cycles */
  uint32 num_drx_cycles;

  /* Total number of DRX Cycles. This value is only reset by diag. */
  uint32 total_drx_cycles;

  /* the next paging occasion (sfn of frame that may contain a PI for this UE) */
  uint16 next_paging_occasion;

  /* SFN of PCCPCH frame that contains the (1st of the) PI bits */
  uint16 pich_sfn; 

  /* Paging Indicator */
  uint32 PI;

  /* 10.3.6.41 Number of PI per frame - obtained from RRC during SCCPCH setup; 
  */
  uint8 pi_per_frame;

  /* IMSI div K, as per 8.3 in 25.304 (version 2000-12), used in calculating paging
     occasion, where K is the number of SCCPCHs that carry a PCH.
     Obtained from RRC during SCCPCH setup*/
  uint64 imsi_div_pch_cnt;

  /* IMSI div 8192, as per 8.3 in 25.304 (version 2001-06), used in calculating PI.
     Obtained from RRC during SCCPCH setup*/
  uint64 imsi_div_8192;

 /*
  * "imsi_div_K modulo drx_cycle_length" which is the fixed term of the 
  * paging occasion; the other term used in calculating paging occasion
  * is a variable term. Paging occasions are (fixed term) + n * drx_cycle_len
  * for n = 0,1,2,...
  * this corresponds to the case of n=0.
  */
  uint16 min_paging_occasion;

  /* CCTrCh table index of the SCCPCH that carries the paging blocks */
  uint8 sccpch_cctrch_idx;

  /* Timing Offset of the SCCPCH that is associated with this PICH */
  uint16 sccpch_offset;

  /* the offset of the PICH relative to the PCCPCH (units of 256 chips)*/
  int16 pich_offset;

  /* Threshold for detecting PI bits; required by the mdsp to provide a Y/N decision whether
     the PI bits were +1 or -1 */
  int16 mdsp_pi_threshold;

  /* Number of bits allocated to a paging indicator = 288/Np, where
   * Np is the PI per frame. Range (2, 4, 8, or 16). This parameter
   * is programmed to the mDSP.
   */
  uint8 mdsp_bits_per_pi;

  /* the time difference (in units of 256 chips) from the start of the pich frame to
     when the page indicator bits start in the pich frame
   * Range (0...143).
   */
  uint8 pi_offset_from_pich;

  /* the time difference between the start of the PI bits to the start of the
    previous PCCPCH frame. In other words, 
    the sum of pi_offset_from_pich and pich_offset parameters described above, modulo 150.
    An mdsp parameter*/
  uint16 mdsp_pi_pn_count;

  /* the count, in 256 chip units, at which pich tear down should occur */
  uint16 mdsp_pich_tear_down_pi_pn_count;

  /* whether sccpch with pich was setup before drx i.e. whether to trust the values of
  bits_per_pi etc ..*/
  boolean sccpch_with_pich_setup;

  /* 25.331 10.3.6.50 : power transmitted on PICH minus power transmitted on CPICH
     in dB (range -10..5) */
  int8 pich_pwr_offset;

  uint8 mdsp_sccpch_setup_cfn;

  /* number of DRX commands received from RRC, and accepted by L1 */
  uint32 num_drx_cmds;

  /* number of pi sum zeros */
  uint32 num_zero_pisum;

  /* number of pi sum non zeros */
  uint32 num_non_zero_pisum;

  /* number of po overruns */
  uint32 num_po_overruns;

  /* Cumulative number of false pages*/
  uint32 num_false_pages;       

  /* number of drx cycles since last slp-time-pullback in a
     short-to-long qxt transition */
  int32 num_drx_cycles_at_last_s2l_pullback;

} drx_parms_struct_type;

/* This structure defines the actual SFN occasion and slot when the mDSP
 * command to demodulate the PI bits is sent. Depending on the position
 * of the PI bits to be demodulated with respect to the PCCPCH frame of the
 * Paging Occasion, the following cases for the activation times result:
 * if pi bit position < 1 slots  
 *   then sfn = (paging occasion - 1), slot 12
 * if 3 slots <= pi bit position <= 13 slots
 *   then sfn = paging occasion
 * if 13 slots < pi bit position <= 16 slots
 *   then sfn = paging occasion, slot 12
 * if 16 slots < pi bit position <= 28 slots
 *   then sfn = paging occasion + 1
 * if 28 slots < pi bit position < 30 slots
 *    then sfn = (paging occasion + 1), slot 12
 *
 * An Event is set up at this activation time and has to be 512 chips
 * before and atmost 13 slots from the PI bits.
 *
 *  To be safer (not assuming mDSP receives command from L1 within 1 slot,
 *  but relaxing that to 3 slots),
 *  use this: 
 *  if (0 slots <= pi bit position < 5 slots) 
 *                             then sfn = (paging occasion - 1; slot 10)
 *  if (5 slots <= pi bit position <= 13 slots) then 
 *                             then sfn = (paging occasion; slot 0)
 *  if (13 slots <= pi bit position < 23 slots) then 
 *                             then sfn = (paging occasion; slot 10)       
 *  if (23 slots <= pi bit position <= 28 slots) then 
 *                             then sfn = (paging occasion + 1; slot 0)
 *  if (28 slots < pi bit position < 30 slots) then 
 *                             then sfn = (paging occasion + 1; slot 10)        
 */

typedef struct
{
  /* whether we are in drx mode or not */
  boolean drx_mode;

  /* SFN of the frame where the mDSP config window begins */
  uint16 sfn;

  /* activation time of the pich demod event in units of 256 chips */
  uint16 act_time_256_chips;

} pich_demod_activation_time;

/* definition of the states of the DRX/Sleep state machine */
typedef enum 
{
  DRX_IDLE,
  DRX_CONFIGURED,
  DRX_PICH_DEMOD,
  DRX_WAITING_ON_SRCH_READY_FOR_SLEEP,
  DRX_WAITING_ON_DL_READY_FOR_SLEEP,
  DRX_WAITING_ON_RF_WARMED_UP,
  DRX_WAITING_ON_SRCH_WOKEN_UP,
  DRX_WAITING_ON_DL_WOKEN_UP,
  DRX_WAITING_ON_BMC_CTCH_ENABLED,
  DRX_WAITING_ON_BMC_CTCH_DROPPED,
  DRX_CTCH_ACTIVE,
  DRX_WAITING_ON_BPLMN_COMPLETE,
  DRX_NUM_STATES
} drx_state_type;


/*
 * Type definition for various profile points in the timeline.
 */

extern drx_state_type drx_state;

/* To remember DRX state at the time of a SUSPEND, for use on RESUME. */
extern drx_state_type drx_state_at_suspend;

/* Holds all the parameters necessary for calculating parameters
 * for PICH demodulation.
 */
extern drx_parms_struct_type drx_parms;

/* Copies of counters, so we can simulate a "reset" of these variables
** without modifying the originals. */
extern uint32 reset_rrcpg1_num_pages;
extern uint32 reset_rrcpg1_num_pages_matched;

/* drx command saved for use when drx is restarted */
extern l1_drx_cmd_type drx_cmd_saved;

/* Holds the actual SFN occasion and slot when the mDSP command
 * to demodulate the PICH has to be sent.
 */
extern pich_demod_activation_time pich_page_occasion;

/*
 * Adjustment to sleep period based on "ticks rem" target
 */
extern int32 ticks_rem_adj_sleep_longer_Cx8;

/*PICH opts will be disabled in next PICH wakeup so adjust timeline by
  4 slots for offline PICH to online PICH transition and 2 slots for
  LPQ reacq search at wakeup as triage cnt > 1 FW doesnt support HPQ.*/
#define DRX_ADJUST_TICKS_REM_SLP_LNGR_BY_SLOTS(num_slots)  \
  (ticks_rem_adj_sleep_longer_Cx8 -= (num_slots * CHIPX8_PER_SLOT))
  
#define DRX_UPDATE_SLPLNGR_TL_VARS() \
   {  ticks_rem_adj_sleep_longer_Cx8_old = ticks_rem_adj_sleep_longer_Cx8; \
      ticks_rem_minus_target = 0; \
   }

/* 60sec (deliberately set >> out-of-service timer which is 12 seconds 
 * DRX delayed timer duration should exceed the max SIB repetition period 
 * (which is 40.96 seconds per spec). This is to take care of inter-freq 
 * neighbors (during sib reads of inter-freq neighbors, reacq search 
 * continuously fails because RF is tuned away;  
 */
/*Timer value is decided at the time of drx_cmd based on drx_cycle_len.*/
extern uint32 drx_delayed_timer_ms;

#define DRX_PICH_TIMER_VAL_MS (30)
#ifdef FEATURE_WCDMA_REACQLESS_PICH
/* Number of offline PICH samples required in case of RLP.
** TO DO - Use 12 BPGs for Np=18 alone and for the rest, use 10 BPGs. 8 BPGs are 
** enough for an INTRA PN LPQ search of 2048 * 1 .*/
#define DRX_NUM_SAMPLES_REQUIRED_FOR_RLP 12
#endif
#if defined(FEATURE_UMTS_BMC) && defined(FEATURE_UMTS_BMC_SLEEP_OPT)
/* Flag set to TRUE if we're waking up before PICH, like to receive CTCH data.
** In these situations, only attempt acquisition list search once.  If it
** fails, go back to sleep.  No heroics done to acquire for CTCH. */
extern boolean  wake_up_before_pich;
#endif

extern boolean tcxo_was_shut_down;
extern uint32 stmr_ref_count;

extern boolean drx_sleep_aborted;
extern boolean drx_delay_sleep_abort;
extern boolean drx_pch_delivery_enabled;

extern boolean drx_nudging_rrc;
extern uint32 sleep_timer_aft_CR;
extern uint32 sfn_aft_CR,cc_aft_CR;
extern boolean sfn_mismatch_check_needed;

extern boolean wl1_use_fws_suspend;

#ifdef FEATURE_WTOW_BACKGROUND_PLMN_SEARCH
#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
/* Variable to store the previous W2G BPLMN state before going to sleep */
extern boolean prev_wtog_bplmn_in_progress;
#endif
#endif

#ifdef FEATURE_XO
extern boolean successful_pich_demod;
#endif /* FEATURE_XO */

extern int32 ticks_until_pi;

/* Rex timer for sleep timer 1.*/
extern rex_timer_type drx_sleep_timer_1;
                                         
extern  drx_outer_loop_adj_mode_type drx_outer_loop_adj_mode;

extern boolean drx_page_ind_received;

typedef struct
{
  /* variable to tell whether idle_rxd is enabled or not via NV. */
  boolean idle_rxd_enabled;
  /* Wakeup handle/entity to tell the reason for wakeup */
  int8 wakeup_entity_for_idle_rxd;
  /* variable to store the current RXD_State after wakeup. */
  rxd_pch_mode_enum_type idle_rxd_curr_mode;
  /* variable to store the next RXD_State during sleep. */
  rxd_pch_mode_enum_type idle_rxd_next_mode;
  /* variable to store the prev RXD_State during sleep. (Used for parallel Reacq case) */
  rxd_pch_mode_enum_type idle_rxd_prev_mode;
  /* variable to store piSum value thats read from MDSP for use by Idle RxD */
  int32  piSum_for_idle_rxd;
  /* Flobal remember the page staus for this DRX cycle  */
  boolean page_received_this_drx;
  /* Variable to tell whether there was a No response to page in this DRX. Either 35ms timer expiry or CRC errors */
  boolean no_rsp_to_page;
  /* Variable to tell whether reacq srch was issued on diversity antenna.*/
  boolean reacq_issued_on_div_ant;
  /* Variable to store filt_eng from the best of two primary searches, to be used while calculating_next_mode() */
  uint32  filt_eng;
  /* Variable to store filt_ecio from the best of two primary searches, to be used while calculating_next_mode() */
  uint16 filt_ecio;
  /* Variable to store filt_ecio from the best of two primary searches, to be used while calculating_next_mode() */
  boolean is_div_srch_better;
  /*variable to store the filtered energy for PWC algorigthm*/
  uint32 filt_eng_pwc;
  /*variable to store the filtered energy for PWC algorigthm*/
  int16 filt_rscp_pwc;
  /* Variable to store the AGC imbalance decision made by RxD module */
  boolean rxd_out_of_lock;
}drx_rxd_pch_struct_type;

/* Holds all the parameters necessary for Idle_RXD feature. */
extern drx_rxd_pch_struct_type drx_rxd_pch;

extern boolean drx_finger_assgn_at_sleep_chk;

extern boolean drx_sleep_entry_proc;
#ifdef FEATURE_WCDMA_REACQLESS_PICH
typedef struct
{
/*whether RLP  conditions met for next cycle or not.*/
  boolean possible_next_cycle; 
/*RLP will happen this cycle */
  boolean this_cycle;  
}drx_rlp_db_type;
#endif
typedef struct
{
  boolean parallel_reacq_next_cycle;
  boolean parallel_reacq_this_cycle;
  boolean parallel_reacq_peformed;
#ifdef FEATURE_WCDMA_REACQLESS_PICH
  drx_rlp_db_type rlp;
#endif
}drx_parallel_reacq_db_type;

extern drx_parallel_reacq_db_type drx_parallel_reacq_db;

extern boolean l1m_bplmn_in_progress;
                                    
typedef void (*drx_pich_entity_srch_to_sleep_cb_type)(void);

extern rex_crit_sect_type drx_evt_mgr_lock;

extern boolean drx_offline_pich_this_cycle;
                                    
extern boolean drx_pich_opts_status;
                                    
extern boolean drx_enable_rgs_update;

extern drx_pich_log_pkt_struct_type drx_pich_log_pkt;

typedef enum
{
 DL_TRI_WORKLOOP_ACTION,
 SRCHZZ_SLEEP_OPS_ACTION,
 DRX_OPTS_SVC_NUM_ACTIONS
}drx_opts_svc_action_type;


extern drx_opts_svc_action_type drx_opts_svc_action;

extern void drx_opts_svc_handler(void);
#ifdef FEATURE_WCDMA_REACQLESS_PICH
/*Variable which says whetehr AGC tune done is not suitable for offline PICH.*/
extern boolean drx_agc_time_not_suitable_rlp;
/*Variable stores the no of bpgs by which AGC tune done is past S1.*/
extern int32 agc_done_to_s1_bpg;
#endif

extern drx_idle_mode_debug_nv_struct_type drx_idle_mode_debug_nv;
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION     DRX_ENSURE_RF_IS_AWAKE

DESCRIPTION
  This function is called to ensure RF is awake
  
PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If RF is not awake, DRX will wake up RF

===========================================================================*/
extern void drx_ensure_rf_is_awake(void);

/*===========================================================================

FUNCTION     DRX_CMD

DESCRIPTION
  This function is called by L1M when RRC commands L1 to begin DRX
  operation. This function calculates the Paging Occasion and the
  Paging Indicator (PI) value and calls the function to calculate
  the parameters that are needed to send to the mDSP to demodulate
  the PICH.

DEPENDENCIES
  None

RETURN VALUE
  True if all went well; False otherwise.

SIDE EFFECTS
  The global variable drx_parms is updated with the computed values.

===========================================================================*/
extern boolean drx_cmd( l1_drx_cmd_type* cmd );

/*===========================================================================

FUNCTION     DRX_CMD_FACH

DESCRIPTION
  This function validates the DRX command received from RRC during FACH
  and then saves the DRX Cycle Length.

DEPENDENCIES
  None

RETURN VALUE
   BOOLEAN, true if validation succeeds, false otherwise.

SIDE EFFECTS
  The global variable drx_parms.drx_cycle_len is updated.

===========================================================================*/
extern boolean drx_cmd_fach( l1_drx_cmd_type* cmd );

/*===========================================================================

FUNCTION     DRX_RESTART

DESCRIPTION
  Restarts the DRX state machine with previous DRX command parameters.

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS


===========================================================================*/
extern void drx_restart(void);

/*===========================================================================

FUNCTION     DRX_REL

DESCRIPTION
  This function is called to stop drx operation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_rel(void);

/*===========================================================================

FUNCTION       DRX_CALC_PI_OFFSET_FROM_PICH

DESCRIPTION
  This function calculates the pnCount (in units of 256 chips) in a frame
  where the UE is to begin demodulating the PICH.  Range 0...143.
  It calculates this PI position for the SFN passed in by the caller,
  and returns the result to the caller.

DEPENDENCIES
  None

RETURN VALUE
  uint8 - Calculated PI PN position (Range 0 - 143).

SIDE EFFECTS
  None

===========================================================================*/
extern uint8 drx_calc_pi_offset_from_pich(uint16 pag_occ);

/*===========================================================================

FUNCTION       DRX_CALC_NEXT_PAGING_OCCASION

DESCRIPTION
  This function calculates the next paging occasion. The value calculated is 
  set in drx_parms.next_paging_occasion

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void drx_calc_next_paging_occasion( void );

/*===========================================================================

FUNCTION       DRX_MDSP_PARMS

DESCRIPTION
  This function calculates the parameters that are needed by the mDSP
  for PICH demodulation:
  piThreshold
  bitsPerPi
  piPnCount
  
  It also calculates the actual SFN and the slot when the mDSP command
  to demodulate the PI bits has to be sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The global variable drx_parms is updated with the calculated values.

===========================================================================*/
extern boolean drx_compute_mdsp_parms( void );

/*===========================================================================

FUNCTION       DRX_PICH_DONE_ISR

DESCRIPTION
  This ISR is called by tramp services when the MDSP_INT5 occurs.
  This function will read the piValue register from the mDSP to ascertain
  whether the PICH was successfully demodulated and if there was a page
  for the UE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the UE was paged, the ISR will be de-registered.
  If the UE was not pages, the mDSP will be commanded to again demodulate
  the PICH after the next drx cycle.

===========================================================================*/
extern void drx_pich_done_isr( void );

/*===========================================================================

FUNCTION       DRX_SEND_MDSP_CMDS_FOR_PICH_SETUP

DESCRIPTION
  This function causes the required mDSP commands to set up the demodulation
  of PICH.  It is possible for this function to get called with PICH suspended
  or even dropped (not registered with DRX Manager).  When that happens, don't
  demodulate PICH.  This should be pretty rare, but can happen.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - PICH demod commands sent to mDSP.
  FALSE - Commands NOT send to mDSP. No PICH at this time.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean drx_send_mdsp_cmds_for_pich_setup( void );

/*===========================================================================

FUNCTION       DRX_LOG_INFO

DESCRIPTION
  This function logs the drx info collected during the last PICH done
  mdsp interrupt. This function is called in task context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void drx_log_info( void );

/*===========================================================================

FUNCTION     DRX_WAKE_UP

DESCRIPTION
  This function is called to prepare L1 for wakeup

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_wake_up( void );

/*===========================================================================

FUNCTION       DRX_SLEEP_ABORT
DESCRIPTION
  This function initiates the abort of sleep mode if already in progress

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void drx_sleep_abort( void );

/*===========================================================================
FUNCTION     DRX_IS_AWAKE_FOR_TCXO_SETTLING

DESCRIPTION
  This function returns whether or not we are intentionally staying awake
  during DRX cycles to allow VCTCXO to settle.  This gives the mDSP a chance
  to adjust trk_lo_adj based upon downlink pilot and drive the VCTCXO
  frequency to a more accurate value.  We do this for approximately 10
  seconds after power up.  This is done to help GSM, in the event that it
  doesn't have rotator support.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - We are allowing VCTCXO to track.
  FALSE - Tracking is complete.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean drx_is_awake_for_tcxo_settling(void);

#ifdef FEATURE_WTOW_BACKGROUND_PLMN_SEARCH
/*===========================================================================

FUNCTION     DRX_CHK_L1_GO_TO_SLEEP

DESCRIPTION
  This function calls the RRC query function and checks whether a WTOW BPLMN
  scan needs to be intiated.  It also checks whether a WTOG BPLMN search is
  active and monitors the GSM CLK. If the GSM clock is enabled and there are
  no GSM neighbors and the WTOG BPLMN search is not in progress then turn off
  the GSM clock.  The RRC query function returns a FALSE if a WTOW BPLMN
  search can be performed and returns TRUE if no search can be performed or a
  WTOG search can be performed.
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE:  Either no WTOW search or WTOG BPLMN search.
          \ FALSE: WTOW BPLMN search can be started.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean drx_chk_l1_go_to_sleep(uint32 ms_to_sleep);
#endif


#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
/*===========================================================================
FUNCTION     l1_rrc_can_l1_go_to_sleep

DESCRIPTION       This function is called by L1 in every DRX cycle to determine
                  if L1 should go to sleep or not. This function call was 
                  introduced due to WTOW and WTOG BPLMN search. If WTOW search needs to be
                  done during the DRX sleep time, then L1 should NOT go to sleep. Else
                  if WTOG search needs to be done then L1 should go to sleep.

DEPENDENCIES      NONE

RETURN VALUE      
                  TRUE:  L1 can go to sleep.
                  FALSE: Otherwise
                                  
SIDE EFFECTS
===========================================================================*/

boolean l1_rrc_can_l1_go_to_sleep(  uint32 timer_val,
  boolean *wtog_bplmn_in_progress
#ifdef FEATURE_DUAL_SIM_BPLMN
  ,boolean *wtow_bplmn_in_progress
#endif
);


/*===========================================================================

FUNCTION l1_rrcpg1_modify_sibs_for_sfn_update

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

boolean l1_rrcpg1_modify_sibs_for_sfn_update
(
  void
);
#endif/*FEATURE_WCDMA_FREE_FLOATING_TASK*/

/*===========================================================================

FUNCTION     DRX_PROCESS_COMMAND

DESCRIPTION
  This function implements the state machine of DRX/Sleep. It processes the
  local commands of type DRX_SLEEP_CMD.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_process_command(const drx_sleep_cmd_type * cmd);


/*===========================================================================

FUNCTION     DRX_PICH_DEMOD_EVENT_ENABLED_TOO_LATE

DESCRIPTION
  This function is called when it is detected that the pich demod
  event wasn't enabled in time. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_pich_demod_enabled_too_late(void);

/*===========================================================================

FUNCTION     DRX_BCH_SETUP

DESCRIPTION
  This function is called when a BCH is setup. It sets a flag in the drx module.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_bch_setup(void);

/*===========================================================================

FUNCTION     DRX_BCH_TEAR_DOWN

DESCRIPTION
  This function is called when BCH is torn down. It resets a flag in the drx module.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_bch_tear_down(void);

/*===========================================================================

FUNCTION     DRX_SLEEP_PERIOD_COMPLETE

DESCRIPTION
  This function is called when the sleep period is over. It calls 
  a function in the sleep module to wake up.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_sleep_period_complete(void);

/*===========================================================================

FUNCTION     DRX_SET/CLR_SLEEP_PERIOD_COMPLETE_SIG, 
             DRX_SET/CLR_COMBINER_COUNT_SLAMMED_SIG
             DRX_WAIT_FOR_COMB_CNT_SLAM
             DRX_DEFINE_WAKE_UP_TIMER

DESCRIPTION
  These functions are called to send a specific signal to L1 (drx is merely 
  acting like a conduit between sleep and L1 here).
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_define_wake_up_timer(rex_timer_type * ptimer);

extern void drx_nudge_rrc(void);

/*===========================================================================

FUNCTION     DRX_BCH_ON

DESCRIPTION
  This function returns true if BCH was setup and FALSE otherwise.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern boolean drx_bch_on(void);

/*===========================================================================

FUNCTION     DRX_DELAYED

DESCRIPTION
  This function handles the recovery from DRX getting stuck for an extended period.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_delayed_cb(uint32 unused);

/*===========================================================================

FUNCTION     DRX_SLEEP_TIMER_1_CLR

DESCRIPTION
  This function is called to clear timer 1 service provided by the sleep controller.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_sleep_timer_1_clr(void);

/*===========================================================================

FUNCTION       DRX_OK_FOR_CONCURRENT_PCCPCH

DESCRIPTION
  This function determines if the DRX module is OK with DL going for a
  concurrent pccpch setup.

DEPENDENCIES
  None

RETURN VALUE
  TRUE, if DRX is OK for a concurrent pccpch setup. Else, False.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean drx_ok_for_concurrent_pccpch(void);

/*===========================================================================

FUNCTION     GET_SLEEP_MS

DESCRIPTION
  This function is called to compute the number of milliseconds for which 
  to sleep
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern uint32 get_sleep_ms(void);

/*===========================================================================

FUNCTION     NEXT_DRX

DESCRIPTION
  This function is called to kick in the next DRX cycle

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void next_drx(boolean reacq_failed);

/*===========================================================================

FUNCTION     SET_DRX_STATE

DESCRIPTION
  This function is called to change the state of the DRX module.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void set_drx_state(drx_state_type newState);

/*===========================================================================
FUNCTION     DRX_QXDM_DIPSW_DISABLES_SLEEP

DESCRIPTION
  This function checks the QXDM DIP switch settings to see if the user has
  disabled sleep mode operation.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if sleep is disabled via QXDM DIP switch setting.  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean drx_qxdm_dipsw_disables_sleep(void);

/*===========================================================================

FUNCTION     DRX_SNAPSHOT_SFN_BEF_CELL_RESEL

DESCRIPTION
This function is called after a cell reselection is triggered and NBCH is
setup and SFN-SFN diff, Noff computation for neighbor cell are done. This
function takes a snapshot of neighbor cell's timing (SFN,CC) and sleep clock
snapshot. This is used for comparison after cell reselection to detect any
timing errors during cell reseleciton. Timing is computed w.r.t target cell

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
extern void drx_snapshot_sfn_bef_cell_resel
(
  tlm_cell_info_struct_type neighbor_cell_info
);

/*===========================================================================

FUNCTION     DRX_SNAPSHOT_SFN_AFTER_CELL_RESEL

DESCRIPTION
This function is called after a cell reselection has completed, and we have
aligned our timing to the new cell.  It snapshots the SFN/chipx1 of the
new serving cell, and also snapshots the sleep clock.  Once we enter DRX
mode on the new cell, we compare reselection durations measured by STMR and
by sleep clock.  If they are too far off, our timing may be off, so we
nudge RRC to recover.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
extern void drx_snapshot_sfn_after_cell_resel
(
  tlm_cell_info_struct_type serving_cell_info
);

/*===========================================================================

FUNCTION     DRX_NUDGE_SUPPORT_AFTER_PCCPCH_SETUP

DESCRIPTION
This function gets called when both PCCPCH_S and PCCPCH_N setup completes,
and CRC's are either passing or failing.  There is some processing that can
happen regarding RRC nudge at this time.

If it's PCCPCH_N, and we're reselecting, now is a good time to snapshot
neighbor timing, so we have a reference after the reselection completes.

If it's PCCPCH_S, because we nudged RRC earlier, we didn't send SETUP_CNF
before, so let's send it now.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void drx_nudge_support_after_pccpch_setup
(
  tlm_cell_info_struct_type cell_info
);

/*===========================================================================

FUNCTION     DRX_CHECK_FOR_RRC_NUDGE

DESCRIPTION
This function gets called at sleep time.  We check if we want to nudge RRC.
Nudging RRC means telling RRC to bring up PCCPCH_S, so we can reestablish
timing of the network.  We do this when UE timing might be off, like when
the duration of a cell reselection seems questionable.  We also do it every
DRX_NUDGE_RRC_TO_READ_MIB_SEC seconds, just to make sure our timing is OK.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
We might nudge RRC to bring up PCCPCH_S.
===========================================================================*/
extern void drx_check_for_rrc_nudge(void);

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
extern void l1_rrc_cphy_nudge_for_sfn_miss_match_rsp_handler
(
boolean status
);
#endif /*FEATURE_WCDMA_FREE_FLOATING_TASK*/


/*===========================================================================

FUNCTION       DRX_RECOVER_STMR

DESCRIPTION
  This function resets and takes the UE oos if STMR was stuck.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void drx_recover_stmr(void);

/*===========================================================================

FUNCTION       drx_is_ue_just_paged

DESCRIPTION
  This function returns the status of the UE page for this page cycle.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if UE got paged. 
  FALSE- if UE is not paged.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean drx_is_ue_just_paged(void);

/*===========================================================================

FUNCTION       DRX_SLEEP_TIMER_1_ISR
DESCRIPTION
  This function is the isr for the sleep timer 1 done interrupt.
  If this fires, it implies that RRC hasn't responded to a PICH-page.
  If in the right state, it posts a local command that kicks sleep back in.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void drx_sleep_timer_1_cb(uint32 unused);

/*===========================================================================
FUNCTION   DRX_RXD_PCH_SET_ENABLE 

DESCRIPTION
  This function sets the value of drx_rxd_pch.idle_rxd_enabled

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void drx_rxd_pch_set_enable(boolean state);

/*===========================================================================
FUNCTION   DRX_RXD_PCH_GET_NEXT_MODE 

DESCRIPTION
  This function returns the next mode in RXD IDLE.

DEPENDENCIES
  None

RETURN VALUE
  RXD_PCH_NO_DIV (or) RXD_PCH_FAST_DIV (or) RXD_PCH_FULL_DIV

SIDE EFFECTS
  None

===========================================================================*/
rxd_pch_mode_enum_type drx_rxd_pch_get_next_mode(void);

/*===========================================================================
FUNCTION   DRX_RXD_PCH_RESET_DB 

DESCRIPTION
  This function clears the entire DRX_RXD_PCH structure.
  This function is called upon receiving DRX_CMD and also upon every wakeup

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void drx_rxd_pch_reset_db(boolean clear_all);


/*===========================================================================

FUNCTION     DRX_ENABLE_OUTER_LOOP_ADJ

DESCRIPTION
  This function is called to set/reset the flag that allows to perform 
  DRX outer loop adjustment.
  
PARAMETERS
  boolean flag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void drx_enable_outer_loop_adj(boolean flag);

/*===========================================================================
FUNCTION     DRX_PRLL_REACQ_CURR_CYCLE

DESCRIPTION
  This function returns if we are in parallel reacq this cycle.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern boolean drx_prll_reacq_curr_cycle(void);

/*===========================================================================
FUNCTION     L1M_MDSP_AGC_TUNE_DONE

DESCRIPTION
  Handler for the AGC tuning done interrupt from MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1m_mdsp_agc_tune_done(void);

/*===========================================================================
FUNCTION     DRX_PICH_INTERRUPT_DELAYED

DESCRIPTION
   CB function for pich demod delayed timer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void drx_pich_interrupt_delayed(uint32 unused);

extern void drx_offline_pich_adjust_outer_loop(void);

extern boolean drx_is_outer_loop_adj_enabled(void);

extern int32 pich_stats_adjust_drx_ticks(void);

extern void drx_calc_pi_bit_time_for_offline_pich(void);

extern void drx_pich_phychan_setup_ops(drx_pich_processing_mode_type mode);

extern void drx_offline_pich_setup(void);

extern boolean l1m_bplmn_in_progress;

extern boolean drx_needs_fast_sccpch_suspend;

extern boolean dl_offline_pich_sccpch_auto_resume;

extern boolean drx_disable_auto_sccpch;
      
extern boolean drx_enable_rgs_update;
      
extern boolean drx_qxt_pending;

extern void drx_ss_mgr_event_notify(drx_ssmgr_notify_evt_type drx_ss_mgr_notification_event,drx_notification_post_cb_type post_cb);

extern boolean drx_get_fast_sccpch_suspend(void);

extern void drx_set_fast_sccpch_suspend(boolean config);

extern void drx_adjust_outer_loop(drx_outer_loop_adj_mode_type outer_loop_adj_mode, boolean adjust_for_qxt);

extern void drx_sccpch_start_event_done_handler(void);

extern void drx_rf_wakeup_with_agc_done(boolean wait_for_agc_done);

extern rex_timer_type drx_qxt_interrupt_delayed_timer;

extern void drx_qxt_interrupt_delayed(uint32 unused);

extern boolean drx_set_pich_entity_srch_to_sleep_pending(drx_pich_entity_srch_to_sleep_cb_type cb);

extern uint32 drx_get_pending_notf_bmsk(void);

extern boolean drx_prll_reacq_this_cycle(void);

extern void drx_pich_opts_init(void);

extern boolean drx_chk_srch_to_slp_status(void);

/*===========================================================================
FUNCTION     DRX_GET_TIMESTAMP_FOR_LOG_PKT

DESCRIPTION
  This function computes the timestamp in the form 
  Bits 0:18 subframe cx8, bits 19:30: frame

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 drx_get_timestamp_for_log_pkt(void);

/*===========================================================================
FUNCTION     DRX_PICH_LOG_INIT

DESCRIPTION
  This function initializes the content of the PICH log pkt.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void drx_pich_log_init(void);

/*===========================================================================
FUNCTION     DRX_PICH_LOG_FREQ_ERR

DESCRIPTION
  This function logs the frequency error for PICH log pkt.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void drx_pich_log_freq_err(void);

/*===========================================================================
FUNCTION     DRX_PICH_LOG_PKT_SUBMIT

DESCRIPTION
  This function submits the PICH log pkt if it can allocate a buffer.

DEPENDENCIES
  Prints only after atleast 1 DRX cycle with true sleep happens.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void drx_pich_log_pkt_submit(void);

/*===========================================================================
FUNCTION     DRX_PICH_LOG_PKT_READ_FW_PARMS

DESCRIPTION
  This function reads the params populated by FW for the PICH log pkt.

DEPENDENCIES
  FW should have populated the correct values.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void drx_pich_log_pkt_read_fw_params(void);

/*===========================================================================
FUNCTION     DRX_PICH_LOG_FING_STATS

DESCRIPTION
  This function logs the fing stats to be printed in the PICH log pkt.
  1. Fing RSSI
  2. TCPFSQ0 value
  3. TCPFSQ2 value/

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void drx_pich_log_fing_stats(void);
extern void drx_ss_mgr_event_done_notify_local_cmd_handler(drx_ssmgr_notify_evt_type drx_ss_mgr_notification_event);

/*===========================================================================

FUNCTION     DRX_RF_WARMED_UP

DESCRIPTION
  This function is used as a call back function thats called when RF has warmed up,
  after a sleep period has expired. It posts a DRX local command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_rf_warmed_up(void);

typedef enum
{
  DRX_CTCH_INVALID,
  DRX_CTCH_SCHEDULE_SFN,
  DRX_CTCH_DATA_SFN,
} drx_ctch_sfn_type;

typedef struct
{
  /* Holds the total amount of interf search time */
  uint8 ifreq_srch_time;

  /* Holds the total amount of interf search time */
  uint8 irat_srch_time;

  /* Holds the info whether to allow searches instead of CTCH
   * Flag will be set in case of bad serving cell conditions
   */
  boolean prioritize_srchs_over_ctch;

  /* Bitmask holds the gap index where the interf/irat searches fit */
  uint32 ifreq_irat_bit_mask;
  uint32 ifreq_bit_mask;
  uint32 irat_bit_mask;

  /* Holds the index of a CTCH schedule SFN between two PICH oksns */
  uint16 ref_ctch_sfn_idx;

  /* Holds the index of next possible CTCH data schedule SFN between two PICH oksns*/
  uint16 nxt_ctch_sfn_idx;
  
  /* Holds the info of the num of CTCH SFNs and the array stores the
   * CTCH SFNs in the following order between two PICH oksns
   */
  /* Array structure have pich sfn1 ctch sfn1 ctch sfn2 ... pich sfn2 */
  uint32 ctch_sfn_arr[DRX_MAX_CTCH_BTW_PICH];
  uint32 num_ctch_sfns_between_pi_oksns ;
  
  /* Holds the schedule type(DATA/SCHEDULE) of the CTCH SFN */
  drx_ctch_sfn_type ctch_sfn_priority[DRX_MAX_CTCH_BTW_PICH];

  /* Flag used to unconditionally prioritize search over CTCH
     when panic conditions are met */

  boolean panic_srch_cond_met;

  uint16 ctch_gap_idx_cnt;

  /* Holds the SFNs and the num of SFNs that are to be notified to DL to invalididate for decoding */
  uint32 ctch_sfn_invalidate[DRX_MAX_CTCH_BTW_PICH];
  uint8 num_ctch_sfn_blown_for_srchs;

  boolean block_srch_for_ctch_decision;
  boolean srch_skipped_for_ctch;
    
} drx_enh_ctch_algo_struct_type;

/* temp struct used to store info required for CTCH log pkt */
typedef struct
{
  /* gap in frames*/
  uint16 gap_size;

  /* whether CTCH skipped*/
  boolean ctch_skipped;

  /* Ifreq srch was fit */
  boolean ifreq_fit;

  /* Irat srch was fit */
  boolean irat_fit;

}drx_ctch_sfn_info_struct_type;

typedef enum
{
  DRX_CTCH_IFREQ_IRAT_FIT,
  DRX_CTCH_IFREQ_FIT,
  DRX_CTCH_IRAT_FIT,
}drx_ctch_srch_type;



/* Database to store the variables/params for Dual Receive DS DS specific
   data.*/
#define DRX_MAX_SRCH_MEAS_FAILED_BAND_INFO_IDX (WRM_MAX_BANDS - 1)
#define DRX_MEAS_FAILED_BAND_INFO_INVALID 0xFF

typedef struct
{
  /* Consecutive PI SUM 0*/
  uint8 num_cons_pi_sum_zero;

  /* Consecutive reacq failures*/
  uint8 num_cons_reacq_fail;
  
  /* Consecutive good pi decodes*/
  uint8 num_cons_pi_decode_succ;

  /* Consecutive reaq passes */
  uint8 num_cons_reacq_pass;
 
  /* Last TRM params passed for reserve/request.*/
  wrm_freq_input_type trm_res_req_band_info;

  /* Band information of failed bands.*/
  wrm_freq_input_type srch_meas_failed_band_info;

  /* Last TRM params passed for reserve/request.*/
  boolean wl1_forced_hybrid_mode;

} drx_dr_dsds_db_struct_type;

extern drx_dr_dsds_db_struct_type drx_dr_dsds_db;

#define DRX_GET_MEAS_FAILED_NUM_BANDS() (drx_dr_dsds_db.srch_meas_failed_band_info.num_bands)
#define DRX_GET_MEAS_FAILED_BAND(idx)   (drx_dr_dsds_db.srch_meas_failed_band_info.bands[idx])
#define DRX_CLR_MEAS_FAILED_BAND_INFO_IN_DB()               (memset(&drx_dr_dsds_db.srch_meas_failed_band_info, 0x0, sizeof(wrm_freq_input_type)))
#define DRX_CLR_RES_REQ_BAND_INFO(band_info_ptr)            (memset(band_info_ptr, 0x0, sizeof(wrm_freq_input_type)))
#define DRX_NUM_CONTINUOUS_REACQ_FAILED()                   (drx_dr_dsds_db.num_cons_reacq_fail)
#define DRX_NUM_CONTINUOUS_PISUM_ZERO()                     (drx_dr_dsds_db.num_cons_pi_sum_zero)
#define DRX_CLR_CONS_REACQ_FAIL_CNT()       (drx_dr_dsds_db.num_cons_reacq_fail = 0)
#define DRX_INC_CONS_REACQ_FAIL_CNT()       (drx_dr_dsds_db.num_cons_reacq_fail++)
#define DRX_CLR_NUM_CONS_PI_SUM_ZERO_CNT()  (drx_dr_dsds_db.num_cons_pi_sum_zero = 0)
#define DRX_INC_CONS_PI_SUM_ZERO_CNT()      (drx_dr_dsds_db.num_cons_pi_sum_zero++) 
#define DRX_NUM_CONTINUOUS_REACQ_SWITCH_THRESH              3
#define DRX_NUM_CONTINUOUS_PISUM_SWITCH_THRESH              3
#define DRX_PANIC_SRCH_COND_MET_FOR_CTCH() (drx_ctch_enh_db_ptr->panic_srch_cond_met)        
#define DRX_NUM_CONTINUOUS_PI_DECODE_PASS()                 (drx_dr_dsds_db.num_cons_pi_decode_succ)  
#define DRX_NUM_CONTINUOUS_REACQ_PASS()                     (drx_dr_dsds_db.num_cons_reacq_pass)
#define DRX_CLR_CONS_REACQ_PASS_CNT()                       (drx_dr_dsds_db.num_cons_reacq_pass = 0)
#define DRX_CLR_CONS_PI_SUM_PASS_CNT()                      (drx_dr_dsds_db.num_cons_pi_decode_succ = 0)
#define DRX_INC_CONS_PI_SUM_PASS_CNT()                      (drx_dr_dsds_db.num_cons_pi_decode_succ++)
#define DRX_INC_CONS_REACQ_PASS_CNT()                       (drx_dr_dsds_db.num_cons_reacq_pass++)
/*=========================================================================== 
FUNCTION DRX_ADD_MEAS_FAILED_BAND_INFO_TO_DB 
 
DESCRIPTION 
  Adds the band info of the failed measurement this DRX cycle. Srchzz calls this.
 
DEPENDENCIES
  None.

PARAMETERS
  Band info of failed measurement.

RETURN VALUE 
  None
 
===========================================================================*/
extern void drx_add_meas_failed_band_info_to_db(wrm_frequency_info band);
extern drx_enh_ctch_algo_struct_type *drx_ctch_enh_db_ptr;
uint8 drx_check_gaps_between_ctchs(void);
void drx_block_srch_for_ctch(uint32 t_irat, uint32 t_ifreq);
void drx_compute_srches_fits_in_gaps(boolean skip_ctch_sfns);

/*===========================================================================
FUNCTION     DRX_INIT_ENH_CTCH_ALGO_INFO

DESCRIPTION
 This func resets all the CTCH info calcualted for the previous PICH wakeup.
 This func should be called when CTCH algo needs to run for searches.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE 
===========================================================================*/
extern void drx_init_enh_ctch_algo_info(void);

/*===========================================================================
FUNCTION     DRX_QUERY_BLOCK_SRCH_FOR_CTCH_DECISION

DESCRIPTION
 This func returns if searches are to be blocked for CTCH.
 If CTCH entity is awake, searches should be blocked.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE 
TRUE -- Allow searches 
FALSE -- Block searches 

===========================================================================*/
boolean drx_allow_srch_with_active_ctch_schd(void);

/*===========================================================================
FUNCTION     DRX_QUERY_TO_EVAL_ENH_CTCH_ALGO

DESCRIPTION
 This func returns the decision to evluate the enhanced CTCH algo.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE 
TRUE -- Evaluate the algo 
FALSE -- Do not evaluate the algo

===========================================================================*/
boolean drx_query_to_eval_enh_ctch_algo(void);

/*===========================================================================
FUNCTION     DRX_PRIORITIZE_SRCH_OVER_CTCH

DESCRIPTION
 The function sets the flag to drop CTCH for searches. Flag is usually set in
 case of serving cell ecio lesser than the threshold/S-criteria failure/Serving cell barred

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE 
 None
===========================================================================*/
void drx_prioritize_srch_over_ctch(void);
extern boolean drx_query_block_srch_for_ctch_decision(void);

/*===========================================================================

FUNCTION     DRX_OFFLINE_PICH_SAMPLE_SIZE_INC_BPGS

DESCRIPTION
  This function returns the number of target ticks increase for SS freeze for short and long QXT.
  SS mode - returns 0
  DS mode - returns 9

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern uint8 drx_offline_pich_sample_size_inc_bpgs(void);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION     drx_reinit_outer_loop_vars

DESCRIPTION
  This function is called to reinitialize ticks_rem_adj_sleep_longer_cx8

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  ticks_rem_adj_sleep_longer_Cx8 may not be reinitialized if less than
  default value
===========================================================================*/
extern void drx_reinit_outer_loop_vars(void);

/*===========================================================================
FUNCTION     DRX_GET_NULL_DRX_CYCLE

DESCRIPTION
  This function returns drx_is_null_drx_cycle varaiable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern boolean drx_get_null_drx_cycle(void);

/*===========================================================================
FUNCTION     DRX_SET_NULL_DRX_CYCLE

DESCRIPTION
  This function sets drx_is_null_drx_cycle varaiable to the 
  value of the argument passed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_set_null_drx_cycle(boolean value);
#endif

/*===========================================================================

FUNCTION     DRX_CALC_MDSP_PARMS_WITH_NEXT_PO

DESCRIPTION
  This function calculates all MDSP parms for the next paging occasion after we know the
  next paging occasion.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void drx_calc_mdsp_parms_with_next_po(void);

/*===========================================================================
FUNCTION     DRX_QUERY_BLOCK_SRCH_FOR_CTCH_DECISION

DESCRIPTION
  This function is called before Ifreq or Irat search is issued. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE 
 TRUE - block search
 FASLE - don't block search
===========================================================================*/
extern boolean drx_query_block_srch_for_ctch_decision(void);

/*=========================================================================== 
FUNCTION DRX_CALCULATE_GAPS_BETWEEN_CTCH 
 
DESCRIPTION 
This function calculates the gap in frames between CTCH sfn's. It considers 
the setup and drop time for CTCH while calculating the gaps 
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE 
 returns the Gap in frames
 
===========================================================================*/
uint16 drx_calculate_gaps_between_ctch(boolean loop_selector);

/*===========================================================================
FUNCTION     DRX_CALL_CAN_L1_GO_TO_SLEEP

DESCRIPTION
  This function will be called while L1 is going to sleep, This
  calls RRC's api so that they can start/abort BPLMN if required.

DEPENDENCIES
  None

RETURN VALUE
  Boolean can_l1_sleep  =  TRUE  - L1 can go to sleep
                           FALSE - L1 shouldnt go to sleep but start BPLMN

PARAMETERS
  ms_to_sleep - Time L1 has to sleep.
   
SIDE EFFECTS

===========================================================================*/
extern boolean drx_call_can_l1_go_to_sleep(uint32 ms_to_sleep);

/*===========================================================================
FUNCTION DRX_READ_IDLE_MODE_DEBUG_NV

DESCRIPTION
      NV used for enable/disable debugging in idle mode.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void drx_read_idle_mode_debug_nv(void);

/*===========================================================================
FUNCTION     DRX_UPDATE_DRX_DELAYED_TIMER_VAL

DESCRIPTION
  This function is called whenever we get a drx_cmd to update the
  value of drx_delayed debug timer.
 
PARAMETERS 
  uint16 drx_cycle_len      - DRX cycle length.
  boolean wake_up_for_ctch  - TRUE : non PICH entitiy is waking up next.
                              FASLE: PICH entity will wakeup next.
  int32 sleep_ms            - Is valid only if wake_up_for_ctch is TRUE.
                              provides sleep time for non PICH entity.
RETURN VALUE
  None
 
SIDE EFFECTS
  Global variable drx_delayed_timer_ms is updated with computed values.

===========================================================================*/
extern void drx_update_drx_delayed_timer_val(uint16 drx_cycle_len,boolean wake_up_for_ctch,int32 sleep_ms);

/*=========================================================================== 
FUNCTION DRX_PREP_FOR_CALL_EST
 
DESCRIPTION 
  This function informs DRX module to prepare for call establishment.
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE 
  None.
 
===========================================================================*/
extern void drx_prep_for_call_est(void);

/*=========================================================================== 
FUNCTION DRX_HANDLE_PREP_FOR_LOCK_REL_CALL_EST
 
DESCRIPTION 
  This function prepares the DRX module to prepare for all its activities
  to quickly release the lock before call setup.
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE 
  None.
 
===========================================================================*/
extern void drx_handle_prep_for_lock_rel_call_est(void);

/*=========================================================================== 
FUNCTION DRX_CHECK_WAITING_ON_CALL_EST
 
DESCRIPTION 
  This DRX function checks if we are waiting on a WRM PENDING CMD from RRC on 
  call establishment.
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE 
  None.
 
===========================================================================*/
extern boolean drx_check_waiting_on_call_est(void);

/*===========================================================================
FUNCTION DRX_GET_CURR_RXD_MODE

DESCRIPTION
    This function returns current RXD_State after wakeup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
rxd_pch_mode_enum_type drx_get_curr_rxd_mode();

/*===========================================================================
FUNCTION DRX_GET_PREV_RXD_MODE

DESCRIPTION
  This function returns prev RXD_State during sleep. (Used for parallel Reacq case) 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
rxd_pch_mode_enum_type drx_get_prev_rxd_mode();

#ifdef FEATURE_WCDMA_REACQLESS_PICH
/*===========================================================================
FUNCTION drx_configure_num_pi_bits_in_rlp

DESCRIPTION
    This function configures the number of PI bits that need to be decoded. This might be 
    different from the actual PI bits transmitted since we want to do partial decoding whenever
    we do not cross the (num_pi_bits/2) at the time of AGC tune done in case of Offline PICH.
  
DEPENDENCIES
   This will be done when FEATURE_WCDMA_REACQ_LESS_PICH is defined.

RETURN VALUE
  None

SIDE EFFECTS
  PI decoding is done on less than the number of PI bits transmitted by the network.
===========================================================================*/
extern void drx_configure_num_pi_bits_in_rlp(uint8 *bits_per_pi_ind,uint16 *trans_time_offset);

/*===========================================================================
FUNCTION drx_update_num_pi_bits_in_rlp

DESCRIPTION
    This function updates the bits per PI for an Reacqless PICH cycle in the pointer 
    that is passed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void drx_update_num_pi_bits_in_rlp(uint8 *bits_per_pi_ind);
/*===========================================================================
FUNCTION DRX_PRINT_FW_RAW_CPICH_SYMBOLS

DESCRIPTION
    This function prints raw FW CPICH and PICH symbols exported by FW
    when RLP and drx debug NV is enabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void drx_print_fw_raw_cpich_symbols(void);
#endif

/*===========================================================================
FUNCTION     DRX_GET_PARMS_FOR_PICH_LOG_PKT

DESCRIPTION
  API used to get FW, freq err and fing info to be populated for
  PICH log pkt later

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None
===========================================================================*/
extern void drx_get_parms_for_pich_log_pkt(void);

/*===========================================================================
FUNCTION     DRX_IS_RLP_ENABLED_WITH_NBRS

DESCRIPTION
  This function returns the NV variable which indicates whether RLP is enabled
   with Nbrs.
  
PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean flag - FALSE : deafult value RLP is disabled with Nbrs.
               - TRUE  : RLP is enabled with all kinds of Nbrs.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean drx_is_rlp_enabled_with_nbrs(void);
/*===========================================================================
FUNCTION    drx_post_m2m_drx

DESCRIPTION


DEPENDENCIES
  None.

PARAMETERS
  none

RETURN VALUE
  none
===========================================================================*/
extern void drx_post_m2m_drx(void);

#ifdef FEATURE_QSH_DUMP
extern void wl1_drx_sleep_dump_cb(void* addr_ptr);
#endif

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION    drx_post_fake_drx

DESCRIPTION
  When an activity with TRM_ACCESS_URGENT requiring a device switch is sent by RRC 
  in PCH before DRX cmd, L1 has to post a DRX to itself first.This enable the release
  of the current device so that the pending req and notify from RRC can be handled.

DEPENDENCIES
  None.

PARAMETERS
  none

RETURN VALUE
  none
===========================================================================*/
extern void drx_post_fake_drx(void);
#endif

#endif /* DRX_H */
