#ifndef SRCHI_H
#define SRCHI_H

/*===========================================================================
   FILE: srchi.h

GENERAL DESCRIPTION:
      This file contains definitions used internally by the Search module.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright(c) 2000 to 2013 by Qualcomm Technologies, Inc. All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchi.h_v   1.43   11 Jul 2002 21:25:56   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wsrchi.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/16   rn      Changes to implement the algorithm to optimize the performance for 
                   x2w resume acq post LTA cases
12/07/15   vn      Corrected Compiler error for S1PPer 
10/20/15   vn     Changes for Step1 Peak Pruner algorithm.
09/09/15   vn      Changes for attenuating W signal through QXDM 
01/29/15   vn      Using correct EcIo table for s criteria calculations while in BPLMN
12/29/14   sl      Added wl1 timer for CPHY_CELL_TRANS_REQ processing
12/23/14   bj      Adding debug command in WPLT for PWRUP ACQ full window PN search.
10/17/14   amj     DSDA search utilization adjustment.
10/15/14   ag      Added a new API to combined ASET EcIo and RSCP value of primary antenna.
10/10/14   amj     Code Cleanup.
10/03/14   jd      Fix for handling pended searches in FACH state.
09/10/14   amj     Nastt Power Optimization changes.
09/09/14   gj      Decoupling the mutex contention between search heartbeat ISR and WL1 task search result processing
08/23/14   bj      Added split acq across DRX support in BPLMN state.
07/28/14   amj     Jolokia Updated searcher tables and RUMI database
07/15/14   ac      NV reorganization
06/23/14   ar      Changes to support AbsPrio Tresel scaling factor for GSM and LTE
06/27/14   ar      Add FILT_IDLEMODE_LTE to deal with int values
06/27/14   bj      Fix to pick correct NcNn(128*36) table for NASTT pilot threshold value in BPLMN state.
06/23/14   bj      Reset async search params during srch_abort due to mode change.
06/16/14   ag      Copying last filtered value to default filter after HHO
05/28/14   amj     Mainline FEATURE_WCDMA_GET_RAT_MEAS_INFO.
05/28/14   amj     MCVS searcher changes for VPE clock.
05/27/14   ag      Update the edch_active for cells from both carr idx.
05/21/14   jk      Moved the periodic measurements handling from srch_ops_cmd to a 
                   new local command.
05/20/14   amj     Cleaned up some unused functions for Step1 processing.
05/13/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
05/01/14   gsk     Changes for Version-2 of qxdm command to crash UE on unexpected search results
04/07/14   ag      Skip measurement evaluation for 3rd carrier
02/28/14   sad     SCCH order optimization check in
04/01/14   km      Rxd/ACQ changes
01/07/14   amj     Clean up old ASET Update code.
12/16/13   as      Porting G2W TA from Triton to Dime.
11/25/13   jk      FR 15721: HSPA+: Call performance improvement when selecting the best cell.
11/21/13   jd      FET 2.0 Searcher two queue approach.
11/06/13   gj      Added 3C searcher code
10/23/13   sm      Search modifications for cpc-drx+rxdpm
10/29/13   sad     Inital check in for Secondary ASET update changes
10/17/13   bs      Preliminary 3C Support
09/12/13   jd      Searcher redesign 
08/19/13   vn      Added flag to indiate RRC that acq is failing 
                   because of a band mismatch
08/13/13   jd      Changes to use mutex during Intraf step1 and nastt reservation.
08/12/13   da      Handle X2W resume acq to active set cells
07/24/13   jd      Changes for Dual band Dual carrier
07/23/13   jd      parallel step1 changes
07/23/13   vs      modified the APIs srch_get_aset_rscp and srch_get_aset_rscp_div
07/03/13   gsk     Adding Qxdm support to crash UE on unexpected search results
06/28/13   mk      Support for ignoring IRAT measurements in PCH and FACH when manual CSG mode is enabled.
06/06/13   amj     Added table for ecio for step1 with NN parameter 30 with and w/o rxd
05/15/13   zr      Externing async parm init so RxD status can be maintained at DCH init
04/08/13   amj     Added search field to ASU procedure
02/05/13   vn      Added macro to calculate pn pos difference in cx8.
01/31/13   ag      Added define for cell reporting threshold in DCH.
01/21/13   stk     Support for Femto/CSG selection and reselection
01/03/13   zr      Moved PN search result buffer to heap memory
09/04/12   pv      Defined MUTEXLOCK/UNLOCK for srchzz prll op.
11/19/12   amj     Supporting imbalanced RxD step 1 search
11/07/12   gsk     Adding lookup tables for step1 search with NN value of 20/40
10/18/12   pr      Mainlined FEATURE_UMTS_SIR_PATHLOSS.
10/17/12   hk      Changes to limit the ref pos update for cells enabled in FW
09/24/12   zr      Deprecating ASET log packet 0x4110 in favor of 0x41AC, which
                   has dual carrier support
09/06/12   amj     Fixed compiler warning
08/09/12   hk      Streamlined FACH and DCH module cleanup procedures
08/15/12   gsk     Updating the RxAGC read with new RF api
08/15/12   zr      Adding search debug counter
08/14/12   zr      Adding heartbeat counter for searcher periodic logging
08/10/12   sr      Adding support for start/stop measurements
                   during DCH/FACH state.
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12   gsk     Idle mode FRs
06/29/12   zr      Replace acq retry limit with acq attempt limit
06/29/12   zr      Correcting comment to reflect new function implementation
06/11/12   vs      MBMS feature cleanup.
05/09/12   sv      Support full window PN search when processing inter-frequency redirection.
05/06/12   sv      Added function prototype to check if search abort is pending.
05/04/12   zr      Supporting rounded RSCP
04/29/12   sv      Avoid scheduling of SFN decode when another internal decode pending. Pend it for later.
04/12/12   cv      Added a new macro for DCH->idle translation table
03/10/12   sv      Removed reference to TxD max retry definition.
02/12/12   hk      Nikel feature cleanup
02/08/12   sv      Externed API to reset search substate.
02/01/12   gnk     Nikel feature cleanup
01/29/12   vs      Feature cleanup.
12/09/11   mmr     Added a define to provide the correct Qxt path threshold.
12/02/11   stk     Added support for FEATURE_WCDMA_RXD_PCH and FEATURE_IDLEMODE_FILTER
11/12/11   sv      Prevent out-of-bounds access when cell Ecio reaches noise floor.
11/12/11   sv      Introduced DC NASTT pending flag to avoid multiple DC NASTT 
                   searches scheduled.
11/12/11   sv      Enhancement to update cell ref_pos before sending events-1A & 1C
09/13/11   sv      Increase max no of intraf events supported to 9.
07/25/11   mmr     Changed freq_hypo from int16 to int32.
06/29/11   sa      Define critical section in CPC DRX mode.
06/17/11   sa      Added code to support list search logging for all cells
                   after every 1 sec.
06/17/11   hk      Blocking the TxD detection retry mechanism
06/10/11   sv      Added define for invalid DC dync neighbor index
05/23/11   sa      Added function prototype.
05/13/11   sv      Added support for periodic TxD determination.
04/28/11    sj      upmerge from offline/main
04/20/11   sv      Fixed compiler warnings. 
04/19/11   sv      Added support for FEATURE_WCDMA_GET_RAT_MEAS_INFO.
03/17/11   sv      Externed new API functions to calculate EcIo and RSCP for both Rx chains.
03/11/11   dp      Ported CPC DRX changes to offline Architecture
02/14/11   sv      Added new NASTT threshold for DC cells.
01/31/11   sv      Externed function that returns freq hypo to be used.
01/20/10   sv      Externed the API that overrides freq common info in FW. 
12/21/10   dm      Moved mutex initializations to wl1m while doing 
                   task initialization.
10/19/10   sv      Added changes to support full search for DC and QICE support.
09/30/10   scm     Implement extended OOS recovery support.
08/17/10   hk      Corrected the STTD validation for blind HHO cases;
08/06/10   sv      Added pullin threshold for GTW full window search.
07/09/10   sv      Added extern for function srch_is_resend_srch_needed.
06/21/10   mr      Compiler warning fixes
06/07/10   ms      Added new function srch_determine_cpich_tx_div_for_dc
06/02/10   stk     Added support for W2LTE measurements and reselection under 
                   FEATURE_WCDMA_ABSOLUTE_PRIORITY & FEATURE_WCDMA_TO_LTE
05/27/10   sa      Added macro to convert linear energy to dB.
05/03/10   sv      Fixed compiler warnings.
04/21/10   sv      Fixed compiler warning.
03/17/10   sv      Added threshold for limited window HHO search.
03/16/10   sv      Externed the function srch_determine_cpich_tx_div.
02/06/10   sv      Added function headers.
09/30/09   sa      Added the decalaration of mutex to replace INTLOCK/INTFREE 
                   with REX based calls.
09/18/09   sv      Externed the srchacq_cur_freq_offset variable.
09/04/09   sv      Added macros needed during GTW acquisition.
09/04/09   sv      Externed the variable srchacq_mech.
09/02/09   uk      Added initialization for SRCHTSCNT and SRCHTBAR critical sections
08/14/09   sv      Support for enabling RxD on Step1, NASTT searches in DCH.
08/11/09   sv      Fix compiler warning.
07/16/09   hk      Reorganized the code to remove INC/SRC violations
                   Moved the file from INC to SRC
06/30/09   sv      Changed the prototype for srch_process_rxd_cb().
05/27/09   stk     Fixed Compiler Warning
05/26/09   sv      Added support for sending freeze\unfreeze sample server command to FW.
05/08/09   sup     Changing the includes for the renaming of header files  dec.h, enci.h, l1m.h, srch.h, srchi.h, 
                   srchlog.h to wdec.h, wenci.h, wl1m_v.h, wsrch.h, wsrchi.h, wsrchlog.h
05/08/09   hk      Corrected the calculation of Tm
05/06/09   sv      Added externs for srch_update_pn_sync_parms, srch_update_rxd_info
                   and srch_process_rxd_cb.
05/05/09   sv      Added extern for srch_reset_ops_sanity_chk.
04/29/09   sq      Removed compiler warnings
04/22/09   stk     Compiler warning fixes
04/15/09   sv      Added extern for srch_map_mcal_srch_to_whl1_srch_type.
04/03/09   sv      Added new thresholds for reacq searches.
03/17/09   sv      Moved the SRCHACQ_MAX_RETRY_CNT define to here.
03/13/09   sv      Corrected the ACQ pullin path threshold.
03/09/09   sv      Added new search subtates under FEATURE_WCDMA_SRCHACQ_RETRY.
02/05/09   sv      Corrected the prototype for srch_send_srch_cleanup_done. Added
                   externs for new functions.
01/20/09   sv      Corrected some search thresholds + code cleanup.
01/09/09   vsr     Addded default STTD state and detection over-ride
12/09/08   sv      Mainlined FEATURE_WCDMA_OFFLINE_SEARCHER, FEATURE_RUMI_BRINGUP 
                   and FEATURE_WCDMA_DL_ENHANCED.
12/04/08   sv      Externed the srch_bplmn_set_samp_serv_freeze function.
12/03/08   rgn     Added support for Tx Diversity detection procedure 
11/13/08   sv      Added searcher support for HHO on Mustang.
10/03/08   hk      Bringing in 7k Mailine fixes
09/25/08   sv      Code cleanup for SRCH-DL i\f.
09/12/08   nd      changes for cell deconfig during state cleanup 
07/22/08   vc      Changed the srch_pn_determine_nc_nn function to public scope.
01/15/08   sv      Added support for (256,16) searches when in STTD mode in DCH. 
                   Changes under the FEATURE_STTD_OPTIMIZATION feature flag.
12/10/07   sv      Added support for dynamic step3 window sizing.
11/12/07   rc      Added support for FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY feature.
10/26/07   sv      Added searcher support for FEATURE_MODEM_MBMS and FEATURE_WCDMA_DL_ENHANCED.
09/26/07   nd      added a new enum for freq scan substate
08/22/07   nd      added few externs
07/24/07   nd      added few extern for S-CPICH feature support
07/16/07   nd      added a structure to store the interf rach rpt params
05/30/07   rc      Externed srchacq_wcdma_hho_sttd_indicator flag.
05/16/07   nd      Added prototype for srchdch_reset_filt_params_all_timing_known_cells()
                   under FEATURE_WCDMA_MULTIPLE_L3_FILTERS_SUPPORTED
03/29/07   vip     Added the protype defintion srchdch_get_cnt_c_sfn_diff() for 
                   FEATURE_WCDMA_CNT_C_SFN_DIFF
03/05/07   rc      Added code for FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2 feature.
01/31/07   nd      report ecio to RRC in l1_get_rssi() under FEATURE_UMTS_REPORT_ECIO
01/22/07   nd      Added event 1J support under FEATURE_WCDMA_HSUPA
11/29/06   nd      Added multiple layer3 filter support for intra freq measurement
                   under FEATURE_WCDMA_MULTIPLE_L3_FILTERS_SUPPORTED
11/01/06   cpe     Mainlined the following features:
                    -FEATURE_STTD_CAPABLE
                    -FEATURE_BARRED_CELL_DURING_CELL_SELECTION
10/26/06   cpe     Mainlined a bunch of features
08/18/06   rc      Externed srch_interf_store_meas_db flag.
07/24/06   sv      Adding extern for intrafrequency step1,2 variables.
06/20/06   bd      Renamed global vars/functions to avoid conflict with 1x names
05/11/06   sk      Rel5 CR 136
04/17/06   vb      Externed srch_find_tx_div_ind()
04/17/06   sv      Changes to support Rx Diversity search filtering.
04/11/06   vip     Extern the flag srchacq_ratchange_sttd_indicator for demod
                   to update the RL database during resume
03/20/06   sv      Changes to support FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE.
11/24/05  nd/gv    Moved extern of srch step1/2 async params here.
11/08/05  gv/nd    Added code for WtoW BPLMN feature.
09/27/05   m       Added macro to perform logarithmic filtering calculation
09/19/05   cpe     Added externs to several functions in srchdch.c
08/26/05   nd      added a flag "stopped" in the meas db to indicate stopped
                   measurements after inter freq HHO
08/02/05   cpe     Added extern to a function defined in srchdch.c
06/23/05   nd      Added an extern for a variable declared in srch.c
06/21/05   cpe     Added an extern for a variable declared in srch.c
06/02/05   m       Added two more substates to ACQ srch state. They are used
                   in HHO procedure
05/31/05   vp      Merging of fixes for Lint errors
05/18/05   cpe     Added extern to a function defined in srchdch.c
05/03/05   m       Fixed bugs reported in integration of srch optimization
                   feature and resume on HHO failure
05/01/05   ub      Cell reselection improvements and fixes
04/11/05   cpe     Code addition for resuming to old config upon HHO failure.
04/07/05   m       Added support for FEATURE_OPTIMIZE_SEARCH_FOR_HHO
04/06/05   ub/ba   HCS based cell reselection feature (FEATURE_WCDMA_HCS)
04/06/05   ub      Use filtered EcIo for cell reselection
03/01/05   cpe     Added extern to a function defined in srchdch.c
10/06/04   scm     Add extern to srchacq_init_vctcxo_and_rotator().
09/22/04   sh      Added flags needed to support SIB modification updates
                   in Idle and FACH states.
08/19/04   cpe     Added cells for meas to srch_intra_freq_meas_parms_struct_type
                   Also added a couple of externs.
06/28/04   cpe     Added an extern
06/15/04   cpe     Added a couple of new structures
06/14/04   cpe     Changed the prototype of srchdch_sort_reporting_cells()
06/07/04   cpe     Changed size of step3 eng from uint16 to uint32
05/02/04   cpe     Added an extern
04/29/04   cpe     Changed the definition of srch_intra_freq_meas_parms_struct_type
                   to track best cell for each 1D event configured
04/21/04   cpe     Added an extern for function srchdch_reset_evt_rpt_dbase
04/15/04   cpe     Added an extern
04/22/04   scm     Don't featurize srch_abort_all_but_mdsp().
04/01/04   scm     Add srchacq_get_reset_vctcxo_param_flag() function.
03/26/04   cpe     Added two more fields to srch_ops_ctl_parm_struct_type
                   Several externs as well.
03/22/04   cpe/sh  Extern declarations for error-recovery related changes.
03/08/04   src     Support for a timeout with search abort-all.
03/01/04   src     Support for error recovery in some cases of mDSP dead/stuck.
02/11/04   cpe     Extern'ed a function from srch.c
02/11/04   sh      Define min thresholds for Q-qualmin and Q-rxlevmin.
12/18/03   scm     Move srchacq_rpt() extern to srchdmod.h.
12/10/03   cpe     Add FEATURE_WCDMA_INTRAF_SUPPORT_IN_FACH
12/01/03   cpe     Added a couple of fields to srch_intra_freq_meas_db structure
11/12/03   cpe     Extern'ed a few variables
11/06/03   cpe     Added an extern function defined in srchacq.c
11/03/03   cpe/src Added code for STTD capability (featurised under the feature
                   flag FEATURE_STTD_CAPABLE).
10/30/03   cpe     Extern'ed a couple of functions.
                   Removed the feature FEATURE_WCDMA_MULTIPLE_INTRA_MEAS_SUPPORTED
                   which is always present
10/24/03   cpe     Extern'ed a couple of functions
09/09/03   m       Modifications to support Inter Freq HHO
08/04/03   cpe     Added prototype for srchacq_dup_frame_peak to fix compile
                   Warning
06/20/03   cpe     Defined SRCH_NO_USET_IDX and added prototype of a function
                   srchmeas_get_oldest_uset_idx()
05/23/03   cpe     Added support for multiple intra freq measurements
05/22/03   sh      Add thresh_interrat_incl to srch_aset_parms_struct_type.
03/21/03   yus      Declared extern variable srch_meas_db_init
01/21/03   yus     Change rpt_deact_thresh and rpt_act_thresh to int16
                   in srch_inter_freq_event_crit_struct_type.
12/05/02   yus     Added support for 2D and 2f event.
11/26/02   ddh/cpe Added srchacq_mode variable
11/05/02   cpe     Replaced FEATURE_WCDMA_INTRAF_SRCH_IN_CM with
                   FEATURE_WCDMA_INTRAF_SUPPORT_IN_CM
                   Added an extern variable which stores the value of
                   RSCP during DRX state
10/01/02   ddh     Added interrat members to aset_parm structure
                   Added prototype for srchacq_dup_frame_peak to fix compile
                   Warning
10/01/02   ddh     Removed NO_SCR_CODE and NO_FREQ these defines are already
                   in l1const.h. Added define for SRCH_NO_RANK
09/31/02   yus     Add support for FEATURE_WCDMA_INTRAF_SRCH_IN_CM.
07/11/02   sh      Changed srchcr_list_srch_ecio_tab_ptr to uint32.
06/18/02   yus     Changed eng unit from uint16 to uint32.
05/18/02   sh      Moved BCH and Cell Reselection related protos and
                   declarations to srchbch.h and srchcr.h.
05/18/02   sh      Added srchcr_list_srch_ecio_tab_ptr. Moved BCH substate
                   and cell reselection related protos to srchbch.h, srchcr.h
05/11/02   cpe     Added prototypes of functions for calculating tm and OFF and
                   also for processing search results in FACH state
04/03/02   cpe     define two constants NO_SCR_CODE and NO_FREQ
03/21/02   cpe     Added an extern for srch_ops_ctl_parms defined in srchevt.c
03/11/02   cpe     Added a few externs for logging aset and nset packets
                   Added a periodic cnt to  srch_ops_ctl_parm_struct_type
                   for scheduling periodic measurement reporting operations
02/22/02   cpe     Added prototypes for srch_log_aset_packet and srch_log_nset_packet
                   functions
02/20/02   cpe     Added prototypes for search_done and srch_ops_cmd in DRX
                   state
                   Added rxagc_log_cnt to srch_ops_ctl_parm_struct_type structure
12/05/01   cpe     Added cell_report_status info to srchdch_evt_crit_struct_type
11/02/01   cpe     Added macros to accomplish conversion between PRI_SCR_CODE and
                   SCR_CODE.
10/15/01   cpe     Added the prototype for srch_out_of_service_area_check()
                   function
09/12/01   wjn     Exported variable "srch_wcdma_filt_rscp' to other srch files.
09/10/01   cpe     Replaced filter_coef parameter with two seperate parameters
                   aset_filter_coef and nset_filter_coef in srch_intra_f_meas_parms
                   structure
08/30/01   wjn     Added SRCH_FS_STATE for WCDMA Carrier Frequency Scan.
08/08/01   cpe     Changed the type of rach_rpt_max to uint16
07/19/01   wjn     Exported variable "srch_aset_upd_mode'.
06/28/01   wjn     "#undef"ed various symbols used for wrapping up the code
                   which is not used currently but may be used in the future.
06/11/01   cpe     Moved the contents of srchtestinput.h here and featurized them
                   with SRCH_UNIT_TEST/SRCH_SURF_TEST flags
05/30/01   cpe     added the condition to raise an error if both SRCH_UNIT_TEST
                   and SRCH_SURF_TEST are defined.
05/25/01   cpe     moved srchdch_evt_crit_struct_type from srchdch.c also
                   expanded the srch_intra_f_meas_parms_struct_type
05/20/01   wjn     Added function prototypes srch_calculate_cpich_rscp(),
                   srchdch_proc_srch_done() and srchdch_proc_srch_ops().
05/14/01   wjn     Deleted function prototypes srch(acq/bch/dch)_proc_lost_srch().
                   Lost search processing for all search states is now
                   consolidated in srch_proc_lost_srch().
05/13/01   wjn     Added the function prototypes "srchdch_sfn_cfn_diff_update()"
                   and "srchdch_proc_lost_srch()".
05/07/01   wjn     Added the function prototypes "srchbch_proc_lost_srch()",
                   "srch_hpq_cmd_post_cb()" and "srch_lpq_cmd_post_cb()".
04/27/01   wjn     Added the function prototype "srch_list_win_ctr()" for
                   updating "ref_pos" (reference position of a cell for
                   centering search window).
                   Updated "srch_aset_parms_struct_type" structure to
                   match Standard Specification of 2000-12 Release. And beefed
                   up comments on the parameters inside the structure.
03/17/01   wjn     Added/Updated code for various things, such as srch
                   module Time stamp, cell reselection delay timer and
                   throttle timer, aset update at various states, and cell
                   camp book keeping, etc.
03/05/01   wjn     Defined non-cell-specific intra-frequency measurement
                   parameter structure and related function prototypes.
02/14/01   wjn     Added code to support unified aset update mechanism cross
                   all states.
02/05/01   wjn     Decoupled searcher sanity check from searcher queue update
                   to save precious DSP cycles --- so we do not have to
                   update search queue parameters each time we issue a search
                   command.
01/24/01   wjn     Added SRCH_IDLE_STATE.
01/23/01   wjn     Added stuff for cell reselection evaluation.
01/17/01   wjn     Added search Energy-EcIo conversion function prototypes.
                   Beefed up comments for various function prototypes.
11/09/00   wjn     Added more code.
07/05/00   wjn     Created file.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "wl1m.h"
#include "wsrch.h"
#include "srchparm.h"
#include "srchcmd.h"
#include "srchset.h"
#include "l1rrcif.h"
#include "wl1srchdlif.h"
#include "srchacq.h"
#include "srchacqproc.h"
#include "srchmeas.h"

#undef SRCH_CELL_SEL_ASET_UPD_TEST /* For BCH ASET update test */
#undef SRCH_FAKE_NBR_TEST          /* For Neighbor Report Test */
#undef SRCH_CELL_RESEL_TEST        /* For Cell Reselection Test */

#define SRCH_SFN_CFN_MEAS_OPTIMIZATION
           /* For optimizing SFN-CFN time difference measuremnt scheduling */
#undef SRCH_DCH_ASET_UPD_TEST      /* For DCH ASET Update Test */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define SRCH_NO_REF_TIME 0xFFFF
   /* No Reference time info for the cell */
#define SRCH_NO_REF_POS  0xFFFFFFFF
   /* No Reference postion for window centering */
#define SRCH_NO_USET_IDX  0xff
#define SRCH_MAX_FREQS  L1_MAX_NUM_FREQ
   /* Max Number of Frequencies */
#define SRCH_MAX_CELLS_PER_FREQ  L1_MAX_NUM_CELLS
   /* Max number of cells per frequency */

#define SRCH_INVALID_FREQ 0xFF
  /* Invalid value of cell frequency */


#define SRCH_NO_RANK  -1000
   /* Initial value to set rank of cell to unrealistic number */

#define SRCH_NO_INTRAF_MEAS_ID (0xff)
#define SRCH_NO_MEAS_INDEX     (0xff)
/*   Max num of reported cells in cell measured results */
#define SRCHDCH_CELL_MEAS_RES_MAX 12

/* Minimum values for Q_qualmin and Q_rxlevmin. This will make sure that
 * we have some sane values when the network settings are not correct.
 */
#define SRCH_MIN_Q_QUALMIN  -20
#define SRCH_MIN_Q_RXLEVMIN -119

#ifdef FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
#error code not present
#endif

/* Size of translation tables from IS-95 Ec/Io to anything. This is just the
 * number of possible Ec/Io values 
 */
#define  ECIO_TAB_SIZ       64

/* mask of the cells that can be sent for PN searches*/
#define SRCH_PN_ASET_INCLUDED             0x00000001
#define SRCH_PN_SYNC_NSET_INCLUDED        0x00000002
#define SRCH_PN_ADD_CSET_INCLUDED         0x00000004
#define SRCH_PN_DROP_CSET_INCLUDED        0x00000008
#define SRCH_PN_USET_INCLUDED             0x00000010
#define SRCH_PN_CAND_USET_INCLUDED        0x00000020
#define SRCH_PN_HSET_INCLUDED             0x00000040
#define SRCH_PN_MBMS_ASET_INCLUDED        0x00000080
#define SRCH_PN_MBMS_SYNC_NSET_INCLUDED   0x00000100
#define SRCH_PN_NBCH_SET_INCLUDED         0x00000200
#define SRCH_PN_BCH_CELL_SELECT_INCLUDED  0x00000400
#define SRCH_PN_TRIAGE_SET_INCLUDED       0x00000800

#ifdef FEATURE_WCDMA_DC_HSDPA
#define SRCH_PN_DC_ASET_INCLUDED          0x00004000
#define SRCH_PN_DC_ADD_CSET_INCLUDED      0x00008000
#define SRCH_PN_DC_CAND_USET_INCLUDED     0x00010000
#define SRCH_PN_DC_DROP_CSET_INCLUDED     0x00020000
#define SRCH_PN_DC_NBCH_SET_INCLUDED      0x00040000
#define SRCH_PN_DC_USET_INCLUDED          0x00080000
#define SRCH_PN_DC_SYNC_SET_INCLUDED      0x00100000
#define SRCH_PN_DC_TRIAGE_SET_INCLUDED    0x00200000
#define SRCH_PN_3C_ASET_INCLUDED          0x00400000
#define SRCH_PN_3C_SYNC_SET_INCLUDED      0x00800000
#define SRCH_PN_3C_USET_INCLUDED          0x01000000


#endif

#ifdef FEATURE_FEMTO_CSG
#define SRCH_PN_CSG_INTRA_INCLUDED        0x4000
#define SRCH_PN_CSG_INTER_INCLUDED        0x8000
#endif


/* mask of the cells (timing unknown) that can be sent for NASTT-async searches*/
#define SRCH_NASTT_INCL_A_NO_PN_SET       0x0001
#define SRCH_NASTT_INCL_ASYNC_NSET        0x0002
#define SRCH_NASTT_INCL_SYNC_NSET         0x0004
#define SRCH_NASTT_INCL_ASET              0x0008

/* mask of timing known cells sent for inter-f NASTT-async searches*/
#define SRCH_NASTT_INCL_OFREQ_ASYNC_NSET  0x0004

#define SRCH_STEP1_ASYNC_PARM_UPD_NEEDED 0x01
#define SRCH_NASTT_ASYNC_PARM_UPD_NEEDED 0x02
#define SRCH_PN_ASYNC_PARM_UPD_NEEDED    0x04

#define SRCH_ALL_ASYNC_PARM_UPD_NEEDED (SRCH_STEP1_ASYNC_PARM_UPD_NEEDED|   \
                                        SRCH_NASTT_ASYNC_PARM_UPD_NEEDED|   \
                                        SRCH_PN_ASYNC_PARM_UPD_NEEDED)

#ifdef FEATURE_WCDMA_DC_HSDPA
/* Invalid DC dync neighbor index */
#define SRCH_NO_DC_SYNC_CELL_IDX        0xFF
#endif

#ifndef SRCHMEAS_MAX_NUM_CARR
#ifdef FEATURE_WCDMA_DC_HSUPA
#define SRCHMEAS_MAX_NUM_CARR 2 
#else
#define SRCHMEAS_MAX_NUM_CARR 1 
#endif
#endif

#define THREE_DB_IDX_ECIO_TAB 6

#define SRCH_RXD_ACQ_MODE(srch_state) (((srch_state) == L1M_ACQ || \
                                       (srch_state) == L1M_BCH) && \
                                        rxd_is_active())

/*check if full win PN search was attempted during RUMI bringup*/
#define SRCH_RUMI_PERFORM_ACQ_FULL_WIN_PN_SRCH() ((search_state == SRCH_ACQ_STATE) && \
                                                  (srch_rumi_db.bringup_enabled) && \
                                                  (srch_rumi_db.acq_use_full_win_pn_srch)) 



/*--------------------------------------------------------------------------
                         Function-like Macros
--------------------------------------------------------------------------*/

/* "(n-1)/n" style IIR filter -- see 8.5.7.7.2 of TS25.331. */
#define  FILT_N( val, newval, n ) \
  ((uint32) (( ((uint32) (val) * ((n)-1)) + (uint32) (newval) ) / (n)))

#if (defined(FEATURE_REL5) && defined(FEATURE_WCDMA_LOG_FILTERING))
#define FILT_LOG_N(val, newval, n) \
   ((uint32) (( ((uint32) (val) * ((n)-1)) + (uint32) (newval) *512 ) / (n)))
#endif /* (defined(FEATURE_REL5) && defined(FEATURE_WCDMA_LOG_FILTERING) */

#define  FILT_IDLEMODE( val, newval, coeff ) \
          ((uint32) ((((uint32) (val) * (1024 - coeff)) + ((uint32) (newval) * (coeff)) ) / (1024)))

#define  FILT_IDLEMODE_LTE( val, newval, coeff ) \
           ((((val) * (1024 - coeff)) + ((newval) * (coeff)) ) / (1024))

/* Given 2 PN positions, compute the PN difference. 
 * NOTE: pn1, pn2 MUST be in cx1 units.
 */
#define SRCH_GET_PN_POS_DIFF(pn1, pn2) ((pn1 - pn2) < CHIP_PER_FRAME/2) ? \
                                        (pn1 - pn2) : \
                                        (CHIP_PER_FRAME - (pn1 - pn2))

/* Given 2 PN positions, compute the PN difference. 
  * NOTE: pn1, pn2 MUST be in cx8 units.
  */
#define SRCH_PN_POS_DIFF(x, y)  \
   ((labs((int32)((int32)(x) - (int32)(y))) < CHIPX4_PER_FRAME ) ? labs((int32)((int32)x-(int32)y)) : (CHIPX8_PER_FRAME-labs((int32)((int32)x-(int32)y))))

/*--------------------------------------------------------------------------
                            SEARCH MODULE STATES

--------------------------------------------------------------------------*/
typedef enum
{
  SRCH_IDLE_STATE,    /* Idle state */
  SRCH_FS_STATE,      /* WCDMA Carrier Frequency Scan */
  SRCH_ACQ_STATE,     /* WCDMA Cell acquisition */
  SRCH_BCH_STATE,     /* Reception of a BCH channel on Serving Cell */
  SRCH_FACH_STATE,    /* Reception of a FACH channel on Serving Cell */
  SRCH_DCH_STATE,     /* Reception of a DCH channel on Serving Cell */
  SRCH_DRX_STATE,     /* DRX in Idle Mode or Connected Mode */
#ifdef FEATURE_WTOW_BACKGROUND_PLMN_SEARCH
  SRCH_BPLMN_STATE,
#endif
  SRCH_MAX_STATE      /* Must remain last item in enum. */
} srch_state_enum_type;

/*--------------------------------------------------------------------------
              ENUMERATION REASON FOR SEARCHER TRIGGERING SFN DECODE
--------------------------------------------------------------------------*/
typedef enum
{
   SRCH_NONE
  ,SRCH_DCH
  ,SRCH_FACH
} srch_sfn_decode_client_enum_type;

/*--------------------------------------------------------------------------
           Search Operation Sanity Check Control Structure Type
--------------------------------------------------------------------------*/
typedef struct
{
  boolean s1_nastt_pn_q_enabled[SRCH_NUM_Q];
  uint16 lost_s1_nastt_pn_srch_cnt[SRCH_NUM_Q];
    /* HPQ List Search Sanity Timer */
  boolean otdoa_q_enabled;
  uint16 lost_otdoa_srch_cnt;
}
srch_ops_sanity_chk_struct_type;

/*--------------------------------------------------------------------------
                Search Operation Control Structure Type
--------------------------------------------------------------------------*/

typedef struct
{
  uint16 rssi_update_cnt;
  uint16 aset_srch_cnt;
  uint16 nset_srch_cnt;
  uint16 fing_assign_cnt;
  uint16 rxagc_log_cnt ;
  uint16 step1_cnt;
  uint16 nastt_cnt;
  uint16 dc_step1_cnt;
  uint16 dc_nastt_cnt;
  uint16 w_3c_step1_cnt;
  uint16 w_3c_nastt_cnt;
  uint16 all_cells_srch_cnt;
  uint16 periodic_meas_rpt_cnt;
  uint16 srch_log_cnt;
  uint16 srch_dbg_cnt;
}
srch_ops_ctl_parm_struct_type;

extern  srch_step_enum_type srch_s1_nastt_pn_q_step[SRCH_NUM_Q];
extern  srch_step_enum_type srch_otdoa_q_step ;

/* Parameters to keep track of fs thresholds,
num retrys etc.  */
typedef struct
{
	int16 raw_scan_thresh;
	int16 fine_scan_thresh;
}
srch_fs_parameters_type;

#ifdef FEATURE_WCDMA_FS_FALSE_PEAK_ELIM_OPT
typedef enum
{
  /* no scanning activity going on*/ 
  SRCH_FS_SCAN_INACTIVE,
  /* Ready to scan next frequency on command*/
  SRCH_FS_SCAN_READY,
  /* send FS_CMD to mdsp and waiting for the result from mdsp*/
  SRCH_FS_SCAN_WAITING_FOR_MDSP_RESULT
}srchfs_scan_substate_enum_type;

#endif

typedef struct
{
	int16 number_of_retries;
}
srchacq_parameters_type;

typedef struct
{
  /* pos can range from 0 -(2560*8 -1) in cx8 units. 16 bit is enough to hold the data */
  uint16  pos;
    /* position, or offset to the Slot Reference Counter,
       of this slot peak in chipx8 resolution */
  uint16  eng;
    /* Energy of this slot peak */
  uint16 freq_hypo_idx;
    /* which freq hypothesis the step1 peak belong to. can take [0..2] 3 hypothesis */
  uint8 ant_id;
    /* if step1 is performed with rxd mode 1 i.e separate then this will indicate which antenna
    this peak belong to. values can be 0-primary and 1 -diversity */
  uint8 carr_idx; 
#ifdef FEATURE_WCDMA_STEP1_PEAK_PRUNER
  /* flag indicating whether this peak will be pruned or not */
  boolean to_be_pruned;
#endif 
}
srch_slot_peak_struct_type;

/*--------------------------------------------------------------------------
                            PILOT PEAK STRUCTURE

   This data structure stores peaks of a pilot from NAS-TT search.

--------------------------------------------------------------------------*/
typedef struct
{
  uint16  scr_code;
    /* Scrambling Code of the pilot */
  uint32  pos;
    /* position of the maximum energy path in 1/8 chip units. */
  uint32  eng;
    /* Energy of the maximum energy path */
  uint8 carrier_idx;
	/*  Carrier index of the peak*/
}
srch_pilot_peak_struct_type;

/*--------------------------------------------------------------------------
          Structure for Serving Cell Search Operation Parameters
--------------------------------------------------------------------------*/

typedef struct
{
  boolean  map_info_included;
    /* Flag indicating if mapping functionality is needed.
       FALSE indicates no further mapping functionality is needed, or using
       implicit mapping: Q_map = Q_meas_LEV. See section 7 of TS25.304.
     */
  l1_mapping_info_struct_type  meas_map_info;
    /* Measurement mapping information. It is used when the above flag is
       TRUE
     */

  l1_meas_quan_enum_type   quan_type;
    /* Measurement quantity -- Ec/Io or RSCP. Note that, for Cell Selection
       and Reselection, we only support Ec/Io Measurement Quantity on FDD cells,
       as required by TS25.304.
     */

  boolean thresh_intra_f_incl;
  int16   thresh_intra_f;
    /* Intra-frequency measurement threshold. Value (in dB) ranges (-32..20)
       by step of 2. 0xFFFF indicates the parameter is not applicable.
     */

  boolean thresh_inter_f_incl;
  int16   thresh_inter_f;
    /* Inter-frequency measurement threshold. Value (in dB) ranges (-32..20)
       by step of 2. 0xFFFF indicates the parameter is not applicable.
     */

  boolean thresh_interrat_incl;
    /* If Sinterrat,m not included, do measurements always */

  uint16  num_rats;
    /* Number of additional RATs included */
  l1_interrat_resel_info_type   inter_s_info[L1_MAX_CELL_MEAS];
    /* Inter-RAT Reselection Information */

  uint16   hyst1;
    /* Hysteresis value of the serving cell for RSCP (?), It is used for
       calculating cell-ranking R in HCS. Value ranges (0..40) by step of 2.
     */
  uint16   hyst2;
    /* Hysteresis value of the serving cell for Ec/Io (?), It is used for
       calculating cell-ranking R in HCS. Value ranges (0..40) by step of 2.
       Default value is qhyst1_s.
     */

  uint16   t_resel_frames;
    /* Cell reselection timer, in 10ms unit */

  int16   max_tx_pwr;
    /* Maximum allowed UL TX power, UE_TXPER_MAX_RACH (dBm) */
  int16   q_qualmin;
    /* Minimum required quality level for Ec/Io measurement (in dB).
       Value ranges (-20..0)
     */
  int16   q_rxlevmin;
    /* Minimum required received level for RSCP measurement (in dBm).
       Value ranges (-115..-25) by step of 2.
     */
  boolean hcs_used;
    /*Flag indicating if HCS is enabled on the current cell or not */

  boolean hcs_info_incl;
    /* Flag indicating if Serving cell HCS information included */
  l1_serv_cell_hcs_info_struct_type hcs_info;
    /* Serving cell HCS information */

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
  boolean prior_used;
    /*Flag indicating if priority information is provided by network or not */

  //TO DO : Not needed, clean it up
  //  boolean prior_info_incl;
    /* Flag indicating if Serving cell Priority information included */

  l1_serving_cell_priority_info_type prior_info;
    /* Serving cell Priority information */
#endif

  boolean thresh_hcs_incl;
  int16   thresh_hcs;
    /* HCS measurement threshold. Value (in dB) ranges (-105..91)
       by step of 2. 0xFFFF indicates the parameter is not applicable.
     */
    
    /* Part of REL5 CR-136 R1*/

    /* This specifies the maximum number of cell reselections in 
       case of non-HCS usage.
    */
  uint16 nhcs_n_cr;
    /* This specifies the duration for evaluating allowed amount of 
       cell reselection(s) in case of non-HCS usage.
    */
  l1_t_cr_max_enum_type nhcs_t_crmax;
    /* This specifies the additional time period before the UE can 
       revert to low-mobility measurements in case of non-HCS usage.
    */
  l1_t_cr_max_hyst_enum_type nhcs_t_crmaxhyst;

    /* Speed dependent ScalingFactor for Treselection */
  uint16 tresel_scale;

    /* Inter-frequency ScalingFactor for Treselection */ 
  uint16 tresel_interf_scale;

    /* GSM ScalingFactor for Treselection */
  uint16 tresel_gsm_scale;

    /* LTE Scaling factor for Treselection*/
  uint16 tresel_lte_scale;

  /* New algorithm to support intra-f detected cell reselections */
  /* Variables to store the max values from INTRA NCL cell list */
  int16  intra_ngbr_max_tx_pwr;
  int16  intra_ngbr_max_q_qualmin;
  int16  intra_ngbr_max_q_rxlevmin;
  int16  intra_ngbr_max_q_offset1;
  int16  intra_ngbr_max_q_offset2;
  
  #ifdef FEATURE_FEMTO_CSG
  /* Store the type of currently camped cell (TRUE: FEMTO, FALSE: MACRO) */
  boolean is_camped_cell_femto;
  boolean is_manual_csg_mode;
  #endif
}
srch_aset_parms_struct_type;


/*-----------------------------------------------------------------------*
 *       Non-cell-specific Intra-Frequency measurement Parameters        *
 *-----------------------------------------------------------------------*/
#define SRCH_MAX_INTRAF_MEAS_ALLOWED 9
#define SRCH_INCLUDE_ALL_CELLS_FOR_MEAS 255
typedef struct 
{
   struct 
   {
     /* This field will indicate if this filtering is currently used or not */
     boolean in_use;
     /* filter coeff value sent in higher layer msg */
     uint16 l3_filt_val;
     /* filter max length obtained from l3_filt_val */
     uint16 aset_max_filt_len;
     uint16 nset_max_filt_len;
     uint16 aset_rxdiv_filt_len;
#ifdef FEATURE_WCDMA_CPC_DRX
     uint16 nset_rxdiv_max_filt_len;
#endif
     /* best aset cell when sorted w.r.t filt eng corresponding to this filtering */
     srch_cell_struct_type * best_aset_ptr[SRCHMEAS_MAX_NUM_CARR];
       
   } filt_db[SRCH_WCDMA_MAX_INTRAF_FILT_SIZE];
  uint16  num_filt_idx;
}  srch_intraf_filt_idx_struct_type;

#define SRCH_DEFAULT_FILTER_IDX 0

/* time to print list search log paket */
#define SRCH_PN_SRCH_LOG_INTVL      100  /* in 10 ms units */
extern srch_intraf_filt_idx_struct_type srch_intraf_filt_idx_s;
extern uint16 srch_intraf_filt_coeff_val ;
extern uint16 srch_intraf_filt_coeff_aset_rxdiv_len;
extern uint16 srch_intraf_filt_coeff_aset_len;
extern uint16 srch_intraf_filt_coeff_nset_len;

extern boolean srchmeas_intra_freq_meas_performed;

typedef struct {
    /* Flag indicating if the following two fields, rach_rpt and rach_rpt_max,
       are included.     */
    boolean  rach_rpt_info_included;
    /* Intra-frequency reporting quantity for RACH reporting as in 10.3.7.42 */
    l1_intra_freq_rach_rpt_quan_struct_type  rach_rpt;
    /* Maximum number of reported cells on RACH as in 10.3.7.68 */
    uint16  rach_rpt_max;
} srch_intraf_rach_rpt_info_struct_type;

extern srch_intraf_rach_rpt_info_struct_type srch_intraf_rach_rpt_info_s;

#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif /* FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING */

typedef struct {
        l1_intra_freq_event_enum_type evt_id ;
        uint16 r;
          /* 0..29 (0..14.5dB) by step of 0.5dB for event 1A and 1B.
             Set to 0xFFFF for all other events.
           */
        uint16 hyst;
          /* Hysteresis. 0..15 (0..7.5dB) by step of 0.5 dB */
        uint16 w;
          /* Weight for even 1A and 1B. 0..20 by step of 0.1. set to 0xFFFF
             for all other events.
           */
        uint16 trig_timer;
          /* Time-to-trigger in 10ms units */

        uint16 rpt_deact_thresh;
          /* Event 1A report deactivation threshold */
        uint16 replace_act_thresh;
          /* Replacement activation threshold for Event 1C */
        int16  thresh;
          /* Threshold used for event 1E and 1F, in dB */

        uint16 num_of_reports ;
        /* How many reports need to be there */

        uint16 reporting_interval ;
        /* The time interval between reports */

        uint16 num_cells_forbidden;
        /* num of cells forbidden for this event */

        uint16 forbidden_cell_scr_code[L1_MAX_CELL_MEAS];
        /* The Prmiary scrambling codes for those cells */

        l1_meas_event_trigger_enum_type trig_cond;
        /*  Event Triggering condition */
        /* Triggering conditions are NSET for 1A & 1C
                             and ASET for 1B for now    */
    } srchmeas_evt_trig_struct_type ;

typedef struct {

    uint16 meas_id;

    l1_meas_report_mode_enum_type rpt_mode;

    /* layer 3 filter_coeff  */
    uint16 layer3_filter_coeff;
    /* filter_idx- Can be 0,1 or 2 */
    uint16 filt_idx;

    boolean meas_quan_incl;
    /* Quantity Measured. Note that, for WCDMA FDD cells, we currently support
         Ec/Io or RSCP Measurement Quantity for DCH Measurement report. */
    l1_meas_quan_enum_type   quan_type;

    struct {
      uint16 periodic_rpt_cnt;

      uint16 ts_cnt;

      uint16 num_reports_req;

      uint16 reporting_interval;
    } periodic_s;

    srchmeas_evt_trig_struct_type  evt_trig_s;
    /* common to both evt trig and periodic meas reporting */
    boolean rpt_cell_status_incl;
    l1_meas_rpt_cell_status_struct_type  rpt_cell_status;

    l1_intra_dch_rpt_quan_struct_type    aset_quan;
      /* Rpt quantities for active set  */
    l1_intra_dch_rpt_quan_struct_type    mset_quan;
      /* Rpt quantities for monitored set  */
    boolean       dset_quan_incl;
    l1_intra_dch_rpt_quan_struct_type    dset_quan;
      /* Rpt quantity for detected set  */

    l1_meas_report_trans_mode_enum_type  rpt_trans_mode;
      /* Measuremnt Report transfer criteria to UTRAN */

    /* If this is event 1D, this points to the best cell */
    srch_cell_struct_type *cTRIGGERED_1D_EVENT;
    uint16 filt_ecio_best_cell ;

    /* For event triggered, maintain a TRIGGERED_EVENT sub structure.
       This is required so that we can report all the cells in here
       every time a cell is reported.
        */
    uint16 num_cells_triggered;
    /* scr code */
    uint16 cells_triggered[L1_MAX_CELL_MEAS];

    uint16 num_cells_for_measurement;
    uint16 cells_for_measurement[L1_MAX_CELL_MEAS];
    boolean stopped;
    /* Indicate if the peridical reporting is active for this measurement*/
    boolean periodic_rpting_active;
    /*periodic timer for this measurement. All cells when reported in periodic evt
    trigger mode ,this timer is used to set the periodic rpting timer for all the
    cells so that in periodic reports all the cells are reported at the same time */
    uint16 periodic_rpting_timer;
} srch_intra_freq_meas_parms_struct_type;

typedef struct
{
  boolean enable;
  uint8 l1m_state;
  uint8 bmask;
  uint8 carr_idx;
  uint8 type_of_search;
  int16 thresh;
  uint16 freq;
  uint16 psc;
} srch_debug_crash_info_struct_type;

typedef struct
{
  boolean enable;
  uint8 attenuate_value;
} srch_debug_attenuate_info_struct_type;


extern srch_intra_freq_meas_parms_struct_type
                      srch_intra_freq_meas_db[SRCHMEAS_MAX_NUM_CARR][SRCH_MAX_INTRAF_MEAS_ALLOWED];
extern uint16 srch_num_intraf_meas_in_progress [SRCHMEAS_MAX_NUM_CARR];

extern boolean srch_meas_db_init;

extern mcalwcdma_srch_pn_result_buffer_type* srch_pn_result_buf;


/* This flag indicates if the inter-freq meas need to be stored across state transitions */
extern boolean srch_interf_store_meas_db;

extern boolean srch_intraf_periodic_rpts_requested[SRCHMEAS_MAX_NUM_CARR] ;
extern boolean srch_intraf_evt_trig_rpts_requested[SRCHMEAS_MAX_NUM_CARR] ;


extern boolean srch_no_inter_freq_meas_as_hho_in_progress;
extern boolean srch_no_inter_rat_meas_as_hho_in_progress ;

typedef struct
{
  boolean activated;
    /* Flag indication if the corresponding event is being activated */
  uint16 meas_id;

  uint16 r;
    /* 0..29 (0..14.5dB) by step of 0.5dB for event 1A and 1B.
       Set to 0xFFFF for all other events.
     */
  uint16 hyst;
    /* Hysteresis. 0..15 (0..7.5dB) by step of 0.5 dB */
  uint16 w;
    /* Weight for even 1A and 1B. 0..20 by step of 0.1. set to 0xFFFF
       for all other events.
     */
  uint16 trig_timer;
    /* Time-to-trigger in 10ms units */

  uint16 rpt_deact_thresh;
    /* Event 1A report deactivation threshold */
  uint16 replace_act_thresh;
    /* Replacement activation threshold for Event 1C */
  int16  thresh;
    /* Threshold used for event 1E and 1F, in dB */

  uint16 num_of_reports ;
  /* How many reports need to be there */

  uint16 reporting_interval ;
  /* The time interval between reports */

  uint16 num_cells_forbidden;
  /* num of cells forbidden for this event */

  uint16 forbidden_cell_scr_code[L1_MAX_CELL_MEAS];
  /* The Prmiary scrambling codes for those cells */

  l1_meas_event_trigger_enum_type trig_cond;
  /*  Event Triggering condition */
  /* Triggering conditions are NSET for 1A & 1C
                       and ASET for 1B for now    */

 boolean rpt_cell_status_incl;
 l1_meas_rpt_cell_status_struct_type  rpt_cell_status;

}
srchdch_evt_crit_struct_type;

extern srchdch_evt_crit_struct_type srchdch_evt_crit[SRCHDCH_MAX_INTRAF_EVT];

extern srch_sfn_decode_client_enum_type srch_sfn_decode_client[SRCHMEAS_MAX_NUM_CARR];

/* keep a snapshot of the time when the LPQ srch was issued*/
extern uint16 srchdch_ts_cnt_at_lpq_issue;

/* flag to indicate when to exclude USET cells from LPQ srch */
extern boolean srchdch_exclude_uset_frm_lpq_srch;
/*--------------------------------------------------------------------------
               Define srch_cell_resel_status_enum_type
--------------------------------------------------------------------------*/
typedef l1_cell_resel_status_enum_type srch_cell_resel_status_enum_type;

/*===========================================================================

        Exported variables and functions from file srch_evt.c

===========================================================================*/

extern srch_ops_ctl_parm_struct_type  srch_ops_ctl_parms;

extern boolean srch_allow_hb_to_schedule_searches;

extern srch_ops_sanity_chk_struct_type  srch_ops_sanity_chk;

#define SRCH_OPS_S1_NASTT_PN_Q_ENABLED() (srch_ops_sanity_chk.s1_nastt_pn_q_enabled[SRCH_S1_NASTT_Q] || \
                                                                                    srch_ops_sanity_chk.s1_nastt_pn_q_enabled[SRCH_PN_Q] )

extern uint16 srch_s1_nastt_pn_sanity_timer ;
  /* Initilized with a big number and it should never be 0 */
extern uint16 srch_otdoa_sanity_timer ;
  /* Initilized with a big number and it should never be 0 */

extern void srch_evt_ctl_reset(void);

extern srch_cell_struct_type *srchdch_unsorted_rpt_set_ptr[SRCHDCH_CELL_MEAS_RES_MAX];
extern srch_cell_struct_type *srchdch_sorted_rpt_set_ptr[SRCHDCH_CELL_MEAS_RES_MAX];
/*===========================================================================

        Exported variables and functions from file srch.c

===========================================================================*/

extern srch_state_enum_type  search_state;

extern uint16 srch_ts_cnt;

extern uint16  srch_cur_freq;

extern srch_aset_parms_struct_type  srch_aset_parms;

/* Saved CPHY_CELL_PARM_REQ, used during SIB modification update */
extern l1_cell_parm_cmd_type srch_saved_cell_parms_cmd;

/* Flag to indicate cell parm update due to SIB modification */
extern boolean srch_sib_mod_cell_parm_update;

/* Saved CPHY_MEASUREMENT_REQ, used during SIB modification update */
extern l1_meas_cmd_type srch_saved_meas_req_cmd;
extern l1_meas_cmd_type srch_saved_inter_f_meas_req_cmd;
#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
extern l1_meas_cmd_type srch_saved_prior_lte_meas_req_cmd;
#endif

/* Flag to indicate cell parm update due to SIB modification */
extern boolean srch_sib_mod_meas_req_update;
extern boolean srch_sib_mod_inter_f_meas_req_update;
#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
extern boolean srch_sib_mod_prior_lte_meas_req_update;
#endif

#ifdef FEATURE_FEMTO_CSG
extern boolean srch_sib_mod_csg_meas_req_update;
l1_meas_cmd_type srch_saved_csg_meas_req_cmd;
#endif

extern srch_slot_peak_struct_type  srch_psc_peaks[SRCH_MAX_PSC_PEAKS];
extern uint16  srch_step1_peak_cnt;

extern srch_pilot_peak_struct_type srch_nastt_pilots[96];
extern uint16  srch_nastt_pilot_cnt;

/* This is the array of interf cell pointers which are sent through regular PN search
by tuning the RF to the target frequency
*/
//extern srch_interf_cell_struct_type  *srch_pn_srch_interf_cell_ptr[SRCH_NORMAL_MODE_MAX_PN_TASKS];

  /* number of peaks configured per task for the PN srch.
      for pd=normal --->  6 peaks (non RxD,combined)  12 peaks (RxD separate)
      for pd=shoulder --->  6*3 peaks (non RxD,combined)  12*3 peaks (RxD separate)    
  */
extern uint16 srch_pn_peaks_per_task;

/*********
Enum to define different binning mechanisms
******/

extern srch_freq_hypo_db_struct_type srchacq_freq_offset_db;
extern mcalwcdma_srch_common_async_parms_struct_type srch_common_parms;


/* Peak Detector Mode */
typedef enum {
  SRCH_PD_NORMAL,
    /* "0x00": Peak Detection enabled. */
  SRCH_PD_BYPASS,
    /* "0x01": Peak Detection Disabled. */
  SRCH_PD_SHOULDER,
    /* "0x02": used for DRX mode search. */
  SRCH_NUM_PD_MODE
} srch_pd_enum_type;

typedef enum
{
  SRCH_RXAGC_VAL_DB10,
  SRCH_RXAGC_VAL_DBM,
  /* Old_code 
  SRCH_RXAGC_DIV_VAL_DBM,
  */
  SRCH_RXAGC_VAL_FOR_RSCP,
  SRCH_RXAGC_VAL_FOR_ROUNDED_RSCP
}srch_rxagc_val_units_enum;

typedef struct
{
  uint16 num_peaks;
  uint16 rxd_mode;
  uint16 max_tasks;
}srch_pn_misc_params_struct_type;

extern const srch_pn_misc_params_struct_type 
          srch_pn_misc_params_struct[MCALWCDMA_SRCH_PD_NUM_TYPES][MCALWCDMA_SRCH_NUM_RXD_MODE-1];


typedef enum
{
  SRCH_USE_PRIMARY,
  SRCH_USE_DIVERSITY,
  SRCH_USE_BETTER_ONE,
  SRCH_USE_AVG_PRI_DIV,
  SRCH_USE_COMB_PRI_DIV
}srch_tot_eng_for_meas;

/* Enum to hold the status of the TxD */
typedef enum
{
  CELL_TXD_OFF,
  CELL_TXD_ON,
  CELL_TXD_UNKNOWN
} srch_cell_txd_status;

/* THis structure hold different information during a aset update procedure */
typedef struct
{
  uint16 new_freq;
  uint16 old_freq;
  uint16 prev_ref_cell_psc;
}srch_aset_upd_params_struct_type;

extern wl1_srch_e1d_enhancement_vars_struct_type 
		  wl1_srch_e1d_vars;

extern srch_aset_upd_params_struct_type srchset_aset_upd_db;

extern boolean srch_aset_srch_due;
extern boolean srch_sync_nset_srch_due;
extern boolean srch_fing_triage_due;
extern boolean srch_step1_srch_due;
extern boolean srch_nastt_srch_due;
extern boolean srch_all_cell_srch_due;

extern int16   srch_async_step1_cfn;

extern uint16 srch_wcdma_filt_rscp;
extern uint16 srch_wcdma_filt_rscp_filt_len;

extern srch_cell_struct_type *srch_camp_cell_ptr;

extern boolean srch_dmod_set_changed;
extern boolean srch_nbr_sfn_read_pending[SRCHMEAS_MAX_NUM_CARR];
extern boolean srch_sfn_read_pending_for_interf_timing_maint;

#ifdef FEATURE_WCDMA_FULL_SRCH_IN_DC
extern uint8 intraf_dc_step1_cfn, intraf_dc_nastt_cfn;
#endif

/* Nc=2048, Nn=1 PN, Nc=2048, Nn=2 PN*/
extern uint32 *srch_pn_ecio_tab_ptr ; 

/* Nc=2048, Nn=2 NASTT*/
extern uint32 *srch_nastt_ecio_tab_ptr ; 

/* STTD-NASTT/PN used during ACQ Nc=128 , Nn=36 */
extern uint32 *srch_nastt_pn_sttd_ecio_tab_ptr_nc128; 
/*NON STTD-NASTT/PN used during ACQ Nc=128 , Nn=36 */
extern uint32 *srch_nastt_pn_nonsttd_ecio_tab_ptr_nc128; 
/* step1 Nc=256 Nn=45*/
extern uint32 *srch_step1_ecio_tab_ptr; 
/*step1 Nc=256  Nn=20*/
extern uint32 *srch_step1_nn_20_ecio_tab_ptr; 
/*step1 Nc=256  Nn=30*/
extern uint32 *srch_step1_nn_30_ecio_tab_ptr; 
/*step1 Nc=256  Nn=30 with rxd*/
extern uint32 *srch_step1_nn_30_ecio_rxd_tab_ptr; 
/*step1 Nc=256  Nn=40*/
extern uint32 *srch_step1_nn_40_ecio_tab_ptr; 

/* step1 rxd-add Nc=256 Nn=45*/
extern uint32 *srch_step1_ecio_rxd_tab_ptr; 

/* Non STTD PN search with Nc = 256 , Nn = 16 */
extern uint32 *srch_nastt_pn_nonsttd_ecio_tab_ptr_nc256; 

extern uint32 *srchcr_list_srch_ecio_tab_ptr;
extern uint32 *srchcr_sttd_list_srch_ecio_tab_ptr;

/* A flag to determine when the a(n)set_update log packet is
   to be logged. This is needed because when a(n)set is updated, the ref
   cell may not be chosen immediately in all cases. This flag will
   determine when the packet needs to be logged */
extern boolean log_aset_update_packet ;
extern boolean log_nset_update_packet ;
extern uint16 srch_rscp_in_sleep_state;

#ifdef FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
#error code not present
#endif

#if defined(FEATURE_UMTS_REPORT_ECIO) || defined(FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY)
/* Ec/Io value for UI bars display */
extern uint16 srch_ecio_ui_display;
#endif

/* Pathloss value for UI bars display */
extern uint16 srch_pathloss_ui_display;


#ifdef FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE
extern int16 srch_div_ant_grp_delay_adj;
#endif /* FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE */


#ifdef FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE
extern uint8 ant_id;
#endif

/*===========================================================================

        Exported variables and functions from file srchacq.c

===========================================================================*/
extern boolean srchacq_inter_freq_acq_list_search;

#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2
extern boolean srchacq_acq_rpt_sent_to_RRC;
#endif

/* Global used to set default TD state */
extern boolean wl1_default_td_status;

extern boolean srchdch_rxdiv_state;

extern int32 srchacq_cur_freq_offset;

extern int16 srch_to_fing_freq_offset;

extern rex_crit_sect_type srchzz_parallel_op_mutex_lock;


#define SRCHZZ_PRLL_OP_MUTEX_LOCK() REX_ISR_LOCK(&srchzz_parallel_op_mutex_lock)
#define SRCHZZ_PRLL_OP_MUTEX_UNLOCK() REX_ISR_UNLOCK(&srchzz_parallel_op_mutex_lock)

/*--------------------------------------------------------------------------
       Macro to convert scrambling code to primary scrambling code
--------------------------------------------------------------------------*/
#define SCR_CODE_TO_PRI_SCR_CODE(scr) ((scr) >> 4)
#define SCR_CODE_TO_SEC_SCR_CODE(scr) ((scr) % 16)

#define PRI_SCR_CODE_TO_SCR_CODE(pri_scr) ((uint16)((pri_scr) << 4))
#define SCR_CODE(pri, sec) (((pri) << 4) + (sec))

#define SRCH_GET_PN_SRCHED_CELL(idx) (srch_pn_srch_cell_ptr[idx])
#define SRCH_GET_PN_SRCH_CELL_CNT() (srch_pn_srch_cell_cnt)

#define SRCH_GET_BETTER_OF_PRIM_DIV_ENG(cell_ptr)                                    (   \
                                     (cell_ptr->srch_result_valid[SRCH_ANT_SEL_DIV]) ?              \
                                     ((cell_ptr->max_eng[0] > cell_ptr->max_eng_div[0]) ?         \
                                       cell_ptr->max_eng[0] : cell_ptr->max_eng_div[0]) :            \
                                       cell_ptr->max_eng[0]                                                    \
                                       )

#define SRCH_GET_BETTER_ENG_POS_FROM_PRIM_DIV(cell_ptr)                                    (   \
                                     (cell_ptr->srch_result_valid[SRCH_ANT_SEL_DIV]) ?              \
                                     ((cell_ptr->max_eng[0] > cell_ptr->max_eng_div[0]) ?         \
                                       cell_ptr->max_pos[0] : cell_ptr->max_pos_div[0]) :            \
                                       cell_ptr->max_pos[0]                                                    \
                                       )

/* Use these Macros for the cases where Nc=2048 and Nn=1/2 for PN and NASTT srch  Nc=2048 and Nn=2resp. */
#define SRCH_GET_PN_ENG(idx)                      (srch_pn_ecio_tab_ptr[idx])
#define SRCH_GET_NASTT_ENG(idx)                   (srch_nastt_ecio_tab_ptr[idx])

/* Use these Macros for the cases where Nc = 256 and Nn=16 for PN srches */
#define SRCH_GET_PN_ENG_256_16_SRCH(idx)          (srch_nastt_pn_nonsttd_ecio_tab_ptr_nc256[idx])

/* during initial acqusition when we use Nc=128 and Nn=36 for NASTT/PN searches,
  use the following macros to get the energy */
#define SRCH_GET_NASTT_ACQ_ENG(idx,sttd)          ((sttd) ? srch_nastt_pn_sttd_ecio_tab_ptr_nc128[idx] :      \
                                                         srch_nastt_pn_nonsttd_ecio_tab_ptr_nc128[idx])

#define SRCH_GET_PN_ACQ_ENG(idx,sttd)             ((sttd) ? srch_nastt_pn_sttd_ecio_tab_ptr_nc128[idx] :      \
                                                         srch_nastt_pn_nonsttd_ecio_tab_ptr_nc128[idx])
                                                          
#define SRCH_GET_STEP1_ENG(idx)                   ((srchdch_rxdiv_state) ? srch_step1_ecio_rxd_tab_ptr[idx] : \
                                                         srch_step1_ecio_tab_ptr[idx])
#define SRCH_GET_STEP1_NN_20_ENG(idx)             (srch_step1_nn_20_ecio_tab_ptr[idx])
#define SRCH_GET_STEP1_NN_40_ENG(idx)             (srch_step1_nn_40_ecio_tab_ptr[idx])
#define SRCH_GET_STEP1_NN_30_ENG(idx)             ((srchdch_rxdiv_state) ? srch_step1_nn_30_srch_rxd_ecio_tab[idx] : \
                                                                                    srch_step1_nn_30_srch_ecio_tab[idx])
#ifdef FEATURE_WCDMA_STEP1_PEAK_PRUNER
#define SRCH_GET_STEP1_ENG_CONNECTED_MODE(idx)     (IS_SRCH_CPC_DRX_ACTIVE() ? SRCH_GET_STEP1_NN_30_ENG(idx) : SRCH_GET_STEP1_ENG(idx))
#endif

#define SRCH_NONACQ_NASTT_PILOT_DET_THRESH_ENG()  SRCH_GET_NASTT_ENG(SRCH_NONACQ_NASTT_NO_RXD_THRESH_IDX)

#define SRCH_GET_ENG_FRM_ECIO_LUT(sttd,idx)       SRCH_GET_PN_ENG(idx)
#define SRCH_CONVERT_ENG_TO_ECIO(eng,sttd)        srch_eng_to_ecio(eng, srch_pn_ecio_tab_ptr)

#define SRCH_CONVERT_ENG_TO_ECIO_BPLMN_STTD(eng)  srch_eng_to_ecio(eng, srch_nastt_pn_sttd_ecio_tab_ptr_nc128)
#define SRCH_CONVERT_ENG_TO_ECIO_BPLMN_NON_STTD(eng)  srch_eng_to_ecio(eng, srch_nastt_pn_nonsttd_ecio_tab_ptr_nc128)



#define SRCH_CONVERT_ENG_TO_ECIO_STEP1_DCH_NON_CPC(eng)     (                                                                                \
              (srchdch_rxdiv_state) ? srch_eng_to_ecio(eng, srch_step1_ecio_rxd_tab_ptr) : srch_eng_to_ecio(eng, srch_step1_ecio_tab_ptr))

#define SRCH_CONVERT_ECIO_TO_ENG(ecio,sttd)       ((ecio >= ECIO_TAB_SIZ) ? SRCH_GET_PN_ENG(ECIO_TAB_SIZ-1): SRCH_GET_PN_ENG(ecio))

#define SRCH_CONVERT_ACQ_ENG_TO_ECIO(eng,sttd)        srch_eng_to_ecio(eng, srch_nastt_pn_sttd_ecio_tab_ptr_nc128) 

/* GTW Full window ACQ search threshold, no TD @ -21.5dB */
#define SRCH_GTW_ACQ_FULL_WINDOW_THRESH_ENG()     SRCH_GET_PN_ENG_256_16_SRCH(SRCH_GTW_ACQ_FULL_WINDOW_THRESH_IDX)

/* GTW Full window ACQ search threshold, no TD @ -23.5dB */
#define SRCH_GTW_ACQ_LIMITED_WINDOW_THRESH_ENG()  SRCH_GET_PN_ACQ_ENG(SRCH_GTW_ACQ_LIMITED_WINDOW_THRESH_IDX, FALSE)

/* In offline searcher both interf and intraf PN srch uses Nc=2048 and Nn=1 */
#define SRCH_CONVERT_INTERF_ENG_TO_ECIO(eng,sttd) SRCH_CONVERT_ENG_TO_ECIO(eng,sttd)

/* Use these macros for different thresholds when the PN/NAS TT srch uses Nc=2048 and Nn=1/2 resp.
These are during Non ACQ cases and some of the ACQ cases when we dont have any freq error in the system
*/

#define SRCH_WEAK_NBR_THRESH_ENG(sttd)            SRCH_GET_ENG_FRM_ECIO_LUT(sttd,SRCH_WEAK_NBR_THRESH_IDX)
#define SRCH_EVT_EVAL_THRESH_ENG(sttd)            SRCH_GET_ENG_FRM_ECIO_LUT(sttd,SRCH_EVT_EVAL_THRESH_IDX)
#define SRCH_USET_PATH_THRESH_ENG(sttd)           SRCH_GET_ENG_FRM_ECIO_LUT(sttd,SRCH_USET_PATH_THRESH_IDX)
#define SRCH_INTERFERING_PILOTS_THRESH_ENG(sttd)  SRCH_GET_ENG_FRM_ECIO_LUT(sttd,30)
#define SRCHCR_PN_SRCH_THRESH_ENG(sttd)           SRCH_GET_ENG_FRM_ECIO_LUT(sttd,SRCHCR_PN_SRCH_ENG_THRESH_IDX)

#define SRCHDCH_NBR_EVAL_THRESH_ENG(sttd)         SRCH_GET_ENG_FRM_ECIO_LUT(sttd,SRCHDCH_NBR_EVAL_THRESH_IDX)
#define SRCHDCH_NBR_SFN_READ_THRESH_ENG(sttd)     SRCH_GET_ENG_FRM_ECIO_LUT(sttd,SRCH_NBR_SFN_READ_THRESH_IDX)
#define SRCHDCH_NBR_RPT_THRESH_ENG(sttd)          SRCH_GET_ENG_FRM_ECIO_LUT(sttd,SRCHDCH_NBR_RPT_THRESH_IDX)  

#define SRCH_CELL_PATH_THRESH_ENG(sttd)           ((sttd) ? SRCH_GET_PN_ENG(SRCH_CELL_PATH_TD_THRESH_IDX) : \
                                                            SRCH_GET_PN_ENG(SRCH_CELL_PATH_NO_TD_THRESH_IDX))

#define SRCH_CELL_QXT_PATH_THRESH_ENG(sttd)       ((sttd) ? SRCH_GET_PN_ENG(SRCH_CELL_QXT_PATH_TD_THRESH_IDX) : \
                                                            SRCH_GET_PN_ENG(SRCH_CELL_QXT_PATH_NO_TD_THRESH_IDX))


#define SRCH_FULL_WINDOW_HHO_THRESH_ENG(sttd)     ((sttd) ? SRCH_GET_PN_ENG(SRCH_FULL_WINDOW_TD_THRESH_IDX) : \
                                                            SRCH_GET_PN_ENG(SRCH_FULL_WINDOW_NO_TD_THRESH_IDX))

#define SRCH_LIMITED_WINDOW_HHO_THRESH_ENG(sttd)  SRCH_GET_PN_ACQ_ENG(SRCH_HHO_LIMITED_WINDOW_THRESH_IDX, sttd)

#define SRCH_UNMON_NASTT_THRESH_ENG()             ((srchdch_rxdiv_state) ? SRCH_GET_NASTT_ENG(SRCH_UNMON_NASTT_RXD_THRESH_IDX): \
                                                                           SRCH_GET_NASTT_ENG(SRCH_UNMON_NASTT_NO_RXD_THRESH_IDX))

#define SRCH_ASYNC_NASTT_THRESH_ENG()             ((srchdch_rxdiv_state) ? SRCH_GET_NASTT_ENG(SRCH_ASYNC_NASTT_RXD_THRESH_IDX): \
                                                                           SRCH_GET_NASTT_ENG(SRCH_ASYNC_NASTT_NO_RXD_THRESH_IDX))
#ifdef FEATURE_WCDMA_DC_HSDPA
#define SRCH_DC_CARR_NASTT_THRESH_ENG()           SRCH_GET_NASTT_ENG(SRCH_DC_NASTT_THRESH_IDX)
#define SRCH_3C_CARR_NASTT_THRESH_ENG()           SRCH_GET_NASTT_ENG(SRCH_3C_NASTT_THRESH_IDX)
#endif
     
/* Use these macros when we use Nc=128 and Nn=36 for the NASTT/PN searches during ACQ when there is a possibility of 
freq error in the system
*/
#define SRCH_ACQ_NASTT_PILOT_DET_THRESH_ENG(sttd)  SRCH_GET_NASTT_ACQ_ENG(SRCH_ACQ_NASTT_ENG_THRESH_IDX,sttd)

/* For GTW ACQ, for a pullin following set a full window search, set the Pullin thresh from the (256x16) table */
#define SRCH_GTW_ACQ_PULLIN_PATH_THRESH_ENG()      SRCH_GET_PN_ENG_256_16_SRCH(ACQ_PULLIN_PATH_THRESH_IDX)

/* For interfreq redirection, for a pullin following set a full window search, set the Pullin thresh from the (256x16) table */
#define SRCH_INTERF_REDIR_PULLIN_PATH_THRESH_ENG() SRCH_GET_PN_ENG_256_16_SRCH(ACQ_PULLIN_PATH_THRESH_IDX)

/* For ACQ, set the Pullin thresh from the (128x36) table */
#define SRCH_ACQ_PULLIN_PATH_THRESH_ENG(sttd)      SRCH_GET_PN_ACQ_ENG(ACQ_PULLIN_PATH_THRESH_IDX, sttd)

/* For X2W resume ACQ which is done after a LTA GAP, returns the reduced pullin threshold */
#define SRCH_ACQ_PULLIN_THRESH_FOR_X2W_RESUME_LTA(sttd) (SRCH_GET_PN_ACQ_ENG(SRCHACQ_PN_PULLIN_THRESH_IDX_FOR_LTA, sttd))

/* For ACQ stage in BPLMN, set the NASTT thresh from the (128x36) table */
#define SRCHBPLMN_NASTT_PILOT_DET_THRESH_ENG(sttd)  SRCH_GET_NASTT_ACQ_ENG(SRCH_NONACQ_NASTT_NO_RXD_THRESH_IDX,sttd)

/* For ACQ stage in BPLMN, set the PN thresh from the (128x36) table */
#define SRCHBPLMN_PN_PATH_DET_THRESH_ENG(sttd)  SRCH_GET_PN_ACQ_ENG(SRCH_NONACQ_NASTT_NO_RXD_THRESH_IDX,sttd)


/* For ACQ stage in BPLMN, set the pullin thresh from the (128x36) table */
#define SRCHBPLMN_ACQ_PULLIN_PATH_THRESH_ENG(sttd)  SRCH_GET_PN_ACQ_ENG(SRCHBPLMN_PULLIN_PATH_THRESH_IDX, sttd)

/* For the non-ACQ state in BPLMN(ie.setting up BCH to read SIB's), set pullin thresh from the (2048x1) table */
#define SRCHBPLMN_NON_ACQ_PULLIN_PATH_THRESH_ENG(sttd)  SRCH_GET_ENG_FRM_ECIO_LUT(sttd, SRCHBPLMN_PULLIN_PATH_THRESH_IDX)

/* For ACQ, set the Pullin thresh from the (2048x1,2) table */
#define SRCH_ACQ_PULLIN_PN_DCH_TO_IDLE_THRESH_ENG()      SRCH_GET_PN_ENG(ACQ_PULLIN_PATH_THRESH_IDX)

#define SRCH_REACQ_SRCH_THRESH_ENG(sttd)              ((sttd) ? SRCH_GET_PN_ENG(SRCH_REACQ_TD_THRESH_IDX) : \
                                                                SRCH_GET_PN_ENG(SRCH_REACQ_NO_TD_THRESH_IDX))

#define SRCH_IS_ASET_UPDATE_SRCH()   (srch_in_asu)

#define IS_SRCHDCH_RXDIV_ENABLED()         (srchdch_rxdiv_state)
#define SET_SRCHDCH_RXDIV_STATE(new_state) {srchdch_rxdiv_state = new_state;}


extern uint16 srchmeas_get_meas_idx_based_on_evt_id(
                l1_intra_freq_event_enum_type evt_id);

extern void srchmeas_get_cells_for_measurement_info(uint8 carr_idx,
      l1_intra_freq_cell_list_struct_type *cell_list_ptr, uint16 meas_id);

/* keep track of num EDCH active set cells */
extern uint8 srchdch_edch_aset_cnt[SRCHMEAS_MAX_NUM_CARR];
/* Critical Section variables/calls used to protect intra freq meas database */
extern rex_crit_sect_type  srch_intra_freq_db_crit_sect;
#define SRCHMEAS_INTRA_FREQ_DB_INTLOCK()  REX_ISR_LOCK(&srch_intra_freq_db_crit_sect)
#define SRCHMEAS_INTRA_FREQ_DB_INTFREE()  REX_ISR_UNLOCK(&srch_intra_freq_db_crit_sect)

/* Critical Section variables/calls used to protect inter freq meas database */
extern rex_crit_sect_type  srch_inter_freq_db_crit_sect;
#define SRCHMEAS_INTER_FREQ_DB_INTLOCK()  REX_ISR_LOCK(&srch_inter_freq_db_crit_sect)
#define SRCHMEAS_INTER_FREQ_DB_INTFREE()  REX_ISR_UNLOCK(&srch_inter_freq_db_crit_sect)

/* Critical Section variables/calls used to protect freq scan substate */
extern rex_crit_sect_type  srchfs_scan_substate_crit_sect;
#define SRCHFS_SUBSTATE_INTLOCK()  REX_ISR_LOCK(&srchfs_scan_substate_crit_sect)
#define SRCHFS_SUBSTATE_INTFREE()  REX_ISR_UNLOCK(&srchfs_scan_substate_crit_sect)

/* Critical Section variables/calls used to protect internal  meas database */
extern rex_crit_sect_type  srch_internal_meas_db_crit_sect;
#define SRCH_INTERNAL_MEAS_DB_INTLOCK()  REX_ISR_LOCK(&srch_internal_meas_db_crit_sect)
#define SRCH_INTERNAL_MEAS_DB_INTFREE()  REX_ISR_UNLOCK(&srch_internal_meas_db_crit_sect)


/* Critical section used to arbitrate concurrent access to search module timestamp counter srch_ts_cnt */
extern rex_crit_sect_type srch_ts_cnt_crit_sect;

#define SRCHTSCNT_INTLOCK()  REX_ISR_LOCK(&srch_ts_cnt_crit_sect)
#define SRCHTSCNT_INTFREE()  REX_ISR_UNLOCK(&srch_ts_cnt_crit_sect)

/* Critical section used to arbitrate concurrent access to srch_ops_sanity_chk */
extern rex_crit_sect_type           srch_ops_sanity_chk_crit_sect;

#define SRCH_OPS_SANITY_CHK_LOCK()  REX_ISR_LOCK(&srch_ops_sanity_chk_crit_sect)
#define SRCH_OPS_SANITY_CHK_FREE()  REX_ISR_UNLOCK(&srch_ops_sanity_chk_crit_sect)

#ifdef FEATURE_WCDMA_CPC_DRX
/* critical section to protect search mode while in CPC DRX */
extern rex_crit_sect_type           srchdch_cpc_srch_mode_chk_crit_sect;

#define SRCHDCH_CPC_DRX_SRCH_MODE_LOCK()  REX_ISR_LOCK(&srchdch_cpc_srch_mode_chk_crit_sect)
#define SRCHDCH_CPC_DRX_SRCH_MODE_FREE()  REX_ISR_UNLOCK(&srchdch_cpc_srch_mode_chk_crit_sect)
#endif

/* Critical section used to arbitrate concurrent access to tbar timer elapsed count */
extern rex_crit_sect_type srch_tbar_elapsed_cnt_crit_sect;

#define SRCHTBAR_INTLOCK()  REX_ISR_LOCK(&srch_tbar_elapsed_cnt_crit_sect)
#define SRCHTBAR_INTFREE()  REX_ISR_UNLOCK(&srch_tbar_elapsed_cnt_crit_sect)

/* Critical section used to arbitrate concurrent access to search scheduler in INTRAF mode */
extern rex_crit_sect_type srchsched_intraf_searches_crit_sect;
/* Critical section used to arbitrate concurrent access to searcher heartbeat counters in INTRAF mode */
extern rex_crit_sect_type srchsched_intraf_counter_crit_sect;

#define SRCHSCHED_INTRAF_INTLOCK()           \
  do {   \
    REX_ISR_LOCK(&srchsched_intraf_searches_crit_sect); \
  } while (0)

#define SRCHSCHED_INTRAF_INTFREE()           \
  do {   \
    REX_ISR_UNLOCK(&srchsched_intraf_searches_crit_sect); \
  } while (0)

#define SRCHSCHED_CNTR_INTLOCK()           \
  do {   \
    REX_ISR_LOCK(&srchsched_intraf_counter_crit_sect); \
  } while (0)

#define SRCHSCHED_CNTR_INTFREE()           \
  do {   \
    REX_ISR_UNLOCK(&srchsched_intraf_counter_crit_sect); \
  } while (0)

/*===========================================================================

FUNCTION SRCH_OTA_MEAS_PARM_INIT

DESCRIPTION
  This function initialize non-cell-specific OTA measuement parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void srch_ota_meas_parm_init( void );

/*===========================================================================

FUNCTION SRCH_ENABLE_OPS_SANITY_CHK

DESCRIPTION
  This function enables search operation sanity check (after a MDSP search
  comand).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_enable_ops_sanity_chk
(
  srch_queue_enum_type srch_q,
  srch_step_enum_type srch_step,
  srch_intraf_queue_enum_type srch_queue_idx
);

/*===========================================================================

FUNCTION SRCH_RESET_OPS_SANITY_CHK

DESCRIPTION
  This function resets the search operation sanity check after a search has been
  completed or aborted.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_reset_ops_sanity_chk
(
  srch_queue_enum_type srch_q,
  srch_intraf_queue_enum_type srch_queue_idx
);

/*===========================================================================

FUNCTION SRCH_RESEND_SRCH_CMD

DESCRIPTION
  This function resends the search command based on the queue
DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srch_resend_srch_cmd ( srch_intraf_queue_enum_type srch_queue_idx );

/*===========================================================================

FUNCTION srch_is_resend_srch_needed

DESCRIPTION:
  This function determines if a search needs to be resent to FW.

DEPENDENCIES

RETURN VALUE
  TRUE:  if resend needed
  FALSE: otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_is_resend_srch_needed(srch_step_enum_type srch_step, srch_ops_code_enum_type srch_type);

/*===========================================================================

FUNCTION SRCH_ABORT_ALL

DESCRIPTION
  This function aborts all on-going searches and discards all SRCH_DONE_CMD
  from L1 local queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchfs_parameters_init(boolean is_list_search );

/*===========================================================================

FUNCTION SRCH_ACQ_PARAMETERS_INIT

DESCRIPTION
  This function sets the frequency scan and acquisition thresholds depending on
  the type of search requested by RRC

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_abort_all( void );

/*===========================================================================

FUNCTION SRCH_ABORT_ALL_BUT_MDSP

DESCRIPTION
  This function aborts all on-going searches and discards all SRCH_DONE_CMD
  from L1 local queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_abort_all_but_mdsp ( void );

/*===========================================================================

FUNCTION SRCH_CHECK_IF_ABORT_PENDING

DESCRIPTION
  This function checks if a search abort is pending or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: if abort pending,
  FALSE: otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srch_check_if_abort_pending( void );

/*=========================================================================

FUNCTION     : SRCH_LOG_ASET_PACKET_V2

DESCRIPTION  : This function logs the aset packet 0x41AC

DEPENDENCIES : None

RETURN VALUE :

SIDE EFFECTS : None

=========================================================================*/
void srch_log_aset_packet_v2(void);

/*=========================================================================

FUNCTION     : SRCH_LOG_NSET_PACKET

DESCRIPTION  : This function logs the nset packet  0x4111

DEPENDENCIES : None

RETURN VALUE :

SIDE EFFECTS : None

=========================================================================*/
void srch_log_nset_packet(void);

/*===========================================================================

FUNCTION SRCH_LIST_WIN_CTR

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
extern void srch_list_win_ctr ( srch_queue_enum_type que );

/*===========================================================================

FUNCTION SRCH_ASET_SRCH_CMD

DESCRIPTION
  This function updates searcher HPQ, if needed, based on aset information and
  issues a search command to DSP.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srch_aset_srch_cmd ( void );

extern boolean srch_aset_srch_cmd_for_asu ( void );

/*===========================================================================

FUNCTION SRCH_ASET_CSET_SRCH_CMD

DESCRIPTION
  This function updates searcher HPQ, if needed, based on aset AND cset
  information and issues a search command to DSP.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_aset_cset_srch_cmd ( void );

extern void srch_aset_cset_srch_cmd_for_asu ( void );

extern void srchdch_notify_aset_upd_failure_to_l1m( void);

extern void srchdch_sort_reporting_cells( uint8 carr_idx,uint16 meas_idx);

/*===========================================================================

FUNCTION SRCH_FILTER_SRCH_RESULT

DESCRIPTION
  This function performs measurement filtering on on cells in the srch queue
  indicated by "que".

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_filter_srch_result( srch_queue_enum_type que );

/*===========================================================================

FUNCTION SRCH_FILTER_S3_RESULT

DESCRIPTION
  This function performs measurement filtering on the passed in cell after
  S3 result

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_filter_s3_result( srch_cell_struct_type *cell_ptr );

/*===========================================================================

FUNCTION SRCH_ENG_TO_ECIO

DESCRIPTION
  This function converts an energy into the 6-bit Ec/Io value required
  for pilot strength measurements. It works by doing a binary search on
  the ecio_tab to find the closest value (without going over), and returning
  the index in the ecio_tab.

DEPENDENCIES
  None.

RETURN VALUE
  6-bits Ec/Io value. If the energy is out of range,  the return value is
  clipped to 0 or 63.

SIDE EFFECTS   None.

===========================================================================*/
extern uint16 srch_eng_to_ecio
(
  uint32 eng,
    /* The energy to be converted */
  const uint32 ecio_tab[]
    /* Pointer to the conversion table to be used */
);

/*===========================================================================

FUNCTION SRCH_ECIO_TO_ENG

DESCRIPTION
  This function converts a 6-bit Ec/Io value into a search energy in MSM5200
  chip. It just performs a table lookup on ecio_tab (clipping values that are
  too large).

DEPENDENCIES
  None.

RETURN VALUE
  The energy.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint32 srch_ecio_to_eng
(
  uint16  ecio,
      /* The Ec/Io value to convert */
  uint32  ecio_tab[]
      /* Pointer to the conversion table to be used */
);

/*===========================================================================

FUNCTION SRCH_CALCULATE_CPICH_RSCP

DESCRIPTION
  This function calculates the CPICH RSCP for the cell pointed by cell_ptr.
  The calculated RSCP value will be directly stored in the cell data structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_calculate_cpich_rscp (srch_cell_struct_type *cell_ptr);

/*===========================================================================

FUNCTION SRCH_EVENT_TIMER_EXP

DESCRIPTION
  This function check if a event has met time-to-trigger criteria.

DEPENDENCIES
  Time-to-trigger value must be less that (65535 x 10)ms.

RETURN VALUE
  TRUE: if Time-to-trigger criteria is met.
  FALSE: otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean srch_event_timer_exp (uint16 start_ts, uint16 time_to_trig);

/*===========================================================================

FUNCTION SRCH_PRINT_DEBUG_SRCH_PARAMS
DESCRIPTION
  This function prints searcher related debug params
  e.g which srch are due what srch is going on etc

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  dont print all these F3s too frequently.

===========================================================================*/
extern void srch_print_debug_srch_params(void);

/*===========================================================================

        Exported variables and functions from file srchacq.c

===========================================================================*/
/*===========================================================================

FUNCTION        SRCHACQ_SET_VCTCXO_PARMS_FLAG

DESCRIPTION
  Used to set srch_reset_vctcxo_param_during_acquisition flag so VCTCXO values
  can be reset next time acquisition is performed.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void srchacq_set_vctcxo_parms_flag( boolean val);

/*===========================================================================

FUNCTION        SRCHACQ_GET_RESET_VCTCXO_PARAM_FLAG

DESCRIPTION
  Returns srch_reset_vctcxo_param_during_acquisition flag to caller.

  TRUE means we will reset the VCTCXO PDM for the next acquisition attempt.
  And this means we have not yet successfully acquired a WCDMA system since
  the WCDMA RAT has been active.

  FALSE means we will not reset the VCTCXO PDM for the next acquisition
  attempt.  And this means that we have, at least once, acquired a WCDMA
  system since the WCDMA RAT has been active.  We then don't reset the PDM
  because it's probably at a decent value since we acquired earlier.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean srchacq_get_reset_vctcxo_param_flag(void);

/*===========================================================================
FUNCTION     SRCHACQ_GET_BIN_TYPE

DESCRIPTION
  During extended OOS, we try different search strategies to regain service.
  This function is called before each acquisition attempt and returns which
  strategy we should try, based on how long we have been OOS.

DEPENDENCIES
  None.

PARAMETERS
  int16 *bin_offset_ppm - Returns bin ppm value in this return pointer,
                          if pointer passed in is not NULL.  Only applies
                          for IGNORE_RGS and IGNORE_RGS_AND_TEMP_TABLE.
RETURN VALUE
  srchacq_bin_type - SRCHACQ_BIN_NORMAL
                     SRCHACQ_BIN_IGNORE_RGS
                     SRCHACQ_BIN_IGNORE_RGS_AND_TEMP_TABLE
SIDE EFFECTS
  None.
===========================================================================*/
extern srchacq_bin_type srchacq_get_bin_type(void);

/*===========================================================================
FUNCTION     SRCHACQ_INIT_VCTCXO_AND_ROTATOR

DESCRIPTION
  This function is invoked at WCDMA activation and before acquisition
  attempts to freeze VCTCXO and optionally set a rotator frequency hypothesis
  to use when attempting to acquire.  In addition, the trk_lo_adj PDM driving
  VCTCXO is initialized to a best estimate based upon what it was in the
  recent past, like when GSM was active for example.

  This entire process is only necessary when WCDMA is first entered.
  Subsequent acquisitions in WCDMA mode leave VCTCXO alone because it is
  probably pretty accurate since it was tracking at some point in the past.

DEPENDENCIES
  None.

PARAMETERS
  read_rot_from_tcxo_mgr - TRUE if we should initialize the rotator frequency
    hypothesis based upon value read from the TCXO MGR.  FASLE otherwise.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchacq_init_vctcxo_and_rotator(boolean read_rot_from_tcxo_mgr);

/*===========================================================================

FUNCTION SRCHACQ_STEP_SRCH_DONE

DESCRIPTION
  This function processes 'SRCH_DONE" local command in SRCHACQ state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchacq_step_srch_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

        Exported variables and functions from file srchdch.c

===========================================================================*/
extern srch_cell_struct_type *srchdch_sort_aset_ptr[SRCHMEAS_MAX_NUM_CARR][SRCH_ASET_MAX_C0];

extern uint8 srch_heartbeat_cfn;

/*===========================================================================

FUNCTION SRCHDCH_PROC_SRCH_DONE

DESCRIPTION
  This function processes 'SRCH_DONE" local command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchdch_proc_srch_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION SRCHDCH_PROC_SRCH_OPS

DESCRIPTION
  This function processes "srch_ops_cmd".

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchdch_proc_srch_ops(srch_ops_cmd_type  *cmd_ptr);

extern uint32 srchdch_calculate_tm(srch_cell_struct_type *cell_ptr);

extern void srchdch_adjust_off_and_tm(srch_cell_struct_type *cell_ptr);

#ifdef FEATURE_WCDMA_CNT_C_SFN_DIFF
extern uint16 srchdch_get_cnt_c_sfn_diff(srch_cell_struct_type *cell_ptr);
#endif

#ifdef FEATURE_REL5
extern void srchdch_send_initial_evt_1d_meas_report ( uint8 carr_idx, uint16 meas_idx );
#endif

extern void srchmeas_reset_intraf_meas_dbase(uint8);
extern uint16 srchmeas_get_oldest_uset_idx(uint8);

extern void srchmeas_initialise_default_filter(void);

#define SRCH_INVALID_FILT_IDX 0xFF
/* return the filter to be used when comparing filt_eng > SFN dec thresh before
   SFN decode in case of multiple filters*/
#define GET_FILT_IDX_FOR_SFN_DEC() (srchdch_filt_idx_for_interf_irat_meas(SRCH_DEFAULT_FILTER_IDX))
/*===========================================================================

        Exported variables and functions from file srchfach.c

===========================================================================*/

extern void srchfach_notify_aset_upd_failure_to_l1m(void);
/*===========================================================================

        Exported variables and functions from file srchzz.c

===========================================================================*/

/*===========================================================================

FUNCTION SRCHZZ_PROC_SRCH_DONE

DESCRIPTION
  This function processes 'SRCH_DONE" local command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchzz_proc_srch_done(srch_done_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION SRCHZZ_PROC_SRCH_OPS

DESCRIPTION
  This function processes "srch_ops_cmd".

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchzz_proc_srch_ops(srch_ops_cmd_type  *cmd_ptr);

extern void srchzz_send_aset_upd_srch_failure_cmd_to_l1m(void);
extern boolean SRCHDCH_IS_IN_CELLS_FOR_MEAS ( void *ptr , uint16 meas_idx);

/*===========================================================================

FUNCTION  SRCHDCH_REMOVE_CELL_FROM_TRIGGERED_CELLS

DESCRIPTION


RETURN VALUE
    void

 ============================================================================*/
extern void srchdch_remove_cell_from_triggered_cells (srch_cell_struct_type *cell_ptr,
                                                                uint16 meas_idx);

/*===========================================================================

FUNCTION  SRCHDCH_REMOVE_FROM_EVT_RPT_CELL_LIST_FOR_MEAS_IDX

DESCRIPTION


RETURN VALUE
    void


 Check if this cell is already waiting to be reported, and if so
   remove it from the list
============================================================================*/
extern void srchdch_remove_from_evt_rpt_cell_list_for_meas_idx(srch_cell_struct_type *cell_ptr ,
                                                             uint16  meas_idx);
/*===========================================================================

FUNCTION SRCHDCH_SFN_CFN_DIFF_UPDATE

DESCRIPTION
  This function processes the SRCH_SFN_CFN_OBSVED_DIFF_CMD .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchidle_sfn_cfn_diff_update
(
  srch_sfn_wrc_diff_cmd_done_type *srch_cmd
);

extern void srchdch_sfn_cfn_diff_update
(
  srch_sfn_wrc_diff_cmd_done_type *srch_cmd
);

extern void srch_sfn_wrc_decode_internal
(
  srch_cell_struct_type *cell_ptr, 
  srch_sfn_decode_client_enum_type client
);

/*===========================================================================

FUNCTION SRCHDCH_SORT_ACTIVE_SET

DESCRIPTION
  This function sorts the active set .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchdch_sort_active_set(uint8, l1_meas_quan_enum_type quan_type,uint16 filt_idx);

/*===========================================================================

FUNCTION SRCHDCH_RESET_EVT_RPT_DBASE

DESCRIPTION
  This function resets the event reporting database .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchdch_reset_evt_rpt_dbase(uint8);

/*===========================================================================

FUNCTION SRCHDCH_REMOVE_FROM_EVT_RPT_CELL_LIST

DESCRIPTION
  This function removes the given cell from the evt_rpt_cell_list.
  This function gets called when this cell's SFN fails to be
  decoded (for some reason).

  The cell will continue to be searched and when

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchdch_remove_from_evt_rpt_cell_list(srch_cell_struct_type *cell_ptr);

/* Prototypes of event evaluation functions */
extern boolean srchdch_evt_1a_1b_eval(srch_cell_struct_type *cell_ptr, uint16 meas_idx);
extern boolean srchdch_evt_1c_eval(srch_cell_struct_type *cell_ptr, uint16 meas_idx);
extern boolean srchdch_evt_1d_eval(srch_cell_struct_type *cell_ptr, uint16 meas_idx);
extern boolean srchdch_evt_1e_1f_eval(srch_cell_struct_type *cell_ptr, uint16 meas_idx);
extern boolean srchdch_evt_1j_eval(srch_cell_struct_type *cell_ptr, uint16 meas_idx);
extern void srchdch_setup_initial_evt_1a_meas_for_aset (void);
extern void srchdch_setup_initial_evt_1e_meas_for_aset (void);

#ifdef FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE
extern void srch_set_div_ant_delay(int16 new_delay);
#endif /* FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE */

extern boolean srch_find_tx_div_ind(uint16 scr_code, uint8 carr_idx);

/*===========================================================================

FUNCTION SRCHDCH_FILT_IDX_FOR_INTERF_IRAT_MEAS

DESCRIPTION: This function will return the filt_idx to be used for aset quality estimate
used in inter freq (2d/2f) and IRAT evt evaluations.
   a) check if the filter configured by inter-freq/IRAT meas also available in intraf filt db. 
       if not go to step 2
   2) check if the default fitler is active. if yes use it else go to 3)
   3) if there are only 1 non default filter, use it.But if there are 2 non default
   filter then use the one which has lower filt_eng

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/
uint16 srchdch_filt_idx_for_interf_irat_meas(uint16);

/*===========================================================================

FUNCTION     SRCHDCH_RESET_FILT_PARAMS_ALL_TIMING_KNOWN_CELLS

DESCRIPTION
  reset the filtered parameters for all the cells for the particular filter
  indicated by filt_idx. This will be done when a new filter is added using MCM
  If we dont reset e.g filt_ecio_hi_res then the filtering process will assume that
  its not a new filter eng and clip it to +/-3dB of the stale filt eng already there

DEPENDENCIES
  None.

PARAMETERS
  
 
RETURN VALUE
  none

SIDE EFFECTS
  i
===========================================================================*/
extern void srchdch_reset_filt_params_all_timing_known_cells(uint16 filt_idx_new);

/*===========================================================================

FUNCTION SRCHDCH_RESET_1A_MEAS_CELLS_WHEN_DEACT_THRES_REACHED

DESCRIPTION Due to active set update if the active set cell cnt becomes greater than the
1A deactivation threshold then no neighbors are evaluated for 1A criteria. In that case it is 
better to reset the 1A parameters for all the neighbors so that they are evaluated again for
TTT period before being reported after deactivation thershold restriction is not there.
.

DEPENDENCIES
  

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srchdch_reset_1a_meas_cells_when_deact_thres_reached(void);

/*======================================================================
FUNCTION SRCHMEAS_INITIALIZE_CELLS_FOR_MEAS_IDX

DESCRIPTION
Used to initialize meas related info for all the cells for the given
index


RETURN VALUE
void

=======================================================================*/
extern void srchmeas_initialize_cells_for_meas_idx(uint8 carr_idx,uint16 meas_idx);


/*===========================================================================

FUNCTION srch_schedule_txd_detection

DESCRIPTION:
   1. Parse td_detect_bmask sent by demod and get the txd status for each cell. 
      I assume each bit correspond the cell_db_idx that was allocated for each cell.
   2. deregister the service from triage. Call dl_tri_remove_per_rl_triage_info()
   3. check if any service is registered for this cell. Call !srchset_cell_has_service(freq,scr_code). if no service then 
       3.a deallocate the cell id. Call dl_cell_deallocate_cell_id(cell_ptr->cell_db_idx)
       3.b remove the cell from triage set in searcher. srchset_remove_from_triage_set()
   4. remove the cell from txd_det_set srch_txd_det_set_ptr[]

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_handle_txd_det_done(uint8 notify_id, boolean status);

 /*===========================================================================

FUNCTION      SRCHACQ_NASTT_SRCH_CMD

DESCRIPTION
        there was a full PN search and the search
        was segmented into multiple tasks. Now the job of the call back fn is to pick the best
        6 peaks and put it into task#1 so that for the processing function following it can assume
        that the full PN srch was done with only 1 task


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/         
extern void srchacq_pn_search_for_best_eng_task(uint16 num_tasks, uint16 peaks_per_task);

/*===========================================================================

FUNCTION SRCH_DETERMINE_NC_NN

DESCRIPTION:
  - determine coherent Nc and non-coherent Nn integration length to be used
     for PN search 
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_pn_determine_nc_nn(srch_cell_struct_type  * cell_ptr, uint16 *nc, uint16 *nn);

 /*=========================================================================
 
 FUNCTION     : SRCH_SEND_SRCH_CLEANUP_DONEb
 
 DESCRIPTION  : This function sends a srch cleanup done to wl1m.
              
 DEPENDENCIES : 
 
 RETURN VALUE : 
       
 SIDE EFFECTS : 
 
 =========================================================================*/
extern boolean srch_send_srch_cleanup_done(void);

/*=========================================================================
 
 FUNCTION     : SRCH_MAP_MCAL_SRCH_TO_WHL1_SRCH_TYPE
 
 DESCRIPTION  : This fucntion maps the MCAL level search enum to a WHL1 enum.
              
 DEPENDENCIES : 
 
 RETURN VALUE : 
     WHL1 search type

 SIDE EFFECTS : 
 
 =========================================================================*/
extern srch_step_enum_type srch_map_mcal_srch_to_whl1_srch_type(mcalwcdma_srch_type srch_type);

/*=========================================================================
 
 FUNCTION     : SRCH_ACQ_SET_NUM_FREQ_HYPO
 
 DESCRIPTION  : Sets the number of frequency hypothesis in common_parms
              
 DEPENDENCIES : 
 
 RETURN VALUE : 

 SIDE EFFECTS : 
 
=========================================================================*/
extern void srch_acq_set_num_freq_hypo(uint8 num_hypo);

/*=========================================================================
 
 FUNCTION     : SRCH_ACQ_SET_FREQ_HYPO
 
 DESCRIPTION  : Sets the frequency hypothesis value in common_parms
              
 DEPENDENCIES : 
 
 RETURN VALUE : 

 SIDE EFFECTS : 
 
=========================================================================*/
extern void srch_acq_set_freq_hypo(uint8 idx, uint16 val);

#ifdef FEATURE_WTOW_BACKGROUND_PLMN_SEARCH
extern void srch_bplmn_set_samp_serv_freeze(srch_step_enum_type srch_step);
#endif

extern boolean srch_deconfig_all_cells_during_state_transition( l1m_state_enum_type next_state );

extern void srch_set_samp_serv_freeze(srch_step_enum_type srch_step);

extern boolean srchset_deregister_aset_cells
( 
  srchdlif_cfg_reason_enum_type config_reason,
  SRCH_CELL_CFG_DECFG_POST_PROCESSOR dereg_post_proc_cb
);

extern void srch_pn_get_total_eng
(
  uint16 num_tasks, 
  srch_cell_struct_type ** cell_pptr,
  srch_tot_eng_for_meas algo
);

extern void srch_step1_post_processing(srch_state_enum_type srch_state);

extern void srch_pn_add_interf_cells_for_srch
(
  srch_interf_cell_struct_type  **cell_pptr,
  uint16 cell_cnt,
  uint16 max_tasks,
  boolean reset
);

/*======================================================================
FUNCTION SRCHMEAS_GET_INTRA_FREQ_EVT_INFO_FOR_VASET

DESCRIPTION
  Computes the lowest meas_id for the VASET_EVT present in the 
  srch_intra_freq_meas_db.    


RETURN VALUE
    pointer to an entry in intra freq meas dbase

=======================================================================*/

extern srch_intra_freq_meas_parms_struct_type* 
      srchmeas_get_intra_freq_evt_info_for_vaset(l1_intra_freq_event_enum_type VASET_EVT);


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

extern void srch_nastt_async_add_interf_cells_for_srch
(
  srch_interf_cell_struct_type  **cell_pptr, 
  uint16 cell_cnt, 
  boolean reset
);
extern void srch_pn_interf_get_total_eng
(
  uint16 num_tasks, 
  srch_interf_cell_struct_type ** cell_pptr,
  srch_tot_eng_for_meas algo
);
void srchzz_interf_update_eng_params_post_full_srch
(
  /* Cell pointer to inter-frequency cell */
  srch_interf_cell_struct_type *cell_ptr,
  uint16 pilot_idx
);

/*===========================================================================
FUNCTION srch_update_pn_sync_parms

DESCRIPTION:
  This function updates the PN sync parms with RxD info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_update_pn_sync_parms( void );

extern void srch_mcal_whl1_status_cb(mcalwcdma_srch_status_type status, srch_intraf_queue_enum_type srch_queue_idx);

extern void srch_mcal_handle_no_srch_cb(mcalwcdma_srch_status_type status, srch_intraf_queue_enum_type srch_queue_idx);

/* RxD callback, gets called once a search with RxD ON completes so that RxDiv module can turn
 * OFF the div path.  
 */
typedef void SRCH_RXD_TRANS_IND_CB_FUNC_TYPE (boolean srch_rxd_trans_success);

/*===========================================================================

FUNCTION srch_update_rxd_info

DESCRIPTION:
  This function holds the RxD callback that is registered with searcher.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_update_rxd_info(boolean enabled, SRCH_RXD_TRANS_IND_CB_FUNC_TYPE *rxd_proc_srch_cb);

/*===========================================================================

FUNCTION srch_send_cmd_for_rxd_cb

DESCRIPTION:
  This function sends the RxD callback if RxD module is waiting on the diversity chain to
  be turned OFF.

DEPENDENCIES
  Needs to be called only after the end of a search and when no other searches are currently
  scheduled.

RETURN VALUE
  TRUE: if RxD module turning OFF diversity path,
  FALSE: otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_send_cmd_for_rxd_cb( void );

#ifdef FEATURE_WCDMA_SEND_SS_FREEZE_UNFREEZE_TO_FW
/*===========================================================================
FUNCTION srch_process_sample_server_cmd_done

DESCRIPTION:
  - callback function registered with MDSP when sending sample server freeze\unfreeze cmd
  - called when CMD_DONE interrupt received for the sample server freeze\unfreeze cmd

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_process_sample_server_cmd_done (uint16 status);

/*===========================================================================
FUNCTION srch_send_sample_server_cmd

DESCRIPTION:
  This function sends the sample server (un)freeze command to the MDSP sync i\f.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_send_sample_server_cmd
(
  /* Indicates whether to freeze SS */
  boolean freeze,
  /* Indicates whether to unfreeze SS */
  boolean unfreeze,
  /* Indicates whether to apply freeze/unfreeze action to chain 0 */
  boolean chain0,
  /* Indicates whether to apply freeze/unfreeze action to chain 1 */
  boolean chain1
);
#endif /* FEATURE_WCDMA_SEND_SS_FREEZE_UNFREEZE_TO_FW */

/*===========================================================================

FUNCTION srch_is_rxd_enabled_on_nastt

DESCRIPTION:
  This function indicates whether RxD should be enabled on NASTT searches or NOT.

DEPENDENCIES

RETURN VALUE
  TRUE: if RxD enabled,
  FALSE: otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_is_rxd_enabled_on_nastt( void );

/*===========================================================================

FUNCTION srch_speed_up_srch_async_parms

DESCRIPTION:
  This function speeds up the next full search.

DEPENDENCIES
  This should be used in DCH.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_speed_up_full_srch( void );

/*===========================================================================

FUNCTION     srch_determine_cpich_tx_div

DESCRIPTION
  Given a freq and scrambling code, this function queries the appropriate interf
  or intraf DB's to determine a cell's STTD setting.

DEPENDENCIES
  None.

PARAMETERS
 L1 setup command(that includes frequency of target cell), scrambling code
 
RETURN VALUE
  CELL_TXD_OFF - if sttd enabled
  CELL_TXD_ON - if sttd not enabled
  CELL_TXD_UNKNOWN - if cell not found

SIDE EFFECTS
  None
===========================================================================*/
extern srch_cell_txd_status srch_determine_cpich_tx_div(l1_setup_cmd_type *setup, uint16 scr_code);

#ifdef FEATURE_WCDMA_DC_HSDPA
/*===========================================================================

FUNCTION     srch_determine_cpich_tx_div_for_dc

DESCRIPTION
  Given a freq and scrambling code, this function queries the appropriate interf
  or intraf DB's to determine a cell's STTD setting.

DEPENDENCIES
  None.

PARAMETERS
 Frequency of target cell, scrambling code
 
RETURN VALUE
  TRUE, if sttd enabled or cell not found 
  FALSE if sttd not enabled on cell.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_determine_cpich_tx_div_for_dc(uint16 freq, uint16 scr_code);

/*===========================================================================

FUNCTION srch_get_carrier_idx_for_step1

DESCRIPTION:
  This function returns the carrier ID (0: Primary, 1: secondary) on which step1 
  search needs to be issued.

DEPENDENCIES

RETURN VALUE
  Carrier ID (0: Primary, 1: secondary)

SIDE EFFECTS
  None
===========================================================================*/
extern mcalwcdma_carr_selection_enum_type srch_get_carrier_idx_for_step1(void);

/*===========================================================================

FUNCTION srch_get_carrier_idx_for_nastt

DESCRIPTION:
  This function returns the carrier ID (0: Primary, 1: secondary) on which NASTT 
  search needs to be issued.

DEPENDENCIES

RETURN VALUE
  Carrier ID (0: Primary, 1: secondary)

SIDE EFFECTS
  None
===========================================================================*/
extern mcalwcdma_carr_selection_enum_type srch_get_carrier_idx_for_nastt(void);

/*===========================================================================

FUNCTION srch_get_carrier_idx_for_pn

DESCRIPTION:
  This function returns the carrier ID (0: Primary, 1: secondary) on which PN 
  search needs to be issued.

DEPENDENCIES

RETURN VALUE
  Carrier ID (0: Primary, 1: secondary)

SIDE EFFECTS
  None
===========================================================================*/
extern mcalwcdma_carr_selection_enum_type srch_get_carrier_idx_for_pn(srch_cell_struct_type *cell_ptr);

#endif

/*===========================================================================

FUNCTION srch_override_freq_cmn_in_fw

DESCRIPTION:
  This function overrides the freq common info in FW.  

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void srch_override_freq_cmn_in_fw(uint16 val);

/*===========================================================================

FUNCTION srch_should_neighbor_searches_run

DESCRIPTION:
  Function to check whether neighbor searches need to be scheduled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean srch_should_neighbor_searches_run(void);

/*===========================================================================
FUNCTION SRCHACQ_GET_FREQ_HYPO

DESCRIPTION
  This function returns the freq hypo to be used.

DEPENDENCIES
  None.

RETURN VALUE
  Freq hypo to be used.
  When srchacq_toggle_freq_hypo_on_verify_srch is TRUE
      2PPM = FIRST ACQ CYCLE
      4PPM = VERIFY ACQ CYCLE
  Else
      3.5PPM

SIDE EFFECTS
  None.
===========================================================================*/
extern int16 srchacq_get_freq_hypo( void );


/*===========================================================================

FUNCTION SRCH_GET_ASET_RSCP

DESCRIPTION 
    This function is used to calculate the unfiltered RSCP from the primary antenna, 
    combined across all cells in the ASET.

DEPENDENCIES
  
RETURN VALUE
    RSCP value (in positive) range:(0-116)*2 

SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 srch_get_aset_rscp(void);

/*===========================================================================

FUNCTION SRCH_GET_ASET_RSCP_PRIM

DESCRIPTION 
    This function is used to calculate the unfiltered RSCP from the primary antenna, 
    combined across all cells in the ASET.

DEPENDENCIES
  
RETURN VALUE
    RSCP value (in positive) range:(0-116)*2 

SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 srch_get_aset_rscp_prim(void);

/*===========================================================================

FUNCTION SRCH_GET_ASET_ECIO

DESCRIPTION 
    This function is used to calculate the unfiltered RSCP from the primary antenna, 
    combined across all cells in the ASET.

DEPENDENCIES
  
RETURN VALUE
    EcIo value (in positive) range:0-63 

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 srch_get_aset_ecio(void);

/*===========================================================================

FUNCTION SRCH_GET_ASET_RSCP_DIV

DESCRIPTION 
    This function is used to calculate the unfiltered RSCP from the diversity antenna, 
    combined across all cells in the ASET.

DEPENDENCIES
  
RETURN VALUE
    RSCP value (in positive) range:0-116 

SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 srch_get_aset_rscp_div(void);

/*===========================================================================

FUNCTION SRCH_GET_ASET_ECIO_DIV

DESCRIPTION 
    This function is used to calculate the unfiltered RSCP from the diversity antenna, 
    combined across all cells in the ASET.

DEPENDENCIES
  
RETURN VALUE
    EcIo value (in positive) range:0-63 

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 srch_get_aset_ecio_div(void);

/*===========================================================================

FUNCTION SRCH_GET_ASET_ECIO_PRIM

DESCRIPTION 
    This function is used to calculate the unfiltered RSCP from the diversity antenna, 
    combined across all cells in the ASET.

DEPENDENCIES
  
RETURN VALUE
    EcIo value (in positive) range:0-63 

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 srch_get_aset_ecio_prim(void);

/*=========================================================================

FUNCTION     : SRCH_GET_S_INTRA_SEARCH 
               
DESCRIPTION  : This function returns cell selection threshold for W cell. 
  
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern int16 srch_get_s_intra_search(void);

/*=========================================================================

FUNCTION     : SRCH_GET_S_INTER_SEARCH 
               
DESCRIPTION  : This function returns cell selection threshold for W inter-freq cell.  
  
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern int16 srch_get_s_inter_search(void);


/*=========================================================================

FUNCTION     : SRCH_GET_S_INTERRAT_SEARCH 
               
DESCRIPTION  : This function returns cell selection threshold for GSM neighbor 
               cell.  
  
DEPENDENCIES : None

RETURN VALUE : s_interRAT
      
SIDE EFFECTS : None

=========================================================================*/
extern int16 srch_get_s_interrat_search(void);

/*=========================================================================

FUNCTION     : SRCH_GET_RXAGC 
               
DESCRIPTION  : This function returns RxAGC in 1\10db units on a given RF chain. 
  
DEPENDENCIES : None

RETURN VALUE : RxAGC in 1\10db units
      
SIDE EFFECTS : None

=========================================================================*/
extern int16 srch_get_rxagc(rfcom_device_enum_type rfcom_transceiver,uint8 carr_idx );

/*=========================================================================

FUNCTION     : SRCH_GET_CELL_CURRENT_ECIO 
               
DESCRIPTION  : This function returns instantaneous ecio in 1\10db units
               for a given cell. 
  
DEPENDENCIES : None

RETURN VALUE : Instantaneous ecio in 1\10db units
      
SIDE EFFECTS : None

=========================================================================*/
extern uint16 srch_get_cell_current_ecio(srch_cell_struct_type *cell_ptr);

/*=========================================================================

FUNCTION     : SRCH_GET_CELL_CURRENT_RSCP
               
DESCRIPTION  : This function returns instantaneous RSCP in 1\10db units
               for a given cell. 
  
DEPENDENCIES : None

RETURN VALUE : Instantaneous RSCP in 1\10db units
      
SIDE EFFECTS : None

=========================================================================*/
extern uint16 srch_get_cell_current_rscp(srch_cell_struct_type *cell_ptr);

/*=========================================================================

FUNCTION     : WCDMA_PROCESS_SIGNAL_INFO
               
DESCRIPTION  : This function processes WCDMA signal. 
  
DEPENDENCIES : None

RETURN VALUE : Instantaneous RSCP in 1\10db units
      
SIDE EFFECTS : None

=========================================================================*/
extern void wcdma_process_signal_info(void);

/*===========================================================================
  FUNCTION  WSRCH_UPDATE_CELL_REF_POS
  
  DESCRIPTION
    This function updates the reference position with max_pos[0] and updates
    the Window Size accordingly.
   
  INPUT
    CELL_PTR for which ref_pos need to be updated

  DEPENDENCIES
    None.
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None.
  
===========================================================================*/ 
extern void wsrch_update_cell_ref_pos(srch_cell_struct_type *cell_ptr);

/*===========================================================================

FUNCTION SRCH_COMBINE_PATH_ENGY

DESCRIPTION:
 -calculate total energy according to the 'algo' specified by the caller
 -calculate total enery for the set of cells specified by 'cell_pptr' and 'num_tasks'
 Total energy will then be used to calculate filtered eng (in DCH/FACH) which will
 be used in HO and reselection measurements
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  updates the following parameters in cell_ptr
  1. tot_eng
  2. tot_ecio
  3. tot_rscp
  4. ecio
===========================================================================*/
extern uint32 srch_combine_path_engy( srch_cell_struct_type *cell_ptr, uint8 ant_idx);

/*===========================================================================

FUNCTION SRCH_IS_CELL_IN_ACSET

DESCRIPTION
  This function checks if the cell is part of acset .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern boolean srch_is_cell_in_acset( srch_cell_struct_type *cell_ptr);

/*=========================================================================

FUNCTION     : SRCH_GET_ROUNDED_RSCP 
               
DESCRIPTION  : This function returns RSCP on a given RF chain rounded to 
               the nearest dBm. 
  
DEPENDENCIES : None

RETURN VALUE : RSCP in dBm
      
SIDE EFFECTS : None

=========================================================================*/
extern uint16 srch_get_rounded_rscp(rfcom_device_enum_type rfcom_transceiver, uint16 ecio,uint8 carr_idx);

/*===========================================================================
FUNCTION        SRCH_CONTROL_MEASUREMENTS

FILE NAME       WSRCH.C

DESCRIPTION     This function is to used to notify start/stop of measurements in FACH/DCH states.
                meas_start = TRUE -> Measurements can start
                meas_start = FALSE -> No more measurements

DEPENDENCIES    

RETURN VALUE    
  void.

SIDE EFFECTS    
===========================================================================*/
extern void srch_control_measurements(uint8 carr_idx,boolean start_meas);

/*===========================================================================
FUNCTION        SRCH_GET_MEASUREMENT_STATUS

FILE NAME       WSRCH.C

DESCRIPTION     This function returns the search measurement status

DEPENDENCIES    

RETURN VALUE    Boolean

SIDE EFFECTS    
===========================================================================*/
extern boolean srch_get_measurement_status(uint8 carr_idx);

/*===========================================================================

FUNCTION srch_get_rxagc_from_upd_rf_api

DESCRIPTION
  This function calls the updated RF api to read the RxAGC value.

ARGUMENTS
rxagc units and rxagc for div or prim

DEPENDENCIES
  None.

RETURN VALUE
  RxAGC

SIDE EFFECTS
  None.

===========================================================================*/
extern int16 srch_get_rxagc_from_upd_rf_api(srch_rxagc_val_units_enum rxagc_units, boolean for_div,uint8 carr_idx);

/*===========================================================================
FUNCTION        SRCH_CONN_STATE_CLEANUP

FILE NAME

DESCRIPTION     This is the connected (FACH/DCH) state cleanup routine for SRCH

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern boolean srch_conn_state_cleanup(l1m_state_enum_type next_state);
/*===========================================================================
FUNCTION        SRCH_VALIDATE_REF_POS_UPDATE

FILE NAME       WSRCH.C

DESCRIPTION     This function validates the ref pos update results from NASTT/recovery
                window mode PN search.
                Ref pos update by more than 1 BPG is not allowed for cells enabled in FW

DEPENDENCIES    

RETURN VALUE    Boolean

SIDE EFFECTS    
===========================================================================*/
extern boolean srch_validate_ref_pos_update(srch_cell_struct_type *cell_ptr, uint32 candidate_pos);

/*===========================================================================

FUNCTION SRCH_CRASH_ON_UNXPECTD_RESULTS_FROM_SRCH

DESCRIPTION
  This function checks if the ecio/rxagc value is lesser/greater than the threshold
  and issues a FW panic cmd and SW err fatal.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_crash_on_unxpectd_results_from_srch(boolean pn_srch);

/*===========================================================================

FUNCTION SRCH_DISABLE_DEBUG_CRASH_ON_SRCH_DONE

DESCRIPTION
  This function disables the debug crash upon search done

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_disable_debug_crash_on_srch_done(void);

/*===========================================================================

FUNCTION SRCH_INIT_RUMI_BRINGUP_DB

DESCRIPTION
  This function initialized all RUMI bringup searcher variables to FALSE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_init_rumi_bringup_db(void);

/*===========================================================================

FUNCTION SRCH_ENABLE_AND_UPDATE_CRASH_INFO

DESCRIPTION
  This function enables the debug crash upon search done and updates the structure
  with the crash info received from the QxDM command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_enable_and_update_crash_info(uint8 l1m_state, uint8 bmask, uint8 carr_idx, 
        uint8 type_of_search, uint8 thresh, uint16 freq, uint16 psc);

/*===========================================================================

FUNCTION SRCH_ENABLE_AND_UPDATE_ATTENUATE_INFO

DESCRIPTION
 This function enables sw attenuating. The desired attenuation
 value passed through QXDM is updated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_enable_and_update_attenuate_info(uint8 attenuate_value);

/*===========================================================================

FUNCTION      SRCH_ASYNC_PARM_INIT_ON_RXD_ACTIVE

DESCRIPTION
  Writes to FW that step1, NASTT for intra freq outside CM gaps will be using 
  RxD mode 2 in DCH. All other step1 async params remain same.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void srch_async_parm_init_on_rxd_active(void);

#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================

FUNCTION   SRCH_IS_DC_CONFIGURED_IN_UL

DESCRIPTION
      This Function tell if DC is configured in UL or not.

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  TRUE, if DC is configure else FALSE

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srch_is_dc_configured_in_ul(void);
#endif

/*===========================================================================

FUNCTION   SRCH_SKIP_EVAL_ON_CARRIER

DESCRIPTION
      This Function is used specifically to skip the evaluation or further processing of code 
      which is not meant for the carrier index  like measurement evaluation on DC carrier 
      while DC is not configured in UL.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE, if further evaluation needs to be done on the carrier specified.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srch_skip_eval_on_carrier(uint8 carr_idx);

/*===========================================================================

FUNCTION SRCH_PROC_RXD_UPD_CB_CMD

DESCRIPTION
  This function calls the RXD update callback after RXD is turning OFF.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_proc_rxd_upd_cb_cmd(void);

/*===========================================================================

FUNCTION SRCH_GET_MAX_RSCP

DESCRIPTION
  Calculates the weighted sum of the primary and secondary RSCP

DEPENDENCIES
  Rxd must be ON
  Input RSCP must be -ve

RETURN VALUE
  RSCP value (-ve)

SIDE EFFECTS
  None.

===========================================================================*/
extern int16 srch_get_max_rscp(int16 rscp_pri, int16 rscp_div, uint8 sttd);

#ifdef FEATURE_DUAL_SIM
extern void srch_triage_set_srch_done_non_drx(srch_done_cmd_type *cmd_ptr);
#endif

/*===========================================================================
FUNCTION        SRCH_UPDATE_DESENSE_STATUS

FILE NAME       WSRCH.C

DESCRIPTION     This function gets called by multisim module whenever desense information needs to be updated.

DEPENDENCIES    

RETURN VALUE    NONE

SIDE EFFECTS    
===========================================================================*/
extern void srch_update_desense_status(boolean desense_info);

#ifdef FEATURE_WCDMA_STEP1_PEAK_PRUNER
/*===========================================================================

FUNCTION SRCH_PRUNE_STEP1_RESULT_FOR_FALSE_PEAKS

DESCRIPTION
  This function prunes the false Step1 peaks for all step1 results.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_prune_step1_result_for_false_peaks();

/*===========================================================================

FUNCTION SRCH_REMOVE_ALL_PRUNED_PEAKS

DESCRIPTION
  This function prunes the false Step1 peaks for all step1 results.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_remove_all_pruned_peaks();
#endif


#endif /* SRCHI_H */
