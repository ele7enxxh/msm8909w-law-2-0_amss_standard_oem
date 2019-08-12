/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   C A L L   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Call Object.

  The Call Object is responsible for:
  1. Processing clients' call commands.
  2. Processing MC replies for clients' call commands.
  3. Processing MC call related notifications.
  4. Notifying the client list of call events.


EXTERNALIZED FUNCTIONS

  cmxcall_info_get
    Copy the current call state information into a specified buffer.

  cmxcall_client_cmd_proc
    Process clients' call commands.

  cmxcall_mc_rpt_proc
    Process MC via UI call commands.

  cmxcall_timer_proc
    Process call timer events


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1998 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/14   sm      Support for 1x SRVCC
03/05/13   sm      Indicate 1x Orig Failure to Clients
11/13/13   xs      Reject 1x MT call during VT call
10/24/13   ss      FR17289 No two Simultaneous 1x MO Calls
08/25/13   sm      VOLTE concurrency during e911 orig
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
01/22/13   xs      Added video share support
12/04/12   cl      Reject MO call if there's 1X MT call in progress
10/19/12   xs      Allow DS call orig when VOLTE call is connected
10/02/12   cl      Reject SO33/32 MT page when we're transition to LPM
09/20/12   vm      Changes for ignore_protocol_activate being added
09/05/12   fj      If there's OTASP call ongoing, end the originated data call.
06/13/12   fj      Update SVDO checking for the new call during OTAPA call.
07/02/12   sg      Added support C+H+G+W+L mode
05/28/12   aj/skk  Handle orig failure similar to call end cnf if call is in idle state
05/25/12   ar      update checks for cmxcall_is_end_suspend_status
05/11/12   ab      Fix for wrong throttling information is returned when hybrid is off
                   and HDR is on the main stack
05/09/12   sks     When all the voice calls have ended, if there is an existing
                   suspended data that is CM initiated, Update the suspend state
                   so that data call will be resumed.
04/14/12   xs      Codec info propagation
04/10/12   sk      Fixing the CW and KW warnings
03/23/12   qf      CM should only wait CALL_END_CNF for 1x SO32/33 data call
03/12/12   cl      CM handles MO/MT SO33/32 calls collisions
03/12/12   cl      CM handling 1X-CP call release
03/06/12   ak      Deprecate is_data_switch_over_call_success from the CM code
02/07/12   xs      VOLTE silent redial support
01/17/12   cl      CM notify call-end event after CALL_END_CNF_F from 1X-CP
01/17/12   cl      Support SO-33 CALL_END_CNF_F from 1X-CP
01/17/12   aj      SC SVLTE SO 33 suspend changes
01/10/12   gm      Mainlining HDR API change integration
01/06/12   gm      Data synchronization changes for freefloating platform
12/21/11   gm      CM frees the call object without cleanup for privacy cmd
12/19/11   xs      Allow WILDCARD voice service option in cmxcall_is_svdo_capable
12/07/11   cl      Redial RSSI after T42 timer expires
11/01/11   rn      End data call if new SMS call is originated
10/31/11   ak      End 1X data call upon 1X voice call origination
10/25/11   xs      Fix crash during VT call orig
09/02/11   rk      S102 less 1XCSFB
08/17/11   xs      Added new mode pref CDMA_HDR_GSM
07/27/11   jh      Adding new mode_pref (GSM_LTE, CDMA_GSM_LTE, HDR_GSM_LTE,
                   WCDMA_LTE, CDMA_WCDMA_LTE, HDR_WCDMA_LTE)
07/24/11   aj      SC SVLTE 1.0 merge to mainline
06/11/11   rk      Send CSFB abort req command to NAS if user ended the call
                   while 1XCP is CSFB activated state
04/11/11   am      Cleaning stubs and runnning qtf SUTE
04/06/11   rk      Adding debug statements
04/05/11   rk      Handle 1XCSFB call PSIST check failures
03/30/11   rk      revising handling of failure cases
03/23/11   rk      Allow client to choose the srv opt for 1XCSFB call
03/22/11   gm      Fix to waiting 4s after call end to force THERM EMERG
03/22/11   rk      Support for handling negative scenarios
03/11/11   rm      GSDI clean-up
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/07/11   xs      Remove FEATURE_T53 for hold answer
01/17/10   ak      Decoupling SO73 from 1x_Adv feature.
01/16/10   rk      Correcting CM_1XCSFB_INCOMING_CALL_F handling not to send
                   CM_CALL_EVENT_INCOM event to CM clients
01/13/11   ak      Assigning SO75 to call type test.
12/23/10   sv      Integrate DSDS feature
12/10/10   rk      1XCSFB MT call changes
10/22/10   rk      Sucessful MO call initial support
11/16/10   xs      Added con_ref notification support
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
10/08/10   sg      Add check to block CS_DATA calls.
09/22/10   xs      Fixed target compiler warning
06/16/10   gm      Added debug msg for dropping OTASP/OTAPA status.
08/02/10   gm      Added CM_MODE_PREF_CDMA_LTE_ONLY for call type voice, data
                   & SMS
06/29/10   gm      Propagte PS_FAILURE_ORIG only if call is attempted on HDR
06/15/10   aj      support for IRAT measurement support
06/10/10   ak      Mainlining memory optimization changes.
06/04/10   ak      Added support for 1X Advanced SO75
05/26/10   jqi     EHRPD VOIP silent redial support.
05/20/10   ak      Removing SO75 support due to dependancies in MOB.
05/14/10   ak      Added support for 1X Advanced SO75
05/07/10   fj      Add support for LTE.
05/11/10   xs      Should not end hybrid HDR PS data call whenever a call was
                   connected on 1X
04/26/10   cl      Remove FEATURE_CM_MVS_API
04/22/10   xs      Added mode pref checking for CDMA_HDR_GW & CDMA_GW
                   in cmxcall_is_valid_mode_pref()
04/01/10   mj      Propagate refresh event session ID
03/18/10   am      Fixing Compilation Warning
03/15/10   sv      Removing use of snd API
02/24/10   aj      Add support for PS optimized silent redial
01/19/10   aj      Added VOIP support for emergency call
10/30/09   ak      Added support for 1X Advanced service options
10/14/09   sg      ZI memory reduction. Mmode should call new Heap Manager API to
                   allocate memory dynamically.
09/01/09   sv      Moving from clk API to time API
08/15/09   mh      Allowing CS data call in CDMA Only mode
08/07/09   rm      CMI phase-2: Removing featurisation from /api files
08/07/09   am      Adding code changing subscription availability on
                   receiving MMGSDI refresh event.
07/31/09   mp      Added call_orig_fail layer debugging aid for epzip update
07/16/09   mh      Updated cmxcall_mc_rpt_proc() not to access ended call
06/10/09   rm      CM should not error fatal for non-EF_RAT refresh.
                   Also CM should handle MMGSDI_REFRESH_NAA_INIT_FCN
05/27/09   aj      Adding support for passing otasp status to clients
04/24/09   rm      Including cmlog_v.h
04/17/09   ks      Added support for blocking MO/MT voice call on Data Card
04/06/09   mj      Update BOOLEAN macro w/ namespace prefix to CM_BOOLEAN
03/30/09   aj      Fixed cmxcall_timer_proc() to return if call is ended
03/24/09   sv      updated to cmtaski.h
02/27/09   rm      Adding header files of other MOBs
02/23/09   rm      Removing cm_log_v.h
02/12/09   mh      Added support for no SR after MAP for IS-707B
01/29/09   rm      RAT balancing phase-2 - Vote YES to EF_RAT refresh during
                   CS/PS calls - MMGSDI changes
01/29/09   fj      Remove FEATURE_HDR_HYBRID for call events
12/17/08   jd      Fixed Lint errors
12/08/08   sv      Adding support for OTASP SPC Error
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
11/14/08   jd      Resolve Klocwork defects
10/05/08   np      Added HPCD Rev B.
10/02/08   aj      Add support for BC 17,18,19
09/05/08   sv      Memory Reduction for ULC
08/20/08   st      Resolve Klockwork defects
08/15/08   sv      Added FEATURE_CM_MVS_API for MVS and 8K support
06/12/08   sv      Resolve Klocwork defects
06/06/08   sv      Featurizing snd api under FEATURE_8K_TO_BE_PURGED
                   Removing unneccesary snd.h
04/09/08   cl      Phone should reject the call and must not crash if CS Data
                   Call is originated during 1X-Only Preference
03/26/08   vs      Update throttling table ONLY for incoming 1x data call,
                   according to DCTM 3.0
03/07/08   sg      Do not end GW call in cmxcall_end_call()
03/07/08   sg      When making E-911 call, do not torn down GPRS data call
03/05/08   vk      corrected code for HYBR_GW configuration
01/04/08   ka      Adding handset based plus code dial feature
12/13/07   sk/ak   Added DCM.
09/26/07   bh      Removed duplicated cm_mc_get_buf_else_err_fatal() in
                   cmxcall_mc_rpt_proc()
08/08/07   cl      Add OTASP Support for ASW Band
08/07/07   ic      Lint clean up
07/24/07   cl      For JCDMA config, reject data call when SO36/35(E911) is active.
06/22/07   sk      DDTM rewrite.
06/11/07   pk      VoIP -> 1x hho enahncements / CR fixes
06/08/07   cl      Fix CR#: 119770 - cmss_orig_thr_tbl_entry_info_put should
                   pass parameter by pointer
05/09/07   pk      Copying the channel information for tunneled message
03/30/07   jqi     Fix MO ending MT PS call issue for HDR only mode(non hybr)
03/27/07   jqi     End MO PS call if there is simutaneous MO and MT PS call.
03/29/07   rp      Setting mode_info.info_type in cmxcall_event_generic()
03/14/07   sk      Fixed code to end data call before originating voice call
                   if SVD is not supported.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/02/07   pk      Ending VoIP call during E911 origination
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
11/17/06   jqi     Added CDMA connect order Ack support.
11/17/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/16/06   pk      Lint Fixes
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
11/07/06   sk      Memory reduction effort.
10/03/06   ka      Changing function call name to
                   cmcall_is_ipcall_over_mode_poss ().
07/14/06   ka      Fixing the issue where call cmd cb for call end api was
                   being called more than once.
07/07/06   pk      Modified hold q processing. hold q processing is done in
                   cmtask
07/10/06   ic      Fixed Klockworks errors (added missing assertions)
05/24/06   pk      Corrected featurization in cmxcall_is_conv_pc_otapa_commit
05/10/06   ka      mode_spec_end used to bunch calls of particular info_type
                   and then send them to wcall, ipcall or xcall. Modifying
                   wcall, ipcall and xcall to end calls of their typeonly.
04/17/06   pk      Added support for CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT
04/16/06   ka      Allow MO HDR call that has been originated to complete
                   MT VoIP call on MAIN instance.
03/24/06   ka      Changing function name to cmxcall_determine_call_type()
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/09/06   ic      Adjusted for cm_num_init() / cm_num_copy() changes in cm.h
01/06/06   ic      Lint cleanup
12/11/05   ic      Lint cleanup
12/03/05   ic      Put incoming call into CM_CALL_INCOM_SUBST_ANSWERED
                   substate once it was answered but while it's not conected
                   yet.
                   Lint clean up with CM_DEBUG on and off
11/17/05   pk      Reseting sms_status in cmxcall_reset()
11/11/05   pk      Modified cmxcall_send_end_cmd to check for call pointers
                   orig_sent_on_mode before sending call end to 1X MC
11/10/05   pk      Added support for privacy_pref, and is_privacy in the
                   phone object
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
11/01/05   ka      Sending nam change to SD after traffic channel is torn
                   down for a OTASP call.
10/21/05   sk      Removed validation of call number based on digit_mode.
10/20/05   pk      Added support for CM_CALL_ORIG_ERR_ALL_ACCT_BLOCK
10/18/05   pk      Modified cmxcall_client_orig_proc, if a PD session is
                   in progress and a voice call is orginated with mode
                   preference that excludes CDMA, the mode preference for the
                   call is set to CDMA
10/02/05   ic      Corrected handling of end_params in call object.
                   Set end_status for HDR calls (where needed) to
                   CM_CALL_END_HDR_RELEASED_BY_CM before calling on
                   cmxcall_send_end_cmd() that sends call event end.
09/29/05   pk      Added support for CM_PRIVACY_SETTING_F MC report
09/12/05   pk      Calling PD session grant algorithm, if a session is
                   pending, afer CM_CALL_EVENT_CONNECT
09/12/05   ic      Fixed lint and compiler warnings
09/08/05   ka      Support for voice call over HDR on Main instance.
08/22/05   ic      Obsoleted CM_REORDER_F.
                   Use CM_CALL_ORIG_FAIL_F instead with orig_fail.code set to
                   CM_CALL_ORIG_ERR_REORDER and call_ptr->end_status set to
                   CM_CALL_END_REORDER
08/16/05   pk      Added support for priority Queues for origination
                   parameters
08/16/05   ka      Changes to allow both Voice and Data calls simultaneously,
                   such that Data call is originated to carry voice
                   origination using VoIP. svd support is not checked for
                   this operation.
08/02/05   ic      Print value that caused error in default switch case
07/29/05   dk      Modified cmxcall_client_orig_proc to end the hdr call in
                   orig state when other non-hdr call attempt needs to be made.
07/26/05   pk      DCTM modifications, Ignoring NID for DCTM, a system
                   is uniquely identified by SID,PZID.
07/25/05   ic      Lint fixes in cmxcall_client_orig_proc()
07/21/05   sk      Removed is_jcdma_emergency flag from the cmd to MC.
                   Removed internal use of is_jcdma_emergency flag.
07/17/05   ic      Lint fixes in cmxcall_client_orig_proc()
07/14/05   ic      Removed is_user_originated_call from cdma_cmcall_type as
                   it is aready present in cmcall_s_type
07/09/05   ic      Changed assertion in cmxcall_set_highest_layer_orig_failure()
                   from INRANGE() to BETWEEN()
07/08/05   ic      Report highest layer call origination failure as part of
                   call events.
06/18/05   jqi     If 1x service is avail and data call is in traffic, force
                   mode to 1x+HDR upon ending the data switch over call.
06/15/05   ic      Removed dead code
06/09/05   ic      Cleaned up cmxcall_cmd_copy_fields()
06/06/05   pk      Reset the origination throttle table after OTAPA/OTASP call
                   Incoming PS_DATA call
06/03/05   ic      Mainlined FEATURE_DS_IS2000
05/05/05   jqi     More fixed the data switch over call.
04/18/05   ic      Added 4GV support, rewrote is_voice_srv_opt() using switch
                   statement.
                   Renamed is_voice_srv_opt() to cmxcall_is_voice_srv_opt()
04/07/05   ic      In cmxcall_info_get() added copying of is_jcdma_emergency
                   field
03/24/05   ic      Moved processing of CM_CALL_CMD_INFO_GET to cmcall.c
                   (CR#59955)
                   Renamed cmcall_client_cmd_forward_mc() to
                   cmxcall_client_cmd_forward_mc()
03/24/05   dk      Externalize cmxcall_send_end_cmd.
02/18/05   sj      Fixed issue of FADE_F deallocating the call_id without
                   EVENT_END when in state WAIT_END_CNF.
02/14/05   pk      Added processing of CM_CALL_EVENT_LINE_CTRL event
02/01/05   dk      Added WLAN Support.
01/24/05   ic      Replaced cmph_is_ph_in_emerg_cb_mode() with
                   cmph_is_in_emergency_cb()
01/10/05   sj      Adding call failure handling for no hdr service & no hdr
                   rf lock.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
01/05/05   sj      Fixed incorrect check for call state while handling
                   INCOMING_CALL_F report.
12/19/04   ic      Moved common call fields from cmxcall_assert() to cmcall.c
                   cmcall_assert()
12/14/04   sj      For QNC call, force srv_type to be CDMA_ONLY.
           jqi     Reject data call while in OTASP or OTAPA call.
11/11/04   sj      Rearranged featurization for DDTM_CNTL, added support
                   HDR call end cnf, moved featurization in
                   cmxcall_send_end_cmd()
11/18/04   dk      Add support to call cmxsms_abort_mo_msg().
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/10/04   dk      Allow Data call over 1X in PREV >6 while there is incoming
                   voice call is not yet in conversation. CR 51150
10/21/04   ic      Removed unnecessary lint -e774 comment
09/30/04   ws      Remove extra break statement from previous check-in.
                   Fix problem with continuing with invalid dial num.
09/14/04   ws      Remove calls to srch_rude_wakeup.
09/09/04   ws      Handle out-of-order events.
09/07/04   ka      Removed usage of is_orig_sent, mode_in_use and added
                   orig_sent_on_mode
09/01/04   ws      Send end command if just 1 of call ids in array need to.
08/20/04   sj      Added support for PRL Preference in sys_sel_pref().
08/19/04   dk      Changed cmxcall_client_orig_proc() to tear-down SMS call
                   only if we are not making PS_data call.
08/18/04   ka      Call direction now is common to system modes.
                   Setting call direction using a function.
08/12/04   ws      Remove cm20 support.
08/11/04   ka      System mode for a call is not updated when in NO_SERVICE.
08/09/04   ws      Save call id in cmcall_end_no_call_id_dealloc.
08/05/04   dk      Removed periodic check of UIM. Kick the GSDI during an
                   incoming call.
08/05/04   ic      Lint clean up - removed !e641
08/04/04   ic      CM Lint clean up - cmxcall.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
08/04/04   dk      Added support to hold Packet Data Call origination in HDR
                   if SMS/DBM is in progress.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmxcall_dummy() to get rid of ADS 1.2 compiler warning
07/21/04   ic/sj   Moved the call to call_ptr->ss =
                   cmcall_determine_ss_for_origination(...) from
                   cmxcall_client_orig_proc() in cmxcall.c to
                   cmcall_client_orig_proc() in cmcall.c so that
                   call_ptr->ss field gets correctly populated for GW calls.
07/21/04   sj      Added support for umts to 1x handover.
07/12/04   ic      Removed code under FEATURE_HDR_TEMP
07/06/04   ws      Wait for HDR call end before 1x origination.
07/06/04   dk      Added function map_cm_mode_to_hdrmc_mode().
07/01/04   dk      Added Call Object Iterator.
07/01/04   sj      Converted redirection /handoff orig failure to its own
                   end status.
06/30/04   ic      Fixed ADS 1.2 and RVCT 2.1 obvious compiler warnings
                   (Saber & Jaguar)
06/30/04   dk      Lint Cleanup.
06/29/04   sj/ic   In cmxcall_mc_rpt_proc() set call_ptr->sr.is_orig_sent
                   to FALSE on CM_CALL_ORIG_ERR_HOLD_ORIG (CR#45468)
06/25/04   dk      Modified call object so that call_obj_array just contains
                   the pointer to the call object. Moved all call relevent
                   information inside the call object.
                   Removed explicit call object allocation/de-allocation.
                   Now it is done as part of call_id allocate/deallocate.
                   Modified in general to support this new behavior.
                   Moved certain generic call obj array walk through functions
                   from cmxcall.c
06/22/04   ic      Fixed obvious RVCT 2.1 compiler warnings
06/22/04   ic      Formatting clean up
06/21/04   ic      Renamed cmxcall_is_there_a_hdrdata_call() to
                   cmxcall_is_hdr_call()
                   Modified cmxcall_is_hybr_hdr_call() to also return call id
                   of the HDR call (if any).
                   Renamed cmxcall_is_there_a_call_type() to cmxcall_find_call()
                   Fixed obvious Lint errors (58: Bad type, 715: variable
                   not referenced)
06/21/04   ank     Mainlined FEATURE_GPSONE_DBM
06/16/04   ic      cmxcall_client_orig_proc() clean up - instead of defining
                   HDR command etc by hand and sending it, call
                   cmxcall_send_end_cmd()
                   Added calls to cmcall_update_call_end_info() before
                   cmcall_event() is called for CM_CALL_EVENT_END event.
                   Set call_connect_time field in call object before cmcall_event()
                   with CM_CALL_EVENT_CONNECT and CM_CALL_EVENT_NDSS_CONNECT
                   is called.
06/10/04   dk      Intialized Call Substate IDLE during call reset and call end.
06/03/04   ws      Add support for redirecting number record.
05/28/04   ic      Replaced all p_rev_in_use <= 6 with comparisons to constant
                   CM_SS_P_REV_IN_USE_SVD_NOT_SUPPORTED
05/27/04   ic      Fixes so it compiles without FEATURE_HDR defined.
                   In cmxcall_client_cmd_proc() call cmcall_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
05/21/04   ic      In cmcall_mc_rpt_proc() removed buffering of messages that
                   may be sent to CM out of sequence:
                   - CM_ABRV_ALERT_F
                   - CM_EXT_DISPLAY_F
                   - CM_SIGNAL_F
                   - CM_CONNECTED_NUM_F
                   - CM_CALLING_PARTY_F
                   - CM_CALLED_PARTY_F
                   - CM_DISPLAY_F
05/21/04   ic      Modified cmxcall_end() to allow for disabling HDR
                   originations when a PS data call ended on 1X in origination
                   state (CR#43568)
05/12/04   ic      In cmxcall_client_orig_proc() changed location of
                   FEATURE_IS2000_REL_A flag to match that of 6500 4.X CM code
                   (//depot/asic/msmshared/services/cm.01.00.10.00/cmxcall.c#48)
05/06/04   ic      In cmxcall_send_orig() added assertions on
                   call_ptr->sr.is_orig_sent
04/29/04   ic      Added cmxcall_CDMA_orig_possible().
                   Changes in cmcall_hybr_redial() and cmcall_hybr_redial_on_1x()
                   to use cmxcall_CDMA_orig_possible().
                   Updated cmxcall_is_valid_mode_pref()
04/15/04   ws      Fixed compiler warning.
04/08/04   ws      Updated function names.
04/02/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.23 and MSMSHARED_CM.01.00.10.00.28
04/01/04   ws      Updated with favored_mode_pref changes.
04/01/04   ws      Allocate callobject for out of sequence events.
03/30/04   ic      Removed unnecessary comment from cmxcall_assert()
03/26/04   ic      Fix cmxcall_assert() so it does not check signal fields
                   without checking first whether signal info is available.
                   Fixed bad comment in cmxcall_is_there_a_hdrdata_call()
03/24/04   ic      Removed commented out declaration of
                   cm_unreserve_connection_id().
                   Featurization bug fixes.
03/18/04   sj      Block MO data calls if SMS/DBM is in progress.
03/02/04   ic      Fixed featurization compile errors
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/18/04   ws      Set the last record flag properly for CM_NSS_REL_F &
                   CM_NSS_AUD_CTRL_F
02/12/04   ic      Fixed compile error when FEATURE_GPSONE is turned off
                   Merged in changes from MSMSHARED_CM.01.00.10.00.20 to
                   MSMSHARED_CM.01.00.10.00.23
02/10/04   jq      Added cmss_proc_call_connected prototype.
02/10/04   ws      Added gsdi calls to indicate tc status.
02/09/04   ic      Changes so the code compiles for unit test.
02/06/04   ws      Initial jaguar/mainline merge.
02/03/04   ak      Fixed compilation error when FEATURE_GPSONE is not defined.
01/29/04   ws      Added gsdi calls to indicate tc status.
01/21/04   RI      Modified cmph_force_pref_on_the_fly() to include service
                   domain preference parameter.
01/19/04   ka      Added support for multiple PDP and get call info list api
01/13/04   ws      End an existing data call if an emergency call is originated.
12/11/03   ic      Replaced is_hybr_redialled with is_hybr_redialed
12/03/03   ic      In cmxcall_reset() added a call to initialize
                   call_ptr->calling_num field
11/19/03   ws      When checking Concurrent services check p_rev_in_use not bs_p_rev.
11/14/03   ws      Added support for is_jcdma_emergency
11/13/03   ws      Check uim status for both cards with dual slots
11/11/03   ws      Added support for nss_rel_rec type for t53
11/10/03   ws      Fix Rel A compilation error.
10/31/03   ws      Allow all srv_types for emergency calls.
10/27/03   ws      Check for p-rev for originations for PD/SMS/OTAPA calls.
10/20/03   ws      Dual Slot Support.
                   Added handling privacy_pref without using call ids.
10/17/03   ws      Fixed problem with NULL call pointer in cmxcall_is_pc_otapa
10/01/03   ws      Merged from GW Targets
                   Do not deallocate call id when answer command is invalid.
09/22/03   ws      Fix a problem with OTASP & PD calls.
09/11/03   ws      Fix a problem with building for RelA.
09/11/03   ws      Properly end OTAPA call during orig.
09/05/03   ws      Fix a RelA compilation problem.
                   Do not deallocate call id if no reply from MC.
                   Update MAX_MC_REPLY_TIME to 7.
08/29/03   ws      Only send sys_sel if the mode_pref is GSM only for
                   FEATURE_CM_PACKET_ORIG_CDMA_ONLY
08/29/03   ws      Correcting typo for previous Dual Slot support
08/29/03   ws      Added support for FEATURE_CM_PACKET_ORIG_CDMA_ONLY
08/26/03   ws      Removed RelA featurization for flash_type.  Aloowed deact
                   answer hold flash in conv state.
                   Fixed a type in checking for call type in origination.  For
                   rel0, rejecting data call if emergency call is already on.
                   Figured the call_id for OTASP/OTAPA commit.
08/22/03   vt      Removed hard-coding of dormant state for MC.
08/21/03   ws      Added support for Automatic mode and Silent ReDial Merge.
08/12/03   ws      Initialize calling num in cmxcall_reset
07/28/03   vt      Set sys mode on incoming call.
07/08/03   vt      Set sys mode for call events.
07/08/03   ws      Pre-empt SMS call if voice call origination.
07/02/03   ic      Allowed handling of CM_ABRV_ALERT_F, CM_EXT_DISPLAY_F and
                   CM_SIGNAL_F in idle
06/20/03   vt      Allowed handling of cm_display_f report in idle.
06/16/03   ws      Added checks for memcpy qos_parms to make sure no overflow
06/04/03   ic      Fixed compiler warnings:
                   - compiled out cmxcall_is_there_a_sms_call()
                   - put feature flag around cmd_mode_info_ptr variable
                   in cmxcall_client_cmd_mc_reply()
06/03/03   ic      For CM_CALL_CMD_END added copying of call_client_ptr and
                   end_reason_included / end_reason fields.
                   In cmxcall_info_get() added copying of end_reason_included /
                   end_reason fields.
05/28/03   vt      Added handling for cdma_hdr service type.
05/15/03   ic      Removed featurization around CMCALL_MAX_MC_REPLY_TIME
05/05/03   vt      Changed the size back in memory copy of subaddresses.
04/28/03   AT      Lint cleanup.
05/02/03   ws      Added checks for unassigned call ids &&
                   check for active voice call when originating data call in Rel0
04/25/03   ws      JCDMA support for call hold
04/07/03   ws      Added check for call_type PD in origination and end the PD call.
03/14/03   ws      Added more fields to initialize in cmxcall_reset
02/28/03   vt      Removed an unnecessary HDR error message.
02/27/03   ws      Changed the check for end_reason_included flag in end_params
02/27/03   ws      Added runtime checks for GW/1X calls.
                   Added EXIT_TC to generic para check
02/24/03   vt      Fixed a compile problem with HDR.
02/21/03   vt      Fixed problems to build for HDR.
02/13/03   prk     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/06/03   ws      Added FEATURE_CONC_SRVC_NEW_API support.
01/29/03   vt      Included sr_id_included field on origination to MC.
01/28/03   ws      Changed is_pkt_data_srv_opt() to check for SO when they are
                   also dependent on mobile origination or mobile termination.
01/24/03   ws      Merged changes for E911 calls to not tell searcher that origination
                   is pending.
01/21/03   ws      Updated copyright information for 2003
01/20/03   ak      Changed client_cmd_check to allow SO35/36 calls while in 911 CB state
                   only if FEATURE_ALLOW_IS801_IN_IDLE_EMERG_MODE is defined.
12/04/02   vt      Added Async service options for pkt data SO check to
                   support QNC.
12/03/02   ws      Modified INCOMING_CALL_F and ALERT_F so it will not allocate
                   a new call object if there is already one there.
11/20/02   vt      Removed de-allocating call id in connected report.
11/15/02   vt      Fixed compile errors for CM_CALL_FAIL_RETRY_ORDER_F report.
11/14/02   vt      Merged changes from GW mainline.
                   [ws]Merged in position determination position changes.
                   [ws]Merged skipped resetting orig error count if it is OTHER (not L3).
10/30/02   vt      Fixed compile errors.
10/28/02   ws      Added checks for call id in events received from MC
10/28/02   vt      Properly featurized/handled OTAPA call type.
10/22/02   ic      Removed #ifdef FEATURE_CM_SILENT_REDIAL / #else / #endif
                   block with silent redial parameters as those has been moved
                   to cmcall.h
10/22/02   vt      Removed usage of is_allocated & call_id in call object.
10/21/02   vt      Replaced checking of call id with call object.
10/18/02   vt      Fixed a compile error when OTASP is on.
10/09/02   vt      Decoupled call object reset from call id allocation.
                   Added mode param to cmcall_callobject_allocate().
10/05/02   vt      Allocated call object on client orig, analog alert, and
                   digital incoming calls. Set proper mode_info and ph_ptr
                   fields in call object.
09/30/02   ws      Added a cm_call_id_deallocate for some events if the call id
                   is allocated but if the state is idle to fix CR#24516.
09/24/02   ic      Replaced cmcall_end with cmcall_end_no_call_id_dealloc for
                   CM_ALERT_F.
09/05/02   vt      Fixed a compiler error with call end command to mc.
08/30/02   vt      Fixed a typo in an EMERGENCY enum, Filled in calling
                   number and subaddresses for MC, RTRE changes, &
                   Fix for flash command
08/30/02   vt      RTRE Changes.
08/07/02   vt      Fixed a link problem with is_pkt_data_srv_opt().
08/07/02   vt      Fixed compile problems when RUIM feature is defined.
08/06/02   HQ      Added FEATURE_CDSMS.
07/23/02   vt      Added is_cs_data_srv_opt().
06/13/02   ws      Changed include from cmmc.h to cmxll.h
05/28/02   ws      Added support for CM_CALL_END_ALREADY_IN_TC and CM_CALL_EVENT_EXIT_TC
07/11/02   PRK     Included cmll.h.
04/18/02   az      Added new voice service option SMV (SO number 56)
04/16/02   AP      Tell MC that they are no calling & called party sub-addresses
04/09/02   SH      Use normal redial instaed of optimized redial for HDR call
                   added is_hdr_not_avail flag for silent redial and
                   removed cmcall_is_in_hdr_call
03/26/02   RC      Fixed get_favored_roam_pref() to return the current roam
                   preference when call is originated while there is no
                   service.
03/21/02   sh      Check E911 call back mode first before setting favored pref for orig
03/13/02   sh      Renamed HDR related macros for HDR call status checking
03/10/02   sh      Separate HDR reports from MC reports; Safeguard HDR call
                   so it will not be ended accidentally by MC reports and vice versa;
                   Redial the HDR call when rcv connection deny with general or
                   netwrok busy.
03/05/02   pj      Send CM_CALL_END_USER_CALL_ORIG_DURING_GPS when ending a PD call
                   due to a user origination.
03/04/02   ic      Undid the fix to CR#20594 as it was determined that it was
                   not a CM problem after all.
03/01/02   AP      Added check: !defined (FEATURE_UIM_RUIM_W_GSM_ACCESS)
02/22/02   sh      Initialized is_hybr_allowed flag in cmcall_reset
02/21/02   ic      Fixed CR#20594
02/21/02   az      Featurized 13K voice service option
02/06/02   sh      Added hybrid 3.0 support
01/19/01   az      Fixed CR# 17624, Added a parameter to the calls to
                   cmph_force_mode_on_the_fly()
01/18/01   az      Fixed CR# 20086
                   Changed the Orig plan so that silent redial command is sent
                   even when AMPS is turned on for trimode. Changed
                   cmcall_orig_proc() so that roaming indicator is considered
                   for reverse link failure when
                   FEATURE_FAVOR_REV_LINK_ROAM_IND is defined.
01/17/02   ic      Added to the comment below a note about the fix for CR#17975
01/12/01   az      Changed the Orig plan so that silent redial command is sent
                   even when APMS is turned on. Added a new fuction call
                   get_favored_roam_ind(). Changed cmcall_force_auto_on_the_fly()
                   so that roaming indicator is considered for reverse link
                   failure when FEATURE_FAVOR_REV_LINK_ROAM_IND is defined.
                   Fixed CR#17975
12/14/01   sh      Fixed incorrect handling of acq fail for force collocated
                   call
12/11/01   sh      Added enhancements for dormant session handoff, prelimary
                   hybrid operation
12/08/01   sh      Added preliminary hybrid operation support
05/07/02   prk     Included cmxsms.h instead of cmsms.h
05/05/02   vt      Fixed compile problem with new mm cm.h
04/01/02   ic      In cmcall_event() and cmcall_event_generic() shortened
                   the string that prints CM commands so they fit in
                   the diagnostic monitor display
03/25/02   ic      Added
                   CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS,
                   CM_CALL_END_REJECTED_BY_BS,
                   CM_CALL_END_INCOMPATIBLE
                   to origination plan tables with action set to
                   CMCALL_ACT_END_ORIG as we want to abort further origination
                   attempts.
                   In cmcall_client_cmd_err() removed call_state variable as
                   it was not needed and caused compiler warnings.
                   In cmcall_mc_rpt_proc() removed call to cm_call_id_deallocate()
                   as it was not correct to call it there; changed the way
                   CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS,CM_CALL_END_REJECTED_BY_BS,
                   and CM_CALL_END_INCOMPATIBLE are handled.
03/18/02   pj      Send CM_CALL_END_USER_CALL_ORIG_DURING_GPS when ending a PD call.
03/08/02   ic      In cmcall_client_cmd_forward_mc(), set
                   mc_ptr->flash.digit_mode  = FALSE; //cmd_info_ptr->num.digit_mode;
                   mc_ptr->flash.number_plan = 0;     //cmd_info_ptr->num.number_plan;
                   mc_ptr->flash.number_type = 0;     //cmd_info_ptr->num.number_type;
                   as we need to pass those parameters from the UI and
                   current MSM UI (MSM_UI.05.04.72) does not do it yet.
                   Changes to how CM_CALL_END_EMERGENCY_FLASHED_F is handled,
                   set call_ptr->end_status = CM_CALL_END_EMERGENCY_FLASHED
03/07/02   ic      In cmcall_client_cmd_forward_mc() case CM_CALL_CMD_FLASH,
                   added copying of cmd_info_ptr->num.digit_mode,
                   cmd_info_ptr->num.number_plan and cmd_info_ptr->num.number_type
                   fields.
                   In cmcall_mc_rpt_proc() added handling of
                   CM_CALL_EMERGENCY_FLASHED_F command.
03/07/02   ic      Put cmcall_event_generic() under FEATURE_T53 to
                   fix compiler warning.
                   Added cmcall_end_no_call_id_dealloc() that allows the CM
                   to handle "alert with info" messages correctly.
                   Beautified comments for cm_out_of_sequence_msg_proc_function()
                   In cmcall_mc_rpt_proc(), changed the way CM_INCOMING_CALL_F
                   is processed so CM can handle "alert with info" messages.
03/06/02   ic      In cmcall_client_cmd_err() fixed the way errors are printed
                   when call id is not allocated.
                   In cmcall_mc_rpt_proc() for CM_DISPLAY_F case replaced
                   call_mode_info_ptr->is_last_cdma_info_rec
                      = cm_mc_rpt_ptr->called_party.hdr.last_record;
                   by
                   call_mode_info_ptr->is_last_cdma_info_rec
                      = cm_mc_rpt_ptr->display.hdr.last_record;
03/01/02   ic      In cmcall_event() added printing of call id
02/25/02   ic      In cmcall_mc_rpt_proc() added support for MC reports that
                   may be sent to CM out of sequence:
                   - CM_ABRV_ALERT_F
                   - CM_EXT_DISPLAY_F
                   - CM_SIGNAL_F
                   - CM_CONNECTED_NUM_F
                   - CM_CALLING_PARTY_F
                   - CM_CALLED_PARTY_F
                   - CM_DISPLAY_F
02/15/02   ic      In cm_call_id_deallocate(), added call to cmcall_reset().
                   In cmcall_client_cmd_err(), added printing of error details
                   and a call to cm_call_id_deallocate() when command is
                   in error.
                   In cmcall_client_cmd_check() changed error code from
                   CM_CALL_CMD_ERR_IN_USE_S toCM_CALL_CMD_ERR_CALL_STATE_S
                   when call object is not in idle state (as required).
                   In cmcall_end(), removed call to cmcall_reset() as it is
                   called in cm_call_id_deallocate() so it's not necessary to
                   call it from there now.
02/13/02   ic      Changed the type of call_id variable from byte/uint8 etc
                   to cm_call_id_type as it should have been to begin with.
02/07/02   ic      Changed the way call ids are allocated.
                   We used to allocate the first call id that was available.
                   Now we allocate call ids in round-robin fashion, from 0 to
                   CM_CALL_ID_MAX and then wrap around to 0 again.
02/06/02   ic      Changed the way CM_EXT_BRST_INTL_F is handled as now the
                   structure associated with it (cm_ext_brst_intl_type) does
                   not have a call id.
                   Added cmcall_event_generic()
02/01/02   ic      In cmcall_mc_rpt_proc(), made CM behavior consistent while
                   handling arrays of call id - all valid ones will be processed,
                   all invalid ones rejected and appropriate error message will
                   be printed.
01/29/02   ic      Removed the restriction of one voice + one data call
                   (under FEATURE_SVD_CM_CP_INTEGRATION)
01/23/02   ic      Fixes related to running with FEATURE_GPSONE_* enabled
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
01/14/02   ic      Fixes related to running with FEATURE_IS2000_REL_A_SVD undefined
                   Fixes related to running with FEATURE_IS2000_REL_A undefined
                   Fixes related to changes in mc_end_type enumeration
01/11/02   ic      Made is_voice() function global (i.e. independed of any
                   feature)
01/03/02   ic      Added ACCT (Access Control based on Call Type) support
                   Added silent redial support for
                   CM_CALL_END_NO_RESPONSE_FROM_BS call end reason.
12/20/01   ic      Under FEATURE_SVD_CM_CP_INTEGRATION, added support
                   for restricting the CM to one voice and one data call
                   at a time as this is what CP code currently supports.
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
11/30/01   sh      Added access network authentication support
11/20/01   sh      Fixed the mode conversion compatibilty problem for SD1.0
11/17/01   sh      Added 1xhdr data session handoff support
11/07/01   sh      Initialized is_force_colloc to False when call is ended
10/29/01   sh      Fixed various bug/deficiencies found from code review
10/29/01   sh      Added MSM5500 release 2.0 hybrid operation support
10/23/01   VT/IC   Baseline for CCS
10/03/01   VT      Added determining whether flash number is emergency or not.
09/20/01   SH      Allowed packet data call for async data service option
09/19/01   az      Set the flag fwd_link_failure according to whether its a
                   forward link failure or reverse link failure
09/11/01   WLI     Fixed cmnv_get_uim_status().
08/21/01   ic      Main-lined FEATURE_NEWCM
08/14/01   WLI     With RUIM read SPC status from RUIM instead of from NV.
08/03/01   RC      Fixed HDR originations.
08/02/01   JRP     Added support for SO54, SO55.
08/01/01   PJ      mode_pref changes for CALL_TYPE_PD when in 911CB state.
06/28/01   SH      Backed out HDR related call end status.
06/26/01   HQ      Abort GPS data burst call when a new non-GPS call is
                   originated.
06/21/01   HQ      Correctly set is_re_orig and orig_count in origination
                   for FEATURE_IS2000_REL_A.
05/25/01   CK      Changed the interface for R-UIM from Auth task to
                   UIM task.
05/16/01   SH      Allowed call to end in the alerting state
04/13/01   SH      Added Preliminary HDR support
03/27/01   RC      Added SD 2.0 trimode support.
03/14/01   RC      Now only calling cmcall_orig_proc when receiving
                   CM_CALL_ORIG_FAIL_F if call-end status != NONE.
03/12/01   PJ      Added GPSONE_DBM ifdefs for previous change.
03/11/01   PJ      Chaned client_cmd_check to allow PD calls while in 911 CB state.
03/08/01   RC      Fix a problem with origination failure status.
03/06/01   HQ      Added support of FEATURE_IS2000_REL_A.
02/23/01   CS      Do not end the existing call if there is an alert with/info
                   event in AMPS and the current state is INCOM. Use the CLI
                   in the CM_ALERT_F message to overwrite the exsiting CLI.
02/23/01   HQ      Do real-time R-UIM query during origination.
02/22/01   SH      Fixed the race problem between incoming OTAPA and User Orig
02/21/01   RC      Eliminated use of T_SLEEP flag.
02/16/01   SH      added support for MC_END_F ack
02/07/01   RC      Added support for System Determination 2.0.
01/29/01   HQ      Properly feature-ized NEWCM, UASMS and GPSONE.
01/26/01   ych     Fixed a Compile Problem
01/23/01   ych     Merged T53 and JCDMA features.
01/23/01   ejv     Mainlined T_SLEEP.
12/28/00   fc      Added support for retry order.
12/15/00   HQ      Added processing of CM_CALL_CMD_PRIVACY_PREF.
12/12/00   YLL     Fixed CR 15322 by unforcing the temp analog call if user
                   originates a STD_OTASP call (*228xx).
10/24/00   HD      Added support for Full TDSO.
10/06/00   HD      Added support to pass the DRS bit setting to CP.
10/09/00   HQ      Added RUIM in-call polling support.
09/15/00   PJ      Added support for position determination call type (SO 35/36).
08/23/00   vt      Added new CM interface changes.
07/18/00   CS      Do not reset the NDSS flags for CM_CALL_ORIG_FAIL_F in ORIG state.
07/18/00   jq      Added support for end_reason
           HD      Added support for TDSO
07/11/00   CS      Reject FLASH command in INCOMING CALL state and in AMPS mode for
                   call forwarding.
07/07/00   HD      Only block Digital calls in case of CDMA_LOCK mode in orig process.
06/14/00   HD      Do not reset the NDSS flags when FADE or REORDER to fix bug.
05/22/00   VT      Modified cmcall_force_auto_on_the_fly() to set home_only to FALSE
                   for reverse link failure.
03/29/00   VT      Fixed a bug by changing macro from BETWEEN to INRANGE for OTASP.
03/27/00   HD      Added support for SO33
03/21/00   CS      Changed PRIVACY_MODE to VOICE_PRIVACY. MC does not set PRIVACY_MODE
03/19/00   RY      Added CM_END_ACTIVATION_F processing to cmcall_mc_rpt_proc()
           RY      Removed an extra cm_kick_dog from cmcall_client_cmd_check()
           RY      Updated cmcall_client_cmd_forward_mc() to FATAL if
                   cm_hold_cmd_alloc() fails
           RY      Removed CM_CMD_TYPE_PH from cm_hold_cmd_handle()
           RY      Removed cm_flush_hold_cmd_q()
03/13/00   RY      Added OTAPA support
03/03/00   CS      Ended the incoming OTAPA call if there is a call origination.
02/03/00   VT      Fixed a compiler warning (is_sufficient_srv) for PCS build.
02/01/00   VT      For voice calls, suffiecnt service check is not performed.
01/24/00   VT      Fixed a compile problem with MAX_ACCESS_PROBE for non-trimode.
01/21/00   VT      CR12537 - Accepts NDSS FAILURE in all cases in ORIG state.
01/20/00   VT      Added support for VOICE_13K_IS733.
01/20/00   VT      Fixed a bug with MAX_ACCESS_PROBE error handling.
12/18/99   CS      Added cm_kick_dog before/after calling nc callback functions.
12/13/99   IT      Added handling for next data call as data.
12/03/99   VT      Merged TriMode Code.
12/01/99   CS      Fixed CR12299 - MC out of buffer with repeated SEND and END. Do not get
                   MC buffer for END CALL command if call state is origination
                   and the the origination request has not been sent to the MC yet.
11/04/99   SH      CR11880 - Fixed a problem with NDSS.
10/19/99   SH      Fixed CR11844 - to NDSS connect or not to UI
09/24/99   SH      Added FEATURE_ACP for CM_N_VC_CLI_F & CM_N_MSG_F
09/14/99   SH      Removed CM_MODE_PREF_ANALOG_PREF and CM_MODE_PREF_DIGITAL_PREF
09/01/99   SH      Modified codes to featurize OTASP
08/31/99   SH      Added online and serving state checking for call related commands
08/25/99   SH      Modified the algorithm for selecting SO for call orig
08/24/99   SH      Added release reason for Base Station release order
08/19/99   SH      Modified processing of call orig failure for hold orig
                   or access failure
08/19/99   SH      Added processing for access failure of call orig
08/09/99   SH      Added alert answer bypass feature
08/06/99   SH      Added SMS support
07/29/99   SH      Set a_data field for MC_ANSWER_F
07/07/99   SH      Added CNAP feature
06/30/99   SH      Added checking for OTASP calls
06/21/99   SH      Added checking for cmd_err in the cmd info
05/31/99   SH      Added CM 2.0 features
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
12/08/98   RC      Changes per second week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */

#if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900))
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "sys.h"       /* System wide definitions */
#include "sys_v.h"
#include "cmcall.h"    /* Interface to CM call object */
#include "cmxcall.h"   /* Interface to CM CDMA call object */
#include "comdef.h"    /* Definition for basic types and macros */
#include <string.h>    /* Interface to string library */
#include "cmtaski.h"    /* CM task interface */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmph.h"      /* Interface to CM phone object */
#include "cmaccessctrl.h"
#include "cmss.h"      /* Interface to CM serving system object */
#include "cai.h"       /* Over the air messages definitions */
#include "cai_v.h"
#include "mc.h"        /* Interface to MC Task */
#include "mc_v.h"
#include "cmxll.h"      /* MC to CM interface*/
#include "db.h"        /* Interface to Database services */
#include "time_svc.h"       /* Interface to clock services */
#include "cmxsms.h"    /* Interface to SMS object */
#include "cmnv.h"
#include "cmll.h"      /* Interface to cm_call_id_allocate/deallocate */
#include "cmll_v.h"
#include "cmreply.h"
#include "cmdbg.h"
#include "cm_msgr_msg.h"
#include "cmxdbm.h"
#include "cmmmgsdi.h"


#ifdef FEATURE_OTASP
#include "diag.h"      /* Interface to DIAG task */
#endif /* FEATURE_OTASP */

#include "sd.h"        /* Interface to System Determination services */
#include "sd_v.h"
#include "prl.h"       /* Interface to PRL services */
#include "prl_v.h"

#ifdef FEATURE_UIM_RUIM
#include "uim.h"       /* Interface to R-UIM services */
#include "uim_v.h"
#endif

#ifdef FEATURE_GPSONE
#include "cmxpd.h"
#endif /* FEATURE_GPSONE */

#ifdef FEATURE_HDR_HYBRID
#include "hdrcp_msg.h"
#endif  /* FEATURE_HDR_HYBRID */

#ifdef FEATURE_MMODE_SC_SVLTE
#include "mmoc_v.h"
#endif

#include "cmlog.h"
#include "cmlog_v.h"

#ifdef FEATURE_HDR_HANDOFF
#include "cmssidm.h"
#endif  /* FEATURE_HDR_HANDOFF */

#ifdef FEATURE_IP_CALL
#include"cmipcall.h"
#endif

#if defined (FEATURE_LTE_TO_1X)
#include "cmsds.h"
#include "cmcsfbcall.h"
#endif

#include "cmutil.h"

#include "cmefs.h"

#ifdef CM_DEBUG
#error code not present
#endif
#include "cmemgext.h"

#include "cmmsc.h"
#include "cmmsc_auto.h"
#include "cmsimcoord.h"
#include "cmsoa.h"
#include "cmsds.h"
#include "cmregprxi.h"
#include "cmdrvcc.h"
#include "wms.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                         FORWARD DECLARATIONS
---------------------------------------------------------------------------*/

static void cmxcall_set_highest_layer_orig_failure(cmcall_s_type*,cm_call_orig_fail_layer_e_type);
#ifdef FEATURE_MMODE_SC_SVLTE
static boolean cmxcall_is_waiting_lpm_oprt_mode_reply(void);
#endif

/*---------------------------------------------------------------------------
                         TUNABLE CALL PARAMETERS
---------------------------------------------------------------------------*/
/*lint --e{750} */
#define CMCALL_MAX_MC_REPLY_TIME          7    /* seconds */
    /* Upper bound on how long a client call-command waits for a reply
    ** from MC. After such time the command considered failed due to
    ** communication problems with MC  */

#ifdef FEATURE_OTASP
#define CMCALL_MAX_OTASP_COMMIT_TIME      10  /* seconds */
    /* Upper bound on how long CM shall waits for a reply from MC when
    ** an OTASP commit is in progress. */
#endif /* FEATURE_OTASP */



/*---------------------------------------------------------------------------
                     TUNABLE SILENT REDIAL PARAMETERS
---------------------------------------------------------------------------*/


/* Silent redial feature to silently redial failed calls or to ride through
** limited periods of no service.
**
** In addition, silent redial can be configured to forced the mode
** preference over to analog if such service available and if calls fail
** under poor CDMA RSSI conditions. This force-over to analog is temporary,
** ending at call teardown.
**
** NOTE! generally the following relation should be maintained
** for the silent redial parameters (assuming MAX_DIALS >= 1):
**
** ((MAX_DIALS-1) * PERIOD_TIME) <= MAX_TIME
*/

#ifdef FEATURE_UIM_RUIM
#define CMCALL_RUIM_POLL_TIME      2
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================
                          FUNCTION PROTOTYPES
===========================================================================*/

#ifdef FEATURE_HDR
/*lint -esym(526, cmss_proc_call_connected) */
extern  void cmss_proc_call_connected(const cmcall_s_type  *call_ptr );
#endif /* FEATURE_HDR */

/*===========================================================================
=============================================================================
======================= INTERNAL MISCELLANEOUS FUNCTIONS ====================
=============================================================================
===========================================================================*/


/* Note that miscellaneous functions (as apposed to call object functions)
** do NOT take a pointer to a call object as their first parameter.
** As a result these functions aren't capable of changing the call state
** nor do they have a lasting effect beyond their returned value.
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Local Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enum stores whether a call is mobile originated (mo) or mobile
** terminated (mt).  It is primarily used for checking and determining
** whether a call type is valid for certain service options.
*/
typedef enum cmxcall_mo_or_mt_e
{
   CMXCALL_MOBILE_ORIGINATED,
      /* Call is mobile originated */

   CMXCALL_MOBILE_TERMINATED
      /* Call is mobile terminated */
} cmxcall_mo_or_mt_e_type;

/* This structure will allow CM to process MC reports
   that arrive out of sequence.

   We need to save the data that arrived inside of the
   out-of-sequence message and then resend it once we
   see the CM_INCOMING_CALL_F.

*/
typedef struct cmxcall_out_of_order_msg_s {

  /* Flag to indicate that CM_CALLING_PARTY_F was saved */
  boolean is_calling_party_flag;

  /* Buffer to save the CM_CALLING_PARTY_F message */
  cm_calling_party_type calling_party_buffer;

  /* Flag to indicate that CM_CALLED_PARTY_F was saved */
  boolean is_called_party_flag;

  /* Buffer to save the CM_CALLED_PARTY_F message */
  cm_called_party_type called_party_buffer;

  /* Flag to indicate that CM_REDIRECTING_NUM_F was saved */
  boolean is_redirecting_number_flag;

  /* Buffer to save the CM_REDIRECTING_NUM_F message */
  cm_redirecting_number_type redirecting_number_buffer;

  /* Flag to indicate that CM_SIGNAL_F was saved */
  boolean is_signal_flag;

  /* Buffer to save the CM_SIGNAL_F message */
  cm_signal_type signal_buffer;

  /* Flag to indicate that CM_DISPLAY_F was saved */
  boolean is_display_flag;

  /* Buffer to save the CM_DISPLAY_F message */
  cm_display_type display_buffer;

  /* Flag to indicate that CM_ABRV_ALERT_F was saved */
  boolean is_abrv_flag;

  /* Buffer to save the CM_ABRV_ALERT_F message */
  cm_abrv_alert_type abrv_buffer;

  /* Flag to indicate that CM_EXT_DISPLAY_F was saved */
  boolean is_ext_disp_flag;

  /* Buffer to save the CM_EXT_DISPLAY_F message */
  cm_ext_disp_type ext_disp_buffer;

  /* Flag to indicate that CM_CONNECTED_NUM_F was saved */
  boolean is_connected_num_flag;

  /* Buffer to save the CM_CONNECTED_NUM_F message */
  cm_connected_num_type connected_num_buffer;

} cmxcall_out_of_order_msg_s_type;


/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/

#if (defined(FEATURE_OTASP) && !defined(FEATURE_MMODE_REMOVE_1X))

/*===========================================================================

FUNCTION otasp_code_cm2mc

DESCRIPTION
  Translates CM enum to MC enum for OTASP activation codes

DEPENDENCIES
  none

RETURN VALUE
  MC enum

SIDE EFFECTS
  none

===========================================================================*/
static mc_activate_code_type otasp_code_cm2mc
(
  cm_activate_code_e_type  cm_code
)
{
  mc_activate_code_type mc_code = PREFERRED; /* For the compiler warning */
  if ( cm_code == CM_ABAND )
  {
    mc_code = ABAND;
  }
  else if ( cm_code == CM_BBAND )
  {
    mc_code = BBAND;
  }
  else if ( cm_code == CM_ABLOCK )
  {
    mc_code = ABLOCK;
  }
  else if ( cm_code == CM_BBLOCK )
  {
    mc_code = BBLOCK;
  }
  else if ( cm_code == CM_CBLOCK )
  {
    mc_code = CBLOCK;
  }
  else if ( cm_code == CM_DBLOCK )
  {
    mc_code = DBLOCK;
  }
  else if ( cm_code == CM_EBLOCK )
  {
    mc_code = EBLOCK;
  }
  else if ( cm_code == CM_FBLOCK )
  {
    mc_code = FBLOCK;
  }
  else if ( cm_code == CM_PREFERRED )
  {
    mc_code = PREFERRED;
  }
  else
  {
    mc_code = ( mc_activate_code_type ) cm_code; /* Should never happen! */
  }

  return( mc_code );
}
#endif /* FEATURE_OTASP */

/*===========================================================================

FUNCTION cmxcall_is_valid_mode_pref

DESCRIPTION
  Check whether a mode preference is valid relative to the call type.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if mode preference is valid relative to the call type,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmxcall_is_valid_mode_pref(

    cm_call_type_e_type    call_type,
        /* call type */

    cm_mode_pref_e_type    mode_pref
        /* mode_pref */
)
{
  switch( call_type )
  {
    /* The following mode preferences can be used in conjunction
    ** with VOICE or OTASP call type.
    */
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_NON_STD_OTASP:
    case CM_CALL_TYPE_STD_OTASP:
      if(cmcall_misc_is_mode_pref(mode_pref, CM_MODE_PREF_CDMA_AMPS_HDR_ONLY))
      {
        return TRUE;
      }

      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following mode preferences can be used in conjunction
    ** with DATA call type.
    */
    case CM_CALL_TYPE_CS_DATA:
    case CM_CALL_TYPE_PS_DATA:
      switch( mode_pref )
      {
        case CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY:
        case CM_MODE_PREF_DIGITAL_ONLY:
        case CM_MODE_PREF_CDMA_ONLY:
        case CM_MODE_PREF_CDMA_HDR_ONLY:
        case CM_MODE_PREF_HDR_ONLY:
        case CM_MODE_PREF_CDMA_HDR_GW:
        case CM_MODE_PREF_CDMA_GW:
        case CM_MODE_PREF_CDMA_LTE_ONLY:
        case CM_MODE_PREF_CDMA_GSM_LTE:
        case CM_MODE_PREF_HDR_GSM_LTE:
        case CM_MODE_PREF_CDMA_WCDMA_LTE:
        case CM_MODE_PREF_HDR_WCDMA_LTE:
        case CM_MODE_PREF_CDMA_HDR_GSM:
        case CM_MODE_PREF_CDMA_GSM:
        case CM_MODE_PREF_CDMA_HDR_GSM_WCDMA_LTE:
        case CM_MODE_PREF_CDMA_HDR_GSM_LTE:
        case CM_MODE_PREF_CDMA_GSM_WCDMA_LTE:
        case CM_MODE_PREF_HDR_TDS_WCDMA:
        case CM_MODE_PREF_HDR_TDS_WCDMA_LTE:
          return TRUE;
        default:
          break;
      }

      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following mode preferences can be used in conjunction
    ** with SMS or TEST call type.
    */
    case CM_CALL_TYPE_SMS:
    #ifdef FEATURE_GPSONE
    case CM_CALL_TYPE_PD:
    #endif /* FEATURE_GPSONE */
    case CM_CALL_TYPE_TEST:
      switch( mode_pref )
      {
        case CM_MODE_PREF_AUTOMATIC:
        case CM_MODE_PREF_DIGITAL_ONLY:
        case CM_MODE_PREF_CDMA_ONLY:
        case CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY:
        case CM_MODE_PREF_CDMA_HDR_GW:
        case CM_MODE_PREF_CDMA_GW:
        case CM_MODE_PREF_CDMA_LTE_ONLY:
        case CM_MODE_PREF_CDMA_GSM_LTE:
        case CM_MODE_PREF_CDMA_WCDMA_LTE:
        case CM_MODE_PREF_CDMA_HDR_GSM:
        case CM_MODE_PREF_CDMA_GSM:
        case CM_MODE_PREF_CDMA_HDR_GSM_WCDMA_LTE:
        case CM_MODE_PREF_CDMA_HDR_GSM_LTE:
        case CM_MODE_PREF_CDMA_GSM_WCDMA_LTE:
        case CM_MODE_PREF_HDR_TDS_WCDMA:
        case CM_MODE_PREF_HDR_TDS_WCDMA_LTE:
          return TRUE;
        default:
          break;
      }

      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following mode preferences can be used in conjunction
    ** with E911 call type.
    */
    case CM_CALL_TYPE_EMERGENCY:
      switch( mode_pref )
      {
        case CM_MODE_PREF_EMERGENCY:
        case CM_MODE_PREF_AMPS_ONLY:
        case CM_MODE_PREF_DIGITAL_ONLY:
        case CM_MODE_PREF_CDMA_ONLY:
        case CM_MODE_PREF_CDMA_AMPS_ONLY:
        case CM_MODE_PREF_AUTOMATIC:
        case CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY:
        case CM_MODE_PREF_ANY_BUT_HDR:
        case CM_MODE_PREF_CDMA_HDR_ONLY:
        case CM_MODE_PREF_CDMA_AMPS_HDR_ONLY:
        case CM_MODE_PREF_HDR_ONLY:
        case CM_MODE_PREF_CDMA_HDR_GW:
        case CM_MODE_PREF_CDMA_GW:
        case CM_MODE_PREF_CDMA_LTE_ONLY:
        case CM_MODE_PREF_CDMA_HDR_GSM:
        case CM_MODE_PREF_CDMA_GSM:
        case CM_MODE_PREF_CDMA_GSM_LTE:
        case CM_MODE_PREF_HDR_GSM_LTE:
        case CM_MODE_PREF_CDMA_WCDMA_LTE:
        case CM_MODE_PREF_HDR_WCDMA_LTE:
        case CM_MODE_PREF_CDMA_HDR_GSM_WCDMA_LTE:
        case CM_MODE_PREF_CDMA_HDR_GSM_LTE:
        case CM_MODE_PREF_CDMA_GSM_WCDMA_LTE:
        case CM_MODE_PREF_HDR_TDS_WCDMA:
        case CM_MODE_PREF_HDR_TDS_WCDMA_LTE:
        case CM_MODE_PREF_CDMA_HDR_LTE_ONLY:
          return TRUE;
        default:
          break;
      }

      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_TYPE_VT:
    case CM_CALL_TYPE_VS:
      return FALSE;

    default:

      CM_ERR_1( "Unexpected call type %d ", (uint8)call_type);

      return FALSE;   /*lint !e527 */

  } /* switch */

}


/*===========================================================================

FUNCTION cmxcall_is_voice_srv_opt

DESCRIPTION
  Check whether a specified service option is a VOICE service option.


DEPENDENCIES
  none

RETURN VALUE
  TRUE if specified service option is a VOICE service option,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmxcall_is_voice_srv_opt
(
  word    cai_srv_opt
    /* CAI service option to check
    */
)
{
  switch(cai_srv_opt)
  {
    case CAI_SO_VOICE_IS96A:
    case CAI_SO_VOICE_13K:
    case CAI_SO_VOICE_13K_IS733:
    case CAI_SO_VOICE_EVRC:
    case CAI_SO_VOICE_SMV:
    case CAI_SO_VOICE_WILDCARD:
    case CAI_SO_VOICE_4GV_NB:
    case CAI_SO_VOICE_4GV_WB:
    case CAI_SO_VOICE_EVRC_NW:
    case CAI_SO_VOICE_IS96:
      CM_MSG_MED_1("Input: cai_srv_opt = %d", cai_srv_opt);
      return TRUE;

    default:
      return FALSE;
  }
}


/*===========================================================================

FUNCTION is_pkt_data_srv_opt

DESCRIPTION
  Check whether a specified service option is a Packet Data service option.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if specified service option is a Packet Data service options,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean is_pkt_data_srv_opt(

    word    srv_opt,
        /* service option to check */

    cmxcall_mo_or_mt_e_type call_origination
        /* where the call originated */
)
{

  switch( srv_opt )
  {
    /* 8k pre IS707 data service options.
    */
    ////case CAI_SO_ASYNC_DATA_PRE707:
    ////case CAI_SO_G3_FAX_PRE707:
    case CAI_SO_PPP_PKT_DATA_PRE707:

    /* 13k pre IS707 data service options.
    ** Also being user for 13k IS707 - check with data folks????
    */
    ////case CAI_SO_ASYNC_DATA_13K_PRE707:
    ////case CAI_SO_G3_FAX_13K_PRE707:
    case CAI_SO_PPP_PKT_DATA_13K_PRE707:

    /* 8k IS707 data service options.
    */
    ////case CAI_SO_ASYNC_DATA_IS707:
    ////case CAI_SO_G3_FAX_IS707:
    case CAI_SO_PPP_PKT_DATA_IS707:

    /* QC proprietary service options
    */
    case CAI_SO_PPP_PKT_DATA_13K_PROPTRY:
    ////case CAI_SO_ASYNC_DATA_13K_PROPTRY:
    ////case CAI_SO_G3_FAX_13K_PROPTRY:

    /* QC HACK for data service option (packet method).
    */
    case CAI_SO_DATA:

    /* MDR service options  */
    case CAI_SO_MDR_PKT_DATA_FRS1_RRS1:
    case CAI_SO_MDR_PKT_DATA_FRS1_RRS2:
    case CAI_SO_MDR_PKT_DATA_FRS2_RRS1:
    case CAI_SO_MDR_PKT_DATA_FRS2_RRS2:
    case CAI_SO_MDR_DPT_FRS1_RRS1:
    case CAI_SO_MDR_DPT_FRS2_RRS2:
    /* 3G packet data service option */
    case CAI_SO_PPP_PKT_DATA_3G:
    #ifdef FEATURE_HDR_HYBRID
    case CAI_SO_HDR_PKT_DATA:
    #endif  /* FEATURE_HDR_HYBRID */

      return TRUE;

    /* Async service options for pkt data to support QNC
    */
    /* These service options are valid whether a call is mobile terminated
    ** or mobile originated.
    */
    case  CAI_SO_ASYNC_DATA_PRE707:                                /* SO 4 */
    case  CAI_SO_ASYNC_DATA_IS707:                              /* SO 4100 */
    case  CAI_SO_ASYNC_DATA_13K_PRE707:                           /* SO 12 */
    case  CAI_SO_ASYNC_DATA_13K_PROPTRY:                      /* SO 0x8021 */
      if (call_origination == CMXCALL_MOBILE_ORIGINATED)
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }

    default:

      return FALSE;
  }
} /* is_pkt_data_srv_opt() */



/*===========================================================================

FUNCTION is_cs_data_srv_opt

DESCRIPTION
  Check whether a specified service option is a circuit switched service option.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if specified service option is a circuit switched Data service options,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean is_cs_data_srv_opt(

    word    srv_opt
        /* service option to check */
)
{
  switch( srv_opt )
  {
    /* 8k pre IS707 data service options.
    */
    case CAI_SO_ASYNC_DATA_PRE707:
    case CAI_SO_G3_FAX_PRE707:

    /* 13k pre IS707 data service options.
    ** Also being user for 13k IS707 - check with data folks????
    */
    case CAI_SO_ASYNC_DATA_13K_PRE707:
    case CAI_SO_G3_FAX_13K_PRE707:

    /* 8k IS707 data service options.
    */
    case CAI_SO_ASYNC_DATA_IS707:
    case CAI_SO_G3_FAX_IS707:

    /* QC proprietary service options
    */
    case CAI_SO_ASYNC_DATA_13K_PROPTRY:
    case CAI_SO_G3_FAX_13K_PROPTRY:
      return TRUE;

    default:
      return FALSE;
  }

}


/*===========================================================================

FUNCTION is_data_srv_opt

DESCRIPTION
  Check whether a specified service option is a DATA service options.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if specified service option is a DATA service options,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
#ifdef CM_DEBUG
#error code not present
#endif

/*===========================================================================

FUNCTION is_sms_srv_opt

DESCRIPTION
  Check whether a specified service option is a SMS service option.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if specified service option is a SMS service option,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean is_sms_srv_opt
(
    word    srv_opt                             /* service option to check */
)
{
  switch( srv_opt )
  {
    case CAI_SO_SMS:
    case CAI_SO_RS2_SMS:

      return TRUE;

    default:

      return FALSE;
  }


}

/*===========================================================================

FUNCTION is_pd_srv_opt

DESCRIPTION
  Check whether a specified service option is a PD service option.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if it is a PD srv option, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

static boolean is_pd_srv_opt
(
  word srv_opt
    /* The service option to check */
)
{
  #ifdef FEATURE_GPSONE
  switch(srv_opt)
  {
    case CAI_SO_RS1_PD:
    case CAI_SO_RS2_PD:
      return TRUE;
    default:
      return FALSE;
  }
  #else
  SYS_ARG_NOT_USED(srv_opt);
  return FALSE;
  #endif
}

/*===========================================================================

FUNCTION CMCALL_IS_1x_DATA_SRV_OPT

DESCRIPTION
  Check whether a specified service option is a 1x Data service option.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if specified service option is a 1x Data service option,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_1x_data_srv_opt
(
  word    srv_opt                               /* service option to check */
)
{
  switch( srv_opt )
  {
    case CAI_SO_FULL_TDSO:
    case CAI_SO_PPP_PKT_DATA_3G:
      return TRUE;

    default:
      return FALSE;
  }
}


/*===========================================================================

FUNCTION IS_OTAPA_SRV_OPT

DESCRIPTION
  Check whether a specified service option is a OTAPA service option.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if specified service option is a OTAPA service option,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean is_otapa_srv_opt
(
  word    srv_opt                               /* service option to check */
)
{
  switch( srv_opt )
  {
    case CAI_SO_RS1_OTAPA:
    case CAI_SO_RS2_OTAPA:
      return TRUE;

    default:
      return FALSE;
  }
}


/*===========================================================================

FUNCTION IS_TEST_SRV_OPT

DESCRIPTION
  Check whether a specified service option is a TEST service option.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if specified service option is a TEST service option,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean is_test_srv_opt
(
  word    srv_opt                               /* service option to check */
)
{
  switch( srv_opt )
  {
    /* Test service options.
    */
    case CAI_SO_LOOPBACK:
    case CAI_SO_MARKOV:
    case CAI_SO_RS1_MARKOV:

    case CAI_SO_LOOPBACK_13K:
    case CAI_SO_MARKOV_13K:
    case CAI_SO_RS2_MARKOV:

   /* MDR Test service options */
    case CAI_SO_MDR_DPT_FRS1_RRS1:
    case CAI_SO_MDR_DPT_FRS2_RRS2:
#ifdef FEATURE_IS2000_TDSO
    case CAI_SO_TDSO:
    case CAI_SO_FULL_TDSO:
#endif  /* FEATURE_IS2000_TDSO */
#ifdef FEATURE_IS2000
    case CAI_SO_MARKOV_SO54:
    case CAI_SO_LOOPBACK_SO55:
#endif  /* FEATURE_IS2000 */
#ifdef FEATURE_IS2000_1X_ADV
    case CAI_SO_MARKOV_SO74:
    case CAI_SO_LOOPBACK_SO75:
#endif /* FEATURE_IS2000_1X_ADV */
      return TRUE;

    default:

      return FALSE;
  }

}



/*===========================================================================

FUNCTION cmxcall_is_valid_srv_type

DESCRIPTION
  Check whether a service type is valid relative to the call type.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if service type is valid relative to the call type,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmxcall_is_valid_srv_type(

    cm_call_type_e_type    call_type,
        /* call type */

    cm_srv_type_e_type     srv_type
        /* service type */
)
{

  switch( call_type )
  {
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_NON_STD_OTASP:

      /* The following service types can be used in conjunction
      ** with VOICE, E911, Non-standard OTASP
      */
      switch( srv_type )
      {
        case CM_SRV_TYPE_AMPS:
        case CM_SRV_TYPE_CDMA_SPECIFIC:
        case CM_SRV_TYPE_CDMA_AUTOMATIC:
        case CM_SRV_TYPE_AUTOMATIC:
    case CM_SRV_TYPE_CS_ONLY:
          return TRUE;
        default:
          break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_TYPE_EMERGENCY:
    case CM_CALL_TYPE_STD_OTASP:
    case CM_CALL_TYPE_VT:

      /*
      ** Allow any srv types for Emergency and Otasp calls and VT call.
      */
      return TRUE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_TYPE_OTAPA:
      switch( srv_type )
      {
        case CM_SRV_TYPE_CDMA_SPECIFIC:
          return TRUE;
        default:
          break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_TYPE_CS_DATA:
    case CM_CALL_TYPE_PS_DATA:

      /* The following service types can be used in conjunction
      ** with DATA call type.
      */
      switch( srv_type )
      {
        case CM_SRV_TYPE_AMPS:
        case CM_SRV_TYPE_CDMA_SPECIFIC:
        case CM_SRV_TYPE_CDMA_HDR:

        #ifdef FEATURE_HDR
        case CM_SRV_TYPE_HDR:
        case CM_SRV_TYPE_HDR_MORE_PREF:
        #endif  /* FEATURE_HDR */

        #ifdef FEATURE_HDR_HYBRID
        case CM_SRV_TYPE_AUTOMATIC:
        #endif /* FEATURE_HDR_HYBRID */

          return TRUE;

        default:
          break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_GPSONE
    case CM_CALL_TYPE_PD:
#endif  /* FEATURE_GPSONE */

    case CM_CALL_TYPE_SMS:

      /* The following service types can be used in conjunction
      ** with SMS call type.
      */
      switch( srv_type )
      {
        case CM_SRV_TYPE_AUTOMATIC:
        case CM_SRV_TYPE_CDMA_AUTOMATIC:
        case CM_SRV_TYPE_CDMA_SPECIFIC:
        case CM_SRV_TYPE_CDMA_HDR:
          return TRUE;
        default:
          break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_TYPE_TEST:

      /* The following service types can be used in conjunction
      ** with TEST call type.
      */
      switch( srv_type )
      {
        case CM_SRV_TYPE_CDMA_SPECIFIC:
        case CM_SRV_TYPE_CDMA_HDR:
          return TRUE;
        default:
          break;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      CM_ERR_0( "Unexpected call type");

  } /* switch */

  return FALSE;

} /* cmxcall_is_valid_srv_type() */




/*===========================================================================

FUNCTION cmxcall_is_valid_srv_opt

DESCRIPTION
  Check whether a service option is valid relative to the call type.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if service option is valid relative to the call type,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmxcall_is_valid_srv_opt(

    cm_call_type_e_type    call_type,
        /* call type */

    word                   srv_opt,
        /* service option */

    cmxcall_mo_or_mt_e_type call_origination
        /* where the call originated */

)
{

  switch( call_type )
  {
    /* The following service options can be used in conjunction
    ** with VOICE, E911, or OTASP call type.
    */
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_EMERGENCY:
    case CM_CALL_TYPE_STD_OTASP:
    case CM_CALL_TYPE_NON_STD_OTASP:
      return cmxcall_is_voice_srv_opt(srv_opt);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following service options can be used in conjunction
    ** with DATA call type.
    */
    case CM_CALL_TYPE_CS_DATA:
      return is_cs_data_srv_opt(srv_opt);

    case CM_CALL_TYPE_PS_DATA:
      return is_pkt_data_srv_opt(srv_opt, call_origination);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following service options can be used in conjunction
    ** with SMS call type.
    */
    case CM_CALL_TYPE_SMS:
      return is_sms_srv_opt( srv_opt );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following service options can be used in conjunction
    ** with PD call type.
    */
    case CM_CALL_TYPE_PD:
      return is_pd_srv_opt( srv_opt );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following service options can be used in conjunction
    ** with TEST call type.
    */
    case CM_CALL_TYPE_TEST:
      return is_test_srv_opt( srv_opt );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following service options can be used in conjunction
    ** with OTAPA call type.
    */
    case CM_CALL_TYPE_OTAPA:
      return is_otapa_srv_opt( srv_opt );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      CM_ERR_1( "Bad call type %d",(uint8) call_type);
      /*
      ** 527: Unreachable
      ** This is because on some targets _SWITCH_ERR evaluates to
      ** sth that has exit(1) in it but it may be turned off for others
      */
      /*lint -save -e527*/
      return FALSE;
      /*lint -restore */

  } /* switch(call_type) */

} /* cmxcall_is_valid_srv_opt() */




/*===========================================================================

FUNCTION cmxcall_find_call

DESCRIPTION

  Checks whether there is an active call of specified type.
  User is able to specify "find me any call" by using CM_CALL_ID_INVALID
  as ignore_call_id parameter or specify a call id of a call that we want
  to ignore if found (for example, we're making a voice call with call id x
  and want to check whether there is another voice call up already, then we'd
  use x as ignore_call_id parameter).

DEPENDENCIES
  None

RETURN VALUE

  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_id_type cmxcall_find_call
(
  cm_call_type_e_type call_type,

  cm_call_id_type     ignore_call_id

)
{
  cm_call_id_type  call_id;
  call_id = cmcall_is_there_a_call_type(CM_SS_MAIN, call_type, ignore_call_id);
  if(call_id == CM_CALL_ID_INVALID)
  {
    call_id = cmcall_is_there_a_call_type(CM_SS_HYBR_1, call_type, ignore_call_id);
  }
  if(call_id == CM_CALL_ID_INVALID)
  {
    call_id = cmcall_is_there_a_call_type(CM_SS_MAX, call_type, ignore_call_id);
  }

  if(call_id == CM_CALL_ID_INVALID && !cmph_no_hybr2())
  {
    call_id = cmcall_is_there_a_call_type(CM_SS_HYBR_2,call_type,ignore_call_id);
  }

  /* return call id
  */
  return call_id;

} /* cmxcall_is_there_a_call_type() */


/*===========================================================================

FUNCTION cmxcall_assert

DESCRIPTION
  Check a series of assertions over the call object to make sure that
  none of its fields are corrupted and to validate its state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmxcall_assert(

    const cmcall_s_type    *call_ptr
        /* pointer to a call object */
)
{
  /*lint -save -e529 Symbol 'call_mode_info_ptr' (line 1840) not subsequently referenced */
  #ifdef CM_DEBUG
  #error code not present
#endif  /* CM_DEBUG */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef CM_DEBUG
  #error code not present
#endif  /* CM_DEBUG */

  CM_ASSERT( call_mode_info_ptr->srv_opt == CM_SRV_OPT_NONE ||
             cmxcall_is_voice_srv_opt(call_mode_info_ptr->srv_opt)  ||
             is_data_srv_opt(call_mode_info_ptr->srv_opt, CMXCALL_MOBILE_ORIGINATED)   ||
             is_data_srv_opt(call_mode_info_ptr->srv_opt, CMXCALL_MOBILE_TERMINATED)   ||
             is_test_srv_opt(call_mode_info_ptr->srv_opt)   ||
             is_sms_srv_opt(call_mode_info_ptr->srv_opt)    ||
             is_otapa_srv_opt(call_mode_info_ptr->srv_opt)  ||
             is_pd_srv_opt(call_mode_info_ptr->srv_opt)

           );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_MMODE_REMOVE_1X
  /* Verify the below only if signal info is avaiable
  */
  if(call_ptr->signal.is_signal_info_avail)
  {

    CM_ASSERT( call_ptr->signal.signal_type <= CAI_SIG_IS54B );
    CM_ASSERT( call_ptr->signal.alert_pitch <= CAI_PITCH_LOW );
    CM_ASSERT( call_ptr->signal.signal <= CAI_TONE_OFF );

  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check integrity privacy indicator.
  */
  CM_ASSERT( call_mode_info_ptr->is_privacy == TRUE ||
             call_mode_info_ptr->is_privacy == FALSE );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_OTASP
  CM_ASSERT( call_mode_info_ptr->otasp_info.otasp_act_code == CM_OTASP_ACT_CODE_NONE ||
             INRANGE(call_mode_info_ptr->otasp_info.otasp_act_code, CM_ABAND, CM_PREFERRED) );
  #else /* #ifdef FEATURE_OTASP */
  CM_ASSERT( call_mode_info_ptr->otasp_info.otasp_act_code == CM_OTASP_ACT_CODE_NONE );
  #endif  /* FEATURE_OTASP */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the call fields are consistent with the call state.
  */
  if( call_ptr->call_type != CM_CALL_TYPE_NONE )
  {
    CM_ASSERT( cmxcall_is_valid_srv_type(call_ptr->call_type,
                                        call_ptr->srv_type) );

    if( call_ptr->ho_type != SYS_HO_TYPE_UMTS_1X &&
        (call_ptr->srv_type != CM_SRV_TYPE_AMPS)
        #ifdef FEATURE_HDR
        && !(CMCALL_IS_HDR_SRV_TYPE(call_ptr))
        #endif  /* FEATURE_HDR */
      )
    {
      /*
      ** Because of ASYNC data service options can be both cs/ps in mobile originations,
      ** we will assume origination to test here.
      */
      CM_ASSERT( cmxcall_is_valid_srv_opt(call_ptr->call_type,
                                         call_mode_info_ptr->srv_opt,
                                         CMXCALL_MOBILE_ORIGINATED));
    }

  }


  if( call_ptr->call_state == CM_CALL_STATE_ORIG &&
      call_ptr->ho_type != SYS_HO_TYPE_UMTS_1X )
  {
    CM_ASSERT( BETWEEN(call_ptr->favored_mode_pref,
                       CM_MODE_PREF_NONE,
                       CM_MODE_PREF_MAX) );

    CM_ASSERT( cmxcall_is_valid_mode_pref(call_ptr->call_type,
                                         call_ptr->favored_mode_pref) );
  }


  #ifdef FEATURE_IS2000
  /*lint -save -e641 */
  CM_ASSERT(call_mode_info_ptr->retry_order.retry_type <= (CM_RETRY_TYPE_MAX-1));
  /*lint -restore */
  #endif /* FEATURE_IS2000 */

  /*lint -save -e685 */
  CM_ASSERT(INRANGE(call_ptr->cmcall_mode_info.info.cdma_info.orig_fail_layer,
                    CM_CALL_ORIG_ERR_LAYER_NONE,
                    CM_CALL_ORIG_ERR_LAYER_MAX));
  /*lint -restore */
  /*lint -restore */
} /* cmxcall_assert() */



/*===========================================================================

FUNCTION cmxcall_set_delay_call_state_for_SO33

DESCRIPTION
  Check if the stacks are suspended due to SO33 call conflict - then delay
  the call event until call_end_cnf is received from 1X-CP.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmxcall_set_call_wait_for_1X_end_cnf(

  cmcall_s_type    *call_ptr
     /* pointer to a call object */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  #ifdef FEATURE_MMODE_SC_SVLTE
  if( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_CDMA )
  {
    /* If not waiting for call end cnf from 1X protocol.
    ** Since this is special handling for SO33/32 - 1X will not
    ** have more than 1 and only 1 call at a time.
    */
    if( ( cmph_ptr()->ss_susp_info[CM_SS_HYBR_2].is_suspend == TRUE ||
          cmph_ptr()->ss_susp_info[CM_SS_HYBR_1].is_suspend == TRUE )
         &&
        cmcall_is_1x_data_srv_opt(call_ptr->cmcall_mode_info.info.cdma_info.srv_opt)
    )
    {
      CM_MSG_HIGH_0("SO: Delay call end event until CALL_END_CNF");
      call_ptr->call_state = CM_CALL_STATE_IDLE;
      call_ptr->call_subst.idle = CM_CALL_IDLE_SUBST_ENDWAIT;
    }
    return;
  }
  #else
  SYS_ARG_NOT_CONST( call_ptr );
  #endif

}

#ifndef FEATURE_MMODE_REMOVE_1X
/*===========================================================================

FUNCTION cmxcall_map_cm_privacy_pref_to_mc_long_code

DESCRIPTION
  Convert voice privacy preference to MC long code request.

DEPENDENCIES
  none

RETURN VALUE
  MC long code request value.

SIDE EFFECTS
  none

===========================================================================*/
static mc_long_code_e_type cmxcall_map_cm_privacy_pref_to_mc_long_code
(
  cm_privacy_pref_e_type  privacy_pref
)
{

  mc_long_code_e_type long_code;

  switch( privacy_pref )
  {
    case CM_PRIVACY_PREF_ENHANCED:
      long_code = MC_LONG_CODE_PRIVATE;
      break;

    case CM_PRIVACY_PREF_STANDARD:
    default:
      long_code = MC_LONG_CODE_PUBLIC;
      break;
  }
  return long_code;
}
#endif

/*===========================================================================

FUNCTION cmxcall_map_mc_long_code_to_cm_privacy_pref

DESCRIPTION
  Convert MC long code request to voice privacy preference

DEPENDENCIES
  none

RETURN VALUE
 cm_privacy_pref_e_type value.

SIDE EFFECTS
  none

===========================================================================*/
static cm_privacy_pref_e_type cmxcall_map_mc_long_code_to_cm_privacy_pref
(
  mc_long_code_e_type long_code
)
{
  switch( long_code )
  {
    case MC_LONG_CODE_PRIVATE :
      return CM_PRIVACY_PREF_ENHANCED;

    case MC_LONG_CODE_PUBLIC :
    default:
      return CM_PRIVACY_PREF_STANDARD;
  }

}

#ifdef FEATURE_HDR_HANDOFF

#ifndef FEATURE_MMODE_REMOVE_1X
/*===========================================================================

FUNCTION map_cm_mode_to_mc_mode

DESCRIPTION
  Map CM system mode to MC digital mode

DEPENDENCIES
  None

RETURN VALUE
  CM system mode

SIDE EFFECTS
  None

===========================================================================*/
static mc_digital_mode_type   map_cm_mode_to_mc_mode(

    sys_sys_mode_e_type   cm_sys_mode
        /* cm system mode */
)
{

  /* Map CM OTASP activation code to SD band.
  */
  switch( cm_sys_mode )
  {
    case SYS_SYS_MODE_CDMA:    return MC_DIGITAL_MODE_CDMA;
    case SYS_SYS_MODE_HDR:     return MC_DIGITAL_MODE_HDR;
    default:
      return MC_DIGITAL_MODE_NONE;
  }
}
#endif /* FEATURE_MMODE_REMOVE_1X */

/*===========================================================================

FUNCTION map_cm_mode_to_hdrmc_mode

DESCRIPTION
  Map CM system mode to HDR MC digital mode

DEPENDENCIES
  None

RETURN VALUE
  HDRMC Digital mode

SIDE EFFECTS
  None

===========================================================================*/

static hdrcp_digital_mode_type   map_cm_mode_to_hdrmc_mode(

    sys_sys_mode_e_type   cm_sys_mode
        /* cm system mode */
)
{

  switch( cm_sys_mode )
  {
    case SYS_SYS_MODE_CDMA:    return HDRCP_DIGITAL_MODE_CDMA;
    case SYS_SYS_MODE_HDR:     return HDRCP_DIGITAL_MODE_HDR;
    default:
      return HDRCP_DIGITAL_MODE_NONE;
  }
}

#endif /* FEATURE_HDR_HANDOFF */


/*===========================================================================
=============================================================================
======================= INTERNAL CALL OBJECT FUNCTIONS ======================
=============================================================================
===========================================================================*/


/* Note that call object functions (as apposed to miscellaneous functions)
** take a pointer to a call object as their first parameter.
** If their call pointer is NOT "const" they are capable of changing the
** call state.
*/

#ifdef FEATURE_HDR_HANDOFF

/*===========================================================================

FUNCTION cmcall_is_pkt_data_call

DESCRIPTION
  This function check the current call is a packet data call or not

DEPENDENCIES
  Should only be called after call is connected and sevice option is
  assigned

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_pkt_data_call
(
  const cmcall_s_type             *call_ptr
)
{
  boolean                   is_pkt_call = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( call_ptr->call_type )
  {
    case CM_CALL_TYPE_PS_DATA:

      is_pkt_call = TRUE;
      break;

    case CM_CALL_TYPE_CS_DATA:

      switch (call_ptr->srv_type)
      {
        case CM_SRV_TYPE_HDR_MORE_PREF:
        case CM_SRV_TYPE_HDR:
          is_pkt_call = TRUE;
          break;

        case CM_SRV_TYPE_CDMA_SPECIFIC:
        case CM_SRV_TYPE_CDMA_HDR:
          is_pkt_call = is_pkt_data_srv_opt(
            call_ptr->cmcall_mode_info.info.cdma_info.srv_opt,
            CMXCALL_MOBILE_TERMINATED);
            break;
        default:
          break;
      }
      break;

    default:
      break;
  }

  return is_pkt_call;
}
#endif /* FEATURE_HDR_HANDOFF */



/*===========================================================================

FUNCTION cmcall_send_hdrmc_end_cmd

DESCRIPTION
  Send an end command to HDRMC task

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_send_hdrmc_end_cmd

(
  const cmcall_s_type    *call_ptr,
     /* pointer to a call object */

  byte                   end_reason
     /* call end reason */
)
/*lint -esym(715,call_ptr, end_reason)*/
{
  #ifdef FEATURE_HDR_HYBRID

  hdrcp_msg_req_u      *hdrcp_req;

  hdrcp_req = (hdrcp_msg_req_u*) cm_mem_malloc(sizeof(hdrcp_msg_req_u));

  hdrcp_req->call_end.call_id = cmcall_call_id_get( call_ptr );
  hdrcp_req->call_end.end_reason = end_reason;

  CM_MSG_HIGH_1("CM->MSGR: send HDR_CP_CALL_END_REQ, reason=%d", end_reason);
  (void) cm_msgr_send( HDR_CP_CALL_END_REQ, MSGR_MM_CM,
                       (msgr_hdr_struct_type*)hdrcp_req, sizeof(hdrcp_msg_req_u) );

  cm_mem_free(hdrcp_req);

  #else
  CM_ERR_2("Invalid call end, call id %d reas %d",
               call_ptr->call_id, end_reason);
  #endif  /* FEATURE_HDR_HYBRID */
}
/*lint +esym(715,call_ptr, end_reason)*/


/*===========================================================================

FUNCTION cmcall_send_mc_end_cmd

DESCRIPTION
  Send an end command to MC task

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_send_mc_end_cmd

(
  const cmcall_s_type    *call_ptr,
     /* pointer to a call object */

  byte                   end_reason
)
{
  #ifndef FEATURE_MMODE_REMOVE_1X
  mc_msg_type *mc_ptr;

  /* The hybrid operation is not enabled so
    send the end to MC to end the call*/
  
  CM_MSG_HIGH_2 ("CallType = %d Call Id = %d ", call_ptr->call_type, cmcall_call_id_get( call_ptr ));

  mc_ptr = cm_mc_get_buf_else_err_fatal();

  if(mc_ptr == NULL )
  {
    return;
  }
  mc_ptr->hdr.cmd = MC_END_F;
  mc_ptr->end.num_call_ids = 1;
  mc_ptr->end.call_ids[0] = cmcall_call_id_get( call_ptr );
  mc_ptr->end.end_reasons[0] = end_reason;

  cm_mc_send_cmd( mc_ptr );
  #else
  #error code not present
#endif
}


/*===========================================================================

FUNCTION cmxcall_send_end_cmd

DESCRIPTION
  Send an end command to either MC or HDRMC task

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_send_end_cmd
(
  cmcall_s_type    *call_ptr,
     /* pointer to a call object */

  byte                   end_reason
     /* call end reason */
)
{
  #if defined (FEATURE_LTE_TO_1X)
  cmsds_s_type                            *sds_ptr               = cmsds_ptr();
  #endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_HDR )
  {
    /* The hybrid operation is enabled and call
    ** is on HDR
    */
    cmcall_send_hdrmc_end_cmd(call_ptr,end_reason);
    /* save the previous call state */
    call_ptr->prev_call_state = call_ptr->call_state;
    call_ptr->call_state = CM_CALL_STATE_IDLE;
    call_ptr->call_subst.idle = CM_CALL_IDLE_SUBST_ENDWAIT;
    cmcall_event( call_ptr, CM_CALL_EVENT_END_REQ );

    /*
    ** Tell phone that Call End procedure has been
    ** started.
    */
    cmph_call_end_req(call_ptr);

    call_ptr->end_req_wait_uptime =
                       time_get_uptime_secs() + CMCALL_HDR_WAIT_ORIG_UPTIME;
    return;
  }
  else if( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_CDMA
	 #if defined (FEATURE_CM_LTE) && defined (FEATURE_IP_CALL) && defined(FEATURE_1X_SRVCC)
	  || (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_LTE &&
		   call_ptr->is_srvcc_call )
   #endif
	)
  {
    #if defined (FEATURE_LTE_TO_1X)
    if (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB && sds_ptr->is_1xcsfb_activated)
    {
      /* Call type is 1XCSFB and 1XCP is activated in CSFB mode
      ** So send the MC_CSFB_CALL_END_F command to MC and
      **  NAS_ESM_1XCSFB_ABORT_REQ to NAS
      */
      cmcsfbcall_send_csfb_abort_req(call_ptr->call_id);
      cmcsfbcall_send_1xcsfb_call_end_req(call_ptr);
      return;
    }
  #endif

    /* Send an end command to MC to end the call */
    cmcall_send_mc_end_cmd(call_ptr,end_reason);
    cmxcall_set_call_wait_for_1X_end_cnf(call_ptr);

    return;
  }
}



/*===========================================================================
=============================================================================
======================= INTERNAL CALL OBJECT FUNCTIONS ======================
=============================================================================
===========================================================================*/


/* Note that call object functions (as apposed to miscellaneous functions)
** take a pointer to a call object as their first parameter.
** If their call pointer is NOT "const" they are capable of changing the
** call state.
*/



/*===========================================================================

FUNCTION cmcall_is_valid_answer

DESCRIPTION
  Check whether a client answers with valid call type relative to the
  type of the incoming call.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if client answered with valid call type relative to the type of
  the incoming call, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_valid_answer(

    const cmcall_s_type    *call_ptr,
        /* pointer to a call object */

    cm_call_type_e_type    answer_call_type
        /* client answer with this call type */
)
{

  CM_ASSERT( call_ptr != NULL );

  CM_ASSERT( BETWEEN(call_ptr->call_type,
                     CM_CALL_TYPE_NONE,
                     CM_CALL_TYPE_MAX) );

  CM_ASSERT( BETWEEN(answer_call_type,
                     CM_CALL_TYPE_NONE,
                     CM_CALL_TYPE_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Answering with the same call type
  ** is always valid.
  */
  if( call_ptr->call_type == answer_call_type )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Otherwise, can only answer analog calls as DATA.
  */
  if( call_ptr->srv_type == CM_SRV_TYPE_AMPS &&
      answer_call_type   == CM_CALL_TYPE_CS_DATA )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the answer call type is invalid relative
  ** to the type of the incoming call - return FALSE.
  */
  return FALSE;

}


/*===========================================================================
FUNCTION cmxcall_process_redirecting_record_from_mc

DESCRIPTION
  Process the MC report of a redirecting number and store it in a CM struct
  for further processing.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_REDIRECTING_NUMBER_INFO_REC
static void cmxcall_process_redirecting_record_from_mc(
  cm_redirecting_num_s_type          *cm_redirecting_number_ptr,
  const cm_redirecting_number_type   *mc_redirecting_number_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cm_redirecting_number_ptr           != NULL);
  CM_ASSERT(mc_redirecting_number_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Extract the redirecting number
  */
  cm_num_copy( &cm_redirecting_number_ptr->num,
               mc_redirecting_number_ptr->chars,
               mc_redirecting_number_ptr->num_chars );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the number plan, number type,
  ** presentation indicator and screening indicator.
  */
  CM_NUM_NUMBER_TYPE( &cm_redirecting_number_ptr->num ) =
                                mc_redirecting_number_ptr->number_type;

  CM_NUM_NUMBER_PLAN( &cm_redirecting_number_ptr->num ) =
                                mc_redirecting_number_ptr->number_plan;

  if (mc_redirecting_number_ptr->pi_si_included)
  {
    CM_NUM_PI( &cm_redirecting_number_ptr->num ) = mc_redirecting_number_ptr->pi;
    CM_NUM_SI( &cm_redirecting_number_ptr->num ) = mc_redirecting_number_ptr->si;
  }
  else
  {
    #ifndef FEATURE_MMODE_REMOVE_1X
    CM_NUM_PI( &cm_redirecting_number_ptr->num ) = (cm_npi_type)CAI_PI_PRESENTATION_ALLOWED;
    CM_NUM_SI( &cm_redirecting_number_ptr->num ) = (cm_si_type)CAI_SI_USER_NOT_SCREENED;
    #else
    #error code not present
#endif

  }

  if (mc_redirecting_number_ptr->redirecting_reason_included)
  {
    cm_redirecting_number_ptr->redirecting_reason =
      (cm_redirecting_reason_e_type)mc_redirecting_number_ptr->redirecting_reason;
  }
  else
  {
    cm_redirecting_number_ptr->redirecting_reason = CM_REDIRECTING_REASON_UNKNOWN;
  }
}  /* cmxcall_process_redirecting_record_from_mc */
#endif

/*===========================================================================
FUNCTION cmxcall_event_generic

DESCRIPTION
  Notify the client list of a call event that does not apply to a call with
  a particular call id (i.e. "global" call event).

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void cmxcall_event_generic(
  const cm_mc_rpt_type   *cm_mc_rpt_ptr,
  cm_call_event_e_type    call_event
)
{
  int i;
  cm_mm_call_info_s_type *call_info_ptr = cmcall_info_alloc();
    /* Allocate a call info buffer
    */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   /* Incoming HPCD number translation : remove IDD */
   cm_num_details_for_conversion_s_type num_details;

   /* Details before calling plus to digits conversion api */
   cm_num_s_type          converted_num;
   /* Converted digits as a result of calling
   ** plus to digits conversion api
  */

   cmutil_conver_result_e_type    conver_result = CMUTIL_CONVER_RESULT_NONE;
   /* Result of calling plus to digits conversion api */

    cmutil_num_conv_req_e_type      num_conv_req = CMUTIL_NUM_CONV_REQ_NONE;
  /* Initialize num conversion request */

   cmss_s_type             *ss_ptr            = cmss_ptr();
    /* Point at serving system object */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( cm_mc_rpt_ptr != NULL );
  CM_ASSERT( BETWEEN(call_event, CM_CALL_EVENT_NONE, CM_CALL_EVENT_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Since all of the below event do not apply to a particular call
  ** - set call state to idle, i.e. no call
  ** - set call type to none, i.e. no call
  ** - set call id to CM_CALL_ID_UNASSIGNED, i.e. no call
  */
  call_info_ptr->call_id    = CM_CALL_ID_UNASSIGNED;
  call_info_ptr->call_state = CM_CALL_STATE_IDLE;
  call_info_ptr->call_type  = CM_CALL_TYPE_NONE;
  call_info_ptr->mode_info.info_type = CM_CALL_MODE_INFO_CDMA;


  switch(call_event)
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_CALL_EVENT_EXT_DISP:

      call_info_ptr->mode_info.info.cdma_call.ext_disp_type =
        cm_mc_rpt_ptr->ext_disp.disp_info.display;

      call_info_ptr->mode_info.info.cdma_call.is_last_cdma_info_rec =
        cm_mc_rpt_ptr->ext_disp.hdr.last_record;

      call_info_ptr->alpha.len =
        cm_mc_rpt_ptr->ext_disp.disp_info.datalen;

      for (i = 0; i < call_info_ptr->alpha.len ; i++)
      {
        call_info_ptr->alpha.buf[i] =
          cm_mc_rpt_ptr->ext_disp.disp_info.data[i];
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_CALL_EVENT_SIGNAL:

      call_info_ptr->mode_info.info.cdma_call.is_last_cdma_info_rec =
        cm_mc_rpt_ptr->signal.hdr.last_record;


      call_info_ptr->signal.signal_type    =
        cm_mc_rpt_ptr->signal.signal_type;          /*lint !e734 */

      call_info_ptr->signal.alert_pitch    =
        cm_mc_rpt_ptr->signal.alert_pitch;          /*lint !e734 */

      call_info_ptr->signal.signal         =
        cm_mc_rpt_ptr->signal.signal;               /*lint !e734 */

      call_info_ptr->signal.is_signal_info_avail = TRUE;

    break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_CALL_EVENT_EXT_BRST_INTL:

      call_info_ptr->mode_info.info.cdma_call.ext_brst_intl_msg.chg_ind =
              cm_mc_rpt_ptr->ext_brst_intl_rec.chg_ind;

      call_info_ptr->mode_info.info.cdma_call.ext_brst_intl_msg.db_subtype =
              cm_mc_rpt_ptr->ext_brst_intl_rec.db_subtype;

      call_info_ptr->mode_info.info.cdma_call.ext_brst_intl_msg.mcc =
              cm_mc_rpt_ptr->ext_brst_intl_rec.mcc;

      call_info_ptr->mode_info.info.cdma_call.ext_brst_intl_msg.sub_unit =
              cm_mc_rpt_ptr->ext_brst_intl_rec.sub_unit;

      call_info_ptr->mode_info.info.cdma_call.ext_brst_intl_msg.unit =
              cm_mc_rpt_ptr->ext_brst_intl_rec.unit;
      break;  /* CM_CALL_EVENT_EXT_BRST_INTL */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_CALL_EVENT_EXIT_TC:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_CALL_EVENT_DISPLAY:
      {
        int   idx;

        call_info_ptr->mode_info.info.cdma_call.is_last_cdma_info_rec =
          cm_mc_rpt_ptr->display.hdr.last_record;

        call_info_ptr->alpha.len =
          MIN( cm_mc_rpt_ptr->display.num_chars, sizeof(call_info_ptr->alpha.buf) );

        call_info_ptr->alpha.len =
          MIN( call_info_ptr->alpha.len, CM_MAX_DATA_DIGITS );

        for (idx = 0; idx < call_info_ptr->alpha.len ; idx++)
        {
           call_info_ptr->alpha.buf[idx] = cm_mc_rpt_ptr->display.chars[idx] ;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_CALL_EVENT_ABRV_ALERT:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_CALL_EVENT_CONNECTED_NUM:

      call_info_ptr->mode_info.info.cdma_call.is_last_cdma_info_rec =
      cm_mc_rpt_ptr->connected_num.hdr.last_record;

      cm_num_copy( &call_info_ptr->num,
                    cm_mc_rpt_ptr->connected_num.chars,
                    cm_mc_rpt_ptr->connected_num.num_chars );

      CM_NUM_NUMBER_TYPE( &call_info_ptr->num ) =
                                   cm_mc_rpt_ptr->connected_num.number_type;

      CM_NUM_NUMBER_PLAN( &call_info_ptr->num ) =
                                   cm_mc_rpt_ptr->connected_num.number_plan;

      CM_NUM_PI( &call_info_ptr->num ) = cm_mc_rpt_ptr->connected_num.pi;
      CM_NUM_SI( &call_info_ptr->num ) = cm_mc_rpt_ptr->connected_num.si;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_CALL_EVENT_CALLER_ID:

      call_info_ptr->mode_info.info.cdma_call.is_last_cdma_info_rec =
        cm_mc_rpt_ptr->calling_party.hdr.last_record;

      /* Extract the caller ID
      */
      cm_num_copy( &call_info_ptr->num,
                   cm_mc_rpt_ptr->calling_party.chars,
                   cm_mc_rpt_ptr->calling_party.num_chars );

      /* calling party number will be stored in ASCII, confirmed with 1x team */
      call_info_ptr->num.digit_mode = CM_DIGIT_MODE_8BIT_ASCII;

      /* Extract the number plan, number type,
      ** presentation indicator and screening indicator.
      */
      CM_NUM_NUMBER_TYPE( &call_info_ptr->num ) =
                                     cm_mc_rpt_ptr->calling_party.number_type;

      CM_NUM_NUMBER_PLAN( &call_info_ptr->num ) =
                                     cm_mc_rpt_ptr->calling_party.number_plan;

      CM_NUM_PI( &call_info_ptr->num ) = cm_mc_rpt_ptr->calling_party.pi;
      CM_NUM_SI( &call_info_ptr->num ) = cm_mc_rpt_ptr->calling_party.si;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* HPCD conversion for inocmming call*/
      /* Init num conversion params */
      cm_util_init_num_conversion_params ( &num_conv_req,
                                           &num_details );

      /* Operation to be performed on num */
      num_conv_req = CMUTIL_NUM_CONV_REQ_DIGITS_TO_PLUS;

      /* Fill the number details before calling conversion */
      num_details.call_type         = call_info_ptr->call_type;
      num_details.current_mobile_country_code  = ss_ptr->info.current_mcc;
      num_details.home_mobile_country_code    = ss_ptr->info.home_mcc;
      num_details.num                  = call_info_ptr->num;

      num_details.hs_based_plus_dial_setting  = ss_ptr->info.plus_dial_setting;

      /* If number begins with IDD try to remove the idd */
      conver_result  = cm_util_perform_num_conversion (  num_conv_req,
                                                         &num_details,
                                                         &converted_num
                                                      );

      CM_MSG_HIGH_1 (" Result of cm_util_perform_num_conversion for incoming %d",
                   conver_result);

      if (conver_result == CMUTIL_CONVER_RESULT_NUM_MODIFIED)
      {
        /* Copy from converted number : Because it is to remove idd,
        ** careful to exising number in the buffer.
        */
        memscpy( call_info_ptr->num.buf, sizeof(call_info_ptr->num.buf), converted_num.buf, sizeof(converted_num.buf) );
        CM_NUM_LEN( &call_info_ptr->num ) = converted_num.len;
        CM_NUM_NUMBER_TYPE( &call_info_ptr->num ) = converted_num.number_type;


        CM_NUM_DIGIT_MODE (&call_info_ptr->num ) =
        CM_NUM_DIGIT_MODE(&converted_num);

      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/




    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  case CM_CALL_EVENT_CALLED_PARTY:

    call_info_ptr->mode_info.info.cdma_call.is_last_cdma_info_rec =
      cm_mc_rpt_ptr->called_party.hdr.last_record;

    cm_num_copy( &call_info_ptr->num,
                 cm_mc_rpt_ptr->called_party.chars,
                 cm_mc_rpt_ptr->called_party.num_chars );

    CM_NUM_NUMBER_TYPE( &call_info_ptr->num ) =
                                   cm_mc_rpt_ptr->called_party.number_type;

    CM_NUM_NUMBER_PLAN( &call_info_ptr->num ) =
                                   cm_mc_rpt_ptr->called_party.number_plan;
    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_REDIRECTING_NUMBER_INFO_REC
  case CM_CALL_EVENT_REDIRECTING_NUMBER:
    cmxcall_process_redirecting_record_from_mc( &call_info_ptr->redirecting_number,
                                                &cm_mc_rpt_ptr->redirecting_number );
    break;
  #endif /* FEATURE_REDIRECTING_NUMBER_INFO_REC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  default:
      CM_ERR_1("Bad call event %d",call_event);
      CM_ASSERT(FALSE);
  }

  /*
  ** Notify clients list of call event
  */
  cmclient_list_call_event_ntfy( call_event, call_info_ptr );

  /* Deallocate the memory after sending the call event. */
  cm_mem_free(call_info_ptr);
}


/*===========================================================================

FUNCTION cmcall_end_no_call_id_dealloc

DESCRIPTION
  Same as cmcall_end() but it does not deallocate call id
  at the end.
  Used to process "alert with info" messages that CM receives
  simply as CM_INCOMING_CALL_F message for a call that is already
  up.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_end_no_call_id_dealloc(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  cm_call_id_type             saved_call_id;

  #ifdef FEATURE_OTASP
  cdma_cmcall_type *call_mode_info_ptr = NULL;
      /* Pointer to call mode info */

  cmph_s_type  *ph_ptr = cmph_ptr();
      /* point at phone object */

  CM_ASSERT( ph_ptr != NULL );
  #endif /* FEATURE_OTASP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );

  /* If call is in idle, we are confused!
  */
  CM_ASSERT( call_ptr->call_state != CM_CALL_STATE_IDLE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3( "Call ends, uptime=%ld, end_stat=%d, sr_left=%d",
                time_get_uptime_secs(),
                call_ptr->end_status,
                call_ptr->sr.sr_left );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_OTASP
  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

  if (call_ptr->call_type == CM_CALL_TYPE_STD_OTASP ||
      call_ptr->call_type == CM_CALL_TYPE_NON_STD_OTASP ||
      call_ptr->call_type == CM_CALL_TYPE_OTAPA ||
      call_mode_info_ptr->otasp_info.otasp_call == CMCALL_BUSY_OTAPA
     )
  {
    /* Tell diag to update its cache*/
    CM_MSG_HIGH_1("diag_otasp_update, call_type: %d", call_ptr->call_type);
    diag_otasp_update();
  }
  #endif /* FEATURE_OTASP */

  /* Change the call state to idle.
  */
  call_ptr->call_state = CM_CALL_STATE_IDLE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the priority queue for this activity
  */

  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_MAIN,
                                 CM_ACT_UPDATE_REAS_ACT_END );

  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_HDR,
                                 CM_ACT_UPDATE_REAS_ACT_END );

  /* Tell phone that call is over.
     Number of allocated calls equal to 1 means that this is the
     last call that is being ended, so we need to notify the phone
     now. */
  if(cm_number_of_allocated_calls() == 1) {
     cmph_call_end( call_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify clients of call end event.
  */
  cmcall_update_call_end_info( call_ptr );
  cmcall_event( call_ptr, CM_CALL_EVENT_END );

  #ifdef FEATURE_OTASP
  CM_MSG_HIGH_1("call end, commit %d",
               call_mode_info_ptr->otasp_info.is_commit_successful);

  if (call_mode_info_ptr->otasp_info.is_commit_successful)
  {

    /* Reset the origination mode to none.
    */
    cmph_orig_mode_reset( ph_ptr, FALSE );

    /* Reset the origination throttle table
    */
    cmss_orig_thr_table_reset( cmss_ptr()->cmss_orig_thr_tbl_ptr );

    /* Clear the is hybr kicked flag
    */
    *cmph_get_is_kicked_hybr_ptr() = FALSE;
    *cmph_get_is_kicked_hybr_2_ptr() = FALSE;


    /* Send a NAM change command to MC to force SD to reload NAM.
    */
    cmph_send_nam_change_to_mc( ph_ptr, ph_ptr->curr_nam );

    /* Flag set to show that subscription change is pending.
    */
    ph_ptr->is_otasp_commit_pending = TRUE;
  }
  #endif /* FEATURE_OTASP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the call object without deallocating call id */

  /* cmcall_reset () resets the call id, so save it and set it back later.
  */
  saved_call_id = call_ptr->call_id;
  cmcall_reset(call_ptr);
  call_ptr->call_id = saved_call_id;
}

/*===========================================================================

FUNCTION cmxcall_determine_call_type_from_so

DESCRIPTION
  Determine the call type according to current call service option.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_call_type_e_type cmxcall_determine_call_type_from_so(

    uint16    srv_opt
        /* pointer to a call object */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( cmxcall_is_voice_srv_opt(srv_opt) )
  {
    return CM_CALL_TYPE_VOICE;
  }

  else if( is_pkt_data_srv_opt(srv_opt, CMXCALL_MOBILE_TERMINATED) ) {
    return CM_CALL_TYPE_PS_DATA;
  }

  else if( is_cs_data_srv_opt(srv_opt) ) {
    return CM_CALL_TYPE_CS_DATA;
  }

  else if( is_sms_srv_opt(srv_opt) ) {
    return CM_CALL_TYPE_SMS;
  }

  else if( is_test_srv_opt(srv_opt) ) {
    return CM_CALL_TYPE_TEST;
  }

  else if( is_pd_srv_opt(srv_opt) ) {
    return CM_CALL_TYPE_PD;
  }

  else if( is_otapa_srv_opt(srv_opt) ) {
    return CM_CALL_TYPE_OTAPA;
    CM_MSG_MED_0("Determine call type OTAPA");
  }

  /* Else, this is an unexpected service option!
  ** Use voice call type as a default.
  */
  else
  {
    CM_ERR_1( "Unexpected srv opt=%d",srv_opt);
    return CM_CALL_TYPE_VOICE;
  }
}

/*===========================================================================

FUNCTION cmxcall_determine_call_type

DESCRIPTION
  Determine the call type according to current call service option.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmxcall_determine_call_type(

    cmcall_s_type    *call_ptr
        /* pointer to a call object */
)
{
  cdma_cmcall_type *call_mode_info_ptr = NULL;
      /* Pointer to call mode info */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
     call_ptr->call_type = CM_CALL_TYPE_EMERGENCY;
  }

  else if( cmxcall_is_voice_srv_opt(call_mode_info_ptr->srv_opt) )
  {
    call_ptr->call_type = CM_CALL_TYPE_VOICE;
  }

  else if( is_pkt_data_srv_opt(call_mode_info_ptr->srv_opt, CMXCALL_MOBILE_TERMINATED) ) {
    call_ptr->call_type = CM_CALL_TYPE_PS_DATA;
  }

  else if( is_cs_data_srv_opt(call_mode_info_ptr->srv_opt) ) {
    call_ptr->call_type = CM_CALL_TYPE_CS_DATA;
  }

  else if( is_sms_srv_opt(call_mode_info_ptr->srv_opt) ) {
    call_ptr->call_type = CM_CALL_TYPE_SMS;
  }

  else if( is_test_srv_opt(call_mode_info_ptr->srv_opt) ) {
    call_ptr->call_type = CM_CALL_TYPE_TEST;
  }

  else if( is_pd_srv_opt(call_mode_info_ptr->srv_opt) ) {
    call_ptr->call_type = CM_CALL_TYPE_PD;
  }

  else if( is_otapa_srv_opt(call_mode_info_ptr->srv_opt) ) {
    call_ptr->call_type = CM_CALL_TYPE_OTAPA;
    CM_MSG_MED_0("Determine call type OTAPA");
  }

  /* Else, this is an unexpected service option!
  ** Use voice call type as a default.
  */
  else
  {
    CM_ERR_1( "Unexpected srv opt=%d",call_mode_info_ptr->srv_opt);
    call_ptr->call_type = CM_CALL_TYPE_VOICE;
  }
}


/*===========================================================================

FUNCTION cmcall_set_so_13k_8k

DESCRIPTION
  Determine the 13k or 8k service option to use for call origination

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE - SO found FALSE - SO not found

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(765,cmcall_set_so_voice) */
boolean cmcall_set_so_voice(

   cdma_cmcall_type   *call_mode_info_ptr
   /* pointer to a call orig command info pointer */
)
{
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /*lint -save -e527*/
    CM_ASSERT(call_mode_info_ptr           != NULL);

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* If EVRC SO Management is supported, use
    ** CAI_SO_VOICE_WILDCARD Service Option,
    ** indicating that MC needs to decide on the originating service
    ** option in accordance with the NV settings.
    */
    call_mode_info_ptr->srv_opt = CAI_SO_VOICE_WILDCARD;

    /*lint -restore */

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    return TRUE;
}

#ifdef FEATURE_MMODE_SC_SVLTE
/*===========================================================================

FUNCTION cmxcall_is_end_suspend_status

DESCRIPTION
  checks if there are any 1x SO 33/32 calls active. If true, UE should still
  suspend hybr 1 / hybr2 staks.

DEPENDENCIES
  None.

RETURN VALUE
   CM_CALL_ID_INVALID- 1x SO 33/32 calls not active;
  valid call id  - 1x SO 33/32 calls active

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmxcall_is_end_suspend_status
(
  void
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type  call_itr;                      /* Call Object Iterator */


  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);
  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_state != CM_CALL_STATE_IDLE  &&
         call_ptr->call_type  == CM_CALL_TYPE_PS_DATA &&
         ( call_ptr->sr.orig_sent_on_mode       == SYS_SYS_MODE_CDMA ||
           call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA ||
           cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,CM_MODE_PREF_CDMA_ONLY)
         ) &&
         cmcall_is_1x_data_srv_opt(call_ptr->cmcall_mode_info.info.cdma_info.srv_opt)
       )
    {
      return call_ptr->call_id;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return CM_CALL_ID_INVALID;


} /* cmxcall_is_end_suspend_status() */

/*===========================================================================

FUNCTION cmxcall_end_1x_data_call_in_subst_endwait

DESCRIPTION
  checks if the call id is in idle state with subst endwait.
  if so, the call is ended and hybr stack operation is resumed.

  To handle race condition where 1xcp sends orig failure to CM
  before it processes the call end request sent by CM.
  The orig failure should be handled in the same way as a
  CM_CALL_END_CNF_F and the call should be cleaned up.
  The end status of the call as reported to clients will be the
  cause that made CM send the end cmd to 1xcp and will not reflect
  the orig fail reason sent by 1xcp.


DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if call was ended
  FALSE - otherwise

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmxcall_end_1x_data_call_in_subst_endwait
(
  cm_call_id_type          call_id
  /* call id of call that needs to be ended
  */
)
{
  cmcall_s_type * call_ptr = NULL;

  if (!cm_call_id_is_allocated (call_id))
  {
    CM_ERR_1("SO: Call Id %d not allocated", call_id);
    return FALSE;
  }

  /* Get the pointer to call object */
  call_ptr = cmcall_ptr( call_id );

  if(call_ptr == NULL)
  {
    CM_MSG_HIGH_0( "SO:call_ptr_NULL");
    return FALSE;
  }

  if ( call_ptr->call_id              != CM_CALL_ID_INVALID &&
       call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_CDMA  &&
       cmcall_is_1x_data_srv_opt(call_ptr->cmcall_mode_info.info.cdma_info.srv_opt) &&
       call_ptr->call_state           == CM_CALL_STATE_IDLE &&
       call_ptr->call_subst.idle      == CM_CALL_IDLE_SUBST_ENDWAIT
     )
  {
    CM_MSG_HIGH_0("SO:orig fail in endwait");
    cmcall_end( call_ptr );
    return TRUE;
  }

  return FALSE;

} /* cmxcall_end_1x_data_call_in_subst_endwait() */

/*===========================================================================

FUNCTION cmxcall_suspend_hybrid_operation

DESCRIPTION
    Since CM has to proceed with 1x page, it needs to suspend HYBR2 and
    HYBR1 stacks. Do this based on whether SVLTE and SVDO are supported.

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmxcall_suspend_hybrid_operation(void)

{

  /* If HYBR2 is operational and there is some request pending with MMOC
  ** or currently HYBR2 is not suspended, send suspend to HYBR2 and update
  ** Cmph to indicate that CM is waiting for MMOC response
  */
  if( ( cmph_ptr()->ss_susp_info[CM_SS_HYBR_2].is_suspend == FALSE ||
       cmph_ptr()->ss_susp_info[CM_SS_HYBR_2].num_requests != 0
      )
     )
  {
    mmoc_cmd_suspend_ss(SD_SS_HYBR_2,TRUE,FALSE);
    cmph_update_suspend_status(CM_SS_HYBR_2, TRUE, TRUE);
  }
  /* If SVDO is supported and there is some request pending with MMOC,
     or currently HYBR2 is not suspended ,send suspend to HYBR1 and update
     Cmph to indicate that CM is waiting for MMOC response
  */
  if( ( cmph_ptr()->ss_susp_info[CM_SS_HYBR_1].is_suspend == FALSE ||
       cmph_ptr()->ss_susp_info[CM_SS_HYBR_1].num_requests != 0
      )
    )
  {
    mmoc_cmd_suspend_ss(SD_SS_HYBR_1,TRUE,FALSE);
    cmph_update_suspend_status(CM_SS_HYBR_1, TRUE, TRUE);
  }

}//cmxcall_suspend_hybrid_operation



/*===========================================================================

FUNCTION cmxcall_unsuspend_hybrid_operation

DESCRIPTION
    Since CM suspended HYBR-1 and HYBR-2 stacks for 1X SO33/32 data call,
    this function resumes the suspension.

    Do this based on whether SVLTE and SVDO are supported.

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_unsuspend_hybrid_operation( void )

{

  /* Phone info pointer */
  cmph_s_type               *ph_ptr = cmph_ptr();


  /* If there are no PS so 33/32 calls active and no pending 1x incoming call
  ** end suspend status
  */
  if( cmxcall_is_end_suspend_status() == CM_CALL_ID_INVALID &&
      ph_ptr->is_1x_await_response == FALSE)
  {
    /* If is suspend state is TRUE or
    ** is_suspend state is false, but there is a pending request sent to MMOC
    ** send a new request to the stack to end suspend state.
    */
    if( ( ph_ptr->ss_susp_info[CM_SS_HYBR_2].is_suspend == TRUE ||
        ph_ptr->ss_susp_info[CM_SS_HYBR_2].num_requests != 0 ) )
    {
      if( cmss_is_hybr2_operational() )
      {
         mmoc_cmd_suspend_ss(SD_SS_HYBR_2,  FALSE,FALSE);
      }
      else
      {
         mmoc_cmd_suspend_ss(SD_SS_HYBR_2, FALSE, TRUE);
      }
      cmph_update_suspend_status(CM_SS_HYBR_2, TRUE, TRUE);
    }
    if( ( ph_ptr->ss_susp_info[CM_SS_HYBR_1].is_suspend == TRUE ||
        ph_ptr->ss_susp_info[CM_SS_HYBR_1].num_requests != 0 ))
    {
      if( cmss_is_hybr_status_on() )
      {
        mmoc_cmd_suspend_ss(SD_SS_HYBR_1, FALSE,FALSE);
      }
      else
      {
        mmoc_cmd_suspend_ss(SD_SS_HYBR_1, FALSE, TRUE);
      }
      cmph_update_suspend_status(CM_SS_HYBR_1, TRUE, TRUE);
    }
  }

}//cmxcall_unsuspend_hybrid_operation



/*===========================================================================

FUNCTION cmxcall_page_proceed

DESCRIPTION
    Since CM has to proceed with 1x page, it needs to suspend HYBR2 and
    HYBR1 stacks. Do this based on whether SVLTE and SVDO are supported.
    If neither needs to be suspended, send proceed indication to 1xcp

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmxcall_page_proceed(void)

{
  /* Send suspend indications to MMOC if necessary (SVLTE or SVDO enabled)
  */
  cmxcall_suspend_hybrid_operation();

  /* If no suspend cmd has been sent, inform 1xcp immediately to proceed
  */
  if( cmph_ptr()->ss_susp_info[CM_SS_HYBR_1].num_requests == 0 &&
      cmph_ptr()->ss_susp_info[CM_SS_HYBR_2].num_requests == 0 &&
      cmph_ptr()->is_1x_await_response )
  {
    cmph_ptr()->is_1x_await_response = FALSE;
    cmxcall_mc_incoming_page_proceed(TRUE);
  }

}//cmxcall_page_proceed


/*===========================================================================

FUNCTION cmxcall_handle_page_proceed_response

DESCRIPTION
  Take action based on DS indication about whether to proceed with 1x page.
  If page proceed = TRUE, proceed to shutdown hybrid operation ( DO is SVDO
  mode and LTE in SVLTE mode).
  If page proceed = FALSE, inform 1xcp to drop the call.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_handle_page_proceed_response(

    boolean   page_proceed
    /* DS indication about whether to proceed with 1x page
    */
)
{
  if( cmph_ptr()->is_1x_await_response == FALSE )
  {
    /* If 1x is no longer waiting for a response, ignore the page proceed
    */
    CM_MSG_HIGH_0("SO: 1x not awaiting resp");
    return;
  }

  /* Override response if we are waiting for LPM command to complete
     This prevent the following conditions causing deadlock:
     1. Incoming page 1X --> CM --> DS (1X waits for CM)
     2. LPM command: UI --> CM --> MMOC --> 1X (MMOC waits for 1X)
     3. DS responded proceed, CM suspend LTE/HDR: CM --> MMOC (CM waits for MMOC)
     4. Deadlocks: CM waits for MMOC, MMOC waits for 1X, 1X waits for CM.
  */
  if( cmxcall_is_waiting_lpm_oprt_mode_reply() )
  {
     /* Override result to FALSE */
     CM_MSG_HIGH_0("SO33/32: Wait on oprt_mode - reject page");
     page_proceed = FALSE;
  }

  /* If CM decides not to proceed with 1x page, it needs to indicate that to
     1xcp
  */
  if(page_proceed == FALSE)
  {
    cmph_ptr()->is_1x_await_response = FALSE;
    cmxcall_mc_incoming_page_proceed(FALSE);
  }
  else
  {
    /* If CM decides to proceed with 1x page, it needs to suspend HYBR2 and
    ** HYBR1 stacks.*/
    cmxcall_page_proceed();
  }
}//cmxcall_handle_page_proceed_response

/*===========================================================================

FUNCTION cmcall_outgoing_data_call_proceed

DESCRIPTION
  Take action based on DS indication about whether to proceed with 1x page.
  If page proceed = TRUE, proceed to shutdown hybrid operation ( DO is SVDO
  mode and LTE in SVLTE mode).
  If page proceed = FALSE, inform 1xcp to drop the call.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
boolean cmxcall_outgoing_data_call_proceed(

)
{
  /* Send suspend indications to MMOC if necessary (SVLTE or SVDO enabled)
  */
  cmxcall_suspend_hybrid_operation();

  /* If no suspend cmd has been sent, allow the call to proceed
  */
  if( cmph_ptr()->ss_susp_info[CM_SS_HYBR_1].num_requests == 0 &&
      cmph_ptr()->ss_susp_info[CM_SS_HYBR_2].num_requests == 0 )
  {

    return TRUE;
  }

  return FALSE;

}//cmxcall_outgoing_data_call_proceed
#endif // FEATURE_MMODE_SC_SVLTE

/*===========================================================================

FUNCTION cmcall_mc_incoming_page_proceed

DESCRIPTION
  Sends  MC_CM_INCOMING_PAGE_CNF_F command to MC with payload indicating whether to
  proceed with the call or not.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_mc_incoming_page_proceed
(
    boolean     call_proceed
        /* Tells 1xcp/MC whether to proceed with call or not */
)
{
  mc_msg_type             *mc_ptr;
      /* MC command pointer */

  /* Get MC command buffer and indicate the
  ** appropriate MC command.
  */
  mc_ptr = cm_mc_get_buf_else_err_fatal();
  mc_ptr->hdr.cmd = MC_CM_INCOMING_PAGE_CNF_F;
  mc_ptr->page_cnf.is_continue = call_proceed;

  /* Send command to MC.
  */
  cm_mc_send_cmd( mc_ptr );

}//cmxcall_mc_incoming_page_proceed

/*===========================================================================

FUNCTION cmcall_determine_orig_srv_opt

DESCRIPTION
  Determine the best service option to be used for call originations.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE - SO found FALSE - SO not found

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_determine_orig_srv_opt(

    cmcall_s_type  *call_ptr
)
{
  cdma_cmcall_type *call_mode_info_ptr = NULL;
  boolean          is_so_found = TRUE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

  /* No need to determine the service option
  ** if service type is AMPS, CDMA_HDR or CDMA-SPECIFIC.
  */
  if( call_ptr->srv_type == CM_SRV_TYPE_AMPS ||
      call_ptr->srv_type == CM_SRV_TYPE_CDMA_HDR ||
      call_ptr->srv_type == CM_SRV_TYPE_CDMA_SPECIFIC )
  {
    return TRUE;
  }
  #ifdef FEATURE_HDR_HYBRID
  else if (CMCALL_IS_HDR_SRV_TYPE(call_ptr))
  {
    return TRUE;
  }
  else if ((call_ptr->call_type == CM_CALL_TYPE_PKT_DATA) &&
           (call_ptr->srv_type == CM_SRV_TYPE_AUTOMATIC) )
  {
    return TRUE;
  }
  #endif /* FEATURE_HDR_HYBRID */

  #ifdef FEATURE_LTE_TO_1X
  else if ((call_ptr->call_type == CM_CALL_TYPE_VOICE ||
            call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) &&
           (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB) &&
           (call_ptr->srv_type == CM_SRV_TYPE_AUTOMATIC ||
            call_ptr->srv_type == CM_SRV_TYPE_CS_ONLY ))
  {
    return TRUE;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Determine the best service option to originate with
  ** according to the call type and the vocoder capability
  */

  switch( call_ptr->call_type )
  {
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_EMERGENCY:
    case CM_CALL_TYPE_STD_OTASP:
    case CM_CALL_TYPE_NON_STD_OTASP:

      is_so_found = cmcall_set_so_voice(call_mode_info_ptr);

      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_TYPE_SMS:

      /* Here CM just follows MC's logic to determine
      *  SO for SMS based on vocoder capability.
      *  If this logic needs to be refined or modified
      *  then both MC and CM need to change together.
      */
      call_mode_info_ptr->srv_opt = CAI_SO_RS2_SMS;
      break;

    #ifdef FEATURE_GPSONE
    case CM_CALL_TYPE_PD:
      call_mode_info_ptr->srv_opt = CAI_SO_RS2_PD;
      break;
    #endif /* FEATURE_GPSONE */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_CALL_TYPE_CS_DATA:
    case CM_CALL_TYPE_PS_DATA:
    case CM_CALL_TYPE_TEST:
    default:

      is_so_found= FALSE;
      CM_MSG_HIGH_1( "Unexpected call type=%d",call_ptr->call_type);
      break;
  }

  return is_so_found;
}




/*===========================================================================

FUNCTION cmcall_send_mc_orig

DESCRIPTION
  Send an origination command to MC.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_send_mc_orig(

    cmcall_s_type    *call_ptr
        /* pointer to a call object */
)
{
  #ifndef FEATURE_MMODE_REMOVE_1X
  cdma_cmcall_type *call_mode_info_ptr = NULL;
      /* Pointer to call mode info */

  mc_msg_type    *mc_ptr  = cm_mc_get_buf_else_err_fatal();
      /* allocate a MC command buffer */

  cmss_s_type              *ss_ptr       = cmss_ptr();
      /* point at serving_system object */

  cm_num_details_for_conversion_s_type num_details;
      /* Details before calling plus to digits conversion api */

  cm_num_s_type                       converted_num;
      /* Converted digits as a result of calling
      ** plus to digits conversion api
      */

  cmutil_conver_result_e_type         conver_result = CMUTIL_CONVER_RESULT_NONE;
      /* Result of calling plus to digits conversion api */

  cmutil_num_conv_req_e_type         num_conv_req = CMUTIL_NUM_CONV_REQ_NONE;
      /* Initialize num conversion request */

  #if defined (FEATURE_LTE_TO_1X)
  cmsds_s_type                            *sds_ptr               = cmsds_ptr();
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( mc_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset (&num_details, 0, sizeof(num_details));
  memset (&converted_num, 0, sizeof(converted_num));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

  /* Indicate that this is an origination command.
  */
  #if defined (FEATURE_LTE_TO_1X)
  if (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB && sds_ptr->is_1xcsfb_activated)
  {
    /* Call type is 1XCSFB and 1XCP is activated in CSFB mode
    ** So send the CSFB origination command
    */
    mc_ptr->hdr.cmd = MC_CSFB_ORIGINATION_F;
  } else
  #endif
  {
    mc_ptr->hdr.cmd = MC_ORIGINATION_F;
  }
  mc_ptr->origination.call_id = cmcall_call_id_get( call_ptr );

  #ifdef FEATURE_IS2000_REL_A
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mc_ptr->origination.orig_reason = call_ptr->sr.re_orig_reason;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mc_ptr->origination.orig_count = call_ptr->sr.re_orig_count;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mc_ptr->origination.is_packet_data =
    is_pkt_data_srv_opt(call_mode_info_ptr->srv_opt, CMXCALL_MOBILE_ORIGINATED);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mc_ptr->origination.is_dormant_packet_data =
    ( cmph_ptr()->packet_state == CM_PACKET_STATE_DORMANT ) ? TRUE : FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mc_ptr->origination.is_emergency = (call_ptr->call_type==CM_CALL_TYPE_EMERGENCY);

  mc_ptr->origination.sr_id_included = call_mode_info_ptr->sr_id_included;
  mc_ptr->origination.sr_id = ( call_mode_info_ptr->sr_id_included ) ?
      call_mode_info_ptr->sr_id : 0;

  mc_ptr->origination.qos_parms_incl = call_mode_info_ptr->qos_parms_incl;
  if ( mc_ptr->origination.qos_parms_incl )
  {
    mc_ptr->origination.qos_parms_len = memscpy( mc_ptr->origination.qos_parms,
                                                 sizeof(mc_ptr->origination.qos_parms),
                                                 call_mode_info_ptr->qos_parms,
                                                 (size_t) call_mode_info_ptr->qos_parms_len );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_MSG_HIGH_5("MC_ORIG: is_sr=%d, sr_cnt=%d, is_emrg=%d is_pkt=%d, is_dormant_pkt=%d",
            mc_ptr->origination.orig_reason,
            mc_ptr->origination.orig_count,
            mc_ptr->origination.is_emergency,
            mc_ptr->origination.is_packet_data,
            mc_ptr->origination.is_dormant_packet_data);
 
  #endif /* FEATURE_IS2000_REL_A */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mc_ptr->origination.service_option = call_mode_info_ptr->srv_opt;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Until MC cleans its code from DS support,
  ** make sure it knows that CM is the one to originate.
  ** Gotta stay under feature as it is under feature flag in mc.h
  */
  #ifdef FEATURE_DS
  mc_ptr->origination.ds_originated = FALSE;
  mc_ptr->origination.amps_data     = FALSE;
  #endif /* FEATURE_DS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy called party number structure.
  */

  /* Depending on whether numbers were modified by call control */
  if (call_ptr->cdma_cc_modified_info.call_control_result == CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS)
  {


    mc_ptr->origination.number_plan = call_ptr->num.number_plan;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Init num conversion params */
    cm_util_init_num_conversion_params ( &num_conv_req,
                     &num_details );

    /* Operation to be performed on num */
    num_conv_req = CMUTIL_NUM_CONV_REQ_PLUS_TO_DIGITS;

    /* Fill the number details before calling conversion */
    num_details.call_type                   = call_ptr->call_type;

    num_details.current_mobile_country_code = ss_ptr->info.current_mcc;

    num_details.home_mobile_country_code    = ss_ptr->info.home_mcc;

    num_details.num                         = call_ptr->num;

    num_details.hs_based_plus_dial_setting  = ss_ptr->info.plus_dial_setting;


    /* If number begins with "+" try to convert into digits */
    conver_result  = cm_util_perform_num_conversion (  num_conv_req,
                                                      &num_details,
                                                      &converted_num
                                                    );

    CM_MSG_HIGH_1 (" Result of cm_util_perform_num_conversion %d",
                   conver_result);

    if (conver_result == CMUTIL_CONVER_RESULT_NUM_MODIFIED)
    {
      CM_MSG_HIGH_0 (" Copying converted number");

      mc_ptr->origination.digit_mode =
      CM_BOOLEAN( CM_NUM_DIGIT_MODE(&converted_num) == CM_DIGIT_MODE_8BIT_ASCII );

      /* Copy from converted number */
      mc_ptr->origination.cnt =
      cm_num_get( &converted_num, mc_ptr->origination.called_address,
      sizeof(mc_ptr->origination.called_address) );

      mc_ptr->origination.number_type = converted_num.number_type;

    }
    else
    {
      mc_ptr->origination.digit_mode =
      CM_BOOLEAN( CM_NUM_DIGIT_MODE(&call_ptr->num) == CM_DIGIT_MODE_8BIT_ASCII );

      /* Copy from number in call object */
      mc_ptr->origination.cnt =
      cm_num_get( &call_ptr->num, mc_ptr->origination.called_address,
      sizeof(mc_ptr->origination.called_address) );

      mc_ptr->origination.number_type = call_ptr->num.number_type;

    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    mc_ptr->origination.called_party_subaddr.extension_bit =
      call_ptr->num.subaddr.extension_bit;

    #ifndef FEATURE_MMODE_REMOVE_1X
    mc_ptr->origination.called_party_subaddr.subadd_type =
      (cai_subaddress_types_type)call_ptr->num.subaddr.subadd_type;
    mc_ptr->origination.called_party_subaddr.odd_even_ind =
      (cai_subaddress_eo_type)call_ptr->num.subaddr.odd_even_ind;
    #else
    #error code not present
#endif

    mc_ptr->origination.called_party_subaddr.num_char = memscpy( mc_ptr->origination.called_party_subaddr.chari,
                                                                 sizeof(mc_ptr->origination.called_party_subaddr.chari),
                                                                 call_ptr->num.subaddr.chari,
                                                                 call_ptr->num.subaddr.num_char );
  }
  else
  {
    mc_ptr->origination.digit_mode =
      CM_BOOLEAN( CM_NUM_DIGIT_MODE(&call_ptr->cdma_cc_modified_info.num) == CM_DIGIT_MODE_8BIT_ASCII );

    mc_ptr->origination.number_plan = call_ptr->cdma_cc_modified_info.num.number_plan;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Init num conversion params */
    cm_util_init_num_conversion_params ( &num_conv_req,
                     &num_details );

    /* Operation to be performed on num */
    num_conv_req = CMUTIL_NUM_CONV_REQ_PLUS_TO_DIGITS;

    /* Fill the number details before calling conversion */
    num_details.call_type                   = call_ptr->call_type;

    num_details.current_mobile_country_code = ss_ptr->info.current_mcc;

    num_details.home_mobile_country_code  = ss_ptr->info.home_mcc;

    num_details.num                       = call_ptr->cdma_cc_modified_info.num;

    /* If number begins with "+" try to convert into digits */
    conver_result  = cm_util_perform_num_conversion (  num_conv_req,
                                                  &num_details,
                                                      &converted_num
                                                     );

    CM_MSG_HIGH_1 (" Result of cm_util_perform_num_conversion %d",
                   conver_result);

    if (conver_result == CMUTIL_CONVER_RESULT_NUM_MODIFIED)
    {
      CM_MSG_HIGH_0 (" Copying converted number");

      /* Copy from converted number */
      mc_ptr->origination.cnt =
      cm_num_get( &converted_num, mc_ptr->origination.called_address,
      sizeof(mc_ptr->origination.called_address) );

      mc_ptr->origination.number_type = converted_num.number_type;


    }
    else
    {
      /* Copy from number in call object */
      mc_ptr->origination.cnt =
      cm_num_get( &call_ptr->cdma_cc_modified_info.num,
                  mc_ptr->origination.called_address,
                  sizeof(mc_ptr->origination.called_address) );

      mc_ptr->origination.number_type =
                  call_ptr->cdma_cc_modified_info.num.number_type;

    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/




    mc_ptr->origination.called_party_subaddr.extension_bit =
      call_ptr->cdma_cc_modified_info.num.subaddr.extension_bit;
    #ifndef FEATURE_MMODE_REMOVE_1X
    mc_ptr->origination.called_party_subaddr.subadd_type =
      (cai_subaddress_types_type)call_ptr->cdma_cc_modified_info.num.subaddr.subadd_type;
    mc_ptr->origination.called_party_subaddr.odd_even_ind =
      (cai_subaddress_eo_type)call_ptr->cdma_cc_modified_info.num.subaddr.odd_even_ind;
    #else
    #error code not present
#endif

    mc_ptr->origination.called_party_subaddr.num_char = memscpy( mc_ptr->origination.called_party_subaddr.chari,
                                                                 sizeof(mc_ptr->origination.called_party_subaddr.chari),
                                                                 call_ptr->cdma_cc_modified_info.num.subaddr.chari,
                                                                 call_ptr->cdma_cc_modified_info.num.subaddr.num_char );
  }

  /* Copy calling party number structure.
  */
  mc_ptr->origination.calling_party_number.number_type =
    call_ptr->calling_num.number_type;
  mc_ptr->origination.calling_party_number.number_plan =
    call_ptr->calling_num.number_plan;
  #ifndef FEATURE_MMODE_REMOVE_1X
  mc_ptr->origination.calling_party_number.pi =
    (cai_call_party_num_pi_type)call_ptr->calling_num.pi;
  mc_ptr->origination.calling_party_number.si =
    (cai_call_party_num_si_type)call_ptr->calling_num.si;
  mc_ptr->origination.calling_party_subaddr.subadd_type =
    (cai_subaddress_types_type)call_ptr->calling_num.subaddr.subadd_type;
  mc_ptr->origination.calling_party_subaddr.odd_even_ind =
    (cai_subaddress_eo_type)call_ptr->calling_num.subaddr.odd_even_ind;
  #else
  #error code not present
#endif

  mc_ptr->origination.calling_party_number.num_char = memscpy( mc_ptr->origination.calling_party_number.chari,
                                                               sizeof(mc_ptr->origination.calling_party_number.chari),
                                                               call_ptr->calling_num.buf,
                                                               call_ptr->calling_num.len );
  mc_ptr->origination.calling_party_subaddr.extension_bit =
    call_ptr->calling_num.subaddr.extension_bit;
  mc_ptr->origination.calling_party_subaddr.num_char = memscpy( mc_ptr->origination.calling_party_subaddr.chari,
                                                                sizeof(mc_ptr->origination.calling_party_subaddr.chari),
                                                                call_ptr->calling_num.subaddr.chari,
                                                                call_ptr->calling_num.subaddr.num_char );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For targets with OTASP, tell MC whether this an OTASP call,
  ** and if so indicate the activation code, as well.
  */
  #ifdef FEATURE_OTASP
  if( CM_CALL_TYPE_IS_OTASP(call_ptr->call_type) )
  {
    mc_ptr->origination.otasp_activation = TRUE;
    mc_ptr->origination.activate_code    =
        otasp_code_cm2mc( call_mode_info_ptr->otasp_info.otasp_act_code );
  }
  else
  {
    mc_ptr->origination.otasp_activation = FALSE;
  }
  #endif /* FEATURE_OTASP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_IS2000
  mc_ptr->origination.drs = call_mode_info_ptr->drs_bit;
  #endif /* FEATURE_IS2000 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mc_ptr->origination.is_special_srv_data_call =
                       call_mode_info_ptr->is_special_srv_data_call;
  CM_MSG_HIGH_1("MC_ORIG: special_srv =%d",
            mc_ptr->origination.is_special_srv_data_call);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_HDR_HANDOFF
  mc_ptr->origination.last_act_data_net =
         map_cm_mode_to_mc_mode( call_ptr->last_act_data_net );
  #endif /* FEATURE_HDR_HANDOFF */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Send the origination command to MC and set orig_sent_on_mode to TRUE
  */
  cm_mc_send_cmd( mc_ptr );
  call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_CDMA;

  /* Log MC origination */
  cmlog_call_orig_send_mc_orig();
  #else
#error code not present
#endif
}

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */




#ifdef FEATURE_HDR
/*===========================================================================

FUNCTION cmcall_send_hdrmc_orig

DESCRIPTION
  Send an origination command to HDRMC.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_send_hdrmc_orig(

    cmcall_s_type    *call_ptr
        /* pointer to a call object */
)
{
  hdrcp_msg_req_u      *hdrcp_req;

  CM_ASSERT( call_ptr != NULL );

  hdrcp_req = (hdrcp_msg_req_u*) cm_mem_malloc(sizeof(hdrcp_msg_req_u));

  hdrcp_req->origination.drs = call_ptr->cmcall_mode_info.info.cdma_info.drs_bit;
  #ifdef FEATURE_HDR_HANDOFF
  hdrcp_req->origination.last_act_data_net =
                    map_cm_mode_to_hdrmc_mode( call_ptr->last_act_data_net);
  #endif
  hdrcp_req->origination.call_id = cmcall_call_id_get( call_ptr );

  CM_MSG_HIGH_0("CM->MSGR: send HDR_CP_CALL_ORIGINATION_REQ");
  (void) cm_msgr_send( HDR_CP_CALL_ORIGINATION_REQ, MSGR_MM_CM,
                       (msgr_hdr_struct_type*)hdrcp_req, sizeof(hdrcp_msg_req_u) );

  cm_mem_free(hdrcp_req);


  call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_HDR;

  /* Log HDRMC origination */
  cmlog_call_orig_send_hdr_orig();

}
#endif // FEATURE_HDR

/*===========================================================================

FUNCTION cmxcall_is_first_originated_voice_call

DESCRIPTION
  This function searches the priority queue to find the voice call object on
  top of the queue. The top call object is the first originated call.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE:  if the current call is the top call object in the priority queue
  FALSE: Otherwise

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmxcall_is_first_originated_voice_call(
                  cmcall_s_type       *call_ptr
                  /* Pointer to the call object of the call
                  ** for which origination is being attempted */
                  )

{
  cm_call_id_type  top_pri_voice_call_id;
     /* Call id of the call obj which is on top of the
     ** all Voice calls in the Priority Queue */

  cm_orig_q_s_type *ph_orig_q_ptr = NULL;
     /* Pointer to the object in Priority Queue */

  cmcall_s_type   *top_pri_voice_call_ptr = NULL;

  cm_ss_e_type    ss  = CM_SS_MAIN;
    /* Currently CDMA service can only be acquired on MAIN stack
    ** so we only need to search the pri queue for CM_SS_MAIN.
    ** If this behavior changes in future, the ss value must be
    ** accordingly initialized */

  ph_orig_q_ptr = cmtask_orig_para_get_top( ss );

  if( ph_orig_q_ptr != NULL )
  {
    while( ph_orig_q_ptr->act_type != CM_ACT_TYPE_PH_OBJ )
    {
      CM_MSG_HIGH_1( "Top Object in Priority Queue: %d",
                   ph_orig_q_ptr->act_type);
      if( ph_orig_q_ptr->act_type == CM_ACT_TYPE_VOICE_CALL )
      {
        CM_MSG_HIGH_1( "Found Top Call Object: Call ID %d",
                     ph_orig_q_ptr->orig->act_id);

        /* Store the call_id & update is call mode info type */
        top_pri_voice_call_id = (cm_call_id_type) ph_orig_q_ptr->orig->act_id;

        top_pri_voice_call_ptr = cmcall_ptr(top_pri_voice_call_id);

        if( top_pri_voice_call_ptr == NULL )
        {
          CM_ERR_0( "Call Object Not Found");
          return FALSE;
        }

        if( top_pri_voice_call_ptr->call_id == call_ptr->call_id )
        {
          
          CM_MSG_HIGH_2( "Match Found. top_pri_voice_call_ptr->call_id %d, call_ptr->call_id %d",
                       top_pri_voice_call_ptr->call_id, call_ptr->call_id);
          return TRUE;
        }
        else
        {
          
          CM_MSG_HIGH_2( "Match NOT Found. top_pri_voice_call_ptr->call_id %d, call_ptr->call_id %d",
                       top_pri_voice_call_ptr->call_id, call_ptr->call_id);
          return FALSE;
        }
      }

      /* Get next object in Priority Queue */
        ph_orig_q_ptr = cmtask_orig_para_get_next( ph_orig_q_ptr, ss );
        if ( ph_orig_q_ptr == NULL )
        {
          CM_MSG_HIGH_0( "orig_top is NULL");
          break;
        }
    }
  }

  CM_MSG_HIGH_0( "ph_orig_q_ptr NULL");
  return FALSE;
}

/*===========================================================================

FUNCTION cmxcall_send_orig

DESCRIPTION
  Send an origination command to MC.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_send_orig(

    cmcall_s_type    *call_ptr
        /* pointer to a call object */
)
{
  #ifdef FEATURE_HDR
  cmss_s_type   *ss_ptr  =  cmss_ptr();
  #endif

  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  sys_drvcc_type_e_type drvcc_type_calc; 
  #endif
  boolean       is_pri_voice_call;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  drvcc_type_calc = cmcall_drvcc_get_setup_call_drvcc_type(call_ptr);
  if(cmcall_drvcc_is_3gpp2_drvcc_type(drvcc_type_calc))
  {
    cmcall_drvcc_send_drvcc_orig(call_ptr, drvcc_type_calc);
  }
  else  
  #endif /* defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) */
  
  {
  #ifdef FEATURE_HDR
  if(call_ptr->ss == CM_SS_HDR ||
      ( call_ptr->ss == CM_SS_MAIN &&
        ss_ptr->info.sys_mode == SYS_SYS_MODE_HDR
      )
    )
  {
    #ifdef FEATURE_DEDICATED_C2K_MODE
    CM_MSG_HIGH_0("DCM: Opening HDR session");
    ss_ptr->is_hdr_session_open = TRUE;
    cmph_update_ddtm_status( cmph_ptr() );
    #endif
    CM_MSG_HIGH_0("Send HDRMC orig");
    cmcall_send_hdrmc_orig( call_ptr );
  }
  else
  #endif /* FEATURE_HDR */
  {
    /* Check if call origination is allowed. Check if the call is
    ** the first call that was originated, if not End it. This
    ** check is done because two simultaneous 1x Voice MO calls
    ** are not allowed */

    is_pri_voice_call = cmxcall_is_first_originated_voice_call( call_ptr );
    /* If this call is not on the top of call objects in the
    ** Priority queue and an MO call, then end this call
    */
    CM_MSG_HIGH_1( "is_pri_voice_call %d", is_pri_voice_call);
    if( !is_pri_voice_call &&
        call_ptr->direction == CM_CALL_DIRECTION_MO &&
        call_ptr->call_type == CM_CALL_TYPE_VOICE )
    {
      /* End the call since there is another CDMA call that
      ** has either gone through or is in redial waiting to
      ** be sent to MC */
      CM_MSG_HIGH_1( "Call %d not originated: Another MO call in Queue",
                   call_ptr->call_id);
      call_ptr->end_status = CM_CALL_END_INCOMPATIBLE;

      cmcall_end( call_ptr );
      /*  after ending the call, set the call ptr to Null, as memory
      **  allocated to call ptr would be released.
      */
      call_ptr = NULL;
	  
      return;
    }

    CM_MSG_HIGH_0("Send MC orig");
    cmcall_send_mc_orig( call_ptr );
  }
  }

  CM_ASSERT(call_ptr->sr.orig_sent_on_mode != SYS_SYS_MODE_NONE);

}

/*===========================================================================

FUNCTION  cmxcall_call_cmd_orig_para_check

DESCRIPTION
  Check parameter errors for call origination command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type  cmxcall_call_cmd_orig_para_check
(
  const cm_call_cmd_s_type       *call_cmd_ptr
)
{
  cm_call_cmd_err_e_type    cmd_err        = CM_CALL_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_cmd_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  #if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900))
  if( !BETWEEN(call_cmd_ptr->info.srv_type, CM_SRV_TYPE_NONE, CM_SRV_TYPE_MAX) )
  {
    cmd_err = CM_CALL_CMD_ERR_SRV_TYPE_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_SRV_TYPE_P");
  }
  else if( !cmxcall_is_valid_srv_type(call_cmd_ptr->info.call_type, call_cmd_ptr->info.srv_type) )
  {
    cmd_err = CM_CALL_CMD_ERR_SRV_TYPE_P;
    CM_MSG_HIGH_0( "CM_CALL_CMD_ERR_SRV_TYPE_P");
  }
  else if( call_cmd_ptr->info.srv_type == CM_SRV_TYPE_CDMA_SPECIFIC &&
      !cmxcall_is_valid_srv_opt(call_cmd_ptr->info.call_type,
      call_cmd_ptr->info.cdma_info.srv_opt,
      CMXCALL_MOBILE_ORIGINATED) )
  {
    cmd_err = CM_CALL_CMD_ERR_SRV_OPT_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_SRV_OPT_P");
  }
  else if( call_cmd_ptr->info.srv_type == CM_SRV_TYPE_CDMA_HDR &&
      !cmxcall_is_valid_srv_opt(call_cmd_ptr->info.call_type,
      call_cmd_ptr->info.cdma_info.srv_opt,
      CMXCALL_MOBILE_ORIGINATED) )
  {
    cmd_err = CM_CALL_CMD_ERR_SRV_OPT_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_SRV_OPT_P");
  }
  #ifdef FEATURE_HDR_HYBRID
  else if( (call_cmd_ptr->info.call_type == CM_CALL_TYPE_PKT_DATA) &&
           (call_cmd_ptr->info.srv_type  == CM_SRV_TYPE_AUTOMATIC) &&
           (cmph_get_user_mode_pref() != CM_MODE_PREF_HDR_ONLY) &&
           (!cmxcall_is_auto_pkt_data_so(call_cmd_ptr->info.cdma_info.srv_opt)) )
  {
    cmd_err = CM_CALL_CMD_ERR_SRV_OPT_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_SRV_OPT_P");
  }
  #endif  /* FEATURE_HDR_HYBRID */
  else if( call_cmd_ptr->info.alpha.len > CM_MAX_ALPHA_LEN_FOR_CALL_ORIG )
  {
    cmd_err = CM_CALL_CMD_ERR_ALPHA_LEN_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_ALPHA_LEN_P");
  }
  #ifdef FEATURE_OTASP
  /* Currently, we are not supporting Band Class 2 & 13 */
  else if ( (call_cmd_ptr->info.call_type == CM_CALL_TYPE_STD_OTASP)      &&
            !( sd_misc_is_target_supp_mode_band_class(SD_SS_MODE_PREF_CDMA,
               cmph_map_otasp_act_code_to_sd_band(call_cmd_ptr->info.cdma_info.otasp_act_code))) )
  {
    cmd_err = CM_CALL_CMD_ERR_ACTIVATE_CODE_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_ACTIVATE_CODE_P");
  }
  else if ( (call_cmd_ptr->info.call_type == CM_CALL_TYPE_STD_OTASP)   &&
            (call_cmd_ptr->info.cdma_info.otasp_act_code > CM_ACTIVATE_CODE_BC18_PSB_BAND))
  {
    cmd_err = CM_CALL_CMD_ERR_ACTIVATE_CODE_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_ACTIVATE_CODE_P");
  }
  else if ((call_cmd_ptr->info.call_type == CM_CALL_TYPE_NON_STD_OTASP) &&
           (call_cmd_ptr->info.cdma_info.otasp_act_code != CM_PREFERRED) )
  {
    cmd_err = CM_CALL_CMD_ERR_ACTIVATE_CODE_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_ACTIVATE_CODE_P");
  }
  #endif /* FEATURE_OTASP */

  #ifdef FEATURE_HDR_HANDOFF
  else if( !BETWEEN(call_cmd_ptr->info.cdma_info.last_act_data_net,
                    SYS_SYS_MODE_NONE,
                    SYS_SYS_MODE_MAX) )
  {
    cmd_err = CM_CALL_CMD_ERR_LAST_ACT_DATA_NET_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_LAST_ACT_DATA_NET_P");
  }
  #endif /* FEATURE_HDR_HANDOFF */
  #endif /* (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900)) */

  return cmd_err;
} /* cmxcall_call_cmd_orig_para_check() */



/*===========================================================================

FUNCTION  cmcall_call_cmd_end_para_check

DESCRIPTION
  Check parameter errors for call end command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_cmd_err_e_type  cmcall_call_cmd_end_para_check(
  const cm_call_cmd_s_type       *call_cmd_ptr
)
{
  /* Point at call command information.
  */
  const cm_call_cmd_info_s_type *cmd_info_ptr = NULL;
  cm_call_cmd_err_e_type         cmd_err      = CM_CALL_CMD_ERR_NOERR;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_cmd_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CALL_CMD_INFO_PTR(call_cmd_ptr);

  /* Check integrity of command parameters */

  if( cmd_info_ptr->num_end_call_ids < 1 ||
      cmd_info_ptr->num_end_call_ids > CMCALL_MAX_CALL_OBJ )
  {
    cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_CALL_ID_P");
  }
  else
  {
    int i;                                                   /* loop index */
    for ( i = 0; i < cmd_info_ptr->num_end_call_ids; i++ )
    {

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* cmxcall should end calls of info_type CDMA only
      */
      if (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
           != CM_CALL_MODE_INFO_CDMA)
      {
        continue;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      if( cmcall_ptr( cmd_info_ptr->end_params[i].call_id ) == NULL )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
        CM_ERR_0( "CM_CALL_CMD_ERR_CALL_ID_P");
        break;
      } /* if( cmcall_ptr( cmd_info_ptr->end_call_ids[i] ) == NULL ) */

    } /* for ( i = 0; i < cmd_info_ptr->num_end_call_ids; i++ ) */

  } /* if( cmd_info_ptr->num_end_call_ids < 1 ||  */

  return cmd_err;
} /* cmcall_call_cmd_end_para_check() */




/*===========================================================================

FUNCTION  cmcall_call_cmd_flash_para_check

DESCRIPTION
  Check parameter errors for call flash command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_cmd_err_e_type  cmcall_call_cmd_flash_para_check(
  const cm_call_cmd_s_type       *call_cmd_ptr
)
{
  /* Point at call command information.
  */
  const cm_call_cmd_info_s_type *cmd_info_ptr = NULL;
  cm_call_cmd_err_e_type         cmd_err      = CM_CALL_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_cmd_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CALL_CMD_INFO_PTR(call_cmd_ptr);

  /* Check integrity of command parameters */

  if( cmcall_ptr( cmd_info_ptr->call_id ) == NULL )
  {
    cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_CALL_ID_P");
  }

  return cmd_err;
} /* cmcall_call_cmd_flash_para_check() */




/*===========================================================================

FUNCTION  cmcall_call_cmd_privacy_pref_para_check

DESCRIPTION
  Check parameter errors for call privacy_pref command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_cmd_err_e_type  cmcall_call_cmd_privacy_pref_para_check(
  const cm_call_cmd_s_type       *call_cmd_ptr
)
{
  const cm_call_cmd_info_cdma_s_type *cmd_mode_info_ptr = NULL;
  cm_call_cmd_err_e_type              cmd_err           = CM_CALL_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_cmd_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_mode_info_ptr = CALL_CMD_MODE_INFO_PTR(call_cmd_ptr);

  /* Check integrity of command parameters */

  #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)

    if( cmd_mode_info_ptr->privacy_pref <= CM_PRIVACY_PREF_NONE ||
        cmd_mode_info_ptr->privacy_pref >= CM_PRIVACY_PREF_MAX )
    {
      cmd_err = CM_CALL_CMD_ERR_BAD_PRIVACY_PREF_S;
      CM_ERR_0( "CM_CALL_CMD_ERR_BAD_PRIVACY_PREF_S");
    }

  #endif   /* #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

  return cmd_err;
} /* cmcall_call_cmd_privacy_pref_para_check() */


#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */


#ifdef FEATURE_OTASP_HOLD_CMD_IN_COMMIT

/*===========================================================================

FUNCTION cmxcall_is_conv_pc_otapa_commit

DESCRIPTION
  Finds if any paging channel otapa call is in the process of commit.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - call found; FALSE - Not.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmxcall_is_conv_pc_otapa_commit
(
  void
)
{
  #ifdef FEATURE_OTASP_HOLD_CMD_IN_COMMIT
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  boolean         ret_status = FALSE;                     /* Return status */
  cm_iterator_type  call_itr;                      /* Call Object Iterator */


  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);
  call_ptr = cmcall_obj_get_next(&call_itr);


  while(call_ptr != NULL)
  {

    if ( call_ptr->call_state == CM_CALL_STATE_CONV &&
         call_ptr->call_subst.conv == CM_CALL_CONV_SUBST_PC_OTAPA_COMMIT )
    {
      ret_status = TRUE;
      break;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return ret_status;
  #else
  return FALSE;
  #endif

} /* cmxcall_is_conv_pc_otapa_commit() */
#endif /* FEATURE_OTASP_HOLD_CMD_IN_COMMIT */



/*===========================================================================

FUNCTION cmxcall_is_pc_otapa

DESCRIPTION
  Finds if any paging channel otapa call is in the process of commit.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - call found; FALSE - Not.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmxcall_is_pc_otapa
(
  void
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  boolean         ret_status = FALSE;                     /* Return status */
  cm_iterator_type  call_itr;                      /* Call Object Iterator */
  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);
  call_ptr = cmcall_obj_get_next(&call_itr);


  while(call_ptr != NULL)
  {

      if (( call_ptr->call_state == CM_CALL_STATE_INCOM &&
            call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_OTAPA ) ||
          ( call_ptr->call_state == CM_CALL_STATE_CONV &&
            call_ptr->call_subst.conv == CM_CALL_CONV_SUBST_PC_OTAPA_REGULAR ) ||
          ( call_ptr->call_state == CM_CALL_STATE_CONV &&
            call_ptr->call_subst.conv == CM_CALL_CONV_SUBST_PC_OTAPA_COMMIT )
          )
      {
        ret_status = TRUE;
        break;
      }  /* if (call_ptr->call_state */

      call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

  return ret_status;

} /* cmxcall_is_pc_otapa() */


#ifdef FEATURE_OTASP

/*===========================================================================

FUNCTION cmxcall_otasp_commit_call_ids

DESCRIPTION
  Computes the call ids that are associated with otasp commit.
  If std orasp, non-std orasp, or otapa, then there is only one call id.
  For busy otapa, it returns all call ids that are in conversation.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - call id found; FALSE - Not.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmxcall_otasp_commit_call_ids
(
  int               *num_call_id,
  cm_call_id_type   *call_ids
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  boolean         ret_status = FALSE;                     /* Return status */
  cm_iterator_type  call_itr;                      /* Call Object Iterator */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(num_call_id           != NULL);
  CM_ASSERT(call_ids           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  *num_call_id = 0;

  cmcall_obj_iterator_init(&call_itr);
  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {

    if ( call_ptr->call_state == CM_CALL_STATE_CONV )
    {
      ret_status = TRUE;
      if ( call_ptr->call_type == CM_CALL_TYPE_STD_OTASP ||
        call_ptr->call_type == CM_CALL_TYPE_NON_STD_OTASP ||
        call_ptr->call_type == CM_CALL_TYPE_OTAPA )
      {
        call_ids[0] = call_ptr->call_id;
        *num_call_id = 1;
        break;
      } /* if ( call_ptr->call_type == CM_CALL_TYPE_STD_OTASP || ) */
      else
      {
        call_ids[ *num_call_id ] = call_ptr->call_id;
        *num_call_id += 1;
      } /* else of if ( call_ptr->call_type == CM_CALL_TYPE_STD_OTASP || ) */
    } /* if ( call_ptr != NULL ) */

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while*/

  return ret_status;
} /* cmxcall_otasp_commit_call_ids() */
#endif /* FEATURE_OTASP */


#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */


/*===========================================================================

FUNCTION  cmxcall_client_cmd_check

DESCRIPTION
  Check for call command parameter errors and whether a specified call command
  is allowed in the current state of the call/phone.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if command is allowed in the current state
  of the call/phone, specific cm_call_cmd_err_e_type otherwise.

SIDE EFFECTS
  If originating dial-string matches one of the special numbers that is
  stored in the phone-book, this function changes the call type to E911,
  OTASP, etc.

===========================================================================*/
cm_call_cmd_err_e_type  cmxcall_client_cmd_check(

    cm_call_cmd_s_type          *call_cmd_ptr
        /* Pointer to a CM command */
)
{

  cm_call_cmd_info_s_type      *cmd_info_ptr      = NULL;
    /* Point at call command information. */

  cm_call_cmd_info_cdma_s_type *cmd_mode_info_ptr = NULL;

  cmcall_s_type                *call_ptr = NULL;
        /* Pointer to a call object */

  cdma_cmcall_type             *call_mode_info_ptr;
        /* Pointer to mode information */

  cmph_s_type                  *ph_ptr       = cmph_ptr();
      /* Point at phone object */

  cmss_s_type                  *ss_ptr       = cmss_ptr();
      /* point at serving_system object */

  cm_call_cmd_err_e_type        cmd_err       = CM_CALL_CMD_ERR_NOERR;
      /* Initialize command error to NO-ERROR */

  int i;
      /* Loop counter */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );
  CM_ASSERT( ph_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr = CALL_CMD_INFO_PTR(call_cmd_ptr);

  cmd_mode_info_ptr = CALL_CMD_MODE_INFO_PTR(call_cmd_ptr);

  /* check cmd is already in error or not */

  if (cmd_info_ptr->cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
      return cmd_info_ptr->cmd_err;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether command is possible.
  */
  switch( call_cmd_ptr->cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*-------------------------*/
                       /* Answer an incoming call */
                       /*-------------------------*/

    case CM_CALL_CMD_ANSWER:

      /* check command parameter errors */

      if((cmd_err = cmcall_call_cmd_answer_para_check(call_cmd_ptr)) != CM_CALL_CMD_ERR_NOERR)
      {
          /* stop further processing if any errors found in the command */
          break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cmd_info_ptr->call_id );
      if( call_ptr == NULL )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
        break;
      }

      call_mode_info_ptr = &call_ptr->cmcall_mode_info.info.cdma_info;

      #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      if (call_mode_info_ptr->alert_ans_bypass)
      {
         /* client answer is not allowed */
         cmd_err = CM_CALL_CMD_ERR_ALERT_ANS_BYPASS_S;
         break;
      }
      #endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Only able to answer a call if phone is online.
      */

      if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE &&
          ph_ptr->oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE
        )
      {
        cmd_err = CM_CALL_CMD_ERR_INVALID_MODE_S;
        break;
      }


      if( ss_ptr->info.sys_mode == SYS_SYS_MODE_NO_SRV )
      {
        cmd_err = CM_CALL_CMD_ERR_SRV_STATE_S;
        break;
      }

      switch( call_ptr->call_state )
      {

        case CM_CALL_STATE_INCOM:

          /* Only able to answer if call was not already answered.
          */
          if(call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_ANSWERED)
          {
            cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Verify that this is a legal switch of call type -
          ** i.e client answered with an acceptable call type relative
          ** to the incoming call type.
          */
          else if( ! cmcall_is_valid_answer( call_ptr,
                                             cmd_info_ptr->call_type) )
          {
            cmd_err = CM_CALL_CMD_ERR_CALL_TYPE_S;
          }

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        default:

          /* In all other call states this command makes no sense.
          */
          cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
          break;

      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*---------------------------*/
                       /* End call/call-origination */
                       /*---------------------------*/

    case CM_CALL_CMD_END:

      /* check command parameter errors */
      if((cmd_err = cmcall_call_cmd_end_para_check(call_cmd_ptr)) != CM_CALL_CMD_ERR_NOERR)
      {
          /* stop further processing if any errors found in the command */
          break;
      }

      /* Check state for each call */
      for( i = 0; i < cmd_info_ptr->num_end_call_ids; i++) {

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


        /* Reassign call_ptr for each call */
        call_ptr = cmcall_ptr( cmd_info_ptr->end_params[i].call_id );
        if(call_ptr == NULL )
        {
          continue;
        }

        /* cmxcall should end calls of info_type CDMA only
        */
          /* However for 1xsrlte calls , mode might not be set yet , hence check is_waiting_on_esr_rsp
          */
        if (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
             != CM_CALL_MODE_INFO_CDMA && call_ptr->is_waiting_on_esr_rsp == FALSE)
        {
          continue;
        }

        /* Only able to end a call if phone is online */
        if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE &&
            ph_ptr->oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE
          )
        {
          cmd_err = CM_CALL_CMD_ERR_OFFLINE_S;
          break;
        }

        if( call_ptr->call_state == CM_CALL_STATE_IDLE )
        {
            /*  Waiting for end_cnf.
            */
            cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
            break;
        }

        #ifdef FEATURE_OTASP_HOLD_CMD_IN_COMMIT

        /* If an end command is already in the hold command q,
        ** return error.
        */
        if ( CMCALL_IS_CONV_COMMIT(call_ptr) ||
             CMCALL_IS_CONV_PC_OTAPA_COMMIT(call_ptr) )
        {
          if ( cm_hold_cmd_q_check( (cm_cmd_type*)call_cmd_ptr ) )
          {
            CM_MSG_HIGH_0("Error: Rej next end cmd");
            cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
            break;
          }
        }

        #endif /* FEATURE_OTASP_HOLD_CMD_IN_COMMIT */

      } /* for( i = 0; i < cmd_info_ptr->num_end_call_ids; i++) */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*----------------------------*/
                       /* Send Flash/Flash with info */
                       /*----------------------------*/

    case CM_CALL_CMD_SUPS:

      /* check command parameter errors */
      if((cmd_err = cmcall_call_cmd_flash_para_check(call_cmd_ptr)) != CM_CALL_CMD_ERR_NOERR)
      {
          /* stop further processing if any errors found in the command */
          break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Only able to send a flash if phone is online.
          */
      if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
      {
        cmd_err = CM_CALL_CMD_ERR_OFFLINE_S;
        break;
      }

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if( ss_ptr->info.sys_mode == SYS_SYS_MODE_NO_SRV )
      {
        cmd_err = CM_CALL_CMD_ERR_SRV_STATE_S;
        break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cmd_info_ptr->call_id );

      if(call_ptr == NULL)
      {
         CM_MSG_HIGH_0( "call_ptr_NULL");
         break ;
      }

      switch( call_ptr->call_state )
      {

        #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

        case  CM_CALL_STATE_INCOM:
          /* Flash is not allowed in AMPS calls.
          */
          if( !cm_is_sys_mode_digital( ss_ptr->info.sys_mode ))
          {
            cmd_err = CM_CALL_CMD_ERR_FLASH_S;
            break;
          }
          /*Fall Through */


        #endif   /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

         if ((call_ptr->call_type == CM_CALL_TYPE_CS_DATA ||
             call_ptr->call_type == CM_CALL_TYPE_PS_DATA) &&
             (cmd_mode_info_ptr->flash_type == CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD ||
              cmd_mode_info_ptr->flash_type == CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD))
         {
            /* Flash is not allowed in data calls */
            cmd_err = CM_CALL_CMD_ERR_FLASH_S;
            break;
         }
        break;

        case CM_CALL_STATE_CONV:

          #ifdef FEATURE_IS2000_REL_A
          /* If this is a flash with info, consult the phone-book to whether
          ** the 3-way call is allowed under the origination restriction
          ** currently in effect.
          */
          if ( cmd_mode_info_ptr->flash_type !=
            CM_CALL_SUPS_TYPE_SIMPLE_FLASH )
          {
              if( CM_NUM_LEN(&cmd_info_ptr->num) != 0 &&
                  ! cm_nc_is_call_allowed_func(&cmd_info_ptr->num) )
              {
                cmd_err = CM_CALL_CMD_ERR_ORIG_RES_S;
              }
          }

          /* Emergency flash is allowed only for digital calls
          */
          if( !cm_is_sys_mode_digital( ss_ptr->info.sys_mode ) &&
              cmd_mode_info_ptr->flash_type == CM_CALL_SUPS_TYPE_SIMPLE_FLASH )
          {
            cmd_err = CM_CALL_CMD_ERR_FLASH_S;
          }

          #endif /* FEATURE_IS2000_REL_A */

          if (cmd_mode_info_ptr->flash_type == CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD)
          {
            cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
          }
          break;


        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        default:

          /* In all other call states this command makes no sense.
          */
          cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
          break;

      }
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------------*/
                     /*    Change privacy preference       */
                     /*------------------------------------*/

    case CM_CALL_CMD_PRIVACY_PREF:
      /* check command parameter errors */
      if((cmd_err = cmcall_call_cmd_privacy_pref_para_check(call_cmd_ptr)) != CM_CALL_CMD_ERR_NOERR)
      {
          /* stop further processing if any errors found in the command */
          break;
      }

      /* This command is valid if there is a CDMA call in conversation state */
      if (cmcall_is_there_a_cdma_call_in_conv () == CM_CALL_ID_INVALID)
      {
        CM_MSG_HIGH_0 ("No CDMA call active for privacy pref command!");
        cmd_err = CM_CALL_CMD_ERR_BAD_PRIVACY_PREF_S;
      }

      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
    #error code not present
#endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      cmd_err = CM_CALL_CMD_ERR_OTHER;
      CM_ERR_1( "Bad call command %d", (uint8) call_cmd_ptr->cmd);
      break;

  }

  return cmd_err;

}

/*===========================================================================

FUNCTION cmxcall_client_cmd_forward_mc

DESCRIPTION
  Forward a client call commands to MC.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if CM needs to wait for a replay from MC before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmxcall_client_cmd_forward_mc(

    cm_call_cmd_s_type    *call_cmd_ptr
        /* Pointer to a CM command */
)
/*lint -esym(818, cmd_ptr)
** Can't change void *ptr to const void *ptr as then at the very bottom
** of call stack I'd have to change mem_free() to take const void *ptr
** and it's not the right change as memheap.c might decide to change that
** memory (like clear it for example) before freeing it.
** This is because we allocate commands from a heap and then return that
** memory to the heap.
*/
{

  /* Point at call command information.
  */
  const cm_call_cmd_info_s_type      *cmd_info_ptr = NULL;

  const cm_call_cmd_info_cdma_s_type *cmd_mode_info_ptr = NULL;

  cmcall_s_type                      *call_ptr = NULL;
        /* Pointer to a call object */

  #ifndef FEATURE_MMODE_REMOVE_1X
  mc_msg_type                        *mc_ptr;
      /* MC command pointer */

  cm_num_s_type                       converted_num;
      /* Converted digits as a result of calling
      ** plus to digits conversion api
      */

  cm_num_details_for_conversion_s_type num_details;
      /* Details before calling plus to digits conversion api */
  #endif

  int i;
    /* Loop index */

  boolean ok_to_send = FALSE;
    /* flag to indicate whether we can send end command or not */
 #ifdef FEATURE_MMODE_DUAL_SIM
  boolean is_call_ended = FALSE;

   cm_orig_q_s_type * q_ptr = NULL;
#endif
  #ifndef FEATURE_MMODE_REMOVE_1X
       /*HPCD */
  cmss_s_type              *ss_ptr       = cmss_ptr();
      /* point at serving_system object */

  cmutil_conver_result_e_type         conver_result = CMUTIL_CONVER_RESULT_NONE;
      /* Result of calling plus to digits conversion api */

  cmutil_num_conv_req_e_type         num_conv_req = CMUTIL_NUM_CONV_REQ_NONE;
      /* Initialize num conversion request */
      /* HPCD */
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr      = CALL_CMD_INFO_PTR(call_cmd_ptr);
  cmd_mode_info_ptr = CALL_CMD_MODE_INFO_PTR(call_cmd_ptr);

  #ifdef FEATURE_MMODE_REMOVE_1X
  #error code not present
#endif

  switch( call_cmd_ptr->cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*-------------------------*/
                     /* Answer an incoming call */
                     /*-------------------------*/

    case CM_CALL_CMD_ANSWER:
      #ifndef FEATURE_MMODE_REMOVE_1X
      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cmd_info_ptr->call_id );
      CM_ASSERT( call_ptr != NULL );

      /* Get MC command buffer and indicate the
      ** appropriate MC command.
      */
      mc_ptr = cm_mc_get_buf_else_err_fatal();
      mc_ptr->hdr.cmd = MC_ANSWER_F;
      mc_ptr->answer.call_id = cmd_info_ptr->call_id;

      /* Until MC cleans their code from DS support,
      ** make sure they know that CM answered the call.
      */
      #ifdef FEATURE_DS

      if(cmd_info_ptr->call_type == CM_CALL_TYPE_CS_DATA ||
         cmd_info_ptr->call_type == CM_CALL_TYPE_PS_DATA )
      {
         mc_ptr->answer.a_data = TRUE;
      }
      else
      {
         mc_ptr->answer.a_data = FALSE;
      }

      mc_ptr->answer.ds_answered = FALSE;
      #endif  /* FEATURE_DS */

      /* Send command to MC.
      */
      cm_mc_send_cmd( mc_ptr );
      #else
      #error code not present
#endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*---------------------------*/
                     /* End call/call-origination */
                     /*---------------------------*/

    case CM_CALL_CMD_END:

      for ( i = 0; i < cmd_info_ptr->num_end_call_ids; i++ )
      {
        /* Get the pointer to call object */
        call_ptr = cmcall_ptr( cmd_info_ptr->end_params[i].call_id );

        if( call_ptr == NULL )
        {
          CM_MSG_HIGH_1( "Invalid call_id: %d", cmd_info_ptr->end_params[i].call_id);
          continue;
        }
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* cmxcall should end calls of info_type CDMA only
        */
          /* However for 1xsrlte calls , mode might not be set yet , hence check is_waiting_on_esr_rsp
          */
        if (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
             != CM_CALL_MODE_INFO_CDMA && call_ptr->is_waiting_on_esr_rsp == FALSE)
        {
          continue;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        ok_to_send = TRUE;

        #ifdef FEATURE_OTASP_HOLD_CMD_IN_COMMIT
        if ( CMCALL_IS_CONV_COMMIT(call_ptr) ||
             CMCALL_IS_CONV_PC_OTAPA_COMMIT(call_ptr)
           )
        {
          CM_MSG_HIGH_0("Add end cmd to hold_cmd_q");
          cm_hold_cmd_q_add((cm_cmd_type*)call_cmd_ptr, sizeof(cm_call_cmd_s_type));

          ok_to_send = FALSE;
          break;
        }
        else /* if ( CMCALL_IS_CONV_COMMIT(call_ptr) || */
        #endif /* FEATURE_OTASP_HOLD_CMD_IN_COMMIT */
        {
          /* If we are in origination state and call origination command was not
          ** sent to MC, there is no need to send the MC_END_F command.
          */
          if( call_ptr->call_state == CM_CALL_STATE_ORIG &&
              (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE))
          {
            ok_to_send = FALSE;
            CM_MSG_HIGH_1("Rcv End, Cancel unsent Orig for call id %d",
              cmd_info_ptr->end_params[i].call_id);
            #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
            if (cm_is_esr_allowed_in_srlte()
                && cmph_is_oprting_in_1xsrlte_mode(call_ptr->asubs_id)
                && call_ptr->call_type == CM_CALL_TYPE_VOICE)
            {
              if (call_ptr->is_waiting_on_esr_rsp == TRUE)
              {
                cmcsfbcall_send_csfb_abort_req(call_ptr->call_id);
              }
            }
            #endif

          }
        } /* if ( CMCALL_IS_CONV_COMMIT(call_ptr) || */

        if ( ok_to_send )
        {

          /* Gotta copy it here into call object because for
          ** HDR call end command we'll send CM_CALL_EVENT_END_REQ
          ** from cmxcall_send_end_cmd(), BEFORE fields related to
          ** call end command are copied in cmxcall_client_cmd_mc_reply() that
          ** calls on cmcall_cmd_copy_fields()
          */
          call_ptr->end_params.call_id   = cmd_info_ptr->end_params[i].call_id;
          CM_ASSERT(call_ptr->call_id == call_ptr->end_params.call_id);

          call_ptr->end_params.info_type = cmd_info_ptr->end_params[i].info_type;

          call_ptr->end_params.end_params.cdma_end.end_reason_included =
            cmd_info_ptr->end_params[i].end_params.cdma_end.end_reason_included;

          call_ptr->end_params.end_params.cdma_end.end_reason =
            ( cmd_info_ptr->end_params[i].end_params.cdma_end.end_reason_included ) ?
              cmd_info_ptr->end_params[i].end_params.cdma_end.end_reason : CAI_REL_NORMAL;

          call_ptr->end_status      = cmd_info_ptr->end_status;

          cmxcall_send_end_cmd(call_ptr,
                               call_ptr->end_params.end_params.cdma_end.end_reason );
          #ifdef FEATURE_MMODE_DUAL_SIM
          is_call_ended = TRUE;
          #endif
        } /* if ( ok_to_send ) */

      } /* ( i = 0; i < cmd_info_ptr->num_end_call_ids; i++ ) */

      #ifdef FEATURE_MMODE_DUAL_SIM
      if( is_call_ended == TRUE &&\
           (call_ptr!= NULL) && (call_ptr->call_type == CM_CALL_TYPE_SMS) )
      {
        // if there is a voice call pending on SUB-2  or SUB-3 then unforce the preferences for SMS call now
        #if defined FEATURE_MMODE_SXLTE_G || defined FEATURE_MMODE_TRIPLE_SIM
        if((!cmph_is_feature_mode_sglte() && (cmcall_is_there_a_call_type(CM_SS_HYBR_2,
           CM_CALL_TYPE_VOICE, CM_CALL_ID_INVALID)!= CM_CALL_ID_INVALID)) ||\
           cmcall_is_there_a_call_type(CM_SS_HYBR_3,
           CM_CALL_TYPE_VOICE, CM_CALL_ID_INVALID)!= CM_CALL_ID_INVALID )
        #endif
        {
          q_ptr = cmtask_orig_para_search_act_id(CM_SS_MAIN, (uint32)call_ptr->call_id);

          if (q_ptr != NULL)
          {
            /* Remove the objects with priority end and above PH obj */
            if (cmtask_orig_para_delete( CM_SS_MAIN, q_ptr, TRUE) )
            {
              cmph_force_orig_mode_on_the_fly(CM_SS_MAIN);
              CM_MSG_HIGH_0( "Unforce SMS call preferences now without 4 sec wait ");
            }
          }
        }
      }
      #endif

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*----------------------------*/
                     /* Send Flash/Flash with info */
                     /*----------------------------*/

    case CM_CALL_CMD_SUPS:
      #ifndef FEATURE_MMODE_REMOVE_1X
      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cmd_info_ptr->call_id );
      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        break;
      }

      /* Get MC command buffer and indicate the
      ** appropriate MC command.
      */
      mc_ptr = cm_mc_get_buf_else_err_fatal();

      if ((cmd_mode_info_ptr->flash_type == CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD) ||
          (cmd_mode_info_ptr->flash_type == CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD))
      {
        mc_ptr->hdr.cmd = MC_HOLD_F;
        cm_reply_set (&call_ptr->reply,
                      (cm_cmd_type*)call_cmd_ptr,                    /* command waiting for reply */
                      CM_CALL_HOLD_L2ACK_F,       /* wait for this MC report */
                      CMCALL_MAX_MC_REPLY_TIME ); /* wait for this long */
      }
      else
      {
        mc_ptr->hdr.cmd = MC_FLASH_F;
      }

      mc_ptr->flash.call_id = cmd_info_ptr->call_id;


      /*- -- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Plus to digit conversion in case number begins with "+" sign */
      /* Init num conversion params */
      cm_util_init_num_conversion_params ( &num_conv_req, &num_details );

      /* Operation to be performed on num */
      num_conv_req = CMUTIL_NUM_CONV_REQ_PLUS_TO_DIGITS;

      /* Fill the number details before calling conversion */
      num_details.call_type                   = call_ptr->call_type;

      num_details.current_mobile_country_code = ss_ptr->info.current_mcc;

      num_details.home_mobile_country_code    = ss_ptr->info.home_mcc;

      num_details.num                         = cmd_info_ptr->num;

      num_details.hs_based_plus_dial_setting  = ss_ptr->info.plus_dial_setting;


      /* If number begins with "+" try to convert into digits */
      conver_result  = cm_util_perform_num_conversion (  num_conv_req,
                                                         &num_details,
                                                         &converted_num
                                                      );

      CM_MSG_HIGH_1 (" Result of cm_util_perform_num_conversion %d",
                     conver_result);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if (conver_result == CMUTIL_CONVER_RESULT_NUM_MODIFIED)
      {
        CM_MSG_HIGH_0 (" Copying converted number");

        /* Copy the originating 3-Way call number (if any).
        */
        mc_ptr->flash.cnt = cm_num_get( &converted_num,
                                       mc_ptr->flash.called_address,
                                       sizeof(mc_ptr->flash.called_address) );

        #ifdef FEATURE_IS2000_REL_A
        mc_ptr->flash.flash_type = cmd_mode_info_ptr->flash_type;

        if (converted_num.digit_mode == CM_DIGIT_MODE_4BIT_DTMF)
        {
          mc_ptr->flash.digit_mode = FALSE;
        }
        else
        {
          mc_ptr->flash.digit_mode = TRUE;
        }

        mc_ptr->flash.number_plan = converted_num.number_plan;
        mc_ptr->flash.number_type = converted_num.number_type;
        #endif  /* FEATURE_IS2000_REL_A */

      }
      else
      {
        /* Copy the originating 3-Way call number (if any).
        */
        mc_ptr->flash.cnt = cm_num_get( &cmd_info_ptr->num,
                                       mc_ptr->flash.called_address,
                                       sizeof(mc_ptr->flash.called_address) );


        #ifdef FEATURE_IS2000_REL_A
        mc_ptr->flash.flash_type = cmd_mode_info_ptr->flash_type;

        /* We need to pass those from MM UI (later) */
        if (cmd_info_ptr->num.digit_mode == CM_DIGIT_MODE_4BIT_DTMF)
        {
          mc_ptr->flash.digit_mode = FALSE;
        }
        else
        {
          mc_ptr->flash.digit_mode = TRUE;
        }

        mc_ptr->flash.number_plan = cmd_info_ptr->num.number_plan;
        mc_ptr->flash.number_type = cmd_info_ptr->num.number_type;
        #endif  /* FEATURE_IS2000_REL_A */

      }

      /* Send command to MC.
      */
      cm_mc_send_cmd( mc_ptr );
      #else
      #error code not present
#endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------------*/
                     /*       Change privacy preference    */
                     /*------------------------------------*/

    case CM_CALL_CMD_PRIVACY_PREF:
      #ifndef FEATURE_MMODE_REMOVE_1X
      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cmd_info_ptr->call_id );

      if(call_ptr == NULL)
      {
         CM_MSG_HIGH_0( "call_ptr_NULL");
         break ;
      }

      /* Get MC command buffer and indicate the
      ** appropriate MC command.
      */
      mc_ptr = cm_mc_get_buf_else_err_fatal();
      mc_ptr->hdr.cmd = MC_PRIVACY_PREF_F;

      /* Copy cmd info
      */
      mc_ptr->privacy_mode.long_code =
                      cmxcall_map_cm_privacy_pref_to_mc_long_code( cmd_mode_info_ptr->privacy_pref );

      /* Send command to MC.
      */
      cm_mc_send_cmd( mc_ptr );
      #else
      #error code not present
#endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
    #error code not present
#endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      CM_ERR_1( "call cmd %d", (uint8) call_cmd_ptr->cmd);

      break;

  } /* switch( call_cmd_ptr->cmd) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the caller of this function to whether
  ** the call object is now waiting for reply from MC.
  */
  if ( call_ptr != NULL)
  {
    return cm_reply_is_waiting( &call_ptr->reply );
  }
  else
  {
    return FALSE;
  }
}
/*lint +esym(818, cmd_ptr) */

/*===========================================================================

FUNCTION cmxcall_client_cmd_mc_reply

DESCRIPTION
  Process MC reply to clients call commands.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmxcall_client_cmd_mc_reply(

    const cm_call_cmd_s_type       *call_cmd_ptr,
        /* Pointer to a CM command */

    cm_call_cmd_err_e_type    cmd_err
        /* Indicate call command error */
)
{
  const cm_call_cmd_info_s_type      *cmd_info_ptr = NULL;

  const cm_call_cmd_info_cdma_s_type *cmd_mode_info_ptr = NULL;

  cmcall_s_type                      *call_ptr = NULL;
      /* Pointer to a call object */

  cdma_cmcall_type                   *call_mode_info_ptr = NULL;
      /* Pointer to call mode info */

  cm_call_id_type                     call_id;
      /* call id */

  int i;
      /* loop index */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN( cmd_err, CM_CALL_CMD_ERR_NONE, CM_CALL_CMD_ERR_MAX));
  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr      = CALL_CMD_INFO_PTR(call_cmd_ptr);
  cmd_mode_info_ptr = CALL_CMD_MODE_INFO_PTR(call_cmd_ptr);
  CM_MSG_LOW_2("START cmxcall_client_cmd_mc_reply, cmd=%d, err=%d",
                  call_cmd_ptr->cmd, cmd_err);

  /* Deallocate call id/object if there is a error
  */
  if( cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    call_ptr = cmcall_ptr( call_cmd_ptr->info.call_id );

    cm_call_id_deallocate( call_cmd_ptr->info.call_id );

  }

  /*
  ** 1. Update call fields
  ** 2. Notify clients of call event
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If necessary, copy specific command fields into call struct,
  ** update call state, and notify clients of event.
  */
  switch( call_cmd_ptr->cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*-------------------------*/
                     /* Answer an incoming call */
                     /*-------------------------*/

    case CM_CALL_CMD_ANSWER:

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cmd_info_ptr->call_id );

      if( call_ptr == NULL )
      {
        CM_ERR_1( "Invalid Call-id: %d", cmd_info_ptr->call_id);
        break;
      }

      /* Mark this call as answered
      */
      call_ptr->call_subst.incom = CM_CALL_INCOM_SUBST_ANSWERED;

      /* Update call fields by copying relevant
      ** command fields into call struct.
      */
      cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );
      cmcall_event( call_ptr, CM_CALL_EVENT_ANSWER );

      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*---------------------------*/
                     /* End call/call-origination */
                     /*---------------------------*/

    case CM_CALL_CMD_END:

      for ( i = 0; i < cmd_info_ptr->num_end_call_ids; i++ )
      {
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* cmxcall should end calls of info_type CDMA only
        */
        if (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
            != CM_CALL_MODE_INFO_CDMA)
        {
          continue;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Get the pointer to call object */
        call_ptr = cmcall_ptr( cmd_info_ptr->end_params[i].call_id );

        if(call_ptr == NULL)
        {
            CM_MSG_HIGH_0( "call_ptr_NULL");
            continue;
        }

        /* If there is an commit in progress */

        #ifdef FEATURE_OTASP_HOLD_CMD_IN_COMMIT
        if ( ! (CMCALL_IS_CONV_COMMIT(call_ptr) ||
                CMCALL_IS_CONV_PC_OTAPA_COMMIT(call_ptr))
           )
        #endif /* FEATURE_OTASP_HOLD_CMD_IN_COMMIT */
        {
          cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );
          call_ptr->call_client_ptr = call_cmd_ptr->client_ptr;

          /* If not waiting for call end cnf from HDR/1XCP protocol.
          */
          if ( call_ptr->call_state != CM_CALL_STATE_IDLE ||
               call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
             )
          {
            cmcall_end( call_ptr );
          }
        }
      } /* for ( i = 0; i < cmd_info_ptr->num_end_call_ids; i++ ) */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*----------------------------*/
                     /* Send Flash/Flash with info */
                     /*----------------------------*/

    case CM_CALL_CMD_SUPS:

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cmd_info_ptr->call_id );

      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        break;
      }
      /* Update call fields by copying relevant
      ** command fields into call struct.
      */
      cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );

      if ((cmd_mode_info_ptr->flash_type == CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD) ||
          (cmd_mode_info_ptr->flash_type == CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD))
      {
        cmcall_event( call_ptr, CM_CALL_EVENT_L2ACK_CALL_HOLD);
      }
      else
      {
        cmcall_event( call_ptr, CM_CALL_EVENT_SUPS );
      }

      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*----------------------------*/
                     /*  Update privacy preference */
                     /*----------------------------*/

    case CM_CALL_CMD_PRIVACY_PREF:

      /* Get a pointer to a CDMA call object */
      call_id = cmcall_is_there_a_cdma_call_in_conv ();
      if (call_id == CM_CALL_ID_INVALID)
      {
        CM_MSG_HIGH_0 ("No CDMA call active for privacy pref change!");
        break;
      }

      call_ptr = cmcall_ptr( call_id );
      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );
      if (call_ptr == NULL)
      {
        CM_MSG_HIGH_1 ("Cannot find callobject for %d for privacy pref change",
          call_id);
        break;
      }

      /* Update call fields by copying relevant
      ** command fields into call struct.
      */
      call_mode_info_ptr->privacy_pref = cmd_mode_info_ptr->privacy_pref;

      /* Update the privacy preference of the phone object
      */

      {
        cmph_s_type *ph_ptr = cmph_ptr();
        CM_ASSERT( ph_ptr != NULL );
        ph_ptr->privacy_pref = cmd_mode_info_ptr->privacy_pref;

      }

      cmcall_event( call_ptr, CM_CALL_EVENT_PRIVACY_PREF );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
    #error code not present
#endif


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      CM_ERR_1( "Bad call command %d", (uint8) call_cmd_ptr->cmd);

      break;

  } /* switch */

} /* cmxcall_client_cmd_mc_reply() */




/*===========================================================================
=============================================================================
=========================== EXTERNALIZED FUNCTIONS ==========================
=============================================================================
===========================================================================*/


#ifdef FEATURE_HDR_HYBRID


/*===========================================================================

FUNCTION cmxcall_is_auto_pkt_data_so

DESCRIPTION
  Check whether the data service option is a packet data service
  option can be used by the automaitc service type


DEPENDENCIES
  none

RETURN VALUE
  TRUE if Automatic service type is allowed
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmxcall_is_auto_pkt_data_so(

    word                   srv_opt
        /* service option */
)
{
    switch( srv_opt )
    {
      /* 8k pre IS707 data service options.
      */
      case CAI_SO_ASYNC_DATA_PRE707:
      /*case CAI_SO_G3_FAX_PRE707:  */
      case CAI_SO_PPP_PKT_DATA_PRE707:

      /* 13k pre IS707 data service options.
      ** Also being user for 13k IS707 - check with data folks????
      */
      case CAI_SO_ASYNC_DATA_13K_PRE707:
      /*case CAI_SO_G3_FAX_13K_PRE707:*/
      case CAI_SO_PPP_PKT_DATA_13K_PRE707:

      /* 8k IS707 data service options.
      */
      case CAI_SO_ASYNC_DATA_IS707:
      /*case CAI_SO_G3_FAX_IS707:*/
      case CAI_SO_PPP_PKT_DATA_IS707:

      /* QC proprietary service options
      */
      case CAI_SO_PPP_PKT_DATA_13K_PROPTRY:
      case CAI_SO_ASYNC_DATA_13K_PROPTRY:
      /*case CAI_SO_G3_FAX_13K_PROPTRY:*/

      /* QC HACK for data service option (packet method).
      */
      case CAI_SO_DATA:

      /* MDR service options  */
      case CAI_SO_MDR_PKT_DATA_FRS1_RRS1:
      case CAI_SO_MDR_PKT_DATA_FRS1_RRS2:
      case CAI_SO_MDR_PKT_DATA_FRS2_RRS1:
      case CAI_SO_MDR_PKT_DATA_FRS2_RRS2:

      /* 3G packet data service option */
      case CAI_SO_PPP_PKT_DATA_3G:
        return TRUE;

      default:

        return FALSE;
    }


}
#endif /* FEATURE_HDR_HYBRID */

/*===========================================================================

FUNCTION cmcall_set_fade_code

DESCRIPTION
  Set the call end status based on the failure reason

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_set_fade_code(

   cmcall_s_type             *call_ptr,
        /* Point at call object */

   cm_call_end_fade_e_type        fade_code
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   switch(fade_code)
   {
     case CM_FADE_SRV_INIT_FAIL:
       call_ptr->end_status = CM_CALL_END_FADE_SRV_INIT_FAIL;
       break;

     case CM_FADE_T50_EXP:
       call_ptr->end_status = CM_CALL_END_FADE_T50_EXP;
       break;

     case CM_FADE_T51_EXP:
       call_ptr->end_status = CM_CALL_END_FADE_T51_EXP;
       break;

     case CM_FADE_RL_ACK_TIMEOUT:
       call_ptr->end_status = CM_CALL_END_FADE_RL_ACK_TIMEOUT;
       break;

   case CM_FADE_BAD_FL:
       call_ptr->end_status = CM_CALL_END_FADE_BAD_FL;
       break;
   case CM_FADE_GENERIC:
     call_ptr->end_status = CM_CALL_END_FADE;
     break;
     default:
       CM_ERR_1( "Invalid fade Code fade_code=%d",
                       (uint8) fade_code);
       break;
   }
    call_ptr->prot_end_status=call_ptr->end_status ;
    CM_MSG_HIGH_2("Call fade code sen: fade_code %d, call_ptr->endstatus %d",
                  fade_code, call_ptr->end_status);

}
/*===========================================================================

FUNCTION cmcall_set_mt_call_rel_code

DESCRIPTION
  Set the call end status based on the BS release reason

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_set_mt_call_rel_code(

   cm_mm_call_info_s_type     *call_info_ptr,
        /* Point at call object */

   cm_rel_reason_e_type       rel_code
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_info_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


   switch(rel_code)
   {
     case CM_CALL_REL_NORMAL:
       call_info_ptr->end_status = CM_CALL_END_REL_NORMAL;
       break;

     case CM_CALL_REL_SO_REJECT:
       call_info_ptr->end_status = CM_CALL_END_REL_SO_REJ;
       break;

     case CM_CALL_REL_OTASP_SPC_ERR:
       call_info_ptr->end_status = CM_CALL_END_OTASP_SPC_ERR;
       break;

     default:
       CM_ERR_1( "Invalid Release Code rel_code=%d",
                       (uint8) rel_code);
       break;
   }
   CM_MSG_HIGH_2( "Call Release Code Sent. rel_code=%d. call_info_ptr->endstatus = %d", rel_code, call_info_ptr->end_status );
}

/*===========================================================================

FUNCTION cmcall_set_rel_code

DESCRIPTION
  Set the call end status based on the BS release reason

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_set_rel_code(

   cmcall_s_type             *call_ptr,
        /* Point at call object */

   cm_rel_reason_e_type       rel_code
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_MSG_LOW_1("Call Release Code Sent; rel_code=%d",rel_code);

   switch(rel_code)
   {
     case CM_CALL_REL_NORMAL:
       call_ptr->end_status = CM_CALL_END_REL_NORMAL;
       break;

     case CM_CALL_REL_SO_REJECT:
       call_ptr->end_status = CM_CALL_END_REL_SO_REJ;
       break;

     case CM_CALL_REL_OTASP_SPC_ERR:
       call_ptr->end_status = CM_CALL_END_OTASP_SPC_ERR;
       break;

     default:
       CM_ERR_1( "Invalid Release Code rel_code=%d",
                       (uint8) rel_code);
       break;
   }
}

/*===========================================================================

FUNCTION cmcall_auto_answer

DESCRIPTION
  Automatically answers the incoming call by sending MC_ANSWER_F
  command to MC

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_auto_answer
(
    cmcall_s_type    *call_ptr
        /* pointer to a call object */
)
{
  #ifndef FEATURE_MMODE_REMOVE_1X
  cdma_cmcall_type        *call_mode_info_ptr = NULL;
      /* Pointer to call mode info */

  mc_msg_type             *mc_ptr;
      /* MC command pointer */

  db_items_value_type      db_item;
      /* Item to do db_get() and db_put() */

  #ifdef FEATURE_MMGSDI_CDMA
  cmss_s_type             *ss_ptr = cmss_ptr ();
  #endif

  #ifdef FEATURE_GPSONE
  cmpd_session_s_type     *pd_ptr      = cmpd_ptr(CMPD_SESSION_OBJ_MAIN);
  cmpd_session_s_type     *pd_dmod_ptr = cmpd_ptr(CMPD_SESSION_OBJ_DMOD);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( pd_ptr      != NULL );
  CM_ASSERT( pd_dmod_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #endif

  CM_ASSERT(call_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

  /* Get MC command buffer and indicate the
  ** appropriate MC command.
  */
  mc_ptr = cm_mc_get_buf_else_err_fatal();
  mc_ptr->hdr.cmd = MC_ANSWER_F;
  mc_ptr->answer.call_id = cmcall_call_id_get( call_ptr );
  mc_ptr->answer.ds_answered = FALSE;
  mc_ptr->answer.a_data      = FALSE;

  /* Send command to MC.
  */
  cm_mc_send_cmd( mc_ptr );

  /*  update the data-base */
  db_item.conv = TRUE;
  db_put( DB_CONV, &db_item );

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  /* Generate an ANSWER event to the clients when the
  ** the call is not answered by the clients. This would
  ** ensure all the clients will receive an ANSWER event
  ** following an INCOMING CALL event.
  */

  if ( call_ptr->call_state == CM_CALL_STATE_INCOM)
  {
    if (call_mode_info_ptr->alert_ans_bypass )
    {
      call_ptr->call_client_ptr = CM_CLIENT_PTR_NONE;
      cmcall_event ( call_ptr, CM_CALL_EVENT_ANSWER );
    }
  }
  #endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  call_ptr->call_state = CM_CALL_STATE_CONV;
  if (call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_OTAPA)
  {
    call_ptr->call_subst.conv = CM_CALL_CONV_SUBST_PC_OTAPA_REGULAR;
  }
  else
  {
    call_ptr->call_subst.conv = CM_CALL_CONV_SUBST_REGULAR;
  }

  if (cmss_ptr()->info.sys_mode != SYS_SYS_MODE_NO_SRV)
  {
    call_ptr->sys_mode = cmss_ptr()->info.sys_mode;
  }

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

  if(call_mode_info_ptr->ndss_in_progress)
  {
    call_mode_info_ptr->ndss_in_progress = FALSE;
    call_ptr->call_connect_time = time_get_uptime_secs();
    cmcall_event( call_ptr, CM_CALL_EVENT_NDSS_CONNECT );
  }
  else
  {
    #ifdef FEATURE_MMGSDI_CDMA
    cmcall_update_gsdi_and_log_call_state(call_ptr->ss);

    #endif /* FEATURE_MMGSDI_CDMA */
    call_ptr->call_connect_time = time_get_uptime_secs();
    cmcall_event( call_ptr, CM_CALL_EVENT_CONNECT );
  }

  #else

  #ifdef FEATURE_MMGSDI_CDMA

  cmcall_update_gsdi_and_log_call_state(call_ptr->ss);

  #endif /* FEATURE_MMGSDI_CDMA */

  call_ptr->call_connect_time = time_get_uptime_secs();
  cmcall_event( call_ptr, CM_CALL_EVENT_CONNECT );

  #endif

  #ifdef FEATURE_GPSONE
  /* Check if we need to force the mode for the PD session
  */
  if( !pd_ptr->is_mode_forced                                  &&
       ( pd_ptr->session_state == CMPD_SESSION_STATE_INITIATED ||
         pd_ptr->session_state == CMPD_SESSION_STATE_GRANTED
       ))
  {
    cmpd_session_grant_proc( pd_ptr->session_type );
  }

  if( !pd_dmod_ptr->is_mode_forced                                 &&
       ( pd_dmod_ptr->session_state == CMPD_SESSION_STATE_INITIATED ||
         pd_dmod_ptr->session_state == CMPD_SESSION_STATE_GRANTED
       ))
  {
    cmpd_session_grant_proc( pd_dmod_ptr->session_type );
  }
  #endif

  #else
  #error code not present
#endif
}//cmcall_auto_answer

/*===========================================================================

FUNCTION cmxcall_out_of_order_msg_proc

DESCRIPTION

   Process out-of-sequence messages that arrived before
   CM_INCOMING_CALL_F/CM_ALERT_F message.

   When this function gets called:
   - cm_mc_rpt_ptr has CM_INCOMING_CALL_F/CM_ALERT_F has data in it with
     call id that we need for out-of-sequence messages
   - saved_data has all of the out-of-sequence messages

   So for each out-of-sequence message
   - allocate CM command buffer
   - copy the message
   - add call_id to it
   - send the message to CM

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmxcall_out_of_order_msg_proc
(
  cm_call_id_type                  call_id,
  cmxcall_out_of_order_msg_s_type *saved_data
)
{
  cm_mc_rpt_type* cmd_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(saved_data           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Flag to indicate that CM_ABRV_ALERT_F was saved */
  if(saved_data->is_abrv_flag) {

    /* Get a buffer */

    cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

    /* Copy CM_ABRV_ALERT_F message */
    *(&(cmd_ptr->abrv)) = *(&(saved_data->abrv_buffer));

    /* Copy call id */
    cmd_ptr->abrv.call_id = call_id;

    /* Reset fields */
    saved_data->is_abrv_flag = FALSE;

    /* Now queue the message to CM the way MC originally did it */
    CM_MSG_LOW_1("Send CM_ABRV_ALERT_F call_id=%d",call_id);
    cm_mc_rpt( cmd_ptr );
  }

  /* Flag to indicate that CM_DISPLAY_F was saved */
  if(saved_data->is_display_flag) {

    /* Get a buffer */
    cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

    /* Copy CM_DISPLAY_F message */
    *(&(cmd_ptr->display)) = *(&(saved_data->display_buffer));

    /* Copy call id */
    cmd_ptr->display.call_id = call_id;

    /* Reset fields */
    saved_data->is_display_flag = FALSE;

    /* Now queue the message to CM the way MC originally did it */
    CM_MSG_LOW_1("Send CM_DISPLAY_F call_id=%d",call_id);
    cm_mc_rpt(cmd_ptr);
  }

  /* Flag to indicate that CM_EXT_DISPLAY_F was saved */
  if(saved_data->is_ext_disp_flag) {

    /* Get a buffer */
    cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

    /* Copy CM_EXT_DISPLAY_F message */
    *(&(cmd_ptr->ext_disp)) = *(&(saved_data->ext_disp_buffer));

    /* Copy call id */
    cmd_ptr->ext_disp.call_id = call_id;

    /* Reset fields */
    saved_data->is_ext_disp_flag = FALSE;

    /* Now queue the message to CM the way MC originally did it */
    CM_MSG_LOW_1("Send CM_EXT_DISPLAY_F call_id=%d",call_id);
    cm_mc_rpt(cmd_ptr);
  }

  /* Flag to indicate that CM_SIGNAL_F was saved */
  if(saved_data->is_signal_flag) {

    /* Get a buffer */
    cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

    /* Copy CM_SIGNAL_F message */
    *(&(cmd_ptr->signal)) = *(&(saved_data->signal_buffer));

    /* Copy call id */
    cmd_ptr->signal.call_id = call_id;

    /* Reset fields */
    saved_data->is_signal_flag = FALSE;

    /* Now queue the message to CM the way MC originally did it */
    CM_MSG_LOW_1("Send CM_SIGNAL_F call_id=%d",call_id);
    cm_mc_rpt(cmd_ptr);
  }

  /* Flag to indicate that CM_CALLING_PARTY_F was saved */
  if(saved_data->is_calling_party_flag) {

    /* Get a buffer */
    cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

    /* Copy CM_CALLING_PARTY_F message */
    *(&(cmd_ptr->calling_party)) = *(&(saved_data->calling_party_buffer));

    /* Copy call id */
    cmd_ptr->calling_party.call_id = call_id;

    /* Reset fields */
    saved_data->is_calling_party_flag = FALSE;

    /* Now queue the message to CM the way MC originally did it */
    CM_MSG_LOW_1("Send CM_CALLING_PARTY_F call_id=%d",call_id);
    cm_mc_rpt(cmd_ptr);
  }

  /* Flag to indicate that CM_CALLED_PARTY_F was saved */
  if(saved_data->is_called_party_flag) {

    /* Get a buffer */
    cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

    /* Copy CM_CALLED_PARTY_F message */
    *(&(cmd_ptr->called_party)) = *(&(saved_data->called_party_buffer));

    /* Copy call id */
    cmd_ptr->called_party.call_id = call_id;

    /* Reset fields */
    saved_data->is_called_party_flag = FALSE;

    /* Now queue the message to CM the way MC originally did it */
    CM_MSG_LOW_1("Send CM_CALLED_PARTY_F call_id=%d",call_id);
    cm_mc_rpt(cmd_ptr);
  }

    /* Flag to indicate that CM_REDIRECTING_NUM_F was saved */
  if(saved_data->is_redirecting_number_flag) {

    /* Get a buffer */
    cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

    /* Copy CM_REDIRECTING_NUM_F message */
    *(&(cmd_ptr->redirecting_number)) = *(&(saved_data->redirecting_number_buffer));

    /* Copy call id */
    cmd_ptr->redirecting_number.call_id = call_id;

    /* Reset fields */
    saved_data->is_redirecting_number_flag = FALSE;

    /* Now queue the message to CM the way MC originally did it */
    CM_MSG_LOW_1("Send CM_REDIRECTING_NUM_F call_id=%d",call_id);
    cm_mc_rpt(cmd_ptr);
  }

  /* Flag to indicate that CM_CONNECTED_NUM_F was saved */
  if(saved_data->is_connected_num_flag) {

    /* Get a buffer */
    cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

    /* Copy CM_CONNECTED_NUM_F message */
    *(&(cmd_ptr->connected_num)) = *(&(saved_data->connected_num_buffer));

    /* Copy call id */
    cmd_ptr->connected_num.call_id = call_id;

    /* Reset fields */
    saved_data->is_connected_num_flag = FALSE;

    /* Now queue the message to CM the way MC originally did it */
    CM_MSG_LOW_1("Send CM_CONNECTED_NUM_F call_id=%d",call_id);
    cm_mc_rpt(cmd_ptr);
  }
}


/*===========================================================================

FUNCTION cmxcall_convert_to_emergency

DESCRIPTION

  For call with given call id change the type of that call to emergency.
  This happens when emergency call was originated but MC converted that
  origination into a flash and then converted an existing voice call into
  emergency call.

  If successful, call object with given call id in CM has its call_type
  set to CM_CALL_TYPE_EMERGENCY and CM_CALL_EVENT_EMERGENCY_FLASHED is sent
  to all clients who registered for it.

DEPENDENCIES
  None

RETURN VALUE
  Return true if successful, false otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean cmxcall_convert_to_emergency
(
   const cm_mc_rpt_type *cm_mc_rpt_ptr
)
{
  cmcall_s_type *original_call_id_call_ptr;
  cmcall_s_type *emergency_flash_call_id_call_ptr;

  cm_call_id_type original_call_id;        /* end this one */
  cm_call_id_type emergency_flash_call_id; /* make this one emergency */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cm_mc_rpt_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  original_call_id        = cm_mc_rpt_ptr->call_emergency_flashed.original_call_id;
  emergency_flash_call_id = cm_mc_rpt_ptr->call_emergency_flashed.emergency_flash_call_id;

  original_call_id_call_ptr        = cmcall_ptr(original_call_id);
  if ( original_call_id_call_ptr == NULL )
  {
    CM_ERR_0( "original_call_id_ptr is empty. INVALID Call-ID");
    return FALSE;
  }

  emergency_flash_call_id_call_ptr = cmcall_ptr(emergency_flash_call_id);
  if( emergency_flash_call_id_call_ptr == NULL )
  {
    CM_ERR_1( "Invalid Call-id: %d", emergency_flash_call_id);
    return FALSE;
  }

  /* Now we're happy campers and we just need to copy stuff from one
  ** call object to another
  */
  emergency_flash_call_id_call_ptr->call_type = CM_CALL_TYPE_EMERGENCY;
  emergency_flash_call_id_call_ptr->srv_type  = CM_SRV_TYPE_AUTOMATIC;

  /* Copy phone number that emergency_flash_call_id_call_ptr is now
  ** connected to
  */
  *(&(emergency_flash_call_id_call_ptr->num)) = *(&(original_call_id_call_ptr->num));

  return TRUE;
}


/*===========================================================================

FUNCTION cmxcall_timer_proc

DESCRIPTION
  Process timer events

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_timer_proc
(
  cmcall_s_type           *call_ptr
      /* Point at call object */
)
{
  cdma_cmcall_type *call_mode_info_ptr = NULL;
      /* Pointer to call mode info */

  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
  dword                uptime = time_get_uptime_secs();
  #endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );
  /* If call needs to be ended, do so and return
  */
  if ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA  &&
        call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_HDR &&
        call_ptr->call_state == CM_CALL_STATE_IDLE &&
        call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT &&
        (call_ptr->end_req_wait_uptime <= time_get_uptime_secs()))
  {
     cmcall_end(call_ptr);
     return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
  if ( uptime *1000 >  cm_get_cache_ind().cache_expiry_timer)
  {
    // If timer expires and we're in wait state. then clear all state,set cache validity
    if (cm_get_cache_ind().state == CMWCALL_CACHE_EXPECTED)
    {
      CM_MSG_HIGH_0("send SRVCC TIMER FAIL rpt to 1xCP");
      cm_process_cmd_on_ims_cache(CMWCALL_CACHE_EXPIRED);
    }
  }
  #endif


  /* Do NON call state specific timer processing.
  */

  /* If we are waiting too long on a reply from MC, end the
  ** waiting period by forcing a MC reply with an error
  ** code indicating on a MC communication problems.
  */
  if( cm_reply_too_long(&call_ptr->reply) )
  {
    /* Point at client command waiting for reply
    */
    cm_cmd_type  *cmd_ptr = cm_reply_get( &call_ptr->reply );
    CM_ASSERT( cmd_ptr != NULL );

    /* Invoke the reply function with the appropriate error code.
    */
    cmcall_client_cmd_err( CALL_CMD_PTR(cmd_ptr), CM_CALL_CMD_ERR_MC_COM_L );

    /* We are done with processing the command that was waiting
    ** for reply - deallocate its command buffer.
    */
    cm_cmd_dealloc( cmd_ptr );

    /* Turn on the signal that cause us to process any pending
    ** clients commands. This is necessary since we were NOT
    ** processing any clients commands while waiting on a reply
    ** from MC.
    */
    (void) rex_set_sigs( rex_self(), CM_CLIENT_CMD_Q_SIG );
  } /* if( cm_reply_too_long(&call_ptr->reply) ) */

  #ifdef FEATURE_OTASP
  if ( (CMCALL_IS_CONV_COMMIT(call_ptr) ||
        CMCALL_IS_CONV_PC_OTAPA_COMMIT(call_ptr)) &&
       call_mode_info_ptr->otasp_info.is_wait_for_commit_reply
     )
  {
    if( cm_commit_reply_too_long(&call_mode_info_ptr->otasp_info.commit_reply) )
    {
      CM_MSG_HIGH_0("commit timer expires, state transition");

      if (CMCALL_IS_CONV_COMMIT(call_ptr))
      {
        call_ptr->call_subst.conv = CM_CALL_CONV_SUBST_REGULAR;
      }
      else if (CMCALL_IS_CONV_PC_OTAPA_COMMIT(call_ptr))
      {
        call_ptr->call_subst.conv = CM_CALL_CONV_SUBST_PC_OTAPA_REGULAR;
      }

      #ifdef FEATURE_OTASP_HOLD_CMD_IN_COMMIT

      /* Process the outstanding commands in the hold command q
      */
      CM_MSG_HIGH_0("Start processing hold cmd");
      cm_hold_cmd_handle( TRUE, CM_HOLD_CMD_REAS_NONE);

      #endif /* FEATURE_OTASP_HOLD_CMD_IN_COMMIT */
    } /* if( cm_commit_reply_too_long(&call_mode_info_ptr->otasp_info.commit_reply) ) */
  } /* if ( (CMCALL_IS_CONV_COMMIT(call_ptr) || */
  #endif /* FEATURE_OTASP */
} /* cmxcall_timer_proc() */



/**--------------------------------------------------------------------------
** Functions - extern
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmxcall_reset

DESCRIPTION
  Reset CDMA-specific call fields to default values reflecting
  a CDMA call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_reset(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  cdma_cmcall_type *call_mode_info_ptr = NULL;
    /* Pointer to call mode info */

  cmph_s_type *ph_ptr = cmph_ptr();

  /* Init value of charge rate information */
  const cm_ext_brst_intl_s_type ext_brst_intl_msg_init_val =
  {
    0x153,  /* Mobile Country Code: fixed '0101010011' */
    1,      /* Data Burst subtype: fixed '000001' */
    0,      /* Charge Indication: "No indication" */
    0,      /* Unit call time (1/10 second) */
    0       /* Unit call time (second) */
  };

  /* Initial values of National Supplementary services - CLIR */
  const cm_nss_clir_rec_s_type nss_clir_rec_init_val =
  {
    0       /* Reason code: "No cause" */
  };

  /* Initial value of National Supplementary services - Audio Control */
  const cm_nss_aud_ctrl_s_type nss_aud_ctrl_init_val =
  {
    0,      /* Audio path from the MS to the BS: "Do not change" */
    0       /* Audio path from the BS to the MS: "Do not change" */
  };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( ph_ptr   != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

  call_mode_info_ptr->srv_opt                  = CM_SRV_OPT_NONE;
  call_mode_info_ptr->is_privacy               = ph_ptr->is_privacy;

  /* Reset otasp_info fields
  */
  call_mode_info_ptr->otasp_info.otasp_call    = CMCALL_OTASP_NONE;
  call_mode_info_ptr->otasp_info.otasp_act_code
                                               = CM_OTASP_ACT_CODE_NONE;
  call_mode_info_ptr->otasp_info.is_commit_successful     = FALSE;
  call_mode_info_ptr->otasp_info.is_wait_for_commit_reply = FALSE;
  call_mode_info_ptr->otasp_info.otasp_status = CM_OTASP_STATUS_NONE;
  call_mode_info_ptr->is_last_cdma_info_rec    = TRUE;

  #ifdef FEATURE_IS2000_REL_A
  call_mode_info_ptr->qos_parms_incl           = FALSE;
  call_mode_info_ptr->qos_parms_len            = 0;
  #endif  /* FEATURE_IS2000_REL_A */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  call_mode_info_ptr->ext_disp_type            = CM_EXT_DISP_TYPE_NONE;
  #endif  /*#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  call_mode_info_ptr->alert_ans_bypass         = FALSE;
  #endif  /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  call_mode_info_ptr->ndss_in_progress         = FALSE;
  call_mode_info_ptr->ndss_connect             = FALSE;
  #endif  /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  call_mode_info_ptr->ext_brst_intl_msg        = ext_brst_intl_msg_init_val;
  call_mode_info_ptr->nss_clir_rec             = nss_clir_rec_init_val;
  call_mode_info_ptr->nss_aud_ctrl_rec         = nss_aud_ctrl_init_val;

  #ifdef FEATURE_IS2000
  call_mode_info_ptr->retry_order.retry_type   = CM_RETRY_TYPE_NONE;
  #endif /* FEATURE_IS2000 */

  #ifdef FEATURE_HDR_AN_AUTH
  call_ptr->an_auth_status                     = FALSE;
  #endif /* FEATURE_HDR_AN_AUTH */

  #ifdef FEATURE_HDR_HYBRID
  call_ptr->is_hybr_allowed                    = FALSE;
  call_ptr->ignore_hdr_call_conn_rpt           = FALSE;
  call_ptr->waiting_for_hdr_call_end           = FALSE;
  #endif /* FEATURE_HDR_HYBRID */

  #ifdef FEATURE_OTASP
  /* Initialize command commit reply object
  */
  cm_reply_init( &call_mode_info_ptr->otasp_info.commit_reply );
  #endif /* FEATURE_OTASP */

  #ifdef FEATURE_HDR_AN_AUTH
  /* Initialize AN authentication status
  */
  call_ptr->an_auth_status                     = FALSE;
  #endif /* FEATURE_HDR */

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
  #error code not present
#endif

  cmcall_set_cdma_cc_result(call_ptr,CM_CC_RESULT_NOT_ALLOWED);

  call_ptr->cmcall_mode_info.info.cdma_info.line_ctrl_polarity_included
                                               =  FALSE;
  call_ptr->cmcall_mode_info.info.cdma_info.line_ctrl_power_denial
                                               =  0;
  call_ptr->cmcall_mode_info.info.cdma_info.line_ctrl_reverse
                                               =  FALSE;
  call_ptr->cmcall_mode_info.info.cdma_info.line_ctrl_toggle
                                               =  FALSE;
  call_ptr->cmcall_mode_info.info.cdma_info.orig_fail_layer
                                               = CM_CALL_ORIG_ERR_LAYER_NONE;

  call_ptr->pd_status                          = CMCALL_PD_STATUS_NONE;

  call_ptr->sms_status                         = CMCALL_SMS_STATUS_NONE;


  #if (defined (FEATURE_UMTS_1X_HANDOVER_1XMSM))
  #error code not present
#endif

} /* cmxcall_reset() */


/*===========================================================================

FUNCTION cmxcall_info_get

DESCRIPTION
  Copy the current call state information into a specified buffer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmxcall_info_get(

    const cmcall_s_type          *call_ptr,
        /* Point at call object */

    cm_mm_call_info_s_type       *call_info_ptr
        /* Copy call state info into this buffer */
)
{
  const cdma_cmcall_type *call_mode_info_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_info_ptr != NULL );
  CM_ASSERT( call_ptr != NULL );

  /* Verify that object was properly initialized.
  */
  CM_ASSERT( CM_INIT_CHECK(call_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

  /* Verify the integrity of the call object.
  */
  cmxcall_assert( call_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy call parameters into call_info struct.
  */
  call_info_ptr->call_id                = cmcall_call_id_get( call_ptr );
  call_info_ptr->call_state             = call_ptr->call_state;
  call_info_ptr->call_client_id         = cmclient_get_client_id( call_ptr->call_client_ptr );
  call_info_ptr->call_type              = call_ptr->call_type;
  call_info_ptr->mode_info.info_type    = call_ptr->cmcall_mode_info.info_type;
  call_info_ptr->sys_mode               = call_ptr->sys_mode;
  call_info_ptr->mode_info.info.cdma_call.srv_opt = call_mode_info_ptr->srv_opt;
  call_info_ptr->num                    = call_ptr->num;
  call_info_ptr->redirecting_number     = call_ptr->redirecting_number;
  call_info_ptr->alpha                  = call_ptr->alpha;
  call_info_ptr->signal                 = call_ptr->signal;
  call_info_ptr->mode_info.info.cdma_call.privacy_pref = call_mode_info_ptr->privacy_pref;
  call_info_ptr->mode_info.info.cdma_call.is_privacy   = call_mode_info_ptr->is_privacy;
  call_info_ptr->end_status             = call_ptr->end_status;
  call_info_ptr->mode_info.info.cdma_call.is_otasp_commit =
                         call_mode_info_ptr->otasp_info.is_commit_successful;
  call_info_ptr->mode_info.info.cdma_call.otasp_status =
                         call_mode_info_ptr->otasp_info.otasp_status;
  call_info_ptr->call_connect_time      = call_ptr->call_connect_time;
  call_info_ptr->call_end_time          = call_ptr->call_end_time;
  call_info_ptr->bytes_received         = call_ptr->bytes_received;
  call_info_ptr->bytes_transmitted      = call_ptr->bytes_transmitted;
  call_info_ptr->direction              = call_ptr->direction;
  call_info_ptr->client_categ           = call_ptr->client_categ;

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  call_info_ptr->mode_info.info.cdma_call.ext_disp_type = call_mode_info_ptr->ext_disp_type;
  #endif  /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  call_info_ptr->mode_info.info.cdma_call.is_last_cdma_info_rec  = call_mode_info_ptr->is_last_cdma_info_rec;

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  call_info_ptr->mode_info.info.cdma_call.alert_ans_bypass       = call_mode_info_ptr->alert_ans_bypass;
  #endif  /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  #ifdef FEATURE_IS2000
  call_info_ptr->mode_info.info.cdma_call.end_reason             = call_mode_info_ptr->end_reason;
  call_info_ptr->mode_info.info.cdma_call.retry_order            = call_mode_info_ptr->retry_order;
  #endif /* FEATURE_IS2000 */

  call_info_ptr->mode_info.info.cdma_call.ext_brst_intl_msg      = call_mode_info_ptr->ext_brst_intl_msg;
  call_info_ptr->mode_info.info.cdma_call.nss_clir_rec           = call_mode_info_ptr->nss_clir_rec;
  call_info_ptr->mode_info.info.cdma_call.nss_aud_ctrl_rec       = call_mode_info_ptr->nss_aud_ctrl_rec;

  #ifdef FEATURE_HDR_AN_AUTH
  //   call_info_ptr->an_auth_status         = call_ptr->an_auth_status;
  #endif /* FEATURE_HDR_AN_AUTH */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  call_info_ptr->mode_info.info.cdma_call.ndss_in_progress  = call_mode_info_ptr->ndss_in_progress;
  #endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  call_info_ptr->mode_info.info.cdma_call.sups_type = call_mode_info_ptr->flash_type;

  #ifdef FEATURE_IS2000_REL_A
  call_info_ptr->mode_info.info.cdma_call.emergency_flash_call_id = call_mode_info_ptr->emergency_flash_call_id;
  call_info_ptr->mode_info.info.cdma_call.emergency_flash_over_incoming_call =
  call_mode_info_ptr->emergency_flash_call_id;

  call_info_ptr->mode_info.info.cdma_call.qos_parms_incl = call_mode_info_ptr->qos_parms_incl;
  if ( call_mode_info_ptr->qos_parms_incl )
  {
    call_info_ptr->mode_info.info.cdma_call.qos_parms_len = memscpy( call_info_ptr->mode_info.info.cdma_call.qos_parms,
                                                                     sizeof(call_info_ptr->mode_info.info.cdma_call.qos_parms),
                                                                     call_mode_info_ptr->qos_parms,
                                                                     call_mode_info_ptr->qos_parms_len );
  }
  #endif /* FEATURE_IS2000_REL_A */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy line control parameters */

  call_info_ptr->mode_info.info.cdma_call.line_ctrl_polarity_included
    = call_ptr->cmcall_mode_info.info.cdma_info.line_ctrl_polarity_included;
  call_info_ptr->mode_info.info.cdma_call.line_ctrl_toggle
    = call_ptr->cmcall_mode_info.info.cdma_info.line_ctrl_toggle;
  call_info_ptr->mode_info.info.cdma_call.line_ctrl_reverse
    = call_ptr->cmcall_mode_info.info.cdma_info.line_ctrl_reverse;
  call_info_ptr->mode_info.info.cdma_call.line_ctrl_power_denial
    = call_ptr->cmcall_mode_info.info.cdma_info.line_ctrl_power_denial;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy end parameters */
  call_info_ptr->end_params.end_params.cdma_end.end_reason_included =  call_ptr->end_params.end_params.cdma_end.end_reason_included;
  call_info_ptr->end_params.end_params.cdma_end.end_reason = call_ptr->end_params.end_params.cdma_end.end_reason;

  call_info_ptr->mode_info.info.cdma_call.umts_cdma_ho_status = call_ptr->umts_cdma_ho_status;
  call_info_ptr->mode_info.info.cdma_call.ho_status = call_ptr->ho_status;

  call_info_ptr->mode_info.info.cdma_call.orig_fail_layer = call_ptr->cmcall_mode_info.info.cdma_info.orig_fail_layer;

  call_info_ptr->mode_info.info.cdma_call.con_ref = call_ptr->cmcall_mode_info.info.cdma_info.con_ref;

  CM_MSG_MED_2("cmxcall_info_get: call orig fail layer = %d con_ref = %d",
  	call_info_ptr->mode_info.info.cdma_call.orig_fail_layer, call_info_ptr->mode_info.info.cdma_call.con_ref);
  /* Return boolean indicating whether call information is valid.
  */
  return CM_BOOLEAN( CM_INIT_CHECK(call_ptr->init_mask) );

} /* cmxcall_info_get() */



/*===========================================================================

FUNCTION cmxcall_srlte_send_tau

DESCRIPTION
  Sends TAU if an ESR was sent before the call orig
  

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmxcall_srlte_send_tau(

    cm_call_id_type  call_id
        /* callid */
)
{  
#if  defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
  if (cmph_ptr()->is_tau_needed == TRUE
      && cmph_is_oprting_in_1xsrlte_mode(SYS_MODEM_AS_ID_1))
  {
    cm_call_id_type cdma_call_id = cmcall_is_there_a_cdma_call_except(call_id);

    CM_MSG_HIGH_2("cdma_call_id %d cmreg state %d",
                   cdma_call_id,
                   cmregprx_get_stack_info_ptr(SD_SS_HYBR_2)->state);
    if (cdma_call_id == CM_CALL_ID_INVALID)
    {
      /* send TAU here , since we will not wait for any response from 1X
         */
      cmcsfbcall_send_mm_dual_rx_1xcsfb_tau_req();
      cmph_ptr()->is_tau_needed = FALSE;
    }
  }
#endif /* FEATURE_MMODE_SC_SVLTE  */
}

/*===========================================================================

FUNCTION cmxcall_end

DESCRIPTION
  Does call end necessary activities (CM side only, no call end
  cmds sent to LL).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_end(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  #ifdef FEATURE_OTASP
  cdma_cmcall_type *call_mode_info_ptr = NULL;
      /* Pointer to call mode info */
  #endif /* FEATURE_OTASP */

  cmph_s_type  *ph_ptr = cmph_ptr();
      /* point at phone object */

  cm_act_update_reas_e_type update_reason = CM_ACT_UPDATE_REAS_ACT_END;
  boolean unsuspend_hybr_stacks = FALSE;

  boolean is_1xcsfb_call = FALSE;
  sys_modem_as_id_e_type as_id = call_ptr->asubs_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( call_ptr != NULL );

  #ifdef FEATURE_OTASP
  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );
  #endif /* FEATURE_OTASP */

  CM_MSG_HIGH_5 ("CallEnd state=%d status=%d is_tau_needed %d CallType=%d SrvType=%d ",
                call_ptr->call_state,
                call_ptr->end_status,
                cmph_ptr()->is_tau_needed,
                call_ptr->call_type,
                call_ptr->srv_type );

 
  #ifdef FEATURE_HDR
  CM_MSG_HIGH_1( "ss=%d ",
                call_ptr->ss);
  #endif /* FEATURE_HDR */


  #if defined(FEATURE_HDR_RESELECT_FORCE_TO_1X)
  /* Check if PS data call is released on 1x.
  */
  {
    boolean ps_call_rel_on_1x_in_orig_state = FALSE;
    boolean ps_call_rel_on_1x_in_conv_state = FALSE;

    /*
    ** If PS data call got released on 1X in origination state,
    ** then the below is true
    */
    ps_call_rel_on_1x_in_orig_state =
    ( call_ptr->call_type           == CM_CALL_TYPE_PS_DATA &&
      call_ptr->call_state          == CM_CALL_STATE_ORIG   &&
      call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_CDMA );

    /*
    ** If PS data call got released on 1X in conversation state,
    ** then the below is true
    */
    ps_call_rel_on_1x_in_conv_state =
      ( call_ptr->call_type   == CM_CALL_TYPE_PS_DATA &&
        call_ptr->call_state  == CM_CALL_STATE_CONV   &&
        call_ptr->sys_mode    == SYS_SYS_MODE_CDMA );

    if( ps_call_rel_on_1x_in_orig_state ||
        ps_call_rel_on_1x_in_conv_state )
    {
      /* Data call torn down on 1x, force to 1x.
      */
      CM_MSG_HIGH_0("Reselect - force to 1x cmxcall_end()");
      cmmsimc_proc_disable_hybr_hdr();
    }
  }
  #else
  CM_MSG_HIGH_0("Reselect - restore from cmxcall_end()");
  cmmsimc_proc_enable_hybr_hdr(TRUE);
  #endif  /* FEATURE_HDR_RESELECT_FORCE_TO_1X */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_OTASP
  if (call_ptr->call_type == CM_CALL_TYPE_STD_OTASP ||
      call_ptr->call_type == CM_CALL_TYPE_NON_STD_OTASP ||
      call_ptr->call_type == CM_CALL_TYPE_OTAPA ||
      call_mode_info_ptr->otasp_info.otasp_call == CMCALL_BUSY_OTAPA
     )
  {
    /* Tell diag to update its cache*/
    CM_MSG_HIGH_1("diag_otasp_update, call_type: %d", call_ptr->call_type);
    diag_otasp_update();
  }
  #endif /* FEATURE_OTASP */

  if(call_ptr->call_state == CM_CALL_STATE_ORIG)
  {
    cmlog_call_orig_end(call_ptr->end_status);
    call_ptr->call_state = CM_CALL_STATE_IDLE;
    call_ptr->call_subst.idle= CM_CALL_IDLE_SUBST_NONE;
    call_ptr->end_req_wait_uptime  = CMCALL_UPTIME_NONE;
  }

  if(call_ptr->end_status == CM_CALL_END_EMERGENCY_ONLY
    || call_ptr->end_status == CM_CALL_END_THERMAL_EMERGENCY)
  {
    update_reason = CM_ACT_UPDATE_REAS_EMERG_ENTER;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the priority queue for this activity
  */

  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_MAIN,
                                 update_reason );
  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_HYBR_2,
                                 update_reason );
  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_HDR,
                                 update_reason );


  /* Tell phone that call is over.
     Number of allocated calls equal to 1 means that this is the
     last call that is being ended, so we need to notify the phone
     now. */
  if(cm_number_of_allocated_calls() == 1) {
     cmph_call_end( call_ptr );
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined (FEATURE_LTE_TO_1X)
  if(!(call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB &&
       call_ptr->direction == CM_CALL_DIRECTION_MT &&
       call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_NONE ))
  #endif
  {
    /* Change the call state to idle.
    */
    call_ptr->call_state = CM_CALL_STATE_IDLE;
    call_ptr->call_subst.idle= CM_CALL_IDLE_SUBST_NONE;
    call_ptr->end_req_wait_uptime  = CMCALL_UPTIME_NONE;

    /* Notify clients of call end event.
    */
    cmcall_update_call_end_info( call_ptr );

    /* Donot send end event, when call waiting for reorig phase2*/
    if(call_ptr->cc_object_ptr == NULL)
    {
      cmcall_event( call_ptr, CM_CALL_EVENT_END );
    }
  }

  if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    cmac_enable_emergency_if_pending();
  }

  #ifdef FEATURE_OTASP
  if (call_mode_info_ptr->otasp_info.is_commit_successful)
  {

    /* Reset the origination mode to none.
    */
    cmph_orig_mode_reset( ph_ptr, FALSE );

     /* Reset the origination throttle table
    */
    cmss_orig_thr_table_reset( cmss_ptr()->cmss_orig_thr_tbl_ptr );

    /* Clear the is hybr kicked flag
    */
    *cmph_get_is_kicked_hybr_ptr() = FALSE;
    *cmph_get_is_kicked_hybr_2_ptr() = FALSE;

    /* Set subscription change pending flag in ph object.
    */
    ph_ptr->is_otasp_commit_pending = TRUE;
  }
  #endif /* FEATURE_OTASP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_LTE_TO_1X
  if (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB)
  {
    is_1xcsfb_call = TRUE;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmcall_is_1x_data_srv_opt(call_ptr->cmcall_mode_info.info.cdma_info.srv_opt))
  {
    unsuspend_hybr_stacks = TRUE;
  }

  if(call_ptr->call_type == CM_CALL_TYPE_VOICE 
     || call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    cmxcall_srlte_send_tau(call_ptr->call_id);
  }
   /* before deallocating call id, check if this was DRVCC setup call */
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  if(cmcall_drvcc_cache_get_drvcc_state() == CM_DRVCC_STATE_SETUP_IN_PROG)
  {
    cmcall_drvcc_check_and_abort_drvcc_setup(call_ptr);
  }
  #endif /* defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) */
  cm_call_id_deallocate( cmcall_call_id_get( call_ptr ) );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_MMODE_SC_SVLTE
  /* If there are no PS so 33/32 calls active and no pending 1x incoming call
  ** end suspend status
  */
  if(unsuspend_hybr_stacks == TRUE)
  {
  cmxcall_unsuspend_hybrid_operation();
  }
  #endif //FEATURE_MMODE_SC_SVLTE

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MMGSDI_CDMA
  /* If there are no more calls, then the traffic channel must be down */
  if (cmcall_is_there_a_cdma_call_in_conv () == CM_CALL_ID_INVALID)
  {
    cmcall_update_gsdi_and_log_call_state(CM_SS_MAIN);

  }
  #endif


  if (is_1xcsfb_call == FALSE)
  {
  cmph_update_ddtm_status(ph_ptr);                   /* update DDTM status */
  }

  /* Inform call has ended, should be called after call object is
  ** deallocated.
  */
  cmmsimc_proc_call_end();


  /* When all the voice calls have ended, if there is an existing suspended data
  ** that is CM initiated, Update the suspend state
  */
  if( cmph_is_msim() &&
      (cmss_ptr()->ue_init_ps_data_suspend != CM_SS_NONE) &&
      (cmcall_is_there_a_call_type(CM_SS_MAX,CM_CALL_TYPE_VOICE,CM_CALL_ID_INVALID) == CM_CALL_ID_INVALID )&&
	    (cmcall_is_there_a_call_type(CM_SS_MAX,CM_CALL_TYPE_SMS,CM_CALL_ID_INVALID) == CM_CALL_ID_INVALID )&&
      (cmcall_is_there_a_call_type(CM_SS_MAX,CM_CALL_TYPE_EMERGENCY,CM_CALL_ID_INVALID) == CM_CALL_ID_INVALID )&&
      (wms_msg_mo_sms_busy(as_id) == FALSE)
    )
  {

    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    if( cmss_ptr()->ue_init_ps_data_suspend == CM_SS_HYBR_3)
    {
      cmss_process_hybr_3_data_suspend_rpt(FALSE);
    }
    else
    #endif
    if( cmss_ptr()->ue_init_ps_data_suspend == CM_SS_MAIN )
    {
      cmss_process_data_suspend_rpt(FALSE);
    }
    else
    {
      cmss_process_hybr_2_data_suspend_rpt(FALSE);
    }
    
    cmss_ptr()->ue_init_ps_data_suspend = CM_SS_NONE;
    cmss_ptr()->ue_init_ps_data_suspend_cause = CM_SMREG_PS_SIG_REL_REQ_CAUSE_MAX;
  }

} /* cmxcall_end() */


/*===========================================================================

FUNCTION CMXCALL_CLIENT_ORIG_PROC

DESCRIPTION
  Process clients' origination call command.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type cmxcall_client_orig_proc
(
  cmcall_s_type  *call_ptr                       /* pointer to a call Object  */
)
{


  cm_call_cmd_err_e_type         cmd_err         = CM_CALL_CMD_ERR_NOERR;



  cmph_s_type                   *ph_ptr          = cmph_ptr();
      /* Point at phone object */


  cm_call_id_type                end_call_id     = CM_CALL_ID_UNASSIGNED;


  cmss_s_type                   *ss_ptr          = cmss_ptr();
      /* Point at serving system object */

  boolean                        is_call_allowed = FALSE;
      /* Computes if new ps or cs_data can be allowed in phone */

  cmcall_s_type                 *end_call_ptr    = NULL;

  cm_iterator_type				end_call_itr;

  sys_modem_as_id_e_type   subId = cmph_map_cm_ss_to_subs(CM_SS_MAIN) ;

  uint8                    stk_id = ph_ptr->ss_subs_map[CM_SS_MAIN].stack_id;

  #ifdef FEATURE_OTASP
  cdma_cmcall_type              *call_mode_info_ptr = NULL;
      /* Pointer to call mode info */
  #endif /* FEATURE_OTASP */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( ph_ptr   != NULL );
  CM_ASSERT( ss_ptr   != NULL );

  SYS_ARG_NOT_USED(ph_ptr);
  SYS_ARG_NOT_USED(ss_ptr);


  if (!cmph_is_as_id_valid(subId))
  {  
    CM_MSG_HIGH_1("Subs Id is not Valid. Not checking further error subId %d", subId);
    return CM_CALL_CMD_ERR_OP_NOT_SUPPORTED_ON_SUBS;
  }
  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if there is a PD session active and this origination is for a
  ** voice call
  ** If it is then set the favoured mode to CDMA_ONLY
  */

  #ifdef FEATURE_GPSONE
  {
    cmpd_session_s_type *pd_ptr      = cmpd_ptr( CMPD_SESSION_OBJ_MAIN);
    cmpd_session_s_type *pd_dmod_ptr = cmpd_ptr( CMPD_SESSION_OBJ_DMOD);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_ASSERT( pd_ptr      != NULL );
    CM_ASSERT( pd_dmod_ptr != NULL );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if( call_ptr->call_type == CM_CALL_TYPE_VOICE
        &&
        !(cmcall_misc_is_mode_pref( call_ptr->favored_mode_pref,
                                    CM_MODE_PREF_CDMA_ONLY ))
        &&
        ( pd_ptr->session_state      != CMPD_SESSION_STATE_NONE  ||
          pd_dmod_ptr->session_state != CMPD_SESSION_STATE_NONE    ))
    {
      call_ptr->favored_mode_pref = CM_MODE_PREF_CDMA_ONLY;
    }
  }
  #endif

  /*
  **  Block data call origination if necessary .
  */
  if ((call_ptr->call_type == CM_CALL_TYPE_PS_DATA) ||
      (call_ptr->call_type == CM_CALL_TYPE_CS_DATA))
  {

    cm_call_id_type voice_call_id;
    cm_call_id_type emergency_call_id;
    #ifdef FEATURE_JCDMA
    cm_call_id_type pd_call_id;
    #endif
    boolean         svd_supported;
    cm_call_id_type otapa_call_id;
    cm_call_id_type otasp_call_id;
    cmcall_s_type   *call_ptr_temp = NULL;


    voice_call_id     = cmxcall_find_call(CM_CALL_TYPE_VOICE,
                                          call_ptr->call_id);

    emergency_call_id = cmxcall_find_call(CM_CALL_TYPE_EMERGENCY,
                                          call_ptr->call_id);

    #ifdef FEATURE_JCDMA
    pd_call_id = cmxcall_find_call(CM_CALL_TYPE_PD,
                                   call_ptr->call_id);
    #endif

    otapa_call_id = cmxcall_find_call(CM_CALL_TYPE_OTAPA,
                                      call_ptr->call_id);

    otasp_call_id = cmxcall_find_call(CM_CALL_TYPE_STD_OTASP,
                                      call_ptr->call_id);

    svd_supported     = cmss_is_cdma_svd_supported();


    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /*
    ** Reject the data call while in a otapa or otasp call unless the otapa
    ** is on SVDO compliant service option.
    */
    CM_MSG_HIGH_2("otapa_call_id=%d, otasp_call_id=%d",
                  otapa_call_id,otasp_call_id);

    call_ptr_temp = cmcall_ptr(otapa_call_id);
    if( call_ptr_temp != NULL )
    {
      if ( otapa_call_id != CM_CALL_ID_INVALID
          && !cmxcall_is_svdo_capable(call_ptr_temp)
          )
      {
        CM_MSG_HIGH_0("Cannot originate a DATA call while in a otapa call");
        return CM_CALL_CMD_ERR_IN_USE_S;
      }
    }

    if (otasp_call_id != CM_CALL_ID_INVALID)
    {
      CM_MSG_HIGH_0("Cannot originate a DATA call while in a otasp call");
      return CM_CALL_CMD_ERR_IN_USE_S;
    }

    SYS_ARG_NOT_USED(call_ptr_temp);

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    #ifdef FEATURE_MMODE_SC_SVLTE
    /*
    ** Reject the data call while HDR / LTE is in suspend status due to
    ** incoming SO 33/32 data call or if CM has recieved incoming
    ** page indication and is still processing it
    */
    if (ph_ptr->is_1x_await_response ||
        ph_ptr->ss_susp_info[CM_SS_HYBR_2].is_suspend == TRUE ||
        ph_ptr->ss_susp_info[CM_SS_HYBR_1].is_suspend == TRUE)

    {
      CM_MSG_HIGH_3("SO:Cannot orig DATA call while in susp state %d %d %d",
                  ph_ptr->is_1x_await_response,
                  ph_ptr->ss_susp_info[CM_SS_HYBR_2].is_suspend,
                  ph_ptr->ss_susp_info[CM_SS_HYBR_1].is_suspend);
      return CM_CALL_CMD_ERR_IN_USE_S;
    }
    #endif //FEATURE_MMODE_SC_SVLTE

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


    if (voice_call_id != CM_CALL_ID_INVALID)
    {

      cmcall_s_type *voice_call_ptr = cmcall_ptr(voice_call_id);

      if(voice_call_ptr != NULL)
      {
        CM_MSG_HIGH_3("voice_call_id=%d, is_volte=%d, mode=%d",
                     voice_call_id,
                     voice_call_ptr->is_volte,
                     voice_call_ptr->cmcall_mode_info.info_type);

        /* If Packet Call is being originated on HDR and Voice is already on
        ** main, Do not allow HDR Call unless the voice call service option
        ** allows SVDO.
        ** If Data call is on HDR and main stack has VOLTE call, allow HDR
        ** call.
        */
        if(call_ptr->ss == CM_SS_HDR)
        {
          if( voice_call_ptr->ss == CM_SS_MAIN &&
              !voice_call_ptr->is_volte
              #if defined( FEATURE_MMODE_SC_SVLTE )
              && !cmxcall_is_svdo_capable(voice_call_ptr)
              #endif
              )
          {
            return CM_CALL_CMD_ERR_IN_USE_S;
          }

          /*If voice call is already active on hybr2, just reject this data call*/
          if(( voice_call_ptr->asubs_id == SYS_MODEM_AS_ID_2) &&
            (cmph_is_dsds()))
          {
            return CM_CALL_CMD_ERR_IN_USE_S;
          }
          else if( voice_call_ptr->asubs_id == SYS_MODEM_AS_ID_2 &&
            (cmph_is_dsda()))
          {
            is_call_allowed = TRUE;
          }
        }
        else if(call_ptr->ss == CM_SS_MAIN)
        {

          /* Accept Data calls in following conditions only else reject.

          ** MT Voice call on main, packet on main
          ** accepted  if (svd_supported + CDMA service on main)

          ** MO Voice on main, packet on main accepted
          ** accepted  if (Service on main is HDR and voice is VoIP).

          ** Allow HDR call originated on MAIN instance in return
          ** for MT VoIP call on MAIN instance.
          ** cmcall_is_voice_over_hdr_poss() not used since MT calls donot
          ** have favored_mode_pref calculated.
          **

          ** MO Voice on main, packet on main accepted
          ** accepted  if (svd_supported + CDMA service on main + voice call_state = CONV).

          ** MO Voice call on HDR, packet on main rejected.
          */

          /*If voice call is already active on hybr2, just reject this data call*/
          if(( voice_call_ptr->asubs_id == SYS_MODEM_AS_ID_2) &&
            (cmph_is_dsds()))
          {
            return CM_CALL_CMD_ERR_IN_USE_S;
          }
          else if( voice_call_ptr->asubs_id == SYS_MODEM_AS_ID_2 )
          {
            is_call_allowed = TRUE;
          }

          if ( (voice_call_ptr->ss == CM_SS_MAIN) &&
               (voice_call_ptr->direction == CM_CALL_DIRECTION_MT) &&
               (svd_supported) &&
               (ss_ptr->new_srv_avl_info[subId].stack_info[stk_id].sys_mode == SYS_SYS_MODE_CDMA)
               )
          {
            is_call_allowed = TRUE;
          }
          else if ( (voice_call_ptr->ss         == CM_SS_MAIN)            &&
                    (voice_call_ptr->direction  == CM_CALL_DIRECTION_MO)  &&
                    (cmcall_is_ipcall_over_mode_poss (SYS_SYS_MODE_HDR,
                                                        voice_call_ptr))  &&
                    (call_ptr->call_type        == CM_CALL_TYPE_PS_DATA)
                    )
          {
            is_call_allowed = TRUE;
          }
          else if ( (voice_call_ptr->ss      == CM_SS_MAIN)             &&
                    (voice_call_ptr->direction == CM_CALL_DIRECTION_MT) &&
                    (voice_call_ptr->sys_mode  == SYS_SYS_MODE_HDR)     &&
                    (call_ptr->call_type       == CM_CALL_TYPE_PS_DATA)
                    )
          {
            is_call_allowed = TRUE;
          }
          else if ( (voice_call_ptr->ss == CM_SS_MAIN)  &&
                    (voice_call_ptr->direction == CM_CALL_DIRECTION_MO) &&
                     svd_supported                     &&
                    (ss_ptr->new_srv_avl_info[subId].stack_info[stk_id].sys_mode == SYS_SYS_MODE_CDMA) &&
                    (voice_call_ptr->call_state == CM_CALL_STATE_CONV)
                    )
          {
            is_call_allowed = TRUE;
          }
          else if((voice_call_ptr->ss == CM_SS_MAIN) &&
                  voice_call_ptr->is_volte &&
                  (call_ptr->call_type == CM_CALL_TYPE_PS_DATA))
          {
            is_call_allowed = TRUE;
          }

          else if(voice_call_ptr->ss == CM_SS_HDR)
          {
            is_call_allowed = FALSE;
          }

          if (!is_call_allowed)
          {
            return CM_CALL_CMD_ERR_IN_USE_S;
          }
        }

      }
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    if(emergency_call_id != CM_CALL_ID_INVALID)
    {
      cmcall_s_type *e_call_ptr;

      e_call_ptr = cmcall_ptr(emergency_call_id);

      if(e_call_ptr == NULL)
      {
        CM_ERR_0( "invalid e_call_id");
        return CM_CALL_CMD_ERR_CALL_ID_P;
      }
      /*
      ** If ss = CM_SS_HYBR_1, the emergency call is being attempted over hybrid HDR,
      ** and the data call should be allowed (since voip is tunnelled over a data connection)
      ** if ss = MAIN or MAX, and if call is not in conv or svd is not supported, do
      ** not allow the data call
      ** if ss = MAX, the call is being attempted over main and hybrid is turned off ,
      */
      if( (e_call_ptr->ss == CM_SS_MAIN || e_call_ptr->ss == CM_SS_MAX)
        && ( e_call_ptr->call_state != CM_CALL_STATE_CONV
                || !svd_supported
           )
        )
      {
        CM_MSG_HIGH_0("E911 call is up, reject data call origination");
        return CM_CALL_CMD_ERR_IN_USE_S;
      }
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    #ifdef FEATURE_JCDMA
    /* If we are in PD call, also blindly block the data call */
    if (pd_call_id != CM_CALL_ID_INVALID)
    {
      cmcall_s_type *pd_call_ptr = cmcall_ptr(pd_call_id);
      CM_ASSERT(pd_call_ptr);
      SYS_ARG_NOT_USED(pd_call_ptr);
      CM_MSG_HIGH_0("SO35/36 call is up , reject data call origination");
      return CM_CALL_CMD_ERR_IN_USE_S;
    }
    #endif
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* Data call will/can go through, so send the pref_sys_chgd
    ** with the top of the queue and wait for ok to orig
    */
    call_ptr->force_top_pref = TRUE;

  } /* if ((call_ptr->call_type == CM_CALL_TYPE_PS_DATA) ||
    **  (call_ptr->call_type == CM_CALL_TYPE_CS_DATA))
    */

  #ifdef FEATURE_MMODE_DUAL_SIM
  if(call_ptr->call_type == CM_CALL_TYPE_VOICE)
  {
    cm_call_id_type emergency_call_id = cmxcall_find_call(CM_CALL_TYPE_EMERGENCY,
                                          call_ptr->call_id);

    cm_call_id_type voice_call_id = cmxcall_find_call(CM_CALL_TYPE_VOICE,
                                          call_ptr->call_id);

    cm_call_id_type cs_data_call_id = cmxcall_find_call(CM_CALL_TYPE_CS_DATA,
                                          call_ptr->call_id);

    if(emergency_call_id != CM_CALL_ID_INVALID )
    {
      cmcall_s_type *e_call_ptr = cmcall_ptr(emergency_call_id);

      if(e_call_ptr == NULL)
      {
        CM_ERR_0( "invalid e_call_id");
        return CM_CALL_CMD_ERR_CALL_ID_P;
      }

      #ifdef FEATURE_MMODE_DUAL_SIM
       /*If voice call is already active on hybr2, just reject this data call*/
      if(((( e_call_ptr->asubs_id== SYS_MODEM_AS_ID_2) &&
         (cmph_is_dsds() || cmph_is_tsts()))
        ||
         (( e_call_ptr->asubs_id== SYS_MODEM_AS_ID_3) &&
           (cmph_is_tsts()))
            )&&call_ptr->asubs_id == SYS_MODEM_AS_ID_1
            )
          {
            return CM_CALL_CMD_ERR_IN_USE_S;
          }
          #endif
    }

    if(voice_call_id != CM_CALL_ID_INVALID )
    {
      cmcall_s_type *voice_call_ptr = cmcall_ptr(voice_call_id);

      if(voice_call_ptr == NULL)
      {
        CM_ERR_0( "invalid voice_call_id");
        return CM_CALL_CMD_ERR_CALL_ID_P;
      }

      #ifdef FEATURE_MMODE_DUAL_SIM
       /*If voice call is already active on hybr2, just reject this data call*/
      if(((( voice_call_ptr->asubs_id== SYS_MODEM_AS_ID_2) &&
         (cmph_is_dsds() || cmph_is_tsts()))
        ||
         (( voice_call_ptr->asubs_id== SYS_MODEM_AS_ID_3) &&
           (cmph_is_tsts()))
            )&&call_ptr->asubs_id == SYS_MODEM_AS_ID_1
            )
          {
            return CM_CALL_CMD_ERR_IN_USE_S;
          }
          #endif
    }

    if(cs_data_call_id != CM_CALL_ID_INVALID )
    {
      cmcall_s_type *csdata_call_ptr = cmcall_ptr(cs_data_call_id);

      if(csdata_call_ptr == NULL)
      {
        CM_ERR_0( "invalid e_call_id");
        return CM_CALL_CMD_ERR_CALL_ID_P;
      }

      #ifdef FEATURE_MMODE_DUAL_SIM
       /*If voice call is already active on hybr2, just reject this data call*/
      if(((( csdata_call_ptr->asubs_id== SYS_MODEM_AS_ID_2) &&
         (cmph_is_dsds() || cmph_is_tsts()))
        ||
         (( csdata_call_ptr->asubs_id== SYS_MODEM_AS_ID_3) &&
           (cmph_is_tsts()))
            )&&call_ptr->asubs_id == SYS_MODEM_AS_ID_1
            )
          {
            return CM_CALL_CMD_ERR_IN_USE_S;
          }
          #endif
    }

  }
  #endif

  {
    /* Kick the dog before making the following function call.
    */
    cm_kick_dog();

    #ifdef FEATURE_JCDMA
    /* Consult the pre-pending dial number.
    **
    ** NOTE! This function changes the dial digits to insert or
    ** delete pre-pending digits for caller ID display indication
    ** (i.e. 184, 186), area code and 2 stage data dialing.
    */
    cm_nc_pre_pending_dial_func( &call_ptr->num, &call_ptr->call_type );

    /* Kick the dog before making the following function call.
    */
    cm_kick_dog();
    #endif /* FEATURE_JCDMA */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_OTASP
    /* If call type is STD_OTASP, SPC is not all 0's, PRL is valid and
    ** PRL is not disabled, default the call type to NON_STD_OTASP.
    */
    #if defined (FEATURE_UIM_RUIM) && (!defined (FEATURE_UIM_RUIM_W_GSM_ACCESS) \
                         || ( defined (FEATURE_UIM_RUN_TIME_ENABLE)))
    #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for the NAM */
    if (nv_rtre_use_ruim_for_nam((byte) ph_ptr->curr_nam))
    #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      if( call_ptr->call_type == CM_CALL_TYPE_STD_OTASP &&
          prl_hdr_nv_get_ptr()->is_valid &&
          ! prl_hdr_nv_get_ptr()->is_disabled )
      {
        if( cmnv_get_uim_spc_status() )
        {
          call_ptr->call_type = CM_CALL_TYPE_NON_STD_OTASP;
        }
      }
    }
    #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else /* R-UIM is not available */
    #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    #endif /* FEATURE_UIM_RUIM */
    #if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    {
      if( call_ptr->call_type == CM_CALL_TYPE_STD_OTASP &&
          ! cmph_ptr()->is_spc_zero &&
          prl_hdr_nv_get_ptr()->is_valid &&
          ! prl_hdr_nv_get_ptr()->is_disabled )
      {
        call_ptr->call_type = CM_CALL_TYPE_NON_STD_OTASP;
      }
    }
    #endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
    #endif /* FEATURE_OTASP */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /*
    **We don't want to originate an OTASP call while in a OTAPA call
    */
    if (CM_CALL_TYPE_IS_OTASP(call_ptr->call_type))
    {
      if ( cmxcall_find_call(CM_CALL_TYPE_OTAPA, call_ptr->call_id) != CM_CALL_ID_INVALID )
      {
        return CM_CALL_CMD_ERR_ORIG_RES_S;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Set the srv type to be automatic for STD OTASP and E911 call
    */

    switch (call_ptr->call_type)
    {
      case CM_CALL_TYPE_STD_OTASP:
      case CM_CALL_TYPE_EMERGENCY:
        call_ptr->srv_type = CM_SRV_TYPE_AUTOMATIC;
        break;
      default:
        break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



    if(!cmxcall_is_valid_mode_pref( call_ptr->call_type,
                                    call_ptr->favored_mode_pref)) {
      return CM_CALL_CMD_ERR_MODE_PREF_P;
    }

    #ifdef FEATURE_HDR_HYBRID

    if ( (call_ptr->favored_mode_pref == CM_MODE_PREF_HDR_ONLY)  &&
         (

          call_ptr->favored_hybr_pref      == CM_HYBR_PREF_CDMA__HDR

          ))
    {
      sys_modem_as_id_e_type asubs_id = cmph_map_cm_ss_to_subs(CM_SS_HYBR_1);

      /* This call has to be on HDR and use the hybrid operation.
         If there is no collocated HDR then reject the request.

      ic - Due to 5500 CM legacy issues it's legal to have
      ph_ptr->main_stack_info.pref_info.mode_pref set to CM_MODE_PREF_HDR_ONLY and
      ph_ptr->main_stack_info.pref_info.hybr_pref set to CM_HYBR_PREF_ON (user is allowed to
      call cm_ph_cmd_sys_sel_pref_new() with CM_MODE_PREF_HDR_ONLY and
      CM_HYBR_PREF_NO_CHANGE, even if it maps later to CM_HYBR_PREF_ON).
      That means once origination comes:
      - ph_ptr->main_stack_info.pref_info.hybr_pref will be CM_HYBR_PREF_ON
      - CM PREF object will not be in hybrid operation
      - favored_mode_pref will be CM_MODE_PREF_HDR_ONLY
      - favored_hybr_pref will be CM_HYBR_PREF_ON

      Therefore we need to call cmss_is_hybr_status_on()
      before we call cmpref_is_hybr_hdr_call_allowed().
      Only if CM PREF is in hybrid operation but hybrid HDR
      call is not allowed, then we should flag an error.
        */
       if(cmss_is_hybr_status_on() && !cmmsc_auto_is_hybr_hdr_call_allowed(asubs_id))
       {
         return CM_CALL_CMD_ERR_NO_COLLOC_HDR;
       }
    }
    #endif /* FEATURE_HDR_HYBRID */




    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Determine the service option to be used
    ** in the origination.
    */

    if( !cmcall_determine_orig_srv_opt(call_ptr) )
    {
       return CM_CALL_CMD_ERR_NO_SO_S;
    }
  }


  /* Used to be cmcall_client_orig_forward_mc () */

  /* We need to end the OTAPA/OTASP call first before sending origination MC
  ** if we are Rel 0 or if Rel A but p-rev is 6 or less.
  */

  if ( !cmss_is_cdma_svd_supported())
  {
    if (cmxcall_is_pc_otapa() &&
        !cmxcall_is_conv_pc_otapa_commit()
        &&(!(cmph_ptr()->is_svdo_supported
          && (call_ptr->call_type == CM_CALL_TYPE_PS_DATA)))
        )
    {
        end_call_id = cmxcall_find_call (CM_CALL_TYPE_OTAPA,
                                         CM_CALL_ID_INVALID);

        end_call_ptr = cmcall_ptr(end_call_id);

        /* Send OTAPA aborted call event to registered clients */
        if(end_call_ptr != NULL)
        {
          call_mode_info_ptr = CALL_MODE_INFO_PTR(end_call_ptr);
          call_mode_info_ptr->otasp_info.otasp_status =
                                               CM_OTASP_STATUS_OTAPA_ABORTED;
          cmcall_event(end_call_ptr,CM_CALL_EVENT_OTASP_STATUS);
        }

        cmxcall_end_call(end_call_id,
                         CM_CALL_END_CLIENT_END,
                         CAI_REL_NORMAL);
    }
  }
  /* If we are originating a voice call, need to handle call collisions:
  ** 1. If there's already an incoming 1X voice call -> end this MO call.
  */
  if( call_ptr->call_type == CM_CALL_TYPE_VOICE )
  {
    /* Find the existing voice call beside the current voice call */
    cm_call_id_type voice_call_id;
    voice_call_id     = cmxcall_find_call(CM_CALL_TYPE_VOICE,
                                          call_ptr->call_id);

    if (voice_call_id != CM_CALL_ID_INVALID)
    {
      cmcall_s_type *voice_call_ptr = cmcall_ptr(voice_call_id);

      if(voice_call_ptr != NULL)
      {
        /* If there's already an incoming call, we would terminate the
        ** current MO call, which by simply return error.
        */
        if ( (voice_call_ptr->ss      == CM_SS_MAIN)             &&
             (voice_call_ptr->direction == CM_CALL_DIRECTION_MT) &&
             (voice_call_ptr->sys_mode  == SYS_SYS_MODE_CDMA))
        {
          return CM_CALL_CMD_ERR_REJECT_DURING_INCOMING_CALL;
        }

        /* If there's another MO call that originated on CDMA, reject
        ** the current MO call with REASON, CM_CALL_CMD_ERR_IN_USE_S
        ** Until this point, we haven't yet sent the ORIG event for
        ** the current call to the clients, so no need to send
        ** CALL_END_EVENT */
        if ( voice_call_ptr->ss                   == CM_SS_MAIN &&
             voice_call_ptr->direction            == CM_CALL_DIRECTION_MO &&
             ( voice_call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_CDMA ||
               voice_call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA )
           )
        {
          CM_MSG_HIGH_4( "cmxcall_client_orig_proc(): ss %d, direction %d, call_mode_info %d orig_sent_on_mode %d",
                       voice_call_ptr->ss, voice_call_ptr->direction,
                       voice_call_ptr->cmcall_mode_info.info_type,
                       voice_call_ptr->sr.orig_sent_on_mode);
         
          return CM_CALL_CMD_ERR_IN_USE_S;
        }
      }
    }
  }


  /* Existing 1x data calls should be ended before sending voice call or SMS
  ** orig to 1x MC.
  */
  /* If data is in conversation state (traffic), then clients can send SMS as data burst
  ** message using cm_sms_cmd_mo_msg interface. That will be honored. CM will block any
  ** new SO 6 call establishment attempt here
  */
  if ( ( call_ptr->call_type == CM_CALL_TYPE_VOICE ||
         call_ptr->call_type == CM_CALL_TYPE_SMS )
       && !cmss_is_cdma_svd_supported() )
  {

    boolean          end_call = FALSE;

    cmcall_obj_iterator_init(&end_call_itr);

    end_call_ptr = cmcall_obj_get_next(&end_call_itr);

    /* The following cases guarantee that the data call is not for a VoIP
    ** call:
    ** 1. The call is in conv state and origination was sent to CDMA.
    ** 2. VoIP is not available on HDR.
    ** 3. Call is in Orig state, hybrid is on and ss for orig is MAIN.
    ** 4. Voice call service option doesnt support SVDO or Data call is on Main stack.
    */
    while(end_call_ptr != NULL)
    {
      if ( ( end_call_ptr->call_type == CM_CALL_TYPE_CS_DATA   ||
             end_call_ptr->call_type == CM_CALL_TYPE_PS_DATA )
            && (end_call_ptr->ss == CM_SS_MAIN ||
                (end_call_ptr->ss == CM_SS_HYBR_1 && !cmxcall_is_svdo_capable(call_ptr)) )
          )
        {
          if(((call_ptr->call_type == CM_CALL_TYPE_VOICE &&
              !cmcall_is_active_ip_app(SYS_SYS_MODE_HDR, CM_CALL_TYPE_VOICE))
              ||
              (call_ptr->call_type == CM_CALL_TYPE_SMS &&
              !cmcall_is_active_ip_app(SYS_SYS_MODE_HDR, CM_CALL_TYPE_SMS))
              )
             || (end_call_ptr->call_state  == CM_CALL_STATE_CONV
                 && end_call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_CDMA)
            )
          {
            end_call = TRUE;
          }
          if( end_call_ptr->call_state  == CM_CALL_STATE_ORIG
              && cmss_is_hybr_status_on()
              && end_call_ptr->ss == CM_SS_MAIN )
          {
            end_call = TRUE;
          }

          if( end_call )
          {
            cmxcall_end_call(end_call_ptr->call_id,
                             CM_CALL_END_USER_CALL_ORIG_DURING_DATA,
                             CAI_REL_NORMAL);
            end_call = FALSE;
          }
      } /* if(end_call_ptr->call_type == CM_CALL_TYPE_CS_DATA) */

      end_call_ptr = cmcall_obj_get_next(&end_call_itr);
    }  /* while(end_call_ptr != NULL) */

  } /* call_ptr->call_type == CM_CALL_TYPE_VOICE */

  /* We need to end an exisiting data call before sending an emergency call origination to MC
  ** if we are Rel 0 or if Rel A but p-rev is 6 or less.
  */
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {

    if ( !cmss_is_cdma_svd_supported() &&
         cmcall_is_no_call_in_gw_cs_per_subs(call_ptr->ss, call_ptr->asubs_id) &&
         cmcall_is_no_call_in_gw_ps_per_subs(call_ptr->ss, call_ptr->asubs_id)
         #ifdef FEATURE_CM_LTE
         && cmcall_is_no_call_in_lte_ps ()
         #endif
       )
    {
      end_call_id = cmxcall_find_call (CM_CALL_TYPE_CS_DATA,
                                       CM_CALL_ID_INVALID);

      cmxcall_end_call(end_call_id,
                       CM_CALL_END_USER_CALL_ORIG_DURING_DATA,
                       CAI_REL_NORMAL);

      end_call_id = cmxcall_find_call (CM_CALL_TYPE_PS_DATA,
                                       CM_CALL_ID_INVALID);

      cmxcall_end_call(end_call_id,
                       CM_CALL_END_USER_CALL_ORIG_DURING_DATA,
                       CAI_REL_NORMAL);

    } /* if ( !cmss_is_cdma_svd_supported()) */



  } /* if (cmd_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY) */

  #ifdef FEATURE_GPSONE
  /* We need to end the GPS call first before sending origination MC
  ** if we are Rel 0 or if Rel A but p-rev is 6 or less.
  */

  if ( !cmss_is_cdma_svd_supported())
  {
    end_call_id = cmxcall_find_call(CM_CALL_TYPE_PD,
                                    CM_CALL_ID_INVALID);

    cmxcall_end_call(end_call_id,
                     CM_CALL_END_USER_CALL_ORIG_DURING_GPS,
                     CAI_REL_NORMAL);
  }

  #endif /* FEATURE_GPSONE */

  #ifdef FEATURE_CDSMS
  /* We need to end the SMS call first before sending origination MC
  ** if we are Rel 0 or if Rel A but p-rev is 6 or less.
  */
  if ( !cmss_is_cdma_svd_supported())
  {
    if( call_ptr->call_type != CM_CALL_TYPE_PS_DATA )
    {
      end_call_id = cmxcall_find_call(CM_CALL_TYPE_SMS,
                                      CM_CALL_ID_INVALID);

      cmxcall_end_call(end_call_id,
                       CM_CALL_END_USER_CALL_ORIG_DURING_SMS,
                       CAI_REL_NORMAL);
    }
  }


  #endif /* FEATURE_CDSMS) */

  /* used to be cmxcall_orig_mc_reply */



  if ((call_ptr->call_type != CM_CALL_TYPE_PS_DATA) &&
      (call_ptr->call_type != CM_CALL_TYPE_CS_DATA))
  {
      #ifdef FEATURE_CDSMS
      /* Abort MO SMS if it is in progress */
      cmxsms_abort_mo_msg();
      #endif  /* FEATURE_CDSMS */
  }


  #ifdef FEATURE_HDR_HYBRID
  /*
  ** We need to end current HDR call if the origination is not HDR data call.
  **
  ** Do not end the hdr call if originating voice
  ** can be put on hdr through a registered VoIP app
  **
  ** Do not end the hdr call if the voice call is on a SVDO compliant service
  ** option.
  */
  if( (!( call_ptr->call_type == CM_CALL_TYPE_PS_DATA
       && call_ptr->ss == CM_SS_HDR ))
       &&
      (!( call_ptr->call_type == CM_CALL_TYPE_PS_DATA
       && (!cmss_is_hybr_status_on()) ))
       &&
      (!((call_ptr->call_type == CM_CALL_TYPE_VOICE)
       && cmcall_is_ipcall_over_mode_poss (SYS_SYS_MODE_HDR, call_ptr)))
       &&
       ((call_ptr->ss == CM_SS_MAIN || call_ptr->ss == CM_SS_MAX) 
       &&(!cmxcall_is_svdo_capable(call_ptr)))
    )
  {
    end_call_id = cmcall_is_hdr_call();

    #ifdef FEATURE_DEDICATED_C2K_MODE
    if( ss_ptr->is_hdr_session_open )
    {
      CM_MSG_HIGH_0("DCM: Closing HDR session for call");
      cmph_send_hdrmc_session_close_cmd();

      /* If emergency call, end session but don't wait for confirmation.
      */
      if(call_ptr->call_type != CM_CALL_TYPE_EMERGENCY)
      {
        call_ptr->waiting_for_hdr_session_close = TRUE;
      }
    }
    #endif

    if( end_call_id != CM_CALL_ID_INVALID )
    {
      cmxcall_end_call( end_call_id,
                        CM_CALL_END_HDR_RELEASED_BY_CM,
                        CAI_REL_NORMAL);


      /* Set flag in originating call object to tell it that it must hold orig
      ** a little longer to wait for hdr to gracefully close its connection.
      */
      call_ptr->waiting_for_hdr_call_end = TRUE;
    }
    else if((end_call_ptr = cmcall_is_hybr_hdr_orig()) != NULL )
    {
         CM_MSG_HIGH_0("Interrupt HDR call for MC call orig");

         /* Not waiting for end_call_cnf, release the call.
         */
         cmcall_end(end_call_ptr);
    }

    
  }

  #endif /* FEATURE_HDR_HYBRID */


  return cmd_err;

} /* cmxcall_client_orig_proc () */


/*===========================================================================

FUNCTION cmxcall_client_cmd_proc

DESCRIPTION
  Process clients' call commands

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_client_cmd_proc
(
  cm_call_cmd_s_type   *call_cmd_ptr                       /* pointer to a CM command */
)
{
  cm_call_cmd_err_e_type    call_cmd_err;
  boolean                   wait_for_reply;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );

  /* We are NOT supposed to process commands while
  ** waiting for MC reply.
  */
  CM_ASSERT( !cm_reply_should_cmd_wait( (cm_cmd_type*)call_cmd_ptr) )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start by checking whether this is a valid command
  ** relative to the current state of the call/phone.
  */
  call_cmd_err = cmxcall_client_cmd_check( call_cmd_ptr );

  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cmcall_client_cmd_err( call_cmd_ptr, call_cmd_err );

  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {

    /* If there was an error with the answer command, we don't want to tear
       down the call.  We will just return the error back to the client and
       ignore the command.
    */
    if (call_cmd_ptr->cmd != CM_CALL_CMD_ANSWER)
    {
      cmxcall_end_call(call_cmd_ptr->info.call_id,
                       CM_CALL_END_CLIENT_END,
                       CAI_REL_NORMAL);
    }
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no errors were found,
  ** so forward the client command to MC.
  */
  wait_for_reply = cmxcall_client_cmd_forward_mc( call_cmd_ptr );

  /* If NO need to wait for replay from MC,
  ** call the reply function right a way.
  */
  if( ! wait_for_reply )
  {
    cmxcall_client_cmd_mc_reply( call_cmd_ptr, CM_CALL_CMD_ERR_NOERR );
  }

} /* cmxcall_client_cmd_proc() */



/*===========================================================================

FUNCTION cmxcall_voip_redial_over_ps_orig_failure

DESCRIPTION

  VOIP silent redial over PS origination failure.

DEPENDENCIES

   None

RETURN VALUE

  None

SIDE EFFECTS
  none

===========================================================================*/
static void  cmxcall_voip_redial_over_ps_orig_failure
(

  cmcall_ps_failure_e_type             failure,

  cm_hdr_connection_deny_e_type        conn_deny,

  cm_call_orig_fail_e_type             orig_err

)
{

  /*- - - -  - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_iterator_type                call_itr;
  /* call Object Iterator */

  cmcall_s_type                   *ipcall_ptr = NULL;
  /* Pointer to ipcall object */

  /* Bear out if the failure is CMCALL_PS_FAILURE_MAX
  */
  if( failure == CMCALL_PS_FAILURE_MAX )
  {
    return;
  }

  /* Bear out if both connection deny and orignation error are invalid.
  */
  if( (conn_deny == CM_CD_MAX) &&
      (orig_err == CM_CALL_ORIG_ERR_MAX )
    )
  {
    return;
  }
  /*- - - -  - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("VOIP SR PS failure=%d, conn deny=%d, orig failure=%d",
              failure, conn_deny, orig_err);

  cmcall_obj_iterator_init(&call_itr);

  ipcall_ptr = cmcall_obj_get_next_voip_silent_redial_call( &call_itr );

  while(ipcall_ptr != NULL  )
  {

    ipcall_ptr->sr.voip_sr.ps_failure = failure;

    switch ( failure )
    {
      case CMCALL_PS_FAILURE_HDR_CONNECTION_FAILURE:

        ipcall_ptr->sr.voip_sr.failure.conn_deny = conn_deny;

        break;

      case  CMCALL_PS_FAILURE_ORIG:

        ipcall_ptr->sr.voip_sr.failure.ps_orig = orig_err;

        break;

      default:

        continue;

    }

    /*- - - -  - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_MSG_HIGH_3( "VOIP SR found VOIP call id=%d type=%d is_valid=%d",
                 ipcall_ptr->call_id,
                 ipcall_ptr->call_type,
                 ipcall_ptr->sr.voip_sr.is_valid);

    /*- - - -  - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    cmcall_orig_proc( ipcall_ptr );

    ipcall_ptr = cmcall_obj_get_next_voip_silent_redial_call(&call_itr);

  } /* while */

  /*- - - -  - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

}

#ifdef FEATURE_MMODE_SC_SVLTE
/*===========================================================================

FUNCTION cmxcall_is_waiting_lpm_oprt_mode_reply

DESCRIPTION
  Determine if either the phone is in non-ONLINE or
  in the transition to non-ONLINE mode.
  Non-ONLINE comprises: LPM/FTM/POWR_DOWN/RESET

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmxcall_is_waiting_lpm_oprt_mode_reply(

)
{
  cmph_s_type             *ph_ptr = cmph_ptr();
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  ph_cmd_ptr = PH_CMD_PTR( ph_ptr->reply.cmd_ptr );
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  if (ph_ptr == NULL)
  {
    CM_ERR_0("ph_ptr == NULL");
  }

  /* If we're not online, we have passed the transition state */
  if (ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE &&
      ph_ptr->oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE )
  {
    return TRUE;
  }

  /* Override response if we are waiting for LPM command to complete
     This prevent the following conditions causing deadlock:
     1. Incoming page 1X --> CM --> DS (1X waits for CM)
     2. LPM command: UI --> CM --> MMOC --> 1X (MMOC waits for 1X)
     3. DS responded proceed, CM suspend LTE/HDR: CM --> MMOC (CM waits for MMOC)
     4. Deadlocks: CM waits for MMOC, MMOC waits for 1X, 1X waits for CM.
  */
  if( ph_ptr->reply.cmd_ptr != NULL )
  {
    if( ph_ptr->reply.cmd_ptr->cmd_type == CM_CMD_TYPE_PH )
    {
      /* If we are not moving online */
      if( ph_cmd_ptr->cmd == CM_PH_CMD_OPRT_MODE &&
          cmd_info_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE &&
          cmd_info_ptr->oprt_mode != SYS_OPRT_MODE_PSEUDO_ONLINE )
      {
        /* Override result to FALSE */
        return TRUE;
      }
    }
  }
  return FALSE;
}
#endif

/*===========================================================================
FUNCTION cmxcall_is_call_type_voice

DESCRIPTION
  This function checks if the call type of the given call is voice, E911,
  OTASP or NON-Std OTASP

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmxcall_is_call_type_voice( cm_call_type_e_type call_type )
{
   if ( call_type == CM_CALL_TYPE_VOICE ||
        call_type == CM_CALL_TYPE_EMERGENCY ||
        call_type == CM_CALL_TYPE_STD_OTASP ||
        call_type == CM_CALL_TYPE_NON_STD_OTASP
      )
   {
     return TRUE;
   }

   return FALSE;
}

/*===========================================================================

FUNCTION cmxcall_expire_hybr_2_no_srv_timer_during_cdma_call

DESCRIPTION
  This function checks if LTE is OOS on hybr_2 stack and no service uptimer
  is set when 1x MO or MT call is received. If yes, the timer is expired by
  setting it equal to the current clk time so that clients can be immedietely
  informed about service lost on LTE.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmxcall_expire_hybr_2_no_srv_timer_during_cdma_call(
                                     cmcall_s_type *call_ptr
                                     /* Pointer to call object */
)
{
  cmss_s_type    *ss_ptr    = cmss_ptr();
  dword          clk_time   = time_get_uptime_secs();
  boolean        is_no_srv  = FALSE;

  CM_MSG_HIGH_5("Hybr_2_No_Srv_Uptime: call_type %d hybr_gw_opr %d clk time %d hybr_gw_no_srv_uptime %d HYBR_2 srv %d",
                call_ptr->call_type, ss_ptr->info.is_hybr_gw_operational,
                clk_time, ss_ptr->hybr_gw_no_srv_uptime,
                cmss_is_3gpp_in_srv(SYS_SYS_MODE_LTE, FALSE));

  if( ( cmss_intl_srv_info_ptr(CM_SS_HYBR_2)->srv_status == SYS_SRV_STATUS_NO_SRV_INTERNAL &&
        ss_ptr->info.gw_sys_mode == SYS_SYS_MODE_LTE ) ||
      ( cmss_intl_srv_info_ptr(CM_SS_HYBR_2)->srv_status == SYS_SRV_STATUS_NO_SRV &&
        !cmss_is_3gpp_in_srv( SYS_SYS_MODE_LTE, FALSE ) )
    )
  {
    is_no_srv = TRUE;
    CM_MSG_HIGH_1( "is_no_srv %d", is_no_srv);
  }

  /* If the call type is voice call and CM has recevied NO_SRV or
  ** NO_SRV_INTERNAL for LTE on hybr_2 then expire the timer
  */
  if( cmxcall_is_call_type_voice( call_ptr->call_type ) &&
      ss_ptr->hybr_gw_no_srv_uptime != CMSS_NO_SRV_UPTIME_NONE &&
      ss_ptr->hybr_gw_no_srv_uptime > clk_time &&
      ss_ptr->info.is_hybr_gw_operational &&
      is_no_srv
     )
  {
    ss_ptr->hybr_gw_no_srv_uptime = clk_time;
    CM_MSG_HIGH_2( "Expiring No Srv Uptimer: gw_no_srv_uptime %d, clk_time %d",
                 ss_ptr->hybr_gw_no_srv_uptime, clk_time);
  }
}

/*===========================================================================
FUNCTION cmxcall_is_there_a_ongoing_cdma_voice_call

DESCRIPTION
  Checks is there is a call object allocated for CDMA Voice Call.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_id_type cmxcall_is_there_a_ongoing_1x_voice_call ( void )
{
  cmcall_s_type    *call_ptr;                /* Call object pointer */
  cm_iterator_type  call_itr;                /* Call Object Iterator */
  cmss_s_type      *ss_ptr  = cmss_ptr();    /* Pointer to Service System */
  cmph_s_type      *ph_ptr          = cmph_ptr();

  /*-----------------------------------------------------------------------*/
  sys_modem_as_id_e_type   subId = cmph_map_cm_ss_to_subs(CM_SS_MAIN) ;

  uint8                    stk_id = ph_ptr->ss_subs_map[CM_SS_MAIN].stack_id;



  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  if (!cmph_is_as_id_valid(subId))
  {
    return CM_CALL_ID_INVALID;
  }

  while(call_ptr != NULL)
  {
    CM_MSG_HIGH_4( "Check_1x_Voice_Call: sys_mode %d, call_id %d, call_type %d Call State %d",
                   ss_ptr->new_srv_avl_info[subId].stack_info[stk_id].sys_mode, call_ptr->call_id,
                   call_ptr->call_type, call_ptr->call_state );
   
    #ifdef FEATURE_HDR_HYBRID
    CM_MSG_HIGH_1( "Check_1x_Voice_Call: Wait for HDR Call End %d",
                   call_ptr->waiting_for_hdr_call_end );
    #endif

    if( ss_ptr->new_srv_avl_info[subId].stack_info[stk_id].sys_mode == SYS_SYS_MODE_CDMA &&
        call_ptr->call_type == CM_CALL_TYPE_VOICE &&
        ( call_ptr->direction == CM_CALL_DIRECTION_MO &&
          ( call_ptr->call_state == CM_CALL_STATE_ORIG || call_ptr->call_state == CM_CALL_STATE_CONV ) )
      )
    {
      CM_MSG_HIGH_1( "Found 1x Voice Call: call_id %d", call_ptr->call_id );
      return call_ptr->call_id;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return CM_CALL_ID_INVALID;

} /* cmxcall_is_there_a_ongoing_1x_voice_call */

/*===========================================================================

FUNCTION cmxcall_e911_orig_fail_ind

DESCRIPTION
  Indicate origination failure reason to clients

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_e911_orig_fail_ind(

 cmcall_s_type  *call_ptr
)
{
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
	call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_CDMA &&
	call_ptr->end_status != CM_CALL_END_NONE)
  {
    cmcall_event(call_ptr,CM_CALL_EVENT_E911_ORIG_FAIL);

    CM_MSG_HIGH_2("Call event sent to clients for  1x ORIG ERR,call id %d, end_reason %d",
      call_ptr->call_id,call_ptr->end_status);
  }
}

#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
/*===========================================================================

FUNCTION cmxcall_handle_srvcc_ims_cache

DESCRIPTION

 Process SRVCC cache info from IMS and reports HO_ETURA_RESP
 to 1xcp.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_handle_srvcc_ims_cache(

  const cm_srvcc_call_context_rsp_s_type *cahce_rpt_ptr
    /* Pointer to MC report */
)
{
  mc_msg_type    *mc_ptr  = cm_mc_get_buf_else_err_fatal();

  cmcall_s_type           *call_ptr          = NULL;

  cm_call_id_type         call_id     = CM_CALL_ID_INVALID;


  uint8 i =0;
  
  uint8 tot_calls = 0 ;


  if(cm_get_cache_ind().state == CMWCALL_CACHE_NOT_EXPECTED ||cm_get_cache_ind().state  == CMWCALL_CACHE_EXPIRED )
  {
    CM_ERR_0( "IMS cache not expected and is ignored");
    return;
  }

  CM_ASSERT( cahce_rpt_ptr != NULL );



  mc_ptr->hdr.cmd = MC_SRVCC_ORIGINATION_F;

   /* if no call or more than MAX_CALL_NUM_SRVCC call, simply set calls to 0 */
  if(cahce_rpt_ptr->num_of_calls > MAX_CALL_NUM_SRVCC || cahce_rpt_ptr->num_of_calls == 0 )
  {
	CM_MSG_HIGH_1 ("cmxcall_handle_srvcc_cache :Rcvd unmanegable Calls for SRVCC handover: %d,",
                        cahce_rpt_ptr->num_of_calls);
	mc_ptr->origination.number_of_calls = 0;

    cm_set_srvcc_ho_error_state (CM_SRVCC_HO_ERR_UNMANAGED_CALLS);

  }
  else
  {
    for(i=0; i<cahce_rpt_ptr->num_of_calls; i++)
	{
	  call_ptr = cmcall_ptr(cahce_rpt_ptr->call_context[i].call_info.ip_call.call_id);
	  
	  /* INT IMS case */
      if(call_ptr!= NULL)
      {
		call_ptr->is_int_ims       = TRUE;

		call_ptr->srvcc_prog_state = CM_SRVCC_STATE_HO_EUTRA_RSP_SENT;

		  
	    /*case where CM sent end request to IMS after HO_START */
		if(call_ptr->call_state      == CM_CALL_STATE_IDLE &&
            call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT)
		{
		  continue;
		}

	  }
	  else
	  {
		call_id = cm_call_id_allocate();

        if(call_id == CM_CALL_ID_INVALID)
        {
          CM_MSG_HIGH_0("cmxcall_handle_srvcc_cache, call not found");
          break;
        }

        if ((call_ptr = cmcall_ptr(call_id)) == NULL)
        {
          sys_err_fatal_null_ptr_exception();
        }

        call_ptr->is_int_ims= FALSE;
	  }


	  call_ptr->is_srvcc_call = TRUE;
	  
	  call_ptr->is_mpty = FALSE;

      call_ptr->call_state = cahce_rpt_ptr->call_context[i].call_info.ip_call.call_state;

      call_ptr->call_subst = cahce_rpt_ptr->call_context[i].call_info.ip_call.call_sub_state;


	  CM_MSG_HIGH_4 ("SRVCC call id:%d ,substate: incom_subst %d , orig_subst %d, conv_subst %d",
          call_ptr->call_id,call_ptr->call_subst.incom, call_ptr->call_subst.orig, call_ptr->call_subst.conv);

      call_ptr->direction = cahce_rpt_ptr->call_context[i].call_info.ip_call.call_direction;

	    call_ptr->call_type = cahce_rpt_ptr->call_context[i].call_info.ip_call.call_type;

      call_ptr->num =cahce_rpt_ptr->call_context[i].call_info.ip_call.call_num;


      if(call_ptr->asubs_id == SYS_MODEM_AS_ID_NONE )
      {
        call_ptr->asubs_id = SYS_MODEM_AS_ID_1;
      }

      cmxcall_fill_srvcc_orig_param(call_ptr, &mc_ptr->origination, FALSE); 

	  tot_calls++;
	
	}

    mc_ptr->origination.number_of_calls = tot_calls;
  }


  cm_mc_send_cmd( mc_ptr );
  cm_process_cmd_on_ims_cache(CMWCALL_CACHE_RECIEVED);
}

#endif

#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) 

/*===========================================================================

FUNCTION cmxcall_set_srvcc_called_party

DESCRIPTION

 Set calledparty number from STN-SR NV provision for
 SRVCC orig request

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_set_srvcc_called_party
(
   mc_origination_type  *orig_param
)
{
  cmxcall_cdma_srvcc_stn_sr_num stn_sr_num;

  int dig_len;

  if ( cmefs_get ( CMEFS_1xSRVCC_CALLED_PARTY_NUM,
                     (byte*)&stn_sr_num,
                     sizeof(cmxcall_cdma_srvcc_stn_sr_num),
                     SYS_MODEM_AS_ID_1)
   )
  {
    /* Check the dial string for valid number of digits */
    if (stn_sr_num.reg_cnt > CM_NV_MAX_STN_SR_DIGITS && !orig_param->is_emergency )
    {
      CM_MSG_MED_1 ("%d: Invalid digit count!", stn_sr_num.reg_cnt  );
    }
	else if(stn_sr_num.e911_cnt > CM_NV_MAX_STN_SR_DIGITS && orig_param->is_emergency )
	{
      CM_MSG_MED_1 ("%d: Invalid digit count!", stn_sr_num.e911_cnt );
	}

	if(!orig_param->is_emergency )
	{
     /* Check the dial string for valid digits */
     for (dig_len = 0; dig_len < stn_sr_num.reg_cnt; dig_len++)
     {
        /* Check for dial digits (0 - 9, *, #) */
        if ( !INRANGE(stn_sr_num.regular_dialed_digits[dig_len], '0', '9') &&
                 (stn_sr_num.regular_dialed_digits[dig_len] != '*') &&
                 (stn_sr_num.regular_dialed_digits[dig_len] != '#') &&
                 (stn_sr_num.regular_dialed_digits[dig_len] != '+'))
        {

         CM_MSG_MED_0 ("Invalid digit set for normal STN_SR");
        }
      }


      orig_param->cnt  = memscpy( &orig_param->called_address,ARR_SIZE(orig_param->called_address), stn_sr_num.regular_dialed_digits, stn_sr_num.reg_cnt );


	}
	else
	{
      /* Check the dial string for valid digits */
      for (dig_len = 0; dig_len < stn_sr_num.e911_cnt; dig_len++)
      {
        /* Check for dial digits (0 - 9, *, #) */
        if ( !INRANGE(stn_sr_num.e911_dialed_digits[dig_len], '0', '9') &&
                 (stn_sr_num.e911_dialed_digits[dig_len] != '*') &&
                 (stn_sr_num.e911_dialed_digits[dig_len] != '#') &&
                 (stn_sr_num.e911_dialed_digits[dig_len] != '+'))
        {

          CM_MSG_MED_0 ("%d: Invalid digit set for e911 STN_SR");
        }
      }

	  orig_param->cnt  = memscpy( orig_param->called_address,ARR_SIZE(orig_param->called_address), stn_sr_num.e911_dialed_digits, stn_sr_num.e911_cnt );

	}
  }
  else
  {
	CM_MSG_HIGH_0 ("STN_SR number not set for e911/normal call");
  }

  CM_MSG_HIGH_3 ("STN_SR provisioned %s, reg_len= %d, e911_len = %d",  orig_param->called_address ,
                  stn_sr_num.reg_cnt ,stn_sr_num.e911_cnt );

}

/*===========================================================================

FUNCTION cmxcall_fill_srvcc_orig_param

DESCRIPTION

 Process SRVCC cache info from IMS and fill 1x SRVCC origination
 fields

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_fill_srvcc_orig_param(

  const cmcall_s_type           *call_ptr,
    /* Pointer to MC report */

  mc_origination_type    *orig_param,
  boolean is_drvcc_call
)
{

  orig_param->call_id = call_ptr->call_id;

  #ifdef FEATURE_IS2000_REL_A
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  orig_param->orig_reason = call_ptr->sr.re_orig_reason;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  orig_param->orig_count = call_ptr->sr.re_orig_count;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  orig_param->is_packet_data = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  orig_param->is_dormant_packet_data =
    ( cmph_ptr()->packet_state == CM_PACKET_STATE_DORMANT ) ? TRUE : FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  orig_param->is_emergency = (call_ptr->call_type==CM_CALL_TYPE_EMERGENCY);

  orig_param->sr_id_included = FALSE;
  orig_param->sr_id = 0;

  orig_param->qos_parms_incl = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_MSG_HIGH_5("cmxcall_fill_srvcc_orig_param: is_sr=%d, sr_cnt=%d, is_emrg=%d is_pkt=%d, is_dormant_pkt=%d",
            orig_param->orig_reason,
            orig_param->orig_count,
            orig_param->is_emergency,
            orig_param->is_packet_data,
            orig_param->is_dormant_packet_data);

  #endif /* FEATURE_IS2000_REL_A */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  orig_param->service_option = CAI_SO_VOICE_WILDCARD;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Until MC cleans its code from DS support,
  ** make sure it knows that CM is the one to originate.
  ** Gotta stay under feature as it is under feature flag in mc.h
  */
  #ifdef FEATURE_DS
  orig_param->ds_originated = FALSE;
  orig_param->amps_data     = FALSE;
  #endif /* FEATURE_DS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy called party number structure.
  */
  if(!is_drvcc_call)
  {
  cmxcall_set_srvcc_called_party( orig_param);
  }

  orig_param->number_plan = CM_NUM_PLAN_ISDN;

  orig_param->digit_mode =CM_DIGIT_MODE_4BIT_DTMF;

  orig_param->number_type = CM_NUM_TYPE_INTERNATIONAL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  orig_param->called_party_subaddr.extension_bit =
    call_ptr->num.subaddr.extension_bit;

  #ifndef FEATURE_MMODE_REMOVE_1X
  orig_param->called_party_subaddr.subadd_type =
      (cai_subaddress_types_type)call_ptr->num.subaddr.subadd_type;
  orig_param->called_party_subaddr.odd_even_ind =
      (cai_subaddress_eo_type)call_ptr->num.subaddr.odd_even_ind;
  #else
  #error code not present
#endif

  orig_param->called_party_subaddr.num_char = memscpy( orig_param->called_party_subaddr.chari,
                                                                 sizeof(orig_param->called_party_subaddr.chari),
                                                                 call_ptr->num.subaddr.chari,
                                                                 call_ptr->num.subaddr.num_char );


   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Copy calling party number structure.
  */
  orig_param->calling_party_number.number_type =
    call_ptr->calling_num.number_type;
  orig_param->calling_party_number.number_plan =
    call_ptr->calling_num.number_plan;
  #ifndef FEATURE_MMODE_REMOVE_1X
  orig_param->calling_party_number.pi =
    (cai_call_party_num_pi_type)call_ptr->calling_num.pi;
  orig_param->calling_party_number.si =
    (cai_call_party_num_si_type)call_ptr->calling_num.si;
  orig_param->calling_party_subaddr.subadd_type =
    (cai_subaddress_types_type)call_ptr->calling_num.subaddr.subadd_type;
  orig_param->calling_party_subaddr.odd_even_ind =
    (cai_subaddress_eo_type)call_ptr->calling_num.subaddr.odd_even_ind;
  #else
  #error code not present
#endif

  orig_param->calling_party_number.num_char = memscpy( orig_param->calling_party_number.chari,
                                                               sizeof(orig_param->calling_party_number.chari),
                                                               call_ptr->calling_num.buf,
                                                               call_ptr->calling_num.len );
  orig_param->calling_party_subaddr.extension_bit =
    call_ptr->calling_num.subaddr.extension_bit;
  orig_param->calling_party_subaddr.num_char = memscpy( orig_param->calling_party_subaddr.chari,
                                                                sizeof(orig_param->calling_party_subaddr.chari),
                                                                call_ptr->calling_num.subaddr.chari,
                                                                call_ptr->calling_num.subaddr.num_char );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For targets with OTASP, tell MC whether this an OTASP call,
  ** and if so indicate the activation code, as well.
  */
  #ifdef FEATURE_OTASP

  orig_param->otasp_activation = FALSE;

  #endif /* FEATURE_OTASP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_IS2000
  orig_param->drs = FALSE;
  #endif /* FEATURE_IS2000 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  orig_param->is_special_srv_data_call = FALSE;
  CM_MSG_HIGH_1("MC_SRVCC_ORIG: special_srv =%d",
            orig_param->is_special_srv_data_call);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_HDR_HANDOFF
  orig_param->last_act_data_net =
         map_cm_mode_to_mc_mode( call_ptr->last_act_data_net );
  #endif /* FEATURE_HDR_HANDOFF */

}


#endif

/*===========================================================================

FUNCTION cmxcall_mc_rpt_proc

DESCRIPTION
  Process MC reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
)
{

  /*lint -e{826} keep it suppressed for general / specific walks as well */
  cm_mc_rpt_type    *cm_mc_rpt_ptr = (cm_mc_rpt_type *) rpt_ptr;
    /* Pointer to MC report */

  static cmxcall_out_of_order_msg_s_type out_of_order_msgs;
      /* Structure that will process MC reports
         that arrive out of sequence */

  cmcall_s_type           *call_ptr          = NULL;
      /* Pointer to call object */

  cdma_cmcall_type        *call_mode_info_ptr = NULL;
      /* Pointer to call mode info */

  cmph_s_type             *ph_ptr            = cmph_ptr();
      /* Point at phone object */

  cmss_s_type             *ss_ptr            = cmss_ptr();
      /* Point at serving system object */

  db_items_value_type      db_item;
      /* Item to do db_get() and db_put() */

  int                      idx;

  int                      i;
      /* Loop index */

  #if !defined(FEATURE_MMODE_REMOVE_1X)
  mc_msg_type             *mc_ptr;
  #endif /*  !defined(FEATURE_MMODE_REMOVE_1X) */

  #ifdef FEATURE_ACP
  static cm_num_s_type     temp_amps_num;
      /* Temporary storage for AMPS caller Id in case ALERT comes late */

  static boolean           temp_amps_num_good = FALSE;
      /* Indicator for whether temporary AMPS number is valid or not */
  #endif

  #ifdef FEATURE_GPSONE
  cmpd_session_s_type  *pd_ptr             = cmpd_ptr(CMPD_SESSION_OBJ_MAIN);
  cmpd_session_s_type  *pd_dmod_ptr        = cmpd_ptr(CMPD_SESSION_OBJ_DMOD);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Incoming HPCD number translation : remove IDD  */
  cm_num_details_for_conversion_s_type num_details;
       /* Details before calling plus to digits conversion api */
  cm_num_s_type           converted_num;
  /* Converted digits as a result of calling
  ** plus to digits conversion api
  */

  cmutil_conver_result_e_type     conver_result = CMUTIL_CONVER_RESULT_NONE;
  /* Result of calling plus to digits conversion api */

  cmutil_num_conv_req_e_type      num_conv_req = CMUTIL_NUM_CONV_REQ_NONE;
  /* Initialize num conversion request */

  sys_sys_mode_e_type             prev_orig_mode = SYS_SYS_MODE_NONE;


  #ifdef FEATURE_GSTK
  cm_subscription_status_e_type    gw_subs_status =
                                          CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      /* GW subscription status */

  cm_subscription_status_e_type    cdma_subs_status =
                                          CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      /* CDMA subscription status */

  #endif /* FEATURE_GSTK */
  cm_mm_call_info_s_type   *call_info_ptr = NULL;
  boolean send_page_fail_event = FALSE;
  
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) 
  cm_iterator_type    call_itr;
  cm_mid_srvcc_ho_comp_list_s_type *ho_comp_list = NULL;
  cm_mm_srvcc_call_info_s_type *srvcc_call_info_ptr = NULL;
  byte  num_srvcc_calls   = 0;
  sys_modem_as_id_e_type ll_as_id = SYS_MODEM_AS_ID_1;
  #endif 
  
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
  cm_call_id_type          call_id = CM_CALL_ID_INVALID;
  cm_call_id_type          active_srvcc_call_id = CM_CALL_ID_INVALID;    
  cm_orig_q_s_type* p_orig_emg = NULL;
  cm_orig_q_s_type* p_orig = NULL;
  cm_orig_q_s_type* ph_p_orig=NULL;
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_mc_rpt_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );

  #ifdef FEATURE_GPSONE
  CM_ASSERT( pd_ptr != NULL );
  CM_ASSERT( pd_dmod_ptr != NULL );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do command specific processing as follows:
  ** 1. Update appropriate call object fields.
  ** 2. Modify the call state if necessary.
  ** 3. Notifying clients of event if necessary.
  */
  switch( cm_mc_rpt_ptr->hdr.cmd )
  {
                     /*------------------------------*/
                     /*     Analog incoming call     */
                     /*------------------------------*/
  case CM_MT_CALL_END_IN_ACCESS_STATE_F:
    call_info_ptr = cmcall_info_alloc();
    switch(cm_mc_rpt_ptr->mt_call_drop_info.rpt_type)
    {
    case CM_LOCK_F:
      call_info_ptr->end_status = CM_CALL_END_CDMA_LOCK;
      break;
    case CM_INTERCEPT_F:
      call_info_ptr->end_status = CM_CALL_END_INTERCEPT;
      break;
    case CM_CALL_RELEASE_F:
      cmcall_set_mt_call_rel_code(call_info_ptr, cm_mc_rpt_ptr->mt_call_drop_info.mt_fail_code.rel_code);
      break;
    case CM_CALL_ORIG_FAIL_F:
      switch(cm_mc_rpt_ptr->mt_call_drop_info.mt_fail_code.mt_orig_fail_code)
      {
      case CM_CALL_ORIG_ERR_ACC_FAIL_REJ_ORD:
        call_info_ptr->end_status = CM_CALL_END_ACC_FAIL_REJ_ORD;
        break;
      case CM_CALL_ORIG_ERR_ACC_FAIL_RETRY_ORD:
        call_info_ptr->end_status = CM_CALL_END_ACC_FAIL_RETRY_ORD;
        break;
      case CM_CALL_ORIG_ERR_TIMEOUT_T42:
        call_info_ptr->end_status = CM_CALL_END_TIMEOUT_T42;
        break;
      case CM_CALL_ORIG_ERR_TIMEOUT_T40:
        call_info_ptr->end_status = CM_CALL_END_TIMEOUT_T40;
        break;
      case CM_CALL_ORIG_ERR_TRM_REQ_FAIL:
        call_info_ptr->end_status = CM_CALL_END_TRM_REQ_FAIL;
        break;
      case CM_CALL_ORIG_ERR_TIMEOUT_T41:
        call_info_ptr->end_status = CM_CALL_END_TIMEOUT_T41;
        break;
      case CM_CALL_ORIG_ERR_REORDER:
        call_info_ptr->end_status = CM_CALL_END_REORDER;
        break;
      case CM_CALL_ORIG_ERR_MAX_ACC:
        call_info_ptr->end_status = CM_CALL_END_MAX_ACCESS_PROBE;
        break;
      default:
              call_info_ptr->end_status = CM_CALL_END_REL_NORMAL;
  CM_ERR_1( "Orig fail code=%d",(uint8)cm_mc_rpt_ptr->mt_call_drop_info.mt_fail_code.mt_orig_fail_code);
  break;
      }
      break;

      default:
  CM_ERR_1( "Orig fail code=%d",(uint8)cm_mc_rpt_ptr->mt_call_drop_info.mt_fail_code.mt_orig_fail_code);
  break;
    }

    CM_MSG_HIGH_2( "call_info_ptr->endstatus = %d srv_opt =%d", call_info_ptr->end_status, cm_mc_rpt_ptr->mt_call_drop_info.srv_opt );
    call_info_ptr->mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
    call_info_ptr->mode_info.info.cdma_call.srv_opt = cm_mc_rpt_ptr->mt_call_drop_info.srv_opt;
    if(call_info_ptr->call_type == CM_CALL_TYPE_NONE)
    {
      call_info_ptr->call_type = cmxcall_determine_call_type_from_so(cm_mc_rpt_ptr->mt_call_drop_info.srv_opt);
    }
    CM_MSG_HIGH_1( "mc_rpt_proc updating call type to =%d",call_info_ptr->call_type);
    cmclient_list_call_event_ntfy(CM_CALL_EVENT_MT_CALL_PAGE_FAIL,call_info_ptr);
    cm_mem_free(call_info_ptr);
    break;

  case CM_ALERT_F:
    if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->alert.call_id))
    {
      CM_ERR_1( "ALERT: Call Id %d not allocated", cm_mc_rpt_ptr->alert.call_id);
      break;
    }


    if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->alert.call_id )) == NULL )
    {
      CM_MSG_HIGH_1( "ALERT: Can't get pointer to call object. id = %d",
        cm_mc_rpt_ptr->alert.call_id);
      break;
    }
    call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

    /* If the call is not in idle state and not in incom state
    ** (e.g. due to a race condition),
    ** first end the call then start the new call.
    */
    if( call_ptr->call_state != CM_CALL_STATE_IDLE &&
      call_ptr->call_state != CM_CALL_STATE_INCOM )
    {
      call_ptr->end_status = CM_CALL_END_INCOM_CALL;
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      call_mode_info_ptr->ndss_connect     =
        call_mode_info_ptr->ndss_in_progress = FALSE;
#endif  /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

      cmcall_end_no_call_id_dealloc( call_ptr );
    }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If caller id gets here before ALERT, copy num
      */
#ifdef FEATURE_ACP
      if ( temp_amps_num_good == TRUE )
      {
        /*
        ** 738: temp_amps_num not explicitely initialized
        ** That's fine, temp_amps_num_good tells us it's OK to use it
        */
        /*lint -esym(738,temp_amps_num)*/
        *(&(call_ptr->num)) = *temp_amps_num;
        /*lint +esym(738,temp_amps_num)*/
        temp_amps_num_good = FALSE;
      }
#endif
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If phone configured to answer incoming calls as voice,
      ** set the call type to VOICE, else set the call type
      ** to DATA.
      */
      if( ph_ptr->answer_voice == CM_ANSWER_VOICE_AS_VOICE )
      {
        call_ptr->call_type = CM_CALL_TYPE_VOICE;
      }
      else
      {
        call_ptr->call_type = CM_CALL_TYPE_CS_DATA;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Indicate that this is an AMPS call.
      */
      call_ptr->srv_type = CM_SRV_TYPE_AMPS;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Analog alert does not have a signal type indication, so set
      ** the signal type to the regular tone signal - i.e. CAI_SIG_IS54B
      */
      call_ptr->signal.signal_type = CAI_SIG_IS54B;

      /* Extract the pitch from bits 7 & 8 of the signal field.
      ** Change to use bit.h services ????
      */
      call_ptr->signal.alert_pitch =
                 (byte) ( (((byte) cm_mc_rpt_ptr->alert.signal) >> 6) & 0x03 );

      /* Extract the signal from bits 1 - 6 of the signal field.
      ** Change to use bit.h services ????
      */
      call_ptr->signal.signal = (byte) (cm_mc_rpt_ptr->alert.signal & 0x3F);

      /* Finally, indicate that the signal information as available.
      */
      call_ptr->signal.is_signal_info_avail = TRUE;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If caller ID is attached in the message, extract the caller ID and
      ** overwrite the existing caller ID if any.
      */
      if( cm_mc_rpt_ptr->alert.cnt != 0 )
      {
        cm_num_copy( &call_ptr->num,
                     cm_mc_rpt_ptr->alert.cpn,
                     cm_mc_rpt_ptr->alert.cnt );

        /* Extract the presentation indicator.
        */
        CM_NUM_PI( &call_ptr->num ) = cm_mc_rpt_ptr->alert.pi;

        /* If screening indicator is valid, extract it over.
        */
        if( cm_mc_rpt_ptr->alert.si <= CAI_NET_SI )
        {
          CM_NUM_SI( &call_ptr->num ) = cm_mc_rpt_ptr->alert.si;
        }
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Update the call state,
      ** tell phone about call start
      ** and notify clients of event.
      */
      call_ptr->call_state = CM_CALL_STATE_INCOM;
      call_ptr->call_subst.incom = CM_CALL_INCOM_SUBST_REGULAR;
      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;

      /* Set the call direction */
      cmcall_set_call_direction (call_ptr, CM_CALL_DIRECTION_MT);

      if (ss_ptr->info.sys_mode != SYS_SYS_MODE_NO_SRV)
      {
        call_ptr->sys_mode = ss_ptr->info.sys_mode;
      }

      cmph_call_start( call_ptr );
      cmcall_event( call_ptr, CM_CALL_EVENT_INCOM );

      break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SIGNAL_F:

      /*
      ** This message may have arrived out of order.
      ** If the call id is unassigned from MC, send the event with the unassigned call id.
      ** if the call id received is valid from MC but is invalid in CM, ignore the report, no need to send the event.
      ** If the call id is valid but the call state is IDLE, buffer the event and
      ** wait for the incoming event.
      ** Otherwise send the event now.
      */
      if (cm_mc_rpt_ptr->signal.call_id == CM_CALL_ID_UNASSIGNED)
      {
        cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_SIGNAL);
        break;
      }

    /* The below check is added for the race condition: If the MT call  is rejected, CM clears the call_ptr.
             CM sends the End Req to 1X. Same time 1x sends the CM_Signal_F/Alerting to CM , then Signal report
             needs to be ignored and no need to send the event to the clients
        */
      if(!cm_call_id_is_allocated (cm_mc_rpt_ptr->signal.call_id))
      {
        break;
      }

      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->signal.call_id )) == NULL )
      {
        CM_ERR_1( "CM_SIGNAL_F: Can't get pointer to call object. id = %d",
                                        cm_mc_rpt_ptr->signal.call_id);
        cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_SIGNAL);
        break;
      }

      if (call_ptr->call_state == CM_CALL_STATE_IDLE)
      {
        /* Save the command contents to process it later.
           The assumption is that a CM_INCOMING_CALL_F is soon to show. */

        /* Set the flags */
        out_of_order_msgs.is_signal_flag = TRUE;

        /* Copy the data */
        CM_MSG_LOW_0 ("Save CM_SIGNAL_F");
        *(&(out_of_order_msgs.signal_buffer)) = *(&(cm_mc_rpt_ptr->signal));
      }
      else
      {
        /* Message did not arrive out of order so process it now */
        call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

       /* receive a signal information record */

       call_mode_info_ptr->is_last_cdma_info_rec = cm_mc_rpt_ptr->signal.hdr.last_record;
       call_ptr->signal.signal_type              = (byte) cm_mc_rpt_ptr->signal.signal_type;
       call_ptr->signal.alert_pitch              = (byte) cm_mc_rpt_ptr->signal.alert_pitch;
       call_ptr->signal.signal                   = (byte) cm_mc_rpt_ptr->signal.signal;
       call_ptr->signal.is_signal_info_avail     = TRUE;
       cmcall_event( call_ptr, CM_CALL_EVENT_SIGNAL );
      }

      break; /* CM_SIGNAL_F */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------*/
                     /*      CDMA Incoming call      */
                     /*------------------------------*/

    case CM_INCOMING_CALL_F:
    #ifdef FEATURE_HDR_HYBRID
    case CM_HDR_INCOMING_CALL_F:
    #endif  /* FEATURE_HDR_HYBRID */

      /* Reset the flag since the call is tracked by call id and call obj now */
      cmph_ptr()->is_1x_mt_call_pending = FALSE;

      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->incoming_call.call_id))
      {
        CM_ERR_1( "INCOMING: Call Id %d is unallocated.",
          cm_mc_rpt_ptr->incoming_call.call_id);
        break;
      }

      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->incoming_call.call_id ))
        == NULL )
      {
        CM_MSG_HIGH_1( "INCOMING: Can't get pointer to call object id = %d",
          cm_mc_rpt_ptr->incoming_call.call_id);
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      /* Setting the ss instance for the call
      */
      call_ptr->ss = CM_SS_MAIN;

      call_ptr->asubs_id = SYS_MODEM_AS_ID_1;

      #ifdef FEATURE_HDR_HYBRID
      if ( (cm_mc_rpt_ptr->hdr.cmd == CM_HDR_INCOMING_CALL_F) &&
           (cmss_is_hybr_status_on())
         )
      {
        call_ptr->ss = CM_SS_HDR;
      }
      #endif  /* FEATURE_HDR_HYBRID */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

#ifdef FEATURE_ACP
      temp_amps_num_good = FALSE;    /* Invalidate temp amps caller id info */
#endif

      #ifdef FEATURE_MMODE_DUAL_SIM
      if ( (cm_mc_rpt_ptr->hdr.cmd == CM_INCOMING_CALL_F) && cmxcall_is_voice_srv_opt(cm_mc_rpt_ptr->incoming_call.so) &&
        cmph_is_msim() && cm_mc_rpt_ptr->incoming_call.subs_capability != cmmsc_get_curr_msc_max_cap(SYS_MODEM_AS_ID_1))
      {
        CM_MSG_HIGH_2("Subs capability mismatch in MMODE 0x%x annd 1x 0x%x",
          cmmsc_get_curr_msc_max_cap(SYS_MODEM_AS_ID_1), cm_mc_rpt_ptr->incoming_call.subs_capability);

        mc_ptr = cm_mc_get_buf_else_err_fatal();
        mc_ptr->hdr.cmd = MC_END_F;
        mc_ptr->end.num_call_ids = 1;
        mc_ptr->end.call_ids[0] = cm_mc_rpt_ptr->incoming_call.call_id;
        mc_ptr->end.end_reasons[0] = CAI_REL_NORMAL;
        
        cm_mc_send_cmd( mc_ptr );
        
        cm_call_id_deallocate (cm_mc_rpt_ptr->incoming_call.call_id);
        break; /* case CM_INCOMING_CALL_F: */


      }
      #endif
      /* If the call is not in idle state (e.g. due to a race condition
         or "alert with info" message that CM sees as simply CM_INCOMING_CALL_F
         for a call that is already up), first end the call then start a new call.
      */
      if ( cmcall_overall_call_state() == CM_OVERALL_CALL_STATE_ORIG ||
           cmcall_overall_call_state() == CM_OVERALL_CALL_STATE_ORIG_INCOM
         )
      {

        cm_call_id_type data_call_id = CM_CALL_ID_INVALID;
        boolean         svd_supported;

        data_call_id = cmcall_is_there_a_call_type(CM_SS_MAIN,
                                     CM_CALL_TYPE_DATA, cm_mc_rpt_ptr->incoming_call.call_id);

       if(data_call_id == CM_CALL_ID_INVALID)
       {
          data_call_id = cmcall_is_there_a_call_type(CM_SS_HYBR_1,
                                     CM_CALL_TYPE_DATA, cm_mc_rpt_ptr->incoming_call.call_id);
       }

        svd_supported = cmss_is_cdma_svd_supported();

        #ifdef FEATURE_MMODE_SC_SVLTE
        /* If incoming 1x data call comes to CM and UE supports SVLTE/SVDO,
        ** 1. If hybr stacks are not in suspend state (error condition), end the
        **    incoming call.
        ** 2. end any outgoing data call
        ** 3. end the incoming SO33 call in case there is any MO call other than data
        */
        if ( cmcall_is_1x_data_srv_opt(cm_mc_rpt_ptr->incoming_call.so)
           )
        {
          /* case 1 : take care of error condition if it occurs . Ideally this
          ** should not happen. 1xcp should send incoming_page indication which
          ** results in CM suspending hybr stacks before sending incoming_call
          ** indication
          */

          if ( ( ph_ptr->ss_susp_info[CM_SS_HYBR_2].is_suspend == FALSE
               ) ||
               ( ph_ptr->ss_susp_info[CM_SS_HYBR_1].is_suspend == FALSE
               )
             )
          {
            CM_MSG_HIGH_0("SO: Hybr stack not susp. End incom call");
            /* Send an end command to MC to end the call */
            cmcall_send_mc_end_cmd(call_ptr, CAI_REL_NORMAL);
            cm_call_id_deallocate (cm_mc_rpt_ptr->incoming_call.call_id);
            cmxcall_unsuspend_hybrid_operation();
            call_ptr = NULL;
            break;
          }

          /*case 2: end MO data call if any */
          if (data_call_id != CM_CALL_ID_INVALID)
          {
            cmcall_s_type *end_call_ptr = cmcall_ptr(data_call_id);
            if (end_call_ptr != NULL)
            {
              CM_MSG_HIGH_1("SO:End data call on SO33/32 call, call id = %d",
                           data_call_id);

              /* Send call end cmd to LL
              */
              cmxcall_send_end_cmd(end_call_ptr, CAI_REL_NORMAL);
              end_call_ptr->end_status = CM_CALL_END_INCOM_CALL;

              /* If not waiting for end_call_cnf, release the call.
              */
              if ( end_call_ptr->call_state != CM_CALL_STATE_IDLE ||
                   end_call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
                 )
              {
                /* End call on CM side */
                cmcall_end(end_call_ptr);
              }
            }
          }
          /* case 3 : end MT 1X data call */
          else /* no need to check svd_supported, as svd_supported will always be false */
          {
            cmcall_s_type *end_call_ptr = cmcall_ptr(cm_mc_rpt_ptr->incoming_call.call_id);
            CM_ASSERT(end_call_ptr);
            CM_MSG_HIGH_1("SO:End incoming data call %d as there is a call in orig state, other than data call",
              cm_mc_rpt_ptr->incoming_call.call_id);
            /* initialize cdma_info.srv_opt, as it would be required when call end cnf would come */
            end_call_ptr->cmcall_mode_info.info.cdma_info.srv_opt = cm_mc_rpt_ptr->incoming_call.so;
            cmcall_send_mc_end_cmd(end_call_ptr,CAI_REL_NORMAL);
            /*cmxcall_set_call_wait_for_1X_end_cnf(call_ptr);*/

            /* If not waiting for call end cnf from 1X protocol.
            ** Since this is special handling for SO33/32 - 1X will not
            ** have more than 1 and only 1 call at a time.
            */
            if( ( cmph_ptr()->ss_susp_info[CM_SS_HYBR_2].is_suspend == TRUE ||
              cmph_ptr()->ss_susp_info[CM_SS_HYBR_1].is_suspend == TRUE )
              &&
              cmcall_is_1x_data_srv_opt(end_call_ptr->cmcall_mode_info.info.cdma_info.srv_opt)
              )
            {
              CM_MSG_HIGH_0("SO: Delay call end event until CALL_END_CNF");
              end_call_ptr->call_state = CM_CALL_STATE_IDLE;
              end_call_ptr->call_subst.idle = CM_CALL_IDLE_SUBST_ENDWAIT;
            }
            else
            {
              /* need not wait for the call end cnf */
              cm_call_id_deallocate (cm_mc_rpt_ptr->incoming_call.call_id);
            }
          }
          break;
        }
        else
        #endif  //FEATURE_MMODE_SC_SVLTE
        if (cmcall_is_1x_data_srv_opt(cm_mc_rpt_ptr->incoming_call.so))
        {
          if ( data_call_id == CM_CALL_ID_INVALID &&
            !svd_supported
            )
          {
            /* End the incoming data call, bcos there are calls
            ** originating other than DATA.
            */
            CM_MSG_HIGH_0("End the Data incoming call");
            /* Get MC command buffer and indicate the
            ** appropriate MC command.
            */
            mc_ptr = cm_mc_get_buf_else_err_fatal();
            mc_ptr->hdr.cmd = MC_END_F;
            mc_ptr->end.num_call_ids = 1;
            mc_ptr->end.call_ids[0] = cm_mc_rpt_ptr->incoming_call.call_id;
            mc_ptr->end.end_reasons[0] = CAI_REL_NORMAL;

            cm_mc_send_cmd( mc_ptr );

            cm_call_id_deallocate (cm_mc_rpt_ptr->incoming_call.call_id);
            break; /* case CM_INCOMING_CALL_F: */
          }
          else if(data_call_id != CM_CALL_ID_INVALID)
          {
            cmcall_s_type *end_call_ptr = cmcall_ptr(data_call_id);
            if (end_call_ptr != NULL)
            {
              CM_MSG_HIGH_1("End MO data call on MT data call, call id = %d",
                data_call_id);

              /* Send call end cmd to LL
              */
              cmxcall_send_end_cmd(end_call_ptr, CAI_REL_NORMAL);
              end_call_ptr->end_status = CM_CALL_END_INCOM_CALL;

              /* If not waiting for end_call_cnf, release the call.
              */
              if ( end_call_ptr->call_state != CM_CALL_STATE_IDLE ||
                end_call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
                )
              {
                /* End call on CM side
                */
                cmcall_end(end_call_ptr);
              }
            }
          }
        }

        #ifdef FEATURE_JCDMA
        /* if Incoming Call is PD Call, end the data call */
        if (( cm_mc_rpt_ptr->incoming_call.so == CAI_SO_RS1_PD ) ||
            ( cm_mc_rpt_ptr->incoming_call.so == CAI_SO_RS2_PD ))
        {
          if (data_call_id != CM_CALL_ID_INVALID)
          {
            cmcall_s_type *end_call_ptr = cmcall_ptr(data_call_id);
            if (end_call_ptr != NULL)
            {
              CM_MSG_HIGH_1("End data call on PD SO35/36 call, call id = %d",
                           data_call_id);

              /* Send call end cmd to LL
              */
              cmxcall_send_end_cmd(end_call_ptr, CAI_REL_NORMAL);
              end_call_ptr->end_status = CM_CALL_END_INCOM_CALL;

              /* If not waiting for end_call_cnf, release the call.
              */
              if ( end_call_ptr->call_state != CM_CALL_STATE_IDLE ||
                   end_call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
                 )
              {
                /* End call on CM side */
                cmcall_end(end_call_ptr);
              }
            }
          }
        }
        #endif
        if(cmph_is_msim() &&(( !cmph_is_dsda() &&((cmcall_is_there_a_call_type(CM_SS_HYBR_2,CM_CALL_TYPE_VOICE,
            cm_mc_rpt_ptr->incoming_call.call_id) != CM_CALL_ID_INVALID)||
	  (cmcall_is_there_a_call_type(CM_SS_HYBR_3,CM_CALL_TYPE_VOICE,
		  cm_mc_rpt_ptr->incoming_call.call_id) != CM_CALL_ID_INVALID))
           )|| cmss_ptr()->ue_init_ps_data_suspend == CM_SS_MAIN)&&
            (is_pkt_data_srv_opt(cm_mc_rpt_ptr->incoming_call.so, CMXCALL_MOBILE_TERMINATED)))
        {
           CM_MSG_HIGH_0("End the 1X data incoming call, as voice call or SMS on other subs");
            /* Get MC command buffer and indicate the
            ** appropriate MC command.
            */
            mc_ptr = cm_mc_get_buf_else_err_fatal();
            mc_ptr->hdr.cmd = MC_END_F;
            mc_ptr->end.num_call_ids = 1;
            mc_ptr->end.call_ids[0] = cm_mc_rpt_ptr->incoming_call.call_id;
            mc_ptr->end.end_reasons[0] = CAI_REL_NORMAL;

            cm_mc_send_cmd( mc_ptr );

            cm_call_id_deallocate (cm_mc_rpt_ptr->incoming_call.call_id);
            break;
        }

        /* We are in ORIG state, and incoming call is OTAPA
        */
        if (is_otapa_srv_opt(cm_mc_rpt_ptr->incoming_call.so))
        {
          if ( data_call_id == CM_CALL_ID_INVALID &&
               !svd_supported
             )
          {
            #if !defined(FEATURE_MMODE_REMOVE_1X)
            /* End the incoming OTAPA call, bcos there are calls
            ** originating other than DATA.
            */
            CM_MSG_HIGH_0("End the OTAPA incoming call");
            /* Get MC command buffer and indicate the
            ** appropriate MC command.
            */
            mc_ptr = cm_mc_get_buf_else_err_fatal();
            mc_ptr->hdr.cmd = MC_END_F;
            mc_ptr->end.num_call_ids = 1;
            mc_ptr->end.call_ids[0] = cm_mc_rpt_ptr->incoming_call.call_id;
            mc_ptr->end.end_reasons[0] = CAI_REL_NORMAL;

            cm_mc_send_cmd( mc_ptr );
            #else
            #error code not present
#endif /* !defined(FEATURE_MMODE_REMOVE_1X)*/
            cm_call_id_deallocate (cm_mc_rpt_ptr->incoming_call.call_id);
            break; /* case CM_INCOMING_CALL_F: */
          }
          else if(data_call_id != CM_CALL_ID_INVALID)
          {
            cmcall_s_type *end_call_ptr = cmcall_ptr(data_call_id);
            if (end_call_ptr != NULL)
            {
              CM_MSG_HIGH_1("End data call on OTAPA call, call id = %d",
                           data_call_id);

              /* Send call end cmd to LL
              */
              cmxcall_send_end_cmd(end_call_ptr, CAI_REL_NORMAL);
              end_call_ptr->end_status = CM_CALL_END_INCOM_CALL;

              /* If not waiting for end_call_cnf, release the call.
              */
              if ( end_call_ptr->call_state != CM_CALL_STATE_IDLE ||
                   end_call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
                 )
              {
                /* End call on CM side
                */
                cmcall_end(end_call_ptr);
              }
            }
          }
        }
      }

      /* Check for any on-going MO calls when CM has receives an "alert with info"
      ** message for INCOMING MT call. For AWIMs, the call id for MO and MT calls
      ** are same. In this case, modem simply replaces the MO call info with MT
      ** call info and sends connects the MT call. With the new change, CM will
      ** send a CALL_END_EVENT for the MO call before sending the events for MT
      ** call. This is only a CALL_END_EVENT sent to clients, the actual call
      ** object is not de-allocated instead it is re-populated with MT call
      ** info in the subsequent steps
      */
      if ( cm_mc_rpt_ptr->hdr.cmd == CM_INCOMING_CALL_F &&
           cmxcall_is_voice_srv_opt( cm_mc_rpt_ptr->incoming_call.so ) )
      {
        cmcall_s_type    *ongoing_voice_call_ptr = NULL;
        cm_call_id_type   ongoing_voice_call_id  = cmxcall_is_there_a_ongoing_1x_voice_call();

        /* Read the NV item to check if the Call End Event is to be sent */

        if ( ongoing_voice_call_id != CM_CALL_ID_INVALID &&
             ph_ptr->is_awim_mo_call_end_event )
        {
          ongoing_voice_call_ptr = cmcall_ptr( ongoing_voice_call_id );

          if ( ongoing_voice_call_ptr != NULL )
          {
            if ( ongoing_voice_call_id == cm_mc_rpt_ptr->incoming_call.call_id )
            {
              /*Set the call end status to INCOM_CALL so AP can recognize 
              ** the reason for MO Call end 
              */ 
              ongoing_voice_call_ptr->end_status = CM_CALL_END_INCOM_CALL;

              cmcall_event( ongoing_voice_call_ptr, CM_CALL_EVENT_END );

              /* Reset the end status to NONE once dummy event is sent
              ** because the same call object will be used for MT Call 
              ** and leaving the end status intact might have side-effects 
              */ 
              ongoing_voice_call_ptr->end_status = CM_CALL_END_NONE;
            }
            else
            {
              ongoing_voice_call_ptr->end_status = CM_CALL_END_INCOMPATIBLE;
              cmcall_end( ongoing_voice_call_ptr );
            }
          }
        }
      }

        /*If sub2 is active & DDS then reject incoming datacall on 1X*/
        if( cmph_is_msim() &&
             (is_pkt_data_srv_opt(cm_mc_rpt_ptr->incoming_call.so, CMXCALL_MOBILE_TERMINATED))
             && (ph_ptr->active_subs & SYS_MODEM_AS_ID_2_MASK)
             &&(ph_ptr->default_data_subs == SYS_MODEM_AS_ID_2)
           )
        {
           CM_MSG_HIGH_0("End the 1X data incoming call as it is on Non-DDS sub");
            /* Get MC command buffer and indicate the
            ** appropriate MC command.
            */
            mc_ptr = cm_mc_get_buf_else_err_fatal();
            mc_ptr->hdr.cmd = MC_END_F;
            mc_ptr->end.num_call_ids = 1;
            mc_ptr->end.call_ids[0] = cm_mc_rpt_ptr->incoming_call.call_id;
            mc_ptr->end.end_reasons[0] = CAI_REL_NORMAL;

            cm_mc_send_cmd( mc_ptr );

            cm_call_id_deallocate (cm_mc_rpt_ptr->incoming_call.call_id);
            break;
        }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        if((cm_mc_rpt_ptr->hdr.cmd == CM_INCOMING_CALL_F)&&
            (cmcall_is_there_a_call_type(CM_SS_MAX,CM_CALL_TYPE_EMERGENCY,
            CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID)&&
            (cmxcall_is_voice_srv_opt(cm_mc_rpt_ptr->incoming_call.so)))
        {
           CM_MSG_HIGH_0("End the 1X  incoming call voice call as emergency call in progress");
            /* Get MC command buffer and indicate the
            ** appropriate MC command.
            */
            mc_ptr = cm_mc_get_buf_else_err_fatal();
            mc_ptr->hdr.cmd = MC_END_F;
            mc_ptr->end.num_call_ids = 1;
            mc_ptr->end.call_ids[0] = cm_mc_rpt_ptr->incoming_call.call_id;
            mc_ptr->end.end_reasons[0] = CAI_REL_NORMAL;

            cm_mc_send_cmd( mc_ptr );

            cm_call_id_deallocate (cm_mc_rpt_ptr->incoming_call.call_id);
            break;
        }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the service option and
      ** decide on the call type accordingly.
      */
      call_mode_info_ptr->srv_opt             = cm_mc_rpt_ptr->incoming_call.so;

      /* Extrace con_ref info
      */
      call_mode_info_ptr->con_ref             = cm_mc_rpt_ptr->incoming_call.con_ref;
      CM_MSG_MED_1("incoming.con_ref=%d",call_mode_info_ptr->con_ref);

    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      call_mode_info_ptr->alert_ans_bypass    =
        cm_mc_rpt_ptr->incoming_call.alert_ans_bypass;
      #endif  /*#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))*/

      /* Only after the call to cmxcall_determine_call_type() the call_type
      ** field will have valid value in it
      */
      cmxcall_determine_call_type( call_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Check if the call is voice call or voice call collision in SV mode,
      ** if so then block it.There are no emergency MT calls, the check added
      ** is a safety check, in case the MT call comes with call type emergency.
      */
      if( (( CMPH_SS_DISABLE_CALL_TYPE_MASK(call_ptr->asubs_id, ph_ptr)& CM_CALL_TYPE_VOICE_MASK ) &&
          ( call_ptr->call_type == CM_CALL_TYPE_VOICE ||
              call_ptr->call_type == CM_CALL_TYPE_EMERGENCY ))
                                                ||
           (( CMPH_SS_DISABLE_CALL_TYPE_MASK(call_ptr->asubs_id, ph_ptr) & CM_CALL_TYPE_CS_DATA_MASK ) &&
            ( call_ptr->call_type == CM_CALL_TYPE_CS_DATA ))
                                                ||
            ( (cmss_is_hybr2_operational()
#if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
                || cmsoa_state_info_ptr()->curr_sv_oprt_mode == CMSOA_SV_OPRT_MODE_SVLTE
#endif
            )&&
           (call_ptr->call_type == CM_CALL_TYPE_VOICE || call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) &&
           (cmcall_is_there_volte_call_obj(
                                        CM_CALL_TYPE_VOICE_MASK |
                                        CM_CALL_TYPE_EMERG_MASK |
                                        CM_CALL_TYPE_VT_MASK) != CM_CALL_ID_INVALID))
        )
      {
            #if !defined(FEATURE_MMODE_REMOVE_1X)
            CM_MSG_HIGH_1("The call collision or call type %d not supported", call_ptr->call_type);
            /* Get MC command buffer and indicate the
            ** appropriate MC command.
            */
            mc_ptr = cm_mc_get_buf_else_err_fatal();
            mc_ptr->hdr.cmd = MC_END_F;
            mc_ptr->end.num_call_ids = 1;
            mc_ptr->end.call_ids[0] = cm_mc_rpt_ptr->incoming_call.call_id;
            mc_ptr->end.end_reasons[0] = CAI_REL_NORMAL;

            cm_mc_send_cmd( mc_ptr );
            #else
            #error code not present
#endif

            cm_call_id_deallocate (cm_mc_rpt_ptr->incoming_call.call_id);
            break;
      }

      /* Indicate that this is a CDMA call.
      */
      call_ptr->srv_type = CM_SRV_TYPE_CDMA_SPECIFIC;

      #ifdef FEATURE_HDR_HYBRID
      if (call_mode_info_ptr->srv_opt == CAI_SO_HDR_PKT_DATA)
      {
        /* Indicate that this is a HDR call.
        */

        call_ptr->srv_type = CM_SRV_TYPE_HDR;

        /* this is a hybrid HDR call.
        */
        call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_HDR;
      }
      else
      #endif
      {
        /* this is not a HDR call.
        */
        call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_CDMA;

	/* If the LTE to DO redirection flag is set , clear it on receiving incoming page on 1x
	**  for the clients to know the true service status.
	*/
	if(ph_ptr->lte_do_redir)
	{
	  cmph_reset_lte_do_redir();
	  if(ss_ptr->main_buffer_srv_uptime != CMSS_INFORM_SRV_CHANGE_INTER_NONE)
	  {
	    cmss_update_cdma_srv_lte_do_redir();
	  }
        }

      }


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Update the call state, send MO SMS to MC if SMS state is in the retry state
      ** tell phone about call start
      ** and notify clients of event.
      */
      #ifdef FEATURE_CDSMS
      /* Send MO SMS if SMS state is in the retry state */
      cmxsms_mo_msg_retry();
      #endif  /* FEATURE_CDSMS */

      call_ptr->call_state = CM_CALL_STATE_INCOM;

      /* Set the call direction */
      cmcall_set_call_direction (call_ptr, CM_CALL_DIRECTION_MT);

      call_ptr->call_subst.incom = (cm_call_incom_subst_e_type)
        ((call_ptr->call_type == CM_CALL_TYPE_OTAPA) ?
            CM_CALL_INCOM_SUBST_OTAPA : CM_CALL_INCOM_SUBST_REGULAR);

      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;

      #ifdef FEATURE_IS2000_REL_A
      //call_ptr->emergency_flash_call_id = cm_mc_rpt_ptr->incoming_call.call_id;
      //call_ptr->emergency_flash_over_incoming_call = 0;
      #endif /* FEATURE_IS2000_REL_A */

      CM_MSG_MED_1("incom subst: %d", call_ptr->call_subst.incom);

      if ( cm_mc_rpt_ptr->hdr.cmd == CM_HDR_INCOMING_CALL_F )
      {
        call_ptr->sys_mode = SYS_SYS_MODE_HDR;
      }
      else
      {
        if (ss_ptr->info.sys_mode != SYS_SYS_MODE_NO_SRV)
        {
          call_ptr->sys_mode = ss_ptr->info.sys_mode;
        }
      }

      #ifdef FEATURE_GPSONE

      /* Tell phone about call start.
      */
      cmph_call_start( call_ptr );

      /* If call is to be ended clean-up and return
      */

      if( call_ptr->pd_status == CMCALL_PD_STATUS_END_ORIG )
      {
        cmcall_end( call_ptr );
        break;
      }
      #endif
      /* make sure signal info is set to false. signal info
         should come from CM_SIGNAL_F */
      call_ptr->signal.is_signal_info_avail  = FALSE;
      cmph_update_ddtm_status(ph_ptr);               /* update DDTM status */

      #ifdef FEATURE_MMGSDI_CDMA
      cmcall_update_gsdi_and_log_call_state(CM_SS_MAIN);
      #endif /* FEATURE_MMGSDI_CDMA */

      #ifdef FEATURE_CALL_THROTTLE
      /* If we have a incoming 1x data call, reset the throttle table entry if
      ** it is present
      */
      if( call_ptr->call_type == CM_CALL_TYPE_PS_DATA &&
          cm_mc_rpt_ptr->hdr.cmd == CM_INCOMING_CALL_F )
      {
        cm_ss_orig_thr_call_status_s_type thr_call_status;

        thr_call_status.call_type   = call_ptr->call_type;
        thr_call_status.call_status = CM_SS_ORIG_THR_DS_PPP_UP_SUCCESS;
        thr_call_status.mode_info.cdma_info
                                    = ss_ptr->info.mode_info.cdma_info;

        if( call_ptr->sys_mode == SYS_SYS_MODE_HDR
             &&
            ss_ptr->info.hdr_hybrid

           )
        {
          thr_call_status.sys_id = ss_ptr->info.hdr_sys_id;
          thr_call_status.mode_info.cdma_info.band_class = ss_ptr->info.hybrid_active_band;
        }
        else
        {
          thr_call_status.sys_id = ss_ptr->info.sys_id;
          thr_call_status.mode_info.cdma_info.band_class = ss_ptr->info.active_band;
        }
        /* We are ignoring NID, so replace it with the wild card nid
        */

        if( thr_call_status.sys_id.id_type == SYS_SYS_ID_TYPE_IS95 )
        {
          thr_call_status.sys_id.id.is95.nid  = SD_WILDCARD_NID;
        }

        (void) cmss_orig_thr_update_tbl_entry_with_call_status(
                                              ss_ptr->cmss_orig_thr_tbl_ptr,
                                              &thr_call_status);

      }/* if( call_ptr->call_type == CM_CALL_TYPE_PS_DATA ) */
      #endif /* FEATURE_CALL_THROTTLE */

      /* Push the origination top pointer again, with this call_id
      */
      (void)cmph_update_orig_param_incoming_call(call_ptr);

      cmcall_event( call_ptr, CM_CALL_EVENT_INCOM );

      /* Automatically answer the call.
      */
      if (call_ptr->call_type == CM_CALL_TYPE_OTAPA)
      {
        call_mode_info_ptr->otasp_info.otasp_call = CMCALL_PC_OTAPA;
        cmcall_auto_answer(call_ptr);
      }

      cmxcall_out_of_order_msg_proc( cm_mc_rpt_ptr->incoming_call.call_id,
                                     &out_of_order_msgs);


      /* If we are in SVLTE mode, check if LTE is OOS on HYBR_2 and if the clients
      ** have been informed yet. If not, expire the hysteresis timer so that client
      ** are informed of srv lost on LTE immedietely
      */
      CM_MSG_HIGH_0( "Check if No_Srv Timer be expired for MT Call");
      cmxcall_expire_hybr_2_no_srv_timer_during_cdma_call( call_ptr );

      break;  /* case CM_INCOMING_CALL_F: */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------*/
                     /*      CDMA Incoming page      */
                     /*------------------------------*/

    case CM_INCOMING_PAGE_F:

      #ifdef FEATURE_MMODE_DUAL_SIM
      if (cmph_is_msim() && cmxcall_is_voice_srv_opt(cm_mc_rpt_ptr->incoming_page.so) &&
          cm_mc_rpt_ptr->incoming_page.subs_capability != cmmsc_get_curr_msc_max_cap(SYS_MODEM_AS_ID_1))
      {
        CM_MSG_HIGH_2("Subs capability mismatch in MMODE 0x%x annd 1x 0x%x",
          cmmsc_get_curr_msc_max_cap(SYS_MODEM_AS_ID_1), cm_mc_rpt_ptr->incoming_page.subs_capability);
        cmxcall_mc_incoming_page_proceed(FALSE);
      }
      else 
      #endif
      if(cmcall_is_there_vowlan_call() != CM_CALL_ID_INVALID)
      {
        /*If other service options are also required for rejection, please add here*/
        if (cmxcall_is_voice_srv_opt(cm_mc_rpt_ptr->incoming_page.so))
        {
          CM_MSG_HIGH_0("VoWLAN call existing. 1X MT Voice page rejected");
          cmxcall_mc_incoming_page_proceed(FALSE);
        }
      }
      #ifdef FEATURE_MMODE_SC_SVLTE

     /* If there is VoLTE call existing, 1X call should be rejected
       */
      else if (cmcall_is_there_volte_call_obj(CM_CALL_TYPE_VOICE_MASK |
                                       CM_CALL_TYPE_EMERG_MASK |
                                       CM_CALL_TYPE_VT_MASK) != CM_CALL_ID_INVALID)
      {
        if (cm_mc_rpt_ptr->incoming_page.so == CAI_SO_VOICE_EVRC)
        {
          CM_MSG_HIGH_0("VoLTE call existing. 1X MT page rejected");
          cmxcall_mc_incoming_page_proceed(FALSE);
        }
      }
      else if( cmss_is_hybr2_operational() == FALSE && cmss_is_hybr_status_on() == FALSE)
      {
        /* If  SVDO & SVLTE is not supported, Immediately respond to 1xcp to
        ** proceed with call
        */
        CM_MSG_HIGH_1("SO:Page proceed cmss_is_hybr2_operational %d",
                   cmss_is_hybr2_operational());
        cmxcall_mc_incoming_page_proceed(TRUE);
      }
      else if( cmcall_is_1x_data_srv_opt(cm_mc_rpt_ptr->incoming_page.so) )
      {
        /* if we're waiting on LPM operation mode - reject page
        ** since 1X is moving to LPM anyways
        */
        if( cmxcall_is_waiting_lpm_oprt_mode_reply() )
        {
          CM_MSG_HIGH_0("SO33/32: Reject incoming page due to LPM");
          cmxcall_mc_incoming_page_proceed(FALSE);
        }
        /* If there's already an existing call, perhaps in orig state, we shall
        ** reject this SO33/32 call. This is avoid the race condition when MO call
        ** has not reached 1X-CP while MT page has arrived (MO/MT calls collision).
        */
        else if (cmxcall_is_end_suspend_status() != CM_CALL_ID_INVALID)
        {
          CM_MSG_HIGH_0("SO:MO call in progress.page reject");
          cmxcall_mc_incoming_page_proceed(FALSE);
        }
        /* If SO is SO 33 & SVDO or SVLTE is supported, inform DS about it.
        ** DS will take decision about whether to proceed with call or not
        */
        else if(  cm_mc_rpt_ptr->incoming_page.so == CAI_SO_PPP_PKT_DATA_3G )
        {
          ph_ptr->is_1x_await_response= TRUE;
          cmcall_event_2( NULL,
                          CM_CALL_EVENT_INCOMING_PAGE,
                          cm_mc_rpt_ptr);
        }
        /* If SO is SO 32 & SVDO/SVLTE is supported, disable hybrid op.
        ** Once that is confirmed, inform 1xcp to proceed
        */
        else if( cm_mc_rpt_ptr->incoming_page.so == CAI_SO_FULL_TDSO)
        {
          /* If HYBR2 is operational , send suspend to HYBR2 and update Cmph to
          ** indicate that CM is waiting for MMOC response
          */
          CM_MSG_HIGH_0("SO: TDSO Page proceed");
          ph_ptr->is_1x_await_response = TRUE;
          cmxcall_page_proceed();
        }
      }
      /* If service option is not SO 33 or SO 32 , dont send event to clients.
      ** Immediately respond to 1xcp to proceed with call
      */
      else
      {
        CM_MSG_HIGH_2("SO:Page proceed -so %d is_tau_needed %d",
                     cm_mc_rpt_ptr->incoming_page.so,cmph_ptr()->is_tau_needed);
        #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
        /* Set the flag so that ue mode switch is not triggerred.
        */
        ph_ptr->is_1x_mt_call_pending = TRUE;

        /* while operating in 1XSRLTE mode
        ** CM just need to notify NAS via ESR that a voice
        ** call is started in 1XSRLTE mode.
        ** However ESR not needed if it is already sent once for
        ** another Call is which is active now
        ** In the response to ESR ,irrecpective of ESR status
        ** cmxcall_mc_incoming_page_proceed should be called with TRUE
        */
        if(cmph_is_oprting_in_1xsrlte_mode(SYS_MODEM_AS_ID_1)
          && cm_is_esr_allowed_in_srlte()
          && cmss_ptr()->info.gw_srv_status == SYS_SRV_STATUS_SRV
          && cmregprx_get_stack_info_ptr(SD_SS_HYBR_2)->state == CMREGPRX_STATE_ACTIVE
          && cmph_ptr()->is_tau_needed == FALSE)
        {
          CM_MSG_HIGH_0("SRLTE: send ESR to NAS ");
          /* use CM_MT_UNALLOCATED_CALL_ID , since no
          ** call-id is created yet , and CM cannot create it
          ** since 1x will create before 1x sends CM_INCOMING_CALL_F .
          ** CM_MT_UNALLOCATED_CALL_ID will be used to identify that it is
          ** an MT call while processing MM_CM_1XCSFB_CALL_RSP
          */
          cmcsfbcall_send_orig_req_call_obj_not_allocated(CM_MT_UNALLOCATED_CALL_ID);
          cmph_ptr()->is_tau_needed = TRUE;
        }
        else
        #endif
        {
          cmxcall_mc_incoming_page_proceed(TRUE);
        }

      }
      #endif //FEATURE_MMODE_SC_SVLTE
      break;  /* case CM_INCOMING_PAGE_F: */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------*/
                     /*      CDMA Call end cnf       */
                     /*------------------------------*/

    /* Call End confirmation from 1X:
    ** - send call end event if there's a pending call end event
    ** - deallocate the call object
    ** - unsuspend the stack
    **
    */
    case CM_CALL_END_CNF_F:

    {
      #ifdef FEATURE_MMODE_SC_SVLTE
      cm_iterator_type  call_itr;

      cmcall_obj_iterator_init(&call_itr);

      call_ptr = cmcall_obj_get_next(&call_itr);

      /* Ending the call with substate 'end-wait' */
      while(call_ptr != NULL)
      {
        if ( call_ptr->call_id != CM_CALL_ID_INVALID &&
             cmcall_is_1x_data_srv_opt(call_ptr->cmcall_mode_info.info.cdma_info.srv_opt) &&
             call_ptr->call_state           == CM_CALL_STATE_IDLE &&
             call_ptr->call_subst.idle      == CM_CALL_IDLE_SUBST_ENDWAIT
           )
        {
          if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_CDMA )
          {
          cmcall_end( call_ptr );
        }
          else if (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE) /* MT SO33 call was ended because of MO Voice call */
          {
            /* Deallocate the call object
            */
            cm_call_id_deallocate( cmcall_call_id_get( call_ptr ) );
            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
            /* If there are no PS so 33/32 calls active and no pending 1x incoming call
            ** end suspend status
            */
            cmxcall_unsuspend_hybrid_operation();
          }
        }
        call_ptr = cmcall_obj_get_next(&call_itr);
      }
      #endif

      #if defined (FEATURE_LTE_TO_1X)
      cmcsfbcall_unforce_csfb_call_end_preferences();
      #endif

      break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------*/
                     /*      CDMA incoming page fail */
                     /*------------------------------*/

    case CM_INCOMING_PAGE_FAIL_F:
      #ifdef FEATURE_MMODE_SC_SVLTE

#if  defined(FEATURE_LTE_TO_1X)
      if (cm_is_esr_allowed_in_srlte()
          && cmph_is_oprting_in_1xsrlte_mode(SYS_MODEM_AS_ID_1))
      {
        cm_call_id_type cdma_call_id = CM_CALL_ID_INVALID;

          /* Deallocate the call object
          */
        cdma_call_id = cmcall_is_there_a_cdma_call();
        CM_MSG_HIGH_2("INCOMING_PAGE_FAIL:%d is_tau_needed %d",
                      cdma_call_id,cmph_ptr()->is_tau_needed);
        if (cdma_call_id == CM_CALL_ID_INVALID
            && cmph_ptr()->is_tau_needed == TRUE)
        {
          cmph_ptr()->is_tau_needed = FALSE;
          /* send TAU here , since we will not wait for any response from 1X
          */
          cmcsfbcall_send_mm_dual_rx_1xcsfb_tau_req();
        }
      }
#endif

      /* Reset the flag indicating there is no pending mt call */
      cmph_ptr()->is_1x_mt_call_pending = FALSE;

      /* Reset the flag indicating that 1x is waiting for a response
      */
      ph_ptr->is_1x_await_response = FALSE;

      /* If there are no PS so 33/32 calls active, end suspend status
      ** Send indication to MMOC to renable hybrid op
      */
      cmxcall_unsuspend_hybrid_operation();

      #endif //FEATURE_MMODE_SC_SVLTE
      break;  /* case CM_INCOMING_PAGE_FAIL_F: */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

               /*-----------------------------------------*/
               /*  Incoming call end due to alert stop    */
               /*-----------------------------------------*/

    case CM_STOP_ALERT_F:
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->stop.call_id))
      {
        CM_ERR_1( "STOP_ALERT_F: Unallocated Call Id %d.",
          cm_mc_rpt_ptr->stop.call_id);
        break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cm_mc_rpt_ptr->stop.call_id );

      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        break;
      }
#ifdef FEATURE_HDR_HYBRID
      if( CMCALL_IS_HDR_CALL_ORIG_OR_HDR_CALL(call_ptr) )
      {
        /* This report can cause call state to change
        and Mobile should not receive this MC report while in HDR call orig or
        HDR call so we need to make sure we discard this message under the
        above condition*/
        CM_MSG_HIGH_0("Discard CM_STOP_ALERT_F");
        break;
      }
#endif /* FEATURE_HDR_HYBRID */
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MMODE_SC_SVLTE

      /* To handle race condition where 1xcp sends this rpt to CM
      ** before it processes the call end request sent by CM.
      ** The orig failure should be handled in the same way as a
      ** CM_CALL_END_CNF_F and the call should be cleaned up.
      ** The end status of the call as reported to clients will be the
      ** cause that made CM send the end cmd to 1xcp and will not reflect
      ** the orig fail reason sent by 1xcp.
      */
      if(cmxcall_end_1x_data_call_in_subst_endwait(cm_mc_rpt_ptr->stop.call_id))
      {
        call_ptr = NULL;
        break;
      }
#endif /* FEATURE_MMODE_SC_SVLTE */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      if(( call_ptr->direction == CM_CALL_DIRECTION_NONE)&& (call_ptr->call_state ==CM_CALL_STATE_IDLE ) )
      {

        switch(cm_mc_rpt_ptr->stop.rpt_type)
        {
        case CM_CALL_RELEASE_F:
          cmcall_set_rel_code(call_ptr, cm_mc_rpt_ptr->stop.error_code.rel_error_code);
          send_page_fail_event = TRUE;
          break;
        case CM_CALL_FADE_F:
          cmcall_set_fade_code(call_ptr, cm_mc_rpt_ptr->stop.error_code.fade_error_code);
          send_page_fail_event = TRUE;
          break;
        default:
          send_page_fail_event = FALSE;
          break;
        }
      }
      if(send_page_fail_event == TRUE)
      {
        call_info_ptr = cmcall_info_alloc();
        if( call_info_ptr != NULL )
        {
          call_info_ptr->mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
          call_info_ptr->end_status = call_ptr->end_status;
        call_info_ptr->mode_info.info.cdma_call.srv_opt = (uint16) cm_mc_rpt_ptr->stop.srv_opt;
        if(call_info_ptr->call_type == CM_CALL_TYPE_NONE)
        {
          call_info_ptr->call_type = cmxcall_determine_call_type_from_so((uint16)cm_mc_rpt_ptr->stop.srv_opt);
        }
        CM_MSG_HIGH_1( "mc_rpt_proc updating call type to =%d",call_info_ptr->call_type);
          cmclient_list_call_event_ntfy(CM_CALL_EVENT_MT_CALL_PAGE_FAIL,call_info_ptr);
          cm_mem_free(call_info_ptr);

        }
        /* deallocate call id */
        cm_call_id_deallocate(cm_mc_rpt_ptr->stop.call_id);
        call_ptr = NULL;
      }
      else
      {


        /* If call is in the incoming state, end the call and
        ** notify the clients.
        */
        if( call_ptr->call_state ==  CM_CALL_STATE_INCOM ||
          call_ptr->call_state ==  CM_CALL_STATE_IDLE
          )
        {
          call_ptr->end_status = CM_CALL_END_ALERT_STOP;
          cmxcall_set_call_wait_for_1X_end_cnf(call_ptr);
          if ( call_ptr->call_state != CM_CALL_STATE_IDLE ||
            call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
            )
          {
            cmcall_end( call_ptr );
            /*  after ending the call, set the call ptr to Null, as memory
            **  allocated to call ptr would be released.
            */
            call_ptr = NULL;
          }

        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Else, this command is an error.
        */
        else { CM_ERR_0( "CM_STOP_ALERT_F" " else make no sense" ); }
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                  /*----------------------------------*/
                  /* Origination end due to intercept */
                  /*----------------------------------*/

    case CM_INTERCEPT_F:
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->inter.call_id))
      {
        CM_ERR_1( "INTERCEPT: Unallocated call id %d.", cm_mc_rpt_ptr->inter.call_id);
        break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cm_mc_rpt_ptr->inter.call_id );

      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        break;
      }
      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

      #ifdef FEATURE_HDR_HYBRID
      if( CMCALL_IS_HDR_CALL_ORIG_OR_HDR_CALL(call_ptr ) )
      {
        /* This report can affect the current call orig
           and Mobile should not receive this MC report while in HDR call orig or
           HDR call so we need to make sure we discard this message under the
           above condition*/
        CM_MSG_HIGH_0("Discard CM_INTERCEPT_F");
        break;
      }
      #endif  /* FEATURE_HDR_HYBRID */
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_MMODE_SC_SVLTE

      /* To handle race condition where 1xcp sends this rpt to CM
      ** before it processes the call end request sent by CM.
      ** The orig failure should be handled in the same way as a
      ** CM_CALL_END_CNF_F and the call should be cleaned up.
      ** The end status of the call as reported to clients will be the
      ** cause that made CM send the end cmd to 1xcp and will not reflect
      ** the orig fail reason sent by 1xcp.
      */
      if(cmxcall_end_1x_data_call_in_subst_endwait(cm_mc_rpt_ptr->inter.call_id))
      {
        call_ptr = NULL;
        break;
      }
      #endif /* FEATURE_MMODE_SC_SVLTE */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we are trying to originate, indicate an INTERCEPT
      ** end status and call the origination function to decide
      ** whether to continue or abort the origination process.
      */

    if(call_ptr->cmcall_mode_info.info.cdma_info.srv_opt == CM_SRV_OPT_NONE)
    {
      call_ptr->cmcall_mode_info.info.cdma_info.srv_opt = cm_mc_rpt_ptr->inter.srv_opt;
      if(call_ptr->call_type == CM_CALL_TYPE_NONE)
      {
        call_ptr->cmcall_mode_info.info.cdma_info.is_so_to_call_type_req = TRUE;
      }
    }


      switch( call_ptr->call_state)
      {

        case CM_CALL_STATE_IDLE:

          /* End the call.
          */
          CM_MSG_HIGH_1 ("INTERCEPT_F received in Idle State: call id %d deallocated",
                        cm_mc_rpt_ptr->inter.call_id);
          call_ptr->end_status = CM_CALL_END_INTERCEPT;
          call_ptr->prot_end_status=call_ptr->end_status ;
          cmxcall_set_call_wait_for_1X_end_cnf(call_ptr);
          if ( call_ptr->call_state != CM_CALL_STATE_IDLE ||
               call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
             )
          {
            cmcall_end(call_ptr);
            /*  after ending the call, set the call ptr to Null, as memory
            **  allocated to call ptr would be released.
            */
            call_ptr = NULL;
          }



          break;

        case  CM_CALL_STATE_ORIG:

          call_ptr->end_status = CM_CALL_END_INTERCEPT;
          call_ptr->prot_end_status=call_ptr->end_status ;
	      cmxcall_e911_orig_fail_ind(call_ptr);
          cmcall_orig_proc( call_ptr );
          break;

        default:

          /* this command is an error.*/
          CM_ERR_1( "CM_INTERCEPT_F, call_state=%d",
                           (uint8) call_ptr->call_state);
          break;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

             /*----------------------------------------------*/
             /*  Call end due to release from base station  */
             /*----------------------------------------------*/

    case CM_CALL_RELEASE_F:
    #ifdef FEATURE_HDR_HYBRID
    case CM_HDR_CALL_RELEASE_F:
    #endif  /* FEATURE_HDR_HYBRID */
      /* Decide on the action according to the call state.
      */
      for ( i = 0; i < cm_mc_rpt_ptr->release.num_call_ids; i++ )
      {
        if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->release.call_ids[i]))
        {
          CM_ERR_1( "RELEASE: Unallocated call id %d.", cm_mc_rpt_ptr->release.call_ids[i]);
          continue;
        }
        /* Get the pointer to call object */
        call_ptr = cmcall_ptr( cm_mc_rpt_ptr->release.call_ids[i] );

        if(call_ptr == NULL)
        {
          CM_MSG_HIGH_0( "call_ptr_NULL");
          continue;
        }
        call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      CM_MSG_HIGH_1( "mc_rpt_proc srv_opt from mc = %d",cm_mc_rpt_ptr->release.srv_opt[i]);
      if(call_ptr->cmcall_mode_info.info.cdma_info.srv_opt == CM_SRV_OPT_NONE)
      {
        call_ptr->cmcall_mode_info.info.cdma_info.srv_opt = cm_mc_rpt_ptr->release.srv_opt[i];
        if(call_ptr->call_type == CM_CALL_TYPE_NONE)
        {
          call_ptr->cmcall_mode_info.info.cdma_info.is_so_to_call_type_req = TRUE;
        }
      }
        #ifdef FEATURE_HDR_HYBRID
        if( cm_mc_rpt_ptr->hdr.cmd == CM_CALL_RELEASE_F  &&
                  CMCALL_IS_HDR_CALL_ORIG_OR_HDR_CALL(call_ptr)  )
        {
          /* rcv MC release but mobile is in HDR call orig or call
            so discard this report */
          CM_MSG_HIGH_0("Discard MC Release-HdrCall");
          continue;
        }
        else  if( cm_mc_rpt_ptr->hdr.cmd == CM_HDR_CALL_RELEASE_F  &&
                  CMCALL_IS_NON_HDR_CALL_ORIG_OR_NON_HDR_CALL(call_ptr))  /*lint !e731 */
        {
          /* rcv HDR release but mobile is in Non HDR call orig or call
            so discard this report */
          CM_MSG_HIGH_0("Discard HDR Release-NonHdrCall");
          continue;
        }


        #endif /* FEATURE_HDR_HYBRID */


        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
        #error code not present
#endif

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        switch( call_ptr->call_state )
        {

          case CM_CALL_STATE_IDLE:

            /* Perhaps waiting for call_end cnf, end the call.
            */
            cmcall_set_rel_code(call_ptr, cm_mc_rpt_ptr->release.rel_codes[i]);
            cmcall_end( call_ptr );

            /*  after ending the call, set the call ptr to Null, as memory
            **  allocated to call ptr would be released.
            */
            call_ptr = NULL;

            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case CM_CALL_STATE_ORIG:

            /* If we are trying to originate, indicate a RELEASE
            ** end status and call the origination function to decide
            ** whether to continue or abort the origination process.
            */


            cmcall_set_rel_code(call_ptr, cm_mc_rpt_ptr->release.rel_codes[i]);
	        cmxcall_e911_orig_fail_ind(call_ptr);
            cmcall_orig_proc( call_ptr );
            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case CM_CALL_STATE_INCOM:

            /* Call was never established -
            ** simply end the call.
            */
            cmcall_set_rel_code(call_ptr, cm_mc_rpt_ptr->release.rel_codes[i]);
            cmxcall_set_call_wait_for_1X_end_cnf(call_ptr);
            if ( call_ptr->call_state != CM_CALL_STATE_IDLE ||
                 call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
               )
            {
              cmcall_end( call_ptr );
              /*  after ending the call, set the call ptr to Null, as memory
              **  allocated to call ptr would be released.
              */
              call_ptr = NULL;
            }

            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case CM_CALL_STATE_CONV:


            CM_MSG_HIGH_1("Reselect release- call type=%d",
                        call_ptr->call_type);

            cmcall_set_rel_code(call_ptr, cm_mc_rpt_ptr->release.rel_codes[i]);

            cmxcall_set_call_wait_for_1X_end_cnf(call_ptr);
            if ( call_ptr->call_state != CM_CALL_STATE_IDLE ||
                 call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
               )
            {
              cmcall_end(call_ptr);
              /*  after ending the call, set the call ptr to Null, as memory
              **  allocated to call ptr would be released.
              */
              call_ptr = NULL;
            }

            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          default:

            CM_ERR_1( "CM_CALL_RELEASE_F, call_state=%d",
                           (uint8) call_ptr->call_state);
            break;

        }
      } /* for ( i = 0; i < cm_mc_rpt_ptr->call_release.num_end_call_ids; i++ ) */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*--------------------------------*/
                   /*     Call end due to a fade     */
                   /*--------------------------------*/

    case CM_CALL_FADE_F:
#ifdef FEATURE_HDR_HYBRID
    case CM_HDR_CALL_FADE_F:
#endif  /* FEATURE_HDR_HYBRID */
      for ( i = 0; i < cm_mc_rpt_ptr->fade.num_call_ids; i++ )
      {
        /* If call id is not assigned, just skip the processing */
        if ( cm_mc_rpt_ptr->fade.call_ids[i] == CM_CALL_ID_UNASSIGNED )
        {
          continue;
        }

        if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->fade.call_ids[i]))
        {
          CM_ERR_1( "FADE: Call Id %d not allocated", cm_mc_rpt_ptr->fade.call_ids[i]);
          continue;
        }

        /* Get the pointer to call object */
        call_ptr = cmcall_ptr( cm_mc_rpt_ptr->fade.call_ids[i] );
        if(call_ptr == NULL)
        {
          CM_MSG_HIGH_0( "call_ptr_NULL");
          continue;
        }

      CM_MSG_HIGH_1( "mc_rpt_proc srv_opt from mc = %d",cm_mc_rpt_ptr->fade.srv_opt);
      if(call_ptr->cmcall_mode_info.info.cdma_info.srv_opt == CM_SRV_OPT_NONE)
      {
        call_ptr->cmcall_mode_info.info.cdma_info.srv_opt = cm_mc_rpt_ptr->fade.srv_opt[i];
        if(call_ptr->call_type == CM_CALL_TYPE_NONE)
        {
          call_ptr->cmcall_mode_info.info.cdma_info.is_so_to_call_type_req = TRUE;
        }
      }

        call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

        if(( call_ptr->direction == CM_CALL_DIRECTION_NONE)&& (call_ptr->call_state ==CM_CALL_STATE_IDLE ) )
        {
          cmcall_set_fade_code(call_ptr, cm_mc_rpt_ptr->fade.call_end_fade_reason[i]);
          call_info_ptr = cmcall_info_alloc();
          if( call_info_ptr != NULL )
          {
            call_info_ptr->mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
            call_info_ptr->end_status = call_ptr->end_status;
          call_info_ptr->mode_info.info.cdma_call.srv_opt = (uint16) cm_mc_rpt_ptr->fade.srv_opt[i];
          if(call_info_ptr->call_type == CM_CALL_TYPE_NONE)
          {
            call_info_ptr->call_type = cmxcall_determine_call_type_from_so((uint16)cm_mc_rpt_ptr->fade.srv_opt[i]);
          }
          CM_MSG_HIGH_2( "mc_rpt_proc updating srv_opt from mc = %d  , call_type =%d",cm_mc_rpt_ptr->fade.srv_opt[i],call_info_ptr->call_type);
            cmclient_list_call_event_ntfy(CM_CALL_EVENT_MT_CALL_PAGE_FAIL,call_info_ptr);
            cm_mem_free(call_info_ptr);

          }
          /* deallocate call id */
          cm_call_id_deallocate(cm_mc_rpt_ptr->fade.call_ids[i]);
          call_ptr = NULL;
          continue;
        }

#ifdef FEATURE_IS2000_REL_A
        /* CM can only receive this command from MC on the Traffic Channel, so we can be
        sure that an L2 ack has been received.  It is safe to set the layer to L3. */
        call_ptr->sr.orig_fail_layer = CM_CALL_ORIG_ERR_LAYER_L3;
        CM_MSG_HIGH_1("call orig fail layer = %d",call_ptr->sr.orig_fail_layer);
#endif /* FEATURE_IS2000_REL_A */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR_HYBRID
        if( cm_mc_rpt_ptr->hdr.cmd == CM_CALL_FADE_F  &&
          CMCALL_IS_HDR_CALL_ORIG_OR_HDR_CALL(call_ptr) )
        {
          /* rcv MC fade but mobile is in HDR call orig or call
          so discard this report */
          CM_MSG_HIGH_0("Discard MC Fade-HdrCall");
          continue;
        }
        else  if( cm_mc_rpt_ptr->hdr.cmd == CM_HDR_CALL_FADE_F  &&
          CMCALL_IS_NON_HDR_CALL_ORIG_OR_NON_HDR_CALL(call_ptr))    /*lint !e731 */
        {
          /* rcv HDR fade but mobile is in Non HDR call orig or call
          so discard this report */
          CM_MSG_HIGH_0("Discard HDR Fade-NonHdrCall");
          continue;
        }
#endif /* FEATURE_HDR_HYBRID */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
        #error code not present
#endif

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        switch( call_ptr->call_state )
        {

          case CM_CALL_STATE_IDLE:
            /* Waiting for end_cnf, end the call.
            */
            cmcall_set_fade_code(call_ptr, cm_mc_rpt_ptr->fade.call_end_fade_reason[i]);
            call_ptr->prot_end_status=call_ptr->end_status ;
            cmcall_end(call_ptr);

            /*  after ending the call, set the call ptr to Null, as memory
            **  allocated to call ptr would be released.
            */
            call_ptr = NULL;

            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case CM_CALL_STATE_ORIG:

            /* If we are trying to originate, indicate a FADE
            ** end status and call the origination function to decide
            ** whether to continue or abort the origination process.
            ** Distinguish between HDR and 1x FADE , so that there can be
            ** different action.
            */
            if( cm_mc_rpt_ptr->hdr.cmd == CM_CALL_FADE_F)
            {
              cmcall_set_fade_code(call_ptr, cm_mc_rpt_ptr->fade.call_end_fade_reason[i]);
              call_ptr->prot_end_status=call_ptr->end_status ;
            }
            else
            {
              call_ptr->end_status = CM_CALL_END_HDR_FADE;
            }
	        cmxcall_e911_orig_fail_ind(call_ptr);
            cmcall_orig_proc( call_ptr );
            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


          case CM_CALL_STATE_INCOM:

            /* We are alerting - simply end the call.
            */
             cmcall_set_fade_code(call_ptr, cm_mc_rpt_ptr->fade.call_end_fade_reason[i]);
             call_ptr->prot_end_status=call_ptr->end_status ;
             cmxcall_set_call_wait_for_1X_end_cnf(call_ptr);
            if ( call_ptr->call_state != CM_CALL_STATE_IDLE ||
                 call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
               )
            {
              cmcall_end( call_ptr );
              /*  after ending the call, set the call ptr to Null, as memory
              **  allocated to call ptr would be released.
              */
              call_ptr = NULL;
            }

            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case CM_CALL_STATE_CONV:

            cmcall_set_fade_code(call_ptr, cm_mc_rpt_ptr->fade.call_end_fade_reason[i]);
            call_ptr->prot_end_status=call_ptr->end_status ;
            cmxcall_set_call_wait_for_1X_end_cnf(call_ptr);
            if ( call_ptr->call_state != CM_CALL_STATE_IDLE ||
                 call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
               )
            {
              cmcall_end(call_ptr);
              /*  after ending the call, set the call ptr to Null, as memory
              **  allocated to call ptr would be released.
              */
              call_ptr = NULL;
            }

            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          default:

            CM_ERR_1( "CM_CALL_FADE_F, call_state=%d",
                           (uint8) call_ptr->call_state);
            break;

        } /* switch( call_ptr->call_state ) */

      } /* for ( i = 0; i < cm_mc_rpt_ptr->fade.num_end_call_ids; i++ ) */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*--------------------------------*/
                   /*         CDMA Lock order        */
                   /*      End any call in progress  */
                   /*--------------------------------*/

    case CM_LOCK_F:

      /* If call is NOT in the idle state,
      ** end it now.
      */

      for ( i = 0; i < CMCALL_MAX_CALL_OBJ; i++ )
      {
        /* Get the pointer to call object */
        if (!cm_call_id_is_allocated ((cm_call_id_type) i))
        {
          CM_MSG_HIGH_1( "LOCK: Call Id %d not allocated", i);
          continue;
        }

        call_ptr = cmcall_ptr( (cm_call_id_type) i );
        if(call_ptr == NULL)
        {
          CM_MSG_HIGH_0( "call_ptr_NULL");
          continue;
        }


        call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

        #ifdef FEATURE_HDR_HYBRID
        if( CMCALL_IS_HDR_CALL_ORIG_OR_HDR_CALL(call_ptr ) )
        {
          /* This report can change the call state
             and Mobile should not receive this MC report while in HDR call orig or
             HDR call so we need to make sure we discard this message under the
             above condition*/
          CM_MSG_HIGH_0("Discard CM_LOCK_F");
          continue;
        }
        #endif  /* FEATURE_HDR_HYBRID */

      if( cm_mc_rpt_ptr->lock.call_id == (cm_call_id_type) i)
      {

        CM_MSG_HIGH_1( "mc_rpt_proc srv_opt from mc = %d",cm_mc_rpt_ptr->lock.srv_opt);
        if(call_ptr->cmcall_mode_info.info.cdma_info.srv_opt == CM_SRV_OPT_NONE)
        {
          call_ptr->cmcall_mode_info.info.cdma_info.srv_opt = cm_mc_rpt_ptr->lock.srv_opt;
          if(call_ptr->call_type == CM_CALL_TYPE_NONE)
          {
            call_ptr->cmcall_mode_info.info.cdma_info.is_so_to_call_type_req = TRUE;
          }
        }
      }

        switch( call_ptr->call_state )
        {

          case CM_CALL_STATE_IDLE:
          case CM_CALL_STATE_CC_IN_PROGRESS:
            CM_MSG_HIGH_1("Lock-Idle: Call Id %d deallocated", i);
            call_ptr->end_status = CM_CALL_END_CDMA_LOCK;
            call_ptr->prot_end_status=call_ptr->end_status ;
            cmcall_end( call_ptr );

            /*  after ending the call, set the call ptr to Null, as memory
            **  allocated to call ptr would be released.
            */
            call_ptr = NULL;

            break;

          case CM_CALL_STATE_ORIG:
          case CM_CALL_STATE_INCOM:
          case CM_CALL_STATE_CONV:
            call_ptr->end_status = CM_CALL_END_CDMA_LOCK;
            call_ptr->prot_end_status=call_ptr->end_status ;
            cmxcall_set_call_wait_for_1X_end_cnf(call_ptr);
            if ( call_ptr->call_state != CM_CALL_STATE_IDLE ||
                 call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
               )
            {
              cmcall_end( call_ptr );
              /*  after ending the call, set the call ptr to Null, as memory
              **  allocated to call ptr would be released.
              */
              call_ptr = NULL;
            }
            break;

          default:
            CM_ERR_1( "Call state=%d", (uint8) call_ptr->call_state);
        } /* switch( call_ptr->call_state ) */
      } /*       for ( i = 0; i < CM_CALL_ID_MAX; i++ ) */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*--------------------------------*/
                   /*       CDMA Un-lock order       */
                   /*--------------------------------*/

    /* CDMA unlock.
    */
    case CM_UNLOCK_F:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*--------------------------------*/
                   /*      Origination Failed        */
                   /*--------------------------------*/

    case CM_CALL_ORIG_FAIL_F:
    #ifdef FEATURE_HDR_HYBRID
    case CM_HDR_CALL_ORIG_FAIL_F:
    #endif  /* FEATURE_HDR_HYBRID */
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->orig_fail.call_id))
      {
        CM_ERR_1( "ORIG FAIL: Call Id %d not allocated", cm_mc_rpt_ptr->orig_fail.call_id);
        break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cm_mc_rpt_ptr->orig_fail.call_id );

      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        break;
      }
      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_MMODE_SC_SVLTE
      /* To handle race condition where 1xcp sends orig failure to CM
      ** before it processes the call end request sent by CM.
      ** The orig failure should be handled in the same way as a
      ** CM_CALL_END_CNF_F and the call should be cleaned up.
      ** The end status of the call as reported to clients will be the
      ** cause that made CM send the end cmd to 1xcp and will not reflect
      ** the orig fail reason sent by 1xcp.
      */
      if(cmxcall_end_1x_data_call_in_subst_endwait(cm_mc_rpt_ptr->orig_fail.call_id))
      {
        call_ptr = NULL;
        break;
      }
      #endif /* FEATURE_MMODE_SC_SVLTE */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      if( call_ptr->call_state != CM_CALL_STATE_ORIG )
      {
        /* If call state is not origination, discard this report */
        CM_MSG_HIGH_0("Discard OrigFail-wrongState");
        break;
      }
      else if( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE)
      {
        /* If origination command was not sent to MC, discard this report. */
        CM_MSG_HIGH_0("Discard OrigFail-OrigNotSent");
        break;
      }
      #ifdef FEATURE_HDR_HYBRID
      else if(cm_mc_rpt_ptr->hdr.cmd == CM_CALL_ORIG_FAIL_F &&
              CMCALL_IS_HDR_CALL_ORIG(call_ptr )
             )
      {
        /* rcv MC orig fail but the current call orig is a hybrid HDR call orig
           so discard the reort */
        CM_MSG_HIGH_0("Discard MC OrigFail- HdrOrig");
        break;
      }
      else if(cm_mc_rpt_ptr->hdr.cmd == CM_HDR_CALL_ORIG_FAIL_F &&
              CMCALL_IS_NON_HDR_CALL_ORIG(call_ptr) )               /*lint !e731*/
      {
        /* rcv HDR orig fail but the current call orig is not a hybrid
           HDR call orig so discard the reort */
        CM_MSG_HIGH_0("Discard HDR OrigFail- NonHdrOrig");
        break;
      }
      #endif /* FEATURE_HDR_HYBRID */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_IS2000_REL_A
      /* Set the flag to indicate whether it is L2 failure or not
      */
      call_ptr->sr.orig_fail_layer = cm_mc_rpt_ptr->orig_fail.layer;
      #endif /* FEATURE_IS2000_REL_A */

      /* Set highest layer origination failure for call object
      */
      cmxcall_set_highest_layer_orig_failure(call_ptr, cm_mc_rpt_ptr->orig_fail.layer);

    CM_MSG_HIGH_1( "mc_rpt_proc srv_opt from mc = %d",cm_mc_rpt_ptr->orig_fail.srv_opt);
    if(call_ptr->cmcall_mode_info.info.cdma_info.srv_opt == CM_SRV_OPT_NONE)
    {
      call_ptr->cmcall_mode_info.info.cdma_info.srv_opt = cm_mc_rpt_ptr->orig_fail.srv_opt;
      if(call_ptr->call_type == CM_CALL_TYPE_NONE)
      {
        call_ptr->cmcall_mode_info.info.cdma_info.is_so_to_call_type_req = TRUE;
      }
    }

      switch( cm_mc_rpt_ptr->orig_fail.code )
      {
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_ORIG_ERR_HOLD_ORIG:

          /* MC discarded the origination because it is temporarily in a
          ** state where it can not process originations, such as idle
          ** handoff.
          ** Hold the origination and let the cmcall_orig_proc() function
          ** to resend the origination on the next 1s boundary.
          */
          call_ptr->end_status      = CM_CALL_END_NONE;
          call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_NONE;

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_ORIG_ERR_NO_SRV:

          /* MC discarded the origination due to No service
          ** let the origination function to decide
          ** whether to continue or abort the origination process.
          */
          if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_HDR)
          {
            call_ptr->end_status = CM_CALL_END_NO_HYBR_HDR_SRV;
          }
          else
          {
            call_ptr->end_status = CM_CALL_END_NO_CDMA_SRV;
          }

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_ORIG_ERR_CDMA_PSIST_NG:

          call_ptr->end_status   = CM_CALL_END_PSIST_NG;
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_ORIG_ERR_ACC_FAIL:

          /* MC discarded the origination due to system access failure (e.g.,
          ** CDMA max access probes, CDMA access denied, CDMA access state
          ** timeout, AMPS overload message, etc.)
          */
          if( cm_mc_rpt_ptr->hdr.cmd == CM_CALL_ORIG_FAIL_F)
          {
            call_ptr->end_status = CM_CALL_END_ACC_FAIL;
            call_ptr->prot_end_status=call_ptr->end_status ;
          }
          else
          {
            call_ptr->end_status = CM_CALL_END_HDR_ACC_FAIL;

          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_ORIG_ERR_ABORT:

          /* MC discarded the origination because it is not able to process
          ** it in the current state (e.g. MC is init state).
          */
          call_ptr->end_status = CM_CALL_END_MC_ABORT;
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_ORIG_ACC_IN_PROG:

          /* MC discarded the origination because access is already in
          ** progress (i.e., MC is in the system access state).
          */
          call_ptr->end_status = CM_CALL_END_ACC_IN_PROG;
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        #ifdef FEATURE_HDR_HYBRID
        case CM_CALL_ORIG_NO_SESSION:

          /* HDR reports No session to CM, set the call end status to
          ** no session
          */

          call_ptr->end_status = CM_CALL_END_HDR_NO_SESSION;
          break;
        #endif  /* FEATURE_HDR_HYBRID */
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_ORIG_ERR_REDIR:
        case CM_CALL_ORIG_ERR_HANDOFF:

          /* MC discarded the origination due to redirection/handoff.
          */
          call_ptr->end_status = CM_CALL_END_REDIR_OR_HANDOFF;
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_ORIG_ERR_CDMA_LOCK:

          /* MC discarded the origination due to MS being CDMA locked until
          ** power-cycle.
          */
          call_ptr->end_status = CM_CALL_END_CDMA_LOCK;
          call_ptr->prot_end_status=call_ptr->end_status ;
          break;


        case CM_CALL_ORIG_ERR_TIMEOUT:
          if( cm_mc_rpt_ptr->hdr.cmd == CM_CALL_ORIG_FAIL_F)
          {
            /* MC report CDMA orig fail due to time out */
            call_ptr->end_status = CM_CALL_END_FADE;
            call_ptr->prot_end_status=call_ptr->end_status ;
          }
          /* for HDR, the command will be like CM_HDR_CALL_ORIG_FAIL_F */
          else
          {
            /* MC discarded the origination due to HDR connection setup timeout
            */
            call_ptr->end_status = CM_CALL_END_HDR_CS_TIMEOUT;
          }
          break;
        case CM_CALL_ORIG_ERR_TIMEOUT_T42:
          call_ptr->end_status = CM_CALL_END_TIMEOUT_T42;
          call_ptr->prot_end_status=call_ptr->end_status ;
          break;
        case CM_CALL_ORIG_ERR_TIMEOUT_T40:
          call_ptr->end_status = CM_CALL_END_TIMEOUT_T40;
          call_ptr->prot_end_status=call_ptr->end_status ;
          break;
    case CM_CALL_ORIG_ERR_TRM_REQ_FAIL:
          call_ptr->end_status = CM_CALL_END_TRM_REQ_FAIL;
      call_ptr->prot_end_status=call_ptr->end_status ;
      break;
    case CM_CALL_ORIG_ERR_TIMEOUT_T41:
          call_ptr->end_status = CM_CALL_END_TIMEOUT_T41;
      call_ptr->prot_end_status=call_ptr->end_status ;
      break;
        case CM_CALL_ORIG_ERR_ACC_FAIL_REJ_ORD:
          call_ptr->end_status = CM_CALL_END_ACC_FAIL_REJ_ORD;
          call_ptr->prot_end_status=call_ptr->end_status ;
          break;
        case CM_CALL_ORIG_ERR_ACC_FAIL_RETRY_ORD:
          call_ptr->end_status = CM_CALL_END_ACC_FAIL_RETRY_ORD;
          call_ptr->prot_end_status=call_ptr->end_status ;
          break;
#ifdef FEATURE_HDR
        case CM_CALL_ORIG_ERR_TCA_TIMEOUT:
        case CM_CALL_ORIG_ERR_RTCACK_TIMEOUT:
          /* MC discarded the origination due to HDR connection setup timeout
          */
          call_ptr->end_status = CM_CALL_END_HDR_CS_TIMEOUT;

          break;
#endif  /* FEATURE_HDR */

        case CM_CALL_ORIG_ERR_INCOMPATIBLE:
          /* Concurrent services requested were not compatible */
          call_ptr->end_status = CM_CALL_END_INCOMPATIBLE;
          #ifdef  FEATURE_VOIP_E911_SUPPORT
          /* If voip is available at time of making the call,
          ** the call should be retried over voip now
          */
          if(call_ptr->sr.voip_sr.e911_voip_redial ==
                                                  CMCALL_VOIP_REDIAL_PENDING)
          {
            call_ptr->end_status = CM_CALL_END_IP_RETRY;
          }
          #endif

          break;

        case CM_CALL_ORIG_ERR_CCS_NOT_SUPPORTED:
          /* Concurrent services are not supported by the MS or the BS */
          call_ptr->end_status = CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS;
          #ifdef  FEATURE_VOIP_E911_SUPPORT
          /* If voip is available at time of making the call,
          ** the call should be retried over voip now
          */
          if(call_ptr->sr.voip_sr.e911_voip_redial ==
                                                  CMCALL_VOIP_REDIAL_PENDING)
          {
            call_ptr->end_status = CM_CALL_END_IP_RETRY;
          }
          #endif

          break;

        case CM_CALL_ORIG_ERR_REJECTED_BY_BS:
          /* BS rejects MS's 2nd call orig for other reason */
          call_ptr->end_status = CM_CALL_END_REJECTED_BY_BS;
          #ifdef  FEATURE_VOIP_E911_SUPPORT
          /* If voip is available at time of making the call,
          ** the call should be retried over voip now
          */
          if(call_ptr->sr.voip_sr.e911_voip_redial ==
                                                  CMCALL_VOIP_REDIAL_PENDING)
          {
            call_ptr->end_status = CM_CALL_END_IP_RETRY;
          }
          #endif

          break;

        case CM_CALL_ORIG_ERR_NO_RESPONSE_FROM_BS:
          /* 2nd call orig is not responded by the BS in 12 seconds */

          call_ptr->end_status = CM_CALL_END_NO_RESPONSE_FROM_BS;
          break; /* CM_CALL_ORIG_ERR_NO_RESPONSE_FROM_BS */

        #ifdef FEATURE_IS2000_REL_A

        case CM_CALL_ORIG_ERR_ACCT_BLOCK:

          /* Origination blocked by the BS */
           call_ptr->end_status = CM_CALL_END_ACCESS_BLOCK;
           break;

        case CM_CALL_ORIG_ERR_ALL_ACCT_BLOCK:

          /* Origination blocked by the BS  for all mobiles, KDDI specific */
          call_ptr->end_status = CM_CALL_END_ACCESS_BLOCK_ALL;
          break;

        #endif /* FEATURE_IS2000_REL_A */

        case CM_CALL_ORIG_ERR_ALREADY_IN_TC:
          /* There already is a traffic channel... Do not retry */
          call_ptr->end_status = CM_CALL_END_ALREADY_IN_TC;
          #ifdef  FEATURE_VOIP_E911_SUPPORT
          /* If voip is available at time of making the call,
          ** the call should be retried over voip now
          */
          if(call_ptr->sr.voip_sr.e911_voip_redial ==
                                                   CMCALL_VOIP_REDIAL_PENDING)
          {
            call_ptr->end_status = CM_CALL_END_IP_RETRY;
          }
          #endif

          break;

        case CM_CALL_ORIG_ERR_HDR_NO_LOCK_GRANTED:
           call_ptr->end_status = CM_CALL_END_HDR_NO_LOCK_GRANTED;
           break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_ORIG_ERR_REORDER:

          /* Origination end due to reorder
          */
          call_ptr->end_status = CM_CALL_END_REORDER;
          call_ptr->prot_end_status=call_ptr->end_status ;
          break;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_ORIG_ERR_MAX_ACC:

          /* Origination end due to max access probes
          ** IS707B_MAX_ACC for IS-707B calls,
          ** MAX_ACCESS_PROBE, otherwise
          */
          if (call_ptr->is_707b_ps_data == TRUE)
          {
            call_ptr->end_status = CM_CALL_END_IS707B_MAX_ACC;
          }
          else
          {
            call_ptr->end_status = CM_CALL_END_MAX_ACCESS_PROBE;
            call_ptr->prot_end_status=call_ptr->end_status ;
             #if defined (FEATURE_LTE_TO_1X) 
             /* For optimized redial during 1xCSFB MAPE call failure remove LTE 
             ** while forcing the preference to SD. This is to avoid UE camping
             ** on LTE instead of looking for another 1x system
             */
              if(call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB )
              {
                call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                    call_ptr->favored_mode_pref,
                                    1,
                                    SD_SS_MODE_PREF_LTE);
                CM_MSG_HIGH_1("1xCSFB: Removing LTE from mode pref during optimized redial %d", 
                  call_ptr->favored_mode_pref);
              }
              #endif
          }
          break;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_ORIG_ERR_CALL_COLLISION:

          /* MC discarded the origination because MT call setup is
          ** in progress when MO E911 origination was received
          */
          call_ptr->end_status = CM_CALL_END_CALL_COLLISION;
          break;

        default:
          call_ptr->end_status = CM_CALL_END_REL_NORMAL;
          CM_ERR_1( "Orig fail code=%d",(uint8) cm_mc_rpt_ptr->orig_fail.code);
          break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      prev_orig_mode = call_ptr->sr.orig_sent_on_mode;
      /* If call-end status is not none, call on the origination processing
      ** function to process the call origination failure.
      */
      if( call_ptr->end_status != CM_CALL_END_NONE )
      {
          /* Inform Clients about the e911 origination failure at 1x.*/
	      cmxcall_e911_orig_fail_ind(call_ptr);

          cmcall_orig_proc( call_ptr );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* VOIP redial over PS origination failure.
      */

      if( prev_orig_mode == SYS_SYS_MODE_HDR )
      {
        cmxcall_voip_redial_over_ps_orig_failure( CMCALL_PS_FAILURE_ORIG,
                                              CM_CD_MAX,
                                              cm_mc_rpt_ptr->orig_fail.code);
      }

      break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     #ifdef FEATURE_OTASP

                 /*-----------------------------------------*/
                 /*           OTASP        status           */
                 /*  Indicate status of OTASP/OTAPA call    */
                 /*-----------------------------------------*/

    case CM_OTASP_STATUS_F:

      CM_MSG_HIGH_1("otasp_status %d",cm_mc_rpt_ptr->otasp_status.otasp_code);
      call_ptr = cmcall_get_otasp_call_obj();

      if(call_ptr != NULL)
      {
        call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

        /* Notify clients of otasp status communicated by 1x cp
        ** Possible status values in cm_otasp_status_e_type
        */
        call_mode_info_ptr->otasp_info.otasp_status =
                                      cm_mc_rpt_ptr->otasp_status.otasp_code;

        cmcall_event( call_ptr, CM_CALL_EVENT_OTASP_STATUS);
      }
      else
      {
        CM_MSG_HIGH_1("Ignoring OTASP status %d- No OTASP/OTAPA/1x call found",
                               cm_mc_rpt_ptr->otasp_status.otasp_code);
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                 /*-----------------------------------------*/
                 /*           OTASP commit status           */
                 /*  Indicate whether commit is in progress */
                 /*-----------------------------------------*/

    case CM_OTASP_COMMIT_STATUS_F:
      { /* Beginning of otasp commit block */

        int               num_call_id;
        cm_call_id_type   call_ids[CM_CALL_ID_MAX];

        /* Get the call_ids that are applicable */
        if ( !cmxcall_otasp_commit_call_ids( &num_call_id, call_ids ) )
        {
           break;
        }

        for ( i = 0; i < num_call_id; i++ )
        {
      /* Get the pointer to call object */
          if ( (call_ptr = cmcall_ptr( call_ids[i] )) == NULL )
          {
            CM_ERR_2( "BAD CALL ID %d, cmd=%d (CM_OTASP_COMMIT_STATUS_F)",
              call_ids[i], cm_mc_rpt_ptr->hdr.cmd);
            break;
          }

          call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

         if( call_ptr->call_state == CM_CALL_STATE_CONV )
         {
          if ( cm_mc_rpt_ptr->otasp_commit_status.commit_in_progress )
          {
           /* OTASP commit starts */
           if (call_ptr->call_subst.conv == CM_CALL_CONV_SUBST_REGULAR)
           {
            call_ptr->call_subst.conv = CM_CALL_CONV_SUBST_COMMIT;

            /* Set the otasp_call type for the busy OTAPA.
            */
            if (call_mode_info_ptr->otasp_info.otasp_call == CMCALL_OTASP_NONE)
            {
              call_mode_info_ptr->otasp_info.otasp_call = CMCALL_BUSY_OTAPA;
            }
           }
           else if (call_ptr->call_subst.conv == CM_CALL_CONV_SUBST_PC_OTAPA_REGULAR)
           {
            call_ptr->call_subst.conv = CM_CALL_CONV_SUBST_PC_OTAPA_COMMIT;
           }

             else { CM_ERR_0( "CM_OTASP_COMMIT_STATUS_F" " else make no sense" ); }

             CM_MSG_HIGH_2("Start commit, state transition, conv subst: %d, Set commit reply timer: %d", 
                           call_ptr->call_subst.conv, CMCALL_MAX_OTASP_COMMIT_TIME);

            /* Setup the reply timer.
            */
            call_mode_info_ptr->otasp_info.is_wait_for_commit_reply = TRUE;
               cm_commit_reply_set(&call_mode_info_ptr->otasp_info.commit_reply,
                  (cm_cmd_type *) NULL, CM_OTASP_COMMIT_STATUS_F,
                  CMCALL_MAX_OTASP_COMMIT_TIME );
          }
          else
          {
           /* OTASP commit finishes */
           if (call_ptr->call_subst.conv == CM_CALL_CONV_SUBST_COMMIT)
           {
             call_ptr->call_subst.conv = CM_CALL_CONV_SUBST_REGULAR;
           }
           else if (call_ptr->call_subst.conv == CM_CALL_CONV_SUBST_PC_OTAPA_COMMIT)
           {
            call_ptr->call_subst.conv = CM_CALL_CONV_SUBST_PC_OTAPA_REGULAR;
           }

             else { CM_ERR_0( "CM_OTASP_COMMIT_STATUS_F" " else make no sense" ); }

             CM_MSG_HIGH_1("Stop commit, state transition, conv subst: %d", 
                            call_ptr->call_subst.conv);

            call_mode_info_ptr->otasp_info.is_wait_for_commit_reply = FALSE;
            call_mode_info_ptr->otasp_info.is_commit_successful = TRUE;

           }
         }
        }
      }
      break;
      #endif /* FEATURE_OTASP */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                 /*----------------------------------*/
                 /*    Analog flash information      */
                 /*  Being used for AMPS caller ID   */
                 /*----------------------------------*/

    case CM_FLASH_F:
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->flash.call_id))
      {
         CM_ERR_1( "FLASH: Call Id %d not allocated", cm_mc_rpt_ptr->flash.call_id);
         break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cm_mc_rpt_ptr->flash.call_id );
      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        break;
      }
      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

      /* This command only makes sense if the call is in the
      ** incoming or conversation state.
      */
      if( call_ptr->call_state == CM_CALL_STATE_INCOM ||
          call_ptr->call_state == CM_CALL_STATE_CONV )
      {
        /* Extract the caller ID.
        */
        cm_num_copy( &call_ptr->num,
                     cm_mc_rpt_ptr->flash.cpn,
                     cm_mc_rpt_ptr->flash.cnt );

        /* Extract the presentation indicator
        ** and screening indicator.
        */
        CM_NUM_PI( &call_ptr->num ) = cm_mc_rpt_ptr->flash.pi;
        CM_NUM_SI( &call_ptr->num ) = cm_mc_rpt_ptr->flash.si;

        /* this is not a CDMA info rec but set it to TRUE to indicate
           that no additional record will be sent */
        call_mode_info_ptr->is_last_cdma_info_rec   = TRUE;

        /* Notify clients of event.
        */
        cmcall_event( call_ptr, CM_CALL_EVENT_CALLER_ID );
      }

      else if (call_ptr->call_state == CM_CALL_STATE_IDLE)
      {
        /* still able to rcv this msg here due to the race
           between this msg and the call end command
        */
      }
      /* Else, this command is an error.
      */
      else { CM_ERR_0( "CM_FLASH_F" " else make no sense" ); }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            /*-----------------------------------------------*/
            /* NAMPS IS91 Extended Protocol Short Message    */
            /*         received on Voice Channel             */
            /*      Being used for AMPS caller ID            */
            /*-----------------------------------------------*/

    case CM_N_VC_MSG_F:

      /* If the message arrived out of sequence, call id will be set to CM_CALL_ID_UNASSIGNED */
      if ( cm_mc_rpt_ptr->n_msg.call_id == CM_CALL_ID_UNASSIGNED )
      {
        CM_MSG_LOW_0 (" CM_N_VC_MSG_F short msg for AMPS ");
        break;
      }

      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->n_msg.call_id))
      {
        CM_ERR_1( "VC_MSG: Call Id %d not allocated", cm_mc_rpt_ptr->n_msg.call_id);
        break;
      }

      call_ptr = cmcall_ptr(cm_mc_rpt_ptr->n_msg.call_id);

      if(!call_ptr)
      {
         CM_ERR_1( "N_MSG: Can't get pointer to call object. id = %d",
                                cm_mc_rpt_ptr->n_msg.call_id);
        break;
      }


      /*
      ** Initilaize the Call type.
      */
      cmcall_set_type(call_ptr,CM_CALL_TYPE_NONE);

      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );
      switch( call_ptr->call_state )
      {

        case CM_CALL_STATE_IDLE:

          /* If the call is in idle state, only save the caller ID
          ** information into the call struct. The assumption is that
          ** a CM_ALERT_F is soon to show.
          */
          cm_num_ep_msg_copy( &call_ptr->num,
                              cm_mc_rpt_ptr->n_msg.data,
                              ARR_SIZE(cm_mc_rpt_ptr->n_msg.data) );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_STATE_ORIG:

          /* If the call is in the orig state, this command is an error.
          */
          CM_ERR_0( "");

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_STATE_INCOM:
        case CM_CALL_STATE_CONV:

          /* If the call is in the incoming or conversation state,
          ** treat this command as a caller ID event.
          */

          /* Extract the caller ID.
          */
          cm_num_ep_msg_copy( &call_ptr->num,
                              cm_mc_rpt_ptr->n_msg.data,
                              ARR_SIZE(cm_mc_rpt_ptr->n_msg.data) );

          /* this is not a CDMA info rec but set it to TRUE to indicate
             that no additional record will be sent */
          call_mode_info_ptr->is_last_cdma_info_rec   = TRUE;


          /* Notify clients of event.
          */
          cmcall_event( call_ptr, CM_CALL_EVENT_CALLER_ID );

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        default:

          CM_ERR_1( "Call state=%d",(uint8) call_ptr->call_state);

      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                 /*----------------------------------*/
                 /*      Calling Party Number        */
                 /*  Being used for CDMA caller ID   */
                 /*----------------------------------*/

    case CM_CALLING_PARTY_F:

      /*
      ** This message may have arrived out of order.
      ** If the call id is unassigned, sent the event with the unassigned call id.
      ** If the call id is valid but the call state is IDLE, buffer the event and
      ** wait for the incoming event.
      ** Otherwise send the event now.
      */
      if (( cm_mc_rpt_ptr->calling_party.call_id == CM_CALL_ID_UNASSIGNED ) ||
          (!cm_call_id_is_allocated (cm_mc_rpt_ptr->calling_party.call_id)) )
      {
        cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_CALLER_ID);
        break;
      }

      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->calling_party.call_id )) == NULL )
      {
        CM_ERR_1( "CM_CALLING_PARTY_F: Can't get pointer to call object. id = %d",
                 cm_mc_rpt_ptr->calling_party.call_id);

        cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_CALLER_ID);
        break;
      }

      if (call_ptr->call_state == CM_CALL_STATE_IDLE)
      {
        /* Save the command contents to process it later.
            The assumption is that a CM_INCOMING_CALL_F is soon to show. */

        /* Set the flags */
        out_of_order_msgs.is_calling_party_flag = TRUE;

        /* Copy the data */
        CM_MSG_LOW_0("Save CM_CALLING_PARTY_F");
        *(&(out_of_order_msgs.calling_party_buffer)) = *(&(cm_mc_rpt_ptr->calling_party));
      }
      else if (call_ptr->call_state == CM_CALL_STATE_INCOM ||
               call_ptr->call_state == CM_CALL_STATE_CONV )
      {
        /* Message did not arrive out of sequence so process it now */
        call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

        call_mode_info_ptr->is_last_cdma_info_rec = cm_mc_rpt_ptr->calling_party.hdr.last_record;

        /* Extract the caller ID
        */
        cm_num_copy( &call_ptr->num,
                     cm_mc_rpt_ptr->calling_party.chars,
                     cm_mc_rpt_ptr->calling_party.num_chars );

        /* calling party number will be stored in ASCII, confirmed with 1x team */
        call_ptr->num.digit_mode = CM_DIGIT_MODE_8BIT_ASCII;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Extract the number plan, number type,
        ** presentation indicator and screening indicator.
        */
        CM_NUM_NUMBER_TYPE( &call_ptr->num ) =
                                       cm_mc_rpt_ptr->calling_party.number_type;

        CM_NUM_NUMBER_PLAN( &call_ptr->num ) =
                                       cm_mc_rpt_ptr->calling_party.number_plan;

        CM_NUM_PI( &call_ptr->num ) = cm_mc_rpt_ptr->calling_party.pi;
        CM_NUM_SI( &call_ptr->num ) = cm_mc_rpt_ptr->calling_party.si;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /* HPCD conversion for inocmming call*/
        /* Init num conversion params */
        cm_util_init_num_conversion_params ( &num_conv_req,
                                             &num_details );

        /* Operation to be performed on num */
        num_conv_req = CMUTIL_NUM_CONV_REQ_DIGITS_TO_PLUS;

        /* Fill the number details before calling conversion */
        num_details.call_type         = call_ptr->call_type;

        num_details.current_mobile_country_code = ss_ptr->info.current_mcc;

        num_details.home_mobile_country_code  = ss_ptr->info.home_mcc;

        num_details.num             = call_ptr->num;

        num_details.hs_based_plus_dial_setting  = ss_ptr->info.plus_dial_setting;

        /* If number begins with IDD try to remove the idd */
        conver_result  = cm_util_perform_num_conversion (  num_conv_req,
                                                           &num_details,
                                                           &converted_num
                                                        );

        CM_MSG_HIGH_1 (" Result of cm_util_perform_num_conversion for incoming %d",
                     conver_result);

        if (conver_result == CMUTIL_CONVER_RESULT_NUM_MODIFIED)
        {
           CM_MSG_HIGH_0 (" Copying converted number for incoming");
           /* Copy from converted number : Because it is to remove idd, careful to
           ** exising number
           ** in the buffer.
           */
           memscpy( call_ptr->num.buf, sizeof(call_ptr->num.buf), converted_num.buf, sizeof(converted_num.buf) );
           call_ptr->num.len = converted_num.len;
           call_ptr->num.number_type = converted_num.number_type;

           CM_NUM_DIGIT_MODE (&call_ptr->num ) =
           CM_NUM_DIGIT_MODE(&converted_num);


        }
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Notify clients of event.
        */
        cmcall_event( call_ptr, CM_CALL_EVENT_CALLER_ID );
      }
      else
      {
        /* Else, this command is an error.
        */
        CM_ERR_0( "CM_CALLING_PARTY_F");
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                 /*----------------------------------*/
                 /*    Call has been established or  */
                 /*      Service option changed      */
                 /*----------------------------------*/

    case CM_CALL_CONNECTED_F:
    #ifdef FEATURE_HDR
    case CM_HDR_CALL_CONNECTED_F:
    #endif  /* FEATURE_HDR */
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->connect.call_id))
      {
        CM_ERR_1( "CONNECTED: Call Id %d not allocated", cm_mc_rpt_ptr->connect.call_id);
        break;
      }


      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->connect.call_id )) == NULL )
      {
        CM_ERR_2( "BAD CALL ID %d, cmd=%d(CM_CALL_CONNECTED_F)",
                 cm_mc_rpt_ptr->connect.call_id,
                 cm_mc_rpt_ptr->hdr.cmd);
        /* Don't deallocate call id. For markov calls and in some race
        ** conditions, we may get this message ahead of incoming.
        */
        break;
      }

      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

       if ((cm_mc_rpt_ptr->hdr.cmd == CM_CALL_CONNECTED_F) &&
           (call_ptr->call_state == CM_CALL_STATE_ORIG) &&
           ( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE ) )
       {
          /* this is due to a race between incoming OTAPA
             and origination.  We need to end the OTAPA call first
             but allow the origination process to continue. This
             connected message is for the previous cancelled
             OTAPA call.
          */
          CM_MSG_HIGH_0("Discard CM_CALL_CONNECTED_F");
          break;
       }

       #ifdef FEATURE_HDR_HYBRID
       if( cm_mc_rpt_ptr->hdr.cmd == CM_HDR_CALL_CONNECTED_F &&
           call_ptr->ignore_hdr_call_conn_rpt)
       {
         call_ptr->ignore_hdr_call_conn_rpt = FALSE;
         CM_MSG_HIGH_0("Discard CM_HDR_CALL_CONNECTED_F");
         break;
       }
       #endif /* FEATURE_HDR_HYBRID */


      /* This command only makes sense if call is NOT in the idle state.
      */
      if( call_ptr->call_state != CM_CALL_STATE_IDLE )
      {

        call_mode_info_ptr->srv_opt = cm_mc_rpt_ptr->connect.so;
        call_mode_info_ptr->con_ref = cm_mc_rpt_ptr->connect.con_ref;
        CM_MSG_MED_1("connect.con_ref=%d",call_mode_info_ptr->con_ref);

        #ifdef FEATURE_HDR_AN_AUTH
        call_ptr->an_auth_status = cm_mc_rpt_ptr->connect.an_auth_status;
        #endif  /* FEATURE_HDR_AN_AUTH */

        #ifdef FEATURE_IS2000_REL_A
        call_mode_info_ptr->qos_parms_incl = cm_mc_rpt_ptr->connect.qos_parms_incl;

        if ( call_mode_info_ptr->qos_parms_incl )
        {
          call_mode_info_ptr->qos_parms_len = memscpy( call_mode_info_ptr->qos_parms,
                                                       sizeof(call_mode_info_ptr->qos_parms),
                                                       cm_mc_rpt_ptr->connect.qos_parms,
                                                       cm_mc_rpt_ptr->connect.qos_parms_len );
        }
        #endif /* FEATURE_IS2000_REL_A */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If call is in the origination or incoming state,
        ** this is a call connect event,
        ** else this is a service option changed event.
        */
        switch( call_ptr->call_state)
        {

          case CM_CALL_STATE_ORIG:
          case CM_CALL_STATE_INCOM:

            /*  update the data-base */
            db_item.conv = TRUE;
            db_put( DB_CONV, &db_item );

            #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
            /* Generate an ANSWER event to the clients when the
            ** the call is not answered by the clients. This would
            ** ensure all the clients will receive an ANSWER event
            ** following an INCOMING CALL event.
            */

            if ( call_ptr->call_state == CM_CALL_STATE_INCOM)
            {
              if (call_mode_info_ptr->alert_ans_bypass )
              {
                call_ptr->call_client_ptr = CM_CLIENT_PTR_NONE;
                cmcall_event ( call_ptr, CM_CALL_EVENT_ANSWER );
              }
            }
            #endif /*#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

            if ( call_ptr->call_state == CM_CALL_STATE_ORIG)
            {
              cmlog_call_orig_connected( call_ptr->call_type,
                                         call_ptr->srv_type,
                                         call_ptr->cmcall_mode_info.info.cdma_info.srv_opt );
            }

            call_ptr->call_state = CM_CALL_STATE_CONV;

           #ifdef FEATURE_LTE_TO_1X
           if (call_ptr->csfb_type != CM_CALL_CSFB_TYPE_1XCSFB)
           {
            cmph_update_ddtm_status(ph_ptr);         /* update DDTM status */
           }
           #else
           cmph_update_ddtm_status(ph_ptr);         /* update DDTM status */
           #endif

            if (call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_OTAPA)
            {
              call_ptr->call_subst.conv = CM_CALL_CONV_SUBST_PC_OTAPA_REGULAR;
            }
            else
            {
              call_ptr->call_subst.conv = CM_CALL_CONV_SUBST_REGULAR;
            }

            if ( cm_mc_rpt_ptr->hdr.cmd == CM_HDR_CALL_CONNECTED_F )
            {
              call_ptr->sys_mode = SYS_SYS_MODE_HDR;
            }
            else
            {
              if (ss_ptr->info.sys_mode != SYS_SYS_MODE_NO_SRV)
              {
                call_ptr->sys_mode = ss_ptr->info.sys_mode;
              }
            }
            call_ptr->call_connect_time = time_get_uptime_secs();
            cmcall_event( call_ptr, CM_CALL_EVENT_CONNECT );

            #ifdef FEATURE_GPSONE
            /* Check if we need to force the mode for the PD session
            */
            if( !pd_ptr->is_mode_forced                                 &&
                ( pd_ptr->session_state == CMPD_SESSION_STATE_INITIATED ||
                  pd_ptr->session_state == CMPD_SESSION_STATE_GRANTED
                ))
            {
              cmpd_session_grant_proc( pd_ptr->session_type );
            }

            if( !pd_dmod_ptr->is_mode_forced                                 &&
                ( pd_dmod_ptr->session_state == CMPD_SESSION_STATE_INITIATED ||
                  pd_dmod_ptr->session_state == CMPD_SESSION_STATE_GRANTED
                ))
            {
              cmpd_session_grant_proc( pd_dmod_ptr->session_type );
            }
            #endif

            #ifdef FEATURE_MMGSDI_CDMA
            cmcall_update_gsdi_and_log_call_state(CM_SS_MAIN);
            #endif /* FEATURE_MMGSDI_CDMA */

            /* Inform CMSS that call was connected.
            */
            #if defined(FEATURE_HDR)
            cmss_proc_call_connected( call_ptr );
            #endif /* FEATURE_HDR */

            /* If we are in SVLTE mode, check if LTE is OOS on HYBR_2 and if the clients
            ** have been informed yet. If not, expire the hysteresis timer so that client
            ** are informed of srv lost on LTE immedietely
            */
            CM_MSG_HIGH_0( "Check if No_Srv Timer be expired for MO Call");
            cmxcall_expire_hybr_2_no_srv_timer_during_cdma_call( call_ptr );

            break;

          case CM_CALL_STATE_CONV:
            cmcall_event( call_ptr, CM_CALL_EVENT_SRV_OPT );
            break;

          default:
            CM_ERR_1("Bad Call State %d",call_ptr->call_state);
            break;
        } /* switch( call_ptr->call_state) */

      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Note that this command can still come in idle state due to a race
      ** condition - i.e. client ends the call just when call is connected.
      */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

               /*----------------------------------*/
               /*  CDMA/AMPS abbreviated alert    */
               /*----------------------------------*/

    case CM_ABRV_ALERT_F:

      /*
      ** This message may have arrived out of order.
      ** If the call id is unassigned, sent the event with the unassigned call id.
      ** If the call id is valid but the call state is IDLE, buffer the event and
      ** wait for the incoming event.
      ** Otherwise send the event now.
      */
      if (( cm_mc_rpt_ptr->abrv.call_id == CM_CALL_ID_UNASSIGNED ) ||
          (!cm_call_id_is_allocated (cm_mc_rpt_ptr->abrv.call_id)) )
      {
        cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_ABRV_ALERT);
        break;
      }

      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->abrv.call_id )) == NULL )
      {
        CM_ERR_2( "BAD CALL ID %d, cmd=%d(CM_ABRV_ALERT_F)",
                  cm_mc_rpt_ptr->abrv.call_id,
                  cm_mc_rpt_ptr->hdr.cmd);

        cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_ABRV_ALERT);
        break;

      }

      if ( call_ptr->call_state == CM_CALL_STATE_IDLE )
      {
        /* Remember the fact that CM_ABRV_ALERT_F arrived.
        ** The assumption is that a CM_ALERT_F or CM_INCOMING_F is soon to appear.
        */

        out_of_order_msgs.is_abrv_flag = TRUE;

        /* Copy everything that was in the message */
        CM_MSG_LOW_0(" Saving CM_ABRV_ALERT_F");
        *(&(out_of_order_msgs.abrv_buffer)) = *(&(cm_mc_rpt_ptr->abrv));
      }
      else
      {
        /* Message did not arrive out of order so process it now */
        cmcall_event( call_ptr, CM_CALL_EVENT_ABRV_ALERT );
      }

      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

               /*----------------------------------*/
               /*  AMPS abbreviated reorder       */
               /*----------------------------------*/

    case CM_ABRV_REORDER_F:
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->abrv_reorder.call_id))
      {
        CM_ERR_1( "ABRV_REORDER: Call id %d not allocated",
          cm_mc_rpt_ptr->abrv_reorder.call_id);
        break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cm_mc_rpt_ptr->abrv_reorder.call_id );
      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        break;
      }

      cmcall_event( call_ptr, CM_CALL_EVENT_ABRV_REORDER );

      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

               /*----------------------------------*/
               /*   AMPS abbreviated intercept    */
               /*----------------------------------*/

    case CM_ABRV_INTERCEPT_F:
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->abrv_intercept.call_id))
      {
        CM_ERR_1( "ABRV_INTERCEPT: Call id %d not allocated",
          cm_mc_rpt_ptr->abrv_intercept.call_id);
        break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cm_mc_rpt_ptr->abrv_intercept.call_id) ;
      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        break;
      }

      cmcall_event( call_ptr, CM_CALL_EVENT_ABRV_INTERCEPT );

      break;




    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

               /*----------------------------------*/
               /*        CDMA display info          */
               /*----------------------------------*/

    case CM_DISPLAY_F:

      /*
      ** This message may have arrived out of order.
      ** If the call id is unassigned, sent the event with the unassigned call id.
      ** If the call id is valid but the call state is IDLE, buffer the event and
      ** wait for the incoming event.
      ** Otherwise send the event now.
      */
      if (( cm_mc_rpt_ptr->display.call_id == CM_CALL_ID_UNASSIGNED ) ||
          (!cm_call_id_is_allocated (cm_mc_rpt_ptr->display.call_id)) )
      {
        cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_DISPLAY);
        break;
      }

      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->display.call_id )) == NULL )
      {
        CM_ERR_2( "BAD CALL ID %d, cmd=%d(CM_DISPLAY_F)",
                 cm_mc_rpt_ptr->display.call_id,
                 cm_mc_rpt_ptr->hdr.cmd);

        cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_DISPLAY);
        break;
      }

      if (call_ptr->call_state == CM_CALL_STATE_IDLE)
      {
        /* Save the command contents to process it later.
           The assumption is that a CM_INCOMING_CALL_F is soon to show. */

        /* Set the flags */
        out_of_order_msgs.is_display_flag = TRUE;

        /* Copy the data */
        CM_MSG_LOW_0("Save CM_DISPLAY_F");
        *(&(out_of_order_msgs.display_buffer)) = *(&(cm_mc_rpt_ptr->display));
      }
      else
      {
        /* Message did not arrive out of sequence so process it here */
        call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

        call_mode_info_ptr->is_last_cdma_info_rec = cm_mc_rpt_ptr->display.hdr.last_record;
        call_ptr->alpha.len                       = cm_mc_rpt_ptr->display.num_chars;

        for (idx = 0; idx < call_ptr->alpha.len ; idx++)
        {
          call_ptr->alpha.buf[idx] = cm_mc_rpt_ptr->display.chars[idx] ;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Notify clients of event.
        */
        cmcall_event( call_ptr, CM_CALL_EVENT_DISPLAY );
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

               /*----------------------------------*/
               /*      CDMA called party info      */
               /*----------------------------------*/

    case CM_CALLED_PARTY_F:

      /*
      ** This message may have arrived out of order.
      ** If the call id is unassigned, sent the event with the unassigned call id.
      ** If the call id is valid but the call state is IDLE, buffer the event and
      ** wait for the incoming event.
      ** Otherwise send the event now.
      */
      if (( cm_mc_rpt_ptr->called_party.call_id == CM_CALL_ID_UNASSIGNED ) ||
          (!cm_call_id_is_allocated (cm_mc_rpt_ptr->called_party.call_id)) )
      {
         cmxcall_event_generic( cm_mc_rpt_ptr, CM_CALL_EVENT_CALLED_PARTY );
         break;
      }

      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->called_party.call_id )) == NULL )
      {
        CM_ERR_1( "CALLED_PARTY: Can't get pointer to call object. id = %d",
                 cm_mc_rpt_ptr->called_party.call_id);

        cmxcall_event_generic( cm_mc_rpt_ptr, CM_CALL_EVENT_CALLED_PARTY );
        break;
      }

      if (call_ptr->call_state == CM_CALL_STATE_IDLE)
      {
        /* Save the command contents to process it later.
           The assumption is that a CM_INCOMING_CALL_F is soon to show. */

        /* Set the flags */
        out_of_order_msgs.is_called_party_flag = TRUE;

        /* Copy the data */
        CM_MSG_LOW_0("Save CM_CALLED_PARTY_F");
        *(&(out_of_order_msgs.called_party_buffer)) = *(&(cm_mc_rpt_ptr->called_party));
      }
      else
      {
        /* Message did not arrive out of sequence so process it here */
        call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

        /* Extract the called party number      */
        call_mode_info_ptr->is_last_cdma_info_rec   = cm_mc_rpt_ptr->called_party.hdr.last_record;

        cm_num_copy( &call_ptr->num,
                     cm_mc_rpt_ptr->called_party.chars,
                     cm_mc_rpt_ptr->called_party.num_chars );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Extract the number plan, number type
        */
        CM_NUM_NUMBER_TYPE( &call_ptr->num ) =
                                       cm_mc_rpt_ptr->called_party.number_type;

        CM_NUM_NUMBER_PLAN( &call_ptr->num ) =
                                       cm_mc_rpt_ptr->called_party.number_plan;


        /* Note called party info does not contain presentation and screening
           indicators information */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Notify clients of event.
        */

        cmcall_event( call_ptr, CM_CALL_EVENT_CALLED_PARTY );
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

               /*----------------------------------*/
               /*  CDMA connected number info      */
               /*----------------------------------*/

    case CM_CONNECTED_NUM_F:

      /*
      ** This message may have arrived out of order.
      ** If the call id is unassigned, sent the event with the unassigned call id.
      ** If the call id is valid but the call state is IDLE, buffer the event and
      ** wait for the incoming event.
      ** Otherwise send the event now.
      */
      if (( cm_mc_rpt_ptr->connected_num.call_id == CM_CALL_ID_UNASSIGNED ) ||
          (!cm_call_id_is_allocated (cm_mc_rpt_ptr->connected_num.call_id)))
      {
        cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_CONNECTED_NUM );
        break;
      }

      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->connected_num.call_id )) == NULL )
      {
        CM_ERR_1( "CM_CONNECTED_NUM_F: Can't get pointer to call object. id = %d",
                 cm_mc_rpt_ptr->connected_num.call_id);

        cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_CONNECTED_NUM );
        break;
      }

      if ( call_ptr->call_state == CM_CALL_STATE_IDLE )
      {
        /* Save the command contents to process it later.
           The assumption is that a CM_INCOMING_CALL_F is soon to show. */

        /* Set the flags */
        out_of_order_msgs.is_connected_num_flag = TRUE;

        /* Copy the data */
        CM_MSG_LOW_0 ("Save CM_CONNECTED_F");
        *(&(out_of_order_msgs.connected_num_buffer)) = *(&(cm_mc_rpt_ptr->connected_num));
      }
      else
      {

        /* Call Id and Call Object is valid so process it here */
        call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

        call_mode_info_ptr->is_last_cdma_info_rec   = cm_mc_rpt_ptr->connected_num.hdr.last_record;

        /* Extract the connected num      */
        cm_num_copy( &call_ptr->num,
                     cm_mc_rpt_ptr->connected_num.chars,
                     cm_mc_rpt_ptr->connected_num.num_chars );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Extract the number plan, number type,
        ** presentation indicator and screening indicator.
        */
        CM_NUM_NUMBER_TYPE( &call_ptr->num ) =
                                       cm_mc_rpt_ptr->connected_num.number_type;

        CM_NUM_NUMBER_PLAN( &call_ptr->num ) =
                                       cm_mc_rpt_ptr->connected_num.number_plan;

        CM_NUM_PI( &call_ptr->num ) = cm_mc_rpt_ptr->connected_num.pi;
        CM_NUM_SI( &call_ptr->num ) = cm_mc_rpt_ptr->connected_num.si;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Notify clients of event.
        */

        cmcall_event( call_ptr, CM_CALL_EVENT_CONNECTED_NUM );
      }

      break;

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

               /*----------------------------------*/
               /*  CDMA extended display info      */
               /*----------------------------------*/

    case CM_EXT_DISPLAY_F:

      /*
      ** This message may have arrived out of order.
      ** If the call id is unassigned, sent the event with the unassigned call id.
      ** If the call id is valid but the call state is IDLE, buffer the event and
      ** wait for the incoming event.
      ** Otherwise send the event now.
      */
      if (( cm_mc_rpt_ptr->ext_disp.call_id  == CM_CALL_ID_UNASSIGNED ) ||
          (!cm_call_id_is_allocated (cm_mc_rpt_ptr->ext_disp.call_id)) )
      {
        cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_EXT_DISP);
        break;
      }

      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->ext_disp.call_id )) == NULL )
      {
        CM_ERR_1( "CM_EXT_DISPLAY: Can't get pointer to call object. id = %d",
                                 cm_mc_rpt_ptr->ext_disp.call_id);

        cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_EXT_DISP);
        break;

      }

      if (call_ptr->call_state == CM_CALL_STATE_IDLE)
      {
        /* Message arrived out of order so save the data to be processed later.
        ** The assumption is that a CM_INCOMING_CALL_F is soon to show.
        */
        /* Set the flags */
        out_of_order_msgs.is_ext_disp_flag = TRUE;

        /* Copy the data so it's available later */

        CM_MSG_LOW_0( "Save CM_EXT_DISPLAY_F");
        *(&(out_of_order_msgs.ext_disp_buffer)) = *(&(cm_mc_rpt_ptr->ext_disp));
      }
      else
      {
        /* Message did not arrive out of sequence so process it now */
        call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

        call_mode_info_ptr->ext_disp_type           = cm_mc_rpt_ptr->ext_disp.disp_info.display;
        call_mode_info_ptr->is_last_cdma_info_rec   = cm_mc_rpt_ptr->ext_disp.hdr.last_record;
        call_ptr->alpha.len                         = cm_mc_rpt_ptr->ext_disp.disp_info.datalen;

        for (idx = 0; idx < call_ptr->alpha.len ; idx++)
        {
          call_ptr->alpha.buf[idx] = cm_mc_rpt_ptr->ext_disp.disp_info.data[idx];
        }

        /* Notify clients of event */
        cmcall_event( call_ptr, CM_CALL_EVENT_EXT_DISP);
      }
      break;

#endif /*#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

    case CM_NDSS_REDIRECTION_F:
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->ndss_redirection.call_id))
      {
        CM_ERR_1( "NDSS_REDIRECTION: Call Id %d not allocated",
          cm_mc_rpt_ptr->ndss_redirection.call_id);
        break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cm_mc_rpt_ptr->ndss_redirection.call_id );
      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        break;
      }

      #ifdef FEATURE_HDR_HYBRID
      if( CMCALL_IS_HDR_CALL_ORIG_OR_HDR_CALL(call_ptr ) )
      {
        /* This report can change the call state
           and Mobile should not receive this MC report while in HDR call orig or
           HDR call so we need to make sure we discard this message under the
           above condition*/
        CM_MSG_HIGH_0("Discard CM_NDSS_REDIRECTION_F");
        break;
      }
      #endif /* FEATURE_HDR_HYBRID */

      /* Only process this command if call state is origination or
      ** conversation and this is a user originated call.
      **
      ** For targets with System Determination 2.0 support, CM is responsible
      ** for re-sending the origination again once the ok to originate
      ** indication is coming, so do the following:
      ** 1. Change the call state to origination,
      ** 2. Inform CM clients of NDSS redirection.
      ** 3. Set the call end status to redirection.
      ** 4. Call on the origination processing function to process the call
      **    origination failure.
      */
      if( (call_ptr->call_state == CM_CALL_STATE_ORIG)
                             ||
          (call_ptr->call_state == CM_CALL_STATE_CONV &&
           call_ptr->is_user_originated_call) )
      {
        call_ptr->call_state = CM_CALL_STATE_ORIG;
        cmcall_event( call_ptr, CM_CALL_EVENT_NDSS_START );

        call_ptr->end_status = CM_CALL_END_ACC_IN_PROG;

	    cmxcall_e911_orig_fail_ind(call_ptr);

        cmcall_orig_proc( call_ptr );
      }
      else { CM_ERR_0( "CM_NDSS_REDIRECTION_F" " else make no sense" ); }
      break;

    #endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_IS2000
    case CM_CALL_FAIL_RETRY_ORDER_F:
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->retry_order.call_id))
      {
        CM_ERR_1( "FAIL_RETRY_ORDER: Call Id %d not allocated",
          cm_mc_rpt_ptr->retry_order.call_id);
        break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cm_mc_rpt_ptr->retry_order.call_id );
      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        break;
      }

      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

      if (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE)
      {
        /* Discard this message because CM did not send Orig command to MC */
        CM_ERR_0( "Unexpected CM_CALL_ORIG_FAIL_F");
        break;
      }
      #ifdef FEATURE_HDR_HYBRID
      else if( CMCALL_IS_HDR_CALL_ORIG_OR_HDR_CALL(call_ptr ) )
      {
        /* This report can change the call state
           and Mobile should not receive this MC report while in HDR call orig or
           HDR call so we need to make sure we discard this message under the
           above condition*/
        CM_MSG_HIGH_0("Discard CM_CALL_FAIL_RETRY_ORDER_F");
        break;
      }
      #endif /* FEATURE_HDR_HYBRID */

      if (call_ptr->call_state ==  CM_CALL_STATE_ORIG)
      {
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
        call_mode_info_ptr->ndss_connect     =
        call_mode_info_ptr->ndss_in_progress = FALSE;
#endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
        call_mode_info_ptr->retry_order = cm_mc_rpt_ptr->retry_order.retry_info;
        call_ptr->end_status = CM_CALL_END_RETRY_ORDER;
        cmcall_end( call_ptr );

        /*  after ending the call, set the call ptr to Null, as memory
        **  allocated to call ptr would be released.
        */
        call_ptr = NULL;

      }
      else { CM_ERR_0( "CM_CALL_FAIL_RETRY_ORDER_F" " else make no sense" ); }
      break;
#endif /* FEATURE_IS2000 */

    case CM_EXIT_TC_F:
      /* Reset the flag since 1xCP has ended the call */
      cmph_ptr()->is_1x_mt_call_pending = FALSE;

      cmxcall_srlte_send_tau(CM_CALL_ID_INVALID);

      /* Notify clients of event */
      cmxcall_event_generic (cm_mc_rpt_ptr, CM_CALL_EVENT_EXIT_TC);      

      #if defined (FEATURE_LTE_TO_1X)
      cmcsfbcall_unforce_csfb_call_end_preferences();
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    case CM_EXT_BRST_INTL_F:

      /* Notify clients of event */

      cmxcall_event_generic(cm_mc_rpt_ptr, CM_CALL_EVENT_EXT_BRST_INTL);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_NSS_CLIR_F:
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->nss_clir_rec.call_id))
      {
        CM_ERR_1( "NSS_CLIR: Call Id %d not allocated",
          cm_mc_rpt_ptr->nss_clir_rec.call_id);
        break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cm_mc_rpt_ptr->nss_clir_rec.call_id );
      CM_ASSERT( call_ptr != NULL );

      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

      call_mode_info_ptr->is_last_cdma_info_rec   = cm_mc_rpt_ptr->nss_clir_rec.hdr.last_record;
      call_mode_info_ptr->nss_clir_rec.cause      = cm_mc_rpt_ptr->nss_clir_rec.cause;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Notify clients of event.
      */

      cmcall_event( call_ptr, CM_CALL_EVENT_NSS_CLIR_REC);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_NSS_REL_F:
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->nss_rel_rec.call_id))
      {
        CM_ERR_1( "NSS_REL: Call Id %d not allocated", cm_mc_rpt_ptr->nss_rel_rec.call_id);
        break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cm_mc_rpt_ptr->nss_rel_rec.call_id );
      CM_ASSERT( call_ptr != NULL );

      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

      call_mode_info_ptr->is_last_cdma_info_rec = cm_mc_rpt_ptr->nss_rel_rec.hdr.last_record;

      /* Notify clients of event.
      */

      cmcall_event( call_ptr, CM_CALL_EVENT_NSS_REL_REC);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_NSS_AUD_CTRL_F:
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->nss_aud_ctrl_rec.call_id))
      {
         CM_ERR_1( "NSS_AUD_CTRL: Call Id %d not allocated",
           cm_mc_rpt_ptr->nss_aud_ctrl_rec.call_id);
         break;
      }

      /* Get the pointer to call object */
      call_ptr = cmcall_ptr( cm_mc_rpt_ptr->nss_aud_ctrl_rec.call_id );
      CM_ASSERT( call_ptr != NULL );

      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

      call_mode_info_ptr->nss_aud_ctrl_rec.down_link =
         cm_mc_rpt_ptr->nss_aud_ctrl_rec.down_link;
      call_mode_info_ptr->nss_aud_ctrl_rec.up_link =
         cm_mc_rpt_ptr->nss_aud_ctrl_rec.up_link;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      call_mode_info_ptr->is_last_cdma_info_rec = cm_mc_rpt_ptr->nss_aud_ctrl_rec.hdr.last_record;

      /* Notify clients of event.
      */
      cmcall_event( call_ptr, CM_CALL_EVENT_NSS_AUD_CTRL);
      break;


  #ifdef FEATURE_HDR_HYBRID

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                 /*----------------------------------------*/
                 /*  Connection Denied                     */
                 /*----------------------------------------*/

    case CM_HDR_CONNECTION_DENY_F:
      for ( i = 0; i < cm_mc_rpt_ptr->conn_deny.num_call_ids; i++ )
      {
        if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->conn_deny.call_ids[i]))
        {
          CM_ERR_1( "CONNECTION_DENY: Call Id %d not allocated",
          cm_mc_rpt_ptr->conn_deny.call_ids[i]);
          continue;
        }

        /* Get the pointer to call object */
        call_ptr = cmcall_ptr( cm_mc_rpt_ptr->conn_deny.call_ids[i] );
        CM_ASSERT( call_ptr != NULL );


       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        if(call_ptr->call_state != CM_CALL_STATE_ORIG)
        {
          /* receive in wrong state */
          CM_MSG_HIGH_0("Discard ConnDeny-WrongState");
          continue;
        }
        else if( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE )
        {
          CM_MSG_HIGH_0("Discard ConnDeny-OrigNotSent");
          continue;
        }
        else if( CMCALL_IS_NON_HDR_CALL_ORIG(call_ptr) )         /*lint !e731 */
        {
          /* the current call orig is not HDR so discard the report */
          CM_MSG_HIGH_0("Discard ConnDeny-NonHdrCall");
          continue;
        }

        CM_MSG_HIGH_1("Rcv conneciton deny code=%d",
              cm_mc_rpt_ptr->conn_deny.code);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

       switch( cm_mc_rpt_ptr->conn_deny.code)
       {
        case CM_CD_GENERAL:
        case CM_CD_NETWORK_BUSY:
        case CM_CD_PREF_CH_NOT_AVAIL:
          /* retry this */
            call_ptr->end_status       = CM_CALL_END_CD_GEN_OR_BUSY;
          break;

        case CM_CD_AUTH_BILLING_FAIL:
        default:
            call_ptr->end_status       = CM_CALL_END_CD_BILL_OR_AUTH;
          break;
       }

       cmcall_orig_proc( call_ptr );


      } /* for ( i = 0; i < cm_mc_rpt_ptr->conn_deny.num_call_ids; i++ ) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* VOIP redial over hdr connection deny failure.
      */

      cmxcall_voip_redial_over_ps_orig_failure(
                                   CMCALL_PS_FAILURE_HDR_CONNECTION_FAILURE,
                                   cm_mc_rpt_ptr->conn_deny.code,
                                   CM_CALL_ORIG_ERR_MAX );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                 /*----------------------------------------*/
                 /*         HDR Redirect                   */
                 /*----------------------------------------*/

    case CM_HDR_REDIRECT_F:
     for ( i = 0; i < cm_mc_rpt_ptr->redirect.num_call_ids; i++ )
      {
        if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->redirect.call_ids[i]))
        {
         CM_ERR_1( "HDR_REDIRECT: Call Id %d not allocated",
              cm_mc_rpt_ptr->redirect.call_ids[i]);
           continue;
        }

      /* Get the pointer to call object */
        call_ptr = cmcall_ptr( cm_mc_rpt_ptr->redirect.call_ids[i] );

      if(call_ptr->call_state != CM_CALL_STATE_ORIG)
      {
        /* receive in wrong state */
        CM_MSG_HIGH_0("Discard HdrRedirect-WrongState");
          continue;
        }
        else if( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE &&
          cm_mc_rpt_ptr->redirect.code == CM_REDIRECT_CHG_HDR)
        {
          CM_MSG_HIGH_0("Discard HdrRedirect-OrigNotSent");
          continue;
        }
        else if( CMCALL_IS_NON_HDR_CALL_ORIG(call_ptr) )           /*lint !e731 */
        {
          /* the current call orig is not HDR so discard the report */
          CM_MSG_HIGH_0("Discard HdrRedirect-NonHdrCall");
          continue;
        }

        CM_MSG_HIGH_1("Rcv HdrRedirect code=%d",
          cm_mc_rpt_ptr->redirect.code);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if( cm_mc_rpt_ptr->redirect.code == CM_REDIRECT_CHG_HDR)
      {
          call_ptr->end_status = CM_CALL_END_CHG_HDR;
      }
      else
      {
          call_ptr->end_status           = CM_CALL_END_EXIT_HDR;
          call_ptr->sr.is_hdr_not_avail = TRUE;
          if( call_ptr->favored_mode_pref == CM_MODE_PREF_HDR_ONLY)
          {
            /* This is a HDR only call but since HDR is not available
            ** end the call
            */
            cmcall_end(call_ptr);

            /*  after ending the call, set the call ptr to Null, as memory
            **  allocated to call ptr would be released.
            */
            call_ptr = NULL;
            continue;
          }
        /* else continue the silent redial */
       }
      cmcall_orig_proc( call_ptr );

     } /* for ( i = 0; i < cm_mc_rpt_ptr->redirect.num_call_ids; i++ ) */

     break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                           /*----------------------------------------*/
                           /*         PRL not preferred              */
                           /*----------------------------------------*/

    case CM_HDR_PRL_NOT_PREFERRED_F:
     for ( i = 0; i < cm_mc_rpt_ptr->prl_not_preferred.num_call_ids; i++ )
     {
        if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->prl_not_preferred.call_ids[i]))
        {
         CM_ERR_1( "PRL_NOT_PREFERRED: Call Id %d not allocated",
             cm_mc_rpt_ptr->prl_not_preferred.call_ids[i]);
           continue;
        }

      /* Get the pointer to call object */
        call_ptr = cmcall_ptr( cm_mc_rpt_ptr->prl_not_preferred.call_ids[i] );
        CM_ASSERT( call_ptr != NULL );

      if(call_ptr->call_state != CM_CALL_STATE_ORIG)
      {
        /* receive in wrong state */
        CM_MSG_HIGH_0("Discard HdrPRLNotPref-WrongState");
          continue;
        }
        else if( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE &&
          cm_mc_rpt_ptr->prl_not_preferred.code == CM_PRL_NOT_PREF_CHG_HDR)
        {
          CM_MSG_HIGH_0("Discard HDRPRLNotPref-OrigNotSent");
          continue;
        }
        else if( CMCALL_IS_NON_HDR_CALL_ORIG(call_ptr) )            /*lint !e731 */
        {
          /* the current call orig is not HDR so discard the report */
          CM_MSG_HIGH_0("Discard HdrPRLNotPref-NonHdrCall");
          continue;
        }

        CM_MSG_HIGH_1("Rcv HdrPRLNotPref code=%d",
            cm_mc_rpt_ptr->prl_not_preferred.code);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if( cm_mc_rpt_ptr->prl_not_preferred.code == CM_PRL_NOT_PREF_CHG_HDR)
      {
          call_ptr->end_status = CM_CALL_END_CHG_HDR;
        }
      else
      {
        call_ptr->end_status           = CM_CALL_END_EXIT_HDR;
        call_ptr->sr.is_hdr_not_avail = TRUE;


          if( call_ptr->favored_mode_pref == CM_MODE_PREF_HDR_ONLY)
          {
            /* This is a HDR only call but HDR is not available
             so end the call */
            cmcall_end(call_ptr);

            /*  after ending the call, set the call ptr to Null, as memory
            **  allocated to call ptr would be released.
            */
            call_ptr = NULL;

            continue;
          }
        /* else continue the silent redial */
      }
      cmcall_orig_proc( call_ptr );

     } /* for ( i = 0; i < cm_mc_rpt_ptr->prl_not_preferred.num_call_ids; i++ ) */

     break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                           /*----------------------------------------*/
                           /*         Deactivate ACK                 */
                           /*----------------------------------------*/

    case CM_HDR_DEACTIVATE_ACK_F:
      /* Tear down all the calls
      */
      for ( i = 0; i < CMCALL_MAX_CALL_OBJ; i++ )
      {
        /* Get the pointer to call object */
        if ( (call_ptr = cmcall_ptr( (cm_call_id_type) i ) ) != NULL )
        {

		 /* end calls which are connected */
          if (((call_ptr->call_type == CM_CALL_TYPE_PS_DATA) ||
               ((call_ptr->call_type == CM_CALL_TYPE_VOICE) &&
                (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP))) &&
              (call_ptr->sys_mode == SYS_SYS_MODE_HDR)
             )
          {
            call_ptr->end_status = CM_CALL_END_EXIT_HDR;
            cmcall_end( call_ptr );
          }

		  /* Try originating call again in next SR */
		  else if ((call_ptr->call_type == CM_CALL_TYPE_PS_DATA) &&
		   (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_HDR) &&
		   (call_ptr->call_state  == CM_CALL_STATE_ORIG))

		  {
			 call_ptr->end_status = CM_CALL_END_EXIT_HDR;
             cmcall_orig_proc( call_ptr );

		  }

          /*  after ending the call, set the call ptr to Null, as memory
          **  allocated to call ptr would be released.
          */
          call_ptr = NULL;

          continue;
        }
      } /* for ( i = 0; i < CM_CALL_ID_MAX; i++ ) */

      break;

  #endif /* FEATURE_HDR_HYBRID */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                         /*---------------------------------------------*/
                         /*    Emergency call flashed over a call       */
                         /*---------------------------------------------*/

    case CM_CALL_EMERGENCY_FLASHED_F:
      {

      cmcall_s_type *original_call_id_call_ptr;
      cmcall_s_type *emergency_flash_call_id_call_ptr;

      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->call_emergency_flashed.emergency_flash_call_id))
      {
         CM_ERR_1( "EMERGENCY_FLASHED: Call Id %d not allocated",
           cm_mc_rpt_ptr->call_emergency_flashed.emergency_flash_call_id);
         break;
      }

      emergency_flash_call_id_call_ptr = cmcall_ptr(
        cm_mc_rpt_ptr->call_emergency_flashed.emergency_flash_call_id);

      if(emergency_flash_call_id_call_ptr == NULL)
      {
         CM_MSG_HIGH_0( "emergency_flash_call_id_call_ptr_NULL");
         break ;
      }

      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->call_emergency_flashed.original_call_id))
      {
         CM_ERR_1( "EMERGENCY_FLASHED: Call Id %d not allocated",
           cm_mc_rpt_ptr->call_emergency_flashed.original_call_id);
         break;
      }

      original_call_id_call_ptr =
        cmcall_ptr( cm_mc_rpt_ptr->call_emergency_flashed.original_call_id );

      if(original_call_id_call_ptr == NULL)
      {
         CM_MSG_HIGH_0( "original_call_id_call_ptr_NULL");
         break ;
      }


      /* Now convert call with emergency_flash_call_id to emergency call type
         in CM and notify all clients that registered for CM_CALL_EVENT_EMERGENCY_FLASHED */
      (void)cmxcall_convert_to_emergency(cm_mc_rpt_ptr);

      /* Notify all the clients about CM_CALL_EVENT_EMERGENCY_FLASHED event */
      cmcall_event(emergency_flash_call_id_call_ptr, CM_CALL_EVENT_EMERGENCY_FLASHED);

      /* End original_call_id_call_ptr */
      original_call_id_call_ptr->end_status = CM_CALL_END_EMERGENCY_FLASHED;
      cmcall_end(original_call_id_call_ptr);

      break;
      }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_CALL_HOLD_L2ACK_F:
    {
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->call_hold_l2ack.call_id))
      {
        CM_ERR_1( "HOLD_L2ACK: Call Id %d not allocated",
                cm_mc_rpt_ptr->call_hold_l2ack.call_id);
        break;
      }

      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->call_hold_l2ack.call_id ))
        == NULL )
      {
        CM_ERR_1( "HOLD_L2ACK: No callobject for call id %d.",
                cm_mc_rpt_ptr->call_hold_l2ack.call_id);
      }
      break;
    }

    case CM_SND_BRST_DTMF_ACK_F:
    {
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->snd_brst_dtmf_ack.call_id))
      {
        CM_ERR_1( "SND_BRST_DTMF: Call Id %d not allocated",
                cm_mc_rpt_ptr->snd_brst_dtmf_ack.call_id);
        break;
      }

      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->snd_brst_dtmf_ack.call_id ))
           == NULL )
      {
        CM_ERR_1( "SND_BRST_DTMF: No callobject for call id %d.",
                cm_mc_rpt_ptr->snd_brst_dtmf_ack.call_id);
      }
      break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CDMA_CONNECT_ORDER_ACK_F:
    {
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->cdma_connect_order_ack.call_id))
      {
        CM_ERR_1( "CONNECT_ORDER_ACK: Call Id %d not allocated",
                cm_mc_rpt_ptr->cdma_connect_order_ack.call_id);
        break;
      }

      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->cdma_connect_order_ack.call_id ))
         == NULL )
      {
        CM_ERR_1( "CONNECT_ORDER_ACK: No callobject for call id %d.",
                cm_mc_rpt_ptr->cdma_connect_order_ack.call_id);
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Notify clients of event.
      */
      cmcall_event( call_ptr, CM_CALL_EVENT_CONNECT_ORDER_ACK );

      break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   #ifdef FEATURE_REDIRECTING_NUMBER_INFO_REC

                 /*----------------------------------*/
                 /*      Redirecting Number          */
                 /*----------------------------------*/

    case CM_REDIRECTING_NUMBER_F:

      /*
      ** This message may have arrived out of order.
      ** If the call id is unassigned, sent the event with the unassigned call id.
      ** If the call id is valid but the call state is IDLE, buffer the event and
      ** wait for the incoming event.
      ** Otherwise send the event now.
      */
      if (( cm_mc_rpt_ptr->redirecting_number.call_id == CM_CALL_ID_UNASSIGNED ) ||
          (!cm_call_id_is_allocated (cm_mc_rpt_ptr->redirecting_number.call_id)) )
      {
        cmxcall_event_generic( cm_mc_rpt_ptr, CM_CALL_EVENT_REDIRECTING_NUMBER );
        break;
      }

      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->redirecting_number.call_id )) == NULL )
      {
        CM_ERR_1( "REDIRECTING_NUMBER: Can't get pointer to call object. id = %d",
                cm_mc_rpt_ptr->redirecting_number.call_id);

        cmxcall_event_generic( cm_mc_rpt_ptr, CM_CALL_EVENT_REDIRECTING_NUMBER );
        break;
      }

      if (call_ptr->call_state == CM_CALL_STATE_IDLE)
      {
        /* Save the command contents to process it later.
           The assumption is that a CM_INCOMING_CALL_F is soon to show. */

        /* Set the flags */
        out_of_order_msgs.is_redirecting_number_flag = TRUE;

        /* Copy the data */
        CM_MSG_LOW_0 ("Save CM_REDIRECTING_NUMBER_F");
        *(&(out_of_order_msgs.redirecting_number_buffer)) = *(&(cm_mc_rpt_ptr->redirecting_number));
      }
      else
      {
        /* Message did not arrive out of sequence so process it now */
        call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

        call_mode_info_ptr->is_last_cdma_info_rec = cm_mc_rpt_ptr->redirecting_number.hdr.last_record;

        cmxcall_process_redirecting_record_from_mc( &call_ptr->redirecting_number,
                                                    &cm_mc_rpt_ptr->redirecting_number );


        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Notify clients of event.
        */
        cmcall_event( call_ptr, CM_CALL_EVENT_REDIRECTING_NUMBER );
      }
      break;
    #endif   /* FEATURE_REDIRECTING_NUMBER_INFO_REC */




    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
    #error code not present
#endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_HDR
    case CM_HDR_CALL_END_CNF_F:
      /* Decide on the action according to the call state.
      */
      for ( i = 0; i < cm_mc_rpt_ptr->call_end_cnf.num_call_ids; i++ )
      {
        if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->call_end_cnf.call_ids[i]))
        {
          CM_MSG_HIGH_1( "CALL END CNF: Unallocated call id %d",
                     cm_mc_rpt_ptr->call_end_cnf.call_ids[i]);
          continue;
        }
        /* Get the pointer to call object */
        call_ptr = cmcall_ptr( cm_mc_rpt_ptr->call_end_cnf.call_ids[i]);
        CM_ASSERT( call_ptr != NULL );
        call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Make sure we are waiting for end cnf.
        */
        if( call_ptr->call_state != CM_CALL_STATE_IDLE ||
            call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
          )
        {
          CM_MSG_HIGH_1("Invalid call end cnf-Idle: Call Id %d ",
                      cm_mc_rpt_ptr->call_end_cnf.call_ids[i]);
        }

        /* Release the call.
        */
        cmcall_end(call_ptr);

        /*  after ending the call, set the call ptr to Null, as memory
        **  allocated to call ptr would be released.
        */
        call_ptr = NULL;

      } /* for ( i = 0; i < cm_mc_rpt_ptr->call_end_cnf.num_end_call_ids; i++ ) */

      /* Invoke the cmcall_timer_proc() to start processing any calls that were
      ** waiting for HDR call end cnf.
      */
      cmcall_timer_proc( 0 );
      break;

    #endif /* FEATURE_HDR */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_LINE_CTRL_F:

      /* Get the pointer to call object */
      if((call_ptr = cmcall_ptr( cm_mc_rpt_ptr->line_ctrl.call_id ))
          == NULL )
      {
        CM_ERR_2( "BAD CALL ID %d, cmd=%d(CM_LINE_CTRL_F)",
                 cm_mc_rpt_ptr->line_ctrl.call_id,
                 cm_mc_rpt_ptr->hdr.cmd);
        break;
      }

      /* Copy the line control parameters
      */
      call_ptr->cmcall_mode_info.info.cdma_info.line_ctrl_polarity_included
                          = cm_mc_rpt_ptr->line_ctrl.polarity_included;
      call_ptr->cmcall_mode_info.info.cdma_info.line_ctrl_toggle
                          = cm_mc_rpt_ptr->line_ctrl.toggle;
      call_ptr->cmcall_mode_info.info.cdma_info.line_ctrl_power_denial
                          = cm_mc_rpt_ptr->line_ctrl.power_denial;
      call_ptr->cmcall_mode_info.info.cdma_info.line_ctrl_reverse
                          = cm_mc_rpt_ptr->line_ctrl.reverse;

      cmcall_event(call_ptr,CM_CALL_EVENT_LINE_CTRL);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PRIVACY_SETTING_F:
      {

        cm_iterator_type       call_itr;
        cm_privacy_pref_e_type privacy_setting =
          cmxcall_map_mc_long_code_to_cm_privacy_pref(
                                     cm_mc_rpt_ptr->privacy_setting.long_code);

        /* If the privacy is enhanced, then set it to 1 else set it to 0
        */

        boolean is_privacy = ((privacy_setting == CM_PRIVACY_PREF_ENHANCED)? 1:0);

        if( ph_ptr->is_privacy != is_privacy )
        {
          ph_ptr->is_privacy = is_privacy;
          cmcall_obj_iterator_init(&call_itr);
          call_ptr = cmcall_obj_get_next(&call_itr);
          while( call_ptr != NULL )
          {
            call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );
            if( call_mode_info_ptr->is_privacy != is_privacy )
            {
              call_mode_info_ptr->is_privacy = is_privacy;
              cmcall_event( call_ptr, CM_CALL_EVENT_PRIVACY );
            }
            call_ptr = cmcall_obj_get_next(&call_itr);
          }
        }
      }
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_TUNNEL_MSG_F:
    case CM_HDR_TUNNEL_MSG_F:
      call_ptr = NULL;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_TUNNEL_MSG_STATUS_F:

      call_ptr = NULL;
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_END_VOIP_CALL_F:
      call_ptr = NULL;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_GET_CSNA_MSG_CONFIG_F:
    case CM_UPDATE_CSNA_MSG_CONFIG_F:
      /*homer_csna_config_msg_update_proc( cm_mc_rpt_ptr );
             commented this as the associated defination
             was removed as a part of FEATURE_UMTS_1X_HANDOVER feature cleanup.
             But as such the FR FEATURE_HDR_CSNA is not yet removed.*/
      call_ptr = NULL;
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_GSTK
    case CM_MMGSDI_REFRESH_FCN_IND_F:
      if(cm_mc_rpt_ptr->voting_mmgsdi.refresh.mode != MMGSDI_REFRESH_NAA_FCN &&
         cm_mc_rpt_ptr->voting_mmgsdi.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT)
      {
        cm_overall_call_state_e_type       cm_state;

        /* Check to see if CM is in a call */
        cm_state = cmcall_overall_call_state();
        if(ph_ptr->refresh_vote_ok == FALSE)
        {
          if(cm_state == CM_OVERALL_CALL_STATE_IDLE)
          {
            cmmmgsdi_ok_to_refresh(
                           MMGSDI_SLOT_1,
                           #ifdef FEATURE_MMGSDI_SESSION_LIB
                           cm_mc_rpt_ptr->voting_mmgsdi.session_id,
                           #endif
                           TRUE
                           );
          }
          else
          {
            CM_MSG_HIGH_0 ("cmmmgsdi_ok_to_refresh is FALSE");
            cmmmgsdi_ok_to_refresh(
                               MMGSDI_SLOT_1,
                               #ifdef FEATURE_MMGSDI_SESSION_LIB
                               cm_mc_rpt_ptr->voting_mmgsdi.session_id,
                               #endif
                               FALSE
                               );
          }
        }
        else
        {
          CM_MSG_HIGH_0 ("cmmmgsdi_ok_to_refresh is TRUE, as refresh_vote_ok is TRUE");
          cmmmgsdi_ok_to_refresh(
                           MMGSDI_SLOT_1,
                           #ifdef FEATURE_MMGSDI_SESSION_LIB
                           cm_mc_rpt_ptr->voting_mmgsdi.session_id,
                           #endif
                           TRUE
                           );
        }
      }
      if (cm_mc_rpt_ptr->voting_mmgsdi.refresh.stage == MMGSDI_REFRESH_STAGE_START &&
         (cm_mc_rpt_ptr->voting_mmgsdi.refresh.mode == MMGSDI_REFRESH_RESET||
          cm_mc_rpt_ptr->voting_mmgsdi.refresh.mode == MMGSDI_REFRESH_NAA_INIT||
          cm_mc_rpt_ptr->voting_mmgsdi.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FCN ||
          cm_mc_rpt_ptr->voting_mmgsdi.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FULL_FCN ||
          cm_mc_rpt_ptr->voting_mmgsdi.refresh.mode == MMGSDI_REFRESH_NAA_APP_RESET ||
          cm_mc_rpt_ptr->voting_mmgsdi.refresh.mode == MMGSDI_REFRESH_3G_SESSION_RESET ||
          cm_mc_rpt_ptr->voting_mmgsdi.refresh.mode == MMGSDI_REFRESH_RESET_AUTO)
         )
      {
        if(cmph_get_received_card_init_completed_evt() == TRUE)
        {
          cmph_set_received_card_init_completed_evt(FALSE);

          /* If CDMA subscription from card is already sent, made CDMA
          ** subscription not available
          */
          if (cmph_get_cdma_subs_from_card_sent() == TRUE)
          {
            cdma_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;

            /* subscription status updated in global
            ** varible for later use
            */
            cmph_set_cdma_subs_from_card_sent( FALSE );
          }

          /* If GW subscription from card is already sent, made GW
          ** subscription not available
          */
          if (cmph_get_gw_subs_from_card_sent() == TRUE)
          {
            gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;

           /* subscription status updated in global
           ** varible for later use
           */
            cmph_set_gw_subs_from_card_sent( FALSE );
          }

          /* If subscription from card for any of the cellular technology
          ** (CDMA or GW) is to be made unavailable, then only queue the
          ** subscription not available command to CM
          */
          if (gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE ||
              cdma_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE)
          {
            if (!cm_ph_cmd_subscription_not_available_new(
                     NULL,
                     NULL,
                     CM_CLIENT_ID_ANONYMOUS,
                     cdma_subs_status,
                     gw_subs_status)
               )
            {
              CM_ERR_2(" subs na CDMA %d, GW %d",
                       cdma_subs_status, gw_subs_status);
            }
            else
            {
              CM_MSG_HIGH_2(" subs na CDMA %d, GW %d",
                      cdma_subs_status, gw_subs_status);
            }
          } /* if (gw_subs_status ==... */
        } /* if(cmph_get_received_card_init_completed_evt()... */
      } /* if (cm_mc_rpt_ptr->voting_mmgsdi.refresh.stage... */
      break;
    #endif /* #ifdef FEATURE_GSTK */

   #if defined (FEATURE_LTE_TO_1X)

    case CM_1XCSFB_ORIG_RES_F:

      CM_MSG_HIGH_2("RXD: CM_1XCSFB_ORIG_RES_F call_id = %d is_allowed= %d",
                  cm_mc_rpt_ptr->csfb_org_res.call_id,
                  cm_mc_rpt_ptr->csfb_org_res.is_allowed);

      /* Get the pointer to call object */
      if((call_ptr = cmcall_ptr( cm_mc_rpt_ptr->csfb_org_res.call_id ))
          == NULL )
      {
        CM_ERR_2( "BAD CALL ID %d, cmd=%d (CM_1XCSFB_ORIG_RES_F)",
                 cm_mc_rpt_ptr->csfb_org_res.call_id,
                 cm_mc_rpt_ptr->hdr.cmd);
        break;
      }
      if(cm_mc_rpt_ptr->csfb_org_res.is_allowed)
      {
        /* PSIST check passed.Send 1XCSFB Call Request (MO) to NAS */
        if((call_ptr->sr.is_ok_to_orig == TRUE) &&
           (cmss_ptr()->info.sys_mode == SYS_SYS_MODE_LTE))
        {
        cmcsfbcall_send_orig_req (call_ptr);
        }
        else
        {
          if(cmsds_is_csfb_deact_allowed())
          {
            cmcsfbcall_send_1xcsfb_call_clear( call_ptr );
          }
          /* Process silent redial */
          cmcall_orig_proc( call_ptr );
          CM_MSG_HIGH_0("Orig_response received while ok_to_orig is FALSE ");
        }
      }
      else
      {
        CM_MSG_HIGH_1("1XCSFB:error code=%d received in CM_1XCSFB_ORIG_RES_F ",\
                              cm_mc_rpt_ptr->csfb_org_res.code );
        switch( cm_mc_rpt_ptr->csfb_org_res.code )
        {
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case CM_CALL_ORIG_ERR_CDMA_PSIST_NG:
          {
            CM_MSG_HIGH_1("1XCSFB:PSIST check failed(code=%d) process hard fail ",\
                              cm_mc_rpt_ptr->csfb_org_res.code );
            call_ptr->end_status   = CM_CALL_END_1XCSFB_HARD_FAILURE;
          }
          break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case CM_CALL_ORIG_ERR_ABORT:
          {

            /* MC discarded the origination because it is not able to process
            ** it in the current state (e.g. MC is init state).
            */
            call_ptr->end_status = CM_CALL_END_1XCSFB_SOFT_FAILURE;
          }
          break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case CM_CALL_ORIG_ACC_IN_PROG:
          {
            /* MC discarded the origination because access is already in
            ** progress (i.e., MC is in the system access state).
            */
            call_ptr->end_status = CM_CALL_END_ACC_IN_PROG;
          }
          break;
          case CM_CALL_ORIG_ERR_REDIR:
          {
           /* MC discarded the origination because 1X protocol is moving to native 1X mode due to redirection.
            */
            call_ptr->end_status = CM_CALL_END_REDIR_OR_HANDOFF;
          }
          break;

          default:
          {
            call_ptr->end_status = CM_CALL_END_REL_NORMAL;
            CM_ERR_1( "Orig fail code=%d",(uint8) cm_mc_rpt_ptr->orig_fail.code);
          }
          break;

        } /* switch( cm_mc_rpt_ptr->csfb_org_res.code ) */
        if((call_ptr->end_status == CM_CALL_END_1XCSFB_HARD_FAILURE || 
          call_ptr->end_status == CM_CALL_END_1XCSFB_SOFT_FAILURE || 
          call_ptr->end_status == CM_CALL_END_REDIR_OR_HANDOFF) &&
          cmsds_is_csfb_deact_allowed())
        {
          cmcsfbcall_send_1xcsfb_call_clear( call_ptr );
        }
       /* Process silent redial */
       cmcall_orig_proc( call_ptr );

      }

      break;

    case CM_1XCSFB_CALL_END_CNF_F:

      CM_MSG_HIGH_1("RXD: CM_1XCSFB_CALL_END_CNF_F call_id = %d ",
                  cm_mc_rpt_ptr->csfb_call_end_cnf.call_id);

      /* Get the pointer to call object */
      if((call_ptr = cmcall_ptr( cm_mc_rpt_ptr->csfb_org_res.call_id ))
          == NULL )
      {
        CM_ERR_2( "BAD CALL ID %d, cmd=%d (CM_1XCSFB_CALL_END_CNF_F)",
                 cm_mc_rpt_ptr->csfb_call_end_cnf.call_id,
                 cm_mc_rpt_ptr->hdr.cmd);
        break;
      }
      /* Clear the call object */
      cmcsfbcall_1xcsfb_call_end(call_ptr);
      break;

              /*------------------------------*/
              /*            1XCSFB Incoming call     */
              /*------------------------------*/
    case CM_1XCSFB_INCOMING_CALL_F:

      CM_MSG_HIGH_1("RXD: CM_1XCSFB_INCOMING_CALL_F, call id = %d",cm_mc_rpt_ptr->csfb_mt_call.call_id);

      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->csfb_mt_call.call_id))
      {
        CM_ERR_1( "1XCSFB INCOMING: Call Id %d is unallocated",
          cm_mc_rpt_ptr->csfb_mt_call.call_id);
        break;
      }

      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->csfb_mt_call.call_id ))
        == NULL )
      {
        CM_MSG_HIGH_1( "1XCSFB INCOMING: Can't get pointer to call object. id = %d",
          cm_mc_rpt_ptr->csfb_mt_call.call_id);
        break;
      }

      #ifdef FEATURE_MMODE_DUAL_SIM
      if ( cm_mc_rpt_ptr->csfb_mt_call.call_type == CM_CALL_TYPE_VOICE && 
           cmph_is_msim() && cm_mc_rpt_ptr->csfb_mt_call.subs_capability != cmmsc_get_curr_msc_max_cap(SYS_MODEM_AS_ID_1))
      {
        CM_MSG_HIGH_2("Subs capability mismatch in MMODE 0x%x annd 1x 0x%x",
          cmmsc_get_curr_msc_max_cap(SYS_MODEM_AS_ID_1), cm_mc_rpt_ptr->csfb_mt_call.subs_capability);

        mc_ptr = cm_mc_get_buf_else_err_fatal();
        mc_ptr->hdr.cmd               = MC_CSFB_CALL_END_F;
        mc_ptr->csfb_call_end.call_id = cm_mc_rpt_ptr->csfb_mt_call.call_id;
        
        /* Send the origination command to MC and set orig_sent_on_mode to TRUE
        */
        
        CM_MSG_HIGH_1( "1XCSFB:Send MC_CSFB_CALL_END_F to 1XCP for call id =%d",\
                     cm_mc_rpt_ptr->csfb_mt_call.call_id );
        cm_mc_send_cmd( mc_ptr );
        
        cm_call_id_deallocate (cm_mc_rpt_ptr->csfb_mt_call.call_id);
        break; /* case CM_INCOMING_CALL_F: */

      }
      #endif
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      /* Setting the ss instance for the call
      */
      call_ptr->ss = CM_SS_MAIN;

      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );
      CM_MSG_HIGH_1( "1XCSFB INCOMING: call_type = %d", cm_mc_rpt_ptr->csfb_mt_call.call_type);
      call_ptr->call_type = cm_mc_rpt_ptr->csfb_mt_call.call_type;
      /* for CSFB call we use service type Automatic.*/
      call_ptr->srv_type = CM_SRV_TYPE_CDMA_AUTOMATIC;

      call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_CDMA;

      /* Update the call state */
      call_ptr->call_state = CM_CALL_STATE_INCOM;
      call_ptr->call_subst.incom = (cm_call_incom_subst_e_type) CM_CALL_INCOM_SUBST_NONE;

      /* Set the call direction */
      cmcall_set_call_direction (call_ptr, CM_CALL_DIRECTION_MT);

      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
      call_ptr->sys_mode = ss_ptr->info.sys_mode;
      /* make sure signal info is set to false. signal info
      should come from CM_SIGNAL_F */
      call_ptr->signal.is_signal_info_avail  = FALSE;

      /* Inform CMPH that call has started.*/
      cmph_call_start(call_ptr);

      /* Push the origination top pointer again, with this call_id */
      call_ptr->csfb_type = CM_CALL_CSFB_TYPE_1XCSFB;
      (void)cmph_update_orig_param_incoming_call(call_ptr);

      /* Send 1XCSFB Call Request (MT) to NAS */
      if ((call_ptr->call_type == CM_CALL_TYPE_VOICE) ||
	  (call_ptr->call_type == CM_CALL_TYPE_PD))
      {
       cmcsfbcall_send_orig_req (call_ptr);
      }

      break;

             /*----------------------------------------*/
             /*            1XCSFB Incoming SMS call response     */
             /*----------------------------------------*/
   case CM_MT_SMS_CALL_RSP:
       /* 1XCSFB call confirmation received */
       CM_MSG_HIGH_2("1XCSFB:Received CM_MT_SMS_CALL_RSP with call_id = %d rej_value = %d",\
                    cm_mc_rpt_ptr->csfb_mt_sms_rsp.call_id, cm_mc_rpt_ptr->csfb_mt_sms_rsp.rej_value );

       if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->csfb_mt_sms_rsp.call_id))
       {
         CM_ERR_1( "1XCSFB INCOMING SMS: Call Id %d is unallocated",
         cm_mc_rpt_ptr->csfb_mt_sms_rsp.call_id);
         break;
       }

       /* get the call pointer from the call ID */
       if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->csfb_mt_sms_rsp.call_id ))
         == NULL )
       {
         CM_MSG_HIGH_1( "1XCSFB INCOMING SMS: Can't get pointer to call object. id = %d",
         cm_mc_rpt_ptr->csfb_mt_sms_rsp.call_id);
         break;
       }

       if(call_ptr->direction == CM_CALL_DIRECTION_MT)
       {
         cmcsfbcall_send_1xcsfb_call_clear( call_ptr);
         cmcsfbcall_1xcsfb_call_end(call_ptr);
         return;
       }
       break;
   #endif /* FEATURE_LTE_TO_1X */

   case CM_CODEC_INFO_F:
      CM_MSG_HIGH_0("RXD: CM_CODEC_INFO_F");
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->mc_codec_info.call_id))
      {
        CM_ERR_1( "CODEC_INFO: Call Id %d not allocated", cm_mc_rpt_ptr->mc_codec_info.call_id);
        break;
      }

      /* Get the pointer to call object */
      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->mc_codec_info.call_id )) == NULL )
      {
        CM_ERR_2( "BAD CALL ID %d, cmd=%d(CM_CODEC_INFO_F)",
                 cm_mc_rpt_ptr->mc_codec_info.call_id,
                 cm_mc_rpt_ptr->hdr.cmd);
        break;
      }

      if (call_ptr->call_state == CM_CALL_STATE_ORIG ||
          call_ptr->call_state == CM_CALL_STATE_INCOM ||
          call_ptr->call_state == CM_CALL_STATE_CONV )
      {

        #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
        if ((cm_mc_rpt_ptr->mc_codec_info.codec_info.cmd == SYS_HO_START) ||
            (cm_mc_rpt_ptr->mc_codec_info.codec_info.cmd == SYS_HO_FAIL)  ||
            (cm_mc_rpt_ptr->mc_codec_info.codec_info.cmd == SYS_HO_COMPLETE))
        {
            if ((call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB) &&
                (call_ptr->is_volte == FALSE))
            {
              call_ptr->voice_ho_type = SYS_VOICE_HANDOVER_1XCSFB;
            }
        }
        #endif

        #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
		    if ((cm_mc_rpt_ptr->mc_codec_info.codec_info.cmd == SYS_HO_START) ||
            (cm_mc_rpt_ptr->mc_codec_info.codec_info.cmd == SYS_HO_COMPLETE))
        {
          if (call_ptr->is_srvcc_call)
          {
			      CM_MSG_HIGH_1("Do not send HO_START/COMPLETE during 1xSRVCC for call:%d" ,cm_mc_rpt_ptr->mc_codec_info.call_id);
			      return;
          }
        }
        #endif
        
        #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
        if(cmcall_drvcc_is_3gpp2_drvcc_type(cmcall_drvcc_get_setup_call_drvcc_type(call_ptr)))
        {
          CM_MSG_HIGH_1("DRVCC: Do not send HO_START/COMPLETE/Fail during 1xDRVCC for call:%d" ,cm_mc_rpt_ptr->mc_codec_info.call_id);
          return;
        }
        #endif
        
        switch(cm_mc_rpt_ptr->mc_codec_info.codec_info.cmd)
        {
        case SYS_HO_START:
          cmcall_event( call_ptr, CM_CALL_EVENT_HO_START );
          break;
        case SYS_HO_FAIL:
          cmcall_event( call_ptr, CM_CALL_EVENT_HO_FAIL );
          break;
        case SYS_HO_COMPLETE:
          cmcall_event( call_ptr, CM_CALL_EVENT_HO_COMPLETE);
          break;
        case SYS_SPEECH_CODEC_INFO:
          call_ptr->speech_codec_info.network_mode = SYS_NETWORK_TYPE_CDMA;
          call_ptr->speech_codec_info.speech_codec_type = cm_mc_rpt_ptr->mc_codec_info.codec_info.speech_codec;
          call_ptr->speech_codec_info.speech_enc_samp_freq = cm_mc_rpt_ptr->mc_codec_info.codec_info.speech_enc_samp_freq;
          CM_MSG_MED_2("1X codec_type=%d, samp_freq=%d",cm_mc_rpt_ptr->mc_codec_info.codec_info.speech_codec, cm_mc_rpt_ptr->mc_codec_info.codec_info.speech_enc_samp_freq);
          cmcall_event( call_ptr, CM_CALL_EVENT_SPEECH_CODEC_INFO);
          break;
        default:
          break;
        }
      }
      else
      {
        CM_MSG_HIGH_1( "There is no active call. Call state: %d",call_ptr->call_state);
        break;
      }

      break;

  case CM_1XSRVCC_HO_EUTRA_IND_F:

    #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
    CM_MSG_HIGH_0("RXD: CM_1XSRVCC_HO_EUTRA_IND_F ");
    call_id = cmcall_find_active_voice_call();

    cm_ims_cache_init();
    cm_ims_cache_update(SYS_HO_TYPE_LTE_1X_SRVCC, ll_as_id);
    if(call_id == CM_CALL_ID_INVALID )
    {
      CM_MSG_HIGH_0("CM_1XSRVCC_HO_EUTRA_IND_F,no voice call found");
      call_info_ptr = cmcall_info_alloc();
      if ( call_info_ptr == NULL )
      {
        CM_MSG_HIGH_0("CM_1XSRVCC_HO_EUTRA_IND_F, can not alloc memory for call info object ");
        return;
      }
      call_info_ptr->voice_ho_type = SYS_VOICE_HO_SRVCC_L_2_C;
      cmclient_list_call_event_ntfy ( CM_CALL_EVENT_HO_START, call_info_ptr );
    }
    else
    {
      CM_MSG_HIGH_0("CM_1XSRVCC_HO_EUTRA_IND_F, voice call found");
      call_ptr = cmcall_ptr (cmcall_get_active_srvcc_volte_call(CM_CALL_TYPE_VOICE_MASK |
                                                                CM_CALL_TYPE_EMERG_MASK  |
                                                                CM_CALL_TYPE_VT_MASK,
                                                                ll_as_id));
      if( call_ptr == NULL )
      {
        CM_MSG_HIGH_0("CM_1XSRVCC_HO_EUTRA_IND_F, No VOLTE call for 1x-SRVCC ");
        return;
      }
      /* need to check IMS type*/
      //cmwcall_set_qc_ims(TRUE);
      call_ptr->srvcc_prog_state = CM_SRVCC_STATE_HO_EUTRA_REQ_RCVD;
      cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_HO_START_NOTIFY, ll_as_id);
      call_ptr->voice_ho_type = SYS_VOICE_HO_SRVCC_L_2_C;
      cmcall_event( call_ptr, CM_CALL_EVENT_HO_START );
    }
    #endif
    break;

   case CM_1XSRVCC_HO_COMPLETE_IND_F:

      #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
      CM_MSG_HIGH_0("Recvd CM_1XSRVCC_HO_COMPLETE_IND_F ");
    cmcall_obj_iterator_init(&call_itr);
      call_ptr = cmcall_obj_get_next(&call_itr);
    ho_comp_list = (cm_mid_srvcc_ho_comp_list_s_type *)cm_mem_malloc
                                      (sizeof(cm_mid_srvcc_ho_comp_list_s_type));
      /* loop through all srvcc call in HO */
      while(call_ptr != NULL)
      {
        if (call_ptr->is_srvcc_call)
        {
		  call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;

		  call_ptr->sys_mode                   = SYS_SYS_MODE_CDMA;
		  
		  call_ptr->srvcc_prog_state = CM_SRVCC_STATE_NONE;

		  #ifdef FEATURE_IP_CALL
            call_ptr->is_volte = FALSE;
          #endif

          srvcc_call_info_ptr =cmcall_srvcc_info_alloc();
          if( srvcc_call_info_ptr == NULL )
          {
            CM_MSG_HIGH_0("CM_1XSRVCC_HO_COMPLETE_IND_F, can not alloc memory for srvcc call info object");
            break;
		  }
		  if(active_srvcc_call_id == CM_CALL_ID_INVALID)
          {
            active_srvcc_call_id =call_ptr->call_id;
          }

          (void) cmcall_srvcc_info_get( call_ptr, srvcc_call_info_ptr );

		  /*copy each srvcc call context to ho_comp_list buffer */
         * (&(ho_comp_list->info[num_srvcc_calls])) = (*srvcc_call_info_ptr);

          cm_mem_free(srvcc_call_info_ptr);
		  num_srvcc_calls++;
		}

        call_ptr = cmcall_obj_get_next(&call_itr);
      }

      call_ptr = cmcall_ptr(active_srvcc_call_id);
	  
	  if(call_ptr == NULL)
	  {
	     CM_MSG_HIGH_0("CM_1XSRVCC_HO_COMPLETE_IND_F, call not found" );
         break;
	  }

	  if(call_ptr->is_int_ims)
	  {
	    ho_comp_list->is_int_ims = TRUE;

		ph_p_orig = cmtask_orig_para_search_act_type( CM_SS_MAIN,CM_ACT_TYPE_PH_OBJ ) ;

	    p_orig =  cmtask_orig_para_search_orig_mode( CM_SS_MAIN, call_ptr->orig_mode ) ;
	    if(p_orig != NULL)
        {
          /* update SRVCCed call orig mode to NORMAL and restore
	      call mode pref with phone pref ( Scenario where call is
	       made with LTE_ONLY) */
		  if(p_orig->orig->orig_mode == SD_SS_ORIG_MODE_VOLTE )
		  {
			p_orig->orig->orig_mode = SD_SS_ORIG_MODE_NORMAL;

			p_orig->orig->orig_mode_pref = ph_p_orig->orig->orig_mode_pref ;

		  }
		}
	  }
	  else
	  {
        ho_comp_list->is_int_ims = FALSE;

		/* Update priority queue
         ** - If we already have same call id in priority queue do nothing
         ** - search if there is existing ext IMS emerg call. replace
         ** - emerg call act_id with new call id
         ** - If no emergency call in queue, add normal voice call to priQ
        */
        if(cmtask_orig_para_search_act_id( CM_SS_MAIN, call_ptr->call_id ) == NULL)
        {
          p_orig_emg = cmtask_orig_para_search_act_id( CM_SS_MAIN, cmemg_get_act_id(CM_SS_MAIN) );
          if(p_orig_emg == NULL)
          {
            (void)cmph_update_orig_param_incoming_call(call_ptr);
          }
          else /* found ext IMS e911 call */
          {
            p_orig_emg->orig->act_id = call_ptr->call_id;
                  /* update call type based on info we have */
            call_ptr->call_type = CM_CALL_TYPE_EMERGENCY;
          }
        }
        /* Exit LTE e911 searching state */
        cmemg_set_reason_per_sub( CM_AC_REASON_NONE, ll_as_id);
	  }

	  cmcall_event( call_ptr, CM_CALL_EVENT_HO_COMPLETE );
    cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_HO_COMPLETE_NOTIFY, ll_as_id);
	  CM_MSG_HIGH_0("CM->MSGR: 1xSRVCC: IPAPP notification sent from HO_COMPLETE");
		 /* set total calls that got transferred to GW */
      ho_comp_list->number_calls= num_srvcc_calls;

      (void) cm_msgr_send( MM_CM_SRVCC_HO_COMPLETE_IND, MSGR_MM_CM,
            (msgr_hdr_s*)ho_comp_list, sizeof(cm_mid_srvcc_ho_comp_list_s_type) );

	  CM_MSG_HIGH_0("CM_1XSRVCC_HO_COMPLETE_IND_F, msgr sent HO calls");
	  cmsds_ptr()->is_cdma_srvcc_activated = FALSE;
      /* HO processing complete, reset the IMS SRVCC cache */
      cm_ims_cache_reset();
      cm_mem_free(ho_comp_list);

      #endif
	  break;

	  case CM_1XSRVCC_HO_FAIL_IND_F:

      #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
      CM_MSG_HIGH_0("Recvd CM_1XSRVCC_HO_FAIL_IND_F ");
	  mc_ptr = cm_mc_get_buf_else_err_fatal();
	  cmcall_obj_iterator_init(&call_itr);
      call_ptr = cmcall_obj_get_next(&call_itr);

      /* loop through all srvcc call in HO */

	  if(cm_get_srvcc_ho_error_state() != CM_SRVCC_HO_ERR_UNMANAGED_CALLS)
	  {
        while(call_ptr != NULL)
        {
		  call_ptr->srvcc_prog_state = CM_SRVCC_STATE_NONE;
		  
          if ( call_ptr->is_srvcc_call && !call_ptr->is_int_ims)
          {
            cmcall_end(call_ptr);
	        CM_MSG_HIGH_0("CM_1XSRVCC_HO_FAIL_IND_F, cleared ext IMS single calls");
			call_info_ptr = cmcall_info_alloc();

			cmclient_list_call_event_ntfy ( CM_CALL_EVENT_HO_FAIL, call_info_ptr );
          }
		  else if ( call_ptr->is_srvcc_call && call_ptr->is_int_ims)
		  {
		     /*when 1x reports HO_FAIL before IMS confirms call end */
		    if(call_ptr->call_state      == CM_CALL_STATE_IDLE &&
            call_ptr->call_subst.idle  == CM_CALL_IDLE_SUBST_ENDWAIT)
		    {
		      call_info_ptr = cmcall_info_alloc();
		      cmclient_list_call_event_ntfy ( CM_CALL_EVENT_HO_FAIL, call_info_ptr );
			  call_ptr = cmcall_obj_get_next(&call_itr);
		      continue;
		    }
			
            call_ptr->parent_call_id = CM_CALL_ID_UNASSIGNED;
			call_ptr->is_srvcc_call = FALSE;
			cmcall_event( call_ptr, CM_CALL_EVENT_HO_FAIL );
			cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_HO_FAIL_NOTIFY, ll_as_id);
		    mc_ptr->hdr.cmd = MC_SRVCC_CALL_CLEAR_F;
	        mc_ptr->srvcc_call_clear.call_id = call_ptr->call_id;
			
			CM_MSG_HIGH_0("1xSRVCC: IPAPP notification sent from HO_FAIL");
		    /* set total calls that got transferred to GW */

	        cm_mc_send_cmd( mc_ptr );
		  }

          call_ptr = cmcall_obj_get_next(&call_itr);
        }

	  }
	  /* Int IMS fills in unmanagebale num. of calls */
	  else
	  {
      call_info_ptr = cmcall_info_alloc();
      cmclient_list_call_event_ntfy ( CM_CALL_EVENT_HO_FAIL, call_info_ptr );
      cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_HO_FAIL_NOTIFY, ll_as_id);
	  }

	  CM_MSG_HIGH_0("1xSRVCC: IPAPP notification sent from HO_FAIL");
          /* HO processing complete, reset the IMS SRVCC cache */
          cm_ims_cache_reset();
		 /* set total calls that got transferred to GW */

      #endif
	  break;

   case CM_1XDRVCC_HO_COMPLETE_IND_F:
    #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
    {
      cm_call_id_type drvcc_switch_in_call_id = CM_CALL_ID_INVALID;
      
      /* On going DRVCC HO is completed send DRVCC HO COMPLETE to IMS
      Cal_event_HO_Complete and MM_CM_SRVCC_HO_COMPLETE_IND to QMI-V 
      and mark DRVCC HO as completed  */  
      CM_MSG_HIGH_0("DRVCC: Recvd CM_1XDRVCC_HO_COMPLETE_IND_F ");
      cmcall_obj_iterator_init(&call_itr);
      call_ptr = cmcall_obj_get_next(&call_itr);
      ho_comp_list = (cm_mid_srvcc_ho_comp_list_s_type *)cm_mem_malloc
                                        (sizeof(cm_mid_srvcc_ho_comp_list_s_type));
      /* loop through all srvcc call in HO */
      while(call_ptr != NULL)
      {
        if ( cmcall_drvcc_is_3gpp2_drvcc_type(cmcall_drvcc_get_setup_call_drvcc_type(call_ptr)) )
        {
          srvcc_call_info_ptr =cmcall_srvcc_info_alloc();
          if( srvcc_call_info_ptr == NULL )
          {
            CM_MSG_HIGH_0("DRVCC: CM_1XDRVCC_HO_COMPLETE_IND_F, can not "
                "alloc memory for srvcc call info object " );
            break;
          }
          if(drvcc_switch_in_call_id == CM_CALL_ID_INVALID)
          {
            drvcc_switch_in_call_id = call_ptr->call_id;
            CM_MSG_HIGH_1("DRVCC: Active switch in call  1XDRVCC_HO_COMPLETE is %d",drvcc_switch_in_call_id);
          }    
         (void) cmcall_srvcc_info_get( call_ptr, srvcc_call_info_ptr );
          /*copy each srvcc call context to ho_comp_list buffer */
          /* fill drvcc_parent_call_id for QMI-V reference */
          srvcc_call_info_ptr->drvcc_parent_call_id  = call_ptr->drvcc_parent_call_id;
          * (&(ho_comp_list->info[num_srvcc_calls])) = (*srvcc_call_info_ptr);
          cm_mem_free(srvcc_call_info_ptr);
          num_srvcc_calls++;
        }    
        call_ptr = cmcall_obj_get_next(&call_itr);
      }
      call_ptr = cmcall_ptr(drvcc_switch_in_call_id);
      if(call_ptr == NULL)
      {
         CM_MSG_HIGH_0("CM_1XDRVCC_HO_COMPLETE_IND_F, switch in call ptr NULL" );
         cm_mem_free(ho_comp_list);
         break;
      }
      
      ho_comp_list->is_int_ims = call_ptr->is_int_ims;

      /* reset drvcc cache as now switch in call id events are to be sent */
      cmcall_drvcc_cache_init(); 
      /* parent drvcc call id should be set to invalid now */
      call_ptr->drvcc_parent_call_id = CM_CALL_ID_INVALID;
      
      /* call event HO Complete and ho_comp_list is to be sent for switch in cs call id */
      cmcall_event( call_ptr, CM_CALL_EVENT_HO_COMPLETE );
      cmipapp_send_drvcc_notification(CMIPAPP_DRVCC_HO_COMPLETE_NOTIFY,call_ptr->orm_sent_status, call_ptr->asubs_id);
      CM_MSG_HIGH_0("DRVCC: IPAPP notification sent from 1XDRVCC_HO_COMPLETE");
       /* set total calls that got transferred to GW */
      ho_comp_list->number_calls= num_srvcc_calls;      
      (void) cm_msgr_send( MM_CM_DRVCC_HO_COMPLETE_IND, MSGR_MM_CM,
              (msgr_hdr_s*)ho_comp_list, sizeof(cm_mid_srvcc_ho_comp_list_s_type) );

      CM_MSG_HIGH_0("CM_1XDRVCC_HO_COMPLETE_IND_F, msgr sent HO calls.");      
      cm_mem_free(ho_comp_list);
    }
    #endif /* defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) */
    break;

   case CM_1XDRVCC_ORM_SUCCESS_IND_F:
   case CM_1XDRVCC_CSFB_ORM_SUCCESS_IND_F:
    #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
    {
      if (!cm_call_id_is_allocated (cm_mc_rpt_ptr->onexdrvcc_orm_success.call_id))
      {
        CM_ERR_1( "DRVCC: ORM success Call Id %d is unallocated.",
          cm_mc_rpt_ptr->onexdrvcc_orm_success.call_id);
        break;
      }

      if ( (call_ptr = cmcall_ptr( cm_mc_rpt_ptr->onexdrvcc_orm_success.call_id ))
        == NULL )
      {
        CM_MSG_HIGH_1( "DRVCC: Can't get pointer to call object.id = %d",
          cm_mc_rpt_ptr->onexdrvcc_orm_success.call_id);
        break;
      }
      CM_MSG_HIGH_1( "DRVCC: ORM status set to TRUE for call id = %d",
          cm_mc_rpt_ptr->onexdrvcc_orm_success.call_id);
      call_ptr->orm_sent_status = TRUE;
    }
    #else
      CM_MSG_HIGH_0("DRVCC: ORM_SUCCESS_IND_F, feature IP call not defined");
    #endif
   break;

  default:
      call_ptr = NULL;
      break;

  } /* switch( cm_mc_rpt_ptr->hdr.cmd ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is a reply to an earlier command we sent to MC,
  ** also do reply processing.
  */
  if( call_ptr != NULL &&
      cmcall_ptr_is_allocated( call_ptr ) == TRUE &&
      cm_reply_check(&call_ptr->reply, cm_mc_rpt_ptr->hdr.cmd) )
  {
    /* Point at client command waiting for reply
    */
    cm_cmd_type  *reply_cmd_ptr = cm_reply_get(&call_ptr->reply);
    CM_ASSERT( reply_cmd_ptr != NULL );

    /* Invoke the reply function with the appropriate error code.
    */
    cmxcall_client_cmd_mc_reply( CALL_CMD_PTR(reply_cmd_ptr), CM_CALL_CMD_ERR_NOERR );

    /* We are done with processing the command that was waiting
    ** for reply - deallocate its command buffer.
    */
    cm_cmd_dealloc( reply_cmd_ptr );

    /* Turn on the signal that causes us to process any pending
    ** clients commands. This is necessary since we were NOT
    ** processing any clients commands while waiting for a reply
    ** from MC.
    */
    (void) rex_set_sigs( rex_self(), CM_CLIENT_CMD_Q_SIG );
  }
} /* cmxcall_mc_rpt_proc() */

/*===========================================================================

FUNCTION cmxcall_cmd_copy_fields

DESCRIPTION
  Copy the active CDMA/HDR fields of a call command into a call object.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmxcall_cmd_copy_fields(

    cmcall_s_type               *call_ptr,
        /* Pointer to a call object */

    const cm_call_cmd_s_type    *cmd_ptr
        /* Pointer to a call command */

)
{
  const cm_call_cmd_info_cdma_s_type *cmd_mode_info_ptr  = NULL;

  cdma_cmcall_type                   *call_mode_info_ptr =  NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(call_ptr != NULL);
  CM_ASSERT(cmd_ptr  != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_mode_info_ptr = CALL_CMD_MODE_INFO_PTR(cmd_ptr);
  call_mode_info_ptr = &call_ptr->cmcall_mode_info.info.cdma_info;


  if( cmd_mode_info_ptr->srv_opt       != CM_SRV_OPT_NONE )
  {
   call_mode_info_ptr->srv_opt          = cmd_mode_info_ptr->srv_opt;
  }

  if( cmd_mode_info_ptr->otasp_act_code != CM_OTASP_ACT_CODE_NONE )
  {
   call_mode_info_ptr->otasp_info.otasp_act_code  = cmd_mode_info_ptr->otasp_act_code;
  }
  call_mode_info_ptr->is_special_srv_data_call = cmd_mode_info_ptr->is_special_srv_data_call;
  #ifdef FEATURE_IS2000
  call_mode_info_ptr->drs_bit      = cmd_mode_info_ptr->drs_bit;
  #endif /* FEATURE_IS2000 */

  #ifdef FEATURE_HDR_HANDOFF
  call_ptr->last_act_data_net  = cmd_mode_info_ptr->last_act_data_net;
  #endif /* FEATURE_HDR_HANDOFF */

  call_mode_info_ptr->flash_type         = cmd_mode_info_ptr->flash_type;

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
  #error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

  #ifdef FEATURE_HDR_HYBRID
  if(cmd_ptr->cmd == CM_CALL_CMD_ORIG)
  {
    sys_modem_as_id_e_type asubs_id = cmph_map_cm_ss_to_subs(CM_SS_MAIN);

    call_ptr->is_hybr_allowed = cmmsc_auto_is_hybr_allowed(
                                  cmph_map_cm_mode_pref_to_sd_mode_pref(
                                       cmd_ptr->info.favored_mode_pref),
                                  cmph_map_cm_hybr_pref_to_sd_hybr_pref(
                                       cmd_ptr->info.favored_hybr_pref ),
                                  asubs_id);
  }
  #endif

  #ifdef FEATURE_IS2000_REL_A
  call_mode_info_ptr->sr_id = cmd_mode_info_ptr->sr_id;
  call_mode_info_ptr->sr_id_included = cmd_mode_info_ptr->sr_id_included;

  call_mode_info_ptr->qos_parms_incl = cmd_mode_info_ptr->qos_parms_incl;

  if ( call_mode_info_ptr->qos_parms_incl )
  {
    call_mode_info_ptr->qos_parms_len = cmd_mode_info_ptr->qos_parms_len;
    call_mode_info_ptr->qos_parms_len = memscpy( call_mode_info_ptr->qos_parms,
                                                 sizeof(call_mode_info_ptr->qos_parms),
                                                 cmd_mode_info_ptr->qos_parms,
                                                 cmd_mode_info_ptr->qos_parms_len );
  }
  #endif /* FEATURE_IS2000_REL_A */
}

/*===========================================================================

FUNCTION cmxcall_end_call

DESCRIPTION
  Ends the specified call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_end_call
(
  cm_call_id_type      call_id,
  cm_call_end_e_type   end_status,
  byte                 end_ll_reason
)
{

    cmcall_s_type       *call_ptr;


    if(call_id == CM_CALL_ID_INVALID)
    {
      return;
    }

    call_ptr = cmcall_ptr (call_id);
    if( call_ptr == NULL )
    {
      CM_ERR_0( "Invalid Callid");
      return;
    }

    /* End the call and clean up.
    */
    if( call_ptr->call_state == CM_CALL_STATE_IDLE )
    {
      return;
    }

#if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
    if(cmsds_is_dual_rx_1xcsfb_enabled() &&
       call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB)
    {
      /* End call on CM side
      */
      call_ptr->end_status = end_status;
      if (cmss_ptr()->info.srv_status == SYS_SRV_STATUS_SRV )
      {
        (void)cmcsfbcall_send_mm_dual_rx_1xcsfb_tau_req();
      }
      else
      {
        CM_MSG_HIGH_0(" not Sending MMCM_DUAL_RX_1XCSFB_TAU_REQ (no FULL SRV)");
      }
      cmcall_end(call_ptr);
      return;
    }
    #endif
    /* Do not end GW calls
    */
    if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS ||
        call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS ||
        call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_LTE )
    {
      return;
    }

    cmxcall_send_end_cmd(call_ptr, end_ll_reason);
    call_ptr->end_status = end_status;

    /* If not waiting for end_call_cnf, release the call.
    */
    if ( call_ptr->call_state != CM_CALL_STATE_IDLE ||
         call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT
       )
    {
        /* End call on CM side
        */
        cmcall_end(call_ptr);
    }

}

/*===========================================================================
FUNCTION cmxcall_set_highest_layer_orig_failure

DESCRIPTION

  Calculate and set in call object the highest layer failure.

  This field is adjusted after every silent redial failure.
  CM_CALL_ORIG_ERR_LAYER_OTHER is lower layer failure than
  CM_CALL_ORIG_ERR_LAYER_L2 and CM_CALL_ORIG_ERR_LAYER_L2 is
  lower layer failure than CM_CALL_ORIG_ERR_LAYER_L3


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmxcall_set_highest_layer_orig_failure(

    cmcall_s_type                 *call_ptr,
      /* call object whose origination failed */

    cm_call_orig_fail_layer_e_type layer
      /* layer where the failure occured */
)
{
  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( BETWEEN(layer,
                     CM_CALL_ORIG_ERR_LAYER_NONE,
                     CM_CALL_ORIG_ERR_LAYER_MAX ));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_MSG_HIGH_2("call orig fail layer = %d, Layer in which failure occured  = %d",
                 call_ptr->cmcall_mode_info.info.cdma_info.orig_fail_layer,
                 layer);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(layer > call_ptr->cmcall_mode_info.info.cdma_info.orig_fail_layer)
  {
    call_ptr->cmcall_mode_info.info.cdma_info.orig_fail_layer = layer;
  }

}

/*===========================================================================

FUNCTION cmxcall_is_svdo_capable

DESCRIPTION
  Compare the call objects for the 1X and DO call and check to see if
  simultaneous operation can be supported.

DEPENDENCIES
  Call objects must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE:  Simultaneous operations possible
  FALSE: Simultaneous operations not possible/call not originated on
         SD_SS_MAIN.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmxcall_is_svdo_capable
(
  const cmcall_s_type  *voice_call_ptr
    /* pointer to voice call Object  */
)
/*lint -esym(818, voice_call_ptr) */
{

  const cdma_cmcall_type *call_mode_info_ptr = NULL;

  CM_ASSERT( voice_call_ptr != NULL );

  call_mode_info_ptr = &(voice_call_ptr->cmcall_mode_info.info.cdma_info);

  if(!cmph_ptr()->is_svdo_supported)
  {
    return FALSE;
  }

  if( voice_call_ptr->ss == CM_SS_MAIN)
  {
    switch (call_mode_info_ptr->srv_opt)
    {
      case CAI_SO_VOICE_IS96A:              /* SO1  */
      case CAI_SO_LOOPBACK:                 /* SO2  */
      case CAI_SO_VOICE_EVRC:               /* SO3  */
      case CAI_SO_SMS:                      /* SO6  */
      case CAI_SO_LOOPBACK_13K:             /* SO9  */
      case CAI_SO_RS2_SMS:                  /* SO14 */
      case CAI_SO_VOICE_13K_IS733:          /* SO17 */
      case CAI_SO_RS1_OTAPA:                /* SO18 */
      case CAI_SO_RS2_OTAPA:                /* SO19 */
      case CAI_SO_RS1_PD:                   /* SO35 */
      case CAI_SO_RS2_PD:                   /* SO36 */
      case CAI_SO_MARKOV_SO54:              /* SO54 */
      case CAI_SO_LOOPBACK_SO55:            /* SO55 */
      case CAI_SO_VOICE_4GV_NB:             /* SO68 */
      case CAI_SO_VOICE_4GV_WB:             /* SO70 */
      case CAI_SO_VOICE_WILDCARD:           /* SO0 */
      case CAI_SO_VOICE_EVRC_NW:            /* SO73 */
      case CAI_SO_MARKOV_SO74:              /* SO74 */

      CM_MSG_MED_1("SVDO : Voice call SO %d compatible with DO",
                        call_mode_info_ptr->srv_opt);
          return TRUE;

      default:
      CM_MSG_HIGH_1("SVDO : Voice call SO %d incompatible with DO",
                        call_mode_info_ptr->srv_opt);

          return FALSE;
    }
  }
  else
  {
    CM_MSG_HIGH_1("SVDO : Call on stack %d, non SVDO scenario",
                       voice_call_ptr->ss);
    return FALSE;
  }

}
/*lint +esym(818, voice_call_ptr) */


/*===========================================================================

FUNCTION cmxcall_is_cdma_ps_call_optimization_enabled

DESCRIPTION
  This function returns if the optimization of holding 1x/HDR data calls
  during BSR is enabled or not.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if enabled
  FALSE if not enabled

SIDE EFFECTS
  none

===========================================================================*/
boolean cmxcall_is_cdma_ps_call_optimization_enabled(cmcall_s_type  *call_ptr)
{

  cmph_s_type                 *ph_ptr             = cmph_ptr();
    /* Point at phone object */

  cmss_s_type                 *ss_ptr             = cmss_ptr();
    /* point at SS object */

  dword curr_uptime  = time_get_uptime_secs();


  CM_MSG_HIGH_3("ps_call_bsr_uptime %d, curr uptime %d, is_custom_home %d",
                 ss_ptr->cdma_ps_call_bsr_uptime, curr_uptime, cmss_ptr()->info.is_cdma_custom_home);
  if( ( (ph_ptr->ps_call_optimized.allowed == CMXCALL_PS_OPT_ENABLED_ROAM_ONLY &&
        ( ss_ptr->info.roam_status != SYS_ROAM_STATUS_OFF && !cmss_ptr()->info.is_cdma_custom_home)) ||
        (ph_ptr->ps_call_optimized.allowed == CMXCALL_PS_OPT_ENABLED_HOME_ROAM)
       ) &&
      (ss_ptr->cdma_ps_call_bsr_uptime > curr_uptime) &&
      (call_ptr->call_type == CM_CALL_TYPE_PS_DATA) &&
      (call_ptr->srv_type == CM_SRV_TYPE_CDMA_SPECIFIC || call_ptr->srv_type == CM_SRV_TYPE_HDR_MORE_PREF ||
       call_ptr->srv_type == CM_SRV_TYPE_CDMA_HDR || call_ptr->srv_type == CM_SRV_TYPE_HDR
      )
    )
  {
    return TRUE;
  }

  return FALSE;
}


#else /* if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900)) */

/*===========================================================================
FUNCTION cmxcall_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmxcall_dummy( void )
/*lint -esym(714,cmxcall_dummy) */
/*lint -esym(765,cmxcall_dummy)
** Can't be static as it would result in compiler warnings
*/
{
}

#endif /* if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900)) */
/* End of cmxcall.c */
