#ifndef SURROUND_CC_NATIVE_L1_SC_H
#define SURROUND_CC_NATIVE_L1_SC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              l1_sc.h

GENERAL DESCRIPTION
   This module contains I/f to the SCE. Functions starting with l1_sc_
   are implemented in the SCE. Functions starting with l1_scdrv_ are
   callbacks to be called by the SCE and are implemented in the L1.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_sc.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   --------  ----------------------------------------------------------
02/11/15   sk        CR932290 Ensure NBCCH is not aborted when prioritized over CCCH
29/03/16   akm       CR739561 SCE opt top2/top6 mode, disable pwr msr of blacklisted cells
28/09/15   mko       CR912681 Don't start monitors scheduling from idle sub when monitors event status is already active/scheduled
30/07/15   am        CR882081 Ensure SCH reconfirmation happens as specified by 3GPP
20/08/15   cgc       CR893193 back to back cell updates, wait for IRAT init_cnf/deinit_cnf's before next is actioned.
29/06/15   am        CR861464 Ensure fn_when_surround_tick_changed is up-to-date
26/05/15   akh       CR838149 Ncell pwr levels for idle sub are not updated during GBTA (G Tr HMSC + G Idle)
17/03/15   xz        CR807490 sovle the address word aligment issue for scheduled_status of SCH
01/03/15   nm        CR800906 Check if sched bcch/pch in transfer was successful
18/12/14   cgc       CR769520 rework l1_sc_check_irat_active() remove obsolete booleans  
05/0114    br        CR776829 Update sce rx.abort properly while leaving dedicated mode 
05/11/14   cgc       CR743421 add G2L_MDSP_FINISH_EARLY, mdsp_g2x_cleanup_cmd() is only sent from isr for G2L cleanup.
03/12/14   ak        CR748234 Problems with legacy SCE Opt 16/17/20
17/11/14   dg        CR758006 IRAT improvements to ease debugging
29/10/14   jj        CR725942 Do Force ncell SCH in first attempt 
20/10/14   sk        CR735697 Ensure GL1 doesnt toggle between SI3/SI4 on TRM denial, and does explicit TRM release before making reservation in non-G only mode
04/11/14   df        CR734490 Rework the wake up recovery mechanism
02/11/14   sb        CR749470 Compile error fix for a LTE compiled out flavour
01/10/14   js        CR749012 GL1 changes for LTE removal
10/10/14   nm        CR735664 Reset start_wcdma_receive after TDS srch done
24/09/14   hd        CR725334: Added the function l1_sci_monscan_meas_in_progress 
25/09/14   cgc       CR728533 cell list change causes G2L stuck in GTOW_SRCH_STATE_SCHEDULE_LTE
18/12/13   ka        CR580134 RLF optimizations for CMCC for SGLTE (FR18608)
16/09/14   cgc       CR723214 Avoid forced stale irat measurements when sub1 is blocked by sub2 MO/MT.
12/09/14   nm        CR723570 GL1 changes for FR21036 - Read own PCH in transfer to BO2.0(to be leveraged for TH/JO)
06/08/14   jj        CR 705509 reducing scell meaurement when BA list is empty in dedicated mode
31/07/14   xz        CR697883 Not allow GBTA if current idle frame is doing TDS IRAT in xfer mode
22/08/14   cgc       CR713634 obsoleted G2L code and featurisations FEATURE_GSM_G2L_TIMED_SRCH and FEATURE_CM_G2L removed
04/08/15   cja       CR703742 Check IRAT state machine is NULL before isr inact is complete
10/07/14   cgc       CR631007 FR21768 enhancement force G2X measurement MPH_SPECIFIC_IRAT_FREQ_UPDATE_REQ
19/05/14   br        CR667202 Issuing ACQ command in G2L measurment gap in PTM.
19/05/14   br        CR665231 Race condition between handling of RACH in task and is state moving to GTOW_SRCH_STATE_WAIT state
25/02/14   xz        CR611918  G2W/G2L 3-tap filter implementation
10/03/14   ka        CR628921 Use LTE earfcn type in gl1 to allow move to 32bit earfcn's
02/07/14   aga       CR675214: G2T TA feature and GTA/G2W TA re-worked optimized solution 
30/05/14   ws        CR657674 Data + MMS Phase 1
27/02/14   cja       CR624068 Add extra frame on TDS reconfirm before NULL state to prevent sleep too early
17/02/14   cja       CR605188 Add PN_offset to WCDMA best six structure
05/02/14   br        CR576666 Prevent the denit if we have already sent the G2L INIT
27/01/14   ss        CR605762 call abort_receive if stale boooking detected in frame manager
27/01/14   dv        CR607497 Crash During G2L PTM with Sub2 in idle camped
23/01/14   cgc       CR606502 rework CR589523 to include TS reconfigure due to MAC_RELEASE 
20/01/13   ap        CR599098: check for compatable RF device for IRAT
24/12/13   nm        CR589209  Increase max SC BCCH attempts to 6 (before abondon)
24/10/13   sp        CR557404: Increase min frames required for NBCCH to 6 if GBTA enabled
22/11/13   cja       CR455527 Remove featurisation from files in API directory
11/20/13   sp        CR557404: Increase min frames required for NBCCH to 6 if GBTA enabled
18/10/13   cgc       CR563485 KW fixes
10/10/13   cgc       CR556101 prevent G2L init_req transition added L1_G2X_terminate()
09/10/13   cja       CR555989 Do not add extra power monitors until first complete TCH multiframe.
05/10/13   js        CR461049Cell Reselection Failure when 3 KHz frequency drift is applied on N cell
27/09/13   cgc       CR523552 add support for FR 16939 L+DSDS LTA/QTA
15/08/13   sk        CR524039 GBTA bringup changes
09/08/13   cgc       CR509276 add id_cycle_lte, make G2L idle search interval dynamic based on pg_mfrms
08/08/13   dv        CR525765 Changes for DSDS T/G IRAT bringup 
17/06/13   cgc       CR495700 rework l1_sc_wcdma_ded_pending() for x2 Qsearch_C values (W+L) rats
17/06/13   cgc       CR486358 3G_SEARCH_PRIO is now deprecated
08/05/13   cgc       CR453758 add FEATURE_CM_G2L search functionality FR-3009
25/06/13   cs        Major Triton TSTS Syncup
29/01/13   xz        CR441401 GL1 implement the new 3-tap RSCP Smooth algorithms.
29/05/13   dv        CR492212 Introduce SM to handle G2W abort
29/04/13   ws        CR481359 enable GTA support
04/04/13   ss        CR471326 Pass gas_id to the function l1_sc_start_wcdma_receive
26/03/13   ws        CR466973 - DSDS BU added prototypes from source code
27/02/13   cgc       CR431006  DSDS version add mutex and l1_sc_frames_available(), prevent concurrent task update and isr measurement
11/02/13   og        CR451559. More changes to distinguish the correct gas id.
10/02/13   ip        CR448394
18/01/13   mc        CR437838
21/12/12   dv        CR433484 Wait for G2T abort to complete before exit idle
12/12/12   cgc       CR431006 add mutex and l1_sc_frames_available(), prevent concurrent task update
26/11/12   zf        CR417719: Optimization of Ncell acquisition
19/11/12   ip/zf     CR341843: Opt16: Re-use SCH timing for re-acquiring Ncell.
10/10/12   ws        CR 401931 - Further LTE featured off compile errors
02/10/12   ky        Fixed compilation issue
03/09/12   zf        CR341843: Opt16: Re-use SCH timing for re-acquiring Ncell.
30/08/12   cgc       CR357522 G2X measuresments based on priority.
03/09/12   zf/npt    CR339131: blacklist Ncell which fails FCB receive - Opt17
02/08/12   pa        CR384676: Consider only JDS cells in G2T measurement.
27/07/12   pa        CR383096: Compilation error: undefined gsm_l1_tdd_frequency_params_type
24/07/12   og        CR379017. G2W reporting problems in idle mode.
17/07/12   cja       CR375594 Ensure dedicated W search is aborted
17/04/12   cgc       CR340815 limit G2X searches to 3W+3L frequencies
26/03/12   pa        CR347049 Added gpl1_gprs_calc_pkt_txfr_gap.
01/03/12   pa        Hash def for minimum TDD cell power level
10/02/12   ws        CR 336194 Enable TD-SCDMA IRAT for GERAN
09/09/11   cja       CR303806  Ensure G2X disable on suspend and inactive
05/09/11   cja       CR303937 Disable FW and RxLM for BPLMN suspend
03/05/11   cja       Add Nikel check sc_active definition
30/03/11   cja       Add Nikel IRAT
08/02/11   cja       CR264690 Acquisition frequency capture range larger
31/01/11   og        Issue the G2X cleanup from ISR context. Resolves CR273322.
19/01/11   cs        CR271591 DSDS Fix Ncell PCH for asynch int handling
17/01/11   og        G2L updates to resolve CRs: 271029, 271051, 271049, 270251,
                     267848,269942 and 270674.
29/11/10   og        Adding G2L support.
25/06/10   pg        Move L1_SC*BUF_LEN* defs and ba struct to l1_sc.h
                     (required for CR244125)
16/06/10   ws        Fix compile error with CR 242521 move BA_CELL_POOL_LEN  to l1_sc.h
28/04/10   nt        CR234959 Optimisation to NCELL power monitors in the idle mode
10-03-16   tjw       Renamed UNUSED to __UNUSED_ARG to avoid name clash.
17/08/08   ap        Add FEATURE_GSM_GPRS_READ_CCCH_IN_XFER
20/02/08   og        CR139102-Ensure the TCH->TCH non-blind handover uses the frequency
                     error for the target cell before camping on it.
13/02/08   nt        Removed featurisation around l1_ncell_afc_info_t as this can be used
                     when FEATURE_GSM_L1_HFREQ_ERR_NCELL_SCH_DECODE is undef.
21/09/07   cs        Reinstated CR117884
20/07/07   nt        CR107476 sync up the wcdma scheduling and the start of the
                     SACCH periods via addition of flag wcdma_start_of_sacch_indication.
11/07/07   og        Filter adjacent arfcns according to their rx power delta in
                     idle ba list. Resolves CR122895.
12/06/07   og        Upgrade the FEATURE_GSM_L1_HFREQ_ERR_NCELL_SCH_DECODE to
                     include dedicated TCH NCELL SCH attempts.
29/03/07   nt        CR:113459 Changed the number of frames required for FCCH/SCH
                     acuisition on SDCCH, so the frame manager will allow the task.
28/02/07   nt        CR112989: Turned on FEATURE_GSM_TRANSFER_DED_SERVING_CELL_INFO, by
                     removing switch from around the code.
24/01/07   nt        109818:Added l1_sc_check_for_ncc_permitted_update to update the ncc
                     permitted if the BAlist has not changed but ncc_permitted has
06/12/06   og        Fix pch block decode failures due to interruption by BPLMN
                     search. Fixes CR107369.
09/11/06   og        Fix to CR92174, handle IDLE NCELL SCH / BCCH decode at high frequency
                     error conditions.
02/01/06   ip        CR85687 & CR85686 Added function prototype
26/01/06   og        Neighbour cell acquisition changes to handle extended dynamic.
05/09/05   sv        Increased the neighbor cell Acquisition window to 20 (from 19).
04/21/05   sv        Remove the unused "power_scan_in_progress" variable
02/02/05   kf        Changes for wcdma id searches in transfer.
01/26/05   sv        Add support for Frame Manager.
01/05/05   dp        Merged dedicated timeline changes for audio optimizations
                     to mainline.  Lint cleanup
10/25/04   bm        Added wcdma_id_srch_in_progress to l1_sc_globals_T
10/20/04   sv        Optimization of Ncell SCE awake time
10/08/04   ws        Removed l1_sci_bcch_active as functionality moved to NPL1
09/22/04   gfr       Added prototype for l1_sci_bcch_active.
07/15/04   gw        Added proto for gpl1_gprs_sc_trans_post_sort_process().
06/21/04   gsc       API changes for l1_sci_set_required_frames and
                     l1_sci_check_available_frames. Added prototype for
                     l1_sci_set_required_future_frames. Added enum l1_sc_activity_T.
06/21/04   dp        Added function prototype for l1_sc_balist_multiband_sort
05/20/04   gw        Renamed some abort functions to reflect calling in ISR
                     not task context.
05/12/04   gw        Removed unused field, timeslot, from l1_sc_globals_T.
03/16/04   gw        Modified receive module design to be compatible with
                     background PLMN search.
02/25/04   gw        Lint clean up.
23/01/04   kf        added reconfig_two_close to trans_T
11/05/03   gfr       Support for quad-band.
10/14/03   gw        Added prototype for l1_sci_ok_to_schedule_cbch().
09/24/03   tb        Removed fcch_decode_failures from cell_T definition.
09/23/03   gw        Increased size of l1_sc_globals.sorted_cells from 32 to
                     BA_MAX_LEN + 1 (33).
09/12/03   tb        Mods to support scheduling of wcdma measurements.
09/01/03   kf        Changed MAX_TRANS_TONE_DETECTION_ATTEMPTS to 22
08/28/03   gw        Modified result reported for SB receives to include blocked
                     as a possibility.
08/24/03   dp        Changed MAX_TCH_TONE_DETECTION_ATTEMPTS to 22
08/12/03   kf        Removed sch_timing_error.
08/06/03   dp        Increased L1_DURATION_FCB by 4 frames to make sure the
                     Ncell ACQ doesn't run into a CCCH decode
07/30/03   kf        Removed l1_sc_abort_receive_callback_gprs prototype.
07/29/03   kf        Added prototypes for gpl1_gprs_sc_idle_post_sort_process,
                     gpl1_gprs_sc_set_pbcch_desc, gpl1_gprs_sc_receive_pbcch_trans,
                     gpl1_gprs_sc_bcch_sched_cb_trans.
07/28/03   kf        removed gpl1_re_start_surround_cell
07/14/03   gw        Removed redefinition of ASSERT.
07/14/03   gw        Added field called active to l1_sc_globals_T.
07/11/03   gfr       Lint cleanup.
07/09/03   kf        added structure trans_T
07/08/03   gw        GRR/L1 SCE cleanup. Changes to definitions for ncell BCCH decoding.
06/19/03   gw        Added prototypes for debug functions.
06/12/03   gw        Added / modified constants related to ncell receives.
                     Removed some unneeded code.
06/09/03   ws        Merged l1_4.0p branch to tip
06/06/03   tb        Modifications to support dedicated gtow searching.
06/03/03   gw        Changes to make updating of BA list more robust.  Added
                     support for detecting when ncells enter/leave to top 6.
05/29/03   kf        ncell pbcch read changse
05/29/03   gw        Added new field, in_old_ba_list, to cell_T struct.
05/14/03   gfr       Common power handler support.
05/02/03   gw        Merged in power transfer changes from branch.
05/01/03   ag        Updates to ncell pbcch reading
04/25/03   gw        Turned off SCE debug messages.
04/24/03   kf        removed redundant function.
04/15/03   tb        Added use of l1_sc_globals.wcdma_list_srch_in_progress
                     to fix problems with aborting idle mode.
04/09/03   gw        Increased length of running average buffers for GPRS from
                     45 to 85.  Added prototypes for new balist functions.
04/08/03   gw        Removed prototype for l1_sc_balist_get_new_cells().
04/04/03   gw        Removed obsolete #defines for debugging. Added declarations
                     to support transferring ncell power info between GSM modes.
04/02/03   gw        Changes to reflect modifications in monscan module.
04/09/03   ws        added l1_sc_abort_receive_callback_gprs() prototype
02/28/03   kf        Added pbcch ncell variable
02/17/03   ag        Update for FEATURE_GPRS_TBF_SUSPEND
02/17/03   kf        Changes for running average in transfer.
02/07/03   ag        Updates for FEATURE_GPRS_TBF_SUSPEND
02/03/03   ws        Merged in changes for NCELL BCCH reading for GPRS from
                     branch
01/16/03   ag        Add BCCH_MASK_4_13NORM.
01/10/03   gw        Renamed some BA list functions as part of SCE clean up.
01/02/03   gw        Added prototype for l1_sci_is_fb_in_progress().
01/02/03   gw        Added prototype for l1_sci_monscan_set_next_period_ded_cb().
                     Removed protoype for l1_sc_report_period_done_dedi() - this
                     function no longer exists.
12/03/02   mk        General GPRS featurization cleanup.
12/02/02   ws        Added new prototypes for GPRS
11/25/02   mk        Updated GPRS baseline.
11/22/02   ag        General improvements to NCELL BCCH reading under switch
                     FEATURE_GPRS_TBF_SUSPEND
11/15/02   mk        Updated GPRS baseline with release 2.0 functionality.
11/12/02   ag        Added PL1 GPRS TBF suspension for NCELL BCCH reading code
                     hooks under switch FEATURE_GPRS_TBF_SUSPEND.
10/08/02   kf        Added PL1 GPRS measurement code hooks under switch
                     FEATURE_GPRS_MEAS.
10/01/02   ATM       Added central get and set FN routines moved ADD_FN to l1_utils
08/15/02   dp        Added BCCH_MASK_7 and BCCH_MASK_8
07/25/02   dp        Added function prototypes l1_sc_monscan_next_period_ded
                     and l1_sc_rx_power_drop_acceptable. Added in_fade to cell_T
07/02/02   gw        Moved l1_running_avg_xxx to l1_sc_int.h.
06/27/02   dp        Added BCCH_MASK_3 and BCCH_MASK_4
06/27/02   gw        Changes to support running averages of ncell power
                     measurements in Idle Mode.
05/21/02   gw        Changed l1_sc_read_bcch to use dBx16_T for power instead
                     of uint8.
04/21/02   dp        Added fn_whenLost to cell structure. Prototype for
                     l1_sc_rx_pwr_acceptable
03/27/02   JC        Moved L1_SC_PWR_MON_MEAS_PER_FRAME, .. to gl1_sys_params.h
                     Also, need to implement and test QCT ASSERT()
03/22/02   JC        Removed DIVIDE macro as cleanup and changed size of
                     rx_power_sum in l1_sc_mon_scan_results_T.
02/20/02   dp        Added idle_frames_to_sb to cell_T struct
02/07/02   gw        Changes to support cleanup of SCE aborts.
12/21/01   gw        Changed prototype for l1_sci_start_sb to support DRX
                     scheduling.
12/18/01   dp        Added power_meas_in_progress to l1_sc_globals to handle
                     aborting of power measurements.   Interface changes to
                     make tone_offset an int32
10/24/01   dp        Added fields last_tone_fn and next_sb_expected to support
                     surround cell functionality on the TCH
10/23/01   mk        Mainlined support for multiple power measurements per
                     frame.
10/16/01   gw        Added support for message layer power measurement and
                     FindBCCH.  Removed prototypes for functions that have
                     been removed. Changed CCWW types to QCT types.
10/10/01   gw        Mainlined feature MON_TMP.
09/25/01   mk        General SCE related cleanup.
09/12/01   JC        Added some QCT std. template stuff.
08/17/01   mk/gw     Added Ncell support, featurized via MONITOR_DEBUG.
08/16/01   mk        Initial version.  Minor Cleanup and Header Addition.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Alphabetical Order */
#include "geran_variation.h"
#include "comdef.h"
#include "l1_sc_drv.h"
#include "l1_sc_int.h"
#include "gl1_msg.h"
#include "gl1_msg_g.h"
#include "gl1_msg_wcdma_meas.h"
#include "l1i.h" /* for running_avg_struct */
#include "gl1_sys_algo.h"
#include "gl1_sys_params.h"
#include "rex.h"

#include "geran_dual_sim.h"
#include "gpl1_dual_sim.h"


#ifdef  FEATURE_GSM_TO_LTE
#include "lte_as.h"
#include "lte_l1_types.h"
#include "lte_cphy_irat_meas_msg.h"
#endif

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

#define __UNUSED_ARG( x ) ( void )x

#define ASSERT_INT()
#define ASSERT_TASK()

#define MODULO %

#define L1_SC_NO_RANK  0xFF

/* define offset of RSRP from EFS item for LTE cell pruning */
#define LTE_PRUNE_LOW_RSRP_LEVEL -140

/* Constants for Ncell power measurements */
#define L1_SC_MAX_BUF_LEN_GSM              16
#define L1_SC_MAX_BUF_LEN_GPRS             85

#define L1_SC_MAX_MEAS_BUF_LEN             L1_SC_MAX_BUF_LEN_GPRS

#define L1_SC_TRFR_MAX_MEAS                11
#define L1_SC_TRFR_MAX_MEAS_GBTA           10

#define L1_SC_DED_MAX_MEAS_PER_MF          24

/* Constants for Ncell acquistion and SCH reconfirmation */
#define MAX_REPOSITORY_TONE_DETECT_ATTEMPTS 11
#define MAX_TCH_TONE_DETECT_ATTEMPTS 22

#define MAX_TRANS_TONE_DETECT_ATTEMPTS 22

#define DUMMY_ARFCN_FOR_SC_MEAS            0xFFEE

#define L1_SC_DURATION_FCB   (SYS_PARAM_MAX_FRAMES_FOR_SCH_NCELL_DECODING+5)


/* Maximum idle FCB duration set to 32 frames to be fit into a QTA gap */
#define  L1_SC_DURATION_FCB_IDLE  (4+SYS_PARAM_MAX_FRAMES_FOR_SCH_DECODING)
#define  L1_SC_DURATION_FCB_SDCCH (20U)

/* Add 1 frame for sending sync rx cmd and 3 frames to abort when FCB acq failed */
#define L1_SC_DURATION_FCB_IDLE_QTA                 (4+SYS_PARAM_MAX_FRAMES_FOR_SCH_DECODING_QTA)
/* Add extra 2 frame margin when SCH is late */
#define L1_SC_DURATION_FCB_IDLE_KNOWN_SCH_TIMING    (6+SYS_PARAM_MAX_FRAMES_FOR_SCH_DECODING_KNOWN_SCH_TIMING)
#define L1_SC_DURATION_FCB_GUARD_QS                 (1*QS_PER_SLOT)


#define L1_SC_DURATION_SB    4

#define L1_SC_DURATION_WCDMA_DED_SRCH    4
/* number of tries to find unused frames. After that, failure is indicated */
#define L1_SC_SB_MAX_TRIES   8

/* Constants for Ncell BCCH decoding */
#define BCCH_MASK_NORM        0x00FF
#define BCCH_MASK_3           0x0044
#define BCCH_MASK_4           0x0088
#define BCCH_MASK_34          (BCCH_MASK_3 | BCCH_MASK_4)
#define BCCH_MASK_7           0x0800
#define BCCH_MASK_8           0x8000
#define BCCH_MASK_78          (BCCH_MASK_7 | BCCH_MASK_8)
#define BCCH_MASK_EXT         0x8800
/* masks used for GPRS surround cells */
#define BCCH_MASK_13NORM      0x0010
#define BCCH_MASK_13EXT       0x0100
#define BCCH_MASK_3_13NORM    (BCCH_MASK_3 | BCCH_MASK_13NORM)
#define BCCH_MASK_4_13NORM    (BCCH_MASK_4 | BCCH_MASK_13NORM)
#define BCCH_MASK_34_13NORM   (BCCH_MASK_3 | BCCH_MASK_4 | BCCH_MASK_13NORM)
#define BCCH_MASK_3_13EXT     (BCCH_MASK_3 | BCCH_MASK_13EXT)
#define BCCH_MASK_4_13EXT     (BCCH_MASK_4 | BCCH_MASK_13EXT)
#define BCCH_MASK_34_13EXT    (BCCH_MASK_3 | BCCH_MASK_4 | BCCH_MASK_13EXT)

#define BCCH_DONT_DO           -2
#define BCCH_DO_AFTER_MEAS_RPT -1
#define BCCH_DO_NOW             0
#define L1_SC_DURATION_BCCH     8
#define L1_SC_DURATION_CBCH     8
#define L1_SC_DURATION_CCCH     5

/* Default Guard Duration for conflict */
#define L1_SC_DURATION_GBTA     8

#define L1_QTA_DURATION_GAP_MAX 37 /* 37 frames max permitted for QTA - 170ms */

#define L1_DURATION_CCCH        5 /* 1 frame to issue sync rx cmd + 4 frames per block */

/* number of tries to find unused frames. After that, failure is indicated */
#define L1_SC_BCCH_MAX_TRIES 6

#define L1_SC_BCCH_COUNT_FOR_PRI_INV 2

#define L1_IN_IRQ_MODE()  rex_is_in_irq_mode()

/* Put one extra cell in cell pool than in BA list to
 * allow for serving cell to be added.
 */
#define BA_CELL_POOL_LEN (BA_MAX_LEN + 1)

#undef DEBUG_SCE_MSG

/* If sce is stopped for some time and restarted, then the counters would be reset and
 * all cells will be deblacklisted if it has been more than some time.
 * Ideally, it should be more than TICKS_PER_FCB_BLACKLIST_CELLS, but can be kept 
 * lower than that.
 */
#define SCE_STOP_START_TIME_DIFF 510

/* sorted_cell_max_count for different modes */
#define SCE_MAX_SORTED_CELLS_IDLE_MODE_WEAK_SCELL     6
#define SCE_MAX_SORTED_CELLS_IDLE_MODE_STRONG_SCELL   2

/* GERAN - LTE reselection
 * 3 WCDMA + 3 LTE FDD
 * 3 LTE TDD to be added later.
 * For TDS, CMCC requires 9 freq
 */

#if defined (FEATURE_GSM_TDS) && defined (FEATURE_GSM_TO_LTE)

/*
** MAX_WCDMA_FREQS is used to dimension the array size only,
** so its meaning has changed and should only be used as such.
*/

/* 3 LTE FDD + 3 LTE TDD + (0 WCDMA | 9) TDS */
#define MAX_WCDMA_FREQS    (6+ MAX_UTRAN_TDD_MPH_FREQ_UPDATE_LIST)

#elif defined (FEATURE_GSM_TDS)

/* 0 WCDMA + 9 TDS*/
#define MAX_WCDMA_FREQS    (MAX_UTRAN_TDD_MPH_FREQ_UPDATE_LIST)

#elif defined (FEATURE_GSM_TO_LTE)

/* 3 LTE FDD + 3 LTE TDD + 3 WCDMA */
#define MAX_WCDMA_FREQS    (9)

#else/**/
#define MAX_WCDMA_FREQS    (6)
#endif/* defined (FEATURE_GSM_TDS) && defined (FEATURE_GSM_TO_LTE) */


#if defined (FEATURE_GSM_TDS)
#define TOTAL_TDS_FREQS              (MAX_UTRAN_TDD_MPH_FREQ_UPDATE_LIST)
#else
#define TOTAL_TDS_FREQS              (0)
#endif

#define LIMIT_WCDMA_LTE_FREQS        (6)
#define TOTAL_WCDMA_FREQS            (3)
#define TOTAL_LTE_FDD_FREQS          (3)
#define TOTAL_LTE_TDD_FREQS          (3)
#define TOTAL_LTE_FREQS              (TOTAL_LTE_FDD_FREQS+TOTAL_LTE_TDD_FREQS)


#define MAX_IGNORE_CELLS   MAX_WCDMA_FREQS /*Is this correct?*/

#define MAX_WCDMA_CELLS_PER_FREQ     32

#ifdef FEATURE_GSM_TDS
#define MIN_TDD_CELL_RSCP_VALUE    -128 /* Minimum RSCP value of TDD cell*/
#endif /*FEATURE_GSM_TDS*/

typedef enum
{
  GtowStateNull,
  GtowStateIdle,
  GtowStateDedicated

} e_GtowState;

typedef enum
{
  GTOW_SRCH_STATE_NULL,
  GTOW_SRCH_STATE_WAIT,
  GTOW_SRCH_STATE_SCHEDULE_WCDMA,
  GTOW_SRCH_STATE_SCHEDULE_LTE,
  GTOW_SRCH_STATE_WCDMA_CLEANUP,
  GTOW_SRCH_STATE_LTE_CLEANUP,
  GTOW_SRCH_STATE_LTE_CLEANUP_WAIT,
  GTOW_SRCH_STATE_LTE_CLEANUP_WAIT_2,
  GTOW_SRCH_STATE_ACTIVE,
  GTOW_SRCH_STATE_ABORTING,
  GTOW_SRCH_STATE_RAN_OUT_OF_FRAMES,
  GTOW_SRCH_STATE_MULTI_SIM_FW_WAIT,
#ifdef FEATURE_GSM_TDS
  GTOW_SRCH_STATE_SCHEDULE_TDS,
  GTOW_SRCH_STATE_TDS_CLEANUP,
  GTOW_SRCH_STATE_TDS_CLEANUP_WAIT
#endif

} e_wcdma_srch_state;

typedef enum
{
  SRCH_WCDMA_IDX,
  SRCH_LTE_IDX
} e_ded_srch_rat_index;

/*Qsearch never and THRESH always have opposite meaning for same value */
#define THRESH_ALWAYS  15
#define QSEARCH_NEVER  15
#define QSEARCH_ALWAYS  7

#define DEF_FDD_QMIN    7

/* This is used to avoid changing the structure typedef used
 * in message layer.
 */
typedef gl1_wcdma_cell_params_type             gsm_l1_wcdma_cell_params_type;

#ifdef FEATURE_GSM_TO_LTE
typedef lte_cphy_irat_meas_meas_params_s       gsm_l1_lte_cell_params_type;
typedef lte_cphy_irat_meas_cell_meas_state_s   gsm_l1_lte_tdd_vcell_type;
#endif

#ifdef FEATURE_GSM_TDS

typedef struct
{
  uint16     cell_parameter_id;
  /*!< Cell Parameter ID of the cell. */
  uint16     cell_position;
  boolean    diversity;
  /*!< Diversity value for the cell. */
  uint8      sync_case;

}gsm_l1_tds_cell_params_type;

#endif


typedef union
{
  gsm_l1_wcdma_cell_params_type  wcdma_cell;
#ifdef FEATURE_GSM_TO_LTE
  gsm_l1_lte_cell_params_type    lte_cell;
#endif
#ifdef FEATURE_GSM_TDS
  gsm_l1_tds_cell_params_type    tds_cell;
#endif /* FEATURE_GSM_TDS */
} gsm_l1_lte_wcdma_cell_params_type;

typedef union
{
  uint32 UARFCN;
#ifdef FEATURE_GSM_TO_LTE
  lte_earfcn_t  euarfcn;
#endif
} gsm_l1_lte_wcdma_arfcn_type;


typedef struct
{
    gsm_l1_lte_wcdma_arfcn_type        arfcn;

    /* Number of specific cells for this frequency */
  uint16 num_cells;
/*
** LTE G2L 
*/
    uint16                             num_detected_cells;

    gsm_l1_lte_wcdma_cell_params_type  cell_list[MAX_WCDMA_CELLS_PER_FREQ];

    boolean                            freq_only_data;

    uint16                             num_scr_codes;
/* removed rf_index as no longer used */

#ifdef FEATURE_GSM_TO_LTE
     lte_bandwidth_e                   dl_bandwidth;
#endif
    /* absolute priority of an IRAT frequency 0..7 */
     uint16                            priority;

    /* WCDMA / LTE / TDD */
    rr_l1_irat_rat_e                   rat;

#if defined (FEATURE_LTE) 
    rr_l1_eutran_type_e                lte_type;                /* added for LTE TDD */
#endif

    uint16                             best_lte_tdd_cell_idx;   /* for LTE TDD vcell_info */ 


} t_frequency_info;


typedef struct
{
  e_GtowState  state;

  /* These state machines are used for scheduling both WCDMA
   * LTE cells.
   */
  e_wcdma_srch_state  id_state;
  e_wcdma_srch_state  reconfirm_state;
  e_wcdma_srch_state  ded_state;

  e_wcdma_srch_state  id_in_tbf_state;

  void    (* done )( void* unused, gas_id_t gas_id );

  /*Flag to mark cells that had IS's aborted*/
  boolean  last_idle_id_aborted[MAX_WCDMA_FREQS];

  uint32   id_tick[MAX_WCDMA_FREQS];
  uint32   reconfirm_tick[MAX_WCDMA_FREQS];

  uint32   ded_tick;
  uint32   page_cycle_tick;
  uint32   ded_cycle_tick;
  uint16   curr_id_freq_offset;

  uint16   curr_id_in_tbf_freq_offset;

  uint16   curr_reconfirm_freq_offset;
  uint16   curr_ded_srch_freq_offset;
  uint16   next_ded_srch_cell_offset[MAX_WCDMA_FREQS];
  uint16   pg_mfrms;
  boolean  send_report;
  boolean  abort;

  /* number of paging cycles per id search cycle */
  uint16  id_cycle;
  uint16  id_cycle_lte;

  /* number of paging cycles per reconfirm search cycle */
  uint16  reconfirm_cycle;

  /* number of cells with outstanding ID srch requests */ /*lint -esym(754, ded_meas_count) suppress 'ded_meas_count' not referenced */
  //uint16  ded_meas_count;

  /* idle frames to wait for srch to finish */
  uint16   ded_timeout_count;

 /* we now need 2nd for LTE rat in Dedicated */
  dBx16_T  min_gsm_rssi_x16[2];
  dBx16_T  max_gsm_rssi_x16[2];


  dBx16_T  priority_search_threshold;

  boolean  use_priority_reselection_algo;

  int16    min_wcdma_ecio;
  uint16   sdcch_meas_frame_count;
  boolean  xfer_mode;
  boolean  xfer_srchdrv_enabled;
  
  /* save the scheduled LTE arcfn details */  
  t_frequency_info sched_earfcn;

} t_wcdma_srch;

 
#define INVALID_IDX    (0xFFFF)
#ifdef FEATURE_GPRS_GBTA
#define GPL1_SCH_SCHDULE_FAIL 0x100
#define GPL1_SCH_SCHDULE_SUCCESS 0x10
#endif /* FEATURE_GPRS_GBTA */

#ifdef FEATURE_GSM_TO_LTE

#define INVALID_IDX    (0xFFFF)
/*
** G2L timed response uses the same cells structure so compatible with existing storage
** combined results of newly detected and previously measured  for dime 9 + 8 
** ensure any results saved < MAX_WCDMA_CELLS_PER_FREQ
*/

/** use LTE values */
#define G2L_CM_LIMIT_MEAS_CELLS (9)

/* reduced to 9 from 12, LFW can only do 8 det and 9 meas ( LFW limitation not as HLD design 12+20). */
#define G2L_CM_LIMIT_DET_CELLS (8)


#define G2L_CM_MAX_MEAS_CELLS   (G2L_CM_LIMIT_MEAS_CELLS)
#define G2L_CM_MAX_DET_CELLS    (G2L_CM_LIMIT_DET_CELLS)
#define G2L_MEAS_DET_MAX_CELLS  (G2L_CM_LIMIT_MEAS_CELLS + G2L_CM_LIMIT_DET_CELLS)

#define INVALID_FN               (0xFFFF)
#define G2L_GAP_START_OFFSET       (3750)
#define G2L_GAP_END_OFFSET          (325)    
#define G2L_GAP_CLOCK_OFFSET          (0)   /* not used by LTE  */

#define G2L_MDSP_START_OFFSET      (3125)   /* offset wrt FNmod26=24 idle-1 */
#define G2L_MDSP_END_OFFSET        (4500)   /* offset wrt FNmod26=25 idle   */

#endif

#ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST

/* quality parameters to go in top-2 mode */
#define SCE_REDUCED_NCELL_LIST_SCELL_MIN_PWR    ((-70)*16)
#define SCE_REDUCED_NCELL_LIST_SCELL_MIN_RXQUAL (32000)
#define SCE_REDUCED_NCELL_LIST_BALIST_MIN_SZ    (4)

/* bmask for reduced_ncell_list_bmask */
/* dynamic */
#define SCE_REDUCED_NCELL_LIST_RR_EN            (1<<0) /* 0x01: rr gives go ahead for top-2 mode                  */
#define SCE_REDUCED_NCELL_LIST_SCELL_PWR_EN     (1<<1) /* 0x02: scell quality is good to go ahead for top-2 mode  */
#define SCE_REDUCED_NCELL_LIST_SCELL_RXQUAL_EN  (1<<2) /* 0x04: scell quality is good to go ahead for top-2 mode  */
/* static */
#define SCE_REDUCED_NCELL_LIST_BALIST_SZ_EN     (1<<4) /* 0x10: BA list sz >=4, so go ahead for top-2 mode        */
#define SCE_REDUCED_NCELL_LIST_FEATURE_EN       (1<<5) /* 0x20: feature enabled, so go ahead for top-2 mode       */
/* top-2 mode is enabled only when both RR gives go ahead, and scell power is good */
#define SCE_REDUCED_NCELL_LIST_DYNAMIC_EN       \
   (SCE_REDUCED_NCELL_LIST_RR_EN | SCE_REDUCED_NCELL_LIST_SCELL_PWR_EN | SCE_REDUCED_NCELL_LIST_SCELL_RXQUAL_EN)
#define SCE_REDUCED_NCELL_LIST_STATIC_EN        \
   (SCE_REDUCED_NCELL_LIST_BALIST_SZ_EN | SCE_REDUCED_NCELL_LIST_FEATURE_EN)
#define SCE_REDUCED_NCELL_LIST_ENABLED          \
   (SCE_REDUCED_NCELL_LIST_DYNAMIC_EN | SCE_REDUCED_NCELL_LIST_STATIC_EN)

#endif /* #ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST */

#ifdef FEATURE_GSM_TDS

/* gsm_l1_tdd_frequency_params_type consists of tdd specific frequency parameters
  stored by GL1*/
typedef struct
{
  uint32                       measuredFreq;
  /* Frequency in Hz */

  int32              rssi; /* RSSI value on the frequency */

  uint16                       ts0_position;
  /* position of TS0 : valid till ACQ hasn't been complete
   after that cell_position will be valid */

  uint8                        last_lna_state;

  uint8                        num_of_jds_cells;

  tdsirat_acq_state_e          last_acq_state;
  /* last acquisition state */

} gsm_l1_tdd_frequency_params_type;

#endif



/*
** G2L if a measuresurment occurred but was not detected, 
** remove from search list after 5 measurements, if det_cnt <=2.
*/
typedef struct
{
  uint8   meas_cnt;
  uint8    det_cnt;
  
} gsm_g2l_det_t;  



/* Local storage of measurement info */
typedef struct
  {
    int16  RSCP_avg;      /* Received signal strength (in dBm) (average of last 2) */
    int16  RSCP_last;     /* Received signal strength (in dBm) (last measurement) */
#ifdef FEATURE_GSM_TDS
    int16  RSCP_last2;    /* Received signal strength (in dBm) (3-tap smooth treatment) */
#endif
    int16  EcNo_avg;      /* Energy value returned from firmware (average) */
    int16  EcNo_last;     /* Energy value (last measurement) */

    int16  RSSI_avg;
    int16  RSSI_last;
    int16  RSSI_last2;
} t_gsm_l1_wcdma_cell_energy_details;


typedef struct
{

  int16              rsrp_avg;
  int16              rsrp_last;
  int16              rsrp_last2;
  
  int16              rsrq_avg;
  int16              rsrq_last;

} t_gsm_l1_lte_cell_energy_details;



typedef struct
{
  t_gsm_l1_wcdma_cell_energy_details  wcdma_cell;
  t_gsm_l1_lte_cell_energy_details    lte_cell;

} t_gsm_l1_lte_wcdma_cell_energy_details;


typedef struct
{
  uint32  freq;

  uint16  scr_code;  /* Scrambling code (0..511) */

  int16   rscp;      /* Received signal strength (in dBm) (average of last 2) */
  int16   rssi;
  int16   ecno;

  uint16  pn_pos;

#ifdef FEATURE_GSM_TDS
  uint16  cell_parameter_id; /* UTRAN TDD 0..127 see 25.213 */
#endif

} t_wcdma_cell_info;


typedef struct
{
#ifdef FEATURE_GSM_TO_LTE
  lte_earfcn_t  freq;
#endif
  uint16        cell_id;  /* Scrambling code (0..511) */

  int16         rsrp;      /* Received signal strength (in dBm) (average of last 2) */
  int16         rsrq;

} t_lte_cell_info;


typedef union
{
  rr_l1_irat_rat_e   rat;
  t_lte_cell_info    lte_cell;
  t_wcdma_cell_info  wcdma_cell;

} t_lte_wcdma_cell_info;

typedef enum
{
   G2L_DEINIT_NULL =0x0,
   G2L_DEINIT_PENDING = 0x01,
   G2L_DEINIT_PREVENT =0x10
} g2l_deinit_req_states_e;

typedef struct
  {
    uint16    UARFCN;
    uint16    scr_code; /* Scrambling code (0..511) */
    int16     s_qualmin;
    int16     s_rxlevmin;

  boolean   threshold_valid;
#ifdef FEATURE_GSM_TDS
  uint16    cell_parameter_id; /* UTRAN TDD 0..127 see 25.213 */
#endif

} t_cell_ignore_info;

typedef struct
{
  /* Number of cells with measurement data */
  uint16                                  num_cells;

  uint32                                  cached_UARFCN;
  uint16                                  cached_xfer_cells;

  rr_l1_irat_rat_e                        rat;   /* Make sure this is filled in correctly */

  gsm_l1_lte_wcdma_cell_params_type       cell_params[MAX_WCDMA_CELLS_PER_UARFCN];
  t_gsm_l1_lte_wcdma_cell_energy_details  cell_energy[MAX_WCDMA_CELLS_PER_UARFCN];

 /*
  ** For G2L in PTM
  */
  gsm_g2l_det_t                           cell_det[MAX_WCDMA_CELLS_PER_UARFCN];

#ifdef FEATURE_GSM_TDS
  /* TDD specific frequency parameters*/
  gsm_l1_tdd_frequency_params_type        tdd_freq_params;
#endif /* FEATURE_GSM_TDS */
} t_stored_meas;


typedef struct
{
/* This is struct holding data that will be passed from L1 to RR */
  uint16                 num_cells;

#ifdef FEATURE_GSM_TO_LTE
  /* G2L This is the size to RR and is currently only 16, may need to change TBC */
  t_lte_cell_info    lte_cell[MAX_LTE_MEASUREMENTS_PER_FREQUENCY];
#endif
  t_wcdma_cell_info  wcdma_cell[MAX_NUMBER_OF_BEST_WCDMA_CELLS];

} t_rr_report;


typedef struct
{
  /* Data received in MPH_IGNORE_WCDMA_NEIGHBORS_REQ from RR layer */

  /* Number of specific cells to measure */
  uint16 num_cells;
  t_cell_ignore_info  cell_list[MAX_IGNORE_CELLS];

} t_rr_ignore_list;


/* This will store the requested reconfirmation cells */
typedef struct
  {
  uint16  num_cells;   /* Number of cells with measurement data */
  uint32  cached_UARFCN;

  uint16  scr_code[MAX_WCDMA_CELLS_PER_DED_RECONF];        /* @@TODO: Update this to include LTE cells */

} t_requested_reconf_meas;


/* Local storage of ded mode reconfirmed cells */
typedef struct
{
  uint16                             num_cells;   /* Number of cells that have been reconfirmed */
  gsm_l1_lte_wcdma_cell_params_type  cells[MAX_WCDMA_CELLS_PER_DED_RECONF];

} t_reconf_cells;


typedef PACKED struct PACKED_POST
{
  uint16  scr_code;
  uint8   diversity;

} l1_log_wcdma_cell_info_T;

typedef PACKED struct PACKED_POST
{
  uint16                    UARFCN;
  uint8                     num_cells;
  l1_log_wcdma_cell_info_T  cell[MAX_WCDMA_CELLS_PER_FREQ];

} l1_log_wcdma_freq_info_T;


typedef struct
{
  boolean  gsm_only;
  uint16   wcdma_QSearch;              /* for Idle */
  uint8    qSearch_C[2];               /* dedicated value idx=0 ->W, 1-> L */

  uint8    wcdma_EcIo_threshold_code;
  uint8    THRESH_priority_search;

  uint8    num_freqs;
  uint8    cached_idle_freqs;
  uint8    geran_priority;

  uint8    num_W_freqs;    /* use for this for TDS as well */
  uint8    num_L_freqs;
  uint8    num_L_tdd_freqs;
/*
** add lte gap params here 
**
*/
  boolean use_next_freq;
  boolean ml1_gap_init;
  
  uint16  cntTimed_Req;
  uint16  cntTimed_Cnf;
  uint16  currFreqIdx;
  uint16  nextFreqIdx;
  uint8   G2L_Timed_Gap;

  /* FR21768 additions */ 
  uint16  ForcedMeas;        /* RR requested a forced measurement is active */
  uint16  ForcedFreqIdx;     /* freq_index of arcfn in current freq_list    */ 
  uint32  ForcedArfcn;       /* the arfcn being requested                   */
  uint32  ForcedExpiryFrame; /* rr meas report required before this expires */

  uint16  bForcedReq2Index;  /* bit mask for each freq_index forced req     */
  uint16  bForcedMeas2Index; /* bit mask for each freq_index forced meas    */

  int16   gl1_lte_cell_rsrp_threshold;  /* SGLTE prune threshold */

  /* W and/or LTE only seem to expect upto 6 frequencies to be searched.
   * MAX_WCDMA_FREQ dimensioned in l1_sc.h according to RATs supported -
   * its not JUST for WCDMA.
   */
  t_frequency_info  freq_list[MAX_WCDMA_FREQS];

} t_rr_params;

/* This structure is used to allow the L1 ncell list to store
 * frequency error correction information when transitions between
 * combined FCCH/SCH and SCH conf or BCCH occur.
 */
typedef struct
{
  int32    afc_freq;
  int32    freq_error;
  boolean  valid;

} l1_ncell_afc_info_t;


typedef struct
{
   l1_sc_bs_T bs;

   /* frame number when the last SB has been received. Only valid if bs.bsic_known */
   uint32     last_bsic_fn;

   /* number of SBs to receive before BCCh is next received. */
   int16      sb_count_to_bcch;

   /* mask specifying which BCCh blocks to receive */
   uint16     bcch_mask; /* bit 0..7: BCCh normal, bit 8..15: BCCh ext */

   /* mask specifiying which bits of bbch_mask L1 can clear autonomously */
   uint16     bcch_clear_mask;


   /* number of current try when receiving (starts with zero) */
   uint8      try_no;
   uint8      fcb_try_no;

   /* Blacklist a cell failed FCB */
   boolean    in_blacklist;

   /* marks cells in need of an SB. Only valid in the dedi modes */
   boolean    reassert;

   /* nothing happens to cell until next_tick >= current tick */
   uint32     next_tick;

   /* power rank of cell. 0..31 or L1_SC_NO_RANK if unknown */
   uint8      rank;
   uint8      prev_rank;

   /* temporary flags used by l1_sc_balist_prepare_new() */
   boolean    in_new_ba_list;
   boolean    in_old_ba_list;

   /* Frame number when most previous tone was detected on this cell */
   uint32     last_tone_fn;

   /* Cumulative number of idle frames till the next sync burst is expected */
   uint32     next_sb_expected;

   /* Number of idle frames till the next sync burst is expected */
   uint32     idle_frames_to_next_sb;

    /* Frame number when ncell was lost */
   int32      fn_when_lost;

   /*Flag to indicate if cell is in a fade or not */
   boolean    in_fade;

   l1_ncell_afc_info_t  afc_info;

   boolean  first_ncell_sch_attempt;

   /* Power measurement enabled for this cell (enabled for non blacklisted cells) 
    * Used when FEATURE_GSM_SCE_DISABLE_PWR_MSR_ON_BLACKLIST_NCELLS is defined.
    */
   boolean    pwr_msr_enabled;

   /* Power measure disabled till this time pwr_msr_disabled_till_tick.
    * See more description in the definition of TICKS_REQD_FOR_PWR_MSR.
    * Used when FEATURE_GSM_SCE_DISABLE_PWR_MSR_ON_BLACKLIST_NCELLS is defined
    */
   uint32     pwr_msr_disabled_till_tick;

} cell_T;

typedef struct
{
   /* the primary mode, idle, sdcch or tch */
   l1_sc_mode_T           mode;

   boolean                active;

   /*
    * Tick for checking surround cells.
    * Incremented each report period in dedi and each
    * paging block period in idle.
    */
   uint32                 surround_tick;

   /* set while a receive is in progress */
   boolean                receive_in_progress;

   /* Set while power measurements is in progress */
   boolean                monscan_in_progress;

   cell_T*                receiving_cell;

   /* cells with reasonable strong power sorted by decreasing power. */
   cell_T*                sorted_cells[ BA_MAX_LEN + 1 ];
   uint8                  sorted_cell_count;

   /* how long (in frames) an SB stays fresh and valid */
   uint16                 max_bsic_age;

   /* if TRUE, send l1_sc_tch_tick function to task */
   boolean                send_tch_tick;

   boolean                wcdma_srch_in_progress;
   boolean                wcdma_id_srch_in_progress;
   /*Indication to l1_sc_wcdma as to the start of a new sacch period */
   boolean                wcdma_start_of_sacch_indication;

   /* if TRUE, send gpl1_gprs_sc_tick_trans function to task */
   boolean                send_gprs_trans_tick;

   /* if TRUE L1 L1 should maintain its own timers for ncell BCCH
    * receives, if FALSE L1 should only do ncell (P)BCCH receives
    * on command from (G)RR.
    */
   boolean                autonomous_bcch;

   boolean                g2w_shutdown_pending;
   boolean                g2l_shutdown_pending;
#ifdef FEATURE_GSM_TDS
   boolean                g2t_shutdown_pending;
   /* Flag to avoid TDS MEAS and ACQ on the same paging block tick */
   boolean                g2t_perform_acq;
#endif

  rr_l1_irat_rat_e        active_irat;

   uint32                 fn_when_surround_tick_changed;

   /* Max BA list size to be reported to RR */
   uint8                  sorted_cell_max_count;

   /* #ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST */
   /* if all flags are set, then use only top 2 for BA list */
   uint8                  reduced_ncell_list_bmask;

   /* report 6 cells to RR irrespective of being in top-2 mode */
   uint8                  sorted_cell_count_rpt_to_rr;

   /* if TRUE, sort BA list to update blacklist info */
   boolean                blacklist_update_pending;
   /* #endif */ /* #ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST */

} l1_sc_globals_T;


/* Used to allow storing of the MS frequency base prior
 * to NCELL SCH/BCCH and restoring it after the attempt
 * complete.
 */
extern l1_ncell_afc_info_t  *l1_scell_afc_info_ptr;

typedef struct
{
   uint32  report_period_timer;
   boolean bcch_in_progress;
   cell_T *bcch_cell;
   boolean reconfig_two_close;
   boolean wcdma_cell_id_sceduled;
   boolean wcdma_cell_id_go;
   boolean tick_wcdma;
   uint32 new_bcch_fn;



#ifdef FEATURE_GPRS_GBTA
  uint32            gbta_current_pch_suspension_FN;
  boolean           gbta_is_tbf_pch_suspension;
  trm_client_enum_t gbta_client;
  trm_reason_enum_t gbta_suspension_reason;
#endif /* FEATURE_GPRS_GBTA */
} trans_T;

extern trans_T trans[NUM_GERAN_DATA_SPACES];

typedef enum
{
   L1_SC_DECODE_FAILURE = 0,
   L1_SC_BLOCKED        = 1,
   L1_SC_DECODE_SUCCESS = 2,
   L1_SC_SCHED_FAIL     = 3
} l1_sc_rcv_result_T;

typedef enum
{
   L1_SC_FCB            = 0,
   L1_SC_SB             = 1,
   L1_SC_BCCH           = 2,
   L1_SC_WCDMA          = 3,
   L1_SC_NONE           = 4
}l1_sc_activity_T;

typedef struct
{
    cell_T                   cell_pool[ 2 * BA_CELL_POOL_LEN ];
    cell_T*                  cell_lists[ 2 ][ BA_CELL_POOL_LEN ];

    cell_T*                  unused_cells[ 2 * BA_CELL_POOL_LEN ];
    byte                     unused_cell_count;

    ARFCN_T                  serving_cell_arfcn;
    byte                     ncc_permitted;

    /* cells in current BA and their count */

    cell_T**                 cells;
    byte                     cell_count;

    /* cells in new BA and their count */

    cell_T**                 new_cells;
    byte                     new_cell_count;
    byte                     new_ncc_permitted;
    boolean                  new_ba_list_pending;
    uint8                    multiband_reporting;

    /* serving cell info for Idle->Idle transitions */
    /* NOTE 1: No running avg buffer is allocated to
     * this cell.
     * NOTE 2: Resetting of fields in this cell is
     * very sloppy.  If you want something reset
     * make sure you do it yourself.
     */
    boolean                  serving_cell_valid;
    cell_T                   serving_cell;

    /* IDs to make sure results reported by
     * monscan correspond to a current BS list.
     */
    uint8                   old_id;
    uint8                   new_id;
} l1_ba_T;

#ifdef FEATURE_GSM_TO_LTE
typedef enum
{
   G2L_MDSP_NULL,
   G2L_MDSP_START,
   G2L_MDSP_CONTINUE,
   G2L_MDSP_FINISH_EARLY,
   G2L_MDSP_CLEANUP_COMPLETED,
   G2L_MDSP_END
} g2l_mdsp_cmd_events_e;
#endif /*FEATURE_GSM_TO_LTE*/

typedef struct
{
  uint8    THRESH_priority_search;
  uint8    geran_priority;
  uint8    num_L_tdd_freqs;
#ifdef FEATURE_GSM_SEARCH_ALL_WCELL
  boolean rr_listed_wcdma_cell_not_found;
  int16 number_of_expanded_wcdma_cell_scan;
#endif /*FEATURE_GSM_SEARCH_ALL_WCELL */

  boolean           first_init;
  dBx16_T           serving_RLA_C;
  boolean           searching_active;
  t_rr_params       rr_params;
  t_rr_ignore_list  rr_ignore_list;

  /* The fact that measurement reporting is triggered in ISR to occur in task
   * means this structure needs to be global as opposed to allocating it on
   * the stack.
   */
  t_rr_report       rr_report;

  t_wcdma_srch      wcdma_srch;

  t_requested_reconf_meas  requested_reconf_meas;

  t_reconf_cells  reconf_cells;

  uint16  requested_ded_id_srchs;

  boolean  ded_srch_aborted;
  uint16   ded_time_out;
  uint16   wcdma_tries;
  boolean  tuneback_to_gsm;

  /* The addresses of the following structures are passed to
   * the msg layer to fill in the measurement results.
   */
  gl1_wcdma_cell_meas_rpt id_rpt;
  gl1_wcdma_cell_meas_rpt recon_rpt;
  gl1_wcdma_cell_ded_rpt  ded_srch_rpt;
  gl1_wcdma_cell_ded_rpt  saved_ded_rpt; /* for use by task */

  gl1_wcdma_cell_ded_reconf_rpt ded_recon_rpt;
  gl1_wcdma_cell_ded_reconf_rpt saved_ded_recon_rpt;   /* for use in task context */


  /* stop_cell_id_srch will contain UARFCNS for which all the cells have been identified */
  uint16  stop_cell_id_srch[MAX_WCDMA_FREQS];

  /* stop_ded_cell_id_srch will contain UARFCNs for which all the cells have been identified */
  uint16  stop_ded_cell_id_srch[MAX_WCDMA_FREQS];

  /* storage for fill sample ram log packet data */
  uint16   log_fsr_freq_off;
  uint16   log_fsr_cell_off;
  uint16   log_fsr_num_cells;
  dword    log_fsr_frame_num;
  boolean  log_fsr_granted;
  boolean  wcdma_list_update_inprogress;
  boolean  wcdma_irat_active;
  boolean  wcdma_mcpm_irat_active;
  boolean  wcdma_update_list_irat_active;
  
  boolean  l1_skip_g2x_pkt_txfr;  /* skip any G2X if gprs timeslot is reconfigured */

#ifdef  FEATURE_GSM_TO_LTE
  boolean  lte_irat_active;
  boolean  lte_update_list_irat_active;

  
  uint16   lte_cm_irat_active;
  
  boolean l1_sc_trigger_cm_gap;
  boolean g2l_cm_start_oneshot;
  boolean g2l_inhibit_next_update;
  g2l_mdsp_cmd_events_e  l1_sc_trigger_g2l_idle_gap;
  uint32  g2l_idle_offset;
  uint16  ggap_g2l_mdsp[2];
  
  uint16  g2l_start_frame;
  uint16  g2l_end_frame;
#endif
#ifdef FEATURE_GSM_TDS
  /* Flag to handle another WCDMA_UPDATE_LIST_REQ*/
  boolean tds_update_list_irat_active;
  /* Make this a global flag so that we know TD cells are
   * present in ncell list. Assumption is that only G & T
   * are active
   */
  boolean tds_irat_active;
  boolean b_tds_irat_gbta_not_allowed;
#endif

  /* This is used to pass WCDMA parameters to the message layer and resolve
   * the problem whereby the union array is cast to the WCDMA structure and
   * message layer accesses it using ++
   */
  gsm_l1_wcdma_cell_params_type  gl1_msg_wcdma_cells_buffer[MAX_WCDMA_CELLS_PER_FREQ];

#ifdef FEATURE_GSM_TDS
  uint32                                reconf_req_fn;
  byte                                  tds_state;
  gsm_l1_tds_gap_info_type              gap_info;
  boolean                               l1_g2t_abort_cmd_sent;
#endif

#ifdef FEATURE_GSM_TDS
  /* Data type of this variable should be changed if MAX_WCDMA_FREQS changes*/
  uint16 toggle_id_reconf;
  boolean l1_tds_issue_cleanup;
#endif

#if (defined(FEATURE_GSM_TDS) && defined(FEATURE_GSM_TDS_DEBUG))
  volatile gap_info_debug_type gap_info_debug;
#endif

  byte                            sc_wcdma_count;
  uint8                           sc_lte_count;
  uint8                           sc_tds_count;
  uint16                          frame_count;
  wcdma_dedicated_meas_T          best_cells[MAX_NUMBER_OF_DED_BEST_WCDMA_CELLS];
  gsm_l1_wcdma_cell_params_type   gl1_msg_wcdma_reconf_cells_buffer[ MAX_WCDMA_CELLS_PER_DED_RECONF ];

  uint16                          l1_sc_send_mode_cnf_pending;

  /* This structure is updated after ID and reconfirmation search.
   * It is used when generating the measurement report to RR.
   */
  t_stored_meas     stored_meas[MAX_WCDMA_FREQS]; // this fixes it

}l1_sc_wcdma_data_t;

extern l1_sc_wcdma_data_t l1_sc_wcdma_data[NUM_GERAN_DATA_SPACES];
extern boolean mdsp_decrement_startup_time_offset_flag[];
extern boolean mdsp_decrement_cleanup_time_offset_flag[];
extern uint32 gl1_cleanup_decrement_time_offset[];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
void    l1_sc_fcb_init( gas_id_t gas_id );
void    l1_sc_fcb_shutdown( void );
void    l1_sc_start_fcb( cell_T* cell, gas_id_t gas_id );
void    l1_sc_start_fcb_tch( cell_T* cell, gas_id_t gas_id );
void    l1_sci_abort_fcb( gas_id_t gas_id );
void    l1_sci_abort_fcb_tch( gas_id_t gas_id );
void    l1_sci_tick_fcb( gas_id_t gas_id );
void    l1_sci_post_tick_fcb( gas_id_t gas_id );
boolean l1_sc_read_fcb( uint32* start_fn, int16* start_qbit, gas_id_t gas_id );
boolean l1_sc_read_fcbsb( uint8 sb_data[4], uint32* start_fn, int16* timing_error, gas_id_t gas_id );
boolean l1_sci_is_fb_in_progress( gas_id_t gas_id );
void    l1_sci_tick_fcb_tch( gas_id_t gas_id );

/* sb.c */

void    l1_sc_sb_init( gas_id_t gas_id );
void    l1_sc_sb_shutdown( gas_id_t gas_id );
void    l1_sc_start_sb( cell_T* cell, gas_id_t gas_id );
void    l1_sc_start_sb_tch( cell_T* cell, gas_id_t gas_id );
void    l1_sci_abort_sb( gas_id_t gas_id );
void    l1_sci_abort_sb_tch( gas_id_t gas_id );
void    l1_sci_tick_sb( gas_id_t gas_id );
void    l1_sci_post_tick_sb( gas_id_t gas_id );
void    l1_sci_tick_sb_tch( gas_id_t gas_id );
l1_sc_rcv_result_T l1_sc_read_sb( uint8 sb_data[4], uint32* start_fn, int16* timing_error, gas_id_t gas_id );

/* bcch.c */

void    l1_sc_bcch_init( gas_id_t gas_id );
void    l1_sc_bcch_shutdown( gas_id_t gas_id );
void    l1_sc_start_bcch( cell_T *cell, gas_id_t gas_id );
void    l1_sci_abort_bcch( gas_id_t gas_id );
void    l1_sci_tick_bcch( gas_id_t gas_id );
void    l1_sci_post_tick_bcch( gas_id_t gas_id );
boolean l1_sc_read_bcch( uint8** bcch_data, dBx16_T* rx_power, int16* timing_error, uint8* tc, uint8* num_bursts, gas_id_t gas_id );
boolean l1_sc_bcch_use_high_priority ( gas_id_t gas_id);
void    l1_sc_bcch_set_high_priority_flag ( gas_id_t gas_id);
void    l1_sc_bcch_reset_high_priority_flag ( gas_id_t gas_id);


/* balist */

void l1_sc_check_for_ncc_permitted_update(gas_id_t gas_id);
void    l1_sc_balist_init( gas_id_t gas_id );
void    l1_sc_balist_clear( gas_id_t gas_id );
boolean l1_sc_balist_prepare_new( inter_task_BCCH_allocation_T* ba_list, uint8 ncc_permitted, gas_id_t gas_id );
void    l1_sc_balist_use_new( gas_id_t gas_id );
uint8   l1_sc_balist_get_best_six( l1_sc_bs_T* best_six[ MAX_NUMBER_OF_SURROUND_CELLS ], gas_id_t gas_id );
cell_T* l1_sc_balist_get_cell_by_arfcn( ARFCN_T arfcn, gas_id_t gas_id );
void    l1_sc_balist_reset_counters( gas_id_t gas_id );
void    l1_sc_balist_reset_powers( gas_id_t gas_id );
uint16  l1_sc_balist_get( cell_T*** cells, gas_id_t gas_id );
l1_sc_bs_T*  l1_sc_balist_get_cell_data( ARFCN_T arfcn, gas_id_t gas_id );
void    l1_sc_balist_set_ra_length( uint8 len, gas_id_t gas_id );
void    l1_sc_balist_set_serving_cell( ARFCN_T arfcn, gas_id_t gas_id );
void    l1_sc_balist_set_multiband_reporting( uint8 multiband_reporting, gas_id_t gas_id );
uint8   l1_sc_balist_get_num_cells( gas_id_t gas_id );
void    l1_sc_balist_sort( boolean include_serving, gas_id_t gas_id );
void    l1_sc_balist_sort_restrict_ba_len( boolean include_serving, gas_id_t gas_id );
void l1_sc_balist_multiband_sort( byte multiband_parameter, gas_id_t gas_id );
uint8   l1_sc_balist_get_best_thirtytwo( l1_sc_bs_T* best_thirtytwo[ 32 ], gas_id_t gas_id );
void    l1_sc_balist_transfer_power_info( gas_id_t gas_id );
void    l1_sc_balist_add_serving_cell( gas_id_t gas_id );
cell_T* l1_sc_balist_get_serving_cell_ptr( gas_id_t gas_id );
void    l1_sc_balist_set_serving_cell_valid( boolean serving_cell_valid, gas_id_t gas_id );
uint8   l1_sc_balist_get_old_id( gas_id_t gas_id );
uint8   l1_sc_balist_get_new_id( gas_id_t gas_id );
cell_T* l1_sc_balist_is_cell_in_new_ba_list( ARFCN_T arfcn, gas_id_t gas_id );
void    l1_sc_balist_add_cell( l1_sc_bs_T *new_bs, gas_id_t gas_id );

/* idle.c */

void l1_sc_enter_idle( gas_id_t gas_id );
void l1_sc_leave_idle( gas_id_t gas_id );
void l1_sc_reselection_apply_freq_correction( ARFCN_T arfcn , gas_id_t gas_id);
void l1_sc_report_period_done_idle( gas_id_t gas_id );
void l1_sc_set_ba_list_idle( inter_task_BCCH_allocation_T* ba_list, uint8 ncc_permitted, gas_id_t gas_id );
void l1_sc_idle_add_serving_cell_to_balist( gas_id_t gas_id );
void l1_sc_idle_post_sort_process( gas_id_t gas_id );
uint32 l1_sc_get_gap_to_next_activity( gas_id_t gas_id );
boolean l1_sc_is_idle_rx_in_progress( gas_id_t gas_id );
boolean l1_sc_is_ncell_acq_pending( gas_id_t gas_id );
void l1_sc_reset_idle_rx_in_progress( gas_id_t gas_id );


/* dedisdcc.c */

void l1_sc_enter_dedi_sdcch( gas_id_t gas_id );
void l1_sc_leave_dedi_sdcch( gas_id_t gas_id );
void l1_sc_report_period_done_sdcch( gas_id_t gas_id );
void l1_sc_set_ba_list_dedi_sdcch( inter_task_BCCH_allocation_T* ba_list, uint8 ncc_permitted, gas_id_t gas_id );

/* deditch.c */

void l1_sc_enter_dedi_tch( gas_id_t gas_id );
void l1_sc_leave_dedi_tch( gas_id_t gas_id );
void l1_sc_report_period_done_tch( gas_id_t gas_id );
void l1_sc_set_ba_list_dedi_tch( inter_task_BCCH_allocation_T* ba_list, uint8 ncc_permitted, gas_id_t gas_id  );
void l1_sci_sb_cnf_dedi_tch( boolean success, uint8 sb_data[4], int16 timing_error );
void l1_sc_tch_tick( void* unused, gas_id_t gas_id );

/* utility.c */

void   l1_sc_adjust_lag( l1_sc_bs_T* bs, int16 timing_error );
uint32 l1_sc_decode_sb( uint8 sb_data[4], uint8* bsic );
void l1_sc_highest_lowest_timslot(gl1_defs_tn_struct* first_tn_ptr,
                                  gl1_defs_tn_struct* last_tn_ptr, gas_id_t gas_id );
void l1_sci_inc_surround_tick( int proposed_surround_tick_inc, gas_id_t gas_id );
void l1_sci_update_surround_tick_fn( uint32 frame_decrease, gas_id_t gas_id );
#ifdef FEATURE_GPRS_GBTA
/* wrapper for above func */
void gl1_allow_sci_inc_surround_tick_gbta( gas_id_t gas_id );
#endif /* FEATURE_GPRS_GBTA */

/* sc.c */

uint32 l1_sc_get_fn( gas_id_t gas_id );
void   l1_sc_report_period_done( gas_id_t gas_id );

/* dedi.c */

void l1_sc_enter_dedi( gas_id_t gas_id );
void l1_sc_leave_dedi( gas_id_t gas_id );
void l1_sc_good_sb_dedi( cell_T* cell, uint8 sb_data[4], uint32 start_fn, int16 timing_error,gas_id_t gas_id );
void l1_sc_bad_sb_dedi( cell_T* cell,gas_id_t gas_id  );

/* receive.c */

void    l1_sc_receive_init( gas_id_t gas_id );
void    l1_sc_receive_shutdown( void );
void    l1_sci_tick_receive( gas_id_t gas_id );
void    l1_sci_post_tick_receive( gas_id_t gas_id );
void    l1_sci_receive_done( gas_id_t gas_id );
void    l1_sc_receive_fcb( cell_T* cell, void ( *done )( cell_T*, gas_id_t gas_id ),gas_id_t gas_id );
void    l1_sc_receive_sb( cell_T* cell, void ( *done )( cell_T*,gas_id_t gas_id ),gas_id_t gas_id );
void    l1_sc_receive_bcch( cell_T* cell, void ( *done )( cell_T*,gas_id_t gas_id ),gas_id_t gas_id );
void    l1_sc_receive_fcb_tch( cell_T* cell, void ( *done )( cell_T*,gas_id_t gas_id ),gas_id_t gas_id);
void    l1_sc_receive_fcb_sb_tch( cell_T* cell, void ( *done )( cell_T* ) );
void    l1_sc_receive_sb_tch( cell_T* cell, void ( *done )( cell_T*,gas_id_t gas_id ),gas_id_t gas_id );
void    l1_sc_abort_receive( gas_id_t gas_id );
l1_sc_rcv_result_T l1_sc_read_sb_tch( uint8 sb_data[4], uint32* start_fn, int16* timing_error, gas_id_t gas_id );
void    l1_sc_abort_receive_callback(gas_id_t gas_id);
void    abort_receive( gas_id_t gas_id );
void    l1_sci_reset_state_machine( gas_id_t gas_id );
/* monscan.c */

void    l1_sc_monscan_init( gas_id_t  gas_id );
void    l1_sc_monscan_abort( gas_id_t gas_id );
void    l1_sc_monscan_update( gas_id_t gas_id );
void    l1_sci_monscan_next_period( gas_id_t gas_id );
void    l1_sci_monscan_next_period_ded( gas_id_t gas_id );
boolean l1_sc_rx_power_acceptable( dBx16_T rx_power);
boolean l1_sc_rx_power_drop_acceptable( dBx16_T rx_power_drop);
void    l1_sc_monscan_set_req_passes( uint8 req_passes, gas_id_t gas_id);
void    l1_sci_monscan_set_next_period_ded_cb( void (*cb)(gas_id_t gas_id), gas_id_t gas_id );
boolean l1_sc_monscan_is_not_active( gas_id_t gas_id );

/* pwrscan.c */

void l1_sc_power_scan_init( void );
void l1_sc_power_scan_shutdown( void );
void l1_sc_power_scan_start( void ( *done )( void ) );
void l1_sc_power_scan_abort( void );
void l1_sc_power_scan_read( int16** power, uint8** measurement_count );
void l1_sci_power_scan_tick( void );


/* Gprs Measurements */

/* Transfer */
void gpl1_gprs_sc_leave_trans(gas_id_t gas_id);
void gpl1_gprs_sc_enter_trans( gas_id_t gas_id );
void gpl1_gprs_sc_tick_trans( gas_id_t gas_id );
void gpl1_gprs_sc_good_sb_trans( cell_T* cell, uint8 sb_data[4], uint32 start_fn, int16 timing_error, gas_id_t gas_id );
void gpl1_gprs_sc_bad_sb_trans( cell_T* cell, gas_id_t gas_id );
void gpl1_gprs_sc_receive_sb_trans( cell_T* cell, void ( *done )( cell_T*,gas_id_t gas_id ),gas_id_t gas_id );
void gpl1_gprs_sc_receive_fcb_trans( cell_T* cell, void ( *done )( cell_T*,gas_id_t gas_id ),gas_id_t gas_id  );
boolean gpl1_gprs_sc_receive_bcch_trans( cell_T* cell, void ( *done )( cell_T*, gas_id_t gas_id ),gl1_msg_chan_type chan_type,async_ccch_type_t async_ccch_type,gas_id_t gas_id );
l1_sc_rcv_result_T gpl1_gprs_sc_read_sb_trans( uint8 sb_data[4], uint32* start_fn, int16* timing_error, gas_id_t gas_id );
void gpl1_gprs_sc_start_fcb_trans( cell_T* cell,gas_id_t gas_id );
void gpl1_gprs_sc_start_sb_trans( cell_T* cell,gas_id_t gas_id );
void gpl1_gprs_sc_start_bcch_trans( cell_T *cell,gl1_msg_chan_type chan_type,async_ccch_type_t async_ccch_type,gas_id_t gas_id  );
void gpl1_gprs_sc_trans_tick( gas_id_t gas_id );
void l1_sci_monscan_next_period_trans( gas_id_t gas_id );
void gpl1_sc_set_ba_list_trans( inter_task_BCCH_allocation_T* ba_list, uint8 ncc_permitted, gas_id_t gas_id );
boolean gpl1_gprs_sc_bcch_sched_cb_trans( gas_id_t gas_id  );
void gpl1_gprs_sci_tick_fcb_trans( gas_id_t gas_id );
void gpl1_gprs_sci_tick_sb_trans( gas_id_t gas_id );
void gpl1_gprs_sci_tick_bcch_trans( gas_id_t gas_id );
void gpl1_gprs_sc_trans_post_sort_process( gas_id_t gas_id );
void gpl1_gprs_calc_pkt_txfr_gap(uint8 *lowest_rx_tn, uint8 *highest_tx_tn, gas_id_t gas_id);
void wcdma_done_trans( void* unused, gas_id_t gas_id );

/* These functions are used to allow the TBF scheduling routines
 * to specify the highest receive/transmit timeslots for
 * the purposes of NCELL acquisition window sizing and openning.
 *
 * These timeslots concern the radio block prior to the IDLE frame
 * and are used in Packet Transfer mode.
 */
void  gpl1_pkt_xfer_set_highest_rx_timeslot( uint8  highest_rx_timeslot, gas_id_t gas_id );

void  gpl1_pkt_xfer_set_highest_tx_timeslot( uint8  highest_tx_timeslot, gas_id_t gas_id );

/* This function allows the FB acquisition module to access the highest
 * receive/transmit timeslots set by the TBF scheduling routines.
 *
 * These timeslots concern the radio block prior to the IDLE frame
 * and are used in Packet Transfer mode.
 */
void  gpl1_pkt_xfer_get_highest_timeslots( uint32*  highest_rx_timeslot_ptr,
                                           uint32*  highest_tx_timeslot_ptr,
                                           gas_id_t gas_id );

/* Idle */
void gpl1_gprs_sc_enter_idle( void );
void gpl1_gprs_sc_leave_idle( void );
void gpl1_gprs_l1_sc_set_ba_list_idle( inter_task_BCCH_allocation_T* ba_list, uint8 ncc_permitted );
void gpl1_gprs_sc_report_period_done_idle( void );
void l1_sc_set_bcch_autonomy( boolean autonomous, gas_id_t gas_id );
void gpl1_gprs_sc_idle_post_sort_process(void);
uint32 gpl1_gprs_get_bcch_fn(uint32 fn_lag, int16 qbit_lag, uint16 bcch_mask, gas_id_t gas_id);

void gpl1_gprs_predict_next_sch(gas_id_t gas_id);
void gpl1_gprs_restart_ncell_sch(gas_id_t gas_id);
void l1_sc_reset_state_machine_in_leave_idle(gas_id_t gas_id);



/* Debug */
void l1_sci_rcv_state_debug_msg( gas_id_t gas_id );
void l1_sci_monscan_state_debug_msg( gas_id_t gas_id );

/* Allow access to the ba cells list and their number */
extern cell_T**  l1sc_get_ba_cells( gas_id_t gas_id );
extern uint8     l1sc_get_ba_cell_count( gas_id_t gas_id );
extern uint8     l1sc_get_ba_blacklisted_cell_count( gas_id_t gas_id );

extern uint8 l1_get_idle_monitors_in_progress(gas_id_t gas_id);
extern void l1_increment_idle_monitors_in_progress(gas_id_t gas_id);
extern void l1_decrement_idle_monitors_in_progress(gas_id_t gas_id);
extern void l1_set_idle_monitors_in_progress(uint8 idle_monitors_in_progress, gas_id_t gas_id);


#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
void gl1_ms_async_ncell_bcch_sleep_adjust( int16 frame_adjust, gas_id_t gas_id );
void gl1_ms_async_ncell_sch_sleep_adjust( int16 frame_adjust, gas_id_t gas_id );
void gl1_ms_async_idle_pch_sleep_adjust( int16 frame_adjust, gas_id_t gas_id );
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

/*===========================================================================
FUNCTION l1_sc_ncell_monitor_timer_start

DESCRIPTION
  Starts the ncell monitor shutdown timer for the time period defined by
  L1_NCELL_MONITOR_SHUTDOWN_TIMER_SIG
===========================================================================*/
void l1_sc_ncell_monitor_timer_start(gas_id_t gas_id);

/*===========================================================================
FUNCTION l1_sc_init_ncell_monitor_timer

DESCRIPTION
  creates/defines the ncell monitor shutdown timer
===========================================================================*/
void l1_sc_init_ncell_monitor_timer(gas_id_t gas_id);

/*===========================================================================
FUNCTION L1_SC_IRAT_CHECK_ACTIVE

DESCRIPTION
  This function is called when MPH_WCDMA_CELL_UPDATE_LIST
  message from RR. It checks if any RATs have become active
  or inactive.
  Also used to force G2X shutdown for inactive and suspend
  and for restarting G2X for resume
===========================================================================*/
void l1_sc_irat_check_active(byte    wcdma_num_of_cells,
                             uint8   lte_freq_count,
                             boolean update_sc_list,
                             byte    tds_freq_count,
                             boolean cm_mode,
                             gas_id_t gas_id);


void l1_sc_gsm_l1_set_ecio_rssi_levels( uint16 qsearch_W, uint16 qsearch_L, uint16 ecode_both, gas_id_t gas_id );

extern l1_sc_globals_T* gl1_ms_switch_l1_sc_globals_store(gas_id_t gas_id);

extern void l1_set_serving_RLA_C(dBx16_T local_serving_RLA_C, gas_id_t gas_id);
extern dBx16_T l1_get_serving_RLA_C(gas_id_t gas_id);
extern void l1_sc_start_wcdma_receive( gas_id_t gas_id );
extern void l1_sc_stop_wcdma_receive( gas_id_t gas_id );
extern boolean l1_sci_monscan_meas_in_progress(gas_id_t gas_id);

/* moved for G2L */
uint8 l1_sc_get_tch_idle_frame( gas_id_t gas_id ); 

void l1_isr_G2L_trigger(gas_id_t gas_id);

void L1_G2X_terminate(gas_id_t gas_id);

boolean l1_is_rf_device_irat_compatable(gas_id_t gas_id);

void    l1_isr_G2L_idle_trigger(gas_id_t gas_id);

#if defined (FEATURE_DUAL_SIM)
/*
** conversion macro for sys_modem_as_id_e_type is in sys.h 
*/
#define GL1_MSGR_SET_HDR_INST(hdrPtr,in_gas_id)  \
                                  { \
                                      sys_modem_as_id_e_type ginst_id = geran_map_gas_id_to_nas_id(in_gas_id); \
                                      msgr_set_hdr_inst( hdrPtr, (SYS_AS_ID_TO_INST_ID(ginst_id)));    \
                                  } \

#else
#define GL1_MSGR_SET_HDR_INST(hdrPtr,in_gas_id)            
/*
** Nothing is required
*/
#endif

#ifdef  FEATURE_GSM_TO_LTE

/*
** Used for LTE messages to set inst_id from as_id from gas_id, 0=0=0
*/



lte_l1_gsm_band_e gsmband2lte(sys_band_T gsmband);

boolean gl1_sc_irat_is_lte_active( gas_id_t gas_id );


typedef enum
{
    LTE_G2L_INIT_NULL    =0x00,
    LTE_G2L_INIT_SENT    =0x01,
    LTE_G2L_INIT_CNF     =0x02,
    LTE_G2L_CNF_PENDING  =0x04,
    LTE_G2L_INIT_PENDING =0x08,
    LTE_G2L_DEINIT_SENT  =0x10,
    LTE_G2L_DEINIT_CNF   =0x20,

/* no longer used on bolt LTE_G2L_CM_MODE */

    LTE_G2L_MASK         =(LTE_G2L_INIT_SENT   | LTE_G2L_INIT_CNF | LTE_G2L_CNF_PENDING),   /* 0x07 */
    LTE_G2L_READY        =(LTE_G2L_INIT_SENT   | LTE_G2L_INIT_CNF ),                        /* 0x03 */
    LTE_G2L_DEINIT       =(LTE_G2L_DEINIT_SENT | LTE_G2L_DEINIT_CNF)                        /* 0x30 */
    
} g2l_init_req_states_e;


uint16  g2l_get_Init_Sent_status(void);

void    g2l_set_Init_Sent_status( uint16 g2l_status_bit);
void    g2l_clr_Init_Sent_status( uint16 g2l_status_bit);

#endif  /* FEATURE_GSM_TO_LTE */

uint16   l1_sc_find_freq_index( uint32 findArfcn , rr_l1_irat_rat_e findRat , boolean anyIndexForRat , gas_id_t gas_id);

void gl1_check_if_irat_state_stuck(gas_id_t gas_id);

boolean  g2x_irat_cnf_pending(gas_id_t gas_id);

/*Keep track of no of frames for which GL1 should wait before triggering a crash
   while aborting a particular state */
#define NO_FRAMES_TO_CHECK_IF_GL1_IS_STUCK 30

#endif
