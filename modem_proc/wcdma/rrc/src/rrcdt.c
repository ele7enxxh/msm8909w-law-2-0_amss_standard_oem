/*===========================================================================
                  R R C  D I R E C T  T R A N S F E R

DESCRIPTION
  This file contains the five procedures that deal with direct transfer:
  Initial Direct Transfer, Uplink Direct Transfer, Downlink Direct Transfer,
  Signalling Connection Release, and Signalling Connection Release Request.

EXTERNALIZED FUNCTIONS
  rrcidt_procedure_event_handler
    This function is the event handler for all events that are sent to the
    Initial Direct Transfer procedure.

  rrcudt_procedure_event_handler
    This function is the event handler for all events that are sent to the
    Uplink Direct Transfer procedure.

  rrcddt_procedure_event_handler
    This function is the event handler for all events that are sent to the
    Downlink Direct Transfer procedure.

  rrcscr_procedure_event_handler
    This function is the event handler for all events that are sent to the
    Signalling Connection Release procedure.

  rrcscrr_procedure_event_handler
    This function is the event handler for all events that are sent to the
    Signalling Connection Release Request procedure.

  rrcdt_init_procedure
    This function initializes substates of initial direct transfer,
    uplink direct transfer and downlink direct transfer procedures and other
    parameters to validate the sessions.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2000-2009 by Qualcomm Technologies, Inc.  
All Rights Reserved.QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcdt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   sp      Made changes to fix QSH Active set metric, UL OTA parameters metrics 
05/09/16   ad      Made changes to log important parms from OTAs
05/09/16   sp      Made changes for Mav debug snapshot
09/11/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
04/24/15   sas     Send failure to open_session_req when the Domain is already active.
11/10/14   sp      Made changes to save procedure substates in crash debug info
10/22/14   gv      Made changes to have connected mode feature set independent of TA status 
10/07/14   bc      Changes to not indicate CSG support in IDT when CSG is disabled through NV
09/30/14   sg      Made changes to send SCRI after CU confirm
09/15/14   sp      Made changes such that RRC triggers CU immediatly after close_session_req is received from NAS if delay CU flag is set.
08/04/14   bc      Made changes to fix compiler warnings.
07/15/14   ac      NV reorganization
06/10/14   sg      Made changes to transmit SCRI after CU is completed if close_session is 
                   is received in PCH or URA-PCH states
03/26/14   sr      Made changes to not perform fast redirection to LTE when LTE is not in supported RAT list
06/09/14   sn      Changes to initialize SCRR if active while aborting dormancy procedure
05/14/14   bc      Made changes to ignore GTOW CCO after getting suspend confirm from
                   L1 if PS session is closed during the interval between suspend req 
                   and confirm.
05/09/14   sr      Made changes to disable UL compression capability in DSDS/DSDA mode.
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
02/19/14   sn      Ignore CCO if PS domain release in progress
03/17/14   sr      Added NV support for uplink compression capability
03/13/14   vi      Stack reduction changes
01/06/14   sr      Made changes to upgrade ASN changes for UL compression for OSYS compiler v6.6.5
12/09/13   sp      Made changes for OSYS 6.6.5 migration
11/27/13   sr      Made changes to indicate capability for Uplink Compression feature
07/09/13   bc      Changes to not indicate CSG support in IDT when CSG is disabled
                   through NV
10/23/13   gv      Made changes to trigger FR2LTE only after a CS RAB gets established and then released
09/11/13   gv      Made changes to clear the csfb call status when CS domain is closed
08/30/13   as      Made chagnes to pass rb-id to get smc complete status function
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
07/09/13   bc      Changes to not indicate CSG support in IDT when CSG is disabled
                   through NV
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
06/07/13   vi      Replaced ALLOC_ASN1ARRAY1 with ALLOC_ASN1ARRAY2
05/20/13   sn      Changes to consider W CSG feature as disabled if there is 
                   no whitelist update from NAS
05/13/13   vi      Fixed compiler and KW errors for Bolt
06/05/13   rmn     Made changes to send close session cnf when SCR from NW is received while waiting for SCRI RLC ACK
04/12/13   gv      Made changes to check for the presence of primary plmn id before rejecting
                   the IDT because of PLMN mismatch
04/09/13   gv      Added code to handle signaling connection release request in PCH state while waiting for L2 ack
03/28/13   ad      Made changes to send SYNC_IND with integrity established when DDT is received 
                   before L2 ack for IDT when fake security is enabled.
03/08/13   sr      Made changes to send SCRI for fast dormancy req received during DCH->FACH transition
01/03/13   ad      Made changes to reduce the redundant F3s
12/28/12   esr     Made changes to provide a new rej cause for an open sess when conn rel is in prog
12/07/12   sn      Changes to populate CSG capability in IDT and conn. setup complete
10/23/12   pm      Made changes to assign pointer to NULL after de-allocation
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
10/14/12   gv      Added code to support exchange of AS ID between WRRC and GRR commands
10/03/12   sr      Made changes  to set IDT establishment cause as given by upper layers
09/27/12   sr      Added code not to send UDT when RCR is active
01/11/10   ss      Bug fix to set START = 0 in IDT if new keys are received for that domain.
08/10/12   gv      Added code to check and change the TRM priority when CS domain is closed
02/09/12   gv      Added code to support inclusion of PLMN ID in IDT message with network sharing
12/13/11   sks     Added support for LTOW SRVCC.
11/30/11   gv      Added handling of DATA_REQ in WAIT_FOR_L2_ACK substate in CELL_PCH/URA_PCH state
11/16/11   ad      Changed the macro for FEATURE_WCDMA_PAUSE_MTPL_BACKOFF_VOICE_CALL
11/11/11   mn      Made changes to send CLOSE_SESSION_IND to NAS, in case of WRLF when RRC is 
                   waiting for L2 ACK for PS SCRI during fast redirection to LTE preparation.
10/31/11   sks     Added support for FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB.
10/28/11   mn      Made changes to allow legacy FD request in URA_PCH state
10/04/11   gv      Added code to provide call termination cause to NAS when RL Failure happens
09/21/11   gv      Fixed memory leaks
08/26/11   ad      Made changes to initialise the nas_sync_ind flag
                   while closing the signalling connection
08/10/11   su      AAdded code to send OPEN_SESSION_CNF failure to NAS when 
                   RCR procedure is active. 
08/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.103.00
07/29/11   ad      Added code for FEATURE_WCDMA_PAUSE_MTPL_BACKOFF_VOICE_CALL feature
07/19/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.102.00
07/12/11   kp      Added code to not trigger Fast Dormancy in URA_PCH state
07/05/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.101.00
06/27/11   kp      Added code to track nas_sync_ind so that DDT messages acn be allowed 
                   if we have sent atleast one nas_sync_ind to that CN doamin.
06/17/11   gv      Made changes to suspend BPLMN search when a call is initiated
                   under the feature FEATURE_WCDMA_CONTINUE_MPLMN
05/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.97.50
04/26/11   su      Added changes to ignore close session request incase there is
                   a SCRR with L2 ack pending.
05/03/11   rl      Fixed the lint error
04/25/11   rl      Fixed the KW error
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   sks     Added support for LTOW PSHO.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/03/10   kp      Added code not to close CN doamin through RBR if IDT is active
11/03/10   rl      Review comments fix
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
08/16/10   ss      Added FAILURE in return statement of discarding UDT when
                   SCRI of that domain is in progress. 
08/02/10   ss      Added code to discard RRC_DATA_REQ only if the SCRI progress 
                   for that CN domain otherwise process  RRC_DATA_REQ from NAS 
                   to send UDT message.
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
05/07/10   prk     Fixed compiler warnings.
05/06/10   ss      Added code to abort SCRR if not initiated by Fast Dormancy
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
03/02/10   as      Fixed Klockwork issues
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
01/11/10   ss      Bug fix to set START = 0 in IDT if new keys are received for that domain.
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
10/27/09   kp      Bug fix to set the flag data_ind_status correctly for DDT messages when freeing 
                   in function rrcddt_procedure_event_handler
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
09/11/09   ps      Made changes for reducing the internal RRC command size 
                   under FEATURE_RRC_REDUCE_CMD_SIZE
08/17/09   ss      Added code to provide ciphering ON/OFF indication to UI 
                   under feature flag FEATURE_UMTS_VOICE_CIPHERING_IND.
08/03/09   kp      Changes to obsolete gs_alloc/gs_free under feature flag FEATURE MODEM HEAP
07/20/09   rm      Modified the call to funtion rrcrce_force_initialize
06/05/09   ps      Made changes to not start the T305 timer if value is infinity
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
05/06/09   kp      Added support to close CN doamin indicated in RB release only 
                   if the CN doamin is open.
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/23/09   kp      Bug fix to set udt substate to init when we delete all saved UDT's.
04/21/09   kp      Bug fix to set rrc_sig_con_rel_status for re transmission 
                   of SCRI instead of close_sess_req_status when sig connection release is in progress.
03/28/09   rm      CMI Featurisation
03/19/09   kp      Added support for handling clash between close_session_ind 
                   from CU due to local release of signalling connection & 
                   pending close_session_req from NAS.
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
10/24/08   ns      Made changes to discard UDTs received from NAS if SCRR is in progress.
                   Also added support for deleting the saved UDTs on getting a positive
                   L2 ACK for SCRR
09/12/08   ns      Added code for REl7 under FEATURE_WCDMA_REL7
07/23/08   ns      Made changes to handle UDT request in Disconnected state. DT will send
                   initialise request to RCE and SIB module
04/21/08   ns      Fixed klockwork errors
03/06/08   kp      Added handling for emergency call in open_session_req when UE is in OOS
                   before even validating open_session_req.
02/21/08   ns      Fixed klockwork errors
11/19/07   kp      Added support for UE initiated dormancy under feature
                   flag FEATURE_UE_INITIATED_DORMANCY.
10/23/07   kp      Added support for network sharing feature.Changes are under feature flag 
                   FEATURE NETWORK SHARING.
10/03/07   ns      Bug fix to take care of a race condition to put through 
                   emergency call in cell DCH state. 
09/24/07   kp      Added support for carrying START value according to 8.5.9 
                   for first IDT message only, the re transmitted IDT’s will 
                   simply append START value.Re-transmission of IDT will happen
                   in case of Lower layer failure or SRB-3 purging, So UE is not 
                   sure whether the first IDT has reached N/W or not. Hence to have 
                   a consistent START value Re-transmitted IDT will carry appended 
                   START value that was calculated by first IDT.
09/07/07   vm      Added support for feature FEATURE_RRC_CALLOC
08/07/07   kp      Bug fix to take care of a race condition to put through 
                   emergency call when UE is in OOS & ABORT_REQ is in progress.
07/17/07   rm      Added support for FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING feature.
06/05/07   kp      Added support to evaluate & send OPEN_SESSION_CNF with new failure
                   cause RRC_OPEN_SESS_FAILURE_AC_BARD if Cell Access Class is bard 
                   for a CN domain.
03/30/07   kp      Moved MAX_NAS_MESSAGES_FOR_CS_DOMAIN from rrcdt.c to rrcmmif.h  
                   as its value is a functionality of NAS.
03/02/07   ps      Added support for WB-AMR .
02/13/07   vr      Fix to send only one NULL terminated message to RR for RR_NAS_DATA_IND after
                   sending all pending IDT and UDT messages during WtoG handover
01/22/07   vr      Fixed Lint errors
09/05/06   vm      The function call rrc_transition_to_disconnected_state() now takes an
                   additional argument.
08/28/06   kp      Added support to search UDT's internal queue for NAS 
                   OTA messages.This is done to avoid NAS sending multiple
                   duplicate NAS OTA messages received by RRC when access 
                   stratum is in OOS for longer time.
08/21/06   kp      Added support to provide new cause for OOS in close_cnf to MM
06/30/06   kp      Added fix to set the establishment cause when UE is in CELL_FACH 
                   RRC state on receiving OPEN_SESSION_REQ from NAS. 
06/14/06   kp      Added support to delete IDT on reception of Close Req from NAS 
                   for the same domain, If IDT is buffered in local queue
05/01/06   kp      Added support to discard any DDT message received during 
                   signaling connection release in progress & waiting 
                   for L2-ACK, for that Core Network domain.
04/28/06   kp      Added the support for correctly setting establishment cause 
                   in IDT,if rrc connection is established due to inter-RAT cell 
                   reselection or inter-RAT cell change order.Also added the support 
                   for setting establishment cause in all subsequent IDT's which uses the 
                   same RRC connection except the first IDT.
04/14/06   vr      Do not purge WM3 if the L2 Ack failed due to WM purging
04/03/06   vr      More Lint Fixes
03/24/06   vr      Lint Fixes
03/07/06   vr      Before re-transmitting UDT message on failed L2 Ack,check to see 
                   if we are waiting for L2 Ack for any other UDT message(s) 
                   of the same domain.If so,it might be still  sitting in the WM 
                   and may go out before this re-trasmitted message and hence the 
                   order of delivery would be reversed.This is not desirable,hence 
                   purge the WM so that serialization is maintained
01/25/06   bd      Lint clean-up
01/23/06   vm      BPLMN Optimization for Automatically re-starting Manual 
                   BPLMN search after completion of LAU/RAU updates & added
                   support for not rejecting Manual PLMN search request when RRC state 
                   is Connecting/FACH/DCH and the establishment cause is Registration. 
01/17/06   vr      Suspend BPLMN if active on RRC_OPEN_SESSION_REQ
12/22/05   vr      Serialize SCRR and IDT to avoid race conditions
12/20/05   bd      Changed syntax for suppressing lint error 613
11/25/05   kp      DDT received before l2ack of IDT will now send open session cnf 
                   and then DATA_IND
11/11/05   kc      Removed fn. to take UE to disconnected.  Updated to call generic fn.
11/10/05   vr      Changes for switching between HEDGE and WEDGE using NV Item
                   All Rel 5 changes are now under the nv check.
11/04/05  da/vr    Made changes to detect UE inactivity and take appropriate actions.
10/20/05   vr      Added the status(Success/Failure) of L2 Ack in the MSG_HIGH.
                   Without the status,the debug message is misleading.
09/19/05   ss      Added the feature flag FEATURE_RRC_CU_IDT_ESTCAUSE, only
                   when this is turned on establishment cause will be sent 
                   over the air
09/12/05   ss      Added the structure rrcidt_est_cause_info_type which stores
                   the information of establishment cause received from NAS. 
                   Added the functions rrc_check_if_est_cause_available, 
                   rrcidt_fill_est_cause, rrcidt_set_est_cause, reset_est_cause.
09/09/07   vm      Added checks in IDT/UDT to abort on-going correction if an 
                   Emergency call indication is received and UE is OOS.  This
                   is to facilitate faster handling of Emergency call on other
                   RATs.
08/08/05   ss      Added the field est_cause in rrcidt_open_sess_req_type.
08/18/05   kc      Added some changes for handling close_sess/sig_conn_close                   
08/11/05   vr      Added more debug messages after receiving L2 ACK for IDT
08/05/05   da      Modified code to set flag exclude_modified_cell_results to 
                   false, when calling func rrcmeas_append_meas_rpt_on_rach.  This
                   is to allow adding of monitored cell results if present.
07/29/05   ss      moved the cs_domain setting of open to the point where IDT is
                   received from NAS instead of setting while tranmsitting
07/28/05   ss      added a defensive check of handling if close is received from
                   NAS when security mode command is recevied before l2 ack for
                   idt is received
06/30/05   ss      Added the check for CU procedure during retransmission 
                   if the MUI indicates failure and also initiation of cell 
                   update if current state is either cellor ura_pch.
                   Similar check has been added in handling of cell update 
                   confirm to see if current state is cell or ura pch to 
                   trigger a cell update with uplink data transmission
                   Also added the function rrcdt_is_idt_waitingfor_l2ack to 
                   return true if waiting for l2ack
06/29/05   vr      While picking up the UDT message from the saved messages for
                   retransmission,check to see if the mui_status is true before
                   picking up.Just checking the mui alone is not sufficient(mui
                   of zero clashes with the initialization value zero)
05/26/05   kc      Added a bunch of clean up and also handling of STATE_CHANGE_IND
                   for all UL procedures in event_handler at top level itself
04/06/05   vr      Include rrclogging.h unconditionally for logging on all platforms
03/22/05   kc      Added support to re-transmit IDT/UDT messages if L2-ACK failure
                   indication is received from RLC for MUI's for these msgs
01/10/05   sgk     Added procedure id as second parameter to function call
                   rrcmeas_append_meas_rpt_on_rach, in functions 
                   rrcidt_send_cs_initial_direct_tranfer_msg,
                   rrcidt_send_ps_initial_direct_tranfer_msg,
                   rrcudt_send_uplink_direct_transfer_msg. 
11/29/04   kc      Set OPEN_SESSION_CNF, CLOSE_SESSION_CNF, SIG_CONN_REL_CNF status
                   correctly based on success/failure of L2-ACK for the above REQ's
11/12/04   kc      Fixed lint errors
11/09/04   kc      Check to see if CU is active before sending any UDT message in 
                   all substates of UDT.
10/12/04   kc      Fixed ABORT_REQ and CLOSE_SESSION_REQ handling when both CN
                   domains are active.
09/27/04   kc      Changed MAX outstanding UL DT messages for CS and PS domains
08/19/04   sgk     Passed NAS Msg priority to rrcudt_delete_saved_data_req 
                   instead of rb_id in function 
                   rrcudt_process_udt_wait_for_l2ack_substate.
                   Assigned the status variable to the values SUCCESS and FAILURE 
                   instead of TRUE and FALSE in functions 
                   rrcudt_delete_saved_data_req, 
                   rrcudt_check_if_msg_status_can_be_saved, 
                   rrcudt_check_saved_message_status, rrcudt_check_saved_mui_status
                   to fix lint error Type mismatch (initialization) (int/enum)
                   and Type mismatch (assignment) (int/enum)
                   Assigned nas message priority variable to RRC_NAS_MSG_HIGH_PRIO       
                   and RRC_NAS_MSG_LOW_PRIO instead of 
                   DCCH_DT_HIGH_PRI_RADIO_BEARER_ID and DCCH_DT_LOW_PRI_RADIO_BEARER_ID
                   in function rrcudt_save_data_req, rrcudt_save_g2w_nas_data_ind,
                   rrcudt_send_uplink_direct_transfer_msg to fix lint error: Type 
                   mismatch (assignment) (int/enum)
                   Fixed lint error enum mismatch for cn domain id in functions 
                   rrcudt_send_uplink_direct_transfer_msg,
                   rrcddt_process_downlink_direct_transfer_msg and 
                   rrcscr_check_active_rabs_in_sig_conn_rel_domain
07/23/04   kc      Fixes for March 04 ASN1 notation
07/07/04   kc      Reset saved DDT flag indication when STATE_CHANGE_IND to idle is
                   received.
06/05/04   kc      Removed ERR_FATAL when max DDT msgs are exceeded.  Also clearing
                   Saved NAS msg's when UE goes to IDLE.
05/27/04   kc      Fix for START bug for not incrementing START between Conn-Setup
                   and IDT.
05/25/04   kc      Route cell_update_comp_cnf in cell/ura PCH substates.  Also
                   check if CU proc is already processing CU in cell/ura PCH 
                   substates      
04/15/04   kc      Fix to not update START value between Conn-Setup-Complete and
                   IDT.
02/10/04   kc      Fix to handle race condition between L2_ACK for SMC and
                   L2_ACK for Open_Session_Req.
02/06/04   sk      Handlle cell update complete cnf in Cell_DCH state.
12/10/03   kc      Fix for CR38143.  Now UE will reject Sig_Conn_Rel_Ind if
                   active RB's are present in a RAB for the domain which is to
                   be released
10/21/03   kc      Append START values to ABORT_IND to write back latest START
                   for each domain during an abnormal call release.
10/21/03   kc      Updated rrcsmc_append_start_per_domain calls to sync up to
                   latest rrcsmc interface.
08/22/03   kc      Updated code to send SYNC_IND to MM for CS if Fake-Security is
                   turned on.
08/18/03   kc      Updated code to send ABORT_IND to MM when MM sends UDT msg and
                   RRC is in DISCONNECTED state.
08/18/03   rj      Updated code to print correct substate in functions
                   rrcudt_process_udt_wait_for_l2ack_substate and 
                   rrcudt_send_nas_data_ind.
08/14/03   bu      In rrcudt_process_udt_wait_for_l2ack_substate reset the
                   rrcudt_substate to INITIAL correctly.
08/11/03   rj      Added code to initialize IDT variables in tye Init Function.
08/11/03   rj      Added code to perform simultaneous initial direct transfer 
                   procedures for both CS and PS domains. 
07/30/03   rj      Added code to implement buffering of DL direct Transfer
                   messages if security mode procedure is active and removed
                   buffering in the UL since it is no longer required.
07/21/03   kc      Added states to wait until SMC received L2_ACK for SMC before
                   sending any uplink message.
07/16/03   kc      Added code to only allow Fake_Security or Integrity flag
                   to be enabled.  Both the flags are mutually-exclusive, since
                   it is possible to enable both through QXDM, added check here.
07/11/03   kc      Added code to send indication to SMC procedure when Sig
                   Conn Release is done for a particular domain.
05/21/03   rj      Added code to reject Downlink Direct Transfer message
                   with suitable RRC STATUS message ifthere is no active
                   Signalling Connection for that domain.
04/25/03   rj      Added cause while sending RRC_ACTIVATION_IND command.
04/24/03   rj      Expanded SCRR state machine to send Signalling Connection
                   Release Request for active Signalling connections if MM 
                   sends RRC_ABORT_REQ. After sucessful transmission of 
                   Signalling Connection Release Request message, RCE will
                   tear down the SRBs are directs the UE to Idle mode.
04/11/03   rj      Added code to enable 3A0 non-critical extensions for
                   Initial direct Transfer message.
04/10/03   rj      Added code to append START value to the Initial Direct
                   Transfer message.
02/27/03   rj      Fixed a lint error.
02/26/03   rj      Implemented support for GSM to WCDMA Hondover and maximum
                   number of Direct Transfer messages that can be transmitted
                   without receiving L2 ACK to 4.
02/05/03   sk      March2002Rel specific changes 
                   Replaced signallingConnectionReleaseRequest with 
                   signallingConnectionReleaseIndication.
12/17/02   rj      Modified code to change the sequence of RRC_OPEN_SESS_CNF
                   and RRC_SYNC_IND when Fake Integrity is enabled.
12/10/02   rj      Implemented RRC_CLOSE_SESSION_CNF. This command will be
                   sent MM in response to RRC_CLOSE_SESSION_REQ.
11/15/02   rj      Updated function rrcudt_send_uplink_direct_transfer_msg
                   to use correct pointer for error fatal failure case.
11/06/02   rj      Memory allocation to embedded buffers is done through 
                   mem_malloc instead of rrc_malloc 
10/31/02   rj      Updated function rrcudt_delete_saved_data_req to return
                   correct value.
10/30/02   rj      Added code to append Measurements on RACH IE to the 
                   Uplink Direct Transfer message if current state is
                   CELL_FACH state.
10/30/02   rj      Modifed code to send Fake Security for both CS and PS 
                   Domains.
10/26/02   rj      Added implementation to store more than one NAS message 
                   during Inter-RAT Handovers and fixed broken SS functionality.
                   The implementation is generic and can be extended to
                   N messages and at present N is set 2 for both domains.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
10/12/02   rj      Added implementation to wait for L2 ACK for Initial
                   Direct Transfer and Uplink Direct Transfer messages. Also
                   added code to forward NAS messages that are pending 
                   to be transmitted or pending to be acknowledged by the
                   UTRAN to RR during Inter-RAT Handover.
08/27/02   kc      Fix for CR24192. Removed the FEATURE defn. for Fake Security 
                   as it is now controlled through NV RAM.  
08/02/02   kc      Added fn. to read NV item FAKE_SECURITY_ENABLED 
05/28/02   rj      Added code to remove the signalling connection after 
                   transmitting Signalling Connection Release Request in the
                   uplink.
05/20/02   rj      Added hack under feature FEATURE_RRC_FAKE_SECURITY_MODE
                   to send RRC_SYNC_IND after transmitting Initial
                   Direct Transfer message to as though Security Mode Control
                   message is received w/o actually receiving it.
05/15/02   rj      Modified F3 messages to fit in 40 charecters.
04/19/02   rj      Updated function rrcidt_send_open_sess_req to append
                   RACH measurements only CELL_FACH state. Also renamed functions
                   rrcidt_send_open_sess_req  and rrcudt_send_data_req to 
                   rrcidt_send_initial_direct_tranfer_msg and 
                   rrcudt_send_uplink_direct_transfer_msg respectively,                   
04/09/02   rj      Fixed a bug in function rrcscrr_process_scrr_initial_substate
                   and renamed function rrcscrr_send_close_session_req to 
                   rrcscrr_send_sig_conn_rel_req. Updated F3 messages.
02/15/02   kc      Fixed PS call typo errors in rrcudt_save_data_req
02/15/02   kc      Fixed CS/PS typo in line 535
02/14/02   rj      Updated code for new Cell Update procedure interface.
31/01/02   kc      Fixed memHeap Assertion errors for releasing same mem block
                   more than once  
31/01/02   kc      Fixed ARM Compilation warnings  
31/01/02   kc      Added CELL_FACH, CELL_PCH/URA_PCH handling code.  Also modified
                   code to include RRC Meas Info on RACH in IDT.
12/20/01   kc      Bug fixes found when testing June release
11/27/01   kc      Changes pertaining to June Spec Conversion.  Main changes
                   to intra_domain_nas_node_selector data structure.
10/23/01   kc      Initialized some uninitialized variables to remove warnings
10/17/01   kc      Removed RRC_SYNC_IND message sent from RRC to MM in DT
                   procedure.  Now it is handled in RRCSMC procedure.
09/19/10   rj      Modified F3 message text to maximum length of 40 charecters.
09/18/01   rj      Added a check for RRC Connection before establishing 
                   Signalling Connections in function 
                   rrcidt_validate_open_session_req.
09/14/01   rj      Modified F3 messages and updated code not to declare
                   ERR_FATAL for race conditions when RLC Id is not found. Added
                   F3 messages to declare Critical conditions are not supported.
09/14/01   kc      Added code to support CELL_FACH, CELL_PCH/URA_PCH in IDT,
                   UDT and SCRR.  All the states in IDT, UDT and SCRR are
                   now handled.  Also added code to log protocol error.
09/03/01   rj      Removed include file rrcasnutil.h since it is no longer 
                   required after moving send_rrc_status_message to SEND CHAIN.
08/30/01   rj      Renamed rrc_send_rrc_staus_message to
                   rrcsend_send_rrc_status_message.
08/30/01   rj      Added code to send RRC_SYNC_IND after PS Signalling 
                   connection is established. Added support to check for
                   critical extensions. Updated functions
                   rrcddt_process_downlink_direct_transfer_msg and 
                   rrcscr_process_signaling_connection_release_msg to support
                   peeking into OTA message.
07/27/01   rj      Added new function to reset Signalling Connections status.
                   This will be RRC Connection Establishment procedure while
                   bringing down RRC Connection. Added code to register for 
                   state trasitions for IDT, UDT and SCRR procedures.
                   Removed processing of State Change Indication from SCR 
                   procedure since it is not required. Converted MSG_LOWs
                   to MSG_HIGHs for all OTA messages and inter-task commands.
06/28/01   rj      Added code to initialize the bit mask for Signalling 
                   Connection Release Request message.
06/19/01   vn      In rrcdt_init_procedure, register for State Change Ind for 
                   all Connected to Disconnected state transitions. Added a fn
                   rrcscr_process_rrc_state_transition that initializes substates
                   of all DT procedures when a State Change Ind is received in
                   Idle Disconnected state.
06/19/01   ram     In the function rrcudt_process_udt_initial_substate the
                   CN Domain id was not being included in the Uplink Direct
                   Transfer message. Copied the CN domain id from the MM command to
                   the message.
05/14/01   rj      Added function rrcscr_release_signalling_connection to
                   provide interface for Radio Bearer Release procedure to
                   release a Signalling Connection if it exists. Also modified
                   function rrcscr_process_signaling_connection_release_msg to use
                   function rrcscr_send_close_session_ind to send 
                   RRC_CLOSE_SESSION_IND to MM task.
05/02/01   rj      Added check for memory allocation for Intra domain NAS 
                   node selector.
04/29/01   rj      Included rrcscmgr.h since rrc_get_state function is moved
                   to RRCSCMGR module from RRCTASK module.
04/18/01   rj      Updated with code review comments. Modified the interface
                   to the function rrc_send_rrc_status_message. Added code to 
                   validate the Downlink Direct transfer payload length. Fixed 
                   typos.
04/16/01   rj      Renamed functions rrcdt_send_open_session_cnf,
                   rrcdt_validate_open_session_req,rrcdt_validate_data_req,
                   and rrcdt_validate_close_session_req to
                   rrcidt_send_open_session_cnf, rrcidt_validate_open_session_req,
                   rrcudt_validate_data_req and rrcscrr_validate_close_session_req
                   respectively. Added and corrected some comments. Fixed some
                   Typos.
04/11/01   rj      Removed freeing MM command buffer, instead declared ERROR
                   FATAL when memory allocation for payload fails.
04/06/01   rj      Changed name of function rrcsddt_process_signaling_connection
                   _release_msg to rrcscr_process_signaling_connection_release_msg.
04/06/01   rj      Replaced RRC_NEW_STATE_IND with RRC_STATE_CHANGE_IND. Fixed
                   a bug in rrcdt_process_downlink_direct_transfer_msg
                   function. Added a check to mem-malloc while allocating
                   memory for the Downlink direct transfer message payload.
                   Changed name of function rrcsddt_process_downlink_direct
                   _transfer_msg to rrcddt_process_downlink_direct_transfer_msg.
03/30/01   rj      Update to be compatible with latest LED rrcasn1.h and 
                   modified to December Spec release. Removed Flow ID concept.
                   Added transaction Id for Downlink direct transfer message and
                   Signalling Connection release message.
03/24/01   rj      Modified function rrcdt_validate_close_session_req to fix
                   a bug.
03/22/01   rj      Fixed some warnings to the rrcidt_substate_e_type,
                   rrcudt_substate_e_type, and rrcscrr_substate_e_type 
                   declarations.
03/14/01   rj      Added templates for the functions 
                   rrcidt_process_idt_wait_for_cell_update_comp_cnf_substate,
                   rrcudt_process_idt_wait_for_cell_update_comp_cnf_substate,
                   rrcscrr_process_idt_wait_for_cell_update_comp_cnf_substate.
                   Added more comments for future implementation.                   
03/09/01   rj      Added code for downlink direct transfer and signalling
                   connection release procedure. Updated code for initial
                   direct transfer, uplink direct transfer and signalling
                   connection release procedure so that future expandability
                   for CELL_PCH, URA_PCH abd CELL_FACH states will be easy.
03/05/01   rj      Updated code for the latest LCM, SEND_CHAIN and RRC-MM
                   interfaces.
02/28/01   rj      Removed Feature definition for Unit testing
01/17/01   jrs     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"

#include "rrccmd_v.h"               /* RRC command definitions */
#include "rrcdt.h"
#include "rrclcm.h"           /* Logical Channel Manager */
#include "rrcmeas.h"              /* Meas Report */
#include "rrcsend.h"              /* For sending a signalling message */
#include "msg.h"
#include "err.h"
#include "rrcasn1.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rrcscmgr.h"
#include "rrccu.h"
#include "rrcrce.h"
#include "rrcnv.h"
#include "rrcmcm.h"
#include "rrcsmc.h"
#include "rrclogging.h"
#include "rrcccm.h"
#include "rrcmisc.h"
#include "rrcrcr.h"
#ifdef FEATURE_INACTIVITY_HANDLING
#include "rrctmr.h"
#endif /* FEATURE_INTACTIVITY_HANDLING */
#include "rrc_rr_types.h"
#include "rr_rrc_if.h"
#include "rrccspi.h"
#include "rrccsp.h"

#ifdef  FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif
#include "rrcrcei.h"
#include "rrcsibproc.h"

#include "rrcdormancy.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif
#include "rrcasn1util.h"
#include "mm_as_types.h"
#include "rrctmr.h"
#include "stringl.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define INTRA_DOMAIN_NAS_NODE_SELECTOR_LENGTH 10  
                                           /* Length of Intra Domain NAS
                                              Node Selector in bits */

#define MAX_NAS_MESSAGES_FOR_PS_DOMAIN (SYS_MAX_PDP_CONTEXTS + 2)
                                              /* Number of NAS messages  
                                              that can be stored for PS*/
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

 /* boolean flag to read NV item and check to see if Fake Secured
    are enabled from QPST
  */
boolean    nv_fake_security_enabled;     

#ifdef FEATURE_WCDMA_UL_COMPR
/* Indicates if N/W supprts UL compression or not */
boolean    rrc_ul_comp_cap_support = FALSE;
#endif

typedef enum
{
  RRCSCRR_MUI_STATUS_FALSE,                 /* No outstanding MUI's                  */
  RRCSCRR_MUI_SIG_CON_REL_CNF,              /* Outstanding MUI for sig-con-rel-cnf   */
  RRCSCRR_MUI_CLOSE_SESSION_CNF             /* Outstanding MUI for close-session-cnf */
}rrcscrr_close_session_mui_status_type;

/*==========================================================================
  Type for defining the substates of Initial direct Transfer Procedure.
===========================================================================*/

typedef enum 
{
  RRCIDT_INITIAL,               /* IDT initial sub state              */
  RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF,
                                /* IDT waits for confirmation from Cell 
                                   Update Procedure complete */
  RRCIDT_WAIT_FOR_L2ACK         /* IDT waits for l2 ACK for Initial Direct
                                   Transfer message */
} rrcidt_substate_e_type;
/* Below #defines are added for logging purpose only */
  #define rrcidt_substate_e_type_value0 RRCIDT_INITIAL,               
  #define rrcidt_substate_e_type_value1 RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF                                                    
  #define rrcidt_substate_e_type_value2 RRCIDT_WAIT_FOR_L2ACK         

/*==========================================================================
  Type for defining the substates of Uplink direct Transfer procedure.
===========================================================================*/

typedef enum 
{
  RRCUDT_INITIAL,               /* UDT initial sub state              */
  RRCUDT_WAIT_FOR_CELL_UPDATE_COMP_CNF,
                                /* UDT waits for confirmation from Cell 
                                   Update Procedure complete */ 
  RRCUDT_WAIT_FOR_L2ACK         /* UDT waits for l2 ACK for Initial Direct
                                   Transfer message */
                                   
} rrcudt_substate_e_type;
/* Below #defines are added for logging purpose only */
  #define rrcudt_substate_e_type_value0 RRCUDT_INITIAL              
  #define rrcudt_substate_e_type_value1 RRCUDT_WAIT_FOR_CELL_UPDATE_COMP_CNF
  #define rrcudt_substate_e_type_value2 RRCUDT_WAIT_FOR_L2ACK 
  
/*==========================================================================
  Type for defining the substates of Signalling Connection Release Request 
  procedure.
===========================================================================*/

typedef enum 
{
  RRCSCRR_INITIAL,               /* SCRR initial sub state              */
  RRCSCRR_WAIT_FOR_CELL_UPDATE_COMP_CNF,
                                /* SCRR waits for confirmation from Cell 
                                   Update Procedure complete */ 
  RRCSCRR_WAIT_FOR_L2ACK       /* SCRR waits for L2 ACK for Signalling
                                  Connection Release Request message */
} rrcscrr_substate_e_type;
/* Below #defines are added for logging purpose only */
  #define rrcscrr_substate_e_type_value0 RRCSCRR_INITIAL        
  #define rrcscrr_substate_e_type_value1 RRCSCRR_WAIT_FOR_CELL_UPDATE_COMP_CNF
  #define rrcscrr_substate_e_type_value2 RRCSCRR_WAIT_FOR_L2ACK 


/* This data structure holds all of the regional data for all the direct 
 * transfer procedures.
 */
typedef struct
{
  /* This flag keep track of whether there is a signalling connection open
   * to the CS domain
   */
  boolean cs_domain_open;

  /* This flag keep track of whether there is a signalling connection open
   * to the PS domain
   */
  boolean ps_domain_open;

} rrcdt_int_data_type;

typedef struct
{
  /* This flags indicate the status of MUIs for Uplink Direct
     Transfer messages that are pending for L2 ACK */
  boolean             mui_status;
 
  /* This variable to stores the corresponding MUIs of Uplink
     Direct Transfer messages that are transmitted */
  uint8               mui;

   /* This flag indicates whether RRC_DATA_REQ is stored or not */
  boolean             data_req_status;

  /* This struct holds the Uplink Direct Transfer Procedure contents 
     which are saved from the DATA_REQ call for  a Domain
   */
  rrc_data_req_type   data_req;

  
}rrcudt_data_req_type;

typedef struct
{
  /* This flag indictes whether RRC_DATA_IND is stored or not */
  boolean           data_ind_status;


  /* This struct holds the Downlink Direct Transfer procedure contents
  that are saved from the RRC_DOWNLINK_SDU_IND for a Domain */
  rrc_data_ind_type data_ind;


}rrcddt_data_ind_type;


typedef struct
{
  /* This flag indicate the status of MUIs for Signalling
     Connection Request */
  rrcscrr_close_session_mui_status_type mui_status;
 
  /* This variable to stores the corresponding MUIs of the
     Signalling Connection Release Request message that is transmitted */
  uint8               mui;

   /* This flag indicates whether RRC_CLOSE_SESSION_REQ is stored or not */
  boolean             close_sess_req_status;

  /* This flag indicates whether RRC_SIG_CONN_RELEASE_REQ is stored or not*/ 
  boolean             rrc_sig_con_rel_status;
  /* Stores the proc_id which sended RRC_SIG_CONN_RELEASE_REQ
   * The same proc_id is used for sending response back to proc */
  rrc_proc_e_type rrc_proc;
}rrcscrr_close_sess_req_type;

typedef struct
{
  /* This flag indicate the status of MUIs for Initial Direct
     Transfer Message */
  boolean             mui_status;
 
  /* This variable to stores the corresponding MUIs of the
    Initial Direct Tranfer message that is transmitted */
  uint8               mui;

   /* This flag indicates whether RRC_OPEN_SESSION_REQ is stored or not */
  boolean             open_sess_req_status;

  /* This struct holds the Initial Direct Transfer Procedure contents 
     which are saved from the OPEN_SESSION_REQ call for  a Domain
   */
  rrc_open_session_req_type             open_session_req;
  
  /* Flag to indicate if L2_ACK for SMC was received before L2_ACK for
     IDT.  In this case, do not send OPEN_SESSION_CNF to MM, as SMC
     would have already sent the OPEN_SESSION_CNF
   */
  boolean             open_sess_cnf_required;
  rrc_establish_cause_e_type  est_cause;
  /*This variable holds the status of whether CU initiated by IDT
   when UE in CELL PCH or URA PCH*/
  boolean cu_init_by_idt;
  /*This variable holds the NAS Selected GSM MAP PLMN Identity which 
    comes as part of OPEN_SESSION_REQ to DT*/
  rrc_plmn_identity_type      selected_plmn_id;

  /*This variable will indicate what type of START value need to be 
   *carried in IDT 
   *TRUE:- IDT should carry START value calculate according to 8.5.9 
   *FASLE:- IDT should carry START value with out calculating according 
   *to 8.5.9. This is set to FALSE for Re-transmitted IDT. For fresh IDT
   *its set to TRUE */
  boolean fresh_start_value_needed;
  
}rrcidt_open_sess_req_type;

typedef struct
{

  boolean   valid; /* indicate the validity */
  rrc_establish_cause_e_type est_cause;
  rrc_proc_e_type   to_be_sent_by;
  boolean               sent_by_cu;
  rrc_proc_e_type  last_proc_sent;
  /*Call Type*/
  rrc_call_type_in_est_req_e_type  call_type;
}rrcidt_est_cause_info_type;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
boolean is_scrr_in_progress = TRUE;

#ifdef FEATURE_3GPP_CSFB
extern boolean rrc_csfb_call_status;
#endif
/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* This variable holds all of the regional data for this procedure
 */
rrcdt_int_data_type        rrcdt_int_data;

/* This holds the establishment cause which CU/IDT send */
rrcidt_est_cause_info_type rrcidt_est_cause_info;

/* This variable holds substate for Initial Direct Transfer Procedure
 */
rrcidt_substate_e_type     rrcidt_substate;
/* This variable holds substate for Uplink Direct Transfer Procedure
 */
rrcudt_substate_e_type     rrcudt_substate;
/* This variable holds substate for Signalling Connection Release Request
 * Procedure */
rrcscrr_substate_e_type    rrcscrr_substate;


/* This struct holds the Initial Direct Transfer Procedure contents 
   which are saved from the OPEN_SESSION_REQ call
 */
rrcidt_open_sess_req_type  rrcidt_cs_saved_open_session_req;

rrcidt_open_sess_req_type  rrcidt_ps_saved_open_session_req;


/* This variable is used to save RRC_DATA_REQ messages that are 
   supposed to be sent CS Domain */

rrcudt_data_req_type  rrcudt_cs_saved_data[MAX_NAS_MESSAGES_FOR_CS_DOMAIN];
/* This variable is used to save RRC_DATA_REQ messages that are 
   supposed to be sent CS Domain */
rrcudt_data_req_type  rrcudt_ps_saved_data[MAX_NAS_MESSAGES_FOR_PS_DOMAIN];

/* This variable is used to save RRC_DATA_IND messages that are supposed
   to be forwarded to NAS. */
rrcddt_data_ind_type rrcddt_saved_data[MAX_NAS_MESSAGES_FOR_CS_DOMAIN +
                                       MAX_NAS_MESSAGES_FOR_PS_DOMAIN];


/* These variable saves close session request
  for either PS or CS call.
*/

rrcscrr_close_sess_req_type   rrcscrr_saved_cs_close_session_req;
rrcscrr_close_sess_req_type   rrcscrr_saved_ps_close_session_req;

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
boolean rrc_cs_rab_established = FALSE;
extern rrcrce_csfb_status_e_type csfb_extended_status;
#endif

extern boolean ps_clsd_after_suspend_req_in_cco;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */



/*===========================================================================
** -----------------------------------------------------------------------------------
**-- ----------NON-Demand paging section Srart--------------------------------------------
** -----------------------------------------------------------------------------------
  This section will have code section that will not be demand paged. Function which should be in this section are
  -RRC functions that are called by L1/L2 which are higher priority then RRC
  -RRC functions that are called in ISR context or RTOS timer call back
  -RRC functions that indirectly gets called by L1/L2 /ISR/timer call back   
  For example: Fun1() gets called by L1. Fun2() is called by Fun1(). Now both  Fun1() & Fun2() should be NON demand paged, 
  so both should be added in this section  
  Usage of pragma 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void foo(void)
  {
    function body here
  }
        __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
  If you have another function to be added in the same section then again it needs to be wrapped with these pragma 
  for example:- function bar() is just below function foo() then function bar() needs to be written like this …. 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void bar(void)
  {
    function body here
  }
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
** ----------------------------------------------------------------------- */



/*===========================================================================

FUNCTION rrcscr_get_signalling_connection_status

DESCRIPTION
  This function gets signalling Connections to release status of Domain.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if there is an active Signalling connection. Otherwise it returns
  FALSE

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/

boolean rrcscr_get_signalling_connection_status
(
  rrc_cn_domain_identity_e_type cn_domain_id                   
)
{
  boolean status = FALSE;      /* local variable for status */

  switch (cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID:
      if(rrcdt_int_data.cs_domain_open)
      { 
        status = TRUE;
      }
      break;

    case RRC_PS_DOMAIN_CN_ID:
      if(rrcdt_int_data.ps_domain_open)
      { 
        status = TRUE;
      }
      break;

    default:
      ERR_FATAL("Invalid CN domain: %d", cn_domain_id,0,0);

  }
  return(status);
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

/*===========================================================================

FUNCTION   rrcdt_is_dt_initiated_cu

DESCRIPTION
  This function checks if CU was initiated by IDT procedure
DEPENDENCIES
  None.

RETURN VALUE
  TRUE : If CU was initiated by IDT
  FALSE : Otherwise
===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/

boolean rrcdt_is_dt_initiated_cu
(
  void
)
{
  boolean status = FALSE;
  rrc_state_e_type rrc_state = rrc_get_state();
  if(rrc_state == RRC_STATE_CELL_PCH || rrc_state == RRC_STATE_URA_PCH)
  {
    if(rrcidt_cs_saved_open_session_req.cu_init_by_idt == TRUE ||
      rrcidt_ps_saved_open_session_req.cu_init_by_idt == TRUE)
    {
      status = TRUE;
    }
  }
  return status;
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/


/*===========================================================================

FUNCTION RRCSCRR_PROCESS_SIG_CONN_REL_REQ

DESCRIPTION
  Check the current RRC state and send the sig conn req.If CU is active
  wait for CU to complete before sending the request

DEPENDENCIES
  None.

RETURN VALUE
  void

===========================================================================*/
void rrcscrr_process_sig_conn_rel_req
(
  rrc_cn_domain_identity_e_type 
);

/*===========================================================================

FUNCTION rrcscrr_send_close_session_cnf  

DESCRIPTION
  This function builds the RRC_CLOSE_SESSION_CNF command with parameters and 
  posts the same to the MM task.
DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type: It returns RRCDT_SUCCESS if it is successfully sends 
  the RRC_CLOSE_SESSION_CNF command. Otherwise it returns RRCDT_FAILURE_MAJOR.

===========================================================================*/

void rrcscrr_send_close_session_cnf
(
  rrc_close_session_status_e_type     session_status,  /* Success or failure
                                                          status*/
  rrc_cn_domain_identity_e_type       cn_domain_id     /* CN Domain Id */

);

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION rrcscr_release_signalling_connection

DESCRIPTION
  This function checks whether Signalling connection exist for the particular
  CN Domain and if it exits, then sends RRC_CLOSE_SESSION_IND/CNF to the MM. Also
  sets the local variable for signalling connection status to FALSE.


DEPENDENCIES
  None.

RETURN VALUE
  rrcscr_sig_conn_statue_e_type: it returns RRCSCR_SIG_CONN_RELEASED if there
  exits a signalling connection for the CN domain specified in the formal 
  parameter and if RRCSCR procedure successfully sends RRC_CLOSE_SESSION_IND
  to the MM. Otherwise it retuns RRCSCR_SIG_CONN_NOT_EXIST.

===========================================================================*/
rrcscr_sig_conn_rel_status_e_type rrcscr_local_release_of_signalling_connection
(
  rrc_cn_domain_identity_e_type cn_domain_id,
  rrc_session_close_cause_e_type cause
)
{
  rrcscr_sig_conn_rel_status_e_type status = RRCSCR_SIG_CONN_NOT_EXIST;
                                   /* Local parameter to store the status */
  
  switch(cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID:
       /* Check whether Signalling Connection for CS Domain is open */
      if(rrcdt_int_data.cs_domain_open == TRUE)
      {
        if(RRCDT_SUCCESS == rrcscr_indicate_close_session(cn_domain_id, cause))
        {
          /* Set the Signalling connection status to closed */
          rrcdt_int_data.cs_domain_open = FALSE;
          status = RRCSCR_SIG_CONN_RELEASED;
          WRRC_MSG0_HIGH("CS DOMAIN CLOSE WITH SUCCESS - DELETE ALL SAVED UDTs");
          rrcudt_delete_all_saved_data_req(RRC_CS_DOMAIN_CN_ID);
          if(rrcudt_any_saved_data_req_for_cn_domain(RRC_PS_DOMAIN_CN_ID) == FALSE)
          {
            WRRC_MSG0_HIGH("There is no saved UDT for PS domain, set udt substate to init");
            rrcudt_substate = RRCUDT_INITIAL;
          }
        }
        else
        {
          WRRC_MSG0_ERROR("Failed to post RRC_CLOSE_SESSION_IND");
        }
      }

      break;

    case RRC_PS_DOMAIN_CN_ID:
      /* Check whether Signalling Connection for PS Domain is open */
      if(rrcdt_int_data.ps_domain_open == TRUE)
      {
        if(RRCDT_SUCCESS == rrcscr_indicate_close_session(cn_domain_id, cause))
        {
          WRRC_MSG0_HIGH("Sent RRC_CLOSE_SESSION_IND, PS DOMAIN CLOSE WITH SUCCESS - DELETE ALL SAVED UDTs ");

          /* Set the Signalling connection status to closed */
          rrcdt_int_data.ps_domain_open = FALSE;
          status = RRCSCR_SIG_CONN_RELEASED;
          rrcudt_delete_all_saved_data_req(RRC_PS_DOMAIN_CN_ID);
          if(rrcudt_any_saved_data_req_for_cn_domain(RRC_CS_DOMAIN_CN_ID) == FALSE)
          {
            WRRC_MSG0_HIGH("There is no saved UDT for CS domain, set udt substate to init");
            rrcudt_substate = RRCUDT_INITIAL;
          }
        }
        else
        {
          WRRC_MSG0_ERROR("Failed to post RRC_CLOSE_SESSION_IND");
        }
      }
      break;

    default:
      ERR_FATAL("This should never happen",0,0,0);
      break;
  }
  return (status);
} /* rrcscr_release_signalling_connection */
/*===========================================================================

FUNCTION rrcscrr_is_close_session_ind_needed

DESCRIPTION
  This function will check & delete any pending CLOSE_SESSION_REQ for a  domain.
  based on scrr substae it will send either close_session_cnf/ind or sig_conn_rel 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If close_session_ind needed for a particular CN domain.
  FALSE:Otherwise.
===========================================================================*/
static boolean rrcscrr_is_close_session_ind_needed
(
  rrc_cn_domain_identity_e_type domain_id
)
{
  boolean status = TRUE;
  rrcscrr_close_sess_req_type *save_close_session_req_ptr = NULL; /* Ptr to close session Req*/
  if(domain_id == RRC_CS_DOMAIN_CN_ID)
  {
    save_close_session_req_ptr = &rrcscrr_saved_cs_close_session_req;
  }
  else
  {
    save_close_session_req_ptr = &rrcscrr_saved_ps_close_session_req;
  }
  
  switch(domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID :
      if(( save_close_session_req_ptr->close_sess_req_status == TRUE)|| 
          ( save_close_session_req_ptr->rrc_sig_con_rel_status == TRUE)||
          (save_close_session_req_ptr->mui_status == RRCSCRR_MUI_CLOSE_SESSION_CNF)||
          (save_close_session_req_ptr->mui_status == RRCSCRR_MUI_SIG_CON_REL_CNF))
      {
        if((save_close_session_req_ptr->mui_status == RRCSCRR_MUI_CLOSE_SESSION_CNF)||
          ( save_close_session_req_ptr->close_sess_req_status == TRUE))
        {
          WRRC_MSG2_HIGH("mui_status=%d, close_sess_req_status =%d",save_close_session_req_ptr->mui_status,
            save_close_session_req_ptr->close_sess_req_status);
          rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_SUCCESS,RRC_CS_DOMAIN_CN_ID);
          status = FALSE;
        }
        else if((save_close_session_req_ptr->mui_status == RRCSCRR_MUI_SIG_CON_REL_CNF)||
           (save_close_session_req_ptr->rrc_sig_con_rel_status == TRUE))
        {
          MSG_HIGH("mui_status=%d, rrc_sig_con_rel_status =%d proc: rrc_proc_e_type_value%d",save_close_session_req_ptr->mui_status,
            save_close_session_req_ptr->rrc_sig_con_rel_status,
            save_close_session_req_ptr->rrc_proc);
          rrcscrr_send_sig_conn_release_cnf(
                        SUCCESS, 
                        save_close_session_req_ptr->rrc_proc);
          status = FALSE;
        }
        /*Its safe to call this function to delete rrcscrr_saved_cs_close_session_req here with out 
        *any condition
        */
        rrcscrr_delete_close_session_req(RRC_CS_DOMAIN_CN_ID);
        if(rrcscrr_saved_ps_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE)
        {
          WRRC_MSG2_HIGH("Scrr sub state is %d,cont.. in this state as mui state of PS is %d ",rrcscrr_substate,
            rrcscrr_saved_ps_close_session_req.mui_status);
        }
        else if((rrcscrr_saved_ps_close_session_req.close_sess_req_status == TRUE)||
          (rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status == TRUE))
        {
          MSG_HIGH("Scrr sub state is %d,cont.. in this state as close_req=%d sig_con_req=%d ",rrcscrr_substate,
            rrcscrr_saved_ps_close_session_req.close_sess_req_status,
            rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status);
        }
        else
        {
          rrcscrr_substate = RRCSCRR_INITIAL; 
        }
        WRRC_MSG2_HIGH("Scrr sub state is %d,cn domain=%d",rrcscrr_substate,domain_id);
      }
      else
      {
        /*Its safe to call this function to delete rrcscrr_saved_cs_close_session_req here with out 
        *any condition
        */
        MSG_HIGH("Scrr sub state is %d,cn domain=%d,PS mui_status=%d",
               rrcscrr_substate,
               domain_id,
               rrcscrr_saved_ps_close_session_req.mui_status);
        rrcscrr_delete_close_session_req(RRC_CS_DOMAIN_CN_ID);
        if(rrcscrr_saved_ps_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE)
        {
          WRRC_MSG2_HIGH("Scrr sub state is %d,cont.. in this state as mui state of PS is %d ",rrcscrr_substate,
            rrcscrr_saved_ps_close_session_req.mui_status);
        }
        else if((rrcscrr_saved_ps_close_session_req.close_sess_req_status == TRUE)||
          (rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status == TRUE))
        {
          MSG_HIGH("Scrr sub state is %d,cont.. in this state as close_req=%d sig_con_req=%d ",rrcscrr_substate,
            rrcscrr_saved_ps_close_session_req.close_sess_req_status,
            rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status);
        }
        else
        {
          rrcscrr_substate = RRCSCRR_INITIAL; 
        }
        WRRC_MSG2_HIGH("Scrr sub state is %d,cn domain=%d",rrcscrr_substate,domain_id);
      }
    break;
  
    case RRC_PS_DOMAIN_CN_ID :
      if(( save_close_session_req_ptr->close_sess_req_status == TRUE)|| 
          ( save_close_session_req_ptr->rrc_sig_con_rel_status == TRUE)||
          (save_close_session_req_ptr->mui_status == RRCSCRR_MUI_CLOSE_SESSION_CNF)||
          (save_close_session_req_ptr->mui_status == RRCSCRR_MUI_SIG_CON_REL_CNF))
      {
        if((save_close_session_req_ptr->mui_status == RRCSCRR_MUI_CLOSE_SESSION_CNF)||
          ( save_close_session_req_ptr->close_sess_req_status == TRUE))
        {
          WRRC_MSG2_HIGH("mui_status=%d, close_sess_req_status =%d",save_close_session_req_ptr->mui_status,
            save_close_session_req_ptr->close_sess_req_status);
          rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_SUCCESS,RRC_PS_DOMAIN_CN_ID);
          status = FALSE;
        }
        else if((save_close_session_req_ptr->mui_status == RRCSCRR_MUI_SIG_CON_REL_CNF)||
           (save_close_session_req_ptr->rrc_sig_con_rel_status == TRUE))
        {
          MSG_HIGH("mui_status=%d, rrc_sig_con_rel_status =%d proc:%d",save_close_session_req_ptr->mui_status,
            save_close_session_req_ptr->rrc_sig_con_rel_status,
            save_close_session_req_ptr->rrc_proc);
          rrcscrr_send_sig_conn_release_cnf(
                        SUCCESS,
                        save_close_session_req_ptr->rrc_proc);
          status = FALSE;
        }
        /*Its safe to call this function to delete rrcscrr_saved_cs_close_session_req here with out 
        *any condition
        */
        rrcscrr_delete_close_session_req(RRC_PS_DOMAIN_CN_ID);
        if(rrcscrr_saved_cs_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE)
        {
          WRRC_MSG2_HIGH("Scrr sub state is %d,cont.. in this state as mui state of CS is %d ",rrcscrr_substate,
            rrcscrr_saved_cs_close_session_req.mui_status);
        }
        else if((rrcscrr_saved_cs_close_session_req.close_sess_req_status == TRUE)||
          (rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status == TRUE))
        {
          MSG_HIGH("Scrr sub state is %d,cont.. in this state as close_req=%d sig_con_req=%d ",rrcscrr_substate,
            rrcscrr_saved_cs_close_session_req.close_sess_req_status,
            rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status);
        }
        else
        {
          rrcscrr_substate = RRCSCRR_INITIAL; 
        }
        WRRC_MSG2_HIGH("Scrr sub state is %d,cn domain=%d",rrcscrr_substate,domain_id);
      }
      else
      {
        /*Its safe to call this function to delete rrcscrr_saved_cs_close_session_req here with out 
        *any condition
        */
        MSG_HIGH("Scrr sub state is %d,cn domain=%d,CS mui_status=%d",
             rrcscrr_substate,
             domain_id,
             rrcscrr_saved_cs_close_session_req.mui_status);
        rrcscrr_delete_close_session_req(RRC_PS_DOMAIN_CN_ID);
        if(rrcscrr_saved_cs_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE)
        {
          WRRC_MSG2_HIGH("Scrr sub state is %d,cont.. in this state as mui state of CS is %d ",rrcscrr_substate,
            rrcscrr_saved_cs_close_session_req.mui_status);
        }
        else if((rrcscrr_saved_cs_close_session_req.close_sess_req_status == TRUE)||
          (rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status == TRUE))
        {
          MSG_HIGH("Scrr sub state is %d,cont.. in this state as close_req=%d sig_con_req=%d ",rrcscrr_substate,
            rrcscrr_saved_cs_close_session_req.close_sess_req_status,
            rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status);
        }
        else
        {
          rrcscrr_substate = RRCSCRR_INITIAL; 
        }
        WRRC_MSG2_HIGH("Scrr sub state is %d,cn domain=%d",rrcscrr_substate,domain_id);
      }
    break;
  
    default:
      WRRC_MSG1_HIGH("Incorrect CN Domain I.D=%d",domain_id);
    break;
  }
  return(status);
}

/*===========================================================================

FUNCTION rrcscr_indicate_close_session

DESCRIPTION
  This function will check & delete any pending CLOSE_SESSION_REQ for a  domain.
  based on scrr substae it will send either close_session_cnf/ind or sig_conn_rel 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If IDT is buffered for a particular CN domain.
  FALSE:Otherwise.
===========================================================================*/
rrcdt_status_e_type rrcscr_indicate_close_session
(
  rrc_cn_domain_identity_e_type       cn_domain_id,     /* CN Domain Id */
  rrc_session_close_cause_e_type      cause
)
{
  rrcdt_status_e_type status = RRCDT_SUCCESS;
  if(rrcscrr_is_close_session_ind_needed(cn_domain_id) == TRUE)
  {
    status = rrcscr_send_close_session_ind(cn_domain_id, cause);
    WRRC_MSG0_HIGH("Sent RRC_CLOSE_SESSION_IND");

    if(RRC_PS_DOMAIN_CN_ID == cn_domain_id)
    {
      /* Abort Dormancy if SCRI for CLOSE_SESSION_REQ is sended */
      rrcdormancy_check_and_abort_dormancy_req(RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);
    }
  }
  
  return(status);
}
/*===========================================================================

FUNCTION rrcidt_is_idt_buffered

DESCRIPTION
  This function will check & return the status of 
  IDT is being buffered for the domain.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If IDT is buffered for a particular CN domain.
  FALSE:Otherwise.
===========================================================================*/
static boolean rrcidt_is_idt_buffered(rrc_cn_domain_identity_e_type domain_id)
{
  boolean status = FALSE;

    switch(domain_id)
    {
      case RRC_CS_DOMAIN_CN_ID :
      if(rrcidt_cs_saved_open_session_req.open_sess_req_status == TRUE)
      {
        status = TRUE;
      }
      break;

      case RRC_PS_DOMAIN_CN_ID :
      if(rrcidt_ps_saved_open_session_req.open_sess_req_status == TRUE)
      {
        status = TRUE;
      }
      break;

     default:
       WRRC_MSG1_HIGH("Incorrect CN Domain I.D=%d",domain_id);
       break;
    }
return status;
}
/*===========================================================================

FUNCTION RRCDT_PURGE_RLC_WM_RB3_RB4

DESCRIPTION

  This function requests RLC to purge RLC UL  watermark for RB3 and RB4
  
DEPENDENCIES


RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
static void rrcdt_purge_rlc_wm_rb3_rb4
(    
  boolean purge_srb3, boolean purge_srb4, rrc_proc_e_type proc_id  
)
{
  uint8 count = 0;            /* Count of LC Ids to be purged */
  rrc_cmd_type *llc_cmd_ptr;  /* Pointer to the RRC Command */

  /* Send CRLC DATA IND REQ to RLC via rrcllc */
  if((llc_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    l2_ul_req_cmd * llc_cmd_l2_ptr = NULL;
    RRC_GET_POINTER_L2_REQ_VALUE(llc_cmd_ptr->cmd.rrc_llc_req.l2_ul_req,llc_cmd_l2_ptr);

    /* Initialize the LLC command header first */
    llc_cmd_ptr->cmd_hdr.cmd_id = RRC_LLC_CMD_REQ;
    llc_cmd_ptr->cmd.rrc_llc_req.procedure = proc_id;
    llc_cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER2_RLC_UL;

    /* Fill the L2 cmd header */
    llc_cmd_ptr->cmd.rrc_llc_req.l2_ul_cmd.cmd_id = RLC_UL_PURGE_WM_REQ;

    /* Include the lc_ids for signalling RBs with RLC AM mode. */
    if(purge_srb3)
    {
      llc_cmd_l2_ptr->ul_purge_wm_ind.lc_id[count] =
        rrclcm_get_ul_rlc_lc_id(DCCH_DT_HIGH_PRI_RADIO_BEARER_ID);
      llc_cmd_l2_ptr->ul_purge_wm_ind.num_sdus[count++] = 
        MAX_SRB2_SDUS_TOBE_PURGED;
    }
    if(purge_srb4)
    {
      llc_cmd_l2_ptr->ul_purge_wm_ind.lc_id[count] =
        rrclcm_get_ul_rlc_lc_id(DCCH_DT_LOW_PRI_RADIO_BEARER_ID);
      llc_cmd_l2_ptr->ul_purge_wm_ind.num_sdus[count++] = 
        MAX_SRB2_SDUS_TOBE_PURGED;
    }

    /* Populate the total number of LCs to be purged */
    llc_cmd_l2_ptr->ul_purge_wm_ind.num_lc_id = count;
    MSG_HIGH("Purging RLC SRB3:%d, SRB4:%d, proc_id:%d",purge_srb3,purge_srb4,proc_id);
    rrc_put_int_cmd(llc_cmd_ptr); 
  }
}

/*===========================================================================

FUNCTION RRCDT_CHECK_AND_SUSPEND_BPLMN

DESCRIPTION
  This functions checks if BPLMN is in progress and if it is,then sends 
  a Suspend Req to CSP with the establishment cause

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcdt_check_and_suspend_bplmn
(
  rrc_establish_cause_e_type  est_cause
)
{
  rrc_bplmn_suspend_cause_e_type suspend_cause;

  WRRC_MSG0_HIGH("Checking if BPLMN is active when RRC_OPEN_SESSION_REQ is received");

  if(WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
  {
    WRRC_MSG0_HIGH( "BPLMN active when RRC_OPEN_SESSION_REQ is received");
     /* Map the establishment cause to the appropriate suspend cause */
    switch(est_cause)
    {
       case RRC_EST_REGISTRATION:
         suspend_cause = BPLMN_SUSPEND_REASON_REGISTRATION;
         break;
       default:
         /* BPLMN search will be suspended for any establishment cause from NAS */
         suspend_cause = BPLMN_SUSPEND_REASON_REGISTRATION; 
         break;
    }
    /* Send the bplmn suspend request */
    rrc_csp_send_bplmn_suspend_req(RRC_PROCEDURE_IDT,suspend_cause);
  }
}

/*===========================================================================

FUNCTION RRCDT_CHECK_AND_SEND_PENDING_SCRR

DESCRIPTION
  Processes pending SCRR if any after IDT has been completed

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If SCRR for the domain was buffered and is being sent out now
  FALSE - SCRR for the domain is not active

===========================================================================*/
static boolean rrcdt_check_and_send_pending_scrr
(
  rrc_cn_domain_identity_e_type domain_id  
)
{
  /* Boolean to indicate that there was a SCRR pending and it is being sent out 
     now.So if IDT had received a NACK,it need not pursue the re-transmissions since 
     the signalling connection is being released */
  boolean ret_val = FALSE;

  /* Check to see if there is a SCRR yet to be processed for this domain.Make sure to
     check the mui status also because retranmisssions are not handled in this function.
     This is only for sending the SCRR out for the first time because it was delayed
     due to IDT being active at that time */
  switch(domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID:
      if(((rrcscrr_saved_cs_close_session_req.close_sess_req_status == TRUE)||
          (rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status == TRUE)) &&
         (rrcscrr_saved_cs_close_session_req.mui_status == RRCSCRR_MUI_STATUS_FALSE))
      {
        WRRC_MSG0_HIGH("Processing saved SCRR for CS domain after IDT indication");
        rrcscrr_process_sig_conn_rel_req(domain_id);
        ret_val = TRUE;
      }
      break;

    case RRC_PS_DOMAIN_CN_ID:
      if(((rrcscrr_saved_ps_close_session_req.close_sess_req_status == TRUE)||
          (rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status == TRUE)) &&
         (rrcscrr_saved_ps_close_session_req.mui_status == RRCSCRR_MUI_STATUS_FALSE))
      {
        WRRC_MSG0_HIGH("Processing saved SCRR for PS domain after IDT indication");
        rrcscrr_process_sig_conn_rel_req(domain_id);
        ret_val = TRUE;
      }
      break;

    default:
      WRRC_MSG1_ERROR("Invalid domain id %d",domain_id);
      break;
  }
  return ret_val;
}

/*===========================================================================

FUNCTION rrcidt_check_if_est_cause_available  

DESCRIPTION
  This function gets the establishment cause value to be filled in OTA message.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE indicates valid value is present.

===========================================================================*/
boolean rrcidt_check_if_est_cause_available
(
       rrc_proc_e_type proc_id,
       rrc_establish_cause_e_type  *est_cause
       ,rrc_call_type_in_est_req_e_type *call_type
)
{
  boolean ret_val =FALSE;


  if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)) 
  {
    if((rrcidt_est_cause_info.valid == TRUE) && (rrcidt_est_cause_info.to_be_sent_by == proc_id))
    {
      ret_val = TRUE;
      *est_cause = rrcidt_est_cause_info.est_cause;
      *call_type = rrcidt_est_cause_info.call_type;
    }
  }
  return ret_val;
}

/*===========================================================================

FUNCTION rrcidt_check_if_est_cause_available  

DESCRIPTION
  This function gets the establishment cause value to be filled in OTA message.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE indicates valid value is present.

===========================================================================*/
boolean rrcidt_is_cs_call_initiate
(
  void
)
{
  boolean ret_val =FALSE;
  if(rrcidt_cs_saved_open_session_req.cu_init_by_idt ==TRUE)
  {
    ret_val = TRUE;
  }
  return ret_val;
}


/*===========================================================================

FUNCTION rrcidt_change_owner_est_cause  

DESCRIPTION
  This function updates the to_be_sent flag with procedure id
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcidt_change_owner_est_cause
(
    rrc_proc_e_type proc_id 
)
{
  if(proc_id == RRC_PROCEDURE_NONE)
  {
    return;
  }
  if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)) 
  {
    if(rrcidt_est_cause_info.valid == TRUE)
    {
      rrcidt_est_cause_info.to_be_sent_by=proc_id;
    }
  }
}
    



/*===========================================================================

FUNCTION rrcidt_fill_est_cause  

DESCRIPTION
  This function fills the establishment cause in ASN structure.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcidt_fill_est_cause
(
  rrc_UL_DCCH_Message     *msg_ptr         /* Pointer to uplink DCCH message msg_ptr*/,
  rrc_cn_domain_identity_e_type       dom_id   
)
{
  rrc_establish_cause_e_type  est_cause;
  rrc_EstablishmentCause  trans_cause;
  rrc_call_type_in_est_req_e_type call_type;

  if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)) 
  {
    if(rrcidt_check_if_est_cause_available(RRC_PROCEDURE_IDT, &est_cause
                                                                    ,&call_type
                                                                    )== TRUE)
    {
      WRRC_MSG0_HIGH("Filling the establishment cause ");
      /* Enable the v3a0NonCriticalExtension structure */
  
     /* Enable the laterNonCriticalExtension field in v3a0NoncriticalExtension structure*/
      RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions,
                                rrc_InitialDirectTransfer_v3a0NonCriticalExtensions_,
                                laterNonCriticalExtensions);
  
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
        laterNonCriticalExtensions);
  
      /* Enable the v590NonCriticalExtension field in laterNonCriticl in v3a0Non critical */
      RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                                laterNonCriticalExtensions,
                                rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_,
                                v590NonCriticalExtensions);
  
      /* set bit_mask to 0 to indicate nonCriticalExtensions to v590 extensions are not present */
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
               v590NonCriticalExtensions);
  
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
               v590NonCriticalExtensions.initialDirectTransfer_v590ext);
  
      /* Enable the establishment cause */
      RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                          laterNonCriticalExtensions.v590NonCriticalExtensions.initialDirectTransfer_v590ext,
                          rrc_,
                          establishmentCause);

      if(dom_id == RRC_CS_DOMAIN_CN_ID)
      {
        est_cause = rrcidt_cs_saved_open_session_req.est_cause;
      }
      else
      {
        est_cause = rrcidt_ps_saved_open_session_req.est_cause;
      }
  
      rrc_translate_establishment_cause(est_cause, &trans_cause);
  
      /* Fill the establishment cause */
      msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
               v590NonCriticalExtensions.initialDirectTransfer_v590ext.establishmentCause = trans_cause;            

      if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
      {
        if((rrc_get_state() == RRC_STATE_CELL_FACH) && 
          (OC_SET_FOR_CELL_DCH != rrcllc_get_ordered_config_status())&&
          (dom_id ==RRC_CS_DOMAIN_CN_ID))
        {
          if(est_cause == RRC_EST_ORIG_CONV_CALL ||
             est_cause == RRC_EST_EMERGENCY_CALL)
          {
            if(!(RRCRBR_R7_MSG_IE_PRESENT(&msg_ptr->message.u.initialDirectTransfer,
                    rrc_InitialDirectTransfer,
                    v3a0NonCriticalExtensions)))
            {
              RRCSET_R7_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer,
                    rrc_InitialDirectTransfer_,
                    v3a0NonCriticalExtensions);
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions);
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.initialDirectTransfer_v3a0ext);
            }
            if(!(RRCRBR_R7_MSG_IE_PRESENT(&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions,
                     rrc_InitialDirectTransfer_v3a0NonCriticalExtensions,
                     laterNonCriticalExtensions)))
            {
              RRCSET_R7_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions,
                     rrc_InitialDirectTransfer_v3a0NonCriticalExtensions_,
                     laterNonCriticalExtensions);
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions);
            }
            if(!(RRCRBR_R7_MSG_IE_PRESENT(&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                     laterNonCriticalExtensions,
                     rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions,
                     v590NonCriticalExtensions)))
            {
              RRCSET_R7_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                     laterNonCriticalExtensions,
                     rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_,
                     v590NonCriticalExtensions);
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                v590NonCriticalExtensions);
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                v590NonCriticalExtensions.initialDirectTransfer_v590ext);
            }
            if(!(RRCRBR_R7_MSG_IE_PRESENT(&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                     laterNonCriticalExtensions.v590NonCriticalExtensions,
                     rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_v590NonCriticalExtensions,
                     v690NonCriticalExtensions)))
            {
              RRCSET_R7_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                     laterNonCriticalExtensions.v590NonCriticalExtensions,
                     rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_v590NonCriticalExtensions_,
                     v690NonCriticalExtensions);
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                v590NonCriticalExtensions.v690NonCriticalExtensions);
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                v590NonCriticalExtensions.v690NonCriticalExtensions.initialDirectTransfer_v690ext);
            }
            if(!(RRCRBR_R7_MSG_IE_PRESENT(&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                     laterNonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions,
                     rrc_v590NonCriticalExtensions_v690NonCriticalExtensions,
                     v770NonCriticalExtensions)))
            {
              RRCSET_R7_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                     laterNonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions,
                     rrc_v590NonCriticalExtensions_v690NonCriticalExtensions_,
                     v770NonCriticalExtensions);
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                v590NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions);
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                v590NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.initialDirectTransfer_v770ext);
            }
            RRCSET_R7_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                    laterNonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
                    v770NonCriticalExtensions.initialDirectTransfer_v770ext,
                    rrc_InitialDirectTransfer_v770ext_IEs_,
                    csCallType);
            WRRC_MSG2_HIGH("Establishment cause : %d Call Type : %d " ,est_cause,call_type);
            
            switch(call_type)
            {
              case RRC_CALL_TYPE_SPEECH:
                msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                           v590NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.initialDirectTransfer_v770ext.
                                                              csCallType = rrc_InitialDirectTransfer_v770ext_IEs_csCallType_speech;
                break;
              case RRC_CALL_TYPE_VIDEO:
                msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                           v590NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.initialDirectTransfer_v770ext.
                                                              csCallType = rrc_InitialDirectTransfer_v770ext_IEs_csCallType_video;
                break;
              case RRC_CALL_TYPE_OTHERS:
                msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                           v590NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.initialDirectTransfer_v770ext.
                                                              csCallType = rrc_InitialDirectTransfer_v770ext_IEs_csCallType_other;
                break;
              default:
                WRRC_MSG1_ERROR("Wrong est call type set by NAS = %d",call_type);
                msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                           v590NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.initialDirectTransfer_v770ext.
                                                              csCallType = rrc_InitialDirectTransfer_v770ext_IEs_csCallType_other;
                break;
            }
          }
        }
      }

    }
  }
}


/*===========================================================================

FUNCTION rrcidt_set_sent_by_cu  

DESCRIPTION
  This function sets the variable to indicate the transmission was done by CU
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcidt_set_sent_by_cu
(   
  rrc_proc_e_type proc_id
)
{


  if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)) 
  {
    rrc_establish_cause_e_type  est_cause;

    rrc_call_type_in_est_req_e_type call_type;

    if(rrcidt_check_if_est_cause_available(proc_id,&est_cause
                                                                    ,&call_type
                                                                   )==TRUE)
    {
      rrcidt_est_cause_info.sent_by_cu= TRUE;
    }
  }
}

/*===========================================================================

FUNCTION rrcidt_set_est_cause  

DESCRIPTION
  This function updates the establishment cause to be used later while transmitting.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcidt_set_est_cause
(  
   rrc_establish_cause_e_type  est_cause, 
   rrc_proc_e_type proc_id,
   rrc_cn_domain_identity_e_type       cn_domain_id     /* CN Domain Id */
)
{

  if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)) 
  {
    boolean multi_rab=FALSE;
    if(rrcidt_est_cause_info.last_proc_sent == RRC_PROCEDURE_RCE)
    {/*First IDT will not carry EST_cause*/
      rrcidt_est_cause_info.est_cause = est_cause;
      rrcidt_est_cause_info.last_proc_sent = proc_id;
      rrcidt_est_cause_info.valid = FALSE;
    }
    else
    {  /*All subsequent IDT's will carry EST_cause which 
         uses same rrc connection*/
      rrcidt_est_cause_info.last_proc_sent = proc_id;

      rrcidt_est_cause_info.valid = TRUE;
        rrcidt_est_cause_info.est_cause=est_cause;
      rrcidt_est_cause_info.to_be_sent_by = proc_id;

      if(cn_domain_id == RRC_CS_DOMAIN_CN_ID)
      {
        rrcidt_est_cause_info.call_type = rrcidt_cs_saved_open_session_req.open_session_req.call_type;
      }

      //multi_rab is retained as such though no use since its used below for debugging 
      // purpose. If its not needed this can be removed
      multi_rab = TRUE; 
    }
    MSG_HIGH("last_proc_sent = %d, est_cause=%d & is valid =%d ",rrcidt_est_cause_info.last_proc_sent,
                                                                                                    rrcidt_est_cause_info.est_cause,
                                                                                                    rrcidt_est_cause_info.valid);
    WRRC_MSG2_HIGH("multi_rab status =%d, prcd to be sent by =%d",multi_rab,rrcidt_est_cause_info.to_be_sent_by);
  }
}
/*===========================================================================

FUNCTION rrcidt_update_proc_id  

DESCRIPTION
  This function will be called by RCE to update the variable 
  last_proc_sent so that IDT cause can be skipped from filling
  for the first time

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcidt_update_proc_id()
{
  rrcidt_est_cause_info.last_proc_sent = RRC_PROCEDURE_RCE;
}
/*===========================================================================

FUNCTION rrcidt_check_if_transmission_success  

DESCRIPTION
  This function checks if CU was successful in transmitting it over the air
  This can happen if priority of cell update cause calculation.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcidt_check_if_transmission_success
(  
   rrc_proc_e_type proc_id
)
{

  if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)) 
  {
    rrc_establish_cause_e_type  est_cause;

    rrc_call_type_in_est_req_e_type call_type;

    if (rrcidt_check_if_est_cause_available(proc_id,&est_cause
                                                                    ,&call_type
                                                                    )==TRUE)
    {
      if(rrcidt_est_cause_info.sent_by_cu==FALSE)
      {
        rrcidt_est_cause_info.to_be_sent_by =RRC_PROCEDURE_IDT;
      }
    }
  }
}



/*===========================================================================

FUNCTION rrcidt_reset_est_cause  

DESCRIPTION
  This function resets the establishment cause once the transmission is successful.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcidt_reset_est_cause
(  
   rrc_proc_e_type proc_id)
{


  if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)) 
  {
    if((rrcidt_est_cause_info.valid == TRUE) && 
       (rrcidt_est_cause_info.to_be_sent_by == proc_id))
    {
      rrcidt_est_cause_info.valid = FALSE;
      rrcidt_est_cause_info.est_cause=RRC_EST_TERM_UNKNOWN;
      rrcidt_est_cause_info.to_be_sent_by = RRC_PROCEDURE_NONE;
      rrcidt_est_cause_info.sent_by_cu = FALSE;

      rrcidt_est_cause_info.call_type = RRC_CALL_TYPE_NONE;
    }
  }
}


/*===========================================================================

FUNCTION rrcidt_delete_open_session_req

DESCRIPTION
  This function deletes the open session request message contents which
  were allocated memory by CELL_FACH, URA_PCH/CELL_PCH states
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrcidt_delete_open_session_req
(
  rrc_cn_domain_identity_e_type       cn_domain_id     /* CN Domain Id */
)
{
  if(cn_domain_id == RRC_CS_DOMAIN_CN_ID)
  {
    rrcidt_cs_saved_open_session_req.open_sess_req_status = FALSE;
    rrcidt_cs_saved_open_session_req.mui_status = FALSE;
    rrcidt_cs_saved_open_session_req.open_sess_cnf_required = FALSE;
    rrcidt_cs_saved_open_session_req.cu_init_by_idt = FALSE;
    rrcidt_cs_saved_open_session_req.fresh_start_value_needed = FALSE;

    /*delete heap memory if it were allocated*/
    if(rrcidt_cs_saved_open_session_req.open_session_req.nas_msg.value != NULL)
    {
      rrc_free( rrcidt_cs_saved_open_session_req.open_session_req.nas_msg.value);

      rrcidt_cs_saved_open_session_req.open_session_req.nas_msg.value = NULL;
      rrcidt_cs_saved_open_session_req.open_session_req.nas_msg.length = 0;
    }
    rrcidt_cs_saved_open_session_req.est_cause = RRC_EST_TERM_UNKNOWN;
    rrcidt_reset_est_cause(RRC_PROCEDURE_IDT);

    rrcidt_cs_saved_open_session_req.open_session_req.call_type = RRC_CALL_TYPE_NONE;
  }
  else if(cn_domain_id == RRC_PS_DOMAIN_CN_ID)
  {
    rrcidt_ps_saved_open_session_req.open_sess_req_status = FALSE;
    rrcidt_ps_saved_open_session_req.mui_status = FALSE;
    rrcidt_ps_saved_open_session_req.open_sess_cnf_required = FALSE;
    rrcidt_ps_saved_open_session_req.cu_init_by_idt = FALSE;
    rrcidt_ps_saved_open_session_req.fresh_start_value_needed = FALSE;

    /*delete heap memory if it were allocated*/
    if(rrcidt_ps_saved_open_session_req.open_session_req.nas_msg.value != NULL)
    {
      rrc_free( rrcidt_ps_saved_open_session_req.open_session_req.nas_msg.value);

      rrcidt_ps_saved_open_session_req.open_session_req.nas_msg.value = NULL;
      rrcidt_ps_saved_open_session_req.open_session_req.nas_msg.length = 0;

    }
    rrcidt_ps_saved_open_session_req.est_cause=RRC_EST_TERM_UNKNOWN;
    rrcidt_reset_est_cause(RRC_PROCEDURE_IDT);
  }
  else
  {
    ERR_FATAL("Invalid Domain ID %d", cn_domain_id, 0, 0);
  }

  /* Check and send any blocked SCRR */
  (void)rrcdt_check_and_send_pending_scrr(cn_domain_id);

}/*rrcidt_delete_open_session_req*/
/*===========================================================================

FUNCTION   rrcidt_is_idt_active

DESCRIPTION
  This function checks if idt for the given domain is in progress
DEPENDENCIES
  None.

RETURN VALUE
  TRUE indicates that it is active, FALSE otherwise.

===========================================================================*/

boolean rrcidt_is_idt_active
(
   rrc_cn_domain_identity_e_type domain_id
)
{
  boolean ret_val = FALSE;
  switch(domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID:
      /* first case says that open session is received, eg waiting for CU */
      /* second condition says that we are waiting for l2ack */
      if((rrcidt_cs_saved_open_session_req.open_sess_req_status == TRUE)||
         (rrcidt_cs_saved_open_session_req.open_sess_cnf_required == TRUE))
      {
        ret_val = TRUE;
      }
      break;

    case RRC_PS_DOMAIN_CN_ID:
      /* first case says that open session is received, eg waiting for CU */
      /* second condition says that we are waiting for l2ack */
      if((rrcidt_ps_saved_open_session_req.open_sess_req_status == TRUE)||
         (rrcidt_ps_saved_open_session_req.open_sess_cnf_required == TRUE))
      {
        ret_val = TRUE;
      }
      break;

    default:
          
      WRRC_MSG0_HIGH("Invalid domain id");
      break;
  }
  return (ret_val);
}
/*===========================================================================

FUNCTION rrcidt_can_buffered_idt_be_deleted

DESCRIPTION
  This function will check whether IDT has not initiated CU
  (a case in cell/URA  PCH),If yes then it shall delete IDT, else
  this funtion does nothing.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:If IDt is deleted.
  FALSE:Otherwise.

===========================================================================*/
static boolean rrcidt_can_buffered_idt_be_deleted
(
  rrc_cn_domain_identity_e_type domain_id
)
{
  boolean status=FALSE;
  switch(domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID:
      if(rrcidt_cs_saved_open_session_req.cu_init_by_idt == FALSE)
      {
        rrcidt_delete_open_session_req(RRC_CS_DOMAIN_CN_ID);
        status = TRUE;
        if(FALSE == rrcidt_is_idt_active(RRC_PS_DOMAIN_CN_ID))
        {
          rrcidt_substate = RRCIDT_INITIAL;  
        }
      }
      break;

    case RRC_PS_DOMAIN_CN_ID:
      if(rrcidt_ps_saved_open_session_req.cu_init_by_idt == FALSE)
      {
        rrcidt_delete_open_session_req(RRC_PS_DOMAIN_CN_ID);
        status = TRUE;
        if(FALSE == rrcidt_is_idt_active(RRC_CS_DOMAIN_CN_ID))
        {
          rrcidt_substate = RRCIDT_INITIAL;  
        }
      }
      break;

    default:
      WRRC_MSG0_HIGH("Invalid domain id");
      break;
  }
  return status;
}
/*===========================================================================

FUNCTION   rrcidt_clear_idt_data_for_race_conditions

DESCRIPTION

RETURN VALUE
  

===========================================================================*/
static void rrcidt_clear_idt_data_for_race_conditions
(
   rrc_cn_domain_identity_e_type domain_id
)
{
  /* wait for IDT to complete if it is still in progress*/
  if( rrcidt_is_idt_active(domain_id) == TRUE)
  {
    WRRC_MSG1_HIGH("DELETE IDT active for domain %d", domain_id);

    switch(domain_id)
    {
      case RRC_CS_DOMAIN_CN_ID:
        rrcidt_delete_open_session_req(RRC_CS_DOMAIN_CN_ID);
        if(FALSE == rrcidt_is_idt_active(RRC_PS_DOMAIN_CN_ID))
        {
          rrcidt_substate = RRCIDT_INITIAL;  
        }
        break;

      case RRC_PS_DOMAIN_CN_ID:
        rrcidt_delete_open_session_req(RRC_PS_DOMAIN_CN_ID);
        if(FALSE == rrcidt_is_idt_active(RRC_CS_DOMAIN_CN_ID))
        {
          rrcidt_substate = RRCIDT_INITIAL;  
        }
        break;

      default:
        WRRC_MSG0_HIGH("Invalid domain id");
        break;
    }
  }
}
/*===========================================================================

FUNCTION rrcidt_send_open_session_cnf  

DESCRIPTION
  This function builds the RRC_OPEN_SESSION_CNF command with parameters and 
  posts the same to the MM task.
DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type: It returns RRCDT_SUCCESS if it is successfully sends 
  the RRC_OPEN_SESSION_CNF command. Otherwise it returns RRCDT_FAILURE_MAJOR.

===========================================================================*/

void rrcidt_send_open_session_cnf
(
  rrc_open_session_status_e_type      session_status,  /* Success or failure
                                                          status*/
  rrc_cn_domain_identity_e_type       cn_domain_id     /* CN Domain Id */

)
{
  mm_cmd_type *cmd_ptr;                      /* Pointer to the MM Command */

  /* Allocate memory for RRC_OPEN_SESSION_CNF command */
  if( (cmd_ptr =  mm_rrc_get_cmd_buf(RRC_OPEN_SESSION_CNF)) != NULL)
  {
    /* Fill in parameters fro RRC_OPEN_SESSION_CNF command */
    cmd_ptr->cmd.rrc_open_session_cnf.cn_domain_id= cn_domain_id ;

    cmd_ptr->cmd.rrc_open_session_cnf.status = session_status;

#ifdef FEATURE_DUAL_SIM
    cmd_ptr->cmd.rrc_open_session_cnf.as_id = rrc_get_as_id();
#endif

    rrc_send_rrc_message_to_mm(cmd_ptr);               /* Sends the command to MM */

    WRRC_MSG0_HIGH("Dispatching out MM cmd RRC_OPEN_SESSION_CNF/ Sent RRC_OPEN_SESS_CNF to MM / Dispatching out MM cmd RRC_OPEN_SESSION_CNF");

  }    

} /* rrcidt_send_open_session_cnf */

/*===========================================================================

FUNCTION rrcidt_send_sync_ind  

DESCRIPTION
  This function builds the RRC_SYND_IND command with parameters and 
  posts the same to the MM task.
DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type: It returns RRCDT_SUCCESS if it is successfully sends 
  the RRC_SYNC_IND command. Otherwise it returns RRCDT_FAILURE_MAJOR.

===========================================================================*/

static void rrcidt_send_sync_ind
(
  rrc_sync_cause_e_type               cause_status,   /*  Cause Status */
  rrc_cn_domain_identity_e_type       cn_domain_id     /* CN Domain Id */

)
{
  mm_cmd_type *cmd_ptr;                      /* Pointer to the MM Command */

  /* Allocate memory for RRC_OPEN_SESSION_CNF command */
  if( (cmd_ptr =  mm_rrc_get_cmd_buf(RRC_SYNC_IND)) != NULL)
  {
    /* Fill in parameters fro RRC_SYNC_IND command */
    cmd_ptr->cmd.rrc_sync_ind.cn_domain_id= cn_domain_id ;

    cmd_ptr->cmd.rrc_sync_ind.cause = cause_status;

    cmd_ptr->cmd.rrc_sync_ind.rab_id_present = FALSE;

#ifdef FEATURE_VOC_AMR_WB
    /* Fill in the codec type used in RRC_SYNC_IND*/
    cmd_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_NONE;
#endif /*FEATURE_VOC_AMR_WB*/

#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND
     #error code not present
#endif

#ifdef FEATURE_DUAL_SIM
    cmd_ptr->cmd.rrc_sync_ind.as_id = rrc_get_as_id();
#endif

    rrc_send_rrc_message_to_mm(cmd_ptr);               /* Sends the command to MM */

    WRRC_MSG0_HIGH("Dispatching out MM cmd / Sent RRC_SYNC_IND to MM");

  }    
} /* rrcidt_send_sync_ind */
/*===========================================================================

FUNCTION rrcidt_check_rrc_connection_status  

DESCRIPTION
  This function check RRC connection status by lloking at RCE substate.
  There is an additional check added to see if UE is OOS & RCE sub state
  is RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL,Call type is emergency call.
  In this case also return TRUE, since we need to put emergency call through
  so IDT will take care of handling required action for emergency call at later 
  part of code.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE : When RCE sub state is final & Call type is emergency call, with RCE substate 
  is RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL, UE is OOS.
  FALSE : Otherwise

===========================================================================*/

static boolean rrcidt_check_rrc_connection_status
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  boolean status = FALSE;
  if((RRCRCE_RRC_CONN_ESTABLISHED == rrcrce_get_rrc_connection_status() ) 
      ||
     (((rrcrce_get_rce_sub_state() == RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE)||
        (rrcrce_get_rce_sub_state() == RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL))&&
      (cmd_ptr->cmd.open_sess_req.est_cause == RRC_EST_EMERGENCY_CALL)&&
      (RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status( RRC_PROCEDURE_IDT ) )
     )
    )
  {
    /*1.If RCE is in Final sub state or 
     *2.RCE is in RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL, 
     *call type is emergency call & UE is OOS(Condition 2 will 
     *take UE out of stuck up case where UE is waiting for service
     *to send out SCRI messages,emergency call should be given 
     *a heigher priority ), return status as TRUE */
    MSG_HIGH("RCE sub state =%d,Est cause = %d, CU state = %d",rrcrce_get_rce_sub_state(),
                                        cmd_ptr->cmd.open_sess_req.est_cause,
                                        rrccu_get_cell_update_complete_status( RRC_PROCEDURE_IDT ));
    status = TRUE;
  }

  return status;
}
/*===========================================================================

FUNCTION rrcidt_validate_open_session_req  

DESCRIPTION
  This function validates RRC_OPEN_SESSION_REQ command received from MM task.

DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type: It returns RRCDT_SUCCESS if it is successfully validates 
  the RRC_OPEN_SESSION_REQ command. Otherwise it returns RRCDT_FAILURE_MINOR.

===========================================================================*/

static rrcdt_status_e_type rrcidt_validate_open_session_req
(
  rrc_cmd_type *cmd_ptr,                 /* Pointer to the RRC Command */
  rrc_open_session_status_e_type *session_status
)
{
  rrcdt_status_e_type status = RRCDT_FAILURE_MINOR; 
  rrc_abort_cause_e_type  cause = RRC_MAX_ACCESS_FAILURE;
  *session_status=RRC_OPEN_SESS_FAILURE;

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
   if(csfb_extended_status == CSFB_CS_RB_RELEASED)
   {
      WRRC_MSG1_HIGH("CSFB_HACK:csfb_extended_status=%d, Fail OPEN_SESSION_REQ",csfb_extended_status);
      return (status);
   }
#endif
   
   /* Check whether RCR procedure is active or not */
   if (rrcrcr_is_proc_active() == TRUE)
   {
     WRRC_MSG0_HIGH("RCR procedure is active");
     *session_status = RRC_OPEN_SESS_FAILURE_CONN_REL_IN_PROG;
     return (status);
   }
   
   /* Check whether RRC Connection Established or not */
   if (rrcidt_check_rrc_connection_status(cmd_ptr) == FALSE)
   {
     WRRC_MSG0_HIGH("RRC Conn is not established");
     return (status);
   }

   /* Check whether NAS selected PLMN ID in IDT is same as the one provided in dedicated OTA (if any) Refer 25.331 sec 8.1.8.2.
    * CCM updates the current requested PLMN ID in CSP internal data with the primary PLMN ID if CN information received in dedicated OTA 
    * contains the IE primary PLMN ID 
    */
  if(rrccsp_is_primary_plmn_id_present())
  {
     if(!rrc_bcch_and_nas_plmns_match(rrccsp_get_curr_nas_requested_plmn(),cmd_ptr->cmd.open_sess_req.selected_plmn_id))
     {
       WRRC_MSG0_HIGH("Primary PLMN ID received in dedicated OTA is not same as NAS selected PLMN ID");
       return (status);
     }
  }

   if(rrcrce_validate_cell_access_status(cmd_ptr->cmd.open_sess_req.cn_domain_id,
                     cmd_ptr->cmd.open_sess_req.est_cause,
                     &cause) != RRCRCE_SUCCESS)
   {
     /* Access Class is bard for this domain, So return from here*/
     *session_status = RRC_OPEN_SESS_FAILURE_AC_BARD;
     WRRC_MSG2_HIGH("Access class Bard for CN doamin I.D = %d, Abort cause = %d",
                                                                          cmd_ptr->cmd.open_sess_req.cn_domain_id,
                                                                          cause);
     return (status);
   }

  /* Validate the Open Session Req primitive. Check to make sure that the
   * payload isn't too large or isn't empty.
   */
  
  if ((cmd_ptr->cmd.open_sess_req.nas_msg.length > RRCDT_MAX_PAYLOAD) ||
      (cmd_ptr->cmd.open_sess_req.nas_msg.length == 0))
  {
    WRRC_MSG1_HIGH("Invalid payload size: %d",
              cmd_ptr->cmd.open_sess_req.nas_msg.length);
  }
  else /* Valid Payload */
  {
    /* check to see if the core n/w choice is RRC_GSM_MAP*/
    if(cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.
                corenetwork_choice != RRC_GSM_MAP)
    {
      /*lint -e793*/
      WRRC_MSG1_ERROR("Invalid CN choice %d",
           cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.
           corenetwork_choice);
      /*lint +e793*/
      return(status);
    }

    /*check and see if we get a valid routing basis */
    if(cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.
                routing_info.routing_basis >=  RRC_MAX_ROUTING_BASIS)
    {
      /*lint -e793*/
      WRRC_MSG1_ERROR("Invalid Routing Basis choice %d",
           cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.
           routing_info.routing_basis);
      /*lint +e793*/
      return(status);
    }    
    
    switch (cmd_ptr->cmd.open_sess_req.cn_domain_id)
    {
      case RRC_CS_DOMAIN_CN_ID :
        if (rrcdt_int_data.cs_domain_open)
        {
          /* A signalling session to the CS domain is already open. Print a
           * message and send a failure back to MM.
           */
          WRRC_MSG0_ERROR("CS domain already open");
          return (status);
        }
        else
        {
          /* Identify the domain to which we will attempt to establish a
           * signalling session with
           */
          status = RRCDT_SUCCESS;
        }
        break;

      case RRC_PS_DOMAIN_CN_ID :
        if (rrcdt_int_data.ps_domain_open)
        {
          /* A signalling session to the PS domain is already open. Print a
           * message and send a failure back to MM.
           */
          WRRC_MSG0_ERROR("PS domain already open");
          return (status);
        }
        else
        {
        /* Identify the domain to which we will attempt to establish a
         * signalling session with
         */
          status = RRCDT_SUCCESS;
        }
        break;

      default :
        WRRC_MSG1_ERROR("Invalid CN Domain ID %d",
                  cmd_ptr->cmd.open_sess_req.cn_domain_id);
        break;
    }
  }
  *session_status=RRC_OPEN_SESS_SUCCESS;
  return (status);
} /* rrcidt_validate_open_session_req */
/*===========================================================================

FUNCTION rrcidt_save_open_session_req

DESCRIPTION
  This function saves the RRC_OPEN_SESSION_REQ message contents 
  in the static variable saved_open_session_req
  
DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type.

===========================================================================*/
static rrcdt_status_e_type rrcidt_save_open_session_req
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{       
  
  unsigned char *payload;               /* Pointer to the MM payload */
  rrcdt_status_e_type save_status = RRCDT_FAILURE_MINOR;
                                        /* dt status to be returned */
  
   /* Set the CN Domain Id value */
  switch (cmd_ptr->cmd.open_sess_req.cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID :
      if((rrcidt_cs_saved_open_session_req.open_sess_req_status == FALSE) &&
         (rrcidt_cs_saved_open_session_req.mui_status == FALSE))
      {
        rrcidt_cs_saved_open_session_req.open_session_req.cn_domain_id = RRC_CS_DOMAIN_CN_ID;
        /* Save the Intra Domain NAS Node selector */
        rrcidt_cs_saved_open_session_req.open_session_req.intra_domain_nas_node_selector.corenetwork_choice = 
          cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.corenetwork_choice;
    
        rrcidt_cs_saved_open_session_req.open_session_req.intra_domain_nas_node_selector.
          routing_info.routing_basis=
          cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.
          routing_info.routing_basis;
  
        rrcidt_cs_saved_open_session_req.open_session_req.intra_domain_nas_node_selector.
          routing_info.routing_parameter[0]=
          cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.
          routing_info.routing_parameter[0];
  
        rrcidt_cs_saved_open_session_req.open_session_req.intra_domain_nas_node_selector.
          routing_info.routing_parameter[1]=
          cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.
          routing_info.routing_parameter[1];
  
        rrcidt_cs_saved_open_session_req.open_session_req.intra_domain_nas_node_selector.
          routing_info.entered_parameter=
          cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.
          routing_info.entered_parameter;
  
        /* Allocate a buffer for the payload. This memory is released by 
         * rrcsend_send_ul_sdu().
         */
        payload = rrc_malloc( cmd_ptr->cmd.open_sess_req.nas_msg.length);
    
        /* Copy the payload from the MM primitive into the newly allocated memory
         */
        WCDMA_MEMCPY(payload, 
                     cmd_ptr->cmd.open_sess_req.nas_msg.length,
                     cmd_ptr->cmd.open_sess_req.nas_msg.value,
                     cmd_ptr->cmd.open_sess_req.nas_msg.length);
    
        /* Set the pointer to the payload and the length of the payload in the 
         * Initial Direct Transfer Message
        */
        rrcidt_cs_saved_open_session_req.open_session_req.nas_msg.value = payload;
        rrcidt_cs_saved_open_session_req.open_session_req.nas_msg.length = 
          (unsigned short) cmd_ptr->cmd.open_sess_req.nas_msg.length;
        /* Set status flag to TRUE */
        rrcidt_cs_saved_open_session_req.open_sess_req_status = TRUE;
        rrcidt_cs_saved_open_session_req.est_cause = cmd_ptr->cmd.open_sess_req.est_cause;

        /* Set fresh_start_value_needed flag to TRUE, So that 1st IDT carries START 
         *calculate according to 8.5.9   */
        rrcidt_cs_saved_open_session_req.fresh_start_value_needed = TRUE;
  
       rrcidt_cs_saved_open_session_req.open_session_req.call_type = cmd_ptr->cmd.open_sess_req.call_type;
  
        /*Save selected PLMN I.D*/
        rrcidt_cs_saved_open_session_req.selected_plmn_id = cmd_ptr->cmd.open_sess_req.selected_plmn_id;
        save_status = RRCDT_SUCCESS;
      }
      break;
    
    case RRC_PS_DOMAIN_CN_ID :
      if((rrcidt_ps_saved_open_session_req.open_sess_req_status == FALSE)&&
         (rrcidt_ps_saved_open_session_req.mui_status == FALSE))
      {
        rrcidt_ps_saved_open_session_req.open_session_req.cn_domain_id = RRC_PS_DOMAIN_CN_ID;
        /* Save the Intra Domain NAS Node selector */
        rrcidt_ps_saved_open_session_req.open_session_req.intra_domain_nas_node_selector.corenetwork_choice = 
          cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.corenetwork_choice;
    
        rrcidt_ps_saved_open_session_req.open_session_req.intra_domain_nas_node_selector.
          routing_info.routing_basis=
          cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.
          routing_info.routing_basis;
  
        rrcidt_ps_saved_open_session_req.open_session_req.intra_domain_nas_node_selector.
          routing_info.routing_parameter[0]=
          cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.
          routing_info.routing_parameter[0];
  
        rrcidt_ps_saved_open_session_req.open_session_req.intra_domain_nas_node_selector.
          routing_info.routing_parameter[1]=
          cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.
          routing_info.routing_parameter[1];
  
        rrcidt_ps_saved_open_session_req.open_session_req.intra_domain_nas_node_selector.
          routing_info.entered_parameter=
          cmd_ptr->cmd.open_sess_req.intra_domain_nas_node_selector.
          routing_info.entered_parameter;
  
        /* Allocate a buffer for the payload. This memory is released by 
         * rrcsend_send_ul_sdu().
         */
        payload = rrc_malloc( cmd_ptr->cmd.open_sess_req.nas_msg.length);
    
        /* Copy the payload from the MM primitive into the newly allocated memory
         */
        WCDMA_MEMCPY(payload, 
                     cmd_ptr->cmd.open_sess_req.nas_msg.length,
                     cmd_ptr->cmd.open_sess_req.nas_msg.value,
                     cmd_ptr->cmd.open_sess_req.nas_msg.length);
    
        /* Set the pointer to the payload and the length of the payload in the 
         * Initial Direct Transfer Message
         */
        rrcidt_ps_saved_open_session_req.open_session_req.nas_msg.value = payload;
        rrcidt_ps_saved_open_session_req.open_session_req.nas_msg.length = 
          (unsigned short) cmd_ptr->cmd.open_sess_req.nas_msg.length;

        /* Set status flag to TRUE */
        rrcidt_ps_saved_open_session_req.open_sess_req_status = TRUE;
        rrcidt_ps_saved_open_session_req.est_cause = cmd_ptr->cmd.open_sess_req.est_cause;

        /* Set fresh_start_value_needed flag to TRUE, So that 1st IDT carries START 
         *calculate according to 8.5.9   */
        rrcidt_ps_saved_open_session_req.fresh_start_value_needed = TRUE;
  
        /*Save selected PLMN I.D*/
        rrcidt_ps_saved_open_session_req.selected_plmn_id = cmd_ptr->cmd.open_sess_req.selected_plmn_id;
        save_status = RRCDT_SUCCESS;
      }
      break;
    
    default :
      ERR_FATAL("Invalid Domain ID %d", 
        cmd_ptr->cmd.open_sess_req.cn_domain_id, 0, 0);
      break;
  }
  
  /* If Open Session Req is successfully saved,check and suspend bplmn */
  if(RRCDT_SUCCESS == save_status)
  {
    rrcdt_check_and_suspend_bplmn(cmd_ptr->cmd.open_sess_req.est_cause);
  }
 
  return(save_status);
} /* rrcidt_save_open_session_req */

/*===========================================================================

FUNCTION rrcidt_send_initial_direct_transfer_msg
DESCRIPTION
  This function sends the Initial Direct Transfer message on  Uplink DCCH for PS Domain
  
DEPENDENCIES
  None.

RETURN VALUE
    void

===========================================================================*/
static void rrcidt_send_initial_direct_transfer_msg
(
 rrc_cn_domain_identity_e_type domain_id,
 rrcidt_open_sess_req_type *open_sess
)
{       
  rrc_UL_DCCH_Message     *msg_ptr;         /* Pointer to uplink DCCH message */ 
  rlc_lc_id_type          ul_lc_id;         /* Uplink logical channel ID */
  rrcsend_status_e_type send_status;        /* send status for UL message */
  uint8 idt_mui;                                  /* Local variable for MUI */
  
  /* local variable to store address of rrc_seq90.ossBitString.value*/
  unsigned char * local_routingParameterValue_ptr = NULL;

  if(open_sess->open_sess_req_status == FALSE)
  {
    WRRC_MSG0_ERROR("No IDT Message Saved for CS");
    return;
  }

  /* Determine the logical channel id for the Uplink DCCH
   */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
    DCCH_DT_HIGH_PRI_RADIO_BEARER_ID, UE_MODE_ACKNOWLEDGED_DATA);
  
  /* Ensure that LCM found a valid logical channel id
   */
  if (ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    msg_ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
      sizeof(struct rrc_UL_DCCH_Message));
    
   /* Identify this message as an Initial Direct Transfer Message
    */
    RRCSET_MSG_TYPE(msg_ptr->message,
            T_rrc_UL_DCCH_MessageType_initialDirectTransfer);
    
   /* Set bit_mask to zero since we aren't doing Measured Results On RACH.
    * There is a need  to look at SIB 11 and decide.
    */

    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer); 
    
    if(rrc_get_state() ==  RRC_STATE_CELL_FACH)
    {
      /* Function call to Measurement Control & Report procedure. */
      if(RRCMEAS_RACH_SUCCESSFUL == rrcmeas_append_meas_rpt_on_rach(
         &msg_ptr->message.u.initialDirectTransfer.measuredResultsOnRACH, RRC_PROCEDURE_IDT, FALSE))
      {
      /* if Measurement Control & Reportprocedure returns RRC_MEAS_RACH_SUCCESFUL,
         then enable the  bit_mask field */
         RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer,
             rrc_InitialDirectTransfer_,
             measuredResultsOnRACH);
       
      }
      else
      {
        MSG_MED("RACH meas is not appended",0,0,0);
      }
    }
    
    /* Set the CN Domain Id value */
    if(domain_id == RRC_PS_DOMAIN_CN_ID)
    {
      msg_ptr->message.u.initialDirectTransfer.cn_DomainIdentity = 
            rrc_CN_DomainIdentity_ps_domain;
    }
    else
    {
      msg_ptr->message.u.initialDirectTransfer.cn_DomainIdentity = 
          rrc_CN_DomainIdentity_cs_domain;
    }
      
    /* Set the Intra Domain NAS Node selector */
    RRCSET_MSG_TYPE(msg_ptr->message.u.initialDirectTransfer.intraDomainNasNodeSelector.version, 
          T_rrc_IntraDomainNasNodeSelector_version_release99);


    /* Set the core network choice */
    switch(open_sess->open_session_req.intra_domain_nas_node_selector.corenetwork_choice)
    {
      case RRC_GSM_MAP:
        /*Allocate memory for release99 */ 
        msg_ptr->message.u.initialDirectTransfer.intraDomainNasNodeSelector.version.u.release99 = 
         rtxMemAllocTypeZ(&enc_ctxt,rrc_IntraDomainNasNodeSelector_release99);
      
        RRCSET_MSG_TYPE(msg_ptr->message.u.initialDirectTransfer.
           intraDomainNasNodeSelector.version.u.release99->cn_Type,
           T_rrc_IntraDomainNasNodeSelector_cn_Type_gsm_Map_IDNNS);
        break;

      default:
        ERR_FATAL("Other than GSM MAP CN are not supported",0,0,0);
        break;

    }

    /*right now only RRC_GSM_MAP is supported add a condition for
      ansi later
     */
    msg_ptr->message.u.initialDirectTransfer.intraDomainNasNodeSelector.version.u.release99->cn_Type.u.gsm_Map_IDNNS = 
      rtxMemAllocTypeZ(&enc_ctxt,rrc_Gsm_map_IDNNS);

    if(open_sess->open_session_req.intra_domain_nas_node_selector.corenetwork_choice == RRC_GSM_MAP)
    {

      switch( open_sess->open_session_req.intra_domain_nas_node_selector.routing_info.routing_basis)
      {
        case RRC_LOCAL_TMSI_OR_PTMSI:
          msg_ptr->message.u.initialDirectTransfer.intraDomainNasNodeSelector.version.u.release99->
          cn_Type.u.gsm_Map_IDNNS->routingbasis.u.localPTMSI = 
            rtxMemAllocTypeZ(&enc_ctxt,rrc_Gsm_map_IDNNS_localPTMSI);

          RRCSET_MSG_TYPE(msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis,
          T_rrc_Gsm_map_IDNNS_routingbasis_localPTMSI);

          msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis.u.localPTMSI->routingparameter.
          numbits=INTRA_DOMAIN_NAS_NODE_SELECTOR_LENGTH;

          local_routingParameterValue_ptr = msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis.u.localPTMSI->routingparameter.
          data;

          break;

        case RRC_TMSI_OR_PTMSI_SAME_PLMN_AND_DIFFERENT_RA_OR_LA:
          msg_ptr->message.u.initialDirectTransfer.intraDomainNasNodeSelector.version.u.release99->
          cn_Type.u.gsm_Map_IDNNS->routingbasis.u.tMSIofsamePLMN = 
           rtxMemAllocTypeZ(&enc_ctxt,rrc_Gsm_map_IDNNS_tMSIofsamePLMN); 

          RRCSET_MSG_TYPE(msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis,
          T_rrc_Gsm_map_IDNNS_routingbasis_tMSIofsamePLMN);

          msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis.u.tMSIofsamePLMN->routingparameter.
          numbits=INTRA_DOMAIN_NAS_NODE_SELECTOR_LENGTH;

          local_routingParameterValue_ptr = msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis.u.tMSIofsamePLMN->routingparameter.
          data;

          break;

        case RRC_TMSI_OR_PTMSI_OF_DIFFERENT_PLMN:
          msg_ptr->message.u.initialDirectTransfer.intraDomainNasNodeSelector.version.u.release99->
          cn_Type.u.gsm_Map_IDNNS->routingbasis.u.tMSIofdifferentPLMN = 
            rtxMemAllocTypeZ(&enc_ctxt,rrc_Gsm_map_IDNNS_tMSIofdifferentPLMN); 

          RRCSET_MSG_TYPE(msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis,
          T_rrc_Gsm_map_IDNNS_routingbasis_tMSIofdifferentPLMN);

          msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis.u.tMSIofdifferentPLMN->routingparameter
          .numbits = INTRA_DOMAIN_NAS_NODE_SELECTOR_LENGTH;

          local_routingParameterValue_ptr = msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis.u.tMSIofdifferentPLMN->
          routingparameter.data;

          break;

        case RRC_IMSI_RESP_TO_IMSI_PAGING:
          msg_ptr->message.u.initialDirectTransfer.intraDomainNasNodeSelector.version.u.release99->
          cn_Type.u.gsm_Map_IDNNS->routingbasis.u.iMSIresponsetopaging = 
           rtxMemAllocTypeZ(&enc_ctxt,rrc_Gsm_map_IDNNS_iMSIresponsetopaging);

          RRCSET_MSG_TYPE(msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis,
          T_rrc_Gsm_map_IDNNS_routingbasis_iMSIresponsetopaging);

          msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis.u.iMSIresponsetopaging->
          routingparameter.numbits=INTRA_DOMAIN_NAS_NODE_SELECTOR_LENGTH;

          local_routingParameterValue_ptr = msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis.u.iMSIresponsetopaging->
          routingparameter.data;
          break;

        case RRC_IMSI_UE_INITIATED_EVENT:
          msg_ptr->message.u.initialDirectTransfer.intraDomainNasNodeSelector.version.u.release99->
          cn_Type.u.gsm_Map_IDNNS->routingbasis.u.iMSIcauseUEinitiatedEvent = 
           rtxMemAllocTypeZ(&enc_ctxt,rrc_Gsm_map_IDNNS_iMSIcauseUEinitiatedEvent);

          RRCSET_MSG_TYPE(msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis,
          T_rrc_Gsm_map_IDNNS_routingbasis_iMSIcauseUEinitiatedEvent);

          msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis.u.iMSIcauseUEinitiatedEvent->
          routingparameter.numbits=INTRA_DOMAIN_NAS_NODE_SELECTOR_LENGTH;

          local_routingParameterValue_ptr = msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis.u.iMSIcauseUEinitiatedEvent->
          routingparameter.data;
          break;

        case RRC_IMEI:
          msg_ptr->message.u.initialDirectTransfer.intraDomainNasNodeSelector.version.u.release99->
          cn_Type.u.gsm_Map_IDNNS->routingbasis.u.iMEI = 
           rtxMemAllocTypeZ(&enc_ctxt,rrc_Gsm_map_IDNNS_iMEI);
          
          RRCSET_MSG_TYPE(msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis,
          T_rrc_Gsm_map_IDNNS_routingbasis_iMEI);

          msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis.u.iMEI->routingparameter.numbits=
          INTRA_DOMAIN_NAS_NODE_SELECTOR_LENGTH;

          local_routingParameterValue_ptr = msg_ptr->message.u.initialDirectTransfer.
          intraDomainNasNodeSelector.version.u.release99->cn_Type.
          u.gsm_Map_IDNNS->routingbasis.u.iMEI->routingparameter.data;
          break;

        default:
          WRRC_MSG1_ERROR("Unknown routing Basis received %d",
               open_sess->open_session_req.intra_domain_nas_node_selector.
               routing_info.routing_basis);
          break;
      }

      if(local_routingParameterValue_ptr != NULL)
      {
        *(local_routingParameterValue_ptr) =
          (unsigned char ) (open_sess->open_session_req.
          intra_domain_nas_node_selector.routing_info.routing_parameter[0]);

        *(local_routingParameterValue_ptr + 1) = 
          (unsigned char ) (open_sess->open_session_req.
          intra_domain_nas_node_selector.routing_info.routing_parameter[1]);
      }
     
    }

    msg_ptr->message.u.initialDirectTransfer.
      intraDomainNasNodeSelector.version.u.release99->cn_Type.
      u.gsm_Map_IDNNS->dummy = 
      open_sess->open_session_req.intra_domain_nas_node_selector.
      routing_info.entered_parameter;

    /* Add 3A0 non-critical extensions */
    RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer,
      rrc_InitialDirectTransfer_,
      v3a0NonCriticalExtensions);

    /* Set no non-critical extensions for V3AO non-critical extensions */
    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions);

    /* Append START value to the message */
    RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
      initialDirectTransfer_v3a0ext, 
      rrc_InitialDirectTransfer_v3a0ext_,
      start_Value);

    msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
      initialDirectTransfer_v3a0ext.start_Value.numbits = RRC_START_VALUE_LENGTH;

 
    WRRC_MSG1_HIGH("IDT fresh_start_value_needed flag status = %d",
                                           open_sess->fresh_start_value_needed);
    if(open_sess->fresh_start_value_needed == TRUE)
    {
      /*This is the first IDT, So calculate START value according to 8.5.9*/
      rrcsmc_check_new_keys_and_reset_append_start_per_domain((byte *) (msg_ptr->message.u.initialDirectTransfer.
                                     v3a0NonCriticalExtensions.
                                     initialDirectTransfer_v3a0ext.start_Value.data),
                                     domain_id,
                                     RRCSMC_EXTRACT_AND_APPEND_START);
      /*Set fresh_start_value_needed flag to FALSE, as subsequent IDT should append START value*/
      open_sess->fresh_start_value_needed = FALSE;
    }
    else
    {
      /*This should be a subsequent IDT so append START value*/
      rrcsmc_check_new_keys_and_reset_append_start_per_domain((byte *) (msg_ptr->message.u.initialDirectTransfer.
                                     v3a0NonCriticalExtensions.
                                     initialDirectTransfer_v3a0ext.start_Value.data),
                                     domain_id,
                                     RRCSMC_APPEND_START);
    }
    
    /* Copy the contents of Saved RRC_OPEN_SESS_REQ 
    */    
    WCDMA_MEMCPY(msg_ptr->message.u.initialDirectTransfer.nas_Message.data,
                 sizeof(OSOCTET) * 4095,
                 open_sess->open_session_req.nas_msg.value, 
                 open_sess->open_session_req.nas_msg.length);

    msg_ptr->message.u.initialDirectTransfer.nas_Message.numocts = 
      open_sess->open_session_req.nas_msg.length;
    
    if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)) 
    {
      rrcidt_fill_est_cause(msg_ptr, domain_id);    
    }

   /* Send the PS Initial Direct Transfer Message on the Uplink DCCH
    */
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
    #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/

    if(rrccsp_is_camped_cell_network_shared() == TRUE)
    {
      WRRC_MSG0_HIGH("NWS:N/W shared cell so include PLMN I.D in IDT");
      if(!(RRC_MSG_COMMON_BITMASK_IE_PTR((&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions),
         rrc_InitialDirectTransfer_v3a0NonCriticalExtensions_,
         laterNonCriticalExtensions)))
      {
        /* Enable the laterNonCriticalExtension field in v3a0NoncriticalExtension structure*/
        RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions,
          rrc_InitialDirectTransfer_v3a0NonCriticalExtensions_,
          laterNonCriticalExtensions);
    
        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
          laterNonCriticalExtensions);
      }
      if(!(RRC_MSG_COMMON_BITMASK_IE_PTR((&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                  laterNonCriticalExtensions),
              rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_,
              v590NonCriticalExtensions)))
      {
        /* Enable the v590NonCriticalExtension field in laterNonCriticl in v3a0Non critical */
        RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                  laterNonCriticalExtensions,
                  rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_,
                  v590NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
          laterNonCriticalExtensions.v590NonCriticalExtensions);
      }
      if(!(RRC_MSG_COMMON_BITMASK_IE_PTR((&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                  laterNonCriticalExtensions.v590NonCriticalExtensions.initialDirectTransfer_v590ext),
                  rrc_,
                  establishmentCause)))
      {
        /* Set bit mask properly if establishment I.E is not added in initialDirectTransfer_v590ext*/
        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
          laterNonCriticalExtensions.v590NonCriticalExtensions.
          initialDirectTransfer_v590ext);
      }
      if(!(RRC_MSG_COMMON_BITMASK_IE_PTR((&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
             laterNonCriticalExtensions.v590NonCriticalExtensions),
             rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_v590NonCriticalExtensions_,
             v690NonCriticalExtensions)))
      {
        /* Enable the v690NonCriticalExtensions field in v590NonCriticalExtension in laterNonCriticl in v3a0Non critical */
        RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
         laterNonCriticalExtensions.v590NonCriticalExtensions,
         rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_v590NonCriticalExtensions_,
         v690NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
          laterNonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
            laterNonCriticalExtensions.v590NonCriticalExtensions.
            v690NonCriticalExtensions.initialDirectTransfer_v690ext);
      }

      /* Enable the rrc_InitialDirectTransfer_v690ext_IEs_plmn_Identity_present field in initialDirectTransfer_v690ext */
      RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
          laterNonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.initialDirectTransfer_v690ext,
          rrc_InitialDirectTransfer_v690ext_IEs_,
          plmn_Identity);
      
      /* Fill PLMN I.D into OTA message which we received from NAS here */
      rrcdt_fill_plmn_id_in_idt(&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
        laterNonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.initialDirectTransfer_v690ext.plmn_Identity,
          domain_id);
    }
#ifdef FEATURE_FEMTO_CSG
    if(is_whitelist_valid == TRUE)
    {
      if((!(RRC_MSG_COMMON_BITMASK_IE_PTR((&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions),
        rrc_InitialDirectTransfer_v3a0NonCriticalExtensions_,
          laterNonCriticalExtensions))))
      {
        /* Enable the laterNonCriticalExtension field in v3a0NoncriticalExtension structure*/
        RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions,
          rrc_InitialDirectTransfer_v3a0NonCriticalExtensions_,
          laterNonCriticalExtensions);
    
        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
          laterNonCriticalExtensions);
      }
      if(!(RRC_MSG_COMMON_BITMASK_IE_PTR((&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                  laterNonCriticalExtensions),
              rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_,
              v590NonCriticalExtensions)))
      {
        /* Enable the v590NonCriticalExtension field in laterNonCriticl in v3a0Non critical */
        RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
                  laterNonCriticalExtensions,
                  rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_,
                  v590NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
          laterNonCriticalExtensions.v590NonCriticalExtensions);
      }
      if(!(RRC_MSG_COMMON_BITMASK_IE_PTR((&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
             laterNonCriticalExtensions.v590NonCriticalExtensions),
             rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_v590NonCriticalExtensions_,
             v690NonCriticalExtensions)))
      {
        /* Enable the v690NonCriticalExtensions field in v590NonCriticalExtension in laterNonCriticl in v3a0Non critical */
        RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
         laterNonCriticalExtensions.v590NonCriticalExtensions,
         rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_v590NonCriticalExtensions_,
         v690NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
          laterNonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
            laterNonCriticalExtensions.v590NonCriticalExtensions.
            v690NonCriticalExtensions.initialDirectTransfer_v690ext);
      }

      if(!(RRC_MSG_COMMON_BITMASK_IE_PTR((&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
             laterNonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions),
             rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_v590NonCriticalExtensions_
             v690NonCriticalExtensions_,v770NonCriticalExtensions)))
      {
        /* Enable the v770NonCriticalExtensions field */
        RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
         laterNonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions,
         rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_v590NonCriticalExtensions_
         v690NonCriticalExtensions_,v770NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
          laterNonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
          v770NonCriticalExtensions);

      }
      if(!(RRC_MSG_COMMON_BITMASK_IE_PTR((&msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
             laterNonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions),
             rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_v590NonCriticalExtensions_
             v690NonCriticalExtensions_v770NonCriticalExtensions_,v860NonCriticalExtensions)))
      {
        /* Enable the v860NonCriticalExtensions field */
        RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
         laterNonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
         v770NonCriticalExtensions,rrc_v3a0NonCriticalExtensions_laterNonCriticalExtensions_
         v590NonCriticalExtensions_v690NonCriticalExtensions_v770NonCriticalExtensions_,v860NonCriticalExtensions);

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.
          laterNonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
          v770NonCriticalExtensions.v860NonCriticalExtensions);

      }
      msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      v590NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.
      initialDirectTransfer_v860ext.m.supportOfCSGPresent = 1;
      msg_ptr->message.u.initialDirectTransfer.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
      v590NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.
      initialDirectTransfer_v860ext.supportOfCSG = rrc_InitialDirectTransfer_v860ext_IEs_supportOfCSG_true;
    }
#endif
#ifdef FEATURE_QSH_EVENT_METRIC    
    rrc_qsh_log_idt_params(rrcidt_cs_saved_open_session_req.est_cause,domain_id);
#endif
    send_status = rrcsend_send_ul_sdu_with_mui(RRC_PROCEDURE_IDT,msg_ptr,ul_lc_id,
                                      RRCSEND_L2ACK_REQUIRED, &idt_mui);
    
    
    if (send_status == RRCSEND_SUCCESS)
    {
     /* Set the MUi and MUI status */
     open_sess->mui_status = TRUE;
     open_sess->open_sess_req_status = FALSE;
     open_sess->open_sess_cnf_required = TRUE;

     open_sess->mui = idt_mui;
      
      WRRC_MSG0_HIGH("Initial Direct Transfer Message Sent");

#ifdef FEATURE_WCDMA_MTPL_THERMAL_BACKOFF 
      if((domain_id == RRC_CS_DOMAIN_CN_ID )&&
         (RRC_EST_EMERGENCY_CALL == open_sess->est_cause))
      {
        rrcrce_set_rrc_est_cause_for_emergency_call();
      }
#endif

      /* Wait for L2 ACK if there is no pending message for other Domain*/
      if(((domain_id == RRC_PS_DOMAIN_CN_ID ) && (rrcidt_cs_saved_open_session_req.open_sess_req_status == FALSE))
          || ((domain_id == RRC_CS_DOMAIN_CN_ID ) && (rrcidt_ps_saved_open_session_req.open_sess_req_status == FALSE))
        )
      {
        rrcidt_substate = RRCIDT_WAIT_FOR_L2ACK; 
      } 
    }
    else
    {
      WRRC_MSG1_ERROR("Error %d sending Initial Direct Xfer Msg",
               send_status);
      /* reset the state machine */
      rrcidt_substate = RRCIDT_INITIAL;

      /* Delete the saved CS RRC_OPEN_SESSION_REQ */
      rrcidt_delete_open_session_req(domain_id);

    }
 
    /* The signalling session has been successfully opened. Set a flag to 
    * indicate that domain has been opened.
    */
     if(domain_id == RRC_CS_DOMAIN_CN_ID)
    {
        rrcdt_int_data.cs_domain_open = TRUE;
    }
    else
    {
       rrcdt_int_data.ps_domain_open = TRUE;

 
      WRRC_MSG0_HIGH("Sig Conn for PS Domain is established");
#ifdef FEATURE_INACTIVITY_HANDLING

    if (rrc_get_state() == RRC_STATE_CELL_DCH) 
    {
        WRRC_MSG1_HIGH("Restarting inact timer %d",INACTIVITY_TIMER_IN_MS);
      rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, INACTIVITY_TIMER_IN_MS);
    }
    else if (rrc_get_state() == RRC_STATE_CELL_FACH)
    {
        WRRC_MSG2_HIGH("Restarting Inactivity timer %d + remain T305 %d",INACTIVITY_TIMER_IN_MS,
               rrctmr_get_remaining_time(RRCTMR_T_305_TIMER));
      /* Restart incativity handling timer */
      if(rrc_convert_t305_to_ms(RRC_GET_T305()) != RRCTMR_INFINITY_VAL )
      {
        rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                          (unsigned long)(INACTIVITY_TIMER_IN_MS + rrctmr_get_remaining_time(RRCTMR_T_305_TIMER))); 
      }
      else
      {
        /*If T305 timer was infinity then the timer wouldnt have been started . In thats case start
            timer of 5 mins plus 24 hrs*/
        rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                          (unsigned long)(INACTIVITY_TIMER_IN_MS + RRCTMR_INFINITY_VAL)); 
      }
    }
    (void)rrcmisc_handle_ps_rab_inactivity(1);

#endif /* FEATURE_INACTIVITY_HANDLING */
   }
             
  }
  else
  {
    WRRC_MSG0_HIGH("RB 3 not found by LCM");

    /* Send RRC_OPEN_SESS_CNF Primitive to MM with Failure status */
    rrcidt_send_open_session_cnf(RRC_OPEN_SESS_FAILURE,
      domain_id);

    rrcidt_delete_open_session_req(domain_id);
  }
} /* rrcidt_send_initial_direct_transfer_msg */
/*===========================================================================

FUNCTION rrcdt_send_cell_update_initiate_req

DESCRIPTION
  This function send the cell update initiate request if we are in
  URA_PCH or CELL_PCH states.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrcdt_send_cell_update_initiate_req
(
  rrc_proc_e_type rrcdt_procedure
)
{
  rrc_cmd_type *int_cmd_ptr;
  //This initiates a CELL Update procedure and enters the substate
  //WAIT_FOR_CELL_UPDATE_COMP_CNF. 
  if( (int_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
  {     
    /* Fill in the command parameters for RRC_CELL_UPDATE_INITIATE_REQ */
    int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
    int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_UL_DATA_TRANSMISSION;
    int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = TRUE;
    int_cmd_ptr->cmd.initiate_cell_update.procedure = rrcdt_procedure;
        
    rrc_put_int_cmd(int_cmd_ptr);             
  }
}/*rrcdt_send_cell_update_initiate_req*/

/*===========================================================================

FUNCTION rrcidt_send_nas_data_ind

DESCRIPTION
  This function sends RR_NAS_DATA_IND if there is any NAS message pending
  to transmitted or pending to be acknowledged from UTRAN.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrcidt_send_nas_data_ind
(
  void
)
{
  rr_nas_data_ind_type nas_data_ind;    /* Local variable to store
                                           NAS message to be sent to RR */
  switch(rrcidt_substate)
  {
    case RRCIDT_INITIAL:
      /* No Pending NAS message. Do not send a NULL message here because it will
         be done at the end of sending UDT messages. RR expects only one NULL
         message at the end of all NAS DATA IND calls (after both IDT and UDT) */
      WRRC_MSG0_HIGH("No Pending IDT messages");
      break;

    case RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF:                               
    case RRCIDT_WAIT_FOR_L2ACK:

      /* Send CS saved IDT, if there is one */ 
      if(((rrcidt_cs_saved_open_session_req.mui_status == TRUE) ||
          (rrcidt_cs_saved_open_session_req.open_sess_req_status == TRUE)) && 
         (rrcidt_cs_saved_open_session_req.open_session_req.nas_msg.length != 0))
      {
        if(rrcidt_cs_saved_open_session_req.open_session_req.nas_msg.length > 255)
        {
          ERR_FATAL("Len of NAS Msg is > 255",0,0,0);
        }

        nas_data_ind.nas_data.message_length = (uint8)
          rrcidt_cs_saved_open_session_req.open_session_req.nas_msg.length;

        WCDMA_MEMCPY(nas_data_ind.nas_data.payload, 
                     sizeof(uint8) * RRC_RR_MAX_GSM_MESSAGE_LENGTH,
                     rrcidt_cs_saved_open_session_req.open_session_req.nas_msg.value,
                     rrcidt_cs_saved_open_session_req.open_session_req.nas_msg.length);

        /* Send the message to RR */
        rr_send_rrc_message_to_rr_internal((void *)(&nas_data_ind),
                                   RR_NAS_DATA_IND,
                                   sizeof(rr_nas_data_ind_type));
      
        WRRC_MSG1_HIGH("Sent RR_NAS_DATA_IND message length: %d",
                  nas_data_ind.nas_data.message_length);
      }

      /* Send PS saved IDT, if there is one */ 
      if(((rrcidt_ps_saved_open_session_req.mui_status == TRUE) ||
          (rrcidt_ps_saved_open_session_req.open_sess_req_status == TRUE)) && 
         (rrcidt_ps_saved_open_session_req.open_session_req.nas_msg.length != 0))
      {
        if(rrcidt_ps_saved_open_session_req.open_session_req.nas_msg.length > 255)
        {
          ERR_FATAL("Len of NAS Msg is > 255",0,0,0);
        }

        nas_data_ind.nas_data.message_length = (uint8)
          rrcidt_ps_saved_open_session_req.open_session_req.nas_msg.length;

        WCDMA_MEMCPY(nas_data_ind.nas_data.payload, 
                     sizeof(uint8) * RRC_RR_MAX_GSM_MESSAGE_LENGTH,
                     rrcidt_ps_saved_open_session_req.open_session_req.nas_msg.value,
                     rrcidt_ps_saved_open_session_req.open_session_req.nas_msg.length);

        /* Send the message to RR */
        rr_send_rrc_message_to_rr_internal((void *)(&nas_data_ind),
                                   RR_NAS_DATA_IND,
                                   sizeof(rr_nas_data_ind_type));
      
        WRRC_MSG1_HIGH("Sent RR_NAS_DATA_IND message length: %d",
                  nas_data_ind.nas_data.message_length);
      }

      /* Delete saved IDTs of both domains since we have sent it to RR */
      rrcidt_delete_open_session_req(RRC_CS_DOMAIN_CN_ID); 
      rrcidt_delete_open_session_req(RRC_PS_DOMAIN_CN_ID); 

      /* Do not send a NULL message here because it will be done at the end of sending 
         UDT messages. RR expects only one NULL message at the end of all NAS DATA IND 
         calls (after both IDT and UDT) */
      break;

    default:  
      ERR_FATAL("Invalid RRCIDT substate: %d ",rrcidt_substate,0,0);
      break;
   }

}/*rrcidt_send_nas_data_ind*/
/*===========================================================================

FUNCTION rrcidt_process_idt_initial_substate

DESCRIPTION
  This function processes RRC_OPEN_SESSION_REQ event that is  dispatched
  to Initial Direct Transfer procedure in RRCIDT_INITIAL substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcidt_process_idt_initial_substate
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrc_open_session_status_e_type session_status = RRC_OPEN_SESS_SUCCESS;
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    /* this happens when a state change ind is received in idle state.
     * do nothing when it happens
     */
    case RRC_OPEN_SESSION_REQ:
      WRRC_MSG0_HIGH("Received RRC_OPEN_SESS_REQ");
      if((RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status( RRC_PROCEDURE_IDT ) )&&
        (rrcidt_check_rrc_connection_status(cmd_ptr) == TRUE))
      {
        if(is_ue_oos_and_em_call_attempted(cmd_ptr, RRC_PROCEDURE_IDT))
        {
          rrcidt_substate = RRCIDT_INITIAL;  
          return;
        }
      }
      /* Before opening a new signalling session, be sure that there isn't one 
       * alreay open to the requested domain.  Also validate contents of the
       * open session request
       */
      if(RRCDT_SUCCESS != rrcidt_validate_open_session_req(cmd_ptr,&session_status))
      {
        /* Send open session failure with appropriate failure status */
        rrcidt_send_open_session_cnf(session_status,
                                    cmd_ptr->cmd.open_sess_req.cn_domain_id);
        return;
      }
    
      /* save the contents of the open_session_req message. will be used
       * later to send out the idt message
       */
      if(RRCDT_SUCCESS != rrcidt_save_open_session_req(cmd_ptr))
      {
        /* Send RRC_OPEN_SESS_CNF Primitive to MM with Failure status */
        rrcidt_send_open_session_cnf(RRC_OPEN_SESS_FAILURE,
                                    cmd_ptr->cmd.open_sess_req.cn_domain_id);
        return;
      }
    
      switch(rrc_get_state())
      {
        case RRC_STATE_URA_PCH:
        case RRC_STATE_CELL_PCH: 
          if(RRCCU_COMPLETED == rrccu_get_cell_update_complete_status(RRC_PROCEDURE_IDT))
          {
            rrcidt_set_est_cause(cmd_ptr->cmd.open_sess_req.est_cause, 
                      RRC_PROCEDURE_CU, cmd_ptr->cmd.open_sess_req.cn_domain_id);
            switch (cmd_ptr->cmd.open_sess_req.cn_domain_id)
            {
              case RRC_CS_DOMAIN_CN_ID :
                rrcidt_cs_saved_open_session_req.cu_init_by_idt = TRUE;
              break;

                case RRC_PS_DOMAIN_CN_ID :
                rrcidt_ps_saved_open_session_req.cu_init_by_idt = TRUE;
              break;

              default:
                WRRC_MSG0_HIGH("Invalid CN Domain");
              break;
            }
            /* if there is not cell update in progress initiate */
            /* cell update with uplink data transmission        */
            rrcdt_send_cell_update_initiate_req(RRC_PROCEDURE_IDT);
            /* Wait until Cell update procedure is completed */
            rrcidt_substate = RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
          }
          else
          {
            if(is_ue_oos_and_em_call_attempted(cmd_ptr, RRC_PROCEDURE_IDT))
            {
              rrcidt_substate = RRCIDT_INITIAL;  
            }
            else
            {
              rrcidt_set_est_cause(cmd_ptr->cmd.open_sess_req.est_cause, RRC_PROCEDURE_IDT,
                  cmd_ptr->cmd.open_sess_req.cn_domain_id);
 
             /* Wait until Cell update procedure is completed */
              rrcidt_substate = RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
            }
          }
          break;

        case RRC_STATE_CELL_FACH:
        case RRC_STATE_CELL_DCH:
          //This queries whether a CELL Update procedure is active.If Cell Update
          //is active, it registers for the complete and enters the substate
          //RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF. 
          if(RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status( RRC_PROCEDURE_IDT ) )
          {
            if(is_ue_oos_and_em_call_attempted(cmd_ptr, RRC_PROCEDURE_IDT))
            {
              rrcidt_substate = RRCIDT_INITIAL;  
            }
            else
            {
              rrcidt_set_est_cause(cmd_ptr->cmd.open_sess_req.est_cause, 
                  RRC_PROCEDURE_IDT, cmd_ptr->cmd.open_sess_req.cn_domain_id);

              /* Wait until Cell update procedure is completed */
              rrcidt_substate = RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
            }
          }
          else /* Cell Update procedure is inactive */
          {
            rrcidt_set_est_cause(cmd_ptr->cmd.open_sess_req.est_cause, 
                  RRC_PROCEDURE_IDT, cmd_ptr->cmd.open_sess_req.cn_domain_id);
            /* Build and send the Initial Direct Transfer Msg to Send Chain */
            if(rrcidt_cs_saved_open_session_req.open_sess_req_status == TRUE)
            {
              rrcidt_send_initial_direct_transfer_msg(RRC_CS_DOMAIN_CN_ID,&rrcidt_cs_saved_open_session_req);
            }
            if(rrcidt_ps_saved_open_session_req.open_sess_req_status == TRUE)
            {
              rrcidt_send_initial_direct_transfer_msg(RRC_PS_DOMAIN_CN_ID,&rrcidt_ps_saved_open_session_req);
            }
          }
          break;
    
        default:
          WRRC_MSG2_HIGH("Invalid Event: %d for IDT in substate:%d", 
                   cmd_ptr->cmd_hdr.cmd_id,rrcidt_substate);
        break;
      }
    
      break;

    case RRC_NAS_DATA_TRANSFER_IND:
      rrcidt_send_nas_data_ind();   
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d for IDT in substate:%d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcidt_substate);
      break;
  }/*end of switch stmt*/
} /* rrcidt_process_idt_initial_substate */

/*===========================================================================

FUNCTION rrcidt_process_idt_wait_for_cell_update_comp_cnf_substate

DESCRIPTION
  This function processes all events that are  dispatched
  to Initial Direct Transfer procedure in RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF
  substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcidt_process_idt_wait_for_cell_update_comp_cnf_substate
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrc_open_session_status_e_type session_status = RRC_OPEN_SESS_SUCCESS;
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_CELL_UPDATE_COMPLETE_CNF:
      rrcidt_cs_saved_open_session_req.cu_init_by_idt = FALSE;
      rrcidt_ps_saved_open_session_req.cu_init_by_idt = FALSE;

      switch(rrc_get_state())
      {
        case RRC_STATE_URA_PCH:
        case RRC_STATE_CELL_PCH:   

          if(RRCCU_COMPLETED == 
                 rrccu_get_cell_update_complete_status( RRC_PROCEDURE_IDT ) )
          {
              /* change the owner of establishment cause to CU */
            rrcidt_change_owner_est_cause(RRC_PROCEDURE_CU);
            if(rrcidt_is_idt_buffered(RRC_CS_DOMAIN_CN_ID) == TRUE)
            {
              rrcidt_cs_saved_open_session_req.cu_init_by_idt = TRUE;
            }
            if(rrcidt_is_idt_buffered(RRC_PS_DOMAIN_CN_ID) == TRUE)
            {
              rrcidt_ps_saved_open_session_req.cu_init_by_idt = TRUE;
            }
            /* initiate cell update if it not already running */
            /* with cause uplink data transmission */
            rrcdt_send_cell_update_initiate_req(RRC_PROCEDURE_IDT);
          }

          /* Wait until Cell update procedure is completed */
          rrcidt_substate = RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
          return;

        default:
                ;
          /* for other states continue */
      }
      rrcidt_check_if_transmission_success(RRC_PROCEDURE_CU);
      rrcidt_reset_est_cause(RRC_PROCEDURE_CU);

      /* Build and send the Initial Direct Transfer Msg to Send Chain */
      if(rrcidt_cs_saved_open_session_req.open_sess_req_status == TRUE)
      {
        rrcidt_send_initial_direct_transfer_msg(RRC_CS_DOMAIN_CN_ID,&rrcidt_cs_saved_open_session_req);
      }
      if(rrcidt_ps_saved_open_session_req.open_sess_req_status == TRUE)
      {
        rrcidt_send_initial_direct_transfer_msg(RRC_PS_DOMAIN_CN_ID,&rrcidt_ps_saved_open_session_req);
      }

      /* Reset the state machine */
      if((rrcidt_cs_saved_open_session_req.mui_status == FALSE)  &&
         (rrcidt_ps_saved_open_session_req.mui_status == FALSE) 
         )
      {
        rrcidt_substate = RRCIDT_INITIAL;  
      }
      break;

    case RRC_OPEN_SESSION_REQ:
      WRRC_MSG0_HIGH("Received RRC_OPEN_SESS_REQ");
      if((RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status( RRC_PROCEDURE_IDT ) )&&
        (rrcidt_check_rrc_connection_status(cmd_ptr) == TRUE))
      {
        if(is_ue_oos_and_em_call_attempted(cmd_ptr, RRC_PROCEDURE_IDT))
        {
          rrcidt_substate = RRCIDT_INITIAL;  
          return;
        }
      }
      /* Before opening a new signalling session, be sure that there isn't one 
       * alreay open to the requested domain.  Also validate contents of the
       * open session request
       */
      if(RRCDT_SUCCESS != rrcidt_validate_open_session_req(cmd_ptr,&session_status))
      {
        /* Send open session failure with appropriate failure status */
        rrcidt_send_open_session_cnf(session_status,
                                    cmd_ptr->cmd.open_sess_req.cn_domain_id);
        return;
      }
    
      /* save the contents of the open_session_req message. will be used
       * later to send out the idt message
       */
      if(RRCDT_SUCCESS != rrcidt_save_open_session_req(cmd_ptr))
      {
        /* Send RRC_OPEN_SESS_CNF Primitive to MM with Failure status */
        rrcidt_send_open_session_cnf(RRC_OPEN_SESS_FAILURE,
                                    cmd_ptr->cmd.open_sess_req.cn_domain_id);
        return;
      }

      if(is_ue_oos_and_em_call_attempted(cmd_ptr, RRC_PROCEDURE_IDT))
      {
        rrcidt_substate = RRCIDT_INITIAL;  
      }
      else  
      {
         rrcidt_set_est_cause(cmd_ptr->cmd.open_sess_req.est_cause, 
                RRC_PROCEDURE_IDT, cmd_ptr->cmd.open_sess_req.cn_domain_id);
      }

      break;

    case RRC_NAS_DATA_TRANSFER_IND:
      rrcidt_send_nas_data_ind();  
      /* Reset the state machine */   
      rrcidt_delete_open_session_req(RRC_CS_DOMAIN_CN_ID);  
      rrcidt_delete_open_session_req(RRC_PS_DOMAIN_CN_ID); 
      rrcidt_reset_est_cause(RRC_PROCEDURE_CU);       
      rrcidt_substate = RRCIDT_INITIAL; 
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d for IDT in substate:%d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcidt_substate);
      break;
  }
 
} /* rrcidt_process_idt_wait_for_cell_update_comp_cnf_substate */

/*===========================================================================

FUNCTION rrcidt_is_idt_waiting_for_l2ack_per_domain

DESCRIPTION
  This function returns TRUE/FALSE depending if L2_ACK is pending for
  CS or PS domain which is passed as a formal parameter to the fn.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  The CN domain requested is waiting for L2_ACK
  FALSE: The CN domain requested is not waiting for L2_ACK 

===========================================================================*/
boolean rrcidt_is_idt_waiting_for_l2ack_per_domain
(
  rrc_cn_domain_identity_e_type cn_domain
)
{
  /*return status to indicate whether OPEN_SESS_CNF is to be sent*/
  boolean status = FALSE;

  /*check the domain and also if mui_status is TRUE i.e. we are waiting for
    L2_ACK for the domain requested
   */
  if((cn_domain == RRC_CS_DOMAIN_CN_ID) && 
     (rrcidt_cs_saved_open_session_req.mui_status == TRUE))
  {
    rrcidt_clear_idt_data_for_race_conditions(cn_domain);
    status = TRUE;
  }
  else if((cn_domain == RRC_PS_DOMAIN_CN_ID) && 
          (rrcidt_ps_saved_open_session_req.mui_status == TRUE))
  {
    rrcidt_clear_idt_data_for_race_conditions(cn_domain);
    status = TRUE;
  }

  return status;
}
/*===========================================================================

FUNCTION rrcidt_process_idt_wait_for_l2ack_substate

DESCRIPTION
  This function processes all events that are  dispatched
  to Initial Direct Transfer procedure in RRCIDT_WAIT_FOR_L2ACK
  substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcidt_process_idt_wait_for_l2ack_substate
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrc_open_session_status_e_type session_status = RRC_OPEN_SESS_SUCCESS;
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_DOWNLINK_L2ACK_IND:

    MSG_6(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"CS domain IDT status: mui_status %d, mui %d, PS domain IDT status: mui_status %d, mui %d Received IDT L2-ACK status %d mui %d ",
           rrcidt_cs_saved_open_session_req.mui_status,
           rrcidt_cs_saved_open_session_req.mui,
           rrcidt_ps_saved_open_session_req.mui_status,
           rrcidt_ps_saved_open_session_req.mui,
           cmd_ptr->cmd.downlink_l2ack_ind.status, 
           cmd_ptr->cmd.downlink_l2ack_ind.mui);

      /* Send RRC_OPEN_SESSION_CNF to MM */
      if((rrcidt_cs_saved_open_session_req.mui_status == TRUE) &&
         (rrcidt_cs_saved_open_session_req.mui == cmd_ptr->cmd.downlink_l2ack_ind.mui))
      {
        /*if L2_ACK for SMC is not received before L2_ACK for IDT,
          send open_sess_cnf here, else open_session_cnf would have
          been sent by SMC just before sending SYNC_IND
         */
        if(rrcidt_cs_saved_open_session_req.open_sess_cnf_required == TRUE)
        {
          /* Check and send any blocked SCRR for this domain */
          boolean cs_scrr_active = rrcdt_check_and_send_pending_scrr(RRC_CS_DOMAIN_CN_ID);

          /* Send a cnf to NAS if L2 ACK succeeded.Also if there is a
             pending SCRR for this domain,send a Cnf to NAS as we will
             not be pursuing the retransmissions */
          if((SUCCESS == cmd_ptr->cmd.downlink_l2ack_ind.status)||
             (TRUE == cs_scrr_active))
          {
            rrcidt_send_open_session_cnf(RRC_OPEN_SESS_SUCCESS,
                                         RRC_CS_DOMAIN_CN_ID);
            rrcidt_cs_saved_open_session_req.open_sess_cnf_required = FALSE;

            /* Delete the saved RRC_OPEN_SESSION_REQ */
            rrcidt_delete_open_session_req(RRC_CS_DOMAIN_CN_ID);

            /* Send RRC_SYNC_IND if Fake Security is enabled */
            if(nv_fake_security_enabled)
            {
              /* For CS send the RRC_SYNC_IND for Integrity Protection */
              rrcidt_send_sync_ind(RRC_INTEGRITY_ESTABLISHED,
                                   RRC_CS_DOMAIN_CN_ID);
            }
          }
          else
          {
            rrcidt_cs_saved_open_session_req.open_sess_req_status = TRUE;
            switch(rrc_get_state())
            {
              case RRC_STATE_URA_PCH:
              case RRC_STATE_CELL_PCH:   
                if(RRCCU_COMPLETED == rrccu_get_cell_update_complete_status( RRC_PROCEDURE_IDT ) )
                {
                   rrcidt_cs_saved_open_session_req.cu_init_by_idt = TRUE;
                   /* initiate cel update if not already in progress */
                   rrcdt_send_cell_update_initiate_req(RRC_PROCEDURE_IDT);
                }
                 
                /* Wait until Cell update procedure is completed */
                rrcidt_substate = RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
                return;
               
              default:
                break;
            }

            if(RRCCU_NOT_COMPLETED == 
               rrccu_get_cell_update_complete_status( RRC_PROCEDURE_IDT ) )
            {
              /* Wait until Cell update procedure is completed */
              rrcidt_substate = RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
              break;
            }

            WRRC_MSG0_HIGH("Re-transmit IDT for CS-Domain");
            rrcidt_send_initial_direct_transfer_msg(RRC_CS_DOMAIN_CN_ID,&rrcidt_cs_saved_open_session_req);   
          }           
        }
        else
        {
          rrcidt_delete_open_session_req(RRC_CS_DOMAIN_CN_ID);
        }
      }
      else if((rrcidt_ps_saved_open_session_req.mui_status == TRUE) &&
              (rrcidt_ps_saved_open_session_req.mui == 
               cmd_ptr->cmd.downlink_l2ack_ind.mui)
              )
      {
        /*if L2_ACK for SMC is not received before L2_ACK for IDT,
          send open_sess_cnf here, else open_session_cnf would have
          been sent by SMC just before sending SYNC_IND
         */
        if(rrcidt_ps_saved_open_session_req.open_sess_cnf_required == TRUE)
        {
          /* Check and send any blocked SCRR for this domain */
          boolean ps_scrr_active = rrcdt_check_and_send_pending_scrr(RRC_PS_DOMAIN_CN_ID);

          /* Send a cnf to NAS if L2 ACK succeeded.Also if there is a
             pending SCRR for this domain,send a Cnf to NAS as we will
             not be pursuing the retransmissions */
          if((SUCCESS == cmd_ptr->cmd.downlink_l2ack_ind.status)||
             (TRUE == ps_scrr_active))
          {
            rrcidt_send_open_session_cnf(RRC_OPEN_SESS_SUCCESS,
                                         RRC_PS_DOMAIN_CN_ID);

            rrcidt_ps_saved_open_session_req.open_sess_cnf_required = FALSE;

            /* Delete the saved RRC_OPEN_SESSION_REQ */
            rrcidt_delete_open_session_req(RRC_PS_DOMAIN_CN_ID);

            /* Send RRC_SYNC_IND if Fake Security is enabled */
            if(nv_fake_security_enabled)
            {
              /* For PS send the RRC_SYNC_IND for Integrity Protection */
              rrcidt_send_sync_ind(RRC_INTEGRITY_ESTABLISHED,
                                   RRC_PS_DOMAIN_CN_ID);
            }
          }
          else
          {
             /* for all other states */
             rrcidt_ps_saved_open_session_req.open_sess_req_status = TRUE;
             switch(rrc_get_state())
             {
               case RRC_STATE_URA_PCH:
               case RRC_STATE_CELL_PCH:   

                 if(RRCCU_COMPLETED == rrccu_get_cell_update_complete_status
                    ( RRC_PROCEDURE_IDT ) )
                 {
                   rrcidt_ps_saved_open_session_req.cu_init_by_idt = TRUE;
                   rrcdt_send_cell_update_initiate_req(RRC_PROCEDURE_IDT);
                 }
                  
                 /* Wait until Cell update procedure is completed */
                 rrcidt_substate = RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
                 return;

               default:
                 break;
             }

             if(RRCCU_NOT_COMPLETED == 
                  rrccu_get_cell_update_complete_status( RRC_PROCEDURE_IDT ) )
             {
                /* Wait until Cell update procedure is completed */
                rrcidt_substate = RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
                break;
             }
         
             WRRC_MSG0_HIGH("Re-transmit IDT for PS-Domain");       
             rrcidt_send_initial_direct_transfer_msg(RRC_PS_DOMAIN_CN_ID,&rrcidt_ps_saved_open_session_req);    
          }     
        }
        else
        {
           rrcidt_delete_open_session_req(RRC_PS_DOMAIN_CN_ID);
        }

      }
      /* Reset the state machine */
      if((rrcidt_cs_saved_open_session_req.mui_status == FALSE)  &&
         (rrcidt_ps_saved_open_session_req.mui_status == FALSE) 
         )
      {
        rrcidt_substate = RRCIDT_INITIAL;  
      }
      break;

    case RRC_OPEN_SESSION_REQ:
      WRRC_MSG0_HIGH("Received RRC_OPEN_SESS_REQ");
      if((RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status( RRC_PROCEDURE_IDT ) )&&
        (rrcidt_check_rrc_connection_status(cmd_ptr) == TRUE))
      {
        if(is_ue_oos_and_em_call_attempted(cmd_ptr, RRC_PROCEDURE_IDT))
        {
          rrcidt_substate = RRCIDT_INITIAL;  
          return;
        }
      }
      /* Before opening a new signalling session, be sure that there isn't one 
       * alreay open to the requested domain.  Also validate contents of the
       * open session request
       */
      if(RRCDT_SUCCESS != rrcidt_validate_open_session_req(cmd_ptr,&session_status))
      {
        /* Send open session failure with appropriate failure status */
        rrcidt_send_open_session_cnf(session_status,
                                    cmd_ptr->cmd.open_sess_req.cn_domain_id);
        return;
      }
    
      /* save the contents of the open_session_req message. will be used
       * later to send out the idt message
       */
      if(RRCDT_SUCCESS != rrcidt_save_open_session_req(cmd_ptr))
      {
        /* Send RRC_OPEN_SESS_CNF Primitive to MM with Failure status */
        rrcidt_send_open_session_cnf(RRC_OPEN_SESS_FAILURE,
                                    cmd_ptr->cmd.open_sess_req.cn_domain_id);
        return;
      }

      if(is_ue_oos_and_em_call_attempted(cmd_ptr, RRC_PROCEDURE_IDT))
      {
        rrcidt_substate = RRCIDT_INITIAL;  
      }
      else
      {

        rrcidt_set_est_cause(cmd_ptr->cmd.open_sess_req.est_cause, 
               RRC_PROCEDURE_IDT,cmd_ptr->cmd.open_sess_req.cn_domain_id);

        /* Build and send the Initial Direct Transfer Msg to Send Chain */
        if(rrcidt_cs_saved_open_session_req.open_sess_req_status == TRUE)
        {
          rrcidt_send_initial_direct_transfer_msg(RRC_CS_DOMAIN_CN_ID,&rrcidt_cs_saved_open_session_req);
        }
        else if(rrcidt_ps_saved_open_session_req.open_sess_req_status == TRUE)
        {
          rrcidt_send_initial_direct_transfer_msg(RRC_PS_DOMAIN_CN_ID,&rrcidt_ps_saved_open_session_req);
        }
      }
      break;

    case RRC_NAS_DATA_TRANSFER_IND:
      rrcidt_send_nas_data_ind();  
      /* Reset the state machine */  
      rrcidt_delete_open_session_req(RRC_CS_DOMAIN_CN_ID);  
      rrcidt_delete_open_session_req(RRC_PS_DOMAIN_CN_ID);
      rrcidt_reset_est_cause(RRC_PROCEDURE_CU);  
      rrcidt_substate = RRCIDT_INITIAL; 
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d for IDT in substate:%d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcidt_substate);
      break;
  }
 
} /* rrcidt_process_idt_wait_for_l2ack_substate */

/*===========================================================================

FUNCTION rrcidt_process_cell_fach_state

DESCRIPTION
  This function processes all events that are dispatched to Initial direct
  Transfer procedure in CELL_FACH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcidt_process_cell_fach_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcidt_substate)
  {
    case RRCIDT_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_OPEN_SESSION_REQ:

        case RRC_NAS_DATA_TRANSFER_IND:
          rrcidt_process_idt_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG1_HIGH("IDT:Invalid eventin Substate: %d",
                    rrcidt_substate);
          break;
      }
      break;

    case RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_CELL_UPDATE_COMPLETE_CNF:
        case RRC_OPEN_SESSION_REQ:
        case RRC_NAS_DATA_TRANSFER_IND:
          rrcidt_process_idt_wait_for_cell_update_comp_cnf_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("IDT:Invalid event in Substate: %d",
                    rrcidt_substate);
          break;
      }
      break;

    case RRCIDT_WAIT_FOR_L2ACK:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_DOWNLINK_L2ACK_IND:
        case RRC_OPEN_SESSION_REQ:
        case RRC_NAS_DATA_TRANSFER_IND:
          rrcidt_process_idt_wait_for_l2ack_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("IDT:Invalid event in Substate: %d",
                    rrcidt_substate);
          break;
      }
      break;


    default:           
      WRRC_MSG1_HIGH("IDT:Invalid substate:%d in CELL_FACH",
                rrcidt_substate);
      break;
  }  
}/* rrcidt_process_cell_fach_state */
/*===========================================================================

FUNCTION rrcidt_process_cell_dch_state

DESCRIPTION
  This function processes all events that are dispatched to Initial direct
  Transfer procedure in CELL_DCH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcidt_process_cell_dch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcidt_substate)
  {
    case RRCIDT_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_OPEN_SESSION_REQ:
        case RRC_NAS_DATA_TRANSFER_IND:
          rrcidt_process_idt_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG1_HIGH("IDT:Invalid event in Substate: %d",
                    rrcidt_substate);
          break;
      }
      break;

    case RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_CELL_UPDATE_COMPLETE_CNF:
        case RRC_OPEN_SESSION_REQ:
        case RRC_NAS_DATA_TRANSFER_IND:
          rrcidt_process_idt_wait_for_cell_update_comp_cnf_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("IDT:Invalid event in Substate: %d",
                    rrcidt_substate);
          break;
      }
      break;

    case RRCIDT_WAIT_FOR_L2ACK:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_DOWNLINK_L2ACK_IND:
        case RRC_OPEN_SESSION_REQ:
        case RRC_NAS_DATA_TRANSFER_IND:
          rrcidt_process_idt_wait_for_l2ack_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("IDT:Invalid event in Substate: %d",
                    rrcidt_substate);
          break;
      }
      break;

    default:           
      WRRC_MSG1_HIGH("IDT:Invalid substate:%d in CELL_DCH",
                rrcidt_substate);
      break;
  }  
}/* rrcidt_process_cell_dch_state */

/*===========================================================================

FUNCTION rrcidt_process_cell_ura_pch_state

DESCRIPTION
  This function processes all events that are dispatched to Initial direct
  Transfer procedure in either in CELL_PCH or URA_PCH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcidt_process_cell_ura_pch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcidt_substate)
  {
    case RRCIDT_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_OPEN_SESSION_REQ:
          rrcidt_process_idt_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG1_HIGH("IDT:Invalid event in Substate: %d",
                    rrcidt_substate);
          break;
      }
      break;

    case RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF:
      rrcidt_process_idt_wait_for_cell_update_comp_cnf_substate(cmd_ptr);
      break;

    default:
      WRRC_MSG1_HIGH("IDT:Invalid substate:%d in PCH State: %d",
                rrcidt_substate);
      break;
  }  
}/* rrcidt_process_cell_ura_pch_state */
/*===========================================================================

FUNCTION rrcidt_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the ininitial direct transfer
  procedure.
  The following events are handled by this procedure:

    RRC_OPEN_SESSION_REQ Primitive (received from MM)
    RRC_CELL_UPDATE_COMPLETE_CNF primitive (received from CELL UPDATE Procedure)
    RRC_NAS_DATA_TRANSFER_IND primitive from RR
    RRC_DOWNLINK_L2ACK_IND primitive (received from RLC)

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcidt_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG2_HIGH_OPT("function rrcidt_procedure_event_handler is called in rrc_state = rrc_state_e_type_value%d and rrcidt_substate = rrcidt_substate_e_type_value%d", rrc_state, rrcidt_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_IDT,rrcidt_substate,cmd_ptr->cmd_hdr.cmd_id);

  if(RRC_STATE_CHANGE_IND == cmd_ptr->cmd_hdr.cmd_id)
  {
    WRRC_MSG2_HIGH("State-Change-Ind idt-state %d in rrc-state %d",rrcidt_substate,rrc_state);
    
    //Any pending message memory needs to be de-allocated here 
    rrcidt_delete_open_session_req(RRC_CS_DOMAIN_CN_ID);  
    rrcidt_delete_open_session_req(RRC_PS_DOMAIN_CN_ID);  
    rrcidt_substate = RRCIDT_INITIAL;  
  }
  else
  {
    switch (rrc_state)
    {
      case RRC_STATE_CELL_FACH:    
      rrcidt_process_cell_fach_state(cmd_ptr); 
      break;
  
      case RRC_STATE_CELL_DCH:  
      rrcidt_process_cell_dch_state(cmd_ptr); 
      break;
  
      case RRC_STATE_CELL_PCH:
      case RRC_STATE_URA_PCH:
      rrcidt_process_cell_ura_pch_state(cmd_ptr);
      break;
  
      default:  
      WRRC_MSG1_HIGH("IDT:Event forwarded in invalid State: %d",rrc_state);
      break;
    }  
  }
} /* rrcidt_procedure_event_handler */
/*===========================================================================

FUNCTION rrcdt_is_cs_domain_open

DESCRIPTION
  This function returns whether session for CS domain is open or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - CS Domain is Open.
  FALSE - CS Domain isn't Open.

===========================================================================*/
boolean rrcdt_is_cs_domain_open(void)
{
  return rrcdt_int_data.cs_domain_open;
}
/*===========================================================================

FUNCTION rrcdt_is_ps_domain_open

DESCRIPTION
  This function returns whether session for PS domain is open or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - PS Domain is Open.
  FALSE - PS Domain isn't Open.

===========================================================================*/
boolean rrcdt_is_ps_domain_open(void)
{
  return rrcdt_int_data.ps_domain_open;
}

/*===========================================================================

FUNCTION rrcudt_get_cn_domain_for_mui

DESCRIPTION
  This function returns the CN Domain Id for the mui

DEPENDENCIES
  None.

RETURN VALUE
  CN domain id

===========================================================================*/
static rrc_cn_domain_identity_e_type rrcudt_get_cn_domain_for_mui
(
  uint8 mui              
)
{
  /* Initialize domain id to keep lint happy */
  rrc_cn_domain_identity_e_type domain_id = RRC_CS_DOMAIN_CN_ID;
  boolean domain_found = FALSE;
  uint32 count = 0;

  for (count = 0;count < MAX_NAS_MESSAGES_FOR_CS_DOMAIN;count++ )
  {
    if((rrcudt_cs_saved_data[count].mui == mui) && 
       (rrcudt_cs_saved_data[count].mui_status == TRUE)) 
    {
      domain_id = RRC_CS_DOMAIN_CN_ID;
	    domain_found = TRUE;
      break;
    }
  }

  if(domain_found != TRUE)
  {
    for (count = 0;count < MAX_NAS_MESSAGES_FOR_PS_DOMAIN;count++ )
    {
      if((rrcudt_ps_saved_data[count].mui == mui) && 
         (rrcudt_ps_saved_data[count].mui_status == TRUE)) 
      {
        domain_id = RRC_PS_DOMAIN_CN_ID;
        domain_found = TRUE;
        break;
      }
    }
  }
  return domain_id;
}

/*===========================================================================

FUNCTION rrcudt_validate_data_req  

DESCRIPTION
  This function validates RRC_DATA_REQ command received from MM task.

DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type: It returns RRCDT_SUCCESS if it is successfully validates 
  the RRC_OPEN_SESSION_REQ command. Otherwise it returns RRCDT_FAILURE_MINOR.

===========================================================================*/

static rrcdt_status_e_type rrcudt_validate_data_req
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrcdt_status_e_type status = RRCDT_FAILURE_MINOR; 

  boolean domain_open;                  /* Indicates if the domain is open */

   /* Check whether signalling connection is open for the received CN
    * Domain Id.
    */
  domain_open = FALSE;

  /* Check whether RCR procedure is active or not */
  if (rrcrcr_is_proc_active() == TRUE)
  {
    WRRC_MSG0_HIGH("RCR procedure is active");
    return (status);
  }

  switch (cmd_ptr->cmd.data_req.cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID :
      if (rrcdt_int_data.cs_domain_open )
      {
        domain_open = TRUE;
      }
      break;

    case RRC_PS_DOMAIN_CN_ID :
      if (rrcdt_int_data.ps_domain_open)
      {
        domain_open = TRUE;
      }
      break;

    default :
      /* MM sent us an invalid CN Domain ID
       */
      WRRC_MSG1_HIGH("Invalid CN Domain ID %d", 
                cmd_ptr->cmd.data_req.cn_domain_id);
      break;
  }

  /* If the domain is not open, print a message and exit
   */
  if (domain_open == FALSE)
  {
    WRRC_MSG1_HIGH("CN Domain ID not open: %d", cmd_ptr->cmd.data_req.cn_domain_id);
    
  }
  else
  {

    /* Validate the Data Req primitive. Check to make sure that the payload 
     * isn't too large or isn't empty.
     */
    if ((cmd_ptr->cmd.data_req.nas_msg.length > RRCDT_MAX_PAYLOAD) ||
        (cmd_ptr->cmd.data_req.nas_msg.length == 0))
    {
      WRRC_MSG1_HIGH("Invalid payload size: %d", cmd_ptr->cmd.data_req.nas_msg.length);
    }
    else
    {
      status = RRCDT_SUCCESS;
    }
  }
  return (status);

} /* rrcudt_validate_data_req */
/*===========================================================================

FUNCTION rrcudt_any_saved_data_req_for_cn_domain

DESCRIPTION
  This function will give status of any saved data req for a given CN domain

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:- If any dat req saved of in process for a CN domain
  FALSE:- otherwise

===========================================================================*/
boolean rrcudt_any_saved_data_req_for_cn_domain
(
  rrc_cn_domain_identity_e_type cn_domain_id
)
{
  uint8 count;                          /* Local variable for index */
  boolean status = FALSE;
  switch (cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID :
      for (count = 0; count <MAX_NAS_MESSAGES_FOR_CS_DOMAIN; count++ )
      {
        if(rrcudt_cs_saved_data[count].mui_status || 
           rrcudt_cs_saved_data[count].data_req_status)
        {
          status = TRUE;
          break;
        }
      }
      break;
    
    case RRC_PS_DOMAIN_CN_ID :
      for (count = 0; count <MAX_NAS_MESSAGES_FOR_PS_DOMAIN; count++ )
      {
        if(rrcudt_ps_saved_data[count].mui_status || 
           rrcudt_ps_saved_data[count].data_req_status)
        {
          status = TRUE;
          break;
        }
      }
      break;
    default:
      WRRC_MSG1_ERROR("Invalid CN Domain:%d",cn_domain_id);
      break;
  }
  WRRC_MSG2_HIGH("Saved data req for CN domain %d, status = %d",cn_domain_id,status);
  return status;
} /* rrcudt_delete_all_saved_data_req */

/*===========================================================================

FUNCTION rrcudt_delete_all_saved_data_req

DESCRIPTION
  This function deletes the Data Req cmd from the static variable
  for the specified CN domain

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcudt_delete_all_saved_data_req
(
  rrc_cn_domain_identity_e_type cn_domain_id
)
{
  uint8 count;                          /* Local variable for index */
  switch (cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID :
      for (count = 0; count <MAX_NAS_MESSAGES_FOR_CS_DOMAIN; count++ )
      {
        if(rrcudt_cs_saved_data[count].mui_status || 
           rrcudt_cs_saved_data[count].data_req_status)
        {
          rrcudt_cs_saved_data[count].mui_status = FALSE;
          rrcudt_cs_saved_data[count].data_req_status = FALSE;

          if(rrcudt_cs_saved_data[count].data_req.nas_msg.value != NULL)
          {
            rrc_free(rrcudt_cs_saved_data[count].data_req.nas_msg.value);
            rrcudt_cs_saved_data[count].data_req.nas_msg.value = NULL;
            rrcudt_cs_saved_data[count].data_req.nas_msg.length = 0;
#ifdef FEATURE_NAS_UDT_OOS
            #error code not present
#endif
          }
        }
      }
      break;
    
    case RRC_PS_DOMAIN_CN_ID :
      for (count = 0; count <MAX_NAS_MESSAGES_FOR_PS_DOMAIN; count++ )
      {
        if(rrcudt_ps_saved_data[count].mui_status || 
           rrcudt_ps_saved_data[count].data_req_status)
        {
          rrcudt_ps_saved_data[count].mui_status = FALSE;
          rrcudt_ps_saved_data[count].data_req_status = FALSE;

          if(rrcudt_ps_saved_data[count].data_req.nas_msg.value != NULL)
          {
            rrc_free(rrcudt_ps_saved_data[count].data_req.nas_msg.value);
            rrcudt_ps_saved_data[count].data_req.nas_msg.value = NULL;
            rrcudt_ps_saved_data[count].data_req.nas_msg.length = 0;
#ifdef FEATURE_NAS_UDT_OOS
            #error code not present
#endif
          }
        }
      }
      break;
    default:
      WRRC_MSG1_ERROR("Invalid CN Domain:%d",cn_domain_id);
      break;
  }

#ifdef FEATURE_WCDMA_UL_COMPR
  rrc_ul_comp_cap_support = FALSE;
#endif
} /* rrcudt_delete_all_saved_data_req */
/*===========================================================================

FUNCTION rrcudt_delete_saved_data_req

DESCRIPTION
  This function deletes the Data Req cmd from the static variable
  for the specified MUI of a particular Radio Bearer

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static uecomdef_status_e_type rrcudt_delete_saved_data_req
(
  rrc_nas_msg_priority_e_type priority,
                                        /* Priority selected NAS for 
                                           xmission */
  uint8 rcvd_mui                        /* Received MUI from RLC */
)
{
  uint8 count;                          /* Local variable for index */
  uecomdef_status_e_type status = FAILURE;
  

  for (count = 0; count < MAX_NAS_MESSAGES_FOR_CS_DOMAIN; count++ )
  {
    if(rrcudt_cs_saved_data[count].mui_status == TRUE)
    {
      if( (rrcudt_cs_saved_data[count].mui == rcvd_mui) &&
          (rrcudt_cs_saved_data[count].data_req.priority == priority) )
      {
        rrcudt_cs_saved_data[count].mui_status = FALSE;
        rrcudt_cs_saved_data[count].data_req_status = FALSE;
        if (rrcudt_cs_saved_data[count].data_req.nas_msg.value !=
            NULL)
        {
          rrc_free(rrcudt_cs_saved_data[count].data_req.nas_msg.value);
          rrcudt_cs_saved_data[count].data_req.nas_msg.value = NULL;
          rrcudt_cs_saved_data[count].data_req.nas_msg.length = 0;
#ifdef FEATURE_NAS_UDT_OOS
          #error code not present
#endif
        }
        status = SUCCESS;
        break;
      }
      
    }
  }
  if(status == SUCCESS)
  {
    return(status);
  }
  for (count = 0; count < MAX_NAS_MESSAGES_FOR_PS_DOMAIN; count++ )
  {
    if(rrcudt_ps_saved_data[count].mui_status == TRUE)
    {
      if( (rrcudt_ps_saved_data[count].mui == rcvd_mui) &&
          (rrcudt_ps_saved_data[count].data_req.priority == priority) )
      {
        rrcudt_ps_saved_data[count].mui_status = FALSE;
        rrcudt_ps_saved_data[count].data_req_status = FALSE;
        if (rrcudt_ps_saved_data[count].data_req.nas_msg.value !=
            NULL)
        {
          rrc_free(rrcudt_ps_saved_data[count].data_req.nas_msg.value);
          rrcudt_ps_saved_data[count].data_req.nas_msg.value = NULL;
          rrcudt_ps_saved_data[count].data_req.nas_msg.length = 0;
#ifdef FEATURE_NAS_UDT_OOS
          #error code not present
#endif
        }
        
        status = SUCCESS;
        break;
      }
      
    }
  }
  return (status); 
} /* rrcudt_delete_saved_data_req */
/*===========================================================================

FUNCTION rrcudt_check_if_msg_status_can_be_saved

DESCRIPTION
  This function checks the saved message status and verifies whether new can
  be saved or not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static uecomdef_status_e_type rrcudt_check_if_msg_status_can_be_saved
(
  rrc_cn_domain_identity_e_type cn_domain_id,
  uint8 *index
)
{
  uecomdef_status_e_type status = FAILURE;
  uint8 count;                /* Local variable for indexing */

  switch (cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID :
      for (count = 0; count <MAX_NAS_MESSAGES_FOR_CS_DOMAIN; count++ )
      {
        if((rrcudt_cs_saved_data[count].mui_status == FALSE) &&
           (rrcudt_cs_saved_data[count].data_req_status ==FALSE) )
        {
          status = SUCCESS;
          *index = count;
          break;
        }
      }
      break;
    
    case RRC_PS_DOMAIN_CN_ID :
      for (count = 0; count <MAX_NAS_MESSAGES_FOR_PS_DOMAIN; count++ )
      {
        if((rrcudt_ps_saved_data[count].mui_status == FALSE) &&
           (rrcudt_ps_saved_data[count].data_req_status == FALSE) )
        {
          status = SUCCESS;
          *index = count;
          break;
        }
      }
      break;
    default:
      WRRC_MSG1_ERROR("Invalid CN domain:%d",cn_domain_id);
      break;
  }
  return (status);
} /* rrcudt_check_if_msg_status_can_be_saved */
/*===========================================================================

FUNCTION rrcudt_check_saved_message_status

DESCRIPTION
  This function checks the saved message status and verifies whether new can
  be saved or not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static uecomdef_status_e_type rrcudt_check_saved_message_status
(
  void
)
{
  uecomdef_status_e_type status = FAILURE;
  uint8 count;                /* Local variable for indexing */

  for (count = 0; count <MAX_NAS_MESSAGES_FOR_CS_DOMAIN; count++ )
  {
    if((rrcudt_cs_saved_data[count].mui_status == TRUE) ||
       (rrcudt_cs_saved_data[count].data_req_status == TRUE) )
    {
      status = SUCCESS;
      break;
    }
  }
  if(status == SUCCESS)
  { 
    return(status);
  }

  for (count = 0; count <MAX_NAS_MESSAGES_FOR_PS_DOMAIN; count++ )
  {
    if((rrcudt_ps_saved_data[count].mui_status == TRUE) ||
       (rrcudt_ps_saved_data[count].data_req_status == TRUE) )
    {
      status = SUCCESS;
      break;
    }
  }
 
  return (status);
} /* rrcudt_check_saved_message_status */

/*===========================================================================

FUNCTION rrcudt_check_saved_mui_status

DESCRIPTION
  This function checks the status of MUIs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static uecomdef_status_e_type rrcudt_check_saved_mui_status
(
 void
)
{
  uecomdef_status_e_type status = FAILURE;
  uint8 count;                /* Local variable for indexing */

  
  for (count = 0; count <MAX_NAS_MESSAGES_FOR_CS_DOMAIN; count++ )
  {
    if(rrcudt_cs_saved_data[count].mui_status == TRUE)
    {
      status = SUCCESS;
      break;
    }
  }

  if(status == SUCCESS)
  {
    return(status);
  }
  for (count = 0; count <MAX_NAS_MESSAGES_FOR_PS_DOMAIN; count++ )
  {
    if(rrcudt_ps_saved_data[count].mui_status == TRUE)
    {
      status = SUCCESS;
      break;
    }
  }
  return (status);
} /* rrcudt_check_saved_mui_status */

/*===========================================================================

FUNCTION rrcudt_save_data_req

DESCRIPTION
  This function saves the Data Req cmd in the static variable
  saved_data_req for CS/PS domain

DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type.

===========================================================================*/
static rrcdt_status_e_type rrcudt_save_data_req
(
  rrc_cmd_type *cmd_ptr                             /* Pointer to RRC Command */
)
{
  unsigned char       *payload;                     /* Pointer to the MM payload */
  rrc_data_req_type   *save_temp_ptr=NULL;          /* pointer to save data req type */
  boolean             *save_data_req_status_ptr=NULL; /* data req flag pointer */
  uint8               index;                        /* Local variable for index */
  rrcdt_status_e_type save_status = RRCDT_FAILURE_MINOR;  /* save status indicator */

  if(rrcudt_check_if_msg_status_can_be_saved(
       cmd_ptr->cmd.data_req.cn_domain_id, &index ) == SUCCESS)
  {
    switch (cmd_ptr->cmd.data_req.cn_domain_id)
    {
      case RRC_CS_DOMAIN_CN_ID :
  
        save_temp_ptr          = &rrcudt_cs_saved_data[index].data_req;
        save_data_req_status_ptr = &rrcudt_cs_saved_data[index].data_req_status;
        save_temp_ptr->cn_domain_id = RRC_CS_DOMAIN_CN_ID;
        break;
      
      case RRC_PS_DOMAIN_CN_ID :
        save_temp_ptr          = &rrcudt_ps_saved_data[index].data_req;
        save_data_req_status_ptr = &rrcudt_ps_saved_data[index].data_req_status;
        save_temp_ptr->cn_domain_id = RRC_PS_DOMAIN_CN_ID;
        break;
      
      default :
        ERR_FATAL("Invalid Domain ID %d", 
                   cmd_ptr->cmd.data_req.cn_domain_id, 0, 0);
        break;
    }
    /* Determine whether the Uplink Direct Transfer Message should go out on the
     * high or low priority radio bearer
     */
    if (cmd_ptr->cmd.data_req.priority == RRC_NAS_MSG_HIGH_PRIO)
    {
      save_temp_ptr->priority = RRC_NAS_MSG_HIGH_PRIO;
    }
    else
    {
      save_temp_ptr->priority = RRC_NAS_MSG_LOW_PRIO;
    }
#ifdef FEATURE_WCDMA_MTPL_THERMAL_BACKOFF 
    save_temp_ptr->is_emergency_call = cmd_ptr->cmd.data_req.is_emergency_call;
#endif
    /* Allocate a buffer for the payload. This memory is released by 
     * rrcsend_send_ul_sdu().
     */
    payload = rrc_malloc(cmd_ptr->cmd.data_req.nas_msg.length);
    
    /* Copy the payload from the MM primitive into the newly allocated memory
     */
    WCDMA_MEMCPY(payload, 
                 cmd_ptr->cmd.data_req.nas_msg.length,
                 cmd_ptr->cmd.data_req.nas_msg.value,
                 cmd_ptr->cmd.data_req.nas_msg.length);
     /*Save Protocol discriminator & Transaction I.D from NAS OTA message*/
#ifdef FEATURE_NAS_UDT_OOS
     #error code not present
#endif
    /* Set the pointer to the payload and the length of the payload in the 
     * Initial Direct Transfer Message
     */
    save_temp_ptr->nas_msg.value = payload;
    save_temp_ptr->nas_msg.length = 
      (unsigned short) cmd_ptr->cmd.data_req.nas_msg.length;
    
    /* set the flag to indicate that DATA_REQ cmd contents are saved and 
     * return success status
     */
    *(save_data_req_status_ptr) = TRUE;
  
    save_status=RRCDT_SUCCESS;
  }
  else
  {
    WRRC_MSG1_HIGH("Can't processData Req for Domain:%d",
              cmd_ptr->cmd.data_req.cn_domain_id);
  }
  return(save_status);
} /* rrcudt_save_data_req */


#ifdef FEATURE_WCDMA_UL_COMPR
/*===========================================================================

FUNCTION rrcdt_nv_supports_ul_comp

DESCRIPTION
  This function checks if UL compression is supported or not

DEPENDENCIES
  None.

RETURN VALUE
  void.

===========================================================================*/
boolean rrcdt_nv_supports_ul_comp
(
  void
)
{
#ifdef FEATURE_DUAL_SIM
  uint16 nv_value = rrc_get_concurrency_nv();
#endif

  if(!rrc_ul_compr_cap_nv)
  {
    WRRC_MSG0_HIGH("ULCOMP : UL compression NV is disabled");
    return FALSE;
  }

#ifdef FEATURE_DUAL_SIM
  if(CHECK_CONCURRENCY_FOR_UL_COMP(nv_value))
  {
    WRRC_MSG1_HIGH("ULCOMP : UL compression NV is disabled for device_mode %d", wcdma_device_mode);
    return FALSE;
  }
#endif

  WRRC_MSG0_HIGH("ULCOMP NV enabled");
  return TRUE;
}
#endif

/*===========================================================================

FUNCTION rrcudt_send_uplink_direct_transfer_msg

DESCRIPTION
  This function sends the uplink direct transfer on the uplink DCCH.

DEPENDENCIES
  None.

RETURN VALUE
  void.

===========================================================================*/
static void rrcudt_send_uplink_direct_transfer_msg
(
 void
)
{
  uint8 count_domain;                   /* Keep count of CS/PS*/
  uint8 count_index;                    /* Count for saved messages */
  rrc_UL_DCCH_Message *msg_ptr;         /* Pointer to uplink DCCH message */ 
  
  rlc_lc_id_type ul_lc_id;              /* Uplink logical channel ID */

  rrcsend_status_e_type send_status;    /* send status of UL message */

  uint8 mui;                            /* Local varibale for MUI */

  rrc_data_req_type *save_temp_ptr=NULL;
  boolean           *save_data_req_status_ptr=NULL;
  uint8             no_of_messages_for_domain;
  
  for (count_domain = 0; count_domain < RRC_MAX_CN_DOMAINS; count_domain++)  
  /* need count_domian 0,1 for cs and ps domain*/
  {
    if(count_domain == 0)
    {
      no_of_messages_for_domain = MAX_NAS_MESSAGES_FOR_CS_DOMAIN;
    }
    else
    {
      no_of_messages_for_domain = MAX_NAS_MESSAGES_FOR_PS_DOMAIN;
    }
    for ( count_index = 0; count_index < no_of_messages_for_domain; 
          count_index++)
    {
      if( count_domain == 0)
      {
        save_data_req_status_ptr = 
          &rrcudt_cs_saved_data[count_index].data_req_status;
        save_temp_ptr = &rrcudt_cs_saved_data[count_index].data_req;
      }
      else 
      { 
        if(count_index < MAX_NAS_MESSAGES_FOR_PS_DOMAIN )
        {
          save_data_req_status_ptr = 
            &rrcudt_ps_saved_data[count_index].data_req_status;
          save_temp_ptr = &rrcudt_ps_saved_data[count_index].data_req;
        }
        else
        {
           WRRC_MSG1_ERROR("count_index %d cant be more than MAX_NAS_MESSAGES_FOR_PS_DOMAIN",
                         count_index);
           return;
        }
      }
    
      if( *(save_data_req_status_ptr) == TRUE)
      {
        if((is_scrr_in_progress) && (rrcscrr_substate != RRCSCRR_INITIAL))
        {
          if((save_temp_ptr->cn_domain_id == RRC_CS_DOMAIN_CN_ID) &&
             (rrcscrr_saved_cs_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE))
          {
            WRRC_MSG0_HIGH("SCRR for CS in progress. Discard CS UDT");
            return;
          }
          else if((save_temp_ptr->cn_domain_id == RRC_PS_DOMAIN_CN_ID) &&
             (rrcscrr_saved_ps_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE))
          {
            WRRC_MSG0_HIGH("SCRR for PS in progress. Discard PS UDT");
            return;
          }
        }
  
        /* Determine the logical channel id for the Uplink DCCH */
        if(save_temp_ptr->priority == RRC_NAS_MSG_HIGH_PRIO)
        {
           ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                             DCCH_DT_HIGH_PRI_RADIO_BEARER_ID, 
                                             UE_MODE_ACKNOWLEDGED_DATA);
        }
        else
        {
           ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                             DCCH_DT_LOW_PRI_RADIO_BEARER_ID, 
                                             UE_MODE_ACKNOWLEDGED_DATA);
        }
        /*lint -save -e650 */
        if( (ul_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND) &&
            (save_temp_ptr->priority == RRC_NAS_MSG_LOW_PRIO) ) 
        {
        /* The logical channel id was not found. If we were looking for the low
         * priority radio bearer (RB 4), try the high priority radio bearer (RB 3).
         */
          
          ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                               DCCH_DT_HIGH_PRI_RADIO_BEARER_ID, 
                                               UE_MODE_ACKNOWLEDGED_DATA);

          save_temp_ptr->priority = RRC_NAS_MSG_HIGH_PRIO;
        }
        /*lint -restore */
        if (ul_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
        {
          WRRC_MSG0_HIGH("RBs 3 and 4 not found by LCM");
  
          /*delete CS and PS domain saved info as LC ID is not present    */
          rrcudt_delete_all_saved_data_req(RRC_CS_DOMAIN_CN_ID);
          rrcudt_delete_all_saved_data_req(RRC_PS_DOMAIN_CN_ID);

          /* Reset the state machine */
          rrcudt_substate = RRCUDT_INITIAL;
  
          return;
        }
        else
        {
        /* Build an Uplink Direct Transfer Message. Start by allocating an Uplink 
         * DCCH message. This memory is released by rrcsend_send_ul_sdu().
         */
          msg_ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
            sizeof(struct rrc_UL_DCCH_Message));
          
         /* Identify this message as an Uplink Direct Transfer Message
          */
          RRCSET_MSG_TYPE(msg_ptr->message,
              T_rrc_UL_DCCH_MessageType_uplinkDirectTransfer);
          
        
          RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.uplinkDirectTransfer);

          if(rrc_get_state() ==  RRC_STATE_CELL_FACH)
          {
            /* Function call to Measurement Control & Report procedure. */
            if(RRCMEAS_RACH_SUCCESSFUL == rrcmeas_append_meas_rpt_on_rach(
               &msg_ptr->message.u.uplinkDirectTransfer.measuredResultsOnRACH, RRC_PROCEDURE_UDT, FALSE))
            {
            /* if Measurement Control & Reportprocedure returns RRC_MEAS_RACH_SUCCESFUL,
               then enable the  bit_mask field */
              RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.uplinkDirectTransfer,
                rrc_UplinkDirectTransfer_,
                measuredResultsOnRACH);
             
            }
            else
            {
              MSG_MED("RACH meas is not appended",0,0,0);
            }
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
            #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/

          }
          
      
          /* Copy the contents of Saved RRC_DATA_REQ 
          */    
          WCDMA_MEMCPY(msg_ptr->message.u.uplinkDirectTransfer.nas_Message.data,
                       sizeof(OSOCTET) * 4095,
                       save_temp_ptr->nas_msg.value, 
                       save_temp_ptr->nas_msg.length);
      
          msg_ptr->message.u.uplinkDirectTransfer.nas_Message.numocts = 
            save_temp_ptr->nas_msg.length;
          
         /* Fill in the CN Domain id given by the command 
          */
    
          if(save_temp_ptr->cn_domain_id == RRC_CS_DOMAIN_CN_ID)
          {
            msg_ptr->message.u.uplinkDirectTransfer.cn_DomainIdentity = 
            rrc_CN_DomainIdentity_cs_domain;
          }
          else
          {
            msg_ptr->message.u.uplinkDirectTransfer.cn_DomainIdentity =
              rrc_CN_DomainIdentity_ps_domain;
          }
          
#ifdef FEATURE_WCDMA_UL_COMPR
        /* Changes pertaining to UL compression support */
        WRRC_MSG2_HIGH("RRC_ULCOMP: rrc_ul_comp_cap_support %d, UL compr NV %d", rrc_ul_comp_cap_support, rrc_ul_compr_cap_nv);
        if((TRUE == rrcdt_nv_supports_ul_comp()) && (rrc_ul_comp_cap_support))
        {          
          rrc_UL_CompressionCapabilitiesList *ul_comp_cap_list_ptr = NULL;
          msg_ptr->message.u.uplinkDirectTransfer.m.laterNonCriticalExtensionsPresent = 1;
          msg_ptr->message.u.uplinkDirectTransfer.laterNonCriticalExtensions.m.uplinkDirectTransfer_r3_add_extPresent = 1;
          msg_ptr->message.u.uplinkDirectTransfer.laterNonCriticalExtensions.uplinkDirectTransfer_r3_add_ext.m.compressionCapabilityPresent = 1;

          msg_ptr->message.u.uplinkDirectTransfer.laterNonCriticalExtensions.uplinkDirectTransfer_r3_add_ext.compressionCapability.m.
            ul_CompressionCapabilitiesListPresent = 1;

          ul_comp_cap_list_ptr = & msg_ptr->message.u.uplinkDirectTransfer.laterNonCriticalExtensions.
            uplinkDirectTransfer_r3_add_ext.compressionCapability.ul_CompressionCapabilitiesList;

          ul_comp_cap_list_ptr->n = 1;

          ALLOC_ASN1ARRAY2(&enc_ctxt,ul_comp_cap_list_ptr,rrc_UL_CompressionCapabilities);

          /* Hardcoding the capabilities for Phase 1 */
          ul_comp_cap_list_ptr->elem[0].compressionMechanism = rrc_CompressionMechanism_ipdc;
          ul_comp_cap_list_ptr->elem[0].compressionVersion.t = T_rrc_UL_CompressionCapabilities_compressionVersion_implicit;

          ul_comp_cap_list_ptr->elem[0].compressionVersion.u.implicit = rrc_CompressionVersion_version1;

          ul_comp_cap_list_ptr->elem[0].memoryBuffer =  rrc_UL_CompressionCapabilities_memoryBuffer_kb16384;

          MSG_HIGH("RRC_ULCOMP: Comp mechanism %d %d %d",ul_comp_cap_list_ptr->elem[0].compressionMechanism,ul_comp_cap_list_ptr->elem[0].compressionVersion.u.implicit,ul_comp_cap_list_ptr->elem[0].memoryBuffer);
        }
#endif

          
         /* Send the Uplink Direct Transfer Message on the Uplink DCCH
          */
          send_status = rrcsend_send_ul_sdu_with_mui(RRC_PROCEDURE_UDT,
                                            msg_ptr,
                                            ul_lc_id,
                                            RRCSEND_L2ACK_REQUIRED,
                                            &mui
                                            );
  
          if (send_status == RRCSEND_SUCCESS)
          {
            switch(save_temp_ptr->cn_domain_id)
            {
              case RRC_CS_DOMAIN_CN_ID :
                if(count_index < MAX_NAS_MESSAGES_FOR_CS_DOMAIN)
                {
                  rrcudt_cs_saved_data[count_index].mui_status = TRUE;
                  rrcudt_cs_saved_data[count_index].mui = mui;
#ifdef FEATURE_WCDMA_MTPL_THERMAL_BACKOFF 
                  if(TRUE == save_temp_ptr->is_emergency_call)
                  {
                    rrcrce_set_rrc_est_cause_for_emergency_call();
                  }
#endif
                }
                break;
              
              case RRC_PS_DOMAIN_CN_ID :
#ifdef FEATURE_INACTIVITY_HANDLING
                if (rrc_get_state() == RRC_STATE_CELL_DCH) 
                {
                  WRRC_MSG1_HIGH("Restarting inact timer due to UDT %d",INACTIVITY_TIMER_IN_MS);
                  rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, INACTIVITY_TIMER_IN_MS);
                }
                else if (rrc_get_state() == RRC_STATE_CELL_FACH)
                {
                  WRRC_MSG2_HIGH("Restarting Inactivity timer %d + remain T305 %d",INACTIVITY_TIMER_IN_MS,
                           rrctmr_get_remaining_time(RRCTMR_T_305_TIMER));
                  /* Restart incativity handling timer */
                  if(rrc_convert_t305_to_ms(RRC_GET_T305()) != RRCTMR_INFINITY_VAL )
                  {
                    rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                                      (unsigned long)(INACTIVITY_TIMER_IN_MS + rrctmr_get_remaining_time(RRCTMR_T_305_TIMER))); 
                  }
                  else
                  {
                    /*If T305 timer was infinity then the timer wouldnt have been started . In thats case start
                        timer of 5 mins plus 24 hrs*/
                    rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                                      (unsigned long)(INACTIVITY_TIMER_IN_MS + RRCTMR_INFINITY_VAL)); 
                  }
                }
                (void)rrcmisc_handle_ps_rab_inactivity(1);
#endif /* FEATURE_INACTIVITY_HANDLING */
                if(count_index < MAX_NAS_MESSAGES_FOR_PS_DOMAIN)
                {
                  rrcudt_ps_saved_data[count_index].mui_status = TRUE;
                  rrcudt_ps_saved_data[count_index].mui = mui;
                }
                break;
              
              default :
                ERR_FATAL("Invalid Domain ID %d", 
                           save_temp_ptr->cn_domain_id, 0, 0);
                break;
            }
  
            WRRC_MSG0_HIGH("Sent Uplink Direct Transfer Message");  
            rrcudt_substate = RRCUDT_WAIT_FOR_L2ACK;
          }
          else
          {
            WRRC_MSG1_ERROR("Error %d sending Uplink Direct Xfer Msg",
                      send_status);
           
            rrcudt_delete_all_saved_data_req(RRC_CS_DOMAIN_CN_ID);
            rrcudt_delete_all_saved_data_req(RRC_PS_DOMAIN_CN_ID);
          
            rrcudt_substate = RRCUDT_INITIAL;
  
          }
          /*set the flag to false.  no need to call delete_data_req as nas_msg.value is
           *already deleted in rrcsend_send_ul_sdu
           */
          *(save_data_req_status_ptr) = FALSE;
        }            
      }
    } /* end for all messages */
  } /* end for CN domains */
}/* rrcudt_send_uplink_direct_transfer_msg */

/*===========================================================================

FUNCTION rrcudt_re_send_uplink_direct_transfer_msg

DESCRIPTION
  This function sends the uplink direct transfer on the uplink DCCH, for
  messages for which we got L2-ACK failure due to re-establishment

DEPENDENCIES
  None.

RETURN VALUE
  void.

===========================================================================*/
static uecomdef_status_e_type rrcudt_re_send_uplink_direct_transfer_msg
(
 uint8                       retransmit_mui,
 rrc_nas_msg_priority_e_type retransmit_msg_priority
)
{
  uint8 count_index;                    /* Count for saved messages */
  rrc_UL_DCCH_Message *msg_ptr;         /* Pointer to uplink DCCH message */ 
  
  rlc_lc_id_type ul_lc_id;              /* Uplink logical channel ID */

  rrcsend_status_e_type send_status;    /* send status of UL message */

  uint8 mui;                            /* Local varibale for MUI */

  rrc_data_req_type *save_temp_ptr=NULL;
  boolean           *save_data_req_status_ptr=NULL;
  boolean           msg_found = FALSE;  

  WRRC_MSG2_HIGH("Attempt to resend UDT for MUI %d, NAS-MSG-Priority %d", 
           retransmit_mui,  
           retransmit_msg_priority);

  //loop thru cs/ps domain saved messages and try and find the message with mui/nas-priority
  //that matches the mui for which we are doing a l2-re-transmission.Also check the mui status
  for ( count_index = 0; count_index < MAX_NAS_MESSAGES_FOR_CS_DOMAIN; count_index++)
  {
    if( (rrcudt_cs_saved_data[count_index].mui == retransmit_mui) &&
        (rrcudt_cs_saved_data[count_index].data_req.priority == retransmit_msg_priority) &&
        (rrcudt_cs_saved_data[count_index].mui_status == TRUE)
        )
    {
      //force data_req_status to true bcos we want to resend this again
      rrcudt_cs_saved_data[count_index].data_req_status = TRUE;
      save_data_req_status_ptr = 
        &rrcudt_cs_saved_data[count_index].data_req_status;
      save_temp_ptr = &rrcudt_cs_saved_data[count_index].data_req;
      msg_found = TRUE;
      break;
    }
  }
  
  if(msg_found != TRUE)
  {
    for ( count_index = 0; count_index < MAX_NAS_MESSAGES_FOR_PS_DOMAIN; count_index++)
    {
      if( (rrcudt_ps_saved_data[count_index].mui == retransmit_mui) &&
          (rrcudt_ps_saved_data[count_index].data_req.priority == retransmit_msg_priority) &&
          (rrcudt_ps_saved_data[count_index].mui_status == TRUE)
          )
      {
        //force data_req_status to true bcos we want to resend this again
        rrcudt_ps_saved_data[count_index].data_req_status = TRUE;
        save_data_req_status_ptr = 
          &rrcudt_ps_saved_data[count_index].data_req_status;
        save_temp_ptr = &rrcudt_ps_saved_data[count_index].data_req;
        msg_found = TRUE;
        break;
      }
    }
  }

  //if no mui for which we failed is found,  return error and don't process any further
  if(msg_found == FALSE)
  {
    WRRC_MSG2_ERROR("Did not find mui %d for nas-priority %d", retransmit_mui, retransmit_msg_priority);
    return FAILURE;
  }
  else
  {
    if(NULL == save_data_req_status_ptr || NULL == save_temp_ptr)
    {
      WRRC_MSG2_ERROR("mui %d for nas-priority %d msg-ptr null", retransmit_mui, retransmit_msg_priority);
      return FAILURE;
    }

    else if( *(save_data_req_status_ptr) == TRUE)
    {
      if((is_scrr_in_progress) && (rrcscrr_substate != RRCSCRR_INITIAL))
      {
        if((save_temp_ptr->cn_domain_id == RRC_CS_DOMAIN_CN_ID) &&
           (rrcscrr_saved_cs_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE))
        {
          WRRC_MSG0_HIGH("SCRR for CS in progress. Discard CS UDT");
          return FAILURE;
        }
        else if((save_temp_ptr->cn_domain_id == RRC_PS_DOMAIN_CN_ID) &&
           (rrcscrr_saved_ps_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE))
        {
          WRRC_MSG0_HIGH("SCRR for PS in progress. Discard PS UDT");
          return FAILURE;
        }
      }

     /* Determine the logical channel id for the Uplink DCCH */
      if(save_temp_ptr->priority == RRC_NAS_MSG_HIGH_PRIO)
      {
         ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                           DCCH_DT_HIGH_PRI_RADIO_BEARER_ID, 
                                           UE_MODE_ACKNOWLEDGED_DATA);
      }
      else
      {
         ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                           DCCH_DT_LOW_PRI_RADIO_BEARER_ID, 
                                           UE_MODE_ACKNOWLEDGED_DATA);
      }
      /*lint -save -e650 */
      if( (ul_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND) &&
          (save_temp_ptr->priority == RRC_NAS_MSG_LOW_PRIO) ) 
      {
      /* The logical channel id was not found. If we were looking for the low
       * priority radio bearer (RB 4), try the high priority radio bearer (RB 3).
       */

        ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                             DCCH_DT_HIGH_PRI_RADIO_BEARER_ID, 
                                             UE_MODE_ACKNOWLEDGED_DATA);

        save_temp_ptr->priority = RRC_NAS_MSG_HIGH_PRIO;
      }
      /*lint -restore */
      if (ul_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
      {
        WRRC_MSG0_HIGH("RBs 3 and 4 not found by LCM");

        /*delete CS and PS domain saved info as LC ID is not present    */
        rrcudt_delete_all_saved_data_req(RRC_CS_DOMAIN_CN_ID);
        rrcudt_delete_all_saved_data_req(RRC_PS_DOMAIN_CN_ID);

        /* Reset the state machine */
        rrcudt_substate = RRCUDT_INITIAL;

        return FAILURE;
      }
      else
      {
      /* Build an Uplink Direct Transfer Message. Start by allocating an Uplink 
       * DCCH message. This memory is released by rrcsend_send_ul_sdu().
       */
        msg_ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
          sizeof(struct rrc_UL_DCCH_Message));

       /* Identify this message as an Uplink Direct Transfer Message
        */
        RRCSET_MSG_TYPE(msg_ptr->message,
               T_rrc_UL_DCCH_MessageType_uplinkDirectTransfer);

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.uplinkDirectTransfer);

        if(rrc_get_state() ==  RRC_STATE_CELL_FACH)
        {
          /* Function call to Measurement Control & Report procedure. */
          if(RRCMEAS_RACH_SUCCESSFUL == rrcmeas_append_meas_rpt_on_rach(
             &msg_ptr->message.u.uplinkDirectTransfer.measuredResultsOnRACH, RRC_PROCEDURE_UDT, FALSE))
          {
          /* if Measurement Control & Reportprocedure returns RRC_MEAS_RACH_SUCCESFUL,
             then enable the  bit_mask field */
            RRCSET_R3_MSG_IE_PRESENT(msg_ptr->message.u.uplinkDirectTransfer,
              rrc_UplinkDirectTransfer_,
              measuredResultsOnRACH);
          }
          else
          {
            MSG_MED("RACH meas is not appended",0,0,0);
          }
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
          #error code not present
#endif /*FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING*/

        }


        /* Copy the contents of Saved RRC_DATA_REQ 
        */    
        WCDMA_MEMCPY(msg_ptr->message.u.uplinkDirectTransfer.nas_Message.data,
                     sizeof(OSOCTET) * 4095,
                     save_temp_ptr->nas_msg.value, 
                     save_temp_ptr->nas_msg.length);

        msg_ptr->message.u.uplinkDirectTransfer.nas_Message.numocts = 
          save_temp_ptr->nas_msg.length;

       /* Fill in the CN Domain id given by the command 
        */

        if(save_temp_ptr->cn_domain_id == RRC_CS_DOMAIN_CN_ID)
        {
          msg_ptr->message.u.uplinkDirectTransfer.cn_DomainIdentity = 
          rrc_CN_DomainIdentity_cs_domain;
        }
        else
        {
          msg_ptr->message.u.uplinkDirectTransfer.cn_DomainIdentity =
            rrc_CN_DomainIdentity_ps_domain;
        }

       /* Send the Uplink Direct Transfer Message on the Uplink DCCH
        */
        send_status = rrcsend_send_ul_sdu_with_mui(RRC_PROCEDURE_UDT,
                                          msg_ptr,
                                          ul_lc_id,
                                          RRCSEND_L2ACK_REQUIRED,
                                          &mui
                                          );

        if (send_status == RRCSEND_SUCCESS)
        {
          switch(save_temp_ptr->cn_domain_id)
          {
            case RRC_CS_DOMAIN_CN_ID :
              /* Check array bounds to keep lint happy */ 
              if(count_index < MAX_NAS_MESSAGES_FOR_CS_DOMAIN)
              {
                rrcudt_cs_saved_data[count_index].mui_status = TRUE;
                rrcudt_cs_saved_data[count_index].mui = mui;
#ifdef FEATURE_WCDMA_MTPL_THERMAL_BACKOFF 
                if(TRUE == save_temp_ptr->is_emergency_call)
                {
                  rrcrce_set_rrc_est_cause_for_emergency_call();
                }								
#endif
              }
              else
              {
                WRRC_MSG1_ERROR("Invalid array index %d in rrcudt_cs_saved_data",count_index);
              }
              break;

            case RRC_PS_DOMAIN_CN_ID :
#ifdef FEATURE_INACTIVITY_HANDLING
                if (rrc_get_state() == RRC_STATE_CELL_DCH) 
                {
                  WRRC_MSG0_HIGH("Restarting inact timer due to UDT");
                  rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, INACTIVITY_TIMER_IN_MS);
                }
                else if (rrc_get_state() == RRC_STATE_CELL_FACH)
                {
                  WRRC_MSG2_HIGH("Restarting Inactivity timer %d + remain T305 %d",INACTIVITY_TIMER_IN_MS,
                           rrctmr_get_remaining_time(RRCTMR_T_305_TIMER));
                  /* Restart incativity handling timer */
                  if(rrc_convert_t305_to_ms(RRC_GET_T305()) != RRCTMR_INFINITY_VAL )
                  {
                    rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                                      (unsigned long)(INACTIVITY_TIMER_IN_MS + rrctmr_get_remaining_time(RRCTMR_T_305_TIMER))); 
                  }
                  else
                  {
                    /*If T305 timer was infinity then the timer wouldnt have been started . In thats case start
                        timer of 5 mins plus 24 hrs*/
                    rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                                      (unsigned long)(INACTIVITY_TIMER_IN_MS + RRCTMR_INFINITY_VAL)); 
                  }
                }
                (void)rrcmisc_handle_ps_rab_inactivity(1);
#endif /* FEATURE_INACTIVITY_HANDLING */
              /* Check array bounds to keep lint happy */ 
              if(count_index < MAX_NAS_MESSAGES_FOR_PS_DOMAIN)
              {
                rrcudt_ps_saved_data[count_index].mui_status = TRUE;
                rrcudt_ps_saved_data[count_index].mui = mui;
              }
              else
              {
                WRRC_MSG1_ERROR("Invalid array index %d in rrcudt_ps_saved_data",count_index);
              }
              break;

            default :
              ERR_FATAL("Invalid Domain ID %d", 
                         save_temp_ptr->cn_domain_id, 0, 0);
              break;
          }

          WRRC_MSG0_HIGH("Sent Uplink Direct Transfer Message");  
          rrcudt_substate = RRCUDT_WAIT_FOR_L2ACK;
        }
        else
        {
          WRRC_MSG1_ERROR("Error %d sending Uplink Direct Xfer Msg",
                    send_status);

          rrcudt_delete_all_saved_data_req(RRC_CS_DOMAIN_CN_ID);
          rrcudt_delete_all_saved_data_req(RRC_PS_DOMAIN_CN_ID);

          rrcudt_substate = RRCUDT_INITIAL;

        }
        /*set the flag to false.  no need to call delete_data_req as nas_msg.value is
         *already deleted in rrcsend_send_ul_sdu
         */
        *(save_data_req_status_ptr) = FALSE;
      }            
    }
  }

  //all error handling done before and failure returned.. return success if we are here
  return SUCCESS;
}/* rrcudt_send_uplink_direct_transfer_msg */
#ifdef FEATURE_NAS_UDT_OOS
#error code not present
#endif
/*===========================================================================

FUNCTION rrcudt_save_g2w_nas_data_ind

DESCRIPTION
  This function saves the NAS Data Req from the other RAT in case of a HO message
  in the static variable saved_data_req for CS domain

DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type.

===========================================================================*/
static rrcdt_status_e_type rrcudt_save_g2w_nas_data_ind
(
  rrc_rr_cmd_type *cmd_ptr                          /* Pointer to RR Command */
)
{
  unsigned char       *payload;                     /* Pointer to the MM payload */
  uint8               index;                        /* Local variable for index */

  /*domain id is hard-coded as we can receive this only in
    CS domain for a GSM to WCDMA handover*/
  if(rrcudt_check_if_msg_status_can_be_saved(RRC_CS_DOMAIN_CN_ID, &index ) != SUCCESS)
  {
    WRRC_MSG0_HIGH("Can't process NAS DATA in CS domain");
    return RRCDT_FAILURE_MINOR;  
  }

  rrcudt_cs_saved_data[index].data_req.cn_domain_id = RRC_CS_DOMAIN_CN_ID;
      

  /* Send the  priority */
  if(cmd_ptr->cmd.interrat_nas_data_ind.SAPI == 0)
  {
  
    rrcudt_cs_saved_data[index].data_req.priority = 
      RRC_NAS_MSG_HIGH_PRIO;
  }
  else if (cmd_ptr->cmd.interrat_nas_data_ind.SAPI == 3)
  {
    rrcudt_cs_saved_data[index].data_req.priority = 
      RRC_NAS_MSG_LOW_PRIO;
  }
  else
  {
    ERR_FATAL("Invalid SAPI is received",0,0,0);
  }
    
  /* Allocate a buffer for the payload. This memory is released by 
   * rrcsend_send_ul_sdu().
   */
  payload = rrc_malloc(cmd_ptr->cmd.interrat_nas_data_ind.l3_message_length);
    
  /* Copy the payload from the MM primitive into the newly allocated memory
   */
  WCDMA_MEMCPY(payload, 
               cmd_ptr->cmd.interrat_nas_data_ind.l3_message_length,
               cmd_ptr->cmd.interrat_nas_data_ind.l3_message,
               cmd_ptr->cmd.interrat_nas_data_ind.l3_message_length);
    
  /* Set the pointer to the payload and the length of the payload in the 
   * Uplink Direct Transfer Message
   */
  rrcudt_cs_saved_data[index].data_req.nas_msg.value = payload;
  rrcudt_cs_saved_data[index].data_req.nas_msg.length = 
    (unsigned short) cmd_ptr->cmd.interrat_nas_data_ind.l3_message_length;
    
  /* set the flag to indicate that DATA_REQ cmd contents are saved and 
   * return success status
   */
  rrcudt_cs_saved_data[index].data_req_status = TRUE;
  
  return RRCDT_SUCCESS;
} /*rrcudt_save_g2w_nas_data_ind*/
/*===========================================================================

FUNCTION rrcudt_process_g2w_nas_data_ind

DESCRIPTION
  This function validates, saves and sends the nas data indication

DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type.

===========================================================================*/
static rrcdt_status_e_type rrcudt_process_g2w_nas_data_ind
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to RRC Command */
)
{
  rrc_rr_cmd_type * nas_msg_ptr;        /* ptr to nas msg typecast   */

  mm_cmd_type *mm_cmd_ptr;              /* mm cmd ptr holder         */

  /*typecast the message to rr type*/
  /*lint -e740*/
  nas_msg_ptr = (rrc_rr_cmd_type *) cmd_ptr;
  /*lint +e740*/
  
  /*set cs domain to open as we have successfully moved to DCH mode due to
    an GtoW Interrat Handover */
  rrcdt_int_data.cs_domain_open = TRUE;
    
  /*if there are no mm messages to send at this time, then send activation ind
  to nas*/
  if(nas_msg_ptr->cmd.interrat_nas_data_ind.l3_message_length == 0)
  {
  /* Get a buffer from MM task */
    mm_cmd_ptr = mm_rrc_get_cmd_buf(RRC_ACTIVATION_IND);

    mm_cmd_ptr->cmd.rrc_activation_ind.status = MM_AS_INTERRAT_HANDOVER;

#ifdef FEATURE_DUAL_SIM
    mm_cmd_ptr->cmd.rrc_activation_ind.as_id = rrc_get_as_id();
#endif

    /* Put the command on MM queue */
    rrc_send_rrc_message_to_mm(mm_cmd_ptr);
    WRRC_MSG0_HIGH("Dispatching out MM cmd / RRC_ACTIVATION_IND sent to MM");

    /*update the cause for activation indication*/
    rrcmcm_update_wcdma_activation_cause(RRCMCM_INTERRAT_HANDOVER_GTOW);

    return RRCDT_SUCCESS; 
  }

  /*check to see if state is CELL_DCH or not*/
  if(rrc_get_state() != RRC_STATE_CELL_DCH)
  {
    WRRC_MSG0_ERROR("NAS_DATA_IND valid only in DCH");
    return RRCDT_FAILURE_MINOR;
  }
    
  if(RRCDT_SUCCESS != rrcudt_save_g2w_nas_data_ind(nas_msg_ptr))
  {
    /* save udt message */
    WRRC_MSG2_HIGH("Failed to Save NAS Data Ind in : %d, %d",
             rrc_get_state(),rrcudt_substate);
    return RRCDT_FAILURE_MINOR;
  }    
  
  /* Build and send the Uplink Direct Transfer to Send Chain */
  rrcudt_send_uplink_direct_transfer_msg();
  
  return RRCDT_SUCCESS;          
}/*rrcudt_process_g2w_nas_data_ind*/


#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION rrcudt_process_open_domain_ind_from_iho

DESCRIPTION
  This function processes open domain indication from IHO and sends
  activation indication to NAS.

DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type.

===========================================================================*/
static void rrcudt_process_open_domain_ind_from_iho
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to RRC Command */
)
{
  mm_cmd_type *mm_cmd_ptr;              /* mm cmd ptr holder         */

  if(cmd_ptr->cmd.rrc_open_domain_ind.domain == RRC_CS_DOMAIN)
  {
    /*set cs domain to open as we have successfully moved to DCH mode due to
      an LtoW SRVCC */
    rrcdt_int_data.cs_domain_open = TRUE;
  }
  else if(cmd_ptr->cmd.rrc_open_domain_ind.domain == RRC_PS_DOMAIN)
  {
  /*set ps domain to open as we have successfully moved to DCH mode due to
    an LtoW Interrat PS Handover */
  rrcdt_int_data.ps_domain_open = TRUE;
  }
  else if(cmd_ptr->cmd.rrc_open_domain_ind.domain == RRC_CS_PLUS_PS_DOMAIN)
  {
    /*set cs and ps domain to open as we have successfully moved to DCH mode due to
      an LtoW SRVCC */
    rrcdt_int_data.cs_domain_open = TRUE;
    rrcdt_int_data.ps_domain_open = TRUE;
  }
    
  /* Get a buffer from MM task */
  mm_cmd_ptr = mm_rrc_get_cmd_buf(RRC_ACTIVATION_IND);

  if(cmd_ptr->cmd.rrc_open_domain_ind.domain == RRC_CS_PLUS_PS_DOMAIN)
  {
    mm_cmd_ptr->cmd.rrc_activation_ind.status = MM_AS_LTOW_SRVCC_CS_PS_HANDOVER;
  }
  else if(cmd_ptr->cmd.rrc_open_domain_ind.domain == RRC_CS_DOMAIN)
  {
    mm_cmd_ptr->cmd.rrc_activation_ind.status = MM_AS_LTOW_SRVCC_CS_HANDOVER;
  }
  else
  {
  mm_cmd_ptr->cmd.rrc_activation_ind.status = MM_AS_INTERRAT_PS_HANDOVER;
  }
  
  #ifdef FEATURE_DUAL_SIM
  mm_cmd_ptr->cmd.rrc_activation_ind.as_id = rrc_get_as_id();
  #endif
  
  /* Put the command on MM queue */
  rrc_send_rrc_message_to_mm(mm_cmd_ptr);
  
  WRRC_MSG0_HIGH("Dispatching out MM cmd / RRC_ACTIVATION_IND sent to MM");
  
  /*update the cause for activation indication*/
  rrcmcm_update_wcdma_activation_cause(RRCMCM_INTERRAT_HANDOVER_GTOW);
  }
#endif

/*===========================================================================

FUNCTION rrcudt_send_nas_data_ind

DESCRIPTION
  This function sends RR_NAS_DATA_IND if there is any NAS message pending
  to transmitted or pending to be acknowledged from UTRAN.


DEPENDENCIES
  None.

RETURN VALUE
  void.

===========================================================================*/
static void rrcudt_send_nas_data_ind
(
 void
)
{
  uint8 count;                          /* Local variable to store index */
  rr_nas_data_ind_type nas_data_ind;    /* Local variable to store
                                           NAS message to be sent to RR */
  switch(rrcudt_substate)
  {
    case RRCUDT_INITIAL:
      /* No Pending NAS message */
      WRRC_MSG0_HIGH("No Pending UDT messages for RR_NAS_DATA_IND, sent RR_NAS_DATA_IND terminator (0 message length)");

      /* Set the message length to 0 */
      nas_data_ind.nas_data.message_length = 0;
  
      /* Send the message to RR */
      rr_send_rrc_message_to_rr_internal((void *)(&nas_data_ind),
                                 RR_NAS_DATA_IND,
                                 sizeof(rr_nas_data_ind_type));
      
      break;

    case RRCUDT_WAIT_FOR_CELL_UPDATE_COMP_CNF:                               
    case RRCUDT_WAIT_FOR_L2ACK:
      if(SUCCESS == rrcudt_check_saved_message_status() )
      {
        /* Copy the saved NAS message for CS */
        for (count = 0; count < MAX_NAS_MESSAGES_FOR_CS_DOMAIN; count++ )
        {
          /* Send the saved message to RR only if it is active and 
             is not of zero length */
          if(((rrcudt_cs_saved_data[count].mui_status == TRUE) ||
              (rrcudt_cs_saved_data[count].data_req_status == TRUE)) &&
              (rrcudt_cs_saved_data[count].data_req.nas_msg.length != 0))
          {
            if (rrcudt_cs_saved_data[count].data_req.nas_msg.length > 255)
            {
              ERR_FATAL("Len of NAS msg > 255",0,0,0);
            }

            nas_data_ind.nas_data.message_length = (uint8)
                rrcudt_cs_saved_data[count].data_req.nas_msg.length;

            WCDMA_MEMCPY(nas_data_ind.nas_data.payload, 
                         sizeof(uint8) * RRC_RR_MAX_GSM_MESSAGE_LENGTH,
                         rrcudt_cs_saved_data[count].data_req.nas_msg.value,
                         rrcudt_cs_saved_data[count].data_req.nas_msg.length );

            /* Send the message to RR */
            rr_send_rrc_message_to_rr_internal((void *)(&nas_data_ind),
                                       RR_NAS_DATA_IND,
                                       sizeof(rr_nas_data_ind_type));

            WRRC_MSG1_HIGH("Sent RR_NAS_DATA_IND message length: %d",
                      nas_data_ind.nas_data.message_length);
          } 
        }

        /* Copy the saved NAS message for PS */
        for (count = 0; count < MAX_NAS_MESSAGES_FOR_PS_DOMAIN; count++ )
        {
          /* Send the saved message to RR only if it is active and 
             is not of zero length */
          if(((rrcudt_ps_saved_data[count].mui_status == TRUE) ||
              (rrcudt_ps_saved_data[count].data_req_status == TRUE)) &&
              (rrcudt_ps_saved_data[count].data_req.nas_msg.length != 0))
          {
            if(rrcudt_ps_saved_data[count].data_req.nas_msg.length > 255)
            {
              ERR_FATAL("Len of NA msg > 255",0,0,0);
            }

            nas_data_ind.nas_data.message_length = (uint8)
                rrcudt_ps_saved_data[count].data_req.nas_msg.length;

            WCDMA_MEMCPY(nas_data_ind.nas_data.payload, 
                         sizeof(uint8) * RRC_RR_MAX_GSM_MESSAGE_LENGTH,
                         rrcudt_ps_saved_data[count].data_req.nas_msg.value,
                         rrcudt_ps_saved_data[count].data_req.nas_msg.length );

            /* Send the message to RR */
            rr_send_rrc_message_to_rr_internal((void *)(&nas_data_ind),
                                       RR_NAS_DATA_IND,
                                       sizeof(rr_nas_data_ind_type));

            WRRC_MSG1_HIGH("Sent RR_NAS_DATA_IND message length: %d",
                      nas_data_ind.nas_data.message_length);
          }
        }

        /* Since we have sent all the saved messages to RR, clear the arrays */
        rrcudt_delete_all_saved_data_req(RRC_CS_DOMAIN_CN_ID);
        rrcudt_delete_all_saved_data_req(RRC_PS_DOMAIN_CN_ID);
        rrcudt_substate = RRCUDT_INITIAL;
      }
      
      /* The last message always has to be NULL message which will act as the 
         terminator for NAS_DATA_IND. Set the message length to 0 */
      nas_data_ind.nas_data.message_length = 0;

      /* Send the message to RR */
      rr_send_rrc_message_to_rr_internal((void *)(&nas_data_ind),
                                RR_NAS_DATA_IND,
                                sizeof(rr_nas_data_ind_type));

      WRRC_MSG0_HIGH("Sent RR_NAS_DATA_IND terminator(0 message length)");
      break;

    default:  
      ERR_FATAL("Invalid RRCUDT substate: %d ",rrcudt_substate,0,0);
      break;
   }   
}/* rrcudt_send_nas_data_ind */
/*===========================================================================

FUNCTION rrcudt_process_udt_initial_substate

DESCRIPTION
  This function processes all events that are  dispatched
  to Uplink Direct Transfer procedure in RRCUDT_INITIAL substate

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcudt_process_udt_initial_substate
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to RRC Command */
)
{
   if((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_NAS_DATA_IND)
   {
      WRRC_MSG0_HIGH("Received RRC_NAS_DATA_IND");
      (void)rrcudt_process_g2w_nas_data_ind(cmd_ptr);
   }
   else
   {
    switch(cmd_ptr->cmd_hdr.cmd_id)
    {
    /* happens when a state change ind is received in idle state.
     * do nothing when it happens
     */
    case RRC_DATA_REQ:
      WRRC_MSG0_HIGH("Received RRC_DATA_REQ");
  
      if(RRCDT_SUCCESS != rrcudt_validate_data_req(cmd_ptr))
      {
        /* If RRC_DATA_REQ is invalid primitive, then trash the same */
        WRRC_MSG0_HIGH("Validation of RRC_DATA_REQ cmd Failed");
        return;
      }
    
      if(RRCDT_SUCCESS != rrcudt_save_data_req(cmd_ptr))
      {
        /* save udt message */
        /*lint -e666 */
        WRRC_MSG2_HIGH("Failed to Save Data Req in : %d, %d",
                 rrc_get_state(),rrcudt_substate);
        /*lint +e666 */
        return;
      }
    
      /* Check RRC state and call functions appropriately.
       */
      switch(rrc_get_state())
      {
        case RRC_STATE_URA_PCH:
        case RRC_STATE_CELL_PCH:
        case RRC_STATE_CELL_FACH:
        case RRC_STATE_CELL_DCH:
          //This queries whether a CELL Update procedure is active.If Cell Update
          //is active, it registers for the complete and enters the substate
          //RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF. 
          if(RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status(RRC_PROCEDURE_UDT) )
          {
            if(is_ue_oos_and_em_call_attempted(cmd_ptr, RRC_PROCEDURE_UDT))
            {
              rrcudt_substate = RRCUDT_INITIAL;  
            }
            else
            {
              /* Wait until Cell update procedure is completed */
              rrcudt_substate = RRCUDT_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
            }
          }
          else /* Cell Update procedure is inactive */
          {
            /* Build and send the Uplink Direct Transfer to Send Chain */
            rrcudt_send_uplink_direct_transfer_msg();
          }
          break;
    
        default:
          WRRC_MSG2_HIGH("UDT:Invalid Event: %d in the substate: %d", 
                   cmd_ptr->cmd_hdr.cmd_id,rrcudt_substate);
          break;
      }
      break;
    case RRC_NAS_DATA_TRANSFER_IND:
      rrcudt_send_nas_data_ind();
      break;

    /*lint -e408*/  

#ifdef FEATURE_LTE_TO_WCDMA
    case RRC_IHO_OPEN_DOMAIN_IND:
      WRRC_MSG0_HIGH("Received RRC_IHO_OPEN_DOMAIN_IND");
      rrcudt_process_open_domain_ind_from_iho(cmd_ptr);
      break;
#endif /*FEATURE_LTE_TO_WCDMA */

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d for UDT in substate:%d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcudt_substate);
      break;
   }/*end of switch stmt*/
  }
} /* rrcudt_process_udt_initial_substate */

/*===========================================================================

FUNCTION rrcudt_process_udt_wait_for_cell_update_comp_cnf_substate

DESCRIPTION
  This function processes all events that are  dispatched
  to Uplink Direct Transfer procedure in RRCUDT_WAIT_FOR_CELL_UPDATE_COMP_CNF
  substate

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcudt_process_udt_wait_for_cell_update_comp_cnf_substate
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_CELL_UPDATE_COMPLETE_CNF:
      /* Build and send the Uplink Direct Transfer to Send Chain */
      rrcudt_send_uplink_direct_transfer_msg();

      break;

    case RRC_DATA_REQ:
      if(is_ue_oos_and_em_call_attempted(cmd_ptr, RRC_PROCEDURE_UDT))
      {
        rrcudt_substate = RRCUDT_INITIAL;  
      }
      else
      {
        /*we may have got a save req for a different CN domain.  try
         *to save the data_req command*/
        (void) rrcudt_save_data_req( cmd_ptr );
      }
      break;

    case RRC_NAS_DATA_TRANSFER_IND:
      /*Copy the saved NAS message */
      rrcudt_send_nas_data_ind();
      /* Reset the state machine */
      rrcudt_substate = RRCUDT_INITIAL;  
      break;
    
    default:
      WRRC_MSG2_HIGH("UDT:Invalid Event: %d in substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcudt_substate);
      break;
  }
 
} /* rrcudt_process_udt_wait_for_cell_update_comp_cnf_substate */
/*===========================================================================

FUNCTION rrcudt_process_udt_wait_for_l2ack_substate

DESCRIPTION
  This function processes all events that are  dispatched
  to Initial Direct Transfer procedure in RRCUDT_WAIT_FOR_L2ACK
  substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcudt_process_udt_wait_for_l2ack_substate
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrc_RB_Identity rb_id;                /* Local variable to store RB Id */

  rrc_nas_msg_priority_e_type nas_msg_priority;

   if((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_NAS_DATA_IND)
   {
     WRRC_MSG0_HIGH("Received RRC_NAS_DATA_IND");
      
      /*process the nas data indication*/
      (void)rrcudt_process_g2w_nas_data_ind(cmd_ptr);
   }
  else
  {
   switch(cmd_ptr->cmd_hdr.cmd_id)
   {
    case RRC_DOWNLINK_L2ACK_IND:
      WRRC_MSG1_HIGH("L2 ACK for UL DirecT xfer Mesg Rcvd.Status: %d",
                 cmd_ptr->cmd.downlink_l2ack_ind.status);

      /* Check for the MUI received */
      rb_id = rrclcm_get_ul_rb_id(cmd_ptr->cmd.downlink_l2ack_ind.lc_id);

      if((rb_id < DCCH_DT_HIGH_PRI_RADIO_BEARER_ID) || 
         (rb_id > DCCH_DT_LOW_PRI_RADIO_BEARER_ID))
      {
        ERR_FATAL("L2 ACK routing is incorrect",0,0,0);
      }

      if(rb_id == DCCH_DT_HIGH_PRI_RADIO_BEARER_ID)
      {
        nas_msg_priority = RRC_NAS_MSG_HIGH_PRIO; 
      }
      else
      {
        nas_msg_priority = RRC_NAS_MSG_LOW_PRIO;
      }

      if(cmd_ptr->cmd.downlink_l2ack_ind.status == SUCCESS)
      {
        if(SUCCESS == rrcudt_delete_saved_data_req((rrc_nas_msg_priority_e_type) nas_msg_priority,
                                                    cmd_ptr->cmd.downlink_l2ack_ind.mui))
        {

#ifdef FEATURE_WCDMA_UL_COMPR
          rrc_ul_comp_cap_support = FALSE;
#endif
          if(FAILURE == rrcudt_check_saved_mui_status())
          {
            rrcudt_substate = RRCUDT_INITIAL;
          }
          else
          {
            rrcudt_substate = RRCUDT_WAIT_FOR_L2ACK;
          }
        }
        else
        {
          WRRC_MSG0_ERROR("Unexpected L2 ACK rcvd");
          /* Do nothing */
        }    
      }
      else /* L2 Failed to transmit the message */
      {
        WRRC_MSG1_HIGH("L2 Ack failed for UDT.Failure reason %d",
                    cmd_ptr->cmd.downlink_l2ack_ind.failure_reason);
         
        /* Before re-transmitting the message,check to see if we are waiting for
        L2 Ack for any other UDT message(s) of the same domain.If so,it might 
        be still  sitting in the WM and may go out before this re-trasmitted 
        message and hence the order of delivery would be reversed.This is not 
        desirable,hence purge the WM so that serialization is maintained */

        /* Do not purge WM if the L2 Ack failure is because of WM purging.This 
           would result in an infinite loop */  
        if(cmd_ptr->cmd.downlink_l2ack_ind.failure_reason != WM_PURGE_REQ)
        {
          uint32 count = 0;
          uint32 pending_count = 0;
          rrc_cn_domain_identity_e_type domain_id;

          /* Get the domain id of the message */
          domain_id = rrcudt_get_cn_domain_for_mui(cmd_ptr->cmd.downlink_l2ack_ind.mui);
  
          if(domain_id == RRC_CS_DOMAIN_CN_ID)
          {
            for (count = 0;count < MAX_NAS_MESSAGES_FOR_CS_DOMAIN;count++ )
            {
              if(rrcudt_cs_saved_data[count].mui_status == TRUE)
              {
                pending_count++; 
              }
            }
          }
          else /* PS domain */
          {
            for (count = 0;count < MAX_NAS_MESSAGES_FOR_PS_DOMAIN;count++ )
            {
              if(rrcudt_ps_saved_data[count].mui_status == TRUE)
              {
                pending_count++; 
              }
            }
          }
  
          /* Check to see if pending count is greater than 1.i.e. another message
             is waiting for a L2 ack in the same domain apart from the one that just
             got the L2 Ack(failure) */
          if(pending_count > 1)
          {
            WRRC_MSG0_HIGH("Need to purge WM3 due to failed L2 Ack");
            rrcdt_purge_rlc_wm_rb3_rb4(TRUE,FALSE,RRC_PROCEDURE_UDT);
          }
        }
        /* Re-transmission of the message */
        if(FAILURE == rrcudt_re_send_uplink_direct_transfer_msg(cmd_ptr->cmd.downlink_l2ack_ind.mui, 
                                                                nas_msg_priority))
        {
          WRRC_MSG0_ERROR("Retransmission attempt failed");
          (void)rrcudt_delete_saved_data_req((rrc_nas_msg_priority_e_type) nas_msg_priority,
                                              cmd_ptr->cmd.downlink_l2ack_ind.mui);
        }
      }
      break;

    case RRC_DATA_REQ:
      WRRC_MSG0_HIGH("Received RRC_DATA_REQ");
      
      if(RRCDT_SUCCESS != rrcudt_validate_data_req(cmd_ptr))
      {
        /* If RRC_DATA_REQ is invalid primitive, then trash the same */
        WRRC_MSG0_HIGH("Validation of RRC_DATA_REQ cmd Failed");
        return;
      }
      
      if(RRCDT_SUCCESS != rrcudt_save_data_req(cmd_ptr))
      {
        /* save udt message */
        /*lint -e666*/
        WRRC_MSG2_HIGH("Failed to Save Data Req in : %d, %d",
                 rrc_get_state(),rrcudt_substate);
        /*lint +e666*/
        return;
      }
      
      /* Check RRC state and call functions appropriately.
       */
      switch(rrc_get_state())
      {
        case RRC_STATE_CELL_FACH:
          //This queries whether a CELL Update procedure is active.If Cell Update
          //is active, it registers for the complete and enters the substate
          //RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF. 
    
          if(RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status(RRC_PROCEDURE_UDT) )
          {
            if(is_ue_oos_and_em_call_attempted(cmd_ptr, RRC_PROCEDURE_UDT))
            {
              rrcudt_substate = RRCUDT_INITIAL;  
            }
            else
            {
              /* Wait until Cell update procedure is completed */
              rrcudt_substate = RRCUDT_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
            }
          }
          else /* Cell Update procedure is inactive */
          {
            /* Build and send the Uplink Direct Transfer to Send Chain */
            rrcudt_send_uplink_direct_transfer_msg();
          }
          break;

        case RRC_STATE_CELL_DCH:
          /* Build and send the Uplink Direct Transfer to Send Chain */
          rrcudt_send_uplink_direct_transfer_msg();
          break;

        /* RRCDT should save RRC_DATA_REQ received in CELL_PCH/URA_PCH states */
        default:
          WRRC_MSG0_HIGH("RRC_DATA_REQ received in CELL_PCH/URA_PCH is saved");
          break;
      }
      break;
    case RRC_NAS_DATA_TRANSFER_IND:
      /*Copy the saved NAS message */
      rrcudt_send_nas_data_ind();
      /* Reset the state machine */
      rrcudt_substate = RRCUDT_INITIAL;  
      break;

    /*lint -e408*/    
      
      

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d for IDT in substate:%d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcudt_substate);
      break;
   }
  }
} /* rrcudt_process_udt_wait_for_l2ack_substate */

/*===========================================================================

FUNCTION rrcudt_process_disconnected_state

DESCRIPTION
  This function processes all events that are dispatched to Uplink direct
  Transfer procedure in Disconnected state of Idle mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcudt_process_disconnected_state
(
void
)
{
  switch (rrcudt_substate)
  {
    case RRCUDT_INITIAL:
      WRRC_MSG0_HIGH("Processing UDT in disconnected state. Force iitialise RCE and SIB modules");
      rrcrce_force_initialize(TRUE);
      rrcsib_force_sib_event_init(RRCSIB_ABORT_SIB7_UPDATE);
      break;

    default:           
      WRRC_MSG1_HIGH("UDT:Invalid substate: %d in DISCONNECTED", rrcudt_substate);
      break;
  }  
}/* rrcudt_process_disconnected_state */

/*===========================================================================

FUNCTION rrcudt_process_cell_fach_state

DESCRIPTION
  This function processes all events that are dispatched to Uplink direct
  Transfer procedure in CELL_FACH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcudt_process_cell_fach_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcudt_substate)
  {
    case RRCUDT_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_DATA_REQ:
        case RRC_NAS_DATA_TRANSFER_IND:
          rrcudt_process_udt_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG1_HIGH("UDT:Invalid event in Substate: %d",
                    rrcudt_substate);
          break;
      }
      break;

    case RRCUDT_WAIT_FOR_CELL_UPDATE_COMP_CNF: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        /* we can either get a complete confirmation or get a data request
         * for a different CN domain.  Process both in wait for cell update comp cnf
         */
        case RRC_CELL_UPDATE_COMPLETE_CNF:
        case RRC_DATA_REQ:
        case RRC_NAS_DATA_TRANSFER_IND:
          rrcudt_process_udt_wait_for_cell_update_comp_cnf_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("UDT:Invalid event in Substate: %d",
                    rrcudt_substate);
          break;
      }
      break;

    case RRCUDT_WAIT_FOR_L2ACK:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_DOWNLINK_L2ACK_IND:
        case RRC_DATA_REQ:
        case RRC_NAS_DATA_TRANSFER_IND:
          rrcudt_process_udt_wait_for_l2ack_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG1_HIGH("UDT:Invalid event in Substate:%d ",
                    rrcudt_substate);
          break;
      }
      break;

    default:           
      WRRC_MSG1_HIGH("UDT:Invalid substate:%d CELL_FACH",
                rrcudt_substate);
      break;
  }  
}/* rrcudt_process_cell_fach_state */
/*===========================================================================

FUNCTION rrcudt_process_cell_dch_state

DESCRIPTION
  This function processes all events that are dispatched to Initial direct
  Transfer procedure in CELL_DCH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcudt_process_cell_dch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcudt_substate)
  {
    case RRCUDT_INITIAL:
      if((cmd_ptr->cmd_hdr.cmd_id == RRC_DATA_REQ)
         || (cmd_ptr->cmd_hdr.cmd_id == RRC_NAS_DATA_TRANSFER_IND)
         || ((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_NAS_DATA_IND)
         #ifdef FEATURE_LTE_TO_WCDMA
         || (cmd_ptr->cmd_hdr.cmd_id == RRC_IHO_OPEN_DOMAIN_IND)
         #endif
         )
       {
         rrcudt_process_udt_initial_substate(cmd_ptr);
       }
       else
       {
          WRRC_MSG1_HIGH("UDT:Invalid event in Substate: %d",
                    rrcudt_substate);
       }
      break;

    case RRCUDT_WAIT_FOR_CELL_UPDATE_COMP_CNF: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        /* we can either get a complete confirmation or get a data request
         * for a different CN domain.  Process both in wait for cell update comp cnf
         */
        case RRC_CELL_UPDATE_COMPLETE_CNF:
        case RRC_DATA_REQ:
        case RRC_NAS_DATA_TRANSFER_IND:
          rrcudt_process_udt_wait_for_cell_update_comp_cnf_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("UDT:Invalid event in Substate: %d",
                    rrcudt_substate);
          break;
      }
      break;

    case RRCUDT_WAIT_FOR_L2ACK:
      if((cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND)
         || (cmd_ptr->cmd_hdr.cmd_id == RRC_DATA_REQ)
         || ((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id == RRC_NAS_DATA_IND)
         || (cmd_ptr->cmd_hdr.cmd_id == RRC_NAS_DATA_TRANSFER_IND)
         )
         {
           rrcudt_process_udt_wait_for_l2ack_substate(cmd_ptr);
         }
         else
         {
            WRRC_MSG1_HIGH("UDT:Invalid event in Substate:%d ",
                    rrcudt_substate);
         }
      break;

    default:           
      WRRC_MSG1_HIGH("UDT:Invalid substate: %d in CELL_DCH",
                rrcudt_substate);
      break;
  }  
}/* rrcudt_process_cell_dch_state */

/*===========================================================================

FUNCTION rrcudt_process_cell_ura_pch_state

DESCRIPTION
  This function processes all events that are dispatched to Initial direct
  Transfer procedure in either in CELL_PCH or URA_PCH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcudt_process_cell_ura_pch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcudt_substate)
  {
    case RRCUDT_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_DATA_REQ:
          rrcudt_process_udt_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG1_HIGH("UDT:Invalid event in Substate: %d",
                    rrcudt_substate);

          break;
      }
      break;

    case RRCUDT_WAIT_FOR_CELL_UPDATE_COMP_CNF:
      rrcudt_process_udt_wait_for_cell_update_comp_cnf_substate(cmd_ptr);
      break;

    case RRCUDT_WAIT_FOR_L2ACK:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_DATA_REQ:
          rrcudt_process_udt_wait_for_l2ack_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("UDT:Invalid event in Substate: %d",
                     rrcudt_substate);
          break;
      }
      break;

    default:           
      WRRC_MSG1_HIGH("UDT:Invalid substate: %d in PCH: %d",
                rrcudt_substate);
      break;
  }  
}/* rrcudt_process_cell_ura_pch_state */
/*===========================================================================

FUNCTION rrcudt_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the ininitial direct transfer
  procedure.
  The following events are handled by this procedure:

    RRC_DATA_REQ Primitive (received from MM)
    RRC_CELL_UPDATE_COMPLETE_CNF primitive (received from CELL UPDATE Procedure)
    RRC_NAS_DATA_TRANSFER_IND primitive from RR
    RRC_DOWNLINK_L2ACK_IND primitive from RLC
#endif

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcudt_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
 
  rrc_state_e_type rrc_state = rrc_get_state();
  WRRC_MSG1_HIGH_OPT("function rrcudt_procedure_event_handler is called in rrcudt_substate rrcudt_substate_e_type_value%d", rrcudt_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_UDT,rrcudt_substate,cmd_ptr->cmd_hdr.cmd_id);

  if(RRC_STATE_CHANGE_IND == cmd_ptr->cmd_hdr.cmd_id)
  {
    WRRC_MSG2_HIGH("State-Change-Ind udt-state %d in rrc-state %d",rrcudt_substate,rrc_state);
    //Any pending message memory needs to be de-allocated here 
    rrcudt_delete_all_saved_data_req(RRC_CS_DOMAIN_CN_ID);
    rrcudt_delete_all_saved_data_req(RRC_PS_DOMAIN_CN_ID);
    rrcudt_substate = RRCUDT_INITIAL;  

#ifdef FEATURE_WCDMA_UL_COMPR
    rrc_ul_comp_cap_support = FALSE;
#endif
  }
  else
  {
    switch (rrc_state)
    {
      case RRC_STATE_DISCONNECTED: 
        rrcudt_process_disconnected_state();
        break;

      case RRC_STATE_CELL_FACH:    
        rrcudt_process_cell_fach_state(cmd_ptr); 
        break;

      case RRC_STATE_CELL_DCH:  
        /* The only appropriate state is CELL_DCH. Later on we will have to add
         * code for the other states.
         */   
        rrcudt_process_cell_dch_state(cmd_ptr); 
        break;

      case RRC_STATE_CELL_PCH:
      case RRC_STATE_URA_PCH:
        rrcudt_process_cell_ura_pch_state(cmd_ptr);
        break;

      default:  
        WRRC_MSG1_HIGH("UDT:Event forwarded in invalid State: %d",rrc_state);
        break;
     }  
  }
} /* rrcudt_procedure_event_handler */


/*===========================================================================

FUNCTION rrcscrr_validate_close_session_req  

DESCRIPTION
  This function validates RRC_CLOSE_SESSION_REQ command received from MM task.

DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type: It returns RRCDT_SUCCESS if it is successfully validates 
  the RRC_OPEN_SESSION_REQ command. Otherwise it returns RRCDT_FAILURE_MINOR.

===========================================================================*/

static rrcdt_status_e_type rrcscrr_validate_close_session_req
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrcdt_status_e_type status = RRCDT_FAILURE_MINOR; 

  boolean domain_open;                  /* Indicates if the domain is open */

  rrc_csp_scan_e_type curr_scan = rrccsp_get_curr_scan();

   /* Assume that the domain associated with the flow ID is closed
   */
  domain_open = FALSE;

  /* Check to see if a signalling session to the requested domain is open
   */
  switch (cmd_ptr->cmd.close_sess_req.cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID :
      if (rrcdt_int_data.cs_domain_open)
      {
        domain_open = TRUE;
      }
      break;

    case RRC_PS_DOMAIN_CN_ID :
      if (rrcdt_int_data.ps_domain_open)
      {
        domain_open = TRUE;
      }
      break;

    default :
     
      WRRC_MSG1_HIGH("Invalid CN Domain Id  %d", 
                cmd_ptr->cmd.close_sess_req.cn_domain_id);
      break;
  }

#ifdef FEATURE_UE_INITIATED_DORMANCY
   /*  DCH_TO_FACH check is introduced because RRC is rejecting the FD request
       and this happens when app level timer expiry colliding with NW level timer expiry 
       thereby UE not sending SCRI */
  if((cmd_ptr->cmd.close_sess_req.close_session_cause == RRC_UE_INITIATED_DORMANCY)&&
     (RRC_NOT_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_SCRR) )&&
     (!(curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN)))
  {
    /* For UE INITIATED DORMANCY feature, If UE is in OOS, 
    * Then return Close session cnf with failuer cause RRC_CLOSE_SESS_UE_OOS,
    * NAS needs to return the Close status Immediately to upper layer
    */
    rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_FAILURE_UE_INIT_DORMANCY_UE_IN_OOS, 
                                cmd_ptr->cmd.close_sess_req.cn_domain_id);  
    status = RRCDT_FAILURE_MINOR;
  }
  else
#endif
  {
    /* If the domain is not open, print a message and exit
     */
    if (domain_open == FALSE)
    {
      WRRC_MSG1_HIGH("CN Domain ID not open: %d", 
                cmd_ptr->cmd.close_sess_req.cn_domain_id);
      if (rrcidt_is_idt_buffered(cmd_ptr->cmd.close_sess_req.cn_domain_id)== TRUE)
      {
        if(rrcidt_can_buffered_idt_be_deleted(cmd_ptr->cmd.close_sess_req.cn_domain_id) == TRUE)
        {
          WRRC_MSG1_HIGH("Deleted IDT for domain %d & sending close cnf to NAS",cmd_ptr->cmd.close_sess_req.cn_domain_id);
    
          if(RRC_NOT_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_SCRR))
          {
            rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_UE_OOS, 
                                       cmd_ptr->cmd.close_sess_req.cn_domain_id);  
          }
          else

          {
            rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_SUCCESS, 
                                       cmd_ptr->cmd.close_sess_req.cn_domain_id);  
          }
    
          status = RRCDT_FAILURE_MINOR;
        }
        else
        {
          WRRC_MSG0_HIGH("IDT has triggered CU, Let SCRR be Serialize with IDT ");
          status = RRCDT_SUCCESS;
        }
      }
  #ifdef FEATURE_UE_INITIATED_DORMANCY
      else if(cmd_ptr->cmd.close_sess_req.close_session_cause == RRC_UE_INITIATED_DORMANCY)
      {
        /* For UE INITIATED DORMANCY feature, If CN doain is not open , 
        * Then return Close session cnf with success cause RRC_CLOSE_SESS_SUCCESS_UE_INIT_DORMANCY,
        * NAS needs to return the Close status Immediately to upper layer.
        * This race condition can happen when Close_req is pending in RRC queue & RRC process 
        * SCR which close this CN doamin 
        */
        rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_SUCCESS_UE_INIT_DORMANCY, 
                                    cmd_ptr->cmd.close_sess_req.cn_domain_id);  
        status = RRCDT_FAILURE_MINOR;
      }
  #endif
      else
      {
        /* If RRC_CLOSE_SESSION_REQ primitive is invalid, just trash it */
        WRRC_MSG0_ERROR("Invalid RRC_CLOSE_SESSION_REQ command");
        rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_FAILURE, 
              cmd_ptr->cmd.close_sess_req.cn_domain_id);
        status = RRCDT_FAILURE_MINOR;
      }
    }
    else
    {
      status = RRCDT_SUCCESS;
    }
  }
  return (status);

} /* rrcscrr_validate_close_session_req */
/*===========================================================================

FUNCTION rrcscrr_delete_close_session_req

DESCRIPTION
  This function deletes the Close Session req data from the static variable
  for the specified CN domain

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcscrr_delete_close_session_req
(
  rrc_cn_domain_identity_e_type cn_domain_id
)
{
  /*
   * Delete the flag indicating whether PS/CS domain open
   */
  switch (cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID :
      rrcscrr_saved_cs_close_session_req.close_sess_req_status = FALSE;
      rrcscrr_saved_cs_close_session_req.mui_status = RRCSCRR_MUI_STATUS_FALSE;
      rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status = FALSE;
      rrcscrr_saved_cs_close_session_req.rrc_proc = RRC_PROCEDURE_NONE;
    break;

    case RRC_PS_DOMAIN_CN_ID :
      rrcscrr_saved_ps_close_session_req.close_sess_req_status = FALSE;
      rrcscrr_saved_ps_close_session_req.mui_status = RRCSCRR_MUI_STATUS_FALSE;
      rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status = FALSE;
      rrcscrr_saved_ps_close_session_req.rrc_proc = RRC_PROCEDURE_NONE;
      break;
    default:
      WRRC_MSG1_ERROR("Invalid CN Domain: %d" , cn_domain_id);
      break;
  
  }
}/*rrcscrr_delete_close_session_req*/

/*===========================================================================

FUNCTION rrcscrr_send_sig_conn_rel_req

DESCRIPTION
  Copy contents of saved_close_session_req and send the Signalling Conection
  Release request message on UL DCCH for CS/PS domains.

DEPENDENCIES
  None.

RETURN VALUE
  void

===========================================================================*/
static void rrcscrr_send_sig_conn_rel_req
(
  rrc_cn_domain_identity_e_type domain_id
)
{
  rrc_UL_DCCH_Message *msg_ptr;                              /* Pointer to uplink DCCH message */  
  rlc_lc_id_type ul_lc_id;                                   /* Uplink logical channel ID */
  rrcscrr_close_sess_req_type *save_close_session_req_ptr;   /* Ptr to close session Req*/
  rrcsend_status_e_type send_status;                         /* send status */
  uint8 msg_mui;                                             /* MUI for the SCRR */
  
  if(domain_id == RRC_CS_DOMAIN_CN_ID)
  {
    save_close_session_req_ptr = &rrcscrr_saved_cs_close_session_req;
  }
  else
  {
    save_close_session_req_ptr = &rrcscrr_saved_ps_close_session_req;
  }
    
  /* Check to see if there is any saved message and send SigConnReq */
  if((save_close_session_req_ptr->close_sess_req_status == TRUE) ||
     (save_close_session_req_ptr->rrc_sig_con_rel_status == TRUE)) 
  {
    /* Determine the logical channel id for the Uplink DCCH */
    ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
      DCCH_AM_RADIO_BEARER_ID, UE_MODE_ACKNOWLEDGED_DATA);
      
    if (ul_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      /* The logical channel id was not found */
      WRRC_MSG0_HIGH("RB 2 not found by LCM");
      rrcscrr_delete_close_session_req(domain_id);
      return;
    }
    else
    {
      /* Build a Signalling Connection Release Request Message. Start by allocating
       * an Uplink DCCH message. This memory is released by rrcsend_send_ul_sdu().
       */
      msg_ptr = (rrc_UL_DCCH_Message *)rrc_malloc(sizeof(struct rrc_UL_DCCH_Message));
      
      /* Identify this message as a Signalling Connection Release Indication Message */
      RRCSET_MSG_TYPE(msg_ptr->message,
      T_rrc_UL_DCCH_MessageType_signallingConnectionReleaseIndication);
      
      /* Initialize the bit mask for Signalling Connection Release Indication Message */
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.signallingConnectionReleaseIndication);
      
      /* Set the CN Domain ID from the MM primitive */
      switch (domain_id)
      {
        case RRC_CS_DOMAIN_CN_ID :
          rrcsmc_update_ciphering_config_info_for_signalling_release(domain_id);        
          msg_ptr->message.u.signallingConnectionReleaseIndication.cn_DomainIdentity = rrc_CN_DomainIdentity_cs_domain;
          break;

        case RRC_PS_DOMAIN_CN_ID :
          rrcsmc_update_ciphering_config_info_for_signalling_release(domain_id);               
          msg_ptr->message.u.signallingConnectionReleaseIndication.cn_DomainIdentity = rrc_CN_DomainIdentity_ps_domain;
          break;

        default :
          /* We should never get here because the Domain ID was already validated above */
          ERR_FATAL("Invalid Domain ID %d",domain_id, 0, 0);
          break;
      }
#ifdef FEATURE_QSH_EVENT_METRIC
      rrc_qsh_log_scri_params(0xFF,domain_id);
#endif
      /* Send the Signalling Connection Release Request Message on the Uplink DCCH */
      send_status = rrcsend_send_ul_sdu_with_mui(RRC_PROCEDURE_SCRR,msg_ptr,ul_lc_id,
                                                 RRCSEND_L2ACK_REQUIRED, &msg_mui);
      if (send_status == RRCSEND_SUCCESS)
      {
        rrcscrr_substate = RRCSCRR_WAIT_FOR_L2ACK;
        save_close_session_req_ptr->mui = msg_mui;
        /* update the mui_status cause and also the flag to indicate that close_sess_req
         * or sig_con_rel_req has been already sent */
        if(save_close_session_req_ptr->close_sess_req_status == TRUE)
        {
          save_close_session_req_ptr->mui_status = RRCSCRR_MUI_CLOSE_SESSION_CNF;
          save_close_session_req_ptr->close_sess_req_status = FALSE;            
        }
        else
        {
          save_close_session_req_ptr->mui_status = RRCSCRR_MUI_SIG_CON_REL_CNF;
          save_close_session_req_ptr->rrc_sig_con_rel_status = FALSE;
        }

        WRRC_MSG2_HIGH("SigConRel for domain %d cause %d ", 
                  domain_id, save_close_session_req_ptr->mui_status);
      }
      else
      {
        WRRC_MSG1_HIGH("Error %d sending SCRR Message", send_status);
        rrcscrr_delete_close_session_req(domain_id);
        rrcscrr_substate = RRCSCRR_INITIAL;
      }
    }
  }
}/*rrcscrr_send_sig_conn_rel_req*/

/*===========================================================================

FUNCTION RRCSCRR_PROCESS_SIG_CONN_REL_REQ

DESCRIPTION
  Check the current RRC state and send the sig conn req.If CU is active
  wair for CU to complete before sending the request

DEPENDENCIES
  None.

RETURN VALUE
  void

===========================================================================*/
void rrcscrr_process_sig_conn_rel_req
(
  rrc_cn_domain_identity_e_type domain_id
)
{
  switch(rrc_get_state())
  {
    case RRC_STATE_URA_PCH:
    case RRC_STATE_CELL_PCH:
      /* If T320 is running, do not initiate CU for SCR now. It will be initiated at expiry */
      if(rrctmr_get_remaining_time(RRCTMR_T_320_TIMER))
      {
        /* Register for CU complete. Upon CUcomplete IND, SCRI will be sent */
        rrccu_set_cu_cause_for_wait_timer(RRC_UL_DATA_TRANSMISSION);
        rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_SCRR);
      }
      /* If CU is already started, Update the CU cause to UL data ind and 
         register for complete notification*/
      else if(RRCCU_STARTED == rrccu_get_cell_update_started_status())
      {
        rrccu_set_cu_cause_to_ul_data_indication();
        rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_SCRR);
      }
      else
      {
	rrcdt_send_cell_update_initiate_req(RRC_PROCEDURE_SCRR);
      }
      rrcscrr_substate = RRCSCRR_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
      break;
		
    case RRC_STATE_CELL_FACH:
   /* This queries whether a CELL Update procedure is active.If Cell Update
    * is active, it registers for the complete and enters the substate
    * RRCIDT_WAIT_FOR_CELL_UPDATE_COMP_CNF */ 
      if(RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status(RRC_PROCEDURE_SCRR) )
      {
        /* Wait until Cell update procedure is completed */
        rrcscrr_substate = RRCSCRR_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
      }
#ifdef FEATURE_WCDMA_HS_RACH      
      else if(TRUE == rrccu_get_hsrach_sib_change())
      { 
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_SCRR,RRC_CELL_RESELECTION,TRUE);
        rrcscrr_substate = RRCSCRR_WAIT_FOR_CELL_UPDATE_COMP_CNF; 
      }
#endif
      /* If any procedure is waiting for L2 ACK for PCH transition, pend the message */
      /* After L2 ACK UE once UE transitions to PCH, CU will be initiated. SCRI can be
         transmitted after CU CPLT IND is received */
      /* If L2 ACK is not received, OOS/RLC RESET will anyways trigger CU SCRR will
         anyways get CU CPLT IND for those cases */
      else if(rrc_check_if_proc_waiting_for_l2_ack_for_pch_transition())
      {
        rrcscrr_substate = RRCSCRR_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
        rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_SCRR);
      }
      else /* Cell Update procedure is inactive */
      {
         /* Build and send the Signalling Connection Release Request to Send Chain */
         rrcscrr_send_sig_conn_rel_req(domain_id);
      }
      break;

    case RRC_STATE_CELL_DCH:
      if(rrc_check_if_proc_waiting_for_l2_ack_for_pch_transition())
      {
        rrcscrr_substate = RRCSCRR_WAIT_FOR_CELL_UPDATE_COMP_CNF;  
        rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_SCRR);
      }
      else
      {
      /* Build and send the Open Session Request to Send Chain */
        rrcscrr_send_sig_conn_rel_req(domain_id);
      }
      break;

    default:
      WRRC_MSG1_HIGH("SCRR:Invalid Event in rrc state: %d",rrc_get_state());
      break;
  }
}

/*===========================================================================

FUNCTION rrcscrr_save_sig_release_req

DESCRIPTION
  This function copies into saved_close_session_req static variable
  the contents of message received in cmd_ptr

DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type.

===========================================================================*/
static rrcdt_status_e_type rrcscrr_save_sig_release_req
(
  rrc_cmd_type *cmd_ptr
)
{
  rrcdt_status_e_type save_status = RRCDT_FAILURE_MINOR;
  
  /* check and see if the close session request is for
   * CS or PS domain
   */
  switch (cmd_ptr->cmd.rrc_sig_conn_req.cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID :
      if (( rrcscrr_saved_cs_close_session_req.close_sess_req_status == FALSE )&&
          ( rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status == FALSE) &&
          (rrcscrr_saved_cs_close_session_req.mui_status == RRCSCRR_MUI_STATUS_FALSE)
         )
      {
        rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status = TRUE;
        /* RCE can send sig_conn_rel for cs domain */
        rrcscrr_saved_cs_close_session_req.rrc_proc = 
          cmd_ptr->cmd.rrc_sig_conn_req.rrc_proc;
        save_status = RRCDT_SUCCESS;
      }
      else
      {
        WRRC_MSG1_HIGH("Multiple Close Sess not allowed: %d", 
                  cmd_ptr->cmd.close_sess_req.cn_domain_id);    
      }
    
      break;
    
    case RRC_PS_DOMAIN_CN_ID :
      if (( rrcscrr_saved_ps_close_session_req.close_sess_req_status == FALSE ) &&
          ( rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status == FALSE ) &&
          ( rrcscrr_saved_ps_close_session_req.mui_status == RRCSCRR_MUI_STATUS_FALSE)
         )
      {
        rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status = TRUE;
        rrcscrr_saved_ps_close_session_req.rrc_proc = 
          cmd_ptr->cmd.rrc_sig_conn_req.rrc_proc;
        save_status = RRCDT_SUCCESS;
      }
      else
      {
        WRRC_MSG1_HIGH("Multiple Close Sess not allowed: %d", 
                  cmd_ptr->cmd.close_sess_req.cn_domain_id);
      }
      break;
    
    default:
      WRRC_MSG0_HIGH("Invalid Domain for Close Session Req");
      break;
  }

  return(save_status);
}/*rrcscrr_save_sig_release_req*/
/*===========================================================================

FUNCTION rrcscrr_save_close_session_req

DESCRIPTION
  This function copies into saved_close_session_req static variable
  the contents of message received in cmd_ptr

DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type.

===========================================================================*/
static rrcdt_status_e_type rrcscrr_save_close_session_req
(
  rrc_cmd_type *cmd_ptr
)
{
  rrcdt_status_e_type save_status = RRCDT_FAILURE_MINOR;
  
  /* check and see if the close session request is for
   * CS or PS domain
   */
  switch (cmd_ptr->cmd.close_sess_req.cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID :
      if (( rrcscrr_saved_cs_close_session_req.close_sess_req_status == FALSE ) &&
          ( rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status == FALSE ) &&
          (rrcscrr_saved_cs_close_session_req.mui_status == RRCSCRR_MUI_STATUS_FALSE)
         )
      {
        rrcscrr_saved_cs_close_session_req.close_sess_req_status = TRUE;
        save_status = RRCDT_SUCCESS;
      }
      else
      {
        WRRC_MSG1_HIGH("Multiple Close Sess not allowed: %d", 
                  cmd_ptr->cmd.close_sess_req.cn_domain_id);    
      }
    
      break;
    
    case RRC_PS_DOMAIN_CN_ID :
      if (( rrcscrr_saved_ps_close_session_req.close_sess_req_status == FALSE ) &&
          ( rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status == FALSE ) &&
          (rrcscrr_saved_ps_close_session_req.mui_status == RRCSCRR_MUI_STATUS_FALSE)
         )
      {
        rrcscrr_saved_ps_close_session_req.close_sess_req_status = TRUE;
        save_status = RRCDT_SUCCESS;
      }
      else
      {
        WRRC_MSG1_HIGH("Multiple Close Sess not allowed: %d", 
                  cmd_ptr->cmd.close_sess_req.cn_domain_id);
      }
      break;
    
    default:
      WRRC_MSG0_HIGH("Invalid Domain for Close Session Req");
      break;
  }

  return(save_status);
}/*rrcscrr_save_close_session_req*/

/*===========================================================================

FUNCTION rrcscrr_send_close_session_cnf  

DESCRIPTION
  This function builds the RRC_CLOSE_SESSION_CNF command with parameters and 
  posts the same to the MM task.
DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type: It returns RRCDT_SUCCESS if it is successfully sends 
  the RRC_CLOSE_SESSION_CNF command. Otherwise it returns RRCDT_FAILURE_MAJOR.

===========================================================================*/

void rrcscrr_send_close_session_cnf
(
  rrc_close_session_status_e_type     session_status,  /* Success or failure
                                                          status*/
  rrc_cn_domain_identity_e_type       cn_domain_id     /* CN Domain Id */

)
{
  mm_cmd_type *cmd_ptr;                      /* Pointer to the MM Command */

#ifdef FEATURE_INACTIVITY_HANDLING
  if (cn_domain_id == RRC_PS_DOMAIN_CN_ID) 
  {
    rrctmr_stop_timer(RRCTMR_INACTIVITY_TIMER);
  }
#endif /* FEATURE_INTACTIVITY_HANDLING */

#ifdef FEATURE_3GPP_CSFB
  /*CS signaling connection is closed, reset CSFB call status flag*/
  if (cn_domain_id == RRC_CS_DOMAIN_CN_ID) 
  {
    rrc_csfb_call_status = FALSE;
  }
#endif

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
  MSG_4(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
    "CSFB_HACK: csfb_extended_status = %d, close session for domain %d, PS domain status %d Lte mode enabled status %d ",
    csfb_extended_status,cn_domain_id,rrcdt_int_data.ps_domain_open, rrcmcm_is_lte_mode_enabled()
    );

  if((RRC_CLOSE_SESS_SUCCESS == session_status) && (csfb_extended_status == CSFB_STARTED) &&
     (cn_domain_id == RRC_CS_DOMAIN_CN_ID) && (rrc_cs_rab_established))
  {
    /* If LTE mode is not enabled reset csfb extended status such that blind direction doesn't happen */
    if(FALSE == rrcmcm_is_lte_mode_enabled())
    {
      csfb_extended_status = CSFB_INVALID;
    }
    else
    {
      rrcrce_post_internal_csfb_abort_cmd();
      rrc_cs_rab_established = FALSE;
      return;
    }
  }
#endif

  /* Allocate memory for RRC_CLOSE_SESSION_CNF command */
  if( (cmd_ptr =  mm_rrc_get_cmd_buf(RRC_CLOSE_SESSION_CNF)) != NULL)
  {
    /* Fill in parameters fro RRC_CLOSE_SESSION_CNF command */
    cmd_ptr->cmd.rrc_close_session_cnf.cn_domain_id= cn_domain_id ;

    rrcsmc_update_sync_ind_info_for_signalling_release(cn_domain_id);

    cmd_ptr->cmd.rrc_close_session_cnf.status = session_status;

#ifdef FEATURE_DUAL_SIM
    cmd_ptr->cmd.rrc_close_session_cnf.as_id = rrc_get_as_id();
#endif

    rrc_send_rrc_message_to_mm(cmd_ptr);               /* Sends the command to MM */

    ps_clsd_after_suspend_req_in_cco = TRUE;

    WRRC_MSG2_HIGH("Sent RRC_CLOSE_SESSION_CNF to MM with status =%d, domain id =%d",session_status,cn_domain_id);

#ifdef FEATURE_DUAL_SIM
    /* When CS domain is closed we should check and allow call on the other rat */
    if (cn_domain_id == RRC_CS_DOMAIN_CN_ID) 
    {
      rrc_check_send_change_priority();
    }
#endif
  }     
} /* rrcscrr_send_close_session_cnf */
/*===========================================================================

FUNCTION rrcscrr_send_sig_conn_release_cnf  

DESCRIPTION
  This function builds the RRC_SIG_CONN_RELEASE_CNF command with parameters and 
  posts the same to the MM task.
DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type: It returns RRCDT_SUCCESS if it is successfully sends 
  the RRC_SIG_CONN_RELEASE_CNF command. Otherwise it returns RRCDT_FAILURE_MAJOR.

===========================================================================*/
void rrcscrr_send_sig_conn_release_cnf
(
  uecomdef_status_e_type              status,  /* Success or failure status*/
  rrc_proc_e_type                     rrc_proc  /* Dest rrc proc */
)
{
  rrc_cmd_type *cmd_ptr;                      /* Pointer to the Command */


  /* Allocate memory for RRC_CLOSE_SESSION_CNF command */
  if( (cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
  {
    cmd_ptr->cmd_hdr.cmd_id = RRC_SIG_CONN_RELEASE_CNF;

    /* Fill in parameters for RRC_SIG_CONN_RELEASE_CNF command */
    cmd_ptr->cmd.rrc_sig_conn_cnf.status = status;
    cmd_ptr->cmd.rrc_sig_conn_cnf.rrc_proc = rrc_proc;
   
    rrc_put_int_cmd(cmd_ptr);                    /* Sends the command to MM */

    WRRC_MSG1_HIGH("Sent RRC_SIG_CONN_RELEASE_CNF to Proc:rrc_proc_e_type_value%d",rrc_proc);
  }    
} /* rrcscrr_send_sig_conn_release_cnf */


/*===========================================================================

FUNCTION rrcscrr_process_scrr_initial_substate

DESCRIPTION
  This function processes all events that are  dispatched
  to Signalling Connection Release Request procedure in RRCSCRR_INITIAL
  substate

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcscrr_process_scrr_initial_substate
(
  rrc_cmd_type *cmd_ptr
)
{
  rrc_cn_domain_identity_e_type domain_id;

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_CLOSE_SESSION_REQ:
    case RRC_SIG_CONN_RELEASE_REQ:
       
      if(cmd_ptr->cmd_hdr.cmd_id == RRC_CLOSE_SESSION_REQ)
      {
        WRRC_MSG0_HIGH("Received RRC_CLOSE_SESSION_REQ");
  
        /* Assume that the domain associated with the flow ID is closed */
        if(RRCDT_SUCCESS != rrcscrr_validate_close_session_req(cmd_ptr))
        {
          /*Moved sending Close Session Cnf in rrcscrr_validate_close_session_req function*/
          return;
        }
      
        if(RRCDT_SUCCESS != rrcscrr_save_close_session_req(cmd_ptr))
        {
          /* If there are multiple requests,then just trash it */
          /*lint -e666*/
          WRRC_MSG2_HIGH("Failed to Save Close Sess Req in: %d, %d",
                   rrc_get_state(),rrcscrr_substate);
          /*lint +e666*/
  
          rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_FAILURE, 
            cmd_ptr->cmd.close_sess_req.cn_domain_id);
          return;
        }
        domain_id = cmd_ptr->cmd.close_sess_req.cn_domain_id;
      }
      else if(cmd_ptr->cmd_hdr.cmd_id == RRC_SIG_CONN_RELEASE_REQ)
      {
         WRRC_MSG0_HIGH("Received RRC_SIG_CONN_RELEASE_REQ");
        /* No need to validate this command */
        if(RRCDT_SUCCESS != rrcscrr_save_sig_release_req(cmd_ptr))
        {
          /* If there are multiple requtes, just trash it */
          /*lint -e666*/
          WRRC_MSG2_HIGH("Failed to Save Sig Rel Req in: %d, %d",
                   rrc_get_state(),rrcscrr_substate);
          /*lint +e666*/
  
          rrcscrr_send_sig_conn_release_cnf(
                    FAILURE,
                    cmd_ptr->cmd.rrc_sig_conn_req.rrc_proc);
          return;
        }
        domain_id = cmd_ptr->cmd.rrc_sig_conn_req.cn_domain_id;
      }
      else
      {
        WRRC_MSG0_HIGH("Invalid domain id");
        return;
      }

      /* Send the SCRR only if an IDT for the same domain is not currently
      active.If an IDT is active,then wait for IDT to complete and then send
      the SigConnRelInd out.This is to avoid a race condition where IDT reaches
      the network after SCRR while the reverse was intended */
      if(FALSE == rrcidt_is_idt_active(domain_id))
      {
        rrcscrr_process_sig_conn_rel_req(domain_id);
      }
      else
      {
        WRRC_MSG1_HIGH("Defer processing SCRR since IDT is active.domain:%d",domain_id);
      }
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d for SCRR in substate:%d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcscrr_substate);
      break;
  }/*end of switch stmt*/
} /* rrcscrr_process_scrr_initial_substate */
/*===========================================================================

FUNCTION rrcscrr_process_scrr_wait_for_cell_update_comp_cnf_substate

DESCRIPTION
  This function processes all events that are  dispatched
  to Signalling Connection Release Request procedure in 
  RRCSCRR_WAIT_FOR_CELL_UPDATE_COMP_CNF substate

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcscrr_process_scrr_wait_for_cell_update_comp_cnf_substate
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_CELL_UPDATE_COMPLETE_CNF:
      /* Check to see if there are any saved SCRR messages waiting to be transmitted.
      Make sure to check both the domains and send out the message on a per domain 
      basis.Check the MUI status also because re-transmission would take a diff
      path */
      if(((rrcscrr_saved_cs_close_session_req.close_sess_req_status == TRUE)||
          (rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status == TRUE)) &&
         (rrcscrr_saved_cs_close_session_req.mui_status == RRCSCRR_MUI_STATUS_FALSE))
      {
        WRRC_MSG0_HIGH("Sending SCRR for CS domain after Cu_Complete_Cnf");
        rrcscrr_send_sig_conn_rel_req(RRC_CS_DOMAIN_CN_ID);
      }
      
      if(((rrcscrr_saved_ps_close_session_req.close_sess_req_status == TRUE)||
          (rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status == TRUE)) &&
         (rrcscrr_saved_ps_close_session_req.mui_status == RRCSCRR_MUI_STATUS_FALSE))
      {
        WRRC_MSG0_HIGH("Sending SCRR for PS domain after Cu_Complete_Cnf");
        rrcscrr_send_sig_conn_rel_req(RRC_PS_DOMAIN_CN_ID);
      }
      break;

    /* if we get a close session request, it can be for a different cn domain.
     * save the request
     */
    case RRC_CLOSE_SESSION_REQ:
    case RRC_SIG_CONN_RELEASE_REQ:
      if(cmd_ptr->cmd_hdr.cmd_id == RRC_CLOSE_SESSION_REQ)
      {
        if(RRCDT_SUCCESS != rrcscrr_validate_close_session_req(cmd_ptr))
        {
          /*Moved sending Close Session Cnf in rrcscrr_validate_close_session_req function*/
          return;
        }
      
        if(RRCDT_SUCCESS != rrcscrr_save_close_session_req(cmd_ptr))
        {
          /* If RRC_CLOSE_SESSION_REQ primitive is invalid, just trash it */
          /*lint -e666*/
          WRRC_MSG2_HIGH("Failed to Save Close Sess Req in: %d, %d",
                   rrc_get_state(),rrcscrr_substate);
          /*lint +e666*/
  
          rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_FAILURE, 
            cmd_ptr->cmd.close_sess_req.cn_domain_id);
          return;
        }
      }
      if(cmd_ptr->cmd_hdr.cmd_id == RRC_SIG_CONN_RELEASE_REQ)
      {
        /* No need to validate this command */
        if(RRCDT_SUCCESS != rrcscrr_save_sig_release_req(cmd_ptr))
        {
          /* If there are multiple requtes, just trash it */
          /*lint -e666*/
          WRRC_MSG2_HIGH("Failed to Save Sig Rel Req in: %d, %d",
                   rrc_get_state(),rrcscrr_substate);
          /*lint +e666*/
          rrcscrr_send_sig_conn_release_cnf(
                    FAILURE,
                    cmd_ptr->cmd.rrc_sig_conn_req.rrc_proc);
          return;
        }
      }
      break;

    default:
      WRRC_MSG2_HIGH("SCRR:Invalid Event: %d in substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcscrr_substate);
      break;
  }  
} /* rrcscrr_process_scrr_wait_for_cell_update_comp_cnf_substate */

/*===========================================================================

FUNCTION rrcscrr_process_scrr_wait_for_l2_ack_substate

DESCRIPTION
  This function processes all events that are  dispatched
  to Signalling Connection Release Request procedure in 
  RRCSCRR_WAIT_FOR_L2_ACK substate

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcscrr_process_scrr_wait_for_l2_ack_substate
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrc_cn_domain_identity_e_type local_domain_id;

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_DOWNLINK_L2ACK_IND:
      WRRC_MSG2_HIGH("SCRR L2-ACK status %d, mui %d",
               cmd_ptr->cmd.downlink_l2ack_ind.status,
               cmd_ptr->cmd.downlink_l2ack_ind.mui);

      if((rrcscrr_saved_cs_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE) && 
         (cmd_ptr->cmd.downlink_l2ack_ind.mui == rrcscrr_saved_cs_close_session_req.mui))
      {
        if(rrcscrr_saved_cs_close_session_req.mui_status == RRCSCRR_MUI_CLOSE_SESSION_CNF)
        {
          //send close-session-cnf status based on l2-ack status
          if(SUCCESS == cmd_ptr->cmd.downlink_l2ack_ind.status)
          {
            rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_SUCCESS,RRC_CS_DOMAIN_CN_ID);
            WRRC_MSG0_HIGH("CS DOMAIN CLOSE SESSION CNF WITH SUCCESS - DELETE ALL SAVED UDTs");
            rrcudt_delete_all_saved_data_req(RRC_CS_DOMAIN_CN_ID);
            rrcdt_int_data.cs_domain_open = FALSE;
            if(rrcudt_any_saved_data_req_for_cn_domain(RRC_PS_DOMAIN_CN_ID) == FALSE)
            {
              WRRC_MSG0_HIGH("There is no saved UDT for PS domain, set udt substate to init");
              rrcudt_substate = RRCUDT_INITIAL;
            }
          }
          else
          {
            rrcscrr_saved_cs_close_session_req.close_sess_req_status =TRUE;
            rrcscrr_send_sig_conn_rel_req(RRC_CS_DOMAIN_CN_ID);
            break;
          }
        }
        else if(rrcscrr_saved_cs_close_session_req.mui_status == RRCSCRR_MUI_SIG_CON_REL_CNF)
        {
          //send sig-conn-release-cnf status based on l2-ack status
          if(SUCCESS == cmd_ptr->cmd.downlink_l2ack_ind.status)
          {
            rrcscrr_send_sig_conn_release_cnf(
                        SUCCESS, 
                        rrcscrr_saved_cs_close_session_req.rrc_proc);
            WRRC_MSG0_HIGH("CS DOMAIN SCRR CNF WITH SUCCESS - DELETE ALL SAVED UDTs");
            rrcudt_delete_all_saved_data_req(RRC_CS_DOMAIN_CN_ID);
            if(rrcudt_any_saved_data_req_for_cn_domain(RRC_PS_DOMAIN_CN_ID) == FALSE)
            {
              WRRC_MSG0_HIGH("There is no saved UDT for PS domain, set udt substate to init");
              rrcudt_substate = RRCUDT_INITIAL;
            }
            rrcdt_int_data.cs_domain_open = FALSE;
          }
          else
          {
            rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status =TRUE;
            rrcscrr_send_sig_conn_rel_req(RRC_CS_DOMAIN_CN_ID);
            break;
          }
        }
       
        if(rrcscrr_saved_ps_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE)
        {
          rrcscrr_substate = RRCSCRR_WAIT_FOR_L2ACK; 
        }
        else
        {
          rrcscrr_substate = RRCSCRR_INITIAL; 
        }

        rrcscrr_delete_close_session_req(RRC_CS_DOMAIN_CN_ID);
          
      }
      else if((rrcscrr_saved_ps_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE) && 
              (cmd_ptr->cmd.downlink_l2ack_ind.mui == rrcscrr_saved_ps_close_session_req.mui))
      {
        //check if the sig_conn_rel_ind was for close_session_req or sig_con_rel_req
        //and send the appropriate RSP to RCE
        if(rrcscrr_saved_ps_close_session_req.mui_status == RRCSCRR_MUI_CLOSE_SESSION_CNF)
        {
          //send cnf based on L2-ACK status
          if(SUCCESS == cmd_ptr->cmd.downlink_l2ack_ind.status)
          {
            rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_SUCCESS, RRC_PS_DOMAIN_CN_ID);
            WRRC_MSG0_HIGH("PS DOMAIN CLOSE SESSION CNF WITH SUCCESS - DELETE ALL SAVED UDTs");
            rrcudt_delete_all_saved_data_req(RRC_PS_DOMAIN_CN_ID);
            rrcdt_int_data.ps_domain_open = FALSE;
            if(rrcudt_any_saved_data_req_for_cn_domain(RRC_CS_DOMAIN_CN_ID) == FALSE)
            {
              WRRC_MSG0_HIGH("There is no saved UDT for CS domain, set udt substate to init");
              rrcudt_substate = RRCUDT_INITIAL;
            }
          }
          else
          {
            rrcscrr_saved_ps_close_session_req.close_sess_req_status =TRUE;
            rrcscrr_send_sig_conn_rel_req(RRC_PS_DOMAIN_CN_ID);
            break;      
          }
        }
        else if(rrcscrr_saved_ps_close_session_req.mui_status == RRCSCRR_MUI_SIG_CON_REL_CNF)
        {
          //send cnf based on L2-ACK status
          if(SUCCESS == cmd_ptr->cmd.downlink_l2ack_ind.status)
          {
            rrcscrr_send_sig_conn_release_cnf(
                        SUCCESS,
                        rrcscrr_saved_ps_close_session_req.rrc_proc);
            WRRC_MSG0_HIGH("PS DOMAIN SCRR CNF WITH SUCCESS - DELETE ALL SAVED UDTs");
            rrcudt_delete_all_saved_data_req(RRC_PS_DOMAIN_CN_ID);
            rrcdt_int_data.ps_domain_open = FALSE;
            if(rrcudt_any_saved_data_req_for_cn_domain(RRC_CS_DOMAIN_CN_ID) == FALSE)
            {
              WRRC_MSG0_HIGH("There is no saved UDT for CS domain, set udt substate to init");
              rrcudt_substate = RRCUDT_INITIAL;
            }

#ifdef FEATURE_INACTIVITY_HANDLING
           rrctmr_stop_timer(RRCTMR_INACTIVITY_TIMER);   
#endif /* FEATURE_INTACTIVITY_HANDLING */

          }
          else
          {
            rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status =TRUE;
            rrcscrr_send_sig_conn_rel_req(RRC_PS_DOMAIN_CN_ID);
            break;
          }
        }

        if(rrcscrr_saved_cs_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE)
        {
          rrcscrr_substate = RRCSCRR_WAIT_FOR_L2ACK; 
        }
        else
        {
          rrcscrr_substate = RRCSCRR_INITIAL; 
        }
        
        rrcscrr_delete_close_session_req(RRC_PS_DOMAIN_CN_ID);
      }
      else
      {
        WRRC_MSG0_ERROR("Invalid L2 ACK is rcvd");
      }
      break;

    /* if we get a close session request, it can be for a different cn domain.
     * save the request
     */
    case RRC_CLOSE_SESSION_REQ:
    case RRC_SIG_CONN_RELEASE_REQ:
      if(cmd_ptr->cmd_hdr.cmd_id == RRC_CLOSE_SESSION_REQ)
      {
        /* Copy the domain id */
        local_domain_id = cmd_ptr->cmd.close_sess_req.cn_domain_id;

        if(RRCDT_SUCCESS != rrcscrr_validate_close_session_req(cmd_ptr))
        {
          /*Moved sending Close Session Cnf in rrcscrr_validate_close_session_req function*/
          return;
        }
      
        if(RRCDT_SUCCESS != rrcscrr_save_close_session_req(cmd_ptr))
        {
          /* If RRC_CLOSE_SESSION_REQ primitive is invalid, just trash it */
          /*lint -e666*/
          WRRC_MSG2_HIGH("Failed to Save Close Sess Req in: %d, %d",
                   rrc_get_state(),rrcscrr_substate);
          /*lint +e666*/
  
          rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_FAILURE,cmd_ptr->cmd.close_sess_req.cn_domain_id);
          return;
        }
      }
      else if(cmd_ptr->cmd_hdr.cmd_id == RRC_SIG_CONN_RELEASE_REQ)
      {
        /* Copy the domain id */
        local_domain_id = cmd_ptr->cmd.rrc_sig_conn_req.cn_domain_id;

        /* No need to validate this command */
        if(RRCDT_SUCCESS != rrcscrr_save_sig_release_req(cmd_ptr))
        {
          /* If there are multiple requtes, just trash it */
          /*lint -e666*/
          WRRC_MSG2_HIGH("Failed to Save Sig Rel Req in: %d, %d",
                   rrc_get_state(),rrcscrr_substate);
          /*lint +e666*/
          rrcscrr_send_sig_conn_release_cnf(
                    FAILURE,
                     cmd_ptr->cmd.rrc_sig_conn_req.rrc_proc);
          return;
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Invalid command");
        return;
      }

      /* Send the SCRR only if an IDT for the same domain is not currently
      active.If an IDT is active,then wait for IDT to complete and then send 
      the SigConnRelInd out.This is to avoid a race condition where IDT reaches
      the network after SCRR while the reverse was intended */
      if(FALSE == rrcidt_is_idt_active(local_domain_id))
      {
        rrcscrr_send_sig_conn_rel_req(local_domain_id);
      }
      else
      {
        WRRC_MSG1_HIGH("Defer processing SCRR since IDT is active.domain:%d",local_domain_id);
      }
      break;

    default:
      WRRC_MSG2_HIGH("SCRR:Invalid Event: %d in substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcscrr_substate);
      break;
  }  
} /* rrcscrr_process_scrr_wait_for_l2_ack_substate */

/*===========================================================================

FUNCTION rrcscrr_is_cn_domain_open

DESCRIPTION
  This function will return the status of CN domain whether its open or closed
  It also checks to see if signalling connection relaese or
  close session request is in progress. This function is used by DDT
  to check whether the domain is open.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE : If the CN Domain is open.
  FALSE: If the CN Domain is closed.

===========================================================================*/
uint8 rrcscrr_is_cn_domain_open(rrc_cn_domain_identity_e_type cn_domain)
{
  uint8 is_cn_domain_open = FALSE;  
  if(cn_domain == RRC_CS_DOMAIN_CN_ID)
  {
    if(rrcdt_int_data.cs_domain_open == TRUE)
    {
      /*Checking if signalling Release in progress*/
      if((rrcscrr_saved_cs_close_session_req.mui_status == RRCSCRR_MUI_SIG_CON_REL_CNF)||
         (rrcscrr_saved_cs_close_session_req.mui_status == RRCSCRR_MUI_CLOSE_SESSION_CNF))
      {
        /*Since DT has sent Signalling connection Release Request to UTRAN & waiting for the
            L2-ACK, So discard the DDT message in this sall window*/
        WRRC_MSG1_HIGH("Signalling connection Release is in progress for cn domain =%d",cn_domain);
        is_cn_domain_open = FALSE;
      }
      else
      {
        is_cn_domain_open = TRUE;
      }
    }
    else
    {/*This CN domain is closed*/
      is_cn_domain_open = FALSE;
    }
  }
  else if(cn_domain == RRC_PS_DOMAIN_CN_ID)
  {
    if(rrcdt_int_data.ps_domain_open == TRUE)
    {
      /*Checking if signalling Release in progress*/
      if((rrcscrr_saved_ps_close_session_req.mui_status == RRCSCRR_MUI_SIG_CON_REL_CNF)||
         (rrcscrr_saved_ps_close_session_req.mui_status == RRCSCRR_MUI_CLOSE_SESSION_CNF))
      {
        /*Since DT has sent Signalling connection Release Request to UTRAN & waiting for the
        L2-ACK, So discard the DDT message in this sall window*/
        WRRC_MSG1_HIGH("Signalling connection Release is in progress for cn domain =%d",cn_domain);
        is_cn_domain_open = FALSE;
      }
      else
      {
        is_cn_domain_open = TRUE;
      }
    }
    else
    {/*This CN domain is closed*/
      is_cn_domain_open = FALSE;
    }
  }
  else
  {/*This will not exicute since this function is called with valid CN domain*/
    WRRC_MSG1_HIGH("Invalid CN Domain = %d",cn_domain);
  }
  return is_cn_domain_open;
}
/*===========================================================================

FUNCTION rrcscrr_process_cell_fach_state

DESCRIPTION
  This function processes all events that are dispatched to Signalling 
  Connection Release Request procedure in CELL_FACH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcscrr_process_cell_fach_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcscrr_substate)
  {
    case RRCSCRR_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_CLOSE_SESSION_REQ:
        case RRC_SIG_CONN_RELEASE_REQ:
          rrcscrr_process_scrr_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG1_HIGH("SCRR:Invalid event in Substate: %d",
                    rrcscrr_substate);
         break;
      }
      break;

    case RRCSCRR_WAIT_FOR_CELL_UPDATE_COMP_CNF: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        /* we can either get a complete confirmation or get a close session request
         * for a different CN domain.  Process both in wait for cell update comp cnf
         */
        case RRC_CELL_UPDATE_COMPLETE_CNF:
        case RRC_CLOSE_SESSION_REQ:
        case RRC_SIG_CONN_RELEASE_REQ:
          rrcscrr_process_scrr_wait_for_cell_update_comp_cnf_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("SCRR:Invalid event in Substate: %d",
                    rrcscrr_substate);

          break;
      }
      break;

    case RRCSCRR_WAIT_FOR_L2ACK: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        /* we can either get a complete confirmation or get a close session request
         * for a different CN domain.  Process both in wait for cell update comp cnf
         */
        case RRC_DOWNLINK_L2ACK_IND:
        case RRC_CLOSE_SESSION_REQ:
        case RRC_SIG_CONN_RELEASE_REQ:
          rrcscrr_process_scrr_wait_for_l2_ack_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("SCRR:Invalid event in Substate: %d",
                    rrcscrr_substate);

          break;
      }
      break;

    default:   
      /*lint -e793*/        
      WRRC_MSG1_HIGH("SCRR:Invalid substate: %d  in CELL FACH",
                rrcscrr_substate);
      /*lint +e793*/
      break;
  }  
}/* rrcscrr_process_cell_fach_state */
/*===========================================================================

FUNCTION rrcscrr_process_cell_dch_state

DESCRIPTION
  This function processes all events that are dispatched to Signalling
  Connection Release Request procedure in CELL_DCH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcscrr_process_cell_dch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcscrr_substate)
  {
    case RRCSCRR_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_CLOSE_SESSION_REQ:
        case RRC_SIG_CONN_RELEASE_REQ:
          rrcscrr_process_scrr_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG1_HIGH("SCRR:Invalid event in Substate: %d",
                    rrcscrr_substate);
          break;
      }
      break;

    case RRCSCRR_WAIT_FOR_CELL_UPDATE_COMP_CNF: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        /* we can either get a complete confirmation or get a close session request
         * for a different CN domain.  Process both in wait for cell update comp cnf
         */
        case RRC_CELL_UPDATE_COMPLETE_CNF:
        case RRC_CLOSE_SESSION_REQ:
        case RRC_SIG_CONN_RELEASE_REQ:
          rrcscrr_process_scrr_wait_for_cell_update_comp_cnf_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("SCRR:Invalid event in Substate: %d",
                    rrcscrr_substate);

          break;
      }
      break;

    case RRCSCRR_WAIT_FOR_L2ACK: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        /* we can either get a complete confirmation or get a close session request
         * for a different CN domain.  Process both in wait for cell update comp cnf
         */
        case RRC_DOWNLINK_L2ACK_IND:
        case RRC_CLOSE_SESSION_REQ:
        case RRC_SIG_CONN_RELEASE_REQ:
          rrcscrr_process_scrr_wait_for_l2_ack_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("SCRR:Invalid event in Substate: %d",
                    rrcscrr_substate);

          break;
      }
      break;

    default:           
      WRRC_MSG1_HIGH("SCRR:Invalid substate: %d in CELL_DCH",
                rrcscrr_substate);
      break;
  }  
}/* rrcscrr_process_cell_dch_state */

/*===========================================================================

FUNCTION rrcscrr_process_cell_ura_pch_state

DESCRIPTION
  This function processes all events that are dispatched to Signalling 
  Connection Release Request procedure in either in CELL_PCH or URA_PCH
  state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcscrr_process_cell_ura_pch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcscrr_substate)
  {
    case RRCSCRR_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_CLOSE_SESSION_REQ:
        case RRC_SIG_CONN_RELEASE_REQ:
          rrcscrr_process_scrr_initial_substate(cmd_ptr);
          break;
        /*lint -e793*/
        default:
          WRRC_MSG1_HIGH("SCRR:Invalid event in Substate: %d",
                    rrcscrr_substate);
          break;
        /*lint +e793*/
      }
      break;

    case RRCSCRR_WAIT_FOR_CELL_UPDATE_COMP_CNF: 
      rrcscrr_process_scrr_wait_for_cell_update_comp_cnf_substate(cmd_ptr);
      break;

    case RRCSCRR_WAIT_FOR_L2ACK:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_SIG_CONN_RELEASE_REQ:
          rrcscrr_process_scrr_wait_for_l2_ack_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("SCRR:Invalid event in Substate: %d",
                    rrcscrr_substate);
          break;
      }
      break;

    default:
      WRRC_MSG1_HIGH("SCRR: Invalid substate: %d in PCH",
                rrcscrr_substate);
      break;
  }  
}/* rrcscrr_process_cell_ura_pch_state */
/*===========================================================================

FUNCTION rrcscrr_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the Signalling Connection Release
  Request procedure.
  The following events are handled by this procedure:

    RRC_CLOSE_SESSION_REQ Primitive (received from MM)
    RRC_CELL_UPDATE_COMPLETE_CNF primitive (received from CELL UPDATE Procedure)

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcscrr_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
 
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG1_HIGH_OPT("function rrcscrr_procedure_event_handler is called in rrcscrr_substate rrcscrr_substate_e_type_value%d", rrcscrr_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_SCRR,rrcscrr_substate,cmd_ptr->cmd_hdr.cmd_id);

  if(RRC_STATE_CHANGE_IND == cmd_ptr->cmd_hdr.cmd_id)
  {
    WRRC_MSG2_HIGH("State-Change-Ind scrr-state %d in rrc-state %d",rrcudt_substate,rrc_state);
    //Any pending message memory needs to be de-allocated here 
    rrcscrr_delete_close_session_req(RRC_CS_DOMAIN_CN_ID);
    rrcscrr_delete_close_session_req(RRC_PS_DOMAIN_CN_ID);
    rrcscrr_substate = RRCSCRR_INITIAL;  
  }
  else
  {
      if((RRC_CLOSE_SESSION_REQ == cmd_ptr->cmd_hdr.cmd_id) ||
         (RRC_SIG_CONN_RELEASE_REQ ==  cmd_ptr->cmd_hdr.cmd_id))
      {
        if(TRUE == rrcdt_update_scrr_to_handle_dormancy(cmd_ptr))
          return;
      }
 
    switch (rrc_state)
    {
      case RRC_STATE_CELL_FACH:    
        rrcscrr_process_cell_fach_state(cmd_ptr); 
        break;

      case RRC_STATE_CELL_DCH:  
        rrcscrr_process_cell_dch_state(cmd_ptr); 
        break;

      case RRC_STATE_CELL_PCH:
      case RRC_STATE_URA_PCH:
        rrcscrr_process_cell_ura_pch_state(cmd_ptr);
        break;

      default:  
        WRRC_MSG1_HIGH("SCRR:Event in an invalid State: %d",rrc_state);
        break;
     }  
  }
} /* rrcscrr_procedure_event_handler */

/*===========================================================================

FUNCTION rrcddt_process_downlink_direct_transfer_msg  

DESCRIPTION
  This function checks whether RRC_DOWNLINK_SDU_IND contains Downlink
  Direct Transfer message.If it contains Downlink Direct Transfer message,
  it will processes the same and send RRC_DATA_IND to the MM.In addition
  it also processes the RRC_SMC_DDT_L2_ACK_IND command and forwards all
  the stored NAS messages to NAS.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcddt_process_downlink_direct_transfer_msg
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;    /* Pointer to Downlink DCCH */


  mm_cmd_type *mm_cmd_ptr;                   /* Pointer to the RRC Command */


  rlc_lc_id_type lc_id;                      /* Local variable to store 
                                                RLC LC Id */
  unsigned char *payload;                    /* Pointer to the MM payload */
 
  uint8 message_choice;                      /* Local varibale to store 
                                                message choice */
  uint8 count;                               /* Local counter for index */

  rrc_cn_domain_identity_e_type cn_id;


  /* Set the status to ASN1 successfully decoded */
  rrc_protocol_error_reject = FALSE;


  /* Assert the received message is of type rrc_DL_CCCH_Message */
  if((RRC_DOWNLINK_SDU_IND == cmd_ptr->cmd_hdr.cmd_id) && 
     (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_DCCH_Message_PDU) )
  {
     message_choice = 
       rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                              cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);


    if(cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL)
    {
      /* Get the OTA message pointer */
      dcch_msg_ptr = (rrc_DL_DCCH_Message *) 
                     cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
    }
    else
    {
      rrc_protocol_error_reject = TRUE;
    }


    /* Check the received message is Downlink Direct transfer message */
    if( message_choice  ==
        T_rrc_DL_DCCH_MessageType_downlinkDirectTransfer)
    {
      WRRC_MSG0_HIGH("Downlink Direct Transfer Msg is rcvd");


      if(rrc_protocol_error_reject == TRUE) /* DCCH ASN1 decoding failed */
      {
        lc_id = rrclcm_check_ul_rlc_lc_id(    
                UE_LOGCHAN_DCCH,          /* Uplink Logical Channel Type. */
                DCCH_AM_RADIO_BEARER_ID,  /* Radio Bearer Id assoicated with
                                            the above Downlink Logical 
                                            Channel type. */
                UE_MODE_ACKNOWLEDGED_DATA);
                                          /* Mode of transmission for the RLC 
                                             service entity. */
        if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
        {
         /*log the protocol error*/
         rrc_log_protocol_error( (uint8) rrc_get_state(),
                                 (uint8)RRC_PROCEDURE_DDT,
                                 (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                                 (uint8) RRCLOG_PROTOCOL_ERROR_ViolationOrEncodingError 
                               );
          WRRC_MSG0_HIGH("Can not find RLC ID for UL RB 2");
        }         
        else /* Valid RLC ID found */
        {
          if(SUCCESS != rrcsend_send_rrc_status_message(
             T_rrc_ProtocolErrorMoreInformation_type1_asn1_ViolationOrEncodingError,
                                           // The protocol error cause to be placed in
                                           // the message
             0,                            // Set the transaction id to 0 since
                                           // ASN1 decoding is failed 
             rrc_ReceivedMessageType_downlinkDirectTransfer,   // Received message identification
             RRC_PROCEDURE_DDT,            // RRC procedure
             lc_id,                        /* Logical Channel Id */
             RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                           /* Indicates whether L2 Ack 
                                              required or not */
          {
            WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
          } 
        } /*  Valid RLC ID found */
      }
      else /* ASN1 decoding is successful */
      {
        /*lint -save -e613 */
        if (RRC_CHECK_MSG_TYPE(dcch_msg_ptr->message.u.downlinkDirectTransfer,
                 T_rrc_DownlinkDirectTransfer_later_than_r3))
        {
          /* Critical extensions are not supported */
          WRRC_MSG0_ERROR("Critial extensions are not supported");


          lc_id = rrclcm_check_ul_rlc_lc_id(    
                  UE_LOGCHAN_DCCH,          /* Uplink Logical Channel Type. */
                  DCCH_AM_RADIO_BEARER_ID,  /* Radio Bearer Id assoicated with
                                              the above Downlink Logical 
                                              Channel type. */
                  UE_MODE_ACKNOWLEDGED_DATA);
                                            /* Mode of transmission for the RLC 
                                               service entity. */
          if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
          {
            WRRC_MSG0_HIGH("Can not find RLC ID for UL RB 2");
          }         
          else /* Valid RLC ID found */
          {
            if(SUCCESS != rrcsend_send_rrc_status_message(
               T_rrc_ProtocolErrorMoreInformation_type1_messageExtensionNotComprehended,
                                             // The protocol error cause to be placed in
                                             // the message
               0,                            // Set the transaction id to 0 since
                                             // critical extensions selected
               rrc_ReceivedMessageType_downlinkDirectTransfer,   // Received message identification
               RRC_PROCEDURE_DDT,            // RRC procedure
               lc_id,                        /* Logical Channel Id */
               RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                             /* Indicates whether L2 Ack 
                                                required or not */
            {
              WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
            } 
          } /*  Valid RLC ID found */
          return;
        }
        /* Check for open signalling connection match */
        if( (( rrc_CN_DomainIdentity_cs_domain ==
               dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
               downlinkDirectTransfer_r3.cn_DomainIdentity) &&
               (rrcscrr_is_cn_domain_open(RRC_CS_DOMAIN_CN_ID))) || 
            (( rrc_CN_DomainIdentity_ps_domain == 
               dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
               downlinkDirectTransfer_r3.cn_DomainIdentity) &&
               (rrcscrr_is_cn_domain_open(RRC_PS_DOMAIN_CN_ID))) )
        {        
          /* Check for valid payload length */
          if((dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
              downlinkDirectTransfer_r3.nas_Message.numocts == 0) ||
             (dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
              downlinkDirectTransfer_r3.nas_Message.numocts >
              RRCDT_MAX_PAYLOAD) )
          {
            WRRC_MSG1_HIGH("Invalid message length for DDT: %d", 
                      dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
                      downlinkDirectTransfer_r3.nas_Message.numocts);
          }
          else /* Valid Message Length */
          {
            if(rrc_CN_DomainIdentity_cs_domain ==
               dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
               downlinkDirectTransfer_r3.cn_DomainIdentity)
            {
              cn_id = RRC_CS_DOMAIN_CN_ID;
            }
            else
            {
              cn_id = RRC_PS_DOMAIN_CN_ID;
            }

#ifdef FEATURE_WCDMA_UL_COMPR
            if((dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.laterNonCriticalExtensions.m.downlinkDirectTransfer_r3_add_extPresent) &&
               (dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.laterNonCriticalExtensions.downlinkDirectTransfer_r3_add_ext.m.proprietaryCapabilityRequestPresent) &&
               (dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.laterNonCriticalExtensions.downlinkDirectTransfer_r3_add_ext.proprietaryCapabilityRequest == rrc_DownlinkDirectTransfer_r3_add_ext_IEs_proprietaryCapabilityRequest_true))
            {
              rrc_ul_comp_cap_support = TRUE;
              WRRC_MSG0_HIGH("RRC_ULCOMP: Enabling rrc_ul_comp_cap_support flag");
            }
            else 
            {
              WRRC_MSG0_HIGH("RRC_ULCOMP: Not enabling rrc_ul_comp_cap_support flag");   
            }
#endif
            /* Check whether Security mode procedure is active */
            if (rrcsmc_get_smc_procedure_complete_status(cn_id, rrclcm_get_dl_rb_id(cmd_ptr->cmd.downlink_sdu_ind.lc_id)) == FALSE)
            {
              /* Security mode procedure is not active */
              payload = 
             (unsigned char *) nas_mem_alloc(
                         dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
                        downlinkDirectTransfer_r3.nas_Message.numocts);
              if (payload != NULL)
              {
                /* Memory allocation is sucessful. Copy the payload */
                WCDMA_MEMCPY(payload,
                             dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.downlinkDirectTransfer_r3.nas_Message.numocts,
                             dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.downlinkDirectTransfer_r3.nas_Message.data,
                             dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.downlinkDirectTransfer_r3.nas_Message.numocts);
  
                /* Allocate memory for RRC_DATA_IND command */
                if( (mm_cmd_ptr =  mm_rrc_get_cmd_buf(RRC_DATA_IND)) != NULL)
                {
                  /* Fill in parameters for RRC_DATA_IND command */
           
#ifdef FEATURE_DUAL_SIM
                  mm_cmd_ptr->cmd.rrc_data_ind.as_id = rrc_get_as_id();
#endif

                  if(dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
                    downlinkDirectTransfer_r3.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
                  {   
                    mm_cmd_ptr->cmd.rrc_data_ind.cn_domain_id = RRC_CS_DOMAIN_CN_ID;
                  }
                  else
                  {
                    mm_cmd_ptr->cmd.rrc_data_ind.cn_domain_id = RRC_PS_DOMAIN_CN_ID;

#ifdef FEATURE_INACTIVITY_HANDLING
                    if (rrc_get_state() == RRC_STATE_CELL_DCH) 
                    {
                      WRRC_MSG0_HIGH("Restarting inact timer due to DDT");
                      rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, INACTIVITY_TIMER_IN_MS);
                    }
                    else if (rrc_get_state() == RRC_STATE_CELL_FACH)
                    {
                      MSG_HIGH("Restarting Inactivity timer %d + remain T305 %d T305 actual val %d",INACTIVITY_TIMER_IN_MS,
                               rrctmr_get_remaining_time(RRCTMR_T_305_TIMER),rrc_convert_t305_to_ms(RRC_GET_T305()));
                      /* Restart incativity handling timer */
                      if(rrc_convert_t305_to_ms(RRC_GET_T305()) != RRCTMR_INFINITY_VAL )
                      {
                        rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                                          (unsigned long)(INACTIVITY_TIMER_IN_MS + rrctmr_get_remaining_time(RRCTMR_T_305_TIMER))); 
                      }
                      else
                      {
                        /*If T305 timer was infinity then the timer wouldnt have been started . In thats case start
                            timer of 5 mins plus 24 hrs*/
                        rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                                          (unsigned long)(INACTIVITY_TIMER_IN_MS + RRCTMR_INFINITY_VAL)); 
                      }
                    }
                    (void)rrcmisc_handle_ps_rab_inactivity(1);
#endif /* FEATURE_INACTIVITY_HANDLING */
                  }
                    
                  /*Perform check to see if IDT waiting for L2 Ack*/
                  if(rrcidt_is_idt_waiting_for_l2ack_per_domain(mm_cmd_ptr->cmd.rrc_data_ind.cn_domain_id))
                  {
                      WRRC_MSG1_HIGH("Treating DDT as Implicit L2 Ack for IDT in %d Domain", mm_cmd_ptr->cmd.rrc_data_ind.cn_domain_id);
                      rrcidt_send_open_session_cnf(RRC_OPEN_SESS_SUCCESS,  mm_cmd_ptr->cmd.rrc_data_ind.cn_domain_id);

                    /* Send RRC_SYNC_IND if Fake Security is enabled */
                    if(nv_fake_security_enabled)
                    {
                      /* For CS send the RRC_SYNC_IND for Integrity Protection */
                      rrcidt_send_sync_ind(RRC_INTEGRITY_ESTABLISHED,
                                           mm_cmd_ptr->cmd.rrc_data_ind.cn_domain_id);
                    }
                  }
                  /*End of check*/  
  
                  mm_cmd_ptr->cmd.rrc_data_ind.nas_msg.length =
                    dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
                    downlinkDirectTransfer_r3.nas_Message.numocts;               
  
                  mm_cmd_ptr->cmd.rrc_data_ind.nas_msg.value = payload;
                 
                  WRRC_MSG0_HIGH("Dispatching out MM cmd / Sent RRC_DATA_IND to the MM");
  
                  rrc_send_rrc_message_to_mm(mm_cmd_ptr);               /* Sends the command to MM */
                }
              } /* Memory allocation for paylod failed */
              else
              {
                // Do nothing. Just drop the message.
              } 
            }
            else /* Security mode procedure is active */
            {
              /* Save the received message in the internal buffer until security mode 
                 procedure is completed */
              for (count = 0; count < (MAX_NAS_MESSAGES_FOR_CS_DOMAIN + 
                                       MAX_NAS_MESSAGES_FOR_PS_DOMAIN); count++)
              {
                if(rrcddt_saved_data[count].data_ind_status == FALSE)
                {
                  break;
                }
              }
              if (count < (MAX_NAS_MESSAGES_FOR_CS_DOMAIN + 
                           MAX_NAS_MESSAGES_FOR_PS_DOMAIN) )
              {
                if( rrc_CN_DomainIdentity_cs_domain ==
                    dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
                    downlinkDirectTransfer_r3.cn_DomainIdentity)
                {
                  rrcddt_saved_data[count].data_ind.cn_domain_id = RRC_CS_DOMAIN_CN_ID;
                }
                else if(rrc_CN_DomainIdentity_ps_domain ==
                  dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
                  downlinkDirectTransfer_r3.cn_DomainIdentity)
                {
                  rrcddt_saved_data[count].data_ind.cn_domain_id = RRC_PS_DOMAIN_CN_ID;
                }
                else
                {
                  ERR_FATAL("Invalid CN domain Id",0,0,0);
                }
                /* Update the length of NAS message */

                rrcddt_saved_data[count].data_ind.nas_msg.length = 
                  dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
                  downlinkDirectTransfer_r3.nas_Message.numocts;

                /* Allocate memory for NAS messages */
                rrcddt_saved_data[count].data_ind.nas_msg.value = 
             (unsigned char *) nas_mem_alloc(
                                    dcch_msg_ptr->message.u.
                  downlinkDirectTransfer.u.r3.downlinkDirectTransfer_r3.nas_Message.numocts);
                /* Memory allocation is sucessful. Copy the payload */
                WCDMA_MEMCPY(rrcddt_saved_data[count].data_ind.nas_msg.value,
                             dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.downlinkDirectTransfer_r3.nas_Message.numocts,
                             dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.downlinkDirectTransfer_r3.nas_Message.data,
                             dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.downlinkDirectTransfer_r3.nas_Message.numocts);

                /* Set the status Flag to TRUE indicating that message is saved*/
                rrcddt_saved_data[count].data_ind_status = TRUE;
              }
            }/* Security procedure is active */
          }
        }
        else
        {
          /* Signalling Connection is not open. Reject the message */
          WRRC_MSG1_HIGH("No Signalling Connection is open: %d ", 
                    dcch_msg_ptr->message.u.downlinkDirectTransfer.u.r3.
                    downlinkDirectTransfer_r3.cn_DomainIdentity);

          lc_id = rrclcm_check_ul_rlc_lc_id(    
                  UE_LOGCHAN_DCCH,          /* Uplink Logical Channel Type. */
                  DCCH_AM_RADIO_BEARER_ID,  /* Radio Bearer Id assoicated with
                                              the above Downlink Logical 
                                              Channel type. */
                  UE_MODE_ACKNOWLEDGED_DATA);
                                            /* Mode of transmission for the RLC 
                                               service entity. */
          if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
          {
            WRRC_MSG0_ERROR("Can not find RLC ID for UL RB 2");
          }         
          else /* Valid RLC ID found */
          {
            if(SUCCESS != rrcsend_send_rrc_status_message(
               T_rrc_ProtocolErrorMoreInformation_type1_messageNotCompatibleWithReceiverState,
                                           // The protocol error cause to be 
                                           // placed in the message
               dcch_msg_ptr->message.
               u.downlinkDirectTransfer.u.r3.
               downlinkDirectTransfer_r3.
               rrc_TransactionIdentifier ,   // Set the transaction 
               rrc_ReceivedMessageType_downlinkDirectTransfer,   // Received message identification
               RRC_PROCEDURE_DDT,            // RRC procedure
               lc_id,                        /* Logical Channel Id */
               RRCSEND_L2ACK_NOT_REQUIRED)   /* Indicates whether L2 Ack */
              )                              /* required or not */
                                               
            {
              WRRC_MSG0_ERROR("Unable to xmit RRC STATUS msg");
            }
            else
            { 
              WRRC_MSG0_HIGH("RRC STATUS is sent");
            }
          } /*  Valid RLC ID found */
        } 
        /*lint -restore */
      } 
    }
    else
    {
      WRRC_MSG1_HIGH("Invalid Msg: %d is fwded to DDT Proc",message_choice);
    }
  }
  else if(RRC_SMC_DDT_L2_ACK_IND == cmd_ptr->cmd_hdr.cmd_id)
  {
    for (count = 0; count < (MAX_NAS_MESSAGES_FOR_CS_DOMAIN + 
                                   MAX_NAS_MESSAGES_FOR_PS_DOMAIN); count++)
    {
      /* Check if any messages are queued */
      if(rrcddt_saved_data[count].data_ind_status == TRUE)
      {
        /* Allocate memory for RRC_DATA_IND command */
        if( (mm_cmd_ptr =  mm_rrc_get_cmd_buf(RRC_DATA_IND)) != NULL)
        {
          /* Fill in parameters for RRC_DATA_IND command */

          mm_cmd_ptr->cmd.rrc_data_ind.cn_domain_id = 
            rrcddt_saved_data[count].data_ind.cn_domain_id;


          mm_cmd_ptr->cmd.rrc_data_ind.nas_msg.length =
            rrcddt_saved_data[count].data_ind.nas_msg.length;               


          mm_cmd_ptr->cmd.rrc_data_ind.nas_msg.value = 
            rrcddt_saved_data[count].data_ind.nas_msg.value;
         
#ifdef FEATURE_DUAL_SIM
          mm_cmd_ptr->cmd.rrc_data_ind.as_id = rrc_get_as_id();
#endif

          WRRC_MSG0_HIGH("Dispatching out MM cmd / Sent RRC_DATA_IND to the MM");


          rrc_send_rrc_message_to_mm(mm_cmd_ptr);               /* Sends the command to MM */

          rrcddt_saved_data[count].data_ind_status = FALSE;
        }
      } /* Check if any messages are queued */ 
    }

  }
  else
  {
    WRRC_MSG2_HIGH("Invalid Event: %d or SDU type: %d to DDT",
              cmd_ptr->cmd_hdr.cmd_id,cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
  }


} /* rrcddt_process_downlink_direct_transfer_msg */
/*===========================================================================

FUNCTION rrcddt_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the downlink direct transfer
  procedure.
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (with DOWNLINK DIRECT TRANSFER MESSAGE)


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcddt_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
 
  uint32 count;                               //for counter purposes

  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG1_HIGH_OPT("function rrcddt_procedure_event_handler is called in rrc_state = %d", rrc_state);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_DDT,RRC_INVALID_PROC_SUBSTATE,cmd_ptr->cmd_hdr.cmd_id);

  //process state_change_ind right here just to be sure that we don't
  //miss going to idle at any cost.. clear saved_data_req allocs if present
  if(cmd_ptr->cmd_hdr.cmd_id == RRC_STATE_CHANGE_IND)
  {
    //loop thru list of msgs in the saved_data database and delete them
    //if saved data flag is true, also reset the indication status
    for (count = 0; count < (MAX_NAS_MESSAGES_FOR_CS_DOMAIN + 
                             MAX_NAS_MESSAGES_FOR_PS_DOMAIN); count++)
    {
      if(rrcddt_saved_data[count].data_ind_status == TRUE)
      {
        modem_mem_free(rrcddt_saved_data[count].data_ind.nas_msg.value, 
        MODEM_MEM_CLIENT_NAS_CRIT);
        rrcddt_saved_data[count].data_ind.nas_msg.value = NULL;
        rrcddt_saved_data[count].data_ind_status = FALSE;
      }
    }
  }
  else
  {
    switch (rrc_state)
    {
      case RRC_STATE_DISCONNECTED: 
      case RRC_STATE_CONNECTING:
        /* In these states there is no RRC Connection and Signalling Connection. Hence
           trash the message */
        WRRC_MSG1_HIGH("DDT Msg received in invalid state: %d",
                  rrc_state); 
        break;

      case RRC_STATE_CELL_FACH:    
      case RRC_STATE_CELL_DCH:  
        /* These two states are valid to receive Downlink Direct Transfer message */
        rrcddt_process_downlink_direct_transfer_msg(cmd_ptr); 
        break;

      case RRC_STATE_CELL_PCH:
      case RRC_STATE_URA_PCH:
        /* The logical DCCH is released in these states.Due to race conditions, RRC
           may receive a valid DOWNLINK DIRECT TRANSFER message. Hence process the same
           and send the Payload to NAS. */
        WRRC_MSG0_HIGH("DDT msg is recieved in PCH state");
        rrcddt_process_downlink_direct_transfer_msg(cmd_ptr);
        break;

      default: 
        /* This should never happen */
        ERR_FATAL("Event fwded to DDT in invalid State: %d",rrc_state,0,0);
        break;
    }  
  }
} /* rrcddt_procedure_event_handler */
/*===========================================================================

FUNCTION rrcscr_send_close_session_ind  

DESCRIPTION
  This function builds the RRC_CLOSE_SESSION_IND command with parameters and 
  posts the same to the MM task.
DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type: It returns RRCDT_SUCCESS if it is successfully sends 
  the RRC_CLOSE_SESSION_IND command. Otherwise it returns RRCDT_FAILURE_MAJOR.

===========================================================================*/

rrcdt_status_e_type rrcscr_send_close_session_ind
(
  rrc_cn_domain_identity_e_type       cn_domain_id,     /* CN Domain Id */
  rrc_session_close_cause_e_type      cause
)
{
  mm_cmd_type *cmd_ptr;                      /* Pointer to the MM Command */

  rrcdt_status_e_type status = RRCDT_FAILURE_MAJOR; 
                                             /* The status is initialized to
                                                Failure */

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
  MSG_HIGH("CSFB_HACK: csfb_extended_status = %d, close session for domain %d, PS domain status %d",
            csfb_extended_status,cn_domain_id,rrcdt_int_data.ps_domain_open);
  if((csfb_extended_status == CSFB_STARTED) &&
      (cn_domain_id == RRC_CS_DOMAIN_CN_ID) &&
      (rrc_cs_rab_established)
#ifdef FEATURE_WRLF_SYSTEM_SEL
       /* Send close session ind to NAS if CS domain is released because of T314 timer expiry. */
       && !(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
       )
  {
    rrcrce_post_internal_csfb_abort_cmd();
    WRRC_MSG1_HIGH("CSFB_HACK: csfb_extended_status = %d",csfb_extended_status);
    rrc_cs_rab_established = FALSE;
    return RRCDT_SUCCESS;
  }
#endif

#ifdef FEATURE_INACTIVITY_HANDLING
  if (cn_domain_id == RRC_PS_DOMAIN_CN_ID) 
  {
    rrctmr_stop_timer(RRCTMR_INACTIVITY_TIMER);
  }
#endif /* FEATURE_INTACTIVITY_HANDLING */

#ifdef FEATURE_3GPP_CSFB
  /*CS signaling connection is released, reset CSFB call status flag*/
    if (cn_domain_id == RRC_CS_DOMAIN_CN_ID) 
    {
      rrc_csfb_call_status = FALSE;
    }
#endif

  /* Allocate memory for RRC_OPEN_SESSION_CNF command */
  if( (cmd_ptr =  mm_rrc_get_cmd_buf(RRC_CLOSE_SESSION_IND)) != NULL)
  {
    /* Fill in parameters fro RRC_OPEN_SESSION_CNF command */
    cmd_ptr->cmd.rrc_close_session_ind.cn_domain_id = cn_domain_id ;


    rrcsmc_update_ciphering_config_info_for_signalling_release(cn_domain_id);
    rrcsmc_update_sync_ind_info_for_signalling_release(cn_domain_id);

     // No causes defined in the interface file
    cmd_ptr->cmd.rrc_close_session_ind.cause = cause;

#ifdef FEATURE_DUAL_SIM
    cmd_ptr->cmd.rrc_close_session_ind.as_id = rrc_get_as_id();
#endif
    WRRC_MSG0_HIGH_OPT("Dispatching out MM cmd RRC_CLOSE_SESSION_IND");
    rrc_send_rrc_message_to_mm(cmd_ptr);               /* Sends the command to MM */

#ifdef FEATURE_DUAL_SIM
    /* Incase there is no rab release associated with CS domain closure, we should check and 
     * allow call on the other rat when CS domain is closed
     */
    if (cn_domain_id == RRC_CS_DOMAIN_CN_ID) 
    {
      rrc_check_send_change_priority();
    }
#endif
 
    status = RRCDT_SUCCESS;            /* sets the status to success */
  }    
  return (status);

} /* rrcscr_send_close_session_ind */

/*===========================================================================

FUNCTION rrcscr_reset_signalling_connection_status

DESCRIPTION
  This function sets both CS and PS signalling Connections to release status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcscr_reset_signalling_connection_status
(
  void                    
)
{

  /* Initialize the flag to show that a signalling session is not open to
   * the CS domain
   */
  rrcdt_int_data.cs_domain_open = FALSE;

  /* Initialize the flag to show that a signalling session is not open to
   * the PS domain
   */
  rrcdt_int_data.ps_domain_open = FALSE;

} /* rrcscr_reset_signalling_connection_status */

/*===========================================================================

FUNCTION rrcscr_check_active_rabs_in_sig_conn_rel_domain

DESCRIPTION
  This function checks if active rabs are present in the CN Domain for which
  we got Signalling_Connection_Release


DEPENDENCIES
  None.

RETURN VALUE
  rrcscr_sig_conn_statue_e_type: it returns RRCSCR_RAB_PRESENT_IN_SIG_DOMAIN
  if successful, else returns RRCSCR_NO_RAB_PRESENT_IN_SIG_DOMAIN

===========================================================================*/
static rrcscr_sig_conn_rel_status_e_type rrcscr_check_active_rabs_in_sig_conn_rel_domain
(
  rrc_cn_domain_identity_e_type cn_domain_id
)
{
  uint32 count,i;                  /* count for counter purposes          */

  /*loop through established_rabs struct and check to see if any active rabs
    are present in the domain for which we are trying to do a SCR*/
  for ( count=0; count<MAX_RAB_TO_SETUP; count++ )
  {
    if (rrc_est_rabs.rabs[count].rab_id != RRC_INVALID_RAB_ID)
    { 
      for (i=0; i< rrc_est_rabs.rabs[count].num_rbs_for_rab; i++)
      {
        if ((rrc_est_rabs.rabs[count].rb_for_rab[i].rb_status ==
             RRC_RB_STARTED) && (rrc_est_rabs.rabs[count].cn_domain == 
          cn_domain_id))
        {
          //we can return right away
          return RRCSCR_RAB_PRESENT_IN_SIG_DOMAIN;
        }
        else
        {
          //this rab does not have any RB's for the domain which is being released.
          //therefore continue to the next RAB as all RB's in a RAB will belong to 
          //same CN domain
          break; 
        }
      }
    }
  }

  return RRCSCR_NO_RAB_PRESENT_IN_SIG_DOMAIN;
}/*rrcscr_check_active_rabs_in_sig_conn_rel_domain*/
/*===========================================================================

FUNCTION rrcscr_process_signaling_connection_release_msg  

DESCRIPTION
  This function checks whether RRC_DOWNLINK_SDU_IND contains Signalling
  connection release message.If it contains Signalling Connection Release
  message, it will processes the same and send RRC_CLOSE_SESSION_IND to the MM.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

static void rrcscr_process_signaling_connection_release_msg
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;    /* Pointer to Downlink DCCH */

  rlc_lc_id_type lc_id;                      /* Local variable to store 
                                                RLC Logical Channel Id */
  rrc_cn_domain_identity_e_type cn_domain_id; /* Local variable for CN Domain */

  uint8 message_choice;                       /* Local variable for message
                                                 choice */

  /* Set the protocol error to FALSE */
  rrc_protocol_error_reject = FALSE;

  /* Assert the receieved message is of type rrc_DL_CCCH_Message */

  if((RRC_DOWNLINK_SDU_IND == cmd_ptr->cmd_hdr.cmd_id) && 
     (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_DCCH_Message_PDU) )
  {
    message_choice = 
      rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                            cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

    if (cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL)
    {
      /* Get the OTA message pointer */
      dcch_msg_ptr = (rrc_DL_DCCH_Message *) 
                     cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
    }
    else
    {
      /* ASN1 decoding failed */
      rrc_protocol_error_reject = TRUE;
    }
 
    /* Check the received message is Signalling Connection Release message */
    if( message_choice == 
        T_rrc_DL_DCCH_MessageType_signallingConnectionRelease)
    {
      WRRC_MSG0_HIGH("Signalling Connection Rel is received");

      if(rrc_protocol_error_reject == TRUE) /* DCCH ASN1 decoding failed */
      {
        rrc_log_protocol_error( (uint8) rrc_get_state(),
                                (uint8)RRC_PROCEDURE_SCRR,
                                (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                                (uint8) RRCLOG_PROTOCOL_ERROR_ViolationOrEncodingError 
                              );

        lc_id = rrclcm_check_ul_rlc_lc_id(    
                UE_LOGCHAN_DCCH,          /* Uplink Logical Channel Type. */
                DCCH_AM_RADIO_BEARER_ID,  /* Radio Bearer Id assoicated with
                                            the above Downlink Logical 
                                            Channel type. */
                UE_MODE_ACKNOWLEDGED_DATA);
                                          /* Mode of transmission for the RLC 
                                             service entity. */
        if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
        {
          WRRC_MSG0_HIGH("Can't find RLC ID for UL RB 2");
        } 
        else /* Valid RLC ID found */
        {
          if(SUCCESS != rrcsend_send_rrc_status_message(
             T_rrc_ProtocolErrorMoreInformation_type1_asn1_ViolationOrEncodingError,
                                           // The protocol error cause to be placed in
                                           // the message
             0,                            // Set the transaction id to 0 since 
                                           // ASN1 decoding failed
             rrc_ReceivedMessageType_signallingConnectionRelease,
                                           // Received Msg Identifcation
             RRC_PROCEDURE_SCR,            // RRC procedure
             lc_id,                        /* Logical Channel Id */
             RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                           /* Indicates whether L2 Ack 
                                              required or not */
          {
            WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
          } 
          
        } /* Valid RLC ID found */
        
      } 
      else /* ASN1 decoding is successful */
      {
        /*lint -save -e613 */
        if(RRC_CHECK_MSG_TYPE(dcch_msg_ptr->message.u.signallingConnectionRelease,
                      T_rrc_DL_DCCH_MessageType_signallingConnectionRelease))
        {
          /* Critical extensions are not supported */
          WRRC_MSG0_ERROR("Critical extensions are not supported");
          return;
        }
        
        /* Update CN Domain ID */
        if(dcch_msg_ptr->message.u.signallingConnectionRelease.u.r3.
              signallingConnectionRelease_r3.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
        {
          cn_domain_id = RRC_CS_DOMAIN_CN_ID;
        }
        else
        {
          cn_domain_id  = RRC_PS_DOMAIN_CN_ID;
        }

#ifdef FEATURE_QSH_EVENT_METRIC
        rrc_qsh_log_scrr_params(cn_domain_id);
#endif

        if(rrcscr_check_active_rabs_in_sig_conn_rel_domain(cn_domain_id) == 
           RRCSCR_RAB_PRESENT_IN_SIG_DOMAIN)
        {
          rrc_log_protocol_error( (uint8) rrc_get_state(),
                                  (uint8) RRC_PROCEDURE_SCRR,
                                  (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                                  (uint8) RRCLOG_PROTOCOL_ERROR_ViolationOrEncodingError 
                                );
          lc_id = rrclcm_check_ul_rlc_lc_id(    
                  UE_LOGCHAN_DCCH,          /* Uplink Logical Channel Type. */
                  DCCH_AM_RADIO_BEARER_ID,  /* Radio Bearer Id assoicated with
                                              the above Downlink Logical 
                                              Channel type. */
                  UE_MODE_ACKNOWLEDGED_DATA);
                                              /* Mode of transmission for the RLC 
                                                 service entity. */
          if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
          {
            WRRC_MSG0_HIGH("Can't find RLC ID for UL RB 2");
          } 
          else /* Valid RLC ID found */
          {
            if(SUCCESS != rrcsend_send_rrc_status_message(
               T_rrc_ProtocolErrorMoreInformation_type1_messageNotCompatibleWithReceiverState,
                                             // The protocol error cause to be placed in
                                             // the message
               dcch_msg_ptr->message.u.signallingConnectionRelease.u.r3.
               signallingConnectionRelease_r3.rrc_TransactionIdentifier,                            
                                            // Set the transaction id to 0 since 
               rrc_ReceivedMessageType_signallingConnectionRelease,
                                             // Received Msg Identifcation
               RRC_PROCEDURE_SCR,            // RRC procedure
               lc_id,                        /* Logical Channel Id */
               RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                             /* Indicates whether L2 Ack 
                                                required or not */
            {
              WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
            } 
              
          } /* Valid RLC ID found */
        }
        else//only if active rab is not found in CN Domain we r trying to release
        {
   
          /* Check for open signalling connection match */
          if((( RRC_CS_DOMAIN_CN_ID == cn_domain_id) &&
               (rrcdt_int_data.cs_domain_open)) ||
             (( RRC_PS_DOMAIN_CN_ID == cn_domain_id) && 
                (rrcdt_int_data.ps_domain_open)) )
          {        
             
            /*On receiving SCR from NW when there is a pending close session req, RRC should send Close session cnf*/
            if((cn_domain_id == RRC_CS_DOMAIN_CN_ID) &&
               (rrcscrr_saved_cs_close_session_req.mui_status == RRCSCRR_MUI_CLOSE_SESSION_CNF))
            {
              rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_SUCCESS,RRC_CS_DOMAIN_CN_ID);
              WRRC_MSG0_HIGH("Interpreting SCR message for CS domain as RLC ACK for SCRI, CS DOMAIN CLOSE SESSION CNF WITH SUCCESS - DELETE ALL SAVED UDTs");
              rrcudt_delete_all_saved_data_req(RRC_CS_DOMAIN_CN_ID);
              rrcdt_int_data.cs_domain_open = FALSE;
              if(rrcudt_any_saved_data_req_for_cn_domain(RRC_PS_DOMAIN_CN_ID) == FALSE)
              {
                WRRC_MSG0_HIGH("There is no saved UDT for PS domain, set udt substate to init");
                rrcudt_substate = RRCUDT_INITIAL;
              }
              
              if(rrcscrr_saved_ps_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE)
              {
                rrcscrr_substate = RRCSCRR_WAIT_FOR_L2ACK; 
              }
              else
              {
                rrcscrr_substate = RRCSCRR_INITIAL; 
              }
              rrcscrr_delete_close_session_req(RRC_CS_DOMAIN_CN_ID);
            }
            else if(( RRC_PS_DOMAIN_CN_ID == cn_domain_id) &&  
                      (rrcscrr_saved_ps_close_session_req.mui_status == RRCSCRR_MUI_CLOSE_SESSION_CNF))     
            {
              rrcscrr_send_close_session_cnf(RRC_CLOSE_SESS_SUCCESS, RRC_PS_DOMAIN_CN_ID);
              WRRC_MSG0_HIGH("Interpreting SCR message for PS domain as RLC ACK for SCRI, PS DOMAIN CLOSE SESSION CNF WITH SUCCESS - DELETE ALL SAVED UDTs");
              rrcudt_delete_all_saved_data_req(RRC_PS_DOMAIN_CN_ID);
              rrcdt_int_data.ps_domain_open = FALSE;
              if(rrcudt_any_saved_data_req_for_cn_domain(RRC_CS_DOMAIN_CN_ID) == FALSE)
              {
                WRRC_MSG0_HIGH("There is no saved UDT for CS domain, set udt substate to init");
                rrcudt_substate = RRCUDT_INITIAL;
              }
              if(rrcscrr_saved_cs_close_session_req.mui_status != RRCSCRR_MUI_STATUS_FALSE)
              {
                rrcscrr_substate = RRCSCRR_WAIT_FOR_L2ACK; 
              }
              else
              {
                rrcscrr_substate = RRCSCRR_INITIAL; 
              }
              rrcscrr_delete_close_session_req(RRC_PS_DOMAIN_CN_ID);
            }
            else if(RRCDT_SUCCESS == rrcscr_send_close_session_ind(cn_domain_id, RRC_MAX_SESSION_CLOSE_CAUSE))
            {
              WRRC_MSG0_HIGH("Sent RRC_CLOSE_SESSION_IND");
              switch(cn_domain_id)
              {
                case RRC_CS_DOMAIN_CN_ID:
                  rrcidt_clear_idt_data_for_race_conditions(RRC_CS_DOMAIN_CN_ID);
                  /* Set the Signalling Connection status to closed */
                  rrcdt_int_data.cs_domain_open = FALSE;
                  break;
     
                case RRC_PS_DOMAIN_CN_ID:
                  /* Set the Signalling Connection Status to closed */
                  rrcidt_clear_idt_data_for_race_conditions(RRC_PS_DOMAIN_CN_ID);
                  rrcdt_int_data.ps_domain_open = FALSE;
                  /* Abort Dormancy even if SCRI for CLOSE_SESSION_REQ is sended */
                  rrcdormancy_check_and_abort_dormancy_req(RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);
                  break;
    
                default:
                  ERR_FATAL("This should never_happen",0,0,0);
                  break;
              }
            }
            else
            {
              WRRC_MSG0_ERROR("Failed to post RRC_CLOSE_SESSION_IND");
            }
          }
          else
          {
            /* Signalling Connection is not open. Trash the message */
            WRRC_MSG1_HIGH("No Signalling Connection is open %d", cn_domain_id);
          } 
        }
        /*lint -restore */
      } 
    }
    else
    {
      WRRC_MSG1_HIGH("Invalid Msg: %d is forwarded to SCR Proc",
                message_choice);
    }
  }
  else
  {
    WRRC_MSG2_HIGH("Invalid Event: %d or SDU type: %d to SCR",
              cmd_ptr->cmd_hdr.cmd_id,cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
  }

} /* rrcscr_process_signaling_connection_release_msg */
/*===========================================================================

FUNCTION rrcscr_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the signalling Connection Release
  procedure.
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (with SIGNALLING CONNECTION RELEASE MESSAGE)


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcscr_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
 
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG1_HIGH_OPT("function rrcscr_procedure_event_handler is called in rrc_state = %d",rrc_state);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_SCR,RRC_INVALID_PROC_SUBSTATE,cmd_ptr->cmd_hdr.cmd_id);

  switch (rrc_state)
  {
    case RRC_STATE_DISCONNECTED: 
    case RRC_STATE_CONNECTING:
      /* In these states there is no RRC Connection and Signalling Connection. Hence
         trash the message */
      WRRC_MSG1_HIGH("SCR Msg is rcvd in invalid state: %d",
                rrc_state); 
      break;

    case RRC_STATE_CELL_FACH:    
    case RRC_STATE_CELL_DCH:  
      /* These two states are valid to receive Downlink Direct Transfer message */
      rrcscr_process_signaling_connection_release_msg(cmd_ptr); 
      break;

    case RRC_STATE_CELL_PCH:
    case RRC_STATE_URA_PCH:
      /* The logical DCCH is released in these states.Due to race conditions, RRC
         may receive a valid SIGNALING CONNECTION RELEASE message. Hence process 
         the same and send the Payload to NAS. */
      WRRC_MSG0_HIGH("SCR msg is recievd in PCH state");
      rrcscr_process_signaling_connection_release_msg(cmd_ptr);
      break;

    default: 
      /* This should never happen */
      MSG_FATAL("Event fwded to SCR in invalid State: %d",rrc_state,0,0);
      break;
   }  
} /* rrcscr_procedure_event_handler */

/*===========================================================================

FUNCTION rrcscr_release_signalling_connection

DESCRIPTION
  This function checks whether Signalling connection exist for the particular
  CN Domain and if it exits, then sends RRC_CLOSE_SESSION_IND to the MM. Also
  sets the local variable for signalling connection status to FALSE.


DEPENDENCIES
  None.

RETURN VALUE
  rrcscr_sig_conn_statue_e_type: it returns RRCSCR_SIG_CONN_RELEASED if there
  exits a signalling connection for the CN domain specified in the formal 
  parameter and if RRCSCR procedure successfully sends RRC_CLOSE_SESSION_IND
  to the MM. Otherwise it retuns RRCSCR_SIG_CONN_NOT_EXIST.

===========================================================================*/

rrcscr_sig_conn_rel_status_e_type rrcscr_release_signalling_connection
(
  rrc_cn_domain_identity_e_type cn_domain_id
)
{
  rrcscr_sig_conn_rel_status_e_type status = RRCSCR_SIG_CONN_NOT_EXIST;
                                   /* Local parameter to store the status */
  
  switch(cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID:
      /* Check whether Signalling Connection for CS Domain is open,ps_domain_open doesnt 
      * give correct status,So make sure IDT procedure is not in progress first case 
      * says that open session is received, eg waiting for CU
      * second condition says that we are waiting for l2ack
      */
      if((rrcdt_int_data.cs_domain_open == TRUE) &&
         (rrcidt_is_idt_active(RRC_CS_DOMAIN_CN_ID) == FALSE))
      {
        if(RRCDT_SUCCESS == rrcscr_indicate_close_session(cn_domain_id, RRC_MAX_SESSION_CLOSE_CAUSE))
        {
          WRRC_MSG0_HIGH("Sent RRC_CLOSE_SESSION_IND, CS DOMAIN CLOSE SESSION CNF WITH SUCCESS - DELETE ALL SAVED UDTs");

          /* Set the Signalling connection status to closed */
          rrcdt_int_data.cs_domain_open = FALSE;

          status = RRCSCR_SIG_CONN_RELEASED;
          rrcudt_delete_all_saved_data_req(RRC_CS_DOMAIN_CN_ID);
          if(rrcudt_any_saved_data_req_for_cn_domain(RRC_PS_DOMAIN_CN_ID) == FALSE)
          {
            WRRC_MSG0_HIGH("There is no saved UDT for PS domain, set udt substate to init");
            rrcudt_substate = RRCUDT_INITIAL;
          }
        }
        else
        {
          WRRC_MSG0_ERROR("Failed to post RRC_CLOSE_SESSION_IND");
        }
      }
      else
      {
        MSG_HIGH("No need to close Doamin =%d,domain_open=%d,IDT active =%d",
           cn_domain_id,rrcdt_int_data.cs_domain_open,rrcidt_is_idt_active(RRC_CS_DOMAIN_CN_ID));
      }

      break;

    case RRC_PS_DOMAIN_CN_ID:
      /* Check whether Signalling Connection for PS Domain is open,ps_domain_open doesnt 
      * give correct status,So make sure IDT procedure is not in progress first case 
      * says that open session is received, eg waiting for CU
      * second condition says that we are waiting for l2ack
      */
      if((rrcdt_int_data.ps_domain_open == TRUE)&&
         (rrcidt_is_idt_active(RRC_PS_DOMAIN_CN_ID) == FALSE))
      {
        if(RRCDT_SUCCESS == rrcscr_indicate_close_session(cn_domain_id, RRC_MAX_SESSION_CLOSE_CAUSE))
        {
          WRRC_MSG0_HIGH("Sent RRC_CLOSE_SESSION_IND, PS DOMAIN CLOSE SESSION CNF WITH SUCCESS - DELETE ALL SAVED UDTs");

          /* Set the Signalling connection status to closed */
          rrcdt_int_data.ps_domain_open = FALSE;

          status = RRCSCR_SIG_CONN_RELEASED;
          rrcudt_delete_all_saved_data_req(RRC_PS_DOMAIN_CN_ID);
          if(rrcudt_any_saved_data_req_for_cn_domain(RRC_CS_DOMAIN_CN_ID) == FALSE)
          {
            WRRC_MSG0_HIGH("There is no saved UDT for CS domain, set udt substate to init");
            rrcudt_substate = RRCUDT_INITIAL;
          }
        }
        else
        {
          WRRC_MSG0_ERROR("Failed to post RRC_CLOSE_SESSION_IND");
        }
      }
      else
      {
        MSG_HIGH("No need to close Doamin =%d,domain_open=%d,IDT active =%d",
           cn_domain_id,rrcdt_int_data.ps_domain_open,rrcidt_is_idt_active(RRC_PS_DOMAIN_CN_ID));
      }
      break;

    default:
      ERR_FATAL("This should never happen",0,0,0);
      break;
  }
  return (status);
} /* rrcscr_release_signalling_connection */


/*===========================================================================

FUNCTION rrcdt_get_nv_items

DESCRIPTION
  This function gets NV item NV_FAKE_SECURITY_ENABLED which 
  used to determine if Fake Security will be sent.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcdt_get_nv_items
(
  sys_modem_as_id_e_type wrrc_as_id
)
{
  /*read fake security ENABLE/DISABLE flag from NV*/
  wnv_get_nv_msim(WNV_ID_RRC_FAKE_SECURITY_ENABLED_I, &nv_fake_security_enabled, sizeof(nv_fake_security_enabled),wrrc_as_id);
  WRRC_MSG1_HIGH("NV_FAKE_SECURITY is %d", nv_fake_security_enabled);

  /*if integrity is enabled, then fake security should be disabled.. only one of the 2
    nv items should be turned on*/
  if(nv_integrity_enabled == 1)
  {
    nv_fake_security_enabled = 0;
  }
} 

/*===========================================================================

FUNCTION rrcdt_is_idt_waitingfor_l2ack

DESCRIPTION
  This function is used by other procedures like CU to see if dt has queued some messages to RLC

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

boolean rrcdt_is_idt_waitingfor_l2ack(void)
{

   boolean ret_val=FALSE;
   if(rrcidt_substate == RRCIDT_WAIT_FOR_L2ACK)
   {
       ret_val =  TRUE;
   }
   return (ret_val);
}

/*===========================================================================

FUNCTION is_ue_oos_and_em_call_attempted

DESCRIPTION
  This function takes UE to idle if UE is OOS (in Connected Mode) and an emergency call 
  is attempted
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: UE is OOS and EM call is attempted. 
  FALSE: otherwise

===========================================================================*/
boolean is_ue_oos_and_em_call_attempted
(
  rrc_cmd_type    *cmd_ptr, 
  rrc_proc_e_type rrc_proc
)
{
  boolean status = FALSE;

  /* If UE is OOS in Connected Mode and an emergency call is attempted then
   * take UE to idle
   */
  if(RRC_NOT_CAMPED_ON == rrc_ccm_get_curr_camping_status(rrc_proc))
  {
    switch (rrc_proc) 
    {
      case RRC_PROCEDURE_IDT:
        if(cmd_ptr->cmd.open_sess_req.est_cause == RRC_EST_EMERGENCY_CALL)
        {
          WRRC_MSG1_HIGH("idt: UE OOS in conn st %d when EM call attmpted. Tx to discon", 
                                                                        rrc_get_state());

          /* Send initiate cell sel command to CSP so that UE is taken to Idle */
          (void)rrc_transition_to_disconnected_state(RRC_PROCEDURE_IDT, RRC_TX_TO_DISCON_EM_CALL);

          /* Initialize state of the variables */
          rrcidt_delete_open_session_req(RRC_CS_DOMAIN_CN_ID);  
          rrcidt_delete_open_session_req(RRC_PS_DOMAIN_CN_ID);  
          rrc_release_cause = RRC_REL_DEEP_FADE;
          status = TRUE;
        }
        break;

      case RRC_PROCEDURE_UDT:
        if(cmd_ptr->cmd.data_req.is_emergency_call == TRUE)
        {
          WRRC_MSG1_HIGH("udt: UE OOS in conn st %d when EM call attmpted. Tx to discon", 
                                                                    rrc_get_state());

          /* Send initiate cell sel command to CSP so that UE is taken to Idle */
          (void)rrc_transition_to_disconnected_state(RRC_PROCEDURE_UDT, RRC_TX_TO_DISCON_EM_CALL);

          /* Initialize state of the variables */
          rrcudt_delete_all_saved_data_req(RRC_CS_DOMAIN_CN_ID);
          rrcudt_delete_all_saved_data_req(RRC_PS_DOMAIN_CN_ID);
          rrcudt_substate = RRCUDT_INITIAL;
          rrc_release_cause = RRC_REL_DEEP_FADE;
          status = TRUE;
        }
        break;

      default: 
        WRRC_MSG1_ERROR("ERR: Incorrect proc_id %d passed to the func", rrc_proc);
        break;
    }/* end switch statement */
  } /* end if statement */

  return (status);
} /* is_ue_oos_and_em_call_attempted */

/*===========================================================================

FUNCTION rrcdt_init_procedure

DESCRIPTION
  This function initializes everything that the Direct Transfer procedures
  needs to have initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcdt_init_procedure(void)

{
  uint8 count;

  for (count = 0; count <MAX_NAS_MESSAGES_FOR_CS_DOMAIN; count++ )
  {
    rrcudt_cs_saved_data[count].mui_status = FALSE;
    rrcudt_cs_saved_data[count].data_req_status = FALSE;
  }

  for (count = 0; count <MAX_NAS_MESSAGES_FOR_PS_DOMAIN; count++ )
  {
    rrcudt_ps_saved_data[count].mui_status = FALSE;
    rrcudt_ps_saved_data[count].data_req_status = FALSE;
  }

  for (count = 0; count < (MAX_NAS_MESSAGES_FOR_CS_DOMAIN + 
                           MAX_NAS_MESSAGES_FOR_PS_DOMAIN); count++)
  {
    rrcddt_saved_data[count].data_ind_status = FALSE;
  }

  /* Initialize close_session req flag for PS/CS domain to false  */
  rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status = FALSE;
  rrcscrr_saved_cs_close_session_req.close_sess_req_status = FALSE;
  rrcscrr_saved_cs_close_session_req.mui_status = RRCSCRR_MUI_STATUS_FALSE;
  rrcscrr_saved_cs_close_session_req.rrc_proc= RRC_PROCEDURE_NONE;

  rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status = FALSE;
  rrcscrr_saved_ps_close_session_req.close_sess_req_status = FALSE;
  rrcscrr_saved_ps_close_session_req.mui_status = RRCSCRR_MUI_STATUS_FALSE;
  rrcscrr_saved_ps_close_session_req.rrc_proc = RRC_PROCEDURE_NONE;

  /* Initialize open_session_req flag for PS/CS domain to false  */
  rrcidt_cs_saved_open_session_req.mui_status = FALSE;
  rrcidt_cs_saved_open_session_req.open_sess_req_status = FALSE;
  
  rrcidt_ps_saved_open_session_req.mui_status = FALSE;
  rrcidt_ps_saved_open_session_req.open_sess_req_status = FALSE;

  /* variables which store the establishment cause received from NAS */
  rrcidt_cs_saved_open_session_req.est_cause=RRC_EST_TERM_UNKNOWN;
  rrcidt_ps_saved_open_session_req.est_cause=RRC_EST_TERM_UNKNOWN;

  /* Initialize the Initial Direct Transfer Procedure variable
   which keeps track whether CU intiated in CELL/URA PCH*/
  rrcidt_cs_saved_open_session_req.cu_init_by_idt = FALSE;
  rrcidt_ps_saved_open_session_req.cu_init_by_idt = FALSE;

  /* Initialize the Initial Direct Transfer Procedure variable
   which keeps track of fresh START value need to be carried in IDT*/
  rrcidt_cs_saved_open_session_req.fresh_start_value_needed = FALSE;
  rrcidt_ps_saved_open_session_req.fresh_start_value_needed = FALSE;
  
  rrcidt_cs_saved_open_session_req.open_session_req.call_type = RRC_CALL_TYPE_NONE;
  
  /* Initialize the Initial Direct Transfer Procedure variable
   which keeps track Selected PLMN I.D*/
  rrcidt_cs_saved_open_session_req.selected_plmn_id.mcc[0]= 0;
  rrcidt_cs_saved_open_session_req.selected_plmn_id.mcc[1]= 0;
  rrcidt_cs_saved_open_session_req.selected_plmn_id.mcc[2]= 0;
  rrcidt_cs_saved_open_session_req.selected_plmn_id.mnc[0]= 0;
  rrcidt_cs_saved_open_session_req.selected_plmn_id.mnc[1]= 0;
  rrcidt_cs_saved_open_session_req.selected_plmn_id.mnc[2]= 0;
  rrcidt_cs_saved_open_session_req.selected_plmn_id.num_mnc_digits = 0;
  rrcidt_ps_saved_open_session_req.selected_plmn_id = 
      rrcidt_cs_saved_open_session_req.selected_plmn_id;
  rrcidt_est_cause_info.valid = FALSE;
  rrcidt_est_cause_info.est_cause = RRC_EST_TERM_UNKNOWN;
  rrcidt_est_cause_info.to_be_sent_by=RRC_PROCEDURE_NONE;
  rrcidt_est_cause_info.last_proc_sent = RRC_PROCEDURE_NONE;

  rrcidt_est_cause_info.call_type = RRC_CALL_TYPE_NONE;


  /* Initialize the flag to show that a signalling session is not open to
   * the CS domain
   */
  rrcdt_int_data.cs_domain_open = FALSE;

  /* Initialize the flag to show that a signalling session is not open to
   * the PS domain
   */
  rrcdt_int_data.ps_domain_open = FALSE;

  /* Initialize the Initial Direct Transfer Procedure substate
   */ 
  rrcidt_substate = RRCIDT_INITIAL;

  /* Initailize the Uplink Direct Transfer Procedure substate 
  */
  rrcudt_substate = RRCUDT_INITIAL;
  /* Initial Signalling Connection Release Request Procedure substate
   */
  rrcscrr_substate = RRCSCRR_INITIAL;

  /* Register with the State Change Manager for all state transitions 
   * from any Connected state to Idle Disconnected state for
   * Initial Direct Transfer Procedure.*/
  rrcscmgr_register_for_scn
  (
    RRC_PROCEDURE_IDT,   
    RRC_STATE_WILDCARD,
    RRC_STATE_DISCONNECTED
  );

  /* Register with the State Change Manager for all state transitions 
   * from any Connected state to Idle Disconnected state for
   * Uplink Direct Transfer Procedure.*/
  rrcscmgr_register_for_scn
  (
    RRC_PROCEDURE_UDT,   
    RRC_STATE_WILDCARD,
    RRC_STATE_DISCONNECTED
  );

  /* Register with the State Change Manager for all state transitions 
   * from any Connected state to Idle Disconnected state for
   * Signalling Connection Release Request Procedure.*/
  rrcscmgr_register_for_scn
  (
    RRC_PROCEDURE_SCRR,
    RRC_STATE_WILDCARD,
    RRC_STATE_DISCONNECTED
  );

  /* Register with the State Change Manager for all state transitions 
   * from any Connected state to Idle Disconnected state for
   * Downlink Direct Transfer Procedure.*/
  rrcscmgr_register_for_scn
  (
    RRC_PROCEDURE_DDT,
    RRC_STATE_WILDCARD,
    RRC_STATE_DISCONNECTED
  );
} /* rrcdt_init_procedure */

/*===========================================================================

FUNCTION rrcdt_check_if_emergency_call_initiated

DESCRIPTION
  This function checks whether emergency call has been initiated for Initial Direct Transfer and Uplink Direct Transfer

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Emergency call is initiated
  FALSE: Otherwise

===========================================================================*/
boolean rrcdt_check_if_emergency_call_initiated()
{
  boolean status=FALSE;
  int i;

  /*If IDT is enqueued in RLC or buffered at DT procedure, return TRUE*/
  if(((rrcidt_cs_saved_open_session_req.mui_status== TRUE) ||
     (rrcidt_cs_saved_open_session_req.open_sess_req_status == TRUE))
     &&
     (rrcidt_cs_saved_open_session_req.est_cause==RRC_EST_EMERGENCY_CALL)
    )
  {
    MSG_HIGH("IDT is in process for Ecall mui_status=%d,open_sess_req_status =%d, est_cause =%d",rrcidt_cs_saved_open_session_req.mui_status,
                                                                                  rrcidt_cs_saved_open_session_req.open_sess_req_status,
                                                                                  rrcidt_cs_saved_open_session_req.est_cause);
    return (TRUE);
  }
  /*Check to see if is_emergency_call is set in any any UDT message*/
  if(rrcudt_substate==RRCUDT_WAIT_FOR_L2ACK)
  {
    for(i=0; i < MAX_NAS_MESSAGES_FOR_CS_DOMAIN ;i++)
    {
      if(((rrcudt_cs_saved_data[i].mui_status == TRUE) ||
         (rrcudt_cs_saved_data[i].data_req_status == TRUE))
         &&
         (rrcudt_cs_saved_data[i].data_req.is_emergency_call == TRUE)
        )
      {
        MSG_HIGH("UDT is in process for Ecall mui_status=%d,data_req_status =%d, is_emergency_call =%d",rrcudt_cs_saved_data[i].mui_status,
                                                                                  rrcudt_cs_saved_data[i].data_req_status,
                                                                                  rrcudt_cs_saved_data[i].data_req.is_emergency_call);
        return (TRUE);
      }
    }
  }
  return status;
}/*rrcdt_check_if_emergency_call_initiated*/


/*==========================================================================

FUNCTION rrcdt_fill_plmn_id_in_idt

DESCRIPTION
  This function will fill's selected PLMN I.D in
  IDT message.
  .
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcdt_fill_plmn_id_in_idt
(
  /* PLMN I.D to be sent over the air */
  rrc_PLMN_Identity *plmn_id_msg_ie,
  /*CN Domain I.Dfor which PLMN I.D to be filled*/
  rrc_cn_domain_identity_e_type cn_id
)
{
  uint32 i, n_index;                            /* Temporary variables for indexing */    
  rrc_plmn_identity_type slelected_plmn_id;     /* Temporary varibale to update linked list members */       
  rrc_MCC *temp_mcc = NULL;
  rrc_MNC *temp_mnc = NULL;

  WRRC_MSG1_HIGH("Fill PLMN I.D from CN I.D= %d",cn_id);
  /*Get the slelected_plmn_id from the stored PLMN depending on CN domain*/
  if(cn_id == RRC_CS_DOMAIN_CN_ID)
  {
    slelected_plmn_id = rrcidt_cs_saved_open_session_req.selected_plmn_id;
  }
  else if(cn_id == RRC_PS_DOMAIN_CN_ID)
  {
    slelected_plmn_id = rrcidt_ps_saved_open_session_req.selected_plmn_id;
  }
  else
  {
    /*This is error handling, whic will not be hit as there are many checks
      in open_session_req handling  */
    WRRC_MSG1_HIGH("Wrong CN I.D passed %d,Fill CS Domain PLMN",cn_id);
    slelected_plmn_id = rrcidt_cs_saved_open_session_req.selected_plmn_id;
  }


  PRINT_PLMN_ID(slelected_plmn_id)

  /*Check for number of digits in MNC either its 2 or 3 & then  print */
  if((slelected_plmn_id.num_mnc_digits != 3)&&
    (slelected_plmn_id.num_mnc_digits != 2))
  {
    /*Lets set the default value to 3, This is error handling,
    whic will not be hit as NAS takes care of setting number of 
    MNC digits to either 2 or 3. This check is good since it directly 
    effects ASN.1 encoding if this feild is set to more than 3*/
    slelected_plmn_id.num_mnc_digits = 3;
    WRRC_MSG1_ERROR("NWS:Invalid Number of MNC digits = %d",slelected_plmn_id.num_mnc_digits);
  }

  /* Update the pointer to the linked list */
  temp_mcc =  &(plmn_id_msg_ie->mcc);

  temp_mcc->n = RRC_MAX_MCC_DIGITS;
  n_index = 0;
  ALLOC_ASN1ARRAY2(&enc_ctxt,temp_mcc,rrc_Digit);
 
  for(i = 0;i < RRC_MAX_MCC_DIGITS;i++)
  {
    temp_mcc->elem[n_index] = slelected_plmn_id.mcc[i];
    n_index++;
  }

  /* Update the pointer to the linked list */
  temp_mnc = &(plmn_id_msg_ie->mnc);
  temp_mnc->n = slelected_plmn_id.num_mnc_digits;
  n_index = 0;
  
  ALLOC_ASN1ARRAY2(&enc_ctxt,temp_mnc,rrc_Digit);

  for(i = 0;i < slelected_plmn_id.num_mnc_digits;i++)
  {
    temp_mnc->elem[n_index] = slelected_plmn_id.mnc[i];
    n_index++;
  }
}

/*===========================================================================

FUNCTION rrcdt_update_scrr_to_handle_dormancy

DESCRIPTION
  This function handles RRC_CLOSE_SESSION_REQ and/or RRC_SIG_CONN_RELEASE_REQ in case 
  RRC_FAST_DORMANCY_REQ is active. 
  It updates the RRC_CLOSE_SESSION_REQ and/or RRC_SIG_CONN_RELEASE_REQ database
  and aborts RRC_FAST_DORMANCY_REQ.

  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Emergency call is initiated
  FALSE: Otherwise

===========================================================================*/
boolean rrcdt_update_scrr_to_handle_dormancy
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
  boolean status = FALSE;
  
  if((RRC_CLOSE_SESSION_REQ == cmd_ptr->cmd_hdr.cmd_id) ||
     (RRC_SIG_CONN_RELEASE_REQ ==  cmd_ptr->cmd_hdr.cmd_id))
  {
    if((TRUE == rrcdormancy_check_dormancy_initiated_scrr_active()) &&
       (RRC_PS_DOMAIN_CN_ID == cmd_ptr->cmd.close_sess_req.cn_domain_id))
    {
      if(!((RRC_SIG_CONN_RELEASE_REQ ==  cmd_ptr->cmd_hdr.cmd_id) &&
          (RRC_PROCEDURE_DORMANCY == cmd_ptr->cmd.rrc_sig_conn_req.rrc_proc))
         )
      {
        /* Update CLOSE_SESSION or SIG_CONN_RELEASE database to act as if SCRI is sent because of them */
        switch(rrcscrr_substate)
        {
          case RRCSCRR_INITIAL:
            /* No need to do anything. Dorrmancy will be aborted and let CLOSE_SESSION
               * or SIG_CONN_REL go on. */
            break;

          case RRCSCRR_WAIT_FOR_CELL_UPDATE_COMP_CNF:
            /* Dormancy initiated SCRI is waiting for CU_COMP_CNF
               * Update as if CLOSE_SESSION or SIG_CONN_REL does that */
            if(RRC_CLOSE_SESSION_REQ == cmd_ptr->cmd_hdr.cmd_id)
            {
              WRRC_MSG0_HIGH("Mapping Dormancy initiated SCRI to CLOSE_SESSION");
              rrcscrr_saved_ps_close_session_req.close_sess_req_status = TRUE;
            }
            
            if(RRC_SIG_CONN_RELEASE_REQ ==  cmd_ptr->cmd_hdr.cmd_id)
            {
              WRRC_MSG0_HIGH("Mapping Dormancy initiated SCRI to SIG_CONN_REL");        
              rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status = TRUE;
            }
            rrcscrr_saved_ps_close_session_req.mui_status = RRCSCRR_MUI_STATUS_FALSE;
            status = TRUE;
            break;
            
          case RRCSCRR_WAIT_FOR_L2ACK:
            if(RRC_CLOSE_SESSION_REQ == cmd_ptr->cmd_hdr.cmd_id)
            {
              WRRC_MSG0_HIGH("Mapping Dormancy initiated SCRI to CLOSE_SESSION");
              rrcscrr_saved_ps_close_session_req.mui_status = RRCSCRR_MUI_CLOSE_SESSION_CNF;
              rrcscrr_saved_ps_close_session_req.close_sess_req_status = FALSE;  
            }
            
            if(RRC_SIG_CONN_RELEASE_REQ ==  cmd_ptr->cmd_hdr.cmd_id)
            {
              WRRC_MSG0_HIGH("Mapping Dormancy initiated SCRI to SIG_CONN_REL");     
              rrcscrr_saved_ps_close_session_req.mui_status = RRCSCRR_MUI_SIG_CON_REL_CNF;
              rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status = FALSE;              
              rrcscrr_saved_ps_close_session_req.rrc_proc = cmd_ptr->cmd.rrc_sig_conn_req.rrc_proc;
            }
            status = TRUE;
            break;
        }

        /* PS domain is anyway getting closed by SCRI */    
        rrcdormancy_check_and_abort_dormancy_req(RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);    
      }
    }
    else
    {
      rrcdormancy_check_and_abort_dormancy_req(RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);    
    }
  }

  return status;
}

#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION rrcdt_check_n_send_sync_ind_for_ps_to_fake_security

DESCRIPTION
  This function sends SYNC_IND for PS domain for integrity
  protection
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Emergency call is initiated
  FALSE: Otherwise

===========================================================================*/
void rrcdt_check_and_send_sync_ind_for_ps_to_fake_security(void)
{
 if(nv_fake_security_enabled)
 {
   /* For PS send the RRC_SYNC_IND for Integrity Protection */
   rrcidt_send_sync_ind(RRC_INTEGRITY_ESTABLISHED, RRC_PS_DOMAIN_CN_ID);
 }
}
#endif

/*===========================================================================

FUNCTION rrcscrr_is_conn_rel_in_progress

DESCRIPTION
  Returns if SCRR for passed domain is in progress
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SCRR in progress else FALSE

===========================================================================*/
boolean rrcscrr_is_conn_rel_in_progress(rrc_cn_domain_identity_e_type cn_domain_id)
{

  switch(cn_domain_id)
  {
    case RRC_CS_DOMAIN_CN_ID:
      if((rrcscrr_saved_cs_close_session_req.close_sess_req_status) ||
         (rrcscrr_saved_cs_close_session_req.rrc_sig_con_rel_status)
        )
      {
        return TRUE;
      }
      break;

    case RRC_PS_DOMAIN_CN_ID:
      if((rrcscrr_saved_ps_close_session_req.close_sess_req_status) ||
         (rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status)
        )
      {
        return TRUE;
      }
      break;

    default:
      WRRC_MSG1_ERROR("This is not expected %d",cn_domain_id);
      return FALSE;
      break;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION rrcscrr_abort_ps_rel_proc

DESCRIPTION
  Initializes SCRR procedure

DEPENDENCIES
  CS domian/CS connection release should not be in progress. 
  Above criteria is met when this is initiated by dormancy procedure

RETURN VALUE
  None

===========================================================================*/
void rrcscrr_abort_ps_rel_proc()
{
  rrcscrr_saved_ps_close_session_req.rrc_sig_con_rel_status = FALSE;
  rrcscrr_saved_ps_close_session_req.mui_status = RRCSCRR_MUI_STATUS_FALSE;
  rrcscrr_substate = RRCSCRR_INITIAL;
  WRRC_MSG0_HIGH("SCRR procedure initialized");
}/*===========================================================================

FUNCTION rrcdt_return_idt_substate

DESCRIPTION
return idt substate

DEPENDENCIES

RETURN VALUE
rrcidt_substate_e_type

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
uint8 rrcdt_return_idt_substate(void)
{
  return rrcidt_substate;
}

/*===========================================================================

FUNCTION rrcdt_return_udt_substate

DESCRIPTION
return udt substate

DEPENDENCIES

RETURN VALUE
rrcudt_substate_e_type

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
uint8 rrcdt_return_udt_substate(void)
{
  return rrcudt_substate;
}

/*===========================================================================

FUNCTION rrcdt_return_scrr_substate

DESCRIPTION
return scrr substate

DEPENDENCIES

RETURN VALUE
rrcscrr_substate_e_type

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
uint8 rrcdt_return_scrr_substate(void)
{
  return rrcscrr_substate;
}


/*===========================================================================

FUNCTION rrcdt_return_cs_domain_open

DESCRIPTION
return true if CS is open

DEPENDENCIES

RETURN VALUE
uint8

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
uint8 rrcdt_return_cs_domain_open()
{
 return ((rrcdt_int_data.cs_domain_open)?1:0);
}