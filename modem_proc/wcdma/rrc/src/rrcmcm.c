/*===========================================================================
         R R C   M O D E C O N T R O L   M O D U L E 

DESCRIPTION

  This module performs the actions related to the RRC mode. It 
  maintanences the RRC mode related information. It handles the WCDMA mode.
  activation and deactivation. It also informs the other procedures when 
  the mode change occurs. This module is used for dual-mode only.
  
  
EXTERNALIZED FUNCTIONS

  rrcmcm_event_handler            Event handler for this module
  rrcmcm_is_process_required      Filtering an incomming command
  rrcmcm_register_mode_change     Register mode change notification
  rrcmcm_get_mode_and_preference  Retrieve the current mode and the mode pref

INITIALIZATION AND SEQUENCING REQUIREMENTS

   
Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcmcm.c_v   1.6   19 Jul 2002 13:36:10   xgao  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcmcm.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   sp      Made changes to log call end info in all cases whenver call is released
06/06/16   sp      Made changes for Mav debug snapshot
01/12/16   sn      Changes for FR30881
05/11/16   vs      Changes to cleanup unnecessary f3
03/14/16   sr      Added changes to recover if lock acquisition fails for CSFB redirection
12/30/15   gv      Release the TRM lock though CSFB call is set but when scan type is not redirection
11/04/15   ad      Made changes not to apply the pending cs keys on wtog handover
10/07/15   vi      Changes for force DRX and skip idlemode measurements based on QMI request 
09/14/15   sr      Made changes to get default band from RF api for lock request
07/13/15   sas     Made changes to honor MPLMN srch req when abort for WtoX irat procedure is active.
07/01/15   sp      Made changes to save procedure substates for LTE commands and to skip logging frequent internal commands
06/22/15   sr      Made changes to ignore TA status when acquiring lock for WCDMA RESUME post LTA
05/18/15   sas     Made changes to indicate RLC about the LTA/QTA start/stop status.
02/12/15   sa      Made changes for QTF with JO CRM
11/20/14   sn      Changes to raise an assert if incorrect cause is received in 
                   stop WCDMA request 
12/02/14   bc      Made changes to reset csfb_call_status on stopping W Stack.
11/18/14   sr      Made changes to move to idle if TA duration is greather than re-est timers only in DCH state
11/10/14   sp      Made changes to save procedure substates in crash debug info
10/22/14   gv      Made changes to have connected mode feature set independent of TA status 
10/17/14   sr      CSFB call optimization changes
10/13/14   as      Fixed compiler warnings
10/10/14   sr      Made changes to trigger tranistion to idle if TA duraitonis mroe than re-establishment timer values
09/10/14   vi      Made changes to ignore TVM/GPS report when RRC is suspended and CCO is in progress
09/04/14   vi      Made changes to reduce curr_scan prints in the log
08/11/14   rmn     Made changes to free rrc_svd_plmn_req_ptr when BPLMN search is aborted 
08/08/14   rmn     Made changes to request lock on supported band indicated by NAS rather
                   than deriving it from Japan PLMN and freq.
08/04/14   vi      Made changes to avoid compiler warnings
07/21/14   sr      F3 reduction changes
07/17/14   sr      Changes for NV refresh FR
07/04/14   sr      Made changes to handle EQ_PLMN_LIST_IND in INACTIVE mode to unbar freqs in case of hot swap
07/14/14   rmn     Made changes to attempt trm_exchange for all XtoW IRAT scenarios 
06/11/14   bc      Made changes to reduce DCH to FACH cell selection time.
06/23/14   rmn     When calling new WRM lock request API, WRRC should indicate lock requested 
                   bands with type 'sys_band_class_e_type'
06/19/14   rmn     Made changes for DR-DSDS Feature
05/20/14   rmn     Made changes to save DS Tuneaway Status Change indication in connected state based on the concurrency NV
05/14/14   sr      Made changes to update TA duration as invalid post WCDMA resume failure
04/23/14   bc      Made changes to send cell info to LTE on receiving UE capability request
                   and also to send cell info WTOL PSHO request.
05/06/14   geg     Use safer versions of memcpy() and memmove()
05/05/14   sr      Made changes to not acquire lock for WCDMA start for redirection when TRM exchange fails
05/02/14   sr      Made changes for ignoring/ delaying preemption if IHO/CCO in progress
04/30/14   sr      Made changes to abort IRAT activity when abort is received from NAS
04/25/14   vi      Made changes to handle LTE deprioritization request in W-RRC
04/16/14   vi      Fixed compilation errors.
04/07/14   sr      Made changes to allow manual PLMN during WRLF
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/14/14   bc      Made changes to send partial plmn results to NAS during manual plmn search
03/13/14   as      Made changes to backup internal/external cmds for crash debugging
01/02/14   rmn     Reduction of unwanted F3's
12/30/13   sr      WRRC support for new requirements for LWG DSDS
12/26/13   sr      Changes to handle DS_STATUS_CHANGE_IND when WCDMA is active
12/06/13   vi      Added new command handling RRC_MUTLIMODE_SUBS_CHGD_IND from NAS to set the correct AS ID
12/03/13   mn      Moved all declarations to top of the code.
12/03/13   as      Made changes to fix CSFB issues related to DSDS
11/16/13   sr      WRRC support for LWG DSDS
11/11/13   gv      Made changes to update TA duration in FACH/DCH before resuming WCDMA after LTA 
                   even though TA status is disabled
10/24/13   rmn     During InterRAT PLMN Search Operation, RRC now accounts for the time consumed to acquire lock before sending Start WCDMA Req. 
10/11/13   gv      Added code to pass SGLTE mode ind to WL1
08/13/13   gv      Added support for FEATURE_TRIPLE_SIM
07/22/13   gv      Changes to release lock when we decide to stop/delay W2G BPLMN
07/08/13   sn      Changes for LTE CSG feature
06/25/13   sn      Block IRAT functionality in manual CSG mode
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
06/27/13   gv      Added code to update AS ID change due to mode pref change when sim is not available
06/14/13   vi      Added handling of RRC_LAI_REJECT_LIST_IND and RRC_TUNEAWAY_ALLOWED_IND in Inactive state.
05/13/13   vi      Fixed compiler and KW errors for Bolt
05/03/13   vg      Modified code to not to send LTE measurement request to L1 when MODE_CHANGE_IND with LTE disabled is received from NAS
04/24/13   sn      WRRC side of changes to meet CSG ADAPT TCs
01/04/13   sr      Made changes to update RRC mode to ACTIVE when lock fails for WCDMA resume during FACH state
04/08/13   sr      Made changes to update dual mode and lte mode status as FALSE when n/w select mode is manual CSG
03/20/13   mp      Removed tmc.h include file
02/15/13   gv      Made changes to stop lock wait timer before entering block wait to acquire lock
01/03/13   ad      Made changes to reduce the redundant F3s
12/07/12   sn      CSG feature changes
10/23/12   pm      Made changes to assign pointer to NULL after de-allocation
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
10/14/12   gv      Added code to support exchange of AS ID between WRRC and GRR commands
09/27/12   sr      Added code changes to introduce an api used by NAS to reset MCM activation cause 
                   if registration won't be triggerred because of IRAT reselection when ISR is active
09/13/12   gv      Added support to send deact request to TDSCDMA while WCDMA is deactivated to facilitate 
                   writing of ACQ DB NV
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/04/12   rv      Made changes to reduce block wait time to acquire lock in DCH/FACH from 
                   300ms to 150ms as no SUB was seen holding holding the TRM lock for 
                   considerable amount of time
05/28/12   sn      Made changes to abort ongoing interrat redirection if we receive 
                   establishment request for detach procedure
05/24/12   sks     Added support for QCHAT specific dynamic DRX.
04/26/12   as      Made changes to handle connection release delay timer expiry in in-active state
03/29/12   sks     Made changes to get the TA duration and pass it to L1.
12/12/11   mn      Made changes to process ps hard abort request received when waiting for lock
09/28/11   ad      Added code changes for SMC to wait for till RRC comes back 
                   to active state if L2 ack for SMC is received when RRC is in 
                   STANDBY mode.
07/11/11   rl      Added the feature flag while accessing the rrc_lte_mode_enabled
07/08/11   sks     Made changes to update the LTE mode status at the time service request is received.
07/08/11   gv      Added code to reset the last mcm activation cause whenever 
                   a fresh acquisition happens
06/27/11   sks     Made changes to handle EUTRA capability response message in all RRC modes.
06/27/11   gv      Modified the code to not update capability information with the NW 
                   in connected mode when NAS sends RRC_CHANGE_MODE_IND
06/17/11   gv      Made changes to clear the barred cells list when NAS sends deact request 
                   to RR and RR in turn sends stop request to RRC
05/03/11   rl      Fixed the lint error
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   vg      added support for mdsp halt recovery
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/21/11   sk/rl   Added code to support L2W CGI
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/07/11   as      Fixed compilation error for Dsim Tuneaway
02/10/11   rl      Fixed the compilation errors
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   sks     Added support for LTOW PSHO.
01/25/11   as      Added code to report Tuneaway Start and Stop events
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/22/10   sks     Added support for handling EM call during WRLF.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   rm      Added code to handle CSFB status indication
09/27/10   sks     Made changes to handle buffered EST_REQ after interrat redirection via
                   RRC connection release msg fails and then resume on W also fails.
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM
09/09/10   rm      Made changes to hold dedicated priorities during OOS
08/09/10   sks     Made changes to ignore saved service req or plmn list req on 
                   reception of RRC_STOP_WCDMA_MODE_REQ from NAS.
07/21/10   rm      When START WCDMA fails due to no lock, added code to send 
                   IRAT PLMN SRCH CNF to RR
07/19/10   sks     Added code for WRLF, to handle OOS timer expiry in INACTIVE state.
07/07/10   sks     Fixed compiler warnings.
07/05/10   rm      Fixed compiler warnings
07/02/10   sks     Added code to support FEATURE_WRLF_SYSTEM_SEL for connected mode OOS.
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/14/10   rm      Added code to support BST search on WCDMA
06/07/10   as      Added code for EOOS under feature flag FEATURE_EOOS
05/10/10   sks     Made changes to not change the establishment_cause in case of registration
                   attempt after GTOW redirection success.
03/25/10   rm      Added code to send the Resume WCDMA failure cause in change mode confirm
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/15/10   sks     Made changes to respond to RRC_RR_STOP_WCDMA_REQ for all RRC modes.
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
11/02/09   sks     Added code to suspend handling of STOP_MODE_REQ, until current 
                   WTOG process is aborted.
09/11/09   ps      Made changes for reducing the internal RRC command size 
                   under FEATURE_RRC_REDUCE_CMD_SIZE
09/22/09   rm      Added code to clear L1 barring info and write ACQ DB to NV on
                   MODE_CHANGE_REQ to LPM 
08/03/09   rm      Added code to send PLMN_LIST_CNF as failure to NAS, if RRC receives
                   PLMN_LIST_REQ when GTOW PCCO is in process and RRC mode is INTERRAT_IN_PROGRESS
06/29/09   rm      Added code to correctly update rrc_dual_mode_enabled from SERVICE_REQ when RRC
                   mode is RRC_MODE_INTERRAT_IN_PROGRESS
06/22/09   rm      Made changes to process funneled Delete all TVM measurements req to MAC when 
                   RRC mode is INACTIVE
06/15/09   rm      Fixed compilation error.
06/15/09   sks     Made changes to support messaging interface with RR for writing 
                   ACQ-DB to NV during power off.
05/13/09   rm      Made changes to update RRC mode and MCM substate before sending 
                   CPHY_START_WCDMA_MODE_REQ to L1.
05/11/09   ps       Feature to dynamically allocate OC, CC, TOC & sib-event ptr 
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/25/09   rm      Made changes to send change mode CNF to IHO when mode is updated from
                   idle_detached to Inactive. Without these changes, RRC is sending
                   RRC_RR_STOP_WCDMA_CNF to RR
03/28/09   rm      CMI Featurisation
03/23/09   rm      Made changes to reset hsdpa_hsupa_ind on sending RRC_STOP_WCDMA_MODE_CNF to MM
03/16/09   rm      Made changes to send RRC_RESELECT_TO_GAN_REJ when STOP W is in progress and
                   Rove-in request is received
02/25/09   rm      Added code to send RRC_RESELECT_TO_GAN_REJ when reselection to GAN
                   is in progress and NAS sends STOP_WCDMA_MODE_REQ.
12/29/08   rm      Added support for 3G<->GAN Handover
12/26/08   rm      Made changes to process RRC_RR_STOP_WCDMA_REQ in RRC Mode INACTIVE
                   Changes are for GAN
11/27/08   sk      Fixed compilation error with W2G BPLMN feature undefined.
11/20/08   ps      Added rrcmcm_get_substate() to get the MCM substate
11/21/08   rm      Made changes to send GAN_MODE_IND after RRC_RR_STOP_WCDMA_CNF to RR
11/06/08   sk      Pending stop mode cmd till W2G BPLMN is aborted
11/04/08   rm      Made chanages to send RR_GAN_MODE_IND to RR with service status as
                   RRC_MODE_RRC_INACTIVE when RRC_MODE_INACTIVE is the new RRC mode.
10/24/08   rm      Added support for Rove out from GAN, IDLE DETACHED MODE
                   Changes are under feature flag FEATURE_GAN_3G_ROVE_OUT_ONLY
09/18/08   rm      Made changes to send service status to MM with UOOSC indicator as FALSE
                   on reception on STOP WCDMA req from NAS and UOOSC search was in progress
08/11/08   sks     Made chagnes to send PLMN list CNF for foreground search when aborted
                   by other NAS requests
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
05/16/08   pk      Added support for Rove-in to GAN. Changes are under the feature 
                   flag FEATURE_GAN.
04/10/08   pk      Teardown BCCH before sending SLEEP_IND to L1, if RRC gets 
                   DEACT_REQ/STOP_MODE_REQ/MODE_CHANGE_REQ when BPLMN search is actively
                   in progress and CSP is waiting for SIBs.
03/11/08   pk      Added support to report current cell id to NAS through RRC_SERVICE_CNF,
                   RRC_SERVICE_IND and RRC_SERVICE_IND_FROM_OTA.
09/06/07   pk      Added support for handling SERVICE_REQ/PLMN_LIST_REQ while waiting for
                   activating the WCDMA stack, if a previously received INTER_RAT_PLMN_LIST_REQ
                   is aborted.                   
07/25/07   vr      Update rrc_dual_mode_enabled flag before forwarding the buffered
                   RRC_SERVICE_REQ to CSP in wait_resume_cnf substate. 
07/23/07   kp      Added support for FEATURE SPEC CR 2567 GTOW SECURITY KEY HANDLING.
                   Allow RR_CIPHER_SYNC_IND even if mcm mode is inactive.    
06/12/07   vr      Added error fatal when change mode request is received from 
                   IRAT_IN_PROGRESS to STANDBY. This scenario should never happen 
                   because WtoG procedure cannot be started when GtoW procedure 
                   is in progress. This error fatal will help in catching such invalid
                   scenarios.
05/23/07   pk      Added support for feature FEATURE_MANUAL_SEARCH_FREQ_SCAN_SKIP_TIMER.
05/03/07   vm      Added fix for CR 118055. Added another argument to the function 
                   rrc_free_embedded_buf()
11/28/06   sgk     Set the mdsp image default to WCDMA GSM in the start mode req. 
11/21/06   vm      Added support for the feature FEATURE UMTS BACKGROUND FULL SERVICE SEARCH.
                   Added another argument of type sys_service_search_e_type in the function
                   rrcmcm_send_plmn_list_cnf_with_failure().
10/18/06   sgk     Fixed lint errors.
10/13/06   sgk     Added 7600 support under feature
                   FEATURE_UMTS_1X_HANDOVER_UMTSMSM
09/25/06   kp      Added support for sib_change_ind interface change
09/08/06   tkk     Added code to handle RRC_DEACT_REQ in standby without
                   MCM indicating to abort GSM search to CSP.
09/08/06   segu    Send PLMN_LIST_CNF with failure if PLMN_LIST_REQ is received 
                   from MM when MCM is in rrcmcm_wait_suspend_cnf_substate_event_
                   handler/rrcmcm_initial_substate_event_handler, RRC state is 
                   connecting and Inter RAT redirection is in progress
06/26/06   segu    stop RSSI timer after receving STOP_LOWER_LAYER_CNF from LLC
                   when initiated by MODE_CHANGE_REQ from NAS
06/07/06   kp      Added support for six hours sib update
05/02/06   sg      Added support for single to dual mode changes
04/13/06   sg      Added support to process EST_REQ in WCDMA suspended mode
04/10/06   vm      Added support to update est cause with inter-RAT reselection 
                   or inter-RAT CCO only when the est cause is Registration.
02/21/06   vr      Added support to send Interrat reason in Suspend and Resume 
                   requests to L1
01/25/06   bd      Lint clean-up
01/23/06   vm      BPLMN Optimization for Automatically re-starting Manual 
                   BPLMN search after completion of LAU/RAU updates & added
                   support for not rejecting Manual PLMN search request when RRC state 
                   is Connecting/FACH/DCH and the establishment cause is Registration. 
01/23/06   kp      Added support for Transaction I.D in BPLMN Abort feature 
01/04/05   kc      Removed routing BPLMN_SRCH_ABORT from mcm to dispatcher
12/30/05   sg      Added support for handling BPLMN Abort search from NAS
12/12/05   kc/vr   Mechanism to buffer change_mode_req from other RRC
                   procedures and act on it after receiving the mode change 
           confirmation from L1
12/02/05   vr      Do not honor PLMN_LIST_REQ if RRC is in DCH/FACH or if
                   RRC is in PCH and an OOS is in progress.
11/30/05   vr      Added more support for BPLMN.If an automatic search is 
                   triggered when Interrat Reselection is in progress,give 
           preference to Reselection and send a PLMN_LIST_CNF with failure  
                   to NAS 
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search.
                   Included file rrccspi.h, abort_ind is now sent to CSP if
                   establishment request is received when W is suspended for WTOG
                   PLMN search, and BPLMN search is aborted and go to sleep ind
                   is correctly sent to L1 if stop_req is received from NAS when
                   BPLMN search is in progress.
10/04/05   vr      Added an interface to update RRC Mode from IRAT_IN_PROGRESS
                   to Active.Currently used by CHO procedure after GTOW PCCO
                   is successful
07/20/05   vr      Implementation of Spec Compliant WTOG Cell Change Order
07/15/05   vm      Added support for handling channel config failure in CSP.
                   Removed header file rrccspi.h; and MCM now does not send 
                   channel config request if the destination procedure is CSP.
07/08/05   vm      FEATURE_OUT_OF_SERVICE_ENHANCEMENT: Added support for OOS in 
                   Connected Mode feature.
                   Included file rrccspi.h.
06/02/05   vr      Support for Interrat Reselection aborts in Idle mode
04/18/05   vr      Handle RRC_ABORT_REQ in Standby mode - If there was a pending
                   est_req,clear the flag and send a abort_cnf from MCM itself. 
04/04/05   vm/bu   Added support for inter-RAT redirection feature.
                   Added new local variable rrc_current_state to 
                   rrcmcm_wait_resume_cnf_substate_event_handler() function.
                   Corrected the mode resume status passed as argument to
                   the function rrcmcm_send_change_mode_cnf().
03/04/05   kc      Added status for Change-Mode-Cnf to procedures.
12/21/04   vr      Fixed lint errors
12/17/04   vr      Added support for WCDMA->1x handover feature
12/07/04   vr      Added support to send stop cnf to MM for MM issued stop cmds
                   in interrat-in-progress mode
11/22/04   vr      Added support for BPLMN search
11/18/04   vm      Added case for RRC_DEEP_SLEEP_NO_SVC_TIMER_EXPIRED_IND.
08/30/04   sgk     Type cast RRCMCM_MAX_NOTIFICATIONS  to uint16 in function
                   rrcmcm_register_for_mode_change and made 
                   rrcmcm_notification_counter uint16 to fix lint warning enum
                   to int conversion.
08/11/04   bu      Set the deep sleep flag in CSP only if WCDMA is stopped due
                   a STOP_WCDMA_MODE_REQ from MM with cause deep sleep.
08/05/04   bu      Fill in the proc in rrcmcm_send_initiate_cell_selection_ind().
08/05/04   kc      Set UE mode to INACTIVE and send CCM/CSP proc mode-change-ind
                   whenever we get STOP-CNF from L1 even when a START is pending.
07/21/04   kc      Set UE-Mode to active when we receive START-WCDMA-CNF even
                   when a STOP-WCDMA-REQ is pending, as the stop handling needs
                   UE to be active in the first place.
06/21/04   bu      Don't set the deep sleep variable for MODE_CHANGE_REQ.
06/14/04   bu      Call the CSP function to set the deep sleep variable based on
                   the cause in the STOP_MODE_REQ.
06/08/04   kc      Corrected PLMN_LIST_CNF cmd to use the correct cmd_type.
05/05/04   bu      Added support for RRC_FREQ_SCAN_TIMER_EXPIRED_IND in all RRC
                   modes.
03/25/04   kc      Set Dual Mode flag correctly when we receive START_CNF from
                   from L1.
03/24/04   kc      Save SERVICE_REQ in WAIT_START_CNF substate.  Also handle
                   SERVICE_REQ and PLMN_LIST_REQ when START_CNF is received.
03/22/04   kc      If RRC is in INACTIVE or STANDBY mode, release embedded buffer
                   for Measurement request or Traffic volume request.
03/19/04   kc      Added support for GtoW PCCO feature.
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
02/18/04   kc      Send ABORT_IND to MM if  RESUME_CNF with failure status is 
                   received and an EST_REQ is saved in MCM procedure.
12/04/03   vn      Fill in the sdu_time for a Suspend Request that goes with an
                   activation time.
08/21/03   bu      Set the stop cause to DEEP_SLEEP if RRC_CHANGE_MODE_REQ for
                   MODE_CHANGE is received.
08/15/03   kc      Added function to update dual mode status flag
08/12/03   bu      Added support to fill activation time while suspending WCDMA
                   mode for a CELL CHANGE ORDER message received from UTRAN.
06/20/03   kc      Added appropriate dualmode definitions to resolve
                   singlemode compilation issues.
06/03/03   kc      Send MODE_CHANGE_CNF to procedure requesting STOP cmd and then
                   send MODE_CHANGE_IND to CCM and CSP procedures to resolve race
                   conditions.
06/03/03   kc      Added code to send MODE_CHANGE_IND to CCM and CSP procedures
                   when WtoG handover fails due to PHY_CHAN_IND Failure. 
05/23/03   kc      Added support of CHANGE_MODE_REQ to take the phone to FTM
                   mode and return to online mode at run time.
05/13/03   kc      Removed Lint Errors due to inclusion of 
                   FEATURE_INTERRAT_HANDOVER_GTOW permanently and also due to
                   indentation issues.
05/05/03   xfg     Sent Mode change confirm to CSP before initiating cell selection 
05/01/03   kc      Added handling of RRC_HO_CIPH_CNF in INACTIVE mode.
04/28/03   kc      Added support to query activation time from IHO procedure
                   for WtoG handover and send the activation time in Suspend_Req
                   to L1.
04/24/03   bu      Removed usage of FEATURE_L1_ACT_TIME. Activation time will be
                   initialized in the l1_cmd_hdr_type.
04/08/03   kc      Changed parameter passed to is_process_required function to
                   cmd_ptr to save EST_REQ in case of a race condition
                   Also added code to send PLMN_LIST_CNF with failure in 
                   IRAT_IN_PROGRESS and STANDBY mode
03/07/03   bu      Added handling of RRC_NEW_STATE_IND and RRC_STATE_CHANGE_IND
                   in INACTIVE state in rrcmcm_is_process_required.
02/20/03   bu      Corrected the feature definition flag.
02/19/03   kc      Added Feature defn. flag around GtoW code to remove compilation
                   warnings.
02/14/03   kc      Changed feature defn from INTERRAT_HANDOVER_GTOW_RRC to 
                   INTERRAT_HANDOVER_GTOW
02/14/03   kc      Added process to handle ActivationRsp for GtoW Handover.  Also
                   provided function to update Activation Cause.
02/05/03   kc      Set dual_mode_flag to TRUE default
01/28/03   sk      Moved the constant definitions and typedefs from rrcmcm.c 
                   to rrcmcm.h (lint error fix)
01/09/03   xfg     Changing mode to Active after resume confirmation is received
12/06/02   xfg     Forwarded RRC_ACTIVATION_RSP to CSP.
12/04/02   xfg     Added process for handling RRC_CHANGE_MODE_IND from MM
11/25/02   xfg     Added process for RRC_CRLC_RELEASE_ALL_CNF for off-line
11/25/02   xfg     Added process for RRC_CPHY_DEACTIVATE_CNF for off-line
11/25/02   xfg     Added FEATURE INTERRAT HANDOVER WTOG around RRC_CHANGE_MODE_CNF to
                   avoid a compilation error
11/25/02   xfg     Added support for handling off line in Inactive mode
11/19/02   xfg     Added handling for RRC_CHANGE_MODE_CNF in Inactive mode
11/11/02   xfg     Added codes to support WCDMA activation if PLMN list request
                   is received at Inactive state
11/08/02   xfg     Translated rrc_stop_casuse_e_type to l1_stop_cause_enum_type
11/06/02   xfg     Added codes to handle failure of the resume
11/04/02   xfg     Added codes to support deep sleep
10/29/02   xfg     Added FEATURE INTERRAT HANDOVER WTOG back in
                   rrcmcm_send_stop_wcdma_mode() upon request from MM people.
10/29/02   xfg     Removed FEATURE INTERRAT HANDOVER WTOG in rrcmcm_send_stop_wcdma_cnf()
                   to avoid breaking the single mode build because MM did not
                   featurelize it changes.
10/29/02   xfg     Added FEATURE INTERRAT HANDOVER WTOG to avoid a compile error
10/25/02   xfg     Added START Values in RRC_STOP_WCDMA_MODE_CNF amd made changes
                   from the code review
10/14/02   xfg     Fxed another compile error due to missing a feature define
10/14/02   xfg     Added FEATURE_INTERRAT_CELL_RESELECTION_GTOW around codes 
                   and definitions which are only related to GSM to WCDMA Cell
                   reselection.
10/10/02   xfg     Added handling for RRC_SERVICE_REQ during inter-RAT operation
10/04/02   xfg     Added support for GSM to WCDMA cell reselection
09/27/02   xfg     Commented out send_channel_config_req after resume for now
                   until handling is implemented in LLC.
09/25/02   xfg     Informed CSP with the correct RRC Mode after the resume
09/25/02   xfg     Modified the feature define to supoort both inter-RAT
                   handover and cell reselection
09/13/02   xfg     Added support for full implementation for suspend/resume
08/15/02   xfg     Removed unused MM to RRC commands and added new MM to RRC
                   commands for SIM I/F changes
07/29/02   xfg     Added changes for supporting inter-RAT handover. All the 
                   changes are made under FEATURE_INTERRAT_HANDOVER_WTOG.
07/18/02   xfg     Sent RRC_MODE_CHANGE_IND to CSP and CCM when the RRC mode 
                   transitions to the inactive.
07/12/02   xfg     Changed the scope of the feature define tag
06/24/02   xfg     Added starting and stopping RSSI reporting timer when RRC
                   mode changes between Active and Inactive for Dual-mode
06/13/02   xfg     Added support for Stop WCDMA mode
05/31/02   xfg     Changed RRC_RAT_Preference to RRC_Dual_Mode_Enabled
05/17/02   xfg     Added handler for RRC_CPHY_START_WCDMA_MODE_CNF in Inactive
05/10/02   xfg     Change FEATURE_CHEETAH_BASELINE to FEATURE DUALMODE BASELINE
04/28/02   xfg     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "err.h"
#include "msg.h"
#include "memheap.h"
#include "rrctmr.h"
#include "rrcsend.h"
#include "rrccmd_v.h"
#include "rrcdata_v.h"
#include "rrcintif.h"
#include "rrclcm.h"
#include "rrcllc.h"
#include "rrcscmgr.h"
#include "rrcccm.h"
#include "rrccspi.h"
#include "rrccsp.h"
#include "rrcsmc.h"
#include "l1task_v.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "sys.h"
#include "rrcrrif.h"
#include "rrcmcm.h"
#include "rrcrce.h"
#include "rrciho.h"
#include "rrccho.h"
#include "rr_rrc_if.h"
#include "rrccui.h"
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif
#include "rrcmisc.h" 
#ifdef FEATURE_DUAL_SIM
#include "rrclogging.h"
#endif

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#ifdef FEATURE_TDSCDMA_TO_WCDMA
#include "rrc_tdsrrc_if.h"
#endif

#include "rrcsibdb.h"
#include "rrcmeas.h"

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
rrc_plmn_list_req_type *rrc_svd_plmn_req_ptr=NULL;
/* RAT Mode                                                                */
rrc_mode_e_type rrc_mode = RRC_MODE_INACTIVE;

/*set activation  cause of WCDMA */
rrcmcm_activation_cause rrcmcm_act_cause = RRCMCM_ACT_CAUSE_NONE;

/*set last activation cause of WCDMA */
rrcmcm_activation_cause rrcmcm_last_act_cause = RRCMCM_ACT_CAUSE_NONE;

/* Mode Preference                                                         */
boolean rrc_dual_mode_enabled = TRUE;

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
boolean rrc_lte_mode_enabled = FALSE;
#endif
#ifdef FEATURE_DUAL_SIM
extern rrc_ds_tuneaway_status_change_ind_type ds_tuneaway_status_change_ind;
#endif

boolean rrc_is_ue_in_sglte_mode = FALSE;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
/* MCM state */
rrcmcm_state_e_type rrcmcm_state = RRCMCM_INITIAL;

/* MCM Mode Notification Table */
rrcmcm_mode_change_table_type rrcmcm_notifications[RRCMCM_MAX_NOTIFICATIONS];

/* Mode change notification counter */
uint16 rrcmcm_notification_counter = 0;

/* Struct to hold the commands which have deferred processing */
rrcmcm_cmd_store_type rrcmcm_cmd_store;

/* Destination procedure for change mode confirmation */
rrc_proc_e_type procedure_id;

/* The reason for stopping WCDMA mode of operation */
rrc_stop_cause_e_type stop_cause;

/*Save EST_REQ type.. This is needed because in STANDBY mode we will save
  the EST_REQ type and only later on going to ACTIVE mode send it to rce.
  If we go to INACTIVE mode contents are deleted*/
rrc_est_req_type rrcmcm_est_req;

/*Flag to indicate if EST_REQ is saved or not*/
boolean rrcmcm_est_req_saved_flag;

/* Flag to indicate if we need to send stop_cnf to MM (stop issued by MM) */
boolean rrcmcm_send_stop_cnf_to_mm_flag;
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
#error code not present
#endif
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif

/* Global HSDPA HSUPA Indicator */
extern sys_hs_ind_e_type hsdpa_hsupa_ind;

#ifdef FEATURE_DUAL_SIM
  wl1_common_fail_enum_type rrcmcm_wl1_failure_cause = WL1_FAILED_NORMAL;
#endif

typedef enum
{
  INIT_ALL,
  INIT_STATE_VARS_ONLY,
  INIT_CMD_STORE_ONLY
} rrcmcm_init_e_type;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION   RRCMCM_SEND_CHANGE_MODE_REQ

DESCRIPTION

  This function sends a RRC_CHANGE_MODE_REQ to MCM

          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_change_mode_req( rrc_mode_e_type mode, rrc_proc_e_type proc_id )
{
  rrc_cmd_type *cmd_ptr;

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CHANGE_MODE_REQ;

  /* Fill in procedure id    */
  cmd_ptr->cmd.change_mode_req.procedure = proc_id;

  /* Fill in the requested mode */
  cmd_ptr->cmd.change_mode_req.mode = mode;
  
  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG0_HIGH("RRC_CHANGE_MODE_REQ sent to MCM");

  return;
} /* end of send_change_mode_req */

/*===========================================================================

FUNCTION  RRCMCM_SET_ESTABLISHMENT_CAUSE

DESCRIPTION

  This function sets the global RRC establishment cause appropriately 
  based on the last INTER-RAT GTOW procedure. No change otherwise.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_set_establishment_cause(void)
{
  /* Update est cause with inter-RAT reselection or inter-RAT CCO
   * only when the est cause is Registration.
   
   * Note: this is done to handle the case where UE reselects from
   * GtoW and before registration could be performed on W, an 
   * emergency call is attempted. If the check for registration 
   * is not done, then UE will incorrectly update the est cause
   * to Inter-RAT Resel/CCO and the emergency call cause will be lost
   */
  if(rrc_establishment_cause == RRC_EST_REGISTRATION)
  {
    if(rrcmcm_last_act_cause == RRCMCM_INTERRAT_CELL_RESELECTION)
    {
      rrc_establishment_cause = RRC_EST_INTER_RAT_CELL_RESELECT;
    }
    else if(rrccho_is_gtow_pcco_in_progress())
    {
      rrc_establishment_cause = RRC_EST_INTER_RAT_CELL_CHANGE_ORDER;
    }
    else
    {
    // Add more cases here
    }
  }

  rrcmcm_last_act_cause = RRCMCM_ACT_CAUSE_NONE;

} /* rrcmcm_set_establishment_cause */


/*===========================================================================

FUNCTION  RRCMCM_INIT_DATA

DESCRIPTION

  This function clears all the module variables and puts the initial
  values wherever needed.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_init_data( rrcmcm_init_e_type init_type )
{                   
  if( (init_type == INIT_ALL) || (init_type == INIT_STATE_VARS_ONLY) )
  {
    /* Reset variables */
    rrcmcm_state = RRCMCM_INITIAL;
    stop_cause = RRC_RAT_CHANGE;
    rrcmcm_est_req_saved_flag = FALSE;
    rrcmcm_send_stop_cnf_to_mm_flag = FALSE;
  }
  
  if( (init_type == INIT_ALL) || (init_type == INIT_CMD_STORE_ONLY) )
  {
    /* init cmd_store */
    memset(&rrcmcm_cmd_store,0,sizeof(rrcmcm_cmd_store));
    rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_RR_ABORT_NONE;
    rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_NONE;
    rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;
  }
  
  return;
}

/*===========================================================================

FUNCTION  rrcmcm_save_est_req

DESCRIPTION

  This function saves est_req if received in STANDBY state
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_save_est_req
(
rrc_cmd_type *cmd_ptr
)
{
  /*copy the contents of est_req from the command locally and set saved flag
    to true*/
  WCDMA_MEMCPY(&rrcmcm_est_req, 
               sizeof(rrc_est_req_type),
               &cmd_ptr->cmd.rrc_est_req, 
               sizeof(rrc_est_req_type));
  rrcmcm_est_req_saved_flag = TRUE;
  WRRC_MSG0_HIGH("Saved  RRC_EST_REQ");
}
/*===========================================================================

FUNCTION  rrcmcm_process_est_req

DESCRIPTION

  This function processes est req if received in STANDBY state..
  If EST_REQ is saved in STANDBY state then it is sent when
  mode changes to ACTIVE.
  If EST_REQ is saved in STANDBY state then it is deleted when
  mode changes to INACTIVE
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_process_est_req
(
rrc_mode_e_type rrc_est_mode
)
{
  rrc_cmd_type *est_req_cmd = rrc_malloc(sizeof(rrc_cmd_type));


  if(rrc_est_mode == RRC_MODE_ACTIVE)
  {
    /*set cmd id to EST_REQ*/
    est_req_cmd->cmd_hdr.cmd_id = RRC_EST_REQ;

    /*copy contents of est req command earlier saved*/
    WCDMA_MEMCPY(&est_req_cmd->cmd.rrc_est_req,
                 sizeof(rrc_est_req_type), 
                 &rrcmcm_est_req, 
                 sizeof(rrc_est_req_type));
    
    /*call rrcrce procedure as it would have been called if not for STANDBY state*/
    rrcrce_procedure_event_handler( est_req_cmd );
  }

  if((rrc_est_mode == RRC_MODE_ACTIVE) || (rrc_est_mode == RRC_MODE_INACTIVE))
  {
    /*whether rrc mode was ACTIVE or INACTIVE this flag needs to be reset*/
    rrcmcm_est_req_saved_flag = FALSE;
  }
  rrc_free(est_req_cmd);
}

#ifdef FEATURE_WRLF_SYSTEM_SEL
/*===========================================================================

FUNCTION  rrcmcm_send_gtow_bplmn_search_cnf_failed

DESCRIPTION

  This function sends GTOW BPLMN search CNF to RR as failure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_gtow_bplmn_search_cnf_failed
(
  void
)
{
  rrc_interrat_plmn_srch_cnf_type *interrat_plmn_srch_cnf = rrc_malloc(sizeof(rrc_interrat_plmn_srch_cnf_type));

  interrat_plmn_srch_cnf->available_plmn_list.length = 0;
  interrat_plmn_srch_cnf->srch_done = TRUE;
  interrat_plmn_srch_cnf->hplmn_found = FALSE;

  rr_send_rrc_message_to_rr_internal((void *)(interrat_plmn_srch_cnf), RRC_INTERRAT_PLMN_SRCH_CNF,
                                    sizeof(rrc_interrat_plmn_srch_cnf_type));
  rrc_free(interrat_plmn_srch_cnf);

}
#ifdef FEATURE_INTERRAT_CELL_RESELECTION_GTOW
/*===========================================================================

FUNCTION  rrccsp_send_gtow_reselection_reject

DESCRIPTION

  This function sends GTOW reselection reject to RR.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccsp_send_gtow_reselection_reject
(
  void
)
{
  rrc_interrat_reselect_rej_type rrc_interrat_reselect_rej;

  rrc_interrat_reselect_rej.reject_reason    = RRC_WCDMA_INAVALID_STATE;
   /* Time ( in Sec ) when the same cell should be considered */
  rrc_interrat_reselect_rej.time_to_reselect = 1280;

  /* Send the RRC_INTERRAT_RESELECT_REJ message to RR */
  rr_send_rrc_message_to_rr_internal((void *)(&(rrc_interrat_reselect_rej)), RRC_INTERRAT_RESELECTION_REJ,
          sizeof(rrc_interrat_reselect_rej_type));

  WRRC_MSG1_HIGH("RRC_INTERRAT_RESELECT_REJ sent to GSM RR. wcdma_time_to_reselect is set to %d secs", rrc_interrat_reselect_rej.time_to_reselect);

}
#endif

#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION  rrcmcm_send_ltow_bplmn_search_cnf_failed

DESCRIPTION

  This function sends LTOW BPLMN search CNF to LTE-RRC as failure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_ltow_bplmn_search_cnf_failed
(
  void
)
{
  lte_irat_plmn_srch_rsp_s *plmn_srch_rsp = rrc_malloc(sizeof(lte_irat_plmn_srch_rsp_s));
  plmn_srch_rsp->srch_done   = TRUE;
  plmn_srch_rsp->hplmn_found = rrc_csp_int_data.rrc_rr_plmn_srch_cnf.hplmn_found;
  plmn_srch_rsp->available_plmn_list.length = 0;
  plmn_srch_rsp->detected_frequency_list.num_freqs = 0;
  rrc_send_lte_cmd(&plmn_srch_rsp->msg_hdr,WCDMA_RRC_LTE_PLMN_SRCH_RSP,sizeof(lte_irat_plmn_srch_rsp_s));
  rrc_free(plmn_srch_rsp);

  WRRC_MSG0_HIGH("LTOW:Sent WCDMA_RRC_LTE_PLMN_SRCH_RSP to LTE RRC");
}
/*===========================================================================

FUNCTION  rrccsp_send_ltow_reselection_reject

DESCRIPTION

  This function sends LTOW reselection CNF to LTE-RRC as failure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccsp_send_ltow_reselection_reject
(
  void
)
{
  wcdma_rrc_lte_resel_failed_rsp_type resel_failed_rsp;
  resel_failed_rsp.failure_cause = WCDMA_RRC_ACQUISITION_FAIL;
  resel_failed_rsp.time_to_reselect = 0;
  WRRC_MSG1_HIGH("LTOW: WCDMA_RRC_LTE_RESEL_FAILED_RSP sent to LTE RRC, cause %d",resel_failed_rsp.failure_cause);
  rrc_send_lte_cmd(&resel_failed_rsp.msg_hdr,WCDMA_RRC_LTE_RESEL_FAILED_RSP,sizeof(wcdma_rrc_lte_resel_failed_rsp_type));
}
#endif
#endif  /* FEATURE_WRLF_SYSTEM_SEL */
/*===========================================================================

FUNCTION  RRCMCM_IS_PROCESS_REQUIRED

DESCRIPTION

  This function returns True if the incoming command is required a further
  process based on the RRC mode. Otherwise, the False is returned.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcmcm_is_process_required( rrc_cmd_type *cmd_ptr )
{                   
  boolean return_status = FALSE;

  rrc_rr_cmd_type *rr_cmd = (rrc_rr_cmd_type *)cmd_ptr;
  

#ifdef FEATURE_DUAL_SIM
  if((cmd_ptr->cmd_hdr.cmd_id == RRC_DS_STATUS_CHANGE_IND) ||
     (cmd_ptr->cmd_hdr.cmd_id == RRC_MODE_PREF_CHANGE_IND))
  {
      return TRUE;
  }

  if(cmd_ptr->cmd_hdr.cmd_id == RRC_DS_TUNEAWAY_STATUS_CHANGE_IND)
  {
    return TRUE;
  }
  if(cmd_ptr->cmd_hdr.cmd_id == RRC_MULTIMODE_SUBS_CHGD_IND)
  {
    return TRUE;
  }
#endif

  if(cmd_ptr->cmd_hdr.cmd_id == RRC_UE_MODE_IND)
  {
    if((cmd_ptr->cmd.rrc_ue_mode_ind.ue_mode == SYS_UE_MODE_SGLTE_HOME) ||
       (cmd_ptr->cmd.rrc_ue_mode_ind.ue_mode == SYS_UE_MODE_SGLTE_TEST_ONLY))
    {
      rrc_is_ue_in_sglte_mode = TRUE;
    }
    else
    {
      rrc_is_ue_in_sglte_mode = FALSE;
    }
    return FALSE;
  }

#ifdef FEATURE_3GPP_CSFB
  if(cmd_ptr->cmd_hdr.cmd_id == RRC_CSFB_CALL_STATUS_IND)
  {
    /*Process the CSFB status Ind from NAS in all modes and states*/
    return TRUE;
  }
#endif

#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
  if((cmd_ptr->cmd_hdr.cmd_id == RRC_CPHY_MDSP_HALT_IND) ||
     (cmd_ptr->cmd_hdr.cmd_id == RRC_CPHY_RECOVER_WCDMA_CNF))
  {
    return TRUE;
  }
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
  if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
  {
#ifdef FEATURE_INTERRAT_CELL_RESELECTION_GTOW
    if((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_INTERRAT_RESELECTION_REQ)
    {
      WRRC_MSG0_HIGH("WRLF:UE in connected mode OOS. Cannot handle GTOW resel req...Send reject");
      rrccsp_send_gtow_reselection_reject();
      return FALSE;
    }
    else 
#endif
    if((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_INTERRAT_PLMN_SRCH_REQ)
    {
      WRRC_MSG0_HIGH("WRLF:UE in connected mode OOS. Cannot handle GTOW PLMN list req...Send reject");
      rrcmcm_send_gtow_bplmn_search_cnf_failed();
      return FALSE;
    }
    else if(cmd_ptr->cmd_hdr.cmd_id == RRC_PLMN_LIST_REQ)
    {
      if(rrc_csp_is_suitable_srch_ended())
      {
        /*Agreement with NAS: In COOS, MPLMN Search request will only be sent after NAS has indicated RRC_ABORT_REQ
         *Thus save MPLMN request for now untill, RRC Connection is aborted.
         */
        rrc_svd_plmn_req_ptr = (rrc_plmn_list_req_type *)rrc_malloc(sizeof(rrc_plmn_list_req_type));
        WCDMA_MEMCPY(rrc_svd_plmn_req_ptr,
                     sizeof(rrc_plmn_list_req_type),
                     &cmd_ptr->cmd.plmn_list_req, 
                     sizeof(rrc_plmn_list_req_type));

        WRRC_MSG0_HIGH("WRLF:MPLMN in COOS: Hard abort in progress. Save PLMN list req.");
      }
      else
      {
      WRRC_MSG0_HIGH("WRLF:UE in connected mode OOS. Cannot handle PLMN list req.");
      rrc_csp_send_mm_plmn_list_cnf_as_aborted(cmd_ptr->cmd.plmn_list_req.trans_id);
      }
      return FALSE;
    }
    else if((cmd_ptr->cmd_hdr.cmd_id == RRC_BPLMN_SEARCH_ABORT_REQ)&&
            (rrc_svd_plmn_req_ptr != NULL)) 
    {
      rrc_csp_send_mm_plmn_list_cnf_as_aborted(rrc_svd_plmn_req_ptr->trans_id);
      rrc_free(rrc_svd_plmn_req_ptr);
      return FALSE;
    } 
    else if((cmd_ptr->cmd_hdr.cmd_id == RRC_ABORT_REQ) &&
              (rrc_svd_plmn_req_ptr!=NULL))
    {
      /*For cases where RRC_ABORT_REQ(hard_abort) followed by PLMN_LIST_REQ which is then again followed by RRC_ABORT_REQ(hard abort for power off and lpm scenario)
       *Ignore the new abort cnf as one is already in progress. Reject save PLMN List Req for this hard abort request*/ 
      WRRC_MSG0_HIGH("WRLF:MPLMN in COOS: Hard abort already in progress. Abort saved PLMN list req.");
      rrc_csp_send_mm_plmn_list_cnf_as_aborted(rrc_svd_plmn_req_ptr->trans_id);
      rrc_free(rrc_svd_plmn_req_ptr);    
      return FALSE;
    }
#ifdef FEATURE_LTE_TO_WCDMA
    else if(cmd_ptr->cmd_hdr.cmd_id == RRC_LTE_RRC_CMD)
    {
      switch(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id)
      {
        case WCDMA_RRC_LTE_RESEL_REQ:
          WRRC_MSG0_HIGH("WRLF:UE in connected mode OOS. Cannot handle LTOW resel req.Send reject");
          rrccsp_send_ltow_reselection_reject();
          break;

        case WCDMA_RRC_LTE_PLMN_SRCH_REQ:
          WRRC_MSG0_HIGH("WRLF:UE in connected mode OOS. Cannot handle LTOW PLMN list req.Send reject");
          rrcmcm_send_ltow_bplmn_search_cnf_failed();
          break;

        default:
          WRRC_MSG1_ERROR("LTE command not handled %d", cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id);
          break;
      }
      return FALSE;
    }
#endif
  }
#endif

  /*After we have received a RRC_MODE_CHANGE_REQ we should not receive any
    other Inter_Layer command.. Print a message if any commands are received,
    this will help debug if race conditions are seen
   */
  if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_MODE_CHANGE)
  {
    WRRC_MSG1_HIGH("Cmd %x rcvd after Mode_Change Rcvd", cmd_ptr->cmd_hdr.cmd_id);
  }

#ifdef FEATURE_WCDMA_TO_LTE
  if((cmd_ptr->cmd_hdr.cmd_id == RRC_LTE_RRC_CMD) &&
    (cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == LTE_RRC_EUTRA_CAPABILITIES_RSP))
  {
    WRRC_MSG1_HIGH("Received EUTRA cap rsp in rrc mode %d.", rrc_mode);
    return TRUE;
  }
#endif

  /* Check the command identifier based the RRC mode */
  switch (rrc_mode)
  {
    case RRC_MODE_INACTIVE:
    {
      rrc_rr_cmd_e_type rrc_rr_cmd;
      WRRC_MSG1_HIGH("Cmd received in Inactive: %x", cmd_ptr->cmd_hdr.cmd_id);

      /* If RRC is in inactive mode */
      rrc_rr_cmd = (rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id;

      if(rrc_rr_cmd == RRC_INTERRAT_PLMN_SRCH_REQ
         || rrc_rr_cmd == RRC_INTERRAT_PLMN_SRCH_ABORT_REQ
         #ifdef FEATURE_INTERRAT_CELL_RESELECTION_GTOW
         || rrc_rr_cmd == RRC_INTERRAT_RESELECTION_REQ
         || rrc_rr_cmd == RRC_INTERRAT_RESELECTION_ABORT_REQ
         #endif
         || rrc_rr_cmd == RRC_INTERRAT_HANDOVER_REQ
         || rrc_rr_cmd == RRC_INTERRAT_HANDOVER_ABORT_REQ
         || rrc_rr_cmd == RRC_INTERRAT_CC_ORDER_REQ
         || rrc_rr_cmd == RR_CIPHER_SYNC_IND
         || rrc_rr_cmd == RRC_INTERRAT_REDIRECT_REQ
         || rrc_rr_cmd == RRC_INTERRAT_REDIRECT_ABORT_REQ
         || rrc_rr_cmd == RR_RRC_STOP_GSM_CNF
        )
      {
          return_status = TRUE;
      }
      else if(rrc_rr_cmd == RRC_RR_STOP_WCDMA_REQ)
      {
        #ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
        #error code not present
#endif
      }
      else
      {
       switch (cmd_ptr->cmd_hdr.cmd_id)
       {
        /* Only the following commands are processed */
        case RRC_SERVICE_REQ:
        case RRC_CPHY_START_WCDMA_MODE_CNF:
        case RRC_ABORT_REQ:
        case RRC_DEACT_REQ:
        case RRC_MODE_CHANGE_REQ:
        case RRC_LOWER_LAYER_DEACT_REQ:
        case RRC_CPHY_DEACTIVATE_CNF:
        case RRC_CRLC_RELEASE_ALL_CNF:
        case RRC_LOWER_LAYER_DEACT_CNF:
        case RRC_INVALIDATE_SIM_DATA_REQ:
        case RRC_SIM_INSERTED_REQ:
        case RRC_SIM_UPDATE_REQ:
        case RRC_STOP_WCDMA_MODE_REQ:
        case RRC_FORBIDDEN_LAI_LIST_UPDATE_REQ:
        case RRC_EQ_PLMN_LIST_CHANGE_IND:
        case RRC_MODE_CHANGE_IND:
/*lint -e408*/
        
        case RRC_BPLMN_FREQ_SCAN_TIMER_EXPIRED_IND:
        case RRC_MANUAL_PLMN_FREQ_SCAN_TIMER_EXPIRED_IND:

        case RRC_BPLMN_SEARCH_ABORT_REQ:

/*lint +e408*/
        case RRC_CHANGE_MODE_CNF:
/*lint -e408*/
        case RRC_CHANGE_MODE_REQ:
        case RRC_HO_CIPH_CNF:
/*lint +e408*/
        case RRC_PLMN_LIST_REQ:
        case RRC_CHANGE_MODE_IND:
        case RRC_NEW_STATE_IND:
        case RRC_STATE_CHANGE_IND:
        case RRC_FREQ_SCAN_TIMER_EXPIRED_IND:
        case RRC_DEEP_SLEEP_NO_SVC_TIMER_EXPIRED_IND:
        case RRC_IRAT_ABORT_IND:
        case RRC_SIX_HRS_SIB_TIMER_UPDATE_IND:
        case RRC_TEN_MIN_SIB_TIMER_UPDATE_IND:
#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
        case RRC_DED_PRI_VALDITY_TIMER_EXPIRED_IND:
#endif
        case RRC_SIB_CHANGE_GUARD_TIMER_EXPIRED_IND:
        /*RR will send RR_CIPHER_SYNC_IND command to RRC when 
         *RRC is inactive, so return success here so that this command 
         *can be processed in dispatcher*/
/*lint -e408*/
/*lint +e408*/
/*lint -e408*/
#ifdef FEATURE_GAN
        #error code not present
#endif

        
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
        #error code not present
#endif
#ifdef FEATURE_TDSCDMA_TO_WCDMA
        case TDSRRC_RRC_STOP_TDS_CNF:
#endif
/*lint +e408*/
#ifdef FEATURE_EOOS
        #error code not present
#endif
        case RRC_SET_DRX_IND:
#ifdef FEATURE_FEMTO_CSG
        case RRC_CSG_WHITE_LIST_UPDATE_IND:
#endif
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
        case RRC_LAI_REJECT_LIST_IND:
        case RRC_TUNEAWAY_ALLOWED_IND:
#endif
#ifdef FEATURE_MODEM_CONFIG_REFRESH
        case RRC_NV_REFRESH_IND:
#endif
#if defined(FEATURE_QSH_EVENT_METRIC) || defined(FEATURE_QSH_DUMP)
        case RRC_QSH_PROCESS_CMD:
#endif
        {
          /* The command needs to be processed */
          return_status = TRUE;
          break;
        }

/*lint -e408*/
        
/*lint +e408*/

        case RRC_LLC_CMD_REQ:
          if(((cmd_ptr->cmd.rrc_llc_req.cmd_dest == LAYER2_MAC_UL) &&
              (cmd_ptr->cmd.rrc_llc_req.l2_ul_cmd.cmd_id == CMAC_UL_TRAFFIC_VOLUME_CONFIG_REQ) &&
              (cmd_ptr->cmd.rrc_llc_req.l2_ul_req != NULL ) &&
              (cmd_ptr->cmd.rrc_llc_req.l2_ul_req->mac_ul_traffic_vol_config->meas_cmd == MAC_TRAFFIC_MEAS_DELETE_ALL)&&
              (cmd_ptr->cmd.rrc_llc_req.l2_ul_req->mac_ul_traffic_vol_config->config_conf_ind == FALSE))
#ifdef FEATURE_FEMTO_CSG
              ||((cmd_ptr->cmd.rrc_llc_req.cmd_dest == LAYER1_PHY)&&
                 (cmd_ptr->cmd.rrc_llc_req.l1_cmd.cmd_id == CPHY_CELL_BAR_REQ))
#endif
            )
          {
            WRRC_MSG1_HIGH("LLC CMD being handled in inactive mode %d",cmd_ptr->cmd.rrc_llc_req.cmd_dest);
            return_status = TRUE;
          }
          else
          {
            return_status = FALSE;
          }
          break;

#ifdef FEATURE_LTE_TO_WCDMA
          case RRC_LTE_RRC_CMD:
            switch(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id)
            {
              case WCDMA_RRC_LTE_RESEL_REQ:
              case WCDMA_RRC_LTE_ABORT_RESEL_REQ:
              case WCDMA_RRC_LTE_PLMN_SRCH_REQ:
              case WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ:
              case WCDMA_RRC_LTE_REDIR_REQ:
              case WCDMA_RRC_LTE_ABORT_REDIR_REQ:
              case WCDMA_RRC_LTE_UTRA_CAPABILITIES_REQ:
              case WCDMA_RRC_LTE_GET_DEDICATED_PRI_REQ:
              case WCDMA_RRC_LTE_CLEAR_DEDICATED_PRI_IND:
              case WCDMA_RRC_LTE_PSHO_REQ:
              case WCDMA_RRC_LTE_ABORT_PSHO_REQ:
              case WCDMA_RRC_LTE_GET_CGI_REQ:
              case WCDMA_RRC_LTE_ABORT_CGI_REQ:
              case WCDMA_RRC_LTE_DEPRI_FREQ_REQ:
#ifdef FEATURE_FEMTO_CSG
              case LTE_RRC_WCDMA_RESEL_RSP:
#endif
              case WCDMA_RRC_LTE_GET_PLMN_PRTL_RESULTS_REQ:
                return_status = TRUE;
                break;
              default :
                return_status = FALSE;
                break;
            }
            break;
#endif
          case RRC_QMI_CMD:
            switch(cmd_ptr->cmd.wcdma_rrc_qmi_cmd.cmd.msg_hdr.id)
            {
              case WCDMA_RRC_QMI_MTC_CFG_REQ:
                return_status = TRUE;
                break;
              default :
                return_status = FALSE;
                break;
            }
            break;
#ifdef FEATURE_WRLF_SYSTEM_SEL
         case RRC_INITIATE_CELL_SELECTION_IND:
         case RRC_CHANNEL_CONFIG_REQ:
         case RRC_CHANNEL_CONFIG_CNF:
         case RRC_T314_EXPIRED_IND:
         case RRC_T315_EXPIRED_IND:
         case RRC_T305_EXPIRED_IND:
         case RRC_T307_EXPIRED_IND:
         case RRC_T316_EXPIRED_IND:
         case RRC_T317_EXPIRED_IND:
         case RRC_CONN_MODE_OOS_TIMER_EXPIRED_IND:
         case RRC_RRC_CON_DELAY_TIMER_EXPIRED_IND:
         case RRC_SIG_CONN_RELEASE_REQ:
           if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
           {
             return_status = TRUE;
           }
           else
           {
             return_status = FALSE;
           }
          break;
#endif

        /* Return False for other commands */
        default:
        { 
          return_status = FALSE;
          break;
        }
      }
     }
      break;
    }

    case RRC_MODE_STANDBY:
    {
      /*if EST_REQ is received save it for future use when we transition
        to ACTIVE substate*/
      rrc_csp_scan_e_type curr_scan = rrccsp_get_curr_scan();
      switch (cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_EST_REQ:
          /* Check if WTOG BPLMN is currently active.If yes,then send an 
             IRAT_ABORT_IND to CSP procedure to abort the bplmn search */
          if((RRC_CSP_WTOG_BPLMN_AUTOMATIC_SCAN == curr_scan)
              || (RRC_CSP_WTOG_BPLMN_MANUAL_SCAN == curr_scan)
              || ((RRC_CSP_INTER_RAT_REDIRECTION_SCAN == curr_scan)
                   &&(cmd_ptr->cmd.rrc_est_req.cause == RRC_EST_DETACH)))
          {
            rrcmcm_send_irat_abort_ind(); 
          }

          rrcmcm_save_est_req(cmd_ptr);
          return_status = FALSE;
          break;
        
        case RRC_ABORT_REQ:
          /* If an abort is received when an est_req is already pending,
           * clear the pending flag and return abort_cnf to mm.No
           * further processing is reqd on this abort,so return false
           */  
          if (rrcmcm_est_req_saved_flag == TRUE)
          {
            WRRC_MSG0_HIGH("Abort Req in Standby mode with pending Est_req.Clearing Est_Req");
            rrcmcm_est_req_saved_flag = FALSE;
            (void)rrcrce_send_rrc_abort_cnf();
            return_status = FALSE;
          }
          else
          {
            /* Abort IRAT activity when ABORT REQ is received from NAS */
            if((procedure_id == RRC_PROCEDURE_CSP) &&
               ((rrc_csp_int_data.curr_scan == RRC_CSP_WCDMA_TO_GSM_CELL_RESELECTION_SCAN) ||
               (rrc_csp_int_data.curr_scan == RRC_CSP_INTER_RAT_REDIRECTION_SCAN)))
            {
              rrcmcm_send_irat_abort_ind();
            }
            return_status = TRUE;
          }
          break;

        case RRC_CMAC_UL_TRAFFIC_IND:
        case RRC_MEASUREMENT_IND:
        /*Ignore TVM/GPS meas report when we are in standby and CCO is in progress*/
          if(!rrccho_is_wtog_cco_active())
          {
             return_status = TRUE;
          }

          break;

        default:
          /*set status to true as all other events are honoured in this
            substate*/
          return_status = TRUE;
          break;
      }
     }
      break;

    case RRC_MODE_ACTIVE:
    case RRC_MODE_INTERRAT_IN_PROGRESS:
    {
      /* Process all commands in the active mode */
      return_status = TRUE;
      break;
    }
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
    #error code not present
#endif
    default:
    {
      if(((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id) == RR_CIPHER_SYNC_IND)
      {
          return_status = TRUE;
      }
      else
      {
       switch (cmd_ptr->cmd_hdr.cmd_id)
       {
        /* Only the following commands are processed */
        case RRC_FREQ_SCAN_TIMER_EXPIRED_IND:
        case RRC_BPLMN_FREQ_SCAN_TIMER_EXPIRED_IND:
        case RRC_MANUAL_PLMN_FREQ_SCAN_TIMER_EXPIRED_IND:
        case RRC_SIX_HRS_SIB_TIMER_UPDATE_IND:
        case RRC_TEN_MIN_SIB_TIMER_UPDATE_IND:
        case RRC_SIB_CHANGE_GUARD_TIMER_EXPIRED_IND:
        /*RR will send RR_CIPHER_SYNC_IND command to RRC when 
         *RRC is inactive, so return success here so that this command 
         *can be processed in dispatcher*/
/*lint -e408*/
  
/*lint +e408*/
#ifdef FEATURE_EOOS
        #error code not present
#endif
          return_status = TRUE;
          break;
       
        default :
          /* RRC Mode is not supported */
          WRRC_MSG1_ERROR("Unsupported RRC Mode: %d", rrc_mode);
          return_status = FALSE;
          break;
      }
    }
   }
  }

#ifndef FEATURE_GAN_3G_ROVE_OUT_ONLY
  if((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_RR_STOP_WCDMA_REQ)
  {
    rr_cmd = (rrc_rr_cmd_type *)cmd_ptr;
    WRRC_MSG1_HIGH("Rxed RRC_RR_STOP_WCDMA_REQ from RR , stop_cause %d",
        rr_cmd->cmd.rrc_rr_stop_wcdma_req.stop_req_cause);
    if((rrc_mode == RRC_MODE_INACTIVE) && (rr_cmd->cmd.rrc_rr_stop_wcdma_req.stop_req_cause == POWER_OFF))
    {
      WRRC_MSG0_HIGH("Rxed RRC_RR_STOP_WCDMA_REQ in Inactive state");
      rrc_csp_write_nv_acq_info();
      wl1_clear_barred_cells();
    }
    /* NAS stopped sending SIM insert request for G only + G only cases.
       Becuase of this RRC doesn't have valid AS-ID.
       Changes are made to respond to same AS-ID as received in STOP request.
    */
#ifdef FEATURE_DUAL_SIM
    rrc_set_as_id(rr_cmd->cmd.rrc_rr_stop_wcdma_req.as_id);
#endif
    rrcmcm_cmd_store.rrc_rr_stop_wcdma_cause = rr_cmd->cmd.rrc_rr_stop_wcdma_req.stop_req_cause;
    rrccsp_send_rrc_rr_stop_wcdma_cnf(rrcmcm_cmd_store.rrc_rr_stop_wcdma_cause); 
    return_status = FALSE;           
  }
#endif

#if defined(FEATURE_TDSCDMA_TO_WCDMA) && defined(FEATURE_WCDMA_TO_TDSCDMA)
  if((rrc_tdsrrc_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_TDSRRC_STOP_WCDMA_REQ)
  {
    WRRC_MSG1_HIGH("Rxed RRC_TDSRRC_STOP_WCDMA_REQ from TDS RRC ,stop_cause %d",
        cmd_ptr->cmd.rrc_tdsrrc_stop_wcdma_req.stop_req_cause);
    if((rrc_mode == RRC_MODE_INACTIVE) && (cmd_ptr->cmd.rrc_tdsrrc_stop_wcdma_req.stop_req_cause == POWER_OFF))
    {
      WRRC_MSG0_HIGH("Rxed RRC_TDSRRC_STOP_WCDMA_REQ in Inactive state");
      rrc_csp_write_nv_acq_info();
      wl1_clear_barred_cells();
    }
    rrcmcm_cmd_store.rrc_rr_stop_wcdma_cause = cmd_ptr->cmd.rrc_tdsrrc_stop_wcdma_req.stop_req_cause;
    rrccsp_send_rrc_tdsrrc_stop_wcdma_cnf(rrcmcm_cmd_store.rrc_rr_stop_wcdma_cause); 
    return_status = FALSE;
  }
#endif
  return (return_status);
} /* end of rrcmcm_is_process_required */


/*===========================================================================

FUNCTION   RRCMCM_GET_SUBSTATE

DESCRIPTION

  This function retrieves the RRC MCM SUBSTATE.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  Returns rrcmcm_state in rrcmcm_state_e_type

SIDE EFFECTS

  None.

===========================================================================*/
rrcmcm_state_e_type rrcmcm_get_substate( void )
{
  /* return the current RRC mode */
  return (rrcmcm_state);
}
/*===========================================================================

FUNCTION   RRCMCM_GET_RRC_MODE

DESCRIPTION

  This function retrieves the RRC current mode.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  Returns rrc_mode in rrc_mode_e_type

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrc_mode_e_type rrcmcm_get_rrc_mode( void )
{
  /* return the current RRC mode */
  return (rrc_mode);
}
/*===========================================================================

FUNCTION   RRCMCM_UPDATE_RRC_MODE_TO_ACTIVE

DESCRIPTION

  This function sets the current RRC mode to active.Currently called 
  by CHO procedure after the GTOW PCCO is successful.
  
  Only IRAT_IN_PROGRESS to ACTIVE is allowed
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_update_rrc_mode_to_active()
{
  if(RRC_MODE_INTERRAT_IN_PROGRESS == rrc_mode)
  {
    WRRC_MSG0_HIGH("RRC Mode updated to Active from IRAT_IN_PROGRESS");
    rrc_mode = RRC_MODE_ACTIVE;
    rrcsmc_reset_pending_cs_key_to_be_activated();
  }
  else
  {
    WRRC_MSG1_ERROR("Invalid request for rrc_mode update to active.Current rrc_mode is %d",rrc_mode);
#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
    /*Still set to ACTIVE*/
    rrc_mode = RRC_MODE_ACTIVE;
#endif

  }
}
/*===========================================================================

FUNCTION   RRCMCM_IS_DUALMODE_ENABLED_no_f3

DESCRIPTION

  This function returns a boolean to indicate if inter-RAT operation is
  enabled.

DEPENDENCIES

  None.

RETURN VALUE

  Returns a boolean value.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcmcm_is_dualmode_enabled_no_f3(void)
{
  /*WRRC_MSG1_HIGH("Dual mode status %d", rrcmcm_int_data.rrc_dual_mode_enabled);*/
  return (rrc_dual_mode_enabled);
}

/*===========================================================================

FUNCTION   RRCMCM_IS_DUALMODE_ENABLED

DESCRIPTION

  This function returns a boolean to indicate if inter-RAT operation is
  enabled.

DEPENDENCIES

  None.

RETURN VALUE

  Returns a boolean value.

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
boolean rrcmcm_is_dualmode_enabled(void)
{
  //WRRC_MSG1_HIGH("Dual mode status %d", rrc_dual_mode_enabled);
  return (rrc_dual_mode_enabled);
}

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)

/*===========================================================================

FUNCTION  RRCMCM_IS_LTE_MODE_ENABLED

DESCRIPTION

  This function returns True if LTE mode is enabled, otherwise FALSE

DEPENDENCIES

  None.

RETURN VALUE

  Return rrc_dual_mode_enabled status in boolean.

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
boolean rrcmcm_is_lte_mode_enabled
(
  void
)
{
  //WRRC_MSG1_HIGH("LTE_IRAT: LTE mode enabled status %d ",rrc_lte_mode_enabled);
  return rrc_lte_mode_enabled;
}

/*===========================================================================

FUNCTION  rrcmcm_update_lte_mode_enabled_status

DESCRIPTION

  This function updates the status of the flag whether if LTE mode is
  enabled or not.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_update_lte_mode_enabled_status
(
  boolean status
)
{
  WRRC_MSG2_HIGH("LTE_IRAT: LTE mode enabled status old:%d, new:%d ",rrc_lte_mode_enabled,status);
  rrc_lte_mode_enabled = status;
}
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST
/*===========================================================================

FUNCTION   RRCMCM_UPDATE_MODE_CAPABILITY

DESCRIPTION

  This function updates GSM, LTE mode capability from NAS message
DEPENDENCIES

  None.

RETURN VALUE

  Returns a boolean value.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_update_mode_capability
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info_ptr
)
{
  uint8 rat_idx = 0;

  rrc_dual_mode_enabled = FALSE;
#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
  rrc_lte_mode_enabled = FALSE;
#endif

  for(rat_idx = 0; rat_idx < rat_pri_list_info_ptr->num_items && rat_idx < SYS_MAX_ACQ_PREF_RAT_LIST_NUM; rat_idx++)
  {
    if(rat_pri_list_info_ptr->priority_list_info[rat_idx].acq_sys_mode == SYS_SYS_MODE_GSM)
    {
      rrc_dual_mode_enabled = TRUE;
      WRRC_MSG0_HIGH("IRAT: GSM mode enabled");
    }
#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
    if(rat_pri_list_info_ptr->priority_list_info[rat_idx].acq_sys_mode == SYS_SYS_MODE_LTE)
    {
      rrc_lte_mode_enabled = TRUE;
      WRRC_MSG0_HIGH("LTE_IRAT: LTE mode enabled");
    }
#endif
  }
}

/*===========================================================================

FUNCTION   RRCMCM_UPDATE_MODE_CAPABILITY_FROM_SERVICE_REQ

DESCRIPTION

  This function updates GSM, LTE mode capability from RRC_SERVICE_REQ.
DEPENDENCIES

  None.

RETURN VALUE

  Returns a boolean value.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_update_mode_capability_from_service_req
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info_ptr
)
{
  uint8 rat_idx = 0;
  rrc_dual_mode_enabled = FALSE;
#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
  rrc_lte_mode_enabled = FALSE;
#endif

  for(rat_idx = 0; rat_idx < rat_pri_list_info_ptr->num_items && rat_idx < SYS_MAX_ACQ_PREF_RAT_LIST_NUM; rat_idx++)
  {
    if(rat_pri_list_info_ptr->priority_list_info[rat_idx].acq_sys_mode == SYS_SYS_MODE_GSM)
    {
      if(rat_pri_list_info_ptr->priority_list_info[rat_idx].bst_rat_acq_required)
      {
        rrc_dual_mode_enabled = TRUE;
      }
    }
    /* Update LTE mode at the start of service search itself */
#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
    else if(rat_pri_list_info_ptr->priority_list_info[rat_idx].acq_sys_mode == SYS_SYS_MODE_LTE)
    {
      rrc_lte_mode_enabled = TRUE;
    }
#endif
  }
#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
  WRRC_MSG2_HIGH("GSM mode enabled status %d , LTE mode enabled status %d",rrc_dual_mode_enabled,rrc_lte_mode_enabled);
#else
  WRRC_MSG1_HIGH("GSM mode enabled status %d",rrc_dual_mode_enabled);
#endif
}


/*===========================================================================

FUNCTION  RRCMCM_IS_MODE_ENABLED_IN_RAT_PRI_LIST

DESCRIPTION

  This function returns True if mode passed as argument is enabled in RAT PRI LIST, otherwise FALSE

DEPENDENCIES

  None.

RETURN VALUE

  Return TRUE is LTE mode is enabled in RAT PRI LIST

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcmcm_is_mode_enabled_in_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info_ptr,
  sys_sys_mode_e_type mode
)
{
  uint8 rat_idx = 0;
  boolean mode_enabled = FALSE;
  for(rat_idx = 0; rat_idx < rat_pri_list_info_ptr->num_items && rat_idx < SYS_MAX_ACQ_PREF_RAT_LIST_NUM; rat_idx++)
  {
    if(rat_pri_list_info_ptr->priority_list_info[rat_idx].acq_sys_mode == mode)
    {
      mode_enabled = TRUE;
      break;
    }
  }
  return mode_enabled;
}
#endif

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif


/*===========================================================================

FUNCTION  RRCMCM_REGISTER_FOR_MODE_CHANGE

DESCRIPTION

  This function registers a notification when a RRC mode change occurs for a
  given condition. The from_mode is the mode from which the transition occurs 
  abd the to_mode is the mode to which the tansition occurs.
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_register_for_mode_change(
  rrc_proc_e_type procedure,
  rrc_mode_e_type from_mode,
  rrc_mode_e_type to_mode
)
{
  /* Verify the procedure identifier */
  if (procedure >= MAX_NUM_OF_PROCEDURES) 
  {
    /* Invalid procedure identifier */
    WRRC_MSG1_ERROR("Invalid Procedure ID: %d", procedure);
  }
  else
  {
    /* Check if the list is full */
    if (rrcmcm_notification_counter < (uint16)RRCMCM_MAX_NOTIFICATIONS) 
    {
      /* Store the request into the mode change notification list */
      rrcmcm_notifications[rrcmcm_notification_counter].procedure = procedure;
      rrcmcm_notifications[rrcmcm_notification_counter].from_mode = from_mode;
      rrcmcm_notifications[rrcmcm_notification_counter].to_mode = to_mode;
      rrcmcm_notification_counter++;
    }
    else
    {
      /* the list is full */
      WRRC_MSG0_ERROR("The mode change list is full");
    }
  }
  return;
}

/*===========================================================================

FUNCTION RRCMCM_MAP_IRAT_REASON

DESCRIPTION

  This function maps the internal change mode request reason to
  the appropriate Suspend/Resume reason for L1 command
          
DEPENDENCIES

  None.
 
RETURN VALUE

  Suspend/Resume reason

SIDE EFFECTS

  None.

===========================================================================*/
static l1_suspend_resume_cause_enum_type rrcmcm_map_irat_reason
( 
  rrc_change_mode_reason_e_type reason 
)
{
  /* Fill in the reason for suspension */
  switch(reason)
  {
    case INTERRAT_REASON_OTHER:
    return L1_IRAT_REASON_OTHER;
  case INTERRAT_RESELECTION:
    return L1_IRAT_RESELECTION;
  case INTERRAT_REDIRECTION:
    return L1_IRAT_REDIRECTION;
  case INTERRAT_OOS:
    return L1_IRAT_OOS;
  case INTERRAT_BPLMN:
    return L1_IRAT_BPLMN;
  case INTERRAT_IHO:
    return L1_IRAT_IHO;
  case INTERRAT_CCO:
    return L1_IRAT_CCO;
#ifdef FEATURE_DUAL_SIM
  case INTERRAT_DSIM_TUNEAWAY:
    return L1_IRAT_DUAL_SIM_TUNEAWAY;
#endif
  case INTERRAT_REASON_NONE:
    return L1_IRAT_NONE;
    default:
      WRRC_MSG1_ERROR("Invalid IRAT reason %d in change mode request",reason); 
    return L1_IRAT_NONE;
  }
}

/*===========================================================================

FUNCTION   RRCMCM_SEND_START_WCDMA_REQ

DESCRIPTION

  This function sends a CPHY_START_WCDMA_MODE_REQ to L1 to initiate the 
  WCDMA mode activation. 
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_start_wcdma_req( void )
{
  l1_ext_cmd_type *l1_cmd_ptr;

  /* Get a L1 command buffer */
  l1_cmd_ptr = l1_get_cmd_buf();

  /* Fill in the Command Header */
  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_START_WCDMA_MODE_REQ;

  l1_cmd_ptr->cmd_hdr.act_time_type = L1_ACTIVATION_TIME_NONE;
  l1_cmd_ptr->cmd_hdr.act_time      = 0;
#ifdef FEATURE_DUAL_SIM
  l1_cmd_ptr->cmd_hdr.sys_modem_as_id = rrc_get_as_id();
#endif
  l1_cmd_ptr->cmd.start.mdsp_image = WL1_MDSP_IMAGE_WCDMA_GSM;
  l1_cmd_ptr->cmd.start.is_ue_in_sglte_mode = rrc_is_ue_in_sglte_mode;
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
  #error code not present
#endif

#ifdef FEATURE_DUAL_SIM
  /*Check if the lock is still available with IRAT Client.
    (Lock should be with IRAT Client for all non-critical IRAT Activities)
    If so, do a trm_exchange */
  rrc_perform_trm_exchange();
#endif

  /* Now send the primitive to L1 */
  rrc_send_l1_cmd(l1_cmd_ptr);
  WRRC_MSG0_HIGH("Dispatching out PHY cmd CPHY_START_WCDMA_MODE_REQ to L1");

  return;
} /* end of rrcmcm_send_start_wcdma_req */


/*===========================================================================

FUNCTION   RRCMCM_SEND_SUSPEND_WCDMA_MODE_REQ

DESCRIPTION

  This function sends a CPHY_SUSPEND_WCDMA_MODE_REQ to L1 to initiate the 
  WCDMA mode deactivation. 
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_suspend_wcdma_mode_req
( 
  rrc_change_mode_reason_e_type irat_reason 
)
{
   l1_req_cmd * llc_cmd_l1_req = NULL;
    rrc_cmd_type *cmd_ptr;

    /* Get a RRC internal command buffer */
    cmd_ptr = rrc_get_int_cmd_buf();
    RRC_GET_POINTER_L1_REQ_VALUE(cmd_ptr->cmd.rrc_llc_req.l1_req,llc_cmd_l1_req);
    /* Fill in the LLC Command Header */
    cmd_ptr->cmd_hdr.cmd_id = RRC_LLC_CMD_REQ;
    cmd_ptr->cmd.rrc_llc_req.procedure = RRC_PROCEDURE_MCM;
    cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER1_PHY;

    /* Fill in L1 Command */
    cmd_ptr->cmd.rrc_llc_req.l1_cmd.cmd_id = CPHY_SUSPEND_WCDMA_MODE_REQ;

    if(procedure_id == RRC_PROCEDURE_IHO)
    {
      /* Get the activation time for WtoG handover.. We will suspend WCDMA mode
        only after this activation time*/
      if( rrciho_get_activation_time_for_wtog_ho(&(cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time)) 
        == SUCCESS)
      {
        cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type = L1_ACTIVATION_TIME_CFN;
        cmd_ptr->cmd.rrc_llc_req.l1_cmd.sdu_time      = rrc_cfn_before_decode[procedure_id]; 
      }
      else
      {
        cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type = L1_ACTIVATION_TIME_NONE;
      }
    }
    else if(procedure_id == RRC_PROCEDURE_CHO)
    {
      /* Get the activation time for WtoG Cell Change Order. We will suspend WCDMA mode
      only after this activation time*/
      if( rrccho_get_activation_time_for_wtog_cho(&(cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time)) 
        == SUCCESS)
      {
        cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type = L1_ACTIVATION_TIME_CFN;
        cmd_ptr->cmd.rrc_llc_req.l1_cmd.sdu_time      = rrc_cfn_before_decode[procedure_id]; 
      }
      else
      {
        cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type = L1_ACTIVATION_TIME_NONE;
      }
    }
    else
    {
      /* Set the activation time to NONE */
      cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type = L1_ACTIVATION_TIME_NONE;
    }

    /* Fill in the irat reason for suspension */
    llc_cmd_l1_req->suspend_req.cause = rrcmcm_map_irat_reason(irat_reason);
    WRRC_MSG1_HIGH("IRAT_reason in Suspend Req is %d...Sending CPHY_SUSPEND_WCDMA_MODE_REQ to L1",
          llc_cmd_l1_req->suspend_req.cause);

  /* Now send the primitive to L1 */
  rrc_put_int_cmd(cmd_ptr);

  return;
} /* end of send_suspend_wcdma_mode_req */



/*===========================================================================

FUNCTION   RRCMCM_SEND_RESUME_WCDMA_MODE_REQ

DESCRIPTION

  This function sends a CPHY_RESUME_WCDMA_MODE_REQ to L1 to initiate the 
  WCDMA mode activation through RRC_LLC_CMD_REQ. 
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_resume_wcdma_mode_req
(
  rrc_change_mode_reason_e_type irat_reason 
)
{
    rrc_cmd_type *cmd_ptr;
    l1_req_cmd * llc_cmd_l1_req = NULL;

    /* Get a RRC internal command buffer */
    cmd_ptr = rrc_get_int_cmd_buf();
    RRC_GET_POINTER_L1_REQ_VALUE(cmd_ptr->cmd.rrc_llc_req.l1_req,llc_cmd_l1_req);
   

    /* Fill in the LLC Command Header */
    cmd_ptr->cmd_hdr.cmd_id = RRC_LLC_CMD_REQ;
    cmd_ptr->cmd.rrc_llc_req.procedure = RRC_PROCEDURE_MCM;
    cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER1_PHY;

    /* Fill in L1 Command */
    cmd_ptr->cmd.rrc_llc_req.l1_cmd.cmd_id = CPHY_RESUME_WCDMA_MODE_REQ;
    cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type = L1_ACTIVATION_TIME_NONE;

    /* Fill in the irat reason for suspension */
    llc_cmd_l1_req->resume_req.cause = rrcmcm_map_irat_reason(irat_reason);
    WRRC_MSG1_HIGH("IRAT_reason in Resume Req is %d...Sending CPHY_RESUME_WCDMA_MODE_REQ to L1",
          llc_cmd_l1_req->resume_req.cause);

  /* Now send the primitive to L1 */
  rrc_put_int_cmd(cmd_ptr);

  return;
} /* end of rrcmcm_send_resume_wcdma_mode_req */


/*===========================================================================

FUNCTION   RRCMCM_SEND_STOP_WCDMA_CNF

DESCRIPTION

  This function sends a RRC_STOP_WCDMA_MODE_CNF to MM to initiate the 
  WCDMA mode has been deactivated. 
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_stop_wcdma_cnf(boolean append_rel_ind)
{
  mm_cmd_type *mm_buf_ptr;

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_STOP_WCDMA_MODE_CNF);

#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_stop_wcdma_mode_cnf.as_id = rrc_get_as_id();
#endif

  if((append_rel_ind) 
#ifdef FEATURE_WRLF_SYSTEM_SEL
      && (!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
    )
  {
    /* Retrieve START values from SMC */
    (void) rrcsmc_append_hfns_to_rel_ind(&(mm_buf_ptr->cmd.rrc_stop_wcdma_mode_cnf.start_cs[0]),
                                  &(mm_buf_ptr->cmd.rrc_stop_wcdma_mode_cnf.start_ps[0]));
  }

  /* Put the command on MM queue */
  rrc_send_rrc_message_to_mm(mm_buf_ptr);
  WRRC_MSG0_HIGH("Dispatching out MM cmd / RRC_STOP_WCDMA_MODE_CNF sent to MM");

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
    WRRC_MSG0_HIGH("Resetting hsdpa_hsupa_ind");
    hsdpa_hsupa_ind = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
  }

#ifdef FEATURE_DUAL_SIM
  rrc_stop_lock_wait_timer();
  rrc_release_lock();
#endif

#ifdef FEATURE_3GPP_CSFB
  rrc_csfb_call_status = FALSE;
#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
  rrc_csfb_reset_while_waiting_for_act_rsp = FALSE;
#endif
#endif

  return;
} /* end of rrcmcm_send_stop_wcdma_cnf */



/*===========================================================================

FUNCTION   RRCMCM_SEND_SERVICE_CNF_WITH_FAILURE

DESCRIPTION

  This function sends a RRC_SERVICE_CNF to MM to with the 
  proper failure cause. 
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_service_cnf_with_failure( void )
{
  mm_cmd_type *mm_buf_ptr;

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_SERVICE_CNF);

#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_service_cnf.as_id = rrc_get_as_id();
#endif

  /*if act_cause is cell change order for gtow, set that cause*/
  if(rrcmcm_act_cause == RRCMCM_INTERRAT_CC_ORDER_GTOW)
  {
    mm_buf_ptr->cmd.rrc_service_cnf.status = MM_AS_INTERRAT_CC_ORDER;
  }
  /* Check current RRC state */
  else if (rrcmcm_act_cause == RRCMCM_INTERRAT_HANDOVER_GTOW) 
  {
    /* Set status to MM_AS_INTERRAT_HANDOVER */
    mm_buf_ptr->cmd.rrc_service_cnf.status = MM_AS_INTERRAT_HANDOVER;
  }
  else if(rrcmcm_act_cause == RRCMCM_INTERRAT_CC_ORDER_WTOG_FAILED)
  {
    /* Set status to MM_AS_WTOG_CC_ORDER_FAILED */
    mm_buf_ptr->cmd.rrc_service_cnf.status  = MM_AS_WTOG_CC_ORDER_FAILED;
  }
  else
  {
    /* Set status to MM_AS_INTERRAT_RESELECTION */
    mm_buf_ptr->cmd.rrc_service_cnf.status = MM_AS_INTERRAT_RESELECTION;
  }

  mm_buf_ptr->cmd.rrc_service_cnf.is_cell_id_valid = FALSE;

  /* Put the command on MM queue */
  rrc_send_rrc_message_to_mm(mm_buf_ptr);
  WRRC_MSG0_HIGH("Dispatching out MM cmd / RRC_SERVICE_CNF sent to MM");

  return;
} /* end of rrcmcm_send_service_cnf_with_failure */


/*===========================================================================

FUNCTION   RRCMCM_SEND_PLMN_LIST_CNF_WITH_FAILURE

DESCRIPTION

  This function sends a RRC_PLMN_LIST_CNF to MM to with the
  proper failure cause.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_send_plmn_list_cnf_with_failure
( 
  sys_network_selection_mode_e_type network_select_mode , 
  uint8 trans_id
  , sys_service_search_e_type svc_srch_bgnd
)
{
  mm_cmd_type *mm_buf_ptr;

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_PLMN_LIST_CNF);

#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.as_id = rrc_get_as_id();
#endif

  /*set the number of plmns to 0 in the plmn list*/
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.avail_plmn_list.num_plmns = 0;

  /* Set the status */
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.status = MM_AS_ABORTED;

  /* Set the network select mode */
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.network_select_mode = network_select_mode;
  mm_buf_ptr->cmd.rrc_plmn_list_cnf.trans_id = trans_id;

  mm_buf_ptr->cmd.rrc_plmn_list_cnf.service_search = svc_srch_bgnd;

  /* Put the command on MM queue */
  rrc_send_rrc_message_to_mm(mm_buf_ptr);
  WRRC_MSG2_HIGH("BSS: svc_srch_bgnd %d sent in plmn_list_cnf. RRC_PLMN_LIST_CNF sent to MM with trans_id = %d / Dispatching out MM cmd", 
  	mm_buf_ptr->cmd.rrc_plmn_list_cnf.trans_id, svc_srch_bgnd);

  return;
} /* end of rrcmcm_send_plmn_list_cnf_with_failure */

/*===========================================================================

FUNCTION   RRCMCM_SEND_STOP_LOWER_LAYER_REQ

DESCRIPTION

  This function sends a RRC_STOP_LOWER_LAYER_CNF to RRC/LLC to initiate the 
  WCDMA mode deactivation in the lower layers. 
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_stop_lower_layer_req( boolean due_to_stop_mode )
{
  rrc_cmd_type *cmd_ptr;

  /* Check if WTOG abort is in progress. If so, wait for RR to respond*/
  if ( (rrc_mode == RRC_MODE_STANDBY  ) &&
       (rrccsp_check_wtog_abort_in_progress(stop_cause)) 
     )
  {
    WRRC_MSG0_HIGH("W2G Abort in Progress. Holding Stop till further");
    return;
  }

#ifdef FEATURE_WRLF_SYSTEM_SEL
  /* If stop is received from NAS, when SIB read is in progress for WRLF, release BCCH.*/
  if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
  {
    rrccsp_check_and_teardown_sbcch_during_wrlf();
  }
#endif
  rrccsp_check_and_teardown_sbcch_during_bplmn();

  rrccsp_cleanup_bplmn_state();

  if(rrc_csp_int_data.curr_scan == RRC_CSP_PLMN_LIST_SCAN)
  {
    rrc_csp_send_mm_plmn_list_cnf_as_aborted(rrc_csp_int_data.bplmn_trans_id);
  }

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_STOP_LOWER_LAYER_REQ;

  /* Fill in procedure id    */
  cmd_ptr->cmd.stop_lower_layer_req.procedure = RRC_PROCEDURE_MCM;

  /* Set the cause field */
  if (stop_cause == RRC_DEEP_SLEEP)
  {
    cmd_ptr->cmd.stop_lower_layer_req.cause = L1_DEEP_SLEEP;

    /* Set the UE in deep sleep variable in CSP only if it's due to 
     * STOP_WCDMA_MODE_REQ and not for RRC_MODE_CHANGE_REQ
     */
    if(due_to_stop_mode)
    {
      rrccsp_set_ue_in_deep_sleep();
    }
  }
  else
  {
    cmd_ptr->cmd.stop_lower_layer_req.cause = L1_RAT_CHANGE;
  }
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
  #error code not present
#endif
  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG1_HIGH("RRC_STOP_LOWER_LAYER_REQ to LLC:%d", stop_cause);

  return;
} /* end of rrcmcm_send_stop_lower_layer_req */


/*===========================================================================

FUNCTION   RRCMCM_SEND_MODE_CHANGE_IND

DESCRIPTION

  This function sends a RRC_MODE_CHANGE_IND to the other RRC procedures that
  have registerred for the notification. 
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_mode_change_ind(
  rrc_proc_e_type procedure,
  rrc_mode_e_type from_mode,
  rrc_mode_e_type to_mode 
)
{
  rrc_cmd_type *cmd_ptr;

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_MODE_CHANGE_IND;

  /* Fill in procedure id that will receive this indication */
  cmd_ptr->cmd.mode_change_ind.procedure = procedure;
  cmd_ptr->cmd.mode_change_ind.from_mode = from_mode;
  cmd_ptr->cmd.mode_change_ind.to_mode = to_mode;

  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG1_HIGH("RRC_MODE_CHANGE_IND sent to %d", procedure);

  return;
} /* end of rrcmcm_send_mode_change_ind */


/*===========================================================================

FUNCTION   RRCMCM_SEND_CHANGE_MODE_CNF

DESCRIPTION

  This function sends a RRC_CHANGE_MODE_CNF to the other RRC procedure that
  initiates the mode change. And also processes EST_REQ if saved in STANDBY
  mode.
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_change_mode_cnf(boolean status)
{
  rrc_cmd_type *cmd_ptr;

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CHANGE_MODE_CNF;

  /* Fill in procedure id that will receive this indication */
  cmd_ptr->cmd.change_mode_cnf.procedure = procedure_id;
  cmd_ptr->cmd.change_mode_cnf.status = status;
  if(rrcmcm_state == RRCMCM_WAIT_STOP_CNF_FOR_MODE_CHANGE)
  {
    cmd_ptr->cmd.change_mode_cnf.mode = RRC_MODE_CHANGE;
  }
  else
  {
    cmd_ptr->cmd.change_mode_cnf.mode = rrc_mode;
  }
  
#ifdef FEATURE_DUAL_SIM
  cmd_ptr->cmd.change_mode_cnf.fail_type = rrcmcm_wl1_failure_cause;
#endif

  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG1_HIGH("RRC_CHANGE_MODE_CNF sent to %d", procedure_id);

  /*process EST_REQ if present.. doing this check here itself as this
    will not be present 99% of time.. this check is for a race condition*/
  if(rrcmcm_est_req_saved_flag == TRUE)
  {
    rrcmcm_process_est_req(rrc_mode);
  }
#ifdef FEATURE_DUAL_SIM
  /* Do not release lock for standby if W2G IHO is active because lock needs to be exchanged */
  if((rrc_mode == RRC_MODE_STANDBY) && !(is_iho_w2g_active()))
  {
#ifdef FEATURE_TRIPLE_SIM
   /* Make sure the tuneaway type is LTA before releasing lock  */
    WCDMA_INTLOCK();
    if(rrcwrm_release_lock_if_current_tuneaway_is_lta() == TRUE)
    {
#endif
    rrc_release_lock();
#ifdef FEATURE_TRIPLE_SIM
    }
    WCDMA_INTFREE();
#endif
  }
#endif
  return;
} /* end of rrcmcm_send_change_mode_cnf */


/*===========================================================================

FUNCTION   RRCMCM_SEND_IRAT_ABORT_IND

DESCRIPTION

  This function sends a RRC_IRAT_ABORT_IND to the corresponsing
  Interrat procedure to abort the ongoing Interrat operation 
  inorder to service a higher priority request from MM
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_send_irat_abort_ind(void)
{
  rrc_cmd_type *cmd_ptr;

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_IRAT_ABORT_IND;

  /* Fill in procedure id that will receive this indication */
  cmd_ptr->cmd.rrc_irat_abort_ind.procedure = procedure_id;
  
  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG1_HIGH("RRC_IRAT_ABORT_IND sent to %d", procedure_id);

  return;
} /* end of rrcmcm_send_irat_abort_ind */

/*===========================================================================

FUNCTION   RRCMCM_UPDATE_MODE_AND_PREFERENCE

DESCRIPTION

  This function updates the current RRC mode amd the RAT preference based on
  the new mode and RAT preference information. 
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_update_mode(
  rrc_mode_e_type new_mode
)
{
  int i;
  
#ifdef FEATURE_GAN
  #error code not present
#endif

  /* Check if the new mode is different from the current mode */
  if (rrc_mode != new_mode) 
  {
    /* Check through the mode change notification table to send 
    RRC_MODE_CHANGE_IND to the registerred procedures */
    for (i = 0; i < rrcmcm_notification_counter; i++) 
    {
      /* If the registerred condition is met, then send the indication */
      if ( ((rrcmcm_notifications[i].from_mode == rrc_mode) ||
        (rrcmcm_notifications[i].from_mode == RRC_MODE_WILDCARD)) &&
           ((rrcmcm_notifications[i].to_mode == new_mode)  ||
        (rrcmcm_notifications[i].to_mode == RRC_MODE_WILDCARD)))
      {
        rrcmcm_send_mode_change_ind(rrcmcm_notifications[i].procedure,
            rrcmcm_notifications[i].from_mode,
            rrcmcm_notifications[i].to_mode);
      }
    }    
  }
  
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
  #error code not present
#endif

  /* update the RRC Mode and RAT Preference */
  rrc_mode = new_mode;

#ifdef FEATURE_GAN
  #error code not present
#endif

  if(rrc_mode == RRC_MODE_ACTIVE)
  {
    rrcsmc_reset_pending_cs_key_to_be_activated();
  }

#ifdef FEATURE_DUAL_SIM
  if(new_mode == RRC_MODE_INACTIVE)
  {
    /* Release the lock only for non CSFB cases */
    /* For CSFB, lock needs to be exchanged hence no release here */

    if((procedure_id == RRC_PROCEDURE_IHO)
#ifdef FEATURE_3GPP_CSFB
       || ((rrc_csfb_call_status) && 
           (rrccsp_get_curr_scan_no_f3() == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN)
           )
#endif
      )
    {
      WRRC_MSG0_HIGH("DSIM: CSFB/HO call. Do not release the lock as exchange is attempted");
    }
    else
    {
      rrc_release_lock();
    }
  }
#endif
  return;
} /* end of rrcmcm_update_mode */


/*===========================================================================

FUNCTION   PROCESS_CHANGE_MODE_REQ

DESCRIPTION

  This function processes the RRC_CHANGE_MODE_REQ message received from the
  other procedures and initiates the corresponding actions. The mode it is going
  to change to can only be STANDBY, ACTIVE or INACTIVE.
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_change_mode_req(
  rrc_change_mode_req_type change_mode_req
)
{

  boolean due_to_stop_mode = FALSE;
#ifdef FEATURE_DUAL_SIM
  rrc_state_e_type rrc_state = rrc_get_state();
  wrm_freq_input_type lock_req_bands;
#endif

  MSG_HIGH("Rvd RRC_CHANGE_MODE_REQ from proc:rrc_proc_e_type_value%d. Current RRC Mode:%d,Requested mode:%d",change_mode_req.procedure,rrc_mode,change_mode_req.mode);

  /* Save the destination procedure for confirmation */
  procedure_id = change_mode_req.procedure;

  /* Check what is the mode it is going to change to */
  switch (change_mode_req.mode)
  {
    /* Standby mode */
    case RRC_MODE_STANDBY:
      /* Chech for the current RRC mode */
      switch(rrc_mode)
      {
        /* Current RRC mode is in Active */
        case RRC_MODE_ACTIVE:
          /* send the CPHY_SUSPEND_WCDMA_MODE_REQ to L1 */
          rrcmcm_send_suspend_wcdma_mode_req(change_mode_req.reason);  

          /* Change the current mode to Standby */
          rrcmcm_update_mode(RRC_MODE_STANDBY);

          rrcmcm_state = RRCMCM_WAIT_SUSPEND_CNF;
          break;
        /* Current RRC Mode is Standby */
        case RRC_MODE_STANDBY:
          /* Send Mode confirmation right away */ 
          rrcmcm_send_change_mode_cnf(TRUE);
          WRRC_MSG0_HIGH("RRC already in Standby");
          break;
#ifdef FEATURE_RRC_ERROR_DETECT
        /* Current RRC Mode is RRC_MODE_INTERRAT_IN_PROGRESS */
        case RRC_MODE_INTERRAT_IN_PROGRESS:
          /* Error Fatal bcos this scenario should never happen. WtoG trigger cannot
             happen when GtoW is in progress */
          ERR_FATAL("Invalid change mode req.Curr mode IRAT_IN_PROGRESS, Req mode STANDBY", 0, 0, 0);
          break;
#endif
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
        #error code not present
#endif
        /* Others */
        default:
          /* Invald mode change request */
          WRRC_MSG1_ERROR("Mode change invald from %d to Standby", rrc_mode);
          break;
      }
      break;

    /* Active mode */
    case RRC_MODE_ACTIVE:
      /* Check for the current RRC mode */
      switch(rrc_mode)
      {
        case RRC_MODE_STANDBY:
#ifdef FEATURE_DUAL_SIM
            rrc_perform_trm_exchange();
            if(rrc_state == RRC_STATE_CELL_DCH || rrc_state == RRC_STATE_CELL_FACH)
            {
              uint32 ta_duration, ta_timer_value, max_ta_duration = rrc_get_max_ta_duration();
              rrc_csp_scan_e_type curr_scan = rrccsp_get_curr_scan();
              ta_timer_value = (max_ta_duration > 16000)? max_ta_duration : 16000;

               /*If suspend is triggered for tuneaway make sure tuneaway duration is updated during resume.
                * Setting the default value to zero.
                */
              if(curr_scan == RRC_CSP_UNLOCK_BY_SCAN)
              {
                wl1_fill_tuneaway_duration(0);
                /* Stop the lock wait timer if running */
                rrc_stop_lock_wait_timer();
                rrctmr_start_timer(RRCTMR_GET_TA_DURATION_TIMER, ta_timer_value);
              }
              if((curr_scan == RRC_CSP_UNLOCK_BY_SCAN) || (rrc_ok_for_tune_away() == TRUE))
              {
                rrc_wrm_populate_bands_for_lock_req(&lock_req_bands, rrc_ccm_get_curr_camped_freq());
                /*Send the tune away start event for QXDM logging*/
                event_report(EVENT_WCDMA_RRC_DSIM_TUNEAWAY_START);
                rrc_send_rlc_ta_status(RLC_LTA_START_IND, RLC_TA_SUCC_MAX_ENUM);
                if(rrc_block_wait_to_acquire_lock_for_time(150, TRM_BG_TRAFFIC, lock_req_bands
#ifdef FEATURE_TRIPLE_SIM
                     ,TRUE
#endif
                   ) == FAILURE)
                {
                  /* Send a change mode cnf with failure */
                  rrcmcm_wl1_failure_cause = WL1_FAILED_NO_WRM_LOCK;
                  rrcmcm_update_mode(RRC_MODE_ACTIVE);
                  rrcmcm_send_change_mode_cnf(FALSE);
                  rrcmcm_init_data(INIT_ALL);
                  rrc_send_rlc_ta_status(RLC_LTA_STOP_IND, RLC_FAIL_IND);
                  return;
                }

                /*Send the tune away stop event for QXDM logging*/
                event_report(EVENT_WCDMA_RRC_DSIM_TUNEAWAY_STOP);
                
              }
              else
              if(rrc_acquire_lock_for_resuming_on_wcdma() == FAILURE)
              {
                /* Updating the RRC mode to ACTIVE because CHANGE_MODE_CNF will
                 * be handled in CSP only if RRC is active. Further actions as to what
                 * to be done will be taken care by CSP */
                rrcmcm_wl1_failure_cause = WL1_FAILED_NO_WRM_LOCK;
                rrcmcm_update_mode(RRC_MODE_ACTIVE);
                /* Send a change mode cnf with failure */
                rrcmcm_send_change_mode_cnf(FALSE);
                rrcmcm_init_data(INIT_ALL);
                return;
              }
              if(curr_scan == RRC_CSP_UNLOCK_BY_SCAN)
              {
               /* Get the TA duration and pass it to L1 for optimizing data call resume time 
                  Release the connectin if TA duration exceeds a calculated maximum */
                ta_duration = ta_timer_value - (rrctmr_get_remaining_time(RRCTMR_GET_TA_DURATION_TIMER));

                /* Re-est timers T314/T315 are applicable only in DCH, T317 which is applicable in FACH has a inifite timer value */
                if((ta_duration >= max_ta_duration) && (rrc_state == RRC_STATE_CELL_DCH))
                {
                  WRRC_MSG2_HIGH("TA duration %d is more than re-establishment timers %d",ta_duration,max_ta_duration);
                  rrccu_process_re_est_timers(TRUE);
                }  
                /*Cap the TA duration to a maximum of 16 secs and pass to L1 as per WL1 requirement */              
                if(ta_duration > 16000)
                {
                  ta_duration = 16000;
                }
                wl1_fill_tuneaway_duration(ta_duration/10);
                rrctmr_stop_timer(RRCTMR_GET_TA_DURATION_TIMER);
              }
            }
#endif
          /* Send the CPHY_RESUME_WCDMA_MODE_REQ to L1 */
          rrcmcm_send_resume_wcdma_mode_req(change_mode_req.reason);
          rrcmcm_state = RRCMCM_WAIT_RESUME_CNF;
          break;
        case RRC_MODE_ACTIVE:
          /* Send Mode confirmation right away */
          rrcmcm_send_change_mode_cnf(TRUE);
          WRRC_MSG0_HIGH("RRC already in Active");
          break;   
        default:
          /* Invald mode change request */
          WRRC_MSG1_ERROR("Mode change invald from %d to Active", rrc_mode);
          break;
      }           
      break;

    /* Interrat in Progress mode */
    case RRC_MODE_INTERRAT_IN_PROGRESS:
      /* Check for the current RRC mode */
      if (rrc_mode == RRC_MODE_INACTIVE) 
      {
        rrc_init_dynamic_ptrs();
        /*Update the substate and mode before posting the req to L1 as L1 will start processing before we updating the substate*/
        rrcmcm_update_mode(RRC_MODE_INTERRAT_IN_PROGRESS);
        rrcmcm_state = RRCMCM_WAIT_START_CNF;
        /* Start WCDMA mode */
        rrcmcm_send_start_wcdma_req();
      }
#ifdef FEATURE_GAN_3G
      #error code not present
#endif
      else
      {
        /* Invald mode change request */
        WRRC_MSG1_ERROR("Mode change invald from %d to INP", rrc_mode);
      }
      break;

    /* Inactive mode */
    case RRC_MODE_INACTIVE:

      /* In general we do not process a mode change to Inactive if the current 
         mode is Standby.But in our Spec compliant WTOG Cell Change Order implementation,
         for the successful case,to stop WCDMA,CHO proc is going to send a 
         Change_mode_request to Inactive instead of MM sending a Stop_Wcdma_Req.
         RRC Mode would be standby in this scenario.Hence make an exception and handle
         this request if it is from CHO */
      if((change_mode_req.procedure == RRC_PROCEDURE_CHO) && (rrc_mode == RRC_MODE_STANDBY))
      {
        WRRC_MSG0_HIGH("Honoring Mode Change Req to Inactive from Standby for proc CHO"); 
        due_to_stop_mode = TRUE;
        stop_cause = RRC_RAT_CHANGE;

        /* send stop wcdma request */
        rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);
        rrcmcm_state = RRCMCM_WAIT_STOP_CNF;
      }
#ifdef FEATURE_GAN_3G
      #error code not present
#endif
      /* Check the current RRC mode */
      else if (rrc_mode == RRC_MODE_INTERRAT_IN_PROGRESS) 
      {
        due_to_stop_mode = TRUE;
        /* send stop wcdma request */
        rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);
        rrcmcm_state = RRCMCM_WAIT_STOP_CNF;
      }
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
      #error code not present
#endif
#ifdef FEATURE_DUAL_SIM
      else if((change_mode_req.procedure == RRC_PROCEDURE_CSP) && (rrc_mode == RRC_MODE_ACTIVE))
      {
        stop_cause = RRC_RAT_CHANGE;
        due_to_stop_mode = TRUE;
        /* send stop wcdma request */
        rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);
        rrcmcm_state = RRCMCM_WAIT_STOP_CNF;
      }
#endif
      else
      {
        /* Invald mode change request */
        WRRC_MSG1_ERROR("Mode change invald from %d to Inactive", rrc_mode);
      }      
      break;

    case RRC_MODE_CHANGE:
      if(rrc_mode == RRC_MODE_INACTIVE)
      {
        rrcmcm_state = RRCMCM_WAIT_STOP_CNF_FOR_MODE_CHANGE;
        rrcmcm_send_change_mode_cnf(TRUE);
        rrcmcm_send_mode_change_ind(RRC_PROCEDURE_IHO, 
                                    RRC_MODE_CHANGE,
                                    RRC_MODE_INACTIVE);
        rrcmcm_update_mode(RRC_MODE_INACTIVE);
        rrcmcm_init_data(INIT_ALL);
      }
      else
      {
        /* send stop wcdma request */
        /* For a mode change set the cause to be sent to L1 as DEEP SLEEP */
        stop_cause = RRC_DEEP_SLEEP;
        due_to_stop_mode = FALSE;
        rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);
        rrcmcm_state = RRCMCM_WAIT_STOP_CNF_FOR_MODE_CHANGE;
      }
      break;
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
      #error code not present
#endif
    default:
      /* invalid the RRC mode to change to */
      WRRC_MSG1_ERROR("Invalid the RRC Mode Transition: %d", change_mode_req.mode);
      break;
  }
  return;
} /* end of process_change_mode_req */

#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION   rrcmcm_send_lte_interrat_plmn_rsp_with_failure

DESCRIPTION

  This function sends interrat PLMN search RSP as failure

DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_lte_interrat_plmn_rsp_with_failure(void)
{
  lte_irat_plmn_srch_rsp_s plmn_srch_rsp;

  memset(&plmn_srch_rsp,0,sizeof(lte_irat_plmn_srch_rsp_s));

  plmn_srch_rsp.available_plmn_list.length = 0;
  plmn_srch_rsp.srch_done = FALSE;
  plmn_srch_rsp.hplmn_found = FALSE;
  WRRC_MSG0_HIGH("DSIM: Send WCDMA_RRC_LTE_PLMN_SRCH_RSP to LTE RRC");
  rrc_send_lte_cmd(&plmn_srch_rsp.msg_hdr,WCDMA_RRC_LTE_PLMN_SRCH_RSP,sizeof(lte_irat_plmn_srch_rsp_s));

}
#endif

/*===========================================================================

FUNCTION   rrcmcm_acquire_lock_and_calc_rem_dur

DESCRIPTION

  This function acquires lock and calculate the remaining duration after deducting the elapsed
  time to acquire lock from the time indicated by Inter RAT Procedure.
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcmcm_acquire_lock_and_calc_rem_dur(uint32* inter_rat_timer_val)
{
  rrc_out_of_lock_wait_e_type wrm_lock_status;
  uint64 rem_lock_timer;
  wrm_freq_input_type lock_req_bands;
  lock_req_bands.num_bands = 1;
  lock_req_bands.bands[0].band = rrc_return_default_band_lock_req();

/* This is to make sure that W being the dest RAT takes care of trm exchange if performed by source RAT */
  rrc_perform_trm_exchange();
  /* If timer value is less than 195 sent failure response */
  if(*inter_rat_timer_val < 195)
  {
    return FALSE;
  }

  rrc_start_lock_wait_timer(*inter_rat_timer_val - 195,TRUE);
  /* TRM priority and lock request time same as used by L1 for start wcdma req */
  wrm_lock_status = rrc_acquire_wrm_lock(TRM_ACQUISITION,100, lock_req_bands);
 
  if(wrm_lock_status == RRC_WRM_LOCK_ACQUIRED) 
  {
    rem_lock_timer = rrctmr_get_remaining_time(RRCTMR_WRM_LOCK_TIMER);
    *inter_rat_timer_val=(uint32)rem_lock_timer+195; 
    rrc_stop_lock_wait_timer();
    return TRUE;
  }
  else if(wrm_lock_status == RRC_ABORT_FROM_RR)
  {
    /* To handle the cases where abort is received when waiting for lock for WCDMA start */
    switch( rrcmcm_cmd_store.rrcmcm_cmd_flag)
    {
    
#ifdef FEATURE_LTE_TO_WCDMA

      case RRCMCM_CMD_LTE_PLMN_SRCH_REQ:
        rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_LTE_ABORT_PLMN_SRCH_REQ;
      break;

      case RRCMCM_CMD_LTE_IRAT_REQ:
        if(rrcmcm_cmd_store.rrcmcm_lte_irat_cmd.cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == WCDMA_RRC_LTE_GET_CGI_REQ)
        {
          rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_LTE_ABORT_CGI_REQ;
        }
        else
        { 
          /* No action */
        }
      break;

#endif

      case RRCMCM_CMD_INTERRAT_PLMN_SRCH_REQ:
        rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_RR_INTERRAT_PLMN_ABORT_REQ;
      break;

      default:
        rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_RR_ABORT_NONE;
    }
  }
  rrc_stop_lock_wait_timer();
  return FALSE;
}

/*===========================================================================

FUNCTION   rrcmcm_send_interrat_plmn_cnf_with_failure

DESCRIPTION

  This function sends interrat PLMN search CNF as failure

DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_send_interrat_plmn_cnf_with_failure(void)
{
  rrc_interrat_plmn_srch_cnf_type interrat_plmn_srch_cnf;
  interrat_plmn_srch_cnf.available_plmn_list.length = 0;
  interrat_plmn_srch_cnf.srch_done = FALSE;
  interrat_plmn_srch_cnf.hplmn_found = FALSE;
  WRRC_MSG0_HIGH("DSIM: Send RRC_INTERRAT_PLMN_SRCH_CNF");
  rr_send_rrc_message_to_rr_internal((void *)(&interrat_plmn_srch_cnf), RRC_INTERRAT_PLMN_SRCH_CNF,
                                    sizeof(rrc_interrat_plmn_srch_cnf_type));
}
/*===========================================================================

FUNCTION   RRCMCM_HANDLE_START_WCDMA_FAILURE_FOR_DUAL_SIM

DESCRIPTION

  This function handles start wcdma failure due to NO LOCK from L1.

DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_handle_start_wcdma_failure_for_dual_sim(void)
{

  boolean free_dynamic_ptrs = TRUE;

  WRRC_MSG0_HIGH("DSIM: L1 unable to get the lock for START_WCDMA_MODE_CNF");
  /*Check if abort flags are set to TRUE,
     >if Abort flag is set to TRUE, no need to send another STOP_WCDMA_REQ to L1, just behave as if STOP_CNF from L1 is received
     >If No Abort Flags, check the cmd flags for mode_change_req, stop_wcdma_mode_req
     >>send the start wcdma req again to WL1
  */
  /*TBD: Check for ABORT Flags needed here, this is for interRAT Hold on*/
  if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_MODE_CHANGE)
  {
    procedure_id = RRC_PROCEDURE_CSP;
    rrcmcm_state = RRCMCM_WAIT_STOP_CNF_FOR_MODE_CHANGE;
    rrcmcm_send_change_mode_cnf(TRUE);
    rrcmcm_update_mode(RRC_MODE_INACTIVE);
    rrcmcm_init_data(INIT_ALL);
  }
  else if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_STOP_WCDMA_REQ)
  {
    rrcmcm_send_stop_cnf_to_mm_flag = TRUE;
    /* Clear RRC change mode requests,if any */
    rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;
  
    /*TBD: check if mode_change_ind needs to be sent or not*/
    rrcmcm_send_mode_change_ind(RRC_PROCEDURE_CSP, rrc_mode, 
                                RRC_MODE_INACTIVE);
    rrcmcm_send_mode_change_ind(RRC_PROCEDURE_CCM, rrc_mode,
                                RRC_MODE_INACTIVE);
    rrcmcm_update_mode(RRC_MODE_INACTIVE);
    /* Send a Stop WCDMA Mode Confirmation to MM */
    rrcmcm_send_stop_wcdma_cnf(TRUE);
    rrcmcm_init_data(INIT_ALL);
  }
  else if((rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_PLMN_ABORT_REQ)
           ||(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_RESELECTION_ABORT_REQ)
           ||(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_REDIRECT_ABORT_REQ)
           ||(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_PLMN_ABORT_REQ)
#ifdef FEATURE_LTE_TO_WCDMA
           ||(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_PLMN_SRCH_REQ)
           ||(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_REDIR_REQ)
           ||(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_RESEL_REQ)
           ||(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_CGI_REQ)
#endif
         )
  {
    if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_PLMN_ABORT_REQ)
    {
      rrccsp_send_bplmn_abort_cnf_to_rr();
    }
    else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_RESELECTION_ABORT_REQ)
    {
      rrccsp_send_interrat_reselection_abort_cnf();
    }
    else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_REDIRECT_ABORT_REQ)
    {
      rrccsp_send_gtow_redirection_abort_cnf();
    }

    else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_PLMN_ABORT_REQ)
    {
       rrccsp_send_bplmn_abort_cnf_to_rr();
    }
#ifdef FEATURE_LTE_TO_WCDMA
    else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_PLMN_SRCH_REQ)
    {
      rrccsp_send_lte_interrat_plmn_srch_abort_rsp();
    }
    else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_REDIR_REQ)
    {
      rrccsp_send_lte_interrat_redirection_abort_rsp();
    }
    else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_RESEL_REQ)
    {
      rrccsp_send_lte_interrat_reselection_abort_rsp();
    }
    else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_CGI_REQ)
    {
      rrccsp_send_lte_interrat_cgi_abort_rsp();
    }

#endif

    rrcmcm_update_mode(RRC_MODE_INACTIVE);
    /*TBD: Need to take care of SERVICE_REQ or LIST_REQ if pending*/
    rrcmcm_init_data(INIT_ALL);
  }
  /* If L1 fails to get TRM lock for start WCDMA in case of inter-RAT,
     send failure to RR immediately. 
  */
  else if(rrc_mode == RRC_MODE_INTERRAT_IN_PROGRESS)
  {
    if(rrcmcm_cmd_store.rrcmcm_interrat_reselection_req_cmd.cmd_hdr.cmd_id== RRC_INTERRAT_RESELECTION_REQ)
    {
      /* Send the Reselection Rej to GSM RR */
      rrccsp_send_interrat_reselection_reject();
    }
    else if(rrcmcm_cmd_store.rrcmcm_interrat_reselection_req_cmd.cmd_hdr.cmd_id == RRC_INTERRAT_REDIRECT_REQ)
    {
      /* Send the Redirection Rej to GSM RR */
      rrccsp_send_gtow_redirection_reject();
    }
#ifdef FEATURE_LTE_TO_WCDMA
    else if(rrcmcm_cmd_store.rrcmcm_lte_irat_cmd.cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == WCDMA_RRC_LTE_RESEL_REQ)
    {
      rrccsp_send_lte_interrat_reselection_failed_rsp();
    }
    else if(rrcmcm_cmd_store.rrcmcm_lte_irat_cmd.cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == WCDMA_RRC_LTE_REDIR_REQ)
    {
      rrccsp_send_lte_interrat_redirection_failed_rsp();
    }
    else if(rrcmcm_cmd_store.rrcmcm_lte_irat_cmd.cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == WCDMA_RRC_LTE_GET_CGI_REQ)
    {
      rrccsp_send_lte_interrat_cgi_rsp();
    }
#endif

    rrcmcm_update_mode(RRC_MODE_INACTIVE);
    rrcmcm_init_data(INIT_ALL);
  }
  else if(rrcmcm_cmd_store.rrcmcm_interrat_reselection_req_cmd.cmd_hdr.cmd_id == RRC_INTERRAT_PLMN_SRCH_REQ)
  {
    rrcmcm_send_interrat_plmn_cnf_with_failure();
    rrcmcm_update_mode(RRC_MODE_INACTIVE);
    rrcmcm_init_data(INIT_ALL);
  }
#ifdef FEATURE_LTE_TO_WCDMA
  else if(rrcmcm_cmd_store.rrcmcm_lte_irat_cmd.cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id == WCDMA_RRC_LTE_PLMN_SRCH_REQ)
  {
    rrcmcm_send_lte_interrat_plmn_rsp_with_failure();
    rrcmcm_update_mode(RRC_MODE_INACTIVE);
    rrcmcm_init_data(INIT_ALL);
  }
#endif
  else
  {
    /* Send a Start WCDMA mode request to L1 */
    rrcmcm_send_start_wcdma_req();
    rrcmcm_state = RRCMCM_WAIT_START_CNF;
    free_dynamic_ptrs = FALSE;
  }

  if(free_dynamic_ptrs)
  {
    rrc_free_dynamic_ptrs();
  }

}

/*===========================================================================

FUNCTION   RRCMCM_ACQUIRE_LOCK_FOR_WCDMA_START

DESCRIPTION

  This function acquires the lock for WCDMA start for time critical procedures like BPLMN, CGI.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_acquire_lock_for_wcdma_start(uint32* timer_val)
{
  if(rrcmcm_acquire_lock_and_calc_rem_dur(timer_val))
  {
    rrcmcm_state = RRCMCM_WAIT_START_CNF;
    rrcmcm_send_start_wcdma_req();
  }
  else
  {
    rrc_stop_lock_wait_timer();
    rrcmcm_handle_start_wcdma_failure_for_dual_sim();
  }
}

#endif /* FEATURE_DUAL_SIM */

/*===========================================================================

FUNCTION   RRCMCM_PROCESS_MODE_CHANGE_REQ

DESCRIPTION

  This function processes the MODE_CHANGE_REQ message received from CSP
  procedure.  This will stop the WCDMA RAT and take it to FTM/GAMING
  mode.  
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_process_mode_change_req
(
  void
)
{
  rrc_change_mode_req_type change_mode_req;
  
  /*CSP handles this primitive from MM.. The mode to take RRC to is
    RRC_MODE_CHANGE.. set this in change_mode_req type and call the
    process_change_mode_req*/
  change_mode_req.procedure = RRC_PROCEDURE_CSP;
  change_mode_req.mode = RRC_MODE_CHANGE;
  /* For a mode change set the cause to be sent to L1 as DEEP SLEEP */
  stop_cause = RRC_DEEP_SLEEP;

  process_change_mode_req(change_mode_req);
}


/*===========================================================================

FUNCTION   RRCMCM_INITIAL_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the MCM is in the RRCMCM_INITIAL substate. This function
  looks at the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_initial_substate_event_handler(
  rrc_cmd_type *cmd_ptr /* Received command                   */
)
{
  rrc_cmd_type *cmd_ptr_buff;
  boolean due_to_stop_mode = FALSE;
  rrc_state_e_type curr_rrc_state; 
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
  #error code not present
#endif
  lte_rrc_utra_serving_cell_info_rsp_s serving_cell_info;
#ifdef FEATURE_DUAL_SIM
  wrm_freq_input_type lock_req_bands;
#endif

  rrc_rr_cmd_e_type rrc_rr_cmd = (rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id;

  if(rrc_rr_cmd == RRC_INTERRAT_PLMN_SRCH_ABORT_REQ)
  {
     switch(rrc_mode)
     {
      case RRC_MODE_INACTIVE:
      
        /* WCDMA is not up and CSP is not processing a PLMN search currently.Inform CSP to reset 
         all the internal databases related to search and send an abort confirm to RR  */
        WRRC_MSG0_HIGH("Rx:Received RRC_INTERRAT_PLMN_SRCH_ABORT_REQ in Initial state/Inactive mode"); 
        rrccsp_send_bplmn_abort_cnf_to_rr();
        break;
      
       case RRC_MODE_INTERRAT_IN_PROGRESS:
    
         /* Forward the request to CSP because CSP is already processing a BPLMN search request  */
         WRRC_MSG0_HIGH("Rx:Received RRC_INTERRAT_PLMN_SRCH_ABORT_REQ in Initial state/Interrat-in-progress mode ");  
         rrc_cell_selection_procedure( cmd_ptr );                
         break;
      
       default:

         /* This should never happen */
         WRRC_MSG0_HIGH("Rx:Received RRC_INTERRAT_PLMN_SRCH_ABORT_REQ in an Initial state/Invalid mode");  
         break;
     }
  }
  else if(rrc_rr_cmd == RRC_INTERRAT_PLMN_SRCH_REQ)
  {
      switch(rrc_mode)
      {
        case RRC_MODE_INACTIVE:
    
          WRRC_MSG0_HIGH("Rx:Received RRC_INTERRAT_PLMN_SRCH_REQ in Initial state/Inactive mode");
      
          /* Process the request */
          rrcmcm_cmd_store.rrcmcm_interrat_reselection_req_cmd = *((rrc_rr_cmd_type *)cmd_ptr);     // Store the command
          rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_INTERRAT_PLMN_SRCH_REQ ;                    // Set the cmd flag
          rrc_init_dynamic_ptrs();

#ifdef FEATURE_DUAL_SIM
          rrcmcm_acquire_lock_for_wcdma_start(&rrcmcm_cmd_store.rrcmcm_interrat_reselection_req_cmd.
            cmd.interrat_plmn_srch_req.timer_val);
#else
          /*Update the substate and mode before posting the req to L1 as L1 will start processing before we updating the substate*/
          rrcmcm_state = RRCMCM_WAIT_START_CNF;                                                     // Set substate to RRCMCM_WAIT_START_CNF
          rrcmcm_send_start_wcdma_req();                                                            // Start wCDMA
#endif
          break;

        default:

          /* We shouldnt be getting this request in any other mode other than Inactive 
          Do we send a cnf back with failure ? */  
          WRRC_MSG0_HIGH("Rx:Received RRC_INTERRAT_PLMN_SRCH_REQ in an Initial state/Invalid mode");  
          break;  
      }
  }
  #ifdef FEATURE_INTERRAT_CELL_RESELECTION_GTOW 
  else if(rrc_rr_cmd == RRC_INTERRAT_RESELECTION_REQ)
  {
    switch(rrc_mode)
      {
        case RRC_MODE_INACTIVE:

          WRRC_MSG0_HIGH("Rvd RRC_INTERRAT_RESELECTION_REQ from RR");
          /* Start WCDMA mode process */
          rrc_mode = RRC_MODE_INTERRAT_IN_PROGRESS;
          procedure_id = RRC_PROCEDURE_CSP;
          rrc_init_dynamic_ptrs();
          /*Update the substate and mode before posting the req to L1 as L1 will start processing before we updating the substate*/
          rrcmcm_cmd_store.rrcmcm_interrat_reselection_req_cmd = *((rrc_rr_cmd_type *)cmd_ptr);
          rrcmcm_state = RRCMCM_WAIT_START_CNF;
          rrcmcm_send_start_wcdma_req();
          break;

        default:
 
          WRRC_MSG0_HIGH("Rvd RRC_INTERRAT_RESELECTION_REQ from RR in Invalid RRC mode");
          break;
      }
  }
  else if(rrc_rr_cmd == RRC_INTERRAT_RESELECTION_ABORT_REQ)
  {
    WRRC_MSG1_HIGH("Rvd RRC_INTERRAT_RESELECTION_ABORT_REQ from RR in Initial substate,rrc_mode: %d",rrc_mode);
       
      switch(rrc_mode)
      {
        case RRC_MODE_INTERRAT_IN_PROGRESS:
          /* Send the abort ind to csp to abort the interrat process */
          rrcmcm_send_irat_abort_ind();
          break;

        case RRC_MODE_INACTIVE:
          /* Inform CSP to send abort confirmation to rr */
          rrccsp_send_interrat_reselection_abort_cnf();
          break;
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
        #error code not present
#endif
        default:
          /* We should not be getting command in any other substate */
          WRRC_MSG1_HIGH("RRC_INTERRAT_RESELECTION_ABORT_REQ in Invalid mode,rrc_mode: %d",rrc_mode);
          break;
      }
  }
  #endif
  else if(rrc_rr_cmd == RRC_INTERRAT_REDIRECT_REQ)
  {
    switch(rrc_mode)
      {
        case RRC_MODE_INACTIVE:

          WRRC_MSG0_HIGH("Rvd RRC_INTERRAT_REDIRECT_REQ from RR");
          /* Start WCDMA mode process */
          rrc_mode = RRC_MODE_INTERRAT_IN_PROGRESS;
          procedure_id = RRC_PROCEDURE_CSP;
          rrc_init_dynamic_ptrs();
          /*Update the substate and mode before posting the req to L1 as L1 will start processing before we updating the substate*/
          rrcmcm_cmd_store.rrcmcm_interrat_reselection_req_cmd = *((rrc_rr_cmd_type *)cmd_ptr);
          rrcmcm_state = RRCMCM_WAIT_START_CNF;
          rrcmcm_send_start_wcdma_req();
          break;

        default:
 
          WRRC_MSG0_HIGH("Rvd RRC_INTERRAT_REDIRECT_REQ from RR in Invalid RRC mode");
          break;
      }
  }
  else if(rrc_rr_cmd == RRC_INTERRAT_REDIRECT_ABORT_REQ)
  {
    WRRC_MSG1_HIGH("Rvd RRC_INTERRAT_REDIRECT_ABORT_REQ from RR in Initial substate,rrc_mode: %d",rrc_mode);

      switch(rrc_mode)
      {
        case RRC_MODE_INTERRAT_IN_PROGRESS:
          /* Send the abort ind to csp to abort the interrat process */
          rrcmcm_send_irat_abort_ind();
    break;

        case RRC_MODE_INACTIVE:
          /* Inform CSP to send abort confirmation to rr */
          rrccsp_send_gtow_redirection_abort_cnf();
          break;

        default:
          /* We should not be getting command in any other substate */
          WRRC_MSG1_HIGH("RRC_INTERRAT_REDIRECT_ABORT_REQ in Invalid mode,rrc_mode: %d",rrc_mode);
    break;
      }
  }
  else
  {
   switch( cmd_ptr->cmd_hdr.cmd_id )
   {
/*lint -e408*/
/*lint -e740*/
   
/*lint +e408*/
/*lint +e740*/

    case RRC_MODE_CHANGE_REQ:
      /* Check the current RRC mode */
      switch(rrc_mode)
      {
        case RRC_MODE_STANDBY:
          /* Store the cmd */
          WRRC_MSG0_HIGH("Received RRC_MODE_CHANGE_REQ in Standby Mode");
          rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr;
          rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_MODE_CHANGE_REQ;

          /* Abort the ongoing interrat procedure to process the service req */
          rrcmcm_send_irat_abort_ind(); 
          break; 

        default:
          /* In all other cases pass the cmd to CSP */
          rrc_cell_selection_procedure( cmd_ptr );
          break;
      }
      break;

    case RRC_DEACT_REQ:
      /* Check the current RRC mode */
      if(rrc_mode == RRC_MODE_STANDBY)
      {
        /* Store the cmd - not really needed */
        rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr;
        rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_DEACT_REQ;
        WRRC_MSG0_HIGH("Received RRC_DEACT_REQ in Standby Mode. MCM is in standby ... Handling deact request");
      }
      rrc_cell_selection_procedure(cmd_ptr );
      break; 

    case RRC_SERVICE_REQ:
      /* Check current RRC mode */
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
      #error code not present
#endif
      switch(rrc_mode)
      {       
        /* RRC is in Active mode */
        case RRC_MODE_ACTIVE:
#ifdef FEATURE_RAT_PRIORITY_LIST
          rrcmcm_update_mode_capability_from_service_req(&cmd_ptr->cmd.rrc_service_req.rat_pri_list_info);
#else
          /* Set the dual-mode enable flag */
          rrc_dual_mode_enabled = cmd_ptr->cmd.rrc_service_req.dual_mode;
#endif

          /* Invoke the initial cell selection */
          rrc_cell_selection_procedure( cmd_ptr );
          break;

          /* RRC is in Inactive Mode */
        case RRC_MODE_INACTIVE:
          /* Start WCDMA mode, store the RRC_SERVICE_REQ command */
          rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr;
#ifdef FEATURE_WRLF_SYSTEM_SEL
          if(!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
          {
            rrc_init_dynamic_ptrs();
          }
          /*Update the substate and mode before posting the req to L1 as L1 will start processing before we updating the substate*/
          rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_SERVICE_REQ;
          rrcmcm_state = RRCMCM_WAIT_START_CNF;
          /* Send a Start WCDMA mode request to L1 */
          rrcmcm_send_start_wcdma_req();
          break;
        
        case RRC_MODE_STANDBY:
          /* Store the cmd */
          WRRC_MSG0_HIGH("Received RRC_SERVICE_REQ in Standby Mode");
          rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr;
          rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_SERVICE_REQ;
             
          /* Abort the ongoing interrat procedure to process the service req */
          rrcmcm_send_irat_abort_ind(); 
          break; 
 
        case RRC_MODE_INTERRAT_IN_PROGRESS:        
          /*This can happen when a G->W reselection is in progress and the user has
           *changed mode from Auto->W only.We would not get an activation_rsp 
           *if we succeed.Hence treat this service req as an implicit activation_rsp
           */
           WRRC_MSG0_HIGH("Received RRC_SERVICE_REQ in Interrat-in-progress Mode,updating RRC Mode to Active");
#ifdef FEATURE_RAT_PRIORITY_LIST
           rrcmcm_update_mode_capability_from_service_req(&cmd_ptr->cmd.rrc_service_req.rat_pri_list_info);
#else
           rrc_dual_mode_enabled = cmd_ptr->cmd.rrc_service_req.dual_mode;
#endif
           rrcmcm_update_mode(RRC_MODE_ACTIVE);

           /* Forward the service request to CSP */
           rrc_cell_selection_procedure(cmd_ptr);

           /* Init data */
           rrcmcm_init_data(INIT_ALL);
           break;

        /* Others */
        default:
          /* Reject RRC_SERVICE_REQ during inter-RAT operation */
          rrcmcm_send_service_cnf_with_failure();
          break;
      }
      break;    

    /* Mode change indication from MM */
    case RRC_CHANGE_MODE_IND:
      {
        sys_band_mask_type gw_band_pref;
        boolean prev_dual_mode_status = rrc_dual_mode_enabled;
        boolean mode_status_changed = FALSE;

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
        boolean prev_lte_mode_status = rrc_lte_mode_enabled;
#endif
        WRRC_MSG0_HIGH("RRC_CHANGE_MODE_IND is received from MM");


#ifdef FEATURE_RAT_PRIORITY_LIST
        gw_band_pref = rrc_extract_gw_band_pref_from_rat_pri_list(
                                   &cmd_ptr->cmd.rrc_change_mode_ind.rat_pri_list_info);
        rrcmcm_update_mode_capability(&cmd_ptr->cmd.rrc_change_mode_ind.rat_pri_list_info);
        if(!prev_dual_mode_status && rrc_dual_mode_enabled)  
        {
          WRRC_MSG1_HIGH("GSM mode enabled gsm_mode %d",
                rrc_dual_mode_enabled);
          mode_status_changed = TRUE;
        }
#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
        if(prev_lte_mode_status != rrc_lte_mode_enabled)
        {
          WRRC_MSG2_HIGH("LTE_IRAT:new lte_mode %d, prev lte_mode %d",
                rrc_lte_mode_enabled,prev_lte_mode_status);
          mode_status_changed = TRUE;
        }
#endif

#else
        gw_band_pref = cmd_ptr->cmd.rrc_change_mode_ind.band_pref;
        WRRC_MSG0_HIGH("DUAL: Updated the DUAL mode flag"); 
        rrc_dual_mode_enabled = cmd_ptr->cmd.rrc_change_mode_ind.dual_mode;
        if(!prev_dual_mode_status && rrc_dual_mode_enabled)
        {
          mode_status_changed = TRUE;
        }
#endif

        if (TRUE == rrccsp_check_if_3g_band_pref_changed(gw_band_pref))
        {
          WRRC_MSG0_HIGH("DUAL:Band preference is not different from what stored in CSP");
        }
        if (mode_status_changed) 
        {
          /* Set the dual-mode enable flag */
          rr_rrc_set_band_pref_internal(gw_band_pref);
          rrc_update_band_preference(gw_band_pref);
#ifdef FEATURE_RAT_PRIORITY_LIST
          rrc_csp_update_rat_pri_list_info(&cmd_ptr->cmd.rrc_change_mode_ind.rat_pri_list_info);
#endif
          /* Update GSM with UE's band preference */
          WRRC_MSG0_HIGH("DUAL: Updated rr and rrc band preferences");
          rrcmcm_send_dual_mode_info_to_meas_ueci(cmd_ptr);
        }
      }

      break;

    case RRC_PSEUDO_LTE_LIST_UPDATE_IND:
        cmd_ptr_buff = rrc_get_int_cmd_buf();
        cmd_ptr_buff->cmd_hdr.cmd_id = RRC_MODE_STATUS_IND;
        rrcmeas_procedure_event_handler(cmd_ptr_buff);
        rrc_free(cmd_ptr_buff);
        break;

    /* RRC PLMN List request */
    case RRC_PLMN_LIST_REQ:
      WRRC_MSG0_HIGH("Rx:RRC_PLMN_LIST_REQ");

      /* Check current RRC mode */
      switch(rrc_mode)
      {
        /* RRC is in Active Mode */
        case RRC_MODE_ACTIVE:
          /* Forward the command to CSP for processing */
          rrc_cell_selection_procedure(cmd_ptr);
          break;

        /* RRC is in Inactive mode */
        case RRC_MODE_INACTIVE:
          /* Store the command and initiate WCDMA activation procedure */
          rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr;
          rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_PLMN_LIST_REQ;

          rrc_init_dynamic_ptrs();
          /*Update the substate and mode before posting the req to L1 as L1 will start processing before we updating the substate*/
          rrcmcm_state = RRCMCM_WAIT_START_CNF;
          /* Send a Start WCDMA mode request to L1 */
          rrcmcm_send_start_wcdma_req();
          break;

        case RRC_MODE_STANDBY:

          WRRC_MSG0_HIGH("Received RRC_PLMN_LIST_REQ in Standby Mode");

          curr_rrc_state = rrc_get_state();

          /* Do not honor PLMN_LIST_REQ in DCH/FACH states*/
          if(((curr_rrc_state == RRC_STATE_CELL_DCH) || (curr_rrc_state == RRC_STATE_CELL_FACH)) && 
             !(rrcrce_is_abort_in_progress())&&
             !(rrciho_return_iho_abort()==1) /*Abort for WtoX IRAT procedure is not active*/
             /* As per current NAS behaviour, Abort will be sent when MPLMN search is triggered while PS data call is active.
                Hence the above abort in progress checks were added to handle MPLMN search instead of ignoring.
                The same may not be required when NAS doesnt send abort.
             */
            )
          {
            WRRC_MSG1_HIGH("Failing PLMN_LIST_REQ bcos rrc state is DCH/FACH. rrc_state %d",curr_rrc_state);
            rrcmcm_send_plmn_list_cnf_with_failure(cmd_ptr->cmd.plmn_list_req.network_select_mode,
                                                   cmd_ptr->cmd.plmn_list_req.trans_id
                                                   ,cmd_ptr->cmd.plmn_list_req.service_search
                                                   );
          }
          /* Also fail the PLMN_LIST_REQ if UE is in PCH states and an OOS is in progress */
          else if( ((curr_rrc_state == RRC_STATE_CELL_PCH) || (curr_rrc_state == RRC_STATE_URA_PCH)) &&
                   (RRC_CSP_OUT_OF_SERVICE_AREA_SCAN == rrccsp_get_curr_scan()))
          {
            WRRC_MSG1_HIGH("Failing PLMN_LIST_REQ in PCH state with OOS in progress: rrc_state %d",curr_rrc_state);
            rrcmcm_send_plmn_list_cnf_with_failure(cmd_ptr->cmd.plmn_list_req.network_select_mode,
                                                   cmd_ptr->cmd.plmn_list_req.trans_id
                                                   ,cmd_ptr->cmd.plmn_list_req.service_search
                                                   );    
          }
          /* Check if the PLMN Req is Automatic.If Yes,then give priority to the ongoing Interrat procedure
          and send a plmn cnf with failure to NAS.Make sure to populate the fields correctly */
          else if((cmd_ptr->cmd.plmn_list_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
#ifdef FEATURE_FEMTO_CSG
          /*Abort ASF if in progress upon auto PLMN request */
                 &&(!rrccsp_asf_actively_in_prog())
#endif
                 )
          {
            WRRC_MSG0_HIGH("RRC_PLMN_LIST_REQ(Automatic mode) in RRC Standby mode.Sending failure immediately");
            rrcmcm_send_plmn_list_cnf_with_failure(cmd_ptr->cmd.plmn_list_req.network_select_mode,
                                                   cmd_ptr->cmd.plmn_list_req.trans_id
                                                   ,cmd_ptr->cmd.plmn_list_req.service_search
                                                   );    
          }
          else if((curr_rrc_state == RRC_STATE_CONNECTING) && (rrc_csp_int_data.curr_scan == RRC_CSP_INTER_RAT_REDIRECTION_SCAN))
          {
            WRRC_MSG1_HIGH("Failing PLMN_LIST_REQ bcos rrc state is CONNECTING. rrc_state %d",curr_rrc_state);
            rrcmcm_send_plmn_list_cnf_with_failure(cmd_ptr->cmd.plmn_list_req.network_select_mode,
                                                   cmd_ptr->cmd.plmn_list_req.trans_id
                                                   ,cmd_ptr->cmd.plmn_list_req.service_search
                                                   );    
          }
          else
          {
          /* Buffer the command */
            WRRC_MSG0_HIGH("Buffering PLMN LIST REQ");
            rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr;
            rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_PLMN_LIST_REQ;

            /* Abort the ongoing interrat procedure to process the plmn list req */
            rrcmcm_send_irat_abort_ind(); 
          }
          break; 

        case RRC_MODE_INTERRAT_IN_PROGRESS:
          if(rrccho_is_gtow_pcco_in_progress())
          {
            /*The check is added to avoid the below stuck up scneario
               >GTOW PCCO is in progress, 
               >RRC received EST_REQ, and RRC is processing it
               >RRC received LIST_REQ and mode is updated to Active
               >Connection establishment failure, CHO procedure sends mode change req to INACTIVE
               >MCM ignores the mode change request to Inactive from Active and CHO is stuck up
            */
            WRRC_MSG1_HIGH("Failing PLMN_LIST_REQ in RRC mode %d because GTOW PCCO is in progress",rrc_mode);
            rrcmcm_send_plmn_list_cnf_with_failure(cmd_ptr->cmd.plmn_list_req.network_select_mode,
                                                   cmd_ptr->cmd.plmn_list_req.trans_id
                                                   ,cmd_ptr->cmd.plmn_list_req.service_search
                                                   );    
          }
          else
          {
            /*This can happen when a G->W reselection is in progress and the user has
             *changed mode from Auto->W only.We would not get an activation_rsp 
             *if we succeed.Hence treat this plmn_list_req as an implicit activation_rsp
             */
            WRRC_MSG0_HIGH("Received RRC_PLMN_LIST_REQ in Interrat-in-progress Mode,updating RRC Mode to Active");
            rrcmcm_update_mode(RRC_MODE_ACTIVE);

            /* Forward the service request to CSP */
            rrc_cell_selection_procedure(cmd_ptr);

            /* Init data */
            rrcmcm_init_data(INIT_ALL);
          }
          break;

        /* Otherwise send a confirmation with a failure status */
        default:
          WRRC_MSG1_HIGH("Received RRC_PLMN_LIST_REQ in invalid RRC mode: %d",rrc_mode);
          rrcmcm_send_plmn_list_cnf_with_failure(cmd_ptr->cmd.plmn_list_req.network_select_mode,
                                                 cmd_ptr->cmd.plmn_list_req.trans_id
                                                 ,cmd_ptr->cmd.plmn_list_req.service_search
                                                 );    
          break;
      }
      break;

    case RRC_STOP_WCDMA_MODE_REQ:
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
      #error code not present
#endif
      /* Extract the cause of stopping WCDMA mode of operation */
      stop_cause = cmd_ptr->cmd.stop_wcdma_mode_req.cause;
      WRRC_MSG1_HIGH("Received RRC_STOP_WCDMA_REQ with Cause %d",stop_cause);

      /* Reset the flag which indicates service request or PLMN list request is saved */
      if((rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_SERVICE_REQ) ||
          (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_PLMN_LIST_REQ))
      {
        WRRC_MSG0_HIGH("Ignore any saved service req or PLMN list req when processing STOP_REQ from NAS.");
        rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_NONE;
      }

#ifdef FEATURE_OOSC_USER_ACTION
#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
      rrccsp_check_and_send_uoosc_end_ind();
#endif
#endif
      /* If the stop cause is RRC_WTOG_CC_ORDER,send a stop-cnf to MM
         We would stop only after a a successful CCO Cnf from RR */
      if(stop_cause == RRC_WTOG_CC_ORDER)
      {
        /* Return a Stop WCDMA Confirmation to MM */
        WRRC_MSG1_HIGH("Received RRC_STOP_WCDMA_REQ with Cause RRC_WTOG_CC_ORDER in Initial substate,rrc_mode:%d",rrc_mode);

        /*Raise an assert if CCO is not in progress*/
        if (!rrccho_is_wtog_cco_active())
        {
          ERR_FATAL("WtoG CHO is not in progress but received stop with cause RRC_WTOG_CC_ORDER",0,0,0);
        }
        rrcmcm_send_stop_wcdma_cnf(FALSE); 
      }

      /* Check if RRC is Inactive mode */
      else if (rrc_mode == RRC_MODE_INACTIVE) 
      {
        /* Return a Stop WCDMA Confirmation to MM */
        rrcmcm_send_stop_wcdma_cnf(TRUE);
      }
      else
      {
        due_to_stop_mode = TRUE;

        /* Send a Stop Lower Layer request to RRC/LLC */
        rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);

        /* Since MM has issued this stop commnad,send a cnf to mm after we stop wcdma */
        rrcmcm_send_stop_cnf_to_mm_flag = TRUE;

        rrcmcm_state = RRCMCM_WAIT_STOP_CNF;
      }
      break;

    case RRC_CHANGE_MODE_REQ:
      /* Process the change mode request */
      process_change_mode_req( cmd_ptr->cmd.change_mode_req );
      break;

/*lint -e408*/
/*lint -e740*/
 
/*lint +e740*/
/*lint +e408*/

/*lint -e408*/

#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
      #error code not present
#endif
/*lint +e408*/
    
    case RRC_ACTIVATION_RSP:
      
      WRRC_MSG0_HIGH("Received Activation Response from MM");

#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
      if(rrccsp_is_rrc_waiting_for_act_rsp_to_handle_mdsp_halt())
      {
        if(rrcmcm_act_cause == RRCMCM_INTERRAT_CC_ORDER_GTOW)
        {
          rrccho_handle_mdsp_halt();
        }
        else
        {
          rrccsp_handle_activation_rsp_for_mdsp_halt();
        }
        break;
      }
#endif
      if((rrcmcm_act_cause == RRCMCM_INTERRAT_CELL_RESELECTION) ||
          (rrcmcm_act_cause == RRCMCM_INTERRAT_CELL_REDIRECTION))
      {
        /* Update the RRC mode to Active */
        rrcmcm_update_mode(RRC_MODE_ACTIVE);
  
        /* Forward the command to CSP for processing */
        rrc_cell_selection_procedure(cmd_ptr);
      
        /* Set the last activation cause here */
        if(rrcmcm_act_cause == RRCMCM_INTERRAT_CELL_REDIRECTION)
        {
          rrcmcm_last_act_cause = RRCMCM_ACT_CAUSE_NONE;
        }
        else
        {
          rrcmcm_last_act_cause = RRCMCM_INTERRAT_CELL_RESELECTION;
        }

        /*set the activation cause to none again.  DT or CSP procedures initializes
        the activation cause after sending activation ind to wcdma*/
        rrcmcm_act_cause = RRCMCM_ACT_CAUSE_NONE;

        WRRC_MSG0_HIGH("FWD RRC_ACTIVATION_RSP to CSP");
      }
      else if( rrcmcm_act_cause ==  RRCMCM_INTERRAT_HANDOVER_GTOW )
      {
        /* Update the PLMN Id and ACQ type */
        rrccsp_update_plmn_info_for_gtow_handover(&cmd_ptr->cmd.activation_rsp);
  
        /* Update the RRC mode to Active */
        rrcmcm_update_mode(RRC_MODE_ACTIVE);
  
        /*send sync indication to mm layer to indicate that HO is successful
          and we need to switch the vocoder to WCDMA and continue to WCDMA call*/
        rrciho_send_sync_ind_for_gtow_ho();
        /* Set the last activation cause here */
        rrcmcm_last_act_cause = RRCMCM_INTERRAT_HANDOVER_GTOW;
      }    
      else if( rrcmcm_act_cause == RRCMCM_INTERRAT_CC_ORDER_GTOW)
      {
        /* Set the last activation cause */
        rrcmcm_last_act_cause = RRCMCM_INTERRAT_CC_ORDER_GTOW;
        rrccho_procedure_event_handler(cmd_ptr);
        WRRC_MSG0_HIGH("FWD RRC_ACTIVATION_RSP to CHO");

      }
      else if( rrcmcm_act_cause == RRCMCM_INTERRAT_CC_ORDER_WTOG_FAILED)
      {
        /* Set the last activation cause */
        rrcmcm_last_act_cause = RRCMCM_INTERRAT_CC_ORDER_WTOG_FAILED;
        rrccho_procedure_event_handler(cmd_ptr);
        WRRC_MSG0_HIGH("FWD RRC_ACTIVATION_RSP to CHO");
      }
      
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
     #error code not present
#endif
     /*set the activation cause to none again.  DT or CSP procedures initializes
        the activation cause after sending activation ind to wcdma*/
      rrcmcm_act_cause = RRCMCM_ACT_CAUSE_NONE;
      
      break;



#ifdef FEATURE_LTE_TO_WCDMA
    case RRC_LTE_RRC_CMD:
      {
        switch(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id)
        {
          case WCDMA_RRC_LTE_RESEL_REQ:
            switch(rrc_mode)
            {
              case RRC_MODE_INACTIVE:
                WRRC_MSG1_HIGH("LTOW:WCDMA_RRC_LTE_RESEL_REQ from LTE RRC, Initial Substate,rrc_mode %d",rrc_mode);
                rrc_init_dynamic_ptrs();
                rrc_mode = RRC_MODE_INTERRAT_IN_PROGRESS;
                procedure_id = RRC_PROCEDURE_CSP;
                rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_LTE_IRAT_REQ;
                rrcmcm_cmd_store.rrcmcm_lte_irat_cmd = *cmd_ptr;
                rrcmcm_state = RRCMCM_WAIT_START_CNF;
                rrcmcm_send_start_wcdma_req();
                break;
              default:
                WRRC_MSG0_ERROR("LTOW:Rvd WCDMA_RRC_LTE_RESEL_REQ from LTE RRC in Invalid RRC mode");
                break;
            }
            break;

          case WCDMA_RRC_LTE_ABORT_RESEL_REQ:
            WRRC_MSG1_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_RESEL_REQ from LTE RRC in Initial substate,rrc_mode: %d",rrc_mode);
            switch(rrc_mode)
            {
              case RRC_MODE_INTERRAT_IN_PROGRESS:
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
              #error code not present
#endif
                rrcmcm_send_irat_abort_ind();
                break;

              case RRC_MODE_INACTIVE:
                rrccsp_send_lte_interrat_reselection_abort_rsp();
                break;

              default:
                /* We should not be getting command in any other substate */
                WRRC_MSG1_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_RESEL_REQ  in Invalid mode,rrc_mode: %d",rrc_mode);
                break;
            }
            break;

          case WCDMA_RRC_LTE_PLMN_SRCH_REQ:
            WRRC_MSG1_HIGH("LTOW: WCDMA_RRC_LTE_PLMN_SRCH_REQ from LTE RRC in Initial substate, rrc_mode %d",rrc_mode);
            switch(rrc_mode)
            {
              case RRC_MODE_INACTIVE:
                rrc_init_dynamic_ptrs();
                rrcmcm_cmd_store.rrcmcm_lte_irat_cmd = *cmd_ptr;
                rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_LTE_PLMN_SRCH_REQ ;

#ifdef FEATURE_DUAL_SIM
                rrcmcm_acquire_lock_for_wcdma_start(&rrcmcm_cmd_store.rrcmcm_lte_irat_cmd.cmd.
                  wcdma_rrc_lte_irat_cmd.cmd.wcmd_rrc_lte_plmn_srch_req.timer_val);
#else
                rrcmcm_state = RRCMCM_WAIT_START_CNF;
                rrcmcm_send_start_wcdma_req();
#endif
                break;
              default:
                WRRC_MSG1_ERROR("LTOW: WCDMA_RRC_LTE_PLMN_SRCH_REQ in invalid RRC mode %d",rrc_mode);
                break;
            }
            break;


          case WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ:
            WRRC_MSG1_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ from LTE RRC in Initial Substate, rrc_mode %d",rrc_mode);
            switch(rrc_mode)
            {
             case RRC_MODE_INACTIVE:
               rrccsp_send_lte_interrat_plmn_srch_abort_rsp();
               break;

              case RRC_MODE_INTERRAT_IN_PROGRESS:
                rrc_cell_selection_procedure( cmd_ptr );
                break;

              default:
                WRRC_MSG1_ERROR("LTOW::WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ in an Initial state/Invalid mode %d",rrc_mode);  
                break;
            }
            break;

          case WCDMA_RRC_LTE_REDIR_REQ:
            WRRC_MSG1_HIGH("LTOW: Rvd WCDMA_RRC_LTE_REDIR_REQ from LTE RRC in Initial substate,rrc_mode: %d",rrc_mode);

            switch(rrc_mode)
            {
              case RRC_MODE_INACTIVE:

                /* Start WCDMA mode process */
                rrc_mode = RRC_MODE_INTERRAT_IN_PROGRESS;
                procedure_id = RRC_PROCEDURE_CSP;
                rrc_init_dynamic_ptrs();
                /*Update the substate and mode before posting the req to L1 as L1 will start processing before we updating the substate*/
                rrcmcm_cmd_store.rrcmcm_lte_irat_cmd = *cmd_ptr;
                rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_LTE_IRAT_REQ;

#ifdef FEATURE_DUAL_SIM
                if(rrc_csfb_call_status)
                {
                  rrc_out_of_lock_wait_e_type wrm_lock_status;                
                  rrc_perform_trm_exchange();
                  lock_req_bands.num_bands = 1;
                  lock_req_bands.bands[0].band = rrc_return_default_band_lock_req();
                  rrc_start_lock_wait_timer(RRCTMR_WRM_LOCK_WAIT_TIMER_FOR_INTERRAT_IN_MS,TRUE);
                  wrm_lock_status = rrc_acquire_wrm_lock(TRM_ACCESS_URGENT,RRCTMR_WRM_LOCK_WAIT_TIMER_FOR_INTERRAT_IN_MS, lock_req_bands);
                  rrc_stop_lock_wait_timer();
				  
                  if(wrm_lock_status == RRC_WRM_LOCK_ACQUIRED)
                  {
                    rrcmcm_state = RRCMCM_WAIT_START_CNF;
                    rrcmcm_send_start_wcdma_req();
                  }
                  else if(wrm_lock_status == RRC_ABORT_FROM_RR)
                  {
                    rrccsp_send_lte_interrat_redirection_abort_rsp();
                    rrcmcm_update_mode(RRC_MODE_INACTIVE);
                    rrcmcm_init_data(INIT_ALL);
                    rrc_free_dynamic_ptrs();
                  }
                  else
                  {
                    rrccsp_send_lte_interrat_redirection_failed_rsp();
                    rrcmcm_update_mode(RRC_MODE_INACTIVE);
                    rrcmcm_init_data(INIT_ALL);
                    rrc_free_dynamic_ptrs();
                  }
                }
                else
                {
                  rrcmcm_state = RRCMCM_WAIT_START_CNF;
                  rrcmcm_send_start_wcdma_req();
                }
#else
                rrcmcm_state = RRCMCM_WAIT_START_CNF;
                rrcmcm_send_start_wcdma_req();
#endif

                break;

              default:

                WRRC_MSG0_ERROR("LTOW: Rvd WCDMA_RRC_LTE_REDIR_REQ from LTE RRC in Invalid RRC mode");
                break;
            }
            break;

          case WCDMA_RRC_LTE_ABORT_REDIR_REQ:
            WRRC_MSG1_HIGH("LTOW: Rvd WCDMA_RRC_LTE_ABORT_REDIR_REQ from LTE RRC in Initial substate,rrc_mode: %d",rrc_mode);
      
            switch(rrc_mode)
            {
              case RRC_MODE_INTERRAT_IN_PROGRESS:
                /* Send the abort ind to csp to abort the interrat process */
                rrcmcm_send_irat_abort_ind();
                break;

              default:
                /* Inform CSP to send abort confirmation to rr */
                rrccsp_send_lte_interrat_redirection_abort_rsp();
                /* We should not be getting command in any other substate */
                WRRC_MSG1_ERROR("LTOW: WCDMA_RRC_LTE_ABORT_REDIR_REQ in Invalid mode,rrc_mode: %d",rrc_mode);
                break;
            }
            break;

          case WCDMA_RRC_LTE_GET_CGI_REQ:
            switch(rrc_mode)
            {
              case RRC_MODE_INACTIVE:
                WRRC_MSG1_HIGH("LTOW:WCDMA_RRC_LTE_GET_CGI_REQ from LTE RRC, Initial Substate,rrc_mode %d",rrc_mode);
                rrc_init_dynamic_ptrs();
                rrc_mode = RRC_MODE_INTERRAT_IN_PROGRESS;
                procedure_id = RRC_PROCEDURE_CSP;
                rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_LTE_IRAT_REQ;
                rrcmcm_cmd_store.rrcmcm_lte_irat_cmd = *cmd_ptr;
#ifdef FEATURE_DUAL_SIM
                rrcmcm_acquire_lock_for_wcdma_start(&rrcmcm_cmd_store.rrcmcm_lte_irat_cmd.cmd.
                  wcdma_rrc_lte_irat_cmd.cmd.wcdma_rrc_lte_cgi_req.time_available_for_cgi);
#else
                rrcmcm_state = RRCMCM_WAIT_START_CNF;
                rrcmcm_send_start_wcdma_req();
#endif
                break;
              default:
                WRRC_MSG0_ERROR("LTOW:Rvd WCDMA_RRC_LTE_GET_CGI_REQ from LTE RRC in Invalid RRC mode");
                break;
            }
            break;

          case WCDMA_RRC_LTE_ABORT_CGI_REQ:
            WRRC_MSG1_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_CGI_REQ from LTE RRC in Initial substate,rrc_mode: %d",rrc_mode);
            switch(rrc_mode)
            {
              case RRC_MODE_INTERRAT_IN_PROGRESS:
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
              #error code not present
#endif
                rrcmcm_send_irat_abort_ind();
                break;

              case RRC_MODE_INACTIVE:
                rrccsp_send_lte_interrat_cgi_abort_rsp();
                break;

              default:
                /* We should not be getting command in any other substate */
                WRRC_MSG1_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_CGI_REQ  in Invalid mode,rrc_mode: %d",rrc_mode);
                break;
            }
            break;

          case LTE_RRC_UTRA_SERVING_CELL_INFO_REQ:
            WRRC_MSG1_HIGH("LTOW: LTE_RRC_UTRA_SERVING_CELL_INFO_REQ from LTE RRC in Initial substate,rrc_mode: %d",rrc_mode);
            switch(rrc_mode)
            {
              case RRC_MODE_INACTIVE:
                serving_cell_info.utra_cell_info.rat = SYS_RAT_UMTS_RADIO_ACCESS;
                serving_cell_info.utra_cell_info.is_rat_active = FALSE;
                rrc_send_lte_cmd(&serving_cell_info.msg_hdr, LTE_RRC_UTRA_SERVING_CELL_INFO_RSP, sizeof(lte_rrc_utra_serving_cell_info_rsp_s));
                break;
              case RRC_MODE_ACTIVE:
                rrc_cell_selection_procedure(cmd_ptr);
                break;
              default:
                /* We should not be getting command in any other substate */
                WRRC_MSG1_HIGH("LTOW: LTE_RRC_UTRA_SERVING_CELL_INFO_REQ  in Invalid mode,rrc_mode: %d",rrc_mode);
                break;
            }
            break;

          default :
            MSG_HIGH("LTOW: Invalid LTE RRC cmd %x in MCM initial substate and rrc_mode ",
                 cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id,rrcmcm_state,rrc_mode);
            break;
        }
      }
      break;
#endif

    default:

      /* No other events are normal in this
      substate */
      WRRC_MSG2_HIGH("Ignoring MCM event %x in substate %d",
               cmd_ptr->cmd_hdr.cmd_id, rrcmcm_state);
      break;
   }
  }
  return;
}



/*===========================================================================

FUNCTION   RRCMCM_WAIT_START_CNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the MCM procedure is in the RRCMCM_WAIT_START_CNF substate.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_wait_start_cnf_substate_event_handler(
  rrc_cmd_type *cmd_ptr /* Received command                   */
)
{
  boolean due_to_stop_mode = FALSE;
  
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
  #error code not present
#endif
  /* Switch on the received command. */
  rrc_rr_cmd_e_type rrc_rr_cmd = (rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id;
  if(rrc_rr_cmd == RRC_INTERRAT_PLMN_SRCH_ABORT_REQ)
  {
      switch(rrc_mode)
      {
        case RRC_MODE_INACTIVE:
          WRRC_MSG0_HIGH("Rx:RRC_INTERRAT_PLMN_SRCH_ABORT_REQ in Wait-Start-cnf State/Inactive mode");

        /* Buffer this command and process it once you get the start confirmation */
          rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_RR_INTERRAT_PLMN_ABORT_REQ;
          //rrcmcm_cmd_store.rrcmcm_interrat_reselection_req_cmd = *((rrc_rr_cmd_type *)cmd_ptr); 
          break;
      
        default:
          WRRC_MSG0_HIGH("Rx:RRC_INTERRAT_PLMN_SRCH_ABORT_REQ in Wait-Start-cnf State/Invalid mode");

          /* This should never happen */
          break;
      }
  }
#ifdef FEATURE_INTERRAT_CELL_RESELECTION_GTOW 
  else if(rrc_rr_cmd == RRC_INTERRAT_RESELECTION_ABORT_REQ)
  {
      /* Buffer this command and process it once you get the start confirmation */
      WRRC_MSG0_HIGH("Rx:RRC_INTERRAT_RESELECTION_ABORT_REQ in Wait-Start-cnf State");
      rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_RR_INTERRAT_RESELECTION_ABORT_REQ;
  }
#endif
  else if( rrc_rr_cmd == RRC_INTERRAT_REDIRECT_ABORT_REQ)
  {
    /* Buffer this command and process it once you get the start confirmation */
      WRRC_MSG0_HIGH("Rx:RRC_INTERRAT_REDIRECT_ABORT_REQ in Wait-Start-cnf State");
      rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_RR_INTERRAT_REDIRECT_ABORT_REQ;
  }
  else
  {
   switch( cmd_ptr->cmd_hdr.cmd_id )
   {

/*lint +e408*/
/*lint +e740*/

/*lint -e408*/
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
    #error code not present
#endif
/*lint +e408*/
    /* Mode change request from MM */
    case RRC_MODE_CHANGE_REQ:
      /* Forward the request to CSP */
      rrc_cell_selection_procedure( cmd_ptr );
      break;

    /* Deact req from MM */
    case RRC_DEACT_REQ:
      /* Forward the request to CSP */
      rrc_cell_selection_procedure( cmd_ptr );
      break;

    case RRC_SERVICE_REQ:
      if((rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_INTERRAT_PLMN_SRCH_REQ) &&
         (rrcmcm_cmd_store.rrcmcm_abort_flag != RRCMCM_RR_INTERRAT_PLMN_ABORT_REQ))
      { 
        /* RR is expected to abort the search. This should ideally never happen*/
        ERR_FATAL("Rx:RRC_SERVICE_REQ when doing BPLMN search - This should never happen", 0, 0, 0);
      }
      else
      {
        /* Overwrite the stored RRC_SERVICE_REQ command */
        rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr;
        rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_SERVICE_REQ;
      }
      break;

    /* Mode change indication from MM */
    case RRC_CHANGE_MODE_IND:
      WRRC_MSG1_HIGH("DUAL:No processing is done for the message RRC_CHANGE_MODE_IND in mcm sstate=%d",rrcmcm_state);
      break;

    case RRC_STOP_WCDMA_MODE_REQ:
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
      #error code not present
#endif
      /* Extract the cause of stopping WCDMA mode of operation */
      stop_cause = cmd_ptr->cmd.stop_wcdma_mode_req.cause;

      /* Set Command received flag */
      rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_STOP_WCDMA_REQ;
      break;

    case RRC_PLMN_LIST_REQ:
      if((rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_INTERRAT_PLMN_SRCH_REQ) &&
         (rrcmcm_cmd_store.rrcmcm_abort_flag != RRCMCM_RR_INTERRAT_PLMN_ABORT_REQ))
      { 
        /* RR is expected to abort the search. This should ideally never happen*/
        ERR_FATAL("Rx:RRC_PLMN_LIST_REQ when doing BPLMN search - This should never happen", 0, 0, 0);
      }
      else
      {
        /* Store the command */
        rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr;
        rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_PLMN_LIST_REQ;
      }
      break;

    case RRC_CPHY_START_WCDMA_MODE_CNF:

#ifdef FEATURE_DUAL_SIM
      if((cmd_ptr->cmd.start_cnf.status == FALSE) && (cmd_ptr->cmd.start_cnf.fail_type == WL1_FAILED_NO_WRM_LOCK))
      {
        rrcmcm_handle_start_wcdma_failure_for_dual_sim();
      }
      else
#endif
      // BPLMN search abort request
      if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_PLMN_ABORT_REQ)      
      {
        WRRC_MSG0_HIGH("Processing buffered RRC_INTERRAT_PLMN_ABORT_REQ after start cnf");
        /* stop wcdma */
        rrcmcm_update_mode(RRC_MODE_INTERRAT_IN_PROGRESS);
        stop_cause = RRC_RAT_CHANGE;
        rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);

        /* Change the state to Wait for Stop Confirmation */
        rrcmcm_state = RRCMCM_WAIT_STOP_CNF;    
      }
      
      else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_RESELECTION_ABORT_REQ)      
      {
        WRRC_MSG0_HIGH("Processing buffered RRC_INTERRAT_RESELECTION_ABORT_REQ after start cnf - Sending Stop cmd");

        /* Stop wcdma */
        rrcmcm_update_mode(RRC_MODE_INTERRAT_IN_PROGRESS);
        due_to_stop_mode = TRUE;
        stop_cause = RRC_DEEP_SLEEP;
        rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);

        /* Change the state to Wait for Stop Confirmation */
        rrcmcm_state = RRCMCM_WAIT_STOP_CNF;    
      }

      else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_REDIRECT_ABORT_REQ)
      {
        WRRC_MSG0_HIGH("Processing buffered RRC_INTERRAT_REDIRECT_ABORT_REQ after start cnf - Sending Stop cmd");

        /* Stop wcdma */
        rrcmcm_update_mode(RRC_MODE_INTERRAT_IN_PROGRESS);
        due_to_stop_mode = TRUE;
        stop_cause = RRC_DEEP_SLEEP;
        rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);

        /* Change the state to Wait for Stop Confirmation */
        rrcmcm_state = RRCMCM_WAIT_STOP_CNF;    
      }
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
      #error code not present
#endif

#ifdef FEATURE_LTE_TO_WCDMA
      else if((rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_RESEL_REQ) ||
              (rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_REDIR_REQ) ||
              (rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_CGI_REQ)    
              )
      {
        WRRC_MSG0_HIGH("LTOW: Processing saved LTOW interrat Abort req after start cnf");
        rrcmcm_update_mode(RRC_MODE_INTERRAT_IN_PROGRESS);
        due_to_stop_mode = TRUE;
        stop_cause = RRC_DEEP_SLEEP;
        rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);
        rrcmcm_state = RRCMCM_WAIT_STOP_CNF;    
      }
      else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_PLMN_SRCH_REQ)      
      {
        WRRC_MSG0_HIGH("LTOW: Processing WCDMA_RRC_ABORT_PLMN_SRCH_REQ after start cnf");
        rrcmcm_update_mode(RRC_MODE_INTERRAT_IN_PROGRESS);
        stop_cause = RRC_RAT_CHANGE;
        rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);
        rrcmcm_state = RRCMCM_WAIT_STOP_CNF;    
      }
      else if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_LTE_IRAT_REQ)
      {
        WRRC_MSG0_HIGH("LTOW: Continue processing LTE->WCDMA request after start cnf");
        rrc_cell_selection_procedure(&rrcmcm_cmd_store.rrcmcm_lte_irat_cmd);
        rrcmcm_init_data(INIT_ALL);
        rrctmr_start_timer( RRCTMR_RSSI_BER_TIMER,
                             RRCTMR_RSSI_BER_REPORT_TIME );
      }
      else if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_LTE_PLMN_SRCH_REQ)
      {
        rrcmcm_update_mode(RRC_MODE_INTERRAT_IN_PROGRESS);
        WRRC_MSG0_HIGH("LTOW: Continue processing WCDMA_RRC_LTE_PLMN_SRCH_REQ after start cnf"); 
        rrc_cell_selection_procedure(&rrcmcm_cmd_store.rrcmcm_lte_irat_cmd);
        rrcmcm_init_data(INIT_ALL);
      }
#endif

      // BPLMN search request triggered the wCDMA start 
      else if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_INTERRAT_PLMN_SRCH_REQ)
      {
        WRRC_MSG0_HIGH("Continue processing RRC_INTERRAT_PLMN_REQ after start cnf"); 

        /* Update RRC mode */
        rrcmcm_update_mode(RRC_MODE_INTERRAT_IN_PROGRESS);

        /* Forward inter-RAT PLMN search request to CSP */
        /*lint -e740*/
        rrc_cell_selection_procedure( (rrc_cmd_type *)(&rrcmcm_cmd_store.rrcmcm_interrat_reselection_req_cmd) );        
        /*lint +e740*/

        /* Go back to initial state */
        rrcmcm_init_data(INIT_ALL);
      }

      /*if mode_change_req is received earlier, then process that command and*/
      else if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_MODE_CHANGE)
      {
        /* Update RRC Mode and RAT preference, and
           send the mode change indication if needed */
        rrcmcm_update_mode(RRC_MODE_ACTIVE);

        rrcmcm_process_mode_change_req();
      }      

      /* Check if a RRC_STOP_WCDMA_MODE_REQ is received */
      else if (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_STOP_WCDMA_REQ) 
      {
        /* Update RRC Mode and RAT preference, and
           send the mode change indication if needed */
        rrcmcm_update_mode(RRC_MODE_ACTIVE);

        due_to_stop_mode = TRUE;

        /* Send a stop lower layer request to LLC */
        rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);

        /* Since MM has issued this stop commnad,send a cnf to mm after we stop wcdma */
        rrcmcm_send_stop_cnf_to_mm_flag = TRUE;

    /* Clear RRC change mode requests,if any */
        rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;

        /* Change the state to Wait for Stop Confirmation */
        rrcmcm_state = RRCMCM_WAIT_STOP_CNF;
      }

      else if (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_SERVICE_REQ)
      {
        /* Update RRC Mode and RAT preference (2.0), and
           send the mode change indication if needed */
        rrcmcm_update_mode(RRC_MODE_ACTIVE);

#ifdef FEATURE_RAT_PRIORITY_LIST
        rrcmcm_update_mode_capability_from_service_req(&rrcmcm_cmd_store.rrcmcm_mm_req_cmd.cmd.rrc_service_req.rat_pri_list_info);
#else
        /* Set the dual-mode enable flag */
        rrc_dual_mode_enabled = rrcmcm_cmd_store.rrcmcm_mm_req_cmd.cmd.rrc_service_req.dual_mode;
        WRRC_MSG1_HIGH("Setting rrc_dual_mode_enabled to %d", rrc_dual_mode_enabled);
#endif

        /* Invoke the initial cell selection */
        rrc_cell_selection_procedure((rrc_cmd_type *) &rrcmcm_cmd_store.rrcmcm_mm_req_cmd);

        /* Start the RSSI reporting timer */
        rrctmr_start_timer( RRCTMR_RSSI_BER_TIMER,
                            RRCTMR_RSSI_BER_REPORT_TIME );

        rrcmcm_init_data(INIT_ALL);
      }

      else if (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_PLMN_LIST_REQ)
      {
        /* Update RRC Mode and RAT preference (2.0), and
           send the mode change indication if needed */
        rrcmcm_update_mode(RRC_MODE_ACTIVE);

        /* Invoke the initial cell selection */
        rrc_cell_selection_procedure((rrc_cmd_type *) &rrcmcm_cmd_store.rrcmcm_mm_req_cmd);

        /* Start the RSSI reporting timer */
        rrctmr_start_timer( RRCTMR_RSSI_BER_TIMER,
                            RRCTMR_RSSI_BER_REPORT_TIME );

        rrcmcm_init_data(INIT_ALL);

      }
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
      #error code not present
#endif
      else 
      {
       /* Check if RRC is in INTERRAT_IN_PROGRESS mode */
        if (rrc_mode == RRC_MODE_INTERRAT_IN_PROGRESS) 
        {
          if (procedure_id == RRC_PROCEDURE_CSP) 
          {
            /*lint -e740*/
            /* Forward inter-RAT cell selection request to CSP */
            rrc_cell_selection_procedure( (rrc_cmd_type *)(&rrcmcm_cmd_store.rrcmcm_interrat_reselection_req_cmd) );
            /*lint +e740*/
          }
          else
          {
            /* Send RRC mode change confirmation */
            rrcmcm_send_change_mode_cnf(TRUE);
          }
        }

        /* Check if there is a pending change mode req */
        else if(rrcmcm_cmd_store.rrcmcm_change_mode_req.mode != RRC_MODE_MAX)
        {
          WRRC_MSG2_HIGH("Pending Ch-Mode-Req to mode %d in MCM-Sub-State %d",
                   rrcmcm_cmd_store.rrcmcm_change_mode_req.mode, rrcmcm_state);

          //send change mode req and reset the cmd_store flag
          rrcmcm_send_change_mode_req(rrcmcm_cmd_store.rrcmcm_change_mode_req.mode, 
                                      rrcmcm_cmd_store.rrcmcm_change_mode_req.proc);
          rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;
          rrcmcm_state = RRCMCM_INITIAL;
        }
        else
        { 
          /* Update RRC Mode and RAT preference (2.0), and 
             send the mode change indication if needed */
          rrcmcm_update_mode(RRC_MODE_ACTIVE);

          if (rrcmcm_cmd_store.rrcmcm_cmd_flag != RRCMCM_CMD_PLMN_LIST_REQ)
          {
#ifdef FEATURE_RAT_PRIORITY_LIST
            rrcmcm_update_mode_capability_from_service_req(&rrcmcm_cmd_store.rrcmcm_mm_req_cmd.cmd.rrc_service_req.rat_pri_list_info);
#else
            /* Set the dual-mode enable flag */
            rrc_dual_mode_enabled = rrcmcm_cmd_store.rrcmcm_mm_req_cmd.cmd.rrc_service_req.dual_mode;
#endif
          }

          /* Start the cell selection */
          rrc_cell_selection_procedure( &rrcmcm_cmd_store.rrcmcm_mm_req_cmd );
        }

        /* Start the RSSI reporting timer */
        rrctmr_start_timer( RRCTMR_RSSI_BER_TIMER,
                            RRCTMR_RSSI_BER_REPORT_TIME );
        rrcmcm_init_data(INIT_ALL);
      }   
      break;

    case RRC_CHANGE_MODE_REQ:
      /*we can receive a CHANGE_MODE_REQ at any state. Therefore
        save the cmd_flag to change_mode type, this can over-ride
        another other saved flag as we should be going to STOP as
        soon as we get this from MM, this will override all other
        commands*/
      if(cmd_ptr->cmd.change_mode_req.mode == RRC_MODE_CHANGE)
      {
        rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_MODE_CHANGE;
      }
      else
      {
        /*  in cases when an rrc procedure does a change-mode-req to another substate
            save the change_mode_req_mode and honor it before honoring any other change
            requests when the current change_mode_req is honored
         */
        MSG_HIGH("Buffering CHANGE_MODE_REQ.req_mode %d,req_proc %d,mcm_substate %d",
                 cmd_ptr->cmd.change_mode_req.mode,
                 cmd_ptr->cmd.change_mode_req.procedure,
                 rrcmcm_state);
        rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = cmd_ptr->cmd.change_mode_req.mode;
        rrcmcm_cmd_store.rrcmcm_change_mode_req.proc = cmd_ptr->cmd.change_mode_req.procedure;
      }
      break;

#ifdef FEATURE_LTE_TO_WCDMA
     case RRC_LTE_RRC_CMD:
        switch(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id)
        {
          case WCDMA_RRC_LTE_ABORT_RESEL_REQ:
            WRRC_MSG2_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_RESEL_REQ from LTE RRC in MCM state %d, RRC mode %d",
                rrcmcm_state,rrc_mode);
            rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_LTE_ABORT_RESEL_REQ;
            break;
          case WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ:
            WRRC_MSG2_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ from LTE RRC in MCM state %d, RRC mode %d",
                rrcmcm_state,rrc_mode);
            rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_LTE_ABORT_PLMN_SRCH_REQ;
            break;
          case WCDMA_RRC_LTE_ABORT_REDIR_REQ:
            WRRC_MSG2_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_REDIR_REQ from LTE RRC in MCM state %d, RRC mode %d",
                rrcmcm_state,rrc_mode);
            rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_LTE_ABORT_REDIR_REQ;
            break;
          case WCDMA_RRC_LTE_ABORT_CGI_REQ:
            WRRC_MSG2_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_CGI_REQ from LTE RRC in MCM state %d, RRC mode %d",
                rrcmcm_state,rrc_mode);
            rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_LTE_ABORT_CGI_REQ;
            break;
          default :
            MSG_HIGH("LTOW: Invalid cmd %x from LTE RRC in MCM state %d, RRC mode %d",
                cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id,rrcmcm_state,rrc_mode);
            break;
        }
        break;
#endif

    default:

      /* No other events are normal in this
      substate */
      WRRC_MSG2_ERROR("Ignoring MCM event %x in state %d",
          cmd_ptr->cmd_hdr.cmd_id, rrcmcm_state);
      break;
   }
  }
  return;
}



/*===========================================================================

FUNCTION   RRCMCM_WAIT_STOP_CNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the procedure is in the Wait for Stop Confirmation substate.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_wait_stop_cnf_substate_event_handler(
  rrc_cmd_type *cmd_ptr  /* Received command                   */
)
{

#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
  #error code not present
#endif
  /* Switch on the received command. */
  rrc_rr_cmd_e_type rrc_rr_cmd = (rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id;
  if(rrc_rr_cmd == RRC_INTERRAT_PLMN_SRCH_ABORT_REQ)
  {
      switch(rrc_mode)
      {
        case RRC_MODE_INTERRAT_IN_PROGRESS:

          WRRC_MSG0_HIGH("Rx:RRC_INTERRAT_PLMN_SRCH_ABORT_REQ in Wait-Stop-cnf State/Interrat-in-progress mode");

          /* Forward the abort request to CSP because CSP is already processing a BPLMN search request  */
          rrc_cell_selection_procedure( cmd_ptr );                
          break;
      
        default:

          /* This should never happen */
          WRRC_MSG0_HIGH("Rx:RRC_INTERRAT_PLMN_SRCH_ABORT_REQ in Wait-Stop-cnf State/Invalid mode");
          break;
      }
  }
#ifdef FEATURE_INTERRAT_CELL_RESELECTION_GTOW 
  else if(rrc_rr_cmd == RRC_INTERRAT_RESELECTION_ABORT_REQ)
  {
      /* Abort the ongoing interrat reselection procedure */
      WRRC_MSG0_HIGH("Rx:RRC_INTERRAT_RESELECTION_ABORT_REQ in Wait-Stop-cnf State");
      rrcmcm_send_irat_abort_ind(); 
  }
#endif
  else if(rrc_rr_cmd == RRC_INTERRAT_REDIRECT_ABORT_REQ)
  {
      /* Abort the ongoing interrat reselection procedure */
      WRRC_MSG0_HIGH("Rx:RRC_INTERRAT_REDIRECT_ABORT_REQ in Wait-Stop-cnf State");
      rrcmcm_send_irat_abort_ind(); 
  }
  else
  {
   switch( cmd_ptr->cmd_hdr.cmd_id )
   {
    
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
    #error code not present
#endif
/*lint +e408*/
    /* Mode change request from MM */
    case RRC_MODE_CHANGE_REQ:
      /* Forward the request to CSP */
      rrc_cell_selection_procedure( cmd_ptr );
      break;

    /* Deact req from MM */
    case RRC_DEACT_REQ:
      /* Forward the request to CSP */
      rrc_cell_selection_procedure( cmd_ptr );
      break;

    case RRC_SERVICE_REQ:
    /* If BPLMN search is active and we get a service request,send a cnf with failure
     This should ideally never happen */
      if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_INTERRAT_PLMN_SRCH_REQ)
      { 
        WRRC_MSG0_HIGH("Rx:RRC_SERVICE_REQ when doing BPLMN search - This should never happen");
        rrcmcm_send_service_cnf_with_failure();
      }
      else
      {
        /* Store the RRC_SERVICE_REQ command and set the command received flag */
        rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr;
        rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_SERVICE_REQ;
      }
      break;

     /* Mode change indication from MM */
   case RRC_CHANGE_MODE_IND:
       WRRC_MSG1_HIGH("DUAL:No processing is done for the message RRC_CHANGE_MODE_IND in mcm sstate=%d",rrcmcm_state);  
     break;

    case RRC_PLMN_LIST_REQ:
      /* If BPLMN search is active and we get a PLMN list request,send a cnf with failure
      This should ideally never happen */
      if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_INTERRAT_PLMN_SRCH_REQ)
      { 
        WRRC_MSG0_HIGH("Rx:RRC_PLMN_LIST_REQ when doing BPLMN search - This should never happen");
        rrcmcm_send_plmn_list_cnf_with_failure(cmd_ptr->cmd.plmn_list_req.network_select_mode,
                                               cmd_ptr->cmd.plmn_list_req.trans_id
                                               ,cmd_ptr->cmd.plmn_list_req.service_search
                                               );    
      }
      else
      {
        /* Store the RRC_PLMN_LIST_REQ command and set the command received flag */
        rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr;
        rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_PLMN_LIST_REQ;
      }
      break;

    case RRC_STOP_WCDMA_MODE_REQ:

      /* Reset the flag which indicates service request or PLMN list request is saved */
      if((rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_SERVICE_REQ) ||
          (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_PLMN_LIST_REQ))
      {
        WRRC_MSG0_HIGH("Ignore any service req or PLMN list req saved when processing STOP_REQ from NAS.");
        rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_NONE;
      }

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
      #error code not present
#endif      
      /* Though we are already processing a stop cmd,
         send a cnf to mm after we stop wcdma,cos MM would be expecting it */
      rrcmcm_send_stop_cnf_to_mm_flag = TRUE;

      break;

    case RRC_STOP_LOWER_LAYER_CNF:
#ifdef FEATURE_WRLF_SYSTEM_SEL
      if(!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
      {
        rrc_free_dynamic_ptrs();
      }

      /* process BPLMN abort message if its pending(recd in start-cnf state) */
      if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_PLMN_ABORT_REQ)
      {
        WRRC_MSG0_HIGH("Continue processing RRC_INTERRAT_PLMN_ABORT_REQ after stop cnf");

        /* Inform CSP to cleanup and send abort confirmation to rr */
        rrccsp_send_bplmn_abort_cnf_to_rr();      

        /* Set mode back to inactive*/ 
        rrcmcm_update_mode(RRC_MODE_INACTIVE);

        if((rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_SERVICE_REQ)
          || (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_PLMN_LIST_REQ))
        {
          /* Go back to initial state */
          rrcmcm_init_data(INIT_STATE_VARS_ONLY);
          rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_RR_ABORT_NONE;
          rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;
        }
        else
        {
          /* Go back to initial state */
          rrcmcm_init_data(INIT_ALL);
        }
      }

      /* process reselection abort message if its pending(recd in start-cnf state) */
      else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_RESELECTION_ABORT_REQ)
      {
        WRRC_MSG0_HIGH("Continue processing RRC_INTERRAT_RESELECTION_ABORT_REQ after stop cnf");

        /* Inform CSP to send abort confirmation to rr */
        rrccsp_send_interrat_reselection_abort_cnf();

        /* Set mode back to inactive*/ 
        rrcmcm_update_mode(RRC_MODE_INACTIVE);

        if((rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_SERVICE_REQ)
          || (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_PLMN_LIST_REQ))
        {
          /* Go back to initial state */
          rrcmcm_init_data(INIT_STATE_VARS_ONLY);
          rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_RR_ABORT_NONE;
          rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;
        }
        else
        {
          /* Go back to initial state */
          rrcmcm_init_data(INIT_ALL);
        }
      }

      else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_RR_INTERRAT_REDIRECT_ABORT_REQ)
      {
        WRRC_MSG0_HIGH("Continue processing RRC_INTERRAT_REDIRECT_ABORT_REQ after stop cnf");

        /* Inform CSP to send abort confirmation to rr */
        rrccsp_send_gtow_redirection_abort_cnf();

        /* Set mode back to inactive*/ 
        rrcmcm_update_mode(RRC_MODE_INACTIVE);

        if((rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_SERVICE_REQ)
          || (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_PLMN_LIST_REQ))
        {
          /* Go back to initial state */
          rrcmcm_init_data(INIT_STATE_VARS_ONLY);
          rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_RR_ABORT_NONE;
          rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;
        }
        else
        {
          /* Go back to initial state */
          rrcmcm_init_data(INIT_ALL);
        }
      }
#ifdef FEATURE_LTE_TO_WCDMA
      else if((rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_RESEL_REQ) ||
                (rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_PLMN_SRCH_REQ) ||
                (rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_REDIR_REQ) ||
                (rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_CGI_REQ)
                ) 
      {
        if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_RESEL_REQ)
        {
          WRRC_MSG0_HIGH("LTOW: Continue processing WCDMA_RRC_LTE_ABORT_RESEL_REQ after STOP_CNF");
          rrccsp_send_lte_interrat_reselection_abort_rsp();
        }
        else if(rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_REDIR_REQ)
        {
          WRRC_MSG0_HIGH("LTOW: Continue processing WCDMA_RRC_LTE_ABORT_REDIR_REQ after STOP_CNF");
          rrccsp_send_lte_interrat_redirection_abort_rsp();
        }
        else if (rrcmcm_cmd_store.rrcmcm_abort_flag == RRCMCM_LTE_ABORT_PLMN_SRCH_REQ)
        {
          WRRC_MSG0_HIGH("LTOW: Continue processing WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ after STOP_CNF");
          rrccsp_send_lte_interrat_plmn_srch_abort_rsp();
        }
        else
        {
          WRRC_MSG0_HIGH("LTOW: Continue processing WCDMA_RRC_LTE_ABORT_CGI_REQ after STOP_CNF");
          rrccsp_send_lte_interrat_cgi_abort_rsp();
        }
        rrcmcm_update_mode(RRC_MODE_INACTIVE);
        if((rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_SERVICE_REQ)
          || (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_PLMN_LIST_REQ))
        {
          rrcmcm_init_data(INIT_STATE_VARS_ONLY);
          rrcmcm_cmd_store.rrcmcm_abort_flag = RRCMCM_RR_ABORT_NONE;
          rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;
        }
        else
        {
          rrcmcm_init_data(INIT_ALL);
        }
      }
#endif
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
      #error code not present
#endif
      /*if mode_change_req is received earlier, then process that command and*/
      if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_MODE_CHANGE)
      {
        rrcmcm_update_mode(RRC_MODE_INACTIVE);
        rrcmcm_send_stop_wcdma_cnf(TRUE);
        rrcmcm_process_mode_change_req();
      }

      /* Check if a RRC_SERVICE_REQ or RRC_PLMN_LIST_REQ is received */
      else if (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_SERVICE_REQ ||
               rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_PLMN_LIST_REQ ) 
      {
        /*send mode change cnf first, then mode change ind to csp/ccm*/
        rrcmcm_update_mode(RRC_MODE_INACTIVE);

        /* Send the mode change indication to CSP&CCM. NOTE: In the future,
           if registring method is implemented, the following will be
           removed */
        rrcmcm_send_mode_change_ind(RRC_PROCEDURE_CSP, rrc_mode, 
                                    RRC_MODE_INACTIVE);
        rrcmcm_send_mode_change_ind(RRC_PROCEDURE_CCM, rrc_mode,
                                    RRC_MODE_INACTIVE);
        WRRC_MSG0_HIGH("Sent Mode Change to CSP and CCM");

#ifdef FEATURE_WRLF_SYSTEM_SEL
        if(!RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
        {
          rrc_init_dynamic_ptrs();
        }
        /*Update the substate and mode before posting the req to L1 as L1 will start processing before we updating the substate*/
        rrcmcm_state = RRCMCM_WAIT_START_CNF;
        /* Send a request to restart the WCDMA Mode */
        rrcmcm_send_start_wcdma_req();
      }

      else
      {
#ifdef FEATURE_INTERRAT_CELL_RESELECTION_GTOW
        /* Check for if RRC is in INTERRAT_IN_PROGRESS mode */
        if (rrc_mode == RRC_MODE_INTERRAT_IN_PROGRESS) 
        {
          
          rrcmcm_update_mode(RRC_MODE_INACTIVE);
          rrcmcm_send_change_mode_cnf(TRUE);

          /* Send stop cnf to MM if needed */
          if(rrcmcm_send_stop_cnf_to_mm_flag)
          {
            /* Send a Stop WCDMA Mode Confirmation to MM */
            rrcmcm_send_stop_wcdma_cnf(TRUE);
          }

          /* Send the mode change indication to CSP&CCM. NOTE: In the future,
             if registring method is implemented, the following will be
             removed */
          rrcmcm_send_mode_change_ind(RRC_PROCEDURE_CSP, rrc_mode, 
                                      RRC_MODE_INACTIVE);
          rrcmcm_send_mode_change_ind(RRC_PROCEDURE_CCM, rrc_mode,
                                      RRC_MODE_INACTIVE);
          WRRC_MSG0_HIGH("Sent Mode Change to CSP and CCM");

        }
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
        #error code not present
#endif
        else
        { 
          /* Send the mode change indication to CSP&CCM. NOTE: In the future,
             if registring method is implemented, the following will be
             removed */
          rrcmcm_send_mode_change_ind(RRC_PROCEDURE_CSP, rrc_mode, 
                                      RRC_MODE_INACTIVE);
          rrcmcm_send_mode_change_ind(RRC_PROCEDURE_CCM, rrc_mode,
                                      RRC_MODE_INACTIVE);
          WRRC_MSG0_HIGH("Sent Mode Change to CSP and CCM");

#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
          #error code not present
#endif

          /* Update the RRC mode and RAT preference, and send mode change
             indications if needed */
          rrcmcm_update_mode(RRC_MODE_INACTIVE);

          if(rrcmcm_send_stop_cnf_to_mm_flag)
          {
            /* Send a Stop WCDMA Mode Confirmation to MM */
            rrcmcm_send_stop_wcdma_cnf(TRUE);
          }
        }
#else
        /* Send the mode change indication to CSP&CCM. NOTE: In the future,
           if registring method is implemented, the following will be
           removed */
        rrcmcm_send_mode_change_ind(RRC_PROCEDURE_CSP, rrc_mode,
                                    RRC_MODE_INACTIVE);
        rrcmcm_send_mode_change_ind(RRC_PROCEDURE_CCM, rrc_mode,
                                    RRC_MODE_INACTIVE);
        WRRC_MSG0_HIGH("Sent Mode Change to CSP and CCM");

        /* Update the RRC mode and RAT preference, and send mode change
           indications if needed */
        rrcmcm_update_mode(RRC_MODE_INACTIVE);

        /* Send a Stop WCDMA Mode Confirmation to MM */
        rrcmcm_send_stop_wcdma_cnf(TRUE);
#endif

        /* Stop the RSSI reporting timer */
        rrctmr_stop_timer(RRCTMR_RSSI_BER_TIMER);
  
        rrcmcm_init_data(INIT_ALL);
      }
      break;

    case  RRC_CONTINUE_STOP_MODE_REQ:
       WRRC_MSG0_HIGH("Rvd RRC_CONTINUE_STOP_MODE_REQ ");
       rrcmcm_send_stop_lower_layer_req(TRUE);
      break;
      
    
    case RRC_CHANGE_MODE_REQ:
      /*we can receive a CHANGE_MODE_REQ at any state. Therefore
        save the cmd_flag to change_mode type, this can over-ride
        another other saved flag as we should be going to STOP as
        soon as we get this from MM, this will override all other
        commands*/
      if(cmd_ptr->cmd.change_mode_req.mode == RRC_MODE_CHANGE)
      {
        rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_MODE_CHANGE;
      }
      break;

#ifdef FEATURE_LTE_TO_WCDMA
    case RRC_LTE_RRC_CMD:
      switch(cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id)
      {
        case WCDMA_RRC_LTE_ABORT_RESEL_REQ:
          WRRC_MSG1_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_RESEL_REQ in wt_stop_cnf, rrc_mode %d",rrc_mode);
          rrcmcm_send_irat_abort_ind();
          break;
        case WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ:
          WRRC_MSG1_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ in wt_stop_cnf, rrc_mode %d",rrc_mode);
          if(rrc_mode == RRC_MODE_INTERRAT_IN_PROGRESS)
          {
            rrc_cell_selection_procedure(cmd_ptr);
          }
          else
          {
            WRRC_MSG1_ERROR("LTOW: WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ in wt_stop_cnf, Invalid rrc_mode %d",rrc_mode);
          }
          break;
        case WCDMA_RRC_LTE_ABORT_REDIR_REQ:
          WRRC_MSG1_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_REDIR_REQ in wt_stop_cnf, rrc_mode %d",rrc_mode);
          rrcmcm_send_irat_abort_ind();
          break;
        case WCDMA_RRC_LTE_ABORT_CGI_REQ:
          WRRC_MSG1_HIGH("LTOW: WCDMA_RRC_LTE_ABORT_CGI_REQ in wt_stop_cnf, rrc_mode %d",rrc_mode);
          rrcmcm_send_irat_abort_ind();
          break;
        default:
          WRRC_MSG1_ERROR("LTOW: Invalid LTE RRC cmd %x",cmd_ptr->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id);
          break;
      }
      break;
#endif
    default:

      /* No other events are normal in this
      substate */
      WRRC_MSG2_ERROR("Ignoring MCM event %x in substate %d",
          cmd_ptr->cmd_hdr.cmd_id, rrcmcm_state);
      break;
   }
  }
  return;
}


/*===========================================================================

FUNCTION   RRCMCM_WAIT_STOP_CNF_FOR_MODE_CHANGE_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the procedure is in the Wait for Stop Confirmation for a Mode Change
  substate.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_wait_stop_cnf_for_mode_change_substate_event_handler(
  rrc_cmd_type *cmd_ptr  /* Received command                   */
)
{
  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_STOP_LOWER_LAYER_CNF:
      rrc_free_dynamic_ptrs();

      rrcmcm_send_change_mode_cnf(TRUE);
      rrcmcm_send_mode_change_ind(RRC_PROCEDURE_IHO, 
                                  RRC_MODE_CHANGE,
                                  RRC_MODE_INACTIVE);
      WRRC_MSG0_HIGH("Game Mode: stopped the RRCTMR_RSSI_BER_TIMER");
      rrctmr_stop_timer(RRCTMR_RSSI_BER_TIMER);
      rrcmcm_update_mode(RRC_MODE_INACTIVE);      
      rrcmcm_init_data(INIT_ALL);

      break;

    case  RRC_CONTINUE_STOP_MODE_REQ:
      WRRC_MSG0_HIGH("Rvd RRC_CONTINUE_STOP_MODE_REQ ");
      rrcmcm_send_stop_lower_layer_req(FALSE);
      break;

    default:

      /* No other events are normal in this
      substate */
      WRRC_MSG2_ERROR("Ignoring MCM event %x in substate %d",
          cmd_ptr->cmd_hdr.cmd_id, rrcmcm_state);
      break;
  }
  return;
}


/*===========================================================================

FUNCTION   RRCMCM_WAIT_SUSPEND_CNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the procedure is in the Wait for SUSPEND Confirmation substate.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_wait_suspend_cnf_substate_event_handler(
  rrc_cmd_type *cmd_ptr  /* Received command */
)
{
  boolean due_to_stop_mode = FALSE;
  rrc_state_e_type curr_rrc_state;

  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    /* RRC_LLC_CMD_CNF */
    case RRC_LLC_CMD_CNF:
      
      /* If RRC_STOP_WCDMA_REQ was received act on it */
      if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_STOP_WCDMA_REQ)
      {
        WRRC_MSG0_HIGH("Processing buffered STOP_WCDMA_REQ on suspend_cnf substate");
        due_to_stop_mode = TRUE;
        
        /* Send a stop lower layer request to LLC */
        rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);

        /* Clear RRC change mode requests,if any */
        rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;

        /* Change the state to Wait for Stop Confirmation */
        rrcmcm_state = RRCMCM_WAIT_STOP_CNF;
      }

      /*if mode_change_req is received earlier, then process that command.. we
        are going to STOP WCDMA mode*/
      else if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_MODE_CHANGE)
      {
        rrcmcm_process_mode_change_req();
      } 
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
      #error code not present
#endif
      else if(rrcmcm_cmd_store.rrcmcm_change_mode_req.mode != RRC_MODE_MAX)
      {
        WRRC_MSG2_HIGH("Pending Ch-Mode-Req to mode %d in MCM-Sub-State %d",
                 rrcmcm_cmd_store.rrcmcm_change_mode_req.mode, rrcmcm_state);

        //send change mode req and reset the cmd_store flag
        rrcmcm_send_change_mode_req(rrcmcm_cmd_store.rrcmcm_change_mode_req.mode, 
                                    rrcmcm_cmd_store.rrcmcm_change_mode_req.proc);
        rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;
        rrcmcm_state = RRCMCM_INITIAL;
      }
      else if (cmd_ptr->cmd.rrc_llc_cnf.ll_cnf == RRC_LL_CPHY_SUSPEND_CNF) 
      {
        /* Check for CPHY_SUSPEND_CNF */
        /* Check if suspend operation is successful */
        if (cmd_ptr->cmd.rrc_llc_cnf.cnf.suspend_cnf.status) 
        {
          WRRC_MSG0_HIGH("Rvd CPHY_SUSPEND_WCDMA_MODE_CNF");
  
          /* Process buffered service req,if any */
          if( (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_SERVICE_REQ) ||
              (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_PLMN_LIST_REQ) ||
              (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_MODE_CHANGE_REQ)||
              (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_DEACT_REQ) )
          {
#ifdef FEATURE_TRIPLE_SIM
            /* Make sure the tuneaway type is LTA before releasing lock  */
            WCDMA_INTLOCK();
            if(rrcwrm_release_lock_if_current_tuneaway_is_lta() == TRUE)
            {
#endif
#ifdef FEATURE_DUAL_SIM
              rrc_release_lock();
#endif
#ifdef FEATURE_TRIPLE_SIM
            }
            WCDMA_INTFREE();
#endif
            /* Send an abort indication to the interrat procedure in order to process the 
               Service_Req from MM */
            WRRC_MSG0_HIGH("Sending IRAT_ABORT_IND to process a buffered MM command");
            rrcmcm_send_irat_abort_ind();

            /* Do not init cmd store because we need to process it later */
            rrcmcm_init_data(INIT_STATE_VARS_ONLY);
          }
          else if(rrcmcm_est_req_saved_flag == TRUE)
          {
            /* Send change mode confirmation */
            rrcmcm_send_change_mode_cnf(TRUE);
            
            WRRC_MSG0_HIGH("Init data is not called so as to keep rrcmcm_est_req_saved_flag to TRUE");
            /* Reset MCM state */
            rrcmcm_state = RRCMCM_INITIAL;
          }
          else
          {
            /* Send change mode confirmation */
            rrcmcm_send_change_mode_cnf(TRUE);
            
            /* Reset MCM state */
            rrcmcm_init_data(INIT_ALL);
          }
        }
        else
        {
          /* If suspend failed, error fatal for now */
          ERR_FATAL("WCDMA L1 Suspend failed!",0,0,0);
        }
      }
      else
      {
        WRRC_MSG1_ERROR("Unexpected LLC Command: %x from LLC", cmd_ptr->cmd.rrc_llc_cnf.ll_cnf);
      }
      break;

    case RRC_CHANGE_MODE_IND:
      WRRC_MSG0_HIGH("DUAL:RRC_CHANGE_MODE_IND is not handled currently in RRC MCM WAIT SUSPEND CNF Handler");      
      break;  

    case RRC_CHANGE_MODE_REQ:
      /*we can receive a CHANGE_MODE_REQ at any state. Therefore
        save the cmd_flag to change_mode type, this can over-ride
        another other saved flag as we should be going to STOP as
        soon as we get this from MM, this will override all other
        commands*/
      if(cmd_ptr->cmd.change_mode_req.mode == RRC_MODE_CHANGE)
      {
        rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_MODE_CHANGE;
      }
      else
      {
        /*  in cases when an rrc procedure does a change-mode-req to another substate
            save the change_mode_req_mode and honor it before honoring any other change
            requests when the current change_mode_req is honored
         */
        MSG_HIGH("Buffering CHANGE_MODE_REQ.req_mode %d,req_proc %d,mcm_substate %d",
                 cmd_ptr->cmd.change_mode_req.mode,
                 cmd_ptr->cmd.change_mode_req.procedure,
                 rrcmcm_state);
        rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = cmd_ptr->cmd.change_mode_req.mode;
        rrcmcm_cmd_store.rrcmcm_change_mode_req.proc = cmd_ptr->cmd.change_mode_req.procedure;
      }
      break;

    /* MM commands that need to be honoured in wait_suspend_cnf substate*/
    case RRC_SERVICE_REQ:
      /* Store the command and act on it after we resume WCDMA */
      WRRC_MSG0_HIGH("Received RRC_SERVICE_REQ in wait_suspend_cnf substate,buffering cmd");
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
      #error code not present
#endif
      rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr; 
      rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_SERVICE_REQ;
      break;
  
  case RRC_PLMN_LIST_REQ:

      WRRC_MSG0_HIGH("Received RRC_PLMN_LIST_REQ in suspend_cnf substate");

      curr_rrc_state = rrc_get_state();

      /* Do not honor PLMN_LIST_REQ in DCH/FACH states */
      if(((curr_rrc_state == RRC_STATE_CELL_DCH) || (curr_rrc_state == RRC_STATE_CELL_FACH)) && 
         !(rrcrce_is_abort_in_progress())&&
         !(rrciho_return_iho_abort()==1) /*Abort for WtoX IRAT procedure is not Active*/
          /* As per current NAS behaviour, Abort will be sent when MPLMN search is triggered while PS data call is active.
             Hence the above abort in progress checks were added to handle MPLMN search instead of ignoring.
             The same may not be required when NAS doesnt send abort.
          */
        )
      {
        WRRC_MSG1_HIGH("Failing PLMN_LIST_REQ bcos rrc state is DCH/FACH. rrc_state %d",curr_rrc_state);
        rrcmcm_send_plmn_list_cnf_with_failure(cmd_ptr->cmd.plmn_list_req.network_select_mode,
                                               cmd_ptr->cmd.plmn_list_req.trans_id
                                               ,cmd_ptr->cmd.plmn_list_req.service_search
                                               );
      }
      /* Also fail the PLMN_LIST_REQ if UE is in PCH states and an OOS is in progress */
      else if( ((curr_rrc_state == RRC_STATE_CELL_PCH) || (curr_rrc_state == RRC_STATE_URA_PCH)) &&
               (RRC_CSP_OUT_OF_SERVICE_AREA_SCAN == rrccsp_get_curr_scan()) )
      {
        WRRC_MSG1_HIGH("Failing PLMN_LIST_REQ in PCH state with OOS in progress: rrc_state %d",curr_rrc_state);
        rrcmcm_send_plmn_list_cnf_with_failure(cmd_ptr->cmd.plmn_list_req.network_select_mode,
                                               cmd_ptr->cmd.plmn_list_req.trans_id
                                               ,cmd_ptr->cmd.plmn_list_req.service_search
                                               );
      }
      /* Check if the PLMN Req is Automatic.If Yes,then give priority to the ongoing Interrat procedure
      and send a plmn cnf with failure to NAS.Make sure to populate the fields correctly */
      else if((cmd_ptr->cmd.plmn_list_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
#ifdef FEATURE_FEMTO_CSG
      /*Abort ASF if in progress upon auto PLMN request */
             &&(!rrccsp_asf_actively_in_prog())
#endif
             )
      {
        WRRC_MSG0_HIGH("RRC_PLMN_LIST_REQ(Automatic mode) in wait_suspend_cnf substate.Sending failure immediately");
        rrcmcm_send_plmn_list_cnf_with_failure(cmd_ptr->cmd.plmn_list_req.network_select_mode,
                                               cmd_ptr->cmd.plmn_list_req.trans_id
                                               ,cmd_ptr->cmd.plmn_list_req.service_search
                                               );    
      }
      else if((curr_rrc_state == RRC_STATE_CONNECTING) &&( rrc_csp_int_data.curr_scan == RRC_CSP_INTER_RAT_REDIRECTION_SCAN))
      {
        WRRC_MSG1_HIGH("Failing PLMN_LIST_REQ bcos rrc state is CONNECTING. rrc_state %d",curr_rrc_state);
        rrcmcm_send_plmn_list_cnf_with_failure(cmd_ptr->cmd.plmn_list_req.network_select_mode,
                                               cmd_ptr->cmd.plmn_list_req.trans_id
                                               ,cmd_ptr->cmd.plmn_list_req.service_search
                                               );
      }
      else
      {
        /* Store the command and act on it after we resume WCDMA */
        WRRC_MSG0_HIGH("Received RRC_PLMN_LIST_REQ in wait_suspend_cnf substate,buffering cmd");
        rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr; 
        rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_PLMN_LIST_REQ;
      }
      break;

    case RRC_MODE_CHANGE_REQ:
      /* Store the command and act on it after we resume WCDMA */
      WRRC_MSG0_HIGH("Received RRC_MODE_CHANGE_REQ in wait_suspend_cnf substate,buffering cmd");
      rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr; 
      rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_MODE_CHANGE_REQ;
      break;

    case RRC_DEACT_REQ:
      /* Store the command and act on it after we resume WCDMA */
      WRRC_MSG0_HIGH("Received RRC_DEACT_REQ in wait_suspend_cnf substate,buffering cmd");
      rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr; 
      rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_DEACT_REQ;
      break;

    case RRC_STOP_WCDMA_MODE_REQ:
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
      #error code not present
#endif
       /* This would happen only when user changes preference from Auto->G only 
       * Store the command and act on it after we get the suspend_cnf 
       */
      WRRC_MSG0_HIGH("Received RRC_STOP_WCDMA_MODE_REQ in wait_suspend_cnf substate,buffering cmd"); 
      rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr; 
      rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_STOP_WCDMA_REQ;

      /* Set the stop cause */
      stop_cause = cmd_ptr->cmd.stop_wcdma_mode_req.cause;
      
      /* Since MM has issued this stop commnad,send a cnf to mm after we stop wcdma */
      rrcmcm_send_stop_cnf_to_mm_flag = TRUE;

      break;
/*lint -e408*/
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
    #error code not present
#endif
/*lint +e408*/
    default:
      /* Unexpected command */
      WRRC_MSG2_ERROR("Unexpected CMD: %x in State: %d", cmd_ptr->cmd_hdr.cmd_id, rrcmcm_state);
      break;
  }
  return;
} /* end of rrcmcm_wait_suspend_cnf_substate_event_handler */


/*===========================================================================

FUNCTION   RRCMCM_WAIT_RESUME_CNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the procedure is in the Wait for RESUME Confirmation substate.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcmcm_wait_resume_cnf_substate_event_handler(
  rrc_cmd_type *cmd_ptr  /* Received command */
)
{
  mm_cmd_type *mm_cmd_ptr;                    /* Pointer to the MM Command */
  boolean due_to_stop_mode = FALSE;
  
  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  { 
  case RRC_LLC_CMD_CNF:
    /*if mode_change_req is received earlier, then process that command and*/
    if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_MODE_CHANGE)
    {
      rrcmcm_process_mode_change_req();
    }  
    
    /* Check if a RRC_STOP_WCDMA_MODE_REQ is received */
    else if (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_STOP_WCDMA_REQ) 
    {
    /* Update RRC Mode and RAT preference, and
      send the mode change indication if needed */
      rrcmcm_update_mode(RRC_MODE_ACTIVE);
      
      WRRC_MSG0_HIGH("Processing buffered STOP_WCDMA_REQ on suspend_cnf substate");
      
      due_to_stop_mode = TRUE;
      
      /* Send a stop lower layer request to LLC */
      rrcmcm_send_stop_lower_layer_req(due_to_stop_mode);
      
      /* Since MM has issued this stop commnad,send a cnf to mm after we stop wcdma */
      rrcmcm_send_stop_cnf_to_mm_flag = TRUE;
      
      /* Clear RRC change mode requests,if any */
      rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;
      
      /* Change the state to Wait for Stop Confirmation */
      rrcmcm_state = RRCMCM_WAIT_STOP_CNF;
    }
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
    #error code not present
#endif

    else if(rrcmcm_cmd_store.rrcmcm_change_mode_req.mode != RRC_MODE_MAX)
    {
      WRRC_MSG2_HIGH("Pending Ch-Mode-Req to mode %d in MCM-Sub-State %d",
        rrcmcm_cmd_store.rrcmcm_change_mode_req.mode, rrcmcm_state);
      
      //send change mode req and reset the cmd_store flag
      rrcmcm_send_change_mode_req(rrcmcm_cmd_store.rrcmcm_change_mode_req.mode, 
        rrcmcm_cmd_store.rrcmcm_change_mode_req.proc);
      rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;
      rrcmcm_state = RRCMCM_INITIAL;
    }
    /* Check for CPHY_RESUME_CNF */
    else if (cmd_ptr->cmd.rrc_llc_cnf.ll_cnf == RRC_LL_CPHY_RESUME_CNF) 
    {
      WRRC_MSG1_HIGH("CPHY_RESUME_WCDMA_MODE_CNF: %d", cmd_ptr->cmd.rrc_llc_cnf.cnf.resume_cnf.status);
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
      #error code not present
#endif
      {
        /* Change the RRC mode */
        rrcmcm_update_mode(RRC_MODE_ACTIVE);
      }
      /* Check the resume status */
      if (cmd_ptr->cmd.rrc_llc_cnf.cnf.resume_cnf.status == FALSE)
      {
        WRRC_MSG0_HIGH("WCDMA resume failed");
#ifdef FEATURE_DUAL_SIM
        WRRC_MSG1_HIGH("WCDMA resume failed with cause %d", cmd_ptr->cmd.rrc_llc_cnf.cnf.resume_cnf.fail_type);

        /* Update TA duration to invalid value. L1 triggers a subsequent OOS post self recovery by RRC */
        if((rrccsp_get_curr_scan() == RRC_CSP_UNLOCK_BY_SCAN) && ((rrc_get_state() == RRC_STATE_CELL_DCH) || (rrc_get_state() == RRC_STATE_CELL_FACH)))
        {
          wl1_fill_tuneaway_duration(0xFFFF);
          rrc_send_rlc_ta_status(RLC_LTA_STOP_IND, RLC_FAIL_IND);
        }

        if(cmd_ptr->cmd.rrc_llc_cnf.cnf.resume_cnf.fail_type == WL1_FAILED_NO_WRM_LOCK)
        {
          WRRC_MSG0_ERROR("DSIM: WCDMA resume failed as L1 failed to acquire lock.");
        }
        rrcmcm_wl1_failure_cause = cmd_ptr->cmd.rrc_llc_cnf.cnf.resume_cnf.fail_type;
#endif
        
        /* If it is an inter-RAT cell reselection, send a command to CSP */
        if (procedure_id == RRC_PROCEDURE_CSP)
        {
        /* reset the est_req_saved_flag to false. We don't want RCE procedure to start
        PCCPCH setup, right after a RESUME_CNF_FAILURE and even before CSP procedure 
          has had time to select a new cell.  Reset this flag and send ABORT_IND to MM */
          if(rrcmcm_est_req_saved_flag == TRUE)
          {
            if(rrccsp_get_curr_scan() == RRC_CSP_INTER_RAT_REDIRECTION_SCAN)
            {
              WRRC_MSG0_HIGH("WTOL:WTOG: Process the buffered EST_REQ.");
              rrcmcm_process_est_req(RRC_MODE_ACTIVE);
            }
            else
            {
              rrcmcm_est_req_saved_flag = FALSE;

              /* Allocate memory for RRC_ABORT_IND command to MM.. */
              mm_cmd_ptr = mm_rrc_get_cmd_buf(RRC_ABORT_IND);

                /* Fill in parameters for RRC_ABORT_IND command */
                mm_cmd_ptr->cmd.rrc_abort_ind.abort_cause = RRC_INTERNAL_ERROR;
                
                /* Update the Max HFNs values to be stored in SIM or USIM */ 
                (void) rrcsmc_append_hfns_to_rel_ind(mm_cmd_ptr->cmd.rrc_abort_ind.hfn_cs,
                  mm_cmd_ptr->cmd.rrc_abort_cnf.hfn_ps);
                
#ifdef FEATURE_DUAL_SIM
                mm_cmd_ptr->cmd.rrc_abort_ind.as_id = rrc_get_as_id();
#endif

                WRRC_MSG1_ERROR("Sent RRC_ABORT_IND to MM with cause: %d / Dispatching out MM cmd", RRC_CONNECTION_NOT_AVAILABLE);

                rrc_send_rrc_message_to_mm(mm_cmd_ptr);               /* Sends the command to MM */
#ifdef FEATURE_QSH_EVENT_METRIC
                rrcrce_qsh_log_connection_est_failure();
#endif
              }
          }

          /* Send change mode confirmation */
          rrcmcm_send_change_mode_cnf(FALSE);
          
        }
        else if((procedure_id == RRC_PROCEDURE_IHO) || (procedure_id == RRC_PROCEDURE_CHO))
        {
          /* Send a change mode cnf with failure */
          rrcmcm_send_change_mode_cnf(FALSE);
        }
        else
        {
          /* Currently W can be suspended only by CSP, IHO, and CHO procedures */
          WRRC_MSG1_ERROR("ERR: W resume for proc rrc_proc_e_type_value%d other than CSP, IHO, CHO", procedure_id);
          
#ifdef FEATURE_RRC_L1_ERROR_RECOVERY_P1
          WRRC_MSG0_HIGH("Error recovery triggered. Take UE to Idle");
          /* Send a change mode cnf with failure for now. Candidate for error fatal */
          rrcmcm_send_change_mode_cnf(FALSE);
#else
          ERR_FATAL("Send resume cnf with failure to the calling proc rrc_proc_e_type_value%d", procedure_id, 0, 0);
#endif
        } 
      }
      /* Resume is successful */
      else
      {
        /* the resume operation is successful */
        /* Send change mode confirmation */
        rrcmcm_send_change_mode_cnf(TRUE);
      }
      
      /* Process any pending request that you received in standby mode */
      if((rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_SERVICE_REQ) ||
        (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_PLMN_LIST_REQ) ||
        (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_MODE_CHANGE_REQ) ||
        (rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_DEACT_REQ)  )
      {
        /* If the buffered cmd is service req, update the dual mode flag before 
           forwarding cmd to CSP */
        if(rrcmcm_cmd_store.rrcmcm_cmd_flag == RRCMCM_CMD_SERVICE_REQ)
        {
#ifdef FEATURE_RAT_PRIORITY_LIST
          rrcmcm_update_mode_capability_from_service_req(&rrcmcm_cmd_store.rrcmcm_mm_req_cmd.cmd.rrc_service_req.rat_pri_list_info);
#else
          rrc_dual_mode_enabled = rrcmcm_cmd_store.rrcmcm_mm_req_cmd.cmd.rrc_service_req.dual_mode;
          WRRC_MSG1_HIGH("Setting rrc_dual_mode_enabled to %d", rrc_dual_mode_enabled);
#endif
        }

        WRRC_MSG1_HIGH("Forwarding buffered MM request %d to CSP",
          rrcmcm_cmd_store.rrcmcm_cmd_flag);
        
        /* Invoke the initial cell selection */
        rrc_cell_selection_procedure((rrc_cmd_type *) &rrcmcm_cmd_store.rrcmcm_mm_req_cmd) ;
      }
      
      rrcmcm_init_data(INIT_ALL);
    }
    else
    {
      WRRC_MSG1_HIGH("Unexpected LLC Command: %x from LLC", cmd_ptr->cmd.rrc_llc_cnf.ll_cnf);
    }
    break;  
    
    /* Mode change request from MM */
    case RRC_MODE_CHANGE_REQ:
      /* Forward the request to CSP */
      rrc_cell_selection_procedure( cmd_ptr );
      break;
      
      /* Deact req from MM */
    case RRC_DEACT_REQ:
      /* Forward the request to CSP */
      rrc_cell_selection_procedure( cmd_ptr );
      break;
      
      /* RRC_SERVICE_REQ from MM */
    case RRC_SERVICE_REQ:
      /* Store the command and act on it after we resume WCDMA */
      WRRC_MSG0_HIGH("Received RRC_SERVICE_REQ in wait_resume_cnf substate,buffering cmd");
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
      #error code not present
#endif
      rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr; 
      rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_SERVICE_REQ;
      break;
      /* Mode change indication from MM */  
    case RRC_CHANGE_MODE_IND:
      WRRC_MSG0_HIGH("DUAL:RRC_CHANGE_MODE_IND is not handled currently in RRC MCM WAIT RESUME CNF Handler");      
      break;    
      
      /* RRC_PLMN_LIST_REQ */
    case RRC_PLMN_LIST_REQ:
      /* Store the command and act on it after we resume WCDMA */
      WRRC_MSG0_HIGH("Received RRC_PLMN_LIST_REQ in wait_resume_cnf substate,buffering cmd");
      rrcmcm_cmd_store.rrcmcm_mm_req_cmd = *cmd_ptr; 
      rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_PLMN_LIST_REQ;
      break;
      
    case RRC_CHANGE_MODE_REQ:
    /*we can receive a CHANGE_MODE_REQ at any state. Therefore
    save the cmd_flag to change_mode type, this can over-ride
    another other saved flag as we should be going to STOP as
    soon as we get this from MM, this will override all other
      commands*/
      if(cmd_ptr->cmd.change_mode_req.mode == RRC_MODE_CHANGE)
      {
        rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_MODE_CHANGE;
      }
      else
      {
      /*  in cases when an rrc procedure does a change-mode-req to another substate
      save the change_mode_req_mode and honor it before honoring any other change
      requests when the current change_mode_req is honored
        */
        MSG_HIGH("Buffering CHANGE_MODE_REQ.req_mode %d,req_proc %d,mcm_substate %d",
          cmd_ptr->cmd.change_mode_req.mode,
          cmd_ptr->cmd.change_mode_req.procedure,
          rrcmcm_state);
        rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = cmd_ptr->cmd.change_mode_req.mode;
        rrcmcm_cmd_store.rrcmcm_change_mode_req.proc = cmd_ptr->cmd.change_mode_req.procedure;
      }
      break;
      
    case RRC_STOP_WCDMA_MODE_REQ:
      /* Extract the cause of stopping WCDMA mode of operation */
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
      #error code not present
#endif
      stop_cause = cmd_ptr->cmd.stop_wcdma_mode_req.cause;
      
      /* Set Command received flag */
      rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_STOP_WCDMA_REQ;
      break;
/*lint -e408*/
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
    #error code not present
#endif
/*lint +e408*/
    default:
      /* Unexpected command */
      WRRC_MSG2_ERROR("Unexpected CMD: %x in State: %d", cmd_ptr->cmd_hdr.cmd_id, rrcmcm_state);
      break;
  }
  return;
} /* end of rrcmcm_wait_resume_cnf_substate_event_handler */


/*===========================================================================

FUNCTION RRCMCM_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Mode Control module. This function is called by the
  dispatcher whenever an event is received for this module.  
  
DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
  
===========================================================================*/
void rrcmcm_event_handler(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  WRRC_MSG2_HIGH("function rrcmcm_event_handler is called in rrcmcm_state = rrcmcm_state_e_type_value%d / Event is received by MCM: %x  ",rrcmcm_state, cmd_ptr->cmd_hdr.cmd_id);
  rrc_crash_debug_log_proc_substates(RRC_PROCEDURE_MCM,rrcmcm_state,cmd_ptr);

  /* Check the procedure substate and procedure the event accordingly */  
  switch( rrcmcm_state )
  {
    case RRCMCM_INITIAL:
      /* Call the event handler for Initial substate */
      rrcmcm_initial_substate_event_handler( cmd_ptr );
      break;

    case RRCMCM_WAIT_START_CNF:
      /* Call the event handler for waiting for start cnf substate */
      rrcmcm_wait_start_cnf_substate_event_handler( cmd_ptr );
      break;
  
    case RRCMCM_WAIT_STOP_CNF:
      /* Call the event handler for wait for stop cnf substate */
      rrcmcm_wait_stop_cnf_substate_event_handler( cmd_ptr );
      break;

    case RRCMCM_WAIT_SUSPEND_CNF:
      /* Call the event handler for wait for suspend substate */
      rrcmcm_wait_suspend_cnf_substate_event_handler( cmd_ptr );
      break;

    case RRCMCM_WAIT_RESUME_CNF:
      /* Call the event handler for wait for resume substate */
      rrcmcm_wait_resume_cnf_substate_event_handler( cmd_ptr );
      break;

    case RRCMCM_WAIT_STOP_CNF_FOR_MODE_CHANGE:
      /* Call the event handler for wait for stop cnf (Mode change) substate */
      rrcmcm_wait_stop_cnf_for_mode_change_substate_event_handler( cmd_ptr );
      break;

    default:
      /* Invalid substate - should never happen. Clear
         all procedure vars */
      WRRC_MSG1_ERROR("Invalid MSM Substate %d", rrcmcm_state);
      rrcmcm_state = RRCMCM_INITIAL;
      break;
  }/* end switch */
  return;
}


/*===========================================================================

FUNCTION  RRCMCM_UPDATE_WCDMA_ACTIVATION_CAUSE

DESCRIPTION

  This function updates the cause of wcdma activation.  Activation could be
  due to either cell reselection or due to GtoW Handover
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_update_wcdma_activation_cause( rrcmcm_activation_cause act_cause )
{
  /*update the activation cause*/
  rrcmcm_act_cause = act_cause;

}/*rrcmcm_update_wcdma_activation_cause*/
/*===========================================================================

FUNCTION  rrcmcm_update_dual_mode_enabled_status

DESCRIPTION

  This function updates the status of the flag whether if dual mode is
  enabled or not.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_update_dual_mode_enabled_status( boolean status )
{
  
  /*update the dual_mode_enabled status*/
  rrc_dual_mode_enabled = status;

}/*rrcmcm_update_dual_mode_enabled_status*/

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif



/*===========================================================================
FUNCTION rrcmcm_get_rrc_est_cause

DESCRIPTION
  Returns establishment cause

DEPENDENCIES
  None

RETURN VALUE
  Returns establishment cause

SIDE EFFECTS
  None
===========================================================================*/
rrc_establish_cause_e_type rrcmcm_get_rrc_est_cause
(
  void                  
)
{
  if(rrcmcm_est_req_saved_flag)
  {
    WRRC_MSG1_HIGH("Est Cause is %d", rrcmcm_est_req.cause); 
    return rrcmcm_est_req.cause;
  }
  else
  {
    return RRC_EST_TERM_UNKNOWN;
  }
} /* rrcmcm_get_rrc_est_cause */

/*===========================================================================
FUNCTION rrc_reset_mcm_irat_resel_act_cause

DESCRIPTION
  Used by NAS to reset activation cause in case registration is 
  not going to be triggered by NAS immediately after IRAT reselection 
  due to ISR being active at NAS. NAS might call this after every service indication

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrc_reset_mcm_irat_resel_act_cause
(
  void
)
{
  if(RRCMCM_INTERRAT_CELL_RESELECTION == rrcmcm_last_act_cause)
  {
    rrcmcm_last_act_cause = RRCMCM_ACT_CAUSE_NONE;
  }
}/* rrc_reset_mcm_irat_resel_act_cause */

/*===========================================================================
FUNCTION rrcmcm_reset_last_act_cause

DESCRIPTION
  Resets activation cause

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcmcm_reset_last_act_cause
(
  void
)
{
  rrcmcm_last_act_cause = RRCMCM_ACT_CAUSE_NONE;
}

/*===========================================================================
FUNCTION rrcmcm_send_dual_mode_info_to_meas_ueci

DESCRIPTION
  Returns None

DEPENDENCIES
  None

RETURN VALUE
  Returns establishment cause

SIDE EFFECTS
  None
===========================================================================*/
void rrcmcm_send_dual_mode_info_to_meas_ueci
(
 rrc_cmd_type *cmd_ptr                  
)
{
  rrc_cmd_type *cmd_ptr_buff;
  
  /* Get a RRC internal command buffer */
  cmd_ptr_buff = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr_buff->cmd_hdr.cmd_id = RRC_MODE_STATUS_IND;  

  /* Put the command in the internal queue */
  rrc_put_int_cmd(cmd_ptr_buff );
  WRRC_MSG0_HIGH("DUAL:RRC_MODE_STATUS_IND is sent to dispatcher ");
} /* rrcmcm_get_rrc_est_cause */

/*===========================================================================

FUNCTION  RRCMCM_INIT_PROCEDURE

DESCRIPTION

  This function initializes the Mode Contro module. This function should be
  only called once at power-on time, and it has to be the first module to
  be initialized.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_init_procedure( void )
{
  /* Initialize data for the module */
  rrc_mode = RRC_MODE_INACTIVE;
  rrc_dual_mode_enabled = TRUE;
  rrcmcm_notification_counter = 0;
  rrcmcm_state = RRCMCM_INITIAL;
  rrcmcm_cmd_store.rrcmcm_cmd_flag = RRCMCM_CMD_NONE;
  rrcmcm_cmd_store.rrcmcm_change_mode_req.mode = RRC_MODE_MAX;
  rrcmcm_act_cause = RRCMCM_ACT_CAUSE_NONE;
  rrcmcm_last_act_cause = RRCMCM_ACT_CAUSE_NONE;
  rrcmcm_est_req_saved_flag=FALSE;

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
   #error code not present
#endif

  return;
}

#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
#error code not present
#endif

/*===========================================================================

FUNCTION  RRC_SEND_RLC_TA_STATUS

DESCRIPTION

  This function sends signal 'RLC_UL_TA_IND' to RLC to indicate the START/STOP status of LTA/QTA 

DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_send_rlc_ta_status(rlc_ta_ind_e_type tune_away_ind, rlc_ta_success_e_type tune_away_succ)
{
  wcdma_l2_ul_cmd_type  *l2_ul_cmd_ptr;
  l2_ul_cmd_ptr = wcdma_l2_ul_get_cmd_buf();

  /* Fill in the Command Header */
  l2_ul_cmd_ptr->cmd_hdr.cmd_id = RLC_UL_TA_IND;
  l2_ul_cmd_ptr->cmd_data.tune_away_info.tune_away_ind = tune_away_ind;
  l2_ul_cmd_ptr->cmd_data.tune_away_info.tune_away_succ = tune_away_succ;
  WRRC_MSG2_HIGH("Sending RLC_UL_TA_IND  TA-Type:%d(0-LtaStart,1-LtaStop,2-QtaStart,3-QtaStop), TA-Status:%d(0-success,1-fail)",tune_away_ind,tune_away_succ);
  rrc_send_l2_ul_cmd(l2_ul_cmd_ptr);
}


