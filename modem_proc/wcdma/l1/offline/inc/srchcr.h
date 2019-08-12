#ifndef SRCHCR_H
#define SRCHCR_H
/*===========================================================================
   S R C H     C E L L     R E S E L E C T I O N     H E A D E R    F I L E

DESCRIPTION
   This file contains procedures declarations pertinent to WCDMA
   cell reselection evaluation.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  srchcr_init() should be called once we enter DRX mode.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2001-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchcr.h_v   1.5   11 Jul 2002 21:29:46   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchcr.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------\
02/24/15   ap      Initial check-in for WL1 QSH framework 
02/16/16   nga     Force rank all W cells if at least one  W frequency is measured
09/07/15   nga     Set res-eval_meas_comp flag to intra,inter& IRAT before issuing EcIo ranking as part of finding next best cell      
08/03/15   mk      Check and Include USET cells for PN searches whenever sorted count is 0 (happens after cell resel/trans failure)
06/09/15   nga     Update EcIo & EcIo rank fields of log packet 0x4005 appropriately 
06/04/15   nga     Check if inter-f nbrs measured before issuing RSCP ranking 
                   seaparately for F0 & F1. Also maintain separate Tresel timers 
                   for F0, F1 of inter-f nbrs
06/01/15   mk      In EDRX mode, calculate inter-f EcIo ranks only when measured.
01/05/15   nga     Removing Round Robin Algorithm to issue panic searches in G2W TA
12/11/14   gsk     Avoid setting bch on for duplicate cell resel indication.
                   Removing the use of unused global.
11/19/14   gsk     FACH FMO log packet and adding support for 0x4027 for FE-FACH
11/17/14   psr     Srchcrutil Cleanup
11/13/14   pr      Added srchcrutils_intraf_ngbr_fail_ecio_check
11/05/14   psr     Fixed linking compilation errors for JO 1.0
10/31/14   gsk     Logic to avoid clear res eval meas pending when CTCH is dropped for search 
10/13/14   ar      Add prototype for reporting IF nbrs to WL1CXM/MCS
10/23/14   gsk     Allow interf ranking in FACH when priorities are defined but FEFACH isnt enabled  
06/20/14   skk     FR19218: ReacqLessPICH(RLP) porting for JO.
10/06/14   ar      Add prototype for posting OOS indication
10/02/14   ar      Moved macros from .c file
09/18/14   mk      Featurizing LTE code.
08/25/14   psr     Added prototype for posting cell resel start event function
08/13/14   ar      Adjust srchcr_res_eval_meas_type to include LTE forced det type
08/12/14   gsk     Remove usage of srchcr_agc_stopped_post_w2x flag.
08/04/14   rs      Fixed compiler warnings
07/23/14   ar      Featurize PWC code with FEATURE_WCDMA_ENH_OOS_RXD_PWC
07/21/14   ar      Remove FEFACH feature definition for some macros
06/05/14   mk      WCDMA FED Feature for DPM_2_0, Flag used FEATURE_WCDMA_L1_DPM_FED_SUPPORT
06/23/14   ar      Changes to support AbsPrio Tresel scaling factor for GSM and LTE
06/24/14   mk      Support for changing EARFCN variable type from uint16 to uint32.
06/19/14   gsk     Idle DR-DSDS changes
05/30/14   ar      Add prototype SRCHCR_FACH_LEGACY_RULES
05/28/14   ar      Add FACH abs priority macro
05/22/14   gsk     Changes for enhanced PN search and excluding USET cell for meas
05/01/14   ar      Add prototype for panic mode check
04/28/14   ar      Add prototype to check if any LTE tresel is running in FACH
04/28/14   ar      Rename srchzz_is_tresel_running_for_any_lte_cells to srchcrutils file
04/10/14   ar      Remove prototype srchcr_clearing_sorted_intra_ngbr_cells_struct
04/03/14   ar      Add nlayers_fach for absolute priority in fach
03/25/14   ar      Add prototype to cleanup srchcr db
03/10/14   ks      Bolt FED API checkin
01/30/14   ar      Add support for distinguishing IRAT READY state between GSM and LTE
01/24/14   ar      Added support for forcing LTE detect searches after a W2L Resel failure
01/22/14   gsk     Ngbr srch opt log packet changes
01/16/14   gsk     Externed oos check function
01/16/14   ar      Change prototype for srchcrutils_prepare_srched_cells
01/13/14   ar      Add SRCHCR_INVALID_PRIORITY_SCHEDULING_MODE
12/16/13   as      Porting G2W TA from Triton to Dime.
12/15/13   ar      FEFACH (FR2759) absolute priority measurements in FACH
11/27/13   hk      Unification of non-intra module cleanup procedures
11/15/13   ar      FEFACH (FR2759) absolute priority measurements in FACH - ranking and evals
10/31/13   ar      Added nastt unmon peak processing function
10/29/13   sr/gsk  During cell reselection, delaying CPHY setup cnf for NBCH drop untill
                   serv cell is configured and nbr cell is deconfigured,                   
10/29/13   gsk     API to enable heartbeat after cell is deconfigured as part of NBCH drop
10/28/13   ar      Removed code under FEATURE_DL_IGNORE_MODE_FACH_MEAS
10/15/13   ar      Support for FEFACH (FR2759) absolute priority measurements in FACH
09/23/13   as      Porting Dual Sim from Triton to Dime.
09/12/13   jd      Searcher redesign
08/27/17   stk     Changed W2L CSG RSRP threshold to -100 as per SDD
08/19/13   vn      Added changes to indicate the reason for barring a band.
07/12/13   ar      Idle RxD enhancement changes for extended 3g coverage
07/05/13   stk/mk  FR 2677 - Initial version. Support for W2LTE CSG.
06/27/13   gsk     Updating the function name
06/22/13   gsk     Externing functions
05/09/13   mk      Support for measuring inter freq cells faster if tresel timer running for any interf cell.
04/11/13   gsk     Idle Mode FR 2741/2743, reducing WL1 wait time between rf_tune_to_chan
                   and search cmd to FW. Also adding support for interf offline PN
03/18/13   mk      Declared a varible as extern
03/04/13   gsk     Adding a function to clear the early rf sleep decision flag
01/21/13   stk     Support for Femto/CSG selection and reselection
09/10/12   stk     Added support for Cell reselection optimizations with HSFACH/HSRACH
10/01/12   gsk     For ngbr_srch_opt, calculating the min of max_tx_pwr of all the cells in a freq.  
11/13/12   gsk     Code cleanup
10/01/12   gsk     For ngbr_srch_opt, calculating the min of max_tx_pwr of all the cells in a freq.  
09/17/12   pv      Changed the return type of early RF response API.
09/10/12   stk     Added support for Cell reselection optimizations with HSFACH/HSRACH
08/30/12   pv      Fixed a variable type.
08/30/12   pv      Added extern.
08/29/12   pv      Remove featurization for early RF sleep resp API.
08/28/12   gsk     Externing the function for clearing the intra ngbr cell structure
08/21/12   stk     Pend the resel flag only when reselection is successfully triggered and RACH not in progress.
07/19/12   gsk     Neigbour Search Optimizations fixes
07/17/12   gsk     Compiler warnings
07/13/11   pv      Offline PICH/Parallel Reacq fixes.
07/05/12   gsk     Idle mode FRs 1782/1705/1947/2152/1950
05/15/12   gsk     FR 1782: WCDMA Inter- Frequency Tuning Optimization.
05/15/12   gsk     Support for feature Inter-freq List search frequency with two 
                   additional frequencies in idle 
06/19/12   gsk     Skipping detect and measurement searches on barred LTE frequency 
                   and setting the timers to max after the freq is unbarred.
11/28/12   gsk     Fix for compiler warnings
10/21/10   nd      Added an element in cell_bar database to indicate if a 
                   cell is reserved for operator use.
10/01/12   gsk     Externing function for checking if the interf is barred
10/01/12   gsk     Externing the function for clearing the intra ngbr cell structure
09/26/12   gsk     Triton fixes for disabling LTE
08/21/12   stk     Pend the resel flag only when reselection is successfully triggered and RACH not in progress.
08/14/12   gsk     Neigbour Search Optimizations fixes
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12   gsk     Idle mode FRs 1782/1705/1947/2152/1950
05/15/12   gsk     FR 1782: WCDMA Inter- Frequency Tuning Optimization.
05/15/12   gsk     Support for feature Inter-freq List search frequency with two 
                   additional frequencies in idle 
06/19/12   gsk     Skipping detect and measurement searches on barred LTE frequency 
                   and setting the timers to max after the freq is unbarred.
06/19/12   vr      HSRACH code update
06/07/12   vs      MBMS feature cleanup.
04/16/12   gsk     Extern the variable srchcr_sorted_inter_f_nset_cnt.
03/27/12   hk      Added support for FEATURE_WCDMA_HS_FACH_DRX
02/03/12   dp      Get RSSI Threshold from common external #define
01/20/12   zr      Mainlined FEATURE_L1_DETECTED_CELLS_FACH, FEATURE_L1_DETECTED_CELLS_IDLE, 
                   FEATURE_WCDMA_HCS, FEATURE_T_BARRED_SUPPORT, and FEATURE_L1_CELL_RESELECTION
12/21/11   stk     Added support for Rel9-RSRQ based W2L reselection. 
12/02/11   stk     Added support for FEATURE_WCDMA_RXD_PCH and FEATURE_IDLEMODE_FILTER
11/12/11   pv      New early RF sleep response API.
09/02/11   stk     fix compiler warnings
09/01/11   stk     Moved srchcr_priority_enum_type to srchcr.h to be used by srchcrgsm
04/26/11   mm/uk   Support for PCH Cleanup
04/26/11   dp/uk   Add support for W2X_IDLE_CMD
08/24/10   stk     Added function prototype to fix compiler warning.
08/23/10   uk      changed algorithm for sending LTE INIT/Deint
08/23/10   stk     Added support to handle pending CELL_BAR_REQ received in PCH_SLEEP state, 
                   to avoid any collision with CPHY_SETUP_REQ for NBCH later on.
08/06/10   mm      Compiler warning fix; added prototype for 
                   srchcr_perform_cell_res_eval_abs_prio
06/02/10   stk     Added support for W2LTE measurements and reselection under 
                   FEATURE_WCDMA_ABSOLUTE_PRIORITY & FEATURE_WCDMA_TO_LTE
03/05/10   stk     Handle OOS after W2G failure recovery and Reacq Fails
05/08/09   sup     Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                    srchlog.h to wdec.h, wenci.h, wl1m_v.h, wsrch.h, wsrchi.h, wsrchlog.h
04/22/09   stk     Compiler warning fixes
01/19/09   stk     Added support for R7 spec CR High Mobility Indicator( spec CR 2948(RP-060712))
12/09/08   uk      Mainling FEATURE_WCDMA_DL_ENHANCED, FEATURE_ENHANCED_CELL_RESELECTION, 
                   FEATURE_WCDMA_DRX_MANAGER, FEATURE_WCDMA_OFFLINE_SEARCHER 
                   and FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT 
12/09/08   uk      Added non_eq_plmn_barred to struct srchcr_barred_cell_struct
10/03/08   hk      Bringing in 7k Mailine fixes
08/13/08   uk      Merging following fixes
                   Cleaned up and added some fn prototypes
                   Fixed warning C2220W: Superfluous ',' in 'enum' declaration
                   Fixed compiler warning
                   Externed fn srchcr_reset_oos_timer()
                   modified feature name for MBMS
01/24/08   sk      Lint/Featurization fixes
01/18/08   uk      Added fn srchcr_start_gsm_srch_on_gsm_clk_en to be called to trigger 
                   IRAT srch when GSM clk and TX sytem time get enabled
12/19/07   sk      Fixed compilation error for FEATURE_MODEM_MBMS undef compilation
12/03/07   sk      Declaration check pl info function
12/05/07   sk      Changes to support PSC scanner with new DRX
11/01/07   ub      MBMS: DRX mode meas filtering when MCCH/MTCH enabled prototypes
10/26/07   sk      Checking in MBMS branch code into mainline with MBMS feature disabled
08/10/07   sk/uk   Added logic to not short tresel for W cells when GSM neigbors are 
                   present and weak reselection condition is entered (certain number 
                   of reselections triggered in a certain amount of time due to S-criteria 
                   failure and  shorting of Tresel under low rssi condition)
10/04/07   sk      Reselection related cleanup
08/23/07   vsr     Made include file change related to removal of dlcmd.* files
07/05/07   ub      Added FLC support
07/16/07   sv      Fixed previous merge issue.
05/23/07  ub/sk    Intergrate GSM meas and code cleanup
05/22/07   sk      Enabled measurement rules
04/26/07   sk      Fixed lint errors
04/25/07  ub/sk    MBMS cell reselection changes
06/04/07   sk      Added FEATURE_SUPPORT_FOR_PARTIAL_ROAMING or CR151
04/05/07   sk      Added extern for srch_cell_bar_update
02/14/07   ub      Define SRCHCR_NV_MAX_TX_PWR_INCREMENT as 1 for correct 
                   Srxlev calculation for power class 3 UE
01/12/07   sk      Unbar frequency if the cell is not highest ranked cell
10/03/06   sk      Defined srchcr_interf_resel_fail
06/20/06   bd      Renamed global vars/functions to avoid conflict with 1x names
05/24/06   ub/mmr  Cleaned up HCS and Rel5 featurization.
05/11/06   sk      Defined extern srchcr_tresel_irat
04/20/06   sk      Changed SRCHCR_QHYST2_ADD to return 2*hist_add to avoid 
                   rounding off
                   Declared variables for common CR timer in FACH and idle.
04/19/06   sk/ub   Declaration of srchcr_ignore_barred_freqs
03/28/06   sk      Declaration for srchcr_get_weakest_uset_idx ()
03/17/06   ub      Lint error fixes
02/20/06   gv 	   Declared extern srchcr_rem_oos_time, this variable stores
				   the remaining OOS time when the OOS timer is stopped.
10/19/05   ub      Declare fast mov when num reselections exceed Ncr
09/13/05   sk      Detected Cells Support in idle mode
08/12/05   sk      Declaration for srchcr_init_gsm_nset()
08/05/05   ub      SFN based full search timers implementation
07/06/05   sk      Declared extern srchcr_strong_cell_ptr
07/15/05   am      removed the srchcr_inter_f_pccpch_n_cc_setup_pending and 
                   srchcr_inter_f_pccpch_n_ph_setup_pending variables
06/16/05   ub      Changes for supporting sfn-sfn observed time difference
05/31/05   am      Merged the latest lint fixes to the mainline
05/27/05   ub      Exclude inter-f and gsm cells using meas rules in FACH
05/24/05   ub      Changed function prototypes for starting inter-f step srch
05/24/05   ub      Updated penalty timer backup and restore functions
05/12/05   sk      T-barred Feature: Removed intraf_barred. Added new enum for
                   bar type (wcdma cell/ wcdma freq/ gsm/ wcdma aset).
05/10/05   sk      T-barred Feature: T-barred variable in barred cell list
                   to hold ms and hence uint32
05/04/05   sk/sh   Added T-barred Feature support. Added barred cell structure
                   and immediate reselection pending flags
05/01/05   ub      Cell reselection improvements and fixes
04/06/05   ub/ba   HCS based cell reselection feature (FEATURE_WCDMA_HCS)
11/19/04   yiz     Optimize FACH GSM BSIC meas scheduling.
09/22/04   sh      Added srchcr_sib_mod_update() which is called to do SIB
                   modification updates.
06/23/04   sh      Add l1m_state_enum_type argument to srchcr_cleanup().
06/04/04   src     Changes concomitant to consolidating certain neighbour-cell
                   parameters into a newly defined aggregate in demod modules.
05/25/04   sh      Added proto srchcr_exclude_cell_from_ranking().
05/10/04   gs      Moved enum srchcr_cell_type_enum_type from its source file
                   Externalized functions srchcr_add_cell_to_srched_list and
                   srchcr_remove_cell_from_srched_list
05/06/04   sh      Added protos for srchcr_intra_f_post_list_srch_dump() and
                   srchcr_inter_f_post_list_srch_dump().
03/26/04   sh      Added flags to sequence inter-frequency nBCH setup.
02/17/04   sh      Removed SRCHCR_OUT_OF_SERV_TMR_PCH_IN_FRAMES.
02/09/04   yiz     Add SRCHCR_GSM_RSSI_THRESHOLD_FOR_BSIC as -110 dBm
12/10/03   yiz     Add srchcr_fach_gsm_calc_rank() for 
                   FEATURE_INTER_RAT_CELL_FACH_RE_SELECTION
12/09/03   scm     Add extern to srchcr_reset(),
11/12/03   sh      Support for inter-frequency cell reselection in idle mode.
10/10/03   sh      Added proto for function void srchcr_reset_t_resel( ).
08/11/03   sh      Correct Qhyst2s_add equation.
07/28/03   sh      Defined full search timer in fach when no meas occasions
                   are specified. Clip step1 peaks to 6 in fach. Added func
                   proto srchcr_check_full_srch_timer_expired().
06/26/03   sh      Modify calculation of additional Qhyst2.
06/23/03   sh      Define SRCHCR_MAX_STEP1_PEAK_CNT to clip step 1 peak count.
                   Declared srchcr_last_known_good_aset_pos to snapshot aset
                   position.
06/11/03   sh      Added flag srchcr_proc_intra_f_srch_due and funtion proto
                   srchcr_reset_proc_start_n_srch().
04/17/03   sh      Changed internal threshold for Sintrasrch to -14.
                   Declared out of service timer for FACH.
                   Added proto srchcr_set_out_of_service_timer_flag().
03/27/03   sh      Added function proto srchcr_start_cell_reselection_eval()
                   to be called by DRX after it completes setting up the
                   downlink channels. This is done only in the case when
                   initial reacquisition has succeeded.
03/25/03   sh      Modified args to function  srchcr_add_pccpch_n().
02/12/03   sh      Added extern srchcr_drx_waiting_on_srch_sleep.
01/29/03   sh      Added extern srchcr_pending_cell_resel_bsic_verif.
                   Changed srchcr_initial_intra_f_srch_in_progress to
                   srchcr_start_initial_srch.
01/06/03   sh      Added flag srchcr_initial_intra_f_srch_in_progress.
10/01/02   ddh     Added type for specifying initial or reacq srch
08/27/02   sh      Added noff to save nbr offset to srchcr_last_nbr_info_type.
07/31/02   sh      Externed srchcr_full_srch_req.
07/11/02   sh      Macro to calculate additional hysterisis based on ecio.
                   Constants for out of service timers during pch/fach.
06/07/02   sh      Added flag srchcr_start_cell_reselection.
06/04/02   sh      Externed variables related to out of service timer and 1
                   sec timer for starting cell reselection after we go to drx.
05/18/02   sh      Removed srch_cell_reselect_check().
04/29/02   sh      Added protos for step1/2/3 and list search functions.
03/27/02   sh      Created file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "wl1m.h"

#include "srchcmd.h"
#include "srchcrutils.h"
#include "srchset.h"
#include "srchcrgsm.h"
#include "srchfach.h"
#if defined(FEATURE_WCDMA_TO_LTE)
#include "l1rrcif.h"
#endif
#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
#include "wsrchlte.h"
#include "wsrchiratmeas.h"
#endif

#ifdef FEATURE_WCDMA_HS_FACH_DRX
#include "srchfachedrx.h"
#endif
#include "drx.h"
#include "srchinterf.h"
#ifdef FEATURE_WCDMA_REACQLESS_PICH   
#include "wsrchlte.h"
#endif
/*===========================================================================

                        CONSTANT DECLARATIONS

===========================================================================*/
/* The max number of intra-frequency cells in Unlisted Set in Idle mode
 * and FACH. The number is chosen so that <srch_sync_nset_cnt+srch_uset_cnt>
 * is 41, same as SRCH_LPQ_MAX_TASKS.
 */
#define SRCHCR_USET_MAX  9

/* Smin value to calculate an internal threshold value for Sintrasrch
 * when Sintrasrch is specified OTA. Tuneable.
 */
#define SRCH_SMIN_FOR_SINTRA -14

/* Additional hysterisis to be used with Qhyst2s
 * Qhyst2s_add = 2 dB,             if Ec/Io < -12dB
 *             = (14+Ec/Io) dB,    if -12dB < Ec/Io < -10dB
 *             = 4dB,              if Ec/Io > -10dB
 */
#define SRCHCR_QHYST2_ADD(ecio)                               \
  (ecio < -24) ? 4 :                                          \
   ( ((ecio <= -20) && (ecio >= -24)) ? (28+(ecio)) : 8 );

/* Minimum value of CPICH Ec/Io that will trigger a full srch. This
 * corresponds to -16dB. This should be compared with Ec/Io returned
 * from the srch interface.
 */
#define SRCHCR_ASET_MIN_ECIO 32

#ifdef FEATURE_FEMTO_CSG
#define SRCHCR_CSG_PANIC_ECIO_THRESH 40 /* Threshold to trigger Panic Intra Full srch when camped on CSG cell */
#define SRCHCR_CSG_RESEL_ECIO_THRESH 36 /* Minimum threshold for CSG cell to qualify as a resel candidate */
#define SRCHCR_CSG_W1_THRESH 20         /* Range W1 = -10.  Constant defined for 2*ECIO */
#define SRCHCR_CSG_W2_THRESH 36         /* Range W2 = -18.  Constant defined for 2*ECIO */
#define SRCHCR_CSG_LTE_RSRP_THRESH -100
#define SRCHCR_CSG_LTE_RSRQ_THRESH -18
#endif

/* Number of frames in 1 second */
#define SRCHCR_1_SEC_TIME_IN_FRAMES 100

/* Number of frames in 30 seconds */
#define SRCHCR_30_SEC_TIME_IN_FRAMES 3000

/* Max number of step 1 peak count to use for step 2 searching */
#define SRCHCR_MAX_STEP1_PEAK_CNT   6

/* Full search timer when no measurement occasions are specified
 * or when no GSM measurements are performed (number of frames).
 */
#define SRCHCR_FULL_SRCH_TIMER_WITH_NO_MEAS_OCCASION 36

/* -110 dBm as minimum average RSSI for a cell to have BSIC meas */
#define SRCHCR_GSM_RSSI_THRESHOLD_FOR_BSIC  SRCHGSM_MIN_ACCEPTABLE_RX_LVL_dBm_x16

/* Maximum value of Ncr, used to define the array size for 
 * storing reselection times 
 */
#define L1_MAX_N_CR_VALUE  16

/* Maximum value of PENALTY_TIMER_n that NW can set for a neighbor
 * cell in SIB11/12 (in seconds)
 */
#define L1_MAX_PENALTY_TIMER   60

/* Value mapping tmp_offset to infinity */
#define L1_TMP_OFFSET_INFINITY   50

#define SRCHCR_NV_MAX_TX_PWR_INCREMENT 1

#define SRCHCR_RESET_RESEL_EVAL_MEAS_PENDING(reset_meas)\
        srchcr_res_eval_meas_pending = (srchcr_res_eval_meas_type) ((uint16) srchcr_res_eval_meas_pending & \
          (~(uint16)(reset_meas)));

#ifdef FEATURE_DUAL_SIM
#define SRCHCR_RESET_SPLIT_RESEL_EVAL_MEAS_PENDING(reset_meas)\
        do { \
           if(srchcr_split_res_eval_meas_pending & reset_meas) SRCHCR_RESET_RESEL_EVAL_MEAS_PENDING(reset_meas); \
           srchcr_split_res_eval_meas_pending = (srchcr_res_eval_meas_type) ((uint16) srchcr_split_res_eval_meas_pending & \
              (~(uint16)(reset_meas))); \
        } while(FALSE); 
#endif

#define SRCH_INTRAF_NSET_CNT_MAX  (SRCH_SYNC_NSET_MAX_C0 + SRCHCR_USET_MAX)
/* Macro to indicate if inter freq resel is in progress */
#define INTERF_RESEL_IN_PROGRESS() (srchcr_inter_f_cell_ptr != NULL)

#ifdef FEATURE_DUAL_SIM
#define INTRAF_RESEL_IN_PROGRESS() (srchcr_pending_cell_resel && (srch_cr_cell_ptr != NULL))
#endif

#define MAX_PEAK_IDX 3
#define INTRA_F_PEAK_IDX 2

/* RF Tune Time in 256 Chip units - 10 ticks post RF tune call, before search 
 * command is issued in idle drx mode */
#define RF_TUNE_TIME_DRX   10

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
/* global flag to disable / enable Absolute Priotity + LTE related F3s for debug purpose */
/* FALSE - Disable (default), TRUE - Enable */
extern boolean srchcr_abs_prio_f3_debug;

/* Min srch period in High Priority Scheduling mode */
#define SRCHCR_HIGH_PRIO_MIN_SRCH_PERIOD_FRAMES 6000

#define FEFACH_DEBUG_F3 0

#endif

/*Default value for the Treselection scaling factor*/
#define SRCHCR_DEFAULT_TRESEL_SCALE  4

/*Maximum size for the FIFO buffer used for neighbor search opt*/
#define MAX_SRCH_BUFFER_SIZE 375

#ifdef FEATURE_WCDMA_REACQLESS_PICH
extern boolean srchcr_check_for_active_gsm_nbr_cells(void);

  #ifdef FEATURE_WCDMA_TO_LTE
  #define SRCHCR_NGBRS_PRESENT()\
      ((srchsetutil_get_sync_nset_count_c0() > 0)          || \
       (srchsetutil_get_interf_sync_nset_count() > 0)      || \
       (srchsetutil_get_uset_count_c0() > 0)               || \
       (srchcr_check_for_active_gsm_nbr_cells())\
       || (wsrchlte_get_kdetected() > 0))
  #else
  #define SRCHCR_NGBRS_PRESENT()\
      ((srchsetutil_get_sync_nset_count_c0() > 0)          || \
       (srchsetutil_get_interf_sync_nset_count() > 0)      || \
       (srchsetutil_get_uset_count_c0() > 0)               || \
       (srchcr_check_for_active_gsm_nbr_cells()))
  #endif

#endif
/*===========================================================================

                        TYPE DECLARATIONS

===========================================================================*/

/* Structure to hold parameters of the last cell RRC set up the N-PCCPCH
 * for after a cell reselection indication, for use during cell transition.
 */
typedef struct
{
/* SFN-SFN difference between the serving cell and the neighbor we
 * may be reselecting to.
 */
  int16 sfn_sfn_diff;

  /* frequency */
  uint16 freq;

  /* nbr pccpch offset from serving cell */
  int32 noff;  

  /* whether this is a inter-frequency neighbor */
  boolean is_inter_f;

} srchcr_last_nbr_info_type;

/* Enumeration to describe what kind of neighbor search is being started.
 * The behaviour varies for performing inter-frequency and inter-RAT 
 * searches depending on the type of neighbor search being performed.
 */
typedef enum
{
  SRCHCR_INITIAL_CR_SRCH,    /* Initial srch for entering DRX */
  SRCHCR_REACQ_FAIL_CR_SRCH, /* Nset srch for a failed reacquisition */
  SRCHCR_RESEL_CR_SRCH       /* Nset srch for normal reselection */
} srchcr_nset_srch_type;

/* Structure used to store the time at which UE sucessfully reselects.
 * This information is needed to calculate if the UE is in normal state
 * or fast moving state
 * Note that both Tcrmax and Tcrmaxhyst are in seconds and hence as a 
 * first pass the reselection times are being stored at a second 
 * granularity to avoid qword computations. If needed it can be 
 * implemented at a ms granularity using qword data type
 */
typedef struct
{
  /* Index into the reselection times structure */
  uint16 res_time_index;

  /* Time in sec when UE completes a successful reselection.
   * This is one more than Max Ncr value since UE declares
   * fast moving when num reselection in time period Tcrmax
   * EXCEEDS Ncr
   */
  dword res_time[L1_MAX_N_CR_VALUE + 1];
} srchcr_res_times_struct_type;
                           
extern srch_cell_struct_type *srchcr_sorted_intra_f_nset_ptr[SRCH_SYNC_NSET_MAX_C0 + SRCHCR_USET_MAX];

/*Flag to indiate RRC that acq is failing because of a band mismatch*/
extern boolean srchacq_barred_band_mismatch;

/* Enum to indicate status of cell reselection evaluation */
typedef enum 
{
  SRCHCR_RES_EVAL_DISABLED,                    /* Before 1s timer expiry */
  SRCHCR_RES_EVAL_INACTIVE,                    /* Possible now (1s timer has expired)  */
  SRCHCR_RES_EVAL_PENDING_MEAS_COMPLETE
} srchcr_res_eval_status_type;

/* Enum to indicate eval meas type */
typedef enum {
  SRCHCR_RES_EVAL_MEAS_NONE         = 0x0,
  SRCHCR_RES_EVAL_MEAS_INTRA_LIST   = 0x1,
  SRCHCR_RES_EVAL_MEAS_INTRA_FULL   = 0x2,
  SRCHCR_RES_EVAL_MEAS_INTRA        = 0x3,
  SRCHCR_RES_EVAL_MEAS_INTER_0_LIST = 0x4,
  SRCHCR_RES_EVAL_MEAS_INTER_0_FULL = 0x8,
  SRCHCR_RES_EVAL_MEAS_INTER_0      = 0xC,
  SRCHCR_RES_EVAL_MEAS_INTER_1_LIST = 0x10,
  SRCHCR_RES_EVAL_MEAS_INTER_1_FULL = 0x20,
  SRCHCR_RES_EVAL_MEAS_INTER_1      = 0x30,
  SRCHCR_RES_EVAL_MEAS_INTER        = 0x3C,
  SRCHCR_RES_EVAL_MEAS_IRAT_RSSI    = 0x40,
  SRCHCR_RES_EVAL_MEAS_IRAT_BSIC    = 0x80,
  SRCHCR_RES_EVAL_MEAS_IRAT         = 0xC0,
  SRCHCR_RES_EVAL_MEAS_INTER_IRAT   = 0xFC,
  #if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
  SRCHCR_RES_EVAL_MEAS_LTE_CELL_DETECT = 0x100,
  SRCHCR_RES_EVAL_MEAS_LTE_CELL_MEAS = 0x200,
  /*This should be used only for W2L one time forced detect feature*/
  SRCHCR_RES_EVAL_MEAS_LTE_CELL_DETECT_FORCED = 0x400, 
  SRCHCR_RES_EVAL_MEAS_LTE       = 0x700,
  SRCHCR_RES_EVAL_MEAS_NON_INTRA = (SRCHCR_RES_EVAL_MEAS_INTER_IRAT|SRCHCR_RES_EVAL_MEAS_LTE),
  SRCHCR_RES_EVAL_MEAS_ALL       = 0x7FF
  #else
  SRCHCR_RES_EVAL_MEAS_NON_INTRA = SRCHCR_RES_EVAL_MEAS_INTER_IRAT,
  SRCHCR_RES_EVAL_MEAS_ALL       = 0xFF
  #endif
} srchcr_res_eval_meas_type;

/* Enum to find out previous state and present state                  */
/* F O R M A T     ---  Previousstate_PresentState                    */
/* X - stands for               not measured state                          */
/* L - Low priority             cells measure state                          */
/* E - Equal priority           cells measure state                         */
/* H - High priority            cells measure state                         */
/*                    E X A M P L E                                                  */
/*   LEH_LEH ==> LowEqualHIgh_LowEqualHigh                        */
/* Previous state measured LEH Present state measures LEH     */

typedef enum
{
  LEH_XEH,
  LEH_LEH,
  LEH_LEX,
  LEX_XEH,
  LEX_LEH,
  LEX_LEX,
  XEH_XEH,
  XEH_LEH,
  XEH_LEX

} prio_gp_state;

/*Struct to indicate a combination of 
  CellFACH_Abs_Prio_Indicator from SIB19 and
  UE capability*/
typedef enum
{
  HIGH_HIGH  = 0x0,
  HIGH_ALL   = 0x1,
  ALL_HIGH   = 0x2,
  ALL_ALL    = 0x3,
  LEGACY     = 0x4
} srchcr_fefach_ue_cap_and_sib19_ind_enum_type;

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
typedef struct
{

  /* 25.133 section 4.2.2.5a
     The total number of configured higher priority E-UTRA 
    and UTRA FDD carrier frequencies and is additionally 
    increased by one if one or more groups of GSM frequencies 
    is configured as a higher priority
    Thigher_priority_search = 60 * Nlayers */

  uint32 nlayers;
#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
  uint8 nlayers_fach;
#endif

    /* 25.133 section 4.2.2.3
     The total number of carriers used for FDD cells.
     The UE shall measure CPICH Ec/Io and CPICH RSCP of detected
     Inter-Frequency cells at least every (Ncarrier-1) * TmeasureFDD */

  uint32 ncarrier;

  /* Indicates if priority based reselection is to be used
     for Inter-Freq layers */
  boolean interf_prior_info_present;

  /* Indicates if any of the Inter-Freq layers configured by
     n/w are higher priority then serving cell */
  boolean higher_prior_w_freq_present;

  /* Indicates if priority based reselection is to be used
     for GSM layers */
  boolean gsm_prior_info_present;

  /* Indicates if any of the GSM Arfcns configured by
     n/w are higher priority then serving cell */
  boolean higher_prior_gsm_arfcn_present;

  boolean g_arfcn_found_in_high_prio_srch;

  #ifdef FEATURE_WCDMA_TO_LTE
  /* Indicates if LTE EARFCNs have been configured in the 
     Neighbor list */
  boolean lte_info_present;

  /* Indicates if any of the LTE EArfcns configured by
     n/w are higher priority then serving cell */
  boolean higher_prior_lte_earfcn_present;

  /* Count of the number of higher prior LTE Earfcns */
  uint16 num_higher_prior_lte_earfcn;
  #endif

  /*Variables to store if AbsPrio in FACH measurement are included in SIB19*/
  boolean fach_abs_prio_meas_incl;
  srchcr_fefach_ue_cap_and_sib19_ind_enum_type fach_abs_prio_ue_cap_sib_ind;
  #ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
  int8 highest_interf_prio;
  int8 highest_gsm_prio;
  #endif
}srchcr_prior_info_for_rats_struct_type;

#endif

typedef enum
{
  SRCHCR_IRAT_MEAS_IDLE_STATE,
  SRCHCR_IRAT_MEAS_DECONFIG_STATE,
  SRCHCR_IRAT_MEAS_FW_STOP_AGC_STATE,
  SRCHCR_IRAT_MEAS_READY_FOR_GSM,
  SRCHCR_IRAT_MEAS_READY_FOR_LTE
}
srchcr_irat_meas_state_enum_type;

#define SRCHCR_COND_CNT_MAX 9

typedef struct
{
    uint16 ecio;
    uint16 rscp;
    uint32 total_eng;
    int16 rxagc;
} fifo_buffer_struct_type;

typedef struct
{
  fifo_buffer_struct_type fifo_buffer[MAX_SRCH_BUFFER_SIZE];
  uint8 idx;
  uint32 candngbrecio;
  int32 candngbrrscp;
  boolean pn_srch_performed;
  uint32 pn_performed_in_drx_cycle;
  int16 q_offset1_min, q_offset2_min, q_rxlevmin_min, q_qualmin_min, max_tx_pwr_min;
  boolean algo_evaled;
  boolean cond[SRCHCR_COND_CNT_MAX];
  int16 step1_eng_th;
  uint32 std_dev_square;
  uint8 rscp_vals_considered;


} srchcr_ngbr_srch_opt_strcut_type;

extern srchcr_irat_meas_state_enum_type srchcr_irat_meas_state;

#ifdef FEATURE_FEMTO_CSG
extern srch_interf_cell_struct_type* srchset_get_interf_cell(uint16 freq, uint16 scr_code);

#ifdef FEATURE_WCDMA_TO_LTE
typedef struct
{
  /*physical cell id of CSG cell*/
  uint16 cell_id;
  /* The earfcn for searching LTE cells on */
  uint32 earfcn;
  /* sss correlation value, used for ranking cell detection results in CM */
  int16 rsrp;
  /* Max RSRQ value combined across Tx-Rx pairs. In linear scale. */
  int16 rsrq;
  /* Max RSSI value combined across Tx-Rx pairs. In linear scale. */
  int16 rssi;
  int16 s_rxlev;
  int16 s_qual;
  int8 p_max;
  uint32 csg_id;
  /* LTE priority info structure */
  wsrchlte_priority_info_type prior_info;
} srchcr_csg_lte_cell_struct_type;
#endif


#define MAX_INTRA_ASF_CELLS 6
typedef struct 
{
  boolean intraf_csg_cells_included; 
  boolean interf_csg_cells_included; 
  uint8 num_tot_asf_cells;
  uint8 num_intra_cells;
  uint8 num_inter_cells;
  uint8 num_lte_cells;
  boolean is_csg_meas_req_processed;
  srch_cell_struct_type* w_intraf_csg_list[MAX_INTRA_ASF_CELLS]; 
  srch_interf_cell_struct_type* w_interf_csg_list[MAX_ASF_CELLS];
  #ifdef FEATURE_WCDMA_TO_LTE
  srchcr_csg_lte_cell_struct_type* lte_csg_list[MAX_ASF_CELLS];
  #endif
  /* bitmask per freq to say whether srch is pending / done on all freq's */
  uint32 csg_meas_pending;
} srchcr_csg_info_struct_type;
 
srchcr_csg_info_struct_type srchcr_csg_info;

#define INVALID_CSG_ID 0xFFFFFFFF
#endif

/*===========================================================================

                   EXTERNAL VARIABLE DECLARATIONS

===========================================================================*/
/* Extern the variable here since its used in srchpch cleanup */
extern uint16 srchcr_sorted_inter_f_nset_cnt[SRCH_OFREQ_FREQ_MAX];

/* Flag to indicate if cell reselection evaluation needs to be started
 * after DRX has completed downlink channel setup.
 */
extern boolean srchcr_wait_drx_start_cell_resel;

/* Timer to keep track number of frames to send out of service ind */
extern uint8 srchcr_out_of_service_timer;

/* Flag to indicate that the out of service timer has expired */
extern boolean srchcr_out_of_service_timer_expired;

/* Flag to indicate that W2G failed and out of service timer has expired */
extern boolean srchcr_resume_after_w2g_fail_OOS_expired;

/* Flag to indicate to DRX to count 1 second before enabling cell reselection */
extern boolean srchcr_start_1_sec_timer;

/* Flag to indicate 1s has elapsed since DRX to start cell reselection */
extern boolean srchcr_start_cell_reselection;

/* Flag to indicate if cell reselection related processing
 * should be done.
 */
extern boolean srch_cell_reselect_enabled;

/* Flag to indicate if a cell reselection is pending */
extern boolean srchcr_pending_cell_resel;

/*Structure which carries the values for Neighbor search optimizations(FR_1947) Algo*/
srchcr_ngbr_srch_opt_strcut_type srchcr_values_for_full_srch[3];

/* Cell pointer to the WCDMA neighbor cell to be reported to RRC via
 * RRC_CPHY_CELL_RESELECTION_IND.
 */
extern srch_cell_struct_type *srch_cr_cell_ptr;

/* Cell pointer to a WCDMA Intra-F neighbor cell which became stronger
 * during a intra-freq cell reselection. In case of a cell reselection failure
 * this pointer will be used to trigger fresh reselection.
 */
extern srch_cell_struct_type *srchcr_strong_cell_ptr;
/* Variable that lets us know that interf reselection has failed and we will
   have to pend rach till 20ms ASET search is done in FACH */
extern boolean srchcr_interf_resel_fail;

/* Cell pointer to the WCDMA Inter-frequency neighbor cell to be
 * reported to RRC via RRC_CPHY_CELL_RESELECTION_IND.
 */
extern srch_interf_cell_struct_type* srchcr_inter_f_cell_ptr;

/* Flag to indicate if a inter-frequency list search is required */
extern boolean srchcr_inter_f_list_srch_req;

/* Cell pointer to the GSM neighbor cell to be reported to RRC via
 * RRC_CPHY_CELL_RESELECTION_IND.
 */
extern srch_gsm_cell_struct_type* srchcr_gsm_cell_ptr;

#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
/* Cell pointer to the LTE neighbor cell to be reported to RRC via
 * RRC_CPHY_CELL_RESELECTION_IND.
 */
extern wsrchlte_cell_struct_type* srchcr_lte_cell_ptr;
#endif

extern srchcr_last_nbr_info_type srchcr_last_nbr;

/* Sorted list of GSM neighbor cells, sorted according to RSSI */
extern srch_gsm_cell_struct_type*
        srchcr_sorted_gsm_nset_ptr[ SRCH_GSM_NSET_MAX ];

/* flag to indicate that a BSIC verification is due for the GSM
 * neighbor before sending the reselection indication.
 */
extern boolean srchcr_pending_cell_resel_bsic_verif;

/* Flag to indicate that a RSSI scan is pending */
extern boolean srchcr_t_meas_gsm_timer_expired;

/* Flags to indicate type of search in progress during cell reselection */
extern boolean srchcr_inter_s_srch_in_progress;

/* Flag to indicate if drx is waiting on srch ready to sleep */
extern boolean srchcr_drx_waiting_on_srch_sleep;

/*variable holding the remaining OOS timer value used during state transition*/
extern rex_timer_cnt_type srchcr_rem_oos_time;

/*variable holding the remaining Cell Resel timer value used 
  during state transition*/
extern rex_timer_cnt_type srchcr_rem_CR_time_ms;

/*Flag indicating whether CR timer has expired and
  srchcr_init() should be called in DRX*/ 
extern boolean srchcr_start_CR;

/* Tresel value to be used for GSM resel*/
extern uint16 srchcr_tresel_gsm;

/*Tresel value to be used for LTE resel*/
extern uint16 srchcr_tresel_lte;

/* Global to indicate reselection evaluation needs to be performed or not */
extern srchcr_res_eval_status_type       srchcr_res_eval_status;

/* Global to inidcate which meas are pending for reselection evaluation */
extern srchcr_res_eval_meas_type         srchcr_res_eval_meas_pending;

#ifdef FEATURE_DUAL_SIM
/* Global to inidcate which meas are pending for reselection evaluation in a split search mode */
extern srchcr_res_eval_meas_type         srchcr_split_res_eval_meas_pending;
#endif

/* Global to indicate which meas have been completed for reselection evaluation */
extern srchcr_res_eval_meas_type         srchcr_res_eval_meas_completed;

/* Global to indicate current meas rules type */
extern srchcr_meas_rules_type srchcr_current_meas_rules;

/* frequency index that is currently being searched */
extern srchcr_inter_f_idx_enum_type srchcr_inter_f_idx;

/* Number of drx cycles the cell selection criteria has failed continously */
extern uint8 srch_cell_select_failed_drx_cycles;

/* Table of Nserv values indexed by K values. K = 0..2 is not valid */
extern uint8 srch_cell_select_nserv_tab[10];

/* Table of TmeasureGSM values indexed by K values. K = 0..2 is not valid */
extern uint8 srchcr_t_meas_gsm_tab[10];

/* Table of TmeasureFDD values indexed by K values. K = 0..2 is not valid */
extern uint8 srchcr_t_meas_fdd_tab[10];


#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY

extern srchcr_prior_info_for_rats_struct_type srchcr_prior_info_for_rats;
extern srchcr_prior_scheduler_mode_enum_type srchcr_curr_prior_scheduling_mode;

#endif

/* global boolean to indicate an external CELL_BAR_REQ message has arrived from RRC but not processed. */
extern boolean srchcr_cell_bar_ext_msg_recd;

/* Table of filter coeffiecients indexed based on drx cycle length*/
extern uint32 srch_idle_mode_filt_coeff_q10[10];

/* Table of idle mode filtering coefficients, indexed by DRX cycle length for neighbor searches. */
extern uint32 srch_idle_mode_filt_coeff_ngbr_q10[10];

/* Temp struct to store first reacq llist srch values */
extern srch_cell_struct_type  temp_srch_aset_ptr;
/* Temp global to save the Prev DRX Meas rules */
extern srchcr_meas_rules_type srchcr_prev_meas_rules;
/* stores the previous DRX cycle's filt_eng to track changes between cycles */
extern uint32 srch_prev_drx_filt_eng;

/* Flag to indicate we are tuned away from serving frequency */
extern boolean srchcr_rf_tuned_away;

extern uint16 srchcr_sorted_intra_f_nset_cnt;

/* The number of monitored cells to evaluate for reselection */
extern uint8 srchcr_inter_f_eval_list_num[SRCH_OFREQ_FREQ_MAX];

extern uint8 srchcr_intra_f_eval_list_num;
/* List of sorted Inter-frequency cells per frequency */
extern srch_ofreq_cell_list_struct_type
      srchcr_sorted_inter_f_nset_ptr[SRCH_OFREQ_FREQ_MAX];

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

extern void srchcr_reset_rftune_state(boolean skip_if_interf_resel);

/*===========================================================================

FUNCTION       SRCHCR_EXCLUDE_CELL_FROM_RANKING

DESCRIPTION
  This function iterates through the searched cells list and if it
  finds the cell pointed to by the passed pointer, will mark this
  cell to be excluded from ranking during the current evaluation.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Will set the exclude flag to true for the cell pointed
  to by cell_ptr.
===========================================================================*/
void srchcr_exclude_cell_from_ranking( void* cell_ptr );

/*===========================================================================

FUNCTION       SRCHCR_RESET_OOS_TIMER

DESCRIPTION
  This function resets the OOS timer and clears the signal and other
  related flags.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchcr_reset_oos_timer( void );

/*===========================================================================
FUNCTION     SRCHZZ_FORCE_RANK_W_CELLS

DESCRIPTION
  This function is called at the beginning of check force rank to force rank W cells
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void srchzz_force_rank_w_cells(srchcr_res_eval_meas_type *res_eval_meas_comp);


/*===========================================================================

FUNCTION     SRCHZZ_CHECK_FORCE_RANKING

DESCRIPTION
  This function is called at the beginning of cell reselection evaluation to check whether or not cells need to be forcefully ranked
  during the current evaluation. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_check_force_ranking(srchcr_res_eval_meas_type *res_eval_meas_comp);

/*===========================================================================

FUNCTION        SRCHCR_SET_OUT_OF_SERVICE_TIMER_FLAG

DESCRIPTION
  This function sets the srchcr_out_of_service_timer_expired flag when
  the out of service timer expires.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  srchcr_out_of_service_timer_expired flag is set.

===========================================================================*/
extern void srchcr_set_out_of_service_timer_flag( void );

/*===========================================================================

FUNCTION        SRCHCR_INIT

DESCRIPTION
  This function initializes this module to start cell reselection
  evaluation. This is called 1 second after we have camped on a cell.
  Currently we enable cell reselection 1 second after we receive
  the DRX command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  srch_intra_cell_reselect_enabled is set to TRUE.
  srch_ignore_heartbeat_evt is set to TRUE.
  All the NSET neighbor cell cell reselection timers are initialized.

===========================================================================*/
extern void srchcr_init( void );

/*===========================================================================

FUNCTION        SRCHCR_RESET

DESCRIPTION
  This function resets the variables related to cell reselection. This is
  called while suspending or ending cell reselection evaluation. It is also
  called from drx_restart(), which is called on cell reselection failure.
  This gets DRX and cell reselection searches started up cleanly again.

DEPENDENCIES
  This function should be called in L1M task context.

RETURN VALUE
  None.

SIDE EFFECTS
  srch_cell_reselect_enabled is set to FALSE
  and other search related variables are reset.

===========================================================================*/
extern void srchcr_reset( void );

/*===========================================================================

FUNCTION        SRCHCR_CLEANUP

DESCRIPTION
  This function resets the variables related to cell reselection. This is
  called while suspending or ending cell reselection evaluation. DRX is
  notified if it is waiting on search ready for sleep.

DEPENDENCIES
  This function should be called in L1M task context.

RETURN VALUE
  None.

SIDE EFFECTS
  srch_cell_reselect_enabled is set to FALSE
  and other search related variables are reset.

===========================================================================*/
void srchcr_cleanup( l1m_state_enum_type next_state );

/*===========================================================================

FUNCTION      SRCHCR_NSET_CELL_RESELECT_EVAL

DESCRIPTION
  This function will rank the serving and neighbor cells according to
  the criteria specified in 25.304 (Section 5.2.6.1.4). If the neighbor
  cell meets the cell selection criteria, the cell reselection timer,
  Treselection, associated with the neighbor cell is incremented.
  If the neighbor cell meets the cell selection criteria for Treselection,
  and if the rank of this neighbor cell is better than the rank of the
  serving cell, it is selected as the best cell suitable for reselection.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the best cell (either the current serving cell or a new
  neighbor cell), NULL if no best cell was found.

SIDE EFFECTS
  None.
===========================================================================*/
extern srch_cell_struct_type* srchcr_nset_cell_reselect_eval( void );

/*===========================================================================

FUNCTION     srchcr_find_best_cell_and_reselect

DESCRIPTION
  This function is called when we first enter DRX and also after List search
  on SYNC_NSET and possibly a Step 1/2/3 search on ASYNC_NSET neighbor cells
  completes. This function will sort all the neighbor cells according to
  their energies and performs nset evaluation for cell reselection.

  If a cell was found better than the serving cell, send a
  RRC_CPHY_CELL_RESELECTION_IND indication to RRC.

  If the serving cell became the most suitable cell, report reacquisition
  success to DRX module.

  If no best cell was found, it means we lost the serving cell and also
  could not find a better neighbor cell. Send reacquisition failed status
  to DRX module.

  If the DRX module is waiting for the reacquisition/cell reselection
  evaluation to complete, notify DRX module we are done.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_find_best_cell_and_reselect( boolean use_legacy);

/*===========================================================================

FUNCTION      SRCHCR_NSET_LIST_SRCH_DONE

DESCRIPTION
  This function is called when a NSET list search completes. If a step1
  search is due, this function will initiate a step1 search.

  If there is no more searching to be done, send SRCH_NSET_SRCH_SORT_DONE_CMD
  local command to L1M. If this search was started when we first entered DRX,
  sort and return to L1M. If we are performing reacquisition after coming
  out of sleep, L1M will send command to sort the nset and continue with
  cell reselection evaluation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_nset_list_srch_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION      SRCHCR_INTRA_F_POST_LIST_SRCH_DUMP

DESCRIPTION
  This function calculates the ECIO and filt ECIO of the cells that
  were searched.

DEPENDENCIES
  This should be called after a list search is done and the
  function srch_list_srch_dump() is called.

RETURN VALUE
  None.

SIDE EFFECTS
  ECIO and Filt ECIO of searched cells are updated.
===========================================================================*/
void srchcr_intra_f_post_list_srch_dump( srch_queue_enum_type que );

/*===========================================================================

FUNCTION       SRCHCR_SORT_ENTIRE_NSET

DESCRIPTION
  This function sorts all the neighbor cells in the SYNC_NSET according
  to increasing order of their energies.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_sort_entire_nset( void );

/*===========================================================================

FUNCTION       srchcr_sort_intraf_nset

DESCRIPTION
  This function sorts just the neighbor cells that were just searched
  according to decreasing order of their filtered energies.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_sort_intraf_nset( void );

/*===========================================================================

FUNCTION      SRCHCR_ADD_PCCPCH_N

DESCRIPTION
  This function is called to set up the PCCPCH on the neighbor during
	cell reselection. A SFN-SFN difference between the neighbor and
	serving cell is performed after the PCCPCH_N has been successfully
	set up.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean indicating if the channel was added successfully or not.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcr_add_pccpch_n
(
  dl_phychan_add_type* phch_cmd,
  uint16 freq
);

/*===========================================================================

FUNCTION      SRCHCR_CHK_N_SRCH_CMD

DESCRIPTION
  This function will start an inter-frequency or an inter-RAT search if
  one is required, otherwise any search results will be processed to 
  determine if a cell reselection indication should be sent to RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_chk_n_srch_cmd( void );

/*===========================================================================

FUNCTION      srchcr_start_gsm_srch_on_gsm_clk_en

DESCRIPTION
  This function will start an inter-RAT search once Tx System Time and
  GSM clk gets enabled for GSM seacrhes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_start_gsm_srch_on_gsm_clk_en( void );

/*===========================================================================

FUNCTION      SRCHCR_UPDATE_NLISTS

DESCRIPTION
  This function will set the flags so the sorted neighbor lists will be 
  updated correctly during the next reselection pass.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_update_nlists( void );

/*===========================================================================

FUNCTION      SRCHCR_RESET_T_RESEL

DESCRIPTION
  This function the Treselction timers associated with each neighbor cell.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_reset_t_resel( void );

/*===========================================================================

FUNCTION       SRCHCR_GSM_NSET_SRCH_CMD

DESCRIPTION
  This function processes a SRCHCR_GSM_NSET_SRCH_CMD

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void  srchcr_gsm_nset_srch_cmd
(
  srchcrgsm_local_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION      SRCHCR_BSIC_IDENTIFY_DONE

DESCRIPTION
  This function performs the next step after a BSIC identification has
  been done. If a PCH cleanup is pending, a module cleanup done command
  will be sent to L1M.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_bsic_identify_done( srchcrgsm_local_cmd_type* cmd_ptr );

/*===========================================================================

FUNCTION       SRCHCR_INTER_F_NSET_STEP_SRCH_CMD

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_inter_f_nset_step_srch_cmd( srchcr_inter_f_idx_enum_type f_idx,
                                        boolean tune_rf );

/*===========================================================================

FUNCTION       SRCHCR_INTER_F_STEP1_SRCH_CMD

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_inter_f_step1_srch_cmd( uint16 freq, boolean tune_rf );

/*===========================================================================

FUNCTION       SRCHCR_INTER_F_STEP2_SRCH_CMD

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_inter_f_step2_srch_cmd( void );

/*===========================================================================

FUNCTION       SRCHCR_INTER_F_STEP3_SRCH_CMD

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_inter_f_step3_srch_cmd( void );
void srchcr_sort_inter_f_nset( void );
void srchcr_nset_inter_f_cell_reselect_eval( void );
void srchcr_sort_entire_inter_f_nset( void );

/*===========================================================================

FUNCTION SRCH_INTER_F_LIST_SRCH_DUMP

DESCRIPTION
  This function retrieve List Search results from DSP memory to 
  search_cells[].

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srch_inter_f_list_srch_dump ( srch_queue_enum_type que );

/*===========================================================================

FUNCTION      SRCHCR_INTER_F_POST_LIST_SRCH_DUMP

DESCRIPTION
  This function calculates the ECIO and filt ECIO of the cells that
  were searched.

DEPENDENCIES
  This should be called after a list search is done and the
  function srch_inter_f_list_srch_dump() is called.

RETURN VALUE
  None.

SIDE EFFECTS
  ECIO and Filt ECIO of searched cells are updated.
===========================================================================*/
void srchcr_inter_f_post_list_srch_dump( srch_queue_enum_type que );

/*===========================================================================

FUNCTION SRCH_INTER_F_LIST_WIN_CTR

DESCRIPTION
  This function performs window centering on cells in the referenced search 
  queue, based on most recent search results. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srch_inter_f_list_win_ctr ( srch_queue_enum_type que );

/*===========================================================================

FUNCTION      SRCHCR_INTER_F_NSET_LIST_SRCH_DONE

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_inter_f_nset_list_srch_done( srch_done_cmd_type* srch_cmd );

/*===========================================================================

FUNCTION       SRCHCR_INTER_F_STEP1_DONE

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_inter_f_step1_done( srch_done_cmd_type* srch_cmd );
void srchcr_inter_f_step2_done( srch_done_cmd_type* srch_cmd );
void srchcr_inter_f_step3_done( srch_done_cmd_type* srch_cmd );

/*===========================================================================

FUNCTION       SRCHCR_TUNE_RF

DESCRIPTION
  This function tunes RF to the specified frequency.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_tune_rf( uint16 freq );

/*===========================================================================

FUNCTION   SRCHCR_FACH_GSM_NSET_RANK_SORT    

DESCRIPTION
  This function calculates the RSCP rank for CELL_FACH GSM neighbor cells
  and sor them in ranking order.

DEPENDENCIES
  Can be called in CELL_FACH state

RETURN VALUE
  None.

SIDE EFFECTS
  srch_gsm_nset_ptr[] is updated
  
===========================================================================*/
void srchcr_fach_gsm_nset_rank_sort(void);

/*===========================================================================

FUNCTION       SRCHCR_ADD_CELL_TO_SRCHED_LIST

DESCRIPTION
  This function adds the specified cell to the list of neighbors that 
  were searched during this cell reselection search cycle.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchcr_add_cell_to_srched_list
(
  /* Type of cell to be added */
  srchcr_cell_type_enum_type    type,
  /* Ptr to Intra-F cell      */
  srch_cell_struct_type        *wcdma_intra_ptr,
  /* Ptr to Inter-F cell      */
  srch_interf_cell_struct_type *wcdma_inter_ptr,
  /* Ptr to GSM cell */
  srch_gsm_cell_struct_type    *gsm_ptr
#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
  ,wsrchlte_cell_struct_type *lte_ptr
#endif
);
/*===========================================================================

FUNCTION       SRCHCR_REMOVE_CELL_FROM_SRCHED_LIST

DESCRIPTION
  This function removes specified cell from the list of neighbors that 
  were searched during this cell reselection search cycle.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchcr_remove_cell_from_srched_list
(
  /* pointer to the cell structure */
  void *cell_ptr
);

/*===========================================================================

FUNCTION       SRCHCR_SIB_MOD_UPDATE

DESCRIPTION
  This function performs SIB modification update if they are pending.
  In case a cphy setup is in progress, this will not clear the SRCH
  and DL module cleanup done flags, since cphy setup request has to
  continue. This function is called when either DL or SRCHFACH module
  completes cleaning up the current searches.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_sib_mod_update( void );

/*===========================================================================

FUNCTION       SRCHCR_CALC_SERVING_CELL_RSCP_RANK

DESCRIPTION
  This function calculates the RSCP rank for the serving cell.

DEPENDENCIES
  Can ONLY be called when RF is in serving frequency in L1 task context.

RETURN VALUE
  None.

SIDE EFFECTS
  srch_aset_ptr[0] is updated
  
===========================================================================*/
void srchcr_calc_serving_cell_rscp_rank(void);


/*===========================================================================

FUNCTION        SRCHCR_INIT_SFN_CTRS

DESCRIPTION
  This function initializes the global storing sfn of last aset list
  search and sfn based full search counters

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Full search sfn counters initialized

===========================================================================*/
void srchcr_init_sfn_ctrs(void);

/*===========================================================================

FUNCTION        SRCHCR_INIT_INTRA_FULL_SRCH_SFN_CTR

DESCRIPTION
  This function initializes intra freq full search sfn counter

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Intra freq full search may get started after next aset list search

===========================================================================*/
void srchcr_init_intra_full_srch_sfn_ctr(void);

/*===========================================================================

FUNCTION        SRCHCR_INIT_INTER_FULL_SRCH_SFN_CTR

DESCRIPTION
  This function initializes inter freq full search sfn counter

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Inter freq full search may get started after next aset list search

===========================================================================*/
void srchcr_init_inter_full_srch_sfn_ctr(void);

#ifdef FEATURE_CELL_FACH_RESEL_CHK_THRESH
/*===========================================================================

FUNCTION       SRCHCR_FACH_EXCLUDE_CELLS_USING_MEAS_RULES

DESCRIPTION
  This function limits the candidates for cell reselection in FACH state
  based on the current meas rules

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  srchcr_srched_cells array of cells is updted
  
===========================================================================*/
void srchcr_fach_exclude_cells_using_meas_rules (void);
#endif

/*===========================================================================

FUNCTION       SRCHCR_CALC_INTRA_NEIGHBOR_PENALTY_AND_H

DESCRIPTION
  This function calculates updates the penalty timer and calculates H
  for the neighbor cell

DEPENDENCIES
  Can ONLY be called when HCS is used on the current serving cell

RETURN VALUE
  None.

SIDE EFFECTS
  srch_cell_struct_type for the neighbor is updated
  
===========================================================================*/
void srchcr_calc_intra_neighbor_penalty_and_h(srch_cell_struct_type* n_ptr,
                                              uint16 time_elapsed,
                                              boolean apply_penalty);

/*===========================================================================

FUNCTION       SRCHCR_CALC_INTER_NEIGHBOR_PENALTY_AND_H

DESCRIPTION
  This function calculates updates the penalty timer and calculates H
  for the neighbor cell

DEPENDENCIES
  Can ONLY be called when HCS is used on the current serving cell

RETURN VALUE
  None.

SIDE EFFECTS
  srch_cell_struct_type for the neighbor is updated
  
===========================================================================*/
void srchcr_calc_inter_neighbor_penalty_and_h(srch_interf_cell_struct_type* n_ptr,
                                              uint16 time_elapsed,
                                              boolean apply_penalty);

/*===========================================================================

FUNCTION       SRCHCR_CALC_GSM_NEIGHBOR_PENALTY_AND_H

DESCRIPTION
  This function calculates updates the penalty timer and calculates H
  for the gsm neighbor cell

DEPENDENCIES
  Can ONLY be called when HCS is used on the current serving cell

RETURN VALUE
  None.

SIDE EFFECTS
  srch_cell_struct_type for the neighbor is updated
  
===========================================================================*/
void srchcr_calc_gsm_neighbor_penalty_and_h(srch_gsm_cell_struct_type* n_ptr,
                                            uint16 time_elapsed);

/*===========================================================================

FUNCTION       SRCHCR_EXCLUDE_CELLS_USING_H

DESCRIPTION
  This function limits the candidates for cell reselection based
  on H value

DEPENDENCIES
  Can ONLY be called when HCS is used on the current serving cell and 
  UE is not in fast moving state

RETURN VALUE
  None.

SIDE EFFECTS
  srchcr_srched_cells array of cells is updted
  
===========================================================================*/
void srchcr_exclude_cells_using_h (void);
      
/*===========================================================================

FUNCTION       SRCHCR_FACH_EXCLUDE_CELLS_USING_MEAS_RULES_HCS

DESCRIPTION
  This function limits the candidates for cell reselection in FACH state
  based on the current HCS meas rules

DEPENDENCIES
  Can ONLY be called after the meas_rules function has correctly set the
  meas_detect flag for the cells.

RETURN VALUE
  None.

SIDE EFFECTS
  srchcr_srched_cells array of cells is updted
  
===========================================================================*/
void srchcr_fach_exclude_cells_using_meas_rules_hcs (void);

/*===========================================================================

FUNCTION       SRCHCR_UPDATE_CELL_RES_TIME

DESCRIPTION
  This function udpate the cell reselection time in the res_time data str

DEPENDENCIES
  Can ONLY be called after a successful cell reselection

RETURN VALUE
  None.

SIDE EFFECTS
  res_time array is updated
  
===========================================================================*/
void srchcr_update_cell_res_time(void);

#ifdef FEATURE_WCDMA_REL7
/*===========================================================================

FUNCTION SRCHCR_UPD_OTA_PARM

DESCRIPTION
  This function updates mobility information, stored in srchcr_ue_fast_moving 
  structure,  based on information conveyed by CPHY_UPD_OTA_PARMS_IND. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchcr_upd_ota_parm(l1_upd_ota_parms_ind_cmd_type *cmd_ptr);
#endif

/*===========================================================================

FUNCTION       SRCHCR_CHECK_FAST_MOVING_CONDITION

DESCRIPTION
  This function checks if the UE is in fast moving condition

DEPENDENCIES
  Can ONLY be called if HCS is enabled and immediately after a 
  CELL_PARM_REQ (SIB3) or a CELL_MEAS_REQ (SIB11/12)

RETURN VALUE
  None.

SIDE EFFECTS
  UE fast moving condition updated
  
===========================================================================*/
void srchcr_check_fast_moving_condition(void);

/*======================================================================
FUNCTION SRCHCR_BACKUP_NEIGH_CELL_PENALTY_TIMER

DESCRIPTION
    Backups the penalty timers for all neighbor cells

RETURN VALUE
    void

SIDE EFFECTS
    backup store is filled in
=======================================================================*/
void srchcr_backup_nbr_penalty_timer(void);

/*======================================================================
FUNCTION SRCHCR_RESTORE_NEIGH_CELL_PENALTY_TIMER

DESCRIPTION
    Restores the penalty timer info for all neighbor cells

RETURN VALUE
    void

SIDE EFFECTS
    Neighbor cell list penalty timers are updated from backup info
=======================================================================*/
void srchcr_restore_nbr_penalty_timer(dword elapsed_time);

/*===========================================================================

FUNCTION       SRCHCR_SEND_DL_WEAK_IND_TO_RRC

DESCRIPTION
  This function sends a CEL RESEL IND to RRC for the PSC passed in

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  CEL RESEL IND sent to RRC
  
===========================================================================*/
void srchcr_send_dl_weak_ind_to_rrc(void);

/*===========================================================================

FUNCTION       SRCHCR_INIT_ON_POWERUP

DESCRIPTION
  This function initializes cell reselection related data structures on 
  power up

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Cell reselection related globals initialized
  
===========================================================================*/
void srchcr_init_on_powerup(void);

/*===========================================================================

FUNCTION       SRCHCR_INIT_GSM_NSET

DESCRIPTION
  This function initializes gsm nset data 
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  
===========================================================================*/
void srchcr_init_gsm_nset( void );

#ifdef FEATURE_IGNORE_TRESEL_ON_S_FAIL
/*===========================================================================

FUNCTION       SRCHCR_CHECK_WEAK_RESEL_CONDITION

DESCRIPTION
  This function check if weak resel condition happened or not and update the flag accordingly.
  weak resel condition is when NCR=10 opt reselections (due to s-fail; bad agc; tresel short
  has been attempted in less than TCR = 60s. 
  This function should be called after receiving TRANSITON_REQ when transitioning to a new cell.
  This function is also called when there is a transition failure. It is based on the number of weak reselections
  attempted.
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  
===========================================================================*/
extern void srchcr_check_weak_resel_condition(void);

/*===========================================================================

FUNCTION       SRCHCR_UPDATE_WEAK_RESEL_CONDITION

DESCRIPTION
  This function checks if weak resel condtion's end time has been reached. If so update reset the weak resel flag.
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  
===========================================================================*/
extern void srchcr_update_weak_resel_times(void);
#endif

/*======================================================================
FUNCTION SRCHCR_SET_NUM_FRAMES_SINCE_PREV_EVAL

DESCRIPTION
  Function to set num frames since previous eval

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
=======================================================================*/
extern void srchcr_set_num_frames_since_prev_eval(void);

/*===========================================================================
FUNCTION       SRCHCR_UPDATE_MEAS_RULES

DESCRIPTION
  This function updates meas rules for all types of meas

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Following globals updated to reflect the latest measurement rules
    srchcr_current_meas_rules
    srchcr_current_meas_prio
===========================================================================*/
extern void srchcr_update_meas_rules(void);

/*===========================================================================
FUNCTION       SRCHCR_ASET_S_CRITERIA_PROCEDURES

DESCRIPTION
  This function performs following S criteria related procedures
  (a) Update aset S criteria
  (b) Check against immediate reselection at low EcIo threshold
  (c) Perform S criteria success or failure procedures

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Serving cell S criteria calculated, Tresel_fdd updated, S criteria
  success or failure procedures performed.
  
===========================================================================*/
extern void srchcr_aset_s_criteria_procedures(void);

/*===========================================================================

FUNCTION      SRCHCR_PERFORM_CELL_RES_EVAL

DESCRIPTION
  This function is called to perform cell reselection evaluation

  It first calls functions to calculate the ngbr cell rank and then 
  If there is no more searching to be done, send SRCHCR_NSET_SRCH_DONE_CMD
  local command to L1M. If this search was started when we first entered DRX,
  sort and return to L1M. If we are performing reacquisition after coming
  out of sleep, L1M will send command to sort the nset and continue with
  cell reselection evaluation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_perform_cell_res_eval(srchcr_res_eval_meas_type res_eval_meas_comp);

/*===========================================================================
FUNCTION       srchcr_perform_cell_res_eval_abs_prio ()

DESCRIPTION
  This function is used only for REL8 UE's when Absolute Priotity 
  is used for resel eval. 
  This is just a wrapper function to which calls the inter-f/gsm/LTE resel eval 
  functions based on the current scheduling mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_perform_cell_res_eval_abs_prio(void);

/*===========================================================================
FUNCTION       SRCHCR_SET_T_RESEL_FDD

DESCRIPTION
  This function set the Treslection value to be used for FDD neighbor cells
  based on serving cell S criteria and EcIo

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Treselection for FDD neighbors set as per optimizations
  
===========================================================================*/
extern void srchcr_set_t_resel_fdd(void);

/*===========================================================================
FUNCTION      SRCHCR_DO_INTERF_STEP1_SRCH

DESCRIPTION
  This function is called to perfom inter freq step1 srch

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if search is started, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcr_do_interf_step1_srch( srchcr_inter_f_idx_enum_type freq_idx, boolean tune_rf );

/*===========================================================================
FUNCTION      SRCHCR_DO_PENDING_PCH_CLEANUP

DESCRIPTION
  This function is called to perform pending PCH cleanup

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_do_pending_pch_cleanup(void);

/*===========================================================================
FUNCTION      SRCHCR_DO_IRAT_SRCH

DESCRIPTION
  This function is called to perfom irat srch - RSSI or BSIC

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if search is started, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcr_do_irat_srch(boolean rssi_due);


/*===========================================================================

FUNCTION       SRCHCR_CALC_ASET_S_CRITERIA

DESCRIPTION
  This function calculates and updates the S criteria of the serving cell.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchcr_calc_aset_s_criteria( void );

/*===========================================================================
FUNCTION      SRCHCR_START_STEP1_SRCH

DESCRIPTION
  This function starts step-1 search in idle mode

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_start_step1_srch(void);

/*===========================================================================
FUNCTION      SRCHCR_START_STEP2_SRCH

DESCRIPTION
  This function starts step-2 search in idle mode

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_start_step2_srch(uint16 peak_idx);

/*===========================================================================
FUNCTION      SRCHCR_START_STEP3_SRCH

DESCRIPTION
  This function starts step-3 search in idle mode

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_start_step3_srch(uint16 peak_idx);

extern void srchcr_update_rssi_all_cell(W_ARFCN_T arfcn, int16 filt_rssi);
extern void srchcr_set_flag_to_sort_entire_nset_after_full_srch(boolean sort);
extern void srchcr_set_flag_to_sort_nset_after_multipath_srch(boolean sort);
void srchcr_eliminate_aset_and_nset_values
(
  srch_cell_struct_type *set_ptr[],
  uint16 max_nof_cells
);
extern void srchcr_eliminate_duplicate_peaks_in_step1_srch( void );
extern void srchcr_eliminate_duplicate_peaks_in_step1_srch_inter( void );
void srchcr_clear_srched_cells(void);
boolean srchcr_aset_squal_meets_gsm_thresh( void );
/*===========================================================================

FUNCTION       srchcr_interf_rank_ecio

DESCRIPTION
  This function calculates the EcIo rank of the inter-frequency neighbor cells and if the cell meets
  cell selection criteria and is better ranked than the serving cell,
  the Treselection will be incremented.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void srchcr_interf_rank_ecio
(
  srch_interf_cell_struct_type* inter_cell_ptr,
  boolean hcs_used_for_cr_eval, 
  boolean is_csg_ranking,
  int16 hist_add,
  uint16 time_elapsed
);

srchcr_rank_cell_struct_type* srchcr_find_best_ecno_cell
(
  void* curr_best_cell_ptr,
  srchcr_cell_type_enum_type cell_type,
  boolean skip_evals_do_ranking, 
  boolean is_csg_ranking
);
void srchcr_fach_nset_inter_f_cell_reselect_eval( void );
srchcr_rank_cell_struct_type* srchcr_find_best_gsm_cell
(
  srchcr_rank_cell_struct_type* cell_ptr
);
srchcr_rank_cell_struct_type* srchcr_find_best_ranked
(
  /* pointer to the current best cell */
  void* curr_best_cell_ptr
);
boolean srchcr_find_next_best_ranked
(
  /* pointer to the current best cell */
  void* curr_best_cell_ptr
);
boolean srchcr_check_gsm( void );
void srchcr_hcs_det_neigh_srch(void);
void srchcr_prune_uset_cells(void);
void srchcrutils_intraf_nset_cell_reselect_eval( void );
void srchcr_proc_inter_f_srch( void );
void srchcr_nset_irat_cell_reselect_eval( void );
void srchcr_find_next_bsic_id( void );
void srchcr_start_bsic_id_srch( void );
void srchcr_start_n_srch( void );
void srchcr_init_intra_f_nset( void );
void srchcr_init_inter_f_nset( void );
void srchcr_init_timers( void );
boolean srchcr_trigger_cell_resel_ind(void );
void PRINT_BARRED_CELLS(void);
void srchcr_add_cell_to_barredlist (l1_cell_bar_cmd_type *barred_cell);
void srchcr_gsm_cell_resel_ind( srch_gsm_cell_struct_type* new_cell_ptr );
void srchcr_exclude_cells_using_prio( int16 included_priority);
void srchcr_update_prio_state(boolean low_prio, boolean hi_prio, prio_gp_state *prio_state);
void srchcr_qualify_intraf_cell( boolean low_prio, boolean eq_prio, boolean hi_prio);
boolean srchcr_qualify_interf_cell( boolean low_prio, boolean eq_prio, boolean hi_prio);
void srchcr_qualify_gsm_cell(boolean low_prio,boolean hi_prio);
void srchcr_check_and_remove_uset_cells(void);
boolean srchcr_inter_f_check_qualified_async_cells(srchcr_inter_f_idx_enum_type freq_index);
boolean srchcr_inter_f_check_qualified_sync_cells(srchcr_inter_f_idx_enum_type freq_index);
boolean srchcr_inter_f_check_all_sync_cells_below_threshold(srchcr_inter_f_idx_enum_type freq_index);
boolean srchcr_inter_f_list_srch_possible_for_freq(srchcr_inter_f_idx_enum_type freq_index);
extern boolean srchcr_inter_f_full_srch_possible(srchcr_inter_f_idx_enum_type freq_index,boolean serving_cell_reacq_successful);
boolean srchcr_check_inter_f_list_srch_in_gap(uint16 *freq_to_meas);
void srchcr_inter_f_srch_in_gap_send_cmd(void);
void srchcr_update_res_eval_status(void);
void srchcr_set_res_eval_status_on_pich_wakeup(void);
extern void srchcr_add_inter_f_sync_cells_to_eval_list(void);
void srchcr_calculate_time_since_previous_gsm_eval(void);
extern void srchcr_set_flag_to_sort_entire_interf_nset_after_full_srch(boolean sort);
extern void srchcr_intraf_nbr_pn_srch_done_handler(void);
extern void srchcr_interf_nbr_pn_srch_done_handler(void);
extern void srchcr_intraf_full_srch_done_handler
(
  boolean aset_meas_updated,
  boolean aset_reacquired
);
extern void srchcr_interf_full_srch_done_handler(uint8 prev_sync_cnt);
extern uint16 srchcr_add_intraf_timing_known_nbr_cells
(
  uint16 max_pn_tasks,
  srch_cell_struct_type **cell_list_pptr
);
extern uint16 srchcr_add_inter_f_timing_known_nbr_cells
(
  uint16 max_pn_tasks,
  srch_interf_cell_struct_type **interf_cell_pptr
);
extern uint16 srchcr_add_intraf_timing_known_hcs_nbr_cells
(
  uint16 max_pn_tasks,
  srch_cell_struct_type **cell_list_pptr
);
extern uint16 srchcr_add_inter_f_timing_known_hcs_nbr_cells
(
  uint16 max_pn_tasks,
  srch_interf_cell_struct_type **interf_cell_pptr
);

extern void srchzz_bsic_id_aborted (W_ARFCN_T aborted_arfcn,
   srchcm_gsm_bsic_struct_type aborted_bsic);

/*===========================================================================

FUNCTION     SRCHCR_CAN_USET_BE_EXLCUDED

DESCRIPTION
  This function checks if USET cells can be excluded. 
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If we can skip USET cell measurement
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcr_can_uset_be_exlcuded(void);


#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
/*============================================================================
FUNCTION: srchcr_init_prior_info_for_all_rats()

DESCRIPTION:
  This fn initialize the global to indicate abscense of priority for all          
  RATs. This info is received in 3rd MEAS_REQ from RRC. We assume that         
  no priorities are configured in the system until the 3rd MEAS_REQ         
  containing prior info is received.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_init_prior_info_for_all_rats(void);

/*============================================================================
FUNCTION: srchcr_update_priority_and_lte_info()

DESCRIPTION:
  This fn processes the 3rd MEAS_REQ to update the priority information
  for all the RATs.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_update_priority_and_lte_info(
  l1_meas_priority_eutra_cell_list_info_type *info);

/*===========================================================================

FUNCTION       SRCHCR_LTE_PROC_SRCH_DONE

DESCRIPTION
  This function 

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcr_lte_proc_srch_done(wsrchlte_local_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION       SRCHCR_START_LTE_SRCH_ON_INIT_CNF

DESCRIPTION
  This function calls the scheduler when LTE INIT CNF message is rcvd, so 
  that LTE search can now be scheduled

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srchcr_start_lte_srch_on_init_cnf( void );

#endif /* FEATURE_WCDMA_ABSOLUTE_PRIORITY */

#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
/*============================================================================
FUNCTION: SRCHCR_DO_LTE_SEARCH

DESCRIPTION:
  This function checks if LTE INIT message has been sent or not. If not sent, 
  then it sends the INIT message, in this case, search will be scheduled when
  LTE INIT cnf is received. If LTE INIT message has already been sent, it calls
  the wsrchlte function to schedule the search
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - If either LTE INIT gets sent or a search gets started.
  FALSE - If search start fails

SIDE EFFECTS:
=============================================================================*/
extern boolean srchcr_do_lte_search(wsrchlte_srch_enum_type srch_type, wsrchlte_frequency_info_type *earfcn_info) ;


/*===========================================================================
FUNCTION      SRCHCR_LTE_SEARCH_SENT_TO_DRIVER

DESCRIPTION
  This function checks that if the IRAT state machine indicates 
  that measurement are in progress. If this functions return TRUE 
  and srchzz_substate is SRCHZZ_LTE, it indicates that search has 
  been sent to driver 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If search has been sent to driver 
  FALSE - Otherwise 

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcr_lte_search_sent_to_driver(void);

#endif


/*===========================================================================
FUNCTION      SRCHCR_IDLE_MODE_W2X_CFG_CNF_CB

DESCRIPTION
  This function should be called in interrupt context when a w2x_idle_cfg_cmd 
  is acknowledged by WFW.  It will update the search state variable and send a
  local message to wl1 indicating it should pickup the search process where it 
  left off.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_idle_mode_w2x_cfg_cnf_cb(uint16 status);

/*===========================================================================
FUNCTION      SRCHCR_IRAT_MEAS_STATE_MC

DESCRIPTION
  This function is the IRAT state machine

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_irat_meas_state_mc(void);

/*===========================================================================
FUNCTION      SRCHCR_RESET_IRAT_MEAS_STATE_MC

DESCRIPTION
  This function re-initializes the IRAT state machine

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_reset_irat_meas_state_mc(void);

/*===========================================================================
FUNCTION      SRCHCR_SEND_START_AGC_CMD_TO_FW

DESCRIPTION
  This function sends a start AGC cmd to FW

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_send_start_agc_cmd_to_fw(void);

/*============================================================================
FUNCTION: SRCHCR_L1_SRCHCR_RESUME_IDLE_SEARCH_HANDLER

DESCRIPTION:
  TODO
      
DEPENDENCIES:
  TODO

RETURN VALUE:
  TODO

SIDE EFFECTS:
=============================================================================*/
void srchcr_l1_srchcr_resume_idle_search_handler(void);

/*===========================================================================
FUNCTION      SRCHCR_SEND_STOP_AGC_CMD_TO_FW

DESCRIPTION
  This function sends the idle mode w2x command to FW and then calls the state 
  machine function so a transition to the next state can occur.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_send_w2x_cmd_to_fw(void);

/*============================================================================
FUNCTION: SRCHCR_UPDATE_FULL_SRCH_COND_VAL

DESCRIPTION:
  This function updates the values needed for performing additional full search
  conditions
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_update_full_srch_cond_val(srchcr_freq_idx_enum_type freq_idx);

/*============================================================================
FUNCTION: SRCHCR_CHECK_ADDTNL_COND_FOR_FULL_SRCH

DESCRIPTION:
  If the full search on any frequency is satisfied, a set of calculations are
  done, to figure out if the full search results in any cells with good energy.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - If full search would be useful.
  FALSE - If full search not needed.

SIDE EFFECTS:
=============================================================================*/
extern boolean srchcr_check_addtnl_cond_for_full_srch(srchcr_freq_idx_enum_type freq_idx);

/*============================================================================
FUNCTION: SRCHCR_UPDATE_STEP1_THRESHOLDS

DESCRIPTION:
  This function updates the step1 search thresholds based on the state
  of the UE(Stationary/Mobile)
      
DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_update_step1_thresholds(srchcr_freq_idx_enum_type freq_idx);

/*============================================================================
FUNCTION: SRCHCR_UPD_TMR_BASED_ON_CELL_ENG

DESCRIPTION:
  If the energy of the cell is less than threshold value(-25), a 15s timer is
  decremented and when the timer expires the cell is removed from the monitored
  set.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_upd_tmr_based_on_cell_eng(srchcr_freq_idx_enum_type freq_idx);

/*============================================================================
FUNCTION: SRCHCR_FIND_MIN_OF_PARAMS

DESCRIPTION:
  This function finds the minimum of the cell parameters(q_offset1, q_offset_2
  q_rxlevmin and q_qualmin) in a frequency.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_find_min_of_params(srchcr_freq_idx_enum_type freq_idx);

/*===========================================================================

FUNCTION       SRCHCR_RESET_RFTUNE_AND_AGC_STATE

DESCRIPTION
  Updates agc status post w2x

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcr_reset_rftune_and_agc_state(boolean skip_if_interf_resel);



#ifdef FEATURE_WCDMA_HS_RACH

/*===========================================================================
FUNCTION     SRCHCR_UNBLOCK_CELL_RESEL

DESCRIPTION 
  This funciton is called by HSRACH module to allow reselections.
 
  Once the EDCH resource is released, the HS RACH controller will call a cell reselection API to
  unblock the cell reselections. The design is to do a immediate resel evals and restart the 80ms eval timer.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_unblock_cell_resel(void);
#endif /* FEATURE_WCDMA_HS_RACH */

#ifdef FEATURE_WCDMA_HS_FACH
/*===========================================================================

FUNCTION        srchfach_reset_tresel_ngbr_cells

DESCRIPTION     This function resets Tresel for IF and GSM based on the cell_type passed.   
                This function will be called if FMO for that RAT is skipped for N_FMOSkip times.

DEPENDENCIES    None

RETURN VALUE    None
  
SIDE EFFECTS    Tresel for all IF / GSM ngbr cells will be reset to 0.
                
===========================================================================*/
void srchcr_reset_tresel_ngbr_cells(srchcr_cell_type_enum_type cell_type);
#endif /* FEATURE_WCDMA_HS_FACH */

/*===========================================================================
FUNCTION     SRCHCR_RESP_EARLY_RF_SLEEP_DECISION

DESCRIPTION
  This function returns the early RF sleep decision already evaluated.

DEPENDENCIES
  None.

RETURN VALUE
  True  - No Pending or ongng searches
  False - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchcr_resp_early_rf_sleep_decision(void);

/*===========================================================================
FUNCTION     SRCHCR_CLR_EARLY_RF_SLEEP_DECISION

DESCRIPTION
  This function clears the early RF sleep decision already evaluated.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_clr_early_rf_sleep_decision(void);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION     srchcr_decfg_cell_for_interf_resel_cb

DESCRIPTION
  This is a CB function which triggers a serving cell configure/deconfigure soon 
  after configuring/deconfiguring of the neighbor cell

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_decfg_cell_for_interf_resel_cb(void);
#endif

/*===========================================================================

FUNCTION     srchcr_enable_heartbeat_aft_decfg_done

DESCRIPTION
  This is a CB function which enabled heartbeat if needed soon after  
  ngbr cell is deconfigured as part of cell resel failure

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_enable_heartbeat_aft_decfg_done(void);

#ifdef FEATURE_FEMTO_CSG
/*============================================================================
FUNCTION: srchcr_update_csg_info()

DESCRIPTION:
  This fn processes the 4th CSG MEAS_REQ and updates the internal CSG database.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_update_csg_info(
  l1_meas_csg_cells_info_type *csg_meas_ptr);

/*============================================================================
FUNCTION: srchcr_perform_csg_eval_procedures()

DESCRIPTION:
  This function 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcr_perform_csg_eval_procedures (srchcr_res_eval_meas_type res_eval_meas_comp);

/*============================================================================
FUNCTION: SRCHSET_GET_INTRA_CSG_CELL_PTR()

DESCRIPTION:
  This function returns the CSG ptr corresponding to the freq,psc thats passed
      
DEPENDENCIES:
  None.

RETURN VALUE:
  Retuns the csg_id mapping to freq, psc that is passed.

SIDE EFFECTS:
=============================================================================*/
extern srch_cell_struct_type* srchset_get_intra_csg_cell_ptr(uint16 freq, uint16 psc);

/*============================================================================
FUNCTION: SRCHSET_GET_INTERF_CSG_CELL_PTR()

DESCRIPTION:
  This function returns the inter-f CSG ptr corresponding to the freq,psc thats passed
      
DEPENDENCIES:
  None.

RETURN VALUE:
  Retuns the csg_id mapping to freq, psc that is passed.

SIDE EFFECTS:
=============================================================================*/
extern srch_interf_cell_struct_type* srchset_get_interf_csg_cell_ptr(uint16 freq, uint16 psc);

#endif

#ifdef FEATURE_DUAL_SIM
/*============================================================================
FUNCTION: SRCHCR_GSM_SRCH_IN_THIS_DRX

DESCRIPTION: This function is used to check if GSM searh is performed in this
DRX cycle(using srchcr_gsm_cell_res_eval_sfn)

      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - If GSM search in this DRX
  FASLE - Otherwise

SIDE EFFECTS:
=============================================================================*/
extern boolean srchcr_gsm_srch_in_this_drx(void);
#endif /*FEATURE_DUAL_SIM */

#ifdef FEATURE_WCDMA_ENH_OOS_RXD_PWC
/*===========================================================================

FUNCTION     SRCHCR_CHECK_OOS_CRIT_USING_SUM_PRI_DIV

DESCRIPTION
 Computes total energy based on SUM(PRIM,DIV) energies and 
 calculates Srxlev and Squal for OOS determination

DEPENDENCIES
  None.

RETURN VALUE
  True  - If serving cell (Squal >0 and Srxlev > 0)
  False - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
void  srchcr_check_oos_crit_using_sum_pri_div(void);
#endif
/*===========================================================================

FUNCTION SRCHCR_SORT_NSET_CELLS

DESCRIPTION
  This functions sorts the peaks from the full search/PN search results

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchcr_sort_nset_cells(void);

/*===========================================================================

FUNCTION       SRCHCR_CALC_SERVING_CELL_RANK

DESCRIPTION
  This function calculates the RSCP or EcIo rank for the serving cell 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  srch_aset_ptr[0] is updated
  
===========================================================================*/
void srchcr_calc_serving_cell_rank(void);

/*===========================================================================

FUNCTION       SRCHCRUTILS_UPDATE_FAST_MOVING_CONDITION

DESCRIPTION
  This function checks if UE is in fast mvoing and can exit fast moving

DEPENDENCIES
  

RETURN VALUE
  None.

SIDE EFFECTS
  UE fast moving condition updated
  
===========================================================================*/
void srchcrutils_update_fast_moving_condition(void);

#ifdef FEATURE_IGNORE_TRESEL_ON_S_FAIL
/*===========================================================================

FUNCTION       SRCHCR_UPDATE_WEAK_RESEL_CONDITION

DESCRIPTION
  This function checks if weak resel condtion's end time has been reached. If so update reset the weak resel flag.
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  
===========================================================================*/
void srchcr_update_weak_resel_condition(void);
#endif

/*===========================================================================

FUNCTION       SRCHCRUTILS_PREPARE_SRCHED_CELLS

DESCRIPTION
  This function clears WCDMA cells in FACH state and all cells otherwise from
  the searched cells list and sets the srched cellcounter accordingly.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchcrutils_prepare_srched_cells( boolean force_clear );

/*===========================================================================
FUNCTION      SRCHCRUTILS_CALCULATE_TIME_SINCE_PREV_EVAL

DESCRIPTION
  This function is called to calculate the time since previous resel eval

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcrutils_calculate_time_since_prev_intraf_eval(void);

/*===========================================================================
FUNCTION      SRCHCRUTILS_CALCULATE_TIME_SINCE_PREV_INTERF_EVAL

DESCRIPTION
  This function is called to calculate the time since previous InterF resel eval

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchcrutils_calculate_time_since_prev_interf_eval(srchcr_inter_f_idx_enum_type freq_idx);

/*===========================================================================

FUNCTION SRCHCRUTILS_ADD_SERV_CELL_TO_RANK_LOG_PKT

DESCRIPTION
  This function will add the Serving Cell to the Ranking log packet (Serving cell is always placed first)

DEPENDENCIES
  None.

RETURN VALUE
NONE

SIDE EFFECTS
  None.

===========================================================================*/
void srchcrutils_add_serv_cell_to_rank_log_pkt(void);

/*===========================================================================

FUNCTION      SRCHCRMEAS_PROCESS_NASTT_UNMON_PEAKS

DESCRIPTION
 This function is called after NASTT is completed. It processes the NASTT peaks and updates
 cell energies if they are already known. Else, we will try to allocate a cell in USET and save
 the newly found cell
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcrmeas_process_nastt_unmon_peaks( boolean *aset_meas_updated, boolean *aset_reacquired);


#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY

/*===========================================================================

FUNCTION      SRCHCRUTILS_GSM_HIGH_PRIO_CELL_FOUND

DESCRIPTION
  Find if GSM HighPrio cells were found in the last HighPrio search

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void  srchcrutils_gsm_high_prio_cell_found(void);

#endif

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
/*============================================================================
FUNCTION: SRCHCR_MARK_ABS_PRIO_FACH_RESEL_CAND

DESCRIPTION:
  This function loops through the srched_cells array and marks reselection Inter/IRAT candidates based on
  absolute priority rules
      
DEPENDENCIES:
  None.

RETURN VALUE:
NONE

SIDE EFFECTS:
NONE
=============================================================================*/
void srchcr_mark_abs_prio_fach_resel_cand(srchcr_cell_type_enum_type cell_type, 
    srchcr_priority_enum_type priority_type);

/*============================================================================
FUNCTION: SRCHCRUTILS_MARK_ALLOWED_FREQ

DESCRIPTION:
  This function is called when scheduling mode changes in FACH. It updates the srch_allowed flag in
  driver search list for the next FMO RAT. Driver uses this information in scheduling the freq for a search
  in the following FMO gap.
      
DEPENDENCIES:
  None.

RETURN VALUE:
NONE

SIDE EFFECTS:
NONE
=============================================================================*/
void srchcrutils_mark_allowed_freq(srchcr_cell_type_enum_type cell_type);
#endif

/*============================================================================
FUNCTION: SRCHCRUTILS_CLEANUP_NBR_DB

DESCRIPTION: This function is called to clear all Interf/Intraf cells counts that srchcr maintains locally.
This is useful when srchset is transitioning cells to SET_E or during ASET UPD where we clear all InterF
cells as part of resel success
      
DEPENDENCIES:
None.

RETURN VALUE:
NONE

SIDE EFFECTS:
=============================================================================*/
extern void srchcrutils_cleanup_nbr_db(srchcr_cell_type_enum_type cell_type, boolean clear_srched_cells);

/*===========================================================================

FUNCTION SRCHCRUTILS_RESET_TRESEL_FOR_INTRAF_CELLS

DESCRIPTION
  This functions resets the Treselection counter and enabled values

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchcrutils_reset_tresel_for_intraf_cells(uint8 carr_idx, srch_set_enum_type set);

/*===========================================================================

FUNCTION SRCHCRUTILS_RESET_TRESEL_FOR_INTERF_CELLS

DESCRIPTION
  This functions resets the Treselection counter and enabled values for INTERF cells

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchcrutils_reset_tresel_for_interf_cells(srch_interf_set_enum_type set);

/*===========================================================================

FUNCTION SRCHCRUTILS_RESET_TRESEL_FOR_GSM_CELLS

DESCRIPTION
  This functions resets the Treselection counter and enabled values for GSM cells

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchcrutils_reset_tresel_for_gsm_cells(srch_gsm_set_enum_type set);

#ifdef FEATURE_REL5  /* For REL5 CR 136 rev1*/
/*===========================================================================

FUNCTION       SRCHCR_SCALE_TRESEL

DESCRIPTION
   This funciton scales up/down tresel values for intra, inter and inter-rat cells depending on
   whether UE is in high mobility or not and based on the different scale factors.
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
extern void  srchcr_scale_tresel(void);
#endif

/*============================================================================
FUNCTION: SRCHCRUTILS_POST_RESEL_EVENT

DESCRIPTION: This function is used to post an event containing cell 
             information when a reselection procedure starts. 
      
DEPENDENCIES:
None.

RETURN VALUE:
None.

SIDE EFFECTS:
=============================================================================*/
void srchcrutils_post_resel_start_event(uint16 uarfcn, uint16 target_psc);

/*============================================================================
FUNCTION: SRCHCRUTILS_POST_OOS_IND_TO_RRC

DESCRIPTION: This function is used to post RRC_CPHY_OUT_OF_SERVICE_AREA_IND to RRC
      
DEPENDENCIES:
None.

RETURN VALUE:
None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcrutils_post_oos_ind_to_rrc(wl1_common_fail_enum_type oos_cause);
/*============================================================================
FUNCTION: SRCHCRUTILS_INTRAF_NGBR_FAIL_ECIO_CHECK

DESCRIPTION: This api checks if all the sync set neighbor's EcIo is below -16 dB or not
      
DEPENDENCIES:
None.

RETURN VALUE:
YES: All the intraf sync neighbor's EcIo is below -16dB
NO: Atleast one of the neighbor has EcIo better than -16dB

SIDE EFFECTS:
None
=============================================================================*/
extern boolean srchcrutils_intraf_ngbr_fail_ecio_check();


#ifdef FEATURE_WCDMA_COEXISTENCE_SW
/*============================================================================
FUNCTION: SRCHCRUTILS_REPORT_INTERF_NBR_TO_CXM

DESCRIPTION: This function de/registers the IDLE mode InterF neighbors with 
             WL1CXM and thereby with MCS. It is called upon 2nd MEAS_REQ
             processing only in DRX state. In other states, CM module
             handles it.
      
DEPENDENCIES:
None.

RETURN VALUE:
None.

SIDE EFFECTS:
=============================================================================*/
extern void srchcrutils_report_interf_nbr_to_cxm(boolean dereg);
#endif

#ifdef FEATURE_QSH_DUMP
/*===========================================================================

FUNCTION WL1_POPULATE_DEBUG_SRCHCR_INFO_STRUCT_TYPE

DESCRIPTION
   This function dumps state info into a global strcuture as part of a crash

DEPENDENCIES
  None.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
void wl1_populate_debug_srchcr_info_struct_type(wl1_cell_resel_mini_debug_info_struct_type * srchcr_cell_resel_dump_ptr);

/*===========================================================================

FUNCTION WL1_CELL_RESEL_DUMP_CB

DESCRIPTION
   This function passes the dump pointer address to populate the dump info

DEPENDENCIES
  None.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
void wl1_cell_resel_dump_cb(void* addr_ptr);
#endif /* FEATURE_QSH_DUMP */

#endif /* SRCHCR_H */
