#ifndef SRCH_ZZ_H
#define SRCH_ZZ_H
/*===========================================================================

            S R C H  Z Z (sleep)   R E L A T E D   D E C L A R A T I O N S

DESCRIPTION This file contains the declarations for the srch module 
related to sleep and wakeup

EXTERNALIZED FUNCTIONS

srchzz_prepare_for_sleep
  This function is called to indicate to srch that a sleep cycle is immanent. 
  This function initiates aborting of all searching in progress, and saves
  the PN positions of the fingers via a status dump. When done, it posts a 
  DRX_SLEEP_CMD local command.

srchzz_prepare_for_wakeup
  This function is called upon wakeup, after RF blocks have warmed up.
  This function initiates reacquisition searching and triage upon successful
  reacq. Finally, it posts a DRX_SLEEP_CMD local command with the status of
  reacquisition.
  

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

REFERENCES

Copyright (c) 2001-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchzz.h_v   1.2   04 Jun 2002 13:29:18   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchzz.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/18/16   am      Fail PICH wakeup if IRAT searches are going on.
06/20/16   nga     FR 30575: W2L measurement support in G2L TA 
02/24/15   ap      Initial check-in for WL1 QSH framework 
05/18/16   mk      W2X command processing needs to 1 frame as seq can reduce the mode duration by 1 frame 
08/05/15   db      Check if CSG related searches are pending before allowing early RF shutdown
05/11/15   rs      Removing references to fast cell deconfig API's 
01/08/15   skk     Fixing Compilation errors.
11/24/14   gsk     SRCHZZ wakeup cleanup changes
11/13/14   psr     Include detected set cells found during a panic full search
11/12/14   skk     Compensate FWCPICH eng in ||el reacq based on diff b/w srchr pos and strongest fing.
10/17/14   psr     FR 19219: Panic Search Optimizations and mainlining FEATURE_WCDMA_ENH_PN_SCHEDULING
06/20/14   skk     FR19218: ReacqLessPICH(RLP) porting for JO.
08/04/14   rs      Fixed compiler warnings
07/09/14   gsk     Register the timeout only for PCH/PCH_SLEEP 
07/07/14   ar      Define the default wakeup cell index used in srchzz
07/03/14   sks     Changes for FCDC
07/01/14   hk      Header file inclusion cleanup
06/19/14   gsk     Idle DR-DSDS changes
06/17/14   ar      Do not register for timeout handlers in BCH state
06/17/14   gsk     Register a timeout handler for idle mode searches
05/30/14   ar      Remove srchzz_is_reacq_srch. This is replaced by srch_is_wkup_srch API
04/28/14   ar      Move srchzz_is_tresel_running_for_any_lte_cells to srchcrutils
04/15/14   ar      Remove deprecated prototype for srchzz_nastt_unmon_processing_peaks
01/30/14   ar      Add support for distinguishing IRAT READY state between GSM and LTE
01/20/14   gsk     Avoid issuing inter frequency searches when freq/cell is barred
01/24/14   ar      Added support for forcing LTE detect searches after a W2L Resel failure
01/23/14   gsk     Changing DL-D EFACH mode to DL-D NON DRX mode
07/01/14   amj     Clean up old ASET update code
12/27/13   rs      Function declaratio to rerun the CTCH algo when a new BMC CPHY request is received.
12/19/13   rs      Prioritize searches unconditionally when S criteria or reacq fails
12/16/13   as      Porting G2W TA from Triton to Dime.
12/16/13   bj      split ACQ feature Porting from Triton to Dime.
11/21/13   jd      FET 2.0 Searcher two queue approach.
10/28/13   ar      Removed code under FEATURE_DL_IGNORE_MODE_FACH_MEAS
09/23/13   as      Porting Dual Sim from Triton to Dime.
09/12/13   jd      Searcher redesign
07/15/13   gsk     Externing a global which stores the test NV value
07/15/13   gsk     FR 3672: CTCH enhancement changes
06/13/13   gsk     While issuing the search, checking the cell cnt from the super set(ofreq_sync) 
                   and the susbset(hcs_sync set) structures
04/11/13   gsk     Idle Mode FR 2741/2743, reducing WL1 wait time between rf_tune_to_chan
                   and search cmd to FW. Also adding support for interf offline PN
02/21/13   hk      Removed unused functions
02/20/13   hk      Removed FEATURE_INTER_SIB_SLEEP
02/11/13   gsk/sr  EDRX Full Sleep initial check in.
02/05/13   pv      New API for FR 2001.
01/28/13   ar      Adding prototype for srchzz cleanup
01/21/13   stk     Support for Femto/CSG selection and reselection
11/28/12   gsk     Fix for compiler warnings
09/27/12   pv      API for resetting the cell db idx for fast cell/fing decfg.
09/26/12   pv      Externed the variable that stores cell db idx for fast cell/fing decfg.
09/19/12   gsk     Externing function    
09/17/12   pv      API for checking cell/fing decfg in progress/complete.
08/30/12   pv      Removed an extern that is not needed. 
08/30/12   pv      Added externs.
08/29/12   pv      New extens for variables/APIs for PICH timeline opts.
08/17/12   gsk     Setting LTE detect srch timer max value to 60s during ALL to HIGH priority mode transition.
08/14/12   gsk     Declaring timer expiry for high_prior_rssi timer only if the prior info is present.
08/01/12   gsk     removing gloabl using_searcher_ref_position 
07/20/12   gsk     FR 2257 Adaptive Reacq Search Length
07/17/12   pv      Removed stray characters.
07/17/12   pv      Warning fixes.
07/13/11   pv      Offline PICH/Parallel Reacq fixes.
08/27/12   gsk     Setting LTE detect srch timer max value to 60s during ALL to HIGH priority mode transition.
08/21/12   gsk     Declaring timer expiry for high_prior_rssi timer only if the prior info is present.
08/13/12   gsk     FR 2257 Adaptive Reacq Search Length
08/13/12   gsk     removing gloabl using_searcher_ref_position 
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12   gsk     Idle mode FRs 1782/1705/1947/2152/1950
05/15/12   gsk     Support for feature Inter-freq List search frequency with two 
                   additional frequencies in idle 
06/07/12   vs      MBMS feature cleanup.
05/03/12   gsk     Externing function srchzz_is_tresel_running_for_any_lte_cells
04/06/12   hk      Fixed compilation warnings
03/27/12   hk      Added support for FEATURE_WCDMA_HS_FACH_DRX
02/16/12   uk      Added fn that checks if any pending searches are due
12/05/11   ag      Compiler warning fixes
12/02/11   stk     Added support for FEATURE_WCDMA_RXD_PCH and FEATURE_IDLEMODE_FILTER
11/12/11   pv      Removal of early RF sleep response API.
11/09/11   zr      Added ability to retrieve cumulative reacq slew
09/26/11   uk      Added back flag srchzz_just_sent_oos
09/22/11   uk      Get rid of CTCH callback fn no longer used
09/19/11   uk      Macro for Chking duration before w2x command queuing
09/15/11   scm     Remove srchzz_just_sent_oos.  NikeL has no FEE, so not used.
05/16/11   stk     Renamed function names and added more comments.
05/16/11   stk     Added support for doing only LTE CELL_DETECT srch's and not CELL_MEASUREMENT srch upon W2G BPLMN completion.
03/04/11   uk      Added function headers 
07/14/10   mm      Fix for compiler warnings 
06/21/10   uk      Compiler warning fix
06/02/10   stk     Added support for W2LTE measurements and reselection under 
                   FEATURE_WCDMA_ABSOLUTE_PRIORITY & FEATURE_WCDMA_TO_LTE
09/04/09   saik    Use new srch api to determine if reacq srch is in progress.
06/03/09   stk     Fixed compiler warning - moved srchzz_config_cell_prior_triage() to srchzz.c
05/27/09   stk     Fixed Lint warnings
05/08/09   sup      Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                    srchlog.h to wdec.h, wenci.h, wl1m.h, wsrch.h, wsrchi.h, wsrchlog.h
04/22/09   stk     Compiler warning fixes	
03/13/09   rmak    Added srchzz_resp_early_rf_sleep to support Early RF Sleep functionality
12/09/08   uk      Mainling FEATURE_WCDMA_DL_ENHANCED, FEATURE_ENHANCED_CELL_RESELECTION, 
                   FEATURE_WCDMA_DRX_MANAGER, FEATURE_WCDMA_OFFLINE_SEARCHER 
                   and FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT 
10/03/08   hk      Bringing in 7k Mailine fixes
09/14/08   uk      Adding srchzz_substate_type and srch_pending bmask for srch
                   done when aset srch is done on aset_reacquisition after reacq fail
06/10/08   ms      Added PACKED_POST attribute for structures to make it
                   compatible for GNU compiler.
12/03/07   sk      Declaration for function to start full search on PL info req
11/05/07   ub      Fixed new cell reselecion feature merge
11/01/07   ub      MBMS: DRX mode filtering when MCCH/MTCH enabled protitypes
10/26/07   sk      Checking in MBMS branch code into mainline with MBMS feature disabled
10/04/07   sk      Code cleanup
08/20/07   kps     Change reacq srch counter variable names
07/17/07   sk      Fix a compilation error due to upmerge to 4111
05/23/07  ub/sk    Intergrate GSM meas and code cleanup
04/28/07   scm     Change DRX_ENTITY_BCCH_S(N) to DRX_ENTITY_BCCH_INT(EXT).
04/25/07  ub/sk    Inter freq resel with MBMS changes
04/01/07  ub/sk    FEATURE_ENHANCED_CELL_RESELECTION changes
12/12/06   mc      Synced up for MBMS Phase2A integration
03/03/07   scm     Change logical entity bitmask size from uint16 to uint32.
02/22/07   scm     Temporary stubs for new SRCH API.
02/20/07   scm     Data definitions to interface with new DRX Manager.
03/07/07    nd      Code changes for PSC scanner srch under FEATURE_WCDMA_L1_PSC_SCANNER
02/07/06   scm     New srchzz_substate: SRCHZZ_NSET_FOR_SIBS for waking from
                   sleep to acquire neighbor and read SIBs.
01/09/06   scm     Mods to support FEATURE_INTER_SIB_SLEEP.
12/22/05   gv  	   Modified the SRCHZZ_ASET_HB substate to SRCHZZ_ASET_NSET_HB.
				   Also removed the substates SRCHZZ_NSET_LIST and 
				   SRCHZZ_OF_NSET_LIST as these list searches are now performed
				   as a part of the substate SRCHZZ_ASET_NSET_HB (20ms).
12/09/04   sh      Added SRCHZZ_ASET_HB substate for aset search using search
                   heartbeat event.
10/28/04   sh      Added srchzz_gsm_clk_ena_req to indicate if srch needs to
                   wait until GSM clk and TX System Time is turned on after
                   a SIB mod update.
06/05/04   asr     Extern declarations of srchzz_reset_reacq_list_passed_good_dl() 
                   and srchzz_get_reacq_list_passed_good_dl().
05/11/04   scm     Extern reacq_slew and num_failed_reacquisitions.
04/28/04   sh      Added substate for inter-frequency list searching.
04/12/04   sh      Removed srchzz substate SRCHZZ_WOKEN_UP.
04/05/04   sh      Added two new states to srchzz.
01/16/04   asr     Added extern declaration of using_searcher_ref_position
11/12/03   sh      Added substates for inter-frequency searching in Idle.
07/28/03   scm     Support for query of various DRX reacquisition statistics.
05/27/03   asr     Created extern declaration of srchzz_reacq_list_search_in_progress.
04/31/03   asr     Created extern declaration of srchzz_just_sent_oos.
04/17/02   sh      Added function proto srchzz_save_and_disable_fing_info().
02/12/03   sh      Added function proto srchzz_send_srch_ready_for_sleep().
06/04/02   sh      Added function srchzz_declare_reacq_status_drx().
05/18/02   sh      Added zz substates for step 1/2/3 and list searches
                   during cell reselection evaluation and other support
                   variables.
01/31/02   asr     created file
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "srchcmd.h"
#include "wsrch.h"
#include "srchinterf.h"
#include "wl1srchdlif.h"
#include "wcdmadiag.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define SRCH_ASET_MAX_IN_IDLE 1
#define SRCHZZ_MAX_NUM_CELLS (SRCH_ASET_MAX_IN_IDLE + \
                              SRCH_COMP_NBCH_SET_MAX_C0)

#define SRCHZZ_WAKEUP_CELL_IDX 0

/* Measurement status on what measurements are pending and what are not*/
#define ASET_SRCH_PENDING 0x1

/* Minimum value of CPICH Ec/Io that until which CTCH will be 
 * prioritized over searches
 */
#define SRCHZZ_CTCH_OVER_MEAS_ASET_ECIO_THRESH 24


/* global flag to disable / enable IDLE MODE FILTER related F3 for debug purpose */
/* FALSE - Disable (default), TRUE - Enable */
extern boolean srchzz_imf_f3_debug;

/* Bitmask for Second reacq srch when idle mode filter is defined */
#define SECOND_ASET_SRCH_PENDING 0x10

#define ASET_REACQ_FAIL_SRCH_PENDING 0x20

#define INTRAF_LIST_SEARCH_PENDING 0x2
#define INTRAF_STEP1_SEARCH_PENDING 0x4

#define INTRAF_STEP_NASTT_SEARCH_PENDING 0x8
#define INTRAF_SEARCH_PENDING  \
     ( INTRAF_LIST_SEARCH_PENDING | INTRAF_STEP1_SEARCH_PENDING | \
     INTRAF_STEP_NASTT_SEARCH_PENDING)

#define INTERF_LIST_SEARCH_PENDING 0x2
#define INTERF_STEP1_SEARCH_PENDING 0x4

#define INTERF_STEP_NASTT_SEARCH_PENDING 0x8

#define LTE_SEARCH_PENDING 0x1

#define INTERF_SEARCH_PENDING  \
   ( INTERF_LIST_SEARCH_PENDING | INTERF_STEP1_SEARCH_PENDING | \
   INTERF_STEP_NASTT_SEARCH_PENDING )

#define IRAT_RSSI_PENDING 0x2
#define IRAT_BSIC_PENDING 0x4
#define IRAT_SEARCH_PENDING  (IRAT_RSSI_PENDING | IRAT_BSIC_PENDING)

#define ALL_SEARCH_PENDING  (INTRAF_SEARCH_PENDING | INTERF_SEARCH_PENDING | IRAT_SEARCH_PENDING) 

#define CHECK_LTE_SEARCH_PENDING() \
                ((meas_status.lte_meas_status & LTE_SEARCH_PENDING) != 0)

#define SRCHZZ_IRAT_SRCH_ONGOING() (srchcr_pending_cell_resel_bsic_verif || (srchzz_substate == SRCHZZ_IRAT) || (srchzz_substate == SRCHZZ_LTE))

/*Avoid registering for BCH state since it is handled through scheduler
and the wakeup search (posted through local cmd) can get abort in scheduler 
due to an external command like IDLE_REQ etc. This will cause us to drop the 
search but the timer will be left dangling*/
#define SRCHZZ_SET_SRCH_SUBSTATE(srch_state) \
do { \
  srchzz_substate = (srchzz_substate_enum_type)srch_state; \
  if(search_state == SRCH_DRX_STATE)\
  {\
    wl1m_timeout_hldr_register_client(WL1_TIMEOUT_HDLR_SRCH_CLIENT, 10, FALSE, NULL); \
  }\
}while (0)


#define SRCHZZ_RESET_SRCH_SUBSTATE() \
do{ \
  srchzz_substate = SRCHZZ_INACTIVE; \
  if(search_state != SRCH_BCH_STATE)\
  {\
    wl1m_timeout_hldr_deregister_client(WL1_TIMEOUT_HDLR_SRCH_CLIENT); \
  }\
}while(0)

  /* Timers */  
#define SRCHZZ_ASET_LIST_SRCH_TIMER 0x1
#define SRCHZZ_INTRA_LIST_SRCH_TIMER 0x2
#define SRCHZZ_INTRA_FULL_SRCH_TIMER 0x4
#define SRCHZZ_INTER_F0_LIST_SRCH_TIMER 0x8
#define SRCHZZ_INTER_F1_LIST_SRCH_TIMER 0x10
#define SRCHZZ_INTER_F0_FULL_SRCH_TIMER 0x20
#define SRCHZZ_INTER_F1_FULL_SRCH_TIMER 0x40
#define SRCHZZ_IRAT_RSSI_TIMER 0x80
#define SRCHZZ_IRAT_BSIC_TIMER 0x100
#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
#define SRCHZZ_LTE_CELL_DETECT_SRCH_TIMER 0x200
#define SRCHZZ_LTE_CELL_MEAS_SRCH_TIMER 0x400
#endif
#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
#define SRCHZZ_IRAT_HIGH_PRIO_RSSI_TIMER 0x800
#endif
#ifdef FEATURE_WCDMA_PANIC_SRCH_OPT
#define SRCHZZ_PANIC_INTRAF_FULL_SRCH_TIMER 0x1000
#define SRCHZZ_PANIC_INTER_F0_FULL_SRCH_TIMER 0x2000
#define SRCHZZ_PANIC_INTER_F1_FULL_SRCH_TIMER 0x4000
#endif
#ifndef SRCH_OFREQ_FREQ_MAX
#define SRCH_OFREQ_FREQ_MAX 2
#endif

#define SRCHZZ_REACQ_SLEW_SMALL_THRESHOLD   20
#define SRCHZZ_REACQ_SLEW_MED_THRESHOLD    256
#define SRCHZZ_REACQ_SLEW_LARGE_THRESHOLD  768

/* Check if inter freq (f0) list srch timer has expired*/
#define CHECK_INTERF0_LIST_DUE( ) \
           (meas_status.timer_expiry_status & SRCHZZ_INTER_F0_LIST_SRCH_TIMER)
/* Check if inter freq (f1) list srch timer has expired*/
#define CHECK_INTERF1_LIST_DUE( ) \
          (meas_status.timer_expiry_status & SRCHZZ_INTER_F1_LIST_SRCH_TIMER)

#define SRCHZZ_INTER_F_ASYNC_CELLS_PRESENT_PER_FREQ(freq_index)  (srch_ofreq_async_nset[freq_index].cell_cnt > 0)
#define SRCHZZ_INTER_F_SYNC_CELLS_PRESENT_PER_FREQ(freq_index)  (srch_ofreq_sync_nset[freq_index].cell_cnt > 0)

#define SRCHZZ_HCS_INTER_F_SYNC_CELLS_PRESENT_PER_FREQ(freq_index) \
          ((srch_aset_parms.hcs_used) && \
           (SRCHZZ_INTER_F_SYNC_CELLS_PRESENT_PER_FREQ(freq_index)) && (hcs_inter_sync_nset_cnt[freq_index] > 0))

/* The MACRO will be called at G2W TA exit to clear out the pending BSIC verification
   as there might not be a new gap opened since we are exiting G2W TA */
#define SRCHZZ_CLEAR_PEND_BSIC_VERIFY_G2W_TA_EXIT() \
{ \
  srchcr_pending_cell_resel_bsic_verif = FALSE;\
  srchcr_gsm_cell_ptr = NULL;\
}
 
#define SRCHZZ_RELEASE_NGBR_MEAS_TRM_ID(idx) \
do\
{\
  wrm_complete_measurement(srchzz_ngbr_trm_info[idx].trm_id); \
  srchzz_ngbr_trm_info[idx].srch_type = SRCHCR_RES_EVAL_MEAS_ALL; \
}while(0)
   
#ifdef FEATURE_FEMTO_CSG
#define IS_CSG_EVALS_PENDING() ((!(srch_aset_parms.is_camped_cell_femto)) && (!(srchcr_csg_info.is_csg_meas_req_processed)) && ((srchcr_csg_info.num_tot_asf_cells) != 0))
#define IS_CSG_SRCH_PENDING() ((IS_CSG_EVALS_PENDING()) && ((srchcr_csg_info.csg_meas_pending) != 0))
#endif

/*--------------------------------------------------------------------------
                          ZZ SEARCH SUBSTATES

--------------------------------------------------------------------------*/
/*SS mangr enums for freeze flag and use flag*/
typedef enum{
  FF_NO_ACTION,
  FF_FREEZE_AFTER_STEP1,
  FF_UNFREEZE_AFTER_NASTT,
  FF_UNFREEZE_BEFORE_PN,
  FF_UNFREEZE_AFTER_PN
}ssmgr_freezeflag_enum_type;

typedef enum{
  UFF_NO_ACTION,
  UFF_NASTT_USES_FROZEN,
  UFF_PN_USES_FROZEN
}ssmgr_usefreezeflag_enum_type;

 typedef enum 
 {
   SRCHZZ_INIT,         /* Initialization     */
   SRCHZZ_INACTIVE,     /* No search activity */
   SRCHZZ_ASLEEP,       /* Stop Search        */
 

   SRCHZZ_INTRA_STEP1,  /* Step 1 search      */
   SRCHZZ_INTRA_STEP_NASTT,  /* Step NASTT search      */
   SRCHZZ_INTRA_PN,   /* PN search        */   



   SRCHZZ_INTER_STEP1,  /* Step 1 search      */
   SRCHZZ_INTER_STEP_NASTT,  /* NASTT -async cell  search      */
   SRCHZZ_INTER_PN,   /* PN search        */   

 

   SRCHZZ_IRAT,         /* IRAT search        */


   SRCHZZ_LTE,

 #ifdef FEATURE_WCDMA_L1_PSC_SCANNER
   SRCHZZ_PSC_SCAN_STEP1, /* PSC scanner srch step1 */
   SRCHZZ_PSC_SCAN_STEP2, /* PSC scanner srch step2 */
   SRCHZZ_PSC_SCAN_STEP3, /* PSC scanner srch step2 */
   SRCHZZ_PSC_SCAN_LIST, /* PSC scanner List srch  */
#endif

   SRCHZZ_STATE_MAX        /* Last item in enum. */
 }srchzz_substate_enum_type;

#define SRCHZZ_MAP_TYPE_TO_QUEUE(srch_type) ( ((srch_type == SRCHZZ_INTRA_PN) || (srch_type == SRCHZZ_INTER_PN)) ? SRCH_PN_Q : SRCH_S1_NASTT_Q)

 typedef enum 
 {
   SRCHZZ_TYPE_INVALID,       /* Initialization */

   /* For INTRA_LIST search type */
   SRCHZZ_INTRA_LIST_TRIAGE_WAKEUP,                /* wakeup traige set */
   SRCHZZ_INTRA_LIST_TRIAGE_NORMAL,                /* normal triage set */

   SRCHZZ_INTRA_LIST_TRIAGE_ASET_REACQUIRED,

   // Below enums may not be needed since target ngbr will get added to traige set on BCH setup */
// SRCHZZ_INTRA_LIST_TRIAGE_WAKEUP_TARGET_NGBR,    /* normal triage + target CR ngbr */
// SRCHZZ_INTRA_LIST_TRIAGE_NORMAL_TARGET_NGBR,    /* normal triage + target CR ngbr */
   SRCHZZ_INTRA_LIST_NGBR                         /* ngbr list search  */

   ,SRCHZZ_INTRA_LIST_TRIAGE_SECOND_ASET
   #ifdef FEATURE_FEMTO_CSG
   ,SRCHZZ_INTRA_LIST_CSG
   ,SRCHZZ_INTER_LIST_CSG
   #endif
   , SRCHZZ_GSM_BSIC
   , SRCHZZ_GSM_RSSI
   , SRCHZZ_GSM_IMMD_BSIC
   , SRCHZZ_LTE_MEAS
   , SRCHZZ_LTE_DETECT
   , SRCHZZ_LTE_FORCED_DETECT

 } srchzz_substate_type_enum_type;

typedef enum 
{
  SRCHZZ_SRCH_MODE_DL_PUNCTURABLE,
  SRCHZZ_SRCH_MODE_DL_NON_PUNCTURABLE_SHORT,
  SRCHZZ_SRCH_MODE_DL_NON_PUNCTURABLE_LONG,
  SRCHZZ_SRCH_MODE_DL_DISABLED,
  SRCHZZ_SRCH_MODE_DL_IGNORED,
  SRCHZZ_SRCH_MODE_DL_DISABLED_NON_DRX,
  SRCHZZ_SRCH_MODE_SPLIT_SRCH,
  SRCHZZ_SRCH_MODE_NA
}srchzz_mode_enum_type;

typedef enum
{
  W_NO_SRCH,
  W_PN_SRCH,
  W_FULL_SRCH,
  W_PN_FULL_SRCH,
  GSM_SRCH,
  LTE_SRCH
}srchzz_split_srch_enum_type;

typedef struct 
{
  /* Current information */
  srchzz_mode_enum_type current_search_mode;
  uint16 current_entity_mask;
  uint16 current_mode_duration; /* In frames */
  
  /* Previous information */
  srchzz_mode_enum_type previous_search_mode;
  uint16 previous_entity_mask;

  /* Next information */
  boolean mode_change_pending;
  srchzz_mode_enum_type next_search_mode;
  uint16 next_entity_mask;
  uint16 entity_wu_after_current_mode;
} srchzz_srch_mode_struct_type;

typedef struct
{
  void (*init)(void);
  void (*cleanup)(void);
  void (*scheduler) (void);
  void (*srch_done) (srch_done_cmd_type *cmd_ptr );
  void (*resel_eval) (void);
} srchzz_srch_mode_handler_struct;
      /* Channel Manager uses this type to identify the logical entity it is
      ** registering with DRX Manager. */
  /*Pending and ignored searches flags*/

/* Pending search types to be performed */
typedef enum 
{
  PENDING_SEARCH_TYPE_NONE     = 0x0,
  PENDING_SEARCH_TYPE_INTRAF   = 0x1,
  PENDING_SEARCH_TYPE_INTERF_0 = 0x2,
  PENDING_SEARCH_TYPE_INTERF_1 = 0x4,
  PENDING_SEARCH_TYPE_INTERF   = 0x6,
  PENDING_SEARCH_TYPE_IRAT     = 0x8,
  PENDING_SEARCH_TYPE_LTE      = 0x10,
  PENDING_SEARCH_TYPE_ALL      = 0xFF
} srchzz_pending_search_type;

/* Gap size in slots */
typedef enum 
{
  MTCH_GAP_SIZE_INVALID=0,
  MTCH_GAP_SIZE_40ms_TTI=7,
  MTCH_GAP_SIZE_80ms_TTI=15
}srchzz_MTCH_gap_size_enum_type;

/* Gap periodicity in  frames */
typedef enum
{
  MTCH_GAP_PERIOD_INVALID = 0,
  MTCH_GAP_PERIOD_40ms_TTI = 8,
  MTCH_GAP_PERIOD_80ms_TTI = 16
}srchzz_MTCH_gap_period;

typedef enum
{
  RESERVED_INTERF,
  RESERVED_USED_INTERF,
  RESERVED_IRAT,
  RESERVED_USED_IRAT,
  RESERVED_NONE=0xFF
}srchzz_MTCH_gap_reserve_enum_type;

typedef struct 
{
  srchzz_MTCH_gap_size_enum_type gap_size;
  uint16 min_gap_periodicity;
  uint16 gap_periodicity;
  uint16 frames_since_last_gap;
  uint16 frames_till_next_gap;
  uint16 next_gap_cfn;
  srchzz_MTCH_gap_reserve_enum_type gap_reserved;
  uint16 mtch_longest_tti;
  uint16 mcch_phch_handle;
  uint16 mcch_sccpch_hwch_idx;
  uint16 mcch_sccpch_offset;
  boolean mcch_sccpch_evt_enabled;
  uint16 bsic_id_abort_cnt;
}srchzz_MTCH_gap_info_struct_type;

typedef struct 
{
  uint16 intra_freq_meas_status;
  uint16 inter_freq_meas_status[SRCH_OFREQ_FREQ_MAX];
  uint16 inter_rat_meas_status;
  uint16 lte_meas_status;
  uint16 timer_expiry_status;
}srchzz_idle_meas_status_struct_type;


extern srchzz_reacq_slew_stats_type srchzz_reacq_slew_stats;

extern srchzz_reacq_srch_stats_type srchzz_reacq_srch_stats;

extern srchzz_substate_enum_type srchzz_substate;

extern boolean srchzz_second_reacq_srch_issued;

extern boolean srchzz_perform_enh_ctch_algo;

/* Flag to indicate if SRCH has to wait until GSM clock is enabled
 * before sending sending SRCH_DONE cmd to DRX. This flag will be set
 * when GSM clock needs to be enabled after a SIB modification update.
 */
extern boolean srchzz_gsm_clk_ena_req;

extern boolean srchzz_reacq_failed;

extern boolean srchzz_just_sent_oos;

extern uint8 srchzz_wakeup_cell_cnt;

extern int32 srchzz_reacq_slew;

/* To count the number of contiguous failed reacquisitions. */
extern uint16 num_failed_reacquisitions;

extern srchzz_substate_type_enum_type srchzz_substate_type;
extern srchzz_srch_mode_handler_struct* srchzz_srch_mode_handler[SRCHZZ_SRCH_MODE_NA];
extern srchzz_srch_mode_struct_type srchzz_mode_info;
extern srchzz_idle_meas_status_struct_type meas_status;
extern srchzz_MTCH_gap_info_struct_type srchzz_mtch_gap_info;
extern boolean srch_delay_ext_cmd_for_wakeup_srch;

#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
extern boolean srchzz_lte_high_det_srch_pend_aft_mode_chng;
#endif

/* Variable that stores the ref_pos before any wakeup srch.*/
extern uint32 srchzz_stored_aset_ref_pos;

/* cell info of the cells that are woken up are stored in wakeup set*/
extern srch_wakeup_set_struct_type srchzz_wakeup_set;

extern uint32 srchzz_value_of_test_nv5458;

extern boolean srchzz_config_decfg_in_prog;

#ifdef FEATURE_WCDMA_REACQLESS_PICH
/*Stores the CPICH energy given by FW in Parallel reacq cycle for reference when RLP is enabled.*/
extern uint16 rlp_fw_raw_pilot_eng_ref;     

/*Stores Cell energy in Parallel reacq cycle for reference when RLP is enabled.*/
extern uint32 srch_cell_eng_ref_rlp;   

/*Used to save the stronges srch pos in reacq cycles which is used for 
  compensating finger assignment based on previous srch results.*/
extern uint32 srch_cell_pos_ref_rlp;
#endif

extern uint16 srchzz_max_duration_for_srch[12][2];

  /*Pending and ignored searches flags*/
#define SRCHZZ_MCCH_MTCH_ENTITY_ENABLED()\
  ((srchzz_mode_info.current_entity_mask & (DRX_ENTITY_MASK_MCCH|DRX_ENTITY_MASK_MTCH)) != 0)
#define SRCHZZ_BCCH_ENTITY_ENABLED() \
  ((srchzz_mode_info.current_entity_mask & (DRX_ENTITY_MASK_BCCH_INT|DRX_ENTITY_MASK_BCCH_EXT)) != 0)
#define SRCHZZ_MTCH_ENTITY_ENABLED()\
  ((srchzz_mode_info.current_entity_mask & (DRX_ENTITY_MASK_MTCH)) != 0)
#define CHECK_IRAT_DUE( ) \
    ((meas_status.timer_expiry_status & (SRCHZZ_IRAT_BSIC_TIMER | SRCHZZ_IRAT_RSSI_TIMER)) != 0)
#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
/* Check if inter freq full search or list srch timer has expired*/

#define CHECK_INTERF0_DUE( ) \
    (meas_status.timer_expiry_status & (SRCHZZ_INTER_F0_LIST_SRCH_TIMER | SRCHZZ_INTER_F0_FULL_SRCH_TIMER))

#define CHECK_INTERF_DUE( ) \
    (meas_status.timer_expiry_status & (SRCHZZ_INTER_F1_LIST_SRCH_TIMER | SRCHZZ_INTER_F0_LIST_SRCH_TIMER \
     | SRCHZZ_INTER_F0_FULL_SRCH_TIMER | SRCHZZ_INTER_F1_FULL_SRCH_TIMER) )

#define CHECK_IRAT_HIGH_PRIO_RSSI_DUE( ) \
    (((meas_status.timer_expiry_status & (SRCHZZ_IRAT_HIGH_PRIO_RSSI_TIMER)) != 0) && \
		 (srchcr_prior_info_for_rats.gsm_prior_info_present))
#endif
#define CHECK_IRAT_RSSI_DUE() \
    ((meas_status.timer_expiry_status &  SRCHZZ_IRAT_RSSI_TIMER) != 0)
#define CHECK_IRAT_BSIC_DUE() \
      ((meas_status.timer_expiry_status &  SRCHZZ_IRAT_BSIC_TIMER) != 0)

#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
/* Check if LTE Cell Detect search or Cell Meas srch timer has expired*/
#define CHECK_LTE_SRCH_DUE( ) \
      (meas_status.timer_expiry_status & (SRCHZZ_LTE_CELL_DETECT_SRCH_TIMER | SRCHZZ_LTE_CELL_MEAS_SRCH_TIMER ))

/* Check if LTE Cell Detect search timer has expired*/
#define CHECK_LTE_CELL_DETECT_SRCH_DUE( ) \
        (meas_status.timer_expiry_status & SRCHZZ_LTE_CELL_DETECT_SRCH_TIMER)

/* Check if LTE Cell Meas search timer has expired*/
#define CHECK_LTE_CELL_MEAS_SRCH_DUE( ) \
        (meas_status.timer_expiry_status & SRCHZZ_LTE_CELL_MEAS_SRCH_TIMER)
#endif

#define MIN_TIME_FOR_IRAT_SRCH 4
/* Idle DL-D mode IRAT BSIC search duration*/
#define MIN_TIME_FOR_IRAT_DRX_BSIC_ID_SRCH 8

/* Additional time to be accounted for due to W2X command processing */
#define SRCHZZ_W2X_CMD_PROCESSING_DELAY 1


#define CHECK_DURATION_FOR_BSIC_ID_BEFORE_W2X() \
         ((srchzz_mode_info.current_mode_duration > (MIN_TIME_FOR_IRAT_DRX_BSIC_ID_SRCH + SRCHZZ_W2X_CMD_PROCESSING_DELAY)) || \
             ((srchzz_mode_info.current_mode_duration > MIN_TIME_FOR_IRAT_DRX_BSIC_ID_SRCH) && (srchcr_irat_meas_state == SRCHCR_IRAT_MEAS_READY_FOR_GSM) ))

#ifdef FEATURE_WCDMA_PANIC_SRCH_OPT
/*define the panic srch max timer to be 6 sec*/
#define SRCHZZ_MAX_PANIC_SRCH_TIME_IN_FRMS 600


#define SRCHZZ_PANIC_SRCH_TIMER_EXPIRED_FOR_INTRAF()\
         (srchzz_frms_since_meas.panic_intraf_full >= srchzz_meas_max_tmr.panic_intraf_full)
         
#define SRCHZZ_PANIC_SRCH_TIMER_EXPIRED_FOR_INTER_F0()\
         (srchzz_frms_since_meas.panic_inter_full_f0>= srchzz_meas_max_tmr.panic_inter_full_f0)

#define SRCHZZ_PANIC_SRCH_TIMER_EXPIRED_FOR_INTER_F1()\
         (srchzz_frms_since_meas.panic_inter_full_f1>= srchzz_meas_max_tmr.panic_inter_full_f1)

/* Set the default currnet panic timers to 0xFFFF so that UE 
 * issues panic full srch immediately upon entering panic mode */
#define SRCHZZ_SET_PANIC_TIMER_INVALID_FOR_INTRA()\
         (srchzz_frms_since_meas.panic_intraf_full= 0xFFFF)

#define SRCHZZ_SET_PANIC_TIMER_INVALID_FOR_INTER_F0()\
         (srchzz_frms_since_meas.panic_inter_full_f0 = 0xFFFF)

#define SRCHZZ_SET_PANIC_TIMER_INVALID_FOR_INTER_F1()\
          (srchzz_frms_since_meas.panic_inter_full_f1 = 0xFFFF)

#define SRCHZZ_IS_INTRAF_ACM_DOABLE()\
          ((srchzz_frms_since_meas.panic_intraf_full != 0xFFFF)&&(SRCHZZ_PANIC_SRCH_TIMER_EXPIRED_FOR_INTRAF() == FALSE))

#define SRCHZZ_IS_INTERF_ACM_DOABLE(freq_idx)\
          (((freq_idx == FREQ_0)&&(srchzz_frms_since_meas.panic_inter_full_f0 != 0xFFFF)&&(SRCHZZ_PANIC_SRCH_TIMER_EXPIRED_FOR_INTER_F0() == FALSE))||\
           ((freq_idx == FREQ_1)&&(srchzz_frms_since_meas.panic_inter_full_f1 != 0xFFFF)&&(SRCHZZ_PANIC_SRCH_TIMER_EXPIRED_FOR_INTER_F1() == FALSE)))
#endif

#define SRCHZZ_CONSIDER_USET_IN_PANIC_MODE()\
           ((srchzz_frms_since_meas.panic_intraf_full != 0xFFFF) && srchcrutils_intraf_ngbr_fail_ecio_check())

/* Idle Mode Timers */
typedef struct
{
  uint16 aset_list;
  uint16 intra_list;
  uint16 intra_full;
  uint16 inter_list_f0;
  uint32 inter_full_f0;
  uint16 inter_list_f1;
  uint32 inter_full_f1;
  uint32 irat_rssi;
  uint32 irat_bsic;
#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
  uint32 irat_rssi_high_prio_detected;
#endif
#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY)
  uint32 lte_detect;
  uint32 lte_meas;
#endif
#ifdef FEATURE_WCDMA_PANIC_SRCH_OPT
  uint16 panic_intraf_full;
  uint16 panic_inter_full_f0;
  uint16 panic_inter_full_f1;
#endif
}srchzz_idle_meas_tmr;

extern srchzz_idle_meas_tmr srchzz_meas_max_tmr;
extern srchzz_idle_meas_tmr srchzz_frms_since_meas;

#define SRCHZZ_MAX_DURATION_TAB_SIZE 12
#define SRCHZZ_GET_SRCH_DURATION(srch_type,duration) \
do{\
    int t = 0; \
    t = WL1_GET_SET_LSB_POS_FROM_START_POS_U16(srch_type,0); \
    duration = (t>=SRCHZZ_MAX_DURATION_TAB_SIZE) ? 0xFF : srchzz_max_duration_for_srch[t][1];\
}while(0)\

#ifdef FEATURE_WCDMA_REACQLESS_PICH
#define SRCHZZ_PERFORM_RLP_THIS_CYCLE() drx_parallel_reacq_db.rlp.this_cycle
#endif
/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION     SRCHZZ_SAVE_AND_DISABLE_FING_INFO

DESCRIPTION
  This function saves and disables the fingers assigned to the serving
  cell. This should be called after intra-frequency searches have completed
  and before going to sleep.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Fingers assigned to serving cell are disabled.
===========================================================================*/
void srchzz_save_and_disable_fing_info( void );

/*===========================================================================

FUNCTION      SRCHZZ_ASET_SRCH_CMD

DESCRIPTION
  This function programs a ASET srch command to the MDSP.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void srchzz_aset_srch_cmd( void );

/*===========================================================================

FUNCTION     SRCHZZ_ASET_UPDATE_SRCH_DONE

DESCRIPTION
  This function handles the Active Set search done needed for an aset
  update during n-BCH setup and during Cell Transition.

DEPENDENCIES
  None.

RETURN VALUE
  None.
SIDE EFFECTS
  The function may change flag srch_dmod_set_changed.

===========================================================================*/
void srchzz_aset_update_srch_done( srch_done_cmd_type *cmd_ptr );

/*===========================================================================

FUNCTION SRCHZZ_NEXT_SRCH_CMD

DESCRIPTION
  This function first check if there is any step of neighbor search command
  to be issued following an ASET (HPQ) search done. if yes, the function
  issues the search command to DSP. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchzz_next_srch_cmd( void );

/*===========================================================================

FUNCTION     SRCHZZ_DECLARE_REACQ_STATUS

DESCRIPTION
  This function processes the results of aset search. After wakeup and upon 
  successful reacquisition searching, this function calls triage to do finger
  assignment. Finally, it posts a DRX_SLEEP_CMD local command with the status
  of reacquisition.  
  
  In subsequent aset searches it simply performs other tasks 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void srchzz_declare_reacq_status( boolean );

/*===========================================================================

FUNCTION     SRCHZZ_RESET_REACQ_LIST_PASSED_GOOD_DL,
             SRCHZZ_GET_REACQ_LIST_PASSED_GOOD_DL.

DESCRIPTION
    Helper functions that reset (return) a flag that indicates whether
    reacq list search passed in good downlink conditions (above a threshold
    which is above the pass/fail threshold).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void srchzz_reset_reacq_list_passed_good_dl(void);
extern boolean srchzz_get_reacq_list_passed_good_dl(void);

/*===========================================================================

FUNCTION     SRCHZZ_HANDLE_SRCH_ABORT

DESCRIPTION
  This fn is the handler for when a srch abort is called in PCH/PCH_SLEEP state.
  It checks for the following:
     - if a wake-up srch was in progress send failure to DRX
     - reset srchzz_substate
     - if any ext cmd was delayed, set the signal back so it can be
       handled now

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_handle_srch_abort(void);

/*===========================================================================

FUNCTION     SRCHZZ_IS_WAKEUP_SRCH_IN_PROGRESS

DESCRIPTION
  This function is called during search abort or external command handling to check if a wakeup search is 
  in progress
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchzz_is_wakeup_srch_in_progress(void);

/*===========================================================================

FUNCTION     SRCHZZ_ENTITY_DROP

DESCRIPTION
  This function handles the DRX->SEARCHER 'Entity drop' request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_entity_drop(uint32 bmask_rem_entities, int16 dropped_handle);

/*===========================================================================

FUNCTION     SRCHZZ_ALL_CELLS_ASLEEP

DESCRIPTION
  This function handles the DRX->SEARCHER 'go to sleep' request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_all_cells_asleep(void);

/*===========================================================================

FUNCTION     SRCHZZ_RESET_WAKEUP_CELL_COUNT

DESCRIPTION
  This function clears the global indicating num of cells woken up

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.
===========================================================================*/
extern void srchzz_reset_wakeup_cell_count(void);

/*===========================================================================

FUNCTION     SRCHZZ_WAKEUP_CELLS

DESCRIPTION
  This function is called by DRX Manager to instruct SRCH to beging tracking
  a cell or cells.  If SRCH is already tracking the PSC('s) passed in, it
  immediately reports success to DRX Manager by calling drx_srch_wakeup_done().
  Otherwise it responds after the initial search completes.

DEPENDENCIES
  None.

PARAMETERS
  wakeup_set_ptr - Array of cell structure(s) to acquire and track.
  num_cells      - Number of cells to acquire and track.

RETURN VALUE
  None.
===========================================================================*/
extern void srchzz_wakeup_cells
(
  srch_wakeup_set_struct_type wakeup_set_info
);

/*===========================================================================

FUNCTION     SRCHZZ_INIT_DRX_SRCH_MODE

DESCRIPTION
  Thisn function is called on entering idle mode. 
  Reset all idle timer and initialize the search mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_init_drx_srch_mode(void);

/*===========================================================================

FUNCTION     SRCHZZ_CHECK_INTRA_NGBRS_MEASURED

DESCRIPTION
  This function is called so that ranking cen be done only on measured cells.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchzz_check_intra_ngbrs_measured(void);

/*===========================================================================

FUNCTION     SRCHZZ_CHECK_INTER_NGBRS_MEASURED

DESCRIPTION
  This function is called so that ranking cen be done only on measured cells.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchzz_check_inter_ngbrs_measured(uint16 freq_idx);

/*===========================================================================

FUNCTION     SRCHZZ_RESET_RSSI_SRCH

DESCRIPTION
  This function is called when RSSI search is done. It reset RSSI srch timer and unreseves the MTCH gap.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_reset_rssi_srch(void);

/*===========================================================================

FUNCTION     SRCHZZ_CHANGE_MODE

DESCRIPTION
  This function does the operation required to change srch mode from one to another when the entity 
  mask changes.
DEPENDENCIES
  No searches should be going ON when this fucntion is called.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_change_mode (void );

/*===========================================================================

FUNCTION     SRCHZZ_SET_RES_EVAL_STATUS

DESCRIPTION
  This function resets the resel eval globals bases on the current search mode.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resel Eval will be reset and will be done only after the next set of measurements are complete.

===========================================================================*/
extern void srchzz_set_res_eval_status(void);

/*===========================================================================

FUNCTION     SRCHZZ_PEND_IRAT_SRCH

DESCRIPTION
  This function keeps the irat search required as TRUE or tells that we need to prioritize irat search over
  other normal searches when scheduler is called next.
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_pend_irat_srch (uint16 pend_flag, uint16 unpend_flag);

/*===========================================================================

FUNCTION     SRCHZZ_INCREMENT_IDLE_MEAS_TIMERS

DESCRIPTION
  This function icnrements the timers which are running for idle mode searches. The increment value is 
  sfn based.
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_increment_idle_meas_timers (void);

/*===========================================================================

FUNCTION     SRCHZZ_UPDATE_TIMER_EXPIRY_STATUS

DESCRIPTION
  This fucntions updates the timer expiry status of all searches based on the current running timer and the
  max periodic timer value. This is called from the scheduler.
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_update_timer_expiry_status(void);

/*===========================================================================

FUNCTION     SRCHZZ_UPDATE_TIMER_EXPIRY_STATUS_F3

DESCRIPTION
  This function calls srchzz_update_timer_expiry_status() + prints all timer related F3s.
  This is called from srchcr_update_prior_meas_rules().
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_update_timer_expiry_status_f3(void);

/*===========================================================================

FUNCTION     SRCHZZ_DL_DISABLED_MODE_SCHEDULER

DESCRIPTION
  Search Scheduler in dl-disabled mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void srchzz_dl_disabled_mode_scheduler (void);

/*===========================================================================

FUNCTION     SRCHZZ_CHECK_IRAT_SRCH

DESCRIPTION
  This function checks if valid irat cells are there to be searched.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchzz_check_irat_srch( void );

/*===========================================================================

FUNCTION     SRCHZZ_PEND_IINTERF_SRCH

DESCRIPTION
  This function keeps the interf search required as TRUE or tells that we need to prioritize interf search over
  other normal searches when scheduler is called next.
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_pend_interf_srch (uint16 pend_flag,uint16 unpend_flag, uint16 freq_idx);

/*===========================================================================

FUNCTION     SRCHZZ_PEND_IINTRAF_SRCH

DESCRIPTION
  This function keeps the intraf search required as TRUE or tells that we need to prioritize intraf search over
  other normal searches when scheduler is called next.
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_pend_intraf_srch (uint16 pend_flag, uint16 unpend_flag);

/*===========================================================================

FUNCTION     SRCHZZ_PEND_LTE_SRCH

DESCRIPTION
  This function pends/unpends the LTE search

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void srchzz_pend_lte_srch (uint16 pend_flag, uint16 unpend_flag);

/*===========================================================================

FUNCTION     SRCHZZ_SET_ALL_SRCH_DUE

DESCRIPTION
  This fucntions sets the timer expiry status of all searches. This function is called on reacq failure or on
  starting cell reselection when all searches need to be triggered.
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_set_all_srch_due(void);

/*===========================================================================

FUNCTION     SRCHZZ_RESET_ALL_SRCH_DUE

DESCRIPTION
  This fucntions sets the timer expiry status of all searches. This function is called on reacq failure or on
  starting cell reselection when all searches need to be triggered.
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_reset_all_srch_due(void);

/*===========================================================================

FUNCTION     SRCHZZ_RESET_BSIC_SRCH

DESCRIPTION
  This function is called when BSIC id search is done.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_reset_bsic_srch(void);

/*===========================================================================

FUNCTION     SRCHZZ_RSSI_SRCH_DONE

DESCRIPTION
  This function is called when Gap mode RSSI search is done. It call the srchcr function to process the
  result and resets the search.
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_rssi_srch_done(void);

/*===========================================================================
FUNCTION     SRCHZZ_UPDATE_MTCH_INFO

DESCRIPTION
  This function is called to update the MTCH info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_update_mtch_info(l1_tti_enum_type longest_tti, uint16 mcch_phch_handle);

/*===========================================================================

FUNCTION     SRCHZZ_RESET_IDLE_MEAS_TIMER

DESCRIPTION
  This function reset the timers corresponding to a particular search.
  Timer expiry status is first reset and the incrementing search timer is also reset. This function should
  when a search has been completed or after a cell reselection.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_reset_idle_meas_timer (uint16 timer_bmsk);

/*===========================================================================
FUNCTION     SRCHZZ_CALCULATE_REACQ_SLEW

DESCRIPTION
  This function is called to calculate reacq search slew

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_calculate_reacq_slew(boolean list_search);

/*===========================================================================

FUNCTION SRCHZZ_CALCULATE_CPICH_RSCP

DESCRIPTION
  This function calculates and stores the RSCP value after every 
  successful re-acquisition search during DRX state.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchzz_calculate_cpich_rscp(void);


/*===========================================================================
FUNCTION     SRCHZZ_MEAS_FILTERING_REQUIRED

DESCRIPTION
  This function checks if meas filtering is required, ie either MCCH or
  MTCH are enabled

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if filtering is required

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchzz_meas_filtering_required(void);

/*===========================================================================
FUNCTION     SRCHZZ_INIT_FILTER_ENERGY

DESCRIPTION
  This function inits filter energy value for the recently measured cells

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if filtering is required

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_init_filter_energy(srch_queue_enum_type que);

extern void srchzz_intraf_step1_done( srch_done_cmd_type* srch_cmd );
extern void srchzz_interf_step1_done( srch_done_cmd_type* srch_cmd );
extern void srchzz_intraf_nastt_done( srch_done_cmd_type* srch_cmd );
extern void srchzz_interf_nastt_done( srch_done_cmd_type* srch_cmd );
extern void srchzz_intraf_nbr_pn_srch_done(srch_done_cmd_type* srch_cmd);
extern void srchzz_interf_nbr_pn_srch_done( srch_done_cmd_type* srch_cmd );
extern void srchzz_srch_cleanup_done(void);
extern void srchzz_intraf_update_eng_params_post_pn_srch(srch_queue_enum_type que);
void srchzz_check_n_do_resel_eval(void);
extern void srchzz_ready_to_sleep(void);
uint16 srchzz_get_bsic_abort_time(void);
srchzz_mode_enum_type srchzz_get_srch_mode (uint16 entity_mask);
void srchzz_check_mode_change (uint16 entity_added, uint16 new_entity_mask);
void srchzz_restore_sleep_set(void);
void srchzz_save_wakeup_set(srch_wakeup_set_struct_type wakeup_set_info);
void srchzz_configure_cells_with_dl_srv_cell_barred_cb(void);
void srchzz_send_wakeup_srch_failure_to_drx(void);
void srchzz_configure_cells_with_dl_cb(void);
void srchzz_configure_cells_on_reacq_fail_cb(void);
void srchzz_save_step1_peaks(uint16 peak_idx);
void srchzz_interf_update_eng_params_post_pn_srch( srch_queue_enum_type que );
void srchzz_restore_step1_peaks(uint16 peak_idx);
extern boolean srchzz_start_step1_srch(void);
void srchzz_process_peaks_interf_nastt(void);

extern void srchzz_configure_cells_with_dl_triage_workloop_cb(void);

void srchzz_do_irat_srch_on_decfg_compl(void);
boolean srchzz_is_serving_cell_wakeup(void);

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY

/*===========================================================================

FUNCTION     SRCHZZ_SET_MAX_TIMER_FOR_PRIOR_SCHEDULING

DESCRIPTION
  This function checks what is the current scheduling mode according to
  priority and sets the max timer for all searches accordingly

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_set_max_timer_for_prior_scheduling(boolean scheduling_mode_changed);

/*===========================================================================

FUNCTION       srchzz_set_lte_timers_to_max

DESCRIPTION
               This function is called from DRX module before going to sleep, 
               to check whether W2G BPLMN has ended in this DRX cycle.
               If W2G BPLMN has ended, it will max out LTE srch timers so that CELL_DETECT 
               srch is performed upon wakeup (since DEINIT-REQ would have sent to LTE before BPLMN start)

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srchzz_set_lte_timers_to_max(void);
#endif

/*===========================================================================

FUNCTION       SRCHZZ_READ_AND_RESET_ACCUM_REACQ_SLEW

DESCRIPTION    This function returns the current value of the accumulated 
               reacq slew and resets the value back to zero. 

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  Accumulated reacq slew.

SIDE EFFECTS
  accum_reacq_slew is reset to zero.
===========================================================================*/
extern int32 srchzz_read_and_reset_accum_reacq_slew(void);

extern void srchzz_enable_qxt(void);

/*===========================================================================

FUNCTION     SRCHZZ_CHECK_ANY_PENDING_SRCH_DUE

DESCRIPTION
  This function checks if any pending searches are due.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If there are any pending search due
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchzz_check_any_pending_srch_due(void);

/*===========================================================================

FUNCTION     SRCHZZ_SEARCH_TRIAGE_SET_CELLS

DESCRIPTION
  This function programs the mDSP to search all cells in the triage set.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_search_triage_set_cells( void );

/*===========================================================================
FUNCTION      SRCHCR_CHECK_INTRAF_LIST_SRCH

DESCRIPTION
  This function is called to check if intra freq list search can be performed
  in time available

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if search can be started, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchzz_check_intraf_pn_srch (uint16 time_available);

/*===========================================================================

FUNCTION     SRCHZZ_CHECK_INTER_NGBRS_MEASURED

DESCRIPTION
  This function is called to perfom intra freq neigbor PN srch

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if search is started, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean  srchzz_do_intraf_neighbor_pn_srch(void);

/*===========================================================================
FUNCTION      SRCHCR_DO_INTRAF_STEP1_SRCH

DESCRIPTION
  This function is called to start intra freq step1 search

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if search started, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchzz_do_intraf_step1_srch(void);

/*===========================================================================
FUNCTION      SRCHCR_DO_INTRAF_NASTT_SRCH

DESCRIPTION
  This function is called to start intra freq NASTT search

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if search started, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchzz_do_intraf_nastt_srch(void);

/*===========================================================================

FUNCTION     SRCHZZ_SET_TIMER_VALUE

DESCRIPTION
  This function sets the timer indicated by the bitmask to the 
  passed value

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_set_timer_value (uint16 timer_bmsk, uint32 value);

/*===========================================================================

FUNCTION     SRCHZZ_SET_TIMER_TO_MAX

DESCRIPTION
  This function sets the timer indicated by the bitmask to the 
  max possible value

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_set_timer_to_max(uint16 timer_bmsk);

/*===========================================================================

FUNCTION     SRCHZZ_GET_TIMER_VALUE

DESCRIPTION
  This function will return the current value of the running timer

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern uint32 srchzz_get_timer_value (uint16 timer_bmsk);

/*===========================================================================
FUNCTION srchzz_read_idle_mode_opt_nv

DESCRIPTION
Reads the IDLE MODE NV value and sets the overrides appropriately.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void srchzz_read_idle_mode_opt_nv(void);
extern boolean srchzz_get_parallel_reacq_staus_from_nv(void);
extern boolean srchzz_get_offline_pich_staus_from_nv(void);
extern boolean srchzz_get_ngr_srch_opt_staus_from_nv(void);
extern boolean srchzz_get_offline_nastt_staus_from_nv(void);
extern boolean srchzz_get_interf_offline_pn_status_from_nv(void);
extern boolean srchzz_get_rf_wakeup_opt_staus_from_nv(void);

extern boolean srchzz_reacq_passed_with_div_chain(void);

extern void srchzz_issue_deferred_reacq_srch(void);

extern rex_crit_sect_type srchzz_drx_st_sched_lock;

extern boolean srchzz_chk_intraf_meas_timer_expiry_status(void);

extern rex_crit_sect_type srchzz_drx_st_sched_lock;

extern void srchzz_process_path_db_for_prll_reacq(void);

/*===========================================================================

FUNCTION     SRCHZZ_CHECK_PANIC_SET_IN

DESCRIPTION
  This function check if panic has set in and panic full searches need to be done.
  PANIC Condition:
    Serving cell as well as the sync-nset cells are bad and are not suitable.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean  srchzz_check_panic_set_in(void);

/*===========================================================================
FUNCTION     SRCHZZ_PERFORM_FAST_CELL_FING_DECFG

DESCRIPTION This function starts the fast cell/fing decfg.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS 
  None.
===========================================================================*/
extern void srchzz_perform_fast_cell_fing_decfg(void);

extern void srchzz_sleep_ops_action_config(void);

/*===========================================================================
FUNCTION     SRCHZZ_CLEANUP

DESCRIPTION This function cleans srchzz state variables

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS 
  None.
===========================================================================*/
void srchzz_cleanup(void);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION     srchzz_check_for_forced_nastt_pos_update

DESCRIPTION  
  This function will return the saved NASTT postion.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_check_for_forced_nastt_pos_update(uint32 ref_pos);
#endif

/*===========================================================================
FUNCTION     srchzz_init_srch_mode

DESCRIPTION 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS 
  None.
===========================================================================*/
void srchzz_init_srch_mode(void);

/*NV read function for disabling PWC algorithm for debug*/
extern void srchzz_get_test_nv5458(void);

extern void srchzz_update_meas_pending_status(boolean ue_paged);

extern boolean srchzz_srches_due(boolean cleanup_needed);

extern void srchzz_triage_set_srch_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION     SRCHZZ_PANIC_SRCH_COND_MET_FOR_CTCH

DESCRIPTION
  The function blocks CTCH unconditionally if panic searches are to be performed.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE = panic search conditions met
  FALSE = otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srchzz_panic_srch_cond_met_for_ctch(void);

/*===========================================================================
FUNCTION     SRCHZZ_PRFRM_ENH_CTCH_ALGO_ON_NEW_SCHD

DESCRIPTION
  This function is called when DL receives a new schdule from BMC. CTCH algorithm 
  is run on the new schedule 
  
PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_prfrm_enh_ctch_algo_on_new_schd(void);

/*===========================================================================

FUNCTION     SRCHZZ_RELEASE_NGBR_MEAS_TRM_IDS

DESCRIPTION
  This function releases the TRM ids associated with the neighbor bands. The
  function is called at search asleep done to release all the neighbor bands
  that are acquired while measurements.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_release_ngbr_meas_trm_ids(void);

/*===========================================================================

FUNCTION     SRCHZZ_TRIGGER_CELL_DECFG

DESCRIPTION
  This function triggers the cell_deconfig procedure based on the triage count. This is called
  during IRAT state machine transition. Typically called before a W->G or W->L measurement 

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void srchzz_trigger_cell_decfg(SRCH_CELL_CFG_DECFG_POST_PROCESSOR srch_post_processor);

/*===========================================================================

FUNCTION     SRCHZZ_DECONFIGURE_CELLS_WITH_DL_CB

DESCRIPTION
  This function is a call back when cell deconfig is done on going to sleep

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_deconfigure_cells_with_dl_cb(void);

/*===========================================================================

FUNCTION     SRCHZZ_SAVE_SLEEP_SET

DESCRIPTION
  Function is called to save the triage set into the sleep set. It is called before the sleep procedure so
  that we can restore it after wakeup

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void srchzz_save_sleep_set(void);

/*===========================================================================

FUNCTION     SRCHZZ_PREPARE_FOR_SLEEP

DESCRIPTION
  Performs actions necessary before going to sleep. 
  * Update the last known cell pos from the aset cell into ref_pos
  * Save triage set into sleep set
  * Clear the Cell Config db (FW sleep would have handled the deconfig of cells)
  * Unfreeze XO loop (we could have frozen them if we did any IRAT searches earlier in this cycle)
  * Post srch asleep done to DRX

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void srchzz_prepare_for_sleep(boolean post_srch_asleep);

#ifdef FEATURE_WCDMA_REACQLESS_PICH
/*===========================================================================
FUNCTION     SRCHZZ_UPDATE_REACQ_LESS_OPTS_THIS_CYCLE

DESCRIPTION 
This function determines whether ReacqLessPich can be performed this
 DRX cycle.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS 
  None.
===========================================================================*/
extern boolean srchzz_update_reacq_less_opts_this_cycle(void);

/*===========================================================================
FUNCTION     SRCHZZ_GET_REACQ_LESS_PICH_OPT_FROM_NV

DESCRIPTION 
This function returns whether ReacqLesSPich NV value, which is used to 
  identify whether RLP is enabled through NV.

DEPENDENCIES
  None.

RETURN VALUE
   FALSE - RLP is disabled
   TRUE - RLP is enabled

SIDE EFFECTS 
  None.
===========================================================================*/
extern boolean srchzz_get_reacq_less_pich_opt_from_nv(void);

/*===========================================================================
FUNCTION SRCH_UPDATE_ECIO_IN_RLP_CYCLE

DESCRIPTION
  This function updates Ec/Io in RLP cycles based on the CPICH energies
    exported by FW as there is no reacq search in RLP cycles.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_update_ecio_in_rlp_cycle(void);
/*===========================================================================
FUNCTION     srchzz_update_reacq_less_opts_this_cycle

DESCRIPTION
  Function clears 
 
PARAMETERS
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_clear_reacq_less_opts_this_cycle(void);
/*===========================================================================
FUNCTION     srchzz_reset_rlp

DESCRIPTION
  Function clears 
 
PARAMETERS
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
extern void srchzz_reset_rlp(void);
#endif
#ifdef FEATURE_QSH_DUMP
/*===========================================================================

FUNCTION WL1_POPULATE_DEBUG_SRCHZZ_INFO_STRUCT_TYPE

DESCRIPTION
   This function dumps state info into a global strcuture as part of a crash

DEPENDENCIES
  None.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
void wl1_populate_debug_srchzz_info_struct_type(wl1_cell_resel_mini_debug_info_struct_type * srchzz_cell_resel_dump_ptr);
#endif /* FEATURE_QSH_DUMP */
#endif /* SRCH_ZZ_H */
