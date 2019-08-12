#ifndef __MCALWCDMA_SRCH_H__
#define __MCALWCDMA_SRCH_H__

/*============================================================================
                        M C A L W C D M A _ S R C H. H
DESCRIPTION
  This module has MCAL (modem core abstraction layer) Searcher APIs.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Function mcalwcdma_srch_init should be at least called once before calling 
  any other mcal searcher api. 

  Function mcalwcdma_srch_set_status_callback_fn installs a callback function.
  If callback function is not installed then the search requests will be rejected.

  Function mcalwcdma_srch_update_srch_async_parms updates the async parameters.

  Function mcalwcdma_srch_do_srch performs a search.

  Function mcalwcdma_srch_get_srch_results returns the search results.

Copyright (c) 2007 - 2011 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mcalwcdma_srch.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
04/23/15    da      Setting srchUseFreezeFlag to 0 upon scheduler mode change to INTRAF in ACQ state
01/16/15    ag      Added Searcher debug code 
10/10/14    amj     Code Cleanup.
09/10/14    amj     Nastt Power Optimization changes.
05/20/14    bj      Added support for Pause and Resume searches in QTA gap and disable RxD searches in QTA gap
07/15/14    bj      WCDMA Narrow Band Rejection for System Search.
06/23/14    bj      Reset async search params during srch_abort due to mode change.
02/07/14    amj     Added Step1 idx to the NASTT sync params.
07/01/14    amj     Clean up offline clk change pending search parameters from APIs
12/20/13    dp      Allow partial task completion from FW for DSDA
12/06/13    jd      Fix for Incorrect pn result processing when cell is removed during search.
11/21/13    jd      FET 2.0 Searcher two queue approach.
11/14/13    jd      Sw/Fw interface changes for searcher two queue approach.
11/06/13    gj      Added 3C searcher code
10/17/13    bs      Preliminary 3C Support
10/15/13    mm      Initial FE-FACH check-in
09/12/13    jd      Searcher redesign
07/23/13    jd      parallel step1 changes
06/11/13    amj     Added API to save the current earch parameters for offline clock change
04/11/13    gsk     Idle Mode FR 2741/2743, reducing WL1 wait time between rf_tune_to_chan
                    and search cmd to FW. Also adding support for interf offline PN
03/20/13    amj     Make changes to incorporate search aborts from all modules.
12/10/12    amj     Fixed compiler error by moving intlock/intfree defs in header
12/10/12    amj     Removed function with duplicate functionality.
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12    gsk     Idle mode FRs 1782/1705/1947/2152/1950
05/24/12    amj     Fixed compiler warning by closing comment section properly. 
05/23/12    amj     Added field SrchSclEnable to pass to FW to apply step1 scaling
03/06/12    zr      Adding API to reschedule aborted PN search
01/20/12    zr      Mainlined FEATURE_WCDMA_GENESIS_SW, removed FEATURE_WCDMA_TAXIS_SW 
12/07/11    sv      Added changes to support Fatpath algorithm in connected mode.
03/11/11    dp      Ported CPC DRX changes to offline Architecture
03/30/10    rvs     IRAT sample-server config changes for Chetak and
                    general clean-up.
02/12/10    sv      Added interface to move PN search posLSW to per task.
02/06/10    sv      Mainlined features FEATURE_MCAL_SRCH_ANTSEL_SYNC_PARAMS and 
                    FEATURE_MCAL_SRCH_CARR_SEL_SYNC_PARAMS.
11/23/09    nd      Enabled passing carr sel bit in step1 sync interface
11/11/09    nd      Srch updates for new FW interface
11/09/09    nd      Added carr sel bit in the asyn srch params.
09/17/09    yh      Removed touching TDM table settings from mcal.
09/23/09    sq      Added FEATURE_WCDMA_GENESIS_SW
07/20/09    sq      Added support for UMTS MOB build
07/16/09    hk      Reorganized the code to remove INC/SRC violations
                    Moved the file from SRC to INC
07/09/09    yh      Support for FEATURE_WCDMA_OFFLINE_MODEM_IRAT_PHASE2
06/01/09    sv      Mainlined the search abort code.
05/05/09    sv      Enabling srch abort cmd to FW.
04/29/09    sv      Disabling srch abort until race condition is fixed.
04/20/09    yh      Compiler warning fixes.
04/02/09    sv      Resolve pending race condition when aborting searches.
03/25/09    sv      Enable search abort command to FW.
02/27/09    sv      Putting the functions for CM+RxD for Phase 1 under feature
                    FEATURE_WCDMA_OFFLINE_MODEM_W2G.
02/26/09    yh      Support for CM+RxD for Phase 1.
02/10/09    sv      Fixed the overflow of PN result buffers. 
02/06/09    sv      Externed the function "mcalwcdma_is_srch_state_active".
01/12/09    sv      Extended the number of search callbacks.
01/02/09    sv      Externed the function "mcalwcdma_is_srch_abort_pending".
11/13/08    yh      Add CM support
11/12/08    sv      Externed function mcalwcdma_srch_reset_srch. 
11/07/08    sv      Removed the definition of critical section from .h.
11/06/08    sv      Introducing SRCH_INTLOCK, SRCH_INTFREE for mutual exclusion of
                    searcher related variable access between Task and ISR. 
10/06/08    sv      Move Antenna select programming from PN async parms to sync.
03/13/08    mg      Add mcalwcdma_srch_pd_type and mcalwcdma_srch_rxd_mode_enum_type
01/08/08    mg      Add compressed mode support
12/20/07    mg      Sync to the latest fw interface
09/28/07    mg      First version. 

===========================================================================*/
#include "comdef.h"
#include "mcalwcdma_common.h"
#include "rex.h"
#include "l1def.h"
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* --------------------------------------------------------------------------
   Defines and enumerations
   --------------------------------------------------------------------------*/

#define MAX_PARALLEL_SRCH 2

/* Critical section for locking srch variables that are accessed simultaneously
 * between task and ISR
 */
extern rex_crit_sect_type wsrch_crit_sect;

#define SRCH_INTLOCK()        REX_ISR_LOCK(&wsrch_crit_sect)
#define SRCH_INTFREE()        REX_ISR_UNLOCK(&wsrch_crit_sect)

/* List of searches */
typedef enum
{
  MCALWCDMA_SRCH_STEP1,
  MCALWCDMA_SRCH_NASTT,
  MCALWCDMA_SRCH_PN,
  MCALWCDMA_SRCH_OTDOA,
  MCALWCDMA_SRCH_NONE,
  MCALWCDMA_SRCH_NUM_TYPES
} mcalwcdma_srch_type;

/* List of search releated events for debugging purpose */
typedef enum
{
  MCALWCDMA_SRCH_LOG_STEP1,  /*step1 search cmd issued event*/
  MCALWCDMA_SRCH_LOG_NASTT,  /*NAS-TT search cmd issued event*/
  MCALWCDMA_SRCH_LOG_PN,     /*PN search cmd issued event*/
  MCALWCDMA_SRCH_LOG_INIT,   /* W int cmd issued event*/
  MCALWCDMA_SRCH_LOG_RESET,  /* Reseting issued event */
  MCALWCDMA_SRCH_LOG_DONE,   /* Search done issued event */
  MCALWCDMA_SRCH_LOG_LOST,   /* Lost search issued event */
  MCALWCDMA_SRCH_LOG_REJECT_DONE, /* Local command Search done reject event */
  MCALWCDMA_SRCH_LOG_REJECT_LOST, /* Local command Lost search reject event  */
  MCALWCDMA_SRCH_LOG_REJECT_ABORT,/* Local command abort search reject event */
  MCALWCDMA_SRCH_LOG_FW_ABORT,    /* Search abort by FW event */ 
  MCALWCDMA_SRCH_LOG_SW_ABORT,    /* Search abort by SW event */ 
  MCALWCDMA_SRCH_LOG_NUM_TYPES
} mcalwcdma_debug_srch_type;

/* List of compressed mode searches */
typedef enum
{
  MCALWCDMA_SRCH_CM_STEP1,
  MCALWCDMA_SRCH_CM_PN,
  MCALWCDMA_SRCH_CM_NUM_TYPES,
  MCALWCDMA_SRCH_CM_NONE
} mcalwcdma_srch_cm_type;

/* Status passed to MCAL SRCH clients in the callback function */
typedef enum
{
  MCALWCDMA_SRCH_CMD_SUCCESS,
  MCALWCDMA_SRCH_CMD_FAIL,
  MCALWCDMA_SRCH_DONE,
  MCALWCDMA_SRCH_LOST,
  MCALWCDMA_SRCH_ABORT_CMD_SUCCESS,
  MCALWCDMA_SRCH_STATUS_NUM_TYPES
} mcalwcdma_srch_status_type;

/* Peak detect mode (00: normal, 01: Bypass, 10: Shoulder) */
typedef enum
{
  MCALWCDMA_SRCH_PD_NORMAL,
  MCALWCDMA_SRCH_PD_BYPASS,
  MCALWCDMA_SRCH_PD_SHOULDER,
  MCALWCDMA_SRCH_PD_NUM_TYPES
} mcalwcdma_srch_pd_type;

/* rxd mode (00: no rxd, 01: separate, 10: Add) */
typedef enum
{
  /* No Receive Diversity */
  MCALWCDMA_SRCH_RXD_MODE_NO_RXD,
  /* FW to search on one antenna at a time and report sorted peaks from
  both the antenna separtely */
  MCALWCDMA_SRCH_RXD_MODE_SEPARATE,
  /*combined energies from both the antenna are returned*/
  MCALWCDMA_SRCH_RXD_MODE_ADD,
  /* total number of Rxd modes available */
  MCALWCDMA_SRCH_NUM_RXD_MODE
} mcalwcdma_srch_rxd_mode_enum_type;

typedef enum
{
  /* Un monitored search */
  MCALWCDMA_SRCH_NASTT_UN_MON_SRCH,

  /* Async search */
  MCALWCDMA_SRCH_NASTT_ASYNC_SRCH,

  MCALWCDMA_SRCH_NASTT_NUM_SRCH

} mcalwcdma_srch_nastt_srch_enum_type;

/* definition of searcher callback function */
typedef void (*mcalwcdma_srch_status_callback_fn_type) (mcalwcdma_srch_status_type status, srch_intraf_queue_enum_type srch_queue_idx);
typedef void (*mcalwcdma_srch_cm_status_callback_fn_type) (mcalwcdma_srch_status_type status, uint8 queue_idx);

/* The following definitions are used to validate the parameters before passing to dsp */
#define MCALWCDMA_SRCH_STEP1_NUM_FREQ_HYPO 3
#define MCALWCDMA_SRCH_STEP1_NON_COHINTG_LEN_MIN 1
#define MCALWCDMA_SRCH_STEP1_NON_COHINTG_LEN_MAX 128
#define MCALWCDMA_SRCH_STEP1_INIT_ACQ_MAX 4
#define MCALWCDMA_SRCH_STEP1_FA_FLAG_MAX 1

#define MCALWCDMA_SRCH_NASTT_NON_COHINTG_LEN_MAX 127
#define MCALWCDMA_SRCH_NASTT_COHINTG_LEN_MAX 10239
#define MCALWCDMA_SRCH_TASK_MODE_MAX 6
#define MCALWCDMA_SRCH_TASK_MODE_INVALID_VAL 5
#define MCALWCDMA_SRCH_ANT_SEL_MAX 1
#define MCALWCDMA_SRCH_NUM_RESULTS_PER_TASK_MIN 1
#define MCALWCDMA_SRCH_NUM_RESULTS_PER_TASK_MAX 7

#define MCALWCDMA_SRCH_CM_STEP1_NON_COHINTG_LEN_MIN 1
#define MCALWCDMA_SRCH_CM_STEP1_NON_COHINTG_LEN_MAX 128
#define MCALWCDMA_SRCH_CM_TASK_MODE_MAX 2
#define MCALWCDMA_SRCH_CM_ANT_SEL_MAX 1
#define MCALWCDMA_SRCH_CM_NASTT_NON_COHINTG_LEN_MAX 127
#define MCALWCDMA_SRCH_CM_NUM_RESULTS_PER_TASK_MIN 1
#define MCALWCDMA_SRCH_CM_NUM_RESULTS_PER_TASK_MAX 7

#define MCALWCDMA_SRCH_TASKMODE_RXD_ADD_MODE_ENA_BMSK       0x1
#define MCALWCDMA_SRCH_TASKMODE_STTD_MODE_ENA_BMSK          0x2
#define MCALWCDMA_SRCH_TASKMODE_RXD_SEPARATE_MODE_ENA_BMSK  0x4

/* Compressed Mode number of command queues*/
#define MCALWCDMA_SRCH_CM_CMQ0        0
#define MCALWCDMA_SRCH_CM_CMQ1        1
#define MCALWCDMA_SRCH_CM_MAX_NUM_CMQ 2
#define MCALWCDMA_SRCH_CM_INVALID_CMQ 0xFF

typedef enum
{
  MCALWCDMA_SRCH_CARRIER_PRIMARY,
  MCALWCDMA_SRCH_CARRIER_SECONDARY,
  MCALWCDMA_SRCH_CARRIER_TERTIARY
} mcalwcdma_carr_selection_enum_type;

#ifdef FEATURE_WCDMA_3C_HSDPA
/* The maximum carrier ID allowable with Dual Carrier */
#define MCALWCDMA_SRCH_CARRIER_ID_MAX    MCALWCDMA_SRCH_CARRIER_TERTIARY
#else
#define MCALWCDMA_SRCH_CARRIER_ID_MAX    MCALWCDMA_SRCH_CARRIER_SECONDARY
#endif /* FEATURE_WCDMA_3C_HSDPA */
/* --------------------------------------------------------------------------
   Structures and unions
   --------------------------------------------------------------------------*/
/* Step1 async parms */
typedef struct
{
  /* Non-coherent integration length (in units of coherent integration length) 
   * for each slot-timing hypothesis in step1.
   * Range : 1-128 
   */
  uint8   srchStep1NonCohIntgLen;
  
  /* 
   * Rx mode for Step1 search, bit 0: RxD (add mode), bit 1: STTD mode, bit 2 RxD (separate mode) 
   * Note: step1 should not use STTD mode
   * Note: For current FW, RxD separate mode is not supported for Step1
   * Range : 0, 1, 2, 3, 4, 6 
   */
  uint8   srchStep1TaskMode;
  /* 0 to select antenna 0 and 1 to select antenna 1, this only applies in non-RxD mode */
  /* Range : 0-1 */
  boolean srchStep1AntSel;
  /* Peak detect mode for step1 00: normal, 01: Bypass, 10: Shoulder*/
  /* Range: 0-4 */
  mcalwcdma_srch_pd_type   srchStep1PdMode;

  /* Number of noncoherent accumulations to be executed before FW copies the 
   * slot data from sample RAM0 to sample RAM1 for later NAS-TT, this variable 
   * is only used when srchSSFreezeFlag is set to 1*/
  /* Range: 0-255 */
  uint8   srchStep1NNcohInitAcq;
  /* Step1 False Alarm reduction enable flag, 1: the FA feature is enabled, 
   * 0: the FA feature is disabled*/
  /* Range: 0-1 */
  boolean srchStep1FAFlag;
  /* Number of peaks to be returned in step1 */
  /* Range: 0-64 */
  uint8   srchStep1NumPeaks;
  /* Step1 search latest finish time (in frame unit, using frame_cnt) */
  /* Range: 0-65535 */
  uint16  srchStep1LateTime;
  /* Energy threshold for step1 peaks */
  /* Range: 0-65535 */
  uint16  srchStep1EnrgTh;
  
} mcalwcdma_srch_step1_async_parms_struct_type;

extern mcalwcdma_srch_step1_async_parms_struct_type srch_step1_parms;

/* Step1 compressed mode async parms */

typedef struct
{
  /* Non-coherent integration length (in units of coherent integration length) 
   * for each slot-timing hypothesis in step1.
   * Range : 1-128 
   */
  uint8  srchStep1NonCohIntgLen;
  /* 
   * Rx mode for Step1 search, bit 0: RxD (add mode), bit 1: STTD mode, bit 2 RxD (separate mode) 
   * Note: step1 should not use STTD mode
   * Note: For current FW, RxD separate mode is not supported for Step1
   * Range : 0, 1, 2, 3, 4, 6 
   */
  uint8  srchStep1TaskMode;
  /* 0 to select antenna 0 and 1 to select antenna 1, this only applies in non-RxD mode */
  /* Range : 0-1 */
  uint8  srchStep1AntSel;
  /* Peak detect mode for step1 00: normal, 01: Bypass, 10: Shoulder*/
  /* Range: 0-4 */
  mcalwcdma_srch_pd_type  srchStep1PdMode;
  /* Number of peaks to be returned in step1 */
  /* Range: 0-64 */
  uint8  srchStep1NumPeaks;
  /* Energy threshold for step1 peaks */
  /* Range: 0-65535 */
  uint16 srchStep1EnrgTh;
  /* Step1 search latest finish time (in frame unit, using frame_cnt) */
  /* Range: 0-65535 */
  uint16 srchStep1LateTime;
} mcalwcdma_srch_cm_step1_async_parms_struct_type;

/* NASTT async parms */
typedef struct
{
  /* NAS-TT non-coherent integrations */
  /* Range: 0-127 */
  uint8   srchNASTTNonCohIntgLen;
  /* Number of peaks reported per task for NAS-TT search */
  /* Range:  1-7 */
  uint8   srchNASTTNumResPerTask;
  /* Rx mode for NAS-TT search, bit 0: RxD (add mode), 
   * bit 1: STTD mode, bit 2 RxD (separate mode)
   * Note: For current FW, RxD separate mode is not supported for NASTT */
  /* Range: 0, 1, 2, 3, 4, 6 */
  uint8   srchNASTTTaskMode;
  /* 0 to select antenna 0 and 1 to select antenna 1, this only applies in non-RxD mode */
  /* Range:  0-1 */
  boolean srchNASTTAntSel;
  /* Peak detect mode for NAS-TT, 00: normal, 01: Bypass, 10: Shoulder
   * Note: normal mode and shoulder mode is not supported in current FW for NASTT */
  /* Range:  0-2 */
  mcalwcdma_srch_pd_type srchNASTTPdMode;
  /* NAS-TT search latest finish time */
  /* Range: 0-65535 */
  uint16  srchNASTTLateTime;
  /* Energy threshold for NAS-TT peaks */
  /* Range: 0-65535 */
  uint16  srchNASTTEnrgTh;
  /* Coherent integration length in NAS-TT */
  /* Range: 0-10239 */
  uint16  srchNASTTCohIntgLen;

} mcalwcdma_srch_nastt_async_parms_struct_type;

/* PN async parms */
typedef struct
{
  /* Number of peaks reported per task in the result buffer */
  /* Range: 0-7 */
  uint8 srchPNNumResPerTask;
  uint8 srchPNNumResPerTaskFatPath;
  /* Range: 0-2 
   * value of 2 tells FW to schedule interf PN on frozen samples
   * value of 1 tells FW PN scheduler to go back 1 slot instead of 2 slots which is default behavior
   * value of 0 will keep FW PN scheduler default behavior
   */
  uint8 srchPNIdleInterfFlag;
} mcalwcdma_srch_pn_async_parms_struct_type, 
  mcalwcdma_srch_cm_pn_async_parms_struct_type;

/* OTDOA async parms */
typedef struct
{
  uint8 srchOTDOANumResPerTask;
} mcalwcdma_srch_otdoa_async_parms_struct_type;

/* Common async parms */
typedef struct
{
  /* 0: No action; 
   * 1: Step1 search freeze samples in the middle of noncoherent samples; 
   * 2: Nastt search unfrozen samples after it’s done; 
   * 3: PN search unfrozen sampels before its execution; 
   * 4: PN search unfrozen samples after it’s done */
  /* Range:  0-4 */
  uint32 srchSSFreezeFlag;
  /* 1. For step1, this is the array of frequency hypothesis to be 
   * applied to the rotator, the array size is 2 to support up to 
   * 2 frequency hypothesis at the same time
   * 2. For NASTT and PN, if numTcxoFreqHypothesis is set to 1, 
   * tcxoFreqHypothesisHw[0] is the frequency error to be applied to rotator */
  /* Range:  -32768-32767 */
  int32 freqHypothesis[3];
  /* Enable frequency hypothesis for search if nonzero
   * 1. For Step1 search, the number indicates how many frequency 
   * hypothesis will be tested in step1, maximum is 2
   * 2. For NASTT and PN search this value can only be either 0 or 1. */
  /* Range:  0-3 */
  uint32 numFreqHypothesis;
  /* 0: No action; 
   * 1: NASTT uses frozen samples in initial acquisition;
   * 2: PN uses frozen samples */
  /* Range: 0-2 */
  uint32 srchUseFreezeFlag;
  /*! 0: Do not program FreqHypothesis to RxFE rotator 
      1:  Program FreqHGypothesiss toRxFE to RxFE rotator */
  uint32 programFreqtoRxFE;
  /*! The frequency error that should be applied to RxFE rotator in ACQ search */
  int32 FreqHypothesistoRxFE; 
  /* 0: No action; 
   * 1: Apply step1 scaling/rounding according to RxAGC value; */
  /* Range: 0 - 1 */
  uint32 srchSclEnable;
	/* 0: No action; 
      1: Step1 stops RF after step1 search is complete */
  uint32 srchStopRFFlag;

} mcalwcdma_srch_common_async_parms_struct_type;

/* NBR async parms */
typedef struct
{
  /* Range:  */
  uint32 rotatorFreq;
  /* Range:   */
  uint8 accumBpg;
  /* Range:   */
  int8 threshold1;
  /* Range:  */
  int8 threshold2;
}mcalwcdma_srch_nbrej_async_parms_struct_type;

/* Searcher async parms */
typedef union
{
  mcalwcdma_srch_step1_async_parms_struct_type step1;
  mcalwcdma_srch_nastt_async_parms_struct_type nastt;
  mcalwcdma_srch_pn_async_parms_struct_type    pn;
  mcalwcdma_srch_otdoa_async_parms_struct_type otdoa;
  mcalwcdma_srch_common_async_parms_struct_type common;
  mcalwcdma_srch_nbrej_async_parms_struct_type nbrej;
} mcalwcdma_srch_async_parms_type;

/* Compressed mode searcher async parms */
typedef union
{
  mcalwcdma_srch_cm_step1_async_parms_struct_type step1;
  mcalwcdma_srch_cm_pn_async_parms_struct_type    pn;
} mcalwcdma_srch_cm_async_parms_type;

/* PN parms sync task type */
typedef struct
{
  /* OVSF code index: 0-255 */
  uint8   ovsf_index;
  /* STTD: STTD mode 0-1 */
  boolean sttd;
  /* First: 0 = first sub-task in CM mode, 1=otherwise */
  boolean first;
  /* Last: 0=last sub-task in CM mode, 1=otherwise */
  boolean last;
  /* Pd: Peak detect mode (00: normal mode; 01: bypass mode; 10: shoulder mode)
   * Normal mode (Pd = 00): The peak detection is enabled, and only the energy 
   * that is larger than threshold, and also greater than the energy of the 
   * hypothesis half-chip earlier and greater than or equal to the energy of 
   * the hypothesis half-chip later will be considered as a peak. 
   * Bypass mode (Pd = 01): The peak detection is disabled, and all energies 
   * above the threshold will be reported as peaks. 
   * Shoulder mode (Pd = 10): This mode is useful when there is a need to find 
   * a more accurate estimate of PN position. Examples include Positioning 
   * Service and PICH in the sleep mode. The searcher hardware returns up 
   * to 2 sets of peaks per search window (each peak set consists of one peak 
   * and its 2 adjacent peaks in chipx2 resolution). The DSP interpolates these 
   * chipx2 resolution peaks to chipx8 resolution. 
   */
  uint8   pd;
  /* Nc:in 64 chipx1 unit, range is 0-127, the coherent accumulation length 
   * will be (Nc+1)*64 chipx1 
   */
  uint8   nc;

  /* Antenna selection in the PN search, 0 for Ant0, 1 for Ant1 */
  uint8 srchPNAntSel;

  /* Carrier selection */
  mcalwcdma_carr_selection_enum_type  srchPNCarrSel;

  /* RxD: RxD mode, 0: no Rxd 1: Rxd seperate mode 2: Rxd add mode */
  mcalwcdma_srch_rxd_mode_enum_type   rxd;
  /* Nn: range is 0-127, the noncoherent integration length will be Nn+1 */
  uint8   nn;
  /* Scrambling code index: the scrambling code used for this PN search, 0-8176 */
  uint16  sc_index;
  /* Window size: the window size for PN search to search for the peak, in chipx1 */
  uint16  win_size_cx1;
  /* Window starting position: the start position for PN search, in chipx1 */
  uint32  win_start_pos;

  /* Position LSW */
  uint8   position_LSW;
} mcalwcdma_srch_pn_task_struct_type;

/* Step 1 sync parms */
typedef struct
{
  /* Carrier select bit 0- primary, 1-secondary */  
  uint8 carrierSel[MAX_PARALLEL_SRCH];
  uint8 carrierCount;
  uint8 dummy;
} mcalwcdma_srch_step1_sync_parms_struct_type, 
  mcalwcdma_srch_cm_step1_sync_parms_struct_type;

/* NASTT sync parms */
#define MCALWCDMA_SRCH_NASTT_CMD_NUM_PEAKS_MAX 12
#define MCALWCDMA_SRCH_NASTT_CMD_NUM_SC_MAX 32

typedef struct
{
  /* Async: 1: NASTT Asynchronous set search 0: NASTT unmonitored set search */
  boolean async;
  /* N : Number of tasks (N<=12) */
  uint8   number_of_peaks;
  /* K: Number of SC index (K<=32) */
  uint8   number_of_sc_index;
  /* SC index n: the scrambling code used for this NASTT search, 
   * there are a maximum of 32 scrambling codes for NAS-TT search in 
   * asynchrnous search mode, in unmonitored/acquisition search mode, 
   * the 32 SC index input will be ignored, the range is 0-511 
   */
  /* Carrier select bit 0- primary, 1-secondary */
  uint8 carrierSel[MCALWCDMA_SRCH_NASTT_CMD_NUM_PEAKS_MAX]; 
  uint16  scrambling_code[MCALWCDMA_SRCH_NASTT_CMD_NUM_SC_MAX];
  /* Index of the step1 results picked for NASTT search */
  uint16 step1_peak_idx[MCALWCDMA_SRCH_NASTT_CMD_NUM_PEAKS_MAX];
  /* Step 1 peak position */
  uint32  peak_pos[MCALWCDMA_SRCH_NASTT_CMD_NUM_PEAKS_MAX];
} mcalwcdma_srch_nastt_sync_parms_struct_type,
  mcalwcdma_srch_cm_nastt_sync_parms_struct_type;

#define SRCH_STEP1_SYNC_PARAMS_CLEAR() (srch_step1_sync_parms.carrierCount = 0)
#define SRCH_STEP1_SYNC_PARAMS_ADD_CARRIER(carr_idx) (srch_step1_sync_parms.carrierSel[srch_step1_sync_parms.carrierCount++] = carr_idx)

#define SRCH_STEP1_RESET_PEAK_IDX() {srchsched_db.step1_nastt_params.cur_psc_peak_indx[CARR_0] = \
									 srchsched_db.step1_nastt_params.cur_psc_peak_indx[CARR_1] = 0;\
									 if(WL1_MAX_NUM_CARRIER_IDX==3) srchsched_db.step1_nastt_params.cur_psc_peak_indx[CARR_2] = 0;}
/* PN sync parms */
#define MCALWCDMA_SRCH_PN_CMD_NUM_TASKS_MAX     64

/* Max 64 tasks at this time. Also no RxD. So,
 * max_peaks for CM = 64 tasks * 6 peaks
 */
#define MCALWCDMA_SRCH_CM_PN_CMD_NUM_PEAKS_MAX (MCALWCDMA_SRCH_PN_CMD_NUM_TASKS_MAX * 6)

/* We can schedule upto a maximum of 64 tasks at a time. This is when
 * using no RxD, normal mode. Each task then can have upto 6 peaks. Thus 
 * max number of peaks is 64*6. For Rxd, since number of peaks increase to 12,
 * we can schedule only 32 tasks, still max peaks will be same, ie 32*12.
 * With fatpath feature, we have 18 peaks per task, so 76*18. 
 */
#define MCALWCDMA_SRCH_PN_CMD_NUM_PEAKS_MAX    1368

typedef struct
{
  /* N : Number of tasks 
   * Assuming number of peaks for each task is 8
   * 1. in normal mode, maximum is 64 
   * 2. in shoulder mode, maximum is 21 
   * 3. in separate RxD plus shoulder mode, maximum is 10 
   * 4. in separate RxD plus normal mode, maximum is 32 
   */
  uint8   num_tasks;
  /* HQ: Set to 1 if the PN search needs to bypass the FW scheduler */
  boolean hQ;

  /* Latest finish time: the latest start time for PN search to finish, 
   * FW sends a timeout interrupt to SW if the search expires */
  uint16  latest_finish_time;
  /* Earliest start time: the earliest start time for PN search */
  uint16  earliest_start_time;
  mcalwcdma_srch_pn_task_struct_type task[MCALWCDMA_SRCH_PN_CMD_NUM_TASKS_MAX];
} mcalwcdma_srch_pn_sync_parms_struct_type,
  mcalwcdma_srch_otdoa_sync_parms_struct_type,
  mcalwcdma_srch_cm_pn_sync_parms_struct_type;

/* Searcher sync parms */
typedef union
{
  mcalwcdma_srch_step1_sync_parms_struct_type step1;
  mcalwcdma_srch_nastt_sync_parms_struct_type nastt;
  mcalwcdma_srch_pn_sync_parms_struct_type    pn;
  mcalwcdma_srch_otdoa_sync_parms_struct_type otdoa;
} mcalwcdma_srch_sync_parms_type;

/* Searcher compressed mode sync parms */
typedef union
{
  mcalwcdma_srch_cm_step1_sync_parms_struct_type step1;
  mcalwcdma_srch_cm_pn_sync_parms_struct_type    pn;
} mcalwcdma_srch_cm_sync_parms_type;

/* Searcher step 1 peak result type */
typedef struct
{
  /* frequency hypothesis associated with this peak result, 
   * only used in initial acquisition search  */
  uint8  freq_index;
  /* the non-coherent energy, 16 bit wide */
  uint16 eng;
  /* Reported in unit of chipx8. Range is 0-20476 (step1 is searching on chipx2 
   * resolution, the range of PSC position is thus from 0 to 5119, which 
   * translates into 0 to 20476 in chipx8) */
  uint16 peak_pos_cx8;
} mcalwcdma_srch_step1_result_peak_type;

/* Searcher step 1 result type */
typedef struct
{
  /* Number of non-coherent integration lengths completed in this step1 search 
   * by FW. Used to determine if all scheduled tasks were completed */
  uint8 num_non_coh_int_completed;
  /* Number of peaks found. Range is 0-64 in Rxd Add or normal mode, 
   * 0-32 in Rxd separate mode. */
  uint8 num_peaks_primary;
  /* Number of peaks found for second antenna (this field is only valid 
   * when RxD is set to 1). Range is 0-32 */
  uint8 num_peaks_secondary;
  /* One bit per search command, set to 0, when searching results are not 
   * obtained through RxD mode, 1 when search results are obtained through 
   * RxD separate mode, 2 when search results are obtained through RxD add mode */
  mcalwcdma_srch_rxd_mode_enum_type rxd;
  /* Step 1 peak result */
  mcalwcdma_srch_step1_result_peak_type peak[64];
} mcalwcdma_srch_step1_result_buffer_type,
  mcalwcdma_srch_cm_step1_result_buffer_type;

/* Searcher NASTT task result type */
typedef struct
{
  /* 16 bit non-coherent accumulation energy, if the energy is 0, 
     it’s an invalid peak */
  uint16 eng;
  /* Scrambling code index: 0-512 */
  uint16 sc_index;
  /* peak position returned by NAS-TT */
  uint32 peak_pos_cx8;
  /* carrier index of each task, when parallel nastt is issued of 2 carriers  */
  uint8 carrier_idx;
} mcalwcdma_srch_nastt_result_task_type;

/* Searcher NASTT result type */
typedef struct
{
  /* Number of tasks finished, each task corresponds to one step1 peak */
  uint8 total_num_results;
  /* One bit per search command, set to 0, when searching results are not 
   * obtained through RxD mode, 1 when search results are obtained 
   * through RxD separate mode, 2 when search results are obtained through RxD add mode */
  mcalwcdma_srch_rxd_mode_enum_type rxd;
  /* Nastt results */
  mcalwcdma_srch_nastt_result_task_type task[64];
} mcalwcdma_srch_nastt_result_buffer_type,
  mcalwcdma_srch_cm_nastt_result_buffer_type;

/* Searcher PN task result type */
typedef struct
{
  /* One bit per search command, set to 0, when searching results are from 
   * antenna 0, set to 1, when searching results are from antenna 1. */
  boolean div;
  /* One bit per search command, set to 0, when searching results are not 
   * obtained through RxD mode, 1 when search results are obtained through 
   * RxD separate mode, 2 when search results are obtained through RxD add mode */
  mcalwcdma_srch_rxd_mode_enum_type rxd;
  /* 16bit non-coherent accumulation energy, if the energy value is 0, 
   * it’s an invalid peak result */
  uint16 eng;
  /* Reported as a combination of 3-bit LSW and 16-bit MSW in unit of chipx8 */
  uint32 peak_pos;
} mcalwcdma_srch_pn_result_task_type;

/* Searcher PN result type */
typedef struct
{
  /* Number of tasks finished, should be equal to N specified in the PN input buffer */
  uint8 num_tasks;
  /* Array that contains how many results in a task */
  uint8 num_results_in_task[MCALWCDMA_SRCH_PN_CMD_NUM_TASKS_MAX]; /* Max 64 tasks */
  /* PN results */
  mcalwcdma_srch_pn_result_task_type result[MCALWCDMA_SRCH_PN_CMD_NUM_PEAKS_MAX];
} mcalwcdma_srch_pn_result_buffer_type,
  mcalwcdma_srch_otdoa_result_buffer_type;

/* CM Searcher PN result type */
typedef struct
{
  /* Number of tasks finished, should be equal to N specified in the PN input buffer */
  uint8 num_tasks;
  /* Array that contains how many results in a task */
  uint8 num_results_in_task[MCALWCDMA_SRCH_PN_CMD_NUM_TASKS_MAX]; /* Max 64 tasks */
  /* PN results */
  mcalwcdma_srch_pn_result_task_type result[MCALWCDMA_SRCH_CM_PN_CMD_NUM_PEAKS_MAX];
} mcalwcdma_srch_cm_pn_result_buffer_type;

/* Searcher result buffer */
typedef union
{
  mcalwcdma_srch_step1_result_buffer_type step1;
  mcalwcdma_srch_nastt_result_buffer_type nastt;
  mcalwcdma_srch_pn_result_buffer_type    pn;
  mcalwcdma_srch_otdoa_result_buffer_type otdoa;
} mcalwcdma_srch_result_buffer_type;

/* Searcher compressed mode result buffer */
typedef union
{
  mcalwcdma_srch_cm_step1_result_buffer_type step1;
  mcalwcdma_srch_cm_pn_result_buffer_type    pn;
} mcalwcdma_srch_cm_result_buffer_type;


extern mcalwcdma_srch_step1_sync_parms_struct_type srch_step1_sync_parms;
extern mcalwcdma_srch_nastt_sync_parms_struct_type srch_nas_tt_sync_parms;
extern mcalwcdma_srch_pn_sync_parms_struct_type srch_pn_sync_parms;

/* --------------------------------------------------------------------------
   MCAL SRCH APIs external functions
   --------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION   mcalwcdma_srch_init

DESCRIPTION
  This is MCAL WCDMA Search initialization function. This function should be
  called before calling any other mcal apis. Currently this is called when 
  wcdma mode is entered. The following operations are done in this function.
   - Initialize the state variables related to searcher module.
   - Install the interrupt handlers with mdsp services
   - Define timers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_srch_init( void ); 

/*===========================================================================
FUNCTION   mcalwcdma_srch_cm_init

DESCRIPTION
  This is MCAL WCDMA Search CM initialization function. This function should be
  called before calling any other mcal apis. This function will:
   - Initialize the state variables related to searcher module.
   - Install the interrupt handlers with mdsp services
   - Define timers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_srch_cm_init( void );

/*===========================================================================
FUNCTION   mcalwcdma_srch_set_status_callback_fn

DESCRIPTION
  This function installs the callback functions for a speific search type.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - successful installation of the callback function
  FALSE - callback function installation failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_srch_set_status_callback_fn
(
  mcalwcdma_srch_type id, 
  mcalwcdma_srch_status_callback_fn_type cb_fn
);

/*===========================================================================
FUNCTION   mcalwcdma_srch_cm_set_status_callback_fn

DESCRIPTION
  This function installs the compressed mode callback functions for a specific
  search.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE - successful installation of the callback function
  FALSE - callback function installation failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_srch_cm_set_status_callback_fn
(
  mcalwcdma_srch_cm_type id, 
  mcalwcdma_srch_cm_status_callback_fn_type cb_fn
);

/*===========================================================================
FUNCTION   mcalwcdma_programfreqtorxfe_reset

DESCRIPTION
  This function resets the programtorxfe flag in firmware interface.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/

extern void mcalwcdma_programfreqtorxfe_reset(void);

/*===========================================================================
FUNCTION   mcalwcdma_srch_update_srch_async_common_parms

DESCRIPTION
  This function updates shared interface with common async parameters.

  mcalwcdma_srch_async_parms_type - parameters to be updated to shared memory.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - parameters updated
  FALSE - parameter update failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_srch_update_srch_async_common_parms
(
  mcalwcdma_srch_async_parms_type * parms,
  srch_intraf_queue_enum_type queue_to_update
);

/*===========================================================================
FUNCTION   mcalwcdma_srch_update_srch_async_parms

DESCRIPTION
  This function updates shared memory with the async parameters. The parameters 
  are vaildated before updating.

  mcalwcdma_srch_type - indicates the search type the parameters belong to.
  mcalwcdma_srch_async_parms_type - parameters to be updated to shared memory.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - parameters updated
  FALSE - parameter update failed
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_srch_update_srch_async_parms
(
  mcalwcdma_srch_type id, 
  mcalwcdma_srch_async_parms_type *parms
);

/*===========================================================================
FUNCTION   mcalwcdma_srch_cm_update_srch_async_parms

DESCRIPTION
  This function updates shared memory with the async parameters.

  mcalwcdma_srch_cm_type - indicates the search type the parameters belong to.
  mcalwcdma_srch_cm_async_parms_type - parameters to be updated to MDSP memory.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE - parameters updated
  FALSE - parameter update failed
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_srch_cm_update_srch_async_parms
(
  mcalwcdma_srch_cm_type id, 
  mcalwcdma_srch_cm_async_parms_type *parms,
  uint8 cmd_queue
);

/*===========================================================================
FUNCTION   mcalwcdma_srch_do_srch

DESCRIPTION
  This function first updates shared memory with the sync parameters and then requests
  MDSP sync interface to send the search command.

  mcalwcdma_srch_type - identifies the search type.
  mcalwcdma_srch_sync_parms_type - contains the sync parameters.
  update_buffer - will update shared memory with sync parameters only if this is true.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - search request success
  FALSE - search request failed
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_srch_do_srch
(
  mcalwcdma_srch_type id,
  mcalwcdma_srch_sync_parms_type * sync_parms
);

/*===========================================================================
FUNCTION   mcalwcdma_srch_do_srch_new

DESCRIPTION
  This function first updates shared memory with the sync parameters and then requests
  MDSP sync interface to send the search command.

  mcalwcdma_srch_type - identifies the search type.
  mcalwcdma_srch_sync_parms_type - contains the sync parameters.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - search request success
  FALSE - search request failed
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_srch_do_srch_from_sched
(
  mcalwcdma_srch_type id,
  mcalwcdma_srch_sync_parms_type * sync_parms
);

/*===========================================================================
FUNCTION   mcalwcdma_proc_srch_done

DESCRIPTION
   This function is called in L1 task context. It resets the "srch_pending" 
   flags, as well as the "cur_id" in srch state db.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mcalwcdma_proc_srch_done(srch_intraf_queue_enum_type srch_queue_idx);

/*===========================================================================
FUNCTION   mcalwcdma_srch_do_pending_srch

DESCRIPTION
  This function schedules any pending search that might have been held up, waiting
  for an abort or previous search to complete.

  mcalwcdma_srch_pending - whether a search is pending
  mcalwcdma_srch_pending_id - identifies the search type.
  mcalwcdma_sync_parms_pending - contains the sync parameters.
  mcalwcdma_srch_update_buffer_pending - updates shared memory with sync parameters only 
                                         if this is true.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_srch_do_pending_srch(void);

/*===========================================================================
FUNCTION   mcalwcdma_srch_cm_do_srch

DESCRIPTION
  This function first determines which compressed mode command queue to use.
  Then it updates mdsp cmd queue with the sync parameters and then requests
  MDSP sync interface to send the compressed mode search command.

  mcalwcdma_srch_cm_type - identifies the compressed mode search type.
  mcalwcdma_srch_cm_sync_parms_type - contains the sync parameters.
  update_buffer - will update shared memory with sync parameters only if this is true.
  first - 0 = first sub-task (non-coherent integration starts), 1 = otherwise.
  last - 0 = last sub-task (non-coherent integration ends), 1 = otherwise.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE - search request success
  FALSE - search request failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_srch_cm_do_srch
(
  mcalwcdma_srch_cm_type id,
  mcalwcdma_srch_cm_sync_parms_type *sync_parms,
  boolean first,
  boolean last,
  uint8 queue_idx
);

/*===========================================================================
FUNCTION   mcalwcdma_srch_get_srch_results

DESCRIPTION
  This function copies the search results into the pointer that is passed.

  mcalwcdma_srch_type - identifies the search type.
  mcalwcdma_srch_result_buffer_type - pointed to where the data should be copied.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE - search data copied into buffer
  FALSE - search data copy failed
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_srch_get_srch_results
(
  mcalwcdma_srch_type id, 
  mcalwcdma_srch_result_buffer_type *buffer
);

/*===========================================================================
FUNCTION   mcalwcdma_srch_cm_get_srch_results

DESCRIPTION
  This function copies the compressed mode search results into the pointer 
  that is passed.

  mcalwcdma_srch_cm_type - identifies the search type.
  mcalwcdma_srch_cm_result_buffer_type - pointed to where the data should be copied.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_srch_cm_get_srch_results
(
  mcalwcdma_srch_cm_type id, 
  mcalwcdma_srch_cm_result_buffer_type *buffer,
  uint8 queue_idx
);

/*===========================================================================
FUNCTION   mcalwcdma_srch_abort

DESCRIPTION
  This function resets the state variables and sends an abort command to dsp to
  reset all the on going searches.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - search is aborted
  FALSE - search abort failed

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_srch_abort(void);

/*===========================================================================
FUNCTION   mcalwcdma_is_srch_abort_pending

DESCRIPTION
  This function indicates if a SRCH abort is pending.

DEPENDENCIES
  None

RETURN VALUE
  TRUE -  Search abort in progress
  FALSE - otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_is_srch_abort_pending(void);

/*===========================================================================
FUNCTION   mcalwcdma_set_srch_abort_pending

DESCRIPTION
  This function sets the "mcalwcdma_srch_aborted" to 
  1 - at the onset of srch abort
  0 - when srch abort is completed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_set_srch_abort_pending(boolean val);

/*===========================================================================
FUNCTION   mcalwcdma_srch_cm_done_isr

DESCRIPTION
   This function is the interrupt handler for Searcher for compressed mode 
   Step 1 and PN searches.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_srch_cm_done_isr (void);

/*===========================================================================
FUNCTION mcalwcdma_srch_get_avail_cm_cmd_q

DESCRIPTION
  This function finds the available Q for search and return its index. If no
  Q is available then it return MCALWCDMA_SRCH_CM_INVALID_CMQ (0xFF)

DEPENDENCIES
  None

RETURN VALUE
  Q index or MCALWCDMA_SRCH_CM_INVALID_CMQ (0xFF) if none available.

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 mcalwcdma_srch_get_avail_cm_cmd_q(void);

/*===========================================================================
FUNCTION   mcalwcdma_srch_deactivate

DESCRIPTION
  This function should be called when the MCAL driver is no longer used.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_srch_deactivate(void);

/*===========================================================================
FUNCTION   mcalwcdma_srch_reset_srch

DESCRIPTION
  This function resets the MCAL search state and search type.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_srch_reset_srch( srch_intraf_queue_enum_type srch_queue_idx );

/*===========================================================================
FUNCTION   mcalwcdma_is_srch_state_active

DESCRIPTION
  This function returns the state of the MCAL search.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_is_srch_state_active(void);

/*===========================================================================
FUNCTION   MCALWCDMA_IS_PENDING_SRCH_STATE_ACTIVE

DESCRIPTION
  This function returns the state of the stored pending MCAL search.

DEPENDENCIES
  None

RETURN VALUE
  True if a search is pending, false otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_is_pending_srch_state_active(void);

/*===========================================================================
FUNCTION   MCALWCDMA_RESET_PENDING_SRCH_TYPE

DESCRIPTION
  This function sets the type of the pending offline callback search.

DEPENDENCIES
  None

RETURN VALUE
  True if a search is pending, false otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_reset_pending_srch_type(void);


/*===========================================================================
FUNCTION   mcalwcdma_is_srch_queue_active

DESCRIPTION
  This function returns the state of the MCAL search accrording to queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mcalwcdma_is_srch_queue_active(srch_intraf_queue_enum_type queue);

/*===========================================================================
FUNCTION   mcalwcdma_get_q0_cur_srch_id

DESCRIPTION
  This function returns the current search id  of the MCAL search for Queue-0.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
mcalwcdma_srch_type mcalwcdma_get_q0_cur_srch_id(void);

/*===========================================================================
FUNCTION   mcalwcdma_get_q1_cur_srch_id

DESCRIPTION
  This function returns the current search id  of the MCAL search for Queue-1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

mcalwcdma_srch_type mcalwcdma_get_q1_cur_srch_id(void);

/*===========================================================================
FUNCTION   mcalwcdma_srch_update_srch_async_common_parms

DESCRIPTION
  This function updates shared interface with common async parameters.

  mcalwcdma_srch_async_parms_type - parameters to be updated to shared memory.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - parameters updated
  FALSE - parameter update failed

SIDE EFFECTS
  None
===========================================================================*/

boolean mcalwcdma_srch_update_nbrej_common_parms
(
  mcalwcdma_srch_async_parms_type * values
);

/*===========================================================================
FUNCTION   MCALWCDMA_ADD_SRCH_HIST
DESCRIPTION
  This function will update mcal search history buffer 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_add_srch_hist(mcalwcdma_debug_srch_type srch_type);

/*===========================================================================
FUNCTION   mcalwcdma_srch_update_nastt_usefreezeflag_intraf_acq

DESCRIPTION
  This function updates the UseFreeFlag to 0 for NASTT in INTRAF acq mode i.e.
  when scheduler is in INTRAF mode in ACQ state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_srch_update_nastt_usefreezeflag_intraf_acq(void);

#endif /* __MCALWCDMA_SRCH_H__ */

