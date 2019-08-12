#ifndef WENC_H
#define WENC_H

/*============================================================================
              E N C O D E R   D R I V E R   H E A D E R   F I L E

DESCRIPTION
This files contains all necessary macros and definitions for interfacing with
the encoder hardware.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/enc.h_v   1.18   10 Jul 2002 19:40:12   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wenc.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
03/18/15    ash     QSH metric addition to add the number of RACH Procedures attempted
02/24/15    ap      Initial check-in for WL1 QSH framework 
01/23/15    gv      Putting back the RACH stats code (incorrectly removed in code cleanup)
11/19/14    vr      Deprecating and removing 0x4312 log packet code.
11/10/14    tsk     Mainline code under feature FEATURE_WCDMA_JOLOKIA_MODEM on Jolokia branch.
10/01/14    gv      Code cleanup. Removal of stale and unused code. 
08/19/14    ash     Added check to see if DL Reslam is done, before calling StmrAction API
08/18/14    pkg     Changes to disable FEATURE_WCDMA_NPA_THERMAL and FEATURE_WCDMA_MTPL_THERMAL_BACKOFF
                    and FEATURE_TEMPERATURE_FC for MAV PL's.
07/28/14    pkg     Code Check-in for 16QAM feature.
07/15/14    ac      NV reorganization
07/11/14    gp      Added support for RF Temp Comp Split architecture
06/19/14    ash     Backed out code for 0x410E Log Pkt
06/04/14    cjz     GCF flag removal and automatic Tx power back-off upon GCF test
06/03/14    tsk     Compilation fixes for JOLOKIA 1.0
05/27/14    pkg     Changes to report Tx power during RACH.
05/23/14    ar      Fix to populate SW buffer with previous frame slotwise TxAGC in HSRACH.
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
05/06/14    raj     DCUPA: QXDM support for TxAccum freeze per carrier
04/27/14    jkb     Add check for Feature dual sim with feature qta
02/14/14    ar      Porting Bolt specific uplink changes on to Bolt 2.0
09/14/13    pr      VSTMR Changes for BOLT
01/22/14    pkg     EUL optimization to avoid HICH result falling into QTA gap and improved RSN management for QTA.
12/11/13    gp      FR:3693 EUL stats API for data services
11/21/13    tsk     FR2573: SAR management with Integrated DSDA changes.
11/19/13    gp      Fix for crash due to race condition on FW dropping DPCH channels immediately
11/19/13    vs      CQI override changes for 3C.
11/14/13    pj      Wrapped HWIO access with WL1_HWIO macro
10/29/13    gp      Fix for thermal mitigation stall 
10/28/13    ar      Removed code under FEATURE_DL_IGNORE_MODE_FACH_MEAS
10/02/13    gp      Add dummy dpcch indication for ul cfg dpcch cmd
09/17/13    jkb     Porting Dual Sim from Triton to Dime
08/22/13    gp      Moved UL Maint parameters into new API enc_set_ul_maint_event_parameters()
                    and ensured that UL DPCCH Cfg Cmd is sent within FW Config Window
08/15/13    at      Post UL DPCH cfg disable done command to L1M after UL DPCH disable done 
                    interrupt from FW is received.
08/15/13    at      Post UL DPCCH cfg disable done command at frame bdry to L1M and
                    trigger fast dch cleanup.
08/13/13    at      Store MPR value per slot for TxAgc filtering and use MPR for event 6D trigger.
07/11/13    sad     Added support to Freeze TxAccum on sec for call spike and 
                    reconfigurations
06/29/13    ash     Clean-up residual transport channels in case the ul_cctrch_cfg_cmd() gets called before the enc_get_data() 
                    ISR fires for the CFN = (ACT TIME - 2) in a Trch Drop case. Wrapped in new function enc_clean_trch()
05/28/13    ash     UL unused Mutex cleanup
05/22/13    oh      Adding DC-HSUPA framework support
05/09/13    gp      Code cleanup for FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND and FEATURE_WCDMA_CONCURRENT_R99_EUL_TX_ENCODE
05/09/12    ar      BOLT Compilation changes
05/06/13    at      SW Workaround for TXHW-A2 issue and cleanup FEATURE_WCDMA_NIKEL_ERAMA_ACCESS_FIX
04/01/13    gp      Code added to provide flexibility to control rach parameters 
                    through NV items.
02/14/13    ash     Adding QXDM Command to crash the UE when specified no. of RACH No-Acks are seen in succession
02/01/13    at      Added code to ensure to disable RF Tx if it is enabled for
                    for RACH procedure to begin when PCCPCH_N for interF SIB read
                    is being set up.
01/24/13    ar      Added support to blindly restore ciphering keys to memory after RF tx is enabled
01/04/13    raj     Changes as a part of ZI memory optimization
12/12/12    ash     Added QXDM cmd for setting Init RACH/DPCCH Pwr and to freeze Tx accum optionally
12/10/12    pkg     wcdma_control_rach_tx_pwr: This API may make UE non-compliant
                    to 3GPP and should be used only for debug and test
11/29/12    pkg     Fixed Compiler Warnings for Triton.
10/17/12    pkg     Added support for WCDMA Tx Power Indication API for GPS
10/01/12    ash     Fixed Compiler warnings for Dime
09/05/12    vr      Moved logging and edrx_hs_notify to task context as HSRACH done
                    ISR was getting severely delayed.
08/31/12    vr      Added fix to avoid unnecessary HARQ level ReTx after Seamless 
                    FACH-->DCH transition for HSRACH
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/28/12    zr      Added feature definitions for Dime
06/19/12    vr      HSRACH code update
06/08/12    gv      Feature HS RACH Initial code check in
05/08/12    ar      Graceful handling of error cond if DeltaHs returned by FW is too high
04/20/12    vr      Warnings on MPSS.NI.3.x. Added function prototype.
04/13/12    ar      Feature cleanup FEATURE_WCDMA_RACH_ABORT_FW_WORKAROUND
03/13/12    sp      Move TX_ENABLED Macros outside RACH Trigger Feature
03/09/12    at      Fixed race condition and crash for Trigger based RACH
02/16/12    sp      Added support for Trigger Based Rach Procedure
02/15/12    sp      Optimized MPR bin implementation to reduce MIPS
02/10/12    rgn     Nikel feature cleanup
02/01/12    raj     Nikel Feature cleanup.
02/01/12    gnk     Nikel Feature cleanup.
02/01/12    gnk     Nikel feature cleanup
01/31/12    raj     Nickel feature cleanup. Mainlined the features FEATURE_WCDMA_UL_TFC_PL_REV_2, FEATURE_WCDMA_REL6_ENH_MPR
01/31/12    gnk     Nikel feature cleanup
01/31/12    gnk     Nikel feature cleanup
01/27/12    vs      Nikel feature cleanup.
01/20/12    bs      free floating changes.
11/29/11    gnk     Removed some unused features
11/24/11    gnk     Removed unused features
10/21/11    sp      Nikel UL CM workaround under FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND
09/21/11    sp      Added NPA Support for Thermal Mitigation under FEATURE_WCDMA_NPA_THERMAL
08/16/11    sp      Delay Enc Cleanup for RACH ABORT Procedure to complete
07/18/11    sp      Added support to use AHB interface for R99 ERAMA WRITE
07/06/11    sb      Upmerge from offline/mainline
06/28/11    sp      Added support for Rach Abort under FEATURE_WCDMA_RACH_ABORT_PROCEDURE
04/28/11    asm     Mainlining FEATURE_WCDMA_TXAGC_DB10 on offline targets
05/11/11    gnk     Mainlining FEATURE_RF_TX_PWR_LIM,FEATURE_RF_TX_PWR_LIM_REV feat
04/28/11    sj      upmerge from offline/main
04/27/11    oh      Support for API to be called by MCPM as a call back 
                    after bringing a modem block out of power collapse.   
04/25/11    geg     Replaced *.h with *_v.h
04/19/11    asm     Enabling the feature FEATURE_WCDMA_HDET_CAL_ERR_IN_L1 for correct Evt 6D triggering
03/30/11    gnk     Mainlined several features
02/23/11    asm     Cleanup of unused, mainlined or irrelevant features in offline UL code
02/11/11    sp      Added support for Thermal Mitigation under FEATURE_WCDMA_MTPL_THERMAL_BACKOFF
01/17/11    sp      Added support for UL_RACH_DEBUG_LOG_PKT (0x4025)
01/17/11    sp      Featurized UL RACH FW Log Pkt (0x4026) under FEATURE_WCDMA_GENESIS_SW_FW
01/07/11    sp      Added support for new UL RACH FW Log Pkt (0x4026)
12/08/10    sp      Keep Burst Interface Enabled in TX_GENERAL_CTL Mask
11/30/10    ksr     TX_GENERAL_CTL should be set to LITTLE ENDIAN for both
                    DCH and E-DCH
11/19/10    sp      Optimized R99 UL Data Dump Log Pkt implementation
11/16/10    gnk     Correcting the calculation of DPCCH pwr for MTD algo
09/02/10    gnk     Checking in the changes for UL open loop MTD feature
09/02/10    gv      Code Checkin for CPC DTX
08/03/10    asm     Delay HS-DPCCH enabling by 1 frame after UL is enabled
07/16/10    asm     Replaced WCDMA_INTLOCK() with local mutexes in R99 UL files
06/10/10    sa      MPR 7.1 support
06/01/10    gnk     QXDM support for setting DIP, RACH init power and Tx freeze
02/26/10    rvs     Fixes for Mustang.
01/13/10    scm     Move enchs_set_cqi_override_from_diag() from enchs.h to wenc.h.
11/20/09    vc      Fixed featuarization.
11/18/09    gv      Use db10 instead of db12 units for computation
08/19/09    ka      Changed UL_NUM_DELTA_HS_VALUES to support Ahs = 38/15.
07/21/09    oh      Fixed inc/src related compilation issue.
06/29/09    oh      Rel-7 E-MPR support for MIMO HS Types.
05/25/09    hk      Fixed compiler warnings
05/22/09    sa      Fixed compiler warnings
04/22/09    gnk     Fixed compiler warnings
04/13/09    asm     Corrections to Evt 6D evaluation based on correct HDET-corrected
                    TxAGC comparison
04/03/09    gv      Creation of wenc.h -- Renaming the file enc.h to wenc.h
01/30/09    ka      Added Support for Enhanced MPR algorithm
01/22/09    gnk     Added support for EDCH to continue TX over seamless reconfig.
12/08/08    ka      Removing external declarations for pa_on and pa_off apis.
12/01/08    oh      Providing API to DEM to turn on/off PA.
10/03/08    hk      Bringing in 7k Mailine fixes
09/16/08    gnk     Lint Fixes
08/20/08    gnk     Not using the function enc_print_rach_timing_debug
07/31/08    oh      EUL changes for Taxis.
07/11/08    ms      Replaced static __inline with INLINE for Q6.
07/11/08    ms      Replaced __inline with static __inline for Q6.
12/27/07    asm     Added a mechanism to calculate slot-wise TxAGC of the prev frame
12/13/07    gnk     Reset RACH stats counter on overflow after printing out stats
08/28/07    gnk     Added, renamed and modified some RACH statistic variables
08/20/07    kps     Change rach_stats_total_attempts to uint16
06/15/07    oh      Code under MSMHW_ENC_REV_6275 moved out of FEATURE_WCDMA_PWR_TFC_ELMN
                    so that file applies for EDGE-only and normal W builds
05/25/07    kps     Add support to output RACH Transmission Preamble Count.
04/17/07    gnk     Changed enc_rach_no_ack_print_debug() to enc_print_rach_timing_debug()
                    to print timing information for all RACH procedures
03/27/07    oh      Enhanced support to control EUL params(DPCH INIT PWR) via QXDM
03/16/07    am      4312 logpkt changes
03/12/07    asm     Inlined function enc_calc_effective_mpr()
03/01/07    vrao    Added enc_tx_pwr_det_done_isr() to process FW interrupt
                    based on WCDMA Tx AGC
02/21/07    stk     Fixed compiler error, by removing the inline
02/12/07    asm     Defined a function to calculate effective MPR
02/06/07    oh      No need to calculate instantaneous DPCCH pwr logging
12/13/06    asm     Corrected Featurization
11/09/06    asm     Added Support for Rel6 Maximum Power Reduction calculations
11/09/06    yiz     Add max_hs_tx_type_hist[] declaration
11/02/06    am      Added code to get current dpcch pwr in dbm
10/25/06    asm     Featurize out HS-specific code in UL TFC PL Rev2
10/03/06    sk      Added enc_resume_rach()
09/29/06    asm     Added Feature UL TFC Power Limiting Rev2
09/04/06    asr     Changed this header file's distinguishing feature name from
                    ENC_H to WENC_H (to avoid conflict with 1x's file)
08/14/06    yiz     Mainline HSUPA code
06/20/06    bd      Renamed global vars/functions to avoid conflict with 1x names
04/03/06    asm     Fixes trashed UL L2 ACK for DL signaling message that triggers re-config, HHO,
                    or cell transiotion in DCH or FACH state.
09/19/05    asm     Added F3 message to record number of transport blocks from
                    each transport channel. Printed everytime the corresponding DL
                    message is printed.
08/23/05    asm     Modifications for new common UL TFC PL log packet 0x4163
                    Also some optimizations to UL TFC PL implementation
07/18/05    yiz     Fixed ul_tx_rate_adj_in_tab[] dimension for compile
06/15/05    yiz     Add enc_hdet_conv_done_isr for RF_TX_PWR_LIM_REV
05/31/05    am      Merged the latest lint fixes to the mainline
05/12/05    gs      Externalized function enc_refresh_beta_table
                    Externalized variable ul_dpch_state
05/10/05    yiz     Moved some UL TFC PL decl from enc.c for Raven
04/08/05    asm     Merged Raven branch encoder files to mainline
03/31/05    yiz     Add decl of enc_hfn_update_evt_handler.
01/25/05    src     Moved in a bunch of macro-definitions from enc.c.
11/09/04    yiz     Add WCDMA_PRACH_SELECTION support.
10/11/04    gs      Added declaration for HSDPA and 6275 related changes
08/20/04    yiz     Del duplicate declaration of ul_log_coding_info_ptr
07/19/04    yiz     Rename l1_phychan_add_waiting_for_mac to
                    l1_phychan_waiting_for_mac.
                    Delete decl of enc_halt_build_frame_ind()
07/16/04    m       Featurize modifications to stop-build-frame-ind logic
06/16/04    m       New protype for halt build frame indication
04/16/04    yiz     Start or stop UL TFC power restriction for AMR only at recfg.
03/22/04    yiz     Add enc_stop_build_frame_ind_for_recfg().
03/16/04    yiz     Add state type decl for UL TFC power selection and fix.
11/12/03    m       Added support to suppress end of call spike
10/01/03    yiz     Added function enc_is_rach_in_progress() and support code
                    to send local command to L1M to drop RACH after RACH
                    DONE_ISR is called.
08/06/03    yiz     Update comments for enc_kill_pa().
06/02/03    gs      Put variable ul_tfcs_restrict_func_ptr under FEATURE
                    FEATURE_TFCS_CM_BY_HLS
05/27/03    gs      Externalized following global variables
                      l1_tti_enum_type rf_tti_map
                      ul_tfcs_restrict_func_ptr
                      rf_tti_map
03/25/03    yiz     Add decl for enc_rach_no_ack_print_debug().
02/27/03    yiz     Fix FEATURE_WCDMA_PWR_TFC_ELMN after test with MAC.
02/04/03    gs      Added TM ciphering support
10/22/02    yiz     Add data and function decls for FEATURE_WCDMA_PWR_TFC_ELMN
08/28/02    gs      Changed ul_tx_enable() to return FALSE if not in UL_PRACH_TX
                    or UL_DPCH_TX
07/10/02    gs      Externalized CM related variables.
05/16/02    scm     Implement computed gain factors.
04/23/02    yiz     Moved def of default computed GF vals from enc.c to be
                    referred in ulcmd.c for logging.
                    Deleted duplicate prototype of enc_cleanup().
01/18/02    gs      Externalized the function to set the ciphering key
12/04/01    sh      Added enc_pc_enable().
11/15/01    sk      Added flag ul_set_pa_on if PA must be turned ON.
08/28/01    sk      Changed FEATURE_L1_LOGGING to FEATURE_L1_LOGGING_ENABLED.
08/09/01    sk      Added logging code.
07/06/01    sk      Externalized prev_tfci and L UL Tx buffer.
05/17/01    sk      Added enc_kill_pa and aich_tt declarations.
01/16/00    sk      Added RACH DONE isr/
09/08/00    sk      file created.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "queue.h"
#include "l1dlphychancfg.h"
#include "l1macdata.h"
#include "ul.h"
#include "l1task_v.h"
#include "ullog.h"
#include "l1cmmgr.h"
#include "mdspasync.h"
#include "mcalwcdma_evt.h"
#include "l1msm.h"
#include "rlcdsapif.h" 
#if defined FEATURE_QSH_DUMP || defined FEATURE_QSH_EVENT_METRIC
#include "wl1_qsh_ext.h"
#endif /*defined FEATURE_QSH_DUMP || FEATURE_QSH_EVENT_METRIC*/
/*===========================================================================

                                 TYPES

===========================================================================*/

/* dont care values for mDSP DPCCH Config command */
#define ENC_DPCCH_SLOT_FMT_X  0
#define ENC_TFCI_X            0

/* control flag defines for mDSP DPCCH processing */
#define ENC_DPCCH_ENA         1
#define ENC_DPCCH_DIS         0

/* flag to indicate dummy dpcch cfg or real dpcch cfg */
#define ENC_DUMMY_DPCCH_CFG   1
#define ENC_REAL_DPCCH_CFG    0

/* defines for Turbo interleaving parameters */
#define ENC_NUM_ROW_INDEX_PA      0  /* 20 rows */
#define ENC_NUM_ROW_INDEX_PB      1  /* 20 rows */
#define ENC_NUM_ROW_INDEX_PC      2  /* 10 rows */
#define ENC_NUM_ROW_INDEX_PD      3  /* 05 rows */

/* default and max GF values for computed GF */
#define ENC_DFLT_Bc           15
#define ENC_DFLT_Bd           15
#define ENC_MAX_BETA          15

/* RACH statistics logging constants */
/* Preamble count */
#define PREAMBLE_CNT_1          1
#define PREAMBLE_CNT_2          2
#define PREAMBLE_CNT_3          3
#define PREAMBLE_CNT_4          4
#define PREAMBLE_CNT_5          5
#define PREAMBLE_CNT_6          6
#define PREAMBLE_CNT_7          7
#define PREAMBLE_CNT_8          8
#define PREAMBLE_CNT_GRTR       9
#define PREAMBLE_CNT_MAX_BINS   10

/* wcdma query tx power timer timeout period in ms */
#define WCDMA_QUERY_TX_POWER_TIME 670 

#ifdef RF_MIN_TX_POWER_DBM
#define RACH_STATS_INITIAL_TXAGC  RF_MIN_TX_POWER_DBM
#else
#define RACH_STATS_INITIAL_TXAGC -57
#endif

/* This should be 0x80 in normal scenario when using INCRCACC Interface
 * to write to ERAMA */
#define UL_TX_GENERAL_CTL_MASK 0x80

#define WCDMA_L1_UL_HWIO_OUT(A,B) WL1_HWIO_OUT(A,B)
#define WCDMA_L1_UL_HWIO_OUTI(A,B,C) WL1_HWIO_OUTI(A,B,C)
#define WCDMA_L1_UL_HWIO_IN(A) WL1_HWIO_IN(A)
#define WCDMA_L1_UL_HWIO_INM(A,M) WL1_HWIO_INM(A,M)

#define WCDMA_L1_UL_HWIO_ADDR(A) WL1_HWIO_ADDR(A)
#define WCDMA_L1_UL_HWIO_ADDRI(A,B) WL1_HWIO_ADDRI(A,B)

#define wcdma_l1_ul_out_dword(A,B) out_dword(A,B)
#define wcdma_l1_ul_in_dword(A) in_dword(A)

/* For PRACH and DPCH Long scrambling code type, we need to set the 25th bit to 1*/
#define WENC_LONG_SCR_CODE_MASK  0x1000000

/* Debug variable for controlling restoring of
 * ciphering keys at every RF tx enable
 */
extern uint32 wenc_debug_restore_ciph_keys_rf_tx_enable;

#define WENC_DEBUG_RESTORE_CIPH_KEYS_BMSK_HSPRACH_GET_DATA (0x00)
#define WENC_DEBUG_RESTORE_CIPH_KEYS_BMSK_ENABLE_TX_FRM_ISR (0x01)
#define WENC_DEBUG_RESTORE_CIPH_KEYS_BMSK_ENC_GET_DATA (0x02)
#define WENC_DEBUG_RESTORE_CIPH_KEYS_BMSK_HSPRACH_MGR_ADD (0x03)

#ifdef FEATURE_WCDMA_TRIGGER_BASED_RACH
/* Mutex to protect enabling of Tx event from ISR and WL1 task context */
extern rex_crit_sect_type wenc_tx_event_mutex;  

/* Mutex Lock/Unlock to protect rach state transistions */
#define WENC_TX_EVENT_MUTEX_LOCK()   REX_ISR_LOCK(&wenc_tx_event_mutex)
#define WENC_TX_EVENT_MUTEX_UNLOCK() REX_ISR_UNLOCK(&wenc_tx_event_mutex)
#endif /* FEATURE_WCDMA_TRIGGER_BASED_RACH */

typedef enum{
 WENC_RACH_FW_ENABLE_ACTION,      /* Indicate FW to enable rach procedure (0)*/
 WENC_RACH_FW_ABORT_ACTION,       /* Indicate FW to abort any ongoing rach procedure (1)*/
 #ifdef FEATURE_WCDMA_HS_RACH
 WENC_HSRACH_FW_ENABLE_ACTION, /* Indicate FW to enable hsrach procedure (2)*/
 #endif
 WENC_RACH_FW_NUM_ACTION
}wenc_rach_fw_action_enum_type;

/* ENUM to identify current RACH STATE */
typedef enum{
  WENC_RACH_INVALID_STATE,        /* Indicates Prach channel is not up */
  WENC_RACH_READY_STATE,          /* Indicates Prach channel is up and 
                                   * Rach proc can be started */
  WENC_RACH_NOT_READY_STATE,      /* Indicates Rach procedure cannot be started*/
  WENC_RACH_IN_PROGRESS_STATE,           /* Indicates Rach procedure is in progress */
  WENC_RACH_EDCH_TX_IN_PROGRESS_STATE,   /* Indicates HS RACH EDCH Tx in progress */
  WENC_RACH_ABORT_STATE,          /* Indicates current Rach Procedure is aborted */
  WENC_RACH_NUM_STATES
}wenc_rach_state_enum_type;


/* ENUM to identify the action occurred in FACH State */
typedef enum{
  WENC_RACH_ACTION_PRACH_ADD,           /* Triggered at the time of adding PRACH channel */
  WENC_RACH_ACTION_CFG_FW,              /* Triggered at the time of starting Rach procedure */
  WENC_RACH_ACTION_FACH_OOS_MEAS,       /* Triggered at the time of Fach OOS Measurement  */
  WENC_RACH_ACTION_PCCPCH_N_ADD,        /* Triggered when PCCPCH_N channel is to be added */
  WENC_RACH_ACTION_CELL_TRANS,          /* Triggered when Cell Trans Request Received */
  WENC_RACH_ACTION_PRACH_DROP,          /* Triggered when PRACH channel is dropped */
  WENC_RACH_ACTION_PRACH_DONE_ISR,      /* Triggered when Rach Procedure is finished */
  WENC_RACH_ACTION_CLEANUP,             /* Triggered at the time of UL Cleanup procedure */
  WENC_RACH_ACTION_EDCH_RSRC_ALLOC,
  WENC_RACH_ACTION_EDCH_RSRC_DELLOC,
  WENC_RACH_ACTION_CELL_RESEL_IND_HS_RACH,
  WENC_RACH_ACTION_QTA_REQ,             /* Request from FRM to abort ongoing RACH for QTA */
  WENC_RACH_ACTION_COMPLETE_BY_L1,      /* Triggered by above occasions to move rach to ready state */
  WENC_RACH_NUM_ACTIONS
}wenc_rach_action_enum_type;

/* Rach state variable to indicate the current state */
extern wenc_rach_state_enum_type wenc_rach_curr_state;

/* Mutex to protect rach state (wenc_rach_state_enum_type) transistions */
extern rex_crit_sect_type wenc_rach_state_mutex;  

/* Bit mask to identify the actions (wenc_rach_action_enum_type) triggered during rach procedure */
extern uint32 wenc_rach_action_bmask;

extern int16 msg_tx_pwr;

#ifdef FEATURE_QSH_DUMP

/* This macro is used for filling the bit fields in UL Mini  Dump Structures */
#define WL1_UL_MINIDUMP_SET_FIELD(buf, field1, value) \
  (buf) = (((buf) & (~WL1_UL_MINIDUMP_##field1##_BMSK)) | \
           (((value) << WL1_UL_MINIDUMP_##field1##_SHFT) & \
             WL1_UL_MINIDUMP_##field1##_BMSK))

/* For e_ul_debug_db_idx */
#define WL1_UL_MINIDUMP_EUL_CURR_PHYCHAN_CFG_DB_BMSK      0x03
#define WL1_UL_MINIDUMP_EUL_CURR_PHYCHAN_CFG_DB_SHFT      0x00

#define WL1_UL_MINIDUMP_EUL_CURR_DPCH_CFG_DB_BMSK         0x0C
#define WL1_UL_MINIDUMP_EUL_CURR_DPCH_CFG_DB_SHFT         0x02

#define WL1_UL_MINIDUMP_UL_PHYCHAN_DPCH_DB_BMSK           0x30
#define WL1_UL_MINIDUMP_UL_PHYCHAN_DPCH_DB_SHFT           0x04

#define WL1_UL_MINIDUMP_DBG_EUL_TIMING_BMSK               0xC0
#define WL1_UL_MINIDUMP_DBG_EUL_TIMING_SHFT               0x06

/* for enc_rach_dpch_qta_dbg_bmsk */
#define WL1_UL_MINIDUMP_UL_DPCCH_CFG_SEND_BMSK            0x01
#define WL1_UL_MINIDUMP_UL_DPCCH_CFG_SEND_SHFT            0x00

#define WL1_UL_MINIDUMP_RACH_ISR_PENDING_BMSK             0x02
#define WL1_UL_MINIDUMP_RACH_ISR_PENDING_SHFT             0x01

#define WL1_UL_MINIDUMP_ENC_CLEANUP_FLAG_BMSK             0x04
#define WL1_UL_MINIDUMP_ENC_CLEANUP_FLAG_SHFT             0x02

#define WL1_UL_MINIDUMP_RF_TX_EN_DONE_BMSK                0x08
#define WL1_UL_MINIDUMP_RF_TX_EN_DONE_SHFT                0x03

#define WL1_UL_MINIDUMP_RACH_TRIGGER_QTA_BMSK             0x10     
#define WL1_UL_MINIDUMP_RACH_TRIGGER_QTA_SHFT             0x04     

#define WL1_UL_MINIDUMP_UL_SET_PA_ON_BMSK                 0x20
#define WL1_UL_MINIDUMP_UL_SET_PA_ON_SHFT                 0x05

#define WL1_UL_MINIDUMP_UL_PA_ON_IN_PCP_BMSK              0x40
#define WL1_UL_MINIDUMP_UL_PA_ON_IN_PCP_SHFT              0x06

#define WL1_UL_MINIDUMP_UL_EUL_QTA_ACTIVE_BMSK            0x80
#define WL1_UL_MINIDUMP_UL_EUL_QTA_ACTIVE_SHFT            0x07

/* for ul_r99_states */
#define WL1_UL_MINIDUMP_UL_STATE_BMSK                     0x07
#define WL1_UL_MINIDUMP_UL_STATE_SHFT                     0x00

#define WL1_UL_MINIDUMP_UL_DPCH_STATE_BMSK                0x18
#define WL1_UL_MINIDUMP_UL_DPCH_STATE_SHFT                0x03

#define WL1_UL_MINIDUMP_RACH_CURR_STATE_BMSK              0xE0
#define WL1_UL_MINIDUMP_RACH_CURR_STATE_SHFT              0x05


/* for wl1_hs_rach_state_sub_state */
#define WL1_UL_MINIDUMP_HS_RACH_STATE_BMSK                0x07
#define WL1_UL_MINIDUMP_HS_RACH_STATE_SHFT                0x00

#define WL1_UL_MINIDUMP_HS_RACH_SUB_STATE_BMSK            0x78
#define WL1_UL_MINIDUMP_HS_RACH_SUB_STATE_SHFT            0x03


/*for wenc_rf_tx_therm_algo_tfc_pwr_elmn_qta_state */
#define WL1_UL_MINIDUMP_UL_TFCPWR_ELMN_STATE_BMSK         0x03
#define WL1_UL_MINIDUMP_UL_TFCPWR_ELMN_STATE_SHFT         0x00

#define WL1_UL_MINIDUMP_THERM_BACKOFF_CURR_STATE_BMSK     0x0C
#define WL1_UL_MINIDUMP_THERM_BACKOFF_CURR_STATE_SHFT     0x02

#define WL1_UL_MINIDUMP_RF_TX_STATE_BMSK                  0x10
#define WL1_UL_MINIDUMP_RF_TX_STATE_SHFT                  0x04

#define WL1_UL_MINIDUMP_QTA_STATE_BMSK                    0x60
#define WL1_UL_MINIDUMP_QTA_STATE_SHFT                    0x05


/* for e_ul_state_edpch_cpc_dtx_state */
#define WL1_UL_MINIDUMP_E_UL_STATE_BMSK                   0x03
#define WL1_UL_MINIDUMP_E_UL_STATE_SHFT                   0x00

#define WL1_UL_MINIDUMP_E_UL_EDPCH_STATE_BMSK             0x1C
#define WL1_UL_MINIDUMP_E_UL_EDPCH_STATE_SHFT             0x02

#define WL1_UL_MINIDUMP_E_UL_CPC_DTX_STATE_BMSK           0xE0
#define WL1_UL_MINIDUMP_E_UL_CPC_DTX_STATE_SHFT           0x05

/* for wl1_debug_ul_edldrx_ctrl_struct_type ->   cpc_drx_active_grant_monitor_en_entity_ids */
#define WL1_UL_MINIDUMP_CPC_DRX_STATUS_ACTIVE_BMSK        0x01
#define WL1_UL_MINIDUMP_CPC_DRX_STATUS_ACTIVE_SHFT        0x00

#define WL1_UL_MINIDUMP_UE_DRX_GRANT_MONITOR_EN_BMSK      0x02
#define WL1_UL_MINIDUMP_UE_DRX_GRANT_MONITOR_EN_SHFT      0x01

#define WL1_UL_MINIDUMP_ENTITY_ID_AG_S_RG_BMSK            0x0C
#define WL1_UL_MINIDUMP_ENTITY_ID_AG_S_RG_SHFT            0x02

#define WL1_UL_MINIDUMP_ENTITY_ID_HICH_BMSK               0x30
#define WL1_UL_MINIDUMP_ENTITY_ID_HICH_SHFT               0x04

#define WL1_UL_MINIDUMP_ENTITY_ID_NS_RG_BMSK              0xC0
#define WL1_UL_MINIDUMP_ENTITY_ID_NS_RG_SHFT              0x06

/* for wl1_debug_ul_edldrx_ctrl_struct_type ->   is_sched_tx_hich_rslt_fn */
#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_0_BMSK     0x01
#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_0_SHFT     0x00

#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_1_BMSK     0x02
#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_1_SHFT     0x01

#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_2_BMSK     0x04
#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_2_SHFT     0x02

#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_3_BMSK     0x08
#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_3_SHFT     0x03

#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_4_BMSK     0x10     
#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_4_SHFT     0x04     

#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_5_BMSK     0x20
#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_5_SHFT     0x05

#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_6_BMSK     0x40
#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_6_SHFT     0x06

#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_7_BMSK     0x80
#define WL1_UL_MINIDUMP_IS_SCHED_TX_HI_RSLT_FN_7_SHFT     0x07


/* for wl1_debug_ul_edch_cm_info_struct_type -> next_cfn_cm_flags_bmsk */
#define WL1_UL_MINIDUMP_NEXT_TTI_IS_IN_CM_FRAME_BMSK      0x01
#define WL1_UL_MINIDUMP_NEXT_TTI_IS_IN_CM_FRAME_SHFT      0x00

#define WL1_UL_MINIDUMP_NEED_CM_FACTOR_UPDATE_BMSK        0x02
#define WL1_UL_MINIDUMP_NEED_CM_FACTOR_UPDATE_SHFT        0x01

#define WL1_UL_MINIDUMP_NEXT_2ms_TTI_IS_CM_BMSK           0x04
#define WL1_UL_MINIDUMP_NEXT_2ms_TTI_IS_CM_SHFT           0x02

#define WL1_UL_MINIDUMP_FIRST_CM_SLOT_BMSK                0xF8
#define WL1_UL_MINIDUMP_FIRST_CM_SLOT_SHFT                0x03

/* for wl1_debug_ul_edch_cm_info_struct_type -> first_10ms_tx_is_cm*/
#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_0_BMSK        0x01
#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_0_SHFT        0x00

#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_1_BMSK        0x02
#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_1_SHFT        0x01

#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_2_BMSK        0x04
#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_2_SHFT        0x02

#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_3_BMSK        0x08
#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_3_SHFT        0x03

#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_4_BMSK        0x10     
#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_4_SHFT        0x04     

#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_5_BMSK        0x20
#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_5_SHFT        0x05

#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_6_BMSK        0x40
#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_6_SHFT        0x06

#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_7_BMSK        0x80
#define WL1_UL_MINIDUMP_FIRST_10MS_TX_IS_CM_7_SHFT        0x07

/* for wl1_debug_ul_prach_status_struct_type-> last_signature_sub_ch */
#define WL1_UL_MINIDUMP_LAST_SIGNATURE_BMSK               0x0F
#define WL1_UL_MINIDUMP_LAST_SIGNATURE_SHFT               0x00

#define WL1_UL_MINIDUMP_LAST_SUBCHANNEL_BMSK              0xF0
#define WL1_UL_MINIDUMP_LAST_SUBCHANNEL_SHFT              0x04

#define WL1_EUL_DBG_TIMING_BUF_SIZE                       0x03

#endif /* FEATURE_QSH_DUMP */

#ifdef FEATURE_QSH_EVENT_METRIC
typedef struct
{
  /* RACH Params */
  uint8   num_rach_pmbl_attempted;
  uint8   num_rach_pmbl_nak;
  uint8   num_rach_pmbl_no_ack;
  uint8   num_rach_metric_samples;
  /* holds the accumuated RACH final Power in dBm, used for Average RACH TX Power Metric */
  int16   sum_last_pmbl_pwr;
  boolean enable_rach_metrics;
  
  /* RACH procedure attempts params */
  uint8   num_rach_proc_cfgd_to_wfw;
  boolean enable_rach_attempts_metrics;
  
  /* TX Pwr Metric Params */
  
  /* holds the accumuated TX AGC in dBm, used for Average TX AGC Metric */
  int16   sum_tx_power;
  int16   sum_ul_dpcch_pwr;
  uint8   num_tx_pwr_samples_collected; 
  boolean enable_ul_tx_pwr_metrics;
  
  /* EUL Stat Metric Params */
  int16   sum_uph_val;
  int16   sum_sg_index;
  uint8   num_eul_stat_samples;
  uint8   num_hich_naks;
  uint8   num_eul_metric_samples;
  boolean enable_eul_stat_metrics;
  
}wl1_ul_metric_internal_db_type;

extern wl1_ul_metric_internal_db_type wl1_ul_metric_internal_db;
#endif /* FEATURE_QSH_EVENT_METRIC */

#ifdef FEATURE_WCDMA_SAR_TX_PWR_FILTER
/* Declaring Globals and functions for slotwise TxAGC history */
/* Buffer to update TxAGC history */
extern int16 wenc_tx_pwr_hist_agc[ SLOTS_PER_FRAME ];
#endif

/* Macros to set/clear/get actions (wenc_rach_action_enum_type) pending in FACH state */
#define WENC_RACH_ACTION_SET_BMASK(x)      (wenc_rach_action_bmask = (wenc_rach_action_bmask |(1<<x)))
#define WENC_RACH_ACTION_GET_BMASK()       (wenc_rach_action_bmask)
#define WENC_RACH_ACTION_INCL(x)           ((wenc_rach_action_bmask & (1<<x)) >> x)
#define WENC_RACH_ACTION_INCL_ONLY(x)      ((1<<x) == wenc_rach_action_bmask)
#define WENC_RACH_ACTION_CLEAR_BMASK(x)    (wenc_rach_action_bmask = (wenc_rach_action_bmask & (~(1<<x))))
#define WENC_RACH_ACTION_CLEAR_ALL_BMASK() (wenc_rach_action_bmask = 0)

/* Macros to get/set current rach state */
#define WENC_RACH_GET_CURR_STATE()        (wenc_rach_curr_state)
#define WENC_RACH_SET_CURR_STATE(state)   (wenc_rach_curr_state = state)

/* Mutex Lock/Unlock to protect rach state transistions */
#define WENC_RACH_STATE_LOCK()   REX_ISR_LOCK(&wenc_rach_state_mutex)
#define WENC_RACH_STATE_UNLOCK() REX_ISR_UNLOCK(&wenc_rach_state_mutex)

/* MACROS to check if any or all TX Events enabled */
#define WENC_IS_TX_EVENTS_ANY_ENABLED() ((stmr_get_event_mask(MCALWCDMA_EVT_TX_TL_INTR) & TX_EVENTS_MASK) || \
          (stmr_get_event_mask(MCALWCDMA_EVT_RX_TL_INTR) & (MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_SET_MOD_TIMING_EVENT))))

#define WENC_IS_TX_EVENTS_ALL_ENABLED() ((stmr_get_event_mask(MCALWCDMA_EVT_TX_TL_INTR) & TX_EVENTS_MASK) && \
          (stmr_get_event_mask(MCALWCDMA_EVT_RX_TL_INTR) & (MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_SET_MOD_TIMING_EVENT))))

 /* RACH statistics logging variables */
 extern uint32 rach_ack_prmbl_cnt[PREAMBLE_CNT_MAX_BINS];
 extern uint32 rach_ack_cnt;
 extern uint32 rach_noack_cnt;
 extern uint32 rach_nack_cnt;
 extern uint8  rach_nak_prmbl_max_cnt;
 extern uint8  rach_max_prmbl_cnt;
 extern uint32 rach_total_attempts;
 extern int16  rach_noack_prmbl_max_pwr_dB;
 extern int16  rach_detected_last_prmbl_avg_pwr_dB;
 extern int16  rach_noack_last_prmbl_avg_pwr_dB;
 extern int32  rach_detected_last_prmbl_tot_pwr_dB;
 extern int32  rach_noack_last_prmbl_tot_pwr_dB;
 extern int16  rach_prmbl_detected_max_pwr_dB;

/* Flag to set through QXDM for crashing UE when a certian no of RACH No-ACKs are seen */
extern boolean enc_enable_fw_sw_crash;
extern uint8 debug_no_ack_ctr_max;

/* flag to indicate if PA must be turned ON, really if TxAgcAccum must also unfreeze */
extern boolean ul_set_pa_on;

/* CFN passed to MAC for build frame indication */
extern uint8 enc_build_fr_ind_cfn;

/* Function pointer to TFCS restriction evaluation function */
extern L1_UL_QUERY_TFCS_RESTRICT_CB_FUNC_TYPE *ul_tfcs_restrict_func_ptr;

/* This is the UL DPCH state */
extern ul_dpch_state_enum_type ul_dpch_state;

/* Previous TFCI - track to issue mDSP DPCCH Config command only when TFCI
 * changes. Temporary until Firmware folks can look into the problem with
 * the mDSP DPCCH Config Command every 10ms frame
 */
extern uint16 prev_tfci;

/* L1/MAC shared Tx buffer */
extern l1_ul_tx_data_type l1_ul_tx_buf;

/* AICH transmit time offset output parameter */
extern l1_dl_aich_tt_enum_type aich_tt;

/*Flag to enable/disable HDET correction */
extern boolean wenc_debug_disable_hdet_correction;

/* externalize rf_tti_map so that it is accessable outside this file */
extern const l1_tti_enum_type rf_tti_map[];

/* Flag to indicate if ENC driver is in a cleanup */
extern boolean enc_cleanup_flg;
extern l1_cm_ul_param_struct_type ul_cm_params;
extern boolean ul_next_frame_is_compressed;

/* Variable to see if RF Temp Comp Part1 had to be delayed */
extern boolean wl1_delay_rf_temp_comp;

#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
/* Indicate whether to call CM API to update the channel timing for getting Blanking pattern */
extern boolean wenc_delay_cxm_ch_timing_update;
#endif

/* Code below moved out of FEATURE_WCDMA_PWR_TFC_ELMN
so that it applies for EDGE-only and normal W builds */

#define UL_NUM_OL_PWR_BETA_PAIRS 30

/* number of unique modifiedRateAdjIn values */
#define NUM_RATEADJ  28

/* HS-DPCCH delta from 25.213 0~8 plus DTX */
#define UL_NUM_DELTA_HS_VALUES        11

/* DTX is 9 so that 0~8 match 25.213 deltas */
#define UL_DELTA_HS_DTX_IDX           (UL_NUM_DELTA_HS_VALUES - 1)

#define UL_NUM_RATEADJ_ELEM           UL_NUM_OL_PWR_BETA_PAIRS

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
#define QTA_LAST_BFI_CFN_DIFF_FOR_ANY_TTI      3
#define QTA_LAST_BFI_CFN_DIFF_FOR_20MS         5
#define QTA_LAST_BFI_CFN_DIFF_FOR_40MS         7
#define QTA_LAST_BFI_CFN_DIFF_FOR_80MS         11
#endif

#ifdef FEATURE_WCDMA_HS_RACH
extern int16 tx_agc_logging;
extern uint32 last_aiSum_logging;
#endif

typedef struct {
  /* elimination history buffer frame by frame */
 uint16 rateAdj_elmn_hist[PWR_TFC_ELMN_Y_FRAME];

 /* sum of elimination history */
 uint16 rateAdj_elmn_sum;

 /* Does this rateAdj have a corresponding TFC? */
 boolean tfc_exists;
 /* Is the modifiedRateAdjIn in Excess Power state? */
 boolean  excess_pwr;
} rateAdj_pwr_elmn_tab_elem_type;

/* table of TFC EXCESS_POWER state flags
** TRUE: Excess Power; FALSE: Supported
**/
extern boolean enc_tfc_excess_pwr_state[L1_UL_TFC_MAX];

/* table of modifiedRateAdjIn values sorted in ascending order */
extern const int16 sortedRateAdjIn[NUM_RATEADJ];
/* power elimination of modifiedRateAdjIn table */

extern rateAdj_pwr_elmn_tab_elem_type rateAdj_pwr_elmn_tab[UL_NUM_RATEADJ_ELEM];

/* elimination history buffer index for next frame */
extern uint16 rateAdj_elmn_hist_frame_idx;

/* tfc_elmn_pwr_buf = maxRateAdjIdx + txRateAdjIn
** circular buffer of three frames:
** process data | transmitting frame | elminate TFC
**/
#define TFC_ELMN_PWR_BUF_SIZE 3

/* index for circular buffer tfc_elmn_pwr_buf[] */
extern uint8 tfc_elmn_pwr_buf_idx;

/* TFCI to sortedRateAdjIn[] index map */
extern uint8  tfci_rateAdj_map[L1_UL_TFC_MAX];

/* TRUE when UL TPC shall be freezed by UL state
 * FALSE when UL TPC may be unfreezed by UL state
 * defaults to TRUE for initial state
 */
extern boolean ul_tpc_freeze_flag;

/* get the sortedRateAdjIn[] index for a betaC and betaD pair */
extern uint8 enc_get_modRateAdj_idx_for_GF(uint8 betaC, uint8 betaD);

extern void ul_init_tfc_elmn( void );

extern int16  dpch_init_tx_pwr_agc;

#ifdef FEATURE_WCDMA_HS_RACH
extern int16 wenc_debug_hsrach_dpch_init_tx_pwr_dbm;
#endif /* FEATURE_WCDMA_HS_RACH */

/* state of UL TFC power restriction */
typedef enum
{
  UL_TFC_PWR_ELMN_STATE_INIT,
  UL_TFC_PWR_ELMN_STATE_PREP, /* 2 frames before TFC elmination start */
  UL_TFC_PWR_ELMN_STATE_GO,
  UL_TFC_PWR_ELMN_STATE_PAUSE /* pause tfc pl for reconfig */
} ul_tfc_pwr_elmn_state_enum_type;

extern ul_tfc_pwr_elmn_state_enum_type ul_tfc_pwr_elmn_state;

/* TRUE when UL TPC shall be freezed by low SIR
 * FALSE when UL TPC may be unfreezed by low SIR
 */
extern boolean low_sir_freeze_flag[WL1_MAX_UL_CARR_IDX];

extern boolean l1_phychan_waiting_for_mac;

/* CFN to suspend build_frame_ind() two frames before act time */
extern uint32  enc_cfn_to_suspend_bfi;

#define ENC_CFN_TO_SUSPEND_BFI_INVALID  ((uint32)-1)

#define UL_NUM_OL_PWR_PARMS      4

#define UL_MOD_BC_IDX 0
#define UL_MOD_BD_IDX 1
#define UL_RACH_MSGDELTA_PWR_IDX 2
#define UL_RATE_ADJ_IN_IDX 3

extern const int16 ul_tx_rate_adj_in_tab[UL_NUM_OL_PWR_BETA_PAIRS + 1][UL_NUM_OL_PWR_PARMS];

/* Macro to obtain modified Bc */
#define UL_MOD_Bc(Bc, Bd) ((Bc == ENC_MAX_BETA) ? \
  ul_tx_rate_adj_in_tab[Bd][UL_MOD_BC_IDX] : \
  ul_tx_rate_adj_in_tab[ENC_MAX_BETA+Bc][UL_MOD_BC_IDX])

/* Macro to obtain modified Bd */
#define UL_MOD_Bd(Bc, Bd) ((Bc == ENC_MAX_BETA) ? \
  ul_tx_rate_adj_in_tab[Bd][UL_MOD_BD_IDX] : \
  ul_tx_rate_adj_in_tab[ENC_MAX_BETA+Bc][UL_MOD_BD_IDX])

/* Macro to obtain Modified RateAdjIn */
#define UL_MOD_RATE_ADJ_IN(Bc, Bd) ((Bc == ENC_MAX_BETA) ? \
  ul_tx_rate_adj_in_tab[Bd][UL_RATE_ADJ_IN_IDX] : \
  ul_tx_rate_adj_in_tab[ENC_MAX_BETA+Bc][UL_RATE_ADJ_IN_IDX])

/* Macro to obtain RACH beta radio. It is same as Rate Adj */
#define RACH_PWR_BETA_RATIO(Bc, Bd) UL_MOD_RATE_ADJ_IN(Bc, Bd)

/* Macro to obtain RACH message delta power */
#define RACH_MSG_DELTA_PWR(Bc, Bd) ((Bc == ENC_MAX_BETA) ? \
  ul_tx_rate_adj_in_tab[Bd][UL_RACH_MSGDELTA_PWR_IDX] : \
  ul_tx_rate_adj_in_tab[ENC_MAX_BETA+Bc][UL_RACH_MSGDELTA_PWR_IDX])



/* Macro to obtain Modified RateAdjIn */
/* The table is sorted by DPCH power gains and the index happens
 * to be deterministic by below formula */
#define ENC_GET_BETA_PAIR_INDEX(Bc, Bd) ((Bc == 15) ? \
  Bd : UL_NUM_OL_PWR_BETA_PAIRS - Bc)

/* TX power parameters for DPCH and HS-DPCCH */
extern mdspasync_ul_tx_pwr_gain_factors_struct_type enc_pwr_params[];
/*variable to indicate prev frame tx pwr params*/
extern uint8 enc_cur_pwr_buf_idx;
/*variable to indicate current tx pwr params in use*/
extern uint8 enc_prev_pwr_buf_idx;

extern WfwTxDpcchAsyncWriteStruct   *wfw_intf_dpcch_async_write_ptr[WL1_MAX_UL_CARR_IDX];
/* DPCH OLPC parameter */
extern int16  dpch_init_tx_pwr_dbm;
extern int16  dpch_init_tx_pwr_agc_for_revert;
extern boolean rfagc_pwr_ctrl_enable;

/* Is DPCH and HS-DPCCH power aparmeters ready ? */
extern boolean enc_dpch_hs_pwr_param_ready;

/* Flag used to indicate if it is a seamless DCH reconfig or not*/
extern boolean enc_seamless_dch_reconfig_flag;

// Macros for Enhanced MPR packing

#define MDSP_MPR_VAL_PACK_BMSK             0x7
#define MDSP_MPR_VAL_PACK_DTX_SHFT         0x0
#define MDSP_MPR_VAL_PACK_ACK_SHFT         0x3
#define MDSP_MPR_VAL_PACK_NACK_SHFT        0x6
#define MDSP_MPR_VAL_PACK_CQI_SHFT         0x9
/*For MIMO HS Types*/
#define MDSP_MPR_VAL_PACK_ACK_ACK_SHFT     0xC
#define MDSP_MPR_VAL_PACK_NACK_NACK_SHFT   0xF
#define MDSP_MPR_VAL_PACK_DELTA_CQI_SHFT   0x12
#define MDSP_MPR_VAL_PACK_ACK_NACK_SHFT    0x15
#define MDSP_MPR_VAL_PACK_NACK_ACK_SHFT    0x18

/* Macro to pack MPR value into packed array */
#define MDSP_MPR_VAL_PACK( x, mpr_array, rhs_type )  \
  (mpr_array |= ( x << MDSP_MPR_VAL_PACK_##rhs_type##_SHFT ))

/* Macro to unpack MPR value from packed array */
#define MDSP_MPR_VAL_UNPACK( x, rhs_type )  \
  (( x >> MDSP_MPR_VAL_PACK_##rhs_type##_SHFT ) & MDSP_MPR_VAL_PACK_BMSK)

/* Rhs types. These have power delta passed OTA from N/W side */
typedef enum
{
  ENCHS_RHS_TYPE_DTX,
  ENCHS_RHS_TYPE_ACK,
  ENCHS_RHS_TYPE_NACK,
  ENCHS_RHS_TYPE_CQI,
  ENCHS_RHS_TYPE_ACK_ACK,
  ENCHS_RHS_TYPE_ACK_NACK,
  ENCHS_RHS_TYPE_NACK_ACK,
  ENCHS_RHS_TYPE_NACK_NACK,
  ENCHS_RHS_TYPE_DELTA_CQI,
  ENCHS_NUM_RHS_TYPE
} enchs_rhs_type_enum_type;

/* Data-Structure to store Rel 6 Enhanced MPR parameters */
typedef struct {
    uint16  rho_d_x15;
    uint16  rho_hs_x15[ ENCHS_NUM_RHS_TYPE ];
    uint16  rho_ec_x15;
    uint16  rho_ed_x15;
    uint8   mu;
    uint8   dpdch_en;
    uint8   hs_dpcch_en;
} ul_mpr_calc_parms;

extern enchs_rhs_type_enum_type max_hs_tx_type_hist[SLOTS_PER_FRAME];


extern int16 min_tx_power_dbm;

typedef enum
{
  WENC_DEBUG_TX_PWR_DISABLE,
  WENC_DEBUG_DPCCH_PWR,
  WENC_DEBUG_RACH_PWR,
  WENC_DEBUG_BOTH_RACH_DPCCH_PWR
}debug_pwr_values_enum_type;

/* Freeze the Tx accumulation to disable the TPC */
extern uint8 wenc_debug_freeze_tx_accum;

/* Variable used to set the init dpcch pwr and the Rach pwr */
extern debug_pwr_values_enum_type wenc_debug_tx_pwr_type;

extern int16 debug_dpcch_tx_pwr_value;
extern int16 debug_rach_tx_pwr_value;
extern int16 debug_rach_dpcch_combined_pwr_val;

// Variables to delay HS-DPCCH enabling
extern uint32 dpcch_est_counter;
extern uint32 debug_dpcch_est_counter;

/* Timer for wcdma query tx power dbm
 * which fires every 670ms
 */
extern rex_timer_type          wcdma_query_tx_power_timer;

/* Possible states for PA Sensor device monitored
 * through NPA */
typedef enum
{
  WENC_THERMAL_NPA_STATE_OKAY,
  WENC_THERMAL_NPA_STATE_WARNING,
  WENC_THERMAL_NPA_STATE_WARNING_EX,
  WENC_THERMAL_NPA_STATE_CRITICAL
}wenc_thermal_npa_state_enum;

#ifdef FEATURE_WCDMA_MTPL_THERMAL_BACKOFF
/* Possible states in MTPL Thermal Backoff Algorithm */
typedef enum
{
  WENC_THERMAL_BACKOFF_START, /* PA Sensor Sate is WARNING_EX and UE in DCH */
  WENC_THERMAL_BACKOFF_PAUSE, /* PA Sensor state is WARNING_EX and UE not in DCH */
  WENC_THERMAL_BACKOFF_STOP,  /* PA Sensor state is not WARNING_EX */
  WENC_THERMAL_BACKOFF_NUM_STATES 
}mtpl_backoff_thermal_algo_state;


/* Initial MTPL backoff value in dB */ 
extern uint8 wenc_thermal_backoff_mtpl_init_val_db;     

/* Variable to indicate whether PA Sensor State is WARNING_EX or NOT. The variable
 * is set to TRUE/FALSE when moving into/out of WARNING_EX state */
extern boolean pa_sensor_state_warning_ex;

/* Mutex to protect state tranisitions of MTPL Backoff Algorithm 
 * This mutex also ensures the TIMER Task is not run in parallel with state transitions */
extern rex_crit_sect_type wenc_thermal_backoff_state_mutex;  

/* MTPL BACKOFF Duty Cyle and Step Timer Periods */
extern uint32                  wenc_thermal_backoff_dutycycle_tup_period_ms;
extern uint32                  wenc_thermal_backoff_dutycycle_tdown_period_ms;
extern uint32                  wenc_thermal_backoff_step_timer_period_ms;
extern rex_timer_type          wenc_thermal_backoff_dutycycle_timer;
extern rex_timer_type          wenc_thermal_backoff_step_timer;

/* Boolean flag to determine whether currently active call is E911 call or not
 * Flag should be set based on the value obtained from RRC during CPHY_SETUP
 */
extern boolean is_e911_call_active;
#endif /* FEATURE_WCDMA_MTPL_THERMAL_BACKOFF */

/* Boolean flag to determine if the currently active call is a voice only call 
 * Voice Only call = TRUE 
 * Voice + Data = FALSE 
 * Data only call = FALSE 
 * IDLE = FALSE */
extern boolean is_voice_only_call;

#define WENC_TX_ERAMA_INIT_VALUE 0x1

/* 1 - Big endian. Applies to both A2 and FW AHB interfaces 
  * for A2, this value has to be zero so that data is automatically
  * converted to BIG-ENDIAN */
#define WENC_TX_MEM_ACCESS_CFG_VALUE 0x0

extern WfwTxAsyncReadStruct         *wfw_intf_tx_async_read_ptr;
extern WfwTxAsyncWriteStruct        *wfw_intf_tx_async_write_ptr;
extern WfwTxAgcAsyncReadStruct      *wfw_intf_txagc_async_read_ptr;
extern WfwTxAgcAsyncWriteStruct     *wfw_intf_txagc_async_write_ptr;
extern WfwTxDpcchAsyncReadStruct    *wfw_intf_dpcch_async_read_ptr[WL1_MAX_UL_CARR_IDX];
extern WfwTxDpcchAsyncWriteStruct   *wfw_intf_dpcch_async_write_ptr[WL1_MAX_UL_CARR_IDX];
extern WfwTxRachAsyncReadStruct     *wfw_intf_rach_async_read_ptr;
extern WfwTxRachAsyncWriteStruct    *wfw_intf_rach_async_write_ptr;
extern WfwTxHsDpcchAsyncWriteStruct *wfw_intf_tx_hsdpcch_async_write_ptr;
extern WfwTxHsDpcchAsyncReadStruct  *wfw_intf_tx_hsdpcch_async_read_ptr;
extern WfwTxRachLogStruct           *wfw_tx_rach_res_ptr;

#ifdef FEATURE_WCDMA_MODEM_TPUT_STATS_API 
/* Filter co-eff for TX Power */
#define WCDMA_TX_POWER_COEFF           50

/* Threshold for W working at LOW Power */
#define WCDMA_TX_POWER_LOW_DBM          0

/* Threshold for W working at HIGH Power */
#define WCDMA_TX_POWER_HIGH_DBM        10

/* RACH message power offset */
#define WCDMA_RACH_POWER_OFFSET           3

/* Logging Bitmaks for Cost of modem power */
#define WCDMA_STATS_COST_OF_TX_POWER_LOGGING_BMSK 0x2

typedef struct
{
  /* Modem power enumeration*/
  wl1_ul_modem_tx_power_enum_type tx_modem_power;

  /* Maintains filtered value of UL TxAgc(dbm)  */
  int16 modem_power_tx_agc;

  /* Indicates if this is the first sample of UL TxAGC */
  boolean first_sample_for_tx_power;

  /* QXDM Control: Indicates if debugging tx_power coeff is ON */
  boolean is_dbg_tx_power_coeff;

  /* Filter co-eff used for UL Txpower(dbm) */
  uint16 tx_power_filter_coeff;
  
} wl1_ul_modem_power_struct_type;

/* Global to enable stats related debugging */
extern uint32 WCDMA_MODEM_STATS_LOGGING;

/* Global that hold internal TX Power stats */
extern wl1_ul_modem_power_struct_type wl1_modem_tx_power;
#endif /* FEATURE_WCDMA_MODEM_TPUT_STATS_API  */

/* bit_position = number -1. Example Master control at 0th bit postion */
#define WL1_NV_RACH_MASTER_CONTROL       0x1
#define WL1_NV_MAX_PREAMBLE_CONTROL      0x2
#define WL1_NV_MAX_NOACK_CONTROL         0x4

#define R99_UL_DPCCH_DROP_INTR_TIMEOUT_MSEC 20

/*==========================================================================

                         CONCURRENCY CONTROL PRIMITIVES

===========================================================================*/
/* This flag indicates the RF Tx has been enabled for
** RACH procedure. This flag was introduced to ensure that
** this is checked and if it is TRUE when a PCCPCH_N
** is being setup for interF SIB read, RF Tx should be
** disabled on the current frequency before retuning RF
** as part of channel add*/
extern boolean wenc_rf_tx_enable_done_for_rach;
// Externalize Mutexes
extern rex_crit_sect_type eul_and_r99_ul_mutex;

// Macros to lock/unlock mutexes
#define EUL_AND_R99_UL_ATOMIC_MUTEX_LOCK()       rex_enter_crit_sect(&eul_and_r99_ul_mutex); REX_DISABLE_PREMPTION();
#define EUL_AND_R99_UL_ATOMIC_MUTEX_UNLOCK()     rex_leave_crit_sect(&eul_and_r99_ul_mutex); REX_ENABLE_PREMPTION(); 

#define EUL_AND_R99_UL_ISR_LOCK()                REX_ISR_LOCK(&eul_and_r99_ul_mutex)
#define EUL_AND_R99_UL_ISR_UNLOCK()              REX_ISR_UNLOCK(&eul_and_r99_ul_mutex)

#define UL_ISR_AND_DCH_RECONFIG_LOCK()           REX_ISR_LOCK(&l1_ul_dch_reconfig_isr_mutex)
#define UL_ISR_AND_DCH_RECONFIG_UNLOCK()         REX_ISR_UNLOCK(&l1_ul_dch_reconfig_isr_mutex)

/* Flag to indicate that the UE is out of sync.
 * When in sync, UL TX shall be delayed (PA off) until phychan establishment
 * indication and enc_ul_out_sync becomes FALSE.
 */
extern boolean enc_ul_out_sync;

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
/* Mutex to perform atomic state transistions */
extern rex_crit_sect_type wl1_tfc_excess_pwr_states_mutex;

/* Mutex to perform atomic state transistions */
extern rex_crit_sect_type wl1_rach_sub_chan_avail_mutex;

#define WL1_TFC_EXCESS_PWR_INTLOCK()  REX_ISR_LOCK(&wl1_tfc_excess_pwr_states_mutex)
#define WL1_TFC_EXCESS_PWR_INTFREE()  REX_ISR_UNLOCK(&wl1_tfc_excess_pwr_states_mutex)

#define WL1_RACH_SUB_CHAN_AVAIL_INTLOCK() REX_ISR_LOCK(&wl1_rach_sub_chan_avail_mutex)
#define WL1_RACH_SUB_CHAN_AVAIL_INTFREE() REX_ISR_UNLOCK(&wl1_rach_sub_chan_avail_mutex)

#endif /*FEATURE_WCDMA_FREE_FLOATING_TASK*/


/*===========================================================================

                                 FUNCTIONS

===========================================================================*/

#ifdef FEATURE_WCDMA_16_QAM
extern uint32 eul_16_qam_get_rand_num(void);
#endif /* FEATURE_WCDMA_16_QAM */

#ifdef FEATURE_WCDMA_HS_RACH

void wenc_hsprach_get_data(void);

void wenc_hsprach_cleanup(void);

void wenc_hsdpch_get_data(void);

void wenc_hsdpch_cleanup(void);

void wenc_hsdpch_set_mod_timing_evt_handler(void);


void wenc_hsprach_set_mod_timing_evt_handler(void);

/*=========================================================================
FUNCTION wenc_prach_process_logging

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/
void wenc_prach_process_logging(int16 txAgc,uint32 ul_last_aisum_val);

/*=========================================================================
FUNCTION wenc_prach_process_tx

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
=========================================================================*/
void wenc_hsprach_process_tx_data(void);
/*=========================================================================
FUNCTION wenc_phychan_set_mod_timing

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
=========================================================================*/
void wenc_phychan_set_mod_timing(void);
/*=========================================================================
FUNCTION ENC_PRACH_GET_DATA

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
=========================================================================*/
void wenc_phychan_get_data(void);

/*=========================================================================
FUNCTION ENC_HSRACH_DONE_ISR

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
=========================================================================*/

void enc_hsrach_done_isr(void);
#endif /*FEATURE_WCDMA_HS_RACH*/

/*====================================================================
FUNCTION: ENC_ACCU_UL_DPCH_PWR_INFO_SAMPLE

DESCRIPTION:
  This function accumulates one UL_DPCH_PWR_INFO_LOG_PKT sample

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  NONE
====================================================================*/
void enc_accu_ul_dpch_pwr_info_sample
(
  uint8   tfci,
  boolean cm_frame_ind,
  uint8   beta_c,
  uint8   beta_d,
  uint8   cm_tx_pwr_delta
);
/*=========================================================================
FUNCTION UL_INDICATE_AMR_ENABLED

DESCRIPTION
  This function is called from l1m_cphy_setup_req_cmd() whenever AMR is 
  added to the configuration

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void ul_indicate_AMR_enabled( void );

/*=========================================================================
FUNCTION UL_INDICATE_AMR_DISABLED

DESCRIPTION
  This function is called from l1m_cphy_setup_req_cmd() whenever AMR is 
  removed from the configuration

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void ul_indicate_AMR_disabled( void );


/*=========================================================================
FUNCTION WENC_THERMAL_EVT_REG

DESCRIPTION This routine is called during WL1 Init to register with NPA for events
            on PA Sensor Device

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void wenc_thermal_npa_evt_reg(void);

/*=========================================================================
FUNCTION WENC_THERMAL_EVTINIT_CB

DESCRIPTION This routine is called after registering for events on 
            PA sensor device. This routine queries current state of PA. 
            Based on the state (OKAY, WARNING, CRITICAL) appropriate action 
            is taken (enable/disable thermal mitigation)

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void wenc_thermal_npa_evtinit_cb(void *context, 
                                 unsigned int event_type, 
                                 void *data, unsigned int data_size);


/*=========================================================================
FUNCTION WENC_THERMAL_EVTCHANGE_CB

DESCRIPTION This routine is called by NPA whenever there is a change in PA 
            Sensor State. 

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void wenc_thermal_npa_evtchange_cb(void *context, 
                                   unsigned int event_type, 
                                   void *data, unsigned int data_size);

/*=========================================================================
FUNCTION WENC_THERMAL_NPA_EVTHANDLER

DESCRIPTION This routine enables/disables thermal mitigation based on
            PA sensor state. The routine is called at the time of registration
            and everytime there is a change in event.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void wenc_thermal_npa_evthandler(wenc_thermal_npa_state_enum npa_curr_state);

#ifdef FEATURE_WCDMA_MTPL_THERMAL_BACKOFF
/*=========================================================================
FUNCTION WENC_THERMAL_BACKOFF_STATE_TRANS

DESCRIPTION
  This routine takes care of transistion between different state of MTPL Backoff algorithm
  The state transistion is protected by a mutex.
  The input to the function is the next state of the algorithm

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void wenc_thermal_backoff_state_trans(
    mtpl_backoff_thermal_algo_state wenc_thermal_backoff_next_state
 );

/*=========================================================================
FUNCTION WENC_THERMAL_BACKOFF_GET_CURR_STATE

DESCRIPTION
  This routine returns the current state of MTPL Backoff Algorithm

DEPENDENCIES

RETURN VALUE
  Current State of MTPL BACKOFF Algorithm (START, PAUSE, STOP)

SIDE EFFECTS
  None.
=========================================================================*/
mtpl_backoff_thermal_algo_state wenc_thermal_backoff_get_curr_state(void);

/*=========================================================================
FUNCTION WENC_MTPL_BACKOFF_CLR_TIMERS

DESCRIPTION
  This routine clears duty cycle and step timers used for MTPL Backoff

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void wenc_thermal_backoff_clr_timers(void);

/*=========================================================================
FUNCTION WENC_MTPL_BACKOFF_DUTYCYCLE_SIG_HANDLER

DESCRIPTION
  This routine handles the duty cycle timer. Based on the Up/Down switch
  appropriate MTPL value is set and timers are restarted.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void wenc_thermal_backoff_dutycycle_sig_handler(uint32 unused);

/*=========================================================================

FUNCTION WENC_MTPL_BACKOFF_STEP_SIG_HANDLER

DESCRIPTION
  This routine handles the step timer. Based on thetimer count,
  new backoff value is calculated by which to reduce MTPL

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void wenc_thermal_backoff_step_sig_handler(uint32 unused);

/*===========================================================================
FUNCTION        WENC_THERMAL_BACKOFF_FOR_VOICE_E911_CALL

DESCRIPTION     This function checks the current state of MTPL_BACKOFF state machine and 
                also checks whether any voice call or Emergency call is active and alters
                the state of MTPL_BACKOFF if required.
                If MTPL_BACKOFF is STARTED and if its a voice only call or E911 call , then PAUSE
                if MTPL_BACKOFF already Paused and if it is not a voice only call then START
 
DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void wenc_thermal_backoff_for_voice_e911_call(void);

#endif /* FEATURE_WCDMA_MTPL_THERMAL_BACKOFF */

#ifdef FEATURE_WCDMA_L1_ZI_OPT
/* ===========================================================================

FUNCTION        UL_DATA_DUMP_LOG_PKT_ALLOC

DESCRIPTION     This function allocates the dynamic memory needed for the
                log packets and related logging structures

DEPENDENCIES    NONE

RETURN VALUE    None

SIDE EFFECTS    None

=========================================================================== */
void ul_data_dump_log_pkt_alloc(void);

/*===========================================================================

FUNCTION        UL_DATA_DUMP_LOG_PKT_DEALLOC

DESCRIPTION     This function de-allocates the dynamic memory needed for the
                log packets and related logging structures

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
void ul_data_dump_log_pkt_dealloc(void);
#endif /* FEATURE_WCDMA_L1_ZI_OPT */

/*===========================================================================

FUNCTION        UL_FW_RACH_DEBUG_LOG_CFG_STATUS_PARAMS

DESCRIPTION     This function records fw parameters which are related to rach
                procedure

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
void ul_fw_rach_debug_log_cfg_status_params(void);

/*=========================================================================
FUNCTION    ENC_LOG_RACH_DEBUG_CFG_PARAMS

DESCRIPTION This routine records RACH Config Parameters as part of
            UL_RACH_DEBUG_LOG_PKT 

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/
void enc_log_rach_debug_cfg_params(
  int8   cpich_tx_pwr,                    /* CPICH Tx Pwr */
  int16  tmp_ec,                          /* RSCP Pwr */
  int8   ul_if,                           /* UL Interference */
  int8   const_val,                       /* Constant Value  */
  int32  prach_init_tx_pwr,               /* Init Preamble Pwr*/
  uint8  max_num_rach_preambles,          /* Max Num of Rach Preambles */
  l1_dl_aich_tt_enum_type local_aich_tt   /* AICH timing */
  #ifdef FEATURE_WCDMA_HS_RACH
  , int8   poweroffset_pp_m                 /* preamble to msg pwr offset */
  #endif
);

/*=========================================================================
FUNCTION    ENC_LOG_RACH_DEBUG_STATUS_PARAMS

DESCRIPTION This routine records RACH Status Parameters as part of
            UL_RACH_DEBUG_LOG_PKT. It is called from enc_rach_done_isr()

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/
void enc_log_rach_debug_status_params(void);

/*=========================================================================

FUNCTION WL1_ENC_INIT

DESCRIPTION
  This routine intializes the encoder driver. It initializes the TrCH mux
  order free queue and the TrCH mux queue.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/
void wl1_enc_init
(
  void
);

/*===========================================================================
FUNCTION     wenc_get_timing_info

DESCRIPTION  This function will return the current Tx bpg .

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS  
===========================================================================*/
uint8 wenc_get_timing_info(void);

/*=========================================================================

FUNCTION ENC_GET_DATA

DESCRIPTION
  This function processes tx data delivered by MAC. It sets up
  appropriate encoder control registers and transfers tx data into ERAMa.
  It sets up TRCH_CFG_0_RAM, TRCH_CFG_1_RAM and TRCH_CTL_RAM as well as the
  number of active/erama transport channels. It calculates RM parameters, SF
  and Gain Factors. It has be done with all its processing within 6ms into the
  frame leaving the encoder at least 4 ms to cipher/encode ERAMa data.

DEPENDENCIES
  This function depends on MAC to have successfully built tx data into the
  Tx buffer before the GET_DATA event.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/
void enc_get_data
(
  void
);



/*=========================================================================
FUNCTION wl1_ul_rach_read_nv_item

DESCRIPTION This function read NV item to enable/disable NV controlled RACH debugging parameters

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/

extern void wl1_ul_rach_read_nv_item(void);

/*=========================================================================

FUNCTION ENC_PC_ENABLE

DESCRIPTION
  In the first slot after OL power is set, re-enable closed loop power
  control and unfreeze the TxAgcAccum.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/
extern void enc_pc_enable
(
  void
);

/*=========================================================================

FUNCTION ENC_RACH_DONE_ISR

DESCRIPTION
  This ISR is invoked by mDSP after a RACH transmit message request has been
  processed. It indicates the status of the transmit (ACK, NACK or a NO-ACK).

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/
void enc_rach_done_isr
(
  void
);



/*=========================================================================

FUNCTION ENC_CLEANUP

DESCRIPTION
  This routine re-initializes the encoder driver. It aborts all Tx
  processing in the encoder. It commands the mDSP to stop control channel
  processing. It disables Tx events on the sequencer Tx timeline.

  It resets ERAMb free address and the number of active TrCHs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/
void enc_cleanup
(
  void
);

/*===========================================================================

FUNCTION WL1_ENC_KILL_PA

DESCRIPTION
  Kill the PA by calling ul_tx_disable()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void wl1_enc_kill_pa( void );

/*===========================================================================
FUNCTION ENC_SET_CIPHERING_KEYS

DESCRIPTION
  This function updates the ciphering key for uplink ciphering operation
  to the ciphering key index locations requested.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
void enc_set_ciphering_key(
  /* ciphering key index (one of the 4 keys) to which the key is set */
  uint8 index,
  /* ciphering key info. Pointer to the array of 4 word32 (= 128 bit) */
  uint8 *ciphering_key_data);

/*===========================================================================
FUNCTION        ENC_CIPHER_TM_TB_KEY_STREAM

DESCRIPTION     This function generates the ciphering keystream for various
                transparant mode TBs. If there is only one TB in that is TM
                and need to be ciphered, it is ciphered in mormal mode using
                hardware so no ciphering keystrean is generated for such TB.

DEPENDENCIES    NONE

RETURN VALUE    NONE

SIDE EFFECTS    It calls function enc_gen_cipher_keystream so same
                side effects as that function.
===========================================================================*/


void enc_cipher_tm_tb_key_stream(
  /* number of TrCh to process */
  uint8 num_trch);

/*===========================================================================

FUNCTION ENC_PRINT_RACH_TIMING_DEBUG

DESCRIPTION
  This function prints debugging information to F3 when RACH no-ack happens.

DEPENDENCIES
  RACH no-ack must have just happened.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_print_rach_timing_debug( void );

/*===========================================================================
FUNCTION WENC_SEND_RACH_ABORT_CMD

DESCRIPTION This routine sends mdsp prach cfg cmd to FW to abort any
            ongoing rach procedure.

DEPENDENCIES
  
RETURN VALUE  

SIDE EFFECTS  It stops further RACH TX.
==========================================================================*/
void wenc_send_rach_abort_cmd(void);

/*===========================================================================
FUNCTION WENC_PROCESS_RACH_ACTION

DESCRIPTION This routine processes actions triggered in FACH state.
            Based on the action, rach state transistion is taken care of.

DEPENDENCIES

RETURN VALUE Boolean value indicating whether the triggered action can proceed further
             or to wait till the current rach procedure is finished/aborted.
             TRUE - Proceed
             FALSE - Block till rach is done

SIDE EFFECTS
==========================================================================*/
boolean wenc_process_rach_action(wenc_rach_action_enum_type rach_curr_action);

/*===========================================================================
FUNCTION UL_UNFREEZE_TPC

DESCRIPTION
  This function unfreezes TPC accum.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Unfreezes TPC accum
==========================================================================*/
void ul_unfreeze_tpc(void);

/*===========================================================================
FUNCTION UL_SEC_UNFREEZE_TPC

DESCRIPTION
  This function freezes/unfreezes TPC accum for secondary carrier.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  freezes/Unfreezes TPC accum for secondary carrier
==========================================================================*/
void ul_sec_unfreeze_tpc(void);

/*===========================================================================
FUNCTION        ENC_STOP_BUILD_FRAME_IND_FOR_RECFG

DESCRIPTION     This function cleans UL TX data buffer and stops
                build_fram_ind()

DEPENDENCIES    Must be called for DCH seamless recfg

RETURN VALUE    None

SIDE EFFECTS    As said above
==========================================================================*/
void enc_stop_build_frame_ind_for_recfg(void);

/*===========================================================================
FUNCTION        ENC_SUSPEND_BFI_TWO_FRAMES_BEFORE_ACT_TIME

DESCRIPTION     This function suspends build_fram_ind() two frames before the
                activation time. It does NOT clean UL TX data buffer.

DEPENDENCIES    Must be called within WCDMA_INTLOCK

RETURN VALUE    None

SIDE EFFECTS    As said above
==========================================================================*/
void enc_suspend_bfi_two_frames_before_act_time(l1_ext_cmd_type* cmd);

/*=========================================================================

FUNCTION ENC_CALC_CPICH_RSCP_FOR_OLPC

DESCRIPTION
  This function calculates CPICH RSCP value used for RACH or DPCH OLPC

DEPENDENCIES
  This function depends on RF for Io and SEARCHER for Ec/Io

RETURN VALUE
  int16, CPICH RSCP in dBm

SIDE EFFECTS
  None.

=========================================================================*/
int16 enc_calc_cpich_rscp_for_olpc(void);

#if (defined(FEATURE_UMTS_CIPHERING_ENABLE) && defined(FEATURE_WCDMA_HFN_UPDATE))
/*=========================================================================

FUNCTION ENC_HFN_UPDATE_EVT_HANDLER

DESCRIPTION
  This function triggers HFN update for MAC

DEPENDENCIES
  HFN Update Event

RETURN VALUE
  None

SIDE EFFECTS
  None.

=========================================================================*/
void enc_hfn_update_evt_handler(void);
#endif

/*===========================================================================
FUNCTION        ENC_UL_MAINT_EVENT_HANDLER

DESCRIPTION     This function is a maintainance function for UL. This is set
                to occur in the middle of frame. Currently it does following.
                1. Set Maintencence paremeters for UL which includes 
                   - Beta table upload to mDSP
                   - HS CM Demod and Mod table updation
                   - HS UL logging
                2. Send UL DPCCH Cmd to FW if Required based on Config Window

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern void enc_ul_maint_event_handler(void);

/*===========================================================================
FUNCTION        ENC_SET_UL_MAINT_EVENT_PARAMETERS

DESCRIPTION     This function is a maintainance function for UL.
                This is called from the UL Maintenence Event Handler as well as from EUL FIQ Path.
                This function does the following:
                Beta table upload to mDSP
                HS CM Demod and Mod table updation
                HS UL logging

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
extern void enc_set_ul_maint_event_parameters(void);

/*===========================================================================
FUNCTION        ENC_REFRESH_BETA_TABLE

DESCRIPTION     This function populates the beta table in mDSP from the current
                active beta table buffer. It doesnot adjust the mDSP beta table
                buffer to upload beta table to. It is intended to be called
                when beta table values have changes after it has been updated
                in UL maintainance event

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/

extern void enc_refresh_beta_table(void);


/*===========================================================================
FUNCTION        ENC_SET_BC_BD_PAIR_VAL

DESCRIPTION     This function saves Bc and Bd values for beta table upload
                during UL maintainance function

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/

extern void enc_set_bc_bd_pair_val(
  /* Beta C value to set */
  uint8 ul_bc,
  /* Beta D value to set */
  uint8 ul_bd);

/*===========================================================================
FUNCTION        ENC_SET_RATE_ADJ_IN_CM_DELTA

DESCRIPTION     This function saves rate adjust delta values for beta table
                upload during UL maintainance function

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/

extern void enc_set_rate_adj_in_cm_delta(
  /* rate adjust delta value */
  uint32 delta);


/*=========================================================================

FUNCTION ENC_HDET_CONV_DONE_ISR

DESCRIPTION
  This ISR is invoked by mDSP after a HDET Conversion is ready for reading
  in order to compensate RF TX power limit.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  RF will be called to read HDET and update RF TX power limit

=========================================================================*/
void enc_hdet_conv_done_isr
(
  void
);

/*=========================================================================

FUNCTION ENC_TRCH_TB_REPORT

DESCRIPTION
  This function reports the number of trasnsport blocks sent out from each
  active transport channel. This function is called in TASK context every
  2 seconds.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  The buffer that holds the information for each transport channel is wiped
  clean.
=========================================================================*/
void enc_trch_tb_report
(
  void
);

/*===========================================================================
FUNCTION  ENC_GET_MTPL_MINUS_TPC_ACCU_HIST

DESCRIPTION
  This function is called by EUL EDPCH module in FIQ to get the history of
  MTPL minus TPC Accum for the past frame in TX AGC unit. 

DEPENDENCIES
  Called by EDPCH module in FIQ

RETURN VALUE
  const int16*, pointer to the history array
  return NULL if no history available

SIDE EFFECTS
  None
===========================================================================*/
const int16* enc_get_mtpl_minus_tpc_accu_hist(void);

/*===========================================================================
  FUNCTION  WENC_GET_DPCCH_INIT_TX_PWR_AGC

DESCRIPTION
  This function returns DPCCH initial power in TX AGC unit

DEPENDENCIES
  Caller must be sure that DPCCH initial power is alrteady calc'ed

RETURN VALUE
  int32 DPCCH initial power in TX AGC unit

SIDE EFFECTS
  None
===========================================================================*/
int32 wenc_get_dpcch_init_tx_pwr_agc(void);

/*===========================================================================
  FUNCTION  ENC_GET_LATEST_BETA_C

DESCRIPTION
  This function returns latest beta_C value

DEPENDENCIES
  None

RETURN VALUE
  uint32 for beta_C

SIDE EFFECTS
  None
===========================================================================*/
uint32 enc_get_latest_beta_c(void);

/*===========================================================================
  FUNCTION  ENC_GET_LATEST_BETA_D

DESCRIPTION
  This function returns latest beta_D value

DEPENDENCIES
  None

RETURN VALUE
  uint32 for beta_D

SIDE EFFECTS
  None
===========================================================================*/
uint32 enc_get_latest_beta_d(void);

/*===========================================================================
  FUNCTION  ENC_NEXT_FRAME_DPDCH_ACTIVE

DESCRIPTION
  This function returns whether next UL DPCH frame has DPDCH active

DEPENDENCIES
  This function can only be called in DCH state by EDPCH module after
  TX_GEN_GET_DATA event

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean enc_next_frame_dpdch_active(void);

/*===========================================================================
FUNCTION  ENC_HAS_DPCH_DONE_PCP

DESCRIPTION
  This function returns TRUE if UL DPCH is TXing normal DPCH after PCP;
  otherwise FALSE.

DEPENDENCIES
  This function can only be called in DCH state by EDPCH module during EDPCH
  event that is after UL TX_GEN_GET_DATA event.

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean enc_has_dpch_done_pcp(void);

/*===========================================================================
FUNCTION  ENC_IS_DPCH_IN_SRB_DELAY

DESCRIPTION
  This function returns TRUE if UL DCH is in SRB delay;
  otherwise FALSE.

DEPENDENCIES
  This function can only be called in DCH state by EDPCH module during EDPCH
  event that is after UL TX_GEN_GET_DATA event.

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean enc_is_dpch_in_srb_delay(void);

/*===========================================================================
FUNCTION        enc_check_seamless_for_eul

DESCRIPTION     This function checks if the present recfg is a simple DCH seamless recfg

DEPENDENCIES    Modifies the enc_seamless_dch_reconfig_flag which will disable the
                EUL tx only if it is not a seamless reconfig

RETURN VALUE    None

SIDE EFFECTS    None
==========================================================================*/
void enc_check_seamless_for_eul(l1_ext_cmd_type* ext_cmd);

#ifdef FEATURE_WCDMA_REL6
/* Functions to calculate the Maximum Power Reduction at MTPL */

/*===========================================================================
FUNCTION enc_calc_mpr_value_enh

DESCRIPTION
  This function is called to calculate the mpr value based on the
  Enhanced Rel 6 MPR algorithm. MPR bins for each hs type are directly 
  written to FW async structures. PACK/UNPACK model is removed.

DEPENDENCIES
  None

RETURN VALUE
  Current Beta_Ed index.

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 enc_calc_mpr_value_enh(ul_mpr_calc_parms *
                                     #ifdef FEATURE_WCDMA_16_QAM
                                     ,l1_e_max_ch_codes_enum_type etfc_coding_sf
                                     #endif /* FEATURE_WCDMA_16_QAM */
                                     );

#ifdef FEATURE_WCDMA_DC_HSUPA 
/*===========================================================================
FUNCTION enc_fetch_dc_mpr_value_to_fw

DESCRIPTION
For a given combination of mu, betahs, betaec, betaed, this function will look up moment value and write to FW interface
wfw_intf_tx_eul_common_async_write_ptr.
The enchs_rhs_type_enum_type input can indicate the Primary or Secondary by its value: ENCHS_NUM_RHS_TYPE means 
secondary.type_enum_type input can indicate the Primary or Secondary by its value: ENCHS_NUM_RHS_TYPE means 
secondary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void enc_fetch_dc_mpr_value_to_fw(uint8 mu, uint16 betahs, 
		uint16 betaec, uint16 betaed, enchs_rhs_type_enum_type betahs_type, uint8 carr_idx);


#endif /*FEATURE_WCDMA_DC_HSUPA  */


#ifdef FEATURE_WCDMA_REL7P1_MPR
/*===========================================================================
FUNCTION UL_MPR_GET_RHO_ED_IDX

DESCRIPTION
  See ul_mpr_find_rho_idx(uint16, RHO_Ed) 
  

DEPENDENCIES
  Takes A_edx15 value as input

RETURN VALUE
  Correspondong rho_ed index. 

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 ul_mpr_get_rho_ed_idx(uint16);
#endif /*FEATURE_WCDMA_REL7P1_MPR*/

#endif /* FEATURE_WCDMA_REL6 */

/*==========================================================================
FUNCTION: ENC_SET_DPCH_INIT_TX_PWR_THROUGH_QXDM

DESCRIPTION:
  This function gives the flexibility to set the value of DPCH INIT PWR to a
  fixed value through QXDM

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:

==========================================================================*/
void enc_set_dpch_init_tx_pwr_through_QXDM(int16 val);

/*===========================================================================
FUNCTION WENC_GET_TXAGC_HIST

DESCRIPTION
  This function is called from L1 command handler and updates history
  table of txAGC values for past 15 slots in TX AGC units, taking into 
  account HDET corrections and CM slots

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Global wenc_tx_pwr_hist_agc[] and wenc_mpr_val_hist[] is copied to the buffer passed in
==========================================================================*/
extern void wenc_get_txagc_hist(int16 *txagc_buf, int16 *mpr_buf);

/*===========================================================================
FUNCTION enchs_set_cqi_override_from_diag

DESCRIPTION
  This function set CQI test mode for UL HS DPCCH. It checks override
  status in mDSP. If mDSP doesn't have override eanabled then it programs
  mDSP with override immediatey.
  If mDSP has already override enabled then it sets up pending flag and UL
  maintainance event will manage uprating pending override.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void enchs_set_cqi_override_from_diag(
  /* indicate override by CQI test pattern.
     TRUE override enabled, FALSE is DIABLED */
  boolean override,
  /* CQI repetition cycle repeat count. Must be minimum 1 for CQI test
     pattern to be effective */
  int16 cqi_repetition_cycle_repeat_count,
  /* length of ACK/NACK pattern. Max 8 */
  uint8 cqi_pattern_len,
  /* Array pointer to ACK/NACK/DTX pattern */
  uint8 cqi_test_pattern);

/*=========================================================================

FUNCTION ul_tx_ctrl

DESCRIPTION


DEPENDENCIES

RETURN VALUE
    None.

SIDE EFFECTS
  None.

=========================================================================*/
extern boolean ul_tx_ctrl(boolean enable_tx, wl1_ul_carr_id_enum_type carr_idx);

/*=========================================================================

FUNCTION wl1_enc_init_register_writes

DESCRIPTION
  This routine intializes the UL tx registers that are initialized during
  wl1m_init(). This API is required to be called by MCPM Tx as call back
  after bringing a modem block out of power collapse.

DEPENDENCIES

RETURN VALUE
    None.

SIDE EFFECTS
  None.

=========================================================================*/

extern void wl1_enc_init_register_writes_cb(void);

/*===========================================================================
FUNCTION wcdma_query_tx_power_dbm_swap

DESCRIPTION
  This is the callback funtion for the timer and gets called every 670ms.
  It updates the buffer index for the max power double buffer and clears the
  old buffer to WCDMA_QUERY_TX_POWER_DBM_NO_INFO. After clearing the buffer
  the timer is again reinitialized.

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
void wcdma_query_tx_power_dbm_swap(uint32 unused);

#ifdef FEATURE_WCDMA_RACH_TX_PWR
/*===========================================================================
FUNCTION wcdma_control_rach_tx_pwr

DESCRIPTION
         wcdma_control_rach_tx_pwr: This API may make UE non-compliant
         to 3GPP and should be used only for debug and test


         Range 1: -57 < rach_opt_min_pwr <= 23

         Range 2: rach_opt_adj_val = 0 User mistake and warning F3.
                  10 <= rach_opt_adj_val < 70; Warning
                  rach_opt_adj_val > 70; Discard input.      

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
/* Proto-type wenc.h */	
void wcdma_control_rach_tx_pwr(uint8 init_rach_opt_adj_val, int8 init_rach_opt_min_pwr);
#endif 

/*===========================================================================

FUNCTION WL1_ENC_DYNAMIC_MEM_ALLOC

DESCRIPTION
  Allocates memory for all the variables dynamically allocated in UL ENC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void wl1_enc_dynamic_mem_alloc(void);

/*===========================================================================

FUNCTION WL1_ENC_DYNAMIC_MEM_DEALLOC

DESCRIPTION
  Deallocates memory for all the variables dynamically allocated in UL ENC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void wl1_enc_dynamic_mem_dealloc(void);

/*===========================================================================
FUNCTION wcdma_query_tx_power_dbm

DESCRIPTION
  This function returns the greater of the max TxAGC values computed for
  the immediately previous 670ms time period and current running time period.
  If not in DCH state it will return WCDMA_QUERY_TX_POWER_DBM_NO_INFO.

DEPENDENCIES
  None

RETURN VALUE
  int16. Returns max TxAGC value which is 1 sec old on an average.
 
SIDE EFFECTS
  None
===========================================================================*/
int16 wcdma_query_tx_power_dbm(void);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
/*===========================================================================
FUNCTION       WL1_INIT_UL_QTA_VARS

DESCRIPTION    This funciton will reset all the QTA related variables
  
DEPENDENCIES   None
 
RETURN VALUE   Will reset all the QTA related variables

SIDE EFFECTS   None
===========================================================================*/
extern void wl1_init_ul_qta_vars(void);

/*===========================================================================
FUNCTION       WL1_QTA_FREE_PENDING_PDU_MEM

DESCRIPTION    This funciton will clear the DSM items that is built just before making the QTA state to active
  
DEPENDENCIES   None
 
RETURN VALUE   Will reset all the QTA related variables

SIDE EFFECTS   None
===========================================================================*/
extern void wl1_qta_free_pending_pdu_mem(l1_ul_tx_data_type *l1_ul_tx_buf_ptr);

/*===========================================================================
FUNCTION WENC_QTA_PROCESS_RACH_ACTION

DESCRIPTION  This is a wrapper routine for wenc_process_rach_action() function
             called by FRM (Frame Reservation Manager) to abort ongoing RACH
             procedure before continuing with QTA procedure. 

DEPENDENCIES None

RETURN VALUE None 

SIDE EFFECTS Aborts any ongoing RACH procedure
==========================================================================*/
void wenc_qta_process_rach_action(void);
#endif /* FEATURE_QTA */

/*===========================================================================
FUNCTION enc_clean_trch()

DESCRIPTION
This function is used to clean up Transport channel data and related globals

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
void enc_clean_trch(void);

/*===========================================================================

FUNCTION    WENC_POST_LOCAL_CMD_DONE_TO_L1M

DESCRIPTION
  Posts drop/cleanup command done to L1M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void wenc_post_local_cmd_done_to_l1m( l1_ul_phychan_enum_type ul_phychan );

/*===========================================================================

FUNCTION    WL1_SET_DPCH_DROP_TIMEOUT

DESCRIPTION
  Sets Timeout for DPCH channel drop. This function is called in pre-callback
for MDSP DPCH disable command to set the Timeout just before the disable command
is configured to FW.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void wl1_set_dpch_drop_timeout(void);

/*===========================================================================
FUNCTION        WL1_QUERY_MODEM_TX_POWER 

DESCRIPTION
   This function computes Tx Modem power in IDLE/FACH and connected state as
   LOW, MEDIUM or HIGH.

DEPENDENCIES
  None

RETURN VALUE
  wl1_ul_modem_tx_power_enum_type: Modem power is HIGH/MEDIUM or LOW.
 
SIDE EFFECTS
  None
===========================================================================*/
extern wl1_ul_modem_tx_power_enum_type wl1_query_modem_tx_power(void);

/*===========================================================================
FUNCTION WENC_UPDATE_TX_PWR_HIST_AGC

DESCRIPTION
  This function reads MDSP and updates history table of txAGC values
  for past 15 slots

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Global wenc_tx_pwr_hist_agc[] and wenc_mpr_val_hist[] are updated from MDSP
==========================================================================*/
extern void wenc_update_tx_pwr_mpr_hist_agc(void);

/*===========================================================================
FUNCTION wenc_gcf_tx_pwr_hist_update(void)

DESCRIPTION
  This function 
    update the min/max Tx Power value in history
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void wenc_gcf_tx_pwr_hist_update(void);

/*===========================================================================
FUNCTION wenc_gcf_tx_accum_hist_update(void)

DESCRIPTION
  This function 
    update the min/max increase Tx accum value in history
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wenc_gcf_tx_accum_hist_update(void);

/*===========================================================================
FUNCTION wenc_power_backoff_if_gcf(void)

DESCRIPTION
  This function 
    set power headroom to 0 if UE is under GCF test;
    otherwise, keep power headroom +2 dB for field performance
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wenc_power_backoff_if_gcf(void);


#ifdef FEATURE_QSH_EVENT_METRIC

/*===========================================================================
FUNCTION     wl1_ul_metric_cb

DESCRIPTION
  Call back from QSH module to collect the UL metrics.
  
DEPENDENCIES
  This will be invoked by WL1 QSH module whenever QSH module calls
  the WL1 CB with one of the UL Metrics Tags set.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_ul_metric_cb(wl1_qsh_metric_e metric_id, uint8* addr_ptr, wl1_qsh_metric_action_enum_type action);

#endif /* FEATURE_QSH_EVENT_METRIC */

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION     wl1_ul_dump_cb

DESCRIPTION
  Dump UL related Globals for UL minidump collection
    
DEPENDENCIES
  This API will be called from WL1 QSH module, which will be invoked by QSH

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_ul_dump_cb(void* addr_ptr);

/*===========================================================================
FUNCTION     wl1_ul_populate_state_vars

DESCRIPTION
  Dump various UL related State Variables and Misc Globals for UL minidump collection
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wl1_ul_dump_state_vars_and_misc_globals(wl1_ul_mini_debug_info_struct_type * ul_mini_dump_ptr);

/*===========================================================================
FUNCTION     wl1_ul_dump_rach_vars

DESCRIPTION
  Dump PRACH related Globals for UL minidump collection
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wl1_ul_dump_rach_vars(wl1_ul_mini_debug_info_struct_type *ul_mini_dump_ptr);

/*===========================================================================
FUNCTION     wl1_ul_dump_dpch_vars

DESCRIPTION
  Dump UL DPCH related Globals for UL minidump collection
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wl1_ul_dump_dpch_vars(wl1_ul_mini_debug_info_struct_type * ul_mini_dump_ptr);

/*===========================================================================
FUNCTION     wl1_ul_dump_cpc_dtx_drx_edrx_vars

DESCRIPTION
  Dump CPC DTX/DRX and EDRX related Globals for UL minidump collection
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wl1_ul_dump_cpc_dtx_drx_edrx_vars(wl1_ul_mini_debug_info_struct_type * ul_mini_dump_ptr);

/*===========================================================================
FUNCTION     wl1_ul_dump_hs_rach_vars

DESCRIPTION
  Dump HS RACH related Globals for UL minidump collection
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wl1_ul_dump_hs_rach_vars(wl1_ul_mini_debug_info_struct_type * ul_mini_dump_ptr);

/*===========================================================================
FUNCTION     wl1_dump_edch_cm_params

DESCRIPTION
  Dump EDCH CM related params for UL Minidump collection
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wl1_dump_edch_cm_params(wl1_debug_ul_edch_cm_info_struct_type *edch_cm_dump_ptr);

/*===========================================================================
FUNCTION     wl1_dump_eul_isr_timing

DESCRIPTION
  Dump EUL ISR timing related info for prev 3 instances of ISR firing for minidump
  collection.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wl1_dump_eul_isr_timing(wl1_debug_ul_eul_isr_timing_struct_type *wl1_eul_timing_dump_ptr);

/*===========================================================================
FUNCTION     wl1_ul_dump_edch_vars

DESCRIPTION
  Dump all EDCH related globals for UL minidump collection
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wl1_ul_dump_edch_vars(wl1_ul_mini_debug_info_struct_type * ul_mini_dump_ptr);

/*===========================================================================
FUNCTION   wl1_dump_edch_phychan_cfg_struct

DESCRIPTION
  Dump EDCH Phychan/Cfg related params and globals for UL minidump collection
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wl1_dump_edch_phychan_cfg_struct(wl1_debug_ul_e_ul_sig_cfg_db_struct_type *e_ul_cfg_db_dump_ptr);

/*===========================================================================
FUNCTION     wl1_ul_dump_edl_vars

DESCRIPTION
  Dump EDL related globals for UL minidump collection
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wl1_ul_dump_edl_vars(wl1_ul_mini_debug_info_struct_type * ul_mini_dump_ptr);

/*===========================================================================
FUNCTION     wl1_ul_dump_agch_vars

DESCRIPTION
  Dump E-AGCH related globals and params for UL minidump collection
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wl1_ul_dump_agch_vars(wl1_ul_mini_debug_info_struct_type * ul_mini_dump_ptr);

/*===========================================================================
FUNCTION     wl1_ul_dump_e_dl_rl_info_vars

DESCRIPTION
  Dump E-HICH/RGCH related globals for UL Minidump Collection
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void wl1_ul_dump_e_dl_rl_info_vars(wl1_ul_mini_debug_info_struct_type *ul_mini_dump_ptr);

#endif /* FEATURE_QSH_DUMP */

#endif /* WENC_H */
