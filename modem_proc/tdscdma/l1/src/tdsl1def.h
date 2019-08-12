#ifndef TDSL1DEF_H
#define TDSL1DEF_H
/*===========================================================================

                    L 1   C O M M O N   D E F I N I T I O N S
DESCRIPTION
  This file contains common declarations and references for the units
  within L1.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2000 - 2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1def.h_v   1.33   19 Jul 2002 13:57:46   djm  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsl1def.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/23/13   yuw     Updated EVENT_TDSCDMA_L1_HSUPA_STATUS
05/24/12   kguo    Updated code under feature FEATURE_TDSCDMA_RX_DIVERSITY
06/30/10   vsr     Updated EQ logging for Dual Carrier
06/03/10   ms      Added support to log DC-HSDPA events
06/02/10   stk     Added support for W2LTE measurements and reselection under 
                   FEATURE_TDSCDMA_ABSOLUTE_PRIORITY & FEATURE_TDSCDMA_TO_LTE
04/20/10   ms      Added TDS_L1_HS_DEREGISTER_CLIENT_CMD
03/30/10   rvs     Remove FEATURE_WCDMA_CME_CONTROLLER.
03/26/10   hk      Freeze/UnFreeze CM during ASU procedure
03/08/10   rgn     Flexible carrier spacing changes 
02/10/10   ms      Added carrier id enum and max val macros
02/08/10   rgn     Syncup to mustang tip 
11/18/09   gv      Use db10 instead of db12 units for computation
11/10/09   gv      Added support for TTI RECONFIG EVENT on the offline builds.
11/09/09   rgn     Added local command to handle pending cell cfg/decfg state
11/07/09   hk      Added the support to enable DC on WPLT test mode
11/06/09   hk      Added the support to enable DC on R99
10/12/09   stk     Create a new local cmd, to call srch_clean_up done in task context
09/24/09   hk      Porting of AICH resetup feature to Offline
09/09/09   rvs     Add support for QICE F/Finv log packet.
07/15/09   rvs     Added the L1_CME_SUBMIT_QICE_LOG event and edited feature
                   flags to include FEATURE_WCDMA_QICE_CONTROLLER.
06/22/09   mr      Support for new HSPA+ events 
06/01/09   sv      Mainlined the search abort code.
05/05/09   sv      Added TDS_SRCH_ABORT_DONE_CMD to list of local cmds
04/15/09   ks      Added enum type for RxD triage local cmd
02/13/09   ks      Added enum type for triage local cmd
02/03/09   ka      Corrected code featurization (FEATURE_WCDMA_REL6_ENH_MPR)
01/20/08   ka      Added code changes for logging 4313
12/19/08   yh      Add CM-CME local cmd header
12/09/08   uk      Mainling FEATURE_TDSCDMA_DL_ENHANCED
11/04/08   rgn     New demod status dump designchanges
10/31/08   vsr     Added support for SCH-IC log packet
10/03/08   hk      Bringing in 7k Mailine fixes
09/29/08   mc      Added support for CME log packet
09/18/08   saik    Ensured compilation for Q6 when msmdsp.h file not present(using feature FEATURE_WFW_SW_INTERFACE)
08/27/08   vsr     Added CME capability under feature FEATURE_WCDMA_CME_CONTROLLER
08/20/08   gnk     Not using the function enc_print_rach_timing_debug
06/10/08   ms      Added PACKED_POST attribute for structures to make it
                   compatible for GNU compiler.
01/18/08   uk      Added TDS_SRCHCR_GSM_CLK_ENABLED_IN_DRX to tdsl1def_l1_local_cmd_enum_type
01/16/08   vp      Added support for PCCPCH weak indication
01/16/08   mc      Added support for MCCH weak indication
12/26/07   rc      Added TDS_L1_INTERNAL_MEAS_EVT_6A_TO_6E_EVAL_CMD to
                   tdsl1def_l1_local_cmd_enum_type.
10/24/07   vsr     Mainlining MBMS and Enhanced DL changes
10/15/07   gnk     Added Prach log packets & F3 msgs temporarily
09/26/07   nd      Added a new local command TDS_SRCH_FS_PROC_DONE which will be sent by the
                   ISR (fw interrupts after processing N slots of sample)
07/09/07   vb      Added rxd log submit local cmd
04/17/07   gnk     Changed enc_rach_no_ack_print_debug() to enc_print_rach_timing_debug()
                   to print timing information for all RACH procedures
03/16/07   am      4312 logpkt changes
01/19/06   kps     Added TDS_TRM_ENABLE_RXD_CMD to enum tdsl1def_l1_local_cmd_enum_type
10/27/06   kps     Added TDS_TCXO_SET_RESTRICTION_CMD to enum tdsl1def_l1_local_cmd_enum_type
10/12/06   gv      Added structures to support the events specific to
                   HSUPA feature(reconfig_or_asu and serv_cell_change events).
10/10/06   ms      Added TDS_L1_HS_MODULE_CLEANUP_AFTER_HS_TEARDOWN_CMD to
                   enum tdsl1def_l1_local_cmd_enum_type.
10/02/06   kps     Added TDS_TRM_STOP_CMD to enum tdsl1def_l1_local_cmd_enum_type
09/25/06   gv      Removed the local command E_DL_DEBUG_LOGGING_CMD,as it is
                   no longer being used.
08/14/06   yiz     Mainline HSUPA code
07/06/06   kps     Added TDS_SIB_TIMER_EXPIRY_CMD to enum tdsl1def_l1_local_cmd_enum_type
05/08/06   scm     Add updated reselection start event with 16 bits for PSC.
04/10/06   mc      Added support for EQ log packets
04/04/06   vip     Added local command for processing the hardware callbacks
                   received by RxD controller
03/21/06   vip     Added the local command for processing RxD
                    state update event
03/16/06   mc      Added support for Equalizer under the feature flag
                   FEATURE_WCDMA_EQ_CONTROLLER
08/18/05   vp      Added support for reslamming the pccpch for the TTI-1
                   hypothesis.
07/21/05   mc      Added HS_DSCH_STATUS event structure.
07/19/05   asm     Added a new command to log agc
07/08/05   mc      Added HS_SERVING_CELL_CHANGE event structure.
06/15/05   yiz     Disable TX pwr lim cmd for FEATURE_RF_TX_PWR_LIM_REV
05/05/05   gs      Added enum TDS_MDSP_CMD_LOG_SUBMIT to local command
03/07/05   rc      Added TDS_QUALMEAS_CRC_UPDATE_CMD to enum tdsl1def_l1_local_cmd_enum_type
                   under feature FEATURE_TDSCDMA_QUALITY_MEAS.
01/28/05   rc      Added TDS_EVALUATE_INTERRAT_EVENTS_CMD to enum tdsl1def_l1_local_cmd_enum_type.
11/15/04   sh      Removed SRCHCR_START_NSET_SRCH_CMD.
10/11/04   gs      Added local command codes for HSDPA
04/15/04   yiz     Rename FACH inter-RAT CR as FEATURE_CELL_FACH_MEAS_OCCASION
12/18/03   scm     Add TDSL1DEF_FAILURE_REASON_NOT_APPLICABLE define for SUCCESS case.
12/10/03   yiz     Add TDS_SRCHFACH_GET_CRNTI_DONE_CMD & TDS_SRCH_FACH_MEAS_CMD for
12/08/03   yiz     Replace last_pwr_sett with rf_tx_pwr_dbm in
                   tdsl1def_prach_evt_type for EVENT_WCDMA_PRACH.
11/10/03   yus     Delete FEATURE_6250_COMPILE.
10/01/03   sh      Added TDS_UL_RACH_DONE_CMD local command enum type.
07/24/03   yus     Include "msm.h" for MSM6250.
05/13/03   scm     Add event structure for new event: EVENT_WCDMA_DRX_CYCLE.
04/02/03   src     Added a new enum TDS_L1_PHYCHAN_NOOP_DONE_CMD to the L1 local
                   command type tdsl1def_l1_local_cmd_enum_type.
03/24/03   yiz     Remove FEATURE_RACH_NO_ACK_DEBUG conditions to always enable.
03/14/03   scm     Remove failure_reason from tdsl1def_resume_evt_type.
03/05/03   scm     Change "num_frames" to "num_frames_suspended" to match ICD.
                   Add event structure for EVENT_WCDMA_L1_STATE.
02/28/03   scm     Putting all WCDMA L1 QXDM event structures in this file.
12/18/02   ATM     Made COMMAND enum CFA-feature friendly
10/01/02   ddh     Added TDS_SRCHCR_CHK_N_SRCH_CMD and SRCHCR_GSM_NSET_SRCH_CMDs
09/06/02   djm     ifdef FEATURE_CM_TGPS_RUNTIME_ERROR runtime error command
08/27/02   sh      Added TDS_SEQ_ALIGN_CELL_TRANS_DONE sent after sequencer
                   aligns to new cell timing during cell reselection.
07/18/02   djm     Addition of Compressed Mode TGPS RunTime Error local command
07/10/02   djm     Addition of Compressed Mode SRCH Done local command for possible use
06/24/02   sh      Added UL_RACH_NO_ACK_DEBUG_CMD for rach no-ack debug.
05/29/02   m       Added support for LOD by adding new L1 local command
05/22/02   sh/src  Undid mapping of tds_l1_tcb to l1_tcb as the latter is no
                   longer used.
05/16/02   yus     Mapping tds_l1_tcb to l1_tcb, tds_l1_task to l1_task.
05/18/02   sh      Added the following local commands to support cell
                   reselection: TDS_SRCH_CELL_RESEL_EVAL_CMD,
                   SRCH_NSET_SRCH_SORT_CMD, SRCH_NSET_SRCH_SORT_DONE_CMD.
05/10/02   cpe     Added TDS_SRCH_SFN_SFN_OBSERVED_DIFF_CMD and TDS_DL_MEAS_SFN_SFN_DIFF_CMD
                   to the l1 local commands
04/29/02   yus     Add #include "msmdsp.h"
04/12/02   yiz     Added TDS_UL_PHYCHAN_DPCH_LOG_DONE_CMD to
                   tdsl1def_l1_local_cmd_enum_type to change to deferred logging.
02/26/02   sh      Added FEATURE_L1_ACT_TIME to tdsl1def_l1_local_cmd_enum_type.
02/25/02   sh      Removed PHYCHAN_SETUP_START_CMD and added TDS_ACT_TIME_CMD to
                   tdsl1def_l1_local_cmd_enum_type.
02/21/02   sh      Added PHYCHAN_SETUP_START_CMD and
                   TDS_UL_DL_PHYCHAN_DROP_DONE_CMD to tdsl1def_l1_local_cmd_enum_type.
01/31/02   asr     added TDS_DRX_SLEEP_CMD to tdsl1def_l1_local_cmd_enum_type
12/13/01   asr     added TDS_DRX_LOG_DONE_CMD to tdsl1def_l1_local_cmd_enum_type
09/13/01   sk/yiz  Added TDS_RF_DO_TX_PWR_LIM_CMD local command definition.
08/09/01   sh/sk   Added defines for DCH and PRACH log functions.
07/15/01   sh      Changed local cmd enum names related to physical channel
                   setups.

05/21/01   sh      Removed TDS_RF_TUNE_DONE_CMD.

05/13/01   wjn     Added TDS_DL_MEAS_SFN_CFN_DIFF_CMD and
                   TDS_SRCH_SFN_CFN_OBSVED_DIFF_CMD.
                   Deleted SRCH_NBCH_SFN_INFO_CMD.

03/29/01   sh      Added local command enum TDS_DL_CELL_TRANS_DROP_DONE_CMD.

02/21/2001 mdl     removed DL_ACQ_PHYCHAN_CFG_CMD AND
                   DL_ACQ_PHYCHAN_FRM_BDRY_CMD from tdsl1def_l1_local_cmd_enum_type

02/14/01   wjn     Added L1 local commands TDS_SRCH_CPHY_ASET_CMD and
                   PHYCHAN_ADD_DONE_CMD for channel setup handshake between
                   SRCh and DL module.
01/24/01   mdl     Added TDS_MODULE_CLEANUP_DONE_CMD enum.

01/10/2001 mdl     Added TDS_MDSP_SYNC_TIMEOUT_CMD enum

12/21/00   sh      Removed cmd_hdr_type and added q_link_type in
                   tdsl1def_l1_local_cmd_hdr_type.

11/16/00   sh      Added DL_ACQ_PHYCHAN_CFG_CMD, DL_ACQ_PHYCHAN_FRM_BDRY_CMD,
                   TDS_RF_TUNE_DONE_CMD local commands.
                   Deleted UL_PHYCHAN_SETUP_DONE_CMD, DL_PHYCHAN_SETUP_DONE_CMD
                   local commands.

08/15/00   yus     Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "customer.h"
#include "rex.h"
#include "queue.h"

#include "log.h"
#include "tdsl1msm.h"

#ifdef FEATURE_TDSCDMA_HSUPA
#include "tdseul.h"
#endif

#ifndef FEATURE_XO
#define FEATURE_XO
#endif

#include "event.h"
#include "event_defs.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* L1 local command Enumerated IDs */
/* ************************************** *
 * This list is used by logging!!!        *
 * Send updates to cfa.team               *
 * ************************************** */
typedef enum
{
  /* Search Dump Signal */
  TDS_SRCH_DONE_CMD               =   0x0,

  /* Search Heartbeat Signal */
  TDS_SRCH_OPS_CMD                =   0x1,
  /*zhangyan modified codes,TBD*/
  TDS_GSM_SRCH_OPS_CMD                =   0x1B,

  /* Lost Search Signal */
  TDS_SRCH_LOST_SRCH_CMD          =   0x2,

  /* Demodulation Cell Update CMD, conveying cell information in CPHY_SETUP */
  TDS_SRCH_CPHY_ASET_CMD          =   0x3,

  /* Local command to indicate nset search/sort is done. */
  TDS_SRCHCR_NSET_SRCH_DONE_CMD   =   0x8,

#if 0
  /* Local command to check if an inter-freq or inter-rat srch
   * should be started or if the intra-freq results should just
   * be processed after the PICH has been demodulated
   */
  TDS_SRCHCR_CHK_N_SRCH_CMD       =   0x9,

#ifdef FEATURE_TDSCDMA_CELL_RESELECT_INTER_S
  /* command to search/sort GSM cells */
  TDS_SRCHCR_GSM_NSET_SRCH_CMD    =   0xA,
#endif /* FEATURE_TDSCDMA_CELL_RESELECT_INTER_S */

#ifdef FEATURE_CM_MEASUREMENT
  /* Compressed Mode Measurement Done Local Command */
  TDS_CM_MEAS_DONE_CMD            =   0xB,
#endif /* FEATURE_CM_MEASUREMENT */

#endif
  /* Command to indicate next operation during Activation Time */
  TDS_ACT_TIME_CMD                =   0xD,

  /* Commands to setup UL/DL CCTrCh */
  TDS_UL_CCTRCH_SETUP_DONE_CMD    =   0xE,
  TDS_DL_CCTRCH_SETUP_DONE_CMD    =   0xF,

  /* Local Commands related to DL/UL Phy Chan drops */
  TDS_UL_DL_PHYCHAN_DROP_DONE_CMD =   0x10,
  TDS_UL_PHYCHAN_DROP_DONE_CMD    =   0x11,
  TDS_DL_PHYCHAN_DROP_DONE_CMD    =   0x12,

  /* Local Commands related to DL/UL Phy Chan configs */
  TDS_UL_PHYCHAN_CFG_DONE_CMD     =   0x13,
  TDS_DL_PHYCHAN_CFG_DONE_CMD     =   0x14,

  /* Local Commands related to DL/UL Phy Chan adds */
  TDS_UL_PHYCHAN_ADD_DONE_CMD     =   0x15,
  TDS_DL_PHYCHAN_ADD_DONE_CMD     =   0x16,

  TDS_UL_CELL_TRANS_DROP_DONE_CMD =   0x18,
  TDS_DL_CELL_TRANS_DROP_DONE_CMD =   0x19,

  /* Indicates sequencer has been aligned to new cell timing
   * during cell transition.
   */
  TDS_SEQ_ALIGN_CELL_TRANS_DONE   =   0x1A,

  /* Local Command sent by modules to signal
   * end of cleanup for the module.
   */
  TDS_MODULE_CLEANUP_DONE_CMD     =   0x1D,
  TDS_MODULE_DROP_DONE_CMD     =   0x1E,
  TDS_MODULE_HSDPA_A2_DONE_CMD     =   0x1F,  
  TDS_DL_PHCH_CFG_DONE_CMD        =   0x20,
  TDS_DL_ACQ_PULLIN_CMD           =   0x22,
  TDS_DL_ACQ_BCH_DECODE_DONE_CMD   =   0x25,
  /* Used to send BCCH BLER status to DL*/
  TDS_DL_PCCPCH_WEAK_IND_CMD       =   0x26,
  TDS_SRCH_SFN_WRC_DIFF_DONE_CMD   =   0x27,
  TDS_DL_BCH_CRC_STATUS_CMD       =   0x28,
  TDS_DL_TRBLK_CRC_STATUS_CMD     =   0x29,
  TDS_UL_PHYCHAN_DPCH_LOG_DONE_CMD=   0x2C,
  TDS_UL_PRACH_LOG_DONE_CMD       =   0x2D,
  TDS_UL_TRCH_CODING_LOG_DONE_CMD =   0x2E,

  TDS_DRX_LOG_DONE_CMD            =   0x2F,
  TDS_DRX_SLEEP_CMD               =   0x30,

#ifdef FEATURE_L1_LOG_ON_DEMAND
  TDS_LOG_ON_DEMAND_CMD           =   0x31,
#endif

  TDS_UL_RACH_TIMING_DEBUG_CMD    =   0x32,

  /* The primordial no-op or unknown op */
  TDS_L1_PHYCHAN_NOOP_DONE_CMD    =   0x33,

  /* signals rach isr done for l1m to proceed with channel setup */
  TDS_UL_RACH_DONE_CMD            =   0x34,

  #ifdef FEATURE_TDSCDMA_CELL_FACH_MEAS_OCCASION
  /* signals SRCHFACH to calc meas oksn's and start meas */
  TDS_SRCHFACH_GET_CRNTI_DONE_CMD =   0x35,
  TDS_SRCH_FACH_MEAS_CMD          =   0x36,
  #endif

  /* Local command to evaluate the inter-rat events */
  TDS_EVALUATE_INTERRAT_EVENTS_CMD =  0x37,

  /* Signals that HSDPA setup has completed */
  TDS_L1_HS_CHAN_CFG_DONE         =   0x38,

  /* Call functions to submit any accumulated log packets in HS */
  TDS_L1_HS_SUBMIT_LOG            =   0x39,

  #ifdef FEATURE_TDSCDMA_QUALITY_MEAS
  TDS_QUALMEAS_CRC_UPDATE_CMD     = 0x3A,
  #endif

  /* Signals to drop MCPM after HSDPA drop  */
  TDS_L1_HS_DROP_MCPM_CMD      =   0x3B,

  TDSRXD_RELEASE_DIV_CHAIN_NOW_CMD =  0x3F,
  TDS_DL_RXD_STATE_UPDATE_CMD     =   0x40,

  TDS_DL_RXD_HW_CB_CMD           =   0x41,
  #ifdef FEATURE_TDSCDMA_RXD_MEAS_OPT
  TDS_RXD_MEAS_LOG_SUBMIT_CMD = 0x42,
  #endif


#ifdef FEATURE_TDSCDMA_HSUPA
  //TDS_E_DL_CHAN_CFG_DONE_CMD    =   0x44,
  TDS_NON_SCHED_GRANT_CFG_DONE_CMD = 0X44,
  TDS_E_DL_FRM_BDRY_DONE_CMD    =   0x45,
  TDS_E_DL_PHYCHAN_DONE_CMD     =   0x46,
  TDS_E_UL_PHYCHAN_DONE_CMD     =   0x47,
  TDS_E_UL_BUILD_SS_TABLES_CMD  =   0x49,
 #if defined( FEATURE_TDSCDMA_L1_LOGGING_ENABLED )
  TDS_E_UL_LOGGING_CMD          =   0x4A,
 #endif
 #ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
  TDS_E_DL_LOGGING_CMD          =   0x4B,
 #endif
  TDS_E_UL_IRQ_FLOW_CTRL_NV_WRITE_CMD = 0x4C,
#endif


  /* Signals that WL1 must go to TDSL1M_WAIT_FOR_TRM_STOP state */
  TDS_TRM_STOP_CMD                =   0x4D,


  /* Call function for module cleanup done after HS teardown in L1 task context */
  TDS_L1_HS_MODULE_CLEANUP_AFTER_HS_TEARDOWN_CMD = 0x4E,

  /* Signals that WCDMA needs to set the tcxo restriction */
  TDS_TCXO_SET_RESTRICTION_CMD       =   0x55,

  /* TDS_DRX_MANAGER_CMD for commands from other subsystems, like SRCH.
  ** TDS_DRX_ENTITY_CMD for commands for a particular channel state machine. */
  TDS_DRX_MANAGER_CMD             =   0x56,
  TDS_DRX_ENTITY_CMD              =   0x57,

  /* Signals that RxD can be enabled since we just got control of the
  ** secondary antenna */
  TDS_TRM_ENABLE_RXD_CMD             =   0x58,
 
#ifndef FEATURE_TDSCDMA_HSUPA
  /*used to log eul dpch logpkt when HSUPA is not defined*/
  TDS_EUL_DPCH_LOGGING_CMD        = 0x59,
#endif
 /*Used to evaluate events 6A to 6G in DCH state when internal meas
     are configured by n/w. Posted in wenc.c every 10msec */
  /*zhangyan modified codes: change 
  TDS_L1_INTERNAL_MEAS_EVT_6A_TO_6E_EVAL_CMD to
  TDS_L1_INTERNAL_MEAS_EVT_6A_TO_6G_EVAL_CMD*/
  TDS_L1_INTERNAL_MEAS_EVT_6A_TO_6G_EVAL_CMD = 0x5D,

  TDS_SRCHCR_GSM_CLK_ENABLED_IN_DRX   = 0x5E,

  TDS_DL_MCAL_CFG_CMD             = 0x60,
  TDS_DL_CELL_CFG_DONE_CMD        = 0x61,
  TDS_DL_CELL_TD_DET_NOTIFY_CMD   = 0x62,
  //#ifdef FEATURE_WCDMA_QICE_CONTROLLER
  //TDS_L1_CM_CME_CMD               = 0x6A,
  //#endif
  /* command for logging UL TX RF log packet */
  TDS_UL_TX_RF_LOGGING_CMD        = 0x6B,

  /* Search Abort done Signal */
  TDS_SRCH_ABORT_DONE_CMD         = 0x6E,

  /*Command for indicating the start of a CFN to NSCH Manager*/
  TDS_DL_NSCH_CFN_UPD_CMD       =   0x6F,

  #if 0
  /* Post a local command so that the srch_cleanup done command and cell_de_cfg is called in TASK context */
  TDS_SRCH_CLEANUP_DONE_CMD     = 0x70,
  #endif
  /* local command to initiate dc drop after hs channel on dc is dropped*/
  TDS_L1_HS_DC_CHAN_DROP_DONE_CMD = 0x71,
  #if 0
  /*to signal dc cfg done */
  TDS_L1_DC_CFG_DONE = 0x72,

  /* to signal cleanup on hold*/
  TDS_SRCH_CLEANUP_ON_HOLD      = 0x73,
  #endif
  /*Local command to indicate the AGC Tuning done and inform the corresponding clients by
    initiating the post processor call backs in task context*/
  TDS_L1_AGC_TUNE_DONE_CMD = 0x74,
  TDS_L1_DC_START_WAIT_FOR_RF_TUNE = 0x75,

  TDS_L1_CM_FREEZE_NORMAL_FRAME_CMD = 0x76,

  TDS_L1_HS_DEREGISTER_CLIENT_CMD = 0x77,

  #if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
  TDS_SRCH_LTE_SRCH_DONE_CMD    = 0x78,
  TDS_SRCH_LTE_SRCH_START_CMD = 0x79,
  #endif 

  /* indicate HHO re-sync success */
  TDS_UL_HHO_RESYNC_SUCCESS_IND_CMD = 0x7A,

  /* indicate HHO re-sync fail for no FPACH ACK */
  TDS_UL_HHO_RESYNC_FAIL_IND_CMD = 0x7B,

#ifdef FEATURE_TDSCDMA_HSUPA
  /* command for EUL to start E-RUCCH transmission */
  TDS_EUL_ERUCCH_START_UL_SYNC_CMD = 0x7C,

  /* command for EUL to abort E-RUCCH transmission */
  TDS_EUL_ERUCCH_ABORT_CMD  = 0x7D,
#endif

  /* command for DL to notify L1M that it's time to tune frequency (if need)
   * and update ASET since N-PCCPCH is added done and DL is ready for decoding
   * NBCH. This command is available in FACH state only
   */
  TDS_L1_CELL_CHANGE_IND_CMD = 0x7E,
  
  TDS_L1_CHANNEL_SETUP_START = 0x7F,

  /*The general DL related logpkt submit entry */
  TDS_L1_DL_LOGPKT_SUBMIT_CMD = 0x80,
  /*DL logpkt sub type : datapath logging*/
  TDS_L1_DL_DATAPATH_LOGPKT_SUBMIT = 0x81,
  TDS_L1_DL_PICH_DECODE_LOGPKT_SUBMIT = 0x82,
  /*DL logpkt sub-type: olpc update logging*/
  TDS_L1_DL_OLPCUPDT_LOGPKT_SUBMIT = 0x83,
  TDS_L1_FACH_RACH_RESUME_DONE_IND_CMD = 0x84,
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  TDS_L1_EUTRA_INTER_RAT_PERIODIC_MEAS = 0x85,  
#endif 
  #ifdef FEATURE_TDSCDMA_L1M_PROFILING_LOG_PACKET
  TDS_L1M_LOG_PACKET_TRIGGER_CMD = 0x86,
  #endif
  TDS_L1_L1M_TRIGGER_RF_SCRIPT_CMD = 0x87,
#ifdef FEATURE_TDSCDMA_DSDS /*FEATURE_DUAL_SIM*/
  TDS_L1_START_CQI_ZERO_CMD                  = 0x88,
  
  TDS_L1_CQI_ZERO_TIMEOUT_CMD                = 0x89,
#endif

  TDS_L1_MCAL_PCH_DECODING_DONE_IND_CMD = 0x8a,
  TDS_L1_SUSPEND_CMD_TRIGGER_CMD = 0x8b,
  
  /* for TDRM */
  TDS_L1_TDRM_GRANT_CALLBACK_CMD = 0x8c,

  TDS_L1_WAKEUP_CANCEL_CMD = 0x8d,
  
    TDS_L1_IRAT_PRD_RPT_CMD = 0x8e,

  TDS_L1_RF_EARLY_SLEEP_CMD = 0x8f,

  TDS_L1_STOP_CMD_TRIGGER_CMD = 0x90,

  TDS_L1_MCVS_PROCESS_CMD = 0x91,

#ifdef FEATURE_TDSCDMA_LOW_SIB_PRIORITY_DUR_CELL_SEL
  TDS_L1_EXTEND_LOCK_CMD  = 0x92,
#endif

  TDS_L1_TDRM_SEC_GRANT_CALLBACK_CMD = 0x93,
  TDS_L1_TDRM_MODIFY_REASON_CMD = 0x94,
  TDS_L1_TDRM_MODIFY_BAND_CMD = 0x95,
  TDS_L1_TDRM_UNLOCK_CMD = 0x96,

  TDS_L1_MCPM_POWER_REPORTING_CMD = 0x97,

#ifdef FEATURE_TDSCDMA_DR_DSDS /*FEATURE_DUAL_SIM*/
  TDS_L1_T2L_SEC_CHAIN_RELEASE_CMD			   = 0x98,
#endif

#ifdef FEATURE_TDSCDMA_MOTION_SENSOR_AID_MEAS_SUPPRESS
  TDS_L1_MOTION_SENSOR_AID_CTRL_CMD = 0x99,
#endif

#ifdef FEATURE_TDSCDMA_BAND_AVOID
  TDS_L1_BAND_AVOID_PWR_SAMPLE_CMD = 0x9A,
#endif

  TDS_L1_PENDING_CLEANUP_PROC_CMD = 0x9B,
  TDS_L1_X2T_FW_CMD = 0x9C,
  TDS_L1_L1M_CMD_PROCESS_DONE = 0x9D,

#if defined(FEATURE_QSH_EVENT_METRIC) || defined(FEATURE_QSH_DUMP)
  TDS_L1_QSH_INFO_REPORT = 0x9E,
#endif

  TDS_L1_LOCAL_NUM_CMD
} tdsl1def_l1_local_cmd_enum_type;

/*Enum type to indicate the carrier indices*/
typedef enum {
  TDSL1DEF_DL_TRI_PRI_CARR_ID
#ifdef FEATURE_TDS_DC_HSDPA
  ,TDSL1DEF_DL_TRI_SEC_CARR_ID
#endif
} tdsl1def_dl_tri_carr_id_enum_type;

#ifdef FEATURE_TDS_DC_HSDPA
#define TDSL1DEF_WL1_MAX_NUM_CARRIER_IDX 2
#define TDSL1DEF_WL1_MAX_VALID_CARRIER_IDX 1
#else
#define TDSL1DEF_WL1_MAX_NUM_CARRIER_IDX 1
#define TDSL1DEF_WL1_MAX_VALID_CARRIER_IDX 0
#endif

/*+ Temporarily modification for TLM clean up*/
/*--------------------------------------------------------------------------
                   STRUCTURE: TLM_CELL_INFO_STRUCT_TYPE

This structure holds information that will identify a WCDMA cell.
--------------------------------------------------------------------------*/
typedef struct 
{
/*the psc of the cell*/
  uint16 cpid;

/*the frequency of the cell*/
  uint16 freq;
} tdstlm_cell_info_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: TIMING_STRUCT_TYPE

This structure holds a sub-frame quantity in chipx8 resolution.The maximum value
for this quantity is 8191 sub-frames.
--------------------------------------------------------------------------*/
typedef struct {
/*Range is 0-8191*/
  uint32 sub_fn;

/*Range is 0-51199*/
  uint32 cx8_offset;
} tdsl1_time_info_struct_type;


/* Header type to be included by all Local Commands */
typedef struct
{
  /* Queue Link Type */
  q_link_type             link;

  /* enumerated command value */
  tdsl1def_l1_local_cmd_enum_type  cmd_code;

} tdsl1def_l1_local_cmd_hdr_type;

/*=========================================================================**
** WCDMA L1 reports various events to the QXDM, as do lots of other        **
** subsystems.  These are high priority, small data packets used for       **
** post-processing analysis and debugging of phone behavior.  The data     **
** packet event structures used by WCDMA L1 are defined here.              **
** Event enumerated types are in event_defs.h.                             **
**                                                                         **
** These event structures are often instantiated on the stack.  Because    **
** of this, they should be relatively small.  If for some reason they      **
** need to grow a significant amount, they should be moved from stack      **
** space to heap space.                                                    **
**=========================================================================*/

/***************************
EVENT_TDSCDMA_L1_ACQ_SUBSTATE
****************************/
typedef enum
{
  TDSL1_EVENT_ACQ_INIT = 0,
  TDSL1_EVENT_ACQ_STEP0_DONE,
  TDSL1_EVENT_ACQ_STEP1_DONE,
  TDSL1_EVENT_ACQ_PCCPCH_ADD_DONE,
  TDSL1_EVENT_ACQ_DECODE_DONE,
  TDSL1_EVENT_ACQ_SLAM_DONE,
  TDSL1_EVENT_ACQ_COMPLTETE,
  TDSL1_EVENT_ACQ_INVALID
}tdsl1def_acq_substate_enum_type;

typedef PACK(struct)
{
  uint8  acq_substate;  /* Corresponds to tdsl1def_acq_substate_enum_type. */
}
tdsl1def_acq_substate_evt_type;

/***************************
**  EVENT_TDSCDMA_L1_STATE  **
****************************/
typedef PACKED struct PACKED_POST
{
  uint8  tdsl1m_state;  /* Corresponds to tdsl1m_state_enum_type. */
}
tdsl1def_state_change_evt_type;

/*************************************
EVENT_TDSCDMA_MEASUREMENT_EVENT 
**************************************/
typedef PACK(struct)
{
  uint8 evt_id;
  uint8 meas_id;  
  uint16 uarfcn;
  uint8 cpid;
}
tdsl1def_meas_evt_type;

/**************************************************
**  EVENT_TDSCDMA_NEW_SERVING_CELL  **
***************************************************/
typedef PACKED struct PACKED_POST
{
  uint8   cpid;                /* Serving Cell Parameter ID, ID  = 0 to 127. */
  uint16  primary_uarfcn;            /* UARFCN. Integer (0..16383) */
  uint16  working_uarfcn;
}
tdsl1def_new_serving_cell_evt_type;

#ifndef FEATURE_JDS_OPT

/********************************
**  EVENT_TDSCDMA_JDCS  **
********************************/
/*=========================================================================**
  ** The event is to alert the change of membership that could be useful in performance 
  ** test and automation. Let's reduce it to include only two cells that are added or dropped 
  ** if size is the issue since it is the most common case to drop a cell and add another. 
  ** The total number of JDCS is a constant.
**=========================================================================*/
typedef PACKED struct PACKED_POST
{
  uint8   cpid1;                /* Cell Parameter ID, ID  = 0 to 127. */
  uint16 uarfcn1;             /* UARFCN. Integer (0..16383) */
  uint8   type1;                  /* 0 - Drop 1 - Add */
  uint8   cpid2;                /* Cell Parameter ID, ID  = 0 to 127. */
  uint16 uarfcn2;             /* UARFCN. Integer (0..16383) */
  uint8   type2;                  /* 0 - Drop 1 - Add */
}
tdsl1def_jdcs_update_evt_type;

#else
/**************************************************
**  EVENT_TDSCDMA_TS0_JDS_UPDATE   **
***************************************************/
typedef PACKED struct PACKED_POST
{
  uint16  freq;               /* Freqency of ts0 JDS cells */
  uint8   num_ts0_cells;      /* number of ts0 cells. */
  uint8   cpid[8];            /* ts0 cpid array */
}
tdsl1def_ts0_jds_evt_type;

/**************************************************
**  EVENT_TDSCDMA_NONTS0_JDS_UPDATE   **
***************************************************/
typedef PACKED struct PACKED_POST
{
  uint8   num_non_ts0_cells;  /* number of non-ts0 cells. */
  uint8   cpid[4];            /* non-ts0 cpid array */
}
tdsl1def_nonts0_jds_evt_type;
#endif

#if 0
#ifdef FEATURE_WCDMA_UL_RACH_EVT_PWR_FIX

/*******************************
**  EVENT_WCDMA_PRACH  **
*******************************/
typedef PACKED struct PACKED_POST
{
  int8   rf_tx_pwr_dbm;  /* Last RF TX power for RACH power in dBm       */
  uint8  num_preambles;  /* Number of preambles transmitted.             */
  uint8  aich_status;    /* Corresponds to "l1_prach_status_enum_type".  */
}
tdsl1def_prach_evt_type;

#else /* FEATURE_WCDMA_UL_RACH_EVT_PWR_FIX */

/*******************************
**  EVENT_WCDMA_LAYER1_PRACH  **
*******************************/
typedef PACKED struct PACKED_POST
{
  int16  last_pwr_sett;  /* Last RACH power: TX AGC unit. */
                         /* "last_pwr_sett" value read from mDSP.        */
  uint8  num_preambles;  /* Number of preambles transmitted.             */
  uint8  aich_status;    /* Corresponds to "l1_prach_status_enum_type".  */
}
tdsl1def_prach_evt_type;

#endif /* FEATURE_WCDMA_UL_RACH_EVT_PWR_FIX */
#endif

/*****************************
**  EVENT_TDSCDMA_L1_SUSPEND  **
*****************************/
typedef PACKED struct PACKED_POST
{
  uint8   last_cfn;  /* CFN once WCDMA L1 is SUSPENDed. */
  uint16  last_sfn;  /* SFN once WCDMA L1 is SUSPENDed. */
}
tdsl1def_suspend_evt_type;

/****************************
**  EVENT_TDSCDMA_L1_RESUME  **
****************************/
typedef PACKED struct PACKED_POST
{
  uint8   first_cfn;             /* CFN after phychan(s) RESUME'd. */
  uint16  first_sfn;             /* SFN after phychan(s) RESUME'd. */
  uint16  num_frames_suspended;  /* Number of frames L1 SUSPENDED. */
}
tdsl1def_resume_evt_type;

/*****************************
**  EVENT_WCDMA_L1_STOPPED  **
*****************************/
typedef PACKED struct PACKED_POST
{
  uint16  sfn;                   /* SFN at STOP time.                      */
  uint16  num_frames_suspended;  /* Number of frames from SUSPEND to STOP. */
}
tdsl1def_stopped_evt_type;

/*************************************************
**  EVENT_TDSCDMA_TO_TDSCDMA_RESELECTION_START  **
**************************************************/

#define  TDSL1DEF_RESEL_TYPE_INTRA_FREQUENCY  0
#define  TDSL1DEF_RESEL_TYPE_INTER_FREQUENCY  1

typedef PACKED struct PACKED_POST
{
  uint8   resel_type;  /* TDSL1DEF_RESEL_TYPE_INTRA_FREQUENCY or 
                          TDSL1DEF_RESEL_TYPE_INTER_FREQUENCY. */
  uint8   target_cpi;  /* Target cell's cell parameter id. */
  uint16  target_freq; /* Target cell's primary frequency.  */
  uint16  start_sfn;   /* SFN at reselection start time.         */
}
tdsl1def_tds_to_tds_resel_start_evt_type;

/***********************************************
**  EVENT_TDSCDMA_TO_TDSCDMA_RESELECTION_END  **
************************************************/

#define TDSL1DEF_FAILURE_REASON_DL_CCTRCH_SETUP_FAIL      0
#define TDSL1DEF_FAILURE_REASON_DL_PHYCHAN_ADD_FAIL       1
#define TDSL1DEF_FAILURE_REASON_CELL_IS_BARRED            2
#define TDSL1DEF_FAILURE_REASON_PARAM_VALIDATION_FAIL     3
#define TDSL1DEF_FAILURE_REASON_RRC_ABORT_SIB_READING     4
#define TDSL1DEF_FAILURE_REASON_CELL_LOWER_THAN_EXPECTED  5
/* Set "failure_reason" to this on SUCCESS. */
#define TDSL1DEF_FAILURE_REASON_NOT_APPLICABLE            0xFF

typedef PACKED struct PACKED_POST
{
  uint16  end_sfn;         /* SFN at reselection end time.      */
  uint8   status;          /* TRUE = SUCCESS, FALSE = FAILURE.  */
  uint8   failure_reason;  /* The reason why reselection failed */
}
tdsl1def_tds_to_tds_resel_end_evt_type;


/***********************************
**  EVENT_HS_SERVING_CELL_CHANGE  **
***********************************/
typedef PACKED struct PACKED_POST
{
  uint16    old_uarcfn;       /*  Frequency of the previous HS serving cell */
  uint16    new_uarcfn;       /*  Frequency of the new HS serving cell */
  uint16    old_psc;          /*  PSC of the previous HS serving cell */
  uint16    new_psc;          /*  PSC of the new HS serving cell */
  boolean   hrnti_change;     /*  Flag indicating a change in H-RNTI */
  uint8     repoint_type;     /*  Type of repoint - hs_repoint_enum_type */
}
tdsl1def_hs_serving_cell_change_evt_type;

/***********************************
**  EVENT_HS_DSCH_STATUS  **
***********************************/
typedef PACKED struct PACKED_POST
{
  uint8     action;              /* HSDPA channel configuation action */
}
tdsl1def_hs_dsch_status_evt_type;

/***********************************
**  EVENT_HS_SCCH_DECODE_DONE  **
***********************************/
typedef PACKED struct PACKED_POST
{
  uint8     version;              
  boolean   valid;              
}
tdsl1def_hs_scch_decode_done_evt_type;

/***********************************
**  EVENT_TDSCDMA_UL_DPCH_CONFIG_CMD  **
***********************************/
typedef PACKED struct PACKED_POST
{
  uint8     version;              
  uint8     uncodedtfci;              
}
tdsl1def_ul_dpch_config_evt_type;

/***********************************
** EVENT_HSPAPLUS_DC              **
***********************************/
typedef PACKED struct PACKED_POST
{
  /* Bit 0:3 Version 
     Bit 4   Mac-hs/Mac-ehs
     Bit 5:7 Reserved
   */
  uint8 version;
  /* Scrambling code of primary carrier
     Bit 0:3  SSC
     Bit 4:12 PSC
   */
  uint16 scr_code_primary_carrier;
  /* Frequency of primary carrier */
  uint16 uarfcn_primary_carrier;
  /* Call type of primary carrier
     Bit 0   64QAM status
     Bit 1   TBS table used
     Bit 2:3 MIMO action
     Bit 4   P-CPICH TD status
     Bit 5:6 HS-PDSCH TD status
     Bit 7   Reserved
   */
  uint8 call_type_primary_carrier;
  /* MIMO parameters of primary carrier
     Bit 0:3   MIMO N_M ratio
     Bit 4     MIMO Pilot configuration
     Bit 5:12  S-CPICH channelization code (ovsf code for MIMO S-CPICH)
     Bit 13:15 Reserved
   */
  uint16 mimo_info_primary_carrier;
  /* DC action */
  uint8 dc_action;
  /* Scrambling code of secondary carrier
     Bit 0:3  SSC
     Bit 4:12 PSC
   */
  uint16 scr_code_secondary_carrier;
  /* Frequency of secondary carrier */
  uint16 uarfcn_secondary_carrier;
  /* Call type of secondary carrier
     Bit 0   64QAM status
     Bit 1   TBS table used
     Bit 2:3 MIMO action
     Bit 4   P-CPICH TD status
     Bit 5:6 HS-PDSCH TD status
     Bit 7   Reserved
   */
  uint8 call_type_secondary_carrier;
  /* MIMO parameters of secondary carrier
     Bit 0:3   MIMO N_M ratio
     Bit 4     MIMO Pilot configuration
     Bit 5:12  S-CPICH channelization code (ovsf code for MIMO S-CPICH)
     Bit 13:15 Reserved
   */
  uint16 mimo_info_secondary_carrier;
}
tdsl1def_hspa_plus_dc_cfg_evt_type;

#ifdef FEATURE_TDS_DC_HSDPA
/***********************************
** EVENT_HS_SCCH_ORDER_CFG        **
***********************************/
typedef PACKED struct PACKED_POST
{
  /* Bit 0:3 Version 
     Bit 4:7 Reserved
   */
  uint8 version;
  /* Bit 0:1 DC order
             0 - No order
             1 - DC start
             2 - DC stop
             3 - Reserved
     Bit 2:3 DTX order
             0 - No order
             1 - DTX activate
             2 - DTX deactivate
             3 - Reserved
     Bit 4:5 DRX order
             0 - No order
             1 - DRX activate
             2 - DRX deactivate
             3 - Reserved
     Bit 6:7 Reserved (for HSL order)
   */
  uint8 order;
}
tdsl1def_hs_scch_order_evt_type;
#endif /* FEATURE_TDS_DC_HSDPA */

#ifdef FEATURE_TDSCDMA_HSUPA
/************************************
** EVENT_EUL_SERVING_CELL_CHANGE  **
*************************************/

typedef PACKED struct PACKED_POST
{
  uint16 new_psc;
  uint8 action;
  uint8 type;
}
tdsl1def_eul_serv_cell_change_evt_type;

/***************************************
** EVENT_EUL_RECONFIG_OR_ASU **
****************************************/
typedef PACKED struct PACKED_POST
{
  uint8 action;
  uint8 eul_as_size;
  uint16 cell_psc[TDSEUL_MAX_RL];
}
tdsl1def_eul_reconfig_or_asu_evt_type;



#endif /* FEATURE_TDSCDMA_HSUPA */



/***************************
EVENT_TDSCDMA_L1_HSUPA_STATUS
****************************/
typedef enum
{
  TDSL1_HSUPA_SETUP_OPS_NOOP = 0,
  TDSL1_HSUPA_SETUP_OPS_START,
  TDSL1_HSUPA_SETUP_OPS_RECFG,
  TDSL1_HSUPA_SETUP_OPS_STOP
}tdsl1def_l1_hsupa_status_enum_type;

typedef PACK(struct)
{
  uint8  action;  /* Corresponds to tdsl1def_l1_hsupa_status_enum_type. */
}
tdsl1def_l1_hsupa_evt_type;


/***************************
EVENT_TDSCDMA_L1_RXD_STATUS
****************************/
typedef enum
{
  TDSL1_EVENT_L1_RXD_INIT ,
  TDSL1_EVENT_L1_RXD_OFF ,
  TDSL1_EVENT_L1_RXD_ON ,
  TDSL1_EVENT_L1_RXD_TRANS ,
  TDSL1_EVENT_L1_RXD_INVALID
}tdsl1def_l1_rxd_status_enum_type;

typedef PACK(struct)
{
  uint8  state_status;  
}
tdsl1def_l1_rxd_evt_type;


/***************************
EVENT_TDSCDMA_DL_SYNC_STATUS 
****************************/
typedef PACK(struct)
{
  uint8 Sync_state;  /*0 - Out of Sync, 1 - In Sync, 2 - radio link failure*/
}
tdsl1def_dl_sync_evt_type;

/***************************
** EVENT_TDSCDMA_UE_PAGED **
****************************/
typedef PACKED struct PACKED_POST
{
  uint16 sub_fn;            /* sub-frame number when PICH is decoded */
  uint8  paging_indicator;  /* paging indicator in this drx cycle*/
  uint16 drx_cycle_len_ms;  /* DRX cycle length (ms) */
}
tdsl1def_ue_paged_evt_type;

/****************************
** EVENT_TDSCDMA_DRX_REACQ **
*****************************/
typedef PACKED struct PACKED_POST
{
  uint16 sub_fn;  /* sub-frame number when re-acq is performed */
  uint8  cpid;    /* Cell parameter Id*/
  uint16 freq;    /* UARFCN of the wake up cell's Primary Frequecy */
  int16  freq_error_Hz; /* freqency error in Hz */
  uint32 walltimeofs;   /* the offset between rx-time and wall-time */
  uint8  out_of_lock_flag; /* 0 - FALSE; 1 - TRUE */
}
tdsl1def_drx_reacq_evt_type;


/*******************************************
**     EVENT_TDSCDMA_PHYCHAN_CFG_CHANGED  **
********************************************/
typedef PACK (struct)
{
  uint8 status;  
  uint8 ho_type;   
}
tdsl1def_phychan_cfg_changed_evt_type;

#define TDSL1DEF_HO_TYPE_NONE                     0
#define TDSL1DEF_HO_TYPE_HHO                      1
#define TDSL1DEF_HO_TYPE_BHO_0MS                  2
#define TDSL1DEF_HO_TYPE_BHO_80MS                 3
#define TDSL1DEF_HO_TYPE_WORKING_FREQ_CHANGED     4
#define TDSL1DEF_HO_TYPE_TIMESLOT_CHANGED         5

#endif /* TDSL1DEF_H */
