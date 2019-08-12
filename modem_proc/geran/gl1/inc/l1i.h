#ifndef L1I_H
#define L1I_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L 1   T Y P E S   H E A D E R   F I L E

DESCRIPTION
   Layer 1 type declarations.


Copyright (c) 2001-2015 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1i.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- --------------------------------------------- 
06/06/16     km      CR1017936 FR 35777: Enable Samsung 'Big Data' feature on 8917 - GERAN GL1
03/06/16   hd       CR1021365 Send MCPM_GERAN_DATA_STOP_REQ while in FTM mode
14/03/16   bg       CR989412 Backout the CR757734
25/01/16   bg       CR757734 When transitioning to transfer from Idle, request TRM after Idle abort finishes, and deact FW/RF
05/11/15   df       CR923258 Avoid SCE crash while aborting idle and receiving MPH_UPDATE_IDLE_DRX_IND  
09/10/15   df       CR921075 FR23269 M2M devices: Handle RR->GL1 MPH_UPDATE_IDLE_DRX_IND
30/03/15   nm       CR765891 Allowing TA for TDS during PS RACH
16/12/14   jk       CR753423:LIF for Idle,voice and data
03/02/15   zf       CR780458: Adding L1 task grm_release handling for callback 
04/02/14   nk       CR767734 Allowing TA during 2-phase access
12/01/14   js       CR775188 GL1 changes to receive EPD preference
06/01/14   am       CR390109 Improved DSDS cell reselection
10/12/14   sp       CR767751: GL1 recovery - dont issue x2g_startup if too late in frame.
04/12/14   br       CR765226 Backingout CR732436,as it is causing the ACQ delayed by one frame
19/11/14   br       CR763774 GL1 logging improvement to ease debugging
17/11/14   cs       CR701894 Enhanced Page Detection Support
31/10/14   xz       CR748816 Extend dual data space for building RF script variable
04/11/14   df       CR734490 Rework the wake up recovery mechanism
01/11/14   df       CR748602 Rework and enable adaptive timeline
27/10/14   am       CR646584 WLAN coex
26/09/14   zf       CR732436: Delay starting ACQ when RF wakes up late
06/10/14   pg       CR734405 Thor compilation errors 
03/10/14   aga      CR732455 Changes required to tune away framework for TH 1.0 single sim build 
26/09/14   fan      CR730788 Moving the start RF timer next to setting RF_WAKEUP flag to the commit mask 
                          and correctly abort PWR mon
03/10/14   ap       CR730903 Handling race conditions of receiving RA req following RA abort and select specific BCCH req in decoupled state
26/09/14   zf       CR730930: Cancel pending GFW commands when RF does not wakeup before next frame to mitigate TRM/RF concurrency delay issue.
30/05/14   zf       CR662056: CMCC power test opt
12/09/14   nm       CR723570 GL1 changes for FR21036 - Read own PCH in transfer to BO2.0(to be leveraged for TH/JO)
09/09/14   jk       CR697308:GL1 changes to support HLin LLin sawless feature
09/09/14   cgc      CR721718 FEATURE_GSM_TO_LTE changes to fix compile errors for non-lte build
09/09/14   pa       CR719024: Implementation of Hybrid Tuneaway Fallback Algorithm 
22/08/14   cgc      CR713634 obsoleted G2L code and featurisations FEATURE_GSM_G2L_TIMED_SRCH and FEATURE_CM_G2L removed
22/08/14   sp       CR685373: Maintain requested cell (BSIC) from RR and report failure if BSIC decoded is not matching in ISR SCH CNF.
13/08/14   dg       CR690573 Remove compilation error
11/08/14   sjw      CR706459: use L1_COMPLETE_DS_ABORT for race condition between abort and release
11/02/14   jk       CR614254:GL1 changes to support IP2 calibration
11/07/14   pjr      CR 692177 Added support for RFACCH 2.0
06/08/14   ka       CR699964 Implement Idle RXD support in GL1
06/08/14   df       CR536059 Full asynchronous sleep support in idle
04/08/14   pa       CR701897: In WTOG BPLMN Resume, check RF device before initialising mult-sim FW and RF.  
10/07/14   jk       CR688587:GL1 changes to support LIF feature
24/07/14   zf       CR500399: Don't schedule PCH receive between PCH and extended PCH
30/07/14   ssh      CR701245: Avoid running an adjoining activity after rude wake up if the FW/RF is being initialized in that frame
30/07/14   sp       CR688572: Memory opts - reduce global data.
29/07/14   ap       CR695234: Changes for GL1's New Rach interface
27/07/14   ap       CR678067: Race condition between APP mode as ACTIVE and Device mode change as DSDA leads to crash
23/06/14   ap       CR678067: Race condition between APP mode as ACTIVE and Device mode change as DSDA leads to crash
09/07/14   am       CR691142 Update new multisim mode when all subs are inactive
15/07/14   dg       CR694542 GL1 to send INIT_REQ to LTE only when it has received LIST REQ
                    from RR with valid LTE neighbors
18/06/14   cos      CR682031 - GL1 control algorithm and changes for supporting AEQ PHASE 4
19/05/14   ss       CR668474 X2G Init via task : Seperate message header for each tech
20/06/14   pjr      DATA+MMS feature - USF utilisation calculation
30/05/14   ws       CR657674 Data + MMS Phase 1
20/05/14   smd      CR662851: Release and request RF device again for RACH in SGLTE+G
25/06/14   cs       Remove FEATURE_ASYMMETRIC_GERAN_BANDS
17/06/14   cs/ch    CR651795 TRM Updated changes for DR-DSDS
29/05/14   cs/pa    CR660831 Use new trm_status definitions specific to GL1
12/05/14   cgc      CR662650 fix update for CR659270
08/05/14    sp      CR655478:Handle Unsupported Bands on WTRs while RANE during Powerscan.
08/05/14   nk       CR660831: GSM Layer 1 compares wrong enum values for internal TRM_DENIAL status.Change TRM_DENIAL enum to TRM_NOT_GRANTED to aoid any confusion going forward.
10/05/14   rc       CR662550: Back out changes made in CR655548
06/05/14   smd      CR655548: Release and request RF device again for RACH in SGLTE+G
23/04/14   zf       CR653611: Reset reduced power monitor duty cycle count when missing a page
16/04/14   ws       CR 645067 - Increase Q6 clk speed when AEQ2A is enabled in EGPRS mode
14/04/14   ss       CR646261 FR GSM CM INIT via message in GL1 task context
07/04/14   cja      CR644445 Mainline feature FEATURE_GSM_ASYNC_FW_INTF
04/04/14   cah      CR643764 VSTMR multi-sim restruct and FEATURE_MSIM_VSTMR removal
04/04/14   sai      CR638388: Delay TBF re-establish in DSDS if G2W resel ongoing on other sub.
08/04/14   gk       CR641603 Ignore yielded power monitor results
18/03/14   ap       CR614949 FR 19574: Performing LTE/TDS measurements while GERAN is in connected mode using the second WTR
12/03/14   ssh      CR627985 Skip first burst decode when waking up from sleep inside a QTA gap
08/03/14   am       CR624825 If TRM is retained for TBF assignment then TRM release should happen on TBF release
19/02/14   jk       CR608290:GERAN RxD ON/OFF adaptive algorithm
20/02/14   ssh      CR613866: If TRM self-denied in the first QTA gap we should avoid running any [means all] activity
24/01/14   cah      CR598552: enable sleep for DSDS GSTMR+VSTMR
14/01/14   ssh      CR597264:GSM uses RF API to choose to use RFM enter mode or VREG+WAKEUP.
19/12/13   jk       CR503359 - GL1 changes to support Rx diversity
06/01/13   sk       CR592996 Ensure GSTMR is not slammed inside the QTA gap
17/12/13   ap       CR566007: W is not getting the lock for Manual PLMN search when Sub2 is in data call
03/12/13   zf       CR584257: Do not start G+G optimized mode immediately after multi SIM init
27/11/13   cja/npt  CR555678 Speed up clock during power scan for RF processing load
04/11/13   mc       CR556742 G+G COEX : Enhancements for monitor positioning and improved SDCCH/FACCH conflict detection
19/11/13   ssh/ip   CR547956 DSDS/DSDA GL1 timeline Opt (parallelize the RF config. with GL1 warm-up)
15/11/13   nm       CR575532 Back-out changes for CR537690 & CR547955 - power optimization changes
28/11/13   cja      CR521061 Remove unused RF API and mainline FEATURE_GSM_RFA_TASK.
20/11/13   cja      CR579775 On Dime use MCVS for settinig clocks for TCH AEq and speed-up
19/10/13   sk       CR563877 Ensure BCCH reads are given priority over PCH when doing reselection and consider early wakeup time into TRM reservation
13/11/13   ap       CR562878 FR 17482: Page Blocking Management in Multi-SIM Single Radio Devices
24/09/13   jk       CR549227:Shifted SACCH changes for VAMOS2
31/10/13   cgc      CR569801 Add wrapper functions expose gl1_mutex_lock() gl1_mutex_unlock()
14/10/13   cja      CR555678 Speed up clock during power scan for RF processing load
08/10/13   js       CR519541: Add wakeup from sleep during QTA support in idle ISR state mc,block sleep in QTA mode
05/10/13   sk       CR554665 Backout the power opt temperorily(CR537690)
05/10/13   js       CR461049Cell Reselection Failure when 3 KHz frequency drift is applied on N cell
27/09/13   sp       CR551580: Add NV logging of outgoing L1 messages
04/10/13   nm       CR552803 tighten power scan threshold from -107dBm to -102dBm for background power scan only
04/10/13   gk       CR492165: Read Extended Page if the second block of the page is between 42-49 in 51 MF
01/10/13   sk       CR552259 CR sync from 1.0/1.1
20/08/13   nn       CR518344 Use secapi_get_random() for generating random numbers.
23/05/13   pg       CR460555: Enable 3GPP Rel10 RACH Power Reduciton feature
24/09/13   zf       CR537690: Reduce G wakeup timeline for MS mode
16/09/13   cs       CR503042 Updated XO RGS handling for Triton
15/08/13   sk       CR524039 GBTA bringup changes
08/05/13   cgc      CR453758 add FEATURE_CM_G2L search functionality FR-3009
24/07/13   jk       CR464982:Changes to support GEA5_4 ciphering
29/07/13   sk       CR519663 Partial QBTA bringup changes
21/06/13   cs       Major Triton TSTS Syncup
13/06/13   pg       CR463973 Increase GFW MSGR buffer size to allow 20ms GSM_L1 task delay
06/06/13   cs       TSTS Updates
13/06/13   sai      CR495383: Add support for therm read in DSDS mode.
06/06/13   ap       CR491896 Delay the release of TRM when pscan ends for MPLMN
16/05/13   ab       CR487396 - Support EGPRS IR Reduced Window in GFW
28/05/13   ap       CR461268: DSDS XO management + async time drift problem
29/05/13   am       CR488891 Ensure SCE is stopped on randomm access
20/05/13   pg       CR463973 Increase GFW MSGR buffer size to allow 20ms GSM_L1 task delay
25/03/13   mc       CR464496 : COEX Support - GERAN arbitrator SW CXM integration
08/03/13   cja      CR461030 Add RF CM Enter and Exit
12/12/12   cgc      CR431006 add L1_NO_ERROR to avoid confusion with default 0 values.
04/02/12   ap       DSDS CR448369  W/G+G DSDS:UE stuck in W2G HO and not recovere
08/02/12   mc       DSDS CR332154 Ensure that a PCH following a BCCH gets decoded upon a missed BCCH block
28/11/12   br       CR383165 added missing declarations to avoid compilation warnings
10/02/12   ss       CR 405129 : Add version in G2T Log Packets  and Change Log Packet Id for CR363991
31/05/12   ab       CR 415136 RX and TX Time Power averaging update using msg router intf.
10/09/12   ss       CR363991 : Enable log packets for G2T activity FEATURE_G2T_SRCH_LOGGING
13/09/12   ws       CR 399455 Fix compile errors when FEATURE_LTE_TO_GSM is disabled
03/07/12   ky       Async message router sleep, app mode, enter mode interface changes
25/05/12   pg       Add support for RF Task
18/05/12   ky       Async message router Interface Changes between SW & FW
31/05/12   ab       CR 366880 RX and TX Time Power averaging Feature for DSDA as part of SAR Req.
16/05/12   dv       CR357448 G2T Abort mechanism
25/04/12   pa       CR355881: G2T Dedicated TCH: TL1 crashed in RF API. Added support of RF_SCRIPTS_CMD.
04/04/12   pa       CR349266: G2T: Workaround for not to do IRAT when there is a BCCH block to read
13/03/12    ws      CR335057 eSACCH buffer flush and restore after inter-cell handovers
05/03/12   cgc      CR339393 Rework speedup to allow speedup on DTM->DED whilst on slot 2
29/02/12   cja      CR339905 Allow concurrent G2W and channel assignment speed-up
10/02/12   ss       CR 405129 : Add version in G2T Log Packets  and Change Log Packet Id for CR363991
30/01/12   cgc      CR332648 add transition state for channel assignment.
10/02/12   ws       CR 336194 Enable TD-SCDMA IRAT for GERAN
28/07/11   og       Cancel the G2L search attempt if sending the startup message
                    fails. CR296627.
19/04/11   cja      Add RxLM buffer for LTE startup
7/12/10    og       CR265406. Allow the LTE ML1 thread to use the gl1 mutex.
29/11/10   og       Adding G2L support.
05/05/11   ip       CR 286693 Recovery mechanism when RPM delay warmup interrupt
25/03/11   njt      DSDS CR280372 - mdsp busy after cell reselection
21/03/11   nt       DSDS CR:278831 - Gl1 attempt to stop mDSP while it is busy
25/01/11   nt       Added suspension state to L1_multi_sim_acquiring.
21/01/11   cs       CR246418 DSDS Allow disabling of SAIC F/W processing in idle
10/01/11   ap       DSDS CR270377 SYNC SIM is disabling L1 ISR while ASYNC SIM is still
                    active when consecutive start/stop GSM mode req
22/11/10   ky       Panic reset mechanism is changed. For all errors gl1 first
                    sees an nv item set or not. if this nv item is set then does
                    error fatal immediately other wise gl1 sends signal to grr for
                    recovery
21/07/10   cs       CR246418 Allow disabling of SAIC F/W processing in idle
21/07/10   ems      Added support for ignoring SACCH power control
15/04/10   dmb      Add the ability to force SI3-only parallel BCCH decodes
10/04/01   tjw      Add customer.h and comdef.h to all SU API files
09/06/09   nf       Added FEATURE_GSM_PCH_SINGLE_BURST_DEC
06/06/09   ab       Feature FEATURE_GSM_EGPRS_IR_OPT for LCU
26/05/09   cs       CR167848 fix for expanded afc_freq I/F to mDSP
24/03/09   dv       CR176933 - Call mute after ending Loopback C Test
13/03/09   og       FEATURE_XO updates.
13/03/09   dv       Fix bug in CR169424
23/02/09   dv       CR169424:Cannot make MO/MT calls
15/01/09   nt       CR167218 - changes to restore old rot and pdm values after HO fails
03/12/08   ws       Added FEATURE_GSM_DYNAMIC_SAIC
12/01/08   gj       CR165617 - mainline FEATURE_GSM_GPRS_ENHANCED_MEASUREMENT_REPORTING feature.
17/11/08   nf       CR163004 - Corrected SACCH buffer size to 23
09/10/08   cs       Allow the RX Sweep acquisition tb be restarted
28/08/08   cs       Implement the Fast Rx Sweep functionality for FTM
22/08/08   nt       Removed compiler warnings
29/07/08   ws       Renamed __align to ALIGN to allow macro to become portable
25/07/08   ap       Added FEATURE_GAN: Changes to support GAN->GSM HO.
16/06/08    nt      CR147984:- Changes for TCXO reset.
05/06/08   ps        Lint Cleanup
07/03/08   ip       Added PSHO changes under FEATURE_GPRS_PS_HANDOVER
19/02/08   og       Handle the case of power scan request containing a zero list.
14/02/08    nf      Added feature enabling other Tech L1's to find if GSM is in a TX state
21/11/07   og       Ensure L1 drops back to IDLE when the single block start
                    time elapses. Resolves CR115529.
09/11/07   nf       Added FEATURE_GSM_R_SACCH
08/07/07   ws       CR - 124046 Open loopback mode C when we need to send FACCH
28/02/07   nt       CR112989: Turned on FEATURE_GSM_TRANSFER_DED_SERVING_CELL_INFO, by
                    removing switch from around the code.
05/01/07   og       Replace err_fatal with panic reset when tbf assignment connected
                    from single block 2-phase access has no valid start time.
19/12/06   nt       CR107800: non_drx timer is started only when leaving transfer.
30/10/06   og       DTM enhanced release code drop.
12/10/06   og       Merge fix to CR101667 / SR860781. Ensure the correct freq err
                    is applied before attenpting to decode BCCH after pscan.
17/08/06   og       Basic featured-off EDTM code drop.
14/08/06   ws       Allow dedicated to be timeslot slammed to lowest received tn
                    and also to place the CS timeslot on TN 2
15/05/06   nt       CR91329 - Added flag to indicate if the previous starting time
                    on the old assignment had expired, upon returning to old channel
                    configuration with the reconnect_channel_req.
07/04/06   nt       Changes for required host test support/compilation
06/02/05   nt       Added new internal message used when handling simultaneous
                    CMM and L1_REL....called MAC_L1_RELEASE_AND_RR_CMM_COMBINED
12/12/05   gw       Added support for L1_ENTER_LPM_REQ.
11/07/05   sv       Add support for FEATURE_WTOG_BACKGROUND_PLMN_SEARCH.
08/11/05   nt       Added new event L1_DTM_CANCEL_PS_RECONFIG to the DTM events
11/07/05   sv       Add enum used to reset BPLMN state when deactivating GSM.
19/09/05   nt       Introduced new macros for DTM for ease of code readability.
                    Added new DTM event L1_DTM_RECONFIG_BOTH_CS_PS.
09/01/05   ws       Linted and code cleanup for FEATURE_GSM_DTM
08/19/05   gfr      Move NPL1 RACH function calls to ISR context
08/16/05   dp       Lint cleanup
08/16/05   sv       Add support for stuck in FIND_BCCH mode.
08/12/05   ws       Added initial FEATURE_GSM_DTM support
06/07/05   av       CR68673 - PCA was not sent in response to a polled PDA. PL1 was going to sleep.
06/13/05   yh       Implementing qsort.
05/25/05   dp       Added prototype for l1_init_dedicated_data
17/05/05   og       Remove the forcing of pkt paging mode to normal.
04/22/05   gfr      Add ALIGN16 macro to align variables on a word boundary
04/21/05   sv       Add support for L1 "Panic Reset"
03/16/05   pp       Added last_cell_rx_power to support agc fix
03/15/05   dp       Added support for enhanced measurement reporting
02/25/05   pp       Added support for GSM to GSM Blind Handovers
02/11/04   bm       Fix to send RESUME_GSM_MODE_CNF after resuming
02/11/04   bm       Support for Background WCDMA PLMN Scan
02/05/05   bm       BG PLMN scan optimizations
01/26/05   sv       Add support for Frame Manager.
10/01/04   bm       Enhanced Parallel BCCH decode support
01/05/05   dp        Merged dedicated timeline changes for audio optimizations
                     to mainline
11/11/04   og       Copy in BCCH/PBCCH bep_period into L1 CCCH/PCCCH data structures.
22/07/04   dlh      Added members to L1_Idle_data and gpl1_Idle data for reselection.
09/27/04   bm       Fixing some merge errors
09/24/04   bm       Support for Split up the L1 deactivation into TASK and ISR
08/03/04   sv     Added new command and sub-state for the full acquistion
                    scenario during GPRS reselection .
07/21/04   bk        Merged in changes to include BEP to gprs_l1_idle_data_t
06/11/04   dp       Removed BCC from l1_isrtsk_blk
06/07/04   gw       Allow reselection pcan to do more than 1 meas per ARFCN.
06/04/04   sv       Removed all references to XXX_mframe_power_XXX and
                    element "last_cell_rex_pwr_lvl" in dedicated_data_T.
06/04/04   sv       Changed the enum type of "L1_confirm_ID".
06/02/04   gw       Do fewer measurements for reselection powerscan.
05/26/04   sv       Changed "l1_isrtsk_blk" structure parameters for better
                    code readability.
05/12/04   gw       Added state for PRACH.
03/18/04   dlh      Added support for cancelling non drx timer if RR requests a
                    surround update
02/25/04   gw       Lint clean up.
02/09/04   dp       Added support for background HPLMN searching
02/04/04   ws       Fixed bug with prototype for L1_send_MPH_READ_SYS_INFO_FAILURE_IND
02/02/04   kf       Added definitions for serve cell bcch read functions
                    for transfer mode.
01/28/04   gw       Changes to support reselection optimizations.
01/09/04   gw       Merged in support for new SI reading API from branch.
11/05/03   gfr      Support for quad-band.
08/29/03   dlh      modifided some pbcch/packet idle enums.
07 21/03   dlh      added  start_time_valid flag.
07/21/03   dlh      added L1_GPRS_IDLE_WAITING_FOR_FEE sub statet for 52 idle.
16/07/03   kf       modified ncell pbcch reading.
10/07/03   kf       added field to L1_send_MPH_SURROUND_UPDATE_FAILURE_IND
07/04/03   ws       Added gprs_init_paging_mode for GPRS
06/13/03   pjr      Added L1_PBCCH_RESELECTION to L1 sub states.
06/10/03   pjr      Added update_pending parameter to gprs_l1_idle_data_t
06/10/03   sw       Added support for SI13 on Extended BCCH.
05/07/03   gsc      Added AMR support
05/02/03   gw       Merged in power transfer changes from branch.
05/02/03   ws       Update to l1 sub states for PBCCH and PCCCH handlers
05/01/03   ag       Updates to ncell pbcch reading
04/14/03   ATM      Cleaup unused variable: DTX_used
04/04/03   gw       Added protos for functions that return Idle Mode
                    serving cell ARFCN, BSIC and rx_power.
04/02/03   gw       Removed some obsolete declarations.
03/10/03   dp       Added prev_paging_mode to paging_data_T
03/04/03   pjr      Added L1_ABORT_PRACH to L1 sub-states
02/28/03   kf       Changes for ncell psi reading
02/21/03   gw       Added declarations to support deep sleep.
02/14/03   pjr      Added pbcch elements to structure gprs_l1_idle_data_t
01/07/03   ws       Added EGSM ARFCN defintions for band checking in GPRS
12/05/02   gw       Changed idle_data_T to support decoding SI 2quater
                    on extended BCCH.
12/03/02   mk       General GPRS featurization cleanup, added
                    L1_send_MPH_SERVING_GSM_IDLE_MEAS_IND().
11/27/03   ws       Added mm non-drx mode
11/25/02   mk       Updated GPRS baseline.
11/18/02   mk       Updated GPRS baseline with release 2.0 functionality.
11/15/02   dp       Added enum for band type
11/14/02   ws       Added new L1_GPRS_IDLE_MODE
11/13/02   DLH      Added nco and nc_non_drx_period to gprs_l1_idle_data_t
11/11/02   ATM      Added ciphering and vocoder control to ho_details
10/08/02   kf       Added prototype L1_send_MPH_NC_MEASUREMENT_IND
10/07/02   dp       Added current_sch_index and current_bcch_index to
                    l1_BCCH_List for fully parallel decodes
10/02/02   ATM      Added compressed mode interRAT handover functionality
10/11/02   mk       Merged GPRS release 1 phase2 code to baseline.
10/10/02   mk       Merged GPRS release 1 phase1 code to baseline.
09/03/02   pjr      Added sub_state L1_SEND_NO_PRACH
08/29/02   JC       Added FEATURE_GSM_MULTIBAND support and eliminated
                    legacy GSM_900, DCS_1800, and PCS_1900 support.
08/17/02   DLH      Added items to GPRS_paging struct
08/13/02   dp       Added new enums/data structures to support parallel acq/
                    BCCH decodes
8/12/02    gw       Changed some pscan definitions to support changes
                    to RR-L1 i/f as part of cell selection/re-selection
                    optimizations
08/20/02   ATM      Standardized L1 main state and substate enums per review.
07/10/02   ATM      Added blind-handover support.
07/02/02   gw       Moved running_avg_struct to l1_sc_int.h.
06/27/02   dp       Added states to decode just a SCH as opposed to doing
                    a combined acquisition
06/27/02   gw       Added running_avg_struct to support making
                    running averages of power measurements.
                    Included a field of this type in l1_idle_data.
06/13/02   mk       Added sub_state to L1_SCAN_.
06/10/02   dp       Added stop_meas_rpt to l1_dedicated_data_T
05/30/02   pjr/mk   Added PL1 GPRS code hooks under switch FEATURE_GSM_GPRS_L1.
05/21/02   plm      Added volatile to sub_state field of the ISRTIM_CMD_BLK struct
05/10/02   jc       Add initial support for FEATURE_MULTIMODE_GSM
05/02/02   JC       Added num_above_sort_threshold_criteria variable to
                    Frequency_List_T for logging.
04/24/02   dp       Added ms_power_level_to_report and timing_advance_to_report
                    to l1_dedicated_data_T
03/27/02   JC       Added array of pointers to AGC_T's in
                    Frequency_List_T so that serving cell and NCELL
                    could use the same power measure handler.
                    Also, added paging group to paging_data_T for logging.
03/20/02   dp       Cleaned up dedicated_data_T
02/22/02   JC       Added support for RXLEV_SUB reporting.
02/20/02   dp       Added L1_DED_SEND_ACCESS_BURST handovers.  Removed
                    L1_DED_SYNC_SURR_CELL since it wasn't used
02/07/02   gw       Removed obsolete SCE declarations.
01/11/02   JC       Added systems algorithms for AGC.
12/21/01   gw       Changed sub_state enum to support DRX scheduling.
12/13/01   gw       Changed sub_sub_state enum to support message layer
                    acquisition.
11/26/01   mk       Added states to sub_state enum for Find BCCH,
                    Access, Idle modes.
09/28/01   jc       Cleaned up random_access_data_T .
09/25/01   mk       General SCE related cleanup.
08/31/01   JC       Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif
#include "sys_stru.h"
#include "sys_cnst.h"
#include "sys_type.h"     /* dBx16_T        */
#include "gl1_sys_algo.h" /* sys_algo_agc_T */
#include "rr_l1.h"
#include "rr_l1_g.h"        /* SCH_BLOCK_SIZE */
#include "l1_sc_int.h"    /* l1_sc_bs_T     */
#include "gs.h"           /* gs_status_T    */
#include "gpl1_gprs_if.h"
#include "gpl1_gprs_if_g.h" /* transfer_data_T    */
#include "l2_l1.h"
#include "l2_l1_g.h"
#ifdef FEATURE_GSM_AMR
#include "l1_amr.h"
#endif
#include "l1.h"

#include "gl1_hw.h"
#include "geran_dual_sim.h"

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
#include "trm.h"
#endif
#ifdef FEATURE_GSM_TO_LTE
#include "lte_as.h"
#include "lte_cphy_irat_meas_msg.h"
#endif
#ifdef FEATURE_GSM_TDS
#include "tdsiratif.h"
#endif

#ifdef FEATURE_G2T_SRCH_LOGGING
#include "log.h"
#include "log_codes.h"
#endif /* FEATURE_G2T_SRCH_LOGGING */

#include "gmacl1sig.h"

#include "msgr.h"
#include "geran_msg.h"
#include "rfgsm_msg.h"

#ifdef FEATURE_GSM_COEX_SW_CXM
#include "wwan_coex_mgr.h"
#endif

#ifdef FEATURE_WLAN_COEX_SW_CXM
#include "wcn_coex_mgr.h"
#endif

#include "trm.h"

#include "cfcm.h"

#include "qsh.h"

#ifndef NO_ERROR      /* Also defined in winerror.h */
#define NO_ERROR 0    /* moved from api/rr_l2.h where ifdefs are not allowed. */
#endif

/* Structure is used to store freq err correction info
 * after a success. combined acq for use in decoding BCCH
 * later on.
 */
typedef struct
{
  boolean  valid;
  int32    word1;
} t_init_acq;


/*
 *           LAYER 1 INTERNAL MESSAGES
 */

/****************************************************************
*                                                               *
*                isrtim scan cnf                                *
*                                                               *
*****************************************************************
*/


typedef struct
{
   IMH_T                           message_header;
   gas_id_t gas_id;
} isrtim_scan_cnf_T;

/****************************************************************
*                                                               *
*                isrtim first fcb cnf                            *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   boolean                          success;
   gas_id_t gas_id;
} isrtim_first_fcb_cnf_T;

/****************************************************************
*                                                               *
*                isrtim sch cnf                            *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   boolean                         success;
   byte                            sch_block[SCH_BLOCK_SIZE];
   gas_id_t gas_id;
} l1_sch_cnf_T;

/****************************************************************
*                                                               *
*                isrtim decode bcch list sch cnf                *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   boolean                         success;
   uint16                          qbit_lag;
   uint32                          fn_lag;
   uint8                           bsic;
} l1_decode_bcch_list_sch_cnf_T;

/****************************************************************
*                                                               *
*                isrtim decode bcch list bcch cnf               *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   boolean                         success;
} l1_decode_bcch_list_bcch_cnf_T;


/****************************************************************
*                                                               *
*                l1 rep period ind                            *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   gas_id_t gas_id;
} l1_rep_period_ind_T;

/****************************************************************
*                                                               *
*                isrtim handover cnf                                *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   gas_id_t gas_id;
} isrtim_handover_cnf_T;

/****************************************************************
*                                                               *
*                isrtim deact cnf_                                *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   boolean                         deep_sleep;
   gas_id_t gas_id;
} isrtim_deact_cnf_T;

#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
/****************************************************************
*                                                               *
*                isrtim suspend cnf_                                *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T                           message_header;
   uint32                          timer_value;
   gas_id_t gas_id;
} isrtim_suspend_cnf_T;
#endif

/****************************************************************
*                                                               *
*                call in task for surrounding cell              *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T   header;
   void ( *code )( void*, gas_id_t gas_id );
   void*   data;
   gas_id_t gas_id;
} l1_sc_call_in_task_T;

/****************************************************************
*                                                               *
*                call in task for panic reset scenario          *
*                                                               *
*****************************************************************
*/

/* PL1 error fatal\panic reset conditions */
typedef enum {
   L1_NO_ERROR,
   L1_STUCK_IN_NULL_MODE,
   L1_STUCK_IN_FIND_BCCH,
   L1_STUCK_IN_IDLE_MODE,
   L1_STUCK_IN_ACCESS_MODE,
   L1_STUCK_ON_BAD_CCH,
   L1_STUCK_ON_BAD_TCH,
   L1_STALLED_ISR,
   L1_ISR_OVERRUN,
   L1_NCELL_COUNT_OUT_OF_RANGE,
   L1_DUPLICATED_POINTERS_IN_SORTED_CELLS,
   L1_DUPLICATED_POINTERS_IN_BA,
   L1_PSI1_REPEAT_PERIOD_ZERO,
   L1_SPLIT_PAGE_CYCLE_ZERO,
   NPL1_PANIC,
   NPL1_WAKEUP_PANIC,
   RR_PANIC,
   L1_WRONG_SINGLE_BLK_SETUP
} l1_panic_reset_type_T;

typedef struct
{
   IMH_T                 header;
   l1_panic_reset_type_T error_code;
   gas_id_t gas_id;
} l1_panic_reset_call_in_task_T;


/****************************************************************
*                                                               *
*                L1_DO_TIME_PWR_AVG                             *
*                                                               *
*****************************************************************
*/
typedef struct
{
   IMH_T                 header;
   uint16                dummy;   /* That prevents static analysis error */
   gas_id_t gas_id;
} l1_do_time_avg_T;

/****************************************************************
*                                                               *
*                enter low power mode request                   *
*                                                               *
*****************************************************************
*/

typedef struct
{
   IMH_T message_header;
  gas_id_t gas_id;
} l1_enter_lpm_req_T;

/****************************************************************
*                                                               *
*                L1_DO_TIME_PWR_AVG                             *
*                                                               *
*****************************************************************
*/
typedef struct
{
  IMH_T    header;
  gas_id_t gas_id;
} l1_do_time_pwr_avg_T;


#ifdef FEATURE_QSH_EVENT_METRIC
/****************************************************************
*                                                               *
*                L1_QSH_EVENT_METRIC_IND                             *
*                                                               *
*****************************************************************
*/
typedef struct
{
   IMH_T                    header;
   gas_id_t                 gas_id;
   qsh_client_cb_params_s   cb_params;
} l1_qsh_metric_ind_T;
#endif /* FEATURE_QSH_EVENT_METRIC */

/****************************************************************
*                                                               *
*                L1_ISR_SB_FAILURE_CNF                          *
*                                                               *
*****************************************************************
*/
typedef struct
{

  IMH_T header;
  gas_id_t gas_id;
} t_l1_isr_single_blk_failure_cnf;

/****************************************************************
*                                                               *
*           MESSAGE TYPE DEFINITIONS                            *
*                                                               *
*****************************************************************
*/
/* ******************************************************* *
 * Please do not renumber this list!!! -- used by logging  *
 * Send changes to cfa.team                                *
 * ******************************************************* */
typedef enum
{
   ISR_SCAN_CNF                       = 0x00,
   ISR_FIRST_FCB_CNF                  = 0x01,
   ISR_SCH_CNF                        = 0x02,
   ISR_DECODE_BCCH_LIST_SCH_CNF       = 0x03,
   ISR_DECODE_BCCH_LIST_BCCH_CNF      = 0x04,
   L1_REP_PERIOD_IND                  = 0x05,
   ISR_HANDOVER_CNF                   = 0x06,
   L1_SC_CALL_IN_TASK_CNF             = 0x07,
   ISR_DEACT_CNF                      = 0x08,
   ISR_SUSPEND_CNF                    = 0x09,
   L1_PANIC_RESET_CALL_IN_TASK_CNF    = 0x0a,
   L1_ENTER_LPM_REQ                   = 0x10,
   MAC_L1_RELEASE_AND_RR_CMM_COMBINED = 0x20,
   L1_ISR_SINGLE_BLK_FAILURE_CNF      = 0x21,
   L1_GRM_REQ_AND_NOTIFY_ENH          = 0x22,
   L1_GRM_RELEASE                     = 0x23,
   L1_GRM_RESERVE_AT                  = 0x24,
   L1_MULTI_SIM_FW_RF_LOAD            = 0x25,
   L1_MULTI_SIM_PANIC_RESET           = 0x26,
   L1_GPLUSG_DEACTIVATE               = 0x27,
   L1_GRM_ABORT_CALLBACK              = 0x28,
   L1_GTA_ABORT_CALLBACK              = 0x29,
   L1_GRM_GRANT_CALLBACK              = 0x30,
   L1_DO_TIME_PWR_AVG                 = 0x31,
   L1_FUNC_CALL_IN_TASK               = 0x32,
   L1_FTM_DO_RXSWEEP_REQ              = 0x33,
   L1_GTA_ABORT_FOR_L_OR_T_CALLBACK   = 0x38,
   L1_HANDLE_RACH_TIMEOUT             = 0x39,
   L1_COMPLETE_DS_ABORT               = 0x3A

#ifdef   FEATURE_GSM_GPRS_READ_CCCH_IN_XFER
   ,L1_GRM_RESERVE_AT_CCCH            = 0x3B
#endif /* FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */

   ,L1_GRM_RELEASE_IN_CALLBACK        = 0x3C

#ifdef FEATURE_G2T_2PHASE_ACCESS
   ,L1_GTA_FOR_TDS_ABORT_CALLBACK      = 0x3D
#endif  // FEATURE_G2T_2PHASE_ACCESS
#ifdef FEATURE_GTA_DURING_PS_RACH
   , L1_GTA_DUR_RACH_PS_FOR_T_CALLBACK   = 0x3E
#endif /*FEATURE_GTA_DURING_PS_RACH*/

#ifdef   FEATURE_QSH_EVENT_METRIC
   ,L1_QSH_METRIC_IND                 = 0x40
#endif /* FEATURE_QSH_EVENT_METRIC */

} l1_l1_message_id_enum_T;

   /* ===========================                                          */
   /* Global Macros & Definitions                                          */
   /* ===========================                                          */

#ifdef MS_CONFIGURATION_P63
#error code not present
#else
#define ENTER_NO_INTERRUPTS
#define LEAVE_NO_INTERRUPTS
#endif

/* Macro to force a variable to be aligned to a 16-bit boundary */
#define ALIGN16 ALIGN(16)

#ifdef L1_HOST_TEST
#error code not present
#endif

/****************************************************************
*                                                               *
*           DIAGNOSTICS CONTROL                                 *
*                                                               *
*****************************************************************
*/

/*
 * Use Port 3 bit 1
 */


#define CLEAR_TDMA_DIAGNOSTIC_BIT   P2 &= ~0x08;
#define SET_TDMA_DIAGNOSTIC_BIT     P2 |= 0x08;

#define CLEAR_SW_TRIG_BIT           P2 &= ~0x10;
#define SET_SW_TRIG_BIT             P2 |= 0x10;

/*
 * set bit as output
 */

#define SETUP_TDMA_DIAGNOSTIC_BIT   DP2 |= 0x08;
#define SETUP_SW_TRIG_BIT           DP2 |= 0x10;


/****************************************************************
*                                                               *
*           SYSTEM PARAMETERS                                   *
*                                                               *
*****************************************************************
*/
#define L1_MIN_TCH_BLOC_QUAL   10
#define L1_MAIN_ISR_INT_TIME   7750

#define TIMESLOTS_IN_FRAME     8

#define L1_PERFECT_RXQUAL      32767
/*maximum full TDMA frame number*/
#define MULTIFRAME_26          26
#define MULTIFRAME_51          51
#define MULTIFRAME_52          52
#define FRAMES_IN_SUPERFRAME  (MULTIFRAME_26 * MULTIFRAME_51)
#define FRAMES_IN_HYPERFRAME  (2048 * FRAMES_IN_SUPERFRAME)
#define GSM_INVALID_BSIC      (255)
#ifdef COMMENT /* Chatlani */
#error code not present
#endif
typedef struct {

   int32  rx_power;    /* accumulator of dBx16_T powers */
   word   no_of_measurements;
   int32  rx_power_sub;
   int32  rx_power_val;
   word   no_of_measurements_val;
   int32  rx_power_block;
   word   no_of_measurements_block;
   uint8  num_received_blocks;
   word   no_of_measurements_sub;
   word   serving_cell_rx_qual;
   word   serving_cell_no_of_qual_measures;
   dBx16_T rx_power_instant;
   boolean SNR_is_bad;
   word   serving_cell_rx_qual_sub;
   word   serving_cell_no_of_qual_measures_sub;
   word   rx_gain_set;
   dBx16_T  snr;
}l1_serving_cell_meas_T;


/*****************************************************/
/* IMPORTANT:Please do not renumber this list!!!     */
/* It is used by logging. This enum is supposed to   */
/* match the events database, please notify gsm.diag */
/* of any proposed changes below.                    */
/*****************************************************/
/* Layer 1 Main States */
enum {
   /* INITIAL STATE(S) */
   L1_INACTIVE_MODE      = 10
  ,L1_NULL_MODE          = 11
  ,L1_SUSPEND_MODE       = 12
  ,L1_RESET_MODE         = 13
  ,L1_WTOG_BPLMN_MODE    = 14

   /* ACQ STATE(S) */
  ,L1_FIND_BCCH          = 20
  ,L1_FIND_SCH           = 21

   /* IDLE STATE(S) */
  ,L1_IDLE_MODE          = 30
  ,L1_GPRS_IDLE_MODE     = 31

   /* ACCESS STATE(S) */
  ,L1_RANDOM_ACCESS_MODE = 40
  ,L1_PACKET_ACCESS_MODE = 41

   /* DEDICATED STATE(S) */
  ,L1_DEDICATED_MODE     = 50
  ,L1_TRANSFER_MODE      = 51

#ifdef FEATURE_GSM_DTM
  /* DTM STATE */
  ,L1_DTM_MODE           = 52
#endif

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
  /* DUAL_SIM STATE(S) */
  ,L1_MULTI_SIM_NULL      = 60
  ,L1_MULTI_SIM_IDLE      = 61
  ,L1_MULTI_SIM_FIND_BCCH = 62
  ,L1_MULTI_SIM_INACTIVE
  ,L1_MULTI_SIM_DUMMY
  ,L1_MULTI_SIM_DECOUPLED
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */
};

/* Layer 1 Commands */
/* This enum is explicitly numbered just to aid debugging */
enum {
     L1_NULL_COMMAND             = 0,
     L1_ENTER_NULL_COMMAND       = 1,
     L1_FIND_BCCH_COMMAND        = 2,
     L1_FIND_SCH_COMMAND         = 3,
     L1_SCAN_COMMAND             = 4,
     L1_FIRST_FCB_COMMAND        = 5,
     L1_SCH_COMMAND              = 6,
     L1_ACQUIRE_COMMAND          = 7, /* Combined FCCH/SCH acquisition */
     L1_DECODE_BCCH_COMMAND      = 8,
     L1_DECODE_SCH_COMMAND       = 9,
     L1_IDLE_COMMAND             =10,
     L1_ACCESS_COMMAND           =11,
     L1_DEDICATED_COMMAND        =12,
     L1_DEDICATED_STOP           =13,
     L1_PRACH_COMMAND            =14,
     L1_TRANSFER_COMMAND         =15, /* packet transfer mode commands */
     L1_GPRS_IDLE_COMMAND        =16,
     L1_SET_CIPHERING_COMMAND    =17,
     L1_SYNCH_TO_TARGET_COMMAND  =18,
     L1_SYNCH_TO_CURRENT_COMMAND =19,
     L1_SET_HANDVR_CIPH_COMMAND  =20,
     L1_ACTIVATE_COMMAND         =21,
     L1_DEACTIVATE_COMMAND       =22,
     L1_GET_TIMING_COMMAND       =23,
     L1_ACQUIRE_BCCH_LIST_COMMAND=24,
     L1_DECODE_BCCH_LIST_COMMAND =25,
     L1_CM_HANDOVER_COMMAND      =26,
     L1_RAT_CHANGE_COMMAND       =27,
     L1_DEEP_SLEEP_COMMAND              =28,
     L1_READ_PBCCH_COMMAND              =29,
     L1_PBCCH_RESELECTION_COMMAND       =30,
     L1_ACQ_THEN_DECODE_PBCCH_COMMAND   =31,
     L1_DECODE_PBCCH_COMMAND            =32,
     L1_ACQUIRE_DURING_RESELECTION_COMMAND = 33,
     L1_SUSPEND_COMMAND       = 34,
     L1_WTOG_BPLMN_COMMAND    = 35,
     L1_RESUME_COMMAND        = 36

#ifdef FEATURE_GSM_DTM
     ,L1_DEDICATED_TO_DTM_COMMAND = 37
#ifdef FEATURE_GSM_EDTM
     ,L1_EDTM_RELEASE_COMMAND = 38
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GPRS_PS_HANDOVER
     ,L1_PSHO_COMMAND = 39
     ,L1_PSHO_REVERT_COMMAND = 40
#endif /* FEATURE_GPRS_PS_HANDOVER */



#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
     , L1_DEDICATED_COMMAND_WAIT_FOR_TRM  = 50
     , L1_TRANSFER_COMMAND_WAIT_FOR_TRM   = 51
     , L1_DS_RESELECTION_APPLY_TIME_CHANGE= 52
     , L1_TRANSFER_COMMAND_ABORTING_IDLE  = 53
     , L1_TRANSFER_COMMAND_WAIT_FOR_FW    = 54
     , L1_MULTI_SIM_IDLE_COMMAND          = 55
     , L1_WAIT_FOR_MDSP_TO_COMPLETE       = 56
     , L1_MULTI_SIM_TRM_ABORTING_COMMAND  = 57
     , L1_GET_TIMING_COMMAND_WAIT_FOR_TRM = 58
     , L1_CM_HANDOVER_COMMAND_WAIT_FOR_TRM= 59
     , L1_DECOUPLE_MULTI_SIM_COMMAND      = 60
#ifdef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
     , L1_WTOG_BPLMN_TRM_REQUEST_COMMAND  = 61
#endif /*FEATURE_WTOG_BACKGROUND_PLMN_SEARCH*/
     , L1_TRANSFER_COMMAND_IDLE_ABORTED   = 62
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */
     };

/* L1 sub-states */
/* This enum is explicitly numbered just to aid debugging */
enum {
   NULL_STATE                    = 0,
   L1_SCAN_START                 = 1,
   L1_SCAN_NEXT_MEAS_PERIOD      = 2,
   L1_SCAN_WAIT                  = 3,
   L1_SCAN_WAIT_NEXT_MEAS_PERIOD = 4,
   L1_SCAN_END                   = 5,
   L1_FCB_INITIALISE             = 6,
   L1_FCB_START                  = 7,
   L1_FCB_FIRST_SEARCH           = 8,
   L1_FCB_SEARCH                 = 9,
   L1_FCB_WAIT_ONE_FRAME         =10,
   L1_FCB_END                    =11,
   L1_SCH_INIT                   =12,
   L1_SCH_START                  =13,
   L1_SCH_FCB_WAIT               =14,
   L1_SCH_SEARCH_1               =15,
   L1_SCH_FIRST_FCB_SEARCH       =16,
   L1_SCH_FCB_SEARCH             =17,
   L1_SCH_END                    =18,
   L1_SCH_WAIT                   =19,
   L1_SCH_SEARCH                 =20,
   L1_BCCH_INIT                  =21,
   L1_BCCH_START                 =22,
   L1_BCCH_RUNNING               =23,
   L1_SCH_RUNNING                =24,
   L1_SCANNING                   =25,/* instead of ISRTIM_SCAN_REQ */
   L1_GETTING_FCB                =26,/* instead of ISRTIM_FIRST_FCB_REQ */
   L1_GETTING_SCH                =27,/* instead of ISRTIM_SCH_REQ */
   L1_ACQUIRING                  =28,/* Getting FCB and SCH on some ARFCN */
   L1_GETTING_BCCH               =29,/* instead of ISRTIM_CCH_UNI_DWN_REQ */
   L1_ACCESS_INIT                =30,
   L1_ACCESS_RUNNING             =31,
   L1_TRANSFER_INIT              =32,   /* new sub states for GPRS */
   L1_TRANSFER_SUSPEND           =33,
   L1_TRANSFER_RECONFIG          =34,
   L1_TRANSFER_STOP              =35,
   L1_FIXED_1                    =36,
   L1_FIXED_2                    =37,
   L1_DYNAMIC_1                  =38,
   L1_DYNAMIC_2                  =39,
   L1_DOWNLINK_1                 =40,
   L1_DOWNLINK_2                 =41,
   L1_READ_PBCCH                 =42,
   L1_SINGLE_BLK_TX              =43,
   L1_SINGLE_BLK_RX              =44,
   L1_INITIAL_PRACH              =45,
   L1_RETRY_PRACH                =46,
   L1_SEND_NO_PRACH              =47,
   L1_DED_INIT                   =48,
   L1_DED_WAIT                   =49,
   L1_DED_START                  =50,
   L1_DED_RUNNING                =51,
   L1_DED_FIRST                  =52,
   L1_DED_SECOND                 =53,
   L1_DED_PENDING                =54,
   L1_DED_BEFORE                 =55,
   L1_DED_AFTER                  =56,
   L1_DED_SUSPEND                =57,
   L1_IDLE_INIT                  =58,
   L1_IDLE_RECEIVING             =59,
   L1_IDLE_WAITING               =60,
   L1_BCCH_LIST_INIT             =61,
   L1_BCCH_LIST_START            =62,
   L1_BCCH_LIST_RUNNING          =63,
   L1_GETTING_BCCH_LIST          =64,
   L1_BCCH_LIST_WAITING          =65,
   L1_ABORT_PRACH                =66,
   L1_READ_PBCCH_ACQ             =67,
   L1_SCAN_WAIT_TO_RESUME_MEAS_PERIOD = 68,
   L1_SCAN_RESUME_MEAS_PERIOD = 69,
   L1_ACQUIRING_DURING_RESELECTION = 70
#ifdef FEATURE_GSM_DTM
   ,L1_DTM_INIT = 71,
   L1_DTM_RUNNING = 72,
   L1_DTM_ABORT = 73

#ifdef  FEATURE_GSM_EDTM
    , L1_EDTM_INIT = 74
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GPRS_PS_HANDOVER
   , L1_PSHO_RECONFIG    = 75
   , L1_PSHO_IN_PROGRESS = 76
#endif /* FEATURE_GPRS_PS_HANDOVER */

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
   ,L1_MULTI_SIM_POWER_SCAN          = 90
   ,L1_MULTI_SIM_RESELECTION
   ,L1_MULTI_SIM_AQUIRING
   ,L1_MULTI_SIM_AQUIRE_BCCH_LIST
   ,L1_MULTI_SIM_FIND_SCH

   ,L1_MULTI_SIM_PSCAN_STOPPED
   ,L1_MULTI_SIM_PSCAN_TRM_WAIT
   ,L1_MULTI_SIM_PSCAN_FW_LOAD_WAIT
   ,L1_MULTI_SIM_PSCAN_RUNNING
   ,L1_MULTI_SIM_PSCAN_ABORT
   ,L1_MULTI_SIM_PSCAN_SUSPENDING

   ,L1_MULTI_SIM_ACQ_BCCH_LIST_STOPPED
   ,L1_MULTI_SIM_ACQ_BCCH_LIST_TRM_WAIT
   ,L1_MULTI_SIM_ACQ_BCCH_LIST_FW_LOAD_WAIT
   ,L1_MULTI_SIM_ACQ_BCCH_LIST_RUNNING
   ,L1_MULTI_SIM_ACQ_BCCH_LIST_ABORT
   ,L1_MULTI_SIM_ACQ_BCCH_LIST_SUSPENDING

   ,L1_MULTI_SIM_ACQUIRING_STOPPED
   ,L1_MULTI_SIM_ACQUIRING_TRM_WAIT
   ,L1_MULTI_SIM_ACQUIRING_FW_LOAD_WAIT
   ,L1_MULTI_SIM_ACQUIRING_RUNNING
   ,L1_MULTI_SIM_ACQUIRING_ABORT
   ,L1_MULTI_SIM_ACQUIRING_SUSPENDING

   ,L1_MULTI_SIM_FIND_SCH_STOPPED
   ,L1_MULTI_SIM_FIND_SCH_TRM_WAIT
   ,L1_MULTI_SIM_FIND_SCH_FW_LOAD_WAIT
   ,L1_MULTI_SIM_FIND_SCH_RUNNING
   ,L1_MULTI_SIM_FIND_SCH_ABORT

   ,L1_MULTI_SIM_RESELECTION_STOPPED
   ,L1_MULTI_SIM_RESELECTION_TRM_WAIT
   ,L1_MULTI_SIM_RESELECTION_FW_LOAD_WAIT
   ,L1_MULTI_SIM_RESELECTION_RUNNING
   ,L1_MULTI_SIM_RESELECTION_ABORT
   ,L1_MULTI_SIM_RESELECTION_SUSPENDING
   ,L1_MULTI_SIM_RESELECTION_COUNTING_FRAMES

   ,L1_MULTI_SIM_QTA_IDLE_WAITING_TO_IDLE_RECEIVING_TRANS
   ,L1_MULTI_SIM_DECOUPLING
   ,L1_MULTI_SIM_DECOUPLED_WAIT_FOR_TRM_GRANT
   ,L1_MULTI_SIM_DECOUPLED_WAIT_FOR_FW_LOAD
#ifdef FEATURE_GTA_DURING_PS_RACH
   /*new sub states added to RACH state machine*/
   ,L1_ACCESS_GTA_SUSPENDING
   ,L1_ACCESS_GTA_TRM_WAIT
   ,L1_ACCESS_GTA_FW_LOAD_WAIT
   ,L1_ACCESS_GTA_INACTIVITY_TMR_EXPIRED
#endif /*FEATURE_GTA_DURING_PS_RACH*/
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */
};


/* L1 sub-sub-states */
/* This enum is explicitly numbered just to aid debugging */
enum {
   L1_NO_DL                 = 0,/* new sub sub states for GPRS */
   L1_DL_1                  = 1,
   L1_DL_2                  = 2,
/* Enumeration for defining various PBCCH sub-sub states */
   L1_PBCCH_INIT            = 3,
   L1_PBCCH_RECEIVING       = 4,
   L1_GPRS_IDLE_INIT        = 5,
   L1_GPRS_IDLE_RECEIVING   = 6,
   L1_GPRS_DRX_SLEEP        = 7,
   L1_GPRS_DRX_WAIT         = 8,
   L1_GET_LAST_DSP_BLOCKS   = 9,

 /* ----------------------- */
 /* L1_ACQUIRING sub-states */
 /* ----------------------- */
   L1_ACQ_INITIALISE              =15,
   L1_ACQ_START                   =16,
   L1_ACQ_WAITING                 =17,
   L1_ACQ_END                     =18,
   L1_ACQ_INITIALISE_FOR_INTERRAT =19,
   L1_ACQ_INITIALISE_BCCH_LIST    =20,
   L1_ACQ_INITIALISE_GSM_BLIND_HO =21

};

/* Power Measurement type */
enum {
   NORMAL_PWR_MEAS,
   SUB_PWR_MEAS,
   SACCH_PWR_MEAS
};


/* Holds the state of TRM interaction with L1 */
typedef enum{
  GL1_TRM_NULL,
  GL1_TRM_CALLBACK_PENDING,
  GL1_TRM_GRANTED,
  GL1_TRM_RETAINED_FOR_ACCESS,
  GL1_TRM_NOT_GRANTED,
  GL1_TRM_ABORTING
}gl1_trm_state_t;

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
/* Firmware load status types */
typedef enum {
  FW_LOAD_IN_PROGRESS = 0,
  FW_PENDING_RELEASE  = 1,
  FW_UNLOADED         = 2,
  FW_LOADED           = 3
}firmware_load_status_t;

/* The number of frames required to get the FW and RF loaded */
#define MULTI_SIM_FW_RF_STARTUP_TIME 4
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

/* Dedicated commands - used in control tables */
#define   L1_DED_NO_COMMAND      0
#define   L1_DED_IDLE            1
#define   L1_DED_READ_SDCCH      2
#define   L1_DED_READ_SACCH      4
#define   L1_DED_SEND_SDCCH      8
#define   L1_DED_SEND_SACCH      16
#define   L1_DED_SEND_ACCESS_BURST  32
#define   L1_FRAME_REPORTING     64
#define   L1_CORRECT_SYNC        128

/*special values for surrounding cell engine*/
#define   L1_SC_SDCCH4_FRAME_REPORTING  (37 - 1)
#define   L1_SC_SDCCH8_FRAME_REPORTING  (12 - 1)

#define   L1_DS_TRIGGER_RATE     5

typedef byte ded_command_T;


#define PGSM_MAXNO_ARFCNS 124 /* PGSM-900 */
#define PGSM_BASE_ARFCN   1

#define EGSM_MAXNO_ARFCNS 174 /* EGSM-900 */
#define EGSM_BASE_ARFCN   0

#define EGSM_MAX_ARFCN_LOWER 124
#define EGSM_MIN_ARFCN_UPPER 975
#define EGSM_MAX_ARFCN_UPPER 1023

#define DCS_MAXNO_ARFCNS  374 /* DCS-1800 */
#define DCS_BASE_ARFCN    512

#define PCS_MAXNO_ARFCNS  299 /* PCS-1900 */
#define PCS_BASE_ARFCN    512

#define GSM_850_MAXNO_ARFCNS  124 /* GSM-850 */
#define GSM_850_BASE_ARFCN    128

  #define BASE_ARFCN   EGSM_BASE_ARFCN

/* Two defines shared between l1_tch.c & l1_tch_amr.c for CR169424 */
/* This is used to delay the abort until 4 bursts have been received
 * and data passed back to L1. This means that by the time the
 * counter set by these values expires the FFA has already started the next
 * block receive and CANNOT ABORT UNTIL THIS COMPLETES. This allows
 * any FACCHSs to be TX'd (FACCH/FR is interleaved over 8 bursts, 6 for HR)
 */
#define L1_WAIT_FOUR_FR_BURSTS_BEFORE_ABORT 6
#define L1_WAIT_FOUR_HR_BURSTS_BEFORE_ABORT 10


/*
   RR_MAX_PSCAN_FREQ_SCAN + 1 to give us an extra item to use as the AGC
   element, so we don't overwrite any existin elemnt.  Will go away once
   acquiistion AGC is in place
*/
typedef struct {
   uint16           length;  /* number of active members */
   uint16           meas_per_ARFCN;                          /* number of measurements per carrier */
   boolean          min_meas_period;                         /* min duration of one measurement cycle */
   ARFCN_T          ARFCN[RR_MAX_PSCAN_FREQ_SCAN + 1];       /* array of ARFCNs */
   sys_algo_agc_T   agc[RR_MAX_PSCAN_FREQ_SCAN + 1];         /* array of agc structs */
   boolean          yielded[RR_MAX_PSCAN_FREQ_SCAN + 1];     /* array of yielded monitors */
   sys_algo_agc_T   *agc_ptrs[RR_MAX_PSCAN_FREQ_SCAN + 1];   /* For convenience of NCELL API */
   uint8            no_of_measurement[RR_MAX_PSCAN_FREQ_SCAN + 1];
   dBx16_T          power_dBm_x16[RR_MAX_PSCAN_FREQ_SCAN + 1]; /* accumulation buffer */
   uint16           sorted_array_indexes[RR_MAX_PSCAN_FREQ_SCAN + 1]; /* indexes of ARFCN/AGC/power in sorted order */
   uint16           num_above_sort_threshold_criteria;


   /* RR provides an array of struct to put the results in. */
   rr_l1_pscan_results_T *results;
} Frequency_List_T;

typedef struct {
   uint16           length;  /* number of active members */
   uint16           num_valid_carriers;
   ARFCN_T          arfcn[ RR_MAX_CANDIDATE_FREQ_LIST ];
   sys_algo_agc_T   agc[ RR_MAX_CANDIDATE_FREQ_LIST ];
   sys_algo_agc_T   *agc_ptrs[ RR_MAX_CANDIDATE_FREQ_LIST];
   boolean          sch_decoded[ RR_MAX_CANDIDATE_FREQ_LIST ];
   boolean          bcch_decoded[ RR_MAX_CANDIDATE_FREQ_LIST ];
   boolean          bcch_failed_cnt[ RR_MAX_CANDIDATE_FREQ_LIST ];
   boolean          bcch_blocked_cnt[ RR_MAX_CANDIDATE_FREQ_LIST ];
   uint32           prev_bcch_fn[ RR_MAX_CANDIDATE_FREQ_LIST ];
   uint32           next_bcch_fn[ RR_MAX_CANDIDATE_FREQ_LIST ];
   uint16           qbit_lag[ RR_MAX_CANDIDATE_FREQ_LIST ]; /*Frame offset in qs */
   uint32           fn_lag[ RR_MAX_CANDIDATE_FREQ_LIST ]; /* FN lag */
   uint8            bsic[ RR_MAX_CANDIDATE_FREQ_LIST ]; /* BSIC */
   boolean          all_arfcns_done; /* If all ARFCNs have been attempted */
   uint16           current_sch_index; /* Index of ARFCN whose SCH is currently scheduled */
   uint16           current_bcch_index; /* Index of ARFCN whose BCCH is currently scheduled */

   t_init_acq      init_acq_afc[ RR_MAX_CANDIDATE_FREQ_LIST ]; /* frequency offset correction info */

   boolean          sch_found;

   boolean          si3_only;
} BCCH_List_T;

/* IMSI */
#define GL1_SIM_IMSI_SIZE  L1_RR_SIM_IMSI_SIZE
#define GL1_SIM_TMSI_SIZE  L1_RR_SIM_TMSI_SIZE
#define GL1_SIM_PTMSI_SIZE L1_RR_SIM_PTMSI_SIZE

typedef struct {
   boolean          imsi_valid;
   imsi_mod_1000_T  imsi_mod_1000;
   boolean          updated;
   uint8            imsi[GL1_SIM_IMSI_SIZE];
   boolean          imsi_full_valid;
   uint8            tmsi[GL1_SIM_TMSI_SIZE];
   boolean          tmsi_valid;
   uint8            ptmsi[GL1_SIM_PTMSI_SIZE];
   boolean          ptmsi_valid;
   rr_l1_epd_preference_t epd_pref;
   }imsi_T;

/* Paging data - pre calculated */
typedef struct {
   byte    paging_multiframe;
   byte    paging_block_index;
   boolean paging_data_valid;
   boolean  all_paging_blocks;
   byte   paging_mode;
   byte   paging_group;
   byte   ccch_group;
   byte   pwr_av_freq;
   byte   prev_paging_mode;
   byte   ext_paging_multiframe;
   byte   ext_paging_block_index;
   } paging_data_T;

/* Data for Random Access Bursts (Channel Request messages) */
typedef struct {
   uint8  random_access_content;
   uint8  ra_delay; /* delay for the channel req */
   int16  txlev;
   uint16 ra_content;
   boolean egprs_ra;
   uint8   egprs_tsc;
  boolean                    pwr_reduction_valid;
  uint8                      pwr_reduction_value;
  boolean                    emergency_call;
#ifdef FEATURE_GTA_DURING_PS_RACH
  boolean  rach_for_ps_service;
  uint32   suspension_FN_GTA;
  uint16   updated_ra_delay;
#endif
   }random_access_data_T;

/* Values for serving cell BCCH mask - used in Idle Mode
 * the mask has a 1 in the bit position(s) corresponding to
 * the TC value where the SI is present.  E.g SI 2 is
 * present when TC = 1 so the mask is (1 << 1).
 */

/* Normal BCCH */
#define SERV_BCCH_MASK_NONE           0
#define SERV_BCCH_MASK_1              (1uL<<0)
#define SERV_BCCH_MASK_2              (1uL<<1)
#define SERV_BCCH_MASK_2BIS           (1uL<<5)
#define SERV_BCCH_MASK_2TER           ((1uL<<4) | (1uL<<5))
#define SERV_BCCH_MASK_2QUATER        ((1uL<<4) | (1uL<<5))
#define SERV_BCCH_MASK_3              ((1uL<<2) | (1uL<<6))
#define SERV_BCCH_MASK_4              ((1uL<<3) | (1uL<<7))
#define SERV_BCCH_MASK_9              (1uL<<4)
#define SERV_BCCH_MASK_13             (1uL<<4)
#define SERV_BCCH_MASK_13ALT          (1uL<<4)
#define SERV_BCCH_MASK_15             (1uL<<4)
#define SERV_BCCH_MASK_ALL            0x000000ff

/* Extended BCCH */
#define SERV_EXT_BCCH_MASK_NONE       0
#define SERV_EXT_BCCH_MASK_2QUATER    (1uL << 5)
#define SERV_EXT_BCCH_MASK_7          (1uL << 7)
#define SERV_EXT_BCCH_MASK_8          (1uL << 3)
#define SERV_EXT_BCCH_MASK_13         (1uL << 0)
#define SERV_EXT_BCCH_MASK_13ALT      (1uL << 0)
#define SERV_EXT_BCCH_MASK_15         (1uL << 1)
#define SERV_EXT_BCCH_MASK_16         (1uL << 6)
#define SERV_EXT_BCCH_MASK_17         (1uL << 2)
#define SERV_EXT_BCCH_MASK_ALL        0x000000ff

#define FRAMES_PER_BLOCK 4

typedef enum
{
   NO_HPLMN_SEARCH,
   POWER_SCAN,
   ACQ_BCCH_LIST
}background_HPLMN_activity_type;


#ifdef FEATURE_GSM_DTM

/* Enum for determining slam mechanism used in dedicated mode */
typedef enum
{
  L1_CS_TN_SLAM_LOWEST,
  L1_CS_TN_SLAM_TN2
}l1_dtm_slam_mode_t;

#endif
/* These defines are copied from rr_defs.h and allow
 * to tell when to break Loopback C permanently
 */
#define TEST_MANAGEMENT_L1      0x0F
#define OPEN_LOOP_CMD_L1        0x06
#define OPEN_LOOP_CMD_ACK_L1    0x81

typedef enum
{
    LOOPBACK_C_OPEN,
    LOOPBACK_C_CLOSED,
    LOOPBACK_C_TEMP_OPEN
} l1_loopback_state_T;


/* L1 Idle Mode Data */
typedef struct {
   layer1_sys_info_T         campedon_cell_data;
   ARFCN_T                   campedon_cell_ARFCN;
   byte                      campedon_cell_BSIC;
   sys_algo_agc_T            campedon_cell_agc;
#ifdef FEATURE_GSM_RX_DIVERSITY_IDLE
   sys_algo_agc_T            campedon_cell_agc_divrx;
#endif /*FEATURE_GSM_RX_DIVERSITY_IDLE*/
   
#ifdef FEATURE_IDLE_DRX_SCALING
   /* Data updated from the MPH_UPDATE_IDLE_DRX_IND from RR */
   byte                      extended_BS_PA_MFRMS; /* scaling factor (skip every n'th page) */
   boolean                   extended_BS_PA_MFRMS_valid; /* DRX scaling enabled ie skip PCHs */
   boolean                   RR_idle_mode_meas_enabled; /* TRUE => RR has not disabled meas */
   boolean                   L1_ready_for_sc_start; /* TRUE => L1 is ready to start SCE, if 
                                                       RR_idle_mode_meas_enabled is also TRUE 
                                                       then SCE start should occur */
   boolean                   L1_ready_for_sc_go; /* TRUE => L1 is ready for SCE go, if 
                                                    RR_idle_mode_meas_enabled is also TRUE 
                                                    then SCE go should be called */
   l1_sc_mode_T              L1_required_mode; /* when L1_ready_for_sc_start==TRUE, 
                                                  this shows what mode is required by L1 */
#endif /* FEATURE_IDLE_DRX_SCALING */
   imsi_T                    imsi_data;
   paging_data_T             paging_data;
   random_access_data_T      random_access_data;
   rr_l1_si_type_T           si_mask;
   uint32                    BCCH_mask;
   uint32                    ext_BCCH_mask;
   ccch_gprs_idle_params_t   layer1_extra_gprs_sys_info; /* CCCH gprs extra params */
   boolean                   starting_time_valid;
   uint32                    starting_time;
   uint8                     non_drx_timer;
   uint8                     drx_timer_max;
   boolean                   mm_non_drx_mode;
   boolean                   cancel_non_drx;
   mph_gprs_reselection_req_t reselection_params_store;
   boolean                    reselection_pending;
   boolean                    reselection_apply_freq_correction;
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
   boolean                    sc_stop_pending;
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

   running_avg_struct        campedon_cell_pwr_meas_avg;

   background_HPLMN_activity_type  background_HPLMN_search_activity;
   boolean                   first_ext_pch_block_read;
   boolean                    pwr_reduction_valid;
   uint8                      pwr_reduction_value;
   boolean                    emergency_call;

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
   boolean                    idle_rx_missed_page;
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */
   uint8                      idle_power_opt;
}idle_data_T;




/* GPRS Paging data */
typedef struct {
   uint32    gprs_paging_multiframe[16][5];
   uint8     gprs_current_paging_mode;
   uint8     gprs_stored_paging_mode;
   uint32    gprs_pccch_group;
   uint32    gprs_paging_group[16];
   uint32     gprs_paging_block;
   uint32     gprs_ext_paging_block;
} gprs_paging_data_t;

/* GPRS L1 Idle Mode Data */
typedef struct {
   pccch_idle_mode_params_t   gprs_campedon_cell_data;
   ARFCN_T                    gprs_campedon_cell_arfcn;
   uint8                      gprs_campedon_cell_bsic;
   uint8                      psi1_repeat_period;
   imsi_T                     imsi_data;
   gprs_paging_data_t         gprs_paging_data;
   uint8                      nco;
   sint15                     nc_non_drx_period;
   boolean                    mm_non_drx_mode;
   channel_t                  ccch_or_pccch;
   boolean                    update_pending;
   boolean                    start_time_valid;
   boolean                    prog;
   sint7                      all_bursts_sent;
   mph_gprs_reselection_req_t pbcch_reselection_params_store;
   boolean                    pbcch_reselection_pending;
   gprs_surround_data_t  gprs_surround_cell_data;

} gprs_l1_idle_data_t;


/* Layer 1 input message buffers */
/*********************************/
/* SDCCH transmit message buffer */

#define MAXNO_BUFFERED_DCCH_MESSAGES  1
#define MAXNO_BUFFERED_SACCH_MESSAGES  1

/* then :- */
#define SACCH_L1_HEADER  2
#define DCCH_MESSAGE_UNIT N201_MAX
#define SACCH_MESSAGE_UNIT (N201_MAX - SACCH_L1_HEADER)
#define DCCH_BUFFER_SIZE  (MAXNO_BUFFERED_DCCH_MESSAGES*DCCH_MESSAGE_UNIT +1)
#define SACCH_BUFFER_SIZE (MAXNO_BUFFERED_SACCH_MESSAGES*SACCH_MESSAGE_UNIT + SACCH_L1_HEADER)
/* The next definition is in units of words! */
#define V110_BUFFER_SIZE  15

/* note: input_data_pointer points at the next available free byte
         output_data_pointer points at the first byte of the oldest
         untransmitted message in the buffer */

typedef struct {
   boolean    data_valid;
   byte   dcch_data[DCCH_BUFFER_SIZE];
   } l1_dcch_buffer_T;

typedef struct {
   boolean    data_valid;
   byte   sacch_data[SACCH_BUFFER_SIZE];
   } l1_sacch_buffer_T;

#define L1_SACCH_HDR_SRO_BIT            0x40
#define L1_SACCH_HDR_LEN                2
#define L1_SACCH_MESSAGE_LEN_23_OCT     SACCH_BUFFER_SIZE
#define L1_SACCH_SRO_BIT_RIGHT_SHIFT    6

typedef struct {
   l2_l1_repetition_T l2_repetition_info;
   boolean            active;
   boolean            network_initiated;
   boolean            prev_crc;
   boolean            srr_flag;
   boolean            sro_flag;
   boolean            nv_flag;
   boolean            nv_active;
   boolean            buffer_valid;
   uint8              buffer[SACCH_BUFFER_SIZE];
   } l1_r_sacch_ctrl_T;

typedef struct {
 /* General Handover Details */
 rr_l1_handover_type_T handover_type;
 ARFCN_T               BCCH_ARFCN;
 byte                  BSIC;

 /* Used for TA and QBIT_LAG calculation */
 hand_sync_ind_T       handover_synchronisation;
 boolean               timing_advance_valid;
 byte                  timing_advance;
 boolean               real_time_difference_valid;
 byte                  real_time_difference;
 byte                  NCI;

 /* Used for vocoder blanking */
 byte                  old_channel_mode;
 byte                  new_channel_mode;
} l1_ho_details_T;

#ifdef FEATURE_GSM_AMR
typedef struct {
  gl1_msg_amr_mode_type dl_acs[MAX_CODEC_MODES];
  gl1_msg_amr_mode_type ul_acs[MAX_CODEC_MODES];
  uint8                 thresh_tbl[MAX_CODEC_MODES - 1];
  uint8                 hyst_tbl[MAX_CODEC_MODES - 1];
  uint8                 dl_icm;
  uint8                 ul_icm;
  cmi_phase_type        dl_phase;
  uint8                 dl_num_acm;
  uint8                 ul_num_acm;
} l1_amr_details_T;
#endif

typedef struct
{
  boolean gl1_inter_handover_in_progress;
  boolean gl1_inter_handover_fail;
} handover_information_T;

typedef struct {
   byte                      cipher_key[CIPHER_KEY_LENGTH];
   byte                      cipher_key_128[CIPHER_KEY_128_LENGTH];
   channel_spec_T            channel_specification;
   channel_spec_T            new_channel_specification;
   sys_algo_agc_T            c0_agc;
#ifdef FEATURE_GSM_RX_DIVERSITY
   sys_algo_agc_T            agc_divrx;
#endif
   sys_algo_agc_T            channel_agc;
   byte                      ordered_ms_power_level;
   byte                      ms_power_change_time;
   byte                      ms_power_level_to_use;
   byte                      ms_power_level_in_use;
   byte                      ms_power_level_to_report;
   byte                      ordered_timing_advance;
   byte                      timing_advance_to_use;
   byte                      timing_advance_in_use;
   byte                      timing_advance_to_report;
   byte                      last_cell_timing_advance;
   byte                      last_cell_tx_power;
   dBx16_T                   last_cell_rx_power;
   dword                     observed_timing_difference;
   boolean                   stop_meas_rpt; /* Indicates if SACCH meas reports
                                               should be sent or not */
   l1_dcch_buffer_T          *l1_dcch_s0_buffer_ptr;  /* set in L1_initialise_L1_data */
   l1_sacch_buffer_T         *l1_sacch_s0_buffer_ptr; /*  ditto   */
   byte                      L1_confirm_msg;
   byte                      no_of_ab_to_send;
   byte                      handover_synchronization;
   uint16                    access_burst_content;
   l1_sc_bs_T                target_cell_details;
   l1_sc_bs_T                current_cell_details;
   l1_sc_bs_T                previous_cell_details;
   byte                      dsp_tch_init;
   boolean                   ds_synchronised;
   boolean                   ds_tx_first_half;
   byte                      ds_mode;
   boolean                   ds_mode_change;
   boolean                   freq_redef_imminent;
   word                      freq_redef_starting_time;
   word                      starting_time;
   word                      new_starting_time;
   channel_information_T     new_channel_info;
   byte                      previous_chnl_type;
   l1_ho_details_T           ho_details;
#ifdef FEATURE_GSM_AMR
   multirate_config_T        multirate_configuration;
   multirate_config_T        new_multirate_configuration;
   l1_amr_details_T          amr_details;
#endif
   /*Used to indicate if if the stating time had expired on the old cell*/
   boolean                   previous_cell_on_after_channels;
   l1_loopback_state_T       l1_loopback_disabled;
#ifdef FEATURE_GSM_DTM
  /* slam mode for use in dedicated */
  l1_dtm_slam_mode_t         l1_dtm_slam_mode;
#endif
  l1_r_sacch_ctrl_T          rsacch_ctrl;
#ifdef FEATURE_GSM_DYNAMIC_SAIC
  boolean                    l1_saic_dis_ena_state;
#endif
  /* These new variables are now used to hold the old PDM and Rotator values then they can
  be restored after a handover failure */
  int32                      previous_cell_freq_value;
  int32                      previous_cell_rotator_value;
  boolean                    previous_cell_afc_valid;
  handover_information_T     handover_info;
#ifdef FEATURE_VAMOS_II
  boolean                    vamos2_ind;
#endif
#ifdef FEATURE_GSM_RX_DIVERSITY
  byte                       prev_rxqual;
  boolean                    l1_enable_rxdiversity;
#endif
}dedicated_data_T;




#ifdef FEATURE_GSM_DTM

/**********************************************/
/* Defines for the DTM support                */
/**********************************************/
typedef enum
{
  L1_DTM_NONE,
  L1_DTM_PS_ONLY,
  L1_DTM_CS_PS,
  L1_DTM_CS_ONLY
} l1_dtm_mode_T;

/* States within the l1_dtm_mode state machine in l1_isr.c */
typedef enum{
  L1_DTM_NULL,
  L1_DTM_RUN,
  L1_DTM_PS_RECONFIG,
  L1_DTM_CS_RECONFIG,
  L1_DTM_CS_PS_RECONFIG,
  L1_DTM_CS_PS_ABORT
#ifdef FEATURE_GSM_EDTM
  ,L1_DTM_CS_ABORT
  ,L1_DTM_CS_ABORTED_WAIT_FOR_END_OF_BLOCK
  ,L1_DTM_CS_ABORT_PS_RECONFIG
#endif
}l1_dtm_mode_state_t;

/* Events for the l1_dtm_mode state machine in l1_isr.c */
typedef enum{
  L1_DTM_NULL_COMMAND,
  L1_DTM_ACTIVATE,
  L1_DTM_STOP,
  L1_DTM_TRANSFER_STOP,
  L1_DTM_DEDICATED_RECONFIG,
  L1_DTM_TRANSFER_RECONFIG,
  L1_DTM_RECONFIG_BOTH_CS_PS,
  L1_DTM_RESET,
  L1_DTM_CANCEL_PS_RECONFIG
#ifdef FEATURE_GSM_EDTM
  ,L1_DTM_ENHANCED_RELEASE
#endif
}l1_dtm_command_event_t;

/* Contains configuration data for Dual Transfer Mode */
typedef struct
{
  l1_dtm_mode_T       l1_dtm_abort_mode;
  l1_dtm_mode_T       l1_dtm_init_mode;
  dedicated_data_T    *Ded_ptr;
  transfer_data_T     *Transfer_ptr;

} dtm_data_T;

/*DTM specific #define's*/
#define DTM_PS_RECONFIG_PENDING(gas_id) ((l1_tsk_buffer[gas_id].l1_state == L1_DTM_MODE) && \
 ((get_dtm_isr_state(gas_id) == L1_DTM_PS_RECONFIG) || \
  (get_dtm_isr_state(gas_id) == L1_DTM_CS_PS_RECONFIG)))\

#define GPRS_PS_RECONFIG_PENDING (L1_TRANSFER_COMMAND == l1_tskisr_blk->main_command)

#define PS_RELEASE_CNF_REQUIRED_BY_ISR(gas_id) \
  ((l1_transfer_data[(gas_id)].confirm_pending == SEND_IN_ISR) && \
   (l1_transfer_data[(gas_id)].release_pending != L2_NO_RELEASE))\

#endif /* FEATURE_GSM_DTM */

/* Internal L1 primitives */
typedef enum
{
   L1_NULL_CNF,
   L1_DECODE_BCCH_LIST_CNF,
   L1_POWER_SCAN_CNF,
   L1_DEACTIVATE_BPLMN_CNF
}l1_cnf_type;

/*
 * Definitions of different scenario-specific parameters
 * passed to L1 ISR shared memory
 */

typedef struct {

   Frequency_List_T  *pScan_list;
   sys_algo_agc_T    *pAgc_data;
   idle_data_T       *pIdle_data;
   dedicated_data_T  *pDedi_data;
   transfer_data_T   *pTransfer_data;

#ifdef FEATURE_GSM_DTM
   dtm_data_T        *pDtm_data;
#endif

} L1_data_pointer;

typedef struct {

   L1_data_pointer L1Data;
   uint8 frames_for_FCBdetect;
   uint8 frames_for_SCHdecode;
   uint8 max_SCH_attempts;
   uint16 BSIC;
   uint32 extended_BCCH_mask;
   int16 QB_lag;
   dBx16_T rx_power;
   boolean reselection_cnf_reqd;
   uint32 frame_lag;
   uint16 ARFCN_index;
   ARFCN_T ARFCN;
   l1_cnf_type L1_confirm_ID;
   /*New flag bsic_expected is assigned on MPH_SELECT_SPECIFIC_BCCH_REQ
     and in CNF if we find a different BSIC, then Gl1 must send a FAILure in SPECIFIC_BCCH_CNF*/
   uint8 bsic_expected;
} l1_isrtsk_blk_params_T;


/*
 * Definitions of the shared memory
 */

typedef struct {
   byte                       lockout;
   byte                       main_command;
   byte                       command;
   byte                       l1_state;
   volatile byte              sub_state;
   byte                       sub_sub_state;
   byte                       pscan_state;
   byte                       acq_bcch_state;
   trm_client_enum_t          client_id;
   gl1_trm_state_t            trm_status;
   gl1_trm_state_t            rx_div_trm_status;
   trm_grant_return_enum_type trm_grant_status;
   uint32                     trm_timestamp_on;

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
   byte                       l1_prev_state;
   byte                       multi_sim_pscan_state;
   byte                       multi_sim_acq_bcch_list_state;
   byte                       multi_sim_acquiring_state;
   byte                       multi_sim_find_sch_state;
   byte                       multi_sim_reselection_state;
   byte                       multi_sim_decoupled_state;

   uint32                     trm_grant_fn;
   uint32                     trm_grant_frames;
   uint32                     trm_reserve_fn;
   uint32                     frame_duration;
   boolean                    firmware_loaded;
   boolean                    trm_requested_at_warmup;
   boolean                    first_multi_sim_init;
   firmware_load_status_t     firmware_load_status;
   boolean                    firmware_unload_in_progress;
   boolean                    going_active; /* Transitioning from Inactive to Active */
   boolean                    clash;

#ifdef FEATURE_QTA
   boolean                    qta_in_progress;
   boolean                    skip_qta_gap;
#endif /* FEATURE_QTA */

   boolean                    reservation_for_bcch;
   boolean                    trm_release_required;
   boolean                    rf_in_deep_sleep_whilst_forced_wakeup;
   boolean                    g2x_resel_in_progress;
   boolean                    stop_gsm_mode_cnf_sent;
   boolean                    rach_ready_to_send;
#ifdef FEATURE_DUAL_DATA
   geran_priority_t priority;
#endif
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */
   boolean                    l1_isr_called_from_sleep_warmup_isr;
   boolean                    rf_wakeup_pending;
    uint32                    rf_wakeup_pending_fn;
   boolean                    time_slot_change_pending;
   l1_isrtsk_blk_params_T     next_params;
   l1_isrtsk_blk_params_T     current_params;
} ISRTIM_CMD_BLK;


#ifdef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
#define MIN_FRAMES_FOR_L1_TO_SUSPEND 4

typedef enum
{
   WTOG_BPLMN_SUSPENDED,
   WTOG_BPLMN_ACTIVE,
   WTOG_BPLMN_RUNNING
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
   ,WTOG_BPLMN_TRM_REQUEST
   ,WTOG_BPLMN_TRM_WAIT
   ,WTOG_BPLMN_RF_FW_LOADING
   ,WTOG_BPLMN_MULTI_SIM_SUSPENDING
   ,WTOG_BPLMN_MULTI_SIM_ENDING
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */
} l1_wtog_bplmn_sub_state_T;

typedef struct {

   background_HPLMN_activity_type state;
   l1_wtog_bplmn_sub_state_T      sub_state;
} l1_wtog_bplmn_struct_T;

#endif

#ifdef FEATURE_GSM_GPRS_TX_INFO
typedef struct
{
    boolean     tx_active;
    ARFCN_T     arfcn;
} l1_store_tx_params_T;
#endif

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )

typedef struct
{
   IMH_T                           message_header;
   gas_id_t                        gas_id;
} gl1_rach_timeout_callback_t;
#endif /* defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM ) */


/************************************************************
 *
 *   Globals
 *
 ************************************************************/

/* Globals for SCE */
extern boolean l1_get_serv_pwr_meas_permitted( gas_id_t gas_id );
extern void l1_set_serv_pwr_meas_permitted(boolean serv_pwr_meas_permitted,gas_id_t gas_id );


#ifdef FEATURE_GSM_DTM
/* DTM event variable, responsible for l1_dtm_mode state machine transitions */
extern void l1_set_dtm_command_event(l1_dtm_command_event_t dtm_command_event,gas_id_t gas_id);
extern l1_dtm_command_event_t l1_get_dtm_command_event( gas_id_t gas_id );
extern l1_dtm_mode_state_t get_dtm_isr_state( gas_id_t gas_id );
#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
void l1_init_idle_data( gas_id_t gas_id );
void l1_init_dedicated_data( gas_id_t gas_id );
ARFCN_T gl1_get_serving_cell_arfcn( gas_id_t gas_id );
dBx16_T gl1_get_serving_cell_rx_power( gas_id_t gas_id );
uint8   gl1_get_serving_cell_BSIC( gas_id_t gas_id );
void gpl1_gprs_setup_idle_non_drx( gas_id_t gas_id );

gs_status_T gl1_set_sacch_tx_pwr_control( boolean tx_pwr_control,
                                          gas_id_t gas_id );
boolean     gl1_sacch_tx_pwr_control_is_active( gas_id_t gas_id );

boolean gl1_cell_valid_state_check( const byte gl1_state );

void  gl1_mutex_lock   ( gas_id_t gas_id );
void  gl1_mutex_unlock ( gas_id_t gas_id );

/*************************************************

   Prototypes of functions common to the L1 task
   and the L1 ISR.

   These are located in the file m10200.c
***************************************************/
void        L1_send_MPH_SURROUND_MEAS_IND( gas_id_t gas_id );
void        L1_send_MPH_NC_MEASUREMENT_IND(gas_id_t gas_id);
void        L1_send_MPH_SURROUND_UPDATE_FAILURE_IND( ARFCN_T ARFCN, boolean state_swap, gas_id_t gas_id );

void        L1_send_PH_CONNECT_IND(l2_channel_type_T chnl_type, gas_id_t gas_id);
void        L1_send_PH_READY_TO_SEND_IND(l2_channel_type_T chnl_type, gas_id_t gas_id);
gs_status_T L1_send_message ( gs_queue_id_T   queue_id,
                                 void       *message_p,
                                 boolean         wait
                              , gas_id_t gas_id );
gs_status_T L1_isr_send_message ( gs_queue_id_T   queue_id,
                                 void       *message_p,
                                 boolean         wait
                                  , gas_id_t gas_id);
void        L1_send_MPH_SERVING_IDLE_MEAS_IND( byte, gas_id_t gas_id );
void        L1_send_MPH_SERVING_AUX_MEAS_IND( boolean own_paging_block, gas_id_t gas_id );
void        L1_send_MPH_READ_SYS_INFO_FAILURE_IND(boolean state_swap, boolean transfer, gas_id_t gas_id);

#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
void        L1_send_MPH_INTERRAT_LIST_SRCH_TIME_IND(uint32 timer_value, gas_id_t gas_id);
void        L1_send_MPH_SUSPEND_GSM_MODE_CNF(uint32 timer_value, boolean suspended, gas_id_t gas_id);
void        L1_send_MPH_RESUME_GSM_MODE_CNF(gas_id_t gas_id);
#endif

void        L1_send_MPH_SERVING_MEAS_IND( byte );

void        L1_send_L1_ISR_SINGLE_BLK_FAILURE_CNF( gas_id_t gas_id );
void        L1_send_gprs_gmac_message(l1_mac_sig_t *common_msg_ptr, uint8 *pacch_data,uint32 FN, gas_id_t gas_id);

void        L1_setup_DEACTIVATE( IMH_T *msg_header );

void        L1_process_READ_SYS_INFO_REQ( mph_read_sys_info_req_T *message_ptr );
void        L1_process_STOP_READ_SYS_INFO_REQ( void );
void        L1_process_READ_SYS_INFO_REQ_TRANS( mph_read_sys_info_req_T *msg_header );
void        L1_process_STOP_READ_SYS_INFO_REQ_TRANS( void );

boolean     L1_setup_PSCAN( IMH_T *msg_header );
dBx16_T     L1_pscan_get_threshold(gas_id_t gas_id);
void        L1_pscan_set_threshold(dBx16_T new_threshold, gas_id_t gas_id);


void        L1_setup_START_IDLE( IMH_T *msg_header );
void        L1_setup_EARLY_CAMPING( IMH_T *msg_header );
void        L1_setup_DECODE_BCCH_LIST( IMH_T *msg_header );

#ifdef FEATURE_GSM_DTM
void l1_ded_store_ftn_info(gl1_msg_frame_tick_type ftn_type, gas_id_t gas_id);
gl1_msg_frame_tick_type l1_ded_get_ftn_info(gas_id_t gas_id);
#endif
void l1_ded_store_bad_sacch_block_cnt(int value, gas_id_t gas_id);
int l1_ded_get_bad_sacch_block_cnt(gas_id_t gas_id);



#ifdef FEATURE_GSM_GPRS_TX_INFO
void gl1_get_current_tx_info (l1_store_tx_params_T *params_ptr);
#endif

boolean l1_tch_is_rfacch_decode_frame(uint16 fn_mod42432, uint8* rfacch_buffer_index, gas_id_t gas_id);
void clear_rfacch_decode_frame(uint8 rfacch_buffer_index, gas_id_t gas_id);
void l1_tch_set_rfacch_fw_buffer_index(uint8 index, gas_id_t gas_id);
uint8 l1_tch_get_rfacch_fw_buffer_index(gas_id_t gas_id);

extern void l1_rsacch_init_params (boolean ded_active, gas_id_t gas_id);

typedef union
{
  msgr_hdr_struct_type              hdr;

  GfwHostAsyncRsp                   gfwHostAsyncRsp;

  GfwHostRfmEnterModeRsp            gfwHostRfmEnterModeRsp;

  GfwHostAppModeConfigRsp           gfwHostAppModeConfigRsp;

  GfwHostSleepRsp                   gfwHostSleepRsp;

  GfwHostAsyncSleepReadyCmd         gfwHostAsyncSleepReadyCmd;

#ifdef FEATURE_WLAN_COEX_SW_CXM
  GfwWlanCxmConfigRsp               gfwWlanCxmConfigRsp;
#endif
} u_geran_gl1_gfw_async_msgr;
typedef struct
{
  q_link_type       link;
  u_geran_gl1_gfw_async_msgr  msg;

} geran_gl1_gfw_async_msgr_t;

#define  GL1_GFW_ASYNC_MSGR_QUEUE_SIZE  15             /* Increase for G2L , GFW is crashing  */


#ifdef  FEATURE_GSM_TO_LTE

void  L1_send_LTE_CPHY_IRAT_MEAS_G2L_INIT_REQ( boolean  CM_mode, gas_id_t gas_id );


void  L1_send_LTE_CPHY_IRAT_MEAS_G2L_DEINIT_REQ( gas_id_t gas_id );

void  L1_send_LTE_CPHY_IRAT_MEAS_G2L_ABORT_REQ( gas_id_t gas_id );

void  l1_sc_lte_process_search_results( const lte_cphy_irat_meas_search_cnf_s*  msg_ptr , gas_id_t gas_id);

void  l1_sc_lte_process_meas_results( const lte_cphy_irat_meas_meas_cnf_s*  msg_ptr, gas_id_t gas_id );

boolean L1_send_LTE_CPHY_IRAT_MEAS_G2L_TIMED_SRCH_MEAS_REQ( uint16 freq_index, boolean ml1_gap_init, lte_earfcn_t nextFreq, gas_id_t gas_id, boolean gap_start_asap);

void    l1_sc_lte_process_timed_search_results(lte_cphy_irat_meas_timed_srch_meas_cnf_s*  msg_ptr, gas_id_t gas_id  );

#endif /* FEATURE_GSM_TO_LTE  */

#ifdef FEATURE_GSM_TDS

void  L1_send_TDS_IRAT_INIT_CMD (void);

void  L1_send_TDS_IRAT_BUILD_RF_SCRIPTS_CMD (gas_id_t gas_id);

void  L1_send_TDS_IRAT_STARTUP_REQ (gsm_l1_tds_gap_info_type *gap_info_ptr, gas_id_t gas_id);

void  L1_send_TDS_IRAT_ACQ_REQ (const tdsirat_acq_list_type * acq_list_ptr, gas_id_t gas_id);

void  L1_send_TDS_IRAT_MEAS_REQ (const tdsirat_freq_list_type * meas_list_ptr, gas_id_t gas_id);

void  L1_send_TDS_IRAT_STOP_CMD (gas_id_t gas_id);

void  L1_send_TDS_RSSI_MEAS_RSP ( gl1_hw_x2g_rssi_meas_ctl_t *ptr );

void L1_send_TDS_G2T_ABORT_REQ (gas_id_t gas_id);

void L1_send_TDS_ACQ_RSP( gsm_irat_acquisition_rsp_t *ptr  );


void L1_send_TDS_SCH_MEAS_RSP( gsm_irat_sch_burst_rsp_t*  sch_store,
                               gl1_defs_rx_hdr_struct*    sch_hdr_ptr,
                               uint8*                     sch_data_ptr,
                               gl1_defs_metrics_rpt*      sch_metrics_ptr
                             );

void  l1_sc_tds_process_acq_results( const tdsirat_freq_list_type*  acq_rsp_ptr, gas_id_t gas_id );

void  l1_sc_idle_tds_acq_results( const tdsirat_freq_list_type*  acq_rsp_ptr, gas_id_t gas_id );

void  l1_sc_ded_tds_acq_results( const tdsirat_freq_list_type*  acq_rsp_ptr, gas_id_t gas_id );

void  l1_sc_tds_process_meas_results( const tdsirat_freq_list_type*  meas_rsp_ptr, gas_id_t gas_id );

void  l1_sc_idle_tds_meas_results( const tdsirat_freq_list_type*  meas_rsp_ptr, gas_id_t gas_id );

void  l1_sc_ded_tds_meas_results( const tdsirat_freq_list_type*  meas_rsp_ptr, gas_id_t gas_id );

void l1_sc_tds_process_abort( gas_id_t gas_id );
void l1_sc_g2t_shutdown (gas_id_t gas_id);

enum {
    TDS_NULL,
    TDS_INIT_CMD,
    TDS_INIT_CNF,
    TDS_STARTUP_REQ,
    TDS_ACQ_REQ,
    TDS_ACQ_RSP,
    TDS_MEAS_REQ,
    TDS_MEAS_RSP,
    TDS_ERROR
};

#endif

#if defined (FEATURE_DUAL_ACTIVE) && defined(FEATURE_SGLTE_DUAL_SIM)
extern void gl1_set_switch_multi_sim_mode(sys_modem_device_mode_e_type new_multi_mode);
extern boolean gl1_is_rf_exit_needed_dsda_to_dsds_switch(void);

#endif

typedef union
{
  msgr_hdr_struct_type              hdr;

  x2g_irat_cm_gsm_init_req_type     x2g_irat_cm_gsm_init_req;

#ifdef  FEATURE_GSM_TO_LTE

  lte_cphy_irat_meas_init_cnf_s     lte_cphy_irat_meas_init_cnf;
  lte_cphy_irat_meas_deinit_cnf_s   lte_cphy_irat_meas_deinit_cnf;
  lte_cphy_irat_meas_timed_srch_meas_cnf_s lte_cphy_irat_meas_timed_srch_meas_cnf;
  lte_cphy_irat_meas_abort_cnf_s    lte_cphy_irat_meas_abort_cnf;

#endif


#ifdef FEATURE_GSM_TDS
  tdsirat_init_cnf_type           tdsirat_init_cnf;
  tdsirat_acq_rsp_type            tdsirat_acq_rsp;
  tdsirat_meas_rsp_type           tdsirat_meas_rsp;
  tdsirat_abort_cnf_type          tdsirat_abort_cnf;
#endif
  cfcm_cmd_msg_type_s             cfcm_monitor_ind_msg;
} u_geran_gl1_msgr;


typedef struct
{
  q_link_type       link;
  u_geran_gl1_msgr  msg;

} geran_gl1_msgr_t;


#define  GL1_MSGR_QUEUE_SIZE  6

typedef union
{
  msgr_hdr_struct_type                   hdr;

  rfa_rf_gsm_enter_mode_cnf_s            rfa_rf_gsm_enter_mode_cnf;
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
  rfa_rf_gsm_gta_enter_mode_cnf_s        rfa_rf_gsm_gta_enter_mode_cnf;
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */
  rfa_rf_gsm_wakeup_cnf_s                rfa_rf_gsm_wakeup_cnf;
  rfa_rf_gsm_sleep_cnf_s                 rfa_rf_gsm_sleep_cnf;
  rfa_rf_gsm_tx_enable_cnf_s             rfa_rf_gsm_tx_enable_cnf;
  rfa_rf_gsm_tx_disable_cnf_s            rfa_rf_gsm_tx_disable_cnf;
  rfa_rf_gsm_rx_burst_cnf_s              rfa_rf_gsm_rx_burst_cnf;
  rfa_rf_gsm_set_tx_band_cnf_s           rfa_rf_gsm_tx_band_cnf;
  rfa_rf_gsm_tx_burst_cnf_s              rfa_rf_gsm_set_tx_burst_cnf;
  rfa_rf_gsm_get_timing_infor_cnf_s      rfa_rf_gsm_get_timing_infor_cnf;
  rfa_rf_gsm_idle_frame_processing_cnf_s rfa_rf_gsm_idle_frame_processing_cnf;
  rfa_rf_gsm_vreg_on_cnf_s               rfa_rf_gsm_vreg_on_cnf;
  rfa_rf_gsm_vreg_off_cnf_s              rfa_rf_gsm_vreg_off_cnf;
  rfa_rf_gsm_cm_enter_cnf_s              rfa_rf_gsm_cm_enter_cnf;
  rfa_rf_gsm_cm_exit_cnf_s               rfa_rf_gsm_cm_exit_cnf;
#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING
   rfa_rf_gsm_set_antenna_cnf_s           rfa_rf_gsm_set_antenna_cnf;
#endif /*FEATURE_GSM_TX_DIV_ANTNA_SWITCHING*/
#ifdef GERAN_L1_IP2CAL
   rfa_rf_gsm_start_ip2_cal_cnf_s         rfa_rf_gsm_start_ip2_cal_cnf;
#endif
#ifdef GERAN_L1_HLLL_LNASTATE
   rfa_rf_gsm_process_burst_metrics_cnf_s rfa_rf_gsm_get_metrics_cnf;
#endif
} u_geran_rf_task_msgr;


typedef struct
{
  q_link_type           link;
  u_geran_rf_task_msgr  msg;

} geran_rf_task_msgr_t;

#define GL1_RF_TASK_MSGR_QUEUE_SIZE  13

#ifdef FEATURE_GSM_COEX_SW_CXM

typedef union
{
  msgr_hdr_struct_type                   hdr;

  cxm_freqid_info_ind_s                  cxm_freqid_info_ind;
  cxm_request_activity_info_s            cxm_request_activity_info;
  cxm_wwcoex_state_update_s              cxm_wwcoex_state_info;
#ifdef FEATURE_WLAN_COEX_SW_CXM
  cxm_coex_metrics_req_s                 cxm_coex_metrics_req;
  cxm_coex_active_policy_ind_s           cxm_coex_active_policy_ind;
#endif
} u_geran_cxm_msgr;

typedef struct
{
  q_link_type           link;
  u_geran_cxm_msgr  msg;

} geran_cxm_msgr_t;

#define GL1_CXM_MSGR_QUEUE_SIZE  7
#endif

/*===========================================================================
FUNCTION l1_idle_saic_disable_timer_start

DESCRIPTION
  Starts the ncell monitor shutdown timer for the time period defined
===========================================================================*/
extern void l1_idle_saic_disable_timer_start( gas_id_t gas_id );

/*===========================================================================
FUNCTION l1_idle_saic_disable_timer_deactivate

DESCRIPTION
  Deactivates the idle mode SAIC timer
===========================================================================*/
extern void l1_idle_saic_disable_timer_deactivate( gas_id_t gas_id );

/*===========================================================================
FUNCTION l1_idle_saic_disable_timer_create

DESCRIPTION
  creates/defines the idle saic disable timer
===========================================================================*/
extern void l1_idle_saic_disable_timer_create( gas_id_t gas_id );

#ifdef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
extern uint32  l1_get_drx_interval( gas_id_t gas_id );
extern void    l1_decrement_drx_interval( gas_id_t gas_id );
extern void    l1_subtract_drx_interval( uint32 sub, gas_id_t gas_id );
extern void    l1_set_drx_interval( uint32 val,gas_id_t gas_id );
extern boolean l1_get_bplmn_suspend_in_progress( gas_id_t gas_id );
extern void    l1_set_bplmn_suspend_in_progress(boolean l1_bplmn_suspend_in_progress,gas_id_t gas_id);
#endif /* FEATURE_WTOG_BACKGROUND_PLMN_SEARCH */

/* Dedicated Speedup States - Bit fields */
enum {
   L1_DED_DATA_NULL             =     0,
   L1_DED_SPEEDUP_CHAN_RECONFIG =     1,
   L1_DED_SPEEDUP_G2W           =     2,
   L1_DED_SPEEDUP_DTM_HANG      =     4,
   L1_DED_SPEEDUP_AEQ           =     8,
   L1_DED_DATA_DTM              =  0x10,
   L1_DED_DATA_TRANS            =  0x20,
   L1_PSCAN_SPEEDUP             =  0x100,
   L1_EGPRS_SPEEDUP             =  0x200,
#ifdef FEATURE_AEQ_PHASE4
   L1_DED_SPEEDUP_AEQ_PH4       =  0x400,
#endif /*FEATURE_AEQ_PHASE4*/
   L1_DED_DATA_CLR              = 0xFFFF
};

enum{
  L1_VAR_NULL                       =     0,
  L1_VAR_RECEIVE_IN_PROGRESS        =     1,
  L1_VAR_MONSCAN_IN_PROGRESS        =     2,
  L1_VAR_W_ID_SRCH_IN_PROGRESS      =     4,
  L1_VAR_W_SRCH_IN_PROGRESS         =     8,
  L1_VAR_IDLE_MONITORS_IN_PROGRESS  =  0x10,
 };

typedef enum{
  L1_FNAME_NULL                       =    0,
  L1_FNAME_L1_SC_IRAT                 =    1,
  L1_FNAME_L1_SC_LTE_IRAT             =    2,
  L1_FNAME_L1_SC_RCV                  =    4,
  L1_FNAME_L1_SC_PWR                  =    8,
  L1_FNAME_GPL1_GPRS_DYNAMIC          =    0x10,
 }l1_fname_T;

void gl1_clkdata_speed(boolean dataSpeedReq, uint16 speedState, uint16 prevSpeedState,gas_id_t gas_id );

void gl1_dtm_speedup_notify(gas_id_t gas_id);
void gl1_clkdata_state_init(gas_id_t gas_id);

extern void    l1_set_asynch_int_possible( gas_id_t gas_id );
extern boolean l1_get_asynch_int_possible( gas_id_t gas_id );
extern byte    l1_get_current_rr_l1_msg_id( gas_id_t gas_id );

extern boolean l1_get_epd_pref(gas_id_t gas_id);
extern void  l1_set_epd_pref(rr_l1_epd_preference_t epdpref , gas_id_t gas_id);

extern boolean l1_is_cell_acq_complete( gas_id_t gas_id );

#ifdef FEATURE_GSM_ADAPTIVE_TMLN
boolean gl1_check_if_current_activity_is_xcch(gas_id_t gas_id);
#endif /* FEATURE_GSM_ADAPTIVE_TMLN */

#ifdef FEATURE_G2T_SRCH_LOGGING

/*--------------------------------------------------------------------------
                 LOG PACKET: G2T_IRAT_MEAS_REQ_LOG_PACKET

This structure is used to log parameters sent to TL1 during T2G measurements
--------------------------------------------------------------------------*/

/* the log code for T2G meas request */
#ifndef G2T_IRAT_MEAS_REQ_LOG_PACKET
#define G2T_IRAT_MEAS_REQ_LOG_PACKET (LOG_GSM_BASE_C  + 0x270)
#else
#error redefinition of LOG command code: G2T_IRAT_MEAS_REQ_LOG_PACKET
#endif


/* information for each g2t gap  */
typedef PACKED struct PACKED_POST {

    /*GSM values in qsymbols*/
   uint32 gap_start_pos_qs;
   uint32 gap_length_qs;

   /*gap position info in TDS Cx8*/
  uint16 gap_start_subfn;
  uint16 gap_start_pos;
  uint16 gap_end_subfn;
  uint16 gap_end_pos;

} g2t_srchlog_tds_gap_info_struct_type;


typedef PACKED struct PACKED_POST {

  uint16 cpid; /*ID TDS code 0..127*/
  int16 rscp;
  uint16 cell_position;
} g2t_srchlog_tdsirat_cell_struct_type;


typedef PACKED struct PACKED_POST {

  uint32             uarfcn;
  uint32             measuredFreq; //frequency Hz
  uint8              num_of_cells;
  uint8              lna_state;
  uint16             ts0_position; //ts0 start position from step0 result
  uint16             acq_state; /* Could have been enum: tdsirat_acq_state_e . but not sure the log packet supports enum*/
  g2t_srchlog_tdsirat_cell_struct_type  cell[TDSIRAT_MAX_TDS_CELLS_PER_FREQ];

}g2t_srchlog_tdsirat_freq_struct_type;


LOG_RECORD_DEFINE(G2T_IRAT_MEAS_REQ_LOG_PACKET)
uint8                                 version;
uint64                                time_since_last_measurement_ms;
g2t_srchlog_tds_gap_info_struct_type  tds_gap_info;
uint32                                rxlm_rx_buffer;
uint8                                 num_of_freq;
g2t_srchlog_tdsirat_freq_struct_type freq[TDSIRAT_MAX_TDS_FREQ];

LOG_RECORD_END


/*--------------------------------------------------------------------------
                 LOG PACKET: G2T_IRAT_MEAS_RESP_LOG_PACKET

This structure is used to log reponse from TL1 during T2G measurement.
--------------------------------------------------------------------------*/

/* the log code for T2G meas response */
#ifndef G2T_IRAT_MEAS_RESP_LOG_PACKET
#define G2T_IRAT_MEAS_RESP_LOG_PACKET (LOG_GSM_BASE_C  + 0x271 )
#else
#error redefinition of LOG command code: G2T_IRAT_MEAS_RESP_LOG_PACKET
#endif


LOG_RECORD_DEFINE(G2T_IRAT_MEAS_RESP_LOG_PACKET)
uint8                                 version;
uint64                                time_since_last_measurement_ms;
uint8                                 num_of_freq;
g2t_srchlog_tdsirat_freq_struct_type freq[TDSIRAT_MAX_TDS_FREQ];

LOG_RECORD_END



/*--------------------------------------------------------------------------
                 LOG PACKET: G2T_IRAT_ACQ_REQ_LOG_PACKET

This structure is used to log parameters sents to Tl1 during T2G Acquisition
--------------------------------------------------------------------------*/

/* the log code for T2G acq request */
#ifndef G2T_IRAT_ACQ_REQ_LOG_PACKET
#define G2T_IRAT_ACQ_REQ_LOG_PACKET (LOG_GSM_BASE_C  + 0x272 )
#else
#error redefinition of LOG command code: G2T_IRAT_ACQ_REQ_LOG_PACKET
#endif

typedef PACKED struct PACKED_POST {

  uint32                       uarfcn;
  uint16                       ts0_position;
  uint8                        last_lna_state;
  tdsirat_acq_state_e           last_acq_state;
}g2t_srchlog_tdsirat_acq_freq_type;


LOG_RECORD_DEFINE(G2T_IRAT_ACQ_REQ_LOG_PACKET)
uint8                                 version;
uint64                                time_since_last_measurement_ms;
g2t_srchlog_tds_gap_info_struct_type  tds_gap_info;
uint32                                rxlm_rx_buffer;
uint8                                 num_of_freq;
g2t_srchlog_tdsirat_acq_freq_type freq[TDSIRAT_MAX_TDS_FREQ];

LOG_RECORD_END



/*--------------------------------------------------------------------------
                 LOG PACKET: G2T_IRAT_ACQ_RESP_LOG_PACKET

This structure is used to log reponse sents to GL1 by Tl1 during T2G Acquisition
--------------------------------------------------------------------------*/

/* the log code for the list seach packet */
#ifndef G2T_IRAT_ACQ_RESP_LOG_PACKET
#define G2T_IRAT_ACQ_RESP_LOG_PACKET (LOG_GSM_BASE_C  + 0x273 )
#else
#error redefinition of LOG command code: G2T_IRAT_ACQ_RESP_LOG_PACKET
#endif


LOG_RECORD_DEFINE(G2T_IRAT_ACQ_RESP_LOG_PACKET)
uint8                                 version;
uint64                                time_since_last_measurement_ms;
uint8                                 num_of_freq;
g2t_srchlog_tdsirat_freq_struct_type freq[TDSIRAT_MAX_TDS_FREQ];

LOG_RECORD_END


#define G2T_IRAT_MEAS_REQ_PACKET_LEN(num_of_freq)  ( \
  FPOS(G2T_IRAT_MEAS_REQ_LOG_PACKET_type, freq ) + \
  (num_of_freq * sizeof(g2t_srchlog_tdsirat_freq_struct_type)) )


#define G2T_IRAT_MEAS_RESP_LOG_PACKET_LEN(num_of_freq)  ( \
  FPOS(G2T_IRAT_MEAS_RESP_LOG_PACKET_type, freq ) + \
  (num_of_freq * sizeof(g2t_srchlog_tdsirat_freq_struct_type)) )



#define G2T_IRAT_ACQ_REQ_PACKET_LEN(num_of_freq)  ( \
  FPOS(G2T_IRAT_ACQ_REQ_LOG_PACKET_type, freq ) + \
  (num_of_freq * sizeof(g2t_srchlog_tdsirat_acq_freq_type)) )


#define G2T_IRAT_ACQ_RESP_LOG_PACKET_LEN(num_of_freq)  ( \
  FPOS(G2T_IRAT_ACQ_RESP_LOG_PACKET_type, freq ) + \
  (num_of_freq * sizeof(g2t_srchlog_tdsirat_freq_struct_type)) )

#endif /* #ifdef FEATURE_G2T_SRCH_LOGGING  */

#if defined (FEATURE_QTA ) && defined (FEATURE_CXM_QTA)
boolean l1_get_qta_active( gas_id_t gas_id );
#endif

void L1_send_X2G_CM_GSM_INIT_RSP( gas_id_t gas_id , rfcom_mode_enum_type  serving_rat );

typedef struct
{
  uint32 frame_num;
  uint32 total_usfs;
  uint32 total_blocks_blanked;
  uint32 num_consecutive_blanked_blocks;
  uint32 blanking_percentage_average;
} l1_usf_utilisation_info_t;

void l1_get_usf_utilisation_info(l1_usf_utilisation_info_t* usf_utilisation_info, 
                                 sys_modem_as_id_e_type as_id);

uint32 l1_get_number_consecutive_blanked_blocks(sys_modem_as_id_e_type as_id);

void    l1_sc_send_irat_g2l_init(boolean g2l_neighbours , gas_id_t gas_id );
void gl1_hw_abort_idle_act_prog(gas_id_t gas_id );
boolean gl1_check_if_current_activity_is_xcch(gas_id_t gas_id);
void gl1_update_scheduled_activity(gas_id_t gas_id);

boolean L1_ftm_enabled( gas_id_t gas_id );

#endif /* L1I_H */
