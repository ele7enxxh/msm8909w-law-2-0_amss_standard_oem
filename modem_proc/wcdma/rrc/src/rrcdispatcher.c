/*===========================================================================
                        R R C  D I S P A T C H E R 

DESCRIPTION

   This module receives and dispatches all external and
   internal commands to the other RRC modules. The RRC task
   has a central wait loop and that wait is performed in the
   dispatcher module. When there are no commands waiting for
   the RRC task, execution control is returned to the os.
   When one or more commands are posted for the RRC task and
   the RRC task is runnable, execution control is returned
   to the dispatcher, and it dispatches the pending commands to
   the appropriate procedures.
   
   rrc_dispatcher() is the main function that waits for
   RRC signals to be set. When a signal is set, the function
   sequentially checks the possible RRC signals and takes
   the appropriate action.
   
   If any of the command signals are set, the dispatcher
   dequeues items from the command queue that has it's signal
   set and dispatches the items to the appropriate functions
   until the command queue is empty.   
   
INITIALIZATION AND SEQUENCING REQUIREMENTS
  
   RRC Task must have been started. All RRC command queues must
   have been initialized. All RRC signals must have been cleared
   at task startup.

Copyright (c) 2000-2009 Qualcomm Technologies, Inc. 
All Rights Reserved.QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcdispatcher.c#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   sp      Made changes to add call end metrics - FR#30856
06/06/16   sp      Made changes for Mav debug snapshot
01/12/16   sn      Changes for FR30881
10/13/15   as      Added code to support LTA durting activation time
10/07/15   vi      Changes for force DRX and skip idlemode measurements based on QMI request 
09/11/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
01/29/15   bc      Changes to crash the UE if there is no response to STATE_CHANGE_IND from LLC 
                   for more than 5 seconds.
01/06/14   bc      Made changes to support W2G CCO Abort mechanism.
01/20/15   bc      Changes to crash the UE if there is no response to ACTIVATION_IND 
                   from NAS during WTOG CCO.
12/12/14   sg      Made changes to start T302 after CMAC STATUS IND for CU
12/10/14   bc      Changes to use access correct structures while processing a command.
12/03/14   sp      Made changes to save one byte status in cmd_ptr to crash debug global
11/14/14   sr      Code reduction to remove GET_ALL_SIBS handling
11/10/14   sp      Made changes to save procedure substates in crash debug info
09/10/14   vi      Made changes to ignore TVM/GPS report when RRC is suspended and CCO is in progress
09/04/14   vi      Made changes to reduce curr_scan prints in the log
08/04/14   bc      Made changes to fix compiler warnings.
07/18/14   ad      Made changes to reduce F3s
07/17/14   sr      Changes for NV refresh FR
06/11/14   bc      Made changes to reduce DCH to FACH cell selection time.
06/18/14   as      Made changes to delay reselection while waiting for L2 ACK for PCH transition
04/23/14   bc      Made changes to send cell info to LTE on receiving UE capability request
                   and also to send cell info WTOL PSHO request.
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/25/14   vi      Made changes to handle LTE deprioritization request in W-RRC
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/26/14   vi      Made changes to print an event for out of service due to no lock case.
03/14/14   bc      Made changes to send partial plmn results to NAS during manual plmn search
11/05/13   rmn     Changes for displaying Partial PLMN Search Results during Manual Search
03/13/14   vi      Stack reduction changes
03/13/14   sp      Removed code under FEATURE_WCDMA_HS_PCH feature flag
03/12/14   rmn     Made changes to ignore preemption from other SUB if there is a CS Call on W SUB
03/11/14   vi      Replaced feature FEATURE_PLI_NMR_UTRAN with FEATURE_WCDMA
02/03/14   as      Made changes to backup internal/external cmds for crash debugging
01/29/14   sr      Made changes to handle FF commands from L1 when waiting for lock
12/30/13   sr      Made changes to convert instance id with AS ID
12/26/13   vi      Moved GPS_CELL_ID changes out of feature FEATURE_CGPS_UMTS_WCDMA
12/06/13   vi      Added new command handling RRC_MUTLIMODE_SUBS_CHGD_IND from NAS to set the correct AS ID
11/27/13   sp      Made changes to add nv support and to save time ticks for 
                   rr to/from rrc mm to/from rrc and to add msg_type for crash debug ota changes
09/16/13   sn      G2W TA feature changes
11/05/13   vi      Corrected MACRO for dedicated priority response
10/27/13   as      Made changes to backup OTAs for crash debugging
10/11/13   gv      Added code to check for AS ID when RRC_CSG_WHITE_LIST_UPDATE_IND is received
10/07/13   rmn     Changes for Feature: RAT Search Order for FDD/TDD LTE
09/30/13   ad      Compilation error in LTE DSDS code 
09/25/13   bc      Added code to support exchange of AS ID when LTE feature is enabled
08/26/13   gv      Made changes to reset L2 ack status for SRB3 after ack is received
08/13/13   sr      Made changes to not set TRM extension flag when in FACH state
08/13/13   vi      Added handling for subsequent DEACT_REQ from NAS even if Lower Layers are deactivated
08/13/13   gv      Added support for FEATURE_TRIPLE_SIM
08/28/13   bc      Made changes to handle GTOW CCO abort and WTOG cco abort requests
07/31/13   vi      Mainlining the MODEM_STATISTICS feature.
07/08/13   sn      Changes for LTE CSG feature
06/28/13   mp      Made changes to ignore OTA messages excpet NAS and SIB if connection release is initiated
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
06/27/13   gv      Added code to update AS ID change due to mode pref change when sim is not available
06/14/13   sr      Made changes to reset IMSI when SIM_INVALIDATE_REQUEST is received
03/25/13   sn      Changes to replace timer based dog reporting with signal based reporting
01/03/13   ad      Made changes to reduce the redundant F3s
12/25/12   db      Made changes to requeue transition to disconnected triggered by CU when there is no response
12/07/12   sn      CSG feature changes
11/27/12   gv      Made changes to allow SIM ID check for Free Floating commands
10/30/12   gv      Added code to support suspend and resume of BPLMN searches from LTE to WCDMA
10/23/12   pm      Made changes to assign pointer to NULL after de-allocation
06/20/11   kp      Added code to trigger close_session_req for HFN 
                   mismatch recovery.
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
10/14/12   gv      Added code to support exchange of AS ID between WRRC and GRR commands
10/04/12   pm      Removed unnecessary comments
09/13/12   gv      Added support to send deact request to TDSCDMA while WCDMA is deactivated to facilitate 
                   writing of ACQ DB NV
09/10/12   gv      Added support for new interface between WRRC and TDS RRC
08/31/12   rd      Transfer dedicated priority parameters to/from WCDMA and GERAN
                   during IRAT procedure
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/26/12   as      Added code to support feature NB/WB handover support
03/28/12   zr      Adding feature definitions for Dime
07/03/12   sks     Added support for WTOL TDD PSHO.
05/24/12   sks     Added support for QCHAT specific dynamic DRX.
05/02/12   rd      Made changes to support NV item for wtol_ps_ho
04/09/12   gv      Added code to prevent CPU hogging during G2W redirection
02/23/12   sks     Added support for WTOL PSHO.
01/04/12   gv      Fixed the issue where NULL pointer is tried to be freed with free floating
12/16/11   gv      Fixed compilation issue
12/12/11   gv      Added code to support free floating of tasks
12/05/11   pm      Added code to support cell id request by CM in DCH state"
11/18/11   mn      Made changes to disable reselection to LTE after attach reject from network
                   with cause 14.
10/31/11   sks     Added support for FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB.
09/28/11   ad      Added code changes for SMC to wait for till RRC comes back 
                   to active state if L2 ack for SMC is received when RRC is in 
                   STANDBY mode.
06/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.99.50
06/02/11   ad      Added MMAL changes to return the trans id of OTA
05/03/11   rl      Fixed the lint error
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/21/11   ad      Fixed compilation warnings
04/20/11   vg      added support for mdsp halt recovery
04/19/11   ad      Changes to add callback function for MMAL support
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/22/11   rl      Moved rb-id variable outside the flag T_WINNT
03/21/11   rl      Added code to support L2W CGI
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/14/11   kp      Bug fix to add cmd .i.d RRC_FAST_DORMANCY_REQ in rrc_check_mm_cmds
                   _for_simid_validation instead of procedure i.d.
03/10/11   sks     Made changes to implement 5s guard timer for LTOW redirection.
03/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.92.50
02/24/11   kp      Added code to store & precess RLC reset during tuneaway
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   sks     Added support for LTOW PSHO.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/02/10   su      Provided fix to process TFCC message while another 
                   Reconfiguration is on going.
12/22/10   su      Made code changes to support FEATURE_WCDMA_ETWS.
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/22/10   sks     Added support for handling EM call during WRLF.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   rm      Added code to handle CSFB status indication
10/28/10   as      Added changes to check for DL ACK for Tuneaway
10/27/10   as      Added changes to save DL SDU during Tuneaway
10/26/10   as      Added changes to skip subscription id check for DS Tuneaway status change ind
10/15/10   rm      Fixed compiler issues when FEATURE_WCDMA_RRC_WDOG_DYNAMIC is defined
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
09/27/10   ss      Added code to support as_id changes for Fast Dormancy
09/22/10   kp      Added code for Qchat timer that delay HSPA status indication
09/17/10   rm      Added code to register RRC with WDOG under FEATURE_WCDMA_RRC_WDOG_DYNAMIC
11/03/10   rl      Updated the log info
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM
09/09/10   rm      Made changes to hold dedicated priorities during OOS
09/07/10   ss      Fixed KW errors.
08/18/10   ss      Added SIB19 logging code.
07/02/10   sks     Added code to support FEATURE_WRLF_SYSTEM_SEL for connected mode OOS.
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/07/10   rm      Made changes to not call DSM API to get the SFN when 
                   FEATURE_WCDMA_BLACKBIRD_BRINGUP is defined
06/07/10   as      Added code for EOOS under feature flag FEATURE_EOOS
05/20/10   gkg     Made changes to log RRC state in SMEM area.
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/17/10   rm      Added code to pass AS ID validation for RRC_DS_STATUS_CHANGE_IND
03/15/10   gkg     Made changes to put changes for "Multiple DRX Cycles" and 
                   "CU wait Timer" under feature R7. 
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/01/10   dm      Added support for handling of RRC_BPLMN_SUSPEND_CNF cmd by
                   MISC procedure.
02/15/10   as      Deleted QTF TEST_FRAMEWORK stub code
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
01/22/10   rm      Modified the usage of qchatupk_wcdma.h with rrc_qchatupk.h
                   (at the request of QCHAT team)
12/17/09   as      Added logs for debuging
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
11/27/09   gkg     RRC changes required to integrate QChat Porting Kit Optimizations 
                   into AMSS. The changes are under feature FEATURE_QCHAT.
11/02/09   sks     Added code to suspend handling of STOP_MODE_REQ, until current 
                   WTOG process is aborted.
09/22/09   rm      Added code to clear L1 barring info and write ACQ DB to NV on
                   MODE_CHANGE_REQ to LPM 
09/14/09   ss      Fixed lint warnings generated by Static Analysis tool
07/30/09   dm      Made changes to send RRC_SIB7_UPDATE_STATUS_IND to CU 
                   procedure also.
07/16/09   ps      Made changes to send STATE CHANGE IND to TFC procedure also
06/15/09   sks     Made changes to support messaging interface with RR for writing 
                   ACQ-DB to NV during power off.
05/14/09   sks     Made changes to suppress compilation errors for RRC UTF.
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/09/09   ps      Made changes to send reconfig ind in CU/or go to idle
                   if RL failure is triggered while l2 ack for reconfig complete
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
01/13/09   rm      Made changes to dispatch RRC_CPHY_START_WCDMA_MODE_CNF to LLC
                   if CSP Scan is RRC_CSP_WCDMA_TO_GAN_CELL_RESELECTION_SCAN
12/29/08   rm      Added support for 3G<->GAN Handover
11/21/08   rm      Added code to call cell selection procedure on receving 
                   RRC_EQ_PLMN_LIST_CHANGE_IND from Higher layers
06/11/08   sk      Added RRC_WTOG_BPLMN_ABORT_GUARD_TIMER_EXPIRED_IND and 
                   RRC_CONTINUE_STOP_MODE_REQ
10/24/08   rm      Added support for Rove out from GAN, IDLE DETACHED MODE
                   Changes are under feature flag FEATURE_GAN_3G_ROVE_OUT_ONLY
09/18/08   ps      Made changes to start T300 timer only after receiving 
                   RRC_CMAC_STATUS_IND from MAC
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7. Removed feature
                   flags FEATURE TFCSCONTROL E NABLE and FEATURE CELLPCH URAPCH
08/25/08   sks     Added code to handle RRC_EXIT_FROM_PWR_SAVE from NAS in connected
                   mode OOS case.
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
05/16/08   pk      Added support for Rove-in to GAN. Changes are under the feature 
                   flag FEATURE_GAN.
03/07/08   vr/da   Added support to route new MBMS commands to MBMS procedure
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
02/27/07   pk      Added support to call RR function rr_rrc_acq_db_save() to write
                   RR ACQ DB to NV on power off.
02/07/07   da      Added support for FEATURE_PLI_NMR_UTRAN
12/21/07   vr      Support for FEATURE MBMS
09/10/07   pk      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
08/31/07   ps      rrcgps.h file is removed and code is move to rrcmeasi.h. So 
                   adding rrcmeasi.h in header instead of rrcgps.h
08/28/07   kp      Added support for capturing RACH transmission Statistic.
08/08/07   ps      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
07/23/07   kp      Added support for hadnling RR_CIPHER_SYNC_IND in 
                   rrc_dispatch_rr_commands.Code changes are under new feature flag
                   FEATURE_SPEC_CR_2567_GTOW_SECURITY_KEY_HANDLING.
05/25/07   vg      added support for FEATURE_CGPS and FEATURE_CGPS_UMTS_CP_WCDMA.
05/23/07   pk      Added support to route RRC_MANUAL_PLMN_FREQ_SCAN_TIMER_EXPIRED_IND 
                   to rrc_cell_selection_procedure under the feature flag 
                   FEATURE_MANUAL_SEARCH_FREQ_SCAN_SKIP_TIMER.
04/06/07   vm      Added comment for clarification.
03/28/07   kp      Added support for handling new command RRC_BCCH_MODIFICATION_TIMER_EXPIRED_IND
                   which indicate BCCH Modification timer expiry. 
03/04/07   vm      Added support for FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2.
02/09/07   kp      Removed extra hop of command interface between SMC & LLC, 
                   for resuming uplink RB’s & updating uplink ciphering keys. 
                   Now SMC directly places these request to L2 task & L1 respectively 
                   bypassing LLC.
02/02/07   da      Made change to ignore RL FAILURE indication during DCH->FACH
                   transition.
01/04/07   kp      Added support for handling new command which Updates Uplink 
                   Ciphering Keys.
11/23/06   kp      SMC suspend request & confirmation will be through call back function 
                   with RLC, Cleaning up code for command interface.
09/28/06   da      Allow forwarding of PDUs to RCR Proc when RCR proc is active.  
                   This is to allow CCCH RCR msg to release the RRC connection
                   when RCR proc is in certain substates.
09/25/06   kp      Added support for sib_change_ind interface change
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/28/06   da      Added support for OOS area handling during transition from DCH.
08/25/06   vm      Added support for FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT.
08/21/06   tkk     Added SRB#5 support.
07/21/06   vm      Added support for the feature UOOSI.
06/07/06   kp      Added support for six hours sib update
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
05/08/06   segu    Added support for RSSI optimazation
05/02/06   sg      Added support for single to dual mode changes
04/11/06   sgk     merged RVDS changes to mainline.
03/14/06   tkk     Lint warnings/errors cleanup
02/23/06   vr      Do not process RRCConnectionSetup and RRCConnectionReject 
                   messages if UE not in RRC_STATE_CONNECTING 
02/15/06   sgk     Added code to forward the act cancel to cu procedure. 
02/07/06   vr      Do not block BCCH-BCH and CUCnf messages if RCR is active.This
                   would allow reselections to happen when UE is waiting for L2 Ack
				   for RRCConnectionReleaseComplete.
01/30/06   vm      Added the following BPLMN fix: routing of RRC_BPLMN_SUSPEND_CNF
                   to RRC_PROCEDURE_CHO (earlier it was getting dropped).
01/04/05   kc      Route BPLMN_SEARCH_ABORT to CSP directly rather than MCM
12/30/05   sg      Added support for handling BPLMN Abort search from NAS
11/17/05   vr      Changed a MSG_HIGH to be more descriptive when we drop L2 Acks.
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search.
11/04/05  da/vr    Made changes to detect UE inactivity and take appropriate actions.
10/19/05   vr      Support for Counter Check Procedure.Route CounterCheckMessage to 
                   CHK procedure.Also fixed a lint error. 
07/21/05   ss      added RRC_IDT_COMPLETED_FOR_DOMAIN to be routed to scrr procedure for queuing
                   of release until IDT is completed.
07/20/05   vr      Support for Spec Compliant WTOG CCO.Route the 
                   RR_INTERRAT_CC_ORDER_CNF to CHO procedure
07/08/05   vm      FEATURE_OUT_OF_SERVICE_ENHANCEMENT: Added support for OOS in 
                   Connected Mode feature.                                      
                   Added support for handling new command RRC_CONN_MODE_OOS_TIMER_EXPIRED_IND
                   which is routed to CSP.
06/21/05   vr      Send RRC_DEACT_REQ and RRC_MODE_CHANGE_REQ to MCM because
                   MCM now handles these commands
06/07/05   kc      Removed DL-SDU length limitation of 512 bytes for non-MMGPS messages.
                   As we started seeing messages > 512 even in regular RRC-msgs.
06/02/05   vr      Added support for Interrat reslection abort in Idle mode
05/11/05   vm      Added case for new NAS command RRC_NW_SEL_MODE_RESET_IND.
                   This command is routed to Cell Selection Procedure
05/03/05   sm      Added support to process cmd 'RRC_ABORT_CIPH_CONFIG_REQ' 
                   from SMC 
04/20/05   sgk     Added support for processing RRC_EQ_PLMN_LIST_CHANGE_IND
                   from MM.
04/06/05   vr      Include rrclogging.h unconditionally for logging on all platforms
04/06/05   da      Added BMC procedure in rrc_dispatch_internal_commands.  Calling
                   BMC event handler in rrc_dispatch_l1_commands() in case of OOS area
                   ind.
04/04/05   bu      Added support for inter-RAT redirection feature
                   . Added case statements for new RR commands:
                     RR_INTERRAT_REDIRECT_REJ and RR_INTERRAT_REDIRECT_ABORT_CNF.
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
03/04/05   kc      Route cell-update-complete-cnf to IHO/CHO if requested.
03/03/05   vm      Added support for inter-frequency redirection feature.
                   Function rrc_cell_selection_procedure() is called for handling of 
                   new command RRC_FREQ_REDIRECT_WAIT_TIMER_EXPIRED_IND.
02/17/05   sm      Added support to validate DL-PDU during SRNS relocation by decoding 
                   PDU before doing integrity check of config message during SRNS 
                   relocation scenario.
12/17/04   vr      Added support for WCDMA->1x handover feature
11/22/04   vm      Added support for feature FEATURE_GTOW_BACKGROUND_PLMN_SEARCH.
                   Route RRC_BPLMN_FREQ_SCAN_TIMER_EXPIRED_IND and 
                   RRC_BPLMN_SRCH_TIMER_EXPIRED_IND to rrc_cell_selection_procedure.
11/22/04   da      Added RRC_PHY_CHAN_FAILURE_TIMER_EXPIRED_IND to rrcrce_procedure_event_handler
11/18/04   vm      Route RRC_DEEP_SLEEP_NO_SVC_TIMER_EXPIRED_IND to 
                   rrc_cell_selection_procedure.
11/18/04   vk      Route RRC_RRC_CON_DELAY_TIMER_EXPIRED_IND expiration to RCE
                   procedure
09/15/04   sk      Added support for RRCTMR_T_OOS_TIMER exiry handling. Route this
                   indication to rrccu procedure.
08/23/04   bu      Route RRC_CPHY_DL_WEAK_IND to CSP.
08/18/04   kc      Peek into Connection-Reject before ASN1 decoding
08/17/04   vk      Ignore the downlink message received only if the message
                   is not downlink direct transfer and RRC Connection Release is active
08/12/04   bu      Changed the order of State change indication for CSP and RCE.
08/05/04   bu      Route RRC_CSP_WCDMA_RESUMED_IND to CSP.
07/16/04   ttl     Added TFCC support.
07/15/04   kc      Route SIB-Modification-Ind to SIB procedure.
07/13/04   kc      Compare message id correctly from the peeked info.
07/07/04   kc      Peek into UE-Id before asn1 decoding of conn-setup message
06/25/04   kc      Print MSG_ERROR only if ASN1 decoding fails.
06/23/04   vk      If a downlink message is received when RCR is active, then ignore
                   that message
06/08/04   kc      ERR_FATAL instead of MSG_HIGH if ASN1 decoding fails due to
                   memory-allocation issues.  Also routing STATE_CHANGE_IND to 
                   DDT procedure.
05/25/04   sk      Added rrccu proc for sib_change_indication
05/13/04   bu      Routed RRC_DCH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND to CSP.
05/05/04   bu      Route RRC_FREQ_SCAN_TIMER_EXPIRED_IND to CSP.
03/19/04   kc      Added support to route internal and inter-rat commands for
                   GtoW PCCO commands.
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
03/04/04   ttl     Added FEATURE_CELLPCH_URAPCH for code of Cell_PCH/URA_PCH.
02/20/04   bu      Route CPHY_START_WCDMA_MODE_CNF and RRC_CPHY_OPERATIONAL_ERROR_IND
                   to LLC if error fatal recovery is in progress.
02/12/04   bu      Added routing of RRC_ERR_FATAL_IND to CSP.
02/05/04   sk      Send RNTI UPDATE CNF to rb procedures 
                   Added support for URA update initiation command.
02/02/04   vk      Added support for A-GPS commands under FEATURE_MMGPS
12/04/03   vn      Get the current CFN when an RRC Signalling message is received
                   from RLC and timestamp it.
11/11/03   kc      Added support to route SRNS relocation commands.
09/21/03   bu      Added code to route RRC_FORBIDDEN_LAI_LIST_UPDATE_REQ to CSP.
09/03/03   bu      Added code to route RRCTMR_DELAY_ERR_FATAL_TIMER_EXPIRED_IND
                   to LLC.
08/15/03   kc      Added support to update dual mode status flag when reselection
                   or handover is received from RR
07/30/03   rj      Added code to route RRC_SMC_DDT_L2_ACK_IND to DDT procedure.
07/22/03   jh      Removed coupling with QXDM logging for RRC signaling message
07/21/03   kc      Routed RRC_SMC_UDT_L2_ACK_IND to RRCUDT procedure.
07/11/03   vn      Routed RRC_CPHY_ERROR_IND to RRCLLC.
06/23/03   vn      Routed RRC_DCH_FACH_CELL_SELECTION_TIMER_EXPIRED_IND
                   to CSP and RRCTMR_L1_DEADLOCK_DETECT_TIMER_EXPIRED_IND
                   to LLC.
06/20/03   bu      Added RRC_PROCEDURE IHO under FEATURE INTERRAT HANDOVER WTOG.
06/03/03   rj      Added code to route inter-task commands to RRCCHO
                   module.
05/23/03   bu      Added code to route RRC_MODE_CHANGE_REQ to CSP and 
                   RRC_MODE_CHANGE_IND to IHO procedure.
05/01/03   kc      Added support to handle RRC_HO_CIPH_REQ and CNF to support
                   GtoW Inter-RAT ciphering handover.
04/24/03   rj      Added code to route case RRC_SIG_CONN_RELEASE_REQ to SCRR
                   and RRC_SIG_CONN_RELEASE_CNF to RCE procedures.
04/10/03   rj      Added code to route Cell Change Order from UTRAN.
04/08/03   kc      Changed is_process_required parameter to cmd_ptr from cmd_id
03/27/03   vn      Added routing of CELL_UPDATE_COMPLETE_CNF to TCRC and PCRC.
03/19/03   kc      Changed MSG_HIGH's to MSG_MED for Integrity Check Failure
                   Failure is already reported as ERR in rrcsmc.c. Changed to
                   avoid confusion while parsing logs.
03/05/03   rj      Added code to route RRC_CPHY_OUT_OF_SERVICE_AREA_IND command
                   to RRCRCE module in Connecting state and Disconnected state
                   and route the same to RRCCU module in CELL_FACH state if 
                   if feature FEATURE_RRC_CELL_UPDATE_ERRORS_SUPPORTED is 
                   not defined..
02/18/03   vn      Added routing of CELL_UPDATE_COMPLETE_CNF to RBR also.
02/23/03   kc      Added feature definitions around IHO procedure constant as
                   it is not supported in single mode builds.
02/19/03   kc      Added feature definitions around GtoW code to remove 
                   compilation warnings when the feature is not defined.
02/14/03   kc      Added code to route GtoW related commands to IHO and MCM
                   procedure.  Also added code to handle new RR messages for
                   GTOW procedures.
02/10/03   xfg     Added code to route STATE_CHANGE_IND to IHO procedure.
01/15/03   sk      Elimination of lint errors:
                   -Replaced tabspaces with spaces to correct indentation
                   -typecasted the return value of rrclcm_get_dl_rb_id() value to 
                   uint8 

01/02/03   bu      Added code to route STATE_CHANGE_IND to CCM procedure.
12/13/02   rj      Added code to route RRC_DOWNLINK_L2ACK_IND to Signalling 
                   Connection Release Request procedure.
12/12/02   jh      Added rrc_get_dl_message_type() function.  Added QXDM event
                   reporting for received downlink signalling messages.
12/04/02   xfg     Added support for RRC_CHANGE_MODE_IND from MM for mode change
11/12/02   xfg     Added support for RRC_PLMN_LIST_REQ
11/04/02   rj      Added code route RRC_IGNORE_BCCH_MOD_TIMER_EXPIRED_IND to
                   Paging Type 1 procedure.
10/30/02   bu      Added support to route the CPHY_DRX_CNF to LLC.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
10/24/02   xfg     Added codes to route RRC_CHANNEL_CONFIG_CNF to IHO procedure
10/12/02   rj      Added code to route RRC_DOWNLINK_L2ACK_IND to Initial 
                   Direct Transfer procedure and Uplink Direct Transfer
                   procedure. Aslo added code to route RRC_NAS_DATA_TRANSFER_IND
                   to Initial Direct Transfer procedure.
10/11/02   xfg     Fixed a message high confusion. 
10/10/02   xfg     Added routing function for RRC_NAS_DATA_TRANSFER_IND command
10/04/02   xfg     Added support for GSM to WCDMA cell reselection.
10/03/02   vk      Added support to route RRC_DELETE_MEAS_REQ to Measurement
                   procedure
09/25/02   vn      Added to dispatch CELL_UPDATE_COMPLETE_CNF to RBE and RBRC 
                   procedures when so indicated in CELL_UPDATE_COMPLETE_CNF.
09/26/02   vk      Added routing of Compressed Mode Failure Indication
                   (RRC_CPHY_COMPRESSED_MODE_IND) to LLC under FEATURE CM SUPPORTED
09/24/02   xfg     Roputed RRC_CPHY_SUSPEND_WCDMA_MODE_CNF and
                   RRC_CPHY_RESUME_WCDMA_MODE_CNF to LLC instead of MCM.
09/19/02   xfg     Added support for WCDMA to GSM inter-RAT cell reselection
09/17/02   xfg     Added handling for RRC_ABORT_REQ for WCDMA to GSM HHO
09/04/02   kc      Added commands for ciphering related functions
08/30/02   vn      Always send a State Change Indication to LLC at the end.
                   Fixes OOS and RCE race conditions - CR 24223
08/27/02   kc      Fix for CR24192. Fixed memory leak of not calling free_cmd_buf 
                   when Integrity check fails for a DOWNLINK_SDU_IND
08/07/02   rj      Removed routing of commands RRC_PAGING_CTRL_REQ and
                   RRC_CIPHER_REQ. Added rounting of RRC_SIM_INSERTED_REQ and
                   RRC_SIM_UPDATE_REQ to rrcmisc module for new RRC-MM interface.
07/30/02   xfg     Added changes for supporting the WCDMA to GSM inter-RAT
                   handover. All changes are made under FEATURE INTERRAT HANDOVER WTOG.
07/18/02   xfg     Added codes to dispatcher RRC_MODE_CHANGE_IND for dual-mode
                   support.
06/17/02   rj      Modified code to route RRC_CRLC_STATUS_IND comamnd to 
                   Cell Update procedure.
06/14/02   vk      Routed the Measurement Control Confirmation from MAC directly
                   to Measurement procedure instead of going through LLC
06/13/02   vn      Added calls to rrcrbrc_procedure_event_handler for Radio
                   Bearer Reconfig procedure specific events.
06/11/02   rj      Updated code to use latest interface to LCM identify the 
                   procedure to distribute L2 ACK.
06/03/02   ram     Removed clearing of sigs in rrc_event_wait().
                   Added support for processing RRC_SIB7_UPDATE_STATUS_IND
                   command.
05/30/02   ram     Added processing for internal command 
                   RRC_SIB7_EXP_TIMER_EXPIRED_IND. Added check for 
                   procedure name for RRC_GET_ALL_SIBS_CNF and
                   RRC_GET_SPECIFIC_SIBS_CNF.
05/17/02   ram     Fixed debug message problem for RRC_STATE_CHANGE_IND
05/17/02   vk      Added new commands RRC_CMAC_UL_TRAFFIC_IND & 
                   RRC_CMAC_MEAS_CTRL_CNF to support Traffic Volume
                   Measurements under the feature
                   FEATURE TRAFFIC VOLUME MEAS
05/16/02   rj      Fixed a previous merge mismatch. 
05/16/02   bu      Merged the following from cell reselection branch:
           bu      Added code to route RRC_CPHY_CELL_RESELECTION_IND to CSP,
                   RRC_CELL_RESELECTION_REQ to LLC and
                   RRC_CELL_RESELECTION_CNF to CCM.
05/14/02   bu      Removed the MSG_HIGHs in rrc_event_wait().
05/13/02   ram     RRC_STATE_CHANGE_IND had been modified so that it can be sent
                   to multiple procedures. Made the corrosponding change to
                   dispatch it to multiple procedures.
05/10/02   bu      Added function rrc_event_wait() to wait on a specified signal.
                   While waiting for the signal it keeps petting the watchdog.
05/06/02   xfg     Added dual-mode changes with FEATURE DUALMODE BASELINE tag
05/09/02   vn      Added calls to Transport and Physical Channel event handlers.
05/07/02   rj      Modified code to route RRC_STATE_CHANGE_IND to SCRR procedure
                   in stead of SCR procedure.
04/23/02   kc      Added support for RRC_CIPHER_REQ
04/10/02   rj      Updated function rrc_perform_integrity_protection_check
                   to initialize the RRC procedure correctly.
03/29/02   rj      Added code to peform integrity check before performing
                   ASN.1 decoding. Also renamed function rrc_decode_dl_sdu to
                   rrc_perform_integrity_protection_check for more readability.
                   If Integrity Check fails the message will be trashed and will
                   not be forwarded to any procedure.
03/28/02   upn     Merged reconfig procedure changes to main line.
03/25/02   rj      Added a missing break in function rrc_identify_procedure.
03/25/02   rj      Modified function rrc_decode_dl_sdu to use rrcasn1_decode_pdu
                   instead of rrcasn1_decode_pdu_with_bug since actual memory
                   leak is fixed.
03/12/02   rj      Added code to route RRC_CELL_UPDATE_COMPLETE_CNF to RCE
                   module.
03/11/02   kc      Added code to handle State change for UMI and SMC procs
03/05/02   kc      Added code to handle L2ACK for RRCSMC procedure.
03/01/02   rj      Added code to route commands RRC_RNTI_UPDATE_REQ and
                   RRC_RNTI_UPDATE_CNF.
02/28/02   rj      Added code route all commands that are destined to Cell
                   Update procedure.
02/28/02   ram     Some ERR macros exceeded the 40 char limit. Fixed those.
                   Fixed a typo in the ERR macro for SIB change indication
                   which was previously saying "State Change" indication.
02/14/02   kc      Added functionality to handle to UMI and PG2 procedures
02/12/02   bu      Added call to rrc_csp_write_nv_acq_info() to write the
                   acquisition database and acquisition list to NV.
02/05/02   ram     Added support for the command RRC_SIB_CHANGE_IND.
01/30/02   xfg     Added the handler for processing messages from TC for 
                   loopback test
11/07/01   vk      Fixed an incorrect message low comment which pointed to
                   cell selection procedure instead of measurement procedure
11/05/01   vk      Added support for RRC_CPHY_MEAS_CTRL_CNF cmd. Also added
                   support for handling measurement control message from UTRAN
10/30/01   rj      Added edit history for previous check-in.
10/30/01   rj      Added code to identify procedure for Measurement Control
                   Message.
10/26/01   bu      Added code for handling the DEACT_REQ from MM. Added new
                   enum type for the RRC deactivation status. 
10/25/01   rj      Modified F3 messages length to less than 40 charecters.
10/18/01   rj      Added code to route RRC_CELL_UPDATE_INITIATE_REQ and 
                   RRC_CPHY_RL_FAILURE_IND comamnds to Cell Update 
                   procedure.
10/18/01   rj      Added code to route RRC_INITIATE_CELL_SELECTION_IND command
                   to Cell Selection Procedure.
10/17/01   kc      Added code to support Security Mode Command
10/17/01   ram     Added processing of command RRC_SIB_WAIT_TIMER_EXPIRED_IND.
10/16/01   vn      Added for dispatching of RRC_CPHY_OUT_OF_SERVICE_AREA_IND,
                   RRC_LOWER_LAYER_DEACT_REQ and RRC_LOWER_LAYER_DEACT_CNF.
10/12/01   vn      Added a call to cell selection for RRC_LAI_LIST_REQ processing.
10/03/01   rj      Used rrcasn1_decode_pdu_with_buf in place of 
                   rrcasn1_decode_pdu to fix memory leak.
09/16/01   vn      Added a message high for RRC_CPHY_DRX_CNF since it is
                   not being processed at present.
09/15/01   bu      Include dog.h and mmtask.h files.
09/14/01   vk      Added RRC_CPHY_MEASUREMENT_IND COMMAND so that dispatcher
                   can dispatch commands sent by L1 to RRC Measurement procedure
09/14/01   rj      Added code route RRC_PAGING_CNNTROL_REQ command. Updated 
                   function rrc_identify_procedure for message type that is
                   similar to the rrcasn1.h
09/14/01   bu      Added support for DEACT_REQ.
09/14/01   xfg     Added the case to handle the RRC_TIMER_FOR_RSSI_BER_EXPIRED_IND
09/14/01   bu      Added watchdog support. Added signal and timer for watchdog
                   reporting 
09/06/01   vn      Added support for frequency scan (RRC_CPHY_FREQ_SCAN_CNF). 
08/31/01   ram     Added support for Active Set Update Procedure.
08/30/01   rj      Deleted feature FEATURE_QXDM_LOGGING and Moved functions
                   rrc_get_dl_dcch_message_type and rrc_get_dl_ccch_message_type
                   to RRCDATA module since these are required by other 
                   RRC procedures.
08/29/01   vk      Added measurement procedure event handler for dispatcher
                   to direct calls to RRC Measurement procedure. Also added
                   the header file rrcmeas.h
08/14/01   rj      Added code to peek into OTA message and identify message
                   type without ASN1 decoding.
08/10/01   rj      Added code to route RRC_T304_EXPIRED_IND command to 
                   UE Capability Information procedure.
08/06/01   rj      Added code to route UE Capability Enquiry  and UE Capability
                   Information Confirm messages. Added code to route State
                   Change indications to IDT, UDT and SCRR Procedures. Added code
                   to route RRC_REDIRECT_REQ and RRC_DISABLE_PREV_FREQ_IND to
                   Cell Selection Procedure. Added code to route
                   RRC_CELL_UPDATE_COMPLETE_CNF command to IDT, UDT, SCRR and
                   UECI Procedures. Updated some comments.
07/30/01   rj      Renamed feature FEATURE_RRC_DISK_LOGGING to T_WINNT.
07/26/01   ram     When a Measurement Control Message or a Security Mode Command
                   Message is received, printed debug messages to indicate
                   these are not yet supported. Fixed bugs in some MSG macros
                   where parameters were passed without the %type to be
                   used for printing.
06/29/01   ram     When a RRC_PAGING_CTRL_REQ command was received from MM
                   printed a message saying it's not supported yet. Earlier
                   we were printing an error message that an unknown mm 
                   command was received.
06/19/01   vn      If a State Change Indication is destined for 
                   RRC_PROCEDURE_SCR, call the corresponding event handler.
06/18/01   jrs     Featurized on-target signalling message logging with 
                   FEATURE_QXDM_LOGGING.
06/18/01   kmp     Fixed a merge error.
06/18/01   jrs     Fixed a compile error.
06/18/01   kmp     Added processing of the Radio Bearer Release SDU in
                   rrc_identify_procedure().
06/15/01   kmp     Added support for RRC_CRLC_RELEASE_ALL_CNF in
                   rrc_dispatch_rlc_commands().
06/11/01   rj      Added a new function rrc_log_dl_sig_message to perfrom 
                   QXDM logging for  Downlink signalling message.
06/11/01   ram     Added support for calling the sib procedure when a 
                   RRC_UPDATE_SIBS_IND command is received.
06/07/01   vn      Added call to rrc_cell_selection_procedure for RRC_ACT_REQ.
06/07/01   rj      Added a check for dl_sdu_length. If dl_sdu_length is zero,
                   then DSM item is trashed.
05/30/01   ram     Added support for calling the Radio Bearer Release procedure
                   event handler when an event for that procedure is received.
05/24/01   vn      rrcllc_cmd_process() is now called for all commands going 
                   to LLC (New LLC architecture). Calls to rrcllc_cmd_process()
                   added for RRC_CPHY_PHYCHAN_ESTABLISHED_IND and CPHY_OUT_OF_SYNC_IND.
05/17/01   ram     When a State change indication or a Downlink L2 ack was
                   received, added code to call the Radio Bearer Setup
                   procedure if the event was for the Radio Bearer Setup
                   procedure. When using the ERR macro in some cases the
                   wrong variable was printed. Fixed that problem.
05/09/01   rj      Modified function rrc_dispatch_internal_commands to route
                   Signalling Connection Message to RRCSCR procedure.
05/01/01   rj      Fixed typo while routing Downlink direct transfer message.
04/29/01   rj      Added support for the Paging Type1 procedure to be called
                   when a RRC_DOWNLINK_SDU_IND containing Paging Type 1 message is
                   received. Renamed FEATURE_RRC_MSG_LOG to 
                   FEATURE_RRC_DISK_LOGGING. Added a break statement after
                   rrcrbe_procedure_event_handler.
04/25/01   ram     Added support for the Radio Bearer Setup procedure to be called
                   when a downlink SDU containing the Radio Bearer Setup message
                   is received.
04/09/01   kmp     Updated a MSG_HIGH to correctly indicate a CPHY_IDLE_CNF
                   in rrc_dispatch_l1_commands().
03/30/01   ram     Included customer.h header file.
03/26/01   kmp     Updated to ahndle new RRC_RESET_L1_REQ/CNF and the
                   CRLC_*_*_CONFIG_CNFs, and CPHY_TRANSITION_REQ. Updated 
                   rrc_identify_procedure() to update the dl_sdu number from a
                   uint8 to a uint32.
03/14/01   rj      Inlcuded rrcdt.h and added code to dispatch events
                   to Initial direct transfer, Uplink direct transfer,
                   downlink direct tranfer, signalling connection release and
                   signalling connection release request procedures. Added
                   code to dispatch RRC_ABORT_REQ command to RCE procedure.
03/08/01   ram     When indicating an unknown command, changed the ERR macro
                   to print the command id in Hex since it's easier to recognize
                   the command id.
02/22/01   rj      Added RRC_WAIT_TIMER_EXPIRED_IND, RRC_T300_EXPIRED_IND,
                   RRC_TIMER_FOR_RCE_L2ACK_EXPIRED_IND, RRC_REDIRECT_CNF,
                   RRC_TIMER_FOR_RCR_L2ACK_EXPIRED_IND, and 
                   RRC_T308_EXPIRED_IND commands to the function
                   rrc_dispatch_internal_commands.
02/19/01   rj      Included rrcrce.h and rrcrce.h. Added new function 
                   rrc_identify_procedure to identify to which procedure
                   the received OTA message belongs to. Updated
                   rrc_decoded_dl_sdu function. Updated function             
                   rrc_dispatch_internal_commands to call RCE and RCR event
                   handlers. Added code to dispatch RRC_DOWNLINK_L2ACK_IND
                   command to the respective procedures and added code to
                   dispatch RRC_EST_REQ to RCE procedure.
02/18/01   vn      Added a call to LLC to process L1 Confirmations.
02/05/01   kmp     Fixed a bug in rrc_dispatch_internal_commands() where a 
                   switch statement was switching on the wrong variable.
01/30/01   rj      rrclcm_lc_id_type and rrclcm_channel_type_e_type are 
                   replaced with rlc_lc_id_type and uecomdef_logchan_e_type
                   repectively from uecomdef.h
01/30/01   vn      Added call to CCM event handler when procedure ID in 
                   RRC_CHANNEL_CONFIG_CNF or RRC_LLC_CMD_CNF indicates so.
01/24/01   vn      Added calls to Cell Selection Procedure for processing
                   RRC_CPHY_CELL_SELECTION_CNF and RRC_GET_SPECIFIC_SIBS_CNF.
                   Routed CPHY_GET_ALL_SIBS_CNF to CCM instead of CSP.
01/23/01   kmp     Updated for new RRC LLC interface.
01/18/01   vn      Added support for processing of RRC_CELL_CHANGE_REQ, 
                   RRC_CELL_CHANGE_CNF and RRC_NEW_CELL_IND.
01/09/01   ram     Added support for two new internal commands - RRC_NEW_STATE_IND
                   that is processed by the rrc state change manager and RRC_STATE
                   _CHANGE_IND that is sent to other procedures by the state
                   change manager.
12/22/00   rj      Moved PDU type identification from rrc_decode_dl_sdu to the
                   RRCLCM. A new field sdu_type is added to the 
                   RRC_DOWNLINK_SDU_IND command so  that it becomes easy to
                   deallocate the memory for decoded message in
                   rrc_free_cmd_buf function.
12/20/00   ram     Added support for the internal RRC command RRC_GET_SPECIFIC
                   _SIBS_REQ.
12/15/00   ram     Changed RRC_GET_ALL_SIBS to RRC_GET_ALL_SIBS_REQ since the
                   the name was changed in rrccmd.h. In rrc_dequeue_dl_sdu
                   added more comments on the function return value.
12/13/00   ram     Implemented the rrc_decode_dl_sdu function. This function
                   first finds the pdu_type for the received message
                   Then it calles the ASN1 decode function to obtain
                   the decoded message. Then, based on the PDU type, it
                   determines the message type of the received message. Finally
                   it determines which procedure should be called to process
                   this message.
12/13/00   rj      In function rrc_dequeue_dl_sdu, rrclcm_get_watermark_ptr and
                   rrclcm_get_rb_id are replaced with rrclcm_get_dl_watermark_ptr
                   and rrclcm_get_dl_rb_id.
12/08/00   rj      Code for logging RRC signalling messages is added 
                   conditiaonally to rrc_dequeue_dl_sdu function.                   
12/07/00   ram     Fixed some compilation errors.
12/07/00   rj      Added a new function rrc_decode_dl_sdu and modified 
                   rrc_dequeue_dl_sdu to dynamically allocate buffer for DL SDU.
12/05/00   ram     The dispatcher has been updated to use the new data types
                   and functions provided by rrccmd.h. Also new commands have
                   been added to some of the dispatcher functions. Added
                   a function to dequeue an RLC SDU from the corrosponding
                   watermark queue when a "DOWNLINK_SDU_IND" command is
                   received.
12/01/00   ram     Included err.h and changed some MSG_HIGHs to ERRs. Commented
                   out rrc_rce_procedure which has not been implemented yet.
11/29/00   ram     Updated the entire module to use command queues and signals
                   instead of events. 
11/14/00   kmp     Updated to add rrc_sib_event_controller() functions calls
                   for IM_CHANNEL_SETUP_CNF and IM_CHANNEL_REL_CNF events.
11/08/00   ram     Whenever there is an unknown event received, added
                   zrex_event_release function call to release the event.
                   Also for events that have no function defined yet for
                   processing them, added a call the release the event.
11/07/00   kmp     Added #includes for L1, L2, and cell selection events.
                   Also added the cell_selection_procedure() call.
11/06/00   ram     Added function to process RRC_DOWNLINK_SDU_IND_EVENT.
11/06/00   ram     Removed rrc_sample_function which was created earlier
                   as a temporary function. Typecast the received event
                   to zrex_event_header_type in all the functions which is
                   the correct way to read the header type. Added comments
                   and corrected several syntax errors.
10/27/00   ram     Updated to include processing for SIB processing events.
                   Corrected syntax errors.
10/27/00   vn      Updated to include processing for Cell Selection events.
10/27/00   kmp     Updated to inlcude the L1 Channel Setup Req/Cnf events.
                   Also updated to include new Internal Release events.
                   Also added rrc_sample_function() stub for now.
10/20/00   ram     Updated based on new ZREX API.
10/12/00   ram     Changed the file to contain only the rrc_dispatcher.
09/22/00   ram     Created file.

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
#include "msg.h"
#include "err.h"
#include "queue.h"
#include "rex.h"
#include "event.h"
#include "rrcasn1.h"
#include "rrcasn1util.h"
#include "rrcasu.h"
#include "rrcdispatcher.h"
#include "rrccmd_v.h"
#include "rrccmdi.h"
#include "rrcdata_v.h"
#include "rrcsigs.h"
#include "rrclcm.h"
#include "rrcllc.h"
#include "rrccsp.h"
#include "rrccspi.h"
#include "rrcmmif.h"
#include "rrcscmgr.h"
#include "rrcsibproc.h"
#include "rrcccm.h"
#include "memheap.h"
#include "dsmutil.h"
#include "rrcrbe.h"
#include "rrcrbreconfig.h"
#include "rrctcreconfig.h"
#include "rrcpcreconfig.h"
#include "rrcrbr.h"
#include "rrcrce.h"
#include "rrcrcr.h"
#include "rrcdt.h"
#include "rrcpg1.h"
#include "rrcmisc.h"
#include "rrcuece.h"
#include "rrcueci.h"
#include "rrcmeas.h"
#include "rrcsmc.h"
#include "rrccu.h"
#include "rrclbt.h"
#include "dog_hb_rex.h"
#include "rrcpg2.h"
#include "rrccspdb.h"
#include "rrcumi.h"
#include "rrccho.h"
#include "rrcmcm.h"
#include "rrclogging.h"
#include "rrcchk.h"

#include "rrcrrif.h"
#include "rrciho.h"
#ifdef T_WINNT  /* Include files when logging is enabled.*/
#error code not present
#endif

#ifdef FEATURE_RR_ACQ_DB
#include "rr_rrc_if.h"
#endif /* FEATURE_RR_ACQ_DB */

#include "seq.h"


#include "rrctfcc.h"


#include "rrcbmc.h"

#include "rrcmeasi.h"

#ifdef FEATURE_QCHAT
#include "rrc_qchatupk.h"
#endif /*FEATURE_QCHAT*/

#include "rrcdormancy.h"

#ifdef FEATURE_WCDMA_LOG_RRC_STATES
#include "rrclogging.h"
#endif /* FEATURE_WCDMA_LOG_RRC_STATES */

#ifdef FEATURE_DUAL_SIM
#include "rrcwrm.h"
#endif

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
#include "rrcfreefloating.h"
#endif

#ifdef FEATURE_WTOL_PS_HANDOVER
#include "rrcnv.h"
#endif

#if defined (FEATURE_TDSCDMA_TO_WCDMA) || defined (FEATURE_WCDMA_TO_TDSCDMA)
#include "rrc_tdsrrc_if.h"
#endif
#include "rrctmr.h"
#include "stringl.h"
#include "rrcsibdb.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define MAX_SIZE_OF_SDU 2048

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef enum 
{
  RRC_DEQUEUE_SUCCESS,              /* Status for success */
  RRC_DEQUEUE_FAILURE               /* Status for failure */
}rrc_sdu_dequeue_status_type;

typedef enum
{
  RRC_NORMAL_OPERATION,
  RRC_DEACT_INITIATED, /* Received DEACT_REQ from MM */
  RRC_DEACT_COMPLETED  /* Received DEACT_CNF from Lower Layers */
}rrc_deact_status_type;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
rrc_deact_status_type rrc_deact_status = RRC_NORMAL_OPERATION;

//used to get status of decoding.. if decoding error due to mem-allocation,
//then do not process the message any further..
extern rrc_decode_info_type rrc_decode_info;

extern rrc_proc_e_type rrc_wait_for_l2ack_for_reconfig;

extern sys_specialization_t  search_order_specialization;

#ifdef FEATURE_DUAL_SIM
extern trm_unlock_event_enum_t rrcwrm_unlock_type;
#endif
/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
#undef  DOG_RRC_RPT
#define DOG_RRC_RPT rrc_dog_rpt_var
dog_report_type rrc_dog_rpt_var = 0;      /* Initial Safety */
  

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
/* Queue for RRC commands sent by LSM or TM task                 */
extern q_type rrc_lsm_cmd_q;
#endif
#ifdef FEATURE_QSH_EVENT_METRIC
extern rrc_qsh_ul_failure_ota_param_s failure_ota;
extern rrc_qsh_metric_db_type rrc_qsh_metric_db[WRRC_QSH_METRIC_MAX+1];
boolean rrc_qsh_dl_committed = TRUE;
#endif

extern void rrc_get_dl_log_message(rrc_cmd_type *cmd_ptr);


#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
extern void rrc_process_modem_stat_info(rrc_cmd_type *cmd_ptr);
#endif


/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/*===========================================================================
FUNCTION RRC_EVENT_SIG

DESCRIPTION
  Wait on the specified signal.  Handle signals as they come in. Only time
  critical watchdog and other signals are acknowledged.

  NOTE: The signal passed into this function is not cleared. It has to
  be cleared by the calling function.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECT
  Some signals are ignored till we're done with the signal

===========================================================================*/
void rrc_event_wait( rex_sigs_type sig)
{
  rex_sigs_type sigs;             /* hold signals from rex_wait() */

  for( ;; )
  {                     /* until we get the specified signal */
    sigs = rex_wait(
          sig | RRC_RPT_TIMER_SIG);

    /* Need to kick the watchdog */
    if( sigs & RRC_RPT_TIMER_SIG )
    {
      rrc_dog_report_and_clear_sig();
    }
    else if (sigs & sig) /* Received the specified signal. Exit the routine */
    {
      return;
    }
  } /* Continue waiting for the signal */
} /* rrc_event_wait */


/*===========================================================================

FUNCTION    rrc_identify_procedure 

DESCRIPTION
  This function looks into choice field and identifies the Message type and 
  then returns the corresponding procedure to which that particular message
  is benlongs.
  
DEPENDENCIES
  None

RETURN VALUE
  rrc_proc_e_type

SIDE EFFECTS
  None

===========================================================================*/
static rrc_proc_e_type rrc_identify_procedure(
  rrc_cmd_type *cmd_ptr                 /* To hold received command */

)
{      
  
  rrc_proc_e_type rrc_proc = RRC_PROCEDURE_NONE;
                                        /* Store name of procedure 
                                           that processes the decoded message */
  uint8 msg_type;                       /* Local variable to store message
                                           type */             
                                    
  switch(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type)
  {
    case rrc_BCCH_FACH_Message_PDU:
    case rrc_BCCH_BCH_Message_PDU:
      /* In this case, there are only 2 types of messages possible both r processed
      by SIB proc.  The messages are
       rrc_SystemInformation_BCH
       rrc_SystemInformationChangeIndication */
      rrc_proc = RRC_PROCEDURE_SIB;
      break;

    case RRCLCM_DL_DCCH_SRB5_SDU_TYPE:
      if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length > RRC_SRB5_SDU_LEN) 
      {
        /* Since the TB size that can be expected on
           this logical ID is not more than 10 (3) bits,
           compare length against one octet */
        WRRC_MSG0_ERROR("TFCC message can't have more than one byte of length..ignoring..");
      } 
      else
      {
        rrc_proc = RRC_PROCEDURE_TFC;         
      }
      WRRC_MSG2_HIGH("TFC Procedure hit - Data->0x%x; Length->%d",
                cmd_ptr->cmd.downlink_sdu_ind.dl_sdu[0],cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
      break;

    case rrc_DL_DCCH_Message_PDU:
      msg_type = 
        rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                     cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
      switch(msg_type)
      {
        /* The following needs to be updated as and when new RRC
           procedures are implemented */
        case T_rrc_DL_DCCH_MessageType_downlinkDirectTransfer:
          rrc_proc = RRC_PROCEDURE_DDT;
          break;
        case T_rrc_DL_DCCH_MessageType_rrcConnectionRelease:
          rrc_proc = RRC_PROCEDURE_RCR;
          break;
        case T_rrc_DL_DCCH_MessageType_signallingConnectionRelease:
          rrc_proc = RRC_PROCEDURE_SCR;
          break;
        case T_rrc_DL_DCCH_MessageType_radioBearerSetup:
          rrc_proc = RRC_PROCEDURE_RBE;
          break;
        case T_rrc_DL_DCCH_MessageType_radioBearerReconfiguration:
          rrc_proc = RRC_PROCEDURE_RBRC;
          break;
        case T_rrc_DL_DCCH_MessageType_transportChannelReconfiguration:
          rrc_proc = RRC_PROCEDURE_TCR;
        break;
        case T_rrc_DL_DCCH_MessageType_transportFormatCombinationControl:
          rrc_proc = RRC_PROCEDURE_TFC;
          break;
        case T_rrc_DL_DCCH_MessageType_physicalChannelReconfiguration:
          rrc_proc = RRC_PROCEDURE_PCR;
        break;
        case T_rrc_DL_DCCH_MessageType_radioBearerRelease:
          rrc_proc = RRC_PROCEDURE_RBR;
          break;
        case T_rrc_DL_DCCH_MessageType_ueCapabilityEnquiry:
          rrc_proc = RRC_PROCEDURE_UECE;
          break;
        case T_rrc_DL_DCCH_MessageType_ueCapabilityInformationConfirm:
          rrc_proc = RRC_PROCEDURE_UECI;
          break;
        case T_rrc_DL_DCCH_MessageType_activeSetUpdate:
         rrc_proc = RRC_PROCEDURE_ASU;
          break;
        case T_rrc_DL_DCCH_MessageType_measurementControl:
         rrc_proc = RRC_PROCEDURE_MCMR;
          break;
        case T_rrc_DL_DCCH_MessageType_securityModeCommand:
          rrc_proc = RRC_PROCEDURE_SMC;
          break;
        case T_rrc_DL_DCCH_MessageType_pagingType2:
          rrc_proc = RRC_PROCEDURE_PG2;
          break;
        case T_rrc_DL_DCCH_MessageType_utranMobilityInformation:
          rrc_proc = RRC_PROCEDURE_UMI;
          break;
        case T_rrc_DL_DCCH_MessageType_cellUpdateConfirm:
        case T_rrc_DL_DCCH_MessageType_uraUpdateConfirm:
          rrc_proc = RRC_PROCEDURE_CU;
          break;
        case T_rrc_DL_DCCH_MessageType_cellChangeOrderFromUTRAN:
          rrc_proc = RRC_PROCEDURE_CHO;
          break;
        case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_GSM:
          rrc_proc = RRC_PROCEDURE_IHO;
          break;
#ifdef FEATURE_WTOL_PS_HANDOVER
        case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_EUTRA:
          if(wcdma_rrc_wtol_ps_ho_support_nv || wcdma_rrc_wtol_tdd_ps_ho_support_nv)
          {
            rrc_proc = RRC_PROCEDURE_IHO;
          }
          else
          {
            WRRC_MSG0_HIGH("WTOL: PSHO message ignored as NV is set to FALSE");
          }
          break;
#endif
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
        #error code not present
#endif 
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
        case T_rrc_DL_DCCH_MessageType_assistanceDataDelivery:
          rrc_proc = RRC_PROCEDURE_MCMR;
          break;
#endif
        case T_rrc_DL_DCCH_MessageType_counterCheck:
          rrc_proc = RRC_PROCEDURE_CHK;
          break;
#ifdef FEATURE_WCDMA_ETWS
        case T_rrc_DL_DCCH_MessageType_etwsPrimaryNotificationWithSecurity:
          rrc_proc = RRC_PROCEDURE_BMC;
          break;
#endif

        default:
          WRRC_MSG1_HIGH("The msg type %d is not supported",
                    msg_type );
          break;
      }  
      break;

    case rrc_DL_CCCH_Message_PDU:

      msg_type = 
        rrc_get_dl_ccch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                     cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
      switch(msg_type )
      {
        /* The following needs to be updated as and when new RRC
           procedures are implemented */
        case T_rrc_DL_CCCH_MessageType_rrcConnectionReject:
          rrc_proc = RRC_PROCEDURE_RCE;
          break;
        case T_rrc_DL_CCCH_MessageType_rrcConnectionRelease:
          rrc_proc = RRC_PROCEDURE_RCR;
          break;
        case T_rrc_DL_CCCH_MessageType_rrcConnectionSetup:
          rrc_proc = RRC_PROCEDURE_RCE;
          break;
        case T_rrc_DL_CCCH_MessageType_cellUpdateConfirm:
        case T_rrc_DL_CCCH_MessageType_uraUpdateConfirm:
          rrc_proc = RRC_PROCEDURE_CU;
          break;
        default:
          WRRC_MSG1_HIGH("The msg type %d is not supported",
                    msg_type);
          break;
      }
      break;

    case rrc_PCCH_Message_PDU:
      rrc_proc = RRC_PROCEDURE_PG1;
      break;

    default:
      WRRC_MSG1_HIGH("The SDU type %d is not supported",
                cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
      break;
 
  }

  return(rrc_proc);
} /* end function rrc_identify_procedure */



/*===========================================================================

FUNCTION    rrc_perform_integrity_protection_check 

DESCRIPTION
  This function call the Security Procedure API to perform Integrity 
  Protection Check. If Integrity Protection Check is successful it
  it will decode the contigous stream of octects  using ASN1 
  to identify the message type. This message type is mapped to 
  the corresponding RRC procedure.
  
DEPENDENCIES
  None

RETURN VALUE
  rrc_proc_e_type

SIDE EFFECTS
  None

===========================================================================*/
static uecomdef_status_e_type rrc_perform_integrity_protection_check(
  rrc_cmd_type *cmd_ptr,                 /* To hold received command */
  rrc_proc_e_type *procedure
)
{  

  uecomdef_status_e_type status = FAILURE;   /* Initially set the status to failure */

  uint8 message_choice;                      /* Local variable to store message
                                                choice */


  rrc_crash_debug_ota_pdu_type sdu_type = RRC_SDU_TYPE_NONE; 


  /* Initialize the procedure to NONE */                                    
  *procedure = RRC_PROCEDURE_NONE;

  if (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu != NULL) 
  {
    /* Initialize decoded message to NULL, even though its expected to be set to NULL */
    if (NULL != cmd_ptr->cmd.downlink_sdu_ind.decoded_msg)
    {
      cmd_ptr->cmd.downlink_sdu_ind.decoded_msg = NULL;
      WRRC_MSG0_HIGH("DL SDU's decoded msg pointer NOT set to NULL. Setting it to NULL");
    }

    /* Here Integrity Check can be peformed */
    if(SUCCESS == rrcsmc_check_downlink_integrity(cmd_ptr) )
    {   
       /* Identifes the to which procedure the message belongs to */
       *procedure = rrc_identify_procedure(cmd_ptr); 

       /* If procedure is one of the procedues that
        * can have activation time, timestamp he point when
        * Downlink SDU is received, before ASN1 decoding
        */
       rrc_cfn_before_decode[*procedure] = seq_get_cfn();

       status = SUCCESS;

       
       if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_CCCH_Message_PDU)
       {
         message_choice = rrc_get_dl_ccch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                                       cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

         if((T_rrc_DL_CCCH_MessageType_rrcConnectionSetup == message_choice) ||
            (T_rrc_DL_CCCH_MessageType_rrcConnectionReject == message_choice)) 
         {
           if(RRC_STATE_CONNECTING != rrc_get_state())
           {
             status = FAILURE;
             WRRC_MSG0_HIGH("UE not in Connecting state.Ignoring ConnSetup/Reject message");
             return status;
           }
           if ((rrcrce_verify_ue_identity(cmd_ptr) == RRCRCE_SUCCESS) ||
               (rrcrce_verify_ue_identity_later_than_r3(cmd_ptr) == RRCRCE_SUCCESS))
           {
             WRRC_MSG0_HIGH("UE Identity check passed");
           } 
           else
           {
             status = FAILURE;
             WRRC_MSG1_HIGH("Msg %d for other UE", message_choice);
             
             return status;
           } 
         }
       }
       else if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == RRCLCM_DL_DCCH_SRB5_SDU_TYPE &&
               *procedure == RRC_PROCEDURE_TFC)
       {
         return(status);
       }

      /* Check for NULL here required because there is a possibility that 
         PDU is already decoded while checking the incoming message for
         SRNS relocation case. */
      if (NULL == cmd_ptr->cmd.downlink_sdu_ind.decoded_msg)
      {
        /* Call the ASN.1 decoder to decode the message. */
        cmd_ptr->cmd.downlink_sdu_ind.decoded_msg =
        rrcasn1_decode_pdu(  cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,(int )
                             cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type,
                             cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length
                          );
      }

      //if decoding fails, then check to see if it failed due to
      //malloc issues or not.. if malloc, err_fatal immediately,
      //else print a msg_error and let the procedure take care of
      //sending status_message
      if( cmd_ptr->cmd.downlink_sdu_ind.decoded_msg == NULL )
      {
        if( rrc_decode_info.decode_status == ASN1_OUT_MEMORY)
        {
          ERR_FATAL("Decoding failed. pdu type %d, pdu size %d", 
                    cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type,
                    cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length, 0);
        }
        else if(rrc_decode_info.decode_status == ASN1_CONSTRAINT_VIOLATED)
          
          //decoding failed due to constraint check or other reasons
          //fwd the msg to procedures to be able to send STATUS msg with ASN1 failure
        {
          WRRC_MSG2_ERROR("Decoding failed. pdu type %d, pdu size %d", 
                    cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type,
                    cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
        }

        /* Please notice that here status is NOT reset to FAILURE because for decode_status
         * other than ASN1_OUT_MEMORY, there are procedures which should send response back 
         * to NW. E.g., all the RB procedures will send back failure with cause 
         * rrc_asn1_ViolationOrEncodingError.
         */
      }
      rrc_get_dl_log_message(cmd_ptr);
    }
    else
    {
      MSG_MED("Integrity Check is failed",0,0,0);
    }

  } /*end if (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu != NULL) */
  else
  {
    WRRC_MSG0_ERROR("DL SDU length is 0");
  }

  if(SUCCESS == status)
  {
    /* Save all DCCH and CCCH commands here. Cell update confirm is saved in CU */
    if((cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_CCCH_Message_PDU) ||
       (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_DCCH_Message_PDU))
    {
      /* Don't buffer the CU confirm OTA received on CCCH now as we may get for other UEs also */
      if(!((*procedure == RRC_PROCEDURE_CU) && (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_CCCH_Message_PDU)))
      {
        sdu_type = (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_DCCH_Message_PDU)?RRC_DCCH_DL_SDU : RRC_CCCH_DL_SDU;
        if(sdu_type == RRC_CCCH_DL_SDU)
        {
          message_choice = rrc_get_dl_ccch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
      }
        else
        {
          message_choice = rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
        } 
        rrc_crash_debug_save_ota(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu, cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length,sdu_type,
                                  message_choice,*procedure);
      }
    }
  }


  return(status);
} /* end function rrc_perform_integrity_protection_check */

/*===========================================================================

FUNCTION    rrc_get_dl_asn1_message_type

DESCRIPTION
  This function determines the type of the message received and returns this
  information as a byte suitable for reporting in a QXDM event.  This value
  is *not* the same as the ASN.1 based message type values returned by other
  similar functions.  See the rrclogging.h file for details.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns the message type if successful. Otherwise it returns 0xff.
  
SIDE EFFECTS
  None

===========================================================================*/
 uint8 rrc_get_dl_asn1_message_type(
  rrc_cmd_type              *cmd_ptr /* To hold received command */
)
{
  uint8 asn1_msg_type=1;

  switch(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type)
  {
    case rrc_BCCH_FACH_Message_PDU:
      asn1_msg_type = T_rrc_BCCH_FACH_MessageType_systemInformationChangeIndication;
      break;

    case rrc_BCCH_BCH_Message_PDU:
      asn1_msg_type = T_rrc_BCCH_FACH_MessageType_systemInformation;
      break;

    case rrc_DL_DCCH_Message_PDU:
      asn1_msg_type = 
        rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                     cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
      break;

    case rrc_DL_CCCH_Message_PDU:

      asn1_msg_type = 
        rrc_get_dl_ccch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                     cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
      break;

    case rrc_PCCH_Message_PDU:
      asn1_msg_type = T_rrc_PCCH_MessageType_pagingType1;
      break;

    default:
      WRRC_MSG1_HIGH("The SDU type %d is not supported",
                cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
      break;
  }

  return (asn1_msg_type-1);
} /* end function rrc_get_asn1_dl_message_type */


/*===========================================================================

FUNCTION    rrc_get_dl_message_type

DESCRIPTION
  This function determines the type of the message received and returns this
  information as a byte suitable for reporting in a QXDM event.  This value
  is *not* the same as the ASN.1 based message type values returned by other
  similar functions.  See the rrclogging.h file for details.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns the message type if successful. Otherwise it returns 0xff.
  
SIDE EFFECTS
  None

===========================================================================*/
static uint8 rrc_get_dl_message_type(
  rrc_cmd_type              *cmd_ptr /* To hold received command */
)
{
  uint8 asn1_msg_type;
  uint8 msg_type = 0xff;

  switch(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type)
  {
    case rrc_BCCH_FACH_Message_PDU:
      msg_type = RRCLOG_SYSTEM_INFORMATION_CHANGE_INDICATION_MSG;
      break;

    case rrc_BCCH_BCH_Message_PDU:
      msg_type = RRCLOG_SYSTEM_INFORMATION_MSG;
      break;

    case rrc_DL_DCCH_Message_PDU:
      asn1_msg_type = 
        rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                     cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
      switch(asn1_msg_type)
      {
        /* The following needs to be updated as and when new RRC
           procedures are implemented */
        case T_rrc_DL_DCCH_MessageType_downlinkDirectTransfer:
          msg_type = RRCLOG_DOWNLINK_DIRECT_TRANSFER_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_rrcConnectionRelease:
          msg_type = RRCLOG_RRC_CONNECTION_RELEASE_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_signallingConnectionRelease:
          msg_type = RRCLOG_SIGNALLING_CONNECTION_RELEASE_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_radioBearerSetup:
          msg_type = RRCLOG_RADIO_BEARER_SETUP_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_radioBearerReconfiguration:
          msg_type = RRCLOG_RADIO_BEARER_RECONFIGURATION_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_transportChannelReconfiguration:
          msg_type = RRCLOG_TRANSPORT_CHANNEL_RECONFIGURATION_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_physicalChannelReconfiguration:
          msg_type = RRCLOG_PHYSICAL_CHANNEL_RECONFIGURATION_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_radioBearerRelease:
          msg_type = RRCLOG_RADIO_BEARER_RELEASE_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_ueCapabilityEnquiry:
          msg_type = RRCLOG_UE_CAPABILITY_ENQUIRY_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_ueCapabilityInformationConfirm:
          msg_type = RRCLOG_UE_CAPABILITY_INFORMATION_CONFIRM_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_activeSetUpdate:
          msg_type = RRCLOG_ACTIVE_SET_UPDATE_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_measurementControl:
          msg_type = RRCLOG_MEASUREMENT_CONTROL_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_securityModeCommand:
          msg_type = RRCLOG_SECURITY_MODE_COMMAND_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_pagingType2:
          msg_type = RRCLOG_PAGING_TYPE_2_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_utranMobilityInformation:
          msg_type = RRCLOG_UTRAN_MOBILITY_INFORMATION_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_cellUpdateConfirm:
          msg_type = RRCLOG_CELL_UPDATE_CONFIRM_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_uraUpdateConfirm:
          msg_type = RRCLOG_URA_UPDATE_CONFIRM_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_cellChangeOrderFromUTRAN:
          msg_type = RRCLOG_CELL_CHANGE_ORDER_FROM_UTRAN_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_handoverFromUTRANCommand_GSM:
          msg_type = RRCLOG_HANDOVER_FROM_UTRAN_COMMAND_MSG;
          break;
        case T_rrc_DL_DCCH_MessageType_counterCheck:
          msg_type = RRCLOG_COUNTER_CHECK_MSG;
          break;

#ifdef FEATURE_WCDMA_ETWS
        case T_rrc_DL_DCCH_MessageType_etwsPrimaryNotificationWithSecurity:
          msg_type = RRCLOG_ETWS_PRIMARY_NOTIFICATION_WITH_SECURITY_MSG;
          break;
#endif
        default:
          WRRC_MSG1_HIGH("The msg type %d is not supported",
                    msg_type );
          break;
      }  
      break;

    case rrc_DL_CCCH_Message_PDU:

      asn1_msg_type = 
        rrc_get_dl_ccch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                     cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
      switch(asn1_msg_type)
      {
        /* The following needs to be updated as and when new RRC
           procedures are implemented */
        case T_rrc_DL_CCCH_MessageType_rrcConnectionReject:
          msg_type = RRCLOG_RRC_CONNECTION_REJECT_MSG;
          break;
        case T_rrc_DL_CCCH_MessageType_rrcConnectionRelease:
          msg_type = RRCLOG_RRC_CONNECTION_RELEASE_MSG;
          break;
        case T_rrc_DL_CCCH_MessageType_rrcConnectionSetup:
          msg_type = RRCLOG_RRC_CONNECTION_SETUP_MSG;
          break;
        case T_rrc_DL_CCCH_MessageType_cellUpdateConfirm:
          msg_type = RRCLOG_CELL_UPDATE_CONFIRM_MSG;
          break;
        case T_rrc_DL_CCCH_MessageType_uraUpdateConfirm:
          msg_type = RRCLOG_URA_UPDATE_CONFIRM_MSG;
          break;
        default:
          WRRC_MSG1_HIGH("The msg type %d is not supported",
                    msg_type);
          break;
      }
      break;

    case rrc_PCCH_Message_PDU:
      msg_type = RRCLOG_PAGING_TYPE_1_MSG;
      break;

    default:
      WRRC_MSG1_HIGH("The SDU type %d is not supported",
                cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
      break;
  }

  return msg_type;
} /* end function rrc_get_dl_message_type */

/*===========================================================================

FUNCTION    rrc_log_dl_sig_message 

DESCRIPTION
    This function will log the downlink encoded OTA messages into QXDM.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void rrc_log_dl_sig_message
(
  rrc_cmd_type *cmd_ptr                 /* To hold received command */
)
{  

#ifndef T_WINNT
  uint32 log_record_length;                 /* Local variable to store Log 
                                               Record Length */
  WCDMA_SIGNALLING_MESSAGE_type *log_record_ptr;
                                            /* Local variable to store Log
                                               record */
  rrclog_sig_msg_event_type sig_msg_event;  /* Variable for message rx event */
  uint8 rb_id;                              /* Variable for radio bearer ID */
  uint8 channel_type;                       /* Variable for channel type */

#endif /* T_WINNT */ 

  uecomdef_logchan_e_type rrclog_dl_lc;     /*Variable required for logging*/

  /* Gets the Logical Channel Type of SDU for logging purpose */
  (void) rrclcm_get_dl_lc_type(cmd_ptr->cmd.downlink_sdu_ind.lc_id,
                               &rrclog_dl_lc);

#ifdef T_WINNT
  #error code not present
#else  /* QXDM logging */

  /* Determine and store RB and channel type info */
  rb_id = (uint8)rrclcm_get_dl_rb_id(cmd_ptr->cmd.downlink_sdu_ind.lc_id);
  switch(rrclog_dl_lc)
  {
    case UE_LOGCHAN_BCCH:         /* Broadcast Control Channel */
      switch(rb_id)
      {
        case BCCH_S_RADIO_BEARER_ID:
        case BCCH_N_RADIO_BEARER_ID:
          channel_type = RRCLOG_SIG_DL_BCCH_BCH;
          break;

        case BCCH_FACH_RADIO_BEARER_ID:
          channel_type = RRCLOG_SIG_DL_BCCH_FACH;
          break;

        default:
          WRRC_MSG1_ERROR("Invalid RB Id: %d for BCCH Log Channel",
               rb_id);
          channel_type = 0xFF;
          break;
      }
      break;

    case UE_LOGCHAN_PCCH:         /* Paging Control Channel.*/
      channel_type = RRCLOG_SIG_DL_PCCH;
      break;

    case UE_LOGCHAN_CCCH:         /* Common Control Channel.*/
      channel_type = RRCLOG_SIG_DL_CCCH;
      break;

    case UE_LOGCHAN_DCCH:         /* Dedicated Control Channel.*/
      channel_type = RRCLOG_SIG_DL_DCCH;
      break;

    default:
      channel_type = 0xFF;
      WRRC_MSG1_ERROR("Sig Msg rcvd on invalid Log Chan Type %d",
           rrclog_dl_lc);
      break;
  }

  /* Construct and send an event. */
  sig_msg_event.channel_type = channel_type;
  sig_msg_event.message_type = rrc_get_dl_message_type(cmd_ptr);
  event_report_payload(EVENT_RRC_MESSAGE_RECEIVED, sizeof(sig_msg_event),
                       (void *)&sig_msg_event);

  /*Calculate the record length */
  log_record_length = 
    FPOS(WCDMA_SIGNALLING_MESSAGE_type, signalling_message) + 
    cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length;

  /*Allocate memory for Log Record. */
  log_record_ptr = (WCDMA_SIGNALLING_MESSAGE_type *)log_alloc( WCDMA_SIGNALLING_MESSAGE, log_record_length);
  
  /* Make sure memory allocation is successful before to proceed with 
     logging */
  if(log_record_ptr != NULL)
  {
    /*Get the Logical Channel Type from LCM */
    (void) rrclcm_get_dl_lc_type(cmd_ptr->cmd.downlink_sdu_ind.lc_id,
                                 &rrclog_dl_lc);
    /* Update RB Id.*/
    log_record_ptr->rb_id = rb_id;

    /*Update Logical Channel Type */
    log_record_ptr->channel_type = channel_type;

    /* Update length of the signalling message */
    log_record_ptr->length = 
      (uint16) cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length;

    /* Copy the Signalling mesaage payload to the log record */
    WCDMA_MEMCPY((void *)log_record_ptr->signalling_message, 
                 (log_record_length - FPOS(WCDMA_SIGNALLING_MESSAGE_type, signalling_message)),
                 cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                 cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

    /*commit the log record */
    log_commit(log_record_ptr);
  }
  else
  {
    /* The QXDM is not turn ON. Hence Do not log the OTA messages */
  }

#endif /*T_WINNT */

} /* end function rrc_log_dl_sig_message */

/*===========================================================================

FUNCTION    rrc_log_dl_sig_message_for_ext_sibs

DESCRIPTION
    This function will log the downlink encoded OTA messages into QXDM for extension sibS.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrc_log_dl_sig_message_for_ext_sibs
(
  rrc_SIB_type sib_type,
  sib_events_index_e_type index
)
{  

#ifndef T_WINNT
  uint32 log_record_length;                 /* Local variable to store Log 
                                               Record Length */
  WCDMA_SIGNALLING_MESSAGE_type *log_record_ptr;
                                            /* Local variable to store Log
                                               record */
  rrclog_sig_msg_event_type sig_msg_event;  /* Variable for message rx event */  
#endif /* T_WINNT */ 
  uint8 rb_id;                              /* Variable for radio bearer ID */

  uecomdef_logchan_e_type rrclog_dl_lc = UE_LOGCHAN_BCCH;     /*Variable required for logging*/
  uint32 dl_sdu_len;
  uint8 msg_type = 0xff;
  
  switch(sib_type)
  {
    case rrc_SIB11bis:         /* Broadcast Control Channel */
      msg_type = RRCLOG_SIB11bis_MSG;
      break;

    case rrc_SIB19:         /* Paging Control Channel.*/
      msg_type = RRCLOG_SIB19_MSG;
      break;

    default:
      WRRC_MSG1_ERROR("Sig Msg rcvd on invalid Log msg type %d",
           rrclog_dl_lc);
      return;
  }

  if(sib_events_ptr->event[index].for_serving_cell == TRUE)
  {
    rb_id = BCCH_S_RADIO_BEARER_ID;
  }
  else
  {
    rb_id = BCCH_N_RADIO_BEARER_ID;
  }

  dl_sdu_len = GET_PDU_LENGTH_IN_BYTES( 
    sib_events_ptr->event[index].tsib[sib_type].sib_tot_length );

#ifdef T_WINNT
  #error code not present
#else  /* QXDM logging */

  /* Construct and send an event. */
  sig_msg_event.channel_type = RRCLOG_EXTENSION_SIB;
  sig_msg_event.message_type = msg_type;
  event_report_payload(EVENT_RRC_MESSAGE_RECEIVED, sizeof(sig_msg_event),
                       (void *)&sig_msg_event);

  /*Calculate the record length */
  log_record_length = 
    FPOS(WCDMA_SIGNALLING_MESSAGE_type, signalling_message) + dl_sdu_len + 1;

  /*Allocate memory for Log Record. */
  log_record_ptr = (WCDMA_SIGNALLING_MESSAGE_type *)log_alloc( WCDMA_SIGNALLING_MESSAGE, log_record_length);
  
  /* Make sure memory allocation is successful before to proceed with 
     logging */
  if(log_record_ptr != NULL)
  {
    /* Update RB Id.*/
    log_record_ptr->rb_id = rb_id;

    /*Update Logical Channel Type */
    log_record_ptr->channel_type = RRCLOG_EXTENSION_SIB;

    /* Update length of the signalling message */
    log_record_ptr->length = dl_sdu_len;

    log_record_ptr->signalling_message[0] = msg_type;

    /* Copy the Signalling mesaage payload to the log record */
    WCDMA_MEMCPY((void *)&(log_record_ptr->signalling_message[1]), 
                 (log_record_length - FPOS(WCDMA_SIGNALLING_MESSAGE_type, signalling_message) - 1),
                 (void *)(sib_events_ptr->event[index].tsib[sib_type].sib_data_ptr),
                 log_record_ptr->length);

    log_record_ptr->length += 1;

    /*commit the log record */
    log_commit(log_record_ptr);    
  }
  else
  {
    /* The QXDM is not turn ON. Hence Do not log the OTA messages */
  }
#endif /*T_WINNT */
} /* end function rrc_log_dl_sig_message */


/*===========================================================================

FUNCTION    RRC_DEQUEUE_DL_SDU 

DESCRIPTION
  This function gets the Logical Channel ID from RRC_DOWNLINK_SDU_IND command
  and identifies the corresponding watermark queue and dequeues the SDU from
  the watermark queue. The dequeued DSM Item is then converted into a 
  contingous stream of octects.
  
DEPENDENCIES
  None

RETURN VALUE
  rrc_sdu_dequeue_status_type

SIDE EFFECTS
  None

===========================================================================*/
static rrc_sdu_dequeue_status_type rrc_dequeue_dl_sdu(
  rrc_cmd_type *cmd_ptr                 /* To hold received command */
)
{  
  rlc_lc_id_type lc_id;                     /* Downlink logical Channel Id */
  dsm_watermark_type *dl_wm_ptr;            /* Downlink Water Mark pointer */
  dsm_item_type *dsm_item;                  /* DSM Item */
  word dl_sdu_length;                       /* Length of the Downlink SDU
                                               in bytes */
  rrc_sdu_dequeue_status_type dequeue_status = RRC_DEQUEUE_FAILURE;
  /* This is used to hold the return value of the function. If we're
  able to sucessfully dequeue the DSM item from RLC, this value is
  changed to RRC_DEQUEUE_SUCCESS. Otherwise RRC_DEQUEUE_FAILURE is
  returned. */

  /* Checks whether received command is RRC_DOWNLINK_SDU_IND */
  if(cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_SDU_IND)
  {
    lc_id = cmd_ptr->cmd.downlink_sdu_ind.lc_id;

    MSG_LOW ("RLC ID received through RRC_DOWNLINK_SDU_IND command %d",
             lc_id,0,0);
    dl_wm_ptr = rrclcm_get_dl_watermark_ptr(lc_id);
                                           /* Gets the Watermark pointer 
                                              corresponding to Logical Channel
                                              Id. */  
    dsm_item = dsm_dequeue(dl_wm_ptr);     /* Dequeues the DSM item from the
                                              water mark queue. */
    if (dsm_item != NULL)
    { 
      dl_sdu_length = (word)dsm_length_packet(dsm_item);
      
      /* Checks whether length of SDU is  valid. */
      if ( (dl_sdu_length > MAX_SIZE_OF_SDU) ||
           (dl_sdu_length == 0) ) 
      {
        WRRC_MSG1_ERROR("Rcvd DSM Item has invalid SDU len: %d", dl_sdu_length);
      }
      else
      {
        /* Allocates the memory for the received SDU and this memory should be
           freed by the rrc_free_cmd_buf function */
        cmd_ptr->cmd.downlink_sdu_ind.dl_sdu = 
                 (uint8 *) rrc_malloc(dl_sdu_length);

        /* Converts the DSM Item into  contiguous stream of bytes */
        dl_sdu_length = dsm_pkt_to_sdu_chain_byte (&dsm_item, 
                            cmd_ptr->cmd.downlink_sdu_ind.dl_sdu);   
  
        MSG_LOW("Received SDU from DSM Queue. %LX %s %d", dl_wm_ptr,
                 cmd_ptr->cmd.downlink_sdu_ind.dl_sdu, dl_sdu_length);

        /* Copy the length into the command itself for future reference */
        cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length = dl_sdu_length;

        /* Log the Downlink signalling message */
        rrc_log_dl_sig_message(cmd_ptr);

        dequeue_status = RRC_DEQUEUE_SUCCESS;
      }
    } /* dl_sdu_length is valid */     
  }    
  return(dequeue_status);
} /* end function rrc_dequeue_dl_sdu */

#ifdef FEATURE_DUAL_SIM

/*===========================================================================

FUNCTION  RRC_CHECK_MM_CMDS_FOR_SIMID_VALIDATION

DESCRIPTION
 
  This function validates SIM id for MM commands.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  SUCCESS: Validation is successful.
  else
  FAILURE
  
SIDE EFFECTS

  None.

===========================================================================*/
uecomdef_status_e_type rrc_check_mm_cmds_for_simid_validation(rrc_cmd_type *cmd_ptr)
{
  sys_modem_as_id_e_type sim_id_recvd;
  sys_modem_as_id_e_type curr_as_id;

  if((cmd_ptr->cmd_hdr.cmd_id == RRC_SIM_INSERTED_REQ) ||
     (cmd_ptr->cmd_hdr.cmd_id == RRC_DS_STATUS_CHANGE_IND)
#ifdef FEATURE_DUAL_SIM
     || (cmd_ptr->cmd_hdr.cmd_id == RRC_DS_TUNEAWAY_STATUS_CHANGE_IND)
#endif
    )
  {
    /*If cmd id is SIM_INSERTED_REQ/DS_STATUS_CHANGE_IND/RRC_DS_TUNEAWAY_STATUS_CHANGE_IND, pass the AS ID validation*/
    return SUCCESS;
  }

  if((cmd_ptr->cmd_hdr.cmd_id == RRC_SERVICE_REQ) &&
      (cmd_ptr->cmd.rrc_service_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
  {
    rrc_is_sim_id_valid = TRUE;
    rrc_sim_id_recvd = cmd_ptr->cmd.rrc_service_req.as_id;
    WRRC_MSG0_HIGH("DSIM: RRC_SERVICE_REQ recvd with Limited N/W sel mode, use the AS ID from the service request");
  }

  /* When SIM is not present and mode pref gets changed resulting in AS ID change it needs to be updated by RRC */
  if(cmd_ptr->cmd_hdr.cmd_id == RRC_MODE_PREF_CHANGE_IND)
  {
    rrc_is_sim_id_valid = TRUE;
    rrc_sim_id_recvd = cmd_ptr->cmd.rrc_subscription_id.as_id;
    WRRC_MSG1_HIGH("DSIM: RRC_MODE_PREF_CHANGE_IND is received, use the AS ID %d from the cmd",rrc_sim_id_recvd);
    return SUCCESS;
  }

  #ifdef FEATURE_DUAL_SIM
  if(cmd_ptr->cmd_hdr.cmd_id == RRC_MULTIMODE_SUBS_CHGD_IND)
  {
    WRRC_MSG0_HIGH("RRC_MULTIMODE_SUBS_CHGD_IND Recvd");
    rrc_is_sim_id_valid = TRUE;

    /* Store the current as_id */
    curr_as_id = rrc_sim_id_recvd;
   
    if(cmd_ptr->cmd.rrc_multimode_subs_chgd_ind.subs_capability[SYS_MODEM_AS_ID_1] == SUBS_CAPABILITY_MULTIMODE)
    {
      rrc_sim_id_recvd = SYS_MODEM_AS_ID_1;
    }
    else if(cmd_ptr->cmd.rrc_multimode_subs_chgd_ind.subs_capability[SYS_MODEM_AS_ID_2] == SUBS_CAPABILITY_MULTIMODE)
    {
      rrc_sim_id_recvd = SYS_MODEM_AS_ID_2;
    }
#ifdef FEATURE_TRIPLE_SIM
    else
    {
      rrc_sim_id_recvd = SYS_MODEM_AS_ID_3;
    }
#endif
    WRRC_MSG1_HIGH("AS-ID Changed to %d",rrc_sim_id_recvd);

    /* Re-read NVs only if the subscription has changed */
    if(curr_as_id != rrc_sim_id_recvd)
    {
      WRRC_MSG0_HIGH("AS ID changed, re-read the NVs");    
      rrc_read_all_nv_items(rrc_sim_id_recvd);
#ifdef FEATURE_QSH_EVENT_METRIC
#ifndef FEATURE_DUAL_WCDMA
      rrc_qsh_update_metric_database(rrc_sim_id_recvd);
#endif
#endif
    }	  

    return SUCCESS;
  }
#endif
  if(!rrc_is_sim_id_valid)
  {
    WRRC_MSG0_HIGH("DSIM: RRC not yet updated with SIM details, Treat as SIM Match");
    return SUCCESS;
  }
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_SERVICE_REQ:
      sim_id_recvd = cmd_ptr->cmd.rrc_service_req.as_id;
      break;

    case RRC_STOP_WCDMA_MODE_REQ:
      sim_id_recvd = cmd_ptr->cmd.stop_wcdma_mode_req.as_id ;
      break;

    case RRC_DEACT_REQ:
    case RRC_MODE_CHANGE_REQ:
      sim_id_recvd = cmd_ptr->cmd.deact_req.as_id;
      break;

    case RRC_CHANGE_MODE_IND:
      sim_id_recvd = cmd_ptr->cmd.rrc_change_mode_ind.as_id;
      break;

    case RRC_FORBIDDEN_LAI_LIST_UPDATE_REQ:
      sim_id_recvd = cmd_ptr->cmd.forbid_lai_list_update_req.as_id;

      break;

    case RRC_PLMN_LIST_REQ:
      sim_id_recvd = cmd_ptr->cmd.plmn_list_req.as_id;
      break;

    case RRC_EST_REQ:
      sim_id_recvd = cmd_ptr->cmd.rrc_est_req.as_id;
      break;
    
    case RRC_ABORT_REQ:
      sim_id_recvd = cmd_ptr->cmd.rrc_abort_req.as_id ;
      break;

    case RRC_OPEN_SESSION_REQ:
      sim_id_recvd = cmd_ptr->cmd.open_sess_req.as_id;
      break;
    
    case RRC_DATA_REQ:
      sim_id_recvd = cmd_ptr->cmd.data_req.as_id;
      break;
    
    case RRC_CLOSE_SESSION_REQ:
      sim_id_recvd = cmd_ptr->cmd.close_sess_req.as_id ;
      break;

#ifdef FEATURE_INTERRAT_CELL_RESELECTION_GTOW
    case RRC_ACTIVATION_RSP:
      sim_id_recvd = cmd_ptr->cmd.activation_rsp.as_id;
      break;
#endif

    case RRC_NW_SEL_MODE_RESET_IND:
      sim_id_recvd = cmd_ptr->cmd.rrc_nw_sel_mode_reset_ind.as_id;
      break;

    case RRC_INVALIDATE_SIM_DATA_REQ:
      rrc_is_sim_id_valid = FALSE;
      return SUCCESS;

    case RRC_SIM_UPDATE_REQ:
      sim_id_recvd = cmd_ptr->cmd.rrc_sim_update_req.as_id;
      break;

    case RRC_FAST_DORMANCY_REQ:
      sim_id_recvd = cmd_ptr->cmd.rrc_fast_dormancy_req.as_id;
      break;

/* WCDMA FF feature is removed here as NAS has FF enabled and we can get below FF commands 
 * This is to avoid crash due to SIM ID mismatch 
 */
    case RRC_LAI_REJECT_LIST_IND:
      sim_id_recvd = cmd_ptr->cmd.rrc_lai_reject_list_ind.as_id;
      break;

    case RRC_TUNEAWAY_ALLOWED_IND:
      sim_id_recvd = cmd_ptr->cmd.rrc_sm_tuneaway_allowed_ind.as_id;
      break;

#ifdef FEATURE_3GPP_CSFB
    case RRC_CSFB_CALL_STATUS_IND:
      sim_id_recvd = cmd_ptr->cmd.rrc_csfb_call_status_ind.as_id;
      break;
#endif
   
#ifdef FEATURE_LTE_TO_WCDMA
    case RRC_LTE_RESELECTION_ALLOWED_STATUS_IND:
      sim_id_recvd = cmd_ptr->cmd.rrc_lte_resel_allowed_status_ind.as_id;
      break;
#endif

    case RRC_SET_DRX_IND:
      sim_id_recvd = cmd_ptr->cmd.rrc_set_drx_ind.as_id;
      break;

#ifdef FEATURE_FEMTO_CSG
    case RRC_CSG_WHITE_LIST_UPDATE_IND:
      sim_id_recvd = cmd_ptr->cmd.white_list.as_id;
      break;
#endif

    default :
      sim_id_recvd = cmd_ptr->cmd.rrc_subscription_id.as_id;
      break;
  }/* end switch */

  if(rrc_sim_id_recvd == sim_id_recvd)
  {
    return SUCCESS;
  }
  return FAILURE;
}

/*===========================================================================

FUNCTION  RRC_CHECK_RR_CMDS_FOR_SIMID_VALIDATION

DESCRIPTION
 
  This function validates SIM id for RR commands.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_check_rr_cmds_for_simid_validation(rrc_rr_cmd_type *cmd_ptr)
{
  sys_modem_as_id_e_type sim_id_recvd;

  /* We can use any interface structure to get the AS ID as it will always be the top element
   * of every structure
   */
  sim_id_recvd = cmd_ptr->cmd.interrat_reselection_req.as_id;

  if(!rrc_is_sim_id_valid)
  {
    WRRC_MSG0_HIGH("DSIM: RRC not yet updated with SIM details, Treat as SIM Match");
    return;
  }

  if(rrc_sim_id_recvd != sim_id_recvd)
  {
    WRRC_MSG2_ERROR("INvalid sub-id %d passed. internal %d",sim_id_recvd,rrc_sim_id_recvd);
    return;
  }
  return;
}

#endif
/*===========================================================================

FUNCTION  RRC_DOG_REPORT_AND_CLEAR_SIG

DESCRIPTION
 
  This function clears sig and reports to dog.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_dog_report_and_clear_sig(void)
{
  (void) rex_clr_sigs( rex_self(), RRC_RPT_TIMER_SIG );
  dog_hb_report(DOG_RRC_RPT);
}

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION  RRC_DISPATCH_L1_FF_COMMANDS_DURING_WAIT_LOCK

DESCRIPTION
 
  This function dispatches all commands received in RRC's free floating
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_l1_ff_commands_during_wait_lock( void )
{
  rrc_cmd_type *cmd_ptr;      /* To hold received command */

  while((cmd_ptr = q_get(&rrc_l1_ff_cmd_q)) != NULL)
  {
    WRRC_MSG1_HIGH_OPT("Dispatching RRC Ext L1FF cmd rrc_cmd_e_type_value_0x%x",cmd_ptr->cmd_hdr.cmd_id);
    switch(cmd_ptr->cmd_hdr.cmd_id)
    {
      case RRC_CPHY_CHECK_SLEEP_REQ:
        rrcff_fail_l1_sleep_req(cmd_ptr);
        break;
      default:
        ERR_FATAL("FF: Unexpected Message %d",cmd_ptr->cmd_hdr.cmd_id,0,0);
        break;
    }
    rrc_free_cmd_buf( cmd_ptr );
    cmd_ptr = NULL;
  }
}
#endif

/*===========================================================================

FUNCTION  RRC_DISPATCH_L1_FF_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's free floating
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_l1_ff_commands( void )
{
  rrc_cmd_type *cmd_ptr;      /* To hold received command */

  while((cmd_ptr = q_get(&rrc_l1_ff_cmd_q)) != NULL)
  {
    WRRC_MSG1_HIGH_OPT("Dispatching RRC Ext L1FF cmd rrc_cmd_e_type_value_0x%x",cmd_ptr->cmd_hdr.cmd_id);
    switch(cmd_ptr->cmd_hdr.cmd_id)
    {
      case RRC_CPHY_CHECK_SLEEP_REQ:
        rrcff_check_l1_sleep_req(cmd_ptr);
        break;
      default:
        ERR_FATAL("FF: Unexpected Message %d",cmd_ptr->cmd_hdr.cmd_id,0,0);
        break;
    }
    rrc_free_cmd_buf( cmd_ptr );
    cmd_ptr = NULL;
  }
}
#endif
/*===========================================================================

FUNCTION  RRC_DISPATCH_INTERNAL_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's internal
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_internal_commands( void )
{
  
  rrc_cmd_type *cmd_ptr;      /* To hold received command */
  rrc_proc_e_type rrc_proc_name; /* To hold procedure name */
  uint32 count;               /* Local Counter             */
  rrc_proc_e_type proc_waiting_for_l2_ack;
  /* We need to dequeue items from the internal
  command queue and process them till the queue is
  empty */

  MSG_LOW("Dispatching RRC Internal Commands",0,0,0);
  while( (cmd_ptr = q_get(&rrc_int_cmd_q)) != NULL)
  {
    if (RRC_DOWNLINK_SDU_IND != cmd_ptr->cmd_hdr.cmd_id)
    {
      WRRC_MSG1_HIGH_OPT("Dispatching RRC int cmd rrc_cmd_e_type_value_0x%x",cmd_ptr->cmd_hdr.cmd_id);
    }
#ifdef FEATURE_DUAL_SIM
    if(rrcwrm_is_process_required(cmd_ptr) == FALSE)
    {
      WRRC_MSG1_HIGH("DSIM_TUNEAWAY: CMD %d is Saved !",cmd_ptr->cmd_hdr.cmd_id);
      continue;
    }
#endif
    /* Check if the incoming internal command is required a further process */
    if (rrcmcm_is_process_required(cmd_ptr) == FALSE) 
    {
      /* Display a XDM message and disgard the command */
      WRRC_MSG1_HIGH("CMD: %d is disgarded!",cmd_ptr->cmd_hdr.cmd_id);
    }
    else if(rrc_deact_status != RRC_DEACT_COMPLETED)
    {

       /* Save the internal commands in rrc_crash_debug_info */
      rrc_crash_debug_save_internal_cmds(cmd_ptr);
#ifdef FEATURE_QSH_EVENT_METRIC
      wrrc_qsh_log_int_cmd_params(cmd_ptr);
#endif
       
      /* Switch on the command id of the received command, and
       * call the appropriate function to process each command. 
       */
      switch( cmd_ptr->cmd_hdr.cmd_id )
      {
        case RRC_DOWNLINK_SDU_IND:
          /* Dequeues the SDU from the corrosponding watermark
          queue. This function dynamically allocates memory for
          DL SDU, the dispatcher should free this memory before freeing
          the command. */
          if (RRC_DEQUEUE_SUCCESS == rrc_dequeue_dl_sdu (cmd_ptr))
          {
#ifdef FEATURE_QSH_EVENT_METRIC
            rrc_qsh_dl_committed = FALSE;
#endif
            /* This function dynamically allocates memory for ASN1 decoded 
            message, the dispatcher should use a special function provided
            by the OSS compiler to free the memory before it frees the the 
            command */        

            if(FAILURE == rrc_perform_integrity_protection_check(cmd_ptr,
                            &rrc_proc_name) )
            {
#ifdef FEATURE_QSH_EVENT_METRIC
              rrc_qsh_log_dl_ota_msg_id(cmd_ptr, rrc_proc_name);
              if((rrc_proc_name == RRC_PROCEDURE_ASU) || (rrc_proc_name == RRC_PROCEDURE_MCMR))
              {
                if(rrc_qsh_metric_db[WRRC_QSH_METRIC_MEAS_ASU_OTA].data_ptr != NULL)
                {
                  rrc_qsh_commit_data(WRRC_QSH_METRIC_MEAS_ASU_OTA, QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL);
                }
              }
              else if(rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr != NULL)
              {
                rrc_qsh_commit_data(WRRC_QSH_METRIC_OTA, QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL);
              }
              rrc_qsh_dl_committed = TRUE;
#endif
              MSG_MED("Integrity Check failed or Message not processed",0,0,0);
            }
            else
            {
              /* Allow only NAS,BCCH messages, CUCnf and RCR if RCR is active */
              if ((rrc_proc_name != RRC_PROCEDURE_DDT) && 
                  (rrc_proc_name != RRC_PROCEDURE_SIB) &&
                  (rrc_proc_name != RRC_PROCEDURE_CU) &&
                  (rrc_proc_name != RRC_PROCEDURE_RCR))
              {
                if (rrcrcr_is_proc_active() == TRUE)
                {
                  WRRC_MSG1_HIGH("Ignore DLMsg for %d",rrc_proc_name);
                  break;
                }
              }
              /* RRC should ingore all OTA messages except NAS and SIB if connection release/decticaion is already started  */
              if((rrc_proc_name != RRC_PROCEDURE_DDT) && 
                  (rrc_proc_name != RRC_PROCEDURE_SIB))
              {
                if(TRUE == rrcllc_curr_cmd_rel_all_req())
                {   
                  WRRC_MSG1_HIGH("Ignore DLMsg for Proc rrc_proc_e_type_value%d as connection release is in progress",rrc_proc_name);
                  break;
                }
              }
              /* Then call the procedure that needs to process this
              command */
              switch (rrc_proc_name)
              {
                case RRC_PROCEDURE_SIB:
                  rrc_sib_event_controller( cmd_ptr );
                  break;

                case RRC_PROCEDURE_CSP:
                  rrc_cell_selection_procedure( cmd_ptr );
                  break;

                case RRC_PROCEDURE_RCE:
                  rrcrce_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_RCR:
                  rrcrcr_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_DDT:
                  rrcddt_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_SCR:
                  rrcscr_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_RBE:
                  rrcrbe_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_RBRC:
                  rrcrbrc_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_TCR:
                  rrctcrc_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_TFC:
                  rrctfc_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_PCR:
                  rrcpcrc_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_RBR:
                  rrcrbr_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_PG1:
                  rrcpg1_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_UECE:
                  rrcuece_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_UECI:
                  rrcueci_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_ASU:
                  rrcasu_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_SMC:
                  rrcsmc_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_MCMR:
                  rrcmeas_procedure_event_handler( cmd_ptr );
                  break;               

                case RRC_PROCEDURE_PG2:
                  rrcpg2_procedure_event_handler( cmd_ptr );
                  break;               

                case RRC_PROCEDURE_UMI:
                  rrcumi_procedure_event_handler( cmd_ptr );
                  break; 
  
                case RRC_PROCEDURE_CU:
                  rrccu_procedure_event_handler( cmd_ptr );
                  break;            

                case RRC_PROCEDURE_IHO:
                  rrciho_event_handler( cmd_ptr );
                  break;
                case RRC_PROCEDURE_CHO:
                  rrccho_procedure_event_handler( cmd_ptr );
                  break;

                case RRC_PROCEDURE_CHK:
                  rrcchk_procedure_event_handler( cmd_ptr );
                  break;  

#ifdef FEATURE_WCDMA_ETWS
                case RRC_PROCEDURE_BMC:
                  rrcbmc_procedure_event_handler( cmd_ptr );
                  break;
#endif


                case RRC_PROCEDURE_NONE:
                  WRRC_MSG0_HIGH("Failed to identify the Msg type ");
                  rrcmisc_procedure_event_handler( cmd_ptr );
                  break;

                default:
                  WRRC_MSG1_ERROR("This proc is not implemented: rrc_proc_e_type_value%d",
                                     rrc_proc_name);
                  break;

              }/*switch stmt*/
#ifdef FEATURE_QSH_EVENT_METRIC
              if(rrc_proc_name != RRC_PROCEDURE_SIB)
              { 
                rrc_qsh_log_dl_ota_msg_id(cmd_ptr, rrc_proc_name);
                /* Prevent calling below api for connection reject and ho from utran */
                if(FALSE == rrc_qsh_prevent_dl_ota_param_log(cmd_ptr))
                {
                  rrc_qsh_log_ota_params(rrc_proc_name);
                }
                if((rrc_proc_name == RRC_PROCEDURE_ASU) || (rrc_proc_name == RRC_PROCEDURE_MCMR))
                {
                  if(rrc_qsh_metric_db[WRRC_QSH_METRIC_MEAS_ASU_OTA].data_ptr != NULL)
                  {
                    rrc_qsh_commit_data(WRRC_QSH_METRIC_MEAS_ASU_OTA, QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL);
                  }
                }
                else if(rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr != NULL)
                {
                  rrc_qsh_commit_data(WRRC_QSH_METRIC_OTA, QSH_CLIENT_METRIC_LOG_REASON_EVENT_INTERNAL);
                }
              }
              rrc_qsh_dl_committed = TRUE;
              rrc_qsh_log_ul_failure_ota();
#endif
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
              rrcdata_buffer_ota_during_activation_time(cmd_ptr, rrc_proc_name );
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
            }/*else stmt for integrity check*/
          }/*if for dequeue msg pdu*/
          else
          {
            /* Unable to dequeue the message PDU from the DSM queue.
              Indicate error. */
            WRRC_MSG0_ERROR("Unable to dequeue msg PDU from queue");
          }
          break;       

        case RRC_GET_SPECIFIC_SIBS_REQ:
          rrc_sib_event_controller( cmd_ptr );
          break;

        case RRC_UPDATE_SIBS_IND:
          rrc_sib_event_controller( cmd_ptr );
          break;

        case RRC_GET_SPECIFIC_SIBS_CNF:
          switch ( cmd_ptr->cmd.get_sibs_cnf.procedure )
          {
            case RRC_PROCEDURE_CSP:
              rrc_cell_selection_procedure( cmd_ptr );
              break;

            case RRC_PROCEDURE_CCM:
              rrc_cell_change_manager( cmd_ptr );
              break;

           
           case RRC_PROCEDURE_GPS:
              rrcgps_procedure_event_handler( cmd_ptr );
              break;
           

            default:
              WRRC_MSG1_ERROR("Unknown Proc 0x%x for GET_SPECIFIC_SIBS_CNF",
                  (cmd_ptr->cmd.get_sibs_cnf.procedure));
              break;
          }
          break;

        case RRC_NEW_STATE_IND:
          rrcscmgr_event_handler( cmd_ptr );
          break;

        case RRC_STATE_CHANGE_IND:
          /* A state change indication command can go to
          multiple procedures. Loop the required number
          of times and send the command to each procedure. */
          for( count=0; 
               count < cmd_ptr->cmd.state_change_ind.num_procedures;
               count++
             )
          {
            switch ( cmd_ptr->cmd.state_change_ind.procedure[count] )
            {
              case RRC_PROCEDURE_SIB:
                rrc_sib_event_controller( cmd_ptr );
                break;
  
              case RRC_PROCEDURE_RCE:
                rrcrce_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_CSP:
                rrc_cell_selection_procedure( cmd_ptr );
                break;
   
              case RRC_PROCEDURE_RCR:
                rrcrcr_procedure_event_handler( cmd_ptr );
                break;
    
              case RRC_PROCEDURE_RBE:
                rrcrbe_procedure_event_handler( cmd_ptr );
                break;
     
              case RRC_PROCEDURE_RBR:
                rrcrbr_procedure_event_handler( cmd_ptr );
                break;
  
              case RRC_PROCEDURE_SCRR:
                rrcscrr_procedure_event_handler( cmd_ptr );
                break;
  
              case RRC_PROCEDURE_IDT:
                rrcidt_procedure_event_handler( cmd_ptr );
                break;
   
              case RRC_PROCEDURE_UDT:
                rrcudt_procedure_event_handler( cmd_ptr );
                break;
    
              case RRC_PROCEDURE_UECI:
                rrcueci_procedure_event_handler( cmd_ptr );
                break;
     
              case RRC_PROCEDURE_MCMR:
                rrcmeas_procedure_event_handler( cmd_ptr );
                break;
  
              case RRC_PROCEDURE_ASU:
                rrcasu_procedure_event_handler( cmd_ptr );
                break;
          
              case RRC_PROCEDURE_LBT:
                rrclbt_procedure_event_handler( cmd_ptr );
                break;
   
              case RRC_PROCEDURE_CU:
                rrccu_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_SMC:
                rrcsmc_procedure_event_handler( cmd_ptr );
                break;
  
              case RRC_PROCEDURE_UMI:
                rrcumi_procedure_event_handler( cmd_ptr );
                break;
   
              case RRC_PROCEDURE_RBRC:
                rrcrbrc_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_TCR:
                rrctcrc_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_PCR:
                rrcpcrc_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_CCM:
                rrc_cell_change_manager( cmd_ptr );
                break;

              case RRC_PROCEDURE_CHO:
                rrccho_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_IHO:
                rrciho_event_handler( cmd_ptr );
                break;
              case RRC_PROCEDURE_DDT:
                rrcddt_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_BMC:
                rrcbmc_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_CHK:
                rrcchk_procedure_event_handler( cmd_ptr );
                break;  
              /* Other procedures should be checked here as needed. */
  
#ifdef FEATURE_INACTIVITY_HANDLING
                case RRC_PROCEDURE_MISC:
                  rrcmisc_procedure_event_handler( cmd_ptr );
                  break;
#else
              case RRC_PROCEDURE_MISC:
                rrcmisc_procedure_event_handler( cmd_ptr );
                break;
                
#endif /* FEATURE_INTACTIVITY_HANDLING */


              case RRC_PROCEDURE_GPS:
                rrcgps_procedure_event_handler(cmd_ptr);
                break;


#ifdef FEATURE_WCDMA_REL7_SPECRS 
              case RRC_PROCEDURE_COUEC:
                rrccouec_procedure_event_handler(cmd_ptr);
                break;
#endif /*FEATURE_WCDMA_REL7_SPECRS*/
              case RRC_PROCEDURE_TFC:
                rrctfc_procedure_event_handler( cmd_ptr );
                break;  
                
              case RRC_PROCEDURE_DORMANCY:
                rrcdormancy_procedure_event_handler( cmd_ptr );
                break;  
               default:
                 WRRC_MSG1_ERROR("Could not id proc: rrc_proc_e_type_value%d for State Chg Ind",
                 ( cmd_ptr->cmd.state_change_ind.procedure[count] ));
                 break;

            }
          }/* end for */
//vn_fix_9
          /* Now send a State Change Indication to RRCLLC */
          rrcllc_process_cmd( cmd_ptr );
// end_vn_fix_9
          break;

        case RRC_SIB_CHANGE_IND:
          /* A state change indication command contains the name
          of the procedure for which the command is intended.
          So switch on the procedure name and call the appropriate
          function */
          switch ( cmd_ptr->cmd.sib_change_ind.procedure )
          {
            case RRC_PROCEDURE_SIB:
                rrc_sib_event_controller( cmd_ptr );
                break;

            case RRC_PROCEDURE_CSP:
                rrc_cell_selection_procedure( cmd_ptr );
                break;

            case RRC_PROCEDURE_RCE:
                rrcrce_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_RCR:
                rrcrcr_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_RBE:
                rrcrbe_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_RBR:
                rrcrbr_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_SCRR:
                rrcscrr_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_IDT:
                rrcidt_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_UDT:
                rrcudt_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_UECI:
                rrcueci_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_MCMR:
                rrcmeas_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_ASU:
                rrcasu_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_CU:
              rrccu_procedure_event_handler( cmd_ptr );
              break;

            /* Other procedures should be checked here as needed. */

            default:
                WRRC_MSG1_ERROR("Could not id Proc: rrc_proc_e_type_value%d for SIB Chg Ind",
                         ( cmd_ptr->cmd.sib_change_ind.procedure ));
                break;

          }
          break;

        case RRC_CELL_CHANGE_REQ:
          rrc_cell_change_manager( cmd_ptr );
          break;

        case RRC_CELL_CHANGE_CNF:
          /* Rout the Cell Change CNF to the procedure ID indicated in cmd data */
          switch(cmd_ptr->cmd.cell_change_cnf.req_proc_id)
          {
            case RRC_PROCEDURE_CSP:
              rrc_cell_selection_procedure( cmd_ptr );
              break;

            /* Add more procedures (that send Cell Change Req.) here */
            default:
              WRRC_MSG1_ERROR("proc rrc_proc_e_type_value%d not found for cell chg cnf",
                                       cmd_ptr->cmd.cell_change_cnf.req_proc_id);
              break;
          }
          break;

        case RRC_NEW_CELL_IND:
          /* Call a CCM function to propagate New Cell Ind to all procedures
           * that need it. */
          rrc_ccm_propagate_new_cell_to_all_procs( cmd_ptr );
          break;

        case RRC_CHANNEL_CONFIG_REQ:
          MSG_LOW("Dispatching RRC_CHANNEL_CONFIG_REQ to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CHANNEL_CONFIG_CNF:
          /* Call the procedure that needs to process this command */
          switch (cmd_ptr->cmd.chan_config_cnf.procedure)
          {
            case RRC_PROCEDURE_SIB:
                rrc_sib_event_controller( cmd_ptr );
                break;

            case RRC_PROCEDURE_CSP:
                rrc_cell_selection_procedure( cmd_ptr );
                break;

            case RRC_PROCEDURE_CCM:
                rrc_cell_change_manager( cmd_ptr );
                break;

            case RRC_PROCEDURE_RCE:
                rrcrce_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_RCR:
                rrcrcr_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_RBE:
                rrcrbe_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_RBR:
                rrcrbr_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_ASU:
                rrcasu_procedure_event_handler( cmd_ptr );
                break;
        
            case RRC_PROCEDURE_LBT:
                rrclbt_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_CU:
                rrccu_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_RBRC:
                rrcrbrc_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_TCR:
                rrctcrc_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_PCR:
                rrcpcrc_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_CHO:
                rrccho_procedure_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_IHO:
                rrciho_event_handler( cmd_ptr );
                break;

            case RRC_PROCEDURE_NONE:
                WRRC_MSG0_HIGH("Dispatch to RRC_PROCEDURE_NONE");
                /* No procedure to be called here. Just break */
                break;

            default:
                WRRC_MSG1_ERROR("Failed to id Proc: rrc_proc_e_type_value%d for Chan Config Cnf",
                         (cmd_ptr->cmd.chan_config_cnf.procedure));
                break;
          } /* end switch */
          break;

        case RRC_RESET_L1_REQ:
          MSG_LOW("Dispatching RRC_RESET_L1_REQ to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_RESET_L1_CNF:
          WRRC_MSG0_HIGH("Dispatching RRC_RESET_L1_CNF to CSP");
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_LOWER_LAYER_DEACT_REQ:
          MSG_LOW("Dispatching LOWER_LAYER_DEACT_REQ to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          rrc_deact_status = RRC_DEACT_INITIATED;
          break;

        case RRC_LOWER_LAYER_DEACT_CNF:
          MSG_LOW("Dispatching LOWER_LAYER_DEACT_CNF to CSP",0,0,0);
          /* Write the acq. database and acq. list to NV */
          rrc_csp_write_nv_acq_info();
          wl1_clear_barred_cells();
          rrc_cell_selection_procedure( cmd_ptr );
          rrc_deact_status = RRC_DEACT_COMPLETED;
          break;

        case RRC_LLC_CMD_REQ:
          MSG_LOW("Dispatching RRC_LLC_CMD_REQ to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_LLC_CMD_CNF:
          /* Call the procedure that needs to process this command */
          switch (cmd_ptr->cmd.rrc_llc_cnf.procedure)
          {
            case RRC_PROCEDURE_SIB:
              MSG_LOW("Dispatching RRC_LLC_CMD_CNF to SIB Procedure",0,0,0);
              rrc_sib_event_controller( cmd_ptr );
              break;

            case RRC_PROCEDURE_CSP:
              MSG_LOW("Dispatching RRC_LLC_CMD_CNF to Cell Selection Procedure",0,0,0);
              rrc_cell_selection_procedure( cmd_ptr );
              break;

            case RRC_PROCEDURE_CCM:
              MSG_LOW("Dispatching RRC_LLC_CMD_CNF to Cell Change Manager",0,0,0);
              rrc_cell_change_manager( cmd_ptr );
              break;

            case RRC_PROCEDURE_MCMR:
              MSG_LOW("Dispatching RRC_LLC_CMD_CNF to Measurement Procedure",0,0,0);
              rrcmeas_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_MCM:
              MSG_LOW("Dispatching RRC_LLC_CMD_CNF to to Mode Control Module",0,0,0);
              rrcmcm_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_CU:
              WRRC_MSG0_HIGH("Dispatching RRC_LLC_CMD_CNF to to CU Module");
              rrccu_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_NONE:
              WRRC_MSG0_HIGH("Dispatch to RRC_PROCEDURE_NONE");
              /* No procedure to be called here. Just break */
              break;

            default:
              WRRC_MSG1_ERROR("Failed to id Proc: rrc_proc_e_type_value%d for LLC Cnf",
                       (cmd_ptr->cmd.rrc_llc_cnf.procedure));
              break;
          } /* end switch */
          break;

        case RRC_DOWNLINK_L2ACK_IND:
#ifdef FEATURE_DUAL_SIM
          if((rrclcm_get_ul_rb_id(cmd_ptr->cmd.downlink_l2ack_ind.lc_id) == DCCH_AM_RADIO_BEARER_ID) ||
             (rrclcm_get_ul_rb_id(cmd_ptr->cmd.downlink_l2ack_ind.lc_id) == DCCH_DT_HIGH_PRI_RADIO_BEARER_ID))
          {
            rrc_reset_l2_ack_req_mask(cmd_ptr->cmd.downlink_l2ack_ind.mui);
          }
#endif
          if(RRCLCM_SUCCESS == rrclcm_identify_proc(cmd_ptr,
               rrclcm_get_ul_rb_id(cmd_ptr->cmd.downlink_l2ack_ind.lc_id),
               &rrc_proc_name))
          {
            if((rrclcm_get_ul_rb_id(cmd_ptr->cmd.downlink_l2ack_ind.lc_id) == DCCH_AM_RADIO_BEARER_ID) &&
                (rrc_check_if_any_proc_waiting_for_l2_ack_for_pch_transition(&proc_waiting_for_l2_ack)) &&
                (proc_waiting_for_l2_ack  == rrc_proc_name)
              )
            {
              rrctmr_stop_timer(RRCTMR_WAIT_FOR_L2_ACK_FOR_PCH_TRANSITION_TIMER);
            }

            if( (rrc_wait_for_l2ack_for_reconfig == rrc_proc_name)
                && (cmd_ptr->cmd.downlink_l2ack_ind.status == SUCCESS))
            {
              rrc_update_reconfig_waiting_for_l2ack_dch(MAX_NUM_OF_PROCEDURES);
            }

            if(rrc_get_state() == RRC_STATE_CELL_FACH)
            {
              /* Update RACH tx stats here if we receive L2 ACK Ind fail*/
              if(cmd_ptr->cmd.downlink_l2ack_ind.status != SUCCESS)
              {
                /*RACH tx failed for other reason, so update RACH tx parameters count here*/
                RRC_DEBUG_INCREMENT_STATS(RACH_RRC_RETRY_FAIL_DUE_TO_OTHER_CAUSE);
              }
            }
            switch(rrc_proc_name)
            {
              case RRC_PROCEDURE_RCE:
                rrcrce_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_RCR:
                rrcrcr_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_RBE:
                rrcrbe_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_RBR:
                rrcrbr_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_MCMR:
                rrcmeas_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_ASU:
                rrcasu_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_CU:
                rrccu_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_SMC:
                rrcsmc_procedure_event_handler( cmd_ptr );
                  break;

              case RRC_PROCEDURE_RBRC:
                rrcrbrc_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_TCR:
                rrctcrc_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_PCR:
                rrcpcrc_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_IDT:
                rrcidt_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_UDT:
                rrcudt_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_SCRR:
                rrcscrr_procedure_event_handler( cmd_ptr );
                break;

              case RRC_PROCEDURE_UMI:
                rrcumi_procedure_event_handler( cmd_ptr );
                break;
              case RRC_PROCEDURE_NONE:
                WRRC_MSG0_HIGH("Dispatched to RRC_PROCEDURE_NONE");
                /* No procedure will be called. Just break */
                break;

              case RRC_PROCEDURE_DORMANCY:
                rrcdormancy_procedure_event_handler(cmd_ptr );
                break;

#ifdef FEATURE_LTE_TO_WCDMA
              case RRC_PROCEDURE_IHO:
                rrciho_event_handler( cmd_ptr );
                break;
#endif

              default:
                WRRC_MSG1_ERROR("L2ACK rcvd for unknown Proc: rrc_proc_e_type_value%d",
                         rrc_proc_name);
                break;
            }
          }
          else
          {
            WRRC_MSG0_HIGH("Failed Id to proc for L2ACK or proc has not registered for L2ACK");
          }
          break;

        case RRC_TIMER_TO_CAMP_ON_EXPIRED_IND:
          rrcrce_procedure_event_handler( cmd_ptr );
          break;

        case RRCTMR_ACT_RSP_EF_TIMER_EXPIRED_IND:
          rrccho_procedure_event_handler( cmd_ptr );
          break;

        case RRCTMR_WT_STATE_CHANGE_EF_TIMER_EXPIRED_IND:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_WAIT_TIMER_EXPIRED_IND:
          rrcrce_procedure_event_handler( cmd_ptr );
          break;

#ifdef FEATURE_WCDMA_REL7_SPECRS 
       
        case RRC_COUEC_REQ:
          rrccouec_procedure_event_handler( cmd_ptr );
          break;


        case RRC_TIMER_COUEC_EXPIRED_IND:
          rrccouec_procedure_event_handler(cmd_ptr);
          break;
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

        case RRC_T320_EXPIRED_IND:
          rrcmisc_procedure_event_handler(cmd_ptr);
          break;          

        case RRC_T323_EXPIRED_IND:
          rrcdormancy_procedure_event_handler(cmd_ptr);
          break;    
        case RRC_PHY_CHAN_FAILURE_TIMER_EXPIRED_IND:
          rrcrce_procedure_event_handler( cmd_ptr );
          break;
        case RRC_RRC_CON_DELAY_TIMER_EXPIRED_IND:
          rrcrce_procedure_event_handler( cmd_ptr );
          break;

        case RRC_T300_EXPIRED_IND:
          rrcrce_procedure_event_handler( cmd_ptr );
          break;

        case RRC_T302_EXPIRED_IND:
          rrccu_procedure_event_handler( cmd_ptr );
          break;

        case RRC_T305_EXPIRED_IND:
          rrccu_procedure_event_handler( cmd_ptr );
          break;

        case RRC_T316_EXPIRED_IND:
          rrccu_procedure_event_handler( cmd_ptr );
          break;

        case RRC_T317_EXPIRED_IND:
          rrccu_procedure_event_handler( cmd_ptr );
          break;

        case RRC_T307_EXPIRED_IND:
          rrccu_procedure_event_handler( cmd_ptr );
          break;

        case RRC_T314_EXPIRED_IND:
          rrccu_procedure_event_handler( cmd_ptr );
          break;

        case RRC_T315_EXPIRED_IND:
          rrccu_procedure_event_handler( cmd_ptr );
          break;
 
        case RRC_CU_TRANS_TO_DISCON_TIMER_EXPIRED:
          rrccu_tran_to_disconn_debug_handler(cmd_ptr);
          break;
 
        case RRC_OOS_TIMER_EXPIRED_IND:
          rrccu_procedure_event_handler( cmd_ptr );
          break;

        case RRC_T3174_EXPIRED_IND:
          rrccho_procedure_event_handler( cmd_ptr );
          break;

        case RRC_CONN_SETUP_IND:
          rrccho_procedure_event_handler( cmd_ptr );
          break;

        case RRC_TIMER_FOR_RCE_L2ACK_EXPIRED_IND:
          rrcrce_procedure_event_handler( cmd_ptr );
          break;

        case RRC_T319_EXPIRED_IND:
          rrcmisc_procedure_event_handler(cmd_ptr);
          break;

        case RRC_REDIRECT_REQ:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_INITIATE_CELL_SELECTION_IND:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_DISABLE_PREV_FREQ_IND:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_GTOW_REDIRECTION_LFS_TIMER_EXPIRED_IND:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_REDIRECT_CNF:
          rrcrce_procedure_event_handler( cmd_ptr );
          break;

        case RRC_TIMER_FOR_RCR_L2ACK_EXPIRED_IND:
          rrcrcr_procedure_event_handler( cmd_ptr );
          break; 

        case RRC_T308_EXPIRED_IND:
          rrcrcr_procedure_event_handler( cmd_ptr );
          break;

        case RRC_T304_EXPIRED_IND:
          rrcueci_procedure_event_handler( cmd_ptr );
          break;

        case RRC_IGNORE_BCCH_MOD_TIMER_EXPIRED_IND:
          rrcpg1_procedure_event_handler( cmd_ptr );
          break;

        case RRC_TIMER_FOR_RSSI_BER_EXPIRED_IND:
          rrc_rssi_ber_timeout_handler();
          break;

        case RRC_SIB_WAIT_TIMER_EXPIRED_IND:
          rrc_sib_event_controller( cmd_ptr );
          break;

#ifdef FEATURE_DUAL_SIM
        case RRC_SIB_STOP_PARTITION_TMR_EXPIRED_IND:
        case RRC_SIB_MASK_INVERT_TMR_EXPIRED_IND:
        case RRC_SIB_EMPTY_MASK_TMR_EXPIRED_IND:
          rrc_sib_event_controller( cmd_ptr );
          break;
#endif

        case RRC_DCH_FACH_CELL_SELECTION_TIMER_EXPIRED_IND:
          WRRC_MSG1_HIGH( "DCH->FACH timer expired in CSP sstate %d",rrc_get_csp_substate());
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND:
          WRRC_MSG1_HIGH( "FACH->FACH/PCH timer expired in CSP sstate %d",rrc_get_csp_substate());
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRCTMR_L1_DEADLOCK_DETECT_TIMER_EXPIRED_IND:
          rrcllc_process_cmd( cmd_ptr );
          break;

#ifdef FEATURE_RRC_DELAY_ERR_FATAL
        case RRCTMR_DELAY_ERR_FATAL_TIMER_EXPIRED_IND:
          rrcllc_process_cmd( cmd_ptr );
          break;
#endif

        case RRC_DCH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND:
          WRRC_MSG1_HIGH( "DCH->PCH timer expired in CSP sstate %d",rrc_get_csp_substate());
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_INITIATE_UE_CAPABILITY_REQ:
          rrcueci_procedure_event_handler( cmd_ptr );
          break;

        case RRC_CELL_SELECTED_MEAS_REQ:
          rrcmeas_procedure_event_handler( cmd_ptr );
          break;

        case RRC_CELL_UPDATE_INITIATE_REQ:
          rrccu_procedure_event_handler( cmd_ptr );
          break;

        case RRC_URA_UPDATE_INITIATE_REQ:
          rrccu_procedure_event_handler( cmd_ptr );
          break;

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
        case RRC_RLC_UL_DATA_IND:
          rrccu_procedure_event_handler( cmd_ptr );
          break;
#endif

        case RRC_RNTI_UPDATE_REQ:
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_RNTI_UPDATE_CNF:
          switch(cmd_ptr->cmd.rnti_update_cnf.procedure)
          { 
            case RRC_PROCEDURE_UMI:
              rrcumi_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_CU:
              rrccu_procedure_event_handler( cmd_ptr );  
              break; 

            case RRC_PROCEDURE_RBE:
              rrcrbe_procedure_event_handler(cmd_ptr );
              break;
        
            case RRC_PROCEDURE_RBRC:
              rrcrbrc_procedure_event_handler(cmd_ptr );
              break;

            case RRC_PROCEDURE_RBR:
              rrcrbr_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_TCR:
              rrctcrc_procedure_event_handler( cmd_ptr );
              break;
            
            case RRC_PROCEDURE_PCR:
              rrcpcrc_procedure_event_handler( cmd_ptr );
              break;

            /* New procedures need to be added here */

            case RRC_PROCEDURE_NONE:
              WRRC_MSG0_HIGH("Dispatched to RRC_PROCEDURE_NONE: ");
              /* This does not call any RRC procedure. Just break */
              break;

            default:
              WRRC_MSG1_ERROR("RNTI Update Cnf rcvd for unknown proc: rrc_proc_e_type_value%d",
                       cmd_ptr->cmd.rnti_update_cnf.procedure);
              break;
          }
          break;

        case RRC_CELL_UPDATE_COMPLETE_CNF:
          switch(cmd_ptr->cmd.cu_complete_cnf.procedure)
          {
            case RRC_PROCEDURE_SCRR:
              rrcscrr_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_DORMANCY:
              rrcdormancy_procedure_event_handler(cmd_ptr );
              break;
            case RRC_PROCEDURE_RCE:
              rrcrce_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_IDT:
              rrcidt_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_UDT:
              rrcudt_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_UECI:
              rrcueci_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_RBE:
              rrcrbe_procedure_event_handler(cmd_ptr );
              break;
        
            case RRC_PROCEDURE_IHO:
              rrciho_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_CHO:
              rrccho_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_RBRC:
              rrcrbrc_procedure_event_handler(cmd_ptr );
              break;

            case RRC_PROCEDURE_RBR:
              rrcrbr_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_TCR:
              rrctcrc_procedure_event_handler( cmd_ptr );
              break;
            
            case RRC_PROCEDURE_PCR:
              rrcpcrc_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_NONE:
              WRRC_MSG0_HIGH("Dispatched to RRC_PROCEDURE_NONE: ");
              /* This does not call any RRC procedure. Just break */
              break;

            default:
              WRRC_MSG1_ERROR("CU Comp Cnf rcvd for unknown proc: rrc_proc_e_type_value%d",
                       cmd_ptr->cmd.cu_complete_cnf.procedure);
              break;
          } 
          break;
      
          case RRC_STOP_LOWER_LAYER_REQ:
            WRRC_MSG0_HIGH("Recd RRC_STOP_LOWER_LAYER_REQ");
            rrcllc_process_cmd( cmd_ptr );
            break;
  
          case RRC_STOP_LOWER_LAYER_CNF:
            WRRC_MSG0_HIGH("Recd RRC_STOP_LOWER_LAYER_CNF");
            rrcmcm_event_handler( cmd_ptr );
            break;

          case RRC_MODE_CHANGE_IND:
            MSG_MED("Recd RRC_MODE_CHANGE_IND",0,0,0);
            
            /* Inform the procedure based on the procedure identifier in
               the command */
            switch(cmd_ptr->cmd.mode_change_ind.procedure)
            {
              case RRC_PROCEDURE_CSP:
                rrc_cell_selection_procedure( cmd_ptr );
                break;
  
              case RRC_PROCEDURE_CCM:
                rrc_cell_change_manager( cmd_ptr );
                break; 

              case RRC_PROCEDURE_IHO:
                rrciho_event_handler( cmd_ptr );
                break;
  
              case RRC_PROCEDURE_SMC:
                rrcsmc_procedure_event_handler( cmd_ptr );
                break; 
  
              /* Only CSP will handle the mode transition to Inactive for now */
              default:
                WRRC_MSG1_ERROR("Procedure:%d doesn't handle MODE_CHANGE_IND",
                    cmd_ptr->cmd.mode_change_ind.procedure);
                break;
            }
            break;

          case RRC_CELL_RESELECTION_REQ:
            MSG_LOW("Dispatching RRC_CELL_RESELECTION_REQ to LLC",0,0,0);
            rrcllc_process_cmd( cmd_ptr );
            break;
  
          case RRC_CELL_RESELECTION_CNF:
            MSG_LOW("Dispatching RRC_CELL_RESELECTION_CNF to CCM",0,0,0);
            rrc_cell_change_manager( cmd_ptr );
            break;
  
          case RRC_SIB7_EXP_TIMER_EXPIRED_IND:
            rrc_sib_event_controller( cmd_ptr );
            break;
  
          case RRC_SIB7_UPDATE_STATUS_IND:
#ifdef FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH
            if(cmd_ptr->cmd.sib7_update_status.procedure == RRC_PROCEDURE_CU)
            { 
              rrccu_procedure_event_handler( cmd_ptr );
            }
            else
#endif
            {
              rrcrce_procedure_event_handler( cmd_ptr );
            }
            break;

          case RRC_CHANGE_MODE_REQ:
            rrcmcm_event_handler( cmd_ptr );
            break;

          case RRC_CHANGE_MODE_CNF:
            switch(cmd_ptr->cmd.change_mode_cnf.procedure)
            {
              /* CSP */
              case RRC_PROCEDURE_CSP:
                rrc_cell_selection_procedure( cmd_ptr );
                break;
        
              /* IHO */
              case RRC_PROCEDURE_IHO:
                rrciho_event_handler( cmd_ptr );
                break;
        

              /* CHO */
              case RRC_PROCEDURE_CHO:
                rrccho_procedure_event_handler( cmd_ptr );
                break;

              default:
                WRRC_MSG1_ERROR("Procedure:%d doesn't handle CHANGE_MODE_CNF",
                    cmd_ptr->cmd.change_mode_cnf.procedure);
                break;
            }
            break;

          case RRC_NAS_DATA_TRANSFER_IND:
            rrcidt_procedure_event_handler( cmd_ptr );
            rrcudt_procedure_event_handler( cmd_ptr );
            break;

          
        case RRC_CSP_ACQ_REQ:
          WRRC_MSG0_HIGH("Rcvd. CSP_ACQ_REQ");
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_CSP_ACQ_CNF:
          rrciho_event_handler( cmd_ptr );
          break;

        case RRC_HO_CIPH_REQ:
          rrcsmc_procedure_event_handler( cmd_ptr );
          break;

        case RRC_HO_CIPH_CNF:
          rrciho_event_handler( cmd_ptr );
          break;

          /*commands added for ciphering related issues*/
          
          case RRC_CIPH_CONFIG_REQ:
            rrcllc_process_cmd(cmd_ptr);
            break;
          
          case RRC_ABORT_CIPH_CONFIG_REQ:
            rrcllc_process_cmd(cmd_ptr);
            break;

           
          case RRC_CIPH_CONFIG_CNF:
            rrcsmc_procedure_event_handler( cmd_ptr );
            break;
          
          case RRC_RECONFIG_COMPLETE_IND:
            if(rrctfcc_is_reconfig_in_progress() == TRUE)
            {
              WRRC_MSG0_HIGH("Routing to TFCC proc if TFCC substate is reconfig in progress");
              rrctfc_procedure_event_handler(cmd_ptr);
            }
            if(rrcsmc_is_reconfig_in_progress() == TRUE)
            {
              WRRC_MSG0_HIGH("Routing to SMC proc if SMC substate is reconfig in progress");
              rrcsmc_procedure_event_handler( cmd_ptr );
            }
            break;

          case RRC_SIG_CONN_RELEASE_REQ:
            rrcscrr_procedure_event_handler( cmd_ptr );
            break;
            
          case RRC_SIG_CONN_RELEASE_CNF:
            switch(cmd_ptr->cmd.rrc_sig_conn_cnf.rrc_proc)
            {
              case RRC_PROCEDURE_RCE:
            rrcrce_procedure_event_handler( cmd_ptr );
            break;

              case RRC_PROCEDURE_DORMANCY:
                rrcdormancy_procedure_event_handler(cmd_ptr );
                break;
                
              default:
                WRRC_MSG0_HIGH("Unknown Proc for Re-est-Cnf");
                break;
            }

          case RRC_COMPRESSED_MODE_IND:
            WRRC_MSG0_HIGH("Routing RRC_COMPRESSED_MODE_IND to PCR procedure");
            rrcpcrc_procedure_event_handler(cmd_ptr);
            break;
            
          case RRC_DELETE_MEAS_REQ:
            WRRC_MSG0_HIGH("Routing RRC_DELETE_MEAS_REQ to Meas procedure");
            rrcmeas_procedure_event_handler(cmd_ptr);
            break;


        case RRC_SMC_DDT_L2_ACK_IND:
          rrcddt_procedure_event_handler( cmd_ptr );    
          break;

        case RRC_RE_EST_RLC_FOR_SRNS_REQ:
          rrcsmc_procedure_event_handler(cmd_ptr);
          break;

        case RRC_RE_EST_RLC_FOR_SRNS_CNF:
          switch(cmd_ptr->cmd.rrc_re_est_srns_cnf.rrc_proc)
          {
            case RRC_PROCEDURE_RBE:
              rrcrbe_procedure_event_handler(cmd_ptr );
              break;
        
            case RRC_PROCEDURE_RBRC:
              rrcrbrc_procedure_event_handler(cmd_ptr );
              break;

            case RRC_PROCEDURE_RBR:
              rrcrbr_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_TCR:
              rrctcrc_procedure_event_handler( cmd_ptr );
              break;
            
            case RRC_PROCEDURE_PCR:
              rrcpcrc_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_CU:
              rrccu_procedure_event_handler( cmd_ptr );
              break;
            
            case RRC_PROCEDURE_UMI:
              rrcumi_procedure_event_handler( cmd_ptr );
              break;

            default:
              WRRC_MSG0_HIGH("Unknown Proc for Re-est-Cnf");
              break;
          }
          break;

        case RRC_LLC_RE_EST_RLC_REQ:
          rrcllc_process_cmd(cmd_ptr);
          break;

        case RRC_LLC_RE_EST_RLC_CNF:
          rrcsmc_procedure_event_handler(cmd_ptr);
          break;
        case RRC_ERR_FATAL_IND:
          WRRC_MSG0_HIGH("Sending ERR_FATAL_IND to CSP");
          rrc_cell_selection_procedure(cmd_ptr);
          break;

        case RRC_CSP_CAMP_REQ:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_CSP_CAMP_CNF:
          rrccho_procedure_event_handler( cmd_ptr );
          break;

        case RRC_FREQ_SCAN_TIMER_EXPIRED_IND:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_BPLMN_SRCH_TIMER_EXPIRED_IND:
          WRRC_MSG1_HIGH( "BPLMN Search Timer expired in CSP sstate %d",rrc_get_csp_substate());
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_BPLMN_FREQ_SCAN_TIMER_EXPIRED_IND:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_MANUAL_PLMN_FREQ_SCAN_TIMER_EXPIRED_IND:
          rrc_cell_selection_procedure( cmd_ptr );
          break;


        case RRC_DEEP_SLEEP_NO_SVC_TIMER_EXPIRED_IND:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_CSP_WCDMA_RESUMED_IND:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_FREQ_REDIRECT_WAIT_TIMER_EXPIRED_IND:
          WRRC_MSG0_HIGH("Inter Freq redirection Wait Timer expired");
          rrc_cell_selection_procedure( cmd_ptr );
          break;
        
        case RRC_IRAT_ABORT_IND:
          switch(cmd_ptr->cmd.rrc_irat_abort_ind.procedure)
          {
            case RRC_PROCEDURE_CSP:
              WRRC_MSG2_HIGH("Dispatching IRAT_ABORT_IND to CSP in sstate %d curr_scan %d",
                              rrc_get_csp_substate(), rrc_csp_int_data.curr_scan);
              rrc_cell_selection_procedure( cmd_ptr );
              break;

            case RRC_PROCEDURE_CHO:
              rrccho_procedure_event_handler(cmd_ptr);
              break;


            default:
              WRRC_MSG0_HIGH("Proc id not supported in IRAT_ABORT_IND,throwing away cmd");
              break;
          }
          break;

        case RRC_CONN_MODE_OOS_TIMER_EXPIRED_IND:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_IDT_COMPLETED_FOR_DOMAIN:
          WRRC_MSG0_HIGH("RRC IDT completed ind to RRCSCRR");
          rrcscrr_procedure_event_handler(cmd_ptr);
          break;

#ifdef FEATURE_INACTIVITY_HANDLING
        case RRC_INACTIVITY_TIMER_EXPIRED_IND:
          rrcmisc_procedure_event_handler(cmd_ptr);
          break;
#endif /* FEATURE_INTACTIVITY_HANDLING */

        case RRC_WTOW_BPLMN_DRX_SRCH_TIMER_EXPIRED_IND:
          WRRC_MSG1_HIGH("DRX_SRCH_TIMER_EX fwd to CSP by dispatcher in CSP sstate %d",rrc_get_csp_substate()); 
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_BPLMN_GUARD_SRCH_TIMER_EXPIRED_IND:
          WRRC_MSG1_HIGH("GUARD_SRCH_TIMER_EXP fwd to CSP by dispatcher in CSP sstate %d",rrc_get_csp_substate()); 
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_BPLMN_SUSPEND_REQ:
          WRRC_MSG2_HIGH("RRC_BPLMN_SUSPEND_REQ fwd to CSP by dispatcher in CSP sstate %d from proc rrc_proc_e_type_value%d",
                          rrc_get_csp_substate(), cmd_ptr->cmd.rrc_csp_bplmn_suspend_req.procedure); 
          rrc_cell_selection_procedure( cmd_ptr );
          break;
  
        case RRC_BPLMN_SUSPEND_CNF:
          switch(cmd_ptr->cmd.rrc_csp_bplmn_suspend_cnf.procedure)
          {
            WRRC_MSG2_HIGH("WTOW Dispatching RRC_BPLMN_SUSPEND_CNF to proc rrc_proc_e_type_value%d status %d", 
                            cmd_ptr->cmd.rrc_csp_bplmn_suspend_cnf.procedure,cmd_ptr->cmd.rrc_csp_bplmn_suspend_cnf.status);
            case RRC_PROCEDURE_RCE:
              rrcrce_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_SIB:
              rrc_sib_event_controller( cmd_ptr );
              break;
  
            case RRC_PROCEDURE_CU:
              rrccu_procedure_event_handler( cmd_ptr );
              break;

            case RRC_PROCEDURE_CHO:
              rrccho_procedure_event_handler( cmd_ptr );
              break;

           case RRC_PROCEDURE_MISC:
              rrcmisc_procedure_event_handler( cmd_ptr );
              break;
              
            default:
              WRRC_MSG1_ERROR("ERR: Proc id rrc_proc_e_type_value%d not supported in BPLMN_SUSPEND_CNF, throwing away cmd",
                                        cmd_ptr->cmd.rrc_csp_bplmn_suspend_req.procedure);
              break;
          }
          break;

        case RRC_BPLMN_SEARCH_RESUME_IND:
          WRRC_MSG1_HIGH("RRC_BPLMN_SEARCH_RESUME_IND fwd to CSP by dispatcher in sstate %d", rrc_get_csp_substate()); 
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_MODE_STATUS_IND:
          {
            rrc_state_e_type rrc_curr_state = rrc_get_state();
            WRRC_MSG1_HIGH("DUAL:RRC_MODE_STATUS_IND fwd to MEAS by dispatcher in rrc st %d", rrc_curr_state);
            rrcmeas_procedure_event_handler( cmd_ptr );
           
           if((rrc_curr_state == RRC_STATE_CELL_FACH)
              ||(rrc_curr_state == RRC_STATE_CELL_PCH)
              ||(rrc_curr_state == RRC_STATE_CELL_DCH)
              ||(rrc_curr_state == RRC_STATE_URA_PCH))
           {
             WRRC_MSG1_HIGH("DUAL:RRC_MODE_STATUS_IND fwd to UECI by dispatcher in rrc st", rrc_curr_state);
             rrcueci_procedure_event_handler( cmd_ptr );
           }
          }
          break;

        case RRC_SIX_HRS_SIB_TIMER_UPDATE_IND:
          WRRC_MSG0_HIGH("6hr sib Timer expired");
          rrc_sib_event_controller( cmd_ptr );
          break;

        case RRC_TEN_MIN_SIB_TIMER_UPDATE_IND:
          rrc_sib_event_controller(cmd_ptr);
          break;

#ifdef FEATURE_EOOS
        #error code not present
#endif

#ifdef FEATURE_EOOS
        #error code not present
#endif


        case RRC_CONN_MODE_DEEP_SLEEP_REQ:
          WRRC_MSG0_HIGH("OOS: dispatcher rcvd RRC_CONN_MODE_DEEP_SLEEP_REQ");
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CONN_MODE_DEEP_SLEEP_WAKEUP_REQ:
          WRRC_MSG0_HIGH("OOS: dispatcher rcvd RRC_CONN_MODE_DEEP_SLEEP_WAKEUP_REQ");
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CONN_MODE_OOS_INITIAL_NO_DS_TIMER_EXPIRED_IND:
          WRRC_MSG0_HIGH("OOS: Initial No DS OOS in Connected Mode Tmr Expd");
          rrc_cell_selection_procedure( cmd_ptr );
          break;
  
        case RRC_CONN_MODE_OOS_DS_TIMER_EXPIRED_IND:
          WRRC_MSG0_HIGH("OOS: DS OOS in Connected Mode Tmr Expd");
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_RB_PROC_COMPL_IND:
          rrccu_procedure_event_handler(cmd_ptr );
          break;

        case RRC_SIB_CHANGE_GUARD_TIMER_EXPIRED_IND:
          WRRC_MSG0_HIGH("SIB CHANGE GUARD Timer expired");
          rrc_sib_event_controller( cmd_ptr );
          break;

        case RRC_BCCH_MODIFICATION_TIMER_EXPIRED_IND:
          WRRC_MSG0_HIGH("BCCH Modification Timer expired");
          rrcpg1_procedure_event_handler( cmd_ptr );
          break;

#ifdef FEATURE_WCDMA
        case RRC_GSTK_UTRAN_NMR_INFO_REQ:
          rrcmeas_procedure_event_handler(cmd_ptr);
          break;
#endif

        case RRC_GTOW_REDIRECTION_WAIT_TIMER_EXPIRED_IND:
          WRRC_MSG0_HIGH("GtoW Redirection: RRC_GTOW_REDIRECTION_WAIT_TIMER_EXPIRED_IND fwd to CSP by dispatcher"); 
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case  RRC_CONTINUE_STOP_MODE_REQ:
          rrcmcm_event_handler( cmd_ptr );
          break;

        case RRC_WTOG_ABORT_GUARD_TIMER_EXPIRED_IND:
          WRRC_MSG0_HIGH("WTOG Abort Timer expired"); 
          rrc_cell_selection_procedure( cmd_ptr );
          break;
#ifdef FEATURE_DUAL_SIM
        case RRC_WRM_PREEMPT_IND:
          if(rrcwrm_unlock_type == TRM_UNLOCK_IMMEDIATELY)
          {  
            WRRC_MSG0_HIGH("DSIM: RRC_WRM_PREEMPT_IND fwd to CSP");
            rrc_cell_selection_procedure(cmd_ptr);
          }
          else
          {
            WRRC_MSG1_HIGH("DSIM: RRC_WRM_PREEMPT_IND ignored for unlock type %d",rrcwrm_unlock_type);
          }
          break;

        case RRC_NO_RESOURCE_AVAILABLE_FROM_L1:
          WRRC_MSG0_HIGH("DSIM: RRC_CPHY_NO_RESOURCE_AVAILABLE recvd from L1");
          rrc_cell_selection_procedure(cmd_ptr);
          break;

        case RRC_DUAL_SIM_WRM_UNLOCK_BY_TIMER_EXP_IND:
          WRRC_MSG0_HIGH("DSIM_TUNEAWAY: RRC_DUAL_SIM_WRM_UNLOCK_BY_TIMER_EXP_IND fwd to CSP");
          rrc_cell_selection_procedure(cmd_ptr);
          break;
#endif

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
        case RRC_DED_PRI_VALDITY_TIMER_EXPIRED_IND:
          WRRC_MSG0_HIGH("WTOL: RRC_DED_PRI_VALDITY_TIMER_EXPIRED_IND");
          rrcmeas_procedure_event_handler(cmd_ptr);
          break;

        case RRC_DED_PRI_CHANGE_IND:
          WRRC_MSG0_HIGH("WTOL: RRC_DED_PRI_CHANGE_IND");
          rrcmeas_procedure_event_handler(cmd_ptr);
          break;
#endif


#ifdef FEATURE_QCHAT
        case RRC_QCHAT_FA_RRC_TIMER_EXPIRED_IND:
          if(qchatupk_wcdma_faopts_enabled())
          {
            /* Processes QChat related internal RRC commands e.g.
               RRC_QCHAT_FA_RRC_TIMER_EXPIRED_IND.
               The command buffer is not cleared by this function, but
               happens after falling through to rrc_free_cmd_buf below. */
            qchatupk_rrc_process_internal_commands(cmd_ptr);
          }
          break;
#endif /* FEATURE_QCHAT */

#ifdef FEATURE_LTE_TO_WCDMA
         case RRC_IHO_OPEN_DOMAIN_IND:
           WRRC_MSG0_HIGH("LTOW_PSHO: RRC_IHO_OPEN_DOMAIN_IND");
           rrcudt_procedure_event_handler(cmd_ptr);
           break;
#endif
#ifdef FEATURE_LTE_TO_WCDMA
      case RRC_LTE_TO_WCDMA_SRCH_TIMER_EXPIRY_IND:
          WRRC_MSG0_HIGH("LTOW: RRC_LTE_TO_WCDMA_SRCH_TIMER_EXPIRY_IND");
          rrc_cell_selection_procedure(cmd_ptr);
          break;
#endif

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
      case RRC_INTERNAL_CSFB_ABORT_REQ:
          WRRC_MSG0_HIGH("CSFB_HACK: RRC_INTERNAL_CSFB_ABORT_REQ");
          rrcrce_procedure_event_handler(cmd_ptr);
          break;
#endif

#ifdef FEATURE_FEMTO_CSG
      case RRC_CSG_ASF_SRCH_TIMER_EXPIRY_IND:
          WRRC_MSG0_HIGH("CSG: RRC_CSG_ASF_SRCH_TIMER_EXPIRY_IND");
          rrc_cell_selection_procedure(cmd_ptr);
          break;
#endif

        case RRC_BPLMN_PARTIAL_REPORT_TIMER_EXPIRED_IND:
          WRRC_MSG0_HIGH("RRC_BPLMN_PARTIAL_REPORT_TIMER_EXPIRED_IND");
          rrc_cell_selection_procedure( cmd_ptr );
          break;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
        case RRC_NV_REFRESH_IND:
          WRRC_MSG0_HIGH("RRC_NV_REFRESH_IND");
          rrcmisc_procedure_event_handler( cmd_ptr );
          break;
#endif
#ifdef FEATURE_QSH_EVENT_METRIC
        case RRC_QSH_PROCESS_CMD:
          WRRC_MSG0_HIGH("RRC_QSH_PROCESS_CMD");
          rrc_process_qsh_cmd((qsh_client_cb_params_s*)&(cmd_ptr->cmd.rrc_qsh_cmd));
          break;
#endif

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
        case RRC_ACTIVATION_TIMER_EXPIRY_FOR_BUFFER_OTA:
          if (RRC_CSP_UNLOCK_BY_SCAN == rrc_csp_int_data.curr_scan)
          {
            WRRC_MSG0_HIGH("activation timer expiry is pended as unlock by scan is in progress");
            rrcdata_misc_info.rrc_buffered_ota_activation_time_expired_during_lta = TRUE;
            /**/
          }
          else
          {
            rrcdata_post_buffer_ota_after_activation_time_expiry();
          }

        break;

        case RRC_POST_BUFFER_OTA_IND:
          rrcdata_process_buffered_ota_message( cmd_ptr);
          break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
        default:
        /* This should not happen. Possibly the command id was never written 
          before this item was put in the queue. Display error for now. */ 
          WRRC_MSG1_ERROR( "Unknown internal cmd rcvd by RRC 0x%x", 
            cmd_ptr->cmd_hdr.cmd_id);
          break;
      }/* end switch */
    }/* end if */
    else
    {
      WRRC_MSG0_HIGH("Throw away the Internal command");
    }
    /* The command has been processed. We can free the memory for this cmd */
    rrc_free_cmd_buf( cmd_ptr );
  }/* end while */
}/* end function */



/*===========================================================================

FUNCTION  RRC_DISPATCH_L1_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's l1
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_l1_commands( void )
{
  
  rrc_cmd_type *cmd_ptr;   /* To hold received command */

  rrc_csp_scan_e_type csp_curr_scan = rrccsp_get_curr_scan_no_f3();

  /* We need to dequeue items from the l1
  command queue and process them till the queue is
  empty */
  while( (cmd_ptr = q_get(&rrc_l1_cmd_q)) != NULL)
  {
    WRRC_MSG1_HIGH_OPT("Dispatching RRC Ext PHY cmd rrc_cmd_e_type_value_0x%x",cmd_ptr->cmd_hdr.cmd_id);
    /* Check if the incoming internal command is required a further process */
    if (rrcmcm_is_process_required(cmd_ptr) == FALSE) 
    {
      /* Display a XDM message and disgard the command */
      WRRC_MSG1_HIGH("CMD: %d is disgarded!",cmd_ptr->cmd_hdr.cmd_id);
    }
    else if(rrc_deact_status != RRC_DEACT_COMPLETED)
    {

      /* Save crash debug info */
      rrc_crash_debug_save_l1_to_rrc_cmds(cmd_ptr);
#ifdef FEATURE_QSH_EVENT_METRIC
      wrrc_qsh_log_l1_rrc_cmd_params(cmd_ptr);
#endif
      /* Switch on the command id of the received command, and
       * call the appropriate function to process each command. 
       */
      switch( cmd_ptr->cmd_hdr.cmd_id )
      {
        case RRC_CPHY_ACQ_CNF:
          MSG_LOW("Dispatching RRC_CPHY_ACQ_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_FREQ_SCAN_CNF:
          MSG_LOW("Dispatching RRC_CPHY_FREQ_SCAN_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_SETUP_CNF:
          MSG_LOW("Dispatching RRC_CPHY_SETUP_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_CELL_SELECTION_CNF:
          MSG_LOW("Dispatching RRC_CPHY_CELL_SELECTION_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_CELL_TRANSITION_CNF:
          MSG_LOW("Dispatching RRC_CPHY_TRANSITION_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_IDLE_CNF:
          MSG_LOW("Dispatching RRC_CPHY_IDLE_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_PHYCHAN_ESTABLISHED_IND:
          MSG_LOW("Dispatching RRC_CPHY_PHYCHAN_ESTABLISHED_IND to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_OUT_OF_SYNC_IND:
          /* TBD: This command might have to go to cell selection later */
          MSG_LOW("Dispatching RRC_CPHY_OUT_OF_SYNC_IND to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_MEASUREMENT_IND:
          /* TBD: This command might have to go to cell selection later */
          MSG_LOW("Dispatching RRC_CPHY_MEASUREMENT_IND to LLC",0,0,0);
          rrcmeas_procedure_event_handler( cmd_ptr );
          break;

        case RRC_CPHY_OUT_OF_SERVICE_AREA_IND:
#ifdef FEATURE_DUAL_SIM
          /*TBD: Is this really needed*/
          if(cmd_ptr->cmd.oos_ind.oos_cause == WL1_FAILED_NO_WRM_LOCK)
          {
            WRRC_MSG0_HIGH("DSIM: Dispatching OUT_OF_SERVICE_IND to CSP");
            event_report(EVENT_WCDMA_OUT_OF_SERVICE);
            rrc_cell_selection_procedure(cmd_ptr);
          }
          else
#endif
          {
            MSG_LOW("Dispatching OUT_OF_SRVC_IND to CU",0,0,0);

            if(rrc_get_state() == RRC_STATE_DISCONNECTED || 
               rrc_get_state() == RRC_STATE_CONNECTING)
            {
              rrcrce_procedure_event_handler( cmd_ptr );
            }
            else
            {
              rrccu_procedure_event_handler( cmd_ptr );
            }
          
#ifdef FEATURE_WCDMA_LOG_RRC_STATES
            SMEM_LOG_EVENT_EX(SMEM_LOG_AREA_CIRCULAR_POWER, SMEM_LOG_RRC_OOS_STATE, 0, 0, 0, 0);
#endif /*FEATURE_WCDMA_LOG_RRC_STATES*/

          
            /* Static Analysis Tool */
            if((rrc_get_state() != RRC_STATE_CELL_FACH) &&
               (rrc_get_state() != RRC_STATE_CONNECTING))
            {
              rrcbmc_procedure_event_handler( cmd_ptr );
            }
          }
#ifdef FEATURE_QSH_EVENT_METRIC
          wrrc_qsh_log_rlf_oos_reject_params(WRRC_OOS, WRRC_METRIC_UPDATE);
          if(rrc_get_state() != RRC_STATE_DISCONNECTED)
          { 
            rrccu_qsh_log_rlf_params(SYS_RLF_WEAK_SIGNAL_OTHER);
          }
#endif
          break;

        case RRC_CPHY_DRX_CNF:
          MSG_LOW("Dispatching RRC_CPHY_DRX_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_RL_FAILURE_IND:
          if ((OC_SET_FOR_DCH_FACH_TRANS == rrcllc_get_ordered_config_status_wo_f3()) )
          {
            /* UE will go to fach thru cell selection anyways. Ingore*/
            WRRC_MSG1_HIGH("Ignoring RL failure ind from L1",rrcllc_get_ordered_config_status());
          }
          else
          {
            rrccu_procedure_event_handler( cmd_ptr );
          }
#ifdef FEATURE_WCDMA_LOG_RRC_STATES
          SMEM_LOG_EVENT_EX(SMEM_LOG_AREA_CIRCULAR_POWER, SMEM_LOG_RRC_OOS_STATE, 0, 0, 0, 0);
#endif /*FEATURE_WCDMA_LOG_RRC_STATES*/  
#ifdef FEATURE_QSH_EVENT_METRIC
          wrrc_qsh_log_rlf_oos_reject_params(WRRC_RLF, WRRC_METRIC_UPDATE);
#endif          
          break;

        case RRC_CPHY_DEACTIVATE_CNF:
          MSG_LOW("Dispatching RRC_CPHY_DEACTIVATE_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_MEAS_CTRL_CNF:
          MSG_LOW("Dispatching RRC_CPHY_MEAS_CTRL_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_CELL_RESELECTION_IND:
          MSG_LOW("Dispatching RRC_CPHY_CELL_RESELECTION_IND to CSP",0,0,0);
          rrc_cell_selection_procedure( cmd_ptr);
          break;

        case RRC_CPHY_COMPRESSED_MODE_IND:
          WRRC_MSG0_HIGH("Received RRC_CPHY_COMPRESSED_MODE_IND");
          rrcllc_process_cmd(cmd_ptr);
          break;
      
        case RRC_CPHY_START_WCDMA_MODE_CNF:
          WRRC_MSG0_HIGH("Recd RRC_CPHY_START_WCDMA_MODE_CNF");

#ifdef FEATURE_RRC_L1_ERROR_RECOVERY_P1
          if(rrcllc_error_recovery_in_progress())
          {
            WRRC_MSG0_HIGH("Routing START_WCDMA_MODE_CNF to LLC");
            rrcllc_process_cmd(cmd_ptr);
          }
          else
          {
            /* For OOS_Scan the start_cnf needs to go to LLC so that LLC can send
             * the commands in the queue (idle request, acq request, etc.) to L1.
             * This is because when the deep sleep timer expires, CSP sends wake up
             * request to LLC followed by idle/acq requests. LLC on receiving the wakeup
             * req sends start_req to L1 and waits for the confirmation from L1 before
             * sending idle/acq req to L1. That is why in this case start_cnf needs to
             * be routed to LLC instead of MCM.
             *
             * We need to check for leaving connected mode scan also because in the case
             * where UE is in deep sleep and T305+T307/T314/T315/etc. expire due to which UE needs to
             * be taken to Idle, CSP on receiving tx to disconnected, will first send wake
             * up request to LLC and then call the function start_sel_leaving_conn_mode
             * so that UE can be taken to Idle. As part of this fn, the curr_scan is changed
             * from OOS_Scan to leaving connected mode scan. Hence even for leaving conn
             * mode scan the start_cnf needs to be sent to LLC instead of MCM.
             */
            
            if(   (csp_curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
               || (csp_curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN)
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
              #error code not present
#endif
              )
            {
              WRRC_MSG0_HIGH("OOS: Routing START_WCDMA_MODE_CNF to LLC for DeepSleep in ConnMode");
              rrcllc_process_cmd(cmd_ptr);
            }
            else
            {
              rrcmcm_event_handler( cmd_ptr );
            }

          }

#else  /* FEATURE_RRC_L1_ERROR_RECOVERY_P1 */

          /* For OOS_Scan the start_cnf needs to go to LLC so that LLC can send
           * the commands in the queue (idle request, acq request, etc.) to L1.
           * This is because when the deep sleep timer expires, CSP sends wake up
           * request to LLC followed by idle/acq requests. LLC on receiving the wakeup
           * req sends start_req to L1 and waits for the confirmation from L1 before
           * sending idle/acq req to L1. That is why in this case start_cnf needs to
           * be routed to LLC instead of MCM.
           *
           * We need to check for leaving connected mode scan also because in the case
           * where UE is in deep sleep and T305+T307/T314/T315/etc. expire due to which UE needs to
           * be taken to Idle, CSP on receiving tx to disconnected, will first send wake
           * up request to LLC and then call the function start_sel_leaving_conn_mode
           * so that UE can be taken to Idle. As part of this fn, the curr_scan is changed
           * from OOS_Scan to leaving connected mode scan. Hence even for leaving conn
           * mode scan the start_cnf needs to be sent to LLC instead of MCM.
           */
          if(   (csp_curr_scan == RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
             || (csp_curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN)
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
             #error code not present
#endif
            )
          {
            WRRC_MSG0_HIGH("OOS: Routing START_WCDMA_MODE_CNF to LLC for DeepSleep in ConnMode");
            rrcllc_process_cmd(cmd_ptr);
          }
          else
          {
            rrcmcm_event_handler( cmd_ptr );
          }



#endif /* FEATURE_RRC_L1_ERROR_RECOVERY_P1 */
          break;

        case RRC_CPHY_STOP_WCDMA_MODE_CNF:
          WRRC_MSG0_HIGH("Recd RRC_CPHY_STOP_WCDMA_MODE_CNF");
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_SUSPEND_WCDMA_MODE_CNF:
          WRRC_MSG0_HIGH("Recd RRC_CPHY_SUSPEND_WCDMA_MODE_CNF");
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CPHY_RESUME_WCDMA_MODE_CNF:
          WRRC_MSG0_HIGH("Recd RRC_CPHY_RESUME_WCDMA_MODE_CNF");
          rrcllc_process_cmd( cmd_ptr );
          break;
        case RRC_CPHY_ERROR_IND:
          WRRC_MSG0_HIGH("Recd RRC_CPHY_ERROR_IND");
          rrcllc_process_cmd( cmd_ptr );
          break;

#ifdef FEATURE_QCHAT
        case RRC_QCHAT_HANDLE_CELL_UPDATE:
          if(qchatupk_wcdma_imo_enabled())
          {
            qchatupk_rrc_process_commands(cmd_ptr);
          }
          break;
#endif /*FEATURE_QCHAT*/

#ifdef FEATURE_RRC_L1_ERROR_RECOVERY_P1
        case RRC_CPHY_OPERATIONAL_ERROR_IND:
          WRRC_MSG0_HIGH("Recd RRC_CPHY_OPERATIONAL_ERROR_IND");
          rrcllc_process_cmd(cmd_ptr);
          break;
#endif
        case RRC_CPHY_DL_WEAK_IND:
          if( (rrc_get_state() == RRC_STATE_CELL_DCH) && (rrc_sib_is_current_event_sib_read_in_dch()))
          {
            rrc_proc_e_type proc_id = rrc_sib_get_proc_id_for_sib_read_in_dch();
            if(proc_id == RRC_PROCEDURE_GPS)
            {

              rrcgps_procedure_event_handler( cmd_ptr );

              WRRC_MSG0_HIGH("Send RRC_CPHY_DL_WEAK_IND to RRC GPS ");
            }
            else 
            {
              WRRC_MSG1_ERROR("Invalid proc id rrc_proc_e_type_value%d for Cell DCH SIB reading",proc_id);
            }
          }
          else
          {
            MSG_LOW("Dispatching RRC_CPHY_DL_WEAK_IND to CSP",0,0,0);
            rrc_cell_selection_procedure( cmd_ptr);
          }
          break;
        
        case RRC_CPHY_ACT_CANCEL_CNF:
          WRRC_MSG0_HIGH("Dispatching RRC_CPHY_ACT_CANCEL_CNF to LLC");
          rrcllc_process_cmd( cmd_ptr );
          break;

#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
        case RRC_CPHY_MDSP_HALT_IND:
          WRRC_MSG0_HIGH("Dispatching RRC_CPHY_MDSP_HALT_IND to LLC");
          rrcllc_process_cmd(cmd_ptr);
          break;
        case RRC_CPHY_RECOVER_WCDMA_CNF:
          WRRC_MSG0_HIGH("Dispatching RRC_CPHY_RECOVER_WCDMA_CNF to LLC");
          rrcllc_process_cmd(cmd_ptr);
          break;
#endif


#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
        case RRC_CPHY_MODEM_STATISTICS_RSP:
          WRRC_MSG0_HIGH("FF: Dispatching RRC_MODEM_STATISTICS_RSP to rrcdata");
          rrc_process_modem_stat_info( cmd_ptr );
          break;
#endif


#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
        case RRC_CPHY_NUDGE_FOR_SFN_MISS_MATCH_REQ:
          WRRC_MSG0_HIGH("FF: Dispatching RRC_NUDGE_FOR_SFN_MISS_MATCH_REQ to RRCPG1");
          rrcpg1_procedure_event_handler( cmd_ptr );
          break;
#endif
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
        case RRC_CPHY_ACTIVATION_TIMER_EXPIRY_IND:
          rrc_send_codec_info_on_activation_timer_expiry();
          break;
#endif
        default:
          /* This should not happen. Possibly the command id was never written 
          before this item was put in the queue. Display error for now. */ 
          WRRC_MSG1_ERROR( "Unknown l1 cmd rcvd by RRC 0x%x", 
                cmd_ptr->cmd_hdr.cmd_id);
          break;
      }/* end switch */
    }/* end if */
    else
    {
      WRRC_MSG0_HIGH("Throw away the L1 command");
    }
    /* The command has been processed. We can free the memory for this cmd */
    rrc_free_cmd_buf( cmd_ptr );

  }/* end while */
}/* end function */


/*===========================================================================

FUNCTION  RRC_DISPATCH_MAC_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's mac
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_mac_commands( void )
{
  
  rrc_cmd_type *cmd_ptr;  /* To hold received command */

  /* We need to dequeue items from the mac
  command queue and process them till the queue is
  empty */

  while( (cmd_ptr = q_get(&rrc_mac_cmd_q)) != NULL)
  {
    WRRC_MSG1_HIGH_OPT("Dispatching RRC Ext Mac cmd rrc_cmd_e_type_value_0x%x",cmd_ptr->cmd_hdr.cmd_id);
    /* Check if the incoming internal command is required a further process */
    if (rrcmcm_is_process_required(cmd_ptr) == FALSE) 
    {
      /* Display a XDM message and disgard the command */
      WRRC_MSG1_HIGH("CMD: %d is disgarded!",cmd_ptr->cmd_hdr.cmd_id);
    }
    else if(rrc_deact_status != RRC_DEACT_COMPLETED)
    {
      /* Save Crash debug info */
      rrc_crash_debug_save_l2_to_rrc_cmds(cmd_ptr);
      /* Switch on the command id of the received command, and
       * call the appropriate function to process each command. 
       */
      switch( cmd_ptr->cmd_hdr.cmd_id )
      {
        case RRC_CMAC_UL_HFN_CNF:
          MSG_LOW("Dispatching RRC_CMAC_UL_HFN_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CMAC_DL_HFN_CNF:
          MSG_LOW("Dispatching RRC_CMAC_DL_HFN_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CMAC_UL_TRAFFIC_IND:
          MSG_LOW("Dispatching RRC_CMAC_UL_TRAFFIC_IND to Meas",0,0,0);
          rrcmeas_procedure_event_handler( cmd_ptr );
          break;

        case RRC_CMAC_STATUS_IND:
          if(rrcrce_get_rce_sub_state() == RRCRCE_WAIT_FOR_RRC_CONN_SETUP)
          {
            rrcrce_procedure_event_handler( cmd_ptr );   
          }
          else if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_CELL_UPDATE_CONFIRM) ||
                  (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_URA_UPDATE_CONFIRM))
	  {
	    rrccu_procedure_event_handler(cmd_ptr);
	  }
          else
          {
            MSG_LOW("Dispatching RRC_CMAC_STATUS_IND to LLC",0,0,0);
            rrcllc_process_cmd( cmd_ptr );
          }
          break;

        case RRC_CMAC_MEAS_CTRL_CNF:
          MSG_LOW("Dispatching RRC_CMAC_MEAS_CTRL_CNF to Meas",0,0,0);
          rrcmeas_procedure_event_handler( cmd_ptr );
          break;

        case RRC_CMAC_UL_TFC_SUBSET_CFG_CNF:
          rrctfc_procedure_event_handler( cmd_ptr );
          break;


        default:
          /* This should not happen. Possibly the command id was never written 
          before this item was put in the queue. Display error for now. */ 
          WRRC_MSG1_ERROR( "Unknown mac cmd rcvd by RRC 0x%x", 
                cmd_ptr->cmd_hdr.cmd_id);
          break;
      }/* end switch */
    }/* end if */
    else
    {
      WRRC_MSG0_HIGH("Throw away the MAC command");
    }
    /* The command has been processed. We can free the memory for this cmd */
    rrc_free_cmd_buf( cmd_ptr );
  }/* end while */
}/* end function */



/*===========================================================================

FUNCTION  RRC_DISPATCH_RLC_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's rlc
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_rlc_commands( void )
{
  
  rrc_cmd_type *cmd_ptr;  /* To hold received command */

  /* We need to dequeue items from the rlc
  command queue and process them till the queue is
  empty */

  while( (cmd_ptr = q_get(&rrc_rlc_cmd_q)) != NULL)
  {
    WRRC_MSG1_HIGH_OPT("Dispatching RRC Ext RLC cmd rrc_cmd_e_type_value_0x%x",cmd_ptr->cmd_hdr.cmd_id);
    /* Check if the incoming internal command is required a further process */
    if (rrcmcm_is_process_required(cmd_ptr) == FALSE) 
    {
      /* Display a XDM message and disgard the command */
      WRRC_MSG1_HIGH("CMD: %d is disgarded!",cmd_ptr->cmd_hdr.cmd_id);
    }
#ifdef FEATURE_DUAL_SIM
    else if(rrcwrm_is_process_required(cmd_ptr) == FALSE)
    {
      WRRC_MSG1_HIGH("DSIM_TUNEAWAY: CMD %d is Saved !",cmd_ptr->cmd_hdr.cmd_id);
      continue;
    }
#endif
    else if(rrc_deact_status != RRC_DEACT_COMPLETED)
    {
      /* Save Crash debug info */
      rrc_crash_debug_save_l2_to_rrc_cmds(cmd_ptr);
    
      /* Switch on the command id of the received command, and
       * call the appropriate function to process each command. 
       */
      switch( cmd_ptr->cmd_hdr.cmd_id )
      {
        case RRC_CRLC_DL_TM_CONFIG_CNF:
          MSG_LOW("Dispatching RRC_CRLC_TM_DL_CONFIG_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CRLC_UL_TM_CONFIG_CNF:
          MSG_LOW("Dispatching RRC_CRLC_TM_UL_CONFIG_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CRLC_DL_UM_CONFIG_CNF:
          MSG_LOW("Dispatching RRC_CRLC_UM_DL_CONFIG_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CRLC_UL_UM_CONFIG_CNF:
          MSG_LOW("Dispatching RRC_CRLC_UM_UL_CONFIG_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CRLC_AM_CONFIG_CNF:
          MSG_LOW("Dispatching RRC_CRLC_AM_CONFIG_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CRLC_RESUME_CNF:
          MSG_LOW("Dispatching RRC_CRLC_RESUME_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CRLC_STATUS_IND:
          MSG_LOW("Dispatching RRC_CRLC_STATUS_IND to LLC",0,0,0);
          rrccu_procedure_event_handler( cmd_ptr );
          break;

        case RRC_CRLC_UL_HFN_CNF:
          MSG_LOW("Dispatching RRC_CRLC_UL_HFN_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CRLC_DL_HFN_CNF:
          MSG_LOW("Dispatching RRC_CRLC_DL_HFN_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;

        case RRC_CRLC_RELEASE_ALL_CNF:
          MSG_LOW("Dispatching RRC_CRLC_RELEASE_ALL_CNF to LLC",0,0,0);
          rrcllc_process_cmd( cmd_ptr );
          break;
#ifdef FEATURE_QCHAT_HFN_MISMATCH_RECOVERY
        case RRC_CRLC_PDU_INVALID_IND:
          WRRC_MSG0_HIGH("Dispatching RRC_CRLC_PDU_INVALID_IND to MISC");
          rrcmisc_procedure_event_handler( cmd_ptr );
          break;
#endif

        default:
          /* This should not happen. Possibly the command id was never written 
          before this item was put in the queue. Display error for now. */ 
          WRRC_MSG1_ERROR( "Unknown rlc cmd rcvd by RRC 0x%x", 
                cmd_ptr->cmd_hdr.cmd_id);
          break;
      }/* end switch */
    } /* end if */
    else
    {
      WRRC_MSG0_HIGH("Throw away the RLC command");
    }
    /* The command has been processed. We can free the memory for this cmd */
    rrc_free_cmd_buf( cmd_ptr );
    
  }/* end while */
}/* end function */



/*===========================================================================

FUNCTION  RRC_DISPATCH_MM_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's mm
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_mm_commands( void )
{
  
  rrc_cmd_type *cmd_ptr;   /* To hold received command */

#ifdef FEATURE_TRIPLE_SIM  
  rrc_state_e_type rrc_state = rrc_get_state();
#endif

  /* We need to dequeue items from the mm
  command queue and process them till the queue is
  empty */

  MSG_MED("Dispatching MM Commands",0,0,0);
  while( (cmd_ptr = q_get(&rrc_mm_cmd_q)) != NULL)
  {
    WRRC_MSG1_HIGH_OPT("Dispatching RRC Ext MM cmd rrc_cmd_e_type_value_0x%x",cmd_ptr->cmd_hdr.cmd_id);
    /* Check if the incoming internal command is required a further process */
    if (rrcmcm_is_process_required(cmd_ptr) == FALSE) 
    {
      /* Display a XDM message and disgard the command */
      WRRC_MSG1_HIGH("CMD: %d is disgarded!",cmd_ptr->cmd_hdr.cmd_id);
    }
    else if(rrc_deact_status == RRC_NORMAL_OPERATION)
    {
      if(!(rrc_disable_crash_debug_info_nv & NV_CRASH_DEBUG_INFO_DISABLE))
      {
        /* Save Crash debug info */
        rrc_crash_debug_set_mm_to_rrc_cmd_id(cmd_ptr);
      }
#ifdef FEATURE_QSH_EVENT_METRIC
      wrrc_qsh_log_mm_rrc_cmd_params(cmd_ptr);
#endif
      /* Switch on the command id of the received command, and
       * call the appropriate function to process each command. 
       */
#ifdef FEATURE_DUAL_SIM
      if(rrc_check_mm_cmds_for_simid_validation(cmd_ptr) == FAILURE)
      {
        ERR_FATAL("DSIM: Invalid SIM ID in MM cmd %x",cmd_ptr->cmd_hdr.cmd_id,0,0);
      }
#endif
      switch( cmd_ptr->cmd_hdr.cmd_id )
      {
        case RRC_SERVICE_REQ:
          WRRC_MSG0_HIGH("Recd: RRC_SERVICE_REQ");

          if(search_order_specialization  == SYS_SPECIAL_SBM_RAT_SEARCH_ORDER)
          {
            WRRC_MSG0_HIGH("Search Order Specialization: Initializing LCF");
            rrc_csp_int_data.last_camped_freq_in_idle_mode = 0;
            rrc_csp_int_data.last_camped_freq_in_connected_mode = 0;
          }
#ifdef FEATURE_TRIPLE_SIM
          /* For cell selection scan set the alternage page flag */
          rrcwrm_set_trm_extension_flag_for_alternate_page(TRUE);
#endif
          rrcmcm_event_handler( cmd_ptr );
          break;

        case RRC_STOP_WCDMA_MODE_REQ:
          WRRC_MSG0_HIGH("Recd: RRC_STOP_WCDMA_MODE_REQ");
          rrcmcm_event_handler( cmd_ptr );
          break;

        case RRC_DEACT_REQ:
          WRRC_MSG0_HIGH("Recd: RRC_DEACT_REQ");
          rrcmcm_event_handler( cmd_ptr );
          break;

        case RRC_MODE_CHANGE_REQ:
          WRRC_MSG0_HIGH("Recd: RRC_MODE_CHANGE_REQ");
          rrcmcm_event_handler( cmd_ptr );
          break;

        case RRC_CHANGE_MODE_IND:
          WRRC_MSG0_HIGH("Recd: RRC_CHANGE_MODE_IND");
          rrcmcm_event_handler( cmd_ptr );
          break;

        case RRC_ACT_REQ:
        case RRC_FORBIDDEN_LAI_LIST_UPDATE_REQ:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_PLMN_LIST_REQ:
          WRRC_MSG0_HIGH("Rx: RRC_PLMN_LIST_REQ");
#ifdef FEATURE_TRIPLE_SIM
          if(!((rrc_state == RRC_STATE_CELL_DCH) || (rrc_state == RRC_STATE_CELL_FACH)))
          {
            /* For BPLMN cases set the alternage page flag */
            rrcwrm_set_trm_extension_flag_for_alternate_page(TRUE);
          }
#endif
          rrcmcm_event_handler( cmd_ptr );
          break;

        case RRC_BPLMN_SEARCH_ABORT_REQ:
          WRRC_MSG2_HIGH("Rx: RRC_BPLMN_SEARCH_ABORT_REQ in CSP sstate %d curr_scan %d",
                          rrc_get_csp_substate(), rrc_csp_int_data.curr_scan);
          rrc_cell_selection_procedure( cmd_ptr );
          break;
        
        case RRC_EST_REQ:
          WRRC_MSG1_HIGH("Rcvd RRC_EST_REQ from MM with cause:%d",
                           cmd_ptr->cmd.rrc_est_req.cause);
          rrcrce_procedure_event_handler( cmd_ptr );
          break;
        
        case RRC_ABORT_REQ:
          WRRC_MSG2_HIGH("Rx: RRC_ABORT_REQ. Hard abort flag is %d Abort Cause is %d", 
                         cmd_ptr->cmd.rrc_abort_req.hard_abort, cmd_ptr->cmd.rrc_abort_req.abort_cause);
          rrciho_event_handler( cmd_ptr );
          break;

        case RRC_OPEN_SESSION_REQ:
          rrcidt_procedure_event_handler( cmd_ptr );
          break;
        
        case RRC_DATA_REQ:
          rrcudt_procedure_event_handler( cmd_ptr );
          break;
        
        case RRC_CLOSE_SESSION_REQ:
          rrcscrr_procedure_event_handler( cmd_ptr );
          break;
        
        case RRC_SIM_INSERTED_REQ:
          rrcmisc_procedure_event_handler( cmd_ptr );
          break;

        case RRC_SIM_UPDATE_REQ:
          rrcmisc_procedure_event_handler( cmd_ptr );
          break;

#ifdef FEATURE_INTERRAT_CELL_RESELECTION_GTOW
        case RRC_ACTIVATION_RSP:
          WRRC_MSG0_HIGH("Recd: RRC_ACTIVATION_RSP");
          rrcmcm_event_handler( cmd_ptr );
          break;
#endif

      case RRC_EQ_PLMN_LIST_CHANGE_IND:
          WRRC_MSG0_HIGH("Recd: RRC_EQ_PLMN_LIST_CHANGE_IND");     
          rrcmeas_procedure_event_handler( cmd_ptr );
          rrc_cell_selection_procedure( cmd_ptr );

        break;
        case RRC_NW_SEL_MODE_RESET_IND:
          rrc_cell_selection_procedure( cmd_ptr );
          break;

#ifdef FEATURE_OOSC_USER_ACTION
        case RRC_EXIT_FROM_PWR_SAVE:
          WRRC_MSG0_HIGH("Key press event in connected mode");
          rrc_cell_selection_procedure( cmd_ptr );
          break;
#endif /*FEATURE_OOSC_USER_ACTION */

#ifdef FEATURE_DUAL_SIM
        case RRC_DS_STATUS_CHANGE_IND:
          WRRC_MSG0_HIGH("DSIM: RRC_DS_STATUS_CHANGE_IND Recvd");
          rrc_cell_selection_procedure( cmd_ptr );
          break;

        case RRC_DS_TUNEAWAY_STATUS_CHANGE_IND:
          WRRC_MSG0_HIGH("DSIM_Tuneaway: RRC_DS_TUNEAWAY_STATUS_CHANGE_IND Recvd");
          rrc_cell_selection_procedure( cmd_ptr );
          break;
#endif

        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_procedure_event_handler(cmd_ptr );
          break;

#ifdef FEATURE_3GPP_CSFB
        case RRC_CSFB_CALL_STATUS_IND:
          WRRC_MSG0_HIGH("CSFB: RRC_CSFB_CALL_STATUS_IND Recvd");
          rrc_cell_selection_procedure(cmd_ptr);
          break;
#endif

        case RRC_LTE_RESELECTION_ALLOWED_STATUS_IND:
          WRRC_MSG0_HIGH("RRC_LTE_RESELECTION_ALLOWED_STATUS_IND is Recvd");
          rrc_cell_selection_procedure(cmd_ptr);
          break;

        case RRC_INVALIDATE_SIM_DATA_REQ:
          WRRC_MSG0_HIGH("Received SIM INVALIDATE REQ");
          /* Resetting the IMSI */
          memset(&rrc_imsi,0, sizeof(rrc_imsi_gsm_map_type));
          rrc_imsi_valid =FALSE;
          break;

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
        case RRC_LAI_REJECT_LIST_IND:
          WRRC_MSG0_HIGH("FF: RRC_LAI_REJECT_LIST_IND Recvd");
          rrc_cell_selection_procedure(cmd_ptr);
          break;

        case RRC_TUNEAWAY_ALLOWED_IND:
          WRRC_MSG0_HIGH("FF: RRC_TUNEAWAY_ALLOWED_IND Recvd");
          rrc_cell_selection_procedure(cmd_ptr);
          break;
#endif

        case RRC_SET_DRX_IND:
          WRRC_MSG0_HIGH("QCHAT_DRX: RRC_SET_DRX_IND is Recvd");
          rrcmisc_procedure_event_handler(cmd_ptr);
          break;

#ifdef FEATURE_FEMTO_CSG
        case RRC_CSG_WHITE_LIST_UPDATE_IND:
          WRRC_MSG0_HIGH("CSG: Received white list update indication from NAS");
          rrcmisc_procedure_event_handler(cmd_ptr);
          break;
#endif
#ifdef FEATURE_DUAL_SIM
        case RRC_MULTIMODE_SUBS_CHGD_IND:
        /*This command is already handled in rrc_check_mm_cmds_for_simid_validation()
          breaking from here to avoid default case*/
          break;
#endif
        case RRC_PSEUDO_LTE_LIST_UPDATE_IND:
          WRRC_MSG0_HIGH("PS_SIB19:RRC_PSEUDO_LTE_LIST_UPDATE_IND received from NAS ");
          /*Free the stored SIB19 so that new SIB19 will get generated when requested*/
          rrcsibdb_free_pseudo_sib19();
          rrcmcm_event_handler(cmd_ptr);
          break;
        default:
          /* This should not happen. Possibly the command id was never written 
          before this item was put in the queue. Display error for now. */ 
          WRRC_MSG1_ERROR( "Unknown mm cmd rcvd by RRC 0x%x", 
                cmd_ptr->cmd_hdr.cmd_id);
          break;
      }/* end switch */
    }/* end if */
    else if(rrc_deact_status == RRC_DEACT_COMPLETED)
    {
      /*Code for handling subsequent deact req when deact cnf for 1st deact req is already sent*/
      switch( cmd_ptr->cmd_hdr.cmd_id )
      {
        case RRC_DEACT_REQ:
          rrccsp_force_send_deact_cnf();
          break;
        default :
          WRRC_MSG0_HIGH("Throw away the MM command");
          break;
      }
    }
    else
    {
      WRRC_MSG0_HIGH("Throw away the MM command");
    }

    /* The command has been processed. We can free the memory for this cmd */
    rrc_free_cmd_buf( cmd_ptr );
  }/* end while */
}/* end function */


/*===========================================================================

FUNCTION  RRC_DISPATCH_TC_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's mm
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

static void rrc_dispatch_tc_commands( void )
{
  
  rrc_cmd_type *cmd_ptr;   /* To hold received command */

  /* We need to dequeue items from the mm
  command queue and process them till the queue is
  empty */

  MSG_MED("Dispatching TC Commands",0,0,0);
  while( (cmd_ptr = q_get(&rrc_tc_cmd_q)) != NULL)
  {
    WRRC_MSG1_HIGH_OPT("Dispatching RRC Ext TC cmd rrc_cmd_e_type_value_0x%x",cmd_ptr->cmd_hdr.cmd_id);
    /* Check if the incoming internal command is required a further process */
    if (rrcmcm_is_process_required(cmd_ptr) == FALSE) 
    {
      /* Display a XDM message and disgard the command */
      WRRC_MSG1_HIGH("CMD: %d is disgarded!",cmd_ptr->cmd_hdr.cmd_id);
    }
    else if(rrc_deact_status == RRC_NORMAL_OPERATION)
    {
      rrclbt_procedure_event_handler(cmd_ptr);
    }/* end if */
    else
    {
      WRRC_MSG0_HIGH("Throw away the TC command");
    }

    /* The command has been processed. We can free the memory for this cmd */
    rrc_free_cmd_buf( cmd_ptr );
  }/* end while */

}/* end function */

/*===========================================================================

FUNCTION  RRC_DISPATCH_RR_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's RR
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_dispatch_rr_commands( void )
{
  
  rrc_rr_cmd_type *cmd_ptr;   /* To hold received command */

  /* We need to dequeue items from the RR
  command queue and process them till the queue is
  empty */

  MSG_MED("Dispatching RR Commands",0,0,0);

  while( (cmd_ptr = q_get(&rrc_rr_cmd_q)) != NULL)
  {
    WRRC_MSG1_HIGH_OPT("Dispatching RRC ext RR cmd rrc_cmd_e_type_value_0x%x",cmd_ptr->cmd_hdr.cmd_id );
    /*lint -e740*/
    /* Check if the incoming internal command is required a further process */
    if (rrcmcm_is_process_required((rrc_cmd_type *)cmd_ptr) == FALSE) 
    {
      /* Display a XDM message and disgard the command */
      WRRC_MSG1_HIGH("CMD: %d is disgarded!",cmd_ptr->cmd_hdr.cmd_id);
    }
    /*lint +e740*/
    else
    {
#ifdef FEATURE_DUAL_SIM
      rrc_check_rr_cmds_for_simid_validation(cmd_ptr);
#endif
      if(rrc_deact_status == RRC_NORMAL_OPERATION)
      {
        if(!(rrc_disable_crash_debug_info_nv & NV_CRASH_DEBUG_INFO_DISABLE))
        {
          /*Save Crash debug info */
          rrc_crash_debug_set_rr_to_rrc_cmd_id(cmd_ptr->cmd_hdr.cmd_id);
        }
#ifdef FEATURE_QSH_EVENT_METRIC
        wrrc_qsh_log_rr_rrc_cmd_params(cmd_ptr);
#endif 
        /* Switch on the command id of the received command, and
           call the appropriate function to process each command. */
        switch(cmd_ptr->cmd_hdr.cmd_id)
        {
          case RR_INTERRAT_RESELECTION_REJ:
          case RR_INTERRAT_RESELECTION_ABORT_CNF:
            /* Forward to CSP to process the cmd */
            /*lint -e740*/
            rrc_cell_selection_procedure((rrc_cmd_type *)cmd_ptr);
            /*lint +e740*/
            break;

          case RR_INTERRAT_CC_ORDER_REJ:
            /* Forward to CHO to process the cmd */
            /*lint -e740*/
            rrccho_procedure_event_handler((rrc_cmd_type *)cmd_ptr);
            /*lint +e740*/
            break;

#ifdef FEATURE_WCDMA_TABASCO_MODEM 
          case RR_INTERRAT_CC_ORDER_ABORT_CNF:
          case RRC_GRR_INTERRAT_PROC_ABORT_IND:
#endif
          case RR_INTERRAT_CC_ORDER_CNF:
            /* Forward to CHO to process the cmd */
            /*lint -e740*/
            rrccho_procedure_event_handler((rrc_cmd_type *)cmd_ptr);
            /*lint +e740*/
            break;

          case RRC_INTERRAT_RESELECTION_REQ:
          case RRC_INTERRAT_RESELECTION_ABORT_REQ:
            /* Forward the command to MCM for processing */
            /*lint -e740*/
            rrcmcm_update_dual_mode_enabled_status(TRUE);
            rrcmcm_event_handler((rrc_cmd_type *)cmd_ptr);
            /*lint +e740*/
            break;
    
          case RR_INTERRAT_HANDOVER_CNF:
          case RR_INTERRAT_HANDOVER_ABORT_CNF:
    
          case RRC_INTERRAT_HANDOVER_ABORT_REQ:
          case RRC_INTERRAT_HANDOVER_REQ:
            /* forward the reset commands to IHO for now */ 
            /*lint -e740*/
            rrcmcm_update_dual_mode_enabled_status(TRUE);
            rrciho_event_handler((rrc_cmd_type *)cmd_ptr);
            /*lint +e740*/
            break;

          case RRC_NAS_DATA_IND:
            /*lint -e740*/
            rrcudt_procedure_event_handler((rrc_cmd_type *) cmd_ptr );
            /*lint +e740*/
            break;
          case RRC_INTERRAT_CC_ORDER_REQ:
          case RRC_INTERRAT_CC_ORDER_ABORT_REQ:
            /*lint -e740*/
            rrccho_procedure_event_handler((rrc_cmd_type *) cmd_ptr );
            /*lint +e740*/
            break;

          case RRC_INTERRAT_PLMN_SRCH_REQ:
          case RRC_INTERRAT_PLMN_SRCH_ABORT_REQ:
            /* Forward the command to MCM for processing */
            /*lint -e740*/
            rrcmcm_update_dual_mode_enabled_status(TRUE);
            rrcmcm_event_handler((rrc_cmd_type *)cmd_ptr);
            /*lint +e740*/
            break;

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
          case RR_INTERRAT_REDIRECT_REJ:
          case RR_INTERRAT_REDIRECT_ABORT_CNF:
            /* Forward to CSP to process the cmd */
            /*lint -e740*/
            rrc_cell_selection_procedure((rrc_cmd_type *)cmd_ptr);
            /*lint +e740*/
            break;
#endif

          case RR_INTERRAT_PLMN_SRCH_CNF:
          case RR_INTERRAT_PLMN_SRCH_ABORT_CNF:
            /* Forward to CSP to process the cmd */
            /*lint -e740*/
            rrc_cell_selection_procedure((rrc_cmd_type *)cmd_ptr);
            /*lint +e740*/
            break;

          case RR_CIPHER_SYNC_IND:
            rrcsmc_procedure_event_handler((rrc_cmd_type *)cmd_ptr );
            break;

#ifdef FEATURE_GAN
          #error code not present
#endif

          case RRC_INTERRAT_REDIRECT_REQ:
          case RRC_INTERRAT_REDIRECT_ABORT_REQ:
            /* Forward the command to MCM for processing */
            rrcmcm_update_dual_mode_enabled_status(TRUE);
            rrcmcm_event_handler((rrc_cmd_type *)cmd_ptr);
            break;			
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
          #error code not present
#endif
#ifdef FEATURE_GAN_3G
          #error code not present
#endif
          case RR_RRC_STOP_GSM_CNF:
            rrc_cell_selection_procedure((rrc_cmd_type *)cmd_ptr);
            break;			
#ifdef FEATURE_WCDMA_DEDICATED_PRI_INFO
          case RR_DEDICATED_PRIORITIES_RSP:
           /*receive Dedicated priorities from RR*/
           rrcmeas_procedure_event_handler((rrc_cmd_type *)cmd_ptr);
           break;		   
          case RRC_DEDICATED_PRIORITIES_REQ:
            rrcmeas_procedure_event_handler((rrc_cmd_type *)cmd_ptr);
            break;			
          case RRC_CLEAR_DEDICATED_PRIORITIES_IND:
            rrcmeas_procedure_event_handler((rrc_cmd_type *)cmd_ptr);
            break;
#endif
           default:
             /* Unexpected RR command */       
             WRRC_MSG1_HIGH("Unknown RR Command: %d", cmd_ptr->cmd_hdr.cmd_id);
             break;
        }
      } /* end if */
      else if((rrc_deact_status == RRC_DEACT_COMPLETED)
                 && (cmd_ptr->cmd_hdr.cmd_id == RR_RRC_STOP_GSM_CNF))
      {
        WRRC_MSG1_HIGH("RR_RRC_STOP_GSM_CNF received", cmd_ptr->cmd_hdr.cmd_id);
        rrc_cell_selection_procedure((rrc_cmd_type *)cmd_ptr);
      }
      else
      {
        WRRC_MSG0_HIGH("Throw away the RR command");
      }
    }
    /* The command has been processed. We can free the memory for this cmd */

    rrc_free_rr_cmd_buf( cmd_ptr );
  }/* end while */
}/* end function */

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
/*===========================================================================

FUNCTION  RRC_DISPATCH_LSM_COMMANDS

DESCRIPTION

  This function dispatches all commands received in RRC's lsm
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_lsm_commands( void )
{
  rrc_cmd_type *cmd_ptr;   
  MSG_MED("Dispatching LSM Commands",0,0,0);
  
  while( (cmd_ptr = q_get(&rrc_lsm_cmd_q)) != NULL)
  {
    WRRC_MSG1_HIGH_OPT("Dispatching RRC ext LSM cmd rrc_cmd_e_type_value_0x%x",cmd_ptr->cmd_hdr.cmd_id );
    
  /* Switch on the command id of the received command, and
    call the appropriate function to process each command. */
    if (rrcmcm_is_process_required(cmd_ptr) == FALSE) 
    {
      /* Display a XDM message and disgard the command */
      WRRC_MSG1_HIGH("CMD: %d is disgarded!",cmd_ptr->cmd_hdr.cmd_id);
    }
    else if(rrc_deact_status == RRC_NORMAL_OPERATION)
    {
      switch( cmd_ptr->cmd_hdr.cmd_id )
      {
      case RRC_MEASUREMENT_IND:
        rrcmeas_procedure_event_handler( cmd_ptr );
        break;

      default:
      /* This should not happen. Possibly the command id was never written
        before this item was put in the queue. Display error for now. */
        WRRC_MSG1_ERROR( "Unknown mm cmd rcvd by RRC 0x%x",
          cmd_ptr->cmd_hdr.cmd_id);
        break;
      }/* end switch */
      
    }/* end if */
    else
    {
      WRRC_MSG0_HIGH("Throw away the LSM command");
    }
    /* The command has been processed. We can free the memory for this cmd */
    rrc_free_cmd_buf( cmd_ptr );
  }/* end while */
}/* end function */

#endif

/*===========================================================================

FUNCTION  RRC_DISPATCH_CM_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's cm
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. 
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_cm_commands( void )
{
  
  rrc_cmd_type *cmd_ptr;   /* To hold received command */

  /* We need to dequeue items from the cm
  ** command queue and process them till the queue is
  ** empty 
  */

  MSG_MED("Dispatching CM Commands",0,0,0);

  while( (cmd_ptr = q_get(&rrc_cm_cmd_q)) != NULL)
  {
    WRRC_MSG1_HIGH_OPT("Dispatching RRC ext CM cmd rrc_cmd_e_type_value_0x%x",cmd_ptr->cmd_hdr.cmd_id );
    /* Switch on the command id of the received command, and
    * call the appropriate function to process each command. 
    */
    switch( cmd_ptr->cmd_hdr.cmd_id )
    {
      #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
      #error code not present
#endif

      /*CM will request RRC to read CID in DCH state*/
      case RRC_CM_DCH_CELL_ID_REQUEST:
        rrcgps_procedure_event_handler( cmd_ptr );

        break;

      default:
        WRRC_MSG1_ERROR( "Unknown mm cmd rcvd by RRC 0x%x", 
              cmd_ptr->cmd_hdr.cmd_id);
        break;

    }/* end switch */


    /* The command has been processed. We can free the memory for this cmd.
    */
    rrc_free_cmd_buf( cmd_ptr );

  }/* end while */

}/* rrc_dispatch_cm_commands() */


#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION  RRC_CHECK_LTE_CMDS_FOR_SIMID_VALIDATION

DESCRIPTION
 
  This function validates SIM id for LTE commands.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None
  
SIDE EFFECTS

  None.

===========================================================================*/

static void rrc_check_lte_cmds_for_simid_validation(wcdma_rrc_lte_irat_cmd_type *cmd_ptr)
{
  sys_modem_as_id_e_type sim_id_recvd;

  sim_id_recvd = (int8)(INST_ID_TO_SYS_AS_ID(MSGR_GET_INST(&cmd_ptr->cmd.msg_hdr)));

  if(!rrc_is_sim_id_valid)
  {
    WRRC_MSG0_HIGH("DSIM: RRC not yet updated with SIM details, Treat as SIM Match");
    return;
  }

  if(rrc_sim_id_recvd != sim_id_recvd)
  {
    WRRC_MSG2_ERROR("INvalid sub-id %d passed. internal %d",sim_id_recvd,rrc_sim_id_recvd);
    return;
  }
  return;
}
#endif

/*===========================================================================

FUNCTION  RRC_DISPATCH_LTE_RRC_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's LTE RRC
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_dispatch_lte_rrc_commands( void )
{
  wcdma_rrc_lte_irat_cmd_type *lte_cmd_ptr;   /* To hold received command */
  rrc_cmd_type *rrc_cmd = rrc_malloc(sizeof(rrc_cmd_type));

  while( (lte_cmd_ptr = q_get(&rrc_lte_cmd_q)) != NULL)
  {
    rrc_cmd->cmd_hdr.cmd_id = RRC_LTE_RRC_CMD;
    WCDMA_MEMCPY(&(rrc_cmd->cmd.wcdma_rrc_lte_irat_cmd),
                 sizeof(wcdma_rrc_lte_irat_cmd_type),
                 lte_cmd_ptr,
                 sizeof(wcdma_rrc_lte_irat_cmd_type));

    WRRC_MSG1_HIGH_OPT("Dispatching RRC ext LTE cmd wcdma_lte_event_umid_value_0x%x",lte_cmd_ptr->cmd.msg_hdr.id );
    /* Check if the incoming internal command is required a further process */
    if (rrcmcm_is_process_required(rrc_cmd) == FALSE) 
    {
      WRRC_MSG1_HIGH("LTE_IRAT:CMD: %x is disgarded!",rrc_cmd->cmd.wcdma_rrc_lte_irat_cmd.cmd.msg_hdr.id);
    }
    else
    {
#ifdef FEATURE_DUAL_SIM
      rrc_check_lte_cmds_for_simid_validation(lte_cmd_ptr);
#endif
      if(rrc_deact_status == RRC_NORMAL_OPERATION)
      {
        /*Save LTE crash debug commands */
        rrc_crash_debug_save_lte_to_rrc_cmds(lte_cmd_ptr->cmd.msg_hdr.id);
#ifdef FEATURE_QSH_EVENT_METRIC
        wrrc_qsh_log_lte_rrc_cmd_params(lte_cmd_ptr);
#endif
        switch(lte_cmd_ptr->cmd.msg_hdr.id)
        {
          case WCDMA_RRC_LTE_RESEL_REQ:
          case WCDMA_RRC_LTE_ABORT_RESEL_REQ:
          case WCDMA_RRC_LTE_PLMN_SRCH_REQ:
          case WCDMA_RRC_LTE_ABORT_PLMN_SRCH_REQ:
          case WCDMA_RRC_LTE_REDIR_REQ:
          case WCDMA_RRC_LTE_ABORT_REDIR_REQ:
            //CGI:
          case WCDMA_RRC_LTE_GET_CGI_REQ:
          case WCDMA_RRC_LTE_ABORT_CGI_REQ:
            rrcmcm_event_handler(rrc_cmd);
            break;
          case WCDMA_RRC_LTE_GET_PLMN_PRTL_RESULTS_REQ:
            rrc_cell_selection_procedure(rrc_cmd);
            break;
          case LTE_RRC_WCDMA_RESEL_FAILED_RSP:
          case LTE_RRC_WCDMA_ABORT_RESEL_RSP:
          case LTE_RRC_WCDMA_PLMN_SRCH_RSP:
          case LTE_RRC_WCDMA_ABORT_PLMN_SRCH_RSP:
          case LTE_RRC_WCDMA_REDIR_FAILED_RSP:
          case LTE_RRC_WCDMA_ABORT_REDIR_RSP:
          case WCDMA_RRC_LTE_PLMN_SRCH_SUSPEND_IND:
#ifdef FEATURE_FEMTO_CSG
          case LTE_RRC_WCDMA_RESEL_RSP:
#endif
            rrc_cell_selection_procedure(rrc_cmd);
            break;			
          case LTE_RRC_EUTRA_CAPABILITIES_RSP:
            rrcueci_procedure_event_handler(rrc_cmd);
            break;			
          case WCDMA_RRC_LTE_UTRA_CAPABILITIES_REQ:
            rrciho_event_handler(rrc_cmd);
            break;		
          case WCDMA_RRC_LTE_GET_DEDICATED_PRI_REQ:
          case WCDMA_RRC_LTE_CLEAR_DEDICATED_PRI_IND:
          case LTE_RRC_GET_DEDICATED_PRI_RSP:
          case WCDMA_RRC_LTE_DEPRI_FREQ_REQ:
            rrcmeas_procedure_event_handler(rrc_cmd);
            break;

#ifdef FEATURE_LTE_TO_WCDMA
          case WCDMA_RRC_LTE_PSHO_REQ:
          case WCDMA_RRC_LTE_ABORT_PSHO_REQ:
            rrciho_event_handler(rrc_cmd);
            break;
#endif
#ifdef FEATURE_WCDMA_TO_LTE
          case LTE_RRC_WCDMA_PSHO_RSP:
          case LTE_RRC_WCDMA_ABORT_PSHO_RSP:
            rrciho_event_handler(rrc_cmd);
            break;
#endif
          case LTE_RRC_UTRA_SERVING_CELL_INFO_REQ:
            rrcmcm_event_handler(rrc_cmd);
            break;

          default :
            WRRC_MSG1_HIGH("LTE_IRAT: ID %xnot processed",lte_cmd_ptr->cmd.msg_hdr.id);
            break;
        }
      } 
      else
      {
        WRRC_MSG0_HIGH("LTE_IRAT:: Throw away the LTE RRC command");
      }
    }
    /*Put the cmd back to free queue*/
    q_put(&rrc_lte_free_cmd_q,&(lte_cmd_ptr->link));
  }
rrc_free(rrc_cmd);
}
#endif


/*===========================================================================

FUNCTION  RRC_CHECK_QMI_CMDS_FOR_SIMID_VALIDATION

DESCRIPTION
 
  This function validates SIM id for QMI commands.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None
  
SIDE EFFECTS

  None.

===========================================================================*/

static void rrc_check_qmi_cmds_for_simid_validation(wcdma_rrc_qmi_cmd_type *cmd_ptr)
{
  sys_modem_as_id_e_type sim_id_recvd;

  sim_id_recvd = (int8)(INST_ID_TO_SYS_AS_ID(MSGR_GET_INST(&cmd_ptr->cmd.msg_hdr)));

  if(!rrc_is_sim_id_valid)
  {
    WRRC_MSG0_HIGH("DSIM: RRC not yet updated with SIM details, Treat as SIM Match");
    return;
  }

  if(rrc_sim_id_recvd != sim_id_recvd)
  {
    WRRC_MSG2_ERROR("INvalid sub-id %d passed. internal %d",sim_id_recvd,rrc_sim_id_recvd);
    return;
  }
  return;
}

/*===========================================================================

FUNCTION  RRC_DISPATCH_QMI_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's QMI
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_dispatch_qmi_commands( )
{
  wcdma_rrc_qmi_cmd_type *qmi_cmd_ptr;   /* To hold received command */
  rrc_cmd_type *rrc_cmd = rrc_malloc(sizeof(rrc_cmd_type));

  while((qmi_cmd_ptr = q_get(&rrc_qmi_cmd_q)) != NULL)
  {
    rrc_cmd->cmd_hdr.cmd_id = RRC_QMI_CMD;
    WCDMA_MEMCPY(&(rrc_cmd->cmd.wcdma_rrc_qmi_cmd),
                 sizeof(wcdma_rrc_qmi_cmd_type),
                 qmi_cmd_ptr,
                 sizeof(wcdma_rrc_qmi_cmd_type));

    WRRC_MSG1_HIGH("Dispatching RRC ext QMI cmd wcdma_lte_event_umid_value_0x%x",qmi_cmd_ptr->cmd.msg_hdr.id);
    /* Check if the incoming internal command is required a further process */
    if (rrcmcm_is_process_required(rrc_cmd) == FALSE) 
    {
      WRRC_MSG0_HIGH("QMI:command is disgarded!");
    }
    else
    {
#ifdef FEATURE_DUAL_SIM
      rrc_check_qmi_cmds_for_simid_validation(qmi_cmd_ptr);
#endif
      if(rrc_deact_status == RRC_NORMAL_OPERATION)
      {
        switch(qmi_cmd_ptr->cmd.msg_hdr.id)
        {
          case WCDMA_RRC_QMI_MTC_CFG_REQ:
            /*Process this command only when UE is in Single SIM mode and M2M NV is enabled.*/
            if(!wcdma_is_ue_in_dual_standby && rrc_m2m_feature_enabled)
            {
               rrcmeas_process_qmi_mtc_cfg_req(rrc_cmd);
            }
            else
            {
              wcdma_rrc_qmi_mtc_cfg_cnf_type qmi_cfg_rsp;
              qmi_cfg_rsp.status = FAILURE;

              msgr_init_hdr(&qmi_cfg_rsp.msg_hdr,MSGR_WCDMA_RRC,WCDMA_RRC_QMI_MTC_CFG_RSP);
              msgr_send(&qmi_cfg_rsp.msg_hdr, sizeof(wcdma_rrc_qmi_mtc_cfg_cnf_type));
              WRRC_MSG2_HIGH("WCDMA_RRC_QMI_MTC_CFG_REQ not processed, sent failure to QMI. UE in DSDS mode %d, M2M NV# 73776 status %d",wcdma_is_ue_in_dual_standby,rrc_m2m_feature_enabled);
            }
            break;

          default :
            WRRC_MSG1_HIGH("QMI: ID %x not processed",qmi_cmd_ptr->cmd.msg_hdr.id);
            break;
        }
      } 
      else
      {
        WRRC_MSG0_HIGH("QMI: Throw away the QMI command");
      }
    }
    /*Put the cmd back to free queue*/
    q_put(&rrc_qmi_free_cmd_q,&(qmi_cmd_ptr->link));
  }
rrc_free(rrc_cmd);
}



#if defined (FEATURE_TDSCDMA_TO_WCDMA) || defined (FEATURE_WCDMA_TO_TDSCDMA)
/*===========================================================================

FUNCTION  RRC_DISPATCH_TDSRRC_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's TRRC
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_dispatch_tdsrrc_commands( void )
{
  rrc_cmd_type *cmd_ptr;       /* To hold received command */

  /* We need to dequeue items from the tds rrc
  command queue and process them till the queue is
  empty */

  MSG_MED("Dispatching TDS RRC Commands",0,0,0);

  while((cmd_ptr = q_get(&rrc_tdsrrc_cmd_q)) != NULL)
  {
    WRRC_MSG1_HIGH_OPT("Dispatching RRC ext TDSRRC cmd rrc_cmd_e_type_value_0x%x",cmd_ptr->cmd_hdr.cmd_id );
    /* Check if the incoming internal command is required a further process */
    if (rrcmcm_is_process_required(cmd_ptr) == FALSE) 
    {
      WRRC_MSG1_HIGH("CMD: %x is disgarded!",cmd_ptr->cmd_hdr.cmd_id);
    }
    else
    {
      if(rrc_deact_status == RRC_NORMAL_OPERATION)
      {
       /* Switch on the command id of the received command, and
         call the appropriate function to process each command. */
        switch(cmd_ptr->cmd_hdr.cmd_id)
        {
          case TDSRRC_RRC_STOP_TDS_CNF:
            rrc_cell_selection_procedure(cmd_ptr);
            break;

           default:
             /* Unexpected TDS RRC command */
             WRRC_MSG1_ERROR("Unknown TDS RRC Command: %x", cmd_ptr->cmd_hdr.cmd_id);
             break;
        }
      }
      else if((rrc_deact_status == RRC_DEACT_COMPLETED)
           && (cmd_ptr->cmd_hdr.cmd_id == TDSRRC_RRC_STOP_TDS_CNF))
      {
        WRRC_MSG1_HIGH("TDSRRC_RRC_STOP_TDS_CNF received", cmd_ptr->cmd_hdr.cmd_id);
        rrc_cell_selection_procedure(cmd_ptr);
      }
      else
      {
        WRRC_MSG1_ERROR("Throw away the TDS RRC command %x",cmd_ptr->cmd_hdr.cmd_id);
      }
    }
    /* The command has been processed. We can free the memory for this cmd */
    rrc_free_tdsrrc_cmd_buf( cmd_ptr );
  }
}
#endif

/*===========================================================================

FUNCTION  RRC_DISPATCHER

DESCRIPTION
 
  The RRC_DISPATCHER is the central wait-loop for the RRC task. This
  function constantly waits for events to be received. When events are
  received, they are dispatched to the appropriate function to be processed.
  
  This function is the only wait-loop in the RRC task. No other procedures
  can perform a local wait loop. 
 
  The set of events that the RRC task waits for is determined by RRC's
  event dispatch table. This table must be created before rrc_dispatcher is
  called. Regardless of the current state of RRC, the set of events that
  the rrc_dispatcher is waiting for is the same.
 
DEPENDENCIES

  Event dispatch table must be created before this function is called.  
   
RETURN VALUE

  None. Note that this function does not return unless the RRC task is being
  shut down. (*** This is TBD ***).
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatcher ( void )
{
  
  rex_sigs_type rrc_requested_sigs;  /* Signals to wait for */
  rex_sigs_type rrc_received_sigs;   /* Signals received    */



  /* Set Rex signal mask to the desired signals */

  rrc_requested_sigs =   
    
         /* Signal for internal RRC commands  */
         RRC_INT_CMD_Q_SIG

         /* Signal for L1 commands            */
      |  RRC_L1_CMD_Q_SIG

         /* Signal for MAC commands           */
      |  RRC_MAC_CMD_Q_SIG

         /* Signal for RLC commands           */
      |  RRC_RLC_CMD_Q_SIG

         /* Signal for MM commands            */
      |  RRC_MM_CMD_Q_SIG

         /* Signal to send watchdog report    */
      |  RRC_RPT_TIMER_SIG
      
         /* Signal for TC commands            */
      |  RRC_TC_CMD_Q_SIG 
         /* Signal for RR commands            */
      |  RRC_RR_CMD_Q_SIG
#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
      | RRC_LSM_CMD_Q_SIG
#endif
      | RRC_CM_CMD_Q_SIG
#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
      | RRC_LTE_RRC_CMD_Q_SIG
#endif
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /*TEST_FRAMEWORK*/
#ifdef FEATURE_DUAL_SIM
      | RRC_WRM_PREEPTION_SIG
#endif
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
      | RRC_L1_FF_CMD_Q_SIG
#endif
#if defined (FEATURE_TDSCDMA_TO_WCDMA) || defined (FEATURE_WCDMA_TO_TDSCDMA)
      | RRC_TDSRRC_CMD_Q_SIG
#endif
      | RRC_QMI_CMD_Q_SIG

         /* Add other signals here            */
            ;

  /*Register with DOG, get the Task ID*/
  rrc_dog_rpt_var      = dog_hb_register_rex((rex_sigs_type)RRC_RPT_TIMER_SIG);
  MSG_MED("Waiting for RRC Signals",0,0,0);

  /* Perform the rex wait */
  while( (rrc_received_sigs = rex_wait( rrc_requested_sigs )) != 0 )
  {

    /* We get here when atleast one of the requested
    signals is set. Check the received signals in sequential
    order of priority so that if more than one signal is
    set, we process the higher priority signal first. */

    /* Signal to send Watchdog report */
    if( rrc_received_sigs & RRC_RPT_TIMER_SIG )
    {
      rrc_dog_report_and_clear_sig();
    }

#ifdef FEATURE_DUAL_SIM
    if( rrc_received_sigs & RRC_WRM_PREEPTION_SIG )
    {
      (void) rex_clr_sigs( rex_self(), RRC_WRM_PREEPTION_SIG );
      rrcwrm_post_preemption_indication();
    }
#endif

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
      if( rrc_received_sigs & RRC_L1_FF_CMD_Q_SIG )
      {
        (void) rex_clr_sigs( rex_self(), RRC_L1_FF_CMD_Q_SIG );
        rrc_dispatch_l1_ff_commands();
      }
#endif

    /* RRC Internal Command Queue */
    if( rrc_received_sigs & RRC_INT_CMD_Q_SIG )
    {
      (void) rex_clr_sigs( rex_self(), RRC_INT_CMD_Q_SIG );
      rrc_dispatch_internal_commands();
    }

    /* RRC L1 Command Queue */
    if( rrc_received_sigs & RRC_L1_CMD_Q_SIG )
    {
      (void) rex_clr_sigs( rex_self(), RRC_L1_CMD_Q_SIG );
      rrc_dispatch_l1_commands();
    }

    /* RRC MAC Command Queue */
    if( rrc_received_sigs & RRC_MAC_CMD_Q_SIG )
    {
      (void) rex_clr_sigs( rex_self(), RRC_MAC_CMD_Q_SIG );
      rrc_dispatch_mac_commands();
    }

    /* RRC RLC Command Queue */
    if( rrc_received_sigs & RRC_RLC_CMD_Q_SIG )
    {
      (void) rex_clr_sigs( rex_self(), RRC_RLC_CMD_Q_SIG );
      rrc_dispatch_rlc_commands();
    }

    /* RRC MM Command Queue */
    if( rrc_received_sigs & RRC_MM_CMD_Q_SIG )
    {
      (void) rex_clr_sigs( rex_self(), RRC_MM_CMD_Q_SIG );
      rrc_dispatch_mm_commands();
    }
    
    /* RRC TC Command Queue */
    if( rrc_received_sigs & RRC_TC_CMD_Q_SIG )
    {
      (void) rex_clr_sigs( rex_self(), RRC_TC_CMD_Q_SIG );
      rrc_dispatch_tc_commands();
    }

    /* RRC RR Command Queue */
    if( rrc_received_sigs & RRC_RR_CMD_Q_SIG )
    {
      (void) rex_clr_sigs( rex_self(), RRC_RR_CMD_Q_SIG );
      rrc_dispatch_rr_commands();
    }

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
    /* RRC LSM Command Queue */
    if( rrc_received_sigs & RRC_LSM_CMD_Q_SIG )
    {
      (void) rex_clr_sigs( rex_self(), RRC_LSM_CMD_Q_SIG );
      rrc_dispatch_lsm_commands();
    }
#endif

    /* RRC CM Command Queue.*/
    if( rrc_received_sigs & RRC_CM_CMD_Q_SIG )
    {
      (void) rex_clr_sigs( rex_self(), RRC_CM_CMD_Q_SIG );
      rrc_dispatch_cm_commands();
    }

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
    /*LTE RRC CMD queue*/
    if(rrc_received_sigs & RRC_LTE_RRC_CMD_Q_SIG)
    {
      (void)rex_clr_sigs(rex_self(),RRC_LTE_RRC_CMD_Q_SIG);
      rrc_dispatch_lte_rrc_commands();
    }
#endif

    /* QMI CMD queue*/
    if(rrc_received_sigs & RRC_QMI_CMD_Q_SIG)
    {
      (void)rex_clr_sigs(rex_self(),RRC_QMI_CMD_Q_SIG);
      rrc_dispatch_qmi_commands();
    }

#if defined (FEATURE_TDSCDMA_TO_WCDMA) || defined (FEATURE_WCDMA_TO_TDSCDMA)
    /*TDRRC CMD queue*/
    if(rrc_received_sigs & RRC_TDSRRC_CMD_Q_SIG)
    {
      (void)rex_clr_sigs(rex_self(),RRC_TDSRRC_CMD_Q_SIG);
      rrc_dispatch_tdsrrc_commands();
    }
#endif

#ifdef TEST_FRAMEWORK
    #error code not present
#endif/*TEST_FRAMEWORK*/     


  }/* end while */
}/* end function */

