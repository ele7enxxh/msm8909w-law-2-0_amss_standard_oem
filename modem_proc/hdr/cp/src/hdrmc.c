/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 H D R   M A I N   C O N T R O L   T A S K

GENERAL DESCRIPTION

The HDR Main Control task runs the following HDR protocols:

- Air Link Management Protocol (connection layer)
- Initialization State Protocol (connection layer)
- Idle State Protocol (connection layer)
- Connected State Protocol (connection layer)
- Overhead Messages Protocol (connection layer)
- HDR Messaging Protocol (application layer)
- Session Boot Protocol (session layer)
- Session Control Protocol (session layer)
- Part of the Route Update Protocol (connection layer)
- Location Update Protocol

EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdrmc_task()   - The entry point into the HDR task
  hdrmc_task_init() - initializes the queues used by this task
  hdrmc_queue_cmd() - Queues an asynchronous command for one of this
                      task's protocols
  hdrmc_queue_ind() - Queues an asynchronous indication for one of this
                      task's protocols
  hdrmc_queue_msg() - Queues a message for one of this task's protocols


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrmc.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/26/16   vko     Fixed race condition between HDR inactive and 
                           HDRMC_STANDBY_PREF_CHGD_CMD_F
06/30/16   vko     FR 33944: Inform D2L redir success and failure to DS
05/03/16   vko     FR 35747: Graceful handling of UE mode collision and X2L IRAT
5/17/16    vko     Fix Heap memory exhaustion issue when TC_MODE cmd is received,
                   when HDR inactive continuosly
04/05/16   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
12/08/15   rmv     Fixed compiler warnings for unused return values
11/28/15   vko     Do not reset ehrpd credential during mcfg refresh
11/12/15   vko     Fixed KW errors
09/14/15   vko     FR 29020: LTE Power Save Mode w/o coordination with network
08/26/15   wsh     Replaced mcfg_refresh_done with mcfg_refresh_done_w_status
07/24/15   vko     Skip earfcn for D2L reselection, if reselection failed with forbidden TA
                   reason
07/23/15   vko     FR 29379 : Disable tuneaway during Qchat call
05/26/15   vko     DO should wait for D2L Abort response, if deactivate command is received
02/02/15   vko     Add srch window param for D2L CGI
12/29/14   vpa     Fixed compilation error
12/28/14   vko     Debug info for hdr
12/17/14   arm     Removed deprecated TRM APIs in HDR
11/18/14   vko     Do not close ehrpd session immediately during hotswap
11/14/14   vko     Fixed race condition when d2l resel failure happens during d2l abort
10/31/14   vko     Handle HDRMC_STANDBY_PREF_CHGD_REQ in tunnel mode.
11/05/14   vko     Fixed compilation error when LTE is disabled.
11/04/14   vko     Fixed compilation error
10/09/14   vko     If IRAT NV is disabled after REFRESH and DO is in tunnel mode. Deactivate HDR,
                           disable tunnel mode and move to INACTIVE
09/25/14   cnx     Process NV refresh cmd in tunnel INACTIVE state.
09/11/14   cnx     Do not ignore USIM registration callback.
09/05/14   cnx     Always register USIM subscription ready event.
07/18/14   cnx     Remove mcfg_refresh.h from hdrcp_msg.h
07/09/14   cnx     Support NV refresh without reset.
05/21/14   cnx     Format LTE 1x capability req with the band provided by LTE
05/09/14   vko     Support to handle bad NV read gracefully
04/06/14   vko     Send dummy response to LTE RRC for L->D reselection abort
04/18/14   cnx     Handle HDRMC_STANDBY_PREF_CHGD_REQ in tunnel mode NO_SRV state.
04/04/14   vko     Do not process SIM card event recieved on non-primary session.
12/19/13   cnx     Move msgr init before rcinit handshake to avoid race condition.
11/26/13   arm     Handle SSSS->DSDA transition in online mode.
09/13/13   arm     Ignored standby_mode_pref_change in DSDA mode when online.
09/03/13   arm     Ignored standby_mode_pref_change in DSDA mode.
03/10/14   vko     Fixed fragile code in hdrmc_execute_cb
09/30/13   cnx     Deactivate DO until D2L resel abort is acked by LTE.
10/16/13   cnx     Go back to the same tunnel state if L2D IRAT fails.
01/16/14   cnx     Read JCDMA mode from 1x JCDMA mode NV.
10/11/13   vko     Fixed de-allocation of all call-id's due to call orig failure
12/24/13   vko     MMOC enum name change for SIM_BUSY feature
11/21/13   vko     HDR changes for the MMOC interface change, due to SIM_BUSY feature
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
09/19/13   ukl     Reduce Code Duplication HDR-CP
08/08/13   rmv     Featurized tmc APIs with ifndef FEATURE_MODEM_RCINIT
09/03/13   wsh     Replaced FEATURE_DIME/TRITON_BOLT_MODEM with HDR version
08/20/13   cnx     Ignore AdviseUnlock in SVLTE mode.
08/16/13   cnx     Handle L2D abort when AT is not in any L2D handoff.
07/31/13   sat     Mainlining Modem Statistics Feature
06/28/13   cnx     Abort D2L if AdviseUnlock rx'ed during D2L resel.
05/14/13   cnx     Process RASM callback when exit online. 
05/09/13   wsh     Fixed sending multiple UATI, not sending RTC msg in tunnel
05/08/13   vko     Check if app_type is MMGSDI_APP_UISM before setting ehrpd session false
05/06/13   mbs     Fixed compiler errors for BOLT build
05/01/13   cnx     Give HDRIND_RUP_ACT_HO_COMPLETED before clear irat_ho_type flag.
03/31/13   cnx     Support LTE deprioritization.
03/21/13   cnx     Write NV when device is powerdown/reset.
03/15/13   cnx     Fix Triton compiler errors.
03/11/13   dsp     Moved intialization of mc tcb ptr before rcinit handshake.
03/07/13   cnx     Set eHRPD credentials to false when USIM card is removed.
03/01/13   dsp     Moved watchdog timer code to watchdog heart-beat model.  
03/01/13   cnx     Removed hdrscmdb_write_all_config_data when TASK_STOP_SIG received.
02/22/13   vko     Fix the logging of EVENT_T_HDR_LTE_RESEL_STATUS   
02/17/13   arm     Supported NV enable/disable for network dtx attributes. 
02/11/13   ukl     Fix to handle a reselection triggered during Active HO.
02/04/13   wsh     Disabled MultiATPage by default
02/04/13   cnx     Replace PriorSession with UpdateUATI in some network.
01/30/13   vko     Optimize write to EFS
01/28/13   arm     Moved intialization of mc tcb ptr before rcinit handshake.
01/24/13   vko     Check if app_type is MMGSDI_APP_UISM, before setting ehrpd session true
01/13/13   ukl     Added NV control for 1x TuneAway during session negotiation.
01/13/13   rkc     Changed all MED F3's to HIGH.
12/14/12   cnx     Replace FEATURE_DUAL_SIM with FEATURE_HDR_DUAL_SIM.
12/13/12   wsh     Report orig failue to CM when doing D2L
11/06/12   cnx     Delay processing tunnel activation if idle handoff in progress
11/04/12   cnx     Go back to tunnel mode after reselection/redirection 
                   abort/failure if tunnel mode is enabled.
10/29/12   mbs     Fixed compiler errors for first Triton build
10/07/12   cnx     Clean up RASM before idle handoff to native HDR network.
10/02/12   cnx     Added return value for hdrmc_process_lte_to_hdr_ho_cnf_cmd().
09/20/12   cnx     Save session to EFS when powerdown in tunnel.
08/28/12   cnx     Ignore Handoff Conf if there is no IRAT handoff in progress.
08/11/12   wsh     Defined QPCH feature, but disable by NV
08/07/12   cnx     Abort Active Handoff if redirection is received.
07/27/12   wsh     Enable MultiATPage by default   
07/24/12   cnx     Handle D2L in non-hybrid mode.
07/18/12   cnx     Fixed compiler errors when Qchat feature is turned on.
06/29/12   dsp     New offline notify processing is added.
06/28/12   cnx     Reset default subtypes if AT exits tunnel without session
06/25/12   arm     MM_GSDI: READY clean up for RCINIT.
06/19/12   cnx     AT moves back to tunnel after ReverseTimeTransfer if 
                   tunnel mode is enabled.
06/19/12   cnx     AT moves back to tunnel after getting CGI if tunnel mode 
                   is enabled.
04/27/12   cnx     Avoid Reselection from eHRPD to LTE forbidden PLMNs.
04/24/12   cnx     Fixed compiler warnings.
04/20/12   cnx     Unsubcribe LTE_RRC_SERVICE_IND.
03/23/12   cnx     Support getting Pilot strength for CGI response.
03/23/12   arm     Fixed featurization bug for RCINIT. 
03/23/12   smd     Removed use of obsolete AEEstd.h.   
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
03/20/12   cnx     Fixed compiler errors.
03/19/12   cnx     Fixed compiler errors.
03/08/12   wsh     Merged NLB support
03/05/12   arm     Fixed RCINIT bugs found during integration.
03/01/12   cnx     Subscribe ATTACH COMPLETE; 
                   Don't enter tunnel mode if IRAT subtypes not supported.
02/29/12   cnx     Fixed typo when copying PN in CGI response.
02/28/12   cnx     Replaced PROT_GEN_CMD_IRAT_TO_GWL with PROT_DEACT_GWL_REDIR.
02/28/12   cnx     Deactivate HDRDEC if UE goes back to tunnel mode.
02/27/12   smd     Added updates of FTM msgr interface changes. 
02/23/12   wsh     UE not proposing eHRPD during session negotiation   
01/18/12   wsh     Added support for interband HO threshold   
10/10/11   arm     Added RCINIT support.
01/18/12   cnx     Merged changes to fix compile errors.
01/18/12   cnx     Update UATI if active handoff fails and UE goes back to LTE.
01/03/12   sju     Added support to enable verbose debug f3 at runtime. 
12/21/11   cnx     Fixed compiler error.   
12/05/11   cnx     Support hdrcp_config_info.conf.
12/02/11   cnx     Save HDR session when receiving PhoneStatusChange in offline.
10/28/11   pba     Optimized handoff phase 2 support
10/27/11   vlc     Fixed compiler errors.   
10/20/11   wsh     Changed PCP init code path to avoid double init
10/11/11   arm     SU API clean up changes for MMODE.
10/10/11   cnx     Support HDRMC_LTE_RRC_SERVICE_IND.
10/03/11   cnx     Added HDRSAP_TUNNEL_MODE_NO_SRV.
09/30/11   cnx     Changed %P in F3 to 0x%x.
09/26/11   cnx     Removed hdrmc_get_buf_else_err_fatal().
09/24/11   cnx     Support SAP message interface;
                   Activate/Deactivate decoder when HDR moves between 
                   tunnel mode and native HDR.
08/31/11   cnx     Activated HDR decoder when HDR moves from tunnel mode to online.
08/19/11   wsh     Replaced depreciated MMGSDI API calls
08/25/11   cnx     Fixed compiler errors.   
08/04/11   cnx     Support pre-registeration information.
08/01/11   cnx     Merged Optimized Handoff changes.
07/27/11   cnx     Dropped HDR_WRITE_DATABASE_TO_EFS cmd if HDR is offline.
05/18/11   wsh     Added network load balancing support
05/11/11   cnx     Moved saving hdrscmdb to 'case HDRMC_PROT_PH_STAT_CHGD_CMD_F'
                   When HDR is offline.
05/02/11   cnx     Saved hdrscmdb when deactivate cmd is received if eMMC is used.
04/27/11   cnx     Supported callback registration.
02/14/10   wsh     Fixed reporting old callid after ConnectionClose
11/29/10   cnx     Fixed compiler warnings.   
11/09/10   pba     Fixed issue of DO CP not reporting call failure to CM during
                   BSR attempt.
10/27/10   wsh     Added dual standby mode support
10/14/10   pxu     Added missing feature checks in hdrmc_feature_is_enabled
10/12/10   pxu     Added JCDMA related features to hdrmc dynamic feature   
                   checking functions.
10/11/10   pba     Clear the context in MMOC when LTE reselection is failed.
09/17/10   pxu     Memory heap changes.
09/16/10   cnx     Fixed compiler warnings.    
09/15/10   ljl     Supported dynamic wdog.
06/28/10   wsh     Changed powerup sequence to avoid crash
06/22/10   wsh     Fixed race condition when reg/unreg PCP flows
06/21/10   cnx     Fixed compiler warnings.
06/10/10   pba     IRAT Measurement feature.
06/08/10   pba     Fixed issue in extracting redirected channel information
06/03/10   cnx     Fixed Klocwork errors.
06/01/10   pba     Added support for FTD stats.
05/19/10   pba     Added support for Reverse Time Transfer.
03/25/10   pba     Populate the 1X-HRPD capabilities container with CDMA band 
                   information.
03/24/10   pba     Reg and de-reg for reselection messages
03/22/10   pba     Added support for eHRPD session negotiation based on 
                   confirming availability of USIM App
03/02/10   pba     Added support for LTE to HDR reselection
02/09/10   pba     Process capabilities request in inactive state also.
02/09/10   pba     Updated comments.
02/09/10   pba     Register and de-register capabilities message ID.
02/09/10   pba     Included AEEstd.h to resolve compilation issue.
01/25/10   wsh     Reverted FEATURE_EHRPD check
01/15/09   wsh     KA not send out after KA time expires
12/01/09   suren   Added QChat optimization changes.
12/06/09   wsh     Fixed dog timeout after TASK_STOP_SIG
11/02/09   wsh     Workaround for HDRMC queue full
10/30/09   pba     Added ehrpd support.
10/20/09   wsh     Report PER info CM
09/30/09   wsh     Fixed KW warnings
09/18/09   wsh     CMI: support changes from modem DATA
09/09/09   wsh     CMI: to support changes in clk and ts APIs
07/21/09   wsh     Prepare for CMI phase 2/3 integration
04/08/09   wsh     Fixed not aborting connection when rcvd HDRMC_END_F.
04/01/09   grl     Added temporary JCDMA featurization.
03/25/09   wsh     CMI: Updated featurization for CMI
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
03/06/09   pmk     Clean up the resources before sending task_stop
02/27/09   wsh     Added run-time check for KDDI feature flags
02/24/09   wsh     CMI: updated included headers due to header split
02/18/09   wsh     Updated file to include private header hdrmc_v.h
01/27/09   wsh     Merged BCMCS 2.0 on to mainbranch
12/10/08   smd     Replaced assert.h with amssassert.h
09/16/08   wsh     Reverted change in handling END_F, DS will provide fix
08/21/08   wsh     For JCDMA builds, SessClose should come before ConClose
04/17/08   wsh     DCM optimization: not closing session unnecessarily when
                   moving across subnet
04/02/07   wsh     DCM feature changes
03/06/08   wsh     Reverted HDRMC queue size back to 30
02/27/08   wsh     Increased HDRMC queue size to 35 to prevent a valid sceanrio
                   where 30 events are queued all at once
11/20/07   wsh     KDDI passport mode phase 1
09/18/07   wsh     Report correct DiversityComb and SimultaneousCommonChannelReceive
                   value in Multi-Mode Discovery protocol
09/11/07   wsh     Removed lint high warnings
09/10/07   wsh     Fixed writing to NV before hdrutil is initialized
08/21/07   wsh     Added dynamic checking if HDR session is tied to PPP
05/29/07   mpa     Update the session NAI when the phone status changes 
04/03/07   wsh     Fixed error where dsm_item is not freed
03/27/07   wsh     Added support for new prior session handling
02/12/07   mpa     Fixed RVCT compiler warnings.
02/09/07   yll     Process HDRMC_CSNA_MSG_CONFIG_F command in Inactive state
01/17/07   yll     Changes for HOMER interface.
01/08/07   yll     Added support for CSNA.
12/19/06   pba     Reverted DOS link_flow_number to rlp_flow.
11/20/06   hal     Updated feature for SupportedCDMAChannels to silently close
                   session if band preferences are changed
11/17/06   pba     Added support for DOS over EMPA.
11/06/06   wshe    Clean up the way timers are handled in ovhd protocol
11/06/06   wsh     Will no longer close a session when data call drops during
                   session negotiation.
10/24/06   mpa     SD no longer needs to be notified when going offline.
10/23/06   mpa     Reply to an mmoc generic cmd with HDR offline if AT the
                   HDRMC_EXIT_SIG is already set (srcher already inactive)
10/23/06   sju     Added support to send Xon/XoffRequest by using RASM.
10/05/06   wsh     Added check to drop config msgs unless session is open
                   in hdrmc_protocol_msg().
10/04/06   pba     Write database to EFS on graceful shutdown.
10/04/06   yll     Check if SMP state is inactive and MUM is enabled before
                   opening new session.
09/08/06   spn     Added code to route the commands and indications to 
                   reservation module. 
07/21/06   etv     Moved hdrcmac_powerup_init from HDRRX to HDRMC.
06/20/06   mpa     Added hdrpac_mdm_powerup_init().
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
06/05/06   pba     Initialize DOS protocol only once during powerup instead of
                   everytime while entering online state.
05/10/06   yll/ksu EMPA: octet stream, octet sequencing
05/10/06   yll/dna Reliable Access Signaling Messages (RASM)
05/09/06   yll     Added support for Minimal UATI Mode.
05/05/06   pba     Added timer handling for SCMDB.
02/21/06   mpa     Restored saving ka timer on powerdown.
01/13/06   kss     Trigger ACMAC persist. index update on phone status change.
12/05/05   mpa     Add back missing break in hdrmc_inactive_process_cmd()
12/01/05   dna     Allow SD to return ACQ_GW or any other new reasons
11/18/05   hal     Uses CM API instead of directly accessing cm_mc_rpt_free_q
09/07/05   etv     Added SCM's process_timer routine to hdrmc_protocol_timer.
08/28/05   mpa     Added handling of Broadcast Commands
08/22/05   etv     Added Message processing and powerup init calls for SCM.
08/07/05   mpa     Added timer handling for MRLP.
07/04/05   dna     Protect hdrhmp.msgs_on_event_q for multi-threaded access.
06/01/05   grl     Removed "sam.h" include.
05/09/05   ar/pba  Integrated Data Over Signaling protocol code
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
03/07/05   pba     Modified RUP timers to use callback functions instead of 
                   signals.
03/07/05   pba     Separated handling of In-Use and In-Config messages for PAC.
02/28/05   ka      Adding prot_subsc_chg param to hdrmc_ph_status_chgd_cmd
01/26/05   hal     Converted all HDROVHD timers to use callbacks.
12/06/04   dna     Mainlined FEATURE_CM_END_CNF.
11/29/04   dna     Send END_CNF to CM with call_id of user ended call.
09/09/04   mpa     Added PAC MC command processing.  QOS related changes.
08/26/04   mpa     Update the ESN on a phone status change command.
06/30/04   dna     Delayed giving mmoc_rpt_prot_auto_deactd_ind() until           
                   HDRIND_LMAC_DEACTIVATE_DONE is received.  This changed     
                   hdralmp_deactivate(), hdralmp_cmd_params_union_type, and 
                   delayed when and where we give HDRIND_ALMP_EXIT_HDR_MODE.  
07/02/04   mpa     Support for new HDR Broadcast Protocol.
07/01/04   mpa     Close HDR Session on rcving MC END cmd w/ SRV_INACT reason
06/16/04   ss      (mpa for) Added HDRMC_BCAST_REGISTRATION_TIMER_SIG
06/02/04   kss     Initial support for HDR BCMCS (HDR Gold Multicast).
01/13/04   mpa     (for sj)  Added hdrmc_hdr_unlock_rf_cmd().
11/11/03   mpa     Added support for HDRMC_IDLE_PGSLOT_CHANGED_F
11/03/03    ic     In hdrmc_send_release() set rel_code for all call ids.
10/23/03    km     Send response to CM only if we have valid Call Id
                   Deallocate Callids when END is received from CM.
10/22/03    km     Added detail to F3 messaging and fixed callid dealloc.
                   Deleted function hdrmc_send_deact_ack
10/15/03    km     Removed support for MC_PPP_CLOSED. Renamed MC_UNLOCK_HDR_RF_F
                   to HDRMC_UNLOCK_HDR_RF_F
10/14/03    km     Added support for MC_UNLOCK_HDR_RF_F
10/09/03    km     Fixed callid deallocation and added to F3 msgs
10/08/03   mpa     Only allow system reselection when not connected
                   (ported from 5500)
10/06/03    km     Replaced cm_get_cur_hybr_status() with sd_misc_is_hybr_opr()
09/24/03   mpa     Temporarily moved the powersave algorithm to exit_online
09/18/03   mpa     Mainlined Exit hybrid powersave algorithm in enter_inactive
09/12/03   mpa     Converted F3 messages to use new MSG2.0 HDR PROT SSID
09/11/03    km     Added support for new CM interface for array of callids
08/10/03   mpa     Merged in 6500 baseline
08/08/03   vas     Initialize hdr call id module
07/31/03   km(sj)  Added MMOC hybrid support.
                   Renamed a few functions to begin with hdrmc_ instead of hdr_.
06/18/03   vas     Handle Call End with reason=abort. Don't init PAC.
03/12/03   mpa     Updated ind data dump info for ind left on event queue
03/05/03   mpa     Removed HDRMC_INIT_SYNC_MSG_TIMER_SIG
                   Start hdrinit pilot acquisition hold period on unlock rf cmd
03/05/03   mpa     Removed UATI_ASSIGN_TIMER_SIG. Added hdramp_process_timer()
03/05/03   dna     Remove special handling for RUDE_CLOSE
03/04/03   dna     Add call to hdrcon_powerup_init() that was just missing.
02/13/03   mpa     Restored save ka timer on powerdown.
02/04/03   mpa     Removed sd_action ASSERTs.
01/16/03   mpa     Added MC_PPP_CLOSED command
01/15/03   mpa     Added hdrsrch.h include
07/18/02   mpa     Added new events for reselection, deep sleep and power save
07/17/02   dna     Queue timer expiration events on hdrmc.event_q because timer
                   callback runs as a separate thread and causes problems.
07/11/01   dna     Move TASK_STOP and EXIT_HDR signal handling to be lower
                   priority than the EVENT_Q_SIG.
07/10/01   dna     Some initialization that should only be done at powerup
                   was happening when we re-enter HDR after power save.
06/19/02   sh      Added processing for unlock HDR RF cmd and disable
                   reselection when HDR is deactivated
06/07/02   dna     Handle NV Write signal as lowest priority
06/05/02   dna     Queue NV writes and write the items one at a time.
05/30/02   dna     Ensure msgs_on_event_q does not get out of sync.
05/16/02   dna/vas Added ERR debug msgs to determine what events are left
                   unprocessed on hdrmc event queue when exiting HDRMC online
                   mode.
05/03/02   dna     If items are left on hdrmc_event_q when we exit HDR mode,
                   for now at least make sure we call message callbacks and
                   free the DSM item, but in the future we should handle all
                   the events, especially session and stream.
05/02/02   dna     Fix for "HDR stuck in power save"
04/17/02   mpa     Save ka timers upon powerdown (in hdrmc_enter_inactive only
                   for now)
04/03/02   mpa     Removed HDRMC_AMP_DUAL_ADDR_TIMER_SIG signal
03/29/02   dna     Changed timers to use REX callback instead of indications
03/15/02   dna     Removed INIT acquisition timer, so the signal is now unused.
03/13/02   sh      Added processing for SD_ACT_AMPS and release call if
                   orig with DRS=0
03/12/02   sq      Added support for FTM.
03/10/02   sh      Use CM_HDR prefix for CM reports from HDR
03/03/02   sh      Removed MC_DEACT_PREF_SYS_CHANGED
02/20/02   sh      Fixed the powersave problem when in the inactive state
02/08/02   mpa     Added hdrlog_powerup_init() function call.
01/21/02   sh      Added routine to process user command and support
                   for SD reselection timers
01/04/02   sh      Added hybrid operation support
11/30/01   om      Added security layer support
09/24/01   sh      Added HDRMC_EXIT_RESET
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/27/01   vas     Removed HDRDS & PAC powerup initialization. They have to
                   be initialized from the DS task.
08/27/01   kss     Added MAC powerup_init() function calls.
08/18/01   sh      Added porotocol logging powerup initialization
08/17/01   vas     Added initializations for hdrds & pac modules.
07/10/01   dna     Code cleanup changes
06/21/01   vas     Removed processing of hmp delay timer signal
06/20/01   vas     Added support for TASK_STOP_SIG & TASK_OFFLINE_SIG
06/16/01   vas     Added function hdrmc_get_exit_reason. Ack the task stop &
                   offline singals.
06/15/01   kss     Added HDR enter/exit signals for HDR Rx and Tx tasks;
                   added dog support for hdrmc task.
06/06/01   dna     Do powerup init after getting task start sig.
05/01/01   dna     Made HDRMC its own task instead of a subtask.
03/29/01   dna     Added location update protocol
03/19/01   dna     Added support for IS-856 compliant session layer
03/10/01   sh      Added HDRMC_IDLE_CONFIG_RES_TIMER_SIG
02/07/01   dna     Renamed HDRMC_TEST_1_SIG to
                   HDRMC_HMP_DELAY_AFTER_ACQ_TIMER_SIG because it looks like
                   its use may be permanent.
01/30/01   dna     Moved powerup timed wait into mc.c so going offline works.
01/26/01   dna     Added call to hdrrup_powerup_init()
09/19/00   sh      added Idle State related signals in hdrmc_subtask
09/07/00   dna     Merged integration changes.
02/02/00   dna     Created module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "customer.h"
#include "hdrmci.h"
#include "dsm.h"
#include "queue.h"
#include "hdrhai.h"
#include "hdrbuf.h"
#include "task.h"
#ifndef FEATURE_MODEM_RCINIT
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT */
#include "hdrdebug.h"
#include "err.h"
#include "mc.h"
#include "modem_mem.h"
#include "fs_errno.h"
#include "fs_public.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#ifdef FEATURE_CMI
#include "mc_v.h"
#endif

#include "mci.h"
#include "cmd.h"
#include "dog.h"
#include "dog_hb_rex.h"
#include "memory.h"
#include "amssassert.h"
#include "secerrno.h"
#include "sys.h"
#ifdef FEATURE_CMI
#include "sys_v.h"
#endif
#include "timer.h"

#include "hdrrx.h"
#include "hdrtx.h"
#include "hdralmp.h"
#include "hdrhmp.h"
#include "hdrhmpi.h"
#include "hdridle.h"
#include "hdridlei.h"
#include "hdrovhd.h"
#include "hdrovhdi.h"
#include "hdrrup.h"
#include "hdrkep.h"
#include "hdrkepi.h"
#include "hdrauth.h"
#include "hdrauthi.h"
#include "hdrutil.h"

#include "hdrsmp.h"
#include "hdrsmpi.h"
#include "hdrsmpka.h"
#include "hdramp.h"
#include "hdrampi.h"
#include "hdrscp.h"
#include "hdrscpi.h"

#ifdef FEATURE_MEASURE_UPTIME_IN_NV
#include "hdrutil.h"
#endif /* FEATURE_MEASURE_UPTIME_IN_NV */

#include "hdrstream.h"
#include "hdrstreami.h"
#include "hdrtrace.h"
#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif

#include "hdrcallid.h"
#include "hdrinit.h"
#include "hdrind.h"
#include "hdrrupi.h"
#include "hdrlmac.h"
#include "hdramac.h"
#include "hdrfmac.h"
#include "hdrrmac.h"
#include "hdrlup.h"
#include "hdrlupi.h"
#include "hdrds_api.h"
#include "hdrpacc.h"
#include "hdrpac_api.h"
#include "hdrplog.h"
#include "hdrlog.h"
#include "hdrsrch.h"
#include "cm.h"

#ifdef FEATURE_CMI
#include "cm_v.h"
#endif

#include "cmxll.h"
#include "mmoc.h"

#ifdef FEATURE_CMI
#include "mmoc_v.h"
#endif

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm.h"
#endif

#ifdef FEATURE_HDR_QOS
#include "hdrmrlpc.h"
#include "hdrmrlp_api.h"
#endif

#ifdef FEATURE_HDR_BCMCS
#include "hdrbccpi.h"
#include "hdrbccp.h"
#include "hdrbctask.h"
#endif /* FEATURE_HDR_BCMCS */

#include "hdrpac_api.h"

#ifdef FEATURE_HDR_DOS
#include "hdrdos.h"
#endif /* FEATURE_HDR_DOS */

#include "hdrscm.h"
#include "hdrscmdb.h"
#include "hdrfcp.h"
#include "hdrcsna.h"
#include "hdrscmgmmcdp.h"

#include "hdrts.h"
#include "hdrftap.h"

#ifdef FEATURE_HDR_QCHAT
#include "hdrqchatupk_opts.h"
#include "hdrqchatupk.h"
#endif /*FEATURE_HDR_QCHAT*/

#ifdef FEATURE_HDR_MC_MSGR
#include "msgr.h"
#include "msgr_rex.h"
#endif  /* FEATURE_HDR_MC_MSGR */

#ifdef FEATURE_LTE_TO_HDR
#include "hdrcp_msg.h"
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_HDR_TO_LTE
#include "trm.h"
#include "hdralmps.h"
#endif /* FEATURE_HDR_TO_LTE */

#include "ds707_extif.h"
#include "hdrrupc.h"
#include "hdrpcp.h"

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI */
#ifdef FEATURE_EHRPD
#include "mmgsdisessionlib.h"
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_HDR_REVC
#include "hdrscmfmac.h"
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#define HDRMC_RCEVT_PREFIX "HDRMC:"
#define HDRMC_RCEVT_READY HDRMC_RCEVT_PREFIX "ready"
#endif

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsaptx.h"
#include "hdrsap.h"
#include "hdrsaps.h"
#include "hdrmc_v.h"
#include "emm_ext_msg.h"
#include "emm_irat_if_msg.h"
#include "lte_rrc_ext_msg.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

#include "fs_public.h"
#include "fs_errno.h"
#include "time_svc.h"
#include "hdralmps.h"
#include "hdrmultirat.h"
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#include "hdrhitcmd.h"
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
rex_tcb_type *hdrmc_tcb_ptr;

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#define HDRMC_IS_HYBRID_ENABLED(mode_pref, hybr_pref)\
      ( ((hybr_pref == SD_SS_HYBR_PREF_CDMA_HDR) &&\
         ((mode_pref == SD_SS_MODE_PREF_DIGITAL) ||\
          (mode_pref == SD_SS_MODE_PREF_ANY))) ? TRUE :FALSE)

#define HDRMC_WRITE_EFS_NV_CONF_FILE( config_fd, file_path ) \
    { \
      result = efs_write( (config_fd), (file_path), strlen((file_path)) ); \
      result += efs_write( (config_fd), new_line, 1 ); \
      if ( result != ( strlen((file_path)) + 1 ) ) \
      { \
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR, \
                        "Error writing to config file errno %d", efs_errno ); \
        result = efs_close( (config_fd) ); \
        return; \
      } \
    }

typedef struct
{
   boolean          is_cnf_pending;
   prot_trans_type  trans_id;

} hdrmc_deact_cnf_type;

typedef struct
{
  boolean jcdma;
  boolean ka_req_disabled;
  boolean session_tied_to_ppp;
  boolean disable_susp_timer;
  boolean allow_spibi_handoffs;
  boolean no_active_handdown;
  boolean remove_off_band_neighbors;
  boolean rl_rate_intertia;
  boolean kddi_restricted_session_call_flow;
  boolean accolc_alternate_mapping;
  boolean kddi_ho_threshold;
  boolean network_load_balancing;
  boolean multi_at_page;
  boolean quick_idle;
  boolean network_dtx;
  /* JCDMA NV is not a separate feature. It reads NV65810 and tells if 
   * JCDMA feature is enabled. It is used by IRATMan to tell if it is in 
   * JCDMA mode.
   * We don't reuse jcdma field simply because other than Force Restricted CF
   * and KDDI handoff threshold feature, all other HDR JCDMA features have 
   * been deprecated. 
   */
  boolean jcdma_nv;
} hdrmc_features_state_type;

/* Constant describing the number of buffers on event queue */
#define HDRMC_EVENT_BUFS_NUM 30

/* Threshold above which event queue is consider near full */
#define HDRMC_EVENT_Q_THRESHOLD 23

/* Constant describing the number of buffers on HDRMC cmd queue */
#define HDRMC_CMD_BUFS_NUM   6

/* HDRCP EFS NV conf file path */
#define HDRCP_EFS_NV_CONF_FILE_PATH  "/nv/item_files/conf/hdrcp_config_info.conf"

#define HDRCP_JCDMA_MODE "/nv/item_files/jcdma/jcdma_mode"

#define HDRMC_REVC_FEATURES          "/nv/item_files/modem/hdr/cp/mc/revcconfig"

#define HDRMC_FEATURES_NETWORK_LOAD_BALANCING_MASK       0x1
#define HDRMC_FEATURES_MULTI_AT_PAGE_MASK                0x2
#define HDRMC_FEATURES_QUICK_IDLE_MASK                   0x4
#define HDRMC_FEATURES_NETWORK_BASED_DTX                 0x8

#define HDRMC_REVC_FEATURES_DEFAULT                      0x0

#ifdef FEATURE_LTE_TO_HDR
typedef enum
{
  HDRMC_IRAT_NONE = 0,
  HDRMC_IRAT_LTE_TO_HDR_REDIRECT = 1,
  HDRMC_IRAT_LTE_TO_HDR_RESELECT = 2,
  HDRMC_IRAT_HDR_TO_LTE_TIME_TRANSFER = 3,
  HDRMC_IRAT_LTE_TO_HDR_ACTIVE_HO = 4,
  HDRMC_IRAT_LTE_TO_HDR_GET_CGI = 5

} hdrmc_irat_ho_type;

typedef struct
{
  /* IRAT HO type */
  hdrmc_irat_ho_type irat_ho_type;

  /*!< Num neighboring bands*/
  uint8 band_count; 

  /*!< Channel info */
  hdrcp_redir_freq_per_band_list 
        freq_per_band_list[HDR_CP_MAX_CDMA2000_BANDS];

  /*!< Band and Frequency  */
  sys_channel_type               sys_info;

  /*!< Number of pn's in list */
  uint8                          pn_list_size;      

  /*!< Maximum time allowed by LTE for DO to do brute-force acq and sync.*/
  uint16                         time_allowed_ms;

  /*!< Cell Id - 0..maxPNOffset(511) */
  sys_pilot_pn_type              pilot_pn[HDR_CP_MAX_PN_LIST_SIZE];

  /*!< LTE RTC counter */
  uint32                         lte_ostmr;

  /*!< CDMA System time */
  uint64                         cdma_sys_time;

  /*!< CDMA search_window_size */
  uint8 search_window_size;
  
#ifdef FEATURE_LTE_TO_HDR_OH
  /* Active handover Message Container */
  hdrmc_lte_to_hdr_active_ho_info_type active_ho_info; 
#endif /* FEATURE_LTE_TO_HDR_OH */

} hdrmc_lte_to_hdr_ho_info_type;
#endif /* FEATURE_LTE_TO_HDR */

#if defined(FEATURE_HDR_TO_LTE) || defined(FEATURE_HDR_TO_LTE_REDIRECTION)
  /* If HDR is deactivated during D2L reselection, HDR aborts D2L first.
   * After receiving the abort confirmation back from LTE, HDR continues
   * to deactivate itself. The structure is used to save the deactivation 
   * info when abort is in progress. */ 
typedef struct
{
  boolean                        deactivation_is_pending;
  hdrhai_abort_reason_enum_type  deactivation_reason;
  boolean                        explicit_mmoc_deactivate;
  prot_trans_type                deactivation_trans_id;
} hdrmc_pending_deact_info_type;
#endif /* defined(FEATURE_HDR_TO_LTE) || defined(FEATURE_HDR_TO_LTE_REDIRECTION) */


#ifdef FEATURE_MMGSDI
#ifdef FEATURE_HDR_NEW_GSDI_API
#define HDRMC_1X_PROV_PRI_SESSION  0
#endif /* FEATURE_HDR_NEW_GSDI_API */
#ifdef FEATURE_EHRPD
#define HDRMC_GW_PROV_PRI_SESSION  1
#define HDRMC_GW_PROV_SEC_SESSION  2
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_HDR_DEBUG_INFO

#define DEBUG_BUF_SIZE 40 /* size of circular buffer */

typedef struct
{
  prot_trans_type transaction_id;
  hdrmc_sent_cmd_type cmd;
  hdrmc_state_type state;
  hdralmps_state_enum_type almp_state;
  time_julian_type sent_time_jul;
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrhai_tunnel_mode_type tunnel_mode;
#endif /* FEATURE_LTE_TO_HDR */
}hdr_sent_rpt_t;

typedef struct
{
  prot_trans_type transaction_id;
  hdrmc_name_type cmd;
  hdrmc_state_type state;
  hdralmps_state_enum_type almp_state;
  time_julian_type rcvd_time_jul;
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrhai_tunnel_mode_type tunnel_mode;
#endif /* FEATURE_LTE_TO_HDR */
}hdr_rcvd_rpt_t;

typedef struct
{
  hdr_sent_rpt_t sent_rpt[DEBUG_BUF_SIZE];
  uint8 sent_rpt_index; /* running index */
  hdr_rcvd_rpt_t rcvd_rpt[DEBUG_BUF_SIZE];
  uint8 rcvd_rpt_index; /* running index */
}hdr_dbg_info_t;

#endif /* FEATURE_HDR_DEBUG_INFO */

typedef struct
{
   boolean pending;
   prot_trans_type trans_id;
}hdrmc_ue_mode_switch_cnf_info_type;

typedef struct
{
  boolean pending; /* pending tc mode updation */
  hdrsrch_tc_mode_enum_type mode; /* tc mode */
}hdrmc_tc_mode_info_type;

LOCAL struct
{
  hdrmc_state_type state;  /* Online or Inactive */
  rex_timer_type rpt_timer;  /* Timer for reporting to watchdog */
  q_type event_q; /* Queue for events to hdrmc task */

  q_type cmd_q;

  timer_type           ss_timer_cb; /* System selection timer */

  hdrmc_exit_enum_type type_of_exit; /* reason to exit HDR mode */
  boolean              powerup_inactive;

#ifdef FEATURE_MEASURE_UPTIME_IN_NV
  int                    uptime_counter_min;
    /* counter to count uptime every 5 min */
  rex_timer_type   uptime_timer;
#endif /* FEATURE_MEASURE_UPTIME_IN_NV */

  hdrmc_features_state_type features;

#ifdef FEATURE_HDR_MC_MSGR
  msgr_client_t msgr_client_id; 
     /* Messenger client ID */
#endif /* FEATURE_HDR_MC_MSGR */

#ifdef FEATURE_LTE_TO_HDR
  boolean lte_to_hdr_ho_aborted; 
    /* Indicates whether handoff is aborted by LTE */

  hdrmc_lte_to_hdr_ho_info_type lte_to_hdr_ho_info;
    /* Redirect or reselect channel info from LTE */

#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_MMGSDI
  mmgsdi_client_id_type mmgsdi_cid;
#ifdef FEATURE_HDR_NEW_GSDI_API
  /* needed to read ICC ID */
  mmgsdi_session_id_type mmgsdi_session_id;
  boolean mmgsdi_session_ready;
#endif /* FEATURE_HDR_NEW_GSDI_API */
#ifdef FEATURE_EHRPD
  mmgsdi_session_id_type pri_session_id;
  mmgsdi_session_id_type sec_session_id;
    /* slot id for which hdr pri session is registed */
  mmgsdi_slot_id_enum_type pri_slot_id; 
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_HDR_REVC
  boolean qos_flow_active;
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_LTE_TO_HDR_OH
  boolean tunnel_activation_is_pending;
#endif /* FEATURE_LTE_TO_HDR_OH */

#if defined(FEATURE_HDR_TO_LTE) || defined(FEATURE_HDR_TO_LTE_REDIRECTION)
  hdrmc_pending_deact_info_type pending_deact_info;
#endif /* defined(FEATURE_HDR_TO_LTE) || defined(FEATURE_HDR_TO_LTE_REDIRECTION) */

  /* Whether it is the first initialization */
  boolean is_initialized;
  boolean hdr_exiting;
#ifdef FEATURE_HDR_DEBUG_INFO
  hdr_dbg_info_t dbg_info; /* debug info */
#endif /* FEATURE_HDR_DEBUG_INFO */
  boolean mcfg_refresh_is_in_progress;
  hdrmc_ue_mode_switch_cnf_info_type ue_mode_switch_cnf;	
  hdrmc_tc_mode_info_type tc_mode_info; /* tc mode info */
} hdrmc;

#ifdef FEATURE_MEASURE_UPTIME_IN_NV

#define HDRMC_UPTIME_TIMER_VAL_MINS  5
  /* Write the uptime every 5 mins (in msec) */
#define HDRMC_UPTIME_TIMER_VAL_MS    HDRMC_UPTIME_TIMER_VAL_MINS * 60 * 1000

#endif /* FEATURE_MEASURE_UPTIME_IN_NV */

#if defined( DOG_DYNAMIC_API ) && defined( FEATURE_HDR_WDOG_DYNAMIC )
dog_report_type   hdrmc_dog_rpt_var = 0;
  /* Dog rpt */
#endif /* DOG_DYNAMIC_API && FEATURE_HDR_WDOG_DYNAMIC */

typedef struct{
  hdrmc_cb_f_type cb_func_ptr;        /* Callback function pointer */
  void            *client_data_ptr;   /* Client data pointer */
} hdrmc_cb_entry_type;

/* Array recording the callback function for each registered events. */
static hdrmc_cb_entry_type hdrmc_cb_entry_map[HDRMC_CB_CLIENT_MAX][HDRMC_CB_EVENT_ENUM_MAX];
void hdrmc_init_features( void );
#ifdef FEATURE_HDR_DEBUG_INFO
boolean hdrmc_get_transaction_id(prot_trans_type *transaction_id, hdrmc_msg_type *cmd_ptr);
#ifdef FEATURE_LTE_TO_HDR
hdrmc_sent_cmd_type hdrmc_get_irat_cmdtype(hdrmc_irat_ho_type irat_ho_type);
#endif /* FEATURE_LTE_TO_HDR */
#endif /* FEATURE_HDR_DEBUG_INFO */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRMC_GET_CMD_BUF

DESCRIPTION
  This function allocates a HDRMC task command buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to allocated CM task command buffer if allocation succeed.
  NULL if allocation failed.

SIDE EFFECTS
  None.
===========================================================================*/
HDRMC_CMD_TYPE *hdrmc_get_cmd_buf(void)
{
  HDRMC_CMD_TYPE *cmd_ptr; /* Pointer to CM command buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( cmd_ptr = ( HDRMC_CMD_TYPE * ) modem_mem_alloc(
       sizeof( HDRMC_CMD_TYPE ), MODEM_MEM_CLIENT_HDR_CRIT ) ) ==
       NULL )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Not enough memory to allocate new cmd buffer");
  }
  else
  {
    HDR_MSG_PROT_2( MSG_LEGACY_LOW, "Cmd ptr alloc = 0x%x , size = %d bytes.",
                    cmd_ptr, sizeof( HDRMC_CMD_TYPE ));
  }

  return cmd_ptr;
} /* hdrmc_get_cmd_buf */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMC_CMD

DESCRIPTION
  This is the outside world's interface to the HDRMC task and subtasks.
  this function takes an already filled out mc_msg_type and places it
  on the hdrmc command queue.  The function returns to the caller after
  setting the status to busy indicaton.  This does _not_ mean that the
  command has been executed yet, just that it is waiting to be executed.

DEPENDENCIES
  The HDRMC task must have been started up already.  All of the items in
  the mc_cmd_type must be already set.

RETURN VALUE
  None

SIDE EFFECTS
  The mc_cmd_type is placed on the HDRMC command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/
void hdrmc_cmd (
  HDRMC_CMD_TYPE  *msg_ptr   /* Pointer to MC message buffer */
)
{
  (void) q_link( msg_ptr, &msg_ptr->hdr.cmd_hdr.link);  /* init link */
  q_put( &hdrmc.cmd_q, &msg_ptr->hdr.cmd_hdr.link);        /* and queue it */

  /* signal the HDRMC task */
  (void) rex_set_sigs( HDRMC_TASK_PTR, HDRMC_CMD_Q_SIG );


} /* hdrmc_cmd */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRMC_GET_CMD_FREE_Q_PTR

DESCRIPTION
  This function return the the HDRMC free queue pointer
DEPENDENCIES
  None.

RETURN VALUE
  NULL, since this queue no longer exists.


SIDE EFFECTS
  None.
===========================================================================*/
q_type *hdrmc_get_cmd_free_q_ptr(void)
{
  return NULL;
} /* hdrmc_get_cmd_free_q_ptr */




/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_PROCESS_WDOG

DESCRIPTION
  This procedure reports wdog and starts the wdog timer.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_process_wdog( void )
{
   (void) rex_clr_sigs( HDRMC_TASK_PTR, HDRMC_RPT_TIMER_SIG );
   dog_hb_report( DOG_HDRMC_RPT );
} /* hdrmc_process_wdog() */

/*===========================================================================

FUNCTION HDRMC_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  This routine also
  checks any queues that are being waited on.  If the queue has an item,
  this routine sets the signal without waiting.  This allows for the queue
  signal to be cleared after each item is removed.  If a queue signal is set,
  this routine checks the queue and clears the signal if there is nothing
  on it.  This centralizes the queue checking.

DEPENDENCIES
  None

PARAMETERS
  requested_mask - Mask of signals to wait for

RETURN VALUE
  For non-queue signals, a signal bit is set in the return mask if the signal
  was in the requested mask and is also set in the REX TCB.

  For queue signals, a signal bit is set in the return mask if the signal was
  in the requested mask and the queue associated with the signal has an item
  on it.

SIDE EFFECTS
  None

===========================================================================*/

rex_sigs_type hdrmc_wait
(
  rex_sigs_type requested_mask
)
{
  rex_sigs_type queue_mask;       /* Mask of sigs indicating queue status    */
  rex_sigs_type rex_signals_mask; /* Mask of sigs returned by rex_get_sigs() */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (requested_mask == 0)
  {
    /* No sig to wait for --> Kick the watchdog and return to caller */
    (void) rex_clr_sigs( HDRMC_TASK_PTR, HDRMC_RPT_TIMER_SIG );
    dog_hb_report( DOG_HDRMC_RPT );
    return (0);
  }

  /* Initialize temporary mask holder for queue information */
  queue_mask = 0;

  /* Check input queue: If the request mask includes the signal for the
   * queue, check the queue and set the signal if there is an item on it.
   */

  if (requested_mask & HDRMC_CMD_Q_SIG)
  {
    if ( q_cnt( &hdrmc.cmd_q ) != 0 )
    {
      queue_mask |= HDRMC_CMD_Q_SIG;
    }
  }

  if (requested_mask & HDRMC_EVENT_Q_SIG)
  {
    if ( q_cnt( &hdrmc.event_q ) != 0 )
    {
      queue_mask |= HDRMC_EVENT_Q_SIG;
    }
  }

  /* Loop until at least one requested signal is set, either in the rex
   * TCB or due to a q_cnt() check that discovered an item on a queue.
   */
  do
  {
    if (queue_mask == 0)
    {
      /* Report to the watchdog, then set a timer so we wake up periodically
       * and report to the watchdog again.
       */
      hdrmc_process_wdog();
        /* report wdog */

      rex_signals_mask = rex_wait( requested_mask | HDRMC_RPT_TIMER_SIG );
    }
    else
    {
      /* No need to wait, just get the latest signals from the TCB */
      rex_signals_mask = rex_get_sigs( rex_self() );
    }

    /* Make sure that if we got a queue signal there is actually an
     * item on the queue.
     */
    if (rex_signals_mask & HDRMC_CMD_Q_SIG)
    {
      if (q_cnt( &hdrmc.cmd_q ) == 0)
      {
        rex_signals_mask &= ~(rex_sigs_type)HDRMC_CMD_Q_SIG;
        (void) rex_clr_sigs( rex_self(), HDRMC_CMD_Q_SIG );
      }
    }

    if (rex_signals_mask & HDRMC_EVENT_Q_SIG)
    {
      if (q_cnt( &hdrmc.event_q ) == 0)
      {
        rex_signals_mask &= ~(rex_sigs_type)HDRMC_EVENT_Q_SIG;
        (void) rex_clr_sigs( rex_self(), HDRMC_EVENT_Q_SIG );
      }
    }
  } while ((queue_mask == 0) &&
           ((requested_mask & rex_signals_mask) == 0));

  /* Kick watchdog one last time on the way out */
  (void) rex_clr_sigs( HDRMC_TASK_PTR, HDRMC_RPT_TIMER_SIG );
  dog_hb_report( DOG_HDRMC_RPT );

  /* Assemble return rex_sigs_type.  Only return signals that were requested */
  return ((rex_signals_mask | queue_mask) & requested_mask);

} /* hdrmc_wait() */

/* <EJECT> */ 

#ifdef FEATURE_HDR_MC_MSGR
/*============================================================================
FUNCTION HDRMC_MSGR_INIT

DESCRIPTION
  Initializes the message interface. 

  Initializes message interface by doing the following. 
    - Creates a messenger Client ID for HDRMC module. 
    - Initializes the free queue and event queue. 
    - Registers the rex queues for the client with the messenger. 
    - Registers to receive indication messages. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_msgr_init ( void )
{

  errno_enum_type  estatus = E_SUCCESS;
    /* Error status */

  msgr_id_t queue_id;
    /* Queue Identifier */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  estatus = msgr_client_create ( &hdrmc.msgr_client_id );

  ASSERT ( estatus == E_SUCCESS );

 /* Create the msgr client and associated with rex queues */
  estatus = msgr_client_add_rex_q ( &hdrmc.msgr_client_id,
                                    HDRMC_TASK_PTR,
                                    HDRMC_CMD_Q_SIG,
                                    &hdrmc.cmd_q,
                                    MSGR_NO_QUEUE,
                                    MSGR_OFFSETOF( hdrmc_msgr_s_type, msg ),
                                    MSGR_OFFSETOF( hdrmc_hdr_type, cmd ),
                                    (uint16) sizeof( hdrmc_msgr_s_type ),
                                    &queue_id );

  ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_LTE_TO_HDR
  /* Register for messages */
  estatus = msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_LTE_REDIR_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_LTE_ABORT_REDIR_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_LTE_1X_HRPD_CAPABILITIES_REQ );

  estatus = msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_LTE_RESEL_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_LTE_ABORT_RESEL_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_LTE_REVERSE_TIMING_TRANSFER_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_LTE_ABORT_REVERSE_TIMING_TRANSFER_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_LTE_GET_CGI_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_LTE_ABORT_CGI_REQ );
#endif /* FEATURE_LTE_TO_HDR */

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_CM_LTE_FPLMN_UPDATE_REQ );

#ifdef FEATURE_LTE_DEPRIORITIZATION
  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_LTE_DEPRI_FREQ_REQ );
#endif /* FEATURE_LTE_DEPRIORITIZATION */

#ifdef FEATURE_HDR_TO_LTE
  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            LTE_RRC_eHRPD_RESEL_FAILED_RSP );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            LTE_RRC_eHRPD_ABORT_RESEL_RSP );
#endif /* FEATURE_HDR_TO_LTE */

#if defined FEATURE_HDR_TO_LTE_REDIRECTION || defined FEATURE_HDR_TO_LTE
  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            LTE_RRC_ACQ_DB_EARFCN_RSP );
#endif /* defined FEATURE_HDR_TO_LTE_REDIRECTION || defined FEATURE_HDR_TO_LTE */

#if defined FEATURE_HDR_TO_LTE_REDIRECTION 

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            LTE_RRC_eHRPD_REDIR_FAILED_RSP );
	
  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            LTE_RRC_eHRPD_ABORT_REDIR_RSP );
		
#endif /* defined FEATURE_HDR_TO_LTE_REDIRECTION */


#ifdef FEATURE_LTE_TO_HDR_OH
  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            LTE_RRC_HDR_SIB8_INFO_IND );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            LTE_RRC_HDR_PRE_REG_INFO_IND );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_LTE_ACTIVE_HO_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_LTE_ABORT_ACTIVE_HO_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            NAS_EMM_ATTACH_COMPLETE_IND );

  /* Start SAP message router */
  hdrsap_msgr_start();

#endif /* FEATURE_LTE_TO_HDR_OH */

estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_PROT_ACT_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_PROT_DEACT_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_PROT_PH_STAT_CHGD_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_PROT_GEN_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_CALL_ORIGINATION_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_CALL_END_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_IDLE_PGSLOT_CHANGED_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_UNLOCK_HDR_RF_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_BCMCS_UPDATE_FLOW_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_BCMCS_LEGACY_UPDATE_FLOW_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_BCMCS_DISCONTINUE_BROADCAST_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_BCMCS_LEGACY_DISCONTINUE_BROADCAST_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_SEND_DATA_OVER_SIGNALING_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_TUNNEL_MSG_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_CSNA_MSG_CONFIG_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_BCMCS_SEND_REGISTRATION_REQ );

#if defined(FEATURE_HDR_BCMCS_2p0) && defined(FEATURE_HDR_BCMCS_DYNAMIC_MODEL)
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 && FEATURE_HDR_BCMCS_DYNAMIC_MODEL */

#ifdef FEATURE_HDR_QCHAT
  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_QCHAT_OPT_REQ );
#endif /*FEATURE_HDR_QCHAT*/

#ifdef FEATURE_LTE_TO_HDR

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_REDIR_PROTOCOL_CNF_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_ACTIVATE_PROTOCOL_CNF_REQ );
#endif /* FEATURE_LTE_TO_HDR */


  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_SET_FTD_STATS_MASK_REQ );  

#ifdef FEATURE_IRAT_PILOT_MEAS

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_MEAS_MODE_ON_REQ ); 
  
  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_MEAS_MODE_OFF_REQ ); 

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_MEAS_REQ ); 
#endif /* FEATURE_IRAT_PILOT_MEAS */

#ifdef FEATURE_HDR_DUAL_SIM
  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_STANDBY_PREF_CHGD_REQ ); 
#endif /* FEATURE_HDR_DUAL_SIM */

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_CM_ABORT_LTE_RESEL_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP, 
                             &hdrmc.msgr_client_id,
                             MSGR_ID_REX,
                             MM_MMOC_OFFLINE_IND );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            MM_CM_TRM_PRIORITY_IND );

  
  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_CM_CHECK_PSM_READY_REQ );

  estatus |= msgr_register ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MSGR_ID_REX,
                            HDR_CP_UE_MODE_CHANGE_REQ );

  ASSERT ( estatus == E_SUCCESS );

} /* hdrmc_msgr_init() */

/*===========================================================================
FUNCTION HDRMC_MSGR_STOP

DESCRIPTION
  Cleansup the message interface. 

  Cleans up the message interface by doing the following. 
    - De-registers the registered messages. 
    - Deletes the messenger Client ID for Decoder module. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_msgr_stop ( void )
{

  errno_enum_type  estatus = E_SUCCESS;
    /* Error status */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrmc.msgr_client_id == 0)
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, 
             "MSGR already deregistered clientID= %d", hdrmc.msgr_client_id);
    return;
  }

#ifdef FEATURE_LTE_TO_HDR
  estatus = msgr_deregister ( MSGR_HDR_CP,
                              &hdrmc.msgr_client_id,
                              HDR_CP_LTE_REDIR_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_LTE_ABORT_REDIR_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_LTE_1X_HRPD_CAPABILITIES_REQ );

  estatus = msgr_deregister ( MSGR_HDR_CP,
                              &hdrmc.msgr_client_id,
                              HDR_CP_LTE_RESEL_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_LTE_ABORT_RESEL_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_LTE_REVERSE_TIMING_TRANSFER_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_LTE_ABORT_REVERSE_TIMING_TRANSFER_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_LTE_GET_CGI_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_LTE_ABORT_CGI_REQ );
#endif /* FEATURE_LTE_TO_HDR */

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_CM_LTE_FPLMN_UPDATE_REQ );

#ifdef FEATURE_LTE_DEPRIORITIZATION
  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_LTE_DEPRI_FREQ_REQ );
#endif /* FEATURE_LTE_DEPRIORITIZATION */

#ifdef FEATURE_HDR_TO_LTE
  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               LTE_RRC_eHRPD_RESEL_FAILED_RSP );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               LTE_RRC_eHRPD_ABORT_RESEL_RSP );
#endif /* FEATURE_HDR_TO_LTE */

#if defined FEATURE_HDR_TO_LTE_REDIRECTION || defined FEATURE_HDR_TO_LTE
  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               LTE_RRC_ACQ_DB_EARFCN_RSP );
#endif /* defined FEATURE_HDR_TO_LTE_REDIRECTION || defined FEATURE_HDR_TO_LTE */

#if defined FEATURE_HDR_TO_LTE_REDIRECTION 

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,                            
                            LTE_RRC_eHRPD_REDIR_FAILED_RSP );
	
  estatus |= msgr_deregister ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            LTE_RRC_eHRPD_ABORT_REDIR_RSP );
		
#endif /* defined FEATURE_HDR_TO_LTE_REDIRECTION */

#ifdef FEATURE_LTE_TO_HDR_OH
  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               LTE_RRC_HDR_SIB8_INFO_IND );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               LTE_RRC_HDR_PRE_REG_INFO_IND );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_LTE_ACTIVE_HO_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_LTE_ABORT_ACTIVE_HO_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               NAS_EMM_ATTACH_COMPLETE_IND );

  /* Stop SAP message router */
  hdrsap_msgr_stop();

#endif /* FEATURE_LTE_TO_HDR_OH */

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_PROT_ACT_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_PROT_DEACT_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_PROT_PH_STAT_CHGD_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_PROT_GEN_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_CALL_ORIGINATION_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_CALL_END_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_IDLE_PGSLOT_CHANGED_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_UNLOCK_HDR_RF_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_BCMCS_UPDATE_FLOW_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_BCMCS_LEGACY_UPDATE_FLOW_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_BCMCS_DISCONTINUE_BROADCAST_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_BCMCS_LEGACY_DISCONTINUE_BROADCAST_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_SEND_DATA_OVER_SIGNALING_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_TUNNEL_MSG_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_CSNA_MSG_CONFIG_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_BCMCS_SEND_REGISTRATION_REQ );

#if defined(FEATURE_HDR_BCMCS_2p0) && defined(FEATURE_HDR_BCMCS_DYNAMIC_MODEL)
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 && FEATURE_HDR_BCMCS_DYNAMIC_MODEL */

#ifdef FEATURE_HDR_QCHAT
  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_QCHAT_OPT_REQ );
#endif /*FEATURE_HDR_QCHAT*/

#ifdef FEATURE_LTE_TO_HDR

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_REDIR_PROTOCOL_CNF_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_ACTIVATE_PROTOCOL_CNF_REQ );
#endif /* FEATURE_LTE_TO_HDR */

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_SET_FTD_STATS_MASK_REQ );  

#ifdef FEATURE_IRAT_PILOT_MEAS
  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_MEAS_MODE_ON_REQ ); 
  
  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_MEAS_MODE_OFF_REQ ); 

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_MEAS_REQ ); 
#endif /* FEATURE_IRAT_PILOT_MEAS */

#ifdef FEATURE_HDR_DUAL_SIM
  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_STANDBY_PREF_CHGD_REQ ); 
#endif /* FEATURE_HDR_DUAL_SIM */

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                               &hdrmc.msgr_client_id,
                               HDR_CP_CM_ABORT_LTE_RESEL_REQ );

  estatus |= msgr_deregister ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            MM_CM_TRM_PRIORITY_IND );  

  
  estatus = msgr_deregister ( MSGR_HDR_CP,
                              &hdrmc.msgr_client_id,
                              HDR_CP_CM_CHECK_PSM_READY_REQ );
  
  estatus |= msgr_deregister ( MSGR_HDR_CP,
                            &hdrmc.msgr_client_id,
                            HDR_CP_UE_MODE_CHANGE_REQ );

  ASSERT ( estatus == E_SUCCESS );

  estatus = msgr_client_delete ( &hdrmc.msgr_client_id );

  ASSERT ( estatus == E_SUCCESS );

} /* hdrmc_msgr_stop() */
#endif /* FEATURE_HDR_MC_MSGR */

#ifdef FEATURE_MODEM_CONFIG_REFRESH

/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRMC_MCFG_REFRESH_CB

DESCRIPTION
  This callback is called by MCFG when modem configuration is going to be
  changed and refreshed. The refresh command is queued to HDRMC and processed
  in HDRMC as MCFG has limited stack size.

DEPENDENCIES
  None

PARAMETERS
  refresh_info - modem config refresh information

RETURN VALUE
  Whether the refresh cmd is queued to HDR to process.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmc_mcfg_refresh_cb
(
  mcfg_refresh_info_s_type *refresh_info
)
{

  HDRMC_CMD_TYPE *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_4( MSG_LEGACY_MED, 
                  "refresh cmd, type (%d), slot (%d), sub (%d), ref ID (%d)",
                  refresh_info->type, refresh_info->slot_index,
                  refresh_info->sub_index, refresh_info->reference_id );

  /* Check if it is for primary session */
  if ( refresh_info->sub_index != MCFG_REFRESH_INDEX_0 )
  {
    return FALSE;
  }

  if ( ( cmd_ptr = ( HDRMC_CMD_TYPE * ) modem_mem_alloc( sizeof( 
                   HDRMC_CMD_TYPE ), MODEM_MEM_CLIENT_HDR_CRIT ) ) == 
                   NULL )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Not enough memory for hdrmc cmd.");
    return FALSE;
  }

  hdrmc.mcfg_refresh_is_in_progress = TRUE;  
  /* As MCFG's stack size is limited, we will queue the refresh cmd and
   * process it in HDR stack. */
  cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  cmd_ptr->hdr.cmd = HDRMC_MODEM_CONFIG_REFRESH;
  cmd_ptr->msgr_cmd.msg.modem_config_refresh.mcfg_refresh_type = (uint8)refresh_info->type;
  cmd_ptr->msgr_cmd.msg.modem_config_refresh.mcfg_refresh_slot_index = (uint8)refresh_info->slot_index;
  cmd_ptr->msgr_cmd.msg.modem_config_refresh.mcfg_refresh_sub_index = (uint8)refresh_info->sub_index;
  cmd_ptr->msgr_cmd.msg.modem_config_refresh.reference_id = refresh_info->reference_id;
  hdrmc_cmd( cmd_ptr );

  return TRUE;

} /* hdrmc_mcfg_refresh_cb() */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRMC_TASK_INIT

DESCRIPTION
  This function performs HDRMC task static initialization.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_task_init( void )
{
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  mcfg_refresh_registration_s_type mcfg_reg;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  /* Initialize queues. */
  ( void ) q_init( &hdrmc.event_q );
  ( void ) q_init( &hdrmc.cmd_q );

  hdrmc.type_of_exit = HDRMC_EXIT_NORMAL;
  hdrmc.powerup_inactive = TRUE;
  hdrmc.is_initialized = FALSE;
  hdrmc.mcfg_refresh_is_in_progress = FALSE;
  hdrmc.tc_mode_info.pending = FALSE;
  hdrmc.tc_mode_info.mode = HDRSRCH_DEFAULT_TC_MODE;
#ifdef FEATURE_HDR_DEBUG_INFO
  memset(&(hdrmc.dbg_info),0, sizeof(hdr_dbg_info_t));
#endif
  hdrcallid_deallocate_all_call_ids();

#ifndef FEATURE_MODEM_RCINIT
#ifdef FEATURE_HDR_MC_MSGR
  hdrmc_msgr_init();
#endif /* FEATURE_HDR_MC_MSGR */
#endif /* FEATURE_MODEM_RCINIT */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  mcfg_reg.order = MCFG_REFRESH_ORDER_50;
  mcfg_reg.slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
  /* HDR only cares about primary session */
  mcfg_reg.sub_mask = MCFG_REFRESH_INDEX_MASK_0;
  mcfg_reg.group = 0;
  mcfg_reg.cb = hdrmc_mcfg_refresh_cb;

  mcfg_refresh_register(&mcfg_reg);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  /* Init SLP/PCP. This needs to be done before RX/TX task */
  hdrslp_init();
  (void) hdrpcp_init();

} /* hdrmc_task_init() */

/* <EJECT> */ 
#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================

FUNCTION HDRMC_EXTRACT_LTE_TO_HDR_HO_CHAN

DESCRIPTION
  This function extracts a channel record from a redirect/reselect information 
  pointed to by the void_info_ptr.  A pointer to this function is passed to SD 
  along with a pointer to the void_info_ptr containing the redirect message 
  payload from LTE, and SD passes the pointer back here to get the channels
  one at a time.

DEPENDENCIES
  None

PARAMETERS
  void_item_ptr - Pointer to the redirect information payload from LTE
  index - Which channel is being requested
  system_ptr - Where to place the extracted system information

RETURN VALUE
  TRUE if a channel is successfully extracted, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean hdrmc_extract_lte_to_hdr_ho_chan
(
  void *void_info_ptr,
  int chan_idx,
  sd_sys_s_type *system_ptr
)
{
  uint8 band_idx = 0;
  uint8 num_freq = 0;
  uint8 prev_num_freq = 0;
    /* Loop counters */
  boolean ret_val = FALSE;
    /* Whether channel info is being returned or not */
  hdrmc_lte_to_hdr_ho_info_type *lte_to_hdr_ho_info_ptr;
    /* Pointer to redirect/reselect information */
  hdrcp_redir_freq_per_band_list *freq_per_band_list;
    /* Pointer to redirect band & ch information */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the convenience pointer */
  lte_to_hdr_ho_info_ptr = (hdrmc_lte_to_hdr_ho_info_type*) void_info_ptr;

  if(lte_to_hdr_ho_info_ptr == NULL)
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                      "Passed ptr is NULL. Returning False !!");
    return FALSE;
  }
  
  ASSERT ( lte_to_hdr_ho_info_ptr->irat_ho_type != HDRMC_IRAT_NONE );

  if ( (lte_to_hdr_ho_info_ptr->irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_RESELECT) ||
       (lte_to_hdr_ho_info_ptr->irat_ho_type == HDRMC_IRAT_HDR_TO_LTE_TIME_TRANSFER) ||
       (lte_to_hdr_ho_info_ptr->irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_GET_CGI) )
  {
    if (chan_idx == 0)
    {
      system_ptr->band = lte_to_hdr_ho_info_ptr->sys_info.band;
      system_ptr->chan = lte_to_hdr_ho_info_ptr->sys_info.chan_num;
      system_ptr->mode = SYS_SYS_MODE_HDR;
      ret_val = TRUE;
    }
    else
    {
      ret_val = FALSE;
    }
  }
  else if (lte_to_hdr_ho_info_ptr->irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_REDIRECT)
  {
    freq_per_band_list = lte_to_hdr_ho_info_ptr->freq_per_band_list;

    if(lte_to_hdr_ho_info_ptr->band_count > HDR_CP_MAX_CDMA2000_BANDS)
    {
    
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "band_idx (%d) more than allowed. returning FALSE",
                      lte_to_hdr_ho_info_ptr->band_count);

      return FALSE;
    }
    
    num_freq = lte_to_hdr_ho_info_ptr->freq_per_band_list[band_idx].freq_count;

    /* Loop to find the channel information corresponding to the index */
    while (( chan_idx > (num_freq -1) ) &&
           ( band_idx < lte_to_hdr_ho_info_ptr->band_count ))
    {
      prev_num_freq = num_freq;
      band_idx++;
      num_freq += freq_per_band_list[band_idx].freq_count;
    }

    /* If the index is within range */
    if ( band_idx < lte_to_hdr_ho_info_ptr->band_count )
    {
      chan_idx = chan_idx - prev_num_freq;

      if((chan_idx >= 0) &&(chan_idx < HDR_CP_MAX_CDMA2000_FREQUENCIES_PER_BAND))
      {
       system_ptr->band = freq_per_band_list[band_idx].band_class;
       system_ptr->chan = freq_per_band_list[band_idx].freq_list[chan_idx];
       system_ptr->mode = SYS_SYS_MODE_HDR;

       ret_val = TRUE;
      }
       else
      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                        "chan_idx (%d) more than allowed. returning FALSE",
                         chan_idx);
      }
    }
  }
  else 
  {
    ret_val = FALSE;
  }

  return ret_val;
} /* hdrmc_extract_lte_to_hdr_ho_chan() */

/*===========================================================================
FUNCTION HDRMC_CLR_LTE_TO_HDR_HO_IN_PROGRESS

DESCRIPTION
  This function clears the flag that indicates LTE to HDR handoff is in 
  progress.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrmc_clr_lte_to_hdr_ho_in_progress( void )
{
#ifdef FEATURE_LTE_TO_HDR_OH
  dsm_item_type           *pkt_ptr = NULL;
  msgr_attach_struct_type *att_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == 
      HDRMC_IRAT_LTE_TO_HDR_ACTIVE_HO)
  {
    if (hdrmc.lte_to_hdr_ho_info.active_ho_info.free_dsm_item == TRUE)
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                   "Active handoff failed dsm item is being freed");

      att_ptr = msgr_get_attach(
          &(hdrmc.lte_to_hdr_ho_info.active_ho_info.msg_payload.msg_hdr), 0);
      ASSERT(att_ptr != NULL)

      msgr_get_dsm_attach(att_ptr, &pkt_ptr);
      ASSERT(pkt_ptr != NULL);

      /* Free dsm item*/
      dsm_free_packet( &pkt_ptr );
    }
    else
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "Active handoff dsm already freed");
    }
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  hdrmc.lte_to_hdr_ho_info.irat_ho_type = HDRMC_IRAT_NONE;

} /* hdrmc_clr_lte_to_hdr_ho_in_progress() */

#ifdef FEATURE_LTE_TO_HDR_OH
/* <EJECT> */

/*===========================================================================

FUNCTION HDRMC_RETURN_TO_TUNNEL_MODE

DESCRIPTION
  This function needs to be called when AT is going to return to tunnel
  after successful IRAT procedures (e.g., get CGI, Reverse Time Transfer)
  or handoff failure/abort.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmc_return_to_tunnel_mode( void )
{
  rex_tcb_type *dec_tcb_ptr; 

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !hdrmc.tunnel_activation_is_pending && 
       ( hdrsap_get_current_tunnel_mode_value( HDRHAI_HDRMC ) 
           == HDRHAI_TUNNEL_MODE_NO_SRV ) )
  {
    /* If AT is in TUNNEL_MODE_NO_SRV state before handoff, AT should go back
     * to TUNNEL_MODE_NO_SRV state after handoff is aborted and wait for 
     * tunnel activation to enter tunnel mode. */
    hdralmp_activate( HDRHAI_HDRMC );
  }
  else
  {
    hdrmc_set_tunnel_activation_is_pending( FALSE );

    /* In active handoff failure case, we want to Update UATI to notify network
     * that UE fails to acquire on native DO and goes back to LTE (tunnel). */
    if ( hdrmc.lte_to_hdr_ho_info.irat_ho_type == 
           HDRMC_IRAT_LTE_TO_HDR_ACTIVE_HO )
    {
      hdramp_set_hdr_to_lte_resel( TRUE );
    }

    /* Reset SAP to tunnel mode;
     * Activate ALMP and OVHD to move it back to INIT and TUNNEL state */
    hdrsap_activate( HDRHAI_HDRMC );
  }

  /* Set signal to HDR decoder task to exit HDR online mode since decoder
   * is not used in tunnel mode */
  dec_tcb_ptr = rcinit_lookup_rextask( "hdrdec" );
  ( void ) rex_set_sigs( dec_tcb_ptr, HDRDEC_EXIT_HDR_SIG );

} /* hdrmc_return_to_tunnel_mode() */
#endif /* FEATURE_LTE_TO_HDR_OH */


/* <EJECT> */

/*===========================================================================

FUNCTION HDRMC_SEND_IRAT_HO_FAILURE_IND_TO_LTE

DESCRIPTION
  This function returns failure response to LTE when DO redirection/
  reselection attempt fails.

DEPENDENCIES
  None
 
PARAMETERS
  back_to_tunnel - whether HDR should go back to tunnel if tunnel mode is enabled.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_send_irat_ho_failure_ind_to_lte
(
  boolean back_to_tunnel
)
{
  hdrcp_msg_rsp_u irat_ho_rsp;
    /* Message to inform LTE about redirect failure */

  errno_enum_type estatus = E_FAILURE;
    /* Error status */
#ifdef FEATURE_HDR_DEBUG_INFO
  hdrmc_sent_cmd_type cmd_type = HDR_NONE;
#endif /* FEATURE_HDR_DEBUG_INFO */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_REDIRECT)
  {
    msgr_init_hdr ( &irat_ho_rsp.lte_redirect_failed.hdr, 
                    MSGR_HDR_CP, 
                    HDR_CP_LTE_REDIR_FAILED_RSP );

    estatus = msgr_send ( &irat_ho_rsp.lte_redirect_failed.hdr, 
                          sizeof ( hdrcp_msg_rsp_u ) );
  }
  else if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_RESELECT)
  {
    msgr_init_hdr ( &irat_ho_rsp.lte_reselect_failed.hdr, 
                    MSGR_HDR_CP, 
                    HDR_CP_LTE_RESEL_FAILED_RSP );

    estatus = msgr_send ( &irat_ho_rsp.lte_reselect_failed.hdr, 
                          sizeof ( hdrcp_msg_rsp_u ) );
  }
  else if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_HDR_TO_LTE_TIME_TRANSFER)
  {
    msgr_init_hdr ( &irat_ho_rsp.lte_rev_tt.hdr, 
                    MSGR_HDR_CP,
                    HDR_CP_LTE_REVERSE_TIMING_TRANSFER_RSP );

    irat_ho_rsp.lte_rev_tt.status = HDRCP_LTE_REVERSE_TIME_TRANSFER_FAILED;

    estatus = msgr_send ( &irat_ho_rsp.lte_rev_tt.hdr,
                          sizeof ( hdrcp_msg_rsp_u ) );
  }
  else if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_GET_CGI)
  {
    msgr_init_hdr ( &irat_ho_rsp.lte_get_cgi_rsp.hdr, 
                    MSGR_HDR_CP,
                    HDR_CP_LTE_GET_CGI_RSP );

#ifdef FEATURE_MEMCPY_REMOVAL
    irat_ho_rsp.lte_get_cgi_rsp.sys_info = hdrmc.lte_to_hdr_ho_info.sys_info;
#else
    memcpy ( &irat_ho_rsp.lte_get_cgi_rsp.sys_info, 
             &hdrmc.lte_to_hdr_ho_info.sys_info,
             sizeof(sys_channel_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

    irat_ho_rsp.lte_get_cgi_rsp.pilot_pn =
        hdrmc.lte_to_hdr_ho_info.pilot_pn[0];

    irat_ho_rsp.lte_get_cgi_rsp.cgi_info_included = FALSE;

    estatus = msgr_send ( &irat_ho_rsp.lte_get_cgi_rsp.hdr,
                          sizeof ( hdrcp_msg_rsp_u ) );
  }
#ifdef FEATURE_LTE_TO_HDR_OH
  else if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_ACTIVE_HO)
  {
    msgr_init_hdr ( &irat_ho_rsp.lte_active_ho_failed.hdr, 
                    MSGR_HDR_CP,
                    HDR_CP_LTE_ACTIVE_HO_FAILED_RSP );

    estatus = msgr_send ( &irat_ho_rsp.lte_active_ho_failed.hdr,
                          sizeof ( hdrcp_msg_rsp_u ) );
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( back_to_tunnel &&
       ( hdrsap_tunnel_mode_is_enabled(HDRHAI_HDRMC) ||
         hdrmc.tunnel_activation_is_pending ) )
  {
    hdrmc_return_to_tunnel_mode();
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_DEBUG_INFO
   cmd_type = hdrmc_get_irat_cmdtype(hdrmc.lte_to_hdr_ho_info.irat_ho_type);
   hdrmc_log_sent_rpt(0, cmd_type);
#endif /* FEATURE_HDR_DEBUG_INFO */
  hdrmc_clr_lte_to_hdr_ho_in_progress();

} /* hdrmc_send_irat_ho_failure_ind_to_lte() */

/*===========================================================================

FUNCTION HDRMC_SEND_TIMING_INFO_TO_LTE

DESCRIPTION
  This function is called to send the timing information back to LTE.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_send_timing_info_to_lte( void )
{
  hdrcp_msg_rsp_u tt_rsp;
    /* Message to inform LTE about abort done. */
  errno_enum_type estatus;
    /* Error status */
  hdrinit_timing_info_type timing_info;
    /* HDR timing information */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (hdrmc.lte_to_hdr_ho_info.irat_ho_type == 
          HDRMC_IRAT_HDR_TO_LTE_TIME_TRANSFER);

  msgr_init_hdr ( &tt_rsp.lte_rev_tt.hdr, MSGR_HDR_CP,
                  HDR_CP_LTE_REVERSE_TIMING_TRANSFER_RSP );

  /* Clear the timing info */
  memset ( &timing_info, 0, sizeof(hdrinit_timing_info_type) );

  /* Obtain the timing information stored in INIT */
  hdrinit_get_timing_info( &timing_info );

  /* Populate the return value to LTE based on whether it is valid or not */
  if (timing_info.is_valid)
  {
    tt_rsp.lte_rev_tt.status = HDRCP_LTE_REVERSE_TIME_TRANSFER_SUCCEEDED; 
    tt_rsp.lte_rev_tt.sys_info = timing_info.sys_info;
    tt_rsp.lte_rev_tt.cdma_sys_time = timing_info.cdma_sys_time;
    tt_rsp.lte_rev_tt.lte_ostmr = timing_info.lte_ostmr;
  }
  else
  {
    tt_rsp.lte_rev_tt.status = HDRCP_LTE_REVERSE_TIME_TRANSFER_FAILED;
  }

  estatus = msgr_send ( &tt_rsp.hdr, sizeof ( hdrcp_msg_rsp_u ) );

  ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_HDRMC ) ||
       hdrmc.tunnel_activation_is_pending )
  {
    hdrmc_return_to_tunnel_mode();
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_DEBUG_INFO
    hdrmc_log_sent_rpt(0, HDR_LTE_REVERSE_TIMING_TRANSFER_RSP);
#endif /* FEATURE_HDR_DEBUG_INFO */
  hdrmc_clr_lte_to_hdr_ho_in_progress();

} /* hdrmc_send_timing_info_to_lte() */

/*===========================================================================

FUNCTION HDRMC_SEND_CGI_TO_LTE

DESCRIPTION
  This function is called to send the CGI back to LTE.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_send_cgi_to_lte( void )
{
  hdrcp_msg_rsp_u cgi_rsp;
    /* Message to inform LTE CGI info. */
  errno_enum_type estatus;
    /* Error status */
  hdrerrno_enum_type err_no;
  uint8 subnet_mask;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (hdrmc.lte_to_hdr_ho_info.irat_ho_type == 
          HDRMC_IRAT_LTE_TO_HDR_GET_CGI);

  msgr_init_hdr ( &cgi_rsp.lte_get_cgi_rsp.hdr, MSGR_HDR_CP,
                  HDR_CP_LTE_GET_CGI_RSP );

  /* Obtain the CGI info stored in OVHD */
  err_no = hdrovhd_get_sector_id( cgi_rsp.lte_get_cgi_rsp.cgi_info,
                         &subnet_mask,
                         HDRHAI_HDRMC );

#ifdef FEATURE_MEMCPY_REMOVAL
  cgi_rsp.lte_get_cgi_rsp.sys_info = hdrmc.lte_to_hdr_ho_info.sys_info;
#else
  memcpy ( &cgi_rsp.lte_get_cgi_rsp.sys_info, 
           &hdrmc.lte_to_hdr_ho_info.sys_info,
           sizeof(sys_channel_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  cgi_rsp.lte_get_cgi_rsp.pilot_pn =
      hdrmc.lte_to_hdr_ho_info.pilot_pn[0];

  cgi_rsp.lte_get_cgi_rsp.pilot_strength = 
    hdrovhd_get_cgi_pilot_strength( cgi_rsp.lte_get_cgi_rsp.pilot_pn );

  /* Populate the return value to LTE based on whether it is valid or not */
  if (err_no == E_SUCCESS)
  {
    cgi_rsp.lte_get_cgi_rsp.cgi_info_included = TRUE;
  }
  else
  {
    cgi_rsp.lte_get_cgi_rsp.cgi_info_included = FALSE;
  }

  HDR_MSG_PROT_4( MSG_LEGACY_HIGH,
                  "CGI resp: band=%d chan=%d PN=%d, CGI included (%d)",
                  cgi_rsp.lte_get_cgi_rsp.sys_info.band, 
                  cgi_rsp.lte_get_cgi_rsp.sys_info.chan_num, 
                  cgi_rsp.lte_get_cgi_rsp.pilot_pn,
                  cgi_rsp.lte_get_cgi_rsp.cgi_info_included );

  estatus = msgr_send ( &cgi_rsp.hdr, sizeof ( hdrcp_msg_rsp_u ) );

  ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_HDRMC ) ||
       hdrmc.tunnel_activation_is_pending )
  {
    hdrmc_return_to_tunnel_mode();
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_DEBUG_INFO
	hdrmc_log_sent_rpt(0, HDR_LTE_GET_CGI_RSP);
#endif /* FEATURE_HDR_DEBUG_INFO */
  hdrmc_clr_lte_to_hdr_ho_in_progress();

} /* hdrmc_send_cgi_to_lte() */

/*===========================================================================

FUNCTION HDRMC_SEND_ABORT_DONE_TO_LTE

DESCRIPTION
  This function returns LTE->HDR redirection abort done response to LTE stack.

DEPENDENCIES
  None

PARAMETERS
  back_to_tunnel - whether HDR should go back to tunnel if tunnel mode is enabled.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_send_abort_done_to_lte
(
  boolean back_to_tunnel
)
{
  hdrcp_msg_rsp_u irat_ho_abort_rsp;
    /* Message to inform LTE about abort done. */

  errno_enum_type estatus = E_FAILURE;
    /* Error status */
	
#ifdef FEATURE_HDR_DEBUG_INFO
  hdrmc_sent_cmd_type cmd_type = HDR_NONE;
#endif /* FEATURE_HDR_DEBUG_INFO */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_REDIRECT)
  {
    msgr_init_hdr ( &irat_ho_abort_rsp.hdr, 
                    MSGR_HDR_CP, 
                    HDR_CP_LTE_REDIR_ABORT_RSP );

    estatus = msgr_send ( &irat_ho_abort_rsp.hdr, 
                          sizeof ( hdrcp_msg_rsp_u ) );
  }
  else if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_RESELECT)
  {
    msgr_init_hdr ( &irat_ho_abort_rsp.hdr, 
                    MSGR_HDR_CP, 
                    HDR_CP_LTE_RESEL_ABORT_RSP );

    estatus = msgr_send ( &irat_ho_abort_rsp.hdr, 
                          sizeof ( hdrcp_msg_rsp_u ) );
  }
  else if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_HDR_TO_LTE_TIME_TRANSFER)
  {
    msgr_init_hdr ( &irat_ho_abort_rsp.hdr, 
                    MSGR_HDR_CP, 
                    HDR_CP_LTE_REVERSE_TIMING_TRANSFER_ABORT_RSP );

    estatus = msgr_send ( &irat_ho_abort_rsp.hdr, 
                          sizeof ( hdrcp_msg_rsp_u ) );
  }
  else if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_GET_CGI)
  {
    msgr_init_hdr ( &irat_ho_abort_rsp.hdr, 
                    MSGR_HDR_CP, 
                    HDR_CP_LTE_GET_CGI_RSP );

#ifdef FEATURE_MEMCPY_REMOVAL
    irat_ho_abort_rsp.lte_get_cgi_rsp.sys_info = 
                  hdrmc.lte_to_hdr_ho_info.sys_info;
#else
    memcpy ( &irat_ho_abort_rsp.lte_get_cgi_rsp.sys_info, 
             &hdrmc.lte_to_hdr_ho_info.sys_info,
             sizeof(sys_channel_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

    irat_ho_abort_rsp.lte_get_cgi_rsp.pilot_pn =
        hdrmc.lte_to_hdr_ho_info.pilot_pn[0];

    irat_ho_abort_rsp.lte_get_cgi_rsp.cgi_info_included = FALSE;

    estatus = msgr_send ( &irat_ho_abort_rsp.hdr, 
                          sizeof ( hdrcp_msg_rsp_u ) );
  }
#ifdef FEATURE_LTE_TO_HDR_OH
  else if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_ACTIVE_HO)
  {
    msgr_init_hdr ( &irat_ho_abort_rsp.hdr, 
                    MSGR_HDR_CP, 
                    HDR_CP_LTE_ACTIVE_HO_ABORT_RSP );

    estatus = msgr_send ( &irat_ho_abort_rsp.hdr, 
                          sizeof ( hdrcp_msg_rsp_u ) );
  }
#endif /* FEATURE_LTE_TO_HDR_OH */
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR,
                  "Recv L2D abort when AT is not any L2D IRAT" );
    estatus = E_SUCCESS;
  }

  ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( back_to_tunnel &&
       ( hdrsap_tunnel_mode_is_enabled(HDRHAI_HDRMC) ||
         hdrmc.tunnel_activation_is_pending ) )
  {
    hdrmc_return_to_tunnel_mode();
    }
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_DEBUG_INFO
   cmd_type = hdrmc_get_irat_cmdtype(hdrmc.lte_to_hdr_ho_info.irat_ho_type);
   hdrmc_log_sent_rpt(0, cmd_type);
#endif /* FEATURE_HDR_DEBUG_INFO */

  hdrmc_clr_lte_to_hdr_ho_in_progress();

  /* Clear abort flag */
  hdrmc.lte_to_hdr_ho_aborted = FALSE;

} /* hdrmc_send_abort_done_to_lte() */

/*===========================================================================

FUNCTION HDRMC_SEND_1X_HRPD_CAPABILITIES_TO_LTE

DESCRIPTION
  This function returns 1X and HRPD capabilities to LTE.

DEPENDENCIES
  None

PARAMETERS
  band_pref - band preference passed from LTE

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_send_1x_hrpd_capabilities_to_lte
(
  sys_band_mask_type band_pref
)
{
  hdrcp_msg_rsp_u msg_rsp;
    /* Message to inform LTE about 1x and HRPD capabilities. */

  errno_enum_type estatus;
    /* Error status */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Init message router message */
  msgr_init_hdr ( &msg_rsp.lte_capabilities_rsp.hdr, 
                  MSGR_HDR_CP, HDR_CP_LTE_1X_HRPD_CAPABILITIES_RSP );

  /* Get the band class information in the packed format */
  msg_rsp.lte_capabilities_rsp.capabilities_container_length = 
      hdrrupc_get_packed_cdma_bandclass_info( 
        (sd_ss_band_pref_e_type)band_pref,
        msg_rsp.lte_capabilities_rsp.capabilities_container );

  /* Send the message */
  estatus = msgr_send ( &msg_rsp.lte_capabilities_rsp.hdr, sizeof ( hdrcp_msg_rsp_u ) );

  ASSERT ( estatus == E_SUCCESS );
#ifdef FEATURE_HDR_DEBUG_INFO
  hdrmc_log_sent_rpt(0, HDR_LTE_1X_HRPD_CAPABILITIES_RSP);
#endif /* FEATURE_HDR_DEBUG_INFO */
} /* hdrmc_send_1x_hrpd_capabilities_to_lte() */

#endif /* FEATURE_LTE_TO_HDR */
/*===========================================================================

FUNCTION HDRMC_QUEUE_CMD

DESCRIPTION
  This function queues a command on the hdrmc.event_q and sets the signal.
  The command data is copied into the command buffer before it is queued.
  This function is called in the task context of whoever is giving the command.

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  commands to protocols in the MC task at the same time.

  cmd_size must not be larger than hdrmc_protocol_cmd_union_type, because
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the command is for
  input_cmd_ptr - Pointer to a union of all protocols in HDRMC task
  cmd_size - Size of the command being given

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_queue_cmd( hdrhai_protocol_name_enum_type protocol_name,
                          void * input_cmd_ptr, uint32 cmd_size )
{
  hdrmc_buf_type *buf_ptr;         /* buffer for command */
  hdrlog_cmd_type *log_cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_FACTORY_TESTMODE
  if(ftm_get_mode() == FTM_MODE) return;
#endif
  HDR_MSG_PROT_1 (MSG_LEGACY_LOW, "hdrmc_queue_cmd (%d)", protocol_name);

  if ( input_cmd_ptr == NULL )
  {
    ERR( "No command to queue!", 0, 0, 0 );
    return;
  }

  ASSERT (cmd_size <= sizeof(hdrmc_protocol_cmd_union_type) );

  if ( hdrmc.state == HDRMC_INACTIVE_STATE && 
       protocol_name == HDRHAI_LOGGING_MODULE )
  {
    log_cmd_ptr = (hdrlog_cmd_type *)input_cmd_ptr;
    if (log_cmd_ptr->name == HDR_WRITE_DATABASE_TO_EFS)
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "Cannot process HDR_WRITE_DATABASE_TO_EFS in offline state");
      return;
    }
  }

  /* Get a buffer from the external event free queue */
  if ( ( buf_ptr = ( hdrmc_buf_type * ) modem_mem_alloc( sizeof( 
                   hdrmc_buf_type ), MODEM_MEM_CLIENT_HDR_CRIT ) ) == 
                   NULL )
  {
    /*lint --e{527} statement unreachable, it could be defined as blank */
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "No free buffers on hdrmc.event_free_q");
    if ((protocol_name == HDRHAI_HDR_MESSAGING_PROTOCOL) &&
        (((hdrhmp_cmd_type *)input_cmd_ptr)->name == HDRHMP_SEND_MSG_CMD))
    {
      hdrhmp_msgs_on_event_q( -1 );
    }
    return;
  }
  else
  {
    HDR_MSG_PROT_2( MSG_LEGACY_LOW, "Buf ptr alloc = 0x%x , size = %d bytes.",
                    buf_ptr, sizeof( hdrmc_buf_type ) );
  }

  /* Fill in the buffer.  Make sure all fields are filled in. */
  buf_ptr->hdr.protocol_name = protocol_name;
  buf_ptr->hdr.type_of_item = HDRBUF_COMMAND;
#ifdef FEATURE_MEMCPY_REMOVAL
  (void) memscpy( &buf_ptr->item.cmd, 
                  sizeof(buf_ptr->item.cmd), input_cmd_ptr, cmd_size );
#else
  (void) memcpy( &buf_ptr->item.cmd, input_cmd_ptr, cmd_size );
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* Queue the buffer and set a signal */
  ( void ) q_link( buf_ptr, &buf_ptr->hdr.link );
  q_put( &hdrmc.event_q, &buf_ptr->hdr.link );
  ( void ) rex_set_sigs( HDRMC_TASK_PTR, HDRMC_EVENT_Q_SIG );

} /* hdrmc_queue_cmd() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_QUEUE_IND

DESCRIPTION
  This function queues an indication on the hdrmc.event_q and sets the
  signal.  If there is any data associated with the indication, it is copied
  into the buffer before it is queued.  This function is called in the task
  context of whoever is giving the indication.

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  an indication to protocols in the MC task at the same time.

  ind_data_size must not be larger than hdrind_ind_data_union_type, because
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  input_ind_ptr - Pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_queue_ind( hdrhai_protocol_name_enum_type protocol_name,
          hdrind_ind_name_enum_type ind_name,
          void *input_ind_data_ptr
   )
{
  hdrmc_buf_type *buf_ptr;        /* Buffer for indication */
  uint32 ind_data_size;           /* Size of data with indication */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_FACTORY_TESTMODE
  if(ftm_get_mode() == FTM_MODE) return;
#endif

  HDR_MSG_PROT_3 (MSG_LEGACY_LOW,  "hdrmc_queue_ind (%d, %d, %lx)",
                  protocol_name, ind_name, input_ind_data_ptr );


  /* Get a buffer from the internal event free queue */
  if ( ( buf_ptr = ( hdrmc_buf_type * ) modem_mem_alloc( sizeof( 
                   hdrmc_buf_type ), MODEM_MEM_CLIENT_HDR_CRIT ) ) == 
                   NULL )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "No free buffers on hdrmc.event_free_q");
    /*lint -e{527} suppress unreachable code */
    return;
  }
  else
  {
    HDR_MSG_PROT_2( MSG_LEGACY_LOW, "Buf ptr alloc = 0x%x , size = %d bytes.",
                    buf_ptr, sizeof( hdrmc_buf_type ) );
  }


  /* Fill in the buffer.  Make sure all fields are filled in. */
  buf_ptr->hdr.protocol_name = protocol_name;
  buf_ptr->hdr.type_of_item = HDRBUF_INDICATION;
  buf_ptr->item.ind.ind_name = ind_name;
  if ( input_ind_data_ptr != NULL )
  {
    ind_data_size = hdrind_get_ind_data_size (ind_name);

    ASSERT (ind_data_size <= sizeof(hdrind_ind_data_union_type) );
#ifdef FEATURE_MEMCPY_REMOVAL
    (void) memscpy( &buf_ptr->item.ind.ind_data, 
                    sizeof(buf_ptr->item.ind.ind_data), 
                    input_ind_data_ptr, ind_data_size );
#else
    (void) memcpy( &buf_ptr->item.ind.ind_data, input_ind_data_ptr,
                   ind_data_size );
#endif /* FEATURE_MEMCPY_REMOVAL */
  }

  /* Queue the buffer and set a signal */
  ( void ) q_link( buf_ptr, &buf_ptr->hdr.link );
  q_put( &hdrmc.event_q, &buf_ptr->hdr.link );
  ( void ) rex_set_sigs( HDRMC_TASK_PTR, HDRMC_EVENT_Q_SIG );

} /* hdrmc_queue_ind() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_QUEUE_MSG

DESCRIPTION
  This function queues a message on the hdrmc.event_q and sets the signal.
  The queued buffer simply holds a pointer to the DSM item chain that is
  the message.  This function is called in the task context of whoever is
  giving the message, which is always the task in which SLP runs.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the message is for
  protocol_instance - Instance of the protocol the message is for
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_queue_msg( hdrhai_protocol_name_enum_type protocol_name,
                      hdrhai_protocol_instance_enum_type protocol_instance,
                      dsm_item_type * item_ptr )
{
  hdrmc_buf_type *buf_ptr;   /* Buffer for message (w/pointer to DSM item) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_FACTORY_TESTMODE
  if (ftm_get_mode() == FTM_MODE) 
  {
    dsm_free_packet( &item_ptr );
    return;
  }    
#endif

  HDR_MSG_PROT_3 (MSG_LEGACY_LOW,  "hdrmc_queue_msg (%d:%d, %lx)",
                  protocol_instance, protocol_name, item_ptr);


  /* Get a buffer from the external event free queue */
  if ( ( buf_ptr = ( hdrmc_buf_type * ) modem_mem_alloc( sizeof( 
                   hdrmc_buf_type ), MODEM_MEM_CLIENT_HDR_CRIT ) ) == 
                   NULL )
  {
    /*lint --e{527} statement unreachable, it could be defined as blank */
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "No free buffers on hdrmc.event_free_q");
    dsm_free_packet( &item_ptr );
    return;
  }
  else
  {
    HDR_MSG_PROT_2( MSG_LEGACY_LOW, "Buf ptr alloc = 0x%x , size = %d bytes.",
                    buf_ptr, sizeof( hdrmc_buf_type ) );
  }


  /* Fill in the buffer.  Make sure all fields are filled in. */
  buf_ptr->hdr.protocol_name = protocol_name;
  buf_ptr->hdr.protocol_instance = protocol_instance;
  buf_ptr->hdr.type_of_item = HDRBUF_MESSAGE;
  buf_ptr->item.item_ptr = item_ptr;

  /* Queue the buffer and set a signal */
  ( void ) q_link( buf_ptr, &buf_ptr->hdr.link );
  q_put( &hdrmc.event_q, &buf_ptr->hdr.link );
  ( void ) rex_set_sigs( HDRMC_TASK_PTR, HDRMC_EVENT_Q_SIG );

} /* hdrmc_queue_msg() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_QUEUE_TIMER

DESCRIPTION
  This function queues a timer event on the hdrmc.event_q and sets the
  signal.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the indication is for
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_queue_timer
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint32 timer_id
)
{
  hdrmc_buf_type *buf_ptr;        /* Buffer for timer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_FACTORY_TESTMODE
  if(ftm_get_mode() == FTM_MODE) return;
#endif

  HDR_MSG_PROT_2 (MSG_LEGACY_LOW,  "hdrmc_queue_timer (%d, %d)",
                  protocol_name, timer_id);

  /* Get a buffer from the internal event free queue */
  if ( ( buf_ptr = ( hdrmc_buf_type * ) modem_mem_alloc( sizeof( 
                   hdrmc_buf_type ), MODEM_MEM_CLIENT_HDR_CRIT ) ) == 
                   NULL )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "No free buffers on hdrmc.event_free_q");
    /*lint -e{527} statement unreachable, it could be defined as blank */
    return;
  }
  else
  {
    HDR_MSG_PROT_2( MSG_LEGACY_LOW, "Buf ptr alloc = 0x%x , size = %d bytes.",
                    buf_ptr, sizeof( hdrmc_buf_type ) );
  }

  /* Fill in the buffer.  Make sure all fields are filled in. */
  buf_ptr->hdr.protocol_name = protocol_name;
  buf_ptr->hdr.type_of_item = HDRBUF_TIMER;
  buf_ptr->item.timer_id = timer_id;

  /* Queue the buffer and set a signal */
  ( void ) q_link( buf_ptr, &buf_ptr->hdr.link );
  q_put( &hdrmc.event_q, &buf_ptr->hdr.link );
  ( void ) rex_set_sigs( HDRMC_TASK_PTR, HDRMC_EVENT_Q_SIG );

} /* hdrmc_queue_timer() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_PROTOCOL_CMD

DESCRIPTION
  All asynchronous commands for protocols that run in the HDRMC task context
  are routed through here. This function dispatches the command to the
  protocol it is for.  Synchronous commands are processed in the context of
  the protocol issuing the command, so they are not queued and thus are not
  routed through here.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the command is for
  hdrmc_cmd_union_ptr - The received command to be routed

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrmc_protocol_cmd
(
  hdrhai_protocol_name_enum_type protocol_name,
  hdrmc_protocol_cmd_union_type * hdrmc_cmd_union_ptr
)
{
  switch ( protocol_name )
  {
    case HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL:
      hdralmp_process_cmd( &hdrmc_cmd_union_ptr->almp );
      break;

    case HDRHAI_INITIALIZATION_STATE_PROTOCOL:
      hdrinit_process_cmd( &hdrmc_cmd_union_ptr->init );
      break;

    case HDRHAI_IDLE_STATE_PROTOCOL:
      hdridle_process_cmd( &hdrmc_cmd_union_ptr->idle );
      break;

    case HDRHAI_CONNECTED_STATE_PROTOCOL:
      hdrcon_process_cmd( &hdrmc_cmd_union_ptr->connected );
      break;

    case HDRHAI_ROUTE_UPDATE_PROTOCOL:
      hdrrup_process_cmd( &hdrmc_cmd_union_ptr->route );
      break;

    case HDRHAI_OVERHEAD_MESSAGES_PROTOCOL:
      hdrovhd_process_cmd( &hdrmc_cmd_union_ptr->ovhd );
      break;

    case HDRHAI_SESSION_MANAGEMENT_PROTOCOL:
      hdrsmp_process_cmd( &hdrmc_cmd_union_ptr->smp );
      break;

    case HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL:
      hdramp_process_cmd( &hdrmc_cmd_union_ptr->amp );
      break;

    case HDRHAI_SESSION_CONFIGURATION_PROTOCOL:
      hdrscp_process_cmd( &hdrmc_cmd_union_ptr->scp );
      break;

    case HDRHAI_HDR_MESSAGING_PROTOCOL:
      hdrhmp_process_cmd( &hdrmc_cmd_union_ptr->hmp );
      break;

    case HDRHAI_LOCATION_UPDATE_PROTOCOL:
      hdrlup_process_cmd( &hdrmc_cmd_union_ptr->lup );
      break;

    case HDRHAI_STREAM_PROTOCOL:
      hdrstream_process_cmd( &hdrmc_cmd_union_ptr->stream );
      break;

    case HDRHAI_PACKET_APPLICATION_CONTROLLER:
      hdrpacc_process_cmd( &hdrmc_cmd_union_ptr->pac );
      break;

    case HDRHAI_FLOW_CONTROL_PROTOCOL:
      hdrfcp_process_cmd( &hdrmc_cmd_union_ptr->fcp );
      break;

#ifdef FEATURE_HDR_BCMCS
    case HDRHAI_BROADCAST_PROTOCOL:
      hdrbccp_process_cmd( &hdrmc_cmd_union_ptr->bcmcs);
      break;
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_DOS
    case HDRHAI_DATA_OVER_SIGNALING_PROTOCOL:
      hdrdos_process_cmd( &hdrmc_cmd_union_ptr->dos );
      break;
#endif /* FEATURE_HDR_DOS */

#ifdef FEATURE_HDR_QOS
    case HDRHAI_RESERVATION_PROTOCOL:
      hdrmrlpresv_process_cmd( &hdrmc_cmd_union_ptr->resv );
      break;
#endif /* FEATURE_HDR_QOS */

    case HDRHAI_LOGGING_MODULE:
      hdrlog_process_cmd( &hdrmc_cmd_union_ptr->log );
      break;

#ifdef FEATURE_HDR_CSNA
    case HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL:
      hdrcsna_process_cmd ( &hdrmc_cmd_union_ptr->csna );
      break;
#endif /* FEATURE_HDR_CSNA */

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRHAI_SIGNALING_ADAPTATION_PROTOCOL:
      hdrsap_process_cmd ( &hdrmc_cmd_union_ptr->sap );
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default:
      ERR( "CMD for unknown protocol %d", protocol_name, 0, 0 );
      break;
  }
} /* hdrmc_protocol_cmd() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_PROTOCOL_IND

DESCRIPTION
  All indications for protocols that run in the HDRMC task context are routed
  through here. This function dispatches the indication to the protocol it
  is for.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the indication is for
  hdrmc_ind_ptr - Received indication for a protocol in MC task to be routed

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrmc_protocol_ind
(
  hdrhai_protocol_name_enum_type protocol_name,
  hdrbuf_ind_type * hdrmc_ind_ptr
)
{
  switch ( protocol_name )
  {
    case HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL:
      hdralmp_process_ind( hdrmc_ind_ptr->ind_name,
             &hdrmc_ind_ptr->ind_data );
      break;

    case HDRHAI_INITIALIZATION_STATE_PROTOCOL:
      hdrinit_process_ind( hdrmc_ind_ptr->ind_name,
             &hdrmc_ind_ptr->ind_data );
      break;

    case HDRHAI_IDLE_STATE_PROTOCOL:
      hdridle_process_ind( hdrmc_ind_ptr->ind_name,
             &hdrmc_ind_ptr->ind_data );
      break;

    case HDRHAI_ROUTE_UPDATE_PROTOCOL:
      hdrrup_process_ind( hdrmc_ind_ptr->ind_name,
        &hdrmc_ind_ptr->ind_data );
      break;

    case HDRHAI_OVERHEAD_MESSAGES_PROTOCOL:
      hdrovhd_process_ind( hdrmc_ind_ptr->ind_name,
             &hdrmc_ind_ptr->ind_data );
      break;

    case HDRHAI_SESSION_MANAGEMENT_PROTOCOL:
      hdrsmp_process_ind( hdrmc_ind_ptr->ind_name,
                          &hdrmc_ind_ptr->ind_data );
      break;

    case HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL:
      hdramp_process_ind( hdrmc_ind_ptr->ind_name,
                          &hdrmc_ind_ptr->ind_data );
      break;

    case HDRHAI_SESSION_CONFIGURATION_PROTOCOL:
      hdrscp_process_ind( hdrmc_ind_ptr->ind_name,
                          &hdrmc_ind_ptr->ind_data );
      break;

    case HDRHAI_HDR_MESSAGING_PROTOCOL:
      hdrhmp_process_ind( hdrmc_ind_ptr->ind_name,
            &hdrmc_ind_ptr->ind_data );
      break;

    case HDRHAI_LOCATION_UPDATE_PROTOCOL:
      hdrlup_process_ind( hdrmc_ind_ptr->ind_name,
            &hdrmc_ind_ptr->ind_data );
      break;

    case HDRHAI_STREAM_PROTOCOL:
      hdrstream_process_ind( hdrmc_ind_ptr->ind_name,
            &hdrmc_ind_ptr->ind_data );
      break;

    case HDRHAI_KEY_EXCHANGE_PROTOCOL:
      hdrkep_process_ind( hdrmc_ind_ptr->ind_name,
            &hdrmc_ind_ptr->ind_data );
      break;

    case HDRHAI_AUTHENTICATION_PROTOCOL:
      hdrauth_process_ind( hdrmc_ind_ptr->ind_name,
            &hdrmc_ind_ptr->ind_data );
      break;

#ifdef FEATURE_HDR_BCMCS
    case HDRHAI_BROADCAST_PROTOCOL:
      hdrbccp_process_ind( hdrmc_ind_ptr->ind_name,
            &hdrmc_ind_ptr->ind_data );
      break;
#endif /* FEATURE_HDR_BCMCS */

    case HDRHAI_PACKET_APPLICATION_CONTROLLER:
      /* Do session configuration related processing of packet
       * application protocols in HDRMC task context
       */
      hdrpacc_process_ind( hdrmc_ind_ptr->ind_name,
            &hdrmc_ind_ptr->ind_data );
      break;

#ifdef FEATURE_HDR_QOS
    case HDRHAI_RESERVATION_PROTOCOL:
        hdrmrlpresv_process_ind( hdrmc_ind_ptr->ind_name, 
            &hdrmc_ind_ptr->ind_data );
      break;

#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRHAI_SIGNALING_ADAPTATION_PROTOCOL:
      hdrsap_process_ind ( hdrmc_ind_ptr->ind_name, 
            &hdrmc_ind_ptr->ind_data );
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default:
      ERR( "IND for unknown protocol %d", protocol_name, 0, 0 );
      break;
  }
} /* hdrmc_protocol_ind() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_PROTOCOL_MSG

DESCRIPTION
  All messages for protocols that run in the HDRMC task context are routed
  through here. This function dispatches the message to the protocol it
  is for.

DEPENDENCIES
  Each protocol is responsible for freeing the DSM item chain it is given.

PARAMETERS
  protocol_name - Who the message is for
  hdrmc_ind_ptr - Received message for a protocol in MC task to be routed

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrmc_protocol_msg
(
  hdrhai_protocol_name_enum_type protocol_name,
  hdrhai_protocol_instance_enum_type protocol_instance,
  dsm_item_type * item_ptr
)
{
  uint8 msg_id;
/*------------------------------------------------------------------------*/

  /* extract msg_id */
  if ( dsm_extract( item_ptr, 0, &msg_id, 1 ) == 0 )
  {
    ERR( "Could not extract message_id, ignoring the message", 0, 0, 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  /* To avoid sending Config messages in the same packet as
     sessionClose, drop all config messages unless session is
     open. Note: session maybe in AMP_SETUP or CLOSING at this point */
  if ( HDRHAI_MSG_IS_CONFIG(msg_id) &&
       ( !hdrsmp_session_is_open(HDRHAI_HDRMC) ) ) 
  {
    HDR_MSG_PROT_2 ( MSG_LEGACY_HIGH, 
                     "Drop config msg (P 0x%x, ID 0x%x); SMP not open",
                     (uint32) protocol_name, msg_id );
    dsm_free_packet( &item_ptr );
    return;
  }

  switch ( protocol_name )
  {
    case HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL:
      hdralmp_process_msg( item_ptr, protocol_instance );
      break;

    case HDRHAI_INITIALIZATION_STATE_PROTOCOL:
      hdrinit_process_msg( item_ptr, protocol_instance );
      break;

    case HDRHAI_IDLE_STATE_PROTOCOL:
      hdridle_process_msg( item_ptr, protocol_instance );
      break;

    case HDRHAI_CONNECTED_STATE_PROTOCOL:
      hdrcon_process_msg( item_ptr, protocol_instance );
      break;

    case HDRHAI_ROUTE_UPDATE_PROTOCOL:
      hdrrup_process_msg( item_ptr, protocol_instance );
      break;

    case HDRHAI_OVERHEAD_MESSAGES_PROTOCOL:
      hdrovhd_process_msg( item_ptr, protocol_instance );
      break;

    case HDRHAI_SESSION_MANAGEMENT_PROTOCOL:
      hdrsmp_process_msg( item_ptr, protocol_instance );
      break;

    case HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL:
      hdramp_process_msg( item_ptr, protocol_instance );
      break;

    case HDRHAI_SESSION_CONFIGURATION_PROTOCOL:
      hdrscp_process_msg( item_ptr, protocol_instance );
      break;

    case HDRHAI_LOCATION_UPDATE_PROTOCOL:
      hdrlup_process_msg( item_ptr, protocol_instance );
      break;

    case HDRHAI_STREAM_PROTOCOL:
      hdrstream_process_msg( item_ptr, protocol_instance );
      break;

    case HDRHAI_PACKET_APPLICATION_CONTROLLER:
     /* Do session configuration related processing of packet  
      * application protocols in HDRMC task context
      */
      hdrpacc_process_msg( item_ptr, protocol_instance);
      break;

    case HDRHAI_KEY_EXCHANGE_PROTOCOL:
      hdrkep_process_msg( item_ptr, protocol_instance );
      break;

    case HDRHAI_AUTHENTICATION_PROTOCOL:
      hdrauth_process_msg( item_ptr, protocol_instance );
      break;

#ifdef FEATURE_HDR_BCMCS
    case HDRHAI_BROADCAST_PROTOCOL:
      hdrbccp_process_msg(item_ptr, protocol_instance);
      break;
#endif

#ifdef FEATURE_HDR_DOS
    case HDRHAI_DATA_OVER_SIGNALING_PROTOCOL:
      hdrdos_process_msg( item_ptr, protocol_instance );
      break;
#endif /* FEATURE_HDR_DOS */

    case HDRHAI_SESSION_CONFIG_MANAGER:
      hdrscm_process_msg(item_ptr, protocol_instance);
      break;

    case HDRHAI_FLOW_CONTROL_PROTOCOL:
      hdrfcp_process_msg(item_ptr);
      break;

#ifdef FEATURE_HDR_QOS
    case HDRHAI_RESERVATION_PROTOCOL:
      hdrmrlpresv_process_msg( item_ptr );
      break;
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_CSNA
    case HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL:
      hdrcsna_process_msg(item_ptr, protocol_instance);
      break;
#endif /* FEATURE_HDR_CSNA */

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRHAI_SIGNALING_ADAPTATION_PROTOCOL:
      hdrsap_process_msg ( item_ptr, protocol_instance );
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default:
      ERR( "MSG for unknown protocol %d", protocol_name, 0, 0 );
      dsm_free_packet( &item_ptr );
      break;
  }
} /* hdrmc_protocol_msg() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_PROTOCOL_TIMER

DESCRIPTION
  All timer expiration events for protocols that run in the HDRMC task context are routed
  through here. This function dispatches the indication to the protocol it
  is for.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the indication is for
  hdrmc_ind_ptr - Received indication for a protocol in MC task to be routed

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrmc_protocol_timer
(
  hdrhai_protocol_name_enum_type protocol_name,
  uint32 timer_id
)
{
  HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "Timer expired pid0x%x t%d",
                  protocol_name,timer_id);
  switch ( protocol_name )
  {
    case HDRHAI_IDLE_STATE_PROTOCOL:
      hdridle_process_timer( timer_id );
      break;

    /* Eventually all of these protocols should add xxx_process_timer() */

    case HDRHAI_INITIALIZATION_STATE_PROTOCOL:
      hdrinit_process_timer( timer_id );
      break;

    case HDRHAI_ROUTE_UPDATE_PROTOCOL:
      hdrrup_process_timer( timer_id );
      break;

    case HDRHAI_OVERHEAD_MESSAGES_PROTOCOL:
      hdrovhd_process_timer( timer_id );
      break;

    case HDRHAI_SESSION_MANAGEMENT_PROTOCOL:
      break;

    case HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL:
      hdramp_process_timer( timer_id );
      break;

    case HDRHAI_SESSION_CONFIGURATION_PROTOCOL:
      break;

    case HDRHAI_STREAM_PROTOCOL:
      break;

    case HDRHAI_KEY_EXCHANGE_PROTOCOL:
      hdrkep_process_timer( timer_id );
      break;

#ifdef FEATURE_HDR_BCMCS
    case HDRHAI_BROADCAST_PROTOCOL:
      hdrbccp_process_timer( timer_id );
      break;
#endif /* FEATURE_HDR_BCMCS */

    case HDRHAI_PACKET_APPLICATION_CONTROLLER:
      hdrpacc_process_timer( timer_id );
      break;

#ifdef FEATURE_HDR_QOS
    case HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL:
      hdrmrlpc_process_timer( timer_id );
      break;
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_DOS
    case HDRHAI_DATA_OVER_SIGNALING_PROTOCOL:
      hdrdos_process_timer( timer_id );
      break;
#endif /* FEATURE_HDR_DOS */

    case HDRHAI_SESSION_CONFIG_MANAGER:
      hdrscm_process_timer( timer_id );
      break;

    case HDRHAI_HDR_MESSAGING_PROTOCOL:
      hdrhmp_process_timer( timer_id );
      break;

    case HDRHAI_FLOW_CONTROL_PROTOCOL:
      hdrfcp_process_timer( timer_id );
      break;

#ifdef FEATURE_HDR_CSNA
    case HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL:
      hdrcsna_process_timer( timer_id );
      break;
#endif /* FEATURE_HDR_CSNA */

    case HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL:
      hdralmp_process_timer( timer_id );
      break;

    case HDRHAI_LOGGING_MODULE:
      hdrlog_process_timer( timer_id );
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRHAI_SIGNALING_ADAPTATION_PROTOCOL:
      hdrsap_process_timer( timer_id );
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default:
      ERR( "TIMER for unknown protocol %d", protocol_name, 0, 0 );
      break;
  }
} /* hdrmc_protocol_timer() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_PROTOCOL_ITEM

DESCRIPTION
  This function routes the item to the protocol it is for, according to
  whether it is a command, indication, or message

DEPENDENCIES
  None

PARAMETERS
  hdrmc_buf_ptr - Buffer containing a command, indication, or message for
                  a protocol in the MC task

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrmc_protocol_item( hdrmc_buf_type * hdrmc_buf_ptr )
{
  switch ( hdrmc_buf_ptr->hdr.type_of_item )
  {
    case HDRBUF_COMMAND: /* The item is a command */
      hdrmc_protocol_cmd( hdrmc_buf_ptr->hdr.protocol_name,
                          &hdrmc_buf_ptr->item.cmd );
      break;

    case HDRBUF_INDICATION: /* The item is an indication */
      hdrmc_protocol_ind( hdrmc_buf_ptr->hdr.protocol_name,
                          &hdrmc_buf_ptr->item.ind );
      break;

    case HDRBUF_MESSAGE: /* The item is a message (DSM item */
      hdrmc_protocol_msg( hdrmc_buf_ptr->hdr.protocol_name,
                          hdrmc_buf_ptr->hdr.protocol_instance,
                          hdrmc_buf_ptr->item.item_ptr );
      break;

    case HDRBUF_TIMER: /* The item is a timer expiration event */
      hdrmc_protocol_timer( hdrmc_buf_ptr->hdr.protocol_name,
                            hdrmc_buf_ptr->item.timer_id );
    break;

    default:
      break;
  }
} /* hdrmc_protocol_item() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMC_CM_MC_RPT

DESCRIPTION
  This function sends a report to CM.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - The pointer to the command to be sent to CM.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void hdrmc_cm_mc_rpt( cm_mc_rpt_type * cmd_ptr )
{
  cmd_ptr->hdr.task_ptr = NULL;

  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "CM MC rpt %d", cmd_ptr->hdr.cmd);
  cm_mc_rpt( cmd_ptr );
} /* hdralmp_cm_mc_rpt() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMC_SEND_RELEASE

DESCRIPTION
  This function sends a release report to the CM task.

DEPENDENCIES
  None.

PARAMETERS
  err_code - The reason for the release.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrmc_send_release( cm_rel_reason_e_type rel_code )
{
  cm_mc_rpt_type *cmd_ptr;         /* Pointer to handset command */
  int i = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (hdrcallid_get_num_active_call_ids() > 0)
  {
    if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
    {
      cmd_ptr->release.hdr.cmd = CM_HDR_CALL_RELEASE_F;
      cmd_ptr->release.num_call_ids = 
        hdrcallid_get_all_call_ids ( cmd_ptr->release.call_ids );

      for(i = 0; i < cmd_ptr->release.num_call_ids; i++) 
      {
        cmd_ptr->release.rel_codes[i] = rel_code;
      }
  
      HDR_MSG_PROT_2 (MSG_LEGACY_HIGH,"Sent release rel_code=%d for %d callids",
              rel_code, cmd_ptr->release.num_call_ids);
      hdrmc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
  
    hdrcallid_deallocate_all_call_ids();
  }

} /* hdrmc_send_release() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRMC_SEND_ORIG_FAILURE

DESCRIPTION
  This function sends a orig fail report to the CM task.

DEPENDENCIES
  None.

PARAMETERS
  err_code - The reason for the origination failure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_send_orig_failure( cm_call_orig_fail_e_type err_code )
{
  cm_mc_rpt_type *cmd_ptr;         /* Pointer to handset command */
  cm_call_id_type call_id = hdrcallid_get_cur_call_id(); /* get the current call id */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(call_id != CM_CALL_ID_INVALID)
  {
    /* Send CM report for current call ids */

    /* Start reorder tone */
    if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
    {
      cmd_ptr->orig_fail.hdr.cmd = CM_HDR_CALL_ORIG_FAIL_F;
      cmd_ptr->orig_fail.code = err_code;
      cmd_ptr->orig_fail.call_id = call_id;
      hdrmc_cm_mc_rpt( cmd_ptr );
      HDR_MSG_PROT_2 (MSG_LEGACY_HIGH,  "Sent ORIG_FAIL code=%d, CallId=%d",
                      err_code, cmd_ptr->orig_fail.call_id);
    }
    else
    {
      ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "De-allocating call id =%d due to call orig failure in hdrmc", call_id);
    hdrcallid_deallocate_call_id(call_id);
  }
  else
  {
       ERR_FATAL("Current Call ID cannot be CM_CALL_ID_INVALID",0,0,0);
  }

} /* hdrmc_send_orig_failure() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMC_SEND_END_CNF

DESCRIPTION
  This function sends an end confirmation report to the CM task.

DEPENDENCIES
  None.

PARAMETERS
  call_id - The call ID of the call that was ended.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_send_end_cnf
(
  cm_call_id_type call_id
)
{
  cm_mc_rpt_type *cmd_ptr;         /* Pointer to handset command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start reorder tone */
  if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
  {
    cmd_ptr->call_end_cnf.hdr.cmd = CM_HDR_CALL_END_CNF_F;
    cmd_ptr->call_end_cnf.num_call_ids = 1;
    cmd_ptr->call_end_cnf.call_ids[0] = call_id;
    hdrmc_cm_mc_rpt( cmd_ptr );
    HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Sent END_CNF CallId=%d", call_id );
  }
  else
  {
    ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }
} /* hdrmc_send_end_cnf() */

/* <EJECT> */

#ifdef FEATURE_IRAT_PILOT_MEAS
/*===========================================================================

FUNCTION HDRMC_SEND_MEAS_MODE_CNF

DESCRIPTION
  This function sends confirmation about measurement mode ON/OFF to the CM task.

DEPENDENCIES
  None.

PARAMETERS
  meas_mode_state - The measurement mode state.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrmc_send_meas_mode_cnf( sys_meas_mode_e_type meas_mode_state )
{
  cm_mc_rpt_type *cmd_ptr;         /* Pointer to handset command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
  {
    cmd_ptr->meas_mode_status.hdr.cmd = CM_HDR_MEAS_MODE_F;
    cmd_ptr->meas_mode_status.hdr.status = CM_DONE_S;
    cmd_ptr->meas_mode_status.meas_mode = meas_mode_state;
    hdrmc_cm_mc_rpt( cmd_ptr );
  }
  else
  {
    ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }

} /* hdrmc_send_meas_mode_cnf() */

/* <EJECT> */
#endif /* FEATURE_IRAT_PILOT_MEAS */


/*===========================================================================
FUNCTION  HDRMC_STATE_PROC_PROT_ACTIVATE

DESCRIPTION
  This function processes protocol activate command

DEPENDENCIES
  None.

RETURN VALUE
  next state of HDRMC task.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL hdrmc_state_type hdrmc_inact_proc_prot_activate
(
  prot_act_e_type                  act_reason
    /* Reason for activating the CDMA protocol. */
)
{
  hdrmc_state_type     next_state; /* Next state of HDRMC Mode Controller  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize next state of HDRMC */
  next_state = HDRMC_INACTIVE_STATE;

  /* Activate CDMA protocol based on act_reason.
  */
  switch ( act_reason )
  {
    case PROT_ACT_ONLINE_ACQ:
      next_state = HDRMC_ONLINE_STATE;
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case PROT_ACT_LTE_HANDOVER:
      if ( hdrscp_is_irat_enabled() )
      {
        if ( hdrmc_lte_to_hdr_resel_is_in_progress() ||
             hdrmc_lte_to_hdr_redir_is_in_progress() ||
             hdrmc_lte_to_hdr_get_cgi_is_in_progress() ||
             hdrmc_hdr_to_lte_time_transfer_is_in_progress() )
        {
          /* There is possibility of receiving tunnel activation command
           * while L2D is in progess. */
          hdrmc_set_tunnel_activation_is_pending(TRUE);
        }
        else
        {
          next_state = HDRMC_ONLINE_STATE;
          hdrsap_set_tunnel_mode( HDRHAI_TUNNEL_MODE_ENABLED );
        }
      }
      else
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "IRAT subtypes are not supported" );
      }
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    case PROT_ACT_OFFLINE:
      /* do nothing */
      break;

    default:
      ERR_FATAL("UnKnownActReason=%d", act_reason, 0, 0 );
      break;

  } /* switch ( act_reason ) */

  return next_state;

} /* hdrmc_inact_prot_activate() */


#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================

FUNCTION HDRMC_PROCESS_LTE_TO_HDR_REDIRECT_CMD

DESCRIPTION
  This function processes a command from LTE to redirect to HDR.

DEPENDENCIES
  None

PARAMETERS
  cmd_ptr - A pointer to the command received from LTE RRC.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_process_lte_to_hdr_redirect_cmd( hdrmc_msg_type * cmd_ptr )
{
#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() )
  {
    if ( hdrmc.lte_to_hdr_ho_info.irat_ho_type 
           == HDRMC_IRAT_LTE_TO_HDR_ACTIVE_HO )
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                    "RX redir in middle of Active Handoff, ignore redir" );
      return;
    }
    /* This means we are waiting for TCA for active handoff.
     * Abort active handoff and starts redirection */
    hdridle_process_tunnel_conn_timeout();
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* Inform MMOC that LTE to HDR redirection is initiated */
  mmoc_cmd_prot_redir_ind (PROT_ACT_GWL_REDIR, PROT_STATE_ONLINE_HDR);

  /* Store the redirection information so that it can be used later. */
  memset( &hdrmc.lte_to_hdr_ho_info, 0, 
          sizeof(hdrmc_lte_to_hdr_ho_info_type) );

  hdrmc.lte_to_hdr_ho_info.irat_ho_type = HDRMC_IRAT_LTE_TO_HDR_REDIRECT; 

  hdrmc.lte_to_hdr_ho_info.band_count = 
      cmd_ptr->msgr_cmd.msg.lte_redirect.band_count;

#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( &hdrmc.lte_to_hdr_ho_info.freq_per_band_list, 
           sizeof(hdrmc.lte_to_hdr_ho_info.freq_per_band_list), 
           &cmd_ptr->msgr_cmd.msg.lte_redirect.freq_per_band_list,
           sizeof(hdrcp_redir_freq_per_band_list) * HDR_CP_MAX_CDMA2000_BANDS );
#else
  memcpy( &hdrmc.lte_to_hdr_ho_info.freq_per_band_list, 
          &cmd_ptr->msgr_cmd.msg.lte_redirect.freq_per_band_list,
          sizeof( hdrcp_redir_freq_per_band_list ) * HDR_CP_MAX_CDMA2000_BANDS );
#endif /* FEATURE_MEMCPY_REMOVAL */

} /* hdrmc_process_lte_to_hdr_redirect_cmd */


/*===========================================================================

FUNCTION HDRMC_PROCESS_LTE_TO_HDR_RESELECT_CMD

DESCRIPTION
  This function processes a command from LTE to reselect to HDR.

DEPENDENCIES
  None

PARAMETERS
  cmd_ptr - A pointer to the command received from LTE RRC.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_process_lte_to_hdr_reselect_cmd( hdrmc_msg_type * cmd_ptr )
{
#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() )
  {
    if ( hdrmc.lte_to_hdr_ho_info.irat_ho_type 
           == HDRMC_IRAT_LTE_TO_HDR_ACTIVE_HO )
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                    "RX reselect in middle of Active Handoff, ignore reselect" );
      return;
    }
    /* This means we are waiting for TCA for active handoff.
     * Abort active handoff and starts reselection */
    hdridle_process_tunnel_conn_timeout();
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* Inform MMOC that LTE to HDR reselection is initiated */
  mmoc_cmd_prot_redir_ind (PROT_ACT_GWL_REDIR, PROT_STATE_ONLINE_HDR);

  /* Store the reselection information so that it can be used later. */
  memset( &hdrmc.lte_to_hdr_ho_info, 0, 
          sizeof(hdrmc_lte_to_hdr_ho_info_type) );

  hdrmc.lte_to_hdr_ho_info.irat_ho_type = HDRMC_IRAT_LTE_TO_HDR_RESELECT; 

#ifdef FEATURE_MEMCPY_REMOVAL
  hdrmc.lte_to_hdr_ho_info.sys_info = 
       cmd_ptr->msgr_cmd.msg.lte_reselect.resel_info;
#else
  memcpy( &hdrmc.lte_to_hdr_ho_info.sys_info, 
          &cmd_ptr->msgr_cmd.msg.lte_reselect.resel_info,
          sizeof( sys_channel_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  hdrmc.lte_to_hdr_ho_info.pn_list_size = 1;

  hdrmc.lte_to_hdr_ho_info.pilot_pn[0] = 
      cmd_ptr->msgr_cmd.msg.lte_reselect.pilot_pn;

  hdrmc.lte_to_hdr_ho_info.cdma_sys_time = 
      cmd_ptr->msgr_cmd.msg.lte_reselect.cdma_sys_time;

  hdrmc.lte_to_hdr_ho_info.lte_ostmr = 
      cmd_ptr->msgr_cmd.msg.lte_reselect.lte_ostmr;
}

/*===========================================================================

FUNCTION HDRMC_PROCESS_LTE_GET_CGI_CMD

DESCRIPTION
  This function processes a command from LTE to get HDR CGI (sector ID).

DEPENDENCIES
  None

PARAMETERS
  cmd_ptr - A pointer to the command received from LTE RRC.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_process_lte_get_cgi_cmd( hdrmc_msg_type * cmd_ptr )
{
  /* Inform MMOC that LTE to HDR CGI request is initiated */
  mmoc_cmd_prot_redir_ind (PROT_ACT_GWL_REDIR, PROT_STATE_ONLINE_HDR);

  /* Store the get CGI information so that it can be used later. */
  memset( &hdrmc.lte_to_hdr_ho_info, 0, 
          sizeof(hdrmc_lte_to_hdr_ho_info_type) );

  hdrmc.lte_to_hdr_ho_info.irat_ho_type = HDRMC_IRAT_LTE_TO_HDR_GET_CGI; 

#ifdef FEATURE_MEMCPY_REMOVAL
  hdrmc.lte_to_hdr_ho_info.sys_info = 
      cmd_ptr->msgr_cmd.msg.lte_get_cgi_req.sys_info;
#else
  memcpy( &hdrmc.lte_to_hdr_ho_info.sys_info, 
          &cmd_ptr->msgr_cmd.msg.lte_get_cgi_req.sys_info,
          sizeof( sys_channel_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  hdrmc.lte_to_hdr_ho_info.pn_list_size = 1;

  hdrmc.lte_to_hdr_ho_info.pilot_pn[0] = 
      cmd_ptr->msgr_cmd.msg.lte_get_cgi_req.pilot_pn;

  hdrmc.lte_to_hdr_ho_info.cdma_sys_time = 
      cmd_ptr->msgr_cmd.msg.lte_get_cgi_req.cdma_sys_time;

  hdrmc.lte_to_hdr_ho_info.lte_ostmr = 
      cmd_ptr->msgr_cmd.msg.lte_get_cgi_req.lte_ostmr;

  hdrmc.lte_to_hdr_ho_info.time_allowed_ms = 
      cmd_ptr->msgr_cmd.msg.lte_get_cgi_req.time_available_for_cgi; 

  hdrmc.lte_to_hdr_ho_info.search_window_size = 
      cmd_ptr->msgr_cmd.msg.lte_get_cgi_req.search_window_size; 
  
}

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRMC_PROCESS_LTE_TO_HDR_ACTIVE_HO_CMD

DESCRIPTION
  This function processes command from LTE to perform active HO to HDR.

DEPENDENCIES
  None

PARAMETERS
  cmd_ptr - A pointer to the command received from LTE RRC.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_process_lte_to_hdr_active_ho_cmd( hdrmc_msg_type * cmd_ptr )
{

  /* Store the reselection information so that it can be used later. */
  memset( &hdrmc.lte_to_hdr_ho_info, 0, 
          sizeof(hdrmc_lte_to_hdr_ho_info_type) );

  hdrmc.lte_to_hdr_ho_info.irat_ho_type = HDRMC_IRAT_LTE_TO_HDR_ACTIVE_HO; 

  if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
  {
    hdridle_stop_tunnel_conn_timer();

    /* Inform MMOC that LTE to HDR reselection is initiated */
    mmoc_cmd_prot_redir_ind (PROT_ACT_GWL_REDIR, PROT_STATE_ONLINE_HDR);

#ifdef FEATURE_MEMCPY_REMOVAL
    hdrmc.lte_to_hdr_ho_info.active_ho_info.msg_payload.msg_hdr = 
          cmd_ptr->msgr_cmd.msg.lte_active_ho.hdr;

    hdrmc.lte_to_hdr_ho_info.active_ho_info.msg_payload.dsm_attach = 
          cmd_ptr->msgr_cmd.msg.lte_active_ho.dsm_attach;
#else
    memcpy (&hdrmc.lte_to_hdr_ho_info.active_ho_info.msg_payload.msg_hdr,
            &cmd_ptr->msgr_cmd.msg.lte_active_ho.hdr,
            sizeof(msgr_hdr_struct_type));

    memcpy (&hdrmc.lte_to_hdr_ho_info.active_ho_info.msg_payload.dsm_attach,
            &cmd_ptr->msgr_cmd.msg.lte_active_ho.dsm_attach,
            sizeof(msgr_attach_s));
#endif /* FEATURE_MEMCPY_REMOVAL */

    hdrmc.lte_to_hdr_ho_info.active_ho_info.free_dsm_item = TRUE;

    hdrmc.lte_to_hdr_ho_info.cdma_sys_time = 
        cmd_ptr->msgr_cmd.msg.lte_active_ho.cdma_sys_time;

    hdrmc.lte_to_hdr_ho_info.lte_ostmr = 
        cmd_ptr->msgr_cmd.msg.lte_active_ho.lte_ostmr;

    hdrmc.lte_to_hdr_ho_info.active_ho_info.msg_payload.dl_msg_info.dl_msg_type =
        LTE_IRAT_TUNNEL_DL_MSG_DL_INFO_TRANSFER;
  }
  else
  {
    HDR_MSG_PROT (MSG_LEGACY_ERROR, 
        "Mobility from EUTRA command recvd when not in active handoff state");
    hdrmc_send_irat_ho_failure_ind_to_lte(FALSE);
  }
}

/*===========================================================================
FUNCTION HDRMC_GET_ACTIVE_HANDOFF_PAYLOAD

DESCRIPTION
  This function returns the payload (HSP, OLP and TCA) received from LTE for 
  handoff.

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
hdrmc_lte_to_hdr_active_ho_info_type* hdrmc_get_active_handoff_payload( void )
{
  return &hdrmc.lte_to_hdr_ho_info.active_ho_info;
}
#endif /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================

FUNCTION HDRMC_PROCESS_LTE_TO_HDR_HO_CNF_CMD

DESCRIPTION
  This function processes a command from MMOC to whether continue with 
  redirect/reselect to HDR.

DEPENDENCIES
  None

PARAMETERS
  cmd_ptr - A pointer to the command received from MMOC.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

hdrmc_state_type hdrmc_process_lte_to_hdr_ho_cnf_cmd
( 
  hdrmc_msg_type * cmd_ptr,
  hdrmc_state_type curr_state
)
{
  hdrmc_state_type next_state = curr_state; 
    /* Next state of HDRMC Mode Controller  */
  sd_ss_act_e_type sd_action;  
    /* Action returned from SD */
#ifdef FEATURE_LTE_TO_HDR_OH
  rex_tcb_type *dec_tcb_ptr; 
#endif /* FEATURE_LTE_TO_HDR_OH */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_NONE )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "IRAT: no IRAT in progress. Ignore HO Conf." );
    return next_state;
  }

  if (cmd_ptr->msgr_cmd.msg.redir_protocol_cnf.is_irat_ho_allowed == TRUE)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
         "IRAT: Continuing with LTE-HDR redirect/reselect/time-transfer cmd");

    if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_REDIRECT)
    {
      /* Give the redirected channel and band information received from LTE
         to SD. SD will tell INIT protocol the channel and band info to 
         acquire when INIT queries later */
      sd_action = sd_ss_ind_hdr_user_redir_lte( hdrmc_extract_lte_to_hdr_ho_chan, 
                                   &hdrmc.lte_to_hdr_ho_info, 
                                   SD_SS_REDIR_HDR_LTE,
                                   NULL );
    }
#ifdef FEATURE_LTE_TO_HDR_OH
    else if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_ACTIVE_HO)
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH, 
           "IRAT: ActiveHO LTE->HDR.. not calling sd_ss_ind_hdr_user_redir_lte");
      sd_action = SD_SS_ACT_ACQ_HDR;
    }
    else if ( (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_GET_CGI) )
    {
      if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_HDRMC ) &&
           hdrovhd_prereg_is_allowed( HDRHAI_HDRMC ) &&
           ( (hdrsaps_get_state() != HDRSAPS_CLOSE) ||
             hdramp_is_waiting_for_uati_assgn() ) )
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                      "IRAT: rx'ed CGI cmd when waiting UATIAssign or has virtual connection, skip CGI" );
  
        /* As long as the SD action is not SD_SS_ACT_ACQ_HDR,
         * HDR reports CGI failure. */
        sd_action = SD_SS_ACT_CONTINUE;
      }
      else
      {
        /* Give the reselected channel and band information received from LTE
           to SD. SD will tell INIT protocol the channel and band info to 
           acquire when ALMP queries later */
        sd_action = sd_ss_ind_hdr_user_redir_lte( hdrmc_extract_lte_to_hdr_ho_chan, 
                                      &hdrmc.lte_to_hdr_ho_info, 
                                      SD_SS_REDIR_HDR_LTE_RESEL,
                                      NULL );
        hdralmp_start_get_cgi_timer();
      }
    }
#endif /* FEATURE_LTE_TO_HDR_OH */
    else 
    /* HDRMC_IRAT_LTE_TO_HDR_RESELECT or HDRMC_IRAT_HDR_TO_LTE_TIME_TRANSFER 
       or HDRMC_IRAT_LTE_TO_HDR_GET_CGI */
    {
      /* Give the reselected channel and band information received from LTE
         to SD. SD will tell INIT protocol the channel and band info to 
         acquire when ALMP queries later */
      sd_action = sd_ss_ind_hdr_user_redir_lte( hdrmc_extract_lte_to_hdr_ho_chan, 
                                    &hdrmc.lte_to_hdr_ho_info, 
                                    SD_SS_REDIR_HDR_LTE_RESEL,
                                    NULL );
    }

    /* Activate the HDR protocol */
    if( sd_action == SD_SS_ACT_ACQ_HDR )
    {
      if ( curr_state == HDRMC_INACTIVE_STATE )
      {
        next_state = hdrmc_inact_proc_prot_activate( PROT_ACT_ONLINE_ACQ );
      }
#ifdef FEATURE_LTE_TO_HDR_OH
      else
      {
        /* Close virtual connection first. */
        hdrsap_idle_handoff_ready();

        /* HDR decoder task is not activated in tunnel mode.
         * Set signal to HDR decoder task to enter HDR online mode. */
        dec_tcb_ptr = rcinit_lookup_rextask("hdrdec");
        ( void ) rex_set_sigs( dec_tcb_ptr, HDRDEC_ENTER_HDR_SIG );

        hdrinit_continue_acquisition( HDRHAI_HDRMC );
      }
#endif /* FEATURE_LTE_TO_HDR_OH */
    }
    else
    {
      mmoc_rpt_prot_auto_deactd_ind( PROT_DEACT_ACQ_OTHERS,
                                     PROT_STATE_ONLINE_HDR );
#ifdef FEATURE_HDR_DEBUG_INFO
      hdrmc_log_sent_rpt(0, HDR_MMOC_RPT_PROT_AUTO_DEACTD_IND);
#endif /* FEATURE_HDR_DEBUG_INFO */
      if ( hdrmc.tunnel_activation_is_pending )
      {
        /* Send failure indication to LTE */
        hdrmc_send_irat_ho_failure_ind_to_lte(TRUE);
      }
      else
      {
        hdrmc_send_irat_ho_failure_ind_to_lte(FALSE);
      }
    }
  }
  else
  {
    if ( hdrmc.tunnel_activation_is_pending )
    {
      /* Send failure indication to LTE */
      hdrmc_send_irat_ho_failure_ind_to_lte(TRUE);
    }
    else
    {
      hdrmc_send_irat_ho_failure_ind_to_lte(FALSE);
    }
  }

  return next_state;
}


/*===========================================================================

FUNCTION HDRMC_PROCESS_REVERSE_TIME_TRANSFER_REQ_CMD

DESCRIPTION
  This function processes a command from LTE RRC to initiate reverse time 
  transfer.

DEPENDENCIES
  None

PARAMETERS
  cmd_ptr - A pointer to the command received from LTE.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_process_reverse_time_transfer_req_cmd( hdrmc_msg_type * cmd_ptr )
{
  /* Inform MMOC that LTE to HDR reselection is initiated */
  mmoc_cmd_prot_redir_ind (PROT_ACT_GWL_REDIR, PROT_STATE_ONLINE_HDR);

  /* Store the time transfer request information so that it can be 
     used later. */
  memset( &hdrmc.lte_to_hdr_ho_info, 0, 
          sizeof(hdrmc_lte_to_hdr_ho_info_type) );

  hdrmc.lte_to_hdr_ho_info.irat_ho_type = HDRMC_IRAT_HDR_TO_LTE_TIME_TRANSFER; 

#ifdef FEATURE_MEMCPY_REMOVAL
  hdrmc.lte_to_hdr_ho_info.sys_info = 
      cmd_ptr->msgr_cmd.msg.lte_rev_tt.sys_info;
#else
  memcpy( &hdrmc.lte_to_hdr_ho_info.sys_info, 
          &cmd_ptr->msgr_cmd.msg.lte_rev_tt.sys_info,
          sizeof( sys_channel_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  hdrmc.lte_to_hdr_ho_info.pn_list_size = 
      cmd_ptr->msgr_cmd.msg.lte_rev_tt.pn_list_size; 

#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( hdrmc.lte_to_hdr_ho_info.pilot_pn, 
          (sizeof( sys_pilot_pn_type ) * HDR_CP_MAX_PN_LIST_SIZE), 
           cmd_ptr->msgr_cmd.msg.lte_rev_tt.pilot_pn,
          (sizeof( sys_pilot_pn_type ) * HDR_CP_MAX_PN_LIST_SIZE) );
#else
  memcpy( hdrmc.lte_to_hdr_ho_info.pilot_pn, 
          cmd_ptr->msgr_cmd.msg.lte_rev_tt.pilot_pn,
          (sizeof( sys_pilot_pn_type ) * HDR_CP_MAX_PN_LIST_SIZE) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  hdrmc.lte_to_hdr_ho_info.time_allowed_ms = 
      cmd_ptr->msgr_cmd.msg.lte_rev_tt.time_allowed_ms; 

}
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION hdrmc_process_modem_config_refresh

DESCRIPTION
  This function processes modem config refresh command.

DEPENDENCIES
  None

PARAMETERS
  ref_id - reference id for refresh cmd

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_process_modem_config_refresh
( 
  uint32 ref_id
)
{

  /* We close session as SIM is changed. */
  if ( hdrsmp_session_is_open(HDRHAI_HDRMC) )
  {    
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "reload NV, close session");
    hdrind_give_ind(HDRIND_AMP_FAILED_NEW_ESN, NULL);
  }

  /* The order is the same as powerup initilization */
  hdrcallid_deallocate_all_call_ids();
  hdrmc_init_features();

  /* No need to call hdrutil_powerup_init() */

#ifdef FEATURE_MEASURE_UPTIME_IN_NV
  if( hdrutil_read_nv_item( NV_HOLD_HDR_TIME_I,
     (nv_item_type *) &(hdrmc.uptime_counter_min)) != NV_DONE_S)
  {
    /* Start the timer from zero if it has never been written before */
    hdrmc.uptime_counter_min = 0;
  }
#endif /* FEATURE_MEASURE_UPTIME_IN_NV */

  hdrplog_powerup_init();
  /* No need to call hdrlog_powerup_init() */
  hdrsmp_powerup_init();
  hdrscm_powerup_init();
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrsap_powerup_init();
#endif /* FEATURE_LTE_TO_HDR_OH */
  hdrhmp_powerup_init();
  hdrlmac_powerup_init();
  hdrcmac_powerup_init();
  hdramac_powerup_init();
  hdrfmac_powerup_init();
  hdrrmac_powerup_init();
  hdrinit_powerup_init();
  hdridle_powerup_init();
  hdrcon_powerup_init();
  hdrovhd_powerup_init();
  hdrrup_powerup_init();
  hdrstream_powerup_init();
  hdrpacc_powerup_init();
  /* No need to call hdrpac_mdm_powerup_init() */
  hdrlup_powerup_init();
#ifdef FEATURE_HDR_DOS
  hdrdos_powerup_init();
#endif /*  FEATURE_HDR_DOS */
  hdrkep_powerup_init();
  hdrauth_powerup_init();
  hdrfcp_powerup_init();
  /* No need to call hdrmrlpresv_powerup_init() */
  /* No need to call hdrbccp_powerup_init() */
#ifdef FEATURE_HDR_CSNA
  hdrcsna_powerup_init();
#endif /* FEATURE_HDR_CSNA */
  /* No need to call hdralmp_powerup_init() */
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrsaptx_powerup_init();
#endif /* FEATURE_LTE_TO_HDR_OH*/

  hdrsrch_read_nv();
  hitapp_refresh_nv();

  /* Notify MCFG that reloading NV is done. */
  mcfg_refresh_done_w_status(ref_id, MCFG_REFRESH_SUCCESS);
  HDR_MSG_PROT_1 ( MSG_LEGACY_HIGH, "MCFG Refresh done for ref_id %d ",ref_id);
  hdrmc.mcfg_refresh_is_in_progress = FALSE;

} /* hdrmc_process_modem_config_refresh() */

#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================

FUNCTION hdrmc_online_process_cmd

DESCRIPTION
  This function processes a command from CM.

DEPENDENCIES
  None

PARAMETERS
  cmd_ptr - A pointer to the command received from CM

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_online_process_cmd( hdrmc_msg_type * cmd_ptr )
{

  hdrhai_abort_reason_enum_type abort_reason = HDRHAI_ABORT_UNSPECIFIED;
    /* reason for deactivating or reacquiring HDR */
  sd_ss_act_e_type sd_action;                   /* action returned from SD */
  boolean is_reselect_allowed = TRUE;     /* is system reselection allowed */
  boolean end_cmd_will_close_con = TRUE;  /* Will we send ConnectionClose? */
  int num_active_call_ids;   /* Number of call IDs allocated for HDR by CM */
  ds707_extif_system_info curr_sys_info = DS707_EXTIF_SYSTEM_NON_EPC;
  hdrind_ind_data_union_type ind_data;
   /* Current system information */
#ifdef FEATURE_LTE_TO_HDR_OH
  rex_tcb_type *dec_tcb_ptr; 
#endif /* FEATURE_LTE_TO_HDR_OH */
#ifdef FEATURE_HDR_TO_LTE
  trm_get_info_input_type        input_info;    /* Input to get trm info */
  trm_get_info_return_type       output_info;   /* Output from get trm info */
#endif /* FEATURE_HDR_TO_LTE */
#ifdef FEATURE_HDR_DEBUG_INFO
  prot_trans_type trans_id=0;
#endif /*  FEATURE_HDR_DEBUG_INFO */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr->hdr.status = HDRMC_DONE_S; /* Default, unless bad */
  
#ifdef FEATURE_HDR_DEBUG_INFO
  HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "Rxed command %d",cmd_ptr->hdr.cmd);

  if(hdrmc_get_transaction_id(&trans_id,cmd_ptr) != TRUE)
  {
	hdrmc_log_rcvd_rpt(trans_id,cmd_ptr->hdr.cmd );
  }
  else
  {
  	HDR_MSG_PROT (MSG_LEGACY_LOW,  "Command ignored");
  }  
#endif /*  FEATURE_HDR_DEBUG_INFO */
  switch ( cmd_ptr->hdr.cmd )
  {
    /*-------------------------------------------------------------------------
    Activate Command
    -------------------------------------------------------------------------*/
     case HDRMC_PROT_ACT_REQ:

      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                      "Online rcv ProtAct reason %d",
                      cmd_ptr->msgr_cmd.msg.activate_protocol.act_reason );

#ifdef FEATURE_LTE_TO_HDR_OH
      if ( cmd_ptr->msgr_cmd.msg.activate_protocol.act_reason == 
           PROT_ACT_LTE_HANDOVER )
      {
        if ( hdrmc_lte_to_hdr_resel_is_in_progress() ||
             hdrmc_lte_to_hdr_redir_is_in_progress() ||
             hdrmc_lte_to_hdr_get_cgi_is_in_progress() ||
             hdrmc_hdr_to_lte_time_transfer_is_in_progress() )
        {
          /* There is possibility of receiving tunnel activation command
           * while L2D is in progess. */
          hdrmc_set_tunnel_activation_is_pending(TRUE);
        }
        else
        {
          /* Enter tunnel again. Tunnel mode will be HDRSAP_TUNNEL_MODE_ENABLED
           * after SAP is activated. */
          hdrsap_activate( HDRHAI_HDRMC );
        }
      }
      else if ( ( cmd_ptr->msgr_cmd.msg.activate_protocol.act_reason == 
                  PROT_ACT_ONLINE_ACQ ) &&
                hdrsap_tunnel_mode_is_enabled( HDRHAI_HDRMC ) )
      {
        /* If activate command is received when tunnel mode is enabled first 
         * deactivate SAP, which will move the protocol out of tunnel mode to 
         * inactive state. Then activate ALMP to enter operation over HRPD 
         * air interface. */
        hdrsap_deactivate( HDRHAI_HDRMC, HDRSAP_DEACT_ACQ_HDR );

        /* HDR decoder task is not activated in tunnel mode.
         * Set signal to HDR decoder task to enter HDR online mode. */
        dec_tcb_ptr = rcinit_lookup_rextask("hdrdec");
        ( void ) rex_set_sigs( dec_tcb_ptr, HDRDEC_ENTER_HDR_SIG );

        /* If there is no session in the tunnel, when moving out of tunnel,
         * need to change back the default subtypes. */
        if ( !hdrstream_stream_is_negotiated( HDRHAI_APP_PACKET_APP_ANY ) )
        {
          hdrscp_update_subtype_defaults(); 
        }

        hdralmp_activate( HDRHAI_HDRMC );
      }
      else
#endif /* FEATURE_LTE_TO_HDR_OH */
      {
        /* Activate the HDR protocol- but HDR is already activated
        */
        ERR("Ignore ProcAct reason=%d (st=online)",
        cmd_ptr->msgr_cmd.msg.activate_protocol.act_reason,0,0);
      }
      break;

    /*-------------------------------------------------------------------------
    Deactivate Command
    -------------------------------------------------------------------------*/
    case HDRMC_PROT_DEACT_REQ:

      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Online rcv ProtDeact reason=%d",
                      cmd_ptr->msgr_cmd.msg.deactivate_protocol.deact_reason);

#ifdef FEATURE_LTE_TO_HDR_OH
      /* Don't have to move resume tunnel activation command if
       * deactivation is received. */
      if ( hdrmc.tunnel_activation_is_pending )
      {
        hdrmc_set_tunnel_activation_is_pending(FALSE);
      }
#endif /* FEATURE_LTE_TO_HDR_OH */

      switch( cmd_ptr->msgr_cmd.msg.deactivate_protocol.deact_reason)
      {

        case PROT_DEACT_OFFLINE:
          abort_reason = HDRHAI_ABORT_OFFLINE;
          break;

        case PROT_DEACT_ACQ_OTHERS:
          abort_reason = HDRHAI_ABORT_ACQ_OTHERS;
          break;

        case PROT_DEACT_SUBSC_CHGD:
          abort_reason = HDRHAI_ABORT_NAM_CHANGE;
          break;

        case PROT_DEACT_PWR_DOWN:
          abort_reason = HDRHAI_ABORT_POWERDOWN;
          break;

        case PROT_DEACT_LPM:
          abort_reason = HDRHAI_ABORT_LPM;
          break;

        case PROT_DEACT_RESET:
          abort_reason = HDRHAI_ABORT_POWERDOWN;
          break;

#if defined(FEATURE_HDR_TO_LTE) || defined(FEATURE_HDR_TO_LTE_REDIRECTION)
        case PROT_DEACT_GWL_REDIR:
          abort_reason = HDRHAI_ABORT_IRAT_TO_GWL;
          if (hdralmps_get_state() == HDRALMPS_SUSPEND)		  
          {

#ifdef FEATURE_HDR_TO_LTE
          if(hdralmp_hdr_to_lte_resel_cmd_is_sent())
          {
             HDRTRACE_LTE_RESEL_STATUS (HDRMC_HDR_LTE_RESELECT_SUCCESS); /* HDR to LTE reselect SUCESS */
          }
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
          if(hdralmp_hdr_to_lte_redir_cmd_is_sent())
          {
             hdralmp_clear_hdr_to_lte_redir(); /* Clear all the redirect related info upon success */
             //HDRTRACE_LTE_RESEL_STATUS (HDRMC_HDR_LTE_REDIRECT_SUCCESS); /* HDR to LTE redirect SUCESS */
             hdrind_give_ind( HDRIND_LMAC_ORAT_REDIR_SUCCESS , NULL ); /* inform success to DS */
          }
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */
          }
          break;
#endif /* defined(FEATURE_HDR_TO_LTE) || defined(FEATURE_HDR_TO_LTE_REDIRECTION) */

#ifdef FEATURE_LTE_TO_HDR_OH
        case PROT_DEACT_LTE_HANDOVER:
          abort_reason = HDRHAI_ABORT_EXIT_TUNNEL;
          hdrsap_deactivate( HDRHAI_HDRMC, HDRSAP_DEACT_NO_SRV );
          break;
#endif /* FEATURE_LTE_TO_HDR_OH */

        case PROT_DEACT_CDMA_AMPS_HANDOFF:
        default:
          ERR( "Invalid deactivate reason %d",
               cmd_ptr->msgr_cmd.msg.deactivate_protocol.deact_reason, 0, 0 );
          cmd_ptr->hdr.status = HDRMC_BADPARM_S;
          break;
      }

#ifdef FEATURE_LTE_TO_HDR_OH
      /* If deactivate is received with reason PROT_DEACT_LTE_HANDOVER this 
         means LTE is nt active RAT, we jsut set pre-reg allowed flag to 
         FALSE but remain in online mode. When pre-reg is allowed again LTE 
         RRC will send SIB8 indication. */
      if ( cmd_ptr->msgr_cmd.msg.deactivate_protocol.deact_reason !=
            PROT_DEACT_LTE_HANDOVER )
#endif /* FEATURE_LTE_TO_HDR_OH */
      {
#ifdef FEATURE_HDR_TO_LTE
        if ( hdralmp_hdr_to_lte_resel_is_in_progress() &&
             hdralmp_hdr_to_lte_resel_cmd_is_sent() &&
             (cmd_ptr->msgr_cmd.msg.deactivate_protocol.deact_reason !=
                PROT_DEACT_GWL_REDIR) )
        {
          hdrmc.pending_deact_info.deactivation_is_pending = TRUE;
          hdrmc.pending_deact_info.explicit_mmoc_deactivate = TRUE;
          hdrmc.pending_deact_info.deactivation_reason = abort_reason;
          hdrmc.pending_deact_info.deactivation_trans_id = cmd_ptr->msgr_cmd.msg.deactivate_protocol.trans_id;
          hdralmp_send_reselect_abort_req_to_lte();
        }
	   else if ( hdralmp_hdr_to_lte_resel_is_in_progress() &&
                 hdralmp_hdr_to_lte_resel_abort_cmd_is_sent() &&
                (cmd_ptr->msgr_cmd.msg.deactivate_protocol.deact_reason !=
                 PROT_DEACT_GWL_REDIR) )
       {
          HDR_MSG_PROT(MSG_LEGACY_HIGH, "Online rcv deactivate when D2L abort is in progress. Pending deactivate");
		  hdrmc.pending_deact_info.deactivation_is_pending = TRUE;
          hdrmc.pending_deact_info.explicit_mmoc_deactivate = TRUE;
          hdrmc.pending_deact_info.deactivation_reason = abort_reason;
          hdrmc.pending_deact_info.deactivation_trans_id = cmd_ptr->msgr_cmd.msg.deactivate_protocol.trans_id;
        }
        else if ( hdralmp_hdr_to_lte_resel_is_in_progress() &&
                  hdralmp_hdr_to_lte_resel_abort_cmd_is_sent() &&
                  (cmd_ptr->msgr_cmd.msg.deactivate_protocol.deact_reason !=
                   PROT_DEACT_GWL_REDIR) )
        {
          HDR_MSG_PROT(MSG_LEGACY_HIGH, "Online rcv deactivate when D2L abort is in progress. Pending deactivate");
          hdrmc.pending_deact_info.deactivation_is_pending = TRUE;
          hdrmc.pending_deact_info.explicit_mmoc_deactivate = TRUE;
          hdrmc.pending_deact_info.deactivation_reason = abort_reason;
          hdrmc.pending_deact_info.deactivation_trans_id = cmd_ptr->msgr_cmd.msg.deactivate_protocol.trans_id;
        }
        else
#endif /* FEATURE_HDR_TO_LTE */
#ifdef FEATURE_HDR_TO_LTE_REDIRECTION       
        {
				 if ( hdralmp_hdr_to_lte_redir_is_in_progress() &&
					 hdralmp_hdr_to_lte_redir_cmd_is_sent() &&
					 (cmd_ptr->msgr_cmd.msg.deactivate_protocol.deact_reason !=
						PROT_DEACT_GWL_REDIR) )
				{
				  hdrmc.pending_deact_info.deactivation_is_pending = TRUE;
				  hdrmc.pending_deact_info.explicit_mmoc_deactivate = TRUE;
				  hdrmc.pending_deact_info.deactivation_reason = abort_reason;
				  hdrmc.pending_deact_info.deactivation_trans_id = cmd_ptr->msgr_cmd.msg.deactivate_protocol.trans_id;
				  hdralmp_send_redirect_abort_req_to_lte();
				}
				else
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */
        {
#ifdef FEATURE_HDR_TO_LTE
					/* if D2L resel is in progress and resel command is 
								 not yet sent to LTE means L1 is still deactivating */
					if( hdralmp_hdr_to_lte_resel_is_in_progress() &&
						!hdralmp_hdr_to_lte_resel_cmd_is_sent())
					{
					  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Deactivate rcvd while D2L resel is in progress");
					  /* send abort request so that TRM reservation is canceled */
					  hdralmp_set_pending_abort_resel( TRUE );
					}
#endif /* FEATURE_HDR_TO_LTE */
          hdralmp_deactivate(abort_reason, TRUE, /* MMoC directed deactivate */
                             cmd_ptr->msgr_cmd.msg.deactivate_protocol.trans_id,
                             HDRHAI_HDRMC);
        }

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION       
         }
#endif
      }
      break;

    /*-------------------------------------------------------------------------
    Phone Status Changed Command
    -------------------------------------------------------------------------*/
    case HDRMC_PROT_PH_STAT_CHGD_REQ:

      HDR_MSG_PROT_3( MSG_LEGACY_HIGH, 
                      "Online rcv PhStatChgd type=%d oprtMode=%d id=%d",
                      cmd_ptr->msgr_cmd.msg.ph_status_chgd.chg_type,
                      cmd_ptr->msgr_cmd.msg.ph_status_chgd.oprt_mode,
                      cmd_ptr->msgr_cmd.msg.ph_status_chgd.trans_id );
      
	  /* Reset the NV flag, to track any NV errors during process of this command */
	  hdrutil_reset_nv_error();
	  
      /* If the phone status changed due to a change in subscription or NAM,
         (for OTASP updates) read the ESN and the NAI to see if they have  
         changed wrt the one stored for the current session.
         Also update the HDRACMAC's persistence index. */
      if ( ((cmd_ptr->msgr_cmd.msg.ph_status_chgd.chg_type == PROT_PH_STAT_CHG_SUBSC) ||
            (cmd_ptr->msgr_cmd.msg.ph_status_chgd.chg_type == PROT_PH_STAT_CHG_NAM) ) &&
           (cmd_ptr->msgr_cmd.msg.ph_status_chgd.is_cdma_subs_avail == 1) )
      {
        hdramp_update_esn(HDRHAI_HDRMC);
        hdramp_update_nai(HDRHAI_HDRMC);
#ifdef FEATURE_EHRPD
        hdrscp_update_ehrpd_credentials(HDRHAI_HDRMC);
#endif /* FEATURE_EHRPD */
        hdramac_update_persist_index();
      }

#ifdef FEATURE_LTE_TO_HDR_OH
      if ( hdrsap_tunnel_mode_is_enabled(HDRHAI_HDRMC) &&
           (cmd_ptr->msgr_cmd.msg.ph_status_chgd.chg_type == 
              PROT_PH_STAT_CHG_OPRT_MODE) )
      {
        switch( cmd_ptr->msgr_cmd.msg.ph_status_chgd.oprt_mode )
        {
          case PROT_OPRT_MODE_PWROFF:
          case PROT_OPRT_MODE_OFFLINE:
          case PROT_OPRT_MODE_OFFLINE_CDMA:
          case PROT_OPRT_MODE_LPM:
          case PROT_OPRT_MODE_RESET:

            /* It is possible that we receive PhoneStatusChange with LPM reason
             * during powerup. To avoid writing NV or save session config to
             * EFS during powerup, we check whether the SCMDB copy of session 
             * config is consistent with the EFS copy, and write to NV/EFS 
             * accordingly. */ 
            if ( hdrscmdb_efs_is_up_to_date() )
            {
              HDR_MSG_PROT( MSG_LEGACY_MED, 
                            "tunnel mode rx PhoneStatusChanged: ignore writing to EFS, as it's already stored" );
            }
            else
            {
              hdrsmpka_save_to_nv();
              hdrscp_power_down();
              (void) hdrscmdb_write_all_config_data();
              HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                            "tunnel mode rx PhoneStatusChanged: save session config successfully" );
            }
            break;
  
          default:
            break;
        }
      }
#endif /* FEATURE_LTE_TO_HDR_OH */
      /* Check if there is any NV read error */
      if(hdrutil_get_nv_error() == TRUE)
      {         
         HDR_MSG_PROT(MSG_LEGACY_HIGH," NV read error during Phone Status Change command");
         /* Acknowledge the Phone Status Change command with MMOC */
         mmoc_rpt_ph_stat_chgd_cnf( cmd_ptr->msgr_cmd.msg.ph_status_chgd.trans_id,
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
	                             SD_MODE_HDR, MMOC_PH_STAT_ERR_NV_ERR);
#else
                                 SD_MODE_HDR);
#endif
      }
      else
      {	  		
      /* Acknowledge the Phone Status Change command with MMOC */
      mmoc_rpt_ph_stat_chgd_cnf( cmd_ptr->msgr_cmd.msg.ph_status_chgd.trans_id,
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
	                             SD_MODE_HDR, MMOC_PH_STAT_NO_ERROR);
#else
                                 SD_MODE_HDR);
#endif
      }	  
#ifdef FEATURE_HDR_DEBUG_INFO
      hdrmc_log_sent_rpt(cmd_ptr->msgr_cmd.msg.ph_status_chgd.trans_id, HDR_MMOC_RPT_PH_STAT_CHGD_CNF);
#endif /* FEATURE_HDR_DEBUG_INFO */
      break;

    /*-------------------------------------------------------------------------
    Generic MMoC Command
    -------------------------------------------------------------------------*/
    case HDRMC_PROT_GEN_REQ:
    /* Call protocol generic command proc, with is act flag
    ** set to TRUE.
    */

      switch( cmd_ptr->msgr_cmd.msg.generic_prot.info.cmd_type)
      {
        case PROT_GEN_CMD_SS_TIMER_EXP:
          abort_reason = HDRHAI_ABORT_RESELECTION;

         /* SD needs to know if the protocol is in a state where it can be
          * interrupted.  We assume that we can be interrupted any time
          * except when a connection is open, being opened, or when we are
          * performing an access attempt.
          */
          is_reselect_allowed = hdralmp_reselect_is_allowed(HDRHAI_HDRMC)  &&
                                hdridle_reselect_is_allowed(HDRHAI_HDRMC);
          HDRTRACE_RESELECTION();
          break;

        case PROT_GEN_CMD_PREF_SYS_CHGD:
          abort_reason = HDRHAI_ABORT_PREF_SYS_CHANGE;
          break;

#ifdef FEATURE_HDR_TO_LTE
        case PROT_GEN_CMD_IRAT_TO_GWL:
          abort_reason = HDRHAI_ABORT_IRAT_TO_GWL;
          if (hdralmps_get_state() == HDRALMPS_SUSPEND)
            HDRTRACE_LTE_RESEL_STATUS (HDRMC_HDR_LTE_RESELECT_SUCCESS); /* HDR to LTE reselect success */
          break;
#endif /* FEATURE_HDR_TO_LTE */

        default:
          /* use default abort reason - unspecified */
          break;
      }

      /* If the HDRMC Exit signal has already been set, hdrsrch is already
         deactivated and HDRMC is about to go offline.  In this case, report
         to SD that HDR is inactive.  Otherwise, tell SD that HDR is still 
         active.  (i.e. it is still waiting for a deactivate cb from srch.) */
#ifdef FEATURE_HDR_DEBUG_INFO
	  hdrmc_log_sent_rpt(cmd_ptr->msgr_cmd.msg.generic_prot.info.trans_id, HDR_MMOC_RPT_PROT_GEN_CMD);
#endif /* FEATURE_HDR_DEBUG_INFO */
      if ( rex_get_sigs(HDRMC_TASK_PTR) & HDRMC_EXIT_SIG )
      {
        sd_action = mmoc_proc_prot_gen_cmd( is_reselect_allowed,
                                            FALSE,
                                            &cmd_ptr->msgr_cmd.msg.generic_prot.info);
      }
      else
      {
        sd_action = mmoc_proc_prot_gen_cmd( is_reselect_allowed,
                                            TRUE,
                                            &cmd_ptr->msgr_cmd.msg.generic_prot.info);
      }

      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Online rcv ProcGenCmd sd=%d",
                      sd_action);

      if( hdrmc_feature_is_enabled(HDRMC_FEATURE_JCDMA) &&
          ( cmd_ptr->msgr_cmd.msg.generic_prot.info.cmd_type == 
           PROT_GEN_CMD_PREF_SYS_CHGD) && hdrrup_band_pref_is_changed() )
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
          "Band pref changed; sending silent session close" );

        hdrsmp_deactivate( HDRHAI_HDRMC, TRUE );
      }

      switch(sd_action)
      {
        case SD_SS_ACT_ACQ_HDR:
#ifdef FEATURE_HDR_TO_LTE
          if (hdralmps_get_state() == HDRALMPS_SUSPEND)
          {
            HDR_MSG_PROT( MSG_LEGACY_HIGH, 
             "Ignoring reacq request from MMOC until LTE reselection result" );
          }
          else 
#endif /* FEATURE_HDR_TO_LTE */
          {
            hdralmp_reacquire(abort_reason, HDRHAI_HDRMC);
          }
          break;

        case SD_SS_ACT_CONTINUE:
          break;

        default:
#ifdef FEATURE_HDR_TO_LTE
          if ( hdralmp_hdr_to_lte_resel_is_in_progress() &&
               hdralmp_hdr_to_lte_resel_cmd_is_sent() &&
               (cmd_ptr->msgr_cmd.msg.generic_prot.info.cmd_type != 
                  PROT_GEN_CMD_IRAT_TO_GWL) )
          {
            hdrmc.pending_deact_info.deactivation_is_pending = TRUE;
            hdrmc.pending_deact_info.explicit_mmoc_deactivate = FALSE;
            hdrmc.pending_deact_info.deactivation_reason = abort_reason;
            hdrmc.pending_deact_info.deactivation_trans_id = 0;
            hdralmp_send_reselect_abort_req_to_lte();
          }
          else
#endif /* FEATURE_HDR_TO_LTE */
#ifdef FEATURE_HDR_TO_LTE_REDIRECTION       
          {
							 if ( hdralmp_hdr_to_lte_redir_is_in_progress() &&
								 hdralmp_hdr_to_lte_redir_cmd_is_sent() &&
								 (cmd_ptr->msgr_cmd.msg.generic_prot.info.cmd_type != 
								  PROT_GEN_CMD_IRAT_TO_GWL))
							{
							  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,	"DLR is in progress. pend deactivate info cmd type %d",
																 cmd_ptr->msgr_cmd.msg.generic_prot.info.cmd_type);
							  hdrmc.pending_deact_info.deactivation_is_pending = TRUE;
							  hdrmc.pending_deact_info.explicit_mmoc_deactivate = FALSE;
							  hdrmc.pending_deact_info.deactivation_reason = abort_reason;
							  hdrmc.pending_deact_info.deactivation_trans_id = cmd_ptr->msgr_cmd.msg.deactivate_protocol.trans_id;
							  hdralmp_send_redirect_abort_req_to_lte();
							}
							else
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */
					{
#ifdef FEATURE_HDR_TO_LTE            
					  /* if D2L resel is in progress and resel command is 
								   not yet sent to LTE means L1 is still deactivating */
					  if( hdralmp_hdr_to_lte_resel_is_in_progress() &&
						  !hdralmp_hdr_to_lte_resel_cmd_is_sent())
          {
						  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Deactivate rcvd while D2L resel is in progress");
						  /* send abort request so that TRM reservation is canceled */
						  hdralmp_set_pending_abort_resel( TRUE );
					  }
#endif /* FEATURE_HDR_TO_LTE */
            /* We need to exit HDR */
            hdralmp_deactivate(abort_reason, FALSE, /* Implicit deactivate */
                               0, HDRHAI_HDRMC);
          }
#ifdef FEATURE_HDR_TO_LTE_REDIRECTION       
           }
#endif

          break;
      }
      break;

#ifndef FEATURE_HDR_BOLT_MODEM
    /*-------------------------------------------------------------------------
     Offline Command
    -------------------------------------------------------------------------*/
    case MM_MMOC_OFFLINE_IND:
      HDR_MSG_PROT (MSG_LEGACY_HIGH,  "Rxed Offline Cmd");
      hdrmc.type_of_exit = HDRMC_EXIT_OFFLINE;
      break;
#endif /* FEATURE_HDR_BOLT_MODEM */

    /*-------------------------------------------------------------------------
    Origination Command
    -------------------------------------------------------------------------*/
    case HDRMC_ORIGINATION_REQ:
      HDR_MSG_PROT (MSG_LEGACY_HIGH,  "Rxed Orig Cmd");

      hdrcallid_set_cur_call_id(cmd_ptr->msgr_cmd.msg.origination.call_id);

      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Orig call id=%d",
                      cmd_ptr->msgr_cmd.msg.origination.call_id);

#ifdef FEATURE_HDR_TO_LTE
      if (hdralmps_get_state() == HDRALMPS_SUSPEND)
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
             "Ignoring call orig from CM until LTE reselection result" );

        hdrmc_send_orig_failure( CM_CALL_ORIG_ERR_ACC_FAIL );
        break;
      }
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_IRAT_PILOT_MEAS
      if ( !hdrsmp_is_to_maintain_evdo_session() )
      {
        hdrmc_send_orig_failure( CM_CALL_ORIG_ERR_INCOMPATIBLE );
        break;
      }
#endif /* FEATURE_IRAT_PILOT_MEAS */

      /* Tell SMP that a call origination is initiated */
      hdrsmp_process_call_orig();

      if(
#ifdef FEATURE_HDR_QOS
         (!hdrstream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN)) &&
         (!hdrstream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN)) &&
         (!hdrstream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)) &&
#endif /* FEATURE_HDR_QOS */
         (!hdrstream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_SN)))
      {
        hdrmc_send_orig_failure( CM_CALL_ORIG_NO_SESSION);

        /* AT fails the origination because we don't have a session. 
         * This could due to the fact that MUM is enabled so SMP doesn't
         * maintain EVDO session, or a session has been closed,
         * or has never been opened. If AT is in AMP_SETUP state when
         * the origination command is received, calling hdrsmp_activate()
         * will cause the AT to send multiple UATI Requests.
         * If AT is idle on a DO system, open the session immediately. */
        if (hdrsmp_session_is_inactive(HDRHAI_HDRMC) &&
            hdrsmp_mum_is_enabled() &&
            hdralmp_system_is_acquired(HDRHAI_HDRMC))
        {
          HDR_MSG_PROT (MSG_LEGACY_HIGH, " HDRMC: Opening new session ...");
          hdrsmp_activate( HDRHAI_HDRMC );
        }
        break;
      }

#ifdef FEATURE_HDR_HANDOFF
      /* Open a connection even of DRS = FALSE in order to ensure
       * that DS gets a positive acknowledgement that the packet
       * data session has been moved to HDR.
       */
      if(cmd_ptr->msgr_cmd.msg.origination.drs == TRUE)
      {
#endif /* FEATURE_HDR_HANDOFF */
        hdralmp_open_connection( HDRHAI_OPEN_USER_REQ, HDRHAI_HDRMC );
#ifdef FEATURE_HDR_HANDOFF
      }
      else
      {
        /* release the call and do not open a connection
           since it is not real call (DRS=0)
           the origination mechanism is only used to
           control when to start the tarnsmission of the
           location notification message for dormant
           data session handoff
        */
        hdrmc_send_release(CM_CALL_REL_NORMAL);
      }

#ifdef FEATURE_EHRPD
      if (hdrmrlpc_current_personality_is_ehrpd())
      {
        curr_sys_info = DS707_EXTIF_SYSTEM_EPC;
      }
#endif /* FEATURE_EHRPD */

      /* If our packet data session is dormant on 1x, we need to move it
       * to HDR, only if it is transferable.
       */
      if( (cmd_ptr->msgr_cmd.msg.origination.last_act_data_net ==
           HDRCP_DIGITAL_MODE_CDMA) && 
          ds707_extif_is_pkt_data_sess_transferable(curr_sys_info) )
      {
        hdrlup_update_loc(HDRHAI_HDRMC);
        hdrlup_send_loc(HDRHAI_HDRMC);
      }
#endif /* FEATURE_HDR_HANDOFF */
      break;

#ifdef FEATURE_HDR_DOS
      /* Prepare to send a Data Over Signaling message */
    case HDRMC_SEND_DATA_OVER_SIGNALING_REQ:
#ifdef FEATURE_HDR_QCHAT
      if (hdrqchatupk_mod_enabled()) 
      {
        hdrqchatupk_dos_process_hdrmc_cmd(
          &cmd_ptr->msgr_cmd.msg.mo_dos);
      }
#endif /*FEATURE_HDR_QCHAT*/        
      hdrdos_send_dos_msg( (dsm_item_type *)
                           cmd_ptr->msgr_cmd.msg.mo_dos.dos_buf_ptr,
                           cmd_ptr->msgr_cmd.msg.mo_dos.rlp_flow,
                           (hdrdos_route_enum_type)
                            cmd_ptr->msgr_cmd.msg.mo_dos.route,
                           cmd_ptr->msgr_cmd.msg.mo_dos.ack_req );
      break;
#endif /* FEATURE_HDR_DOS */

    /*-------------------------------------------------------------------------
    Call End Command
    -------------------------------------------------------------------------*/
    case HDRMC_END_REQ:
      HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "Rcvd HDRMC_END_F CallId=%d, reason=0x%x",
                      cmd_ptr->msgr_cmd.msg.call_end.call_id , 
                      cmd_ptr->msgr_cmd.msg.call_end.end_reason);

      /* We will close the connection if any of the following is true:
       * 1) end reason is CAI_REL_RUDE
       * 2) FEATURE_HDR_SESSION_TIED_TO_PPP is enabled and CAI_REL_SRV_INACT
       * 3) for all other reason if the following are all true:
       *   a) ALMP connection is open
       *   b) session is not configuring
       *   c) there are some call_id alive before deletion
       *   d) there are no call_id left after current is deleted
       */       

      end_cmd_will_close_con = TRUE;

      /* # of callid before deleting is needed for checking 3(c) */
      num_active_call_ids = hdrcallid_get_num_active_call_ids();
      /*-------------------------------------------------------------------
        If HDRMC receives a End command with reason to Abort the Call,
        this is a notification telling us just get rid of the specified
        Call ID. In this particular case, we do not tear down the entire
        HDR traffic channel, but simply get rid of the specified call id.

        However, after removing the call id, if there are no other
        call ids associated with the HDR traffic channel, then the
        connection is torn down.
      -------------------------------------------------------------------*/
      hdrcallid_deallocate_call_id( cmd_ptr->msgr_cmd.msg.call_end.call_id );

      if(cmd_ptr->msgr_cmd.msg.call_end.end_reason == CAI_REL_RUDE)
      {
        /* CAI__REL_RUDE will always result in relesing TC */
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Rcvd CAI_REL_RUDE, close");
        // hdralmp_close_connection(HDRHAI_ABORT_USER_RUDE_CLOSE, HDRHAI_HDRMC );
      }
#ifdef FEATURE_HDR_TO_LTE_REDIRECTION 
      else if(cmd_ptr->msgr_cmd.msg.call_end.end_reason == HDRCP_CAI_IRAT_MEAS)
      {
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Rcvd call end due to IRAT meas, close");
        hdralmp_set_hdr_to_lte_redir_meas_in_progress(TRUE);        
      }
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */
      /* Close the HDR session if the PPP was up */
      else if (hdrmc_feature_is_enabled(
                 HDRMC_FEATURE_HDR_SESSION_TIED_TO_PPP) &&
               hdrmc_feature_is_enabled(HDRMC_FEATURE_JCDMA) &&
               (cmd_ptr->msgr_cmd.msg.call_end.end_reason == CAI_REL_SRV_INACT))
      {
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Rcvd End Cmd, Close HDR Session");
#ifndef FEATURE_DEDICATED_C2K_MODE
        if (hdrpacdsif_is_sn_ppp_up())
        {
          /* Deregister for Session Close event so DS does not attempt to close the 
             HDR Session as well. */
          hdrsmp_deactivate(HDRHAI_HDRMC, FALSE);
          hdrpacdsif_post_smp_session_closed_cmd_to_ds();
        }
#else
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "DCM: Don't post SMP_sess_closed to DS");
        hdrsmp_dcm_deactivate();
#endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */
      }
      else if ((!hdralmp_connection_is_open(HDRHAI_HDRMC)) &&
               (!hdridle_is_connecting()))
      {
        /* The connection was never opened and AT is not setting up a connection
         * we don't need to send ConnectionClose. 
         */
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "ALMP not open/connecting, no need to close");
        end_cmd_will_close_con = FALSE;
      }
      else if (hdrscp_is_configuring())
      {
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "SMP configuring, keep connection");
        end_cmd_will_close_con = FALSE;
      }
      else if (num_active_call_ids == 0)
      {
        /* This is a race condition where we have deallocated the call ID
         * due to some other reason (e.g. fade or release).  We still 
         * send the end_cnf.
         */
        ERR("No call IDs allocated while connected",0,0,0);
        end_cmd_will_close_con = FALSE;
      }
      else if (hdrcallid_get_num_active_call_ids() != 0)
      {
        HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                        "Dealloc. invalid CallID %d, still connected",
                        cmd_ptr->msgr_cmd.msg.call_end.call_id );
        end_cmd_will_close_con = FALSE;
      }

      if (end_cmd_will_close_con)
      {
        hdrcallid_save_user_callid( cmd_ptr->msgr_cmd.msg.call_end.call_id );
        HDR_MSG_PROT (MSG_LEGACY_HIGH, "Call ID aborted, closing connection");                
        hdralmp_close_connection(HDRHAI_ABORT_NORMAL_CLOSE, HDRHAI_HDRMC );
      }
      else 
      {
        /* The purpose of CM_HDR_CALL_END_CNF_F is to allow for CM to wait for the
         * ConnectionClose message to be sent before it issues another command.  
         * In this case the connection was a) never even opened or b) not being 
         * closed by this command, so it is OK to send it now.
         */
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "Call ID aborted, still connected" );
        hdrmc_send_end_cnf( cmd_ptr->msgr_cmd.msg.call_end.call_id );
      }

    break;

    /*-------------------------------------------------------------------------
    Idle Paging Slot Changed Command
    -------------------------------------------------------------------------*/
    case HDRMC_IDLE_PGSLOT_CHANGED_REQ:

      /* cdma idle paging slot changed */
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "Rcv IdlePgslotChange");
      hdridle_update_pref_cntl_chan_cycle(HDRHAI_HDRMC);

      break;

    /*-------------------------------------------------------------------------
    Unlock HDR RF Command
    -------------------------------------------------------------------------*/
    case HDRMC_UNLOCK_HDR_RF_REQ:

      HDR_MSG_PROT (MSG_LEGACY_HIGH, "RcvUnlockHDRRF, advising srch");

      /* advice HDRSRCH that 1x needs the RF lock */
#ifdef FEATURE_HDR_TO_LTE
      /* Only if on non-SVLTE device, 1x and LTE compete for RF.
       * When we receive advice unlock RF this means 1x wants the RF for voice 
       * call if reselection is in progress we need to abort it.
       */

      input_info.client_id = TRM_HDR;
      input_info.info_type = TRM_SIMUL_CAPABILITY;
      input_info.input_data.client_info.requested_client_id = TRM_HDR;
      trm_get_info( &input_info, &output_info);

      if ( ( output_info.info.simul_cap_info  & TRM_SVLTE_IS_ENABLED ) )
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                      "On SVLTE, ignore advise unlock." );
      }
      else if ( !hdralmp_hdr_to_lte_resel_is_in_progress() )
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                      "Rcv advise unlock while reselection is not in progress." );
        hdrlmac_advice_unlock_rf();
      }
      else
      {
        if ( hdralmp_hdr_to_lte_resel_cmd_is_sent() )
        {
          /* HDR SRCH has been deactivated. Abort reselection. */
          hdralmp_send_reselect_abort_req_to_lte();
        }
        else
        {
          /* HDR SRCH has been asked to be deactivated. Wait SRCH's 
           * deactivation done callback and reacqure. */
          HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                        "Rcv UnlockRF before sending resel cmd to LTE, reacq later" );
          hdralmp_set_pending_abort_resel( TRUE );
          hdrlmac_advice_unlock_rf();
        }
      }
#else
      hdrlmac_advice_unlock_rf();
#endif /* FEATURE_HDR_TO_LTE */

    break;

    /*-------------------------------------------------------------------------
    Broadcast Update Flow Request List Command
    -------------------------------------------------------------------------*/
    case HDRMC_BCMCS_UPDATE_FLOW_REQ:
    case HDRMC_BCMCS_LEGACY_UPDATE_FLOW_REQ:
#ifdef FEATURE_HDR_BCMCS

      HDR_MSG_PROT(MSG_LEGACY_HIGH,"Process BCMCS UpdateFlowRequest Cmd");

#ifdef FEATURE_HDR_BCMCS_2p0
      #error code not present
#else
      ASSERT(cmd_ptr->msgr_cmd.msg.bcmcs_update_flow_legacy.num_of_flows 
             <= HDRMC_BCMCS_MAX_FLOWS);
      hdrbccp_update_flow_request ( 
        HDRHAI_HDRMC,
        cmd_ptr->msgr_cmd.msg.bcmcs_update_flow_legacy.transaction_id,
        cmd_ptr->msgr_cmd.msg.bcmcs_update_flow_legacy.num_of_flows,
        cmd_ptr->msgr_cmd.msg.bcmcs_update_flow_legacy.flow_addrs
                                   );
#endif /* FEATURE_HDR_BCMCS_2p0 */

#endif /* FEATURE_HDR_BCMCS */
      break;


    /*-------------------------------------------------------------------------
    Discontinue Broadcast Command
    -------------------------------------------------------------------------*/
    case HDRMC_BCMCS_DISCONTINUE_BROADCAST_REQ:
    case HDRMC_BCMCS_LEGACY_DISCONTINUE_BROADCAST_REQ:
#ifdef FEATURE_HDR_BCMCS

      HDR_MSG_PROT(MSG_LEGACY_HIGH,"Process BCMCS DiscontinueBroadcast Cmd");
      hdrbccp_discontinue_broadcast ( 
        HDRHAI_HDRMC,
#ifdef FEATURE_HDR_BCMCS_2p0
        #error code not present
#else
        cmd_ptr->msgr_cmd.msg.bcmcs_discontinue_bc_legacy.transaction_id
#endif /* FEATURE_HDR_BCMCS_2p0 */
                                     );
#endif /* FEATURE_HDR_BCMCS */
    break;

    /*-------------------------------------------------------------------------
    Tunneling Messages on DO channel
    -------------------------------------------------------------------------*/
    case HRDMC_TUNNEL_MSG_REQ:
      #ifdef FEATURE_HDR_CSNA
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "Process CSNA TunnelMsg Cmd");

      hdrcsna_tunnel_msg ( HDRHAI_HDRMC,
                           cmd_ptr->msgr_cmd.msg.tunnel_msg.payload.dest_protocol,
                           cmd_ptr->msgr_cmd.msg.tunnel_msg.payload.chan,
                           cmd_ptr->msgr_cmd.msg.tunnel_msg.payload.p_rev,
                           cmd_ptr->msgr_cmd.msg.tunnel_msg.payload.len,
                           cmd_ptr->msgr_cmd.msg.tunnel_msg.payload.payload );
      #endif /* FEATURE_HDR_CSNA */
      break;

    /*-------------------------------------------------------------------------
    Tunneling Messages on DO channel
    -------------------------------------------------------------------------*/
    case HDRMC_CSNA_MSG_CONFIG_REQ:
      #ifdef FEATURE_HDR_CSNA
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "Process CSNA Msg Config Cmd");
  
      hdrcsna_update_msg_config ( HDRHAI_HDRMC,
                                  &cmd_ptr->msgr_cmd.msg.csna_msg_config );
      #endif /* FEATURE_HDR_CSNA */
      break;

    /*-------------------------------------------------------------------------
    BCMCS 2.0 - Send Registration
    -------------------------------------------------------------------------*/
    case HDRMC_BCMCS_SEND_REGISTRATION_REQ:
#ifdef FEATURE_HDR_BCMCS_2p0
      #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
      break;     

#if defined(FEATURE_HDR_BCMCS_2p0) && defined(FEATURE_HDR_BCMCS_DYNAMIC_MODEL)
    #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 && FEATURE_HDR_BCMCS_DYNAMIC_MODEL */

#ifdef FEATURE_HDR_QCHAT
    case HDRMC_QCHAT_REQ:
        if (hdrqchatupk_imo_enabled())
        {
          hdrqchatupk_idle_process_cmd(&cmd_ptr->msgr_cmd.msg.qchat_opt);
        }
    break;
#endif /*FEATURE_HDR_QCHAT*/

#ifdef FEATURE_LTE_TO_HDR
#ifdef FEATURE_LTE_TO_HDR_OH
    /*-------------------------------------------------------------------------
    LTE to HDR redirection
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_REDIRECT_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Online rcv LTE-HDR redirect cmd");
      hdrmc_process_lte_to_hdr_redirect_cmd(cmd_ptr);

    break;

    /*-------------------------------------------------------------------------
    LTE to HDR reselection
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_RESELECT_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Online rcv LTE-HDR reselect cmd");
      hdrmc_process_lte_to_hdr_reselect_cmd(cmd_ptr);

    break;

    /*-------------------------------------------------------------------------
    LTE to HDR redirection/reselection confirmation from MMOC
    -------------------------------------------------------------------------*/
    case HDRMC_PROT_LTE_TO_HDR_HO_CNF_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Online rcv LTE-HDR handoff cnf cmd");
      (void) hdrmc_process_lte_to_hdr_ho_cnf_cmd(cmd_ptr, 
                                                 HDRMC_ONLINE_STATE);
    break;

    /*-------------------------------------------------------------------------
    Timing transfer request from LTE.
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_REVERSE_TIMING_TRANSFER_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Online rcv timing transfer req");
      hdrmc_process_reverse_time_transfer_req_cmd(cmd_ptr);

    break;

    /*-------------------------------------------------------------------------
    Get Cell Global Identity (CGI) from LTE.
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_GET_CGI_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Online rcv LTE-HDR get CGI cmd");
      hdrmc_process_lte_get_cgi_cmd(cmd_ptr);

    break;
#endif /* FEATURE_LTE_TO_HDR_OH */


    /*-------------------------------------------------------------------------
    Abort LTE to HDR redirection.
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_ABORT_REDIRECT_REQ:
    case HDRMC_LTE_ABORT_RESELECT_REQ:
#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRMC_LTE_ABORT_ACTIVE_HO_REQ:
#endif /* FEATURE_LTE_TO_HDR_OH */

      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Online rcv abort LTE to HDR handoff");

      if (hdrmc_lte_to_hdr_ho_is_in_progress())
      {
        hdrmc.lte_to_hdr_ho_aborted = TRUE;
        hdralmp_deactivate(HDRHAI_ABORT_OFFLINE, FALSE, 0, HDRHAI_HDRMC);
      }
      else
      {
	    hdrmc_send_abort_done_to_lte(FALSE);
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Recvd LTE-HDR abort command when LTE to HDR handoff not in progress. send dummy response");
      }

      break;

    /*-------------------------------------------------------------------------
    Confirmation from SD for auto HDR activation following LTE-HDR redirection.
    -------------------------------------------------------------------------*/
    case HDRMC_PROT_ACT_CNF_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Rcvd conf from SD for auto HDR activation");

#ifdef FEATURE_LTE_TO_HDR_OH
      if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == 
          HDRMC_IRAT_LTE_TO_HDR_ACTIVE_HO)
      {
        hdrrup_act_ho_completed();
      }
#endif /* FEATURE_LTE_TO_HDR_OH */

      if ((hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_REDIRECT)
          ||(hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_RESELECT))
      {
        hdralmp_send_acq_db_earfcn_req_to_lte();
      }

      hdrmc_clr_lte_to_hdr_ho_in_progress();

      /* L2D Redirection/reselection successful case */
      if ( hdrmc.tunnel_activation_is_pending )
      {
        hdrmc_set_tunnel_activation_is_pending(FALSE);
      }
#ifdef FEATURE_LTE_TO_HDR_OH
      /* If there is no session in the tunnel, when moving out of tunnel,
       * need to change back the default subtypes. */
      if ( !hdrstream_stream_is_negotiated( HDRHAI_APP_PACKET_APP_ANY ) )
      {
        hdrscp_update_subtype_defaults(); 
      }
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_EHRPD
      if (hdrmrlpc_current_personality_is_ehrpd())
      {
        HDRTRACE_MRLP_EHRPD_PERSONALITY_IS_ACTIVE(TRUE);
      }
      else
      {
        HDRTRACE_MRLP_EHRPD_PERSONALITY_IS_ACTIVE(FALSE);
      }
#endif /* FEATURE_EHRPD */
      break;

    /*-------------------------------------------------------------------------
    1X/HRPD capabilities request from LTE
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_1X_HRPD_CAPABILITIES_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Rcvd 1X/HRPD capabilities request from LTE");
      hdrmc_send_1x_hrpd_capabilities_to_lte(
        cmd_ptr->msgr_cmd.msg.lte_capabilities_req.cdma_band_preference);

      break;

    /*-------------------------------------------------------------------------
    Abort HDR to LTE time transfer
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_ABORT_REVERSE_TIMING_TRANSFER_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Online rcv abort HDR to LTE time transfer");

      if (hdrmc_hdr_to_lte_time_transfer_is_in_progress())
      {
        hdrmc.lte_to_hdr_ho_aborted = TRUE;
        hdralmp_deactivate(HDRHAI_ABORT_OFFLINE, FALSE, 0, HDRHAI_HDRMC);
      }
      else
      {
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Recvd LTE-HDR abort command in valid state");
      }

      break;

    /*-------------------------------------------------------------------------
    Abort get Cell Global Identity (CGI) from LTE.
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_ABORT_CGI_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Online rcv abort get CGI");

      if (hdrmc_lte_to_hdr_get_cgi_is_in_progress())
      {
        hdrmc.lte_to_hdr_ho_aborted = TRUE;
        hdralmp_deactivate(HDRHAI_ABORT_OFFLINE, FALSE, 0, HDRHAI_HDRMC);
      }
      else
      {
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Recvd LTE-HDR abort command in valid state");
      }

      break;
#endif /* FEATURE_LTE_TO_HDR */

     /*-------------------------------------------------------------------------
     Collect and report FTD stats
     -------------------------------------------------------------------------*/
    case HDRMC_SET_FTD_STATS_MASK_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "Process Collect FTD Stats Cmd");

      hdrlog_process_collect_ftd_stats( 
        cmd_ptr->msgr_cmd.msg.ftd_stats.bit_mask );
      break;

#ifdef FEATURE_IRAT_PILOT_MEAS
    /*-------------------------------------------------------------------------
    Activate Measurement Mode
    -------------------------------------------------------------------------*/
    case HDRMC_MEAS_MODE_ON_REQ:
      /* Activate measurement mode */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv meas mode activated");

      /* Turn on DCM that is AT is not allowed to open a session and respond to
         messages from AN */
      hdrsmp_set_meas_mode_is_on_flag(TRUE); 
      hdrsmp_dcm_deactivate(); 

      /* Send confirmation to CM */
      hdrmc_send_meas_mode_cnf(SYS_MEAS_MODE_HDR_ON);
      break;

    /*-------------------------------------------------------------------------
    De-activate Measurement Mode
    -------------------------------------------------------------------------*/
    case HDRMC_MEAS_MODE_OFF_REQ:
      /* De-Activate measurement mode */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv meas mode de-activated");

      /* Turn off DCM that is AT is allowed to open a session and respond to 
         messages from AN */
      hdrsmp_set_meas_mode_is_on_flag(FALSE);
      hdrsmp_process_call_orig();

      /* Send confirmation to CM */
      hdrmc_send_meas_mode_cnf(SYS_MEAS_MODE_HDR_OFF);

      /* When measurment mode is turned off open HDR session if it is already
         not open */
      if(
#ifdef FEATURE_HDR_QOS
         (!hdrstream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN)) &&
         (!hdrstream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN)) &&
#endif /* FEATURE_HDR_QOS */
         (!hdrstream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_SN)))
      {
        if (hdrsmp_session_is_inactive(HDRHAI_HDRMC) &&
            hdralmp_system_is_acquired(HDRHAI_HDRMC))
        {
          HDR_MSG_PROT (MSG_LEGACY_HIGH, " HDRMC: Opening new session ...");
          hdrsmp_activate( HDRHAI_HDRMC );
        }
      }
      break;

    /*-------------------------------------------------------------------------
    IRAT measurement request
    -------------------------------------------------------------------------*/
    case HDRMC_MEAS_REQ_CMD_REQ:

      /* De-Activate measurement mode */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv meas request");

      hdrrup_irat_pilot_meas_req( HDRHAI_HDRMC, 
                                  cmd_ptr->msgr_cmd.msg.irat_meas.pilot_meas); 
      break;
#endif /* FEATURE_IRAT_PILOT_MEAS */

#ifdef FEATURE_HDR_DUAL_SIM
     case HDRMC_STANDBY_PREF_CHGD_REQ:

#ifdef FEATURE_LTE_TO_HDR_OH
       /* HDR Tunnel mode is considered as being offline. */
      if ( ( hdrsap_tunnel_mode_is_enabled( HDRHAI_HDRMC ) 
               == TRUE ) )
      {
	    if ( ( hdrsap_get_current_tunnel_mode_value( HDRHAI_HDRMC )
	 	               != HDRHAI_TUNNEL_MODE_NO_SRV ) )
        {
             HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "HDRMC_STANDBY_PREF_CHGD_REQ: tunnel mode is NOT NO SRV. Tunnel mode = %d",
			 	                                hdrsap_get_current_tunnel_mode_value( HDRHAI_HDRMC ));
        }
        hdrlmac_standby_pref_chgd(cmd_ptr->msgr_cmd.msg.sys_standby_cmd.info);
        mmoc_rpt_ds_stat_chgd_cnf(cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id);
#ifdef FEATURE_HDR_DEBUG_INFO
        hdrmc_log_sent_rpt(cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id, HDR_MMOC_RPT_DS_STAT_CHGD_CNF);
#endif /* FEATURE_HDR_DEBUG_INFO */
        HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "standby_pref_chgd tid %d pref %d", 
                        cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id, 
                        cmd_ptr->msgr_cmd.msg.sys_standby_cmd.info );
      }
      else
#endif /* FEATURE_LTE_TO_HDR_OH */
      {
      /* If in DSDS mode or switching to DSDS mdoe then error fatal because
         we cannot do DSDS->SSSS switch in online mode */
      if ( ( hdrmultirat_get_mode_pref() == HDRMULTIRAT_DUAL_SIM_DUAL_STANDBY ) ||
             ( cm_get_device_mode()== SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY ) )
      {
        /* HDRMC_STANDBY_PREF_CHGD_CMD_F must be received when HDR is inactive */
        if(hdrmc.hdr_exiting == TRUE)/* if HDR is exiting */
		{
           hdrlmac_standby_pref_chgd(cmd_ptr->msgr_cmd.msg.sys_standby_cmd.info);
        
           mmoc_rpt_ds_stat_chgd_cnf(cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id);
#ifdef FEATURE_HDR_DEBUG_INFO
           hdrmc_log_sent_rpt(cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id, HDR_MMOC_RPT_DS_STAT_CHGD_CNF);
#endif /* FEATURE_HDR_DEBUG_INFO */
           HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "standby_pref_chgd tid %d pref %d", 
                       cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id, 
                       cmd_ptr->msgr_cmd.msg.sys_standby_cmd.info);
		}
		else
		{
        ERR_FATAL("HDRMC_STANDBY_PREF_CHGD_CMD_F rcvd when online", 0, 0, 0);
		}        
      }
      else
      {
        /* Change mode for L1 */
        hdrlmac_standby_pref_chgd(cmd_ptr->msgr_cmd.msg.sys_standby_cmd.info);
        mmoc_rpt_ds_stat_chgd_cnf(cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id);
#ifdef FEATURE_HDR_DEBUG_INFO
        hdrmc_log_sent_rpt(cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id, HDR_MMOC_RPT_DS_STAT_CHGD_CNF);
#endif /* FEATURE_HDR_DEBUG_INFO */
        HDR_MSG_PROT_2(MSG_LEGACY_MED, "standby_pref_chgd tid %d pref %d", 
                      cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id, 
                      cmd_ptr->msgr_cmd.msg.sys_standby_cmd.info);
      }
     }
      break;
#endif /* FEATURE_HDR_DUAL_SIM */

#ifdef FEATURE_HDR_TO_LTE
    /*-------------------------------------------------------------------------
    LTE Reselect failed response
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_RESEL_FAILED_RSP:
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv LTE reselect failed");

      HDRTRACE_LTE_RESEL_STATUS (HDRMC_HDR_LTE_RESELECT_FAIL); /* HDR to LTE reselect failed */

      if(cmd_ptr->msgr_cmd.msg.lte_resel_failed_rsp.fail_cause == 
	  	          LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_TA)
      {
        HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "Online rcv LTE reselect failed with reasn %d bar time %d", 
			                    LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_TA,
                                            cmd_ptr->msgr_cmd.msg.lte_resel_failed_rsp.cell_bar_time);
        hdrovhd_update_lte_fearfcn(hdralmp_get_reselect_earfcn(),
			   cmd_ptr->msgr_cmd.msg.lte_resel_failed_rsp.cell_bar_time);
      }
	  
	  if (hdralmp_hdr_to_lte_resel_is_in_progress())
      {
        /* If HDR is deactivated during D2L reselection, HDR aborts D2L first.
         * After receiving the abort confirmation back from LTE, HDR continues
         * to deactivate itself. However due to race condition, d2l resel failure
           can come before d2l abort rsp. */ 
        if ( hdrmc.pending_deact_info.deactivation_is_pending )
        {
		  HDR_MSG_PROT (MSG_LEGACY_HIGH, "deactivation_is_pending when LTE reselect failed is rcvd");
          hdralmp_deactivate( hdrmc.pending_deact_info.deactivation_reason,
                              hdrmc.pending_deact_info.explicit_mmoc_deactivate,
                              hdrmc.pending_deact_info.deactivation_trans_id,
                              HDRHAI_HDRMC );
          memset(&hdrmc.pending_deact_info, 0, sizeof(hdrmc_pending_deact_info_type));
        }
        else if( hdrmc.ue_mode_switch_cnf.pending == TRUE)
        {
          HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "UE mode switch cnf pending with trans_id %d",hdrmc.ue_mode_switch_cnf.trans_id);
          
          mmoc_hdr_rpt_ue_mode_switch_cnf(hdrmc.ue_mode_switch_cnf.trans_id, SYS_MODEM_AS_ID_1);
          memset(&hdrmc.ue_mode_switch_cnf, 0, sizeof(hdrmc_ue_mode_switch_cnf_info_type));

          HDR_MSG_PROT (MSG_LEGACY_HIGH, "UE mode switch report sent !!");

          /* Reacquire HDR */
          hdralmp_reacquire( HDRHAI_ABORT_D2L_FAILURE, HDRHAI_HDRMC);
        }
        else
        {
     /* Reacquire HDR */
       HDR_MSG_PROT (MSG_LEGACY_HIGH, "reacquiring when D2L failed is rcvd");
       hdralmp_reacquire( HDRHAI_ABORT_D2L_FAILURE, HDRHAI_HDRMC);
        }
      }
	  else
      {
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, 
		              "Ignoring reselect failed response from LTE. pending deactivation %d",
			          hdrmc.pending_deact_info.deactivation_is_pending);          
      }
      break;

    /*-------------------------------------------------------------------------
    LTE Reselect abort response
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_ABORT_RESEL_RSP:
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv D2L resel abort response");
	  
      HDRTRACE_LTE_RESEL_STATUS (HDRMC_HDR_LTE_RESELECT_ABORT); /* HDR to LTE reselect aborted */

      if (hdralmp_hdr_to_lte_resel_is_in_progress())
      {
        hdralmp_clr_hdr_to_lte_resel_abort_cmd_is_sent();
		
        /* If HDR is deactivated during D2L reselection, HDR aborts D2L first.
         * After receiving the abort confirmation back from LTE, HDR continues
         * to deactivate itself. */ 
        if ( hdrmc.pending_deact_info.deactivation_is_pending )
        {
          hdralmp_deactivate( hdrmc.pending_deact_info.deactivation_reason,
                              hdrmc.pending_deact_info.explicit_mmoc_deactivate,
                              hdrmc.pending_deact_info.deactivation_trans_id,
                              HDRHAI_HDRMC );
          memset(&hdrmc.pending_deact_info, 0, sizeof(hdrmc_pending_deact_info_type));
        }
        else
        {
          /* Reacquire HDR */
          hdralmp_reacquire( HDRHAI_ABORT_D2L_FAILURE, HDRHAI_HDRMC);
        }
      }
      else
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH, 
               "Ignoring abort response from LTE. ALMP not in suspend state");
      }
      break;

    /*-------------------------------------------------------------------------
    Abort LTE reselect request from CM
    -------------------------------------------------------------------------*/
    case HDRMC_CM_ABORT_LTE_RESEL_REQ:
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv abort LTE resel req from CM");

      hdralmp_send_reselect_abort_req_to_lte();

      break;
#endif /* FEATURE_HDR_TO_LTE */

/*-------------------------------------------------------------------------
 UE Mode change request from MMODE
 -------------------------------------------------------------------------*/
    case HDRMC_UE_MODE_CHANGE_REQ:
   
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv UE mode change request");
#ifdef FEATURE_HDR_TO_LTE   
      if ( hdralmp_hdr_to_lte_resel_is_in_progress() &&
           hdralmp_hdr_to_lte_resel_cmd_is_sent())
      {
	    hdrmc.ue_mode_switch_cnf.pending = TRUE;
	    hdrmc.ue_mode_switch_cnf.trans_id = cmd_ptr->msgr_cmd.msg.ue_mode_chg_req.trans_id;

	    HDR_MSG_PROT (MSG_LEGACY_HIGH, "D2L reselection is in progress. Send abort request to LTE");
	 
	    hdralmp_send_reselect_abort_req_to_lte();
      }
      else if ( hdralmp_hdr_to_lte_resel_is_in_progress() &&
			    hdralmp_hdr_to_lte_resel_abort_cmd_is_sent())
      {
	    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Online rcv UE mode change request when D2L abort is in progress. Pending ue mode report");
	    hdrmc.ue_mode_switch_cnf.pending = TRUE;
	    hdrmc.ue_mode_switch_cnf.trans_id = cmd_ptr->msgr_cmd.msg.ue_mode_chg_req.trans_id;			
      }
      else
#endif /* FEATURE_HDR_TO_LTE */
      {
	    HDR_MSG_PROT (MSG_LEGACY_HIGH, "D2L reselection is NOT in progress. Send UE mode switch cnf immediately to MMOC");
	    /* inform MMOC immediately */
	    mmoc_hdr_rpt_ue_mode_switch_cnf(cmd_ptr->msgr_cmd.msg.ue_mode_chg_req.trans_id, SYS_MODEM_AS_ID_1);

	    HDR_MSG_PROT (MSG_LEGACY_HIGH, "UE mode switch report sent !!");
      }	  
   break;


#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
    /*-------------------------------------------------------------------------
    LTE Reselect failed response
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_REDIR_FAILED_RSP:
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv LTE redirect failed");

      if (hdralmp_hdr_to_lte_redir_is_in_progress())
      {
         hdralmp_clear_hdr_to_lte_redir(); /* Clear all the redirect related info upon failure */
        /* If HDR is deactivated during D2L redirect, HDR aborts D2L first.
         * After receiving the abort confirmation back from LTE, HDR continues
         * to deactivate itself. However due to race condition, d2l redir failure
           can come before d2l abort rsp. */ 
        ind_data.irat_meas_fail_reason = HDRIND_IRAT_MEAS_HARD_FAILURE;
        /* if failed, give report to DS */
        hdrind_give_ind( HDRIND_LMAC_ORAT_MEAS_FAILED, (void *)&ind_data );		 
        if ( hdrmc.pending_deact_info.deactivation_is_pending )
        {
          HDR_MSG_PROT (MSG_LEGACY_HIGH, "deactivation_is_pending when LTE redirect failed is rcvd");
          hdralmp_deactivate( hdrmc.pending_deact_info.deactivation_reason,
                              hdrmc.pending_deact_info.explicit_mmoc_deactivate,
                              hdrmc.pending_deact_info.deactivation_trans_id,
                              HDRHAI_HDRMC );
          memset(&hdrmc.pending_deact_info, 0, sizeof(hdrmc_pending_deact_info_type));
        }
        else
        {
     /* Reacquire HDR */
       HDR_MSG_PROT (MSG_LEGACY_HIGH, "reacquiring when D2L failed is rcvd");
       hdralmp_reacquire( HDRHAI_ABORT_D2L_FAILURE, HDRHAI_HDRMC);
        }
      }
      else
      {
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, 
                      "Ignoring redirect failed response from LTE. pending deactivation %d",
                      hdrmc.pending_deact_info.deactivation_is_pending);          
      }
      break;

    /*-------------------------------------------------------------------------
    LTE Reselect abort response
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_ABORT_REDIR_RSP:
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv D2L redirect abort response");
      
      /* HDR to LTE redirect aborted */

      if (hdralmp_hdr_to_lte_redir_is_in_progress())
      {
        hdralmp_clear_hdr_to_lte_redir(); /* Clear all the redirect related info upon abort confirm */
        /* If HDR is deactivated during D2L reselection, HDR aborts D2L first.
         * After receiving the abort confirmation back from LTE, HDR continues
         * to deactivate itself. */ 
        hdralmp_clr_hdr_to_lte_redir_abort_cmd_is_sent();
        if ( hdrmc.pending_deact_info.deactivation_is_pending )
        {
          hdralmp_deactivate( hdrmc.pending_deact_info.deactivation_reason,
                              hdrmc.pending_deact_info.explicit_mmoc_deactivate,
                              hdrmc.pending_deact_info.deactivation_trans_id,
                              HDRHAI_HDRMC );
          memset(&hdrmc.pending_deact_info, 0, sizeof(hdrmc_pending_deact_info_type));
        }
        else
        {
          /* Reacquire HDR */
          hdralmp_reacquire( HDRHAI_ABORT_D2L_FAILURE, HDRHAI_HDRMC);
        }
      }
      else
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH, 
               "Ignoring abort response from LTE. ALMP not in suspend state");
      }
      break;
    /*-------------------------------------------------------------------------
    Abort LTE redirect request from CM
    -------------------------------------------------------------------------*/
    /*
      case HDRMC_CM_ABORT_LTE_REDIR_REQ:
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv abort LTE redir req from CM");

      hdralmp_send_redirect_abort_req_to_lte();

      break;
     */

    case HDRMC_LTE_RRC_ACQ_DB_EARFCN_RSP:
        HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv LTE RRC ACQ DB EARFCN RSP");
        hdralmp_update_earfcn_plmnid_info(&cmd_ptr->msgr_cmd.msg.lte_rrc_acq_db_earfcn_rsp); /* D2L redirection */
        break;
        
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */


#ifdef FEATURE_LTE_TO_HDR_OH

    case HDRMC_LTE_RRC_HDR_SIB8_INFO_IND:
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Online rcv LTE SIB8 info" );
      hdrovhd_update_sib8_info( &cmd_ptr->msgr_cmd.msg.sib8_info, TRUE );
      break;

    case HDRMC_LTE_RRC_HDR_PRE_REG_INFO_IND:
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Online rcv LTE pre-reg info" );
      hdrovhd_update_prereg_info( &cmd_ptr->msgr_cmd.msg.pre_reg_info, TRUE );
      break;

    case HDRMC_LTE_ATTACH_COMPLETE_IND:
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Online rcv LTE attach complete ind" );
      hdrsap_stop_backoff_timer();
      break;

    case HDRMC_LTE_ACTIVE_HO_REQ:
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv LTE active ho req");
      hdrmc_process_lte_to_hdr_active_ho_cmd(cmd_ptr);
      break;

#endif /* FEATURE_LTE_TO_HDR_OH */

    /*-------------------------------------------------------------------------
    Update LTE FPLMN list
    -------------------------------------------------------------------------*/
    case HDRMC_CM_LTE_FPLMN_UPDATE_REQ:
      hdrovhd_update_lte_fplmn( cmd_ptr->msgr_cmd.msg.cm_lte_fplmn.plmn_list_size,
                                cmd_ptr->msgr_cmd.msg.cm_lte_fplmn.plmn, TRUE );
    break;

#ifdef FEATURE_LTE_DEPRIORITIZATION
    /*-------------------------------------------------------------------------
    Update LTE deprioritized frequency
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_DEPRI_FREQ_REQ:
      hdrovhd_update_lte_depri_freq( &(cmd_ptr->msgr_cmd.msg.lte_depri_freq_req), TRUE );
    break;
#endif /* FEATURE_LTE_DEPRIORITIZATION */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
    case HDRMC_MODEM_CONFIG_REFRESH:
      if ( hdrsap_tunnel_mode_is_enabled(HDRHAI_HDRMC) )
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "Online rcv NV refresh cmd, in tunnel mode.");
		
        hdrmc_process_modem_config_refresh( 
           cmd_ptr->msgr_cmd.msg.modem_config_refresh.reference_id );
		
		if ( FALSE == hdrscp_is_irat_enabled() )/* If DO is in tunnel mode and IRAT NV is not enabled after REFRESH. 
                                                   Then deactivate HDR, disable tunnel mode and move to INACTIVE */
		{		
		    HDR_MSG_PROT( MSG_LEGACY_HIGH, "IRAT is not enabled. Moving to inactive" );                 
			hdrsap_deactivate( HDRHAI_HDRMC, HDRSAP_DEACT_ACQ_HDR );   /* In this case we can reuse HDRSAP_DEACT_ACQ_HDR
			                                                              to just deactivate ALMP, and SAP. Tunnel mode also gets disabled */
			(void) rex_set_sigs( rex_self(), HDRMC_EXIT_SIG );  
			hdrind_give_ind( HDRIND_ALMP_EXIT_HDR_MODE , NULL );
		}  		
      }
      else
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                      "Online rcv NV refresh cmd, report failure" );

        mcfg_refresh_done_w_status( cmd_ptr->msgr_cmd.msg.modem_config_refresh.reference_id, 
                                    MCFG_REFRESH_STATUS_SUB_IS_ACTIVE );
      }
    break;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
 /*-------------------------------------------------------------------------
    TRM priority change command for QCHAT
    -------------------------------------------------------------------------*/
   case HDRMC_CM_TRM_PRIORITY_IND:		
		
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Online rcv TRM priority indication with %d",
			                               cmd_ptr->msgr_cmd.msg.cm_trm_prioirty.trm_priority_high);
        if(cmd_ptr->msgr_cmd.msg.cm_trm_prioirty.trm_priority_high == TRUE)
        {
          hdrlmac_set_tc_mode(HDRSRCH_DISABLE_TA_TC_MODE);
        }
        else
        {
          hdrlmac_set_tc_mode(HDRSRCH_ENABLE_TA_TC_MODE);		  
        }
	
        /* this is to make sure pending tc mode is not acted upon, just when 
           TC mode cmd is to be set in ONLINE mode */
        hdrmc.tc_mode_info.pending = FALSE;
	
	break;

	
  /*-------------------------------------------------------------------------
	PSM Ready Request Command
	-------------------------------------------------------------------------*/
    case HDRMC_CM_CHECK_PSM_READY_REQ:
		HDR_MSG_PROT (MSG_LEGACY_HIGH, "Online rcv PSM Ready Request");
	    hdralmp_process_psm_req();
    break;

    /*-------------------------------------------------------------------------
    Unknown Command
    -------------------------------------------------------------------------*/
    default:
      
      ERR( "Invalid HDRMC cmd %d", cmd_ptr->hdr.cmd, 0, 0 );
      cmd_ptr->hdr.status = HDRMC_BADPARM_S;
    
      break;
  } /* end of switch */

} /* hdrmc_online_process_cmd() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION hdrmc_exit_online

DESCRIPTION
  This function does the exit online processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Reason for exit.

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_exit_online( void )
{

  hdrmc_buf_type *hdrmc_buf_ptr;
  hdrhmp_send_msg_params_type *send_msg_ptr; /* in case a tx msg was left */
  rex_tcb_type *rx_tcb_ptr; /* temp ptr */
  rex_tcb_type *tx_tcb_ptr; /* temp ptr */
#ifdef FEATURE_HDR_BCMCS
  rex_tcb_type *bc_tcb_ptr; /* temp ptr */
#endif /* FEATURE_HDR_BCMCS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT (MSG_LEGACY_HIGH, "Exit HDRMC Online State");

#ifdef FEATURE_EHRPD
  hdrscp_set_ehrpd_no_check(FALSE); /* Reset flag when HDR is inactive */
#endif

  /* Return all internal and external command queue buffers to the free queues
  * and clear their signals.
  */
  while ( ( hdrmc_buf_ptr = ( hdrmc_buf_type * ) q_get( &hdrmc.event_q ))
              != NULL)
  {
    switch(hdrmc_buf_ptr->hdr.type_of_item)
    {
      case HDRBUF_COMMAND:
        ERR( "CMD left on event_q! p:0x%x c:%d",
          hdrmc_buf_ptr->hdr.protocol_name,
          hdrmc_buf_ptr->item.cmd.almp.name, 0);
        break;
      case HDRBUF_INDICATION:
        /* Note that the reason we print the link_id is because
           ind_data is a union & we cannot directly print the ind_data. Since
           this field is uint32, we will print out the first 8 bits in the
           indication data. This data can be any of the indication data in
           the union, not necessarily the link_id
        */
        ERR( "IND left on event_q! p:0x%x i:%d d:0x%d",
          hdrmc_buf_ptr->hdr.protocol_name,
          hdrmc_buf_ptr->item.ind.ind_name,
          (uint32)hdrmc_buf_ptr->item.ind.ind_data.link_id);
        break;
      case HDRBUF_MESSAGE:
        ERR( "MSG left on event_q! p:0x%x i:0x%x m:0x%x",
          hdrmc_buf_ptr->hdr.protocol_name,
          hdrmc_buf_ptr->hdr.protocol_instance,
          (hdrmc_buf_ptr->item.item_ptr &&
           hdrmc_buf_ptr->item.item_ptr->data_ptr) ?
           *(hdrmc_buf_ptr->item.item_ptr->data_ptr) : 0);
        dsm_free_packet( &hdrmc_buf_ptr->item.item_ptr );
        break;
      default:
        break;
    }

    if ((hdrmc_buf_ptr->hdr.type_of_item  == HDRBUF_COMMAND) &&
        (hdrmc_buf_ptr->hdr.protocol_name == HDRHAI_HDR_MESSAGING_PROTOCOL) &&
        (hdrmc_buf_ptr->item.cmd.hmp.name == HDRHMP_SEND_MSG_CMD))
    {
      hdrhmp_msgs_on_event_q( -1 );
      send_msg_ptr = &hdrmc_buf_ptr->item.cmd.hmp.params.send_msg;
      /* Call back to the sender to tell them about this failure */
      if (send_msg_ptr->outcome_cb != NULL)
      {
        ( *send_msg_ptr->outcome_cb )
        ( send_msg_ptr->outcome_tag_ptr, E_FAILURE );
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "Failing msg tx pid:%d",
                        send_msg_ptr->protocol_name);

        /* Dig the message ID out of the DSM item, for debugging */
        if (send_msg_ptr->msg_ptr)
        {
          if ((send_msg_ptr->msg_ptr->used > 1) &&
              (send_msg_ptr->msg_ptr->data_ptr))
          {
            HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "mid%d",
                            *(send_msg_ptr->msg_ptr->data_ptr));
          }
          else if (( send_msg_ptr->msg_ptr->used == 1 ) &&
                   ( send_msg_ptr->msg_ptr->pkt_ptr ) &&
                   ( send_msg_ptr->msg_ptr->pkt_ptr->data_ptr ))
          {
            /* This is a dup, so MSG_ID is first byte of pkt_ptr->data_ptr */
            HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "mid%d",
                            *(send_msg_ptr->msg_ptr->pkt_ptr->data_ptr));
          }
        }
      }
      dsm_free_packet( &send_msg_ptr->msg_ptr );
    }
    else if ( (hdrmc_buf_ptr->hdr.type_of_item == HDRBUF_COMMAND) &&
              (hdrmc_buf_ptr->hdr.protocol_name == HDRHAI_HDR_MESSAGING_PROTOCOL) &&
              (hdrmc_buf_ptr->item.cmd.hmp.name == HDRHMP_RASM_CB_CMD) )
    {
      /* HDR is deactivated while there are still RASM message in the SLP queue,
       * RASM callback is called in HDRTX and queued to HDRMC task. We need to
       * execute these callbacks to reset RASM states. 
       * RASM callbacks queued after this loop will be queued in HDRMC queue 
       * until HDR goes online next time. Those callbacks will be executed 
       * before HDR acquires network so that RASM will be in correct state. */ 
      hdrhmp_process_cmd(&hdrmc_buf_ptr->item.cmd.hmp);
    }

    modem_mem_free( hdrmc_buf_ptr , MODEM_MEM_CLIENT_HDR_CRIT ); /* event queue free */
    HDR_MSG_PROT_1( MSG_LEGACY_LOW, "Buf ptr free = 0x%x.", hdrmc_buf_ptr );
  }
  ( void ) rex_clr_sigs( HDRMC_TASK_PTR, HDRMC_EVENT_Q_SIG );

  /* Tell HDR Rx and Tx tasks to exit HDR mode */
  rx_tcb_ptr = rcinit_lookup_rextask("hdrrx");
  tx_tcb_ptr = rcinit_lookup_rextask("hdrtx");
#ifdef FEATURE_HDR_BCMCS
  bc_tcb_ptr = rcinit_lookup_rextask("hdrbc");
#endif /* FEATURE_HDR_BCMCS */

  ( void ) rex_set_sigs( rx_tcb_ptr, HDRRX_EXIT_HDR_SIG );
  ( void ) rex_set_sigs( tx_tcb_ptr, HDRTX_EXIT_HDR_SIG );
#ifdef FEATURE_HDR_BCMCS
  ( void ) rex_set_sigs( bc_tcb_ptr, HDRBC_EXIT_HDR_SIG );
#endif

#ifdef FEATURE_HDR_HYBRID
  if(sd_misc_is_hybr_opr())
  {
    hdrmc.type_of_exit = HDRMC_EXIT_HYBR_POWERSAVE;
    HDRTRACE_HYBRID_POWER_SAVE();
  }
#endif /* FEATURE_HDR_HYBRID */

  /* Write any NV items left on the queue */
  (void) rex_clr_sigs(HDRMC_TASK_PTR, HDRMC_NV_WRITE_SIG);
  while (hdrutil_nv_q_srv())
  {
    (void)hdrmc_wait(HDRMC_NV_WRITE_SIG);
    (void) rex_clr_sigs(HDRMC_TASK_PTR, HDRMC_NV_WRITE_SIG);
  }

  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Type of exit=%d",hdrmc.type_of_exit);
} /* hdrmc_exit_online */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_ENTER_ONLINE

DESCRIPTION
  This function is the main loop for HDR protocol operation.  It handles
  commands, indications, and messages for protocols running within the HDRMC
  task.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Reason for exit.

SIDE EFFECTS
  None

===========================================================================*/

hdrmc_state_type hdrmc_enter_online( void )
{
  rex_sigs_type sigs;                     /* Signals returned by hdrmc_wait */
  rex_sigs_type wait_mask;                /* Mask of signals to wait on */
   hdrmc_msg_type *cmd_ptr;     /* HDRMC Commands from other tasks  */

  hdrmc_buf_type *hdrmc_buf_ptr;
    /* Command, Indication, or Message for a protocol in HDRMC task */
  boolean exit_hdr_mode = FALSE;
    /* Flag to leave HDR mode */
  hdrmc_state_type     next_state=HDRMC_ONLINE_STATE;
    /* Next state of HDRMC Mode Controller  */

  rex_tcb_type *rx_tcb_ptr; /* temp ptr */
  rex_tcb_type *tx_tcb_ptr; /* temp ptr */
#ifdef FEATURE_HDR_BCMCS
  rex_tcb_type *bc_tcb_ptr; /* temp ptr */
#endif /* FEATURE_HDR_BCMCS */
#ifdef FEATURE_MMGSDI
  mmgsdi_session_info_query_type mcmmgsdi_query;
  mmgsdi_session_get_info_type	 mcmmgsdi_get_session_info;
#endif /* FEATURE_MMGSDI */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrmc.type_of_exit = HDRMC_EXIT_NORMAL;

  HDR_MSG_PROT (MSG_LEGACY_HIGH, "Enter HDRMC Online State");

#ifdef FEATURE_JCDMA
  hdrmc_set_feature_jcdma(TRUE);
#else
  hdrmc_set_feature_jcdma(FALSE);
#endif

  /* Tell HDR Rx and Tx tasks to enter HDR mode */
  rx_tcb_ptr = rcinit_lookup_rextask("hdrrx");
  tx_tcb_ptr = rcinit_lookup_rextask("hdrtx");
#ifdef FEATURE_HDR_BCMCS
  bc_tcb_ptr = rcinit_lookup_rextask("hdrbc");
#endif /* FEATURE_HDR_BCMCS */

  ( void ) rex_set_sigs( rx_tcb_ptr, HDRRX_ENTER_HDR_SIG );
  ( void ) rex_set_sigs( tx_tcb_ptr, HDRTX_ENTER_HDR_SIG );
#ifdef FEATURE_HDR_BCMCS
  ( void ) rex_set_sigs( bc_tcb_ptr, HDRBC_ENTER_HDR_SIG );
#endif

  /* Initialize all protocols that require it, including those in other tasks.
   * Since other task protocols are inactive, there are no race conditions.
   */

  hdrhmp_init();
  hdrovhd_init();
  hdrinit_init();
  hdridle_init();
  hdrrup_init();

  hdrcallid_init();
  
  /* Delete the db information file stored on the EFS, once AT enter online */
  hdrscmdb_delete();

#ifdef FEATURE_HDR_HYBRID
  if(sd_misc_is_hybr_opr())
  {
    /* recompute the preferred control channel cycle so
    that it will not collide with the CDMA idle paging slot */
    hdridle_update_pref_cntl_chan_cycle(HDRHAI_HDRMC);
  }
#endif /*  FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_LTE_TO_HDR_OH
  /* This is the only protocol command that is issued from outside the
   * protocol stack, so for logging we just say that SAP or ALMP is
   * activating itself */
  if ( hdrsap_tunnel_mode_is_enabled(HDRHAI_HDRMC) )
  {
    /* When tunnel mode is enabled we first activate SAP which in turn activates 
       ALMP */
    hdrsap_activate( HDRHAI_HDRMC );
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    /* Start HDR operation by activating the Air Link Management Protocol */
    hdralmp_activate( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
#ifdef FEATURE_LTE_TO_HDR_OH
    hdramp_set_entering_tunnel( FALSE );
#endif /* FEATURE_LTE_TO_HDR_OH */
  }

#ifdef FEATURE_MMGSDI
  /* Query the slot id for hdr primary session id */
  mcmmgsdi_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_ID;
  mcmmgsdi_query.query_key.session_id = hdrmc.pri_session_id;
  if (mmgsdi_session_get_info_sync( mcmmgsdi_query,  &mcmmgsdi_get_session_info ) == MMGSDI_SUCCESS)
  {
	HDR_MSG_PROT(MSG_LEGACY_HIGH, " mmgsdi_session_get_info_sync returned success");
	hdrmc.pri_slot_id = mcmmgsdi_get_session_info.slot_id;
  }
  else
  {
	HDR_MSG_PROT(MSG_LEGACY_HIGH, " mmgsdi_session_get_info_sync returned failure");
	hdrmc.pri_slot_id = 0;
  }
  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "slot id returned after mmgsdi_session_get_info_sync:%d", hdrmc.pri_slot_id);
  
#endif /* FEATURE_MMGSDI */

hdrmc_set_hdr_exiting(FALSE);

  if(hdrmc.tc_mode_info.pending == TRUE)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "setting pending TC MODE now to :%d", hdrmc.tc_mode_info.mode);
    hdrlmac_set_tc_mode(hdrmc.tc_mode_info.mode);
    hdrmc.tc_mode_info.pending = FALSE;
  }

  while ( !exit_hdr_mode )
  {
    /* Wait for a signal and process one per loop.  This ensures that
     * If the processing of one signal clears another, the cleared signal
     * will not be processed.
     */
    wait_mask = (HDRMC_CMD_Q_SIG   |
                HDRMC_NV_WRITE_SIG |
                HDRMC_EVENT_Q_SIG |
                HDRMC_EXIT_SIG |
                HDRMC_IDLE_MONITOR_ENTERED_SIG |
                HDRMC_SMP_KA_TIMER_SIG |
                HDRMC_SCP_CONFIG_RSP_TIMER_SIG |
                HDRMC_SCP_AN_INIT_STATE_TIMER_SIG |
                HDRMC_STREAM_CONFIG_RSP_TIMER_SIG |
                HDRMC_KEP_KEYGEN_DONE_SIG |
                TASK_OFFLINE_SIG |
                TASK_STOP_SIG );

    sigs = hdrmc_wait( wait_mask );
    HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "Processing sig(s) %lx", sigs);

    if ( ( sigs & TASK_STOP_SIG ) != 0 )
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR, TASK_STOP_SIG );
      exit_hdr_mode = TRUE;
      hdrmc.type_of_exit = HDRMC_EXIT_POWER_DOWN;

#ifdef FEATURE_HDR_MC_MSGR
      hdrmc_msgr_stop();
#endif /* FEATURE_HDR_MC_MSGR */

#ifndef FEATURE_MODEM_RCINIT
      tmc_task_stop();
#endif /* FEATURE_MODEM_RCINIT */
    }              /* end TASK_STOP_SIG */
    else if ( ( sigs & HDRMC_EVENT_Q_SIG ) != 0 )
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR, HDRMC_EVENT_Q_SIG );

      hdrmc_buf_ptr = ( hdrmc_buf_type * ) q_get( &hdrmc.event_q );
      ASSERT(hdrmc_buf_ptr);
      hdrmc_protocol_item( hdrmc_buf_ptr );
      modem_mem_free( hdrmc_buf_ptr , MODEM_MEM_CLIENT_HDR_CRIT ); /* event queue free */
      HDR_MSG_PROT_1( MSG_LEGACY_LOW, "Buf ptr free = 0x%x.", hdrmc_buf_ptr );
    }              /* end HDRMC_EVENT_Q_SIG */
    else if ( ( sigs & TASK_OFFLINE_SIG ) != 0 )
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR, TASK_OFFLINE_SIG );
      exit_hdr_mode = TRUE;
      hdrmc.type_of_exit = HDRMC_EXIT_OFFLINE;
#ifndef FEATURE_MODEM_RCINIT
      tmc_task_offline();
#endif /* FEATURE_MODEM_RCINIT */
    }              /* end TASK_OFFLINE_SIG */
    else if ( ( sigs & HDRMC_CMD_Q_SIG ) != 0 )
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR, HDRMC_CMD_Q_SIG );
      cmd_ptr = ( hdrmc_msg_type * ) q_get( &hdrmc.cmd_q );

      ASSERT(cmd_ptr);
      hdrmc_online_process_cmd( cmd_ptr );
      modem_mem_free( cmd_ptr, MODEM_MEM_CLIENT_HDR_CRIT );
      HDR_MSG_PROT_1( MSG_LEGACY_LOW, "Cmd ptr free = 0x%x.", cmd_ptr );
    }              /* end HDRMC_CMD_Q_SIG */
    else if ( ( sigs & HDRMC_IDLE_MONITOR_ENTERED_SIG ) != 0 )
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR, HDRMC_IDLE_MONITOR_ENTERED_SIG );
      hdridle_process_sig( HDRMC_IDLE_MONITOR_ENTERED_SIG );
    }              /* end HDRMC_IDLE_MONITOR_ENTERED_SIG */
    else if ( ( sigs & HDRMC_SMP_KA_TIMER_SIG ) != 0 )
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR, HDRMC_SMP_KA_TIMER_SIG );
      hdrsmp_process_sig( HDRMC_SMP_KA_TIMER_SIG );
    }              /* end HDRMC_SMP_KA_TIMER_SIG */
    else if ( ( sigs & HDRMC_SCP_CONFIG_RSP_TIMER_SIG ) != 0 )
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR, HDRMC_SCP_CONFIG_RSP_TIMER_SIG );
      hdrscp_process_sig( HDRMC_SCP_CONFIG_RSP_TIMER_SIG );
    }              /* end HDRMC_SCP_CONFIG_RSP_TIMER_SIG */
    else if ( ( sigs & HDRMC_SCP_AN_INIT_STATE_TIMER_SIG ) != 0 )
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR,
                             HDRMC_SCP_AN_INIT_STATE_TIMER_SIG );
      hdrscp_process_sig( HDRMC_SCP_AN_INIT_STATE_TIMER_SIG );
    }              /* end HDRMC_SCP_AN_INIT_STATE_TIMER_SIG */
    else if ( ( sigs & HDRMC_STREAM_CONFIG_RSP_TIMER_SIG ) != 0)
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR,
                             HDRMC_STREAM_CONFIG_RSP_TIMER_SIG );

      hdrstream_process_sig( HDRMC_STREAM_CONFIG_RSP_TIMER_SIG );
    }              /* end HDRMC_STREAM_CONFIG_RSP_TIMER_SIG */
    else if ( ( sigs & HDRMC_KEP_KEYGEN_DONE_SIG ) != 0)
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR,
                             HDRMC_KEP_KEYGEN_DONE_SIG );

      hdrkep_process_sig( HDRMC_KEP_KEYGEN_DONE_SIG );
    }              /* end HDRMC_KEP_KEYGEN_DONE_SIG */
    else if ( sigs & HDRMC_NV_WRITE_SIG )
    {
      (void) rex_clr_sigs(HDRMC_TASK_PTR, HDRMC_NV_WRITE_SIG);
      (void) hdrutil_nv_q_srv();
    }
    else if ( ( sigs & HDRMC_EXIT_SIG ) != 0 )
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR, HDRMC_EXIT_SIG );
      exit_hdr_mode = TRUE;
      /* hdrmc.type_of_exit set before HDRMC_EXIT_SIG was set */
    }              /* end HDRMC_EXIT_SIG */

  } /* end of while */


  /* perform the exit processing */
  hdrmc_exit_online();

  /* currently only online and inactive state */
  next_state = HDRMC_INACTIVE_STATE;

  return (next_state);
} /* hdrmc_enter_online() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMC_INACTIVE_PROCESS_CMD

DESCRIPTION
  This function processes a command from MMOC while in the inactive state.

DEPENDENCIES
  None

PARAMETERS
  cmd_ptr - A pointer to the command received from MMOC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

hdrmc_state_type hdrmc_inactive_process_cmd( hdrmc_msg_type * cmd_ptr )
{
  hdrmc_state_type     next_state; /* Next state of HDRMC Mode Controller  */
  sd_ss_act_e_type     sd_action;  /* action returned from SD */
#ifdef FEATURE_HDR_DEBUG_INFO
  prot_trans_type trans_id=0;
#endif /*  FEATURE_HDR_DEBUG_INFO */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize next state of HDRMC */
  next_state = HDRMC_INACTIVE_STATE;
  cmd_ptr->hdr.status = HDRMC_DONE_S;

  #ifdef FEATURE_HDR_DEBUG_INFO
	HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "Rxed command %d",cmd_ptr->hdr.cmd);
  
	if(hdrmc_get_transaction_id(&trans_id,cmd_ptr) != TRUE)
	{
      hdrmc_log_rcvd_rpt(trans_id,cmd_ptr->hdr.cmd );
	}
	else
	{
      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Command ignored");
	}  
#endif /*  FEATURE_HDR_DEBUG_INFO */

  switch ( cmd_ptr->hdr.cmd )
  {
#ifndef FEATURE_HDR_BOLT_MODEM
    /*-------------------------------------------------------------------------
    Offline Command
    -------------------------------------------------------------------------*/
    case MM_MMOC_OFFLINE_IND:
      HDR_MSG_PROT (MSG_LEGACY_HIGH,  "Rxed Offline Cmd");
      hdrmc.type_of_exit = HDRMC_EXIT_OFFLINE;
      break;
#endif /* FEATURE_HDR_BOLT_MODEM */

    /*-------------------------------------------------------------------------
    Activate Command
    -------------------------------------------------------------------------*/
    case HDRMC_PROT_ACT_REQ:
      
      /* Activate the CDMA protocol based on activate reason */
      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Inact rcv prot act reason=%d",
                      cmd_ptr->msgr_cmd.msg.activate_protocol.act_reason);
      next_state = hdrmc_inact_proc_prot_activate(
         cmd_ptr->msgr_cmd.msg.activate_protocol.act_reason);
    
    break;

    /*-------------------------------------------------------------------------
    Deactivate Command
    -------------------------------------------------------------------------*/
    case HDRMC_PROT_DEACT_REQ:
      /* Protocol already in deactivated state, send deactivated
      ** confirmation anyways.
      */
      HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, 
                  "Inact rcv prot deact reason=%d. Inact send DeactCnf id=%d",
                   cmd_ptr->msgr_cmd.msg.deactivate_protocol.deact_reason,
                      cmd_ptr->msgr_cmd.msg.deactivate_protocol.trans_id);

      mmoc_rpt_prot_deactd_cnf(
        cmd_ptr->msgr_cmd.msg.deactivate_protocol.trans_id );
#ifdef FEATURE_HDR_DEBUG_INFO
      hdrmc_log_sent_rpt(cmd_ptr->msgr_cmd.msg.deactivate_protocol.trans_id, HDR_MMOC_RPT_PROT_DEACTD_CNF);
#endif /* FEATURE_HDR_DEBUG_INFO */
    break;

    /*-------------------------------------------------------------------------
    Phone Status Changed Command
    -------------------------------------------------------------------------*/
    case HDRMC_PROT_PH_STAT_CHGD_REQ:
       /* Process the HDR phone status command.
       */
      HDR_MSG_PROT_3 (MSG_LEGACY_HIGH,
                       "Inact rcv PhStatChg type=%d oprtMode=%d id=%d",
                      cmd_ptr->msgr_cmd.msg.ph_status_chgd.chg_type,
                      cmd_ptr->msgr_cmd.msg.ph_status_chgd.oprt_mode,
                      cmd_ptr->msgr_cmd.msg.ph_status_chgd.trans_id);
      
	  /* Reset the NV flag, to track any NV errors during process of this command */
	  hdrutil_reset_nv_error();
	  
      /* If the phone status changed due to a change in subscription or NAM,
         (for OTASP updates) read the ESN and the NAI to see if they have  
         changed wrt the one stored for the current session.
         Also update the HDRACMAC's persistence index. */
      if ( ((cmd_ptr->msgr_cmd.msg.ph_status_chgd.chg_type == PROT_PH_STAT_CHG_SUBSC) ||
            (cmd_ptr->msgr_cmd.msg.ph_status_chgd.chg_type == PROT_PH_STAT_CHG_NAM)) &&
           (cmd_ptr->msgr_cmd.msg.ph_status_chgd.is_cdma_subs_avail == 1) )
      {
        hdramp_update_esn(HDRHAI_HDRMC);
        hdramp_update_nai(HDRHAI_HDRMC);
#ifdef FEATURE_EHRPD
        hdrscp_update_ehrpd_credentials(HDRHAI_HDRMC);
#endif /* FEATURE_EHRPD */
        hdramac_update_persist_index();
      }
      if (cmd_ptr->msgr_cmd.msg.ph_status_chgd.chg_type == PROT_PH_STAT_CHG_OPRT_MODE)
      {
        switch( cmd_ptr->msgr_cmd.msg.ph_status_chgd.oprt_mode )
        {
          case PROT_OPRT_MODE_PWROFF:
          case PROT_OPRT_MODE_OFFLINE:
          case PROT_OPRT_MODE_OFFLINE_CDMA:
          case PROT_OPRT_MODE_LPM:
          case PROT_OPRT_MODE_RESET:

            /* It is possible that we receive PhoneStatusChange with LPM reason
             * during powerup. To avoid writing NV or save session config to
             * EFS during powerup, we check whether the SCMDB copy of session 
             * config is consistent with the EFS copy, and write to NV/EFS 
             * accordingly. */ 
            if ( hdrscmdb_efs_is_up_to_date() )
            {
              HDR_MSG_PROT( MSG_LEGACY_MED, 
                            "Offline PhoneStatusChanged: ignore writing to EFS, as it's already stored" );
            }
            else
            {
              hdrsmpka_save_to_nv();
              hdrscp_power_down();
              (void) hdrscmdb_write_all_config_data();
              HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                            "Offline PhoneStatusChanged: save session config successfully" );
            }
            break;
  
          default:
            break;
        }
      }
      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Inact send phStatChgdCnf id=%d",
                       cmd_ptr->msgr_cmd.msg.ph_status_chgd.trans_id);					   
						   
      /* Check if there is any NV read error */
      if(hdrutil_get_nv_error() == TRUE)
      {
         HDR_MSG_PROT(MSG_LEGACY_HIGH," NV read error during Phone Status Change command");
         /* Acknowledge the Phone Status Change command with MMOC */       
         mmoc_rpt_ph_stat_chgd_cnf( cmd_ptr->msgr_cmd.msg.ph_status_chgd.trans_id,
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
	                             SD_MODE_HDR, MMOC_PH_STAT_ERR_NV_ERR);
#else
                                 SD_MODE_HDR);
#endif
      }
      else
      {	  
         /* Acknowledge the Phone Status Change command with MMOC */		 
      mmoc_rpt_ph_stat_chgd_cnf( cmd_ptr->msgr_cmd.msg.ph_status_chgd.trans_id,
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
                                  SD_MODE_HDR, MMOC_PH_STAT_NO_ERROR );
#else
                                  SD_MODE_HDR );
#endif
      }	  
#ifdef FEATURE_HDR_DEBUG_INFO
      hdrmc_log_sent_rpt(cmd_ptr->msgr_cmd.msg.ph_status_chgd.trans_id, HDR_MMOC_RPT_PH_STAT_CHGD_CNF);
#endif /* FEATURE_HDR_DEBUG_INFO */
    break;

    /*-------------------------------------------------------------------------
    Generic MMoC Command
    -------------------------------------------------------------------------*/
    case HDRMC_PROT_GEN_REQ:
      /* Call protocol generic command proc, with active flag
      ** set to FALSE
      */
#ifdef FEATURE_HDR_DEBUG_INFO
	  hdrmc_log_sent_rpt(cmd_ptr->msgr_cmd.msg.generic_prot.info.trans_id, HDR_MMOC_RPT_PROT_GEN_CMD);
#endif /* FEATURE_HDR_DEBUG_INFO */
      sd_action = mmoc_proc_prot_gen_cmd( TRUE, FALSE,
                 &cmd_ptr->msgr_cmd.msg.generic_prot.info);

      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,"Inact rcv ProcGenCmd sd=%d",sd_action);

      if(sd_action ==  SD_SS_ACT_ACQ_HDR )
      {
        next_state = HDRMC_ONLINE_STATE;
      }
    break;

    /*-------------------------------------------------------------------------
    Origination Command
    -------------------------------------------------------------------------*/
    case HDRMC_ORIGINATION_REQ:

      hdrcallid_set_cur_call_id(cmd_ptr->msgr_cmd.msg.origination.call_id);
      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Orig call id=%d",
                      cmd_ptr->msgr_cmd.msg.origination.call_id);

      hdrmc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT);

    break;

    /*-------------------------------------------------------------------------
    Broadcast Update Flow Request List Command
    -------------------------------------------------------------------------*/
    case HDRMC_BCMCS_UPDATE_FLOW_REQ:
    case HDRMC_BCMCS_LEGACY_UPDATE_FLOW_REQ:
#ifdef FEATURE_HDR_BCMCS
      HDR_MSG_PROT(MSG_LEGACY_HIGH,"Inact rcv BCMCS UpdateFlowRequest Cmd");

#ifdef FEATURE_HDR_BCMCS_2p0      
      #error code not present
#else
      ASSERT(cmd_ptr->msgr_cmd.msg.bcmcs_update_flow_legacy.num_of_flows <=
             HDRMC_BCMCS_MAX_FLOWS);
      hdrbccp_update_flow_request ( 
         HDRHAI_HDRMC,
         cmd_ptr->msgr_cmd.msg.bcmcs_update_flow_legacy.transaction_id,
         cmd_ptr->msgr_cmd.msg.bcmcs_update_flow_legacy.num_of_flows,
         cmd_ptr->msgr_cmd.msg.bcmcs_update_flow_legacy.flow_addrs );
#endif /* FEATURE_HDR_BCMCS_2p0 */
#endif /* FEATURE_HDR_BCMCS */
    break;


    /*-------------------------------------------------------------------------
    Discontinue Broadcast Command
    -------------------------------------------------------------------------*/
    case HDRMC_BCMCS_DISCONTINUE_BROADCAST_REQ:
    case HDRMC_BCMCS_LEGACY_DISCONTINUE_BROADCAST_REQ:
#ifdef FEATURE_HDR_BCMCS

      HDR_MSG_PROT(MSG_LEGACY_HIGH,"Inact rcv BCMCS DiscontinueBroadcast Cmd");

      hdrbccp_discontinue_broadcast ( 
        HDRHAI_HDRMC,
#ifdef FEATURE_HDR_BCMCS_2p0          
        #error code not present
#else
        cmd_ptr->msgr_cmd.msg.bcmcs_discontinue_bc_legacy.transaction_id                                    );
#endif /* FEATURE_HDR_BCMCS_2p0 */
#endif /* FEATURE_HDR_BCMCS */
    break;

    /*-------------------------------------------------------------------------
    Commands Ignored in Inactive State
    -------------------------------------------------------------------------*/
    case HDRMC_END_REQ:
    case HDRMC_IDLE_PGSLOT_CHANGED_REQ:
    case HDRMC_UNLOCK_HDR_RF_REQ:
    case HRDMC_TUNNEL_MSG_REQ:
      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "HDRMC Inactive: Ignore cmd %d",
                      cmd_ptr->hdr.cmd);
    break;

    /*-------------------------------------------------------------------------
    CSNA Configuration Command
    -------------------------------------------------------------------------*/
    case HDRMC_CSNA_MSG_CONFIG_REQ:
      #ifdef FEATURE_HDR_CSNA
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "Process CSNA Msg Config Cmd");

      hdrcsna_update_msg_config ( HDRHAI_HDRMC,
                                  &cmd_ptr->msgr_cmd.msg.csna_msg_config );
      #endif /* FEATURE_HDR_CSNA */
    break;

     /*-------------------------------------------------------------------------
     Collect and report FTD stats
     -------------------------------------------------------------------------*/
    case HDRMC_SET_FTD_STATS_MASK_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "Process Collect FTD Stats Cmd");

      hdrlog_process_collect_ftd_stats( cmd_ptr->msgr_cmd.msg.ftd_stats.bit_mask );
      break;

    /*-------------------------------------------------------------------------
    BCMCS Send Registration Command
    -------------------------------------------------------------------------*/
    case HDRMC_BCMCS_SEND_REGISTRATION_REQ:
#ifdef FEATURE_HDR_BCMCS_2p0
      #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
    break;

#if defined(FEATURE_HDR_BCMCS_2p0) && defined(FEATURE_HDR_BCMCS_DYNAMIC_MODEL)
    #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 && FEATURE_HDR_BCMCS_DYNAMIC_MODEL */    

#ifdef FEATURE_HDR_QCHAT
    case HDRMC_QCHAT_REQ:
      if (hdrqchatupk_imo_enabled()) 
      {
        hdrqchatupk_mc_handle_imminent_orig();
      }
    break;
#endif /*FEATURE_HDR_QCHAT*/

#ifdef FEATURE_LTE_TO_HDR
    /*-------------------------------------------------------------------------
    LTE to HDR redirection
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_REDIRECT_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Inact rcv LTE-HDR redirect cmd");
      hdrmc_process_lte_to_hdr_redirect_cmd(cmd_ptr);

    break;

    /*-------------------------------------------------------------------------
    LTE to HDR reselection
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_RESELECT_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Inact rcv LTE-HDR reselect cmd");
      hdrmc_process_lte_to_hdr_reselect_cmd(cmd_ptr);

    break;

    /*-------------------------------------------------------------------------
    LTE to HDR redirection/reselection confirmation from MMOC
    -------------------------------------------------------------------------*/
    case HDRMC_PROT_LTE_TO_HDR_HO_CNF_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Inact rcv LTE-HDR handoff cnf cmd");
      next_state = hdrmc_process_lte_to_hdr_ho_cnf_cmd(cmd_ptr,
                                                       HDRMC_INACTIVE_STATE);

#ifdef FEATURE_LTE_TO_HDR_OH
      /* Resume tunnel activation command if it is pending */
      if ( hdrmc.tunnel_activation_is_pending && 
           next_state == HDRMC_INACTIVE_STATE )
      {
        hdrmc_set_tunnel_activation_is_pending(FALSE);
        next_state = hdrmc_inact_proc_prot_activate( PROT_ACT_LTE_HANDOVER );
      }
#endif /* FEATURE_LTE_TO_HDR_OH */
    break;

    /*-------------------------------------------------------------------------
    1X/HRPD capabilities request from LTE
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_1X_HRPD_CAPABILITIES_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Rcvd 1X/HRPD capabilities request from LTE");
      hdrmc_send_1x_hrpd_capabilities_to_lte(
        cmd_ptr->msgr_cmd.msg.lte_capabilities_req.cdma_band_preference);

    break;

    /*-------------------------------------------------------------------------
    Abort LTE to HDR redirection.
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_ABORT_REDIRECT_REQ:
    case HDRMC_LTE_ABORT_RESELECT_REQ:
    case HDRMC_LTE_ABORT_REVERSE_TIMING_TRANSFER_REQ:
    case HDRMC_LTE_ABORT_CGI_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Inactive rcv abort LTE to HDR handoff");

      mmoc_rpt_prot_auto_deactd_ind( PROT_DEACT_REDIR_ABORT, 
                                     PROT_STATE_ONLINE_HDR );
#ifdef FEATURE_HDR_DEBUG_INFO
      hdrmc_log_sent_rpt(0, HDR_MMOC_RPT_PROT_AUTO_DEACTD_IND);
#endif /* FEATURE_HDR_DEBUG_INFO */
      /* Indicate to LTE that handoff abort operation is done */
      hdrmc_send_abort_done_to_lte(FALSE);

#ifdef FEATURE_LTE_TO_HDR_OH
      /* Resume tunnel activation command if it is pending */
      if ( hdrmc.tunnel_activation_is_pending )
      {
        hdrmc_set_tunnel_activation_is_pending(FALSE);
        next_state = hdrmc_inact_proc_prot_activate( PROT_ACT_LTE_HANDOVER );
      }
#endif /* FEATURE_LTE_TO_HDR_OH */
    break;

    /*-------------------------------------------------------------------------
    Timing transfer request from LTE.
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_REVERSE_TIMING_TRANSFER_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Inactive rcv timing transfer req");
      hdrmc_process_reverse_time_transfer_req_cmd(cmd_ptr);

    break;

    /*-------------------------------------------------------------------------
    Get Cell Global Identity (CGI) from LTE.
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_GET_CGI_REQ:
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "IRAT: Inact rcv LTE-HDR get CGI cmd");
      hdrmc_process_lte_get_cgi_cmd(cmd_ptr);

      break;
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_IRAT_PILOT_MEAS
    /*-------------------------------------------------------------------------
    Activate Measurement Mode
    -------------------------------------------------------------------------*/
    case HDRMC_MEAS_MODE_ON_REQ:

      /* Activate measurement mode */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Inact rcv meas mode activated");

      /* Turn on DCM that is AT is not allowed to open a session and respond to
         messages from AN */
      hdrsmp_set_meas_mode_is_on_flag(TRUE);
      hdrsmp_dcm_deactivate();  

      /* Send confirmation to CM */
      hdrmc_send_meas_mode_cnf(SYS_MEAS_MODE_HDR_ON);
      break;

    /*-------------------------------------------------------------------------
    De-activate Measurement Mode
    -------------------------------------------------------------------------*/
    case HDRMC_MEAS_MODE_OFF_REQ:

      /* De-Activate measurement mode */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Inact rcv meas mode de-activated");

      /* Turn off DCM that is AT is allowed to open a session and respond to 
         messages from AN */
      hdrsmp_set_meas_mode_is_on_flag(FALSE); 
      hdrsmp_process_call_orig();

      /* Send confirmation to CM */
      hdrmc_send_meas_mode_cnf(SYS_MEAS_MODE_HDR_OFF);
      break;
#endif /* FEATURE_IRAT_PILOT_MEAS */

#ifdef FEATURE_HDR_DUAL_SIM
     case HDRMC_STANDBY_PREF_CHGD_REQ:

        hdrlmac_standby_pref_chgd(cmd_ptr->msgr_cmd.msg.sys_standby_cmd.info);

        mmoc_rpt_ds_stat_chgd_cnf(cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id);
#ifdef FEATURE_HDR_DEBUG_INFO
        hdrmc_log_sent_rpt(cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id, HDR_MMOC_RPT_DS_STAT_CHGD_CNF);
#endif /* FEATURE_HDR_DEBUG_INFO */
        HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "standby_pref_chgd tid %d pref %d", 
                       cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id, 
                       cmd_ptr->msgr_cmd.msg.sys_standby_cmd.info);

      break;
#endif /* FEATURE_HDR_DUAL_SIM */

#ifdef FEATURE_HDR_TO_LTE
    /*-------------------------------------------------------------------------
    LTE Reselect abort response
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_ABORT_RESEL_RSP:

      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Inact rcv LTE reselect aborted");

      break;
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_LTE_TO_HDR_OH

    case HDRMC_LTE_RRC_HDR_SIB8_INFO_IND:
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Inact rcv LTE SIB8 info" );
      hdrovhd_update_sib8_info( &cmd_ptr->msgr_cmd.msg.sib8_info, FALSE );
      break;

    case HDRMC_LTE_RRC_HDR_PRE_REG_INFO_IND:
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Inact rcv LTE pre-reg info" );
      hdrovhd_update_prereg_info( &cmd_ptr->msgr_cmd.msg.pre_reg_info, FALSE );
      break;

    case HDRMC_LTE_ATTACH_COMPLETE_IND:
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Inact rcv LTE attach complete ind, ignore!" );
      break;

#endif /* FEATURE_LTE_TO_HDR_OH */

    /*-------------------------------------------------------------------------
    Update LTE FPLMN list
    -------------------------------------------------------------------------*/
    case HDRMC_CM_LTE_FPLMN_UPDATE_REQ:
      hdrovhd_update_lte_fplmn( cmd_ptr->msgr_cmd.msg.cm_lte_fplmn.plmn_list_size,
                                cmd_ptr->msgr_cmd.msg.cm_lte_fplmn.plmn, FALSE );
    break;

#ifdef FEATURE_LTE_DEPRIORITIZATION
    /*-------------------------------------------------------------------------
    Update LTE deprioritized frequency
    -------------------------------------------------------------------------*/
    case HDRMC_LTE_DEPRI_FREQ_REQ:
      hdrovhd_update_lte_depri_freq( &(cmd_ptr->msgr_cmd.msg.lte_depri_freq_req), FALSE );
    break;
#endif /* FEATURE_LTE_DEPRIORITIZATION */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
    case HDRMC_MODEM_CONFIG_REFRESH:
      hdrmc_process_modem_config_refresh( 
         cmd_ptr->msgr_cmd.msg.modem_config_refresh.reference_id );
    break;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

 /*-------------------------------------------------------------------------
    Update TRM priority
    -------------------------------------------------------------------------*/
	case HDRMC_CM_TRM_PRIORITY_IND:		
		
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Inact rcv TRM priority indication with %d",
			                               cmd_ptr->msgr_cmd.msg.cm_trm_prioirty.trm_priority_high);
        if(cmd_ptr->msgr_cmd.msg.cm_trm_prioirty.trm_priority_high == TRUE)
        {
          hdrmc.tc_mode_info.mode = HDRSRCH_DISABLE_TA_TC_MODE;
        }
        else
        {
          hdrmc.tc_mode_info.mode = HDRSRCH_ENABLE_TA_TC_MODE;		  
        }
        hdrmc.tc_mode_info.pending = TRUE;
        HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "TC mode pending with %d",hdrmc.tc_mode_info.mode);
		
	break;

      /*-------------------------------------------------------------------------
	PSM Ready Request Command
	-------------------------------------------------------------------------*/
	case HDRMC_CM_CHECK_PSM_READY_REQ:
		HDR_MSG_PROT (MSG_LEGACY_HIGH, "Inactive rcv PSM Ready Request");
		hdralmp_process_psm_req();
	break;
    /*-------------------------------------------------------------------------
        UE Mode change request from MMODE
        -------------------------------------------------------------------------*/
    case HDRMC_UE_MODE_CHANGE_REQ:

      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Inactive rcv UE mode change request.Send UE mode switch cnf immediately to MMOC");
      /* inform MMOC immediately */
      mmoc_hdr_rpt_ue_mode_switch_cnf(cmd_ptr->msgr_cmd.msg.ue_mode_chg_req.trans_id, SYS_MODEM_AS_ID_1);

      HDR_MSG_PROT (MSG_LEGACY_HIGH, "UE mode switch report sent !!");

      break; 
    /*-------------------------------------------------------------------------
    Unknown Command
    -------------------------------------------------------------------------*/
    default:
      ERR( "Invalid HDRMC cmd %d", cmd_ptr->hdr.cmd, 0, 0 );
      cmd_ptr->hdr.status = HDRMC_BADCMD_S;
      break;
  }

  return next_state;
} /* hdrmc_inactive_process_cmd() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION hdrmc_exit_inactive

DESCRIPTION
  This function does the exit inactive processing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Reason for exit.

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_exit_inactive( void )
{
  HDR_MSG_PROT (MSG_LEGACY_HIGH, "Exit HDRMC Inactive State");

  if(hdrmc.powerup_inactive)
  {
    /* first time leaving inactive state set the flag to false */
    hdrmc.powerup_inactive = FALSE;
  }
} /* hdrmc_exit_inactive */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRMC_ENTER_INACTIVE

DESCRIPTION
  The entrance procedure to the inactive state of Mode Controller which
  determines what need to be processed and vectors control to the appropiate
  procedure.

DEPENDENCIES
  None.

RETURN VALUE
  The next state of Mode Controller to be processed.

SIDE EFFECTS
  None.
===========================================================================*/
hdrmc_state_type hdrmc_enter_inactive(void)
{
  dword               sigs;        /* Signals returned by mc_wait() */
  dword               hdrmc_wait_mask;
  hdrmc_msg_type         *cmd_ptr; /* HDRMC Commands */
  hdrmc_state_type    next_state; /* Next state of HDRMC Mode Controller  */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT (MSG_LEGACY_HIGH, "Enter HDRMC Inactive State");

  /* Initialize next state of HDRMC */
  next_state = HDRMC_INACTIVE_STATE;

  hdrmc_set_hdr_exiting(FALSE);

  hdrmc_wait_mask = HDRMC_CMD_Q_SIG |
                    HDRMC_NV_WRITE_SIG   |
                    TASK_STOP_SIG      |
                    TASK_OFFLINE_SIG;


  while (next_state == HDRMC_INACTIVE_STATE)
  {
    sigs = hdrmc_wait(hdrmc_wait_mask);

    if ((sigs & HDRMC_CMD_Q_SIG) != 0)
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR, HDRMC_CMD_Q_SIG );
      cmd_ptr = ( hdrmc_msg_type * ) q_get( &hdrmc.cmd_q );
      ASSERT(cmd_ptr);
      next_state = hdrmc_inactive_process_cmd( cmd_ptr );
      modem_mem_free( cmd_ptr, MODEM_MEM_CLIENT_HDR_CRIT );
      HDR_MSG_PROT_1( MSG_LEGACY_LOW, "Cmd ptr free = 0x%x.", cmd_ptr );
    }
    else if ( sigs & HDRMC_NV_WRITE_SIG )
    {
      (void) rex_clr_sigs(HDRMC_TASK_PTR, HDRMC_NV_WRITE_SIG);
      /* Write all the items left on the queue */
      while (hdrutil_nv_q_srv())
      {
        (void)hdrmc_wait(HDRMC_NV_WRITE_SIG);
        (void) rex_clr_sigs(HDRMC_TASK_PTR, HDRMC_NV_WRITE_SIG);
      }
    }
    else if ( ( sigs & TASK_STOP_SIG ) != 0 )
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR, TASK_STOP_SIG );

#ifdef FEATURE_HDR_MC_MSGR
      hdrmc_msgr_stop();
#endif /* FEATURE_HDR_MC_MSGR */

#ifndef FEATURE_MODEM_RCINIT
      tmc_task_stop();
#endif /* FEATURE_MODEM_RCINIT */
    }
    else if ( ( sigs & TASK_OFFLINE_SIG ) != 0 )
    {
      ( void ) rex_clr_sigs( HDRMC_TASK_PTR, TASK_OFFLINE_SIG );
#ifndef FEATURE_MODEM_RCINIT
      tmc_task_offline();
#endif /* FEATURE_MODEM_RCINIT */
    }

  } /* end while (next_state == HDRMC_INACTIVE_STATE)  */

  /* perform exit processing */
  hdrmc_exit_inactive();

  return next_state;

} /* hdrmc_inactive_state */

#ifdef FEATURE_MEASURE_UPTIME_IN_NV
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_UPTIME_TIMER_CB

DESCRIPTION
  This function updates the uptime count

DEPENDENCIES
  None

PARAMETERS
  dummy - Required for REX, ignore

RETURN VALUE
  Reason for exit.

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_uptime_timer_cb
(
  uint32 timer_id
)
{
  static nv_cmd_type hdr_nv_uptime_cmd;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_set_timer( &hdrmc.uptime_timer, HDRMC_UPTIME_TIMER_VAL_MS );
    /* Set timer for 5 min */

  hdrmc.uptime_counter_min += HDRMC_UPTIME_TIMER_VAL_MINS;
    /* increase uptime counter by another 5 minutes */

  hdr_nv_uptime_cmd.cmd = NV_WRITE_F;
  hdr_nv_uptime_cmd.tcb_ptr = HDRMC_TASK_PTR;
  hdr_nv_uptime_cmd.sigs = HDRMC_NV_CMD_SIG;
  hdr_nv_uptime_cmd.done_q_ptr = NULL;
  hdr_nv_uptime_cmd.item = NV_HOLD_HDR_TIME_I;
  hdr_nv_uptime_cmd.data_ptr = (nv_item_type *) &hdrmc.uptime_counter_min;

  nv_cmd(&hdr_nv_uptime_cmd);

  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "Uptime Counter : %d min",
                  hdrmc.uptime_counter_min);
} /* hdrmc_uptime_timer_cb() */
#endif /* FEATURE_MEASURE_UPTIME_IN_NV */


/* <EJECT> */ 
#ifdef FEATURE_MMGSDI
/*==============================================================================

FUNCTION HDRMC_MMGSDI_CLIENT_EVT_REG_CB

DESCRIPTION
  This function handles the MMGSDI client event registration callback.  


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrmc_mmgsdi_client_evt_reg_cb
(
  const mmgsdi_event_data_type *event
)
{

  if ( event == NULL )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Recvd MMGSDI event cb without any data");
    return;
  }

  if ( !hdrscp_ehrpd_auth_is_in_usim() )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
      "eHRPD auth is not in USIM so ignore MMGSDI event" );
    return;
  }

  switch( event->evt )
  {
    case MMGSDI_SUBSCRIPTION_READY_EVT:
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                      "MMGSDI_SUBSCRIPTION_READY_EVT received, session id %d, app type %d", 
                      event->session_id,
                      event->data.subscription_ready.app_info.app_data.app_type );
 
#ifdef FEATURE_HDR_NEW_GSDI_API
      if (event->session_id == hdrmc.mmgsdi_session_id)
      {
        hdrmc.mmgsdi_session_ready = TRUE;
      }      
#endif /* FEATURE_HDR_NEW_GSDI_API */

#ifdef FEATURE_EHRPD
      /* Handle eHRPD credentials only for primary seesion, i.e. Slot ID 1.
       * Only Slot ID 1 can have ehrpd/CDMA sunscription. */
      if ( (event->session_id == hdrmc.pri_session_id) &&
           (event->data.subscription_ready.app_info.app_data.app_type == MMGSDI_APP_USIM) )
      {
        /* Set eHRPD credentials available flag */
        hdrscp_set_ehrpd_credentials_are_valid( TRUE );
      }
#endif /* FEATURE_EHRPD */
      break;

    case MMGSDI_SESSION_CHANGED_EVT:
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                      "MMGSDI_SESSION_CHANGED_EVT received, activated %d, app type %d",
                      event->data.session_changed.activated,
                      event->data.session_changed.app_info.app_data.app_type ); 

#ifdef FEATURE_EHRPD
      /* Handle eHRPD credentials only for primary seesion, i.e. Slot ID 1.
       * Only Slot ID 1 can have ehrpd/CDMA sunscription. */
      if ( (event->session_id == hdrmc.pri_session_id) && 
           (event->data.session_changed.activated == 0) &&
	   (event->data.session_changed.app_info.app_data.app_type == MMGSDI_APP_USIM) )
      {
        /* Set eHRPD credentials available flag */
        hdrscp_set_ehrpd_credentials_are_valid( FALSE );
      }
#endif /* FEATURE_EHRPD */
      break;

	case MMGSDI_CARD_ERROR_EVT:
      HDR_MSG_PROT_4( MSG_LEGACY_HIGH, "MMGSDI_CARD_ERROR_EVT received for slot ID %d info %d hdrmc.slot %d hdrmc.pri_sessid %d", 
							 event->data.card_error.slot, event->data.card_error.info, hdrmc.pri_slot_id, hdrmc.pri_session_id  );			
#ifdef FEATURE_EHRPD
		   /* Handle CARD ERROR event only for primary seesion, i.e. Slot ID 1.
			* Only Slot ID 1 can have ehrpd/CDMA subscription. */
			if ((event->data.card_error.slot == hdrmc.pri_slot_id) && 
                            (hdrmc.state == HDRMC_ONLINE_STATE) && 
                            (event->data.card_error.info == MMGSDI_CARD_ERR_CARD_REMOVED))
		  {
			hdrscp_set_ehrpd_no_check(TRUE); /* If this is set, then do not check for EHRPD change and close session
												MMOC is going to deactivate HDR. */
                        hdrscp_set_ehrpd_credentials_are_valid( FALSE );
												
			HDR_MSG_PROT ( MSG_LEGACY_HIGH, "Hotswap:Deactivate pending from MMOC.Set pending deactivate due to card remove TRUE ");
		  }
#endif /* FEATURE_EHRPD */
		  break;

    default:
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "Ignored event [%d] for session ID [%d]", 
                       event->evt, event->session_id );
      break;
  }
} /* hdrmc_mmgsdi_client_evt_reg_cb */

/*===========================================================================
FUNCTION HDRMC_MMGSDI_SESSION_REG_CB

DESCRIPTION
  This function handles the MMGSDI SESSION OPEN command callback. It will 
  assign the global Session ID value which will be used in mmgsdi function 
  calls

DEPENDENCIES
  None

PARAMETERS
  status - status code of mmgsdi operation
  cnf - type of the mmgsdi operation
  cnf_ptr - mmgsdi data_ptr

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void hdrmc_mmgsdi_session_reg_cb
( 
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( cnf_ptr != NULL );

  if ((cnf == MMGSDI_SESSION_OPEN_CNF) && (status == MMGSDI_SUCCESS))
  {
#ifdef FEATURE_HDR_NEW_GSDI_API
    if (cnf_ptr->response_header.client_data == HDRMC_1X_PROV_PRI_SESSION)
    {
      hdrmc.mmgsdi_session_id =
          cnf_ptr->session_open_cnf.response_header.session_id;
    }
#endif /* FEATURE_HDR_NEW_GSDI_API */

#ifdef FEATURE_EHRPD
    if (cnf_ptr->response_header.client_data == HDRMC_GW_PROV_PRI_SESSION)
    {
      hdrmc.pri_session_id = 
          cnf_ptr->session_open_cnf.response_header.session_id;
    }
    else if (cnf_ptr->response_header.client_data == HDRMC_GW_PROV_SEC_SESSION)
    {
      hdrmc.sec_session_id = 
          cnf_ptr->session_open_cnf.response_header.session_id;
    }
#endif /* FEATURE_EHRPD */
  }
  else
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
      "Session ID Registration Failed [%d] with client data [%d]", 
       status, cnf_ptr->response_header.client_data );
  }

} /* hdrmc_mmgsdi_session_reg_cb() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_MMGSDI_CID_REG_CB

DESCRIPTION
  This function is a callback passed into mmgsdi_client_id_reg(). It will be 
  called to receive the client_id.

DEPENDENCIES
  None

PARAMETERS
  status - status code of mmgsdi operation
  cnf - type of the mmgsdi operation
  cnf_ptr - mmgsdi data_ptr
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmc_mmgsdi_cid_reg_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
#if defined(FEATURE_EHRPD) || defined(FEATURE_HDR_NEW_GSDI_API)
  mmgsdi_return_enum_type mmgsdi_status;
#endif /* defined(FEATURE_EHRPD) || defined(FEATURE_HDR_NEW_GSDI_API) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_HDR_NEW_GSDI_API
  if ((cnf == MMGSDI_CLIENT_ID_AND_EVT_REG_CNF) && (status == MMGSDI_SUCCESS))
#else
  if ((cnf == MMGSDI_CLIENT_ID_REG_CNF) && (status == MMGSDI_SUCCESS))
#endif /* FEATURE_HDR_NEW_GSDI_API */
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "MMGSDI registration successful");

#ifdef FEATURE_HDR_NEW_GSDI_API
    hdrmc.mmgsdi_cid = 
      cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id;
#else
    hdrmc.mmgsdi_cid = cnf_ptr->client_id_reg_cnf.response_header.client_id;
#endif /* FEATURE_HDR_NEW_GSDI_API */

#ifdef FEATURE_HDR_NEW_GSDI_API
    mmgsdi_status = mmgsdi_session_open( hdrmc.mmgsdi_cid,
                                         MMGSDI_1X_PROV_PRI_SESSION,
                                         NULL,
                                         hdrmc_mmgsdi_client_evt_reg_cb,
                                         FALSE,
                                         hdrmc_mmgsdi_session_reg_cb,
                                         HDRMC_1X_PROV_PRI_SESSION );

    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "MMGSDI session open (CSIM) status:%d", 
                   mmgsdi_status);
#endif /* FEATURE_HDR_NEW_GSDI_API */

#ifdef FEATURE_EHRPD
    /* The only place where HDR needs USIM is to read eHRPD credentials.
     * Ideally HDR should not open GW session if eHPRD credentials are stored
     * in NV. For simplicity, we always open GW session at powerup. 
     * If eHRPD credentials are stored in NV, we simply ignore the MMGSDI event. 
     */ 
    mmgsdi_status = mmgsdi_session_open( hdrmc.mmgsdi_cid,
                                         MMGSDI_GW_PROV_PRI_SESSION,
                                         NULL,
                                         hdrmc_mmgsdi_client_evt_reg_cb,
                                         FALSE,
                                         hdrmc_mmgsdi_session_reg_cb,
                                         HDRMC_GW_PROV_PRI_SESSION );
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "MMGSDI primary session (USIM) open status:%d", 
                   mmgsdi_status);

    mmgsdi_status = mmgsdi_session_open( hdrmc.mmgsdi_cid,
                                         MMGSDI_GW_PROV_SEC_SESSION,
                                         NULL,
                                         hdrmc_mmgsdi_client_evt_reg_cb,
                                         FALSE,
                                         hdrmc_mmgsdi_session_reg_cb,
                                         HDRMC_GW_PROV_SEC_SESSION );
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "MMGSDI secondary session (USIM) open status:%d", 
                   mmgsdi_status);
#endif /* FEATURE_EHRPD */
  }
  else
  {
    /* We cannot register with MMGSDI and get a client ID */
    HDR_MSG_PROT(MSG_LEGACY_FATAL, 
                 "Registration with MMGSDI failed");
    hdrmc.mmgsdi_cid = 0;
  }
} /* hdrmc_mmgsdi_cid_reg_cb */

#ifdef FEATURE_HDR_NEW_GSDI_API
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_GET_REG_MMGSDI_SID

DESCRIPTION
  This function returns the session_id associated with MMGSDI.

DEPENDENCIES
  mmgsdi_client_id_reg should have been called already.

PARAMETERS
  None
  
RETURN VALUE
  MMGSDI client id.
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmc_get_reg_mmgsdi_sid
( 
  mmgsdi_session_id_type* sid
)
{
  if (hdrmc.mmgsdi_session_ready)
  {
    *sid = hdrmc.mmgsdi_session_id;
  }

  return hdrmc.mmgsdi_session_ready; 
} /* hdrmc_get_reg_mmgsdi_sid() */
#else
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRMC_GET_REG_MMGSDI_CID

DESCRIPTION
  This function returns the client_id associated with MMGSDI.

DEPENDENCIES
  mmgsdi_client_id_reg should have been called already.

PARAMETERS
  None
  
RETURN VALUE
  MMGSDI client id.
  
SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_client_id_type hdrmc_get_reg_mmgsdi_cid( void )
{
  return hdrmc.mmgsdi_cid; 
} /* hdrmc_get_reg_mmgsdi_cid() */
#endif /* FEATURE_HDR_NEW_GSDI_API */
#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION HDRMC_INIT_FEATURE_REVC

DESCRIPTION
  This function reads NV items and sets RevC related features accordingly

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_init_feature_revc( void )
{
  uint8 revc_features;
  int efs_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read REVC related features from EFS NV. */

  efs_len = efs_get( HDRMC_REVC_FEATURES, &revc_features,
                     sizeof(uint8) );

  if ( efs_len == (int)sizeof(uint8) )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "Read HDR RevC features successfully %d",
                    revc_features );
  }
  else 
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "Read HDR RevC features failed." );

    /* fallback to default RevC features (MultiATPage on, NLB off) */
    revc_features = HDRMC_REVC_FEATURES_DEFAULT;
  }

#ifdef FEATURE_HDR_REVC
  hdrmc.features.network_load_balancing = 
    revc_features & HDRMC_FEATURES_NETWORK_LOAD_BALANCING_MASK;

  hdrmc.features.multi_at_page = 
    revc_features & HDRMC_FEATURES_MULTI_AT_PAGE_MASK;

  hdrmc.features.quick_idle = 
    revc_features & HDRMC_FEATURES_QUICK_IDLE_MASK;
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_HDR_REVB_DTX_MODE
  hdrmc.features.network_dtx = 
    revc_features & HDRMC_FEATURES_NETWORK_BASED_DTX;
#endif /* FEATURE_HDR_REVB_DTX_MODE */


    /* temp change to enable NLB by default */
} /* hdrmc_init_feature_revc */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRMC_INIT_FEATURES

DESCRIPTION
  This function is initializes hdrmc features based on NV

DEPENDENCIES
  None
 
PARAMETERS 
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmc_init_features( void )
{
  boolean jcdma_mode = FALSE;
  boolean force_restricted_cf = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read L2 failure backoff time table from EFS NV. */
  if ( (int)efs_get( HDRCP_JCDMA_MODE, &jcdma_mode, 1 )
         == (int)1 )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Read JCDMA mode NV successfully" );
  }
  else 
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Fail to read JCDMA mode NV" );
  }
  hdrmc.features.jcdma_nv = jcdma_mode;

  /* Per reduced JCDMA requirement (80-N5428-1_B), only Force Restricted CF
     and KDDI handoff threshold feature are required for JCDMA.
     NV_HDRSCP_FORCE_RESTRICTED_CF_I is reused to control both of these
     features.
   
     Other features in the hdrmc.features are no longer needed for JCDMA
     and can be removed, but they are kept intact for now */

  if (hdrutil_read_nv_item(NV_HDRSCP_FORCE_RESTRICTED_CF_I,
           (nv_item_type*)&force_restricted_cf) != NV_DONE_S)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "SCP force restricted CF data not active in NV");
    hdrmc.features.kddi_restricted_session_call_flow = FALSE;
    hdrmc.features.kddi_ho_threshold = FALSE;
  }
  else if (force_restricted_cf)
  {
    hdrmc.features.kddi_restricted_session_call_flow = TRUE;
    hdrmc.features.kddi_ho_threshold = TRUE;
  }
  else
  {
    hdrmc.features.kddi_restricted_session_call_flow = FALSE;
    hdrmc.features.kddi_ho_threshold = FALSE;
  }

#if defined FEATURE_HDR_REVC || defined FEATURE_HDR_REVB_DTX_MODE
  hdrmc_init_feature_revc();
#endif
}

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRMC_CREATE_EFS_ITEM_CONF_FILE

DESCRIPTION
  Put(append and add newline) item_file_path into conf_file_path
  A call to this function is necessary so that QPST can back up EFS items
  to QCN files. For HDR CP, use "/nv/item_files/conf/hdrcp_config_info.conf"
  for conf_file_path.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
 None

SIDE EFFECTS
  None

NOTE
  Please list all the EFS/NV item nos. & description in the commnents below
===========================================================================*/
LOCAL void hdrmc_create_efs_item_conf_file( void )
{
  int32              config_fd, result;
  char               new_line[] = "\n";

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  /* Open conf file. If it doesn't exist create it and reopen it. */
  config_fd = efs_open( HDRCP_EFS_NV_CONF_FILE_PATH, O_RDWR|O_APPEND );
  if ( config_fd >= 0 )
  {
    /* File already exists, no need to put nv item into conf file */
    result = efs_close( config_fd );
    return;
  }
  else
  {
    /* Conf file does not exist */
    if ( ENOENT == efs_errno )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Creating EFS NV item config file" );

      config_fd = efs_open( HDRCP_EFS_NV_CONF_FILE_PATH, 
                            O_WRONLY|O_CREAT|O_TRUNC, ALLPERMS );

      if ( config_fd < 0 )
      {
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                        "Error creating EFS NV item config file errno %d",
                        efs_errno );
        return;
      }
    }
    else
    {
      /* Could not open conf file for some other reason */
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                      "Error opening config file errno %d", efs_errno );
      return;
    }

    HDRMC_WRITE_EFS_NV_CONF_FILE( config_fd, HDROVHD_PREF_DEFAULT_CACHE_PERIOD_FILENAME );
#ifdef FEATURE_EHRPD
    HDRMC_WRITE_EFS_NV_CONF_FILE( config_fd, HDRSCP_EHRPD_NETWORK_SETTING_FILENAME );
#endif /* FEATURE_EHRPD */
    HDRMC_WRITE_EFS_NV_CONF_FILE( config_fd, HDROVHD_HDR_TO_LTE_RESEL_ENABLED_FILENAME );
#ifdef FEATURE_LTE_TO_HDR_OH
    HDRMC_WRITE_EFS_NV_CONF_FILE( config_fd, HDRSAP_SET_PREREG_STATUS );
    HDRMC_WRITE_EFS_NV_CONF_FILE( config_fd, HDRSAP_BACKOFF_TIME_TABLE_L2_FAILED );
    HDRMC_WRITE_EFS_NV_CONF_FILE( config_fd, HDRHMP_BACKOFF_TIME_TABLE_SESSION_FAILED );
#endif /* FEATURE_LTE_TO_HDR_OH */
#ifdef FEATURE_HDR_REVC
    HDRMC_WRITE_EFS_NV_CONF_FILE( config_fd, HDRMC_REVC_FEATURES );
#endif /* FEATURE_HDR_REVC */
    HDRMC_WRITE_EFS_NV_CONF_FILE( config_fd, HDRPLOG_HDRCP_VERBOSE_DEBUG );
    HDRMC_WRITE_EFS_NV_CONF_FILE( config_fd, HDRRUP_1X_TUNEAWAY_IN_SES_CFG_ENABLED );
#ifdef FEATURE_LTE_TO_HDR_OH
    HDRMC_WRITE_EFS_NV_CONF_FILE( config_fd, HDROVHD_MCC_NO_PRIOR_SESSION_FILENAME );
#endif /* FEATURE_LTE_TO_HDR_OH */

    result = efs_close( config_fd );
    if ( 0 != result )
    {
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR,
                      "Error closing newly created EFS NV item config file errno %d",
                      efs_errno );
      return;
    }
  }

} /* hdrmc_create_efs_item_conf_file() */


/*===========================================================================

FUNCTION HDRMC_TASK

DESCRIPTION
  This function is the entry point to the HDR Main Control task.

DEPENDENCIES
  None

PARAMETERS
  dummy - Required for REX, ignore

RETURN VALUE
  Reason for exit.

SIDE EFFECTS
  None

===========================================================================*/

void hdrmc_task( uint32 dummy )
{
#ifdef FEATURE_MMGSDI
  mmgsdi_return_enum_type mmgsdi_status;
#endif /* FEATURE_MMGSDI */

  SYS_ARG_NOT_USED(dummy); /* remove this line when dummy is used */

  /*-------------------------------------------------------------------------
   Perform task initialization.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT
  /* RCINIT calls init function defined in task scons file in rcinit context
     All tcb related intializations should be done after rcinit starts our
     task */
  hdrmc_tcb_ptr = rex_self();

  /* Put message router before RCINIT handshake to avoid race condition */
#ifdef FEATURE_HDR_MC_MSGR
  hdrmc_msgr_init();
#endif /* FEATURE_HDR_MC_MSGR */

  rcinit_handshake_startup();

  /* Run time synch with other tasks is done here if needed, following APIs
  ** used: rcinit_lookup(),  rcevt_wait_name()
  */
  (void) rcevt_signal_name(HDRMC_RCEVT_READY);
 
  hdrmc_dog_rpt_var = dog_hb_register_rex( (rex_sigs_type) HDRMC_RPT_TIMER_SIG );
    /* Register with dog HB */

#else /* FEATURE_MODEM_RCINIT */

  /* the HDRMC tcb handle should be initialized before hdrmc_task_init(). */
  hdrmc_tcb_ptr = rex_self();

  hdrmc_task_init();

  /*-------------------------------------------------------------------------
   Wait for the start signal from the Main Control task.
  -------------------------------------------------------------------------*/
  tmc_task_start();
#endif /* FEATURE_MODEM_RCINIT */

  hdrmc_create_efs_item_conf_file();

  hdrmc_init_features();

  hdrutil_powerup_init();

#ifdef FEATURE_MEASURE_UPTIME_IN_NV
  if( hdrutil_read_nv_item( NV_HOLD_HDR_TIME_I,
     (nv_item_type *) &(hdrmc.uptime_counter_min)) != NV_DONE_S)
  {
    /* Start the timer from zero if it has never been written before */
    hdrmc.uptime_counter_min = 0;
  }

#endif /* FEATURE_MEASURE_UPTIME_IN_NV */

  (void) rex_clr_sigs( HDRMC_TASK_PTR, TASK_START_SIG );

  /* Do powerup initialization of all protocols that need it */
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  /* initialize the logging statistics
     This should be done first before initializing any protocol
  */

  hdrplog_powerup_init();
  hdrlog_powerup_init();

  /*-------------------------------------------------------------------------
   Initialize all the other modules.
  -------------------------------------------------------------------------*/
  hdrsmp_powerup_init(); /* SMP must be initialized before any protocols
                          * that have session-related data */
  hdrscm_powerup_init();
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrsap_powerup_init();
#endif /* FEATURE_LTE_TO_HDR_OH */
  hdrhmp_powerup_init();
  hdrlmac_powerup_init();
  hdrcmac_powerup_init();
  hdramac_powerup_init();
  hdrfmac_powerup_init();
  hdrrmac_powerup_init();
  hdrinit_powerup_init();
  hdridle_powerup_init();
  hdrcon_powerup_init();
  hdrovhd_powerup_init();
  hdrrup_powerup_init();
  hdrstream_powerup_init();
  hdrpacc_powerup_init();  /* PACC powerup must be after stream powerup */
  hdrpac_mdm_powerup_init(); /* PAC_MDM powerup must be after stream powerup */
  hdrlup_powerup_init();
#ifdef FEATURE_HDR_DOS
  hdrdos_powerup_init();
#endif /*  FEATURE_HDR_DOS */
  hdrkep_powerup_init();
  hdrauth_powerup_init();
  hdrfcp_powerup_init();
#ifdef FEATURE_HDR_QOS
  hdrmrlpresv_powerup_init();
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR_BCMCS
  hdrbccp_powerup_init();
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_CSNA
  hdrcsna_powerup_init();
#endif
  hdralmp_powerup_init();

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrsaptx_powerup_init();
#endif /* FEATURE_LTE_TO_HDR_OH*/
 
  
#ifdef FEATURE_MEASURE_UPTIME_IN_NV
  rex_def_timer_ex( &hdrmc.uptime_timer,
                    hdrmc_uptime_timer_cb, 0);

  rex_set_timer( &hdrmc.uptime_timer, HDRMC_UPTIME_TIMER_VAL_MS );
#endif /* FEATURE_MEASURE_UPTIME_IN_NV */

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_HDR_NEW_GSDI_API
   hdrmc.mmgsdi_session_ready = FALSE;
   mmgsdi_status = mmgsdi_client_id_and_evt_reg(hdrmc_mmgsdi_client_evt_reg_cb, hdrmc_mmgsdi_cid_reg_cb, 0);
   /* Register hdrmc_mmgsdi_client_evt_reg_cb to listen to client events also */
#else
   mmgsdi_status = mmgsdi_client_id_reg(hdrmc_mmgsdi_cid_reg_cb, 0);
#endif /* FEATURE_HDR_NEW_GSDI_API */
   HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "MMGSDI reg status:%d", mmgsdi_status);
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_HDR_REVC
   hdrmc.qos_flow_active = FALSE;
#endif /* FEATURE_HDR_REVC */

  /*-------------------------------------------------------------------------
   Send the first watchdog report and set the timer for report interval.
  -------------------------------------------------------------------------*/
  hdrmc_process_wdog();

  hdrmc.state = HDRMC_INACTIVE_STATE;
  hdrmc.is_initialized = TRUE;
  /*-------------------------------------------------------------------------
   Enter the task loop to determine the sub-state to enter
  -------------------------------------------------------------------------*/
  for (;;)
  {
    switch (hdrmc.state)
    {
       /* Process Online state */
        case HDRMC_ONLINE_STATE:

        hdrmc.state = hdrmc_enter_online();
        break;

      /* Process Inactive state */
      case HDRMC_INACTIVE_STATE:
        hdrmc.state = hdrmc_enter_inactive();
        break;

      default:
        ERR_FATAL("Invalid HDRMC state %d", hdrmc.state, 0, 0);
        break;

    } /* end switch (hdrmc.state) */
  }
} /* hdrmc_task() */

/*===========================================================================

FUNCTION HDRMC_MULTIMODE_SANITY_ERR_FATAL

DESCRIPTION
  The function to call the err_fatal because of the reason passed in.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrmc_multimode_sanity_err_fatal
(

     mmoc_sanity_err_e_type     reason
)
{

  switch ( reason )
  {
    case MMOC_SANITY_ERR_DEACT_CMD_CNF:
      /* The HDR protocol did not process/respond to the MC_CDMA_PROT_DEACT_CMD_F
      ** The HDR protocol should respond within 15 seconds, in case of
      ** deactivating because of power down it is 30 seconds.
      */
      ERR_FATAL( "HDR did not respond to  deactivate command",
                 0, 0, 0
                );
      break;

    case MMOC_SANITY_ERR_PH_STAT_CMD_CNF:
      /* The HDR protocol did not process/respond to the MC_CDMA_PROT_PH_STAT_CHGD_CMD_F
      ** The HDR protocol should respond within 15 seconds.
      */
      ERR_FATAL( "HDR did not respond to phone status chgd command",
                 0, 0, 0
                );
      break;

    case MMOC_SANITY_ERR_GENERIC_CMD_CNF:
      /* The CDMA protocol did not process/respond to the MC_CDMA_PROT_GEN_CMD_F( because
      ** of pref_sys changed or SD reselection timeout.
      ** The CDMA protocol should respond within 15 seconds.
      */
      ERR_FATAL( "HDR did not respond to generic command",
                 0, 0, 0
                );
      break;

    default:
     ERR_FATAL( "Unknown reason", 0, 0, 0 );
     break;

  } /* switch ( reason ) */

} /* hdrmc_multimode_sanity_err_fatal() */

/*===========================================================================

FUNCTION HDRMC_FEATURE_IS_ENABLED

DESCRIPTION
  The function checks if a JCDMA sub-feature is enabled.

DEPENDENCIES
  This function should only be called from HDRMC task

PARAMETERS
  feature - JCDMA related feature to check against

RETURN VALUE
  True - If the feature is enabled
  FALSE - Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_feature_is_enabled
(
  hdrmc_feature_enum_type feature
)
{
  boolean feature_enabled = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(feature)
  {
    case HDRMC_FEATURE_JCDMA:
      feature_enabled = hdrmc.features.jcdma;
      break;
    case HDRMC_FEATURE_HDR_KA_REQUEST_DISABLED:
      feature_enabled = hdrmc.features.ka_req_disabled;
      break;
    case HDRMC_FEATURE_HDR_SESSION_TIED_TO_PPP:
      feature_enabled = hdrmc.features.session_tied_to_ppp;
      break;
    case HDRMC_FEATURE_HDR_DISABLE_SUSPEND_TIMER:
      feature_enabled = hdrmc.features.disable_susp_timer;
      break;
    case HDRMC_FEATURE_HDR_ALLOW_SPIBI_HANDOFFS:
      feature_enabled = hdrmc.features.allow_spibi_handoffs;
      break;
    case HDRMC_FEATURE_HDR_ACCOLC_ALTERNATE_MAPPING:
      feature_enabled = hdrmc.features.accolc_alternate_mapping;
      break;
    case HDRMC_FEATURE_HDR_NO_ACTIVE_HANDDOWN:
      feature_enabled = hdrmc.features.no_active_handdown;
      break;
    case HDRMC_FEATURE_REMOVE_OFF_BAND_NEIGHBORS:
      feature_enabled = hdrmc.features.remove_off_band_neighbors;
      break;
    case HDRMC_FEATURE_HDR_RL_RATE_INERTIA:
      feature_enabled = hdrmc.features.rl_rate_intertia;
      break;
    case HDRMC_FEATURE_KDDI_RESTRICTED_SESSION_CALL_FLOW:
      feature_enabled = hdrmc.features.kddi_restricted_session_call_flow;
      break;
    case HDRMC_FEATURE_KDDI_OFS_HO_THRESHOLD:
      feature_enabled = hdrmc.features.kddi_ho_threshold;
      break;
    case HDRMC_FEATURE_NETWORK_LOAD_BALANCING:
      feature_enabled = hdrmc.features.network_load_balancing;
      break;
    case HDRMC_FEATURE_MULTI_AT_PAGE:
      feature_enabled = hdrmc.features.multi_at_page;
      break;
    case HDRMC_FEATURE_QUICK_IDLE:
      feature_enabled = hdrmc.features.quick_idle;
      break;
    case HDRMC_FEATURE_HDR_NETWORK_DTX:
      feature_enabled = hdrmc.features.network_dtx;
      break;
    case HDRMC_FEATURE_HDR_JCDMA_NV:
      feature_enabled = hdrmc.features.jcdma_nv;
      break;
    default:
      ERR_FATAL("hdrmc_feature_is_enabled with invalid feature: %d", 
                feature, 0, 0);
  }
  return feature_enabled;
} /* hdrmc_feature_is_enabled */

/*===========================================================================
FUNCTION HDRMC_SET_FEATURE_JCDMA

DESCRIPTION
  The function enable/disable a JCDMA

DEPENDENCIES
  This function should only be called from HDRMC task

PARAMETERS
  enabled - To enable/disable JCDMA

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrmc_set_feature_jcdma
(
  boolean enabled
)
{
  /* Per reduced JCDMA requirement (80-N5428-1_B), only Force Restricted CF
     and KDDI handoff threshold feature are required for JCDMA.

     Other features in the hdrmc.features are no longer needed for JCDMA
     and can be removed, but they are kept intact for now */

#ifdef FEATURE_JCDMA
  /* In phase 1, FEATURE_JCDMA is still defined, and cannot be switch off */
  ASSERT(enabled);
  hdrmc.features.jcdma = TRUE;
#else
  hdrmc.features.jcdma = enabled;
#endif

#ifdef FEATURE_HDR_KA_REQUEST_DISABLED
  /* if a feature is #defined, it cannot be turned off. */
  hdrmc.features.ka_req_disabled = TRUE;
#else
  hdrmc.features.ka_req_disabled = enabled;
#endif

#ifdef FEATURE_HDR_SESSION_TIED_TO_PPP
  hdrmc.features.session_tied_to_ppp = TRUE;
#else
  hdrmc.features.session_tied_to_ppp = enabled;
#endif

#ifdef FEATURE_HDR_ALLOW_SPIBI_HANDOFFS
  hdrmc.features.allow_spibi_handoffs = TRUE;
#else
  hdrmc.features.allow_spibi_handoffs = enabled;
#endif

#ifdef FEATURE_HDR_ACCOLC_ALTERNATE_MAPPING
  hdrmc.features.accolc_alternate_mapping = TRUE;
#else
  hdrmc.features.accolc_alternate_mapping = enabled;
#endif

#ifdef FEATURE_HDR_NO_ACTIVE_HANDDOWN
  hdrmc.features.no_active_handdown = TRUE;
#else
  hdrmc.features.no_active_handdown = enabled;
#endif

#ifdef FEATURE_REMOVE_OFF_BAND_NEIGHBORS
  hdrmc.features.remove_off_band_neighbors = TRUE;
#else
  hdrmc.features.remove_off_band_neighbors = enabled;
#endif

#ifdef FEATURE_HDR_RL_RATE_INERTIA
  hdrmc.features.rl_rate_intertia = TRUE;
#else
  hdrmc.features.rl_rate_intertia = enabled;
#endif

#ifdef FEATURE_HDR_DISABLE_SUSPEND_TIMER
  hdrmc.features.disable_susp_timer = TRUE;
#else
  hdrmc.features.disable_susp_timer = enabled;
#endif
} /* hdrmc_set_feature_jcdma */

/*===========================================================================

FUNCTION HDRMC_EVENT_Q_OVER_THRESHOLD

DESCRIPTION
  The function checks if HDRMC event Q has more items than a predetermined
  threshold.

DEPENDENCIES
  HDRMC queue must be initialized

RETURN VALUE
  FALSE - Since this is a dynamic allocation implementation

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_event_q_over_threshold
( 
  hdrhai_protocol_name_enum_type protocol_name 
)
{
    return FALSE;
} /* hdrmc_event_q_over_threshold() */


/*===========================================================================

FUNCTION HDRMC_CLIENT_CB_REG

DESCRIPTION
  This function allows a client to register/deregister a callback with HDR to
  receive registered HDR events.
  
  Only one event can be registered/deregistered once. To register/deregister
  multiple events, call this function several times.

DEPENDENCIES
  None
 
PARAMETERS 
  client - Modules which will register the callbacks  
  cb_event - HDR event types to be registered
  reg_type - register or deregister
  cb_func_ptr - pointer to the callback functions
  client_data_ptr - pointer to the callback client data
 
RETURN VALUE
  E_NOT_ALLOWED - parameters are invalid
  E_SUCCESS - register callbacks successfully

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrmc_client_cb_reg
(
  hdrmc_cb_client_id_enum_type      client,
    /* client id */
  
  hdrmc_cb_event_enum_type          cb_event,
    /* Events that client wants to register */ 
       
  hdrmc_cb_reg_enum_type            reg_type,
    /* Register or deregister */      

  hdrmc_cb_f_type                   cb_func_ptr,
    /* Pointer to a callback function to notify the client */    

  void                              *client_data_ptr
    /* Pointer to the client data */  
)
{      
  /* Sanity check */
  if ( client >= HDRMC_CB_CLIENT_MAX )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Client ID %d is not allowed", client);
    return E_NOT_ALLOWED;
  }

  if ( cb_event >= HDRMC_CB_EVENT_ENUM_MAX )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                    "Event (%d) to be registered is not allowed", cb_event);
    return E_NOT_ALLOWED;
  }

  if ( (reg_type != HDRMC_CB_REG) && (reg_type != HDRMC_CB_DEREG) )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Invalid register type %d", reg_type);
    return E_NOT_ALLOWED;
  }

  /* Register or deregister */
  if ( reg_type == HDRMC_CB_REG )
  {  
    /* Register the event */
    HDR_MSG_PROT_2( MSG_LEGACY_LOW, "Client %d registers event %d", 
                    client, cb_event);

    if ( cb_func_ptr == NULL )
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "Callback function is missing" );
      return E_NOT_ALLOWED;
    }
    /* Store the callback function and client data */
    hdrmc_cb_entry_map[client][cb_event].cb_func_ptr = cb_func_ptr;
    hdrmc_cb_entry_map[client][cb_event].client_data_ptr = client_data_ptr;

  } /* if (reg_type == HDRMC_CB_REG) */
  else
  {
    /* Deregister the event */
    HDR_MSG_PROT_2( MSG_LEGACY_LOW, "Client %d deregisters event %d", 
                    client, cb_event);
    hdrmc_cb_entry_map[client][cb_event].cb_func_ptr = NULL;
    hdrmc_cb_entry_map[client][cb_event].client_data_ptr = NULL;

  } /* if (reg_type != HDRMC_CB_REG) */

  return E_SUCCESS;
} /* hdrmc_client_cb_reg */

/*===========================================================================

FUNCTION HDRMC_EXECUTE_CB

DESCRIPTION
  This function looks for registered clients according to the events passed in
  and execute the callback functions if a registered client is found.

DEPENDENCIES
  None
 
PARAMETERS 
  cb_event - the event type that triggers the callback
  params - parameters of callback function
 
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmc_execute_cb
(
  hdrmc_cb_params_rsp_union_type *params
)
{
  /* Client ID */
  hdrmc_cb_client_id_enum_type client;
  /* Events */
  hdrmc_cb_event_enum_type cb_event;
  /* Convenience pointer for callback function pointer */
  hdrmc_cb_f_type cb_func_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cb_event = params->cb_type;

  if(cb_event >= HDRMC_CB_EVENT_ENUM_MAX)
  {
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, " cb_type index %d >= HDRMC_CB_EVENT_ENUM_MAX. Cannot call callback ", 
                     cb_event );
      return ;
  }

  /* Look for client that has registered for this event. */
  for ( client = (hdrmc_cb_client_id_enum_type)0; 
        client < HDRMC_CB_CLIENT_MAX; client++ )
  {
    cb_func_ptr = hdrmc_cb_entry_map[client][cb_event].cb_func_ptr;
    if ( cb_func_ptr != NULL )
    {
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "Execute callbacks for client %d, event %d", 
                      client, cb_event );
      cb_func_ptr(params, hdrmc_cb_entry_map[client][cb_event].client_data_ptr);
    }
  }
} /* hdrmc_exec_cb */

#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================
FUNCTION HDRMC_LTE_TO_HDR_HO_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether LTE to HDR is in 
  progress or not.
   
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if LTE to HDR HO is in progress, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_lte_to_hdr_ho_is_in_progress( void )
{
  if ((hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_REDIRECT) 
      ||
      (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_RESELECT)
      ||
      (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_ACTIVE_HO))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* hdrmc_lte_to_hdr_ho_is_in_progress() */

/*===========================================================================
FUNCTION HDRMC_HDR_TO_LTE_TIME_TRANSFER_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether LTE to HDR is in 
  progress or not.
   
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if LTE to HDR HO is in progress, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_hdr_to_lte_time_transfer_is_in_progress( void )
{
  if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == 
      HDRMC_IRAT_HDR_TO_LTE_TIME_TRANSFER) 
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* hdrmc_hdr_to_lte_time_transfer_is_in_progress() */

/*===========================================================================
FUNCTION HDRMC_LTE_TO_HDR_RESEL_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether reselection is in 
  progress or not.
   
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if reselection is in progress, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_lte_to_hdr_resel_is_in_progress( void )
{

  if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_RESELECT)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* hdrmc_lte_to_hdr_resel_is_in_progress() */

/*===========================================================================
FUNCTION HDRMC_LTE_TO_HDR_GET_CGI_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether get CGI is in 
  progress or not.
   
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if get CGI is in progress, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_lte_to_hdr_get_cgi_is_in_progress( void )
{

  if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_GET_CGI)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* hdrmc_lte_to_hdr_get_cgi_is_in_progress() */

/*===========================================================================
FUNCTION HDRMC_LTE_TO_HDR_REDIR_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether redirection is in 
  progress or not.
   
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if redirection is in progress, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_lte_to_hdr_redir_is_in_progress( void )
{

  if (hdrmc.lte_to_hdr_ho_info.irat_ho_type == HDRMC_IRAT_LTE_TO_HDR_REDIRECT )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* hdrmc_lte_to_hdr_redir_is_in_progress() */

/*===========================================================================
FUNCTION HDRMC_LTE_TO_HDR_HO_IS_ABORTED

DESCRIPTION
  This function returns whether LTE to HDR handoff was aborted.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - if LTE to HDR handoff was aborted. FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_lte_to_hdr_ho_is_aborted( void )
{
  return hdrmc.lte_to_hdr_ho_aborted;
} /* hdrmc_lte_to_hdr_ho_is_aborted() */

/*===========================================================================

FUNCTION HDRMC_GET_RESELECT_TIMING_INFO

DESCRIPTION
  This function is used to retrieve the reselection timing information passed
  to HDR by LTE stack.

DEPENDENCIES
  None.

PARAMETERS
  lte_ostmr - Pointer to LTE RTC counter
  cdma_sys_time - Pointer to CDMA System time

RETURN VALUE
  LTE RTC counter and CDMA System time

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_get_reselect_timing_info 
(
  uint32 *lte_ostmr,
  uint64 *cdma_sys_time
)
{

  *lte_ostmr = hdrmc.lte_to_hdr_ho_info.lte_ostmr;
  *cdma_sys_time = hdrmc.lte_to_hdr_ho_info.cdma_sys_time;

} /* hdrmc_get_reselect_timing_info() */

/*===========================================================================

FUNCTION HDRMC_GET_RESELECT_PILOT_PN

DESCRIPTION
  This function is used to retrieve the reselection pilot pn passed to HDR by 
  LTE stack.

DEPENDENCIES
  None.

PARAMETERS
  pilot_pn - Pointer to HDR pilot pn

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_get_reselect_pilot_pn 
(
  sys_pilot_pn_type *pilot_pn
)
{

  *pilot_pn = hdrmc.lte_to_hdr_ho_info.pilot_pn[0];

} /* hdrmc_get_reselect_pilot_pn() */

/*===========================================================================

FUNCTION HDRMC_GET_TIMING_TRANSFER_PN_LIST

DESCRIPTION
  This function is used to retrieve the timing transfer pn list passed to HDR
  by LTE stack.

DEPENDENCIES
  None.

PARAMETERS
  pn_list_size - Number of pn's in list
  pilot_pn - Pointer to HDR pilot pn

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_get_timing_transfer_pn_list 
(
  uint8             *pn_list_size,
  sys_pilot_pn_type *pilot_pn
)
{
  *pn_list_size = hdrmc.lte_to_hdr_ho_info.pn_list_size;

#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy (pilot_pn, 
           sizeof(sys_pilot_pn_type) * HDR_CP_MAX_PN_LIST_SIZE,  
           hdrmc.lte_to_hdr_ho_info.pilot_pn, 
           (sizeof (sys_pilot_pn_type) * HDR_CP_MAX_PN_LIST_SIZE) );
#else
  memcpy (pilot_pn, hdrmc.lte_to_hdr_ho_info.pilot_pn, 
          (sizeof (sys_pilot_pn_type) * HDR_CP_MAX_PN_LIST_SIZE) );
#endif /* FEATURE_MEMCPY_REMOVAL */

} /* hdrmc_get_timing_transfer_pn_list() */

/*===========================================================================

FUNCTION HDRMC_GET_DURATION_ALLOWED_FOR_TIME_TRANSFER

DESCRIPTION
  This function is used to retrieve the Maximum time allowed by LTE for DO 
  to do brute-force acq and sync.

DEPENDENCIES
  None.

PARAMETERS
  time_allowed_ms - Maximum time allowed by LTE for DO to do brute-force 
  acq and sync.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_get_duration_allowed_for_time_transfer 
(
  uint16 *time_allowed_ms
)
{

  *time_allowed_ms = hdrmc.lte_to_hdr_ho_info.time_allowed_ms;

} /* hdrmc_get_duration_allowed_for_time_transfer() */

/*===========================================================================

FUNCTION HDRMC_GET_CGI_SRCH_WINDOW_SIZE

DESCRIPTION
  This function is used to retrieve the cgi search window size information passed
  to HDR by LTE stack.

DEPENDENCIES
  None.

PARAMETERS
  srch_window_size - Pointer to SRCH window size

RETURN VALUE
  SRCH window size

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmc_get_cgi_srch_window_size 
(
  uint8 *srch_window_size
)
{

  if(srch_window_size != NULL)
  {
  	*srch_window_size = hdrmc.lte_to_hdr_ho_info.search_window_size;
  }
  else
  {
	HDR_MSG_PROT(MSG_LEGACY_ERROR,"A NULL pointer has been passed.");
  }

} /* hdrmc_get_cgi_srch_window_size() */

#endif /* FEATURE_LTE_TO_HDR */

/*===========================================================================
FUNCTION HDRMC_GET_STATS_INFO 

DESCRIPTION
  This function is used by CM get the FTD statistics.

DEPENDENCIES
  None.

PARAMETERS
  ftd_stats_ptr - Pointer to location where the FTD stats need to be copied.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmc_get_stats_info 
(
  sys_hdr_information_s_type *ftd_stats_ptr
)
{
  hdrlog_cm_get_ftd_stats_report ( ftd_stats_ptr );
} /* hdrmc_get_stats_info() */


#ifdef FEATURE_HDR_REVC
/*===========================================================================
FUNCTION HDRMC_SET_QOS_FLOW_STATUS

DESCRIPTION
  This function is to be called by DS layer when QoS flow status changes:
    1) when any QoS flow becomes active on HDR tech
    2) when all QoS flow become inactive/unrequested

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrmc_set_qos_flow_status
(
  boolean active
)
{
  /* This function is to be called by DS layer when QoS flow
     status changes */
  HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "QoS flow status %d => %d",
                 hdrmc.qos_flow_active, active);  
  
  if (hdrmc.qos_flow_active != active)
  {
    hdrmc.qos_flow_active = active;
    if (hdrfmac_is_loadinfo_supported())
    {
      hdrind_give_ind(HDRIND_MC_QOS_FLOW_STATUS_CHANGED, NULL);
    }
  }
}

/*===========================================================================
FUNCTION HDRMC_IS_QOS_FLOW_ACTIVE

DESCRIPTION
  This function returns current QoS flow status set by DS  

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  True  - if any QoS flow is active
  False - if no QoS flow is active

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrmc_is_qos_flow_active( void )
{
  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "QoS: %d", hdrmc.qos_flow_active);

  return hdrmc.qos_flow_active;
}
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION HDRMC_TUNNEL_ACTIVATION_IS_PENDING

DESCRIPTION
  This function returns the value of tunnel_activation_is_pending flag.

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  boolean value of flag tunnel_activation_is_pending

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_tunnel_activation_is_pending( void )
{
  return hdrmc.tunnel_activation_is_pending;

} /* hdrmc_tunnel_activation_is_pending() */

/*===========================================================================
FUNCTION HDRMC_SET_TUNNEL_ACTIVATION_IS_PENDING

DESCRIPTION
  This function sets the vaule of tunnel_activation_is_pending flag

DEPENDENCIES 
  None 
 
PARAMETERS 
  value - boolean value of flag tunnel_activation_is_pending
 
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrmc_set_tunnel_activation_is_pending
(
  boolean value
)
{
  hdrmc.tunnel_activation_is_pending = value;
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Set tunnel_activation_is_pending: %d", 
                  hdrmc.tunnel_activation_is_pending );

} /* hdrmc_set_tunnel_activation_is_pending() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================
FUNCTION HDRMC_IS_FIRST_INIT

DESCRIPTION
  This function tells whether the task has been initialized or not

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  TRUE if HDRMC is not initialized yet. FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_is_first_init( void )
{
  return !hdrmc.is_initialized;
}

#ifdef FEATURE_HDR_DEBUG_INFO

/*===========================================================================
FUNCTION HDRMC_LOG_RCVD_RPT

DESCRIPTION
  This function logs the rcvd event from MMOC/LTE in hdr debug buffer

DEPENDENCIES 
  None 
 
PARAMETERS 
  Transaction id - transaction id used only for MMODE commands
  cmd - name of the command recieived
 
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void hdrmc_log_rcvd_rpt(prot_trans_type transaction_id, hdrmc_name_type cmd)
{
   uint8 rcvd_rpt_index;
   rcvd_rpt_index = hdrmc.dbg_info.rcvd_rpt_index++;
   
   hdrmc.dbg_info.rcvd_rpt[rcvd_rpt_index].transaction_id = transaction_id;
   hdrmc.dbg_info.rcvd_rpt[rcvd_rpt_index].cmd = cmd;   
   time_get_julian(&(hdrmc.dbg_info.rcvd_rpt[rcvd_rpt_index].rcvd_time_jul));
   hdrmc.dbg_info.rcvd_rpt[rcvd_rpt_index].state = hdrmc.state;
   hdrmc.dbg_info.rcvd_rpt[rcvd_rpt_index].almp_state = hdralmps_get_state();
#ifdef FEATURE_LTE_TO_HDR_OH   
   hdrmc.dbg_info.rcvd_rpt[rcvd_rpt_index].tunnel_mode = hdrsap_get_current_tunnel_mode_value( HDRHAI_HDRMC );
#endif /* FEATURE_LTE_TO_HDR */
   hdrmc.dbg_info.rcvd_rpt_index %= DEBUG_BUF_SIZE;   
   
}

/*===========================================================================
FUNCTION HDRMC_LOG_SENT_RPT

DESCRIPTION
  This function logs the sent event to MMOC/LTE in hdr debug buffer

DEPENDENCIES 
  None 
 
PARAMETERS 
  Transaction id - transaction id used only for MMODE commands
  cmd - name of the command sent
 
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void hdrmc_log_sent_rpt(prot_trans_type transaction_id, hdrmc_sent_cmd_type cmd)
{
   uint8 sent_rpt_index;
   sent_rpt_index = hdrmc.dbg_info.sent_rpt_index++;
   
   hdrmc.dbg_info.sent_rpt[sent_rpt_index].transaction_id = transaction_id;
   hdrmc.dbg_info.sent_rpt[sent_rpt_index].cmd = cmd;   
   time_get_julian(&(hdrmc.dbg_info.sent_rpt[sent_rpt_index].sent_time_jul));
   hdrmc.dbg_info.sent_rpt[sent_rpt_index].state = hdrmc.state;
   hdrmc.dbg_info.sent_rpt[sent_rpt_index].almp_state = hdralmps_get_state();
#ifdef FEATURE_LTE_TO_HDR_OH   
   hdrmc.dbg_info.sent_rpt[sent_rpt_index].tunnel_mode = hdrsap_get_current_tunnel_mode_value( HDRHAI_HDRMC );
#endif /* FEATURE_LTE_TO_HDR */
   hdrmc.dbg_info.sent_rpt_index %= DEBUG_BUF_SIZE;
}


boolean hdrmc_get_transaction_id(prot_trans_type *transaction_id, hdrmc_msg_type *cmd_ptr)
{
  boolean cmd_ignored = FALSE;
  *transaction_id = 0;
   
  switch ( cmd_ptr->hdr.cmd )
  {
  case HDRMC_PROT_DEACT_REQ:
    *transaction_id = cmd_ptr->msgr_cmd.msg.deactivate_protocol.trans_id;
    break;
  
  case HDRMC_PROT_PH_STAT_CHGD_REQ:
    *transaction_id = cmd_ptr->msgr_cmd.msg.ph_status_chgd.trans_id;
    break;


  #ifdef FEATURE_HDR_DUAL_SIM
  case HDRMC_STANDBY_PREF_CHGD_REQ:
     *transaction_id = cmd_ptr->msgr_cmd.msg.sys_standby_cmd.trans_id;
     break;
  #endif

  case HDRMC_END_REQ:
  case HDRMC_IDLE_PGSLOT_CHANGED_REQ:
  case HDRMC_UNLOCK_HDR_RF_REQ:
  case HRDMC_TUNNEL_MSG_REQ:
    if(hdrmc.state == HDRMC_INACTIVE_STATE)
  	 cmd_ignored = TRUE;
     break;

  default:
     break;
  }
  return cmd_ignored;
}

#ifdef FEATURE_LTE_TO_HDR
hdrmc_sent_cmd_type hdrmc_get_irat_cmdtype(hdrmc_irat_ho_type irat_ho_type)
{
	hdrmc_sent_cmd_type cmd_type=HDR_NONE;
	
	switch(irat_ho_type)
	{
#ifdef FEATURE_LTE_TO_HDR_OH	
	case HDRMC_IRAT_LTE_TO_HDR_ACTIVE_HO:
	   cmd_type = HDR_LTE_ACTIVE_HO_FAILED_RSP;
	   break;
#endif /* FEATURE_LTE_TO_HDR_OH */
	case HDRMC_IRAT_LTE_TO_HDR_GET_CGI:
	   cmd_type = HDR_LTE_GET_CGI_FAILED_RSP;
	   break;
	   
	case HDRMC_IRAT_HDR_TO_LTE_TIME_TRANSFER:
	 cmd_type = HDR_LTE_REVERSE_TIMING_TRANSFER_FAILED_RSP;
	   break;
	   
	case HDRMC_IRAT_LTE_TO_HDR_RESELECT:
	 cmd_type = HDR_LTE_RESEL_FAILED_RSP;
	   break;
	   
	case HDRMC_IRAT_LTE_TO_HDR_REDIRECT:
	 cmd_type = HDR_LTE_REDIR_FAILED_RSP;
	   break;
    default:
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "NO VALID CMD TYPE");
      break;
	}
    return cmd_type;
}
#endif /* FEATURE_LTE_TO_HDR */
#endif /* FEATURE_HDR_DEBUG_INFO */


/*===========================================================================
FUNCTION HDRMC_MCFG_REFRESH_IN_PROGRESS

DESCRIPTION
  This function checks if mcfg refresh is in progress

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  value - boolean value of flag 

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrmc_mcfg_refresh_in_progress()
{
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "MCFG refresh in progress: %d", 
                  hdrmc.mcfg_refresh_is_in_progress );
  
  return hdrmc.mcfg_refresh_is_in_progress;
} /* hdrmc_get_mcfg_refresh_in_progress() */

/*===========================================================================
FUNCTION HDRMC_SET_HDR_EXITING

DESCRIPTION
  This function sets the vaule of hdr exiting flag

DEPENDENCIES 
  None 
 
PARAMETERS 
  value - boolean value of flag 
 
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrmc_set_hdr_exiting
(
  boolean value
)
{
  hdrmc.hdr_exiting = value;
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Set hdr_exiting: %d", 
                  hdrmc.hdr_exiting );

} /* hdrmc_set_hdr_exiting() */

