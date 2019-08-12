#ifndef MDSP_SYNC_H
#define MDSP_SYNC_H

/*===========================================================================
                             M S D P _ S Y N C . H


GENERAL DESCRIPTION
  The command interface between the ARM and the mDSP on the MSM5200.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2000 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/mdspsync.h_v   1.23   21 Jun 2002 11:17:18   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mdspsync.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
02/24/15    ap      Initial check-in for WL1 QSH framework 
12/01/15    gp      CPC DTX: Fix to save the queued CPC config in MDSP queue buffer 
09/11/15    stk     FR 30139: SVS+ and NOM+ frequency corner support in MCVS
04/22/15    ash     Maintain local copy of the CPC DTX DB Idx while queuing up mdsp DTX cmd
11/24/14    svh     L1 changes for MDSP Recovery.
11/18/14    mit     fixing typecasting problem for clock level
11/17/14    cc      Send gap start action time in CM/FMO for FW to get a reference frame
11/10/14    tsk     Mainline code under feature FEATURE_WCDMA_JOLOKIA_MODEM on Jolokia branch.
11/7/14     sks     Redesign of MCVS algo.
09/08/14    mm      Changed start slot field for FMO gaps to int8
02/18/13    sr      Initial checkin for CPC Lite sleep
07/15/14    bj      WCDMA Narrow Band Rejection for System Search.
07/01/14    hk      Header file inclusion cleanup
06/23/14    sad     Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag
06/03/14    tsk     Compilation fixes for JOLOKIA 1.0
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
05/06/14    geg     Use safer versions of memcpy() and memmove()
04/27/14    jkb     Add check for Feature dual sim with feature qta
04/21/14    kcm     Bolt Rumi Featurization Removal 
04/14/14    cc      Create an API for RF to call idle W2X search cfg to WFW in FTM mode
03/27/14    as      Mainlining FEATURE_PARTIAL_QBTA.
03/13/14    ac      Add MDSP_TO_ARM_INT_WL1_AGC_CFG_DONE event
03/10/14    ks      Bolt FED API checkin
02/14/14    ar      Porting Bolt specific uplink changes on to Bolt 2.0
01/21/14    abs     Cleaning up DSDA~CXM(FR:2334) changes
12/10/13    bs      Improve SW-FW handshake of TDEC clock usage
11/21/13    jd      FET 2.0 Searcher two queue approach.
11/21/13    mm      Send ADC TQ config cmd to FW for dynamic TQ allocation
11/20/13    vs      added MDSP_TO_ARM_INT_DEMOD_HS_DPCCH_CHAN_CFG_DONE in mdsp events.
10/23/13    hdk     Moving to enum for SW-FW command ID
10/15/13    mm      Initial FE-FACH check-in
10/14/13    pr/sr   Channel config optimizations
09/10/13    rsr     3C API Checkin changes
10/02/13    gp      Add dummy dpcch indication for ul cfg dpcch cmd
09/23/13    as      Porting Dual Sim from Triton to Dime.
08/21/13    at      Fixed Bolt Compilation errors.
08/15/13    at      Post UL DPCH cfg disable done command to L1M after UL DPCH disable done 
                    interrupt from FW is received.
08/12/13    cc      DBDC CM W2W feature check in
07/24/13    jd      Changes for Dual band Dual carrier
06/18/13    vs      Added support for the ASDiv feature.
06/06/13    pr      Changed rxlmDynGroupMask type from uint8 to uint32
05/20/13    pr      SW support for FW Shadow Cell
05/22/13    oh      Adding DC-HSUPA framework support
05/21/13    sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
05/23/13    geg     WSW decoupling (except for component of QCHAT)
05/09/13    pr      Featurized code under FEATURE_WCDMA_BOLT_RUMI_HWIO_DEMOD as part of Bolt Bringup.
04/17/13    pr      Add support for RF cal to WFW
03/27/13    pr      SW Changes for TxD command done from FW.
02/11/13    gsk/sr  EDRX Full Sleep initial check in.
02/05/13    pv      Featurize out FR 2001 for Triton.
02/05/13    pv      Support for FR 2001 - RF Wakeup Optimization.
02/05/13    ms      Added support for HS RxLM
01/09/13    dp      Changes to allow sending w2l cm gap-end time to WFW
01/02/13    pr      Moved mdsp_sync_send_rf_cfg_cmd , mdsp_sync_send_rf_init_cmd and
                        mdsp_sync_iq_sample_capture_cmd to public API's for RF.
07/13/11    pv      Offline PICH/Parallel Reacq fixes.
08/31/12    vr      Added new field for ASC and Preamble Sig Ind usage for calculating 
                    Common EDCH Rsrc Index for HSRACH
08/22/12    pr      Fixed merge errors.
08/14/12    vs      Changed the FW structure naming for the ept calibration.
08/14/12    vs      L1 changes for W EPT calibration Support.
08/13/12    pr      RF Changes for Dime Bringup.
07/30/12    pr      Dime Bringup Changes for RF.
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12    pv      Offline PICH changes.
06/08/12    gv      Feature HS RACH Initial code check in
06/07/12    vs      MBMS feature cleanup.
05/08/12    ms      Added support to send Treset sync cmd to FW
04/24/12    mmr     Removed mdsp_sync_send_iq_cap_cmd()
03/08/12    vs      Added the support for the feature "QPA+DPD".
02/12/12    hk      Nikel feature cleanup
02/01/12    raj     Nikel Feature cleanup.
02/01/12    gnk     Nikel Feature cleanup.
01/27/12    vs      nikel feature cleanup.
01/20/12    zr      Mainlined FEATURE_WCDMA_GENESIS_SW and FEATURE_WCDMA_GENSIS_FWS, 
                    removed FEATURE_MDSP_DEBUG_NAK, FEATURE_WCDMA_OFFLINE_CLK_SPEED_CHANGE,
                    FEATURE_WCDMA_L1_MDSP_CMD_LOG, FEATURE_WCDMA_SAMPLE_SERVER_DATA_LOGGING
01/17/12    vsr     Added ARD Algorithm Support
11/30/11    rc      Added DSR support
11/29/11    gnk     Removed some unused features
10/26/11    dp      Added RxD reconfig capabilities after irat searches
10/20/11    gnk     Removed MTD related code for NikeL
09/13/11    sv      Support Step1 command type for intra freq step1 searches.
09/07/11    kcm     Added support for Tx DAC Cal
07/13/11    mk      Moved mdsp_sync_send_sample_server_cmd to wl1api.h
07/12/11    mk      Compilation error fix. 
06/28/11    sp      Added support for Rach Abort under FEATURE_WCDMA_RACH_ABORT_PROCEDURE
05/11/11    gnk     Mainlining FEATURE_RF_TX_PWR_LIM,FEATURE_RF_TX_PWR_LIM_REV feat
05/26/11    dp      Added code so W2X_IDLE_CFG gets cleanup buf idx
04/28/11    vsr     CME changes for NikeL
04/28/11    mmr     Added MDSP IQ capture command for xo cal purposes.
04/27/11    mc      Removed mDSP API to API folder for CMI compliance.
04/27/11    oh      Move the func. prototypes for TxLM Cfg cmd to wl1api.h 
04/26/11    pv      Added an ENUM for AGC tune done interrupt from MDSP.
04/26/11    dp/uk   Add support for W2X_IDLE_CMD
04/25/11    mc      SW decoupling
04/10/11    mc      Removed unused macros
04/04/11    oh      Changes to support UL DCH on NikeL
04/04/11    yh      Added support for RxLM/TxLM
04/01/11    sj      Added more debug info in mdsp_sync_cmd
04/01/11    sj      Changed WFW_START_MODE_G2W to WFW_START_MODE_X2W to match up with FW intf change
03/11/11    dp      Ported CPC DRX changes to offline Architecture
02/09/11    pv     Support for sending one of three offline clck speed in mdsp_sync_send_clock_mode().
12/01/10    dp      Added mdsp_sync_send_rf_tune_cmd function prototype along 
                    with new structures used by the function definition.
09/24/10    hk      Removed the mutex added for mDSP demod cmd pre callback
09/20/10    hk      Added mutex protection for mDSP demod cmd pre callback
09/17/10    hk      Changes to perform the config window check in mdsp pre cb
09/02/10    gnk     Checking in the changes for UL open loop MTD feature
09/02/10    gv      Code Checkin for CPC DTX
08/23/10    ks      Removed inclusion of mdsp header for Genesis (featurized)
07/13/10    sup     Fixed  lint errors
07/06/10    sup     Added a callback fn type definition for a pointer to 
                    a function which takes a boolean argument
05/03/10    hk      Fixed compiler warnings
03/30/10    rvs     IRAT sample-server config changes for Chetak and
                    general clean-up.
03/25/10    ms      Added prototype for mdsp_sync_find_pending_mdsp_cmd
02/26/10    ks      Added support to use FWS to send START/STOP and Apps Enable/
                    Disable command to FW
11/30/09    rgn     pass dc state to fw in hs-dpcch cfg command
11/18/09    ms      Added support to handle HS-SCCH orders.
09/22/09    sup     Removed log code assignments and placed them in
                    log_codes_wcdma_int_v.h and removed featurizations using FEATURE_DEBUG_MDSP_NAK 
                    to avoid log code duplications
09/09/09    rvs     Add support for QICE F/Finv log packet.
09/04/09    mc      Fix compilation error
09/04/09    mc      Added support for MIMO subframe rollover
07/31/09    yh      Remove handling of WFW_SW_CM_GAP_START_FLAG and WFW_SW_CM_GAP_STOP_FLAG
                    interrupts.
07/16/09    sv      Enabling the SS freeze unfreeze cmd support.
07/15/09    rvs     Added oride QICE config command.
07/09/09    yh      Support for FEATURE_WCDMA_OFFLINE_MODEM_IRAT_PHASE2
06/15/09    uk      Support for sending START_FACH_MEAS cmd to fw before 
                    starting a FMO gap for GSM meas
05/26/09    sv      Added support for sending freeze\unfreeze sample server command to FW.
04/22/09    gnk     Fixed compiler warnings
02/11/09    scm     Implement mdsp_sync_reset_wfw_cmd_done_counters().
02/05/09    rvs     Added mdsp_sync_pre_send_sync_cmd_cb() protoype.
01/27/09    stk/yh  Added CM Start and End enums for IRAT, under FEATURE_WCDMA_OFFLINE_MODEM_W2G
01/19/08    mg      Added MDSP_TO_ARM_INT_WL1_FW_CRASH_DONE
11/18/08    sv      Added cfn logging back into sync cmd struct.
11/13/08    yh      Add CM support
11/11/08    rgn     Replaced cfn in sync cmd with frc frame num/cx1
10/03/08    hk      Bringing in 7k Mailine fixes
10/01/08    mc      Added support for MIMO UL.
09/29/08    rgn     Increased the sync cmd q lenght to 60
09/16/08    mg      extern swCmdDoneFlagCounter
08/27/08    vsr     Added CME support in mdsp_sync_send_demod_cfg_cmd()
07/31/08    oh      EUL changes for Taxis.
06/18/08    ms      Removed occurences of FEATURE_WCDMA_HSPA_DEC.
06/17/08    vsr     Moved mdsp_phychan_enum_type to MCAL DL from mdspsync
                    Removed references to mdsp_phychan_enum_type
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
01/09/08    mg      Add mdsp_sync_send_offline_cm_search_cmd 
10/24/07    vsr     Mainlining MBMS and Enhanced DL changes 
09/26/07    nd      Added a new mdsp-wl1 event MDSP_TO_ARM_INT_WL1_FS_PROC_DONE
04/09/07    mc      Added support for EQ 2nd interrupt done from mDSP.
03/01/07    vrao    Added MDSP_ARM_INT_TX_POWER_DET_ISR_DONE - FW interrupt 
                    based on WCDMA Tx AGC
10/17/06    ms      Added support for FDPCH.
10/04/06    bbd     Changed FMO command to use uint16 datatype for RF setup buffer index.
09/25/06    gv      Added function prototype for function to clear the
                    current comb shadow.
08/14/06    yiz     Mainline HSUPA code
03/21/06    vip     Support for new AGC interface with two receive chains
03/21/06    mg      Add MDSP_TO_ARM_INT_WL1_SS_DATA_DONE for Sample Server data 
                    done interrupt.
03/16/06    mc      Added support for Equalizer under the feature flag
                    FEATURE_WCDMA_EQ_CONTROLLER
12/05/05    bbd     Added support for CM STTD searches.
05/18/05    vp      Added support to reslam physical channels
06/15/06    yiz     Add MDSP_TO_ARM_INT_WL1_HDET_DONE for RF_TX_PWR_LIM_REV
05/11/05    gs      Externalized function mdsp_cmd_log_cm_fing_cfg
                    Moved finger cfg async write related macros to this module
05/05/05    gs      Added support for mDSP command log packet
08/30/04    gs      Added definitions for HS channel config macros, structure
                    and functions
                    Added boolean for command suspended status for each command
05/10/04    gs      Added macro MDSP_CMD_START_FACH_MEAS_CMD_LEN
                    Changed macro SRCHCM_LIST_SRCH_MAX_TASK value to 16
                    Added structure definition for FACH inter-freq search start
                    Included start inter-freq search start command structure
                    in union of mDSP commands
                    Externalized function mdsp_send_start_fach_interf_meas_cmd
03/04/04    yus     Increase MDSP_SYNC_DSP_CMD_Q_LEN_MAX to 12 from 6.
10/24/03     gs     Enclosed definition and declaration for compressed mode
                    step 1 and list search in feature
                    FEATURE_CM_FOR_INTER_FREQUENCY_MEASUREMENT.
08/18/03    gs      Defined macro MDSP_CMD_CM_START_COMPRESSED_MODE_LEN
                    Defined structure mdsp_cm_start_compressed_mode_struct_type
                    Added structure start_compressed_mode to union of mDSP
                    command
                    Changed declaration of mdsp_cm_init_cmd
07/02/03    src     Added two new hash-defined macros MDSP_SYNC_CMD_DONE and
                    MDSP_SYNC_CMD_NONE for mDSP command statuses.
06/04/2003   m      Added code to dump DSP dedug variables
06/18/2002  gs      Added the macros, structures, function declaration for CM
                    step 1 and list search.
06/11/2002  asr     Changes to support combiner counter slamming up on wake up from
                    sleep using the sleep controller: declaration of new function 
                    mdsp_sync_send_slam_combiner_time_cmd() and associated parameters.
05/12/2002  asr     Changes to support fast PICH setup: declaration of
                    function mdsp_sync_send_pich_phychan_cfg_cmd(), and 
                    declaration of special DL_PHYCHAN_CFG_CMD_ENA_PICH_BMSK
                    and ..._SHFT
01/31/2002  gs      Changed the UL_DPCCH_CFG_CMD_SLOT_FMT_BMSK to 0x1F to
                    accomodate the UL DPCCH slotformat in compressed mode.
01/02/2002  gs      1. Changed the slot format bitmask for dl phychan cfg cmd
                       to 0x3f from 0x1f to accomodate the slotformat in
                       compressed mode.
                    2. Added the declaration for function mdsp_cm_init_cmd.
11/02/2001  cpe     Added dltriparm.h to the included files
09/02/2001  mdl     Removed DSP wakeup command.  Removed feature flags.
07/31/2001  mdl     Fixed uint16 power params for PRACH Cfg Command.
06/15/2001  mdl     Added TTI to mdsp_sync_send_ul_prach_cfg_cmd
03/19/2001  mdl     changed MSM5200*.h to msm5200*.h in #include statements
02/20/2001  mdl     changed prototype of mdsp_sync_send_fing_cfg_cmd
01/11/2001  mdl     changed mdsp_sync_call_timeout_cmd to
                    mdsp_sync_call_timeout_fn

01/10/2001  mdl     added prototype for mdsp_sync_timeout routine, added
                    extern for mdsp_sync_call_timeout_fn

12/18/2000  mdl     removed finger as a parameter of the Modulator Set
                    Timing command.

12/01/2000  mdl     changed CFNmod8 to CFN in all references, changed masks
                    for CFN field from 3 to 8 bits, changed mask of
                    transmit time offset to 8 bits

11/08/2000  mdl     added #define NO_MEMCPY to force mem16cpy() to be used.
                    wrapped extern declaration of *memcpy() in #ifndef
                    NO_MEMCPY wrapper.  This was needed because the move
                    to using DIAG based MSG printing requires that
                    the PC_EMULATOR flag be removed and thus the prototype
                    for *memcpy() is no longer being #included.

10/30/2000  mdl     added support for trans_time_offset field in phychan
                    config command

10/18/2000  mdl     changed mdsp_phychan_enum_type order so that it
                    corresponds to values actually used by mDSP.

07/10/2000  mdl     first cut
===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "err.h"
#include "queue.h"
#include "msm.h"
#include "dltriparm.h"
#include "mdsputil.h"
#include "l1sapcommon.h"
#include "l1cmmgr.h"
#include "log_codes_wcdma_int.h"
#include "log.h"

#include "eulcfg.h"  //taxis work around- need to remove these dependencies because of mcal
//#include "edlcfg.h"
#include "mcalwcdma_edl.h"           // taxis workaround - check later whether necessary to include this header file

#include "wfw_sw_intf.h"
#include "wl1api.h"
#include "l1extif.h"

#define NO_MEMCPY
 /* define this to avoid using ARM memcpy function */

#define MDSP_SYNC_DSP_CMD_Q_LEN_MAX 6*2*5
 /* Maximum length of mDSP command queue */

/* Sync command status macros */
#define MDSP_SYNC_CMD_DONE 0x0000
#define MDSP_SYNC_CMD_NONE 0xFFFF

/*--------------------------------------------------------------------------
                        MDSP PARAMETER BLOCK LENGTHS

These are used by the mDSP sync interface
--------------------------------------------------------------------------*/
#define MDSP_CMD_STATUS_DUMP_LEN          0
#define MDSP_CMD_TEST_COMMAND_LEN         0
#define MDSP_CMD_FINGER_CONFIG_LEN        0

/* this command doesn't exist yet but is here for compilation */
#ifdef FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE
#define MDSP_CMD_START_AGC_LEN            2
#else /* FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE */
#define MDSP_CMD_START_AGC_LEN            0
#endif /* FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE */

#define MDSP_CMD_RF_INIT_LEN              2
#define MDSP_CMD_RF_CFG_LEN               2

#define MDSP_CMD_RF_DYN_LEN               2


/* length of IQ capture command in multiples of 16 bit word */
#define MDSP_CMD_IQ_CAPTURE_LEN           8
#define WFW_SAPT_IQ_CAPTURE_CMD_LEN       4
#define WFW_EPT_CAL_APPLY_DPD_CMD_LEN        2
#define WFW_EPT_TLB_MEM_MAP_CMD_LEN   8

/* the length of mDSP commands that have no parameters:
   Finger Config
   Start AGC
   Go To Sleep
   Wake Up
*/
#define MDSP_CMD_NO_PARM_LEN              0


#define MDSP_CMD_PHCH_CONFIG_LEN          8
#define MDSP_CMD_MODULATOR_TIMING_LEN     5
#define MDSP_CMD_UPLINK_DPCCH_CONFIG_LEN  3
#define MDSP_CMD_UPLINK_PRACH_CONFIG_LEN  8


#define MDSP_CMD_STEP1_SEARCH_LEN         1
#define MDSP_CMD_STEP2_SEARCH_LEN         1
#define MDSP_CMD_STEP3_SEARCH_LEN         1
#define MDSP_CMD_LIST_SEARCH_LEN          1
#define MDSP_CMD_ABORT_SEARCH_LEN         0
#define MDSP_CMD_ALL_SRCH_LEN             1
#define MDSP_CMD_SLAM_COMBINER_LEN        4

#define MDSP_CMD_CM_START_COMPRESSED_MODE_LEN 1
#define MDSP_CMD_CM_STEP1_SEARCH_LEN      1
#define MDSP_CMD_CM_LIST_SEARCH_LEN       1
#define MDSP_CMD_CM_PN_SEARCH_LEN         MDSP_CMD_CM_LIST_SEARCH_LEN
#define MDSP_CMD_START_FACH_MEAS_CMD_LEN  3

#define MDSP_HS_COMB_CHAN_CONFIG_LEN 1
#define MDSP_HS_UL_CHAN_CONFIG_LEN 0

#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
/* length of the antenna switch command to FW */
#define MDSP_ANT_SWITCH_CMD_LEN 28
#endif 

#ifdef FEATURE_WCDMA_HS_FACH
#define MDSP_HS_DEMBACK_TRESET_CMD_LEN 0
#endif

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
#define WFW_QTA_CMD_LEN 3
#endif /* FEATURE_QTA */

#define MDSP_QICE_NO_HW_PROG            (0)
#define MDSP_QICE_HW_PROG               (1)
#define MDSP_QICE_NO_FW_CONFIG          (0)
#define MDSP_QICE_FW_CONFIG             (1)
#define MDSP_QICE_NO_TEST_BUS           (0)
#define MDSP_QICE_TEST_BUS              (1)
#define MDSP_QICE_NO_SFC_LOG_CONFIG     (0)
#define MDSP_QICE_SFC_LOG_CONFIG        (1)
#define MDSP_QICE_NO_FFINV_LOG_CONFIG   (0)
#define MDSP_QICE_FFINV_LOG_CONFIG      (1)
#define MDSP_QICE_NO_FHT_PROG           (0)
#define MDSP_QICE_FHT_PROG(_eq_index)   (1 << (_eq_index))
#define MDSP_QICE_NO_EQ_CONFIG          (0)
#define MDSP_QICE_EQ_CONFIG(_eq_index)  (1 << (_eq_index))
#define MDSP_QICE_NO_EQ_RESET           (0)
#define MDSP_QICE_EQ_RESET(_eq_index)   (1 << (_eq_index))
#define MDSP_QICE_NO_SFC_RESET          (0)
#define MDSP_QICE_SFC_RESET(_eq_index)  (1 << (_eq_index))

#define MDSP_DAC_CAL_CMD_LEN 2
/*---------------------------------------------------------------------------*
 *                         MDSP Command Status Values                        *
 *---------------------------------------------------------------------------*/

/****************************************************************************
 ****************************************************************************
             uP TO mDSP DEMODULATOR COMMAND PACKET FORMATS
 ****************************************************************************
 ***************************************************************************/

/*--------------------------------------------------------------------------
                    STRUCTURE:  SRCH_CMD_STRUCT_TYPE

This structure is used to contain the parameters for the various search
commands STEP1_SEARCH, STEP2_SEARCH, STEP3_SEARCH, and LIST_SEARCH.
--------------------------------------------------------------------------*/
typedef enum {
  SRCH_CMD_NO_SRCH,
  SRCH_CMD_STEP1,
  SRCH_CMD_STEP2,
  SRCH_CMD_STEP3,
  SRCH_CMD_LIST,
  SRCH_CMD_ABORT,
  NUM_SRCH_CMD
} srch_cmd_enum_type;
typedef enum {
  SRCH_CMD_OFFLINE_NO_SRCH,
  SRCH_CMD_OFFLINE_STEP1,
  SRCH_CMD_OFFLINE_NASTT,
  SRCH_CMD_OFFLINE_PN,
  SRCH_CMD_OFFLINE_OTDOA,
  SRCH_CMD_OFFLINE_ABORT,
  NUM_OFFLINE_SRCH_CMD
} offline_srch_cmd_enum_type;

typedef enum {
  SRCH_CMD_OFFLINE_CM_NO_SRCH,
  SRCH_CMD_OFFLINE_CM_STEP1,
  SRCH_CMD_OFFLINE_CM_PN,
  NUM_OFFLINE_SRCH_CM_CMD
} offline_srch_cm_cmd_enum_type;

typedef enum {
  SRCH_CMD_NOQ_ENA,
  SRCH_CMD_LPQ_ENA,
  SRCH_CMD_HPQ_ENA,
  SRCH_CMD_BOTHQ_ENA
} srch_q_ena_enum_type;

typedef enum
{
  /*! Default step1 in 1 cmd */
  SRCH_STEP1_CMD_TYPE_DEFAULT,
  /*! First subtask for multiple commands step1 */
  SRCH_STEP1_CMD_TYPE_FIRST_SUBTASK,
  /*! Middle subtask for multiple commands step1 */
  SRCH_STEP1_CMD_TYPE_MIDDLE_SUBTASK,
  /*! Last subtask for multiple commands step1 */
  SRCH_STEP1_CMD_TYPE_LAST_SUBTASK
} srch_step1_cmd_enum_type;

typedef struct {
  /* enable bits for the two queues.  Use the #defined constants above */
  uint16 q_ena;
  srch_step1_cmd_enum_type step1_type;
  srch_intraf_queue_enum_type intraf_queue;
} srch_cmd_parms_struct_type;

#ifdef FEATURE_WCDMA_MDSP_RECOVERY
typedef enum
{
  /* Recovery indication received from FW */
  MDSP_RECOVERY_REQ_RECEIVED = 0,
  /* 0 + CPHY_SETUP/CELL_TRANS in progress */
  MDSP_RECOVERY_WAIT_FOR_EXT_CMD = 1,
  /* 0 + Sent RLF/OOS/WEAK_IND to RRC */
  MDSP_RECOVERY_RRC_NOTIFIED = 2,
  /* 0 + Using an existing IDLE REQ sent by RRC */
  MDSP_RECOVERY_EXISTING_IDLE_REQ = 3,
  /* 0 + Wait for QTA stop from RRC */
  MDSP_RECOVERY_WAIT_FOR_QTA = 4,
  MDSP_MAX_RECOVERY_TYPE
}mdsp_recovery_status_enum_type;
#endif

/*--------------------------------------------------------------------------
                     ENUM: DL_PHYCHAN_ACTION_ENUM_TYPE

This enumerates the three types of actions that can be taken on a downlink
physical channel.

SETUP: add the channel, it is currently INACTIVE
RECFG: reconfigure the channel, it is currently ACTIVE
TEARDOWN: tear down the channel, it is currently ACTIVE
--------------------------------------------------------------------------*/
typedef enum {
  DL_PHYCHAN_SETUP,
  DL_PHYCHAN_RECFG,
  DL_PHYCHAN_TEARDOWN
} dl_phychan_action_enum_type;

#ifdef FEATURE_WCDMA_CPC_DRX
typedef enum
{
  MDSP_SYNC_CMD_CPC_DRX_DTX_DISABLE = 0,
  MDSP_SYNC_CMD_CPC_DRX_DTX_ENABLE = 1,
  MDSP_SYNC_CMD_CPC_DRX_DTX_RECFG = 2
}mdsp_sync_cpc_drx_dtx_ops_enum_type;
#endif

/*--------------------------------------------------------------------------
                 STRUCTURE: DL_PHYCHAN_CFG_CMD_STRUCT_TYPE

This structure is used to contain the parameters for the downlink physical
channel config command to the mDSP.  Also included are masks for the valid
bits for each field.
--------------------------------------------------------------------------*/

// DL_PHYCHAN_CFG_CMD_HW_CHAN
#define DL_PHYCHAN_CFG_CMD_HW_CHAN_BMSK       0x0007

// DL_PHYCHAN_CFG_CMD_ARAE
#define DL_PHYCHAN_CFG_CMD_ARAE_BMSK           0x000F
#define DL_PHYCHAN_CFG_CMD_ARAE_SHFT           0
// DL_PHYCHAN_CFG_CMD_ASAP
#define DL_PHYCHAN_CFG_CMD_ASAP_BMSK           0x0002
#define DL_PHYCHAN_CFG_CMD_ASAP_SHFT           1
#define DL_PHYCHAN_CFG_CMD_HW_CHAN_SHFT       0


// DL_PHYCHAN_CFG_CMD_AUTO_SCCPCH
#define DL_PHYCHAN_CFG_CMD_AUTO_SCCPCH        0x0008


// DL_PHYCHAN_CFG_CMD_ENA
#define DL_PHYCHAN_CFG_CMD_ENA_BMSK           0x0001
#define DL_PHYCHAN_CFG_CMD_ENA_SHFT           0

// DL_PHYCHAN_CFG_CMD_RESLAM
#define DL_PHYCHAN_CFG_CMD_RESLAM_BMSK        0x0004
#define DL_PHYCHAN_CFG_CMD_RESLAM_SHFT        2

// for fast setup of PICh
#define DL_PHYCHAN_CFG_CMD_ENA_PICH_BMSK      0x0003
#define DL_PHYCHAN_CFG_CMD_ENA_PICH_SHFT      0

// DL_PHYCHAN_CFG_CMD_PHYCHAN_TYPE
#define DL_PHYCHAN_CFG_CMD_PHYCHAN_TYPE_BMSK  0x0007
#define DL_PHYCHAN_CFG_CMD_PHYCHAN_TYPE_SHFT  0

// DL_PHYCHAN_CFG_CMD_SLOT_FORMAT
#define DL_PHYCHAN_CFG_CMD_SLOT_FORMAT_BMSK   0x003F
#define DL_PHYCHAN_CFG_CMD_SLOT_FORMAT_SHFT   0

// DL_PHYCHAN_CFG_CMD_CCTRCH
#define DL_PHYCHAN_CFG_CMD_CCTRCH_BMSK        0x000F
#define DL_PHYCHAN_CFG_CMD_CCTRCH_SHFT        0

// DL_PHYCHAN_CFG_CMD_CFNMOD8
#define DL_PHYCHAN_CFG_CMD_CFN_BMSK           0x00FF
#define DL_PHYCHAN_CFG_CMD_CFN_SHFT           0

// DL_PHYCHAN_CFG_CMD_REF_FING
#define DL_PHYCHAN_CFG_CMD_REF_FING_BMSK      0x000F
#define DL_PHYCHAN_CFG_CMD_REF_FING_SHFT      0

// DL_PHYCHAN_CFG_CMD_TRANS_TIME_OFFSET
#define DL_PHYCHAN_CFG_CMD_TRANS_TIME_OFFSET_BMSK 0x00FF
#define DL_PHYCHAN_CFG_CMD_TRANS_TIME_OFFSET_SHFT 0

typedef struct {
  /* the hardware channel number */
  uint16 hw_chan;

  /* enable or disable the channel.  1 => enable; 0 => disable */
  uint16 arae;

  /* the type of downlink physical channel */
  uint16 phychan_type;

  /* the slot format, note this will change in an upcoming version */
  uint16 slot_format;

  /* the table index for the CCTrCH that this physical channel will carry.
     Set to 0 if no CCTrCH applies, i.e. for PICH, AICH */
  uint16 CCTrCH_idx;

  /* the CFN mod 8 of the frame that this channel will become active on */
  uint16 CFN;

  /* the transmit time offset relative to PCCPCH */
  uint16 trans_time_offset;

} dl_phychan_cfg_cmd_parms_struct_type;

/*--------------------------------------------------------------------------
                       STRUCTURE: SET_MOD_TIMING_CMD_STRUCT_TYPE

This structure contains the parameters for the "set modulator timing relation"
command.
--------------------------------------------------------------------------*/
// SET_MOD_TIMING_CMD_SLAM
#define SET_MOD_TIMING_CMD_SLAM_BMSK        0x0001
#define SET_MOD_TIMING_CMD_SLAM_SHFT        0

// SET_MOD_TIMING_CMD_PHCH
#define SET_MOD_TIMING_CMD_PHCH_BMSK        0x0007
#define SET_MOD_TIMING_CMD_PHCH_SHFT        0

// SET_MOD_TIMING_CMD_OFFSET
#define SET_MOD_TIMING_CMD_OFFSET_BMSK      0xFFFF
#define SET_MOD_TIMING_CMD_OFFSET_SHFT      0

// SET_MOD_TIMING_CMD_TTI
#define SET_MOD_TIMING_CMD_TTI_BMSK         0x0007
#define SET_MOD_TIMING_CMD_TTI_SHFT         0

#ifdef FEATURE_WCDMA_CPC_DTX
// SET_MOD_TIMING_CMD_CFN
#define SET_MOD_TIMING_CMD_CFN_BMSK         0x00FF
#define SET_MOD_TIMING_CMD_CFN_SHFT         0
#endif



typedef struct {
  /* whether or not to slam or slew in response to channel timing changes */
  uint16 slam;

  /* the hardware channel of the downlink physical channel that is used to
     establish the timing relation for the uplink channel */
  uint16 phCh;

  /* Absolute Tx PN position (DL DPCH timing + T0) */
  uint32 txPnPositioncx8;

  /* the TTI counter value that applies to the first frame when the channel
     is activated */
  uint16 tti;

  /* DL reference offset */
  int8 dl_ref_offset;

} set_mod_timing_cmd_parms_struct_type;


/*--------------------------------------------------------------------------
                STRUCTURE: UL_DPCCH_CFG_CMD_STRUCT_TYPE

This struture contains the parameters for the uplink DPCCH configuration
command.
--------------------------------------------------------------------------*/
// UL_DPCCH_CFG_CMD_ENA
#define UL_DPCCH_CFG_CMD_ENA_BMSK         0x0001
#define UL_DPCCH_CFG_CMD_ENA_SHFT         0

// UL_DPCCH_CFG_CMD_SLOT_FMT
#define UL_DPCCH_CFG_CMD_SLOT_FMT_BMSK    0x001F
#define UL_DPCCH_CFG_CMD_SLOT_FMT_SHFT    0

// UL_DPCCH_CFG_CMD_UNC_TFCI
#define UL_DPCCH_CFG_CMD_UNC_TFCI_BMSK    0x03FF
#define UL_DPCCH_CFG_CMD_UNC_TFCI_SHFT    0

typedef struct {
  /* enable or disable DPCCH processing */
  uint16 ena;

  /* dummy dpcch command to trigger DL in-sync Detection in FW */
  /* Set it to 1 for dummy DPCCH cmd and set it to 0 for real DPCCH cmd */
  uint16 inSyncDetect;

  /* slot format as given in 25.211 for the uplink DPCCH */
  uint16 slot_fmt;

  /* the uncoded TFCI bits to be used on the uplink DPCCH */
  uint16 uncoded_tfci;

  /* Wait for SCCH orders to enable secondary UL carrier
  WfwTxDcHsupaWaitScchOrderTypeEnum - 0:noop, 1: true, 2:false */
  uint8 waitonscchorder;

  /*! For DC->SC reconfig, disable only secondary carrier
  this bit is relevant only if en = 1. en = 0 disables both. */
  uint8 disableseccarrier;

} ul_dpcch_cfg_cmd_parms_struct_type;


/*--------------------------------------------------------------------------
                  STRUCTURE: UL_PRACH_CFG_CMD_STRUCT_TYPE

This struture contains the parameters for the uplink PRACH configuration
command.
--------------------------------------------------------------------------*/
// UL_PRACH_CFG_CMD_UNC_TFCI
#define UL_PRACH_CFG_CMD_UNC_TFCI_BMSK        0x03FF
#define UL_PRACH_CFG_CMD_UNC_TFCI_SHFT        0

// UL_PRACH_CFG_CMD_AVAIL_SIG
#define UL_PRACH_CFG_CMD_AVAIL_SIG_BMSK       0xFFFF
#define UL_PRACH_CFG_CMD_AVAIL_SIG_SHFT       0

// UL_PRACH_CFG_CMD_FIRST_SUB_CH
#define UL_PRACH_CFG_CMD_FIRST_SUB_CH_BMSK    0xF000
#define UL_PRACH_CFG_CMD_FIRST_SUB_CH_SHFT    12

// UL_PRACH_CFG_CMD_AVAIL_SUB_CH
#define UL_PRACH_CFG_CMD_AVAIL_SUB_CH_BMSK    0x0FFF
#define UL_PRACH_CFG_CMD_AVAIL_SUB_CH_SHFT    0

// UL_PRACH_CFG_CMD_INIT_TX_PWR
#define UL_PRACH_CFG_CMD_INIT_TX_PWR_BMSK     0xFFFF
#define UL_PRACH_CFG_CMD_INIT_TX_PWR_SHFT     0

// UL_PRACH_CFG_CMD_PRE_PWR_STEP
#define UL_PRACH_CFG_CMD_PRE_PWR_STEP_BMSK    0xFFFF
#define UL_PRACH_CFG_CMD_PRE_PWR_STEP_SHFT    0

// UL_PRACH_CFG_CMD_MSG_PWR_STEP
#define UL_PRACH_CFG_CMD_MSG_PWR_STEP_BMSK    0xFFFF
#define UL_PRACH_CFG_CMD_MSG_PWR_STEP_SHFT    0

// UL_PRACH_CFG_CMD_XT
#define UL_PRACH_CFG_CMD_XT_BMSK              0x8000
#define UL_PRACH_CFG_CMD_XT_SHFT              15

// UL_PRACH_CFG_CMD_PRE_MAX_RETRANS
#define UL_PRACH_CFG_CMD_PRE_MAX_RETRANS_BMSK 0x00FF
#define UL_PRACH_CFG_CMD_PRE_MAX_RETRANS_SHFT 0

//UL_PRACH_CFG_CMD_TTI
#define UL_PRACH_CFG_CMD_TTI_BMSK             0x0001
#define UL_PRACH_CFG_CMD_TTI_SHFT             0

typedef struct {
  /* Rach Action defined by wenc_rach_fw_action_enum_type */
  uint8 action;

  /* the uncoded TFCI bits used on the PRACH */
  uint16 unc_tfci;

  /* bitmask of available signatures for use on the PRACH */
  uint16 avail_sig;

  /* the first subchannel and available subchannel mask */
  uint16 sub_chan_info;

  /* the initial TX power at the start of the access preamble */
  uint16 init_tx_pwr;

  /* the power step size between successive preambles */
  uint16 pre_pwr_step;

  /* the power step size between the last preamble and the message */
  uint16 msg_pwr_step;

  /* the AICH transmission timing parameter and maximum preamble
     retransmission count */
  uint16 xt_and_max_preamble_retrans;

  /* the enum value of the PRACH TTI.  Note that this shares
     the definition with the downlink TTI's which have a
     larger range, i.e. 40ms, and 80ms.  Fortunately the
     10 and 20ms TTI's line up correctly with what the mDSP
     expects.   Phewwww! */
  uint16 tti;

  /*! @brief hsRachEaiCfged, 0->EAI for HS RACH proc is NOT cfg'ed, 1->EAI for HS RACH proc is cfg'ed*/
  uint32 hsRachEaiCfged;
  /*! @ (hsRachY +1) denotes the number of EDCH resources allocated for this cell, 0-31 */
  uint32 hsRachY;

  /* bitmask used by FW only for HS RACH cases for computation of SigInd */
  uint16 avail_cell_prach_sig_mask;
} ul_prach_cfg_cmd_parms_struct_type;

/*--------------------------------------------------------------------------
                  STRUCTURE: STATUS_DUMP_CMD_STRUCT_TYPE

This structure is a dummy for the mDSP status dump command.  There are no
parameters associated with the command.
--------------------------------------------------------------------------*/
typedef struct {
  /* just a dummy parameter to keep the compiler quiet. */
  uint16 dummy;
} status_dump_cmd_parms_struct_type;


/*--------------------------------------------------------------------------
                  STRUCTURE: STATUS_DUMP_RESULTS_STRUCT_TYPE

This structure is used to describe the format of the status dump results
output buffer.
--------------------------------------------------------------------------*/
#define STATUS_DUMP_REF_CNT_MSB_BMSK      0x0007
#define STATUS_DUMP_REF_CNT_LSB_BMSK      0xFFFF
#define STATUS_DUMP_TX_TIME_MSB_BMSK      0x003F
#define STATUS_DUMP_TX_TIME_LSB_BMSK      0xFFFF
#define STATUS_DUMP_FING_POS_MSB_BMSK     0x000F
#define STATUS_DUMP_FING_POS_LSB_BMSK     0xFFFF
#define STATUS_DUMP_DESKEW_ADDR_BMSK      0xFFFF

/* Not used. Commenting it for now */

/*--------------------------------------------------------------------------
                   STRUCTURE: WAKE_UP_CMD_STRUCT_TYPE

This structure is a dummy for the mDSP wake up command.  There are no
parameters associated with the command.
--------------------------------------------------------------------------*/
typedef struct {
  /* just a dummy parameter to keep the compiler quiet. */
  uint16 dummy;
} wake_up_cmd_parms_struct_type;


/*--------------------------------------------------------------------------
                     STRUCTURE: SLEEP_CMD_STRUCT_TYPE

This structure is a dummy for the mDSP go to sleep command.  There are
no parameters associated with this command.
--------------------------------------------------------------------------*/
typedef struct {
  /* just a dummy parameter to keep the compiler quiet. */
  uint16 dummy;
} sleep_cmd_parms_struct_type;

#ifdef FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE 

#define AGC_CMD_CONFIG_BMSK 0x8000
#define AGC_CMD_CONFIG_SHFT 15
#define AGC_CMD_START_BMSK 0x0001
#define AGC_CMD_START_SHFT 0

/*--------------------------------------------------------------------------
                  STRUCTURE: AGC_CMD_STRUCT_TYPE
This structure is for the mDSP  agc command.  description of the
parameters associated with the command are as follows.
 |15|14|13|12|11|10|9 |8 |7 |6 |5 |4 |3 |2 |1 |0 |         
1 |D0|                                                   |A0|         
  -------------------------------------------------------------         
2 |D1|                                                   |A1|          
  --------------------------------------------------------------          
  D1 D0:  Dirty1/Dirty0 bit                                 
  A1 A0:  Agc1/Agc0 Start/stop bit                            

  D1 D0     A1 A0                                                
   0 0        X X  -- do nothing                                 
   0 1        X 0  -- stop_agc0                                   
   0 1        X 1  -- start_agc0                                  
   1 0        0 X  -- stop_agc1                                   
   1 0        1 X  -- start_agc1                                  
   1 1        0 0  -- stop_dual_agc                                
   1 1        0 1  -- stop_agc1_start_agc0                         
   1 1        1 0  -- stop_agc0_start_agc1                         
   1 1        1 1  -- start_dual_agc        
--------------------------------------------------------------------------*/
typedef struct
{
  uint32 agc0_cfg;
  /*! @ brief RxLM buffer index */
  uint8 rxlm_buf_idx;
  /*! @ brief RxLM buffer index for Diversity if needed */
  uint8 rxlm_buf_idx_rxd;
  /*! @ brief start/stop mode indicator for DSR update 
      0: no action
      1: stop AGC first and then start AGC immediately */
  uint8 stop_start_flag;
  /*! @ brief RF linearity power mode indicator
      0: no change
      1: low linearity mode
      2: high linearity mode  */
  uint8 lin_mode;
  /*! @ brief Total time needed for the completion of SSBI execution + PLL tune time 
        SSBI includes two parts 
  1-  AutoB group of SSBIs (happen before PLL settling) 
  2-  AutoA group of SSBIs (happen in parallel with PLL settling) 
  This time is execTime(AutoB) + max(execTime(AutoA), Pll_settling_time)
  This time is ONLY used if rfWarmupFlag is set and is in units chipx8 */
  UWord16 rfWarmupTime;
  /*! @ brief Flag to indicate the AGC start during warmup */
  UWord8 rfWarmupFlag;

  /*! @ brief Rx calibration flag 
       0: Normal mode
       1: Rx Cal, do not start AGC power accumulations */
  UWord8 rxCal;
  /*! @ brief NBR feature enabling
       0: disabled
       1: enabled - nbr should be running as part of rxagc start cmd processing */
  uint8 nbrEnFlag;                 
  
  /*! @ brief rxlmBufIndex for modified G chain */
  uint8 nbrRxlmBufIndex;

} agc_cmd_parms_struct_type;
#else
/*--------------------------------------------------------------------------
                  STRUCTURE: START_AGC_CMD_STRUCT_TYPE

This structure is a dummy for the mDSP start agc command.  There are no
parameters associated with the command.
--------------------------------------------------------------------------*/
typedef struct {
  /* just a dummy parameter to keep the compiler quiet. */
  uint16 dummy;
} start_agc_cmd_parms_struct_type;
#endif

/*--------------------------------------------------------------------------
                    STRUCTURE: TEST_CMD_STRUCT_TYPE

This structure is a dummy for the mDSP test command.  There are no
parameters associated with the command
--------------------------------------------------------------------------*/
typedef struct {
  /* just a dummy parameter to keep the compiler quiet */
  uint16 dummy;
} test_cmd_parms_struct_type;

/*--------------------------------------------------------------------------
                    STRUCTURE: SLAM_COMBINER_CMD_STRUCT_TYPE

This structure is used to describe the format for the combiner slam command
--------------------------------------------------------------------------*/
#define SLAM_COMB_CMD_HW_CHAN_BMSK       0x0003
#define SLAM_COMB_CMD_HW_CHAN_SHFT           0

#define SLAM_COMB_CMD_COMB_LSW_BMSK      0xFFFF
#define SLAM_COMB_CMD_COMB_LSW_SHFT          0

#define SLAM_COMB_CMD_COMB_MSW_BMSK      0x0007
#define SLAM_COMB_CMD_COMB_MSW_SHFT          0

#define SLAM_FR_REF_COUNT_BMSK           0xFFFF
#define SLAM_FR_REF_COUNT_SHFT               0

typedef struct {
  uint16 hw_chan;
  uint16 combiner_count_modifier_lsw_Cx8;
  uint16 combiner_count_modifier_msw_Cx8;
  uint16 target_frame_reference_count_Cx1;
} slam_combiner_command_struct_type;

/*--------------------------------------------------------------------------
                    STRUCTURE: MDSP_CM_START_COMPRESSED_MODE_STRUCT_TYPE
--------------------------------------------------------------------------*/
typedef struct
{
  /** WTR index info **/
  uint8 wtrindex;
  /** DL Frame type */
  uint8 dl_frame_type;
  /** ITP bit */
  uint8 itp;
  /** RPP type */
  uint8 rpp;
  /** UL compressed mode method */
  uint8 ul_method;
  /** DL compressed mode method */
  uint8 dl_method;
  /** First compressed slot */
  uint8 first_slot;
  /** transmission gap length */
  uint8 gap_length;
  /** transmission gap measurement purpose */
  uint8 tgmp;
  /* the rxlm buffer index used for tuning away at the start of the gap */
  uint32 rxlm_setup_buf_idx;
  /* the rxlm buffer index used for tuning back the primary Rx chain at the end of the gap */
  uint32 rxlm_home_buf_idx;
  /* the rxlm buffer index used for tuning back the rxd chain at the end of the gap */
  uint32 rxlm_home_rxd_buf_idx;
  #ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  /* the frc frame number of the time when it's safe for WFW to start actions for end of gap */
  uint32 tune_back_time_fr;
  /* the frc frame offset (cx8) of the time when it's safe for WFW to start actions for end of gap */
  uint32 tune_back_time_offset;
  #endif
  /* gap start time in stmr */
  uint32 gap_action_time_in_stmr;
} mdsp_cm_start_compressed_mode_struct_type;

/*--------------------------------------------------------------------------
                    STRUCTURE: MDSP_CM_STEP1_SRCH_CMD_STRUCT_TYPE
--------------------------------------------------------------------------*/
typedef struct
{
  /** First Step1 gap */
  uint8 first;
  /** Last Step1 gap */
  uint8 last;
  /** Command Queue  */
  uint8 queue;
} mdsp_cm_step1_srch_cmd_parms_struct_type;

/*--------------------------------------------------------------------------
                STRUCTURE: MDSP_CM_PN_SRCH_CMD_STRUCT_TYPE
--------------------------------------------------------------------------*/
typedef struct
{
  uint8 qsel;
} mdsp_cm_pn_srch_cmd_parms_struct_type;

#define MDSP_DEMOD_NO_COMB_PARAMS 0
#define MDSP_DEMOD_NO_FING_PARAMS 0
#define MDSP_DEMOD_NO_CELL_PARAMS 0
#define MDSP_DEMOD_NO_PHCH_PARAMS 0
#define MDSP_DEMOD_NO_CME_PARAMS 0
#define MDSP_DEMOD_NO_COMMON_PARAMS 0
#define MDSP_DEMOD_NO_ARD_PARAMS 0

  /* Start FACH inter frequency meas */
typedef struct
{
  /* WTR index info */
  uint8 wtrindex;

  /** Combiner channel number/HW channel index **/
  uint8 chan_num;

  /** Start CFN of the gap */
  uint8 start_cfn;

  /** start slot of the gap */
  int8 start_slot;

  /** measurement period length in slots */
  uint8 gap_len_slots;

  /* Measurement purpose */
  uint16 tgmp;

  /* the rxlm buffer index used for tuning away at the start of the gap */
  uint32 rxlm_setup_buf_idx;

  /* the rxlm buffer index used for tuning back the primary Rx chain at the end of the fach search */
  uint32 rxlm_home_buf_idx;

  /* the rxlm buffer index used for tuning back the rxd chain at the end of the fach search */
  uint32 rxlm_home_rxd_buf_idx;

  /* gap start time in stmr */
  uint32 gap_action_time_in_stmr;

} mdsp_start_fach_meas_cmd_parms_struct_type;

typedef struct
{
  /** the rxlm buffer index used to tune back the primary Rx chain to WCDMA at
      the end of an idle w2x search */
  uint32 rxlm_home_buf_idx;
  /** the rxlm buffer index used to tune back the diversity Rx chain to WCDMA at
      the end of an idle w2x search */
  uint32 rxlm_home_rxd_buf_idx;
  /** determines if the search is w2l or w2g */
  mdsp_sync_w2x_idle_cmd_mode_enum_type mode;
} mdsp_sync_w2x_idle_cmd_parms_struct_type;


/* HS combiner channel setup command */
typedef struct
{
  /* config DB index */
  uint16 db_idx;
} mdsp_hs_comb_chan_cfg_cmd_parms_struct_type;

/* HS UL DPCCH channel setup command */
typedef struct
{
  /* Word 1 */

  /*! @brief HS TX Action - 0->Disable, 1->Enable, 2->Reslam w/timing change, 
      3->Reslam w/o timing change */
  uint32 action : 2;
  /*! @brief HS N/ACK Preamble Mode Enable (Range 0,1) */
  uint32 ackPmblEn : 1;
  /*! @brief HS CQI Feedback Factor (Range 1,2,4,5,10,20,40,80) */
  uint32 cqiFbackFactor : 8;
  /*! @brief HS CQI repetition factor (Range 1-4) */
  uint32 cqiRep : 3;
  /*! @brief HS N/ACK repetition factor (Range 1-4) */
  uint32 ackRep : 3;
  /*! @brief Offset in BPG between DPCCH frame boundary and first HS-DPCCH 
      subframe contained in that frame (Range 0-29) */
  uint32 dpcchOffset : 8;
  /*! @brief HS MIMO enum representation the ratio M_cqi_type_A/M_cqi */
  uint32 hsMimoNcqiTypeAMcqiRatio : 4;
  /*! @brief HS MIMO is enabled flag */
  uint32 hsMimoIsEnabled : 1;
  /*! @brief UNUSED */
  UWord32 reserved_0 : 2;

  /* Word 2 */

   /*! @brief HS DB Idx for control table and log packet. This is same as index
      passed in UL HS-DPCCH cfg: 0-1 = primary serving cell, 2-3=secondary 
      serving cell */
  uint32 dbIdx : 3;
  /*! @brief Initial HS DPCCH global subframe number, valid if enable or reslam 
      (Range 0-1279) */ 
  uint32 initGsfn : 11;
  /*! @brief Final HS DPCCH global subframe number, valid if reslam or disable 
      (Range 0-1279) */ 
  uint32 finalGsfn : 11;
  /*! @brief Number of subframes to be corrected at CFN rollover to compute MIMO CQI type */
  uint32 numSfnCorrectedCFNRollover : 3;
  /*! @brief HS DC is enabled flag */
  UWord32 hsSecondaryCellEnabled : 2;
  /*! @brief UNUSED */
  uint32 reserved : 2;
} mdsp_hs_ul_chan_cfg_cmd_parms_struct_type;



  /* EUL chan config command */
typedef struct
{
  boolean                       ir;
  e_tti_enum_type               tti;
  e_ul_chan_action_enum_type    action;
  uint16                        start_sub_fn;
  uint16                        final_sub_fn;
  boolean                       dirty_bit;
  e_ul_chan_action_enum_type    action_sec;
  uint16                        start_sub_fn_sec;
  uint16                        final_sub_fn_sec;
  boolean                       dirty_bit_sec;
}  mdsp_e_ul_chan_cfg_cmd_parms_struct_type;

/*use the same structure for both downlink and uplink channel configuration cmd*/
typedef mdsp_e_ul_chan_cfg_cmd_parms_struct_type mdsp_e_dl_chan_cfg_cmd_parms_struct_type;

/* HS RACH JUDC config command*/
#ifdef FEATURE_WCDMA_HS_RACH

typedef enum{
  /*! JUDC diable command */
  MDSP_JUDC_CFG_DISABLE = 0,
  /*! JUDC enable command */
  MDSP_JUDC_CFG_ENABLE,
  /*! FACH to DCH seamless transition */
  MDSP_JUDC_CFG_SEAMLESS_TRANS,
  MDSP_JUDC_CFG_NUM_CFG
}mdsp_hsrach_judc_action_enum_type;

typedef struct
{
  mdsp_hsrach_judc_action_enum_type hs_rach_enable_UL;
  mdsp_hsrach_judc_action_enum_type hs_rach_enable_DL;
  uint32 phychan_bmsk;
  uint32 comb_bmsk;
  boolean ul_dpcch_cfg_present;
  boolean e_ul_cfg_present;
  mdsp_e_ul_chan_cfg_cmd_parms_struct_type e_ul_cfg_info;
  ul_dpcch_cfg_cmd_parms_struct_type ul_dpcch_cfg_info;
}  mdsp_judc_cfg_cmd_parms_struct_type;
#endif /*FEATURE_WCDMA_HS_RACH*/

typedef struct
{
  /** cell bmsk that needs to be configured */
  uint16 cell_bmsk; 
  /** fing bmsk that needs to be configured */
  uint32 fing_bmsk;
  /** combiner bmsk that needs to be configured */
  uint16 comb_bmsk;
  /** phychan bmsk that needs to be configured */
  uint32 phychan_bmsk;
  /** CME (EQ) cfg bmsk that needs to be configured  */
  uint32 cme_bmsk;
  /** CME Common cfg bmsk that needs to be configured */
  uint32 cmn_bmsk;
  /** ARD cfg bmsk that needs to be configured */
  uint32 ard_bmsk;
} mdsp_offline_demod_cfg_cmd_struct_type;

#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
typedef struct
{
  /*! Demod config update command info */
  mdsp_offline_demod_cfg_cmd_struct_type demodCfg;

} mdsp_sync_ant_switch_cmd_params_struct_type;
#endif

typedef struct
{
  /** boolean to indicate whether QICE HW parameters should be overridden */
  uint8 hw_prog_bmsk;
  /** boolean to indicate whether QICE FW parameters should be overridden */
  uint8 fw_config_bmsk;
  /** boolean to indicate whether QICE test bus parameters should be overridden */
  uint8 test_bus_bmsk;
  /** boolean to indicate whether QICE SFC log parameters should be overridden */
  uint8 sfc_log_config_bmsk;
  /** boolean to indicate whether QICE F & Finv log parameters should be overridden */
  uint8 ffinv_log_config_bmsk;
  /** bitmask which indicates which EQ cell's QICE FHT parameters should be overridden */
  uint8 fht_prog_bmsk;
  /** bitmask which indicates which EQ cell's QICE EQ config parameters should be overridden */
  uint8 eq_config_bmsk;
  /** bitmask which indicates which EQ cell's QICE params should be reset */
  uint8 eq_reset_bmsk;
  /** bitmask which indicates which EQ cell's SFC values should be reset */
  uint8 sfc_reset_bmsk;
} mdsp_oride_qice_cfg_cmd_struct_type;

typedef struct
{
  uint16 cctrch_dirty_bmsk;
  boolean interrupt_reqd;
} mdsp_cctrch_dirty_bmsk_struct_type;

#ifdef FEATURE_WCDMA_SEND_SS_FREEZE_UNFREEZE_TO_FW
typedef struct
{
  /** boolean to indicate whether a freeze is needed */
  uint8 freeze; 
  /** boolean to indicate whether an unfreeze is needed */
  uint8 unfreeze;
  /** boolean to indicate whether sample server 0 (SS0) is affected */
  uint8 chain0;
  /** boolean to indicate whether SS1 is affected */
  uint8 chain1;
  /* Indicates the action to be taken with RTR after sample server cmd action time.*/
  uint8 rfPwrUpdateFlag;
  /* Indicates the STMR Cx1 value for the action time for the freeze/unfreeze.*/
  uint16 actionTime;
} mdsp_sample_server_cfg_cmd_struct_type;
#endif

/* Clock configuration */
typedef struct
{
  uint16 clk_mode;
} mdsp_clk_cfg_cmd_parms_struct_type;

typedef struct
{
  /*! @Brief Per slot Srch Cycles */
  uint32 srch_cycles;
} mdsp_srch_cycle_cmd_struct_type;

   
#if defined(FEATURE_WCDMA_CPC_DRX) || defined(FEATURE_WCDMA_CPC_DTX)
typedef struct
{
  /** cmd type 0-DTX 1-DRX    */
  uint16 cmd_type;
  /** cmd action 0 - disable, 1-enable, 2-reconfig */
  uint16 cmd_action;
  /** action time cfn */
  uint16 cmd_action_cfn;
  /** drx state */
  boolean wait_on_scch_order;
}mdsp_sync_cpc_drx_dtx_cfg_cmd_struct_type;
#endif

/* struct containing parameters for an RF tune command*/
typedef struct
{
  /* determines the buffer index of the tuneback script to use (valid 0-3) */
  uint8 buf_idx;
  /* Trigger time (cx1) - Timed Range 0:38399, for immediate time: 0xFFFF */
  uint16 trigger_time;
} mdsp_rf_tune_cmd_struct;
 
/*mDSP TxLM config cmd length*/
#define MDSP_TXLM_CFG_CMD_LEN 2
#define MDSP_TXLM_UPDATE_CMD_LEN 1
typedef struct {

  /*TxLM mode 1: enable, 0:disable*/
  boolean mode_enable;

  /* TxLM buffer index */
  uint32 buf_idx;

} mdsp_sync_txlm_cfg_cmd_parms_struct_type;

typedef struct {

  /* TxLM buffer index */
  uint32 buf_idx;

} mdsp_sync_txlm_update_cmd_parms_struct_type;

typedef struct 
{
  /* Index for ADC based TQ config */
  uint32 pri_adc_index;
  /* Secondary index for DBDC scenarios */
  uint32 sec_adc_index;
} mdsp_sync_adc_config_struct_type;

typedef struct 
{
  /* Whether to do dac cal on ant 0 */
  boolean ant0;
  /* Whether to do dac cal on ant 1 */
  boolean ant1;
 }mdsp_sync_dal_cal_cmd_params_struct_type;

typedef struct
{
  uint32 rxlmBufferId;
  uint32 chain;
  uint32 startTime;
  uint16 carrier;
  uint16 numSamples;
} mdsp_start_iq_capture_type;

typedef struct
{
  uint32 event_id;
  uint32 offset;
} mdsp_rf_init_cmd_type;

typedef struct
{
  uint8 rxlmBufferId[2]; /* for both antenna chains*/
  uint32 rxlmDynGroupMask; /*rxlm dynamic update to configure the rxfE ici coeff*/
  uint32 rxAntennaMask;/*(1: Rx0, 2:Rx1, 3: both Rx)*/
} mdsp_rf_rxlm_dyn_update_cmd_type;

typedef struct
{
  uint32 event_id;
  uint32 offset;
} mdsp_rf_cfg_cmd_type;

typedef struct
{
  uint32 event_id;
  uint32 offset;
} mdsp_rf_update_cmd_type;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)

/*Structure to support QTA Start Stop commands to WFW*/
typedef struct
{
  /* RF home buffer index (used for tuning back) */
  uint16 rf_home_buf_idx;
  /* the rxlm buffer index used for tuning back the primary Rx chain at the end of the gap */
  uint32 rxlm_home_buf_idx;
  /* the rxlm buffer index used for tuning back the rxd chain at the end of the gap */
  uint32 rxlm_home_rxd_buf_idx;
  /* Reference phch index for Quick Tune Away start */
  uint32 ref_ph_ch_idx;
  /* Soft Buffer flush indicator for quick tune away stop */
  uint32 soft_buf_flush;
  /*Command type indicating Start/Stop QTA*/
  uint16 cmd_type;
  /* Field to indicate if this was an empty QTA gap, 
  ** WFW will check this field only when processing stop QTA cmd */
  uint16 is_empty_qta_gap;
  /*Field to indicate support of Partial QBTA based QTA,
  ** WFW will check this field only when processing Start QTA cmd*/
  uint16 is_qbta_supported;
} mdsp_sync_qta_cfg_struct_type;
#endif /* FEATURE_QTA */

#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
/*Device Mode Preference enum*/
typedef enum
{
  WL1_SINGLE_SIM_SINGLE_STANDBY_MODE,
  WL1_DUAL_SIM_DUAL_STANDBY_MODE,
  WL1_DUAL_SIM_DUAL_ACTIVE_MODE,
  WL1_INVALID_MODE
} wl1_mode_pref_enum_type;

/*New struct which enables WL1 in sending Mode pref as part of
**WFW_START_WFW_CMD_ID, Should be able to do away with this piece of code
**Once WFW is able to support a command for runtime indication mode pref*/
typedef struct
{
  mdsp_sync_start_cfg_enum_type start_cfg;
  wl1_mode_pref_enum_type mode_pref;
} mdsp_sync_cxm_start_cfg_struct_type;
#endif /* FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW */

/*--------------------------------------------------------------------------
                       UNION: MDSP_CMD_UNION_TYPE

This is a union of the various mDSP command structures.
--------------------------------------------------------------------------*/
typedef union {
  /* search command, applies for all search commands except abort */
  srch_cmd_parms_struct_type srch;

  /* downlink physical channel configuration command */
  dl_phychan_cfg_cmd_parms_struct_type dl_phychan;

  /* uplink set timing relation command */
  set_mod_timing_cmd_parms_struct_type mod_timing;

  /* uplink DPCCH configuration command */
  ul_dpcch_cfg_cmd_parms_struct_type ul_dpcch;

  /* uplink PRACH configuration command */
  ul_prach_cfg_cmd_parms_struct_type ul_prach;

  /* status dump command */
  status_dump_cmd_parms_struct_type status_dump;

  /* mDSP wake up command */
  wake_up_cmd_parms_struct_type wake_up;

  /* mDSP sleep command */
  sleep_cmd_parms_struct_type sleep;

#ifdef FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE 
  WfwRxAgcCmdCfgStruct agc_cmd;
#else
  /* mDSP start agc command */
  start_agc_cmd_parms_struct_type start_agc;
#endif

#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
  /*start config*/
  /*New struct which enables WL1 in sending Mode pref as part of
  **WFW_START_WFW_CMD_ID, Should be able to do away with this piece of code
  **Once WFW is able to support a command for runtime indication of mode pref*/
  mdsp_sync_cxm_start_cfg_struct_type start_cxm_cfg;
#else
  /*start config*/
  mdsp_sync_start_cfg_enum_type start_cfg;
#endif /* FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW */

  /* mdsp slam combiner command */
  slam_combiner_command_struct_type slam_combiner;

  /* mDSP test command */
  test_cmd_parms_struct_type test;

  /* CM init command */
  mdsp_cm_start_compressed_mode_struct_type start_compressed_mode;

  /* CM inter frequency step 1 search */
  mdsp_cm_step1_srch_cmd_parms_struct_type cm_step1_srch;

  /* CM inter frequency PN search */
  mdsp_cm_pn_srch_cmd_parms_struct_type cm_pn_srch;

  mdsp_cctrch_dirty_bmsk_struct_type cctrch_dirty_bmsk_info;

  /* Start FACH inter frequency meas */
  mdsp_start_fach_meas_cmd_parms_struct_type start_fach_meas;

  mdsp_sync_w2x_idle_cmd_parms_struct_type idle_mode_w2x;

  /* HS combiner chan config command */
  mdsp_hs_comb_chan_cfg_cmd_parms_struct_type hs_comb_chan_cfg;

  /* HS UL DPCCH chan config command */
  mdsp_hs_ul_chan_cfg_cmd_parms_struct_type hs_ul_chan_cfg;

  /* EUL combiner chan config command */
  mdsp_e_dl_chan_cfg_cmd_parms_struct_type  e_dl_comb_chan_cfg;

  /* EDPCH chan config command */
  mdsp_e_ul_chan_cfg_cmd_parms_struct_type  e_ul_chan_cfg;

/* HS RACH config command*/
#ifdef FEATURE_WCDMA_HS_RACH
  mdsp_judc_cfg_cmd_parms_struct_type hs_rach_judc_cfg;
#endif/*FEATURE_WCDMA_HS_RACH*/
  mdsp_offline_demod_cfg_cmd_struct_type offline_demod_cfg;

#ifdef FEATURE_WCDMA_SEND_SS_FREEZE_UNFREEZE_TO_FW
  /* Sample server config command */
  mdsp_sample_server_cfg_cmd_struct_type ss_cfg;
#endif

  mdsp_clk_cfg_cmd_parms_struct_type clk_cfg;

  mdsp_srch_cycle_cmd_struct_type srch_cycle_cmd;

  mdsp_oride_qice_cfg_cmd_struct_type oride_qice_cfg;

#if defined(FEATURE_WCDMA_CPC_DRX) || defined(FEATURE_WCDMA_CPC_DTX)
  /** drx cmd param struct */
  mdsp_sync_cpc_drx_dtx_cfg_cmd_struct_type cpc_drx_dtx_cfg;
#endif

#ifdef FEATURE_WCDMA_CPC_DTX
  e_ul_cpc_dtx_cfg_db_struct_type e_ul_cpc_dtx_cfg;
#endif /* FEATURE_WCDMA_CPC_DTX*/

 mdsp_rf_tune_cmd_struct rf_tune_cfg;
 mdsp_start_iq_capture_type   start_iq_cfg;
 mdsp_sync_txlm_cfg_cmd_parms_struct_type txlm_cfg_cmd_parms;
 mdsp_sync_txlm_update_cmd_parms_struct_type txlm_update_cmd_parms;

 mdsp_sync_adc_config_struct_type rf_adc_tq_config;
WfwStartSaptIqCaptureCmdStruct IQ_sample_capture_cmd_parms;
WfwEptCalApplyDpdCmdStruct apply_dpd_cmd_params;
WfwEptSampleBufferCfgCmdStruct tlb_mem_map_parms;
mdsp_sync_dal_cal_cmd_params_struct_type tx_dac_cal_cmd_params;

  mdsp_rf_init_cmd_type rf_init_params;
  mdsp_rf_cfg_cmd_type  rf_cfg_params;
  mdsp_rf_update_cmd_type  rf_update_params;
  mdsp_rf_rxlm_dyn_update_cmd_type rf_dyn_update_params;

#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
  /* Structure used for the antenna switch command */
  mdsp_sync_ant_switch_cmd_params_struct_type ant_switch_cmd_params;
#endif

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
  /* Start/Stop QTA command */
  mdsp_sync_qta_cfg_struct_type  qta_cfg;
#endif /* FEATURE_QTA */

#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
  /*CxM command types*/
  WfwCxmParamCfgCmdStruct cxm_cfg;
#endif /* FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW */

} mdsp_sync_cmd_parms_union_type;


/*--------------------------------------------------------------------------
                    STRUCTURE: MDSP_SYNC_CMD_TYPE

This structure is used for sending commands to the mDSP via the L1 sync
interface.
--------------------------------------------------------------------------*/
typedef struct {

  /* Link for the queue routines to manage this item on the queue */
  q_link_type link;

  /* Index of the entry */
  uint16 index;

  /* which command is being sent */
  WfwSwCmdEnum cmd_title;

  /* length of the parameter block associated with this command */
  uint16 cmd_len;

  /* cfn */
  uint16 cfn; 

  /*frc frame num*/
  uint16 frame_num_cnt;

  /* frc cx1 count */
  uint16 frame_cx1_cnt;

  /* timetick count */
  uint32 timetick;

  /* cmd parameters to be put in the the data area of the buffer */
  mdsp_sync_cmd_parms_union_type parms;

  /* This is a callback function which will be called by the L1 Sync
     module before delivering the command to the DSP. This can be used by
     the calling function to initialize the time criticial DSP
     resources *just* before writing the command to the DSP. */
  pre_cb_fn pre_proc_callback;

  /* This callback function will be executed after the command submitted
     to the DSP is executed and CMD DONE Interrupt is asserted. The L1 Sync
     module client will register a function to this callback which can  do post
     processing such as reading/shipping data from the DSP or signalling
     a task about the command execution. */
  post_cb_fn post_proc_callback;

  /* indicate this command is suspended */
  boolean suspended;
} mdsp_sync_cmd_type;

typedef enum
{
  MDSP_TO_ARM_INT_WL1_COMMAND_DONE,
  MDSP_TO_ARM_INT_WL1_SEARCH_DONE,
  MDSP_TO_ARM_INT_STATUS_DUMP_DONE,
  MDSP_TO_ARM_INT_DEMOD_CFG_DONE,
  MDSP_TO_ARM_INT_WL1_PICH_DONE,
  MDSP_TO_ARM_INT_WL1_LPQ_DONE,
  MDSP_TO_ARM_INT_WL1_HPQ_DONE,
  MDSP_TO_ARM_INT_WL1_DECODE_DONE,
 #ifdef FEATURE_WCDMA_DC_HSDPA
   /* SCCH orders */
  MDSP_TO_ARM_SCCH_ORDER,
  MDSP_TO_ARM_RXFE_START_INT,
 #endif
  MDSP_TO_ARM_INT_WL1_RACH_DONE,
  MDSP_TO_ARM_INT_WL1_CM_SEARCH_DONE,
  MDSP_TO_ARM_INT_WL1_OTDOA_SEARCH_DONE,
  MDSP_TO_ARM_INT_WL1_HDET_DONE,
  MDSP_TO_ARM_INT_WL1_FW_CRASH_DONE,
  #ifdef FEATURE_WCDMA_CPC_DRX
  MDSP_TO_ARM_INT_SCCH_DEC_DONE,
  #endif
  MDSP_TO_ARM_INT_WL1_AGC_TUNE_DONE,
  MDSP_TO_ARM_INT_WL1_AGC_CFG_DONE,
  MDSP_TO_ARM_INT_WL1_SS_FREEZE_UNFREEZE_DONE,
  MDSP_TO_ARM_INT_WL1_QXT_DONE,
  MDSP_TO_ARM_INT_HSRACH_FDPCH_SETUP_DONE,
  MDSP_TO_ARM_INT_UL_DPCCH_DISABLE_DONE,
  MDSP_TO_ARM_INT_TXD_DET_DONE,
  MDSP_TO_ARM_INT_SHADOW_CELL_REQ_DONE,
  #ifdef FEATURE_WCDMA_DC_HSUPA 
  MDSP_TO_ARM_INT_DC_HSUPA_RF_RETUNE_INDICATION,
  #endif
  #ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
  MDSP_TO_ARM_INT_ANTENNA_SWITCH_DONE,
  #endif
  MDSP_TO_ARM_INT_CME_CFG_PROC_DONE,
  MDSP_TO_ARM_INT_CCTRCH_UPDATE_DONE_DONE,
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
  MDSP_TO_ARM_INT_QTA_CMD_DONE,
#endif
  MDSP_TO_ARM_INT_DEMOD_R99_CHAN_CFG_DONE,
  MDSP_TO_ARM_INT_DEMOD_EDL_CHAN_CFG_DONE,
  MDSP_TO_ARM_INT_DEMOD_HS_CHAN_CFG_DONE,
  MDSP_TO_ARM_INT_DEMOD_HS_DPCCH_CHAN_CFG_DONE,
  MDSP_TO_ARM_INT_CPC_DTX_CFG_DONE,
  MDSP_TO_ARM_INT_CPC_DRX_CFG_DONE,
  MDSP_TO_ARM_INT_WL1_OVERLAPPED_SEARCH_DONE,
  /*MDSP has completed Narrowband rejection algo and indicating result to sw*/
  MDSP_TO_ARM_INT_WL1_NBR_PROC_DONE,

  MDSP_TO_ARM_INT_CPCDRX_POWER_DOWN_IND,
  MDSP_TO_ARM_INT_CPCDRX_POWER_UP_IND,
  #ifdef FEATURE_WCDMA_MDSP_RECOVERY
  /* Interrupt for MDSP Recovery. */
  MDSP_TO_ARM_INT_TRIGGER_MDSP_RECOVERY,
  #endif
  MDSP_TO_ARM_INT_WL1_NUM_EVENTS
} mdsp_wcdma_l1_event_type;

/* callback function pointer */
typedef void MDSP_WCDMA_L1_EVENT_CB_FUNC_TYPE(void);


/* Double buffer addresses */
#define MDSP_E_DL_COMB_CHAN_CFG_DB0 (MDSP_eulCombCfgParamDB0)
#define MDSP_E_DL_COMB_CHAN_CFG_DB1 (MDSP_eulCombCfgParamDB1)

#define MDSP_EDPCH_CHAN_CFG_DB0     (MDSP_edpchCfgDb0)
#define MDSP_EDPCH_CHAN_CFG_DB1     (MDSP_edpchCfgDb1)

#define MDSP_E_DL_COMB_CHAN_CFG_DB0_IDX 0
#define MDSP_E_DL_COMB_CHAN_CFG_DB1_IDX 1

/*mDSP AGCH config db size */
#define   MDSP_AGCH_CFG_DB_LEN                      9

/*mDSP RGCH-HICH config db size */
#define   MDSP_RGCH_HICH_CFG_DB_LEN           15

/*mDSP EUL dl comb channel config db size */
#define   MDSP_E_DL_COMB_CHAN_CFG_LEN   (MDSP_AGCH_CFG_DB_LEN+EUL_MAX_RL*MDSP_RGCH_HICH_CFG_DB_LEN)

/*mDSP EUL dl comb channel config cmd length */
#define   MDSP_E_DL_COMB_CHAN_CFG_CMD_LEN    1

/*bit location of config db Index of configuration parameter table- 
 FW HLD:Table/Figure 2 2 */
#define EUL_DL_COMB_CHAN_CFG_DB_IDX_BMSK            0x0001
#define EUL_DL_COMB_CHAN_CFG_DB_IDX_SHFT            0

/* The following are the fields in the AGCH configuration information 
   required by mDSP-FW HLD:Figure 2 3 */

/*This bit indicates the action for AGCH is changed or not.
 -bit number 15 in row 0*/
#define EUL_AGCH_CFG_DB_NEW_CFG_BMSK                0x8000
#define EUL_AGCH_CFG_DB_NEW_CFG_REG                 0
#define EUL_AGCH_CFG_DB_NEW_CFG_SHFT                15

/* These bits indicate the Action of the AGCH Config command 
 - bits 0-1 in row 0*/
#define EUL_AGCH_CFG_DB_ACTION_BMSK                 0x0003
#define EUL_AGCH_CFG_DB_ACTION_REG                  0
#define EUL_AGCH_CFG_DB_ACTION_SHFT                 0

/* This bit indicates the TTI of the AGCH channel.- bit number 15 in row 1*/
#define EUL_AGCH_CFG_DB_TTI_BMSK                    0x8000
#define EUL_AGCH_CFG_DB_TTI_REG                     1
#define EUL_AGCH_CFG_DB_TTI_SHFT                    15

/* These bits indicate Reference finger used for calculating AGCH timing
  (combiner timing) .bits 0-3 in row 1*/
#define EUL_AGCH_CFG_DB_REF_FINGER_BMSK             0x000F
#define EUL_AGCH_CFG_DB_REF_FINGER_REG              1
#define EUL_AGCH_CFG_DB_REF_FINGER_SHFT             0

/* This bit indicates the presence of STTD mode- bit number 15 in row 2*/
#define EUL_AGCH_CFG_DB_STTD_BMSK                   0x8000
#define EUL_AGCH_CFG_DB_STTD_REG                    2
#define EUL_AGCH_CFG_DB_STTD_SHFT                   15

/* These bits indicate OVSF code .bits 0-8 in row 2*/
#define EUL_AGCH_CFG_DB_OVSF_CODE_BMSK              0x01FF
#define EUL_AGCH_CFG_DB_OVSF_CODE_REG               2
#define EUL_AGCH_CFG_DB_OVSF_CODE_SHFT              0

/* These bits indicate  the cell in the combiner cfg dB.-bits 0-2 in row 3*/
#define EUL_AGCH_CFG_DB_CELL_IDX_BMSK          0x0007
#define EUL_AGCH_CFG_DB_CELL_IDX_REG           3
#define EUL_AGCH_CFG_DB_CELL_IDX_SHFT          0

/* These bits indicate Combiner number of DPCH channel - bits 8-10 in row 4*/
#define EUL_AGCH_CFG_DB_DPCH_CHNUM_BMSK            0x0700
#define EUL_AGCH_CFG_DB_DPCH_CHNUM_REG             4
#define EUL_AGCH_CFG_DB_DPCH_CHNUM_SHFT            8

/* These bits indicate the offset into the DPCH Channel's timeline 
 - bits 0-7 in row 4*/
#define EUL_AGCH_CFG_DB_DPCH_OFFSET_AGCH_BMSK      0x00FF
#define EUL_AGCH_CFG_DB_DPCH_OFFSET_AGCH_REG       4
#define EUL_AGCH_CFG_DB_DPCH_OFFSET_AGCH_SHFT      0

/* These bits indicate Starting CFN number (10msec TTI) or  starting subframe
  number (2msec TTI) of AGCH- - bits 0-10  in row 5*/
#define EUL_AGCH_CFG_DB_START_SUB_FRAME_NUM_BMSK       0x07FF
#define EUL_AGCH_CFG_DB_START_SUB_FRAME_NUM_REG        5
#define EUL_AGCH_CFG_DB_START_SUB_FRAME_NUM_SHFT       0

/* These bits indicate Last CFN/subframe of the present configuration.
  - bits 0-10  in row 6*/
#define EUL_AGCH_CFG_DB_FINAL_SUB_FRAME_NUM_BMSK       0x87FF
#define EUL_AGCH_CFG_DB_FINAL_SUB_FRAME_NUM_REG        6
#define EUL_AGCH_CFG_DB_FINAL_SUB_FRAME_NUM_SHFT       0

/* These bits indicate  primary E-RNTI  allocated - bits 0-15  in row 7*/
#define EUL_AGCH_CFG_DB_PRIMARY_ERNTI_BMSK         0xFFFF
#define EUL_AGCH_CFG_DB_PRIMARY_ERNTI_REG          7
#define EUL_AGCH_CFG_DB_PRIMARY_ERNTI_SHFT         0

/* These bits indicate  secondary E-RNTI  allocated - bits 0-15  in row 8*/
#define EUL_AGCH_CFG_DB_SECONDARY_ERNTI_BMSK       0xFFFF
#define EUL_AGCH_CFG_DB_SECONDARY_ERNTI_REG        8
#define EUL_AGCH_CFG_DB_SECONDARY_ERNTI_SHFT       0

/* The following are the fields in the RGCH/HICH configuration information
   required by mDSP-FW HLD:Figure 2 3 */

/*This bit indicates the action for RGCH/HICH is changed or not.
 -bit number 15 in row 0*/
#define EUL_RGCH_HICH_CFG_DB_HICH_NEW_CFG_BMSK           0x8000
#define EUL_RGCH_HICH_CFG_DB_HICH_NEW_CFG_REG            0
#define EUL_RGCH_HICH_CFG_DB_HICH_NEW_CFG_SHFT           15

/* These bits indicate the Action of the RGCH/HICH Config command 
- bits 0-1 in row 0*/
#define EUL_RGCH_HICH_CFG_DB_HICH_ACTION_BMSK            0x0003
#define EUL_RGCH_HICH_CFG_DB_HICH_ACTION_REG             0
#define EUL_RGCH_HICH_CFG_DB_HICH_ACTION_SHFT            0

/* This bit indicates the TTI of the RGCH/HICH channel.
- bit number 15 in row 1*/
#define EUL_RGCH_HICH_CFG_DB_TTI_BMSK               0x8000
#define EUL_RGCH_HICH_CFG_DB_TTI_REG                1
#define EUL_RGCH_HICH_CFG_DB_TTI_SHFT               15

/* These bits indicate Reference finger used for calculating RGCH/HICH
  timing (combiner timing) for a given cell. .bits 0-3 in row 1*/
#define EUL_RGCH_HICH_CFG_DB_REF_FINGER_BMSK        0x000F
#define EUL_RGCH_HICH_CFG_DB_REF_FINGER_REG         1
#define EUL_RGCH_HICH_CFG_DB_REF_FINGER_SHFT        0

/* This bit indicates the presence of STTD mode- bit number 15 in row 2*/
#define EUL_RGCH_HICH_CFG_DB_STTD_BMSK              0x8000
#define EUL_RGCH_HICH_CFG_DB_STTD_REG               2
#define EUL_RGCH_HICH_CFG_DB_STTD_SHFT              15

/* These bits indicate OVSF code .bits 0-8 in row 2*/
#define EUL_RGCH_HICH_CFG_DB_OVSF_CODE_BMSK         0x01FF
#define EUL_RGCH_HICH_CFG_DB_OVSF_CODE_REG          2
#define EUL_RGCH_HICH_CFG_DB_OVSF_CODE_SHFT         0

/* These bits indicate  the cell in the combiner cfg dB.-bits 0-2 in row 3*/
#define EUL_RGCH_HICH_CFG_DB_CELL_IDX_BMSK     0x0007
#define EUL_RGCH_HICH_CFG_DB_CELL_IDX_REG      3
#define EUL_RGCH_HICH_CFG_DB_CELL_IDX_SHFT     0

/* These bits indicate Combiner number of DPCH channel - bits 8-10 in row 4*/
#define EUL_RGCH_HICH_CFG_DB_DPCH_CHNUM_BMSK        0x0700
#define EUL_RGCH_HICH_CFG_DB_DPCH_CHNUM_REG         4
#define EUL_RGCH_HICH_CFG_DB_DPCH_CHNUM_SHFT        8

/* These bits indicate the offset into the DPCH Channel's timeline 
 - bits 0-7 in row 4*/
#define EUL_RGCH_HICH_CFG_DB_DPCH_OFFSET_HICH_BMSK   0x00FF
#define EUL_RGCH_HICH_CFG_DB_DPCH_OFFSET_HICH_REG    4
#define EUL_RGCH_HICH_CFG_DB_DPCH_OFFSET_HICH_SHFT   0

/* These bits indicate Starting CFN number (10msec TTI) or the starting 
 subframe number (2msec TTI) of  HICH- - bits 0-10  in row 5*/
#define EUL_RGCH_HICH_CFG_DB_HICH_START_SUB_FRAME_NUM_BMSK 0x07FF
#define EUL_RGCH_HICH_CFG_DB_HICH_START_SUB_FRAME_NUM_REG  5
#define EUL_RGCH_HICH_CFG_DB_HICH_START_SUB_FRAME_NUM_SHFT 0

/* These bits indicate last CFN number (10msec TTI) or the last subframe 
  number (2msec TTI) of current HICH configuration- bits 0-10 in row 6*/
#define EUL_RGCH_HICH_CFG_DB_HICH_FINAL_SUB_FRAME_NUM_BMSK 0x87FF
#define EUL_RGCH_HICH_CFG_DB_HICH_FINAL_SUB_FRAME_NUM_REG  6
#define EUL_RGCH_HICH_CFG_DB_HICH_FINAL_SUB_FRAME_NUM_SHFT 0

/*This is an index into the 25.211-6.70 -table 16B which helps with the
 selections of the Hardmard sequence per slot . - bits 0-5  in row 7*/
#define EUL_RGCH_HICH_CFG_DB_HICH_SIG_SEQ_IDX_BMSK     0x003F
#define EUL_RGCH_HICH_CFG_DB_HICH_SIG_SEQ_IDX_REG      7
#define EUL_RGCH_HICH_CFG_DB_HICH_SIG_SEQ_IDX_SHFT     0

/* This bit indicates time offset of the physical channel  from the reference
  finger. In case of serving cell tau_rgch =tau_hich while in non-serving cell
  tau_rgch becomes a constant offset of 5120chips.The value is in units of 
  256chips. Ex: if tau_hich is 51200chips SW will program 51200/256=200.
-bits 0-15 in row 8*/
#define EUL_RGCH_HICH_CFG_DB_TAU_BMSK               0xFFFF
#define EUL_RGCH_HICH_CFG_DB_TAU_REG                8
#define EUL_RGCH_HICH_CFG_DB_TAU_SHFT               0

/*This bit indicates the action for RGCH is changed or not.
 -bit number 15 in row 9*/
#define EUL_RGCH_HICH_CFG_DB_RGCH_NEW_CFG_BMSK           0x8000
#define EUL_RGCH_HICH_CFG_DB_RGCH_NEW_CFG_REG            9
#define EUL_RGCH_HICH_CFG_DB_RGCH_NEW_CFG_SHFT           15

/* These bits indicate the Action of the RGCH Config command 
- bits 0-1 in row 9*/
#define EUL_RGCH_HICH_CFG_DB_RGCH_ACTION_BMSK            0x0003
#define EUL_RGCH_HICH_CFG_DB_RGCH_ACTION_REG             9
#define EUL_RGCH_HICH_CFG_DB_RGCH_ACTION_SHFT            0

/* This bit indicates if the cell belongs to a serving RLS or a non serving
  RLS -bit number 15 in row 10*/
#define EUL_RGCH_HICH_CFG_DB_SRV_FLAG_BMSK          0x8000
#define EUL_RGCH_HICH_CFG_DB_SRV_FLAG_REG           10
#define EUL_RGCH_HICH_CFG_DB_SRV_FLAG_SHFT          15

/* These bits indicate RG comb index- bits 0-2 in row 10*/
#define EUL_RGCH_HICH_CFG_DB_RG_COMB_IDX_BMSK       0x0007
#define EUL_RGCH_HICH_CFG_DB_RG_COMB_IDX_REG        10
#define EUL_RGCH_HICH_CFG_DB_RG_COMB_IDX_SHFT       0

/* These bits indicate the RGCH offset from the DPCH Channel's timeline 
 - bits 0-7 in row 11*/
#define EUL_RGCH_HICH_CFG_DB_DPCH_OFFSET_RGCH_BMSK  0x00FF
#define EUL_RGCH_HICH_CFG_DB_DPCH_OFFSET_RGCH_REG   11
#define EUL_RGCH_HICH_CFG_DB_DPCH_OFFSET_RGCH_SHFT  0

/* These bits indicate Starting CFN number (10msec TTI) or the starting
 subframe number (2msec TTI) of RGCH- - bits 0-10  in row 12*/
#define EUL_RGCH_HICH_CFG_DB_RGCH_START_SUB_FRAME_NUM_BMSK 0x07FF
#define EUL_RGCH_HICH_CFG_DB_RGCH_START_SUB_FRAME_NUM_REG  12
#define EUL_RGCH_HICH_CFG_DB_RGCH_START_SUB_FRAME_NUM_SHFT 0

/* These bits indicate last CFN number (10msec TTI) or the last subframe
 number(2msec TTI) of current RGCH configuration- -- bits 0-10 in row 13*/
#define EUL_RGCH_HICH_CFG_DB_RGCH_FINAL_SUB_FRAME_NUM_BMSK 0x87FF
#define EUL_RGCH_HICH_CFG_DB_RGCH_FINAL_SUB_FRAME_NUM_REG  13
#define EUL_RGCH_HICH_CFG_DB_RGCH_FINAL_SUB_FRAME_NUM_SHFT 0

/*This is an index into the 25.211-6.70 -table 16B which helps with the 
 selections of the Hardmard sequence per slot . - bits 0-5  in row 14*/
#define EUL_RGCH_HICH_CFG_DB_RGCH_SIG_SEQ_IDX_BMSK     0x003F
#define EUL_RGCH_HICH_CFG_DB_RGCH_SIG_SEQ_IDX_REG      14
#define EUL_RGCH_HICH_CFG_DB_RGCH_SIG_SEQ_IDX_SHFT     0

/* The following are the fields in the EDPCH configuration information
  required by mDSP-FW HLD:Figure 2 10 */

/*mDSP EDPCH config db size.  FW HLD-2.1.1.4 */
#define   MDSP_EDPCH_CFG_LEN                      3

/*mDSP EDPCH config cmd length.  FW HLD-2.1.1.4 */
#define   MDSP_EDPCH_CFG_CMD_LEN             1

/*bit location of config db Index of EDPCH configuration cmd
- FW HLD:Figure 2 9 -bit 0*/
#define EUL_UL_CHAN_CFG_DB_IDX_BMSK                 0x0001
#define EUL_UL_CHAN_CFG_DB_IDX_SHFT                 0

/* This bit indicates the TTI of the EDPCH channel.
- bit number 15 in row 0*/
#define EUL_EDPCH_CFG_DB_TTI_BMSK                   0x8000
#define EUL_EDPCH_CFG_DB_TTI_REG                    0
#define EUL_EDPCH_CFG_DB_TTI_SHFT                   15

/* This bit indicates IR combining or chase combing.
- bit number 2 in row 0*/
#define EUL_EDPCH_CFG_DB_IR_BMSK                    0x0004
#define EUL_EDPCH_CFG_DB_IR_REG                     0
#define EUL_EDPCH_CFG_DB_IR_SHFT                    2

/* These bits describe the Action of the E-DPCH Config command
- bits 0-1 in row 0*/
#define EUL_EDPCH_CFG_DB_ACTION_BMSK               0x0003
#define EUL_EDPCH_CFG_DB_ACTION_REG                0
#define EUL_EDPCH_CFG_DB_ACTION_SHFT               0

/* These bits indicate Starting CFN number (10msec TTI) or the starting
 subframe number (2msec TTI) of  EDPCH- - bits 0-10  in row 1*/
#define EUL_EDPCH_CFG_DB_START_SUB_FRAME_NUM_BMSK       0x07FF
#define EUL_EDPCH_CFG_DB_START_SUB_FRAME_NUM_REG        1
#define EUL_EDPCH_CFG_DB_START_SUB_FRAME_NUM_SHFT       0

/* These bits indicate last CFN number (10msec TTI) or the last subframe
 number(2msec TTI) of current EDPCH configuration - bits 0-10  in row 2*/
#define EUL_EDPCH_CFG_DB_FINAL_SUB_FRAME_NUM_BMSK       0x07FF
#define EUL_EDPCH_CFG_DB_FINAL_SUB_FRAME_NUM_REG        2
#define EUL_EDPCH_CFG_DB_FINAL_SUB_FRAME_NUM_SHFT       0

/*macro for filling the shadow buffer with the config db values*/
#define EUL_SET_MDSP_SHADOW_BUF_VAL(buffer, offset, shift, mask, val) \
  buffer[offset] &= (~((uint16)(mask))); \
  buffer[offset] |= (((val) << (shift)) & (mask))

/* macro for deciding the bit position ,location in shadow buffer */
#define EUL_AGCH_CFG_DB_SET_PARAM(buffer, param, val) \
  EUL_SET_MDSP_SHADOW_BUF_VAL(buffer, \
                 EUL_AGCH_CFG_DB_##param##_REG, \
                 EUL_AGCH_CFG_DB_##param##_SHFT, \
                 EUL_AGCH_CFG_DB_##param##_BMSK, \
                 val)

/*macro for filling the shadow buffer with the config db values*/
#define EUL_SET_RGCH_HICH_BUF_VAL(buffer, cell_idx,offset,shift,mask,val) \
  buffer[cell_idx][offset] &= (~((uint16)(mask))); \
  buffer[cell_idx][offset] |= (((val) << (shift)) & (mask))

/* macro for deciding the bit position ,location in shadow buffer */
#define EUL_RGCH_HICH_CFG_DB_SET_PARAM(buffer, cell_idx,param, val) \
  EUL_SET_RGCH_HICH_BUF_VAL(buffer, \
                 cell_idx, \
                 EUL_RGCH_HICH_CFG_DB_##param##_REG, \
                 EUL_RGCH_HICH_CFG_DB_##param##_SHFT, \
                 EUL_RGCH_HICH_CFG_DB_##param##_BMSK, \
                 val)

/* macro for deciding the bit position ,location in shadow buffer */
#define EUL_EDPCH_CFG_DB_SET_PARAM(buffer, param, val) \
  EUL_SET_MDSP_SHADOW_BUF_VAL(buffer, \
                 EUL_EDPCH_CFG_DB_##param##_REG, \
                 EUL_EDPCH_CFG_DB_##param##_SHFT, \
                 EUL_EDPCH_CFG_DB_##param##_BMSK, \
                 val)

/* the dl combiner channel cfg structure as per the mdsp buffer*/
typedef PACKED struct PACKED_POST
{
 /* refer sec 2.1.1.2 of f/w HLD doc */
 /*agch config param info*/
  uint16   agch_shadow_info[MDSP_AGCH_CFG_DB_LEN];
 /*rgch-hich config param info*/
  uint16   rgch_hich_shadow_info[EUL_MAX_RL][MDSP_RGCH_HICH_CFG_DB_LEN];
}  mdsp_e_dl_comb_chan_cfg_type;

/* the edpch cfg structure as per the mdsp buffer*/
typedef PACKED struct PACKED_POST
{
 /* refer sec 2.1.1.4 of f/w HLD doc */
  uint16   edpch_shadow_info[MDSP_EDPCH_CFG_LEN];
}  mdsp_edpch_cfg_type;

extern mdsp_e_dl_comb_chan_cfg_type mdsp_e_dl_comb_chan_cfg_shadow[2];

/******************************************************************************
*******************************************************************************
                     G l o b a l   V a r i a b l e s
*******************************************************************************
******************************************************************************/

/*--------------------------------------------------------------------------
                The L1 Sync Inteface Free Buffer Queue

This queue contains buffers that are available to be used for mDSP commands.
Clients of the L1 sync interface get a free buffer, fill it with the
appropriate parameters and call mdsp_sync_send_cmd() to put the command on
the mDSP command queue or if possible immediately issue the command.
The L1 sync interface will return the buffer back to the free queue when
the command is completed.
--------------------------------------------------------------------------*/
extern q_type mdsp_sync_cmd_free_q;

/*-------------------------------------------------------------------------
                    The L1 Sync DSP Queue length

This variable holds the length of the mdsp sync queue. The queue contains
all the commands that are waiting to be sent to the mDSP.               
--------------------------------------------------------------------------*/
extern uint8 mdsp_sync_dsp_cmd_q_len;

/*--------------------------------------------------------------------------
                    The L1 Sync DSP Busy semaphore

This variable notes whether there is a pending mDSP command that we haven't
yet received the command done interrupt for yet.
--------------------------------------------------------------------------*/
extern boolean mdsp_sync_dsp_busy;



/*--------------------------------------------------------------------------
          The mDSP command timeout detection routine enable flag

This flag indicates to the sequencer that the timeout routine
mdsp_sync_timeout should be called at the CFN maintenance event on the
system timeline.  This is used for mDSP command timeout detection
--------------------------------------------------------------------------*/
extern boolean mdsp_sync_call_timeout_fn;

/* Holds the address of the main fw sw intf structure */
extern WfwSwIntfStruct *mcalwcdma_fwsw_intf_addr_ptr;
extern UWord32 swCmdDoneFlagCounter[WFW_SW_NUM_FLAGS];

/*--------------------------------------------------------------------
           F U N C T I O N    P R O T O T Y P E S
--------------------------------------------------------------------*/

#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_HS_RACH_JUDC_CFG_CMD

DESCRIPTION     This function sends the HS RACH JUDC config command to
                the mDSP. The requirement before calling this function is that already
                most of the parameters are prepared and ready to send this command.
                
DEPENDENCIES    There must be a free mDSP command buffer on the l1 sync free
                Q.

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS    The HS RACH JUDC command will be sent with valid info

===========================================================================*/
extern boolean mdsp_sync_send_judc_cfg_cmd(
    /** pre call back to be executed before the command is sent
   *  to fw */
  pre_cb_fn pre_cb,
  /** post call back to be executed after the command is sent to
   *  fw */
  post_cb_fn post_cb);
#endif /* FEATURE_WCDMA_HS_RACH */

/*===========================================================================

FUNCTION : MDSP_SYNC_GET_CUR_DSP_CMD

DESCRIPTION :   This function returns the pointer to mDSP command buffer that
                is being handled currently

DEPENDENCIES    None.

RETURN VALUE    mDSP command buffer pointer
                

SIDE EFFECTS    None.

===========================================================================*/

extern mdsp_sync_cmd_type* mdsp_sync_get_cur_dsp_cmd(void);


/*===========================================================================

FUNCTION : MDSP_SYNC_SUSPEND_CUR_DSP_CMD

DESCRIPTION :   This function suspends current mDSP command being handled

DEPENDENCIES    None.

RETURN VALUE    None
                

SIDE EFFECTS    None.

===========================================================================*/

extern void mdsp_sync_suspend_cur_dsp_cmd(void);

/*===========================================================================

FUNCTION : MDSP_SYNC_SUSPEND_CUR_DSP_CMD

DESCRIPTION :   This function un-suspends mDSP command pointer passed in as
                function argument. If mDSP sync interface is not busy then it
                send command to mDSP immediately.

DEPENDENCIES    None.

RETURN VALUE    None
                

SIDE EFFECTS    None.

===========================================================================*/

extern void mdsp_sync_unsuspend_dsp_cmd(mdsp_sync_cmd_type *dsp_cmd);


/*===========================================================================

FUNCTION      MDSP_SYNC_SEND_CMD

DESCRIPTION   This is the client interface to the L1 Sync service.  Clients
              wishing to send a command to the mDSP format the command packet
              and pass a pointer to it in the call.

DEPENDENCIES  Assumes the L1 Sync service has been initialized and that the
              mDSP is up and running.

RETURN VALUE  FALSE if there is no room on the mDSP command queue.  TRUE
              otherwise.

SIDE EFFECTS  The passed command will be enqueued on the mDSP command queue.

===========================================================================*/
extern boolean mdsp_sync_send_cmd( mdsp_sync_cmd_type *dsp_cmd );


/*===========================================================================

FUNCTION : MDSP_SYNC_CMD_DONE_ISR

DESCRIPTION :

  This ISR is associated with the Command Done Interrupt issued by the
  mDSP to the Host. mDSP generates this interrupt when it is done with the
  command submitted to it. However the case of search commands, the command
  execution is actually done when the SRCH dump Interrupt is asserted by
  the mDSP. The command done interrupt in this case simply means that the
  mDSP firmware has started the search and can accept next command. Host
  cannot start search on the same queue to which previous search was sent.

  This isr calls the post-processing callback registered by the calling
  function to do post command done processing such as reading/shipping
  the data or setting signal to the process which sent a high level
  command to the mDSP.

  This ISR writes next command from the mdsp command queue or from the dsp
  config command buffer to the mDSP.  To avoid race conditions, the queue
  length will be decremented only in this ISR.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
extern void mdsp_sync_cmd_done_isr( void );


/*===========================================================================
FUNCTION     MDSP_SYNC_RESET_WFW_CMD_DONE_COUNTERS

DESCRIPTION
  Whenever WCDMA starts up, before we ever send a command to WFW, make sure
  WFW->SW command counters are reset.  If phone was in GSM mode previously,
  these counters probably changed during GSM activity, so take a snapshot
  before sending new commands to WFW to start counting WCDMA commands now.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void mdsp_sync_reset_wfw_cmd_done_counters(void);


/*===========================================================================

FUNCTION        MDSP_SYNC_INIT

DESCRIPTION     This function initializes the L1 sync interface

DEPENDENCIES

RETURN VALUE    None

SIDE EFFECTS

===========================================================================*/
extern void mdsp_sync_init( void );

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_SRCH_CMD

DESCRIPTION     This function formats and sends a search command to the
                mDSP by way the the mDSP command queue.  Note that the
                search queues are assumed to be initialized as desired
                prior to calling this function.

                Note the caller does not have to worry about putting the
                command buffer back on the free q.  That is handled by
                the L1 sync module.

DEPENDENCIES    The search queue(s) must be initialized as desired

RETURN VALUE    TRUE if the command is sent, FALSE otherwise.  The function
                can return FALSE if either the parameters supplied are bad
                or if there are no free command buffers available.

SIDE EFFECTS    A command will be either placed on the mDSP command queue
                or will be immediately sent to the mDSP.

===========================================================================*/
boolean mdsp_sync_send_srch_cmd
(
  /* the command to send */
  srch_cmd_enum_type srch_cmd,

  /* which queue(s) does this command apply to */
  srch_q_ena_enum_type q_ena,

  /* pre-command callback */
  pre_cb_fn pre_cb,

  /* post-command callback */
  post_cb_fn post_cb
);

boolean mdsp_sync_send_offline_search_cmd  
(
  /* the command to send */
  offline_srch_cmd_enum_type srch_cmd,

  /* intraf queue to send the search */
  srch_intraf_queue_enum_type queue_to_update,
  
  /* pre-command callback */
  pre_cb_fn pre_cb,

  /* post-command callback */
  post_cb_fn post_cb
);

boolean mdsp_sync_send_offline_cm_search_cmd
(
  /* the command to send */
  offline_srch_cm_cmd_enum_type srch_cmd,

  /* pre-command callback */
  pre_cb_fn pre_cb,

  /* post-command callback */
  post_cb_fn post_cb,

  /* Queue info */
  uint8 queue,

  /* first */
  uint8 first,
  
  /* last  */
  uint8 last
);

/*===========================================================================

FUNCTION        mdsp_sync_send_ul_hsprach_cfg_cmd

DESCRIPTION     This function sends the uplink PRACH config command to the
                mDSP.

DEPENDENCIES    An mDSP command buffer must be available on the l1 sync
                free Q.

RETURN VALUE    TRUE if all goes well, FALSE if there were no available
                mDSP command buffers

SIDE EFFECTS    The uplink PRACH config command will be sent to the mDSP.

===========================================================================*/
boolean mdsp_sync_send_ul_hsprach_cfg_cmd
(
  ul_prach_cfg_cmd_parms_struct_type prach_params,

  /* the pre-command callback */
  pre_cb_fn pre_cb,

  /* the post-command callback */
  post_cb_fn post_cb
);


/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_UL_PRACH_CFG_CMD

DESCRIPTION     This function sends the uplink PRACH config command to the
                mDSP.

DEPENDENCIES    An mDSP command buffer must be available on the l1 sync
                free Q.

RETURN VALUE    TRUE if all goes well, FALSE if there were no available
                mDSP command buffers

SIDE EFFECTS    The uplink PRACH config command will be sent to the mDSP.

===========================================================================*/
boolean mdsp_sync_send_ul_prach_cfg_cmd
(
  /* Rach Action defined by wenc_rach_fw_action_enum_type */
  uint8 action,
  
  /* the uncoded TFCI bits */
  uint16 uncoded_tfci,

  /* the available signature mask */
  uint16 avail_sig_mask,

  /* the first sub-channel */
  uint16 first_sub_ch,

  /* the available sub-channel mask */
  uint16 avail_sub_ch_mask,

  /* initial TX power */
  int16 init_tx_pwr,

  /* the preamble power step size */
  uint16 preamb_pwr_step,

  /* the message power step size */
  int16 msg_pwr_step,

  /* the AICH transmit timing parameter */
  boolean aich_xmit_timing,

  /* the maximum number of preamble retransmissions */
  uint16 preamb_max_retries,

  /* the enum value of the PRACH TTI.  Note that this shares
     the definition with the downlink TTI's which have a
     larger range, i.e. 40ms, and 80ms.  Fortunately the
     10 and 20ms TTI's line up correctly with what the mDSP
     expects.   Phewwww! */
  l1_tti_enum_type tti,

  /* the pre-command callback */
  pre_cb_fn pre_cb,

  /* the post-command callback */
  post_cb_fn post_cb
);


/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_MISC_CMD

DESCRIPTION     This function sends a miscellaneous command to the mDSP.
                This function can be used for all command that carry no
                data in the command packet.  This includes
                Finger Config
                Status Dump
                Go To Sleep
                Start AGC

DEPENDENCIES    There must be an available command buffer

RETURN VALUE    TRUE if all goes well, FALSE if no buffer is available

SIDE EFFECTS    The passed in command will be sent to the mDSP

===========================================================================*/
boolean mdsp_sync_send_misc_cmd
(
  /* which command to send */
  WfwSwCmdEnum cmd_title,

  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_FING_CFG_CMD

DESCRIPTION     This function sends the finger config command to the mDSP

DEPENDENCIES    The finger config record shadow must be updated as desired.
                Note, all changes marked in the finger config record shadow
                will be applied as a result of this command.  Further note
                the the L1 Sync interface will lump separate finger cfg
                commands into a single command sent to the mDSP so the driver
                need simply update the finger cfg record shadow and call this
                function.  If a finger config command is pending the second
                command will be ignored.  This is fine since all of the command
                information lies in the finger config record, not the command
                itself.

                Note that while the pre-command callback is specified as a
                parameter it really should always be set to xfer_fing_cfg_shadow
                which copies the record shadow from the ARM to the actual record
                in mDSP memory.  Regardless of what pre-command callaback is
                used this operation must be performed in order for the command
                to function properly.

RETURN VALUE    TRUE if all goes well, FALSE if no command buffer was available

SIDE EFFECTS    The finger config command will be sent to the mDSP.  If used
                properly the finger config record shadow is copied to the mDSP
                hosted finger config record.
===========================================================================*/
boolean mdsp_sync_send_fing_cfg_cmd
(
  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb

);


/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_STATUS_DUMP_CMD

DESCRIPTION     This function sends a status dump command to the mDSP

DEPENDENCIES    There must be an available command buffer

RETURN VALUE    TRUE if all goes well, FALSE if no buffer is available

SIDE EFFECTS    A status dump command is sent to the mDSP, the pre and post
                callback functions will be called.

===========================================================================*/
boolean mdsp_sync_send_status_dump_cmd
(
  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);



/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_GOTO_SLEEP_CMD

DESCRIPTION     This function sends the Goto Sleep command to the mDSP

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean mdsp_sync_send_goto_sleep_cmd
(
  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);

#ifdef FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE

/* Code stripped out*/

#else /* FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE  */

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_START_AGC_CMD

DESCRIPTION     This function sends the Start AGC command to the mDSP

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean mdsp_sync_send_start_agc_cmd
(
  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);
#endif /* FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE  */

/*===========================================================================
FUNCTION        MDSP_SYNC_FIND_PENDING_MDSP_CMD

FILE NAME       mdspsync.c

DESCRIPTION     This function is called by mdsp_sync_timeout function.

                It goes over the command done flag counters in SW & FW and
                determines the first counter that is mismatched.

DEPENDENCIES    None

RETURN VALUE    Index of first cmd done counter that is mismatched.

SIDE EFFECTS    None
===========================================================================*/
extern uint8 mdsp_sync_find_pending_mdsp_cmd(void);

/*===========================================================================
FUNCTION        MDSP_SYNC_TIMEOUT

FILE NAME       mdspsync.c

DESCRIPTION     This function is called by sequencer at tick 0 of the system
                timeline and is used for lost command detection.

                The typcial usage scenario is that when mdsp_sync_put_dsp_cmd
                executes the variable mdsp_sync_call_timeout_fn is set to
                TRUE.  When the mdsp_sync_cmd_done_isr executes that variable
                is cleared unless another command is to be sent.  This suffices
                for the majority of the cases however there is the case where
                a command is sent close enough to the 0th tick that that duration
                is too short to be a timeout.  In this case the variable
                mdsp_sync_call_timeout_fn is set to TRUE but an additional
                variable mdsp_sync_ignore_first_timeout is also set to TRUE.
                This instructs this function to ignore the first call but
                pay attention to the second one.  The response time for a mDSP
                command should be about 256 chips and so a timeout of a few
                milliseconds is more than enough.  Using this scheme the worst
                case timeout is just over 10 ms.

DEPENDENCIES    Proper operation assumes that the system timeline is running and
                that the CFN update event is enabled.

RETURN VALUE    None

SIDE EFFECTS    A local command indicating a missed mDSP command may be sent.
===========================================================================*/
void mdsp_sync_timeout(void);


/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_SLAM_COMBINER_TIME_CMD

DESCRIPTION     This function sends the Slam Combiner Time command to the mDSP

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean mdsp_sync_send_slam_combiner_time_cmd
(
  uint8 hwch,

  uint32 combiner_count_Cx8,

  uint16 target_ref_cnt_Cx1,

  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);

/*===========================================================================
FUNCTION        MDSP_CM_INIT_CMD

FILE NAME       mdspsync.c

DESCRIPTION     This function is called by L1m manager to initialize the CM.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    A local command indicating a missed mDSP command may be sent.
===========================================================================*/
extern void mdsp_cm_init_cmd(
  /* WTR index info */
  uint8 wtrindex,
  /* RPP */
  uint16 rpp,
  /* ITP */
  uint16 itp,
  /* DL Frame type */
  uint16 dl_frame_type,
  /* UL Method */
  uint16 ul_cm_method,
  /* DL Method */
  uint16 dl_cm_method,
  /* First slot */
  uint16 first_cm_slot,
  /* Gap length */
  uint16 tgl,
  /* Measurement Purpose */
  uint16 tgmp,
  /* the rxlm buffer index used for tuning away at the start of the gap */
  uint32 rxlm_setup_buf_idx, 
  /* the rxlm buffer index used for tuning back the primary Rx chain at the end of the gap */
  uint32 rxlm_home_buf_idx,
  /* the rxlm buffer index used for tuning back the rxd chain at the end of the gap */
  uint32 rxlm_home_rxd_buf_idx,
  /* gap start time in stmr */
  uint32 gap_action_time_in_stmr,
  /* pre-command callback function */
  pre_cb_fn pre_cb,
  /* post-command callback function */
  post_cb_fn post_cb);

#ifdef FEATURE_WCDMA_OFFLINE_MODEM_W2G
/*===========================================================================
FUNCTION        mdsp_sync_send_clock_mode

FILE NAME       mdspsync.c

DESCRIPTION     This function requests new clock modes to be set by WFW.
                HIGH clock means that WFW will change the offline source 
                clock to be WCDMA ready while SLOW clock means that the
                offline source clock will be GSM ready. HW will also
                be reconfigured to be either GSM or WCDMA mode.
                Currently, HIGH mode is required during high data 
                transfers and LOW mode when no high data OR taking
                GSM measurements.  

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    Offline clocks and TDM table will be reconfigured.
===========================================================================*/
extern void mdsp_sync_send_clock_mode(
  /* Configure WFW with high clock speed */
  uint16 clock_MHz
);

/*===========================================================================
FUNCTION        mdsp_sync_send_srch_cycles

FILE NAME       mdspsync.c

DESCRIPTION     Used to send the search cycles count to FW.           

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void mdsp_sync_send_srch_cycles(
  /* Configure WFW with srch cycle count */
  uint32 srch_cycles
);
#endif /* FEATURE_WCDMA_OFFLINE_MODEM_W2G */

/*===========================================================================
FUNCTION mdsp_send_cm_step1_srch_cmd

DESCRIPTION     
  This function sends the CM interfrequency step 1 search cmd to mDSP.

DEPENDENCIES    
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mdsp_send_cm_step1_srch_cmd(
  /* Is it a first sub task */
  boolean first_sub_task,
  /* Is it a last sub task */
  boolean last_sub_task,
  /* result Q select */
  uint8 result_q,
  /* pre-command callback function */
  pre_cb_fn pre_cb,
  /* post-command callback function */
  post_cb_fn post_cb);

/*===========================================================================
FUNCTION mdsp_send_cm_list_srch_cmd

DESCRIPTION     
  This function sends the CM interfrequency list search cmd to mDSP.

DEPENDENCIES    
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mdsp_send_cm_list_srch_cmd(
  /* result Q select */
  uint8 cmd_result_q,
  /* pre-command callback function */
  pre_cb_fn pre_cb,
  /* post-command callback function */
  post_cb_fn post_cb);

/*===========================================================================
FUNCTION        MDSP_SEND_START_FACH_MEAS_CMD

FILE NAME       mdspsync.c

DESCRIPTION     This function is called by FACH measurement
                ocassion for inter-freq, W2G, W2L searches.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/

extern void mdsp_send_start_fach_meas_cmd(
  /* WTR index info */
  uint8 wtrindex,
  /* hw channel index */
  uint8 hwch_idx,
  /* start cfn */
  uint8 start_cfn,
  /* start slot number */
  int8 start_slot,
  /* gap length in slots */
  uint8 gap_len_slots,
  /* Measurement purpose */
  uint16 tgmp,
  /* the rxlm buffer index used for tuning away at the start of the gap */
  uint32 rxlm_setup_buf_idx, 
  /* the rxlm buffer index used for tuning back the primary Rx chain at the end of the fach search */
  uint32 rxlm_home_buf_idx, 
  /* the rxlm buffer index used for tuning back the rxd chain at the end of the fach search */
  uint32 rxlm_home_rxd_buf_idx,
  /* gap start time in stmr */
  uint32 gap_action_time_in_stmr,
  /* pre-command callback function */
  pre_cb_fn pre_cb,
  /* post-command callback function */
  post_cb_fn post_cb);

/*===========================================================================
FUNCTION        MDSP_SEND_IDLE_MODE_W2X_CFG_CMD

FILE NAME       mdspsync.c

DESCRIPTION     This function is used for W2X inter-rat searches from idle
                modes. It notifies WFW of the RXLM buffer index used for 
                tuning back to WCDMA, as well as the kind of search that will
                take place.
                

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/

void mdsp_send_idle_mode_w2x_cfg_cmd(
  /* the rxlm buffer index used to tune back the primary Rx chain to WCDMA at
     the end of an idle w2x search */
  uint32 rxlm_home_buf_idx,
  /* the rxlm buffer index used to tune back the diversity Rx chain to WCDMA at
     the end of an idle w2x search */
  uint32 rxlm_home_rxd_buf_idx,
  /* determines if the search is w2l or w2g */
  mdsp_sync_w2x_idle_cmd_mode_enum_type mode,
  /* pre-command callback function */
  pre_cb_fn pre_cb,
  /* post-command callback function */
  post_cb_fn post_cb);

#ifdef FEATURE_WCDMA_HS_FACH
/*===========================================================================
FUNCTION        MDSP_SYNC_SEND_HS_DEMBACK_TRESET_CMD

FILE NAME       mdspsync.c

DESCRIPTION     This function is sends HS Demback Treset command

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void mdsp_sync_send_hs_demback_treset_cmd(
  /* pre-command callback function */
  pre_cb_fn pre_cb,
  /* post-command callback function */
  post_cb_fn post_cb);
#endif /* FEATURE_WCDMA_HS_FACH */

/*===========================================================================
FUNCTION        MDSP_SYNC_SEND_HS_UL_CFG_CMD

FILE NAME       mdspsync.c

DESCRIPTION     This function is sends HS UL channel config command

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/

extern void mdsp_sync_send_hs_ul_cfg_cmd(
  /* config structure */
  WfwSWCmdTxHsDpcchCfgStruct *hsCfg,
  /* pre-command callback function */
  pre_cb_fn pre_cb,
  /* post-command callback function */
  post_cb_fn post_cb);

#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
/*===========================================================================
FUNCTION        MDSP_SYNC_SEND_ANT_SWITCH_CMD

FILE NAME       mdspsync.c

DESCRIPTION     This function is sends the Antenna switch command to FW

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/

extern void mdsp_sync_send_ant_switch_cmd(
  /* config structure */
  WfwAntSwitchCfgCmdStruct *AntSwitchCfg,
  /* pre-command callback function */
  pre_cb_fn pre_cb,
  /* post-command callback function */
  post_cb_fn post_cb);
#endif

/*===========================================================================
FUNCTION mdsp_sync_register_event_done_cb

DESCRIPTION     
  This function sends the CM interfrequency list search cmd to mDSP.

DEPENDENCIES    
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mdsp_wcdma_l1_register_event_done_cb(
  /* event type */
  mdsp_wcdma_l1_event_type event_type,
  /* callback function pointer */
  MDSP_WCDMA_L1_EVENT_CB_FUNC_TYPE *cb_func);

extern void mdsp_wcdma_l1_suspend_event_done_cb(
  /* event type */
  mdsp_wcdma_l1_event_type event_type);

extern void mdsp_wcdma_l1_resume_event_done_cb(
  /* event type */
  mdsp_wcdma_l1_event_type event_type);

extern void mdsp_wcdma_l1_execute_event_handler(
  /* event type */
  mdsp_wcdma_l1_event_type event_type);

/*===========================================================================
FUNCTION MDSP_WCDMA_L1_EVENT_ISR_HANDLER

DESCRIPTION     
  MDSP to L1 ISR handler

DEPENDENCIES    
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mdsp_wcdma_l1_event_isr_handler(void);

/*===========================================================================

FUNCTION        MDSP_COPY_AGCH_TO_E_DL_COMB_CHAN_CFG_SHADOW

DESCRIPTION     This function backs up the current comb shadow for debug

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.

===========================================================================*/
void mdsp_backup_current_comb_shadow(mdsp_e_dl_comb_chan_cfg_type* dest_comb_chan_shadow,
                                     mdsp_e_dl_comb_chan_cfg_type* src_comb_chan_shadow);

/*===========================================================================

FUNCTION        MDSP_CLEAR_CURRENT_COMB_SHADOW

DESCRIPTION     This function clears the current comb shadow before starting 
                a new config

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.

===========================================================================*/

extern void mdsp_clear_current_comb_shadow(void);

/*===========================================================================

FUNCTION        MDSP_PATCH_AGCH_START_SUB_FN_TO_E_DL_COMB_CFG_SHADOW

DESCRIPTION     This function patches the start_fn into the agch info in
                e_dl_comb_cfg_shadow

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    None.

===========================================================================*/
void mdsp_patch_agch_start_sub_fn_to_e_dl_comb_cfg_shadow(
       uint16 start_sub_fn,
       mdsp_e_dl_comb_chan_cfg_type* e_dl_comb_chan_cfg_shadow);

/*===========================================================================

FUNCTION        MDSP_PATCH_AGCH_FINAL_SUB_FN_TO_E_DL_COMB_CFG_SHADOW

DESCRIPTION     This function patches the start_fn into the agch info in
                e_dl_comb_cfg_shadow

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    None.

===========================================================================*/
void mdsp_patch_agch_final_sub_fn_to_e_dl_comb_cfg_shadow(
       uint16 final_sub_fn,
       mdsp_e_dl_comb_chan_cfg_type* e_dl_comb_chan_cfg_shadow);

/*===========================================================================

FUNCTION        MDSP_PATCH_HICH_START_SUB_FN_TO_E_DL_COMB_CFG_SHADOW

DESCRIPTION     This function patches the start_fn into the hich info in
                e_dl_comb_cfg_shadow

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    None.

===========================================================================*/
void mdsp_patch_hich_start_sub_fn_to_e_dl_comb_cfg_shadow(
       uint16 start_sub_fn,
       mdsp_e_dl_comb_chan_cfg_type* e_dl_comb_chan_cfg_shadow,
       uint16 e_dl_rl_idx);

/*===========================================================================

FUNCTION        MDSP_PATCH_HICH_FINAL_SUB_FN_TO_E_DL_COMB_CFG_SHADOW

DESCRIPTION     This function patches the final_fn into the hich info in
                e_dl_comb_cfg_shadow

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    None.

===========================================================================*/
void mdsp_patch_hich_final_sub_fn_to_e_dl_comb_cfg_shadow(
       uint16 final_sub_fn,
       mdsp_e_dl_comb_chan_cfg_type* e_dl_comb_chan_cfg_shadow,
       uint16 e_dl_rl_idx);

/*===========================================================================

FUNCTION        MDSP_PATCH_RGCH_START_SUB_FN_TO_E_DL_COMB_CFG_SHADOW

DESCRIPTION     This function patches the start_fn into the rgch info in
                e_dl_comb_cfg_shadow

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    None.

===========================================================================*/
void mdsp_patch_rgch_start_sub_fn_to_e_dl_comb_cfg_shadow(
       uint16 start_sub_fn,
       mdsp_e_dl_comb_chan_cfg_type* e_dl_comb_chan_cfg_shadow,
       uint16 e_dl_rl_idx);

/*===========================================================================

FUNCTION        MDSP_PATCH_RGCH_FINAL_SUB_FN_TO_E_DL_COMB_CFG_SHADOW

DESCRIPTION     This function patches the final_fn into the rgch info in
                e_dl_comb_cfg_shadow

DEPENDENCIES    None 

RETURN VALUE    None

SIDE EFFECTS    None.

===========================================================================*/
void mdsp_patch_rgch_final_sub_fn_to_e_dl_comb_cfg_shadow(
       uint16 final_sub_fn,
       mdsp_e_dl_comb_chan_cfg_type* e_dl_comb_chan_cfg_shadow,
       uint16 e_dl_rl_idx);

/*===========================================================================

FUNCTION        MDSP_XFER_E_DL_COMB_CHAN_CFG_SHADOW

DESCRIPTION     This function copies the EUL DL combiner channel config shadow 
                to mdsp eul dl cfg buffer.

DEPENDENCIES    None

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS    The mDSP EUL DL comb chan config data will be sent to the mDSP.

===========================================================================*/
extern boolean mdsp_xfer_e_dl_comb_chan_cfg_shadow
(
  /* indicates config page to be used to copy the params */
  uint8 cfg_db_idx,

  /* pointer to eul dl combiner channel config shadow */
  mdsp_e_dl_comb_chan_cfg_type* e_dl_comb_chan_cfg_shadow
);
/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_E_DL_COMB_CHAN_CFG_CMD

DESCRIPTION     This function sends the EUL DL combiner channel config command
                to the mDSP.

DEPENDENCIES    There must be a free mDSP command buffer on the l1 sync free Q.

RETURN VALUE    TRUE if all goes well, FALSE if there are no mdsp buffers free.

SIDE EFFECTS    The mDSP EULl DL comb chan config cmd will be sent to the mDSP.

===========================================================================*/
extern boolean mdsp_sync_send_e_dl_comb_chan_cfg_cmd
(
  /* indicates config page to be used to copy the params */
  uint8 cfg_db_idx,

  /* the pre-command callback function */
  pre_cb_fn pre_cb,

  /* the post-commadn callback function */
  post_cb_fn post_cb
);

/*===========================================================================

FUNCTION        MDSP_XFER_EDPCH_CFG_SHADOW

DESCRIPTION     This function copies the EUL edpch config shadow to mdsp eul
                edpch cfg buffer.

DEPENDENCIES    None

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS    The mDSP EUL edpch config data will be sent to the mDSP.

===========================================================================*/
extern boolean mdsp_xfer_edpch_cfg_shadow
(
  /* indicates config page to be used to copy the params */
  uint8 cfg_db_idx,

  /* pointer to eul edpch config shadow*/
  mdsp_edpch_cfg_type* edpch_cfg_shadow
);
/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_EDPCH_CFG_CMD

DESCRIPTION     This function sends the EUL EDPCH channel config command to
                mDSP.

DEPENDENCIES    There must be a free mDSP command buffer on the l1 sync free
                Q.

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS    The mDSP EUL EDPCH config will be sent to the mDSP.

===========================================================================*/
extern boolean mdsp_sync_send_edpch_cfg_cmd
(
  /* the pre-command callback function */
  pre_cb_fn pre_cb,

  /* the post-commadn callback function */
  post_cb_fn post_cb
);

extern boolean mdsp_sync_send_demod_cfg_cmd(
  /** cell bmsk that needs to be configured */
  uint16 cell_bmsk, 
  /** fing bmsk that needs to be configured */
  uint32 fing_bmsk,
  /** combiner bmsk that needs to be configured */
  uint16 comb_bmsk,
  /** phychan bmsk that needs to be configured */
  uint32 phychan_bmsk,
    /** CME (EQ) cfg bmsk that needs to be configured  */
  uint32 cme_bmsk,
  /** CME Common cfg bmsk that needs to be configured */
  uint32 cmn_bmsk,
  /** ARD cfg bmsk that needs to be configured */
  uint32 ard_bmsk,
  /** pre call back to be executed before the command is sent
   *  to fw */
  pre_cb_fn pre_cb,
  /** post call back to be executed after the command is sent to
   *  fw */
  post_cb_fn post_cb);


/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_CCTRCH_DIRTY_BIT_UPDATE_CMD

DESCRIPTION     This function sends the CCTrCh dirty bit update command to the mDSP

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean mdsp_sync_send_cctrch_dirty_bit_update_cmd
(
  uint16 dirty_bmsk,

  boolean interrupt_reqd,

  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);

#ifdef FEATURE_WCDMA_FS_FALSE_PEAK_ELIM_OPT

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_FREQ_SCAN_CMD

DESCRIPTION     This function sends the finger config command to the mDSP

DEPENDENCIES    

RETURN VALUE    TRUE if all goes well, FALSE if no command buffer was available

SIDE EFFECTS    .
===========================================================================*/
extern boolean mdsp_sync_send_freq_scan_cmd
(
  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);
#endif


/*===========================================================================

FUNCTION        MDSP_SYNC_PRE_SEND_SYNC_CMD_CB

DESCRIPTION     Based on the type of command that is about to be synced,
                reads the associated shared memory so that it is guaranteed
                this data has been committed to memory when the sync command
                occurs.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    .
===========================================================================*/
extern void mdsp_sync_pre_send_sync_cmd_cb(void);

/*===========================================================================

FUNCTION        MDSP_SYNC_SEND_FW_PANIC_CMD

DESCRIPTION     This function sends Panic cmd to FW.

DEPENDENCIES    There must be a free mDSP command buffer on the l1 sync free
                Q.

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS    Panic cmd sent to FW incase SW error fatals and for debugging both
                SW and FW states are required.

===========================================================================*/

extern boolean mdsp_sync_send_fw_panic_cmd(
  /* the pre-command callback function */
  pre_cb_fn pre_cb,

  /* the post-commadn callback function */
  post_cb_fn post_cb
);

/*===========================================================================
FUNCTION        MDSP_SYNC_SEND_START_STOP_CMD

FILE NAME       mdspsync.c

DESCRIPTION     This function sends START/STOP command to mDSP

DEPENDENCIES    

RETURN VALUE    TRUE, if command delivered succesfully. Otherwise, FALSE

SIDE EFFECTS    None
===========================================================================*/
extern boolean mdsp_sync_send_start_stop_cmd(
  /** flag to indicate start or stop */
  boolean start,
  /** what mode to start the firmware in*/
  mdsp_sync_start_cfg_enum_type start_mode,
  /** pre call back to be executed before the command is sent
   *  to fw */
  pre_cb_fn pre_cb,
  /** post call back to be executed after the command is sent to
   *  fw */
  post_cb_fn post_cb);

/*================================================================================
FUNCTION        MDSP_CHECK_START_STOP_CMD_STATUS

FILE NAME       mdspsync.c

DESCRIPTION     This function updates the status of START/STOP cmd status variable

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    None
==================================================================================*/
extern void mdsp_check_start_stop_cmd_status(uint16 status);

/*===========================================================================
FUNCTION        MDSP_SYNC_CHECK_IGNORE_FIRST_TIMEOUT

FILE NAME       mdspsync.c

DESCRIPTION     This function checks to see whether the sequencer is at the
                last slot of a frame and returns TRUE if so.  The idea is
                that if we are in the last slot of a frame then the next CFN
                update event is too close to be used as a mDSP command
                timeout point.  Thus we'll choose to ignore the first
                CFN update event and declare a mDSP command timeout on the
                second.  This function however is ignorant of that use, it
                simply returns whether we are on the last slot of a frame.

DEPENDENCIES    The L1 sequencer.

RETURN VALUE    TRUE if we are on slot 14 of the current frame, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean mdsp_sync_check_ignore_first_timeout(void);

/*=========================================================================

FUNCTION : mdsp_sync_buf_read_last_cmd

DESCRIPTION : This function reads last byte of sync buffer and updates in
              global var for the given cmd type. This is done to make sure
              shared memeory refresh the cache and have MDSP reads right
              data contents from shared memory

DEPENDENCIES    None.

RETURN VALUE    return last byte of the given cmd

SIDE EFFECTS    None.

=========================================================================*/
extern void mdsp_sync_buf_read_last_cmd(WfwSwCmdEnum cmd_title);

/*=========================================================================

FUNCTION : MDSP_SYNC_DELIVER_DSP_CMD

DESCRIPTION : This function accepts a command of dcqm_cmd_type and
  delivers it to the DSP command buffer. This function also checks for
  the status of the previous command and if it is un-expected then spits
  out a error or warning message.

DEPENDENCIES    None.

RETURN VALUE    TRUE  : If command was put to the DSP successfully
                FALSE : otherwise

SIDE EFFECTS    None.

=========================================================================*/
extern boolean mdsp_sync_deliver_dsp_cmd( mdsp_sync_cmd_type *cmd );

/*===========================================================================

FUNCTION        MDSP_SYNC_MEMCPY

DESCRIPTION     Copies from uncached to cached memory.  Does not cast to
                uint32 to avoid erros with unaligned access.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    .
===========================================================================*/
LOCAL void mdsp_sync_memcpy(void *  dest_ptr,
                            void *  src_ptr,
                            uint32  length);

/*================================================================================
FUNCTION        MDSP_SYNC_GET_DEMOD_CFG_CMD

FILE NAME       mdspsync.c

DESCRIPTION     This function returns the mdsp_cmd_ptr of the demod config command
                if it is in suspend state and if the combiner/phychan config update bmsk matches

DEPENDENCIES    

RETURN VALUE    mdsp cmd ptr for the demod config cmd in the queue

SIDE EFFECTS    None
==================================================================================*/
extern mdsp_sync_cmd_type* mdsp_sync_get_demod_cfg_cmd(uint32 comb_cfg_upd_bmsk, uint32 phch_cfg_upd_bmsk);

#if defined(FEATURE_WCDMA_CPC_DRX) || defined(FEATURE_WCDMA_CPC_DTX)
/*===========================================================================

FUNCTION        mdsp_sync_send_drx_dtx_cfg_cmd

DESCRIPTION     This function sends a drx dtx cfg command to fw

DEPENDENCIES

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS    None.

===========================================================================*/
extern boolean mdsp_sync_send_drx_dtx_cfg_cmd( 
  /** drx dtx cfg cmd ptr */
  mdsp_sync_cpc_drx_dtx_cfg_cmd_struct_type *cfg_cmd_ptr,
    /* the pre-command callback function */
    pre_cb_fn pre_cb,
    /* the post-commadn callback function */
  post_cb_fn post_cb);
#endif

/*================================================================================
DESCRIPTION     This function queues up a FW command to tune RF to a frequency

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void mdsp_sync_send_rf_tune_cmd( 
  /* the index of the buffer (0-3) containing the tuning script */
  uint8 buf_idx,
  /* the trigger time of the tune in cx1 
     Timed Range 0:38399, for immediate time: 0xFFFF */
  uint16 trigger_time);

extern boolean mdsp_sync_send_rf_update_cmd
(
  uint32 event_id,

  uint32 offset,

  /* pre-command callback function */
  pre_cb_fn pre_cb,

  /* post-command callback function */
  post_cb_fn post_cb
);
/*===========================================================================

FUNCTION        MDSP_SYNC_APPLY_DPD_CMD

DESCRIPTION     This function sends the Apply DPD command to
                the mDSP. Invoked from RF SW for dynamic updates.

DEPENDENCIES    There must be a free mDSP command buffer on the l1 sync free
                Q.

RETURN VALUE    TRUE if all goes well, FALSE if there are no buffers free.

SIDE EFFECTS

===========================================================================*/
boolean mdsp_sync_apply_dpd_cmd
(
  /* DPD Index related to the capture */
  uint16 dpd_index,

  /* the pre-command callback function */
  pre_cb_fn pre_cb,

  /* the post-commadn callback function */
  post_cb_fn post_cb
);


boolean mdsp_sync_ept_cal_tlb_mem_map_cmd
(
  /* Size of the memory mapped for captures */
  uint32 mem_size,

  /*Pointer to the Memory address*/
  uint32* mem_address,

  /* the pre-command callback function */
  pre_cb_fn pre_cb,

  /* the post-commadn callback function */
  post_cb_fn post_cb
);


#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
/*===========================================================================
FUNCTION        MDSP_SYNC_DELETE_CMD

FILE NAME       mdspsync.c

DESCRIPTION    Deletes command from mdsp_sync_dsp_cmd_q and puts it back in mdsp_sync_cmd_free_q

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void mdsp_sync_delete_cmd
(
  /*Command pointer to be deleted*/
  mdsp_sync_cmd_type *cmd
);
#endif 

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION        MDSP_QSH_POPULATE_CMD_TRACE_BUFF

FILE NAME       mdspsync.c

DESCRIPTION    This function fills MDSP cmd related info for QSH

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void mdsp_qsh_populate_cmd_trace_buff(wl1_common_mini_debug_info_struct_type *common_dump_ptr);
#endif /* FEATURE_QSH_DUMP */

#endif/*end of MDSP_SYNC_H*/
