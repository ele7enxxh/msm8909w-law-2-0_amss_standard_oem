#ifndef SRCH_PARM_H
#define SRCH_PARM_H
/*===========================================================================
   FILE: srch_parm.h

GENERAL DESCRIPTION:
      This header file contains parameter definitions used by search module.
      
      
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c)2000-2012 by Qualcomm Technologies, Inc. All Rights Reserved.
      
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchparm.h_v   1.34   11 Jul 2002 21:30:46   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchparm.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/08/16   rn      Changes to implement the algorithm to optimize the performance for 
                   x2w resume acq post LTA cases
02/03/15   vn      New searcher thresholds for Jolokia 256*30 Add RxD step1.
12/17/14   vn      Changes for new searcher thresholds for Jolokia.
10/10/14   amj     Code Cleanup.
08/20/14   amj     Correct full search period for CPC for all cycle lengths.
08/12/14   vn      Defined few parameters for Narrow Band Rejection log packet.
05/15/14   jd      Support for 3C searches in cpc-drx.
05/28/14   amj     MCVS changes for VPE clock
07/01/14   amj     Add full search timer for non-DCH INTRAF state
11/14/13   jd      Sw/Fw interface changes for searcher two queue approach.
11/06/13   gj      Added 3C searcher code
09/12/13   jd      Searcher redesign
07/23/13   jd      parallel step1 changes
04/11/13   gsk    Idle Mode FR 2741/2743, reducing WL1 wait time between rf_tune_to_chan
                   and search cmd to FW. Also adding support for interf offline PN
02/05/13   vn      Defined parameters to schedule a cell for SFN decoding when PN 
                   position of cell moves by more than 256 chips and remove cell 
                   from DROP_C when the cell is bad for 1 sec.
01/31/13   ag      Added new reporting threshold. 
11/19/12   amj     Supporting imbalanced RxD step 1 search
11/07/12   gsk     Adding NN values for step1 search
09/10/12   gsk     Creating variables for step1 parameters
07/10/12   ag      Reduced the threshold for USET detection from -18dB to -22dB.07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/13/12   sv      Fixed the non-acq NASTT thresholds.
03/10/12   sv      Corrected event evaluation threshold for intrafrequency cells.
03/08/12   sv      Added definition SRCH_RX_MIN_AGC_VAL.
02/10/12   rgn     Nikel feature cleanup
02/01/12   gnk     Nikel Feature cleanup.
01/27/12   vs      nikel feature cleanup.
01/26/12   zr      Mainlined FEATURE_WCDMA_NO_MRM_FOR_WEAK_NEIGHBORS
01/07/12   zr      Increasing SRCH_BYPASS_MODE_MAX_PN_TASKS to 64
12/09/11   mmr     Update Qxt path thresholds.
12/07/11   sv      Added changes to support Fatpath algorithm in connected mode.
11/11/11   sv      Frames to be reserved for Step1 searches during DCH
09/13/11   sv      Increase max no of intraf events supported to 9.
09/03/11   sv      Mainlined FEATURE_RXAGC_10_BIT.
06/10/11   sv      Added max ref age define for DC sync neighbor.
05/09/11   sv      Added and modified values for frames reserved during DCH.  
03/11/11   dp      Ported CPC DRX changes to offline Architecture
02/14/11   sv      Added NASTT threshold for DC cells.
01/31/11   sv      Added new freq hypo defines to be used during ACQ.
10/19/10   sv      Added changes to support full search for DC and QICE support.
08/16/10   sv      Inverted the previously incorrect thresholds for TD and no TD cells during HHO.
03/17/10   sv      Added threshold for limited window HHO search.
01/25/10   sv      Renamed few defines.
09/04/09   sv      Added search thresholds for GTW Full and limited window searches. 
08/14/09   sv      Support for enabling RxD on Step1, NASTT searches in DCH.
04/13/09   sv      Modified macros relating to number of frames reserved by NASTT
                   searches during compressed mode.
04/03/09   sv      Added new thresholds for reacq searches.
03/17/09   sv      Removed seperate pullin threshold for IRAT.
03/13/09   sv      Update non ACQ step1 threshold.
03/03/09   sv      Added new defines for max ref_age and redetect_age.
02/19/09   rgn     Changed pullin thresholds based on system team recommendation
01/21/09   sv      Added PN search threshold in ACQ state.
01/20/09   sv      Corrected some search thresholds + code cleanup.
12/12/08   sv      Removed SRCH_MAX_PULLIN_PILOTS.
10/03/08   hk      Bringing in 7k Mailine fixes
08/08/08   nd      SRCH_ANT_SEL_PRIMARY and SRCH_ANT_SEL_DIV macro updated
07/26/08   mg      Correct aset nset search periods
06/10/08   ms      Full-window PN search support
01/15/08   sv      Added support for (256,16) searches when in STTD mode in DCH. 
                   Changes under the FEATURE_STTD_OPTIMIZATION feature flag.
09/26/07   nd      added params for number of slots and thres to be sent to fw
                   for finding out if the signal is narrowband or not
08/22/07   nd      Added params for max ref_age for syn_n and uset cells.
07/19/07   scm     Increase Step1 threshold from 2987 to 3600 for initial ACQ.
07/13/07   sv      Tuned DRX NM, HM switching thresholds to fix ecio fluctuation
                   issues. Also removed STTD switch threshold defines. 
06/28/07   rc      Modified SRCH_FREQ_SCAN_RAW_SCAN_STEP from 3.2 MHz to 3MHz 
                   when FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2 is defined
03/05/07   sk      Added code for FEATURE_FLEX_FREQ_BIN and 
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2 features.
02/28/07   cpe     Reduce the wait time for forced step1, step2 searches
                   by 100ms when CM gaps are active.
02/22/07   nd      Modified the min RSCP to 121 which correspond to delta RSCP -5
01/22/07   nd      Added event 1J support under FEATURE_WCDMA_HSUPA
11/01/06   cpe     Mainlined the following features:
                    -FEATURE_STTD_CAPABLE
10/26/06   cpe     Mainlined a bunch of features
07/06/06   mg      Mainline FEATURE SLP CLK PHASE 2 CAL
06/30/06   sv      Updated AGC log packet feature define.
05/02/06   nd      Defined SRCH_WEAK_NBR_THRESH_IDX for checking neighbors
                   below -22 so that those does not get evaluated.
04/11/06   asm     Added definition for new AGC log packet 0x4728
03/16/06   bbd     Added support for FEATURE_SHALLOW_BUCKSHOT_SEARCH.
03/15/06   rc      Defined SRCH_STEP1_FALSE_PEAK_ENG_THRESH and 
                   SRCH_STEP1_NUM_FALSE_PEAK_THRESH for step1 false peak detection.
03/15/05   scm     Replace SRCH_FREQ_OFFSET_DELTA with SRCHACQ_FREQ_OFFSET_PPM
                   to generate freq offset based on carrier frequency.
03/07/06   bbd     Moved SRCH_STTD_COH_LEN_FOR_LOGGING and 
                   SRCH_STTD_NON_COH_LEN_FOR_LOGGING into this file.
11/08/05  gv/nd    Added code for WtoW BPLMN feature.
11/01/05   bbd     Added DRX specific list search parameters.
10/28/05   gs      Added support for NV based Rel5/R99 functionality
10/19/05   vb/cpe  Undid the prev change re SRCH_NSET_SRCH_PERIOD
10/04/05   vb      change of SRCH_NSET_SRCH_PERIOD value from 80 msec to 20msec
09/19/05   cpe     Expanded RSCP lower range to -120 dbm under FEATURE_REL5
09/13/05   sk/nd   Detected Cells Support
08/08/05   rc      Changed SRCHACQ_MAX_VERIFY_SRCH to 0 so as not to do verify
                   searches in any given frequency offset.
08/02/05   rc      Added def for SRCH_MAX_PILOT_PEAKS_FOR_SIX_CELLS and
                   SRCHACQ_NUM_STEP1_PEAKS_TO_STEP2.
07/12/05   ub      Modified constant num frames required in FACH for step 1,
                   2 and 3 search when FACH meas oksn are configured
05/10/05   ub      Added path thresholds for detecting unlisted cells in FACH
03/01/05   cpe     Added SRCH_EVT_EVAL_THRESH_IDX 
02/25/05   asm     Added new AGC log packet 0x4165 sampled at 10ms rate
02/08/05   cpe     Added SRCH_NBR_SFN_READ_THRESH_IDX and set it to 34
01/12/04   rc      Updated the list search HM and NM thresholds.
11/11/04   rc      Added frequency scan thresholds for BPLMN support under 
                   FEATURE_GTOW_BACKGROUND_PLMN_SEARCH feature.
11/11/04   scm     Change SRCH_RAT_NON_BLIND_ENERGY_THRESHOLD to 2400.
10/28/04   rc      Updated search step 3 Non-acquisition thresholds and STTD List
                   search parameters.
10/06/04   scm     Define SRCH_RAT_NON_BLIND_ENERGY_THRESHOLD to 8000.
10/02/04   cpe     Added Normal mode and Highmode definitions for 6250
08/19/04   cpe     Added definitions for max and min pathloss
08/01/04   cpe     Reverted the RAT switch acq parameters.
06/30/04   cpe     RAT switch acquisition list search parameters changed
06/15/04   cpe     Changed FS thresholds for service request/PLSM search types
                   Also changed Min Ec/No and RSCP values.
06/07/04   cpe     Moved cell path threshold indices to outside feature defs
                   since these are common with and without STTD.
06/07/04   cpe     STTD related parameters have been checked and consolidated
06/04/04   cpe     Changed SRCH_FREQ_SCAN_AGC_THRESH to -103 when the feature
                   FEATURE_FREQ_SCAN_OPTIMIZATION is defined
05/04/04   src     Upped the searcher STTD threshold to not allow too many
                   spurious peaks and added non-STTD parameters for butterfly.
04/23/04   sh      Changed timer SRCH_LOST_STEP1_SRCH_PERIOD to 10 frames.
03/10/04   gs      Added macro SRCHDCH_NUM_REQED_FRAME_FOR_STEP1_2_SRCH
03/10/04   yus     Replace FEATURE_MSM6250_COMPILE with 
                   FEATURE_BUTTERFLY_CORRELATION.
02/26/04   cpe     Introduced SRCHDCH_MAX_STEP1_PEAK_CNT to limit step1 peaks
                   in DCH state
02/02/04   sh      Change SRCH_NUM_REQED_FRAME_FOR_STEP1_2_SRCH from 5 to 10.
12/10/03   cpe     Add FEATURE_WCDMA_INTRAF_SUPPORT_IN_FACH
11/21/03   cpe/src Modified support for STTD capability by having taken into
                   account searcher parameters for two different MSM targets.
11/11/03   yus     Replace FEATURE_6250_COMPILE with 
                   FEATURE_SRCH_BITWIDTH_MULTI_FACTOR
11/03/03   cpe/src Added code for STTD capability (featurised under the feature
                   flag FEATURE_STTD_CAPABLE).
09/17/03   cpe     Changed SRCHDCH_MAX_INTRAF_EVT to 8 to support second instance
                   of 1E and 1F events
09/09/03   m       Modifications to support Inter Freq HHO
07/24/03   yus     Add SRCH_BITWIDTH_MULTI_FACTOR for MSM6250.
07/08/03   cpe     Changed the following parameters
                    SRCH_STEP3_NON_COH_INTG_LEN
                    SRCHACQ_STEP3_THRESH
                   Moved ACQ_PULLIN_PATH_THRESH from dldem to here
05/23/03   cpe     Added def for SRCH_WCDMA_MAX_INTRAF_FILT_SIZE
02/07/03   cpe     Changed several paramters based on systems recommendation
                   SRCH_NONACQ_STEP3_COH_INTG_LEN from 6 to 7
                   SRCHDCH_NSET_STEP1_THRESH from 2460 to 2987
                   SRCHDCH_NSET_STEP2_THRESH from 272  to 365
                   SRCHDCH_NSET_STEP3_THRESH from 3000 to 2412
                   SRCH_STEP1_NON_COH_INTG_LEN from 30 to 45
                   SRCH_STEP1_ENG_THRESH 2460 to 2987
                   SRCH_STEP2_NON_COH_INTG_LEN from 2 to 3
                   SRCH_STEP2_ENG_THRESH from 272 to 365
                   SRCH_STEP3_NON_COH_INTG_LEN from 5 to 10
                   SRCHACQ_STEP3_THRESH from 1200 to 876 
                   SRCH_ASET_NC from 23 to 27 
04/17/03   sh      Set SRCH_HPQ_LEN_FOR_PCH_AND_FACH to 2 (was set to 20).                                                       
02/25/03   asr     Under FEATURE SLP CLK PHASE 2 CAL, define SRCHZZ_ASET_WIN_SIZE to be 192 chips.
01/07/03   ddh     Changed SRCH_FREQ_SCAN_AGC_THRESH back to -105. Another
                   solution was found to the acquisition timing problem that
                   was being worked on. 
01/02/03   ddh     Added SRCH_FINE_FREQ_SCAN_AGC_THRESH to 105 dBm in 1/12 
                   dBm units                    
12/13/02   ddh     Changed SRCH_FREQ_SCAN_AGC_THRESH from -105 to -99 dBm 
11/26/02   ddh/cpe change SRCH_LOST_HPQ_LIST_SRCH_PERIOD from 5 to 20, to allow
                   adequate time when operating in dual mode.
                   Added SRCH_RAT_ACQ_LIST_NC and SRCH_RAT_ACQ_LIST_NN
11/05/02   cpe     Fixed the code to set SRCH_NUM_REQED_FRAME_FOR_STEP1_2_SRCH 
                   to 5 instead of 50
                   Also replaced FEATURE_WCDMA_INTRAF_SRCH_IN_CM with
                   FEATURE_WCDMA_INTRAF_SUPPORT_IN_CM
09/31/02   yus     Add support for FEATURE_WCDMA_INTRAF_SRCH_IN_CM.
07/11/02   ma      Added SRCH_HIGH_MODE_PATH_THRESHOLD and
                   SRCH_NORMAL_MODE_PATH_THRESHOLD for cell reselection.
06/21/02   yus     Added hash-define SRCH_PATH_SATURATED_THRESHOLD
                   and deleted SRCH_FILTER_NOISE_FLOOR and 
                   SRCH_ENG_THRESH_FOR_OUT_OF_SERVICE.
05/18/02   sh      Added parameters for List and step 1/2/3 searches during
                   Cell Reselection.
02/20/02   cpe     Added SRCH_AGC_LOG_PERIOD and SRCHZZ_ASET_WIN_SIZE
11/15/01   asr     Changed SRCHDCH_NSET_STEP3_THRESH to 3000 (from 1500)
11/13/01   cpe     Added  SRCH_NONACQ_STEP3_COH_INTG_LEN and 
                   SRCH_NONACQ_STEP3_NON_COH_INTG_LEN Also modified
                   SRCHDCH_NSET_STEP1(2)(3)_THRESH thresholds
                   Also changed STEP1 and STEP3 thresholds during ACQ
10/15/01   cpe     Changed SRCH_FREQ_SCAN_AGC_THRESH to (105)dBm.
                   Added SRCH_HPQ_LEN_FOR_PCH_AND_FACH  to track the hpq filter 
                   length in PCH and FACH states
09/19/01   wjn     Changed the unit of "SRCH_MIN_CPICH_RSCP" to 0.5dBm.
09/12/01   wjn     Added the filter length parameter "SRCH_WCDMA_RSCP_FILT_LEN"
                   for WCDMA RSSI displayed by UI.
09/10/01   cpe     Added several filtering related paramters.
                   Changed default HPQ and LPQ filter lengths to 2.
                   Changed SRCH_PATH_THRESHOLD to 2309 from 2500
                   (corresponding to -22 dB)
08/30/01   wjn     Added several WCDMA carrier frequency scan related 
                   parameters.
                   Changed Step3 search window size from 32 chips to 64 chips.
08/22/01   cpe     Changed SRCH_MIN_NBR_FILT_LEN from 2 to 1
07/18/01   cpe     changed SRCH_TS_CNT_ADJ  to 0
06/01/01   wjn     Added parameter SRCH_STEP3_PATH_THRESHOLD.
05/20/01   wjn     Added parameters for CPICH RSCP calculation. 
                   Modified some parameters to speed up SRCH SW unit test.
03/17/01   wjn     Defined more parameters for cell reseletion delay timer and
                   throttle timer and for DCH report event evaluation.
02/21/01   wjn     Added parameters for DCH search operation.
01/30/01   wjn     Added SRCHACQ_STEP3_THRESH and SRCHACQ_MAX_VERIFY_SRCH.
01/12/01   mdl     changed SRCH_PATH_THRESHOLD TO 4000
                   changed SRCHBCH_NSET_ENG_THRESH to 9000
                   changed SRCH_STEP1_NON_COH_INTG_LEN TO 30
                   changed SRCH_STEP1_ENG_THRESH to 1500
                   changed SRCH_STEP2_NON_COH_INTG_LEN to 2
                   changed SRCH_STEP3_COH_INTG_LEN to 1
                   changed SRCH_STEP3_NON_COH_INTG_LEN to 5
                   all based on observations made during acquisition in a 
                   fading scenario and from suggested values from systems
                   
01/11/01   wjn     Added more parameters.
11/09/00   wjn     Commented out Step1 search early dump parameters, since DSP
                   does not support early dump currently.
09/25/00   wjn     Created file.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
             CELL Reselection SEARCHER PARAMETERS
--------------------------------------------------------------------------*/

/* Thresholds for List Searches */
#define SRCHCR_LIST_HPQ_ENG_THRESH  2732
#define SRCHCR_LIST_LPQ_ENG_THRESH  2732

/* corresponding to EcIo of -23.5dB */
#define SRCHCR_PN_SRCH_ENG_THRESH_IDX 47

/*--------------------------------------------------------------------------
                Search Operation Control Parameter
--------------------------------------------------------------------------*/
/** Heartbeat count for Active and Candidate Set search,
 *  corresponding to 20ms */
#define SRCHSCHED_ASET_PN_TIMER                  2
/** Heartbeat count for Neighbor Set search, corresponding to 80ms */
#define SRCHSCHED_NSET_PN_TIMER                  8
/* Heartbeat count for FACH reselection evals */
#define SRCHFACH_RESEL_EVAL_TIMER                9


/*==================================================================
      Search step1/2/3 parameters (During acquisition)
=================================================================*/
/* Step 1 Search Async Parameters */

#define SRCH_STEP1_NON_COH_INTG_LEN 45
#define SRCH_STEP1_NON_COH_INTG_LEN_OPT 40
#define SRCH_STEP1_NON_COH_INTG_LEN_OPT2 20
#ifdef FEATURE_WCDMA_CPC_DRX
#define SRCH_DRX_STEP1_NON_COH_INTG_LEN 30
#endif

    /* Maximum Non-coherent integration length for Step_1 search */

#if defined(FEATURE_WCDMA_JOLOKIA_MODEM) || defined(FEATURE_WCDMA_TABASCO_MODEM)

/* For JO/TA threshold is -25dB. energy corresponding to ecio of -25 dB */
#define SRCH_STEP1_ENG_THRESH_IDX 50

/* Step1 energy corresponding to -25 dB */
#define SRCH_STEP1_ENG_THRESH    71

/* this value corresponds to -25 dB */
#define SRCH_STEP1_FALSE_PEAK_ENG_THRESH 71

#else

/* energy corresponding to ecio of -24.5 dB */
#define SRCH_STEP1_ENG_THRESH_IDX 49

/* Step1 energy corresponding to -24.6 dB */
#define SRCH_STEP1_ENG_THRESH    95

/* this value corresponds to -24.6dB */
#define SRCH_STEP1_FALSE_PEAK_ENG_THRESH 95

#endif


#define SRCH_ACQ_STEP1_MAX_PEAKS 32

/* step1 false peak detection starts if no. of peaks is greater than this */
#define SRCH_STEP1_NUM_FALSE_PEAK_THRESH	15

/****************************************************
             NASTT Async Parameters                                 
****************************************************/
/*  for ACQ cases */
/* Nc =128 */
#define SRCH_ACQ_NASTT_COH_INTG_LEN     128
/* Nn =36*/
#define SRCH_ACQ_NASTT_NON_COH_INTG_LEN 36

#if defined(FEATURE_WCDMA_JOLOKIA_MODEM) || defined(FEATURE_WCDMA_TABASCO_MODEM)

/* energy corresponding to -23 dB */
#define SRCH_ACQ_NASTT_ENG_THRESH_IDX 46

#else
/* energy corresponding to -22.5 dB */
#define SRCH_ACQ_NASTT_ENG_THRESH_IDX 45
#endif


#define SRCH_NASTT_NUM_RES_PER_TASK 4

/****************************************************
             PN Async Parameters                                 
****************************************************/
/* in 64 cx1 units. So nc =128 will be (1+1)(*64)*/
#define SRCH_ACQ_PN_COH_INTG_LEN   1
/* Nn = 35+1=36*/
#define SRCH_ACQ_PN_NON_COH_INTG_LEN    35

#define SRCH_PN_NUM_RESULT_PER_TASK 6

/* These parameters define the possible values for the flag srchPNIdleInterfFlag
 * in the srch PN async param FW interface
 *   value 0 will keep FW PN scheduler default behavior
 *   value of 1 tells FW PN scheduler to go back 1 slot
 *   instead of 2 slots which is default behavior
 *   value 2 tells FW to schedule interf PN on frozen samples
*/
#define SRCH_PN_IDLE_INTERF_SCHEDULER_OPT_OFF 0
#define SRCH_PN_IDLE_INTERF_SCHEDULER_OPT_ON 1
#define SRCH_PN_IDLE_INTERF_OFFLINE_SCHEDULER_OPT_ON 2

/* PN threshold in ACQ state, -23.5dB */
#define SRCHACQ_PN_THRESH_IDX        47

/* PN threshold for searches in ACQ after LTA gap, -24dB -> srchparm.h */
#define SRCHACQ_PN_PULLIN_THRESH_IDX_FOR_LTA        48

/*Trigger SFN recalculation when PN pos has changed by more than this*/
#define SRCH_PN_POS_DELTA_FOR_SFN_RESCHEDULE_CX8        2048
/*Move a cell from DROP_C to SYNC_N if ref_age is above this threshold*/
#define SRCH_DROP_C_TO_SYNC_N_AGE_MAX   50

/* window size for PN search. This is for ACQ only */
#define SRCH_ACQ_PN_WIN_SIZE    (192*8)
#define SRCH_NORMAL_MODE_MAX_PN_TASKS 64  
    /* Energy threshold of PSC peaks. */

/*number of peaks per task  returned for bypass mode searches */
#define SRCH_PN_NUM_RESULT_PER_TASK_FAT_PATH   12
/* number of MAX task for bypass mode searches */
#define SRCH_BYPASS_MODE_MAX_PN_TASKS  64

#define SRCHACQ_MAX_VERIFY_SRCH  1
  /* maximum number of "Verify" searches in any given frequency offset */

#define  SRCHACQ_DUP_FRAME_PEAK_DIFF 4
  /* Difference of frame peak positions, in chipx8, used for duplicate 
     frame peak detection.
   */
#ifdef FEATURE_WTOW_BACKGROUND_PLMN_SEARCH
   /* BLPMN search related thresholds */
/* corresponding to -23 dB ecio */

#if defined(FEATURE_WCDMA_JOLOKIA_MODEM) || defined(FEATURE_WCDMA_TABASCO_MODEM)
   /* BLPMN search related thresholds */
/* corresponding to -23 dB ecio for Jo modem*/

#define SRCHBPLMN_NASTT_ENG_THRESH_IDX  46
#else
   /* BLPMN search related thresholds */
/* corresponding to -22.5 dB ecio */

#define SRCHBPLMN_NASTT_ENG_THRESH_IDX  45
#endif

#define SRCHBPLMN_PULLIN_PATH_THRESH_IDX (SRCHBPLMN_NASTT_ENG_THRESH_IDX)
#endif

#define SRCH_STEP1_SLOT_FREEZ_SAMPLES 41

#define SRCH_FREQ_HYPO_MAX 15

/*number of freq hypothesis step1 can search in parallel */
#define SRCHACQ_MAX_FREQ_HYPO_IN_PARALLEL 2

/* Minimum pilot energy for pull-in operation during acquisition threshold 
 * corresponding to -22.0 dB EcIo 
 */
#define ACQ_PULLIN_PATH_THRESH_IDX 44

/*==================================================================
      Search step1/2/3 parameters (Non-acquisition)
=================================================================*/
#define  SRCH_NONACQ_STEP1_NON_COH_INTG_LEN SRCH_STEP1_NON_COH_INTG_LEN
/* same as the one during acquisition */
//#define  SRCH_NONACQ_STEP1_ENG_THRESH   SRCH_STEP1_ENG_THRESH
/* same as the one during acquisition */

#define SRCH_NONACQ_STEP1_MAX_PEAKS 12
#if defined(FEATURE_WCDMA_JOLOKIA_MODEM) || defined(FEATURE_WCDMA_TABASCO_MODEM)
 
  /* energy corresponding to ecio of -25 dB */
#define SRCH_NONACQ_STEP1_ENG_THRESH_IDX 50
 
 /* energy corresponding to ecio of -23 dB */
#define SRCH_NONACQ_STEP1_ENG_THRESH_IDX_OPT 46
 
 /* energy corresponding to ecio of -25 dB */
#define SRCH_NONACQ_NO_RXD_STEP1_ENG_THRESH_IDX 50
 
 /* energy corresponding to ecio of -24 dB */
#define SRCH_NONACQ_RXD_STEP1_ENG_THRESH_IDX 48
 
  /* energy corresponding to ecio of -24.5 dB for CPC DRX step1 searches*/
#define SRCH_NONACQ_CPC_RXD_STEP1_ENG_THRESH_IDX 49

 
 /* energy corresponding to ecio of -25 dB */
#define SRCH_NONACQ_RXD_IMB_STEP1_ENG_THRESH_IDX 50
 
#else

 /* energy corresponding to ecio of -24.5 dB */
#define SRCH_NONACQ_STEP1_ENG_THRESH_IDX 49

/* energy corresponding to ecio of -22.5 dB */
#define SRCH_NONACQ_STEP1_ENG_THRESH_IDX_OPT 45

/* energy corresponding to ecio of -24.5 dB */
#define SRCH_NONACQ_NO_RXD_STEP1_ENG_THRESH_IDX 49

/* energy corresponding to ecio of -23.5 dB */
#define SRCH_NONACQ_RXD_STEP1_ENG_THRESH_IDX 47

  /* energy corresponding to ecio of -23 dB for CPC DRX step1 searches*/
#define SRCH_NONACQ_CPC_RXD_STEP1_ENG_THRESH_IDX 47


/* energy corresponding to ecio of -24.5 dB */
#define SRCH_NONACQ_RXD_IMB_STEP1_ENG_THRESH_IDX 49

#endif

/*energy corresponding to ecio of -19.5 
 Default eng value for step1 peak elimination
 as part of Ngbr srch opt*/
#define SRCH_STEP1_PEAK_ELIMINATION_ENG_THRESH 39

/*CPICH ECIO is usually 5dB lesser than SCH ECIO 
  i.e SCH + 5db = CPICH */
#define SRCH_VAL_FOR_CPICH_TO_SCH_ECIO 10

/****************************************************
             NASTT Async Parameters                                 
****************************************************/
/*  for NON-ACQ cases */
/* no RxD search, corresponds to -23.1 dB */   
#define SRCH_NONACQ_NASTT_NO_RXD_THRESH_IDX 46
/* RxD search, corresponds to -21.8 dB */   
#define SRCH_NONACQ_NASTT_RXD_THRESH_IDX    44

/* for unmonitored searches*/
/* no RxD search, corresponds to -23.1 dB */   
#define SRCH_UNMON_NASTT_NO_RXD_THRESH_IDX  46
/* RxD search, corresponds to -21.8 dB */   
#define SRCH_UNMON_NASTT_RXD_THRESH_IDX     44

/* for ASYNC searches*/
/* no RxD search, corresponds to -25.5 dB */   
#define SRCH_ASYNC_NASTT_NO_RXD_THRESH_IDX  51
/* RxD search, corresponds to -24.0 dB */   
#define SRCH_ASYNC_NASTT_RXD_THRESH_IDX     48

/* Nc =2048 */
#define SRCH_NONACQ_NASTT_COH_INTG_LEN     2048
/* Nn =2 */
#define SRCH_NONACQ_NASTT_NON_COH_INTG_LEN 2

#ifdef FEATURE_WCDMA_DC_HSDPA
/* NASTT threshold for DC cells, corresponds to -22.0 dB */   
#define SRCH_DC_NASTT_THRESH_IDX            44
#define SRCH_3C_NASTT_THRESH_IDX            44
#endif

/*======================================================
    List search parameters 
  =======================================================*/
#define SRCH_STTD_COH_LEN_FOR_LOGGING (1024)
#define SRCH_STTD_NON_COH_LEN_FOR_LOGGING  (2)

#define  SRCH_ASET_WIN_SIZ  (192 * 8)
  /* Search window size for active set cells, in chipx8 */

#define  SRCH_DEFAULT_NSET_WIN_SIZ  (192 * 8)
  /* Default search window size for neighbor cells, in chipx8 */

#define SRCH_CELL_RECOVERY_SRCH_WIN_SIZ  (384*8)
  /*  Cell recovery window size */

#define SRCH_FULL_WINDOW_PN_WIN_SIZ (38400*8)

#define  SRCH_NSET_WIN_SIZ  (192 * 8)
  /* Search window size for neighbor cells, in chipx8 */

/* Threshold below which to reject the peaks as noise */
/* Corresponds to -23.0 dB */
#define SRCH_CELL_PATH_NO_TD_THRESH_IDX  46
/* Corresponds to -22.0 dB */
#define SRCH_CELL_PATH_TD_THRESH_IDX     44

/* Threshold to detect short/long Qxt */
/* Corresponds to -11.0 dB */
#define SRCH_CELL_QXT_PATH_NO_TD_THRESH_IDX  22
/* Corresponds to -8.0 dB */
#define SRCH_CELL_QXT_PATH_TD_THRESH_IDX  16

/* Threshold below which to reject deteced cell peaks as noise*/
/* Corresponds to -22dB */
#define SRCH_USET_PATH_THRESH_IDX 44

/*-------------------------------------------------------------------------
               Minimum Period between Async Neighbor searches
-------------------------------------------------------------------------*/
#define SRCH_ASYNC_SEARCH_TIMER  50
  /* Time period that async neighbor search can be triggered, 
     in 10ms units. This the maximum time period use for all state.
     Each state's async search timer muct be smaller than this value.
   */
  /* Value TBD. */

/*-------------------------------------------------------------------------
               Parameters for Calulating CPICH RSCP
-------------------------------------------------------------------------*/
#ifdef FEATURE_REL5
#define SRCH_MIN_CPICH_RSCP  ((l1m_query_rel5_supported()) ? 242 : 232)
/*  Minimum CPICH RSCP, corresponding to -121dBm */
#else
#define SRCH_MIN_CPICH_RSCP  232
#endif
  /* Minimum CPICH RSCP, corresponding to -116dBm */

#ifdef FEATURE_REL5
/* used for meas reporting purposes */
#define SRCH_MIN_CPICH_RSCP_RPTING 121
#endif

#define SRCH_MIN_CPICH_ECIO  49
  /* Minimum Usable CPICH Ec/Io Value for CPICH RSCP calculation,
     Corresponding to -24.5 dB
   */
#define NO_CPICH_TX_PWR (0xFF)

#define SRCH_PATHLOSS_NOT_VALID (0xFF)
#define SRCH_MAX_CPICH_PATHLOSS 158
#define SRCH_MIN_CPICH_PATHLOSS 46

#define SRCH_RSCP_FILT_LEN  5
  /* Filter length for RSCP. TBD. */

#define SRCH_RX_MIN_AGC_VAL -106

#define SRCHDCH_PATHLOSS_MIN_VAL (46)
#define SRCHDCH_PATHLOSS_MAX_VAL (158)
#define SRCHDCH_PATHLOSS_TABLE_OFFSET (38)
#define SRCHDCH_PATHLOSS_TBL_MAX_SIZE (120)
#define SRCHDCH_SMALLEST_PATHLOSS (8)

/*--------------------------------------------------------------------------
                Search Operation Control Parameter
--------------------------------------------------------------------------*/
#define  SRCH_ASET_SRCH_PERIOD  2
  /* Heartbeat count for Active and Candidate Set search,
     corresponding to 20ms */
#define  SRCH_NSET_SRCH_PERIOD  8
  /* Heartbeat count for Neighbor Set search, corresponding to 80ms */
#define  SRCH_FING_ASSIGN_PERIOD  4
  /* Heartbeat count for Finger Assignment Operation, corresponding to 40ms */
#define  SRCH_RSSI_UPDATE_PERIOD  2
  /* Heartbeat count for RSSI update, corresponding to 20ms */

/*--------------------------------------------------------------------------
Narrow Band Rejection Algo Log packet Requirement
-------------------------------------------------------------------------*/
#define WSRCH_NB_REJ_ACCUM_BPG_LOG  32
#define WSRCH_NB_REJ_NBEE_NUM_LOG  3

/*--------------------------------------------------------------------------
                   Search Operation Sanity Check Parameters
--------------------------------------------------------------------------*/
/* Heartbeat count for lost search, corresponding to 140ms */
#define SRCH_LOST_SRCH_PERIOD  14
 
/*---------------------------------------------------------------------

         Search parameters for RAT switch acquisition 

---------------------------------------------------------------------*/

/* corresponding to -19 EcIo */
#define SRCH_RAT_NON_BLIND_ENG_THRESH_IDX 38 

/* Maximum Non-coherent integration length for RAT acq list search */
#define SRCH_RAT_ACQ_PN_NON_COH_INTG_LEN 15   /* 16 */
/* Maximum coherent integration length for RAT acq list search */
#define SRCH_RAT_ACQ_PN_COH_INTG_LEN 3       /*  (3+1)*64 = 256  */

#define SRCH_PN_COH_INTG_LEN  31 /* 32*64 = 2048 */
#define SRCH_PN_NON_COH_INTG_LEN  0 /* 0=1 */

#define SRCH_DRX_PN_NON_COH_INTG_LEN  1  /* 1=2 */
#define SRCH_ANT_SEL_PRIMARY (rxd_prim_ant_id)
#define SRCH_ANT_SEL_DIV (rxd_div_ant_id)

#define SRCH_STEP1_LATEST_FINISH_TIME_FRAMES   10
#define SRCH_NASTT_LATEST_FINISH_TIME_FRAMES   10
#define SRCH_PN_LATEST_FINISH_TIME_FRAMES      20

/*---------------------------------------------------------------------
         Search parameters for full window search 
---------------------------------------------------------------------*/
/* energy corresponding to -23.5 dB EcIo for a NO TD cell */
#define SRCH_FULL_WINDOW_NO_TD_THRESH_IDX         47

/* energy corresponding to -20.5 dB EcIo for a TD cell */
#define SRCH_FULL_WINDOW_TD_THRESH_IDX            41

/*---------------------------------------------------------------------
         Search parameters for GTW ACQ full window search 
---------------------------------------------------------------------*/
/* energy corresponding to -21.5 dB EcIo*/
#define SRCH_GTW_ACQ_FULL_WINDOW_THRESH_IDX    43

/*---------------------------------------------------------------------
         Search parameters for GTW ACQ limited window search 
---------------------------------------------------------------------*/
/* energy corresponding to -23.5 dB EcIo*/
#define SRCH_GTW_ACQ_LIMITED_WINDOW_THRESH_IDX 47

/*---------------------------------------------------------------------
         Search parameters for limited window search during HHO
---------------------------------------------------------------------*/
/* energy corresponding to -23.5 dB EcIo*/
#define SRCH_HHO_LIMITED_WINDOW_THRESH_IDX     47

/*--------------------------------------------------------------------------
                Search Parameters used for BCH
--------------------------------------------------------------------------*/
/* energy corresponding to -22.5 dB EcIo*/
#define  SRCHBCH_NSET_ENG_THRESH_IDX  45

/* The minimum total energy for a neighbor cell in BCH State */
  /* Exactly value TBD. */

#define  SRCHBCH_MIN_ABOVE_THRESHOLD_NBR_CNT  6
  /* The minimum number of neighbors with above-threshold energy in BCH 
     state. Below which, async neighbor search may be triggered.
   */
  /* Exactly value TBD. */

#define SRCHBCH_MIN_ASYNC_SRCH_TIME  15 /* 150ms */  
  /* Minimum time period that async neighbor search can be triggered in 
     10ms units */
  /* Exact value TBD. */

/*--------------------------------------------------------------------------
                Search Parameters used during DRX
--------------------------------------------------------------------------*/
/* energy corresponding to -24.0 dB EcIo*/
#define SRCH_REACQ_TD_THRESH_IDX         48

/* energy corresponding to -25.5 dB EcIo*/
#define SRCH_REACQ_NO_TD_THRESH_IDX      51

/* Search window size for active set cells, in chipx8 */
#define SRCHZZ_ASET_WIN_SIZE  (192*8)

/*--------------------------------------------------------------------------
                Search Parameters used for DCH
--------------------------------------------------------------------------*/
/* Limit max no of step1 peaks in DCH to 12 */
#define SRCHDCH_MAX_STEP1_PEAK_CNT 12

/* Num of  step1 peaks we send for NASTT(unmon). In DCH we have max 12 step1 peaks.
correspondingly we will have (12/SRCHDCH_INTRAF_STEP1_PEAKS_PER_UNMON_NASTT) NASTT
(unmon) searches 
*/
#define SRCHDCH_INTRAF_STEP1_PEAKS_PER_UNMON_NASTT 4

#define SRCH_NON_DCH_OR_FACH_ASYNC_SRCH_TIME   36 /* 360 ms for non DCH or FACH full search */

#define SRCHDCH_ASYNC_SRCH_TIME   18 /* 180ms */
#define SRCHDCH_DC_ASYNC_SRCH_TIME   36 /* 360ms */
#define SRCHDCH_3C_ASYNC_SRCH_TIME   72 /* 720ms */
#define SRCHDCH_4C_ASYNC_SRCH_TIME   72 /* 720ms */
  /* Minimum time period that async neighbor search can be triggered in 
     10ms units */
#ifdef FEATURE_WCDMA_CPC_DRX
#define SRCHCPCDRX_ASYNC_SRCH_TIME_DRX_CYCLE_HIGH   64  /* 640 ms */
#define SRCHCPCDRX_ASYNC_SRCH_TIME_DRX_CYCLE_LOW    36  /* 360 ms */
#endif

  
#define SRCHDCH_MIN_ABOVE_THRESHOLD_NBR_CNT 3
  /* The minimum number of neighbors with above-threshold energy in BCH 
     state. Below which, async neighbor search may be triggered.
   */
  /* Exact value TBD. */


/* corresponding to EcIo= -22 dB */
#define  SRCHDCH_NBR_EVAL_THRESH_IDX  44

/* Reporting threshold for cells to be included in Measurement Report */ 
#define  SRCHDCH_NBR_RPT_THRESH_IDX  47 /* correponding to -23.5 dB */     

/* Neighbor eneergy threshold for SFN decoding */
#define SRCH_NBR_SFN_READ_THRESH_IDX 34  /* corresponding to -17 dB */

  /* The threshold below which the 1X equations won't be eval'ed */
#define SRCH_EVT_EVAL_THRESH_IDX  SRCHDCH_NBR_EVAL_THRESH_IDX
  /* Minimum energy for a neighbor cell to be evaluates for evt rpt */
  /* Exact value TBD. */

  /* neighbors which are below -22dB wont be evaluated for intraf events*/
#define SRCH_WEAK_NBR_THRESH_IDX 44

#define SRCH_SFN_DIFF_FRESH_INTVL    500
  /* SFN-SFN diff freshness interval, in 10ms unit. */
  /* Exact value TBD. */


/* The followings are not performance related parameters. But they will
   be used by multiple fiels.
 */
/* Indice to Intra-frequency event criteria array */
#define SRCHDCH_EVT_1A_INX  0
#define SRCHDCH_EVT_1B_INX  1
#define SRCHDCH_EVT_1C_INX  2
#define SRCHDCH_EVT_1D_INX  3
#define SRCHDCH_EVT_1E_INX  4
#define SRCHDCH_EVT_1F_INX  5
#define SRCHDCH_EVT_2ND_1E_INX 6
#define SRCHDCH_EVT_2ND_1F_INX 7
#define SRCHDCH_EVT_1J_INX 9
#define SRCHDCH_MAX_INTRAF_EVT  9  


/* Indice to Inter-frequency event criteria array */
#define SRCHDCH_EVT_2A_INX  0
#define SRCHDCH_EVT_2B_INX  1
#define SRCHDCH_EVT_2C_INX  2
#define SRCHDCH_EVT_2D_INX  3
#define SRCHDCH_EVT_2E_INX  4
#define SRCHDCH_EVT_2F_INX  5

#define SRCHDCH_MAX_INTERF_EVT  6

/* this indicates how many step1 peaks will be sent for NASTT srch in one go.
If N be the num of step1 peaks, then N/SRCHFACH_NUM_STEP1_PEAKS_FOR_NASTT
NASTT searches will follow the one step1 srch
*/
#define SRCHFACH_NUM_STEP1_PEAKS_FOR_NASTT 6

#define SRCH_NASTT_ASYNC_MAX_SCR_CODES 32

/* Frames to be reserved for Step1 searches during DCH */
#define SRCHDCH_NUM_REQD_FRAME_FOR_STEP1_SRCH 3

#define SRCHDCH_NUM_REQED_FRAME_FOR_FORCE_RESV_STEP1_SRCH 4

/* Frames to be reserved for NASTT searches during DCH */
#define SRCHDCH_NUM_REQD_FRAME_FOR_NASTT_SRCH 2

/* Frames to be reserved for Step1 searches during SET_A_NO_PN search procedure */
#define SRCHDCH_NUM_FRAMES_FOR_SET_A_NO_PN_STEP1_SRCH 4

/* Frames to be reserved for NASTT searches during during SET_A_NO_PN search procedure */
#define SRCHDCH_NUM_FRAMES_FOR_SET_A_NO_PN_NASTT_SRCH 4

#ifdef FEATURE_WCDMA_FULL_SRCH_IN_DC
  /* The second step1 timer time period. When step1 timer 2 expires, 
     the UE is to schedule a step1 search and block RAT or frequency 
     switchovers. This timer ensures a minimun search rate for step1 
  */
#define SRCH_ASYNC_STEP1_TIMER2  (SRCHDCH_ASYNC_SRCH_TIME+4)

#define SRCH_ASYNC_NASTT_TIMER   4

/* The force Step1 timer, at which time UE blocks frames for a forceful
 * Step1 search on secondary carrier. 
 */
#define SRCH_DC_FORCE_STEP1_TIMER   4

#define SRCH_DC_NASTT_TIMER         4

/* Frames to be reserved for DC Step1 searches during DCH */
#define SRCH_NUM_REQD_FRAME_FOR_DC_STEP1_SRCH 3

#define SRCHDCH_NUM_REQED_FRAME_FOR_FORCE_RESV_DC_STEP1_SRCH  4


/* Frames to be reserved for NASTT searches during DCH */
#define SRCH_NUM_REQD_FRAME_FOR_DC_NASTT_SRCH 2
#else
  /* The second step1 timer time period. When step1 timer 2 expires, 
     the UE is to schedule a step1 search and block RAT or frequency 
     switchovers. This timer ensures a minimun search rate for step1 
  */
#define SRCH_ASYNC_STEP1_TIMER2  (SRCHDCH_ASYNC_SRCH_TIME+10)

#define SRCH_ASYNC_NASTT_TIMER   10

#endif

#define SRCH_NUM_FRAME_STEP1_SRCH    3

/* When FW is running at 107MHz,
 * NAS -TT Unmonitored (Nc=2048, Nn=2, win_siz=1, RxD mode= 1) search takes
 * W\o RxD, 4 peaks takes ~24 ms, so ~6ms per step1 peak
 * W RxD,   4 peaks takes ~45 ms, so ~11ms per step1 peak 
 * ie. Num frames needed = (duration_per_peak*num_peaks/10) + 1
 * Note that 0 peaks are not taken care of; so take care to not pass zero here. 
 */
#define SRCH_NUM_FRAME_FOR_NASTT_UNMON_SRCH_WO_RXD(peak) ((6*peak + 10)/10)

#define SRCH_NUM_FRAME_FOR_NASTT_UNMON_SRCH_RXD(peak)    ((11*peak + 10)/10)

/* When FW clock is running at 53MHz (instead of 107 MHz), then it takes longer to 
 * complete searches. Infact, when operating in non-RxD mode, the searches take as 
 * much as it takes with RxD enabled on 107MHz. Hence, use this new macro to point to
 * the RxD macro.
 */
#define SRCH_NUM_FRAME_FOR_NASTT_UNMON_LOW_CLK_WO_RXD(peak) SRCH_NUM_FRAME_FOR_NASTT_UNMON_SRCH_RXD(peak)

/* When FW is running at 107MHz,
 * NAS -TT Async (Nc=2048, Nn=2, win_siz=1, RxD mode= 1) search takes 
 * W\o RxD, 12 peaks take ~8ms,  so ~0.75 ms per step1 peak
 * W RxD,   12 peaks take ~13ms, so ~1.1 ms per step1 peak
 * ie. Num frames needed = (duration_per_peak*num_peaks/10) + 1 
 * Note that 0 peaks are not taken care of; so take care to not pass zero here. 
 */
#define SRCH_NUM_FRAME_FOR_NASTT_ASYNC_SRCH_WO_RXD(peak) (( 3*peak + 40)/40)

#define SRCH_NUM_FRAME_FOR_NASTT_ASYNC_SRCH_RXD(peak)    ((11*peak + 100)/100)

#define SRCHDCH_NUM_REQED_FRAME_FOR_STEP1_2_SRCH 3

#define SRCHDCH_NUM_REQED_FRAME_FOR_FORCE_RESV_STEP1_2_SRCH 4

/* When FW clock is running at 53MHz (instead of 107 MHz), then it takes longer to 
 * complete searches. Infact, when operating in non-RxD mode, the searches take as 
 * much as it takes with RxD enabled on 107MHz. Hence, use this new macro to point to
 * the RxD macro.
 */
#define SRCH_NUM_FRAME_FOR_NASTT_ASYNC_LOW_CLK_WO_RXD(peak) SRCH_NUM_FRAME_FOR_NASTT_ASYNC_SRCH_RXD(peak)

/* PN (Nc=2048, Nn=1, win_siz=192cx1, RxD mode= 1) search takes ~0.125 ms per task */
#define SRCH_NUM_FRAME_FOR_PN_SRCH(tasks) ((tasks+7)/8)

/* Num frames needed for full searches in FACH when FW operating at 107 MHz */
/* when ASYNC searches to be included */
#define SRCH_NUM_FRAME_FACH_FULL_SRCH_ASYNC_NASTT(s1_peaks)                                       \
                                          ( SRCH_NUM_FRAME_STEP1_SRCH  +                          \
                                            SRCH_NUM_FRAME_FOR_PN_SRCH(1)  +                      \
                                            SRCH_NUM_FRAME_FOR_PN_SRCH(32)  +                     \
                                            SRCH_NUM_FRAME_FOR_NASTT_ASYNC_SRCH_WO_RXD(s1_peaks)  \
                                          )

/* when UNMON searches to be included */
#define SRCH_NUM_FRAME_FACH_FULL_SRCH_UNMON_NASTT(s1_peaks)                                       \
                                          ( SRCH_NUM_FRAME_STEP1_SRCH  +                          \
                                            SRCH_NUM_FRAME_FOR_PN_SRCH(1)  +                      \
                                            SRCH_NUM_FRAME_FOR_PN_SRCH(32)  +                     \
                                            SRCH_NUM_FRAME_FOR_NASTT_UNMON_SRCH_WO_RXD(s1_peaks)  \
                                          )

/* Num frames needed for full searches in FACH when FW operating at 53 MHz */
/* when ASYNC searches to be included */
#define SRCH_NUM_FRAME_FACH_FULL_SRCH_ASYNC_NASTT_LO_CLK(s1_peaks)                                  \
                                          ( SRCH_NUM_FRAME_STEP1_SRCH  +                            \
                                            SRCH_NUM_FRAME_FOR_PN_SRCH(1)  +                        \
                                            SRCH_NUM_FRAME_FOR_PN_SRCH(32)  +                       \
                                            SRCH_NUM_FRAME_FOR_NASTT_ASYNC_LOW_CLK_WO_RXD(s1_peaks) \
                                          )

/* when UNMON searches to be included */
#define SRCH_NUM_FRAME_FACH_FULL_SRCH_UNMON_NASTT_LO_CLK(s1_peaks)                                  \
                                          ( SRCH_NUM_FRAME_STEP1_SRCH  +                            \
                                            SRCH_NUM_FRAME_FOR_PN_SRCH(1)  +                        \
                                            SRCH_NUM_FRAME_FOR_PN_SRCH(32)  +                       \
                                            SRCH_NUM_FRAME_FOR_NASTT_UNMON_LOW_CLK_WO_RXD(s1_peaks) \
                                          )

// TODO what should we do with this - should it be 1 or 15?
#define SRCH_NUM_SEGMENT_FULL_PN_SRCH 1

/*--------------------------------------------------------------------------
        Parameters related to Cell Reselection Evaluation and Report
--------------------------------------------------------------------------*/

#define SRCH_HIGH_THRESH_SEARCHES  3
  /* The number of continuous ASET searches with above a high threshold energy
     to disable cell reselection evaluation. Exact value is TBD.
   */
#define SRCH_DEFAULT_ASET_HIGH_THRESH  48000
  /* Default ASET high energy threshold above which cell reselection 
     evaluation may be disbaled. Exact value is TBD.
   */
#define SRCH_CR_MAX_DELAY_TIME     200
  /* Maximum Interval between search reports, in 10ms units. */
  /* Exact value TBD. Depending on the frequency that SIB_11/12 be sent 
     by network. 
   */
#define SRCH_CR_THROT_INTERVAL   50
  /* Minimum Interal between search reports to prevent cell reselection
     report thrashing.
   */
   
/*--------------------------------------------------------------------------
                Parameters related to RxD Evaluation Threshold
--------------------------------------------------------------------------*/
	
	/* Minimum Ec/I0 where original step 1 RxD-add threshold is used, in 0.5 dB units */
#define SRCH_RXD_IMB_ASET_CELL_ECIO_THRESH 36
	
	/* Minimum RxAGC level where original step 1 RxD-add threshold is used, in dBm */
#define SRCH_RXD_IMB_RXAGC_ANT_LOW_THRESH 95
	
	/* Minimum RxAGC difference between two antennae where original step 1 RxD-add
	   threshold is used, in dB */
#define SRCH_RXD_IMB_RXAGC_ANT_DIFF_THRESH 5

/*--------------------------------------------------------------------------
              Parameters related to Search Measurement Filtering
--------------------------------------------------------------------------*/
#define SRCH_FILTER_CNT_THRESH 2
/* Count threshold used for applying noise thresholds during filtering */

/* #define SRCH_FILTER_NOISE_FLOOR  SRCH_PATH_THRESHOLD */
 /* Corresponding to  -22 dB */

#define SRCH_HPQ_LEN_FOR_PCH_AND_FACH   2
/* The hpq filter length for PCH and FACH states */

/*--------------------------------------------------------------------------
              Other Miscellaneous Parameters 
--------------------------------------------------------------------------*/
#define  SRCHACQ_FREQ_OFFSET_PPM  350
  /* During initial acquisition, WL1 tries different frequency offsets on
  ** either side of the center frequency.  The offset is +/- 3.5 ppm.  The
  ** function that uses this value wants (ppm * 100), 350 in this case.
  */

#define  SRCHACQ_FREQ_OFFSET_PPM_ON_FIRST_ACQ  200
  /* During first acquisition, WL1 tries different frequency offsets on
  ** either side of the center frequency.  The offset is +/- 2.0 ppm.  The
  ** function that uses this value wants (ppm * 100), 200 in this case.
  */

#define  SRCHACQ_FREQ_OFFSET_PPM_ON_VERIFY_ACQ  400
  /* During verify search, WL1 tries different frequency offsets on
  ** either side of the center frequency.  The offset is +/- 4.0 ppm.  The
  ** function that uses this value wants (ppm * 100), 400 in this case.
  */

#ifdef FEATURE_FLEX_FREQ_BIN
  /*  The offset is +/- 1.35 ppm, if temperature table entry is available*/
#define  SRCHACQ_FREQ_OFFSET_VALID_TEMP_PPM  135
#endif

#define  SRCH_PRI_CPICH_OVSF  0

/* Maximum Slot (PSC) peaks returned by DSP from Step_1 Search */
#define  SRCH_MAX_PSC_PEAKS  64

/*     - NASTT cmd can be sent with 12 step1 peaks
     - srchNASTTNumResPerTask, range [1..7] = M
     - total results (No RxD/Combined)  = 12M 
                        (RxD one by one)     = 12M(prim)+12M(div) = 24M
*/
#define SRCH_MAX_NASTT_PILOTS (24* SRCH_NASTT_NUM_RES_PER_TASK)

#define SRCH_NONACQ_MAX_STEP1_PEAK_FOR_NASTT 6

#define SRCH_NONACQ_MAX_STEP1_PEAK_FOR_PARALLEL_NASTT 5

/* M-peaks per task async mdsp variable range=[1 ..7]
in shoulder mode we can have total 3M i.e 21 peaks per antenna */
#define SRCH_MAX_PILOT_PEAKS (3*SRCH_PN_NUM_RESULT_PER_TASK)

  /* the max number of peaks returned per pilot from step 3 or list search */

/* the max number of peaks per six pilots from step 3 */
#define SRCH_MAX_PILOT_PEAKS_FOR_SIX_CELLS 24

#define SRCH_MAX_NUM_STEP1_PEAKS 32

/* We can schedule upto a maximum of 64 tasks at a time. This is when
 * using no RxD, normal mode. Each task then can have upto 6 peaks. Thus 
 * max number of peaks is 64*6. For Rxd, since number of peaks increase to 12,
 * we can schedule only 32 tasks, still max peaks will be same, ie 32*12
 * With Fatpath algo we can have max 38 task with 18 peaks each (6 for normal mode
 * and 12 for bypass mode) for primary and diversity antenna, So total number of peaks 
 * will be 38*(6+12)*2 = 1368 
 */
#define SRCH_PN_CMD_NUM_PEAKS_MAX    1368

#define SRCH_MAX_NUM_PEAKS_BYPASS_SRCH 12

#define  SRCH_CHIPX2_PER_SLOT  5120
#define  SRCH_CHIPX8_PER_FRAME 307200


#define RX_AGC_THRESH_FOR_OUT_OF_SERVICE  (-102)
/* For this intermediate release this is the RF AGC threshold below
  which, in idle mode, searcher would declare an out of service area 
  indication to RRC */

/* #define  SRCH_ENG_THRESH_FOR_OUT_OF_SERVICE  SRCH_PATH_THRESHOLD  */
/* For this intermediate release this is the energy threshold below
  which, in idle mode, searcher would declare an out of service area 
  indication to RRC */


#define SRCH_TS_CNT_ADJ   0
  /* Event Time Stamp Adjustment which takes neighbor cell measurement 
     frequency into account, in units of 10ms. It is set to half of 
     neighbor measurement period. ( needs a further look)
   */

#define SRCH_FREQ_SCAN_OFFSETS 4  
  /* The number of frequency offsets to be used for shifting center 
     frequency during WCDMA frequency RAW scan. 4 is the maximum.
   */
  
#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2
#define SRCH_FREQ_SCAN_RAW_SCAN_STEP  15
/* 15 corresponds to 3 MHz which is the new raw scan bin size*/
#else
/* 10 corresponds to 2 MHz step size */
#define SRCH_FREQ_SCAN_RAW_SCAN_STEP  10
#endif
  /* RAW scan Incremental step for Frequency scan. Each unit corresponds to 
     200KHz. "10" corresponds to 2MHz.
   */
#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2
#define SRCH_FREQ_SCAN_FINE_SCAN_RANGE  7
#else
#define SRCH_FREQ_SCAN_FINE_SCAN_RANGE  5
#endif
  /* Fine scan range around the specified frequency. Each unit corresponds to
     200KHz.
   */
   
/* The following two thresholds are to be used for service requests */ 
/* AGC threshold used for WCDMA carrier frequency, in dBm */
#define SRCH_FREQ_SCAN_AGC_THRESH_FOR_SRV_REQ  (-103)

/* AGC threshold used for WCDMA carrier frequency, in dB10 */
#define SRCH_FINE_FREQ_SCAN_AGC_THRESH_FOR_SRV_REQ (-482)

/* The following two thresholds are to be used for PLMN search requests */
/* AGC threshold used for WCDMA carrier frequency, in dBm */
#define SRCH_FREQ_SCAN_AGC_THRESH_FOR_PLMN_SRCH  (-100)

/* AGC threshold used for WCDMA carrier frequency, in dB10 */
#define SRCH_FINE_FREQ_SCAN_AGC_THRESH_FOR_PLMN_SRCH (-452)

#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) ||defined(FEATURE_WTOW_BACKGROUND_PLMN_SEARCH)
/* The following two thresholds are to be used for PLMN search requests */
/* AGC threshold used for WCDMA carrier frequency, in dBm.  */
#define SRCH_FREQ_SCAN_AGC_THRESH_FOR_BPLMN_SRCH  (-100)
/* AGC threshold used for WCDMA carrier frequency, in dB10.  */
#define SRCH_FINE_FREQ_SCAN_AGC_THRESH_FOR_BPLMN_SRCH (-452)
#endif


#ifdef FEATURE_WCDMA_FS_FALSE_PEAK_ELIM_OPT
/* Num of slots of IQ data fw will perform FFT on
   S1 - number of slots for primary check
   S2 - number of slots for secondary check*/
#define SRCH_FREQ_SCAN_FA_DET_NUM_SLOT_S1 2
#define SRCH_FREQ_SCAN_FA_DET_NUM_SLOT_S2 0
/* PSD variance threshold */
#define SRCH_FREQ_SCAN_FA_DET_VAR_THRESH  12
#define SRCH_FREQ_SCAN_FA_DET_VAR_THRESH_Q4 (SRCH_FREQ_SCAN_FA_DET_VAR_THRESH <<4)
/* this guard timer will protect in case mdsp did not respond to FREQ_SCAN_CMD 
    time in srch HB units. 10 =10*10ms = 100ms */
#define SRCH_FREQ_SCAN_GUARD_TIME_FOR_MDSP 10
#endif

#define SRCH_WCDMA_RSCP_FILT_LEN 5 
  /* Value of N for the (N-1)/N Filter Coefficient for ASET filtered RSCP 
     used for UI display.
   */

#if defined(FEATURE_WCDMA_AGC_LOG_PKT_0x4165) || defined(FEATURE_WCDMA_AGC_LOG_PKT_VER3)
#define SRCH_AGC_LOG_PERIOD 1
#else
#define SRCH_AGC_LOG_PERIOD 2
#endif
/*   interval for logging AGC packets when other search ops have been 
     disabled. 20ms currently. Note that if search operations are 
     scheduled, then this logging happens every 20 ms along with aset
     searches   */
     
#define SRCH_WCDMA_MAX_INTRAF_FILT_SIZE 3
 /* As per 3GPP, atmost 2 filter coefficients are allowed. Add one to this
    as the default, we get 3    */

#define SRCH_MAX_SRCH_REISSUE_TS_CNT 5
/* when a LPQ list srch gets aborted, its reissued by lpq_isr. This reissue can continue for
SRCH_MAX_SRCH_REISSUE_TS_CNT (in HB units i.e 10ms units). After this duration, LPQ srch 
reissuing will be stopped so that other searhes e.g HPQ can continue  */

#define SRCH_LPQ_MAX_TASKS_DURING_CM 20
/* keep the LPQ task list to a max of 20
 during compressed mode of operation */

#define SRCH_MAX_REF_AGE_SYNC_N  50
/* this is the max ref age for a SYNC_N cell. Once it reaches/exceeds this it will be 
deleted and moved to ASYNC_N and step3 will find out the cell if good */

#ifdef FEATURE_WCDMA_DC_HSDPA
/* this is the max ref age for a DC sync neighbor cell. Once it reaches/exceeds this it will be 
deleted and moved to empty set */
#define SRCH_MAX_REF_AGE_DC_SYNC_N  10
#endif

#define SRCH_MAX_REF_AGE_USET  10
/* this is the max ref age for a uset cell. Once it reaches/exceeds this it will be 
deleted and moved to empty set */

#define SRCH_MAX_REF_AGE       10
/* this is the max ref age for any cell */

#define SRCH_MAX_REDETECT_AGE  10
/* this is the max redetect age for any cell */

#endif /* SRCH_PARM_H */

